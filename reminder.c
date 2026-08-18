#include "types.h"
#include "lcd.h"
#include "delay.h"
#include "rtc.h"
#include "water.h"
#include "lcd-defines.h"
#include "KPM.h"

static u16 Reminder_Minute = 0;
/* store starting RTC time */
static s32 StartHour;
static s32 StartMin;
static s32 StartSec;
static u8 ReminderFlag = 0;

u16 Get_Reminder_Minute(void)
{
    return Reminder_Minute;
}

void Reminder_Init(void)
{
    Reminder_Minute = 0;
    ReminderFlag = 0;
}

/*
 User enters reminder interval
 Max limit: 180 minutes, Max input length: 3 digits
*/
void Reminder_Setup(void)
{
    u8 key;
    u16 time = 0;
    u8 digit_count = 0; // Tracks keypress length

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("SET REMINDER");
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("MIN:");

    while(1)
    {
        key = GetKey();
        Wait_Key_Release(); // Ensures clean single keypress detection

        /* Number input: Strictly allow max 3 digits */
        if(key >= '0' && key <= '9')
        {
            if(digit_count < 3)
            {
                time = (time * 10) + (key - '0');
                digit_count++;

                // Clear previous output space and print formatted number
                CmdLCD(GOTO_LINE2_POS0 + 4);
                StrLCD("    ");
                CmdLCD(GOTO_LINE2_POS0 + 4);
                Display_Number(time);
            }
            // If digit_count == 3, do nothing (wait for '=' or 'C')
        }

        /* '=' Enter key */
        else if(key == '=')
        {
            /* Validation: Must be strictly between 1 and 180 */
            if(time == 0 || time > 180)
            {
                CmdLCD(CLEAR_LCD);
                CmdLCD(GOTO_LINE1_POS0);
                StrLCD("INVALID TIME");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("MAX: 180 MIN");

                delay_ms(1500);

                /* Redraw setup screen */
                CmdLCD(CLEAR_LCD);
                CmdLCD(GOTO_LINE1_POS0);
                StrLCD("SET REMINDER");
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD("MIN:");

                time = 0;
                digit_count = 0;
                continue;
            }

            Reminder_Minute = time;

            /* Store current RTC time reference */
            GetRTCTimeInfo(&StartHour, &StartMin, &StartSec);

            CmdLCD(CLEAR_LCD);
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("REMINDER SET");
            delay_ms(1000);
            return;
        }

        /* Clear Key 'C' */
        else if(key == 'C')
        {
            time = 0;
            digit_count = 0;
            CmdLCD(GOTO_LINE2_POS0 + 4);
            StrLCD("    ");
            CmdLCD(GOTO_LINE2_POS0 + 4);
        }
    }
}

void Reminder_Task(void)
{
    s32 hour, min, sec;
    u32 current_sec;
    u32 start_sec;

    /* Stop reminder if goal completed */
    if(Get_Water_Count() >= Get_Water_Goal())
    {
        ReminderFlag = 0;
        return;
    }
    if(Reminder_Minute == 0)
        return;

    GetRTCTimeInfo(&hour, &min, &sec);
    current_sec = (hour * 3600) + (min * 60) + sec;
    start_sec   = (StartHour * 3600) + (StartMin * 60) + StartSec;

    if((current_sec - start_sec) >= (Reminder_Minute * 60))
    {
        ReminderFlag = 1;

        /* Reset timer baseline for next interval */
        StartHour = hour;
        StartMin  = min;
        StartSec  = sec;
    }
}

u8 Get_Reminder_Flag(void)
{
    return ReminderFlag;
}

void Clear_Reminder_Flag(void)
{
    ReminderFlag = 0;
}
