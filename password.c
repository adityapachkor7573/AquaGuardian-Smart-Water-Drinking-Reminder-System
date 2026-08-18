// password.c
#include <LPC21xx.h>

#include "password.h"
#include "lcd.h"
#include "lcd-defines.h"
#include "KPM.h"
#include "delay.h"
#include "rtc.h"
#include "water.h"

// Define BUZZER pin if not defined in header (e.g., P0.21)
#ifndef BUZZER
#define BUZZER (1 << 6) 
#endif

static u8 stored_pass[PASS_LENGTH] = {'1','1','1','1'};
static u8 entered_pass[PASS_LENGTH];
static u8 attempts = 0;

// Helper function to render asterisks for cleared characters
static void LCD_ShowStars(u8 count)
{
    u8 i;
    for(i = 0; i < count; i++)
    {
        CharLCD('*');
    }
}

// --------------------------------------------------
void Password_Init(void)
{
    attempts = 0;
    
    // Configure Buzzer Pin as GPIO Output
    PINSEL1 &= ~(3 << 10); // Configure P0.21 as GPIO
    IODIR0  |= BUZZER;     // Set P0.21 as Output
    IOCLR0   = BUZZER;     // Ensure Buzzer is OFF initially
}

// --------------------------------------------------
static void GetPassword(u8 *pass)
{
    u8 idx = 0;
    char key;

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("ENTER PASS:");
    CmdLCD(GOTO_LINE2_POS0);

    while(1)
    {
        key = GetKey();

        // DIGIT INPUT (0-9)
        if((key >= '0') && (key <= '9'))
        {
            if(idx < PASS_LENGTH)
            {
                pass[idx++] = key;
                CharLCD('*');

                // AUTO-SUBMIT as soon as length matches
                if(idx == PASS_LENGTH)
                {
                    delay_ms(200); // Short delay to visually register last star
                    return;
                }
            }
        }
        // CLEAR LAST DIGIT ('C')
        else if(key == 'C')
        {
            if(idx > 0)
            {
                idx--;
                CmdLCD(GOTO_LINE2_POS0);
                LCD_ShowStars(idx);
                CharLCD(' ');
                CmdLCD(GOTO_LINE2_POS0 + idx);
            }
        }
        // SUBMIT ('=') - Kept as fallback
        else if(key == '=')
        {
            if(idx == PASS_LENGTH)
            {
                return;
            }
        }
    }
}

// --------------------------------------------------
u8 Password_Verify(void)
{
    u8 i;
    u8 remaining;

    while(attempts < 3)
    {
        GetPassword(entered_pass);

        for(i = 0; i < PASS_LENGTH; i++)
        {
            if(entered_pass[i] != stored_pass[i])
            {
                break;
            }
        }

        // CORRECT PASSWORD
        if(i == PASS_LENGTH)
        {
            attempts = 0;

            CmdLCD(CLEAR_LCD);
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("ACCESS GRANTED");
            delay_ms(500);

            RTC_SubMenu();

            return 1;
        }

        // WRONG PASSWORD
        attempts++;
        remaining = 3 - attempts;

        CmdLCD(CLEAR_LCD);
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("WRONG PASS");

        CmdLCD(GOTO_LINE2_POS0);
        if(remaining > 0)
        {
            StrLCD("LEFT: ");
            CharLCD(remaining + '0');
        }

        // Beep Buzzer once for error
        IOSET0 = BUZZER;
        delay_ms(150);
        IOCLR0 = BUZZER;
        delay_ms(800);
    }
        
    // LOCK SYSTEM FOR 10 SECONDS AFTER 3 FAILED ATTEMPTS
    {
        s8 sec;

        CmdLCD(CLEAR_LCD);
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD("SYSTEM LOCKED");

        IOSET0 = BUZZER; // Turn on continuous alarm during lockout

        for(sec = 10; sec > 0; sec--)
        {
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD("WAIT ");

            if(sec < 10)
            {
                CharLCD('0');
                CharLCD(sec + '0');
            }
            else
            {
                CharLCD((sec / 10) + '0');
                CharLCD((sec % 10) + '0');
            }

            StrLCD(" SEC");

            delay_ms(1000);
        }

        IOCLR0 = BUZZER; // Turn off alarm
    }

    attempts = 0;

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("TRY AGAIN");

    delay_ms(500);

    return 0;
}

