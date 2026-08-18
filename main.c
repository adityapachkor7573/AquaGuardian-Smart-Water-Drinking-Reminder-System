
//main.c
#include <lpc214x.h>

#include "lcd.h"
#include "lcd-defines.h"
#include "password.h"
#include "rtc.h"
#include "rtc_defines.h"
#include "delay.h"
#include "interrupt.h"    
#include "KPM.h"  
#include  "water.h"
#include "reminder.h"
#include "led_buzzer.h"
#define GREEN_LED (1 << 22) //p0.22

s32 hour,min,sec;
s32 date,month,year;
s32 day;
u8 count,goal;
u8 remaining;
u16 percentage;
//volatile u8 MenuFlag = 0;
void StartupScreen(void);
void ShowMenu(void);

int main(void)
{
	InitLCD();
	Load_Glass_Custom_Chars();
	StartupScreen();
	EINT0_Init();	
  RTC_Init();
  InitKPM();
	Water_Init();
	LED_Buzzer_Init();
		
    SetRTCTimeInfo(22,51,00);
    SetRTCDateInfo(31,12,2026);
    SetRTCDay(SAT);
	  
	Water_Button_Init();
		
      while(1)
    {
			 if(MenuFlag)
				{
					MenuFlag = 0;
					ShowMenu();
				}
				CmdLCD(DSP_ON_CUR_OFF);
        GetRTCTimeInfo(&hour,&min,&sec);
        DisplayRTCTime(hour,min,sec);

        GetRTCDateInfo(&date,&month,&year);
				DisplayRTCTime(hour, min, sec);
        CharLCD(' ');
        CharLCD(' ');
        GetRTCDay(&day);
        DisplayRTCDay(day);
				
				Check_Day_Reset();
				
				Get_Water_Status(&count,&goal);
				remaining = (goal > count) ? (goal - count) : 0;
        percentage = (goal > 0) ? ((count * 100) / goal) : 0;
				CmdLCD(GOTO_LINE2_POS0);
        
				switch ((sec / 2) % 3)
        {
            case 0:
                // STAGE 1 (0-2s): Direct Date Print (Prevents Screen Flickering)
                StrLCD("DATE:");
                CharLCD(date/10 + '0');
                CharLCD(date%10 + '0');
                CharLCD('/');
                CharLCD(month/10 + '0');
                CharLCD(month%10 + '0');
                CharLCD('/');
                u32LCD(year); // Prints YYYY[cite: 7]
                CharLCD(' ');
                break;

            case 1:
                // STAGE 2 (2-4s): Display Consumed Intake vs Target Goal
                // Format: DRANK: 10/10
                StrLCD("DRANK: ");
                Display_Number(count);
                CharLCD('/');
                Display_Number(goal);
                StrLCD("      ");
                break;

            case 2:
                // STAGE 3 (4-6s): Display Remaining Glasses & Progress %
                // Format: REM:0   PCT:100%
                StrLCD("REM:");
                Display_Number(remaining);
                StrLCD(" [");
								Display_Animated_Glass(percentage); // <--- PRINTS GLASS ICON HERE
								StrLCD("] ");
                Display_Number(percentage);
                CharLCD('%');
                StrLCD(" ");
                break;
        }
				
			
        Check_Goal_Completion();
				Check_Missed_Reminders();
   
        if(WaterGoal > 0 && WaterCount < WaterGoal)
        {
            Reminder_Task();

            if(Get_Reminder_Flag())
            {
                Drink_Water_Alert();
                Clear_Reminder_Flag();
                CmdLCD(CLEAR_LCD);                       
            }
			}
		}
}

void StartupScreen(void)
{
    u8 i;

    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("AquaGuardian");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("Initializing");

    for(i=0;i<3;i++)
    {
        StrLCD(".");
        delay_ms(100);
    }

    delay_ms(200);

    CmdLCD(CLEAR_LCD);
}
/*---------------- MENU ----------------*/
void ShowMenu(void)
{
    u8 key;
    u32 count = 0;

    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("1.SET RTC");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("2.SET GOAL");


    while(1)
    {
        key = KeyScan();

        if(key == '1')
        {
            CmdLCD(CLEAR_LCD);
						Wait_Key_Release();
						Password_Verify();
            delay_ms(800);
						CmdLCD(CLEAR_LCD);
            break;
        }

        if(key == '2')
        {
         
            Wait_Key_Release();
            Water_Goal_Menu();
            break;
        }

        if(key == 'C')
        {
            break;
        }

        /* 5 second timeout */
        delay_ms(10);
        count++;

        if(count >= 500)     
        {
            CmdLCD(CLEAR_LCD);
            break;           // return to main
        }
    }
}
