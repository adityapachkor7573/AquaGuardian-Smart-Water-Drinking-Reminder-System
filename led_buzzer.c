#include <Lpc214x.h>
#include "lcd.h"
#include "led_buzzer.h"
#include "delay.h"
#include "lcd-defines.h"
#include "water.h"

#define LED     (1<<5)     // P0.5
#define BUZZER  (1<<6)     // P0.6
u8 reminder;

void LED_Buzzer_Init(void)
{
    IODIR0 |= LED | BUZZER;

    IOCLR0 = LED | BUZZER;
}

void LED_ON(void)
{
    IOSET0 = LED;
}

void LED_OFF(void)
{
    IOCLR0 = LED;
}

void BUZZER_ON(void)
{
    IOSET0 = BUZZER;
}

void BUZZER_OFF(void)
{
    IOCLR0 = BUZZER;
}

void Drink_Water_Alert(void)
{
    u8 i;
    u8 time_left;

    CmdLCD(CLEAR_LCD);

    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("DRINK WATER");

    for(i = 0; i < 30; i++)
    {
        time_left = 30 - i;

        CmdLCD(GOTO_LINE2_POS0);
        StrLCD("TIME LEFT: ");
        CharLCD((time_left / 10) + '0');
        CharLCD((time_left % 10) + '0');
        StrLCD("s ");

        LED_ON();
        BUZZER_ON();

        delay_ms(500);

        LED_OFF();
        BUZZER_OFF();

        delay_ms(500);

        if(DrinkFlag)
        {
            u8 count, goal;
            DrinkFlag = 0;
            Increase_Water_Count();
            Get_Water_Status(&count, &goal);
            return;
        }    
    }
    
    Handle_Missed_Reminder_Increment();
    LED_OFF();
    BUZZER_OFF();
    CmdLCD(CLEAR_LCD);
}
