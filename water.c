#include <lpc214x.h>

#include "KPM.h"
#include "delay.h"
#include "lcd-defines.h"
#include "rtc.h"
#include "lcd.h"
#include "interrupt.h"
#include "reminder.h"

#define RED_LED   (1 << 23)
#define GREEN_LED (1 << 22)

volatile u8 DrinkFlag = 0;
volatile u8 MissedReminders = 0;

/* Global variables */
u8 WaterCount = 0;
u8 WaterGoal = 0;        
static u8 StartHour = 0;   
static u8 StartMin = 0;
static u8 StartSec = 0;

void Wait_Key_Release(void)
{
    while(KeyScan() != 0);
    delay_ms(50);
}

/* Initialize water reminder system */
void Water_Init(void)
{
    WaterGoal = 0;
    WaterCount = 0;
    
    StartHour = 0;
    StartMin  = 0;
    StartSec  = 0;

    // Configure P0.22 as GPIO output for Green LED
    PINSEL1 &= ~(3 << 12);
    IODIR0  |= GREEN_LED;
    IOCLR0   = GREEN_LED; // Start turned OFF
}

/* Set daily water target */
void Set_Water_Goal(u8 goal)
{
    s32 hour, min, sec;

    WaterGoal = goal;
    WaterCount = 0;

    /* Store starting RTC time */
    GetRTCTimeInfo(&hour, &min, &sec);

    StartHour = (u8)hour;
    StartMin  = (u8)min;
    StartSec  = (u8)sec;
}

/* Return total target glasses */
u8 Get_Water_Goal(void)
{
    return WaterGoal;
}

/* Increment consumed water count */
void Drink_Water(void)
{
    if(WaterCount < WaterGoal)
    {
        WaterCount++;
    }
}

/* Return consumed glasses */
u8 Get_Water_Count(void)
{
    return WaterCount;
}

/* Reset water count after 24 hours or specifically at 23:59:58 */
void Check_Day_Reset(void)
{
    s32 hour, min, sec;
    u32 CurrentSeconds;
    u32 StartSeconds;

    GetRTCTimeInfo(&hour, &min, &sec);

    CurrentSeconds = (hour * 3600) + (min * 60) + sec;
    StartSeconds   = (StartHour * 3600) + (StartMin * 60) + StartSec;

    /* 
     * CANCEL NEARBY REMINDERS:
     * If time is between 23:59:55 and 23:59:57, disarm any trigger flag to 
     * prevent alarms right before midnight reset.
     */
    if (hour == 23 && min == 59 && sec >= 55 && sec < 58)
    {
        Clear_Reminder_Flag();
    }

    /* 
     * TRIGGER RESET AT 23:59:58 (or standard 24h elapsed threshold)
     */
    if ((hour == 23 && min == 59 && sec >= 58) || 
        CurrentSeconds < StartSeconds || 
        (CurrentSeconds - StartSeconds) >= 86400)
    {
        // 1. Reset all hydration metrics
        WaterGoal = 0;        // Resets target goal
        WaterCount = 0;       // Resets consumed count
        MissedReminders = 0;  // Resets missed counter

        // 2. Disarm pending alarms
        Clear_Reminder_Flag();

        // 3. Update tracking reference time
        StartHour = (u8)hour;
        StartMin  = (u8)min;
        StartSec  = (u8)sec;

        // 4. Turn off status LEDs
        PINSEL1 &= ~((3 << 12) | (3 << 14));
        IODIR0  |= (GREEN_LED | RED_LED);
        IOCLR0   = GREEN_LED | RED_LED; // Clear LED outputs
    }
}

/* Get current display value Example : 6/10 */
void Get_Water_Status(u8 *count, u8 *goal)
{
    *count = WaterCount;
    *goal  = WaterGoal;
}

void Display_Number(u16 num)
{
    if(num >= 100)
    {
        CharLCD((num / 100) + '0');
        CharLCD(((num / 10) % 10) + '0');
        CharLCD((num % 10) + '0');
    }
    else if(num >= 10)
    {
        CharLCD((num / 10) + '0');
        CharLCD((num % 10) + '0');
    }
    else
    {
        CharLCD(num + '0');
    }
}