// --------------------------------------------------
void Password_Change(void)
{
    u8 new_pass[PASS_LENGTH];
    u8 confirm_pass[PASS_LENGTH];
    u8 old_pass[PASS_LENGTH];
    u8 i;

    // STEP 1: PROMPT EXPLICITLY FOR OLD PASSWORD
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("OLD PASS:");
    CmdLCD(GOTO_LINE2_POS0);

    for(i = 0; i < PASS_LENGTH; i++)
    {
        char key = 0;

        while(key == 0)
        {
            key = KeyScan();
        }

        delay_ms(20);
        Wait_Key_Release();

        old_pass[i] = key;
        CharLCD('*');
    }

    // VERIFY OLD PASSWORD
    for(i = 0; i < PASS_LENGTH; i++)
    {
        if(old_pass[i] != stored_pass[i])
        {
            CmdLCD(CLEAR_LCD);
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("WRONG OLD PASS");

            IOSET0 = BUZZER;
            delay_ms(400);
            IOCLR0 = BUZZER;
            delay_ms(600);

            return; // Exit if old password is wrong
        }
    }

    // STEP 2: PROMPT FOR NEW PASSWORD
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("NEW PASS:");
    CmdLCD(GOTO_LINE2_POS0);

    for(i = 0; i < PASS_LENGTH; i++)
    {
        char key = 0;

        while(key == 0)
        {
            key = KeyScan();
        }

        delay_ms(20);
        Wait_Key_Release();

        new_pass[i] = key;
        CharLCD('*');
    }

    // STEP 3: CONFIRM NEW PASSWORD
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("CONFIRM:");
    CmdLCD(GOTO_LINE2_POS0);

    for(i = 0; i < PASS_LENGTH; i++)
    {
        char key = 0;

        while(key == 0)
        {
            key = KeyScan();
        }

        delay_ms(20);
        Wait_Key_Release();

        confirm_pass[i] = key;
        CharLCD('*');
    }

    // STEP 4: VERIFY NEW & CONFIRM MATCH
    for(i = 0; i < PASS_LENGTH; i++)
    {
        if(new_pass[i] != confirm_pass[i])
        {
            CmdLCD(CLEAR_LCD);
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("NOT MATCHED!");

            IOSET0 = BUZZER;
            delay_ms(400);
            IOCLR0 = BUZZER;

            return;
        }
    }

    // STEP 5: SAVE NEW PASSWORD
    for(i = 0; i < PASS_LENGTH; i++)
    {
        stored_pass[i] = new_pass[i];
    }

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("PASS UPDATED");

    delay_ms(700);
    return;
}

/* Submenu displayed ONLY after password verification */
void RTC_SubMenu(void)
{
    u8 key;
    u32 count = 0;

    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD("1.EDIT RTC");

    CmdLCD(GOTO_LINE2_POS0);
    StrLCD("2.CHANGE PASS");

    while(1)
    {
        key = GetKey();

        // 1. EDIT RTC
        if(key == '1')
        {
            CmdLCD(CLEAR_LCD);
            Wait_Key_Release();
            RTC_Edit();
            delay_ms(500);
            break;
        }
        // 2. CHANGE PASSWORD
        if(key == '2')
        {
            CmdLCD(CLEAR_LCD);
            Wait_Key_Release();
            Password_Change();
            break;
        }
        if(key == 'C')
        {
            break;
        }
                
        delay_ms(10);
        count++;

        if(count >= 500) 
        {
            CmdLCD(CLEAR_LCD);
            return;
        }
    }
}