void Water_Goal_Menu(void)
{
    u8 key;
    u16 add_glasses = 0;
    u16 initial_goal = 0;
    u8 digit_count = 0;

    CmdLCD(CLEAR_LCD);
    delay_ms(2);

    /* ==================================================================== */
    /* CASE 1: SUBSEQUENT ACCESS (WaterGoal > 0) -> UPDATE MENU             */
    /* ==================================================================== */
    if (WaterGoal > 0)
    {
        while(1)
        {
            // LINE 1: Display G:<WaterGoal> T:<Reminder_Minute>m
            CmdLCD(CLEAR_LCD);
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("G:");
            Display_Number(WaterGoal);               // Current goal
            StrLCD("  T:");
            Display_Number(Get_Reminder_Minute());  // Reminder duration
            StrLCD("m");

            // LINE 2: Options "1:Update 2:Exit"
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("1:Update 2:Exit");

            key = GetKey();
            Wait_Key_Release();

            /* Option 2: Exit -> Return to main screen */
            if(key == '2')
            {
                CmdLCD(CLEAR_LCD);
                return;
            }

            /* Option 1: Update Goal & Reminder */
            else if(key == '1')
            {
                /* CHECK MAX LIMIT CONDITION (Goal is already 100) */
                if (WaterGoal >= 100)
                {
                    CmdLCD(CLEAR_LCD);
                    CmdLCD(GOTO_LINE1_POS0);
                    StrLCD("MAX LIMIT REACH");
                    
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("MAX RANGE: 100");
                    
                    delay_ms(1500);
                    continue; // Redisplays the update menu
                }

                // STEP A: Ask for additional glasses
                CmdLCD(CLEAR_LCD);
                CmdLCD(GOTO_LINE1_POS0);
                StrLCD("ADD GLASSES:");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("ENTER:");
                
                add_glasses = 0;
                digit_count = 0;

                while(1)
                {
                    key = GetKey();
                    Wait_Key_Release();

                    /* Number input: Allow max 3 digits */
                    if(key >= '0' && key <= '9')
                    {
                        if(digit_count < 3)
                        {
                            add_glasses = (add_glasses * 10) + (key - '0');
                            digit_count++;

                            // Clear previous output space on Line 2 and print number
                            CmdLCD(GOTO_LINE2_POS0 + 6);
                            StrLCD("    ");
                            CmdLCD(GOTO_LINE2_POS0 + 6);
                            Display_Number(add_glasses);
                        }
                    }
                    else if(key == '=')
                    {
                        u16 total_combined = (u16)WaterGoal + add_glasses;

                        /* Dynamic boundary check: Ensures total doesn't exceed 100 */
                        if(add_glasses == 0 || total_combined == 0 || total_combined > 100)
                        {
                            u8 max_addable = 100 - WaterGoal; // Calculate maximum allowed addition

                            CmdLCD(CLEAR_LCD);
                            CmdLCD(GOTO_LINE1_POS0);
                            StrLCD("INVALID GOAL");
                            
                            // Displays dynamic allowed addition range e.g., "RANGE: 01 - 20"
                            CmdLCD(GOTO_LINE2_POS0);
                            StrLCD("RANGE: 01 - ");
                            Display_Number(max_addable);

                            delay_ms(1500);

                            /* Reset current entry and redraw ADD GLASSES screen */
                            CmdLCD(CLEAR_LCD);
                            CmdLCD(GOTO_LINE1_POS0);
                            StrLCD("ADD GLASSES:");
                            CmdLCD(GOTO_LINE2_POS0);
                            StrLCD("ENTER:");

                            add_glasses = 0;
                            digit_count = 0;
                            continue;
                        }

                        /* Apply combined goal when valid */
                        WaterGoal = (u8)total_combined;

                        /* STEP B: Ask to update reminder time in minutes */
                        CmdLCD(CLEAR_LCD);
                        Reminder_Setup(); 

                        /* STEP C: Show updated target status screen */
                        CmdLCD(CLEAR_LCD);
                        CmdLCD(GOTO_LINE1_POS0);
                        StrLCD("GOAL UPDATED!");
                        
                        CmdLCD(GOTO_LINE2_POS0);
                        StrLCD("PREV + NEW:");
                        Display_Number(WaterGoal);

                        delay_ms(2000);
                        CmdLCD(CLEAR_LCD);
                        return; // Returns back to main screen
                    }
                    else if(key == 'C')
                    {
                        // Exit cleanly on 'C' press when buffer is empty
                        if (digit_count == 0)
                        {
                            CmdLCD(CLEAR_LCD);
                            return;
                        }

                        add_glasses = 0;
                        digit_count = 0;
                        CmdLCD(GOTO_LINE2_POS0 + 6);
                        StrLCD("    ");
                        CmdLCD(GOTO_LINE2_POS0 + 6);
                    }
                }
            }
        }
    }

    /* ==================================================================== */
    /* CASE 2: FIRST TIME SETUP (WaterGoal == 0) -> INITIAL MENU            */
    /* ==================================================================== */
    else
    {
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("SET GOAL");

        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("ENTER:");

        digit_count = 0;
        initial_goal = 0;

        while(1)
        {
            key = GetKey();
            Wait_Key_Release();

            /* Number input: Allow max 3 digits */
            if(key >= '0' && key <= '9')
            {
                if(digit_count < 3)
                {
                    initial_goal = (initial_goal * 10) + (key - '0');
                    digit_count++;

                    CmdLCD(GOTO_LINE2_POS0 + 6);
                    StrLCD("    "); // Clear previous digit placement
                    CmdLCD(GOTO_LINE2_POS0 + 6);
                    Display_Number(initial_goal);
                }
            }
            else if(key == '=')
            {
                if(initial_goal == 0 || initial_goal > 100)
                {
                    CmdLCD(CLEAR_LCD);
                    CmdLCD(GOTO_LINE1_POS0);
                    StrLCD("INVALID GOAL");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 01 - 100");
                    delay_ms(1500);

                    CmdLCD(CLEAR_LCD);
                    CmdLCD(GOTO_LINE1_POS0);
                    StrLCD("SET GOAL");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("ENTER:");

                    initial_goal = 0;
                    digit_count = 0;
                    continue;
                }

                Set_Water_Goal((u8)initial_goal);
                Reminder_Setup(); // Setup initial reminder time

                CmdLCD(CLEAR_LCD);
                return;
            }
            else if(key == 'C')
            {
                if (digit_count == 0)
                {
                    CmdLCD(CLEAR_LCD);
                    return;
                }

                initial_goal = 0;
                digit_count = 0;
                CmdLCD(GOTO_LINE2_POS0 + 6);
                StrLCD("      ");
                CmdLCD(GOTO_LINE2_POS0 + 6);
            }
        }
    }
}

void Increase_Water_Count(void) 
{
    if (WaterGoal > 0 && WaterCount < WaterGoal) 
    {
        WaterCount++;
        MissedReminders = 0; // Reset missed counter on successful intake
        
        PINSEL1 &= ~(3 << 14);
        IOCLR0   = RED_LED;  // Turn OFF Red LED
    }
}

void Check_Goal_Completion(void)
{
    PINSEL1 &= ~(3 << 12);
    IODIR0 |= GREEN_LED;

    if (WaterGoal > 0 && WaterCount >= WaterGoal) 
    {
        IOSET0 = GREEN_LED;      
        Clear_Reminder_Flag();  
    } 
    else 
    {
        IOCLR0 = GREEN_LED;   
    }
}

void Handle_Missed_Reminder_Increment(void) 
{
    if (WaterCount < WaterGoal) 
    {
        MissedReminders++;
    }
}

void Check_Missed_Reminders(void) 
{
    PINSEL1 &= ~(3 << 14); 
    IODIR0  |= RED_LED;    

    if (MissedReminders >= 1 && WaterCount < WaterGoal) 
    {
        IOSET0 = RED_LED;  
    } 
    else 
    {
        IOCLR0 = RED_LED;  
    }
}
