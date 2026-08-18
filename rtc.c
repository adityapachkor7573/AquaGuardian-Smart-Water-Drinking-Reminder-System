#include <lpc214x.h>

#include "lcd.h"
#include "lcd-defines.h"
#include "rtc.h"
#include "rtc_defines.h"
#include "delay.h"
#include "KPM.h"   

s8 week[][4] =
{
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
};

void RTC_Init(void)
{
    CCR = RTC_RESET;
    PREINT = PREINT_VAL;
    PREFRAC = PREFRAC_VAL;
    CCR = RTC_ENABLE;
    CCR = RTC_ENABLE | RTC_CLKSRC;
}

void GetRTCTimeInfo(s32 *hour,s32 *minute,s32 *second)
{
    *hour = HOUR;
    *minute = MIN;
    *second = SEC;
}

void DisplayRTCTime(u32 hour,u32 minute,u32 second)
{
    CmdLCD(TIME_POS);

    CharLCD(hour/10+'0');
    CharLCD(hour%10+'0');
    CharLCD(':');
    CharLCD(minute/10+'0');
    CharLCD(minute%10+'0');
    CharLCD(':');
    CharLCD(second/10+'0');
    CharLCD(second%10+'0');
}

void SetRTCTimeInfo(u32 hour,u32 minute,u32 second)
{
    HOUR = hour;
    MIN = minute;
    SEC = second;
}

void GetRTCDateInfo(s32 *date,s32 *month,s32 *year)
{
    *date = DOM;
    *month = MONTH;
    *year = YEAR;
}

void DisplayRTCDate(u32 date,u32 month,u32 year)
{
    CmdLCD(DATE_POS);
    CharLCD(date/10+'0');
    CharLCD(date%10+'0');
    CharLCD('/');
    CharLCD(month/10+'0');
    CharLCD(month%10+'0');
    CharLCD('/');
    u32LCD(year);
}

void SetRTCDateInfo(u32 date,u32 month,u32 year)
{
    DOM = date;
    MONTH = month;
    YEAR = year;
}

void GetRTCDay(s32 *day)
{
    *day = DOW;
}

void DisplayRTCDay(u32 day)
{
    CmdLCD(DAY_POS);
    StrLCD(week[day]);
}

void SetRTCDay(u32 day)
{
    DOW = day;
}

void RTC_Edit(void)
{
    char key;

    s8 timeStr[9];
    s8 dateStr[11];
    s8 dayStr[2];
    s32 hour, minute, second;
    s32 date, month, year;
    s32 day;
    u8 positions[] =
    {
        0, 1, 3, 4, 6, 7,                // Time:  HH:MM:SS
        8, 9, 11, 12, 14, 15, 16, 17,    // Date:  DD/MM/YYYY
        19                               // Day:   D
    };

    u8 index = 0;
    u32 idle_time = 0;

    /* Get current RTC values */
    GetRTCTimeInfo(&hour, &minute, &second);
    GetRTCDateInfo(&date, &month, &year);
    GetRTCDay(&day);

    /* Convert time to string */
    timeStr[0] = (hour / 10) + '0';
    timeStr[1] = (hour % 10) + '0';
    timeStr[2] = ':';
    timeStr[3] = (minute / 10) + '0';
    timeStr[4] = (minute % 10) + '0';
    timeStr[5] = ':';
    timeStr[6] = (second / 10) + '0';
    timeStr[7] = (second % 10) + '0';
    timeStr[8] = '\0';

    /* Convert date to string */
    dateStr[0]  = (date / 10) + '0';
    dateStr[1]  = (date % 10) + '0';
    dateStr[2]  = '/';
    dateStr[3]  = (month / 10) + '0';
    dateStr[4]  = (month % 10) + '0';
    dateStr[5]  = '/';
    dateStr[6]  = ((year / 1000) % 10) + '0';
    dateStr[7]  = ((year / 100) % 10) + '0';
    dateStr[8]  = ((year / 10) % 10) + '0';
    dateStr[9]  = (year % 10) + '0';
    dateStr[10] = '\0';

    dayStr[0] = day + '0';
    dayStr[1] = '\0';

    /* Display Initial Screen */
    CmdLCD(CLEAR_LCD);
    CmdLCD(GOTO_LINE1_POS0);
    StrLCD(timeStr);
    CmdLCD(GOTO_LINE2_POS0);
    StrLCD(dateStr);
    CmdLCD(GOTO_LINE2_POS0 + 12);
    StrLCD("D:");
    CmdLCD(GOTO_LINE2_POS0 + 14);
    StrLCD(dayStr);
    CmdLCD(DSP_ON_CUR_BLINK);

    while(1)
    {
        u8 p = positions[index];

        /* Update cursor position on screen */
        if(p <= 7)
        {
            CmdLCD(GOTO_LINE1_POS0 + p);
        }
        else if(p <= 17)
        {
            CmdLCD(GOTO_LINE2_POS0 + (p - 8));
        }
        else
        {
            CmdLCD(GOTO_LINE2_POS0 + 14);
        }

        key = GetKey();

        /* No key pressed timeout handling */
        if(key == '\0')
        {
            delay_ms(100);
            idle_time += 100;
            if(idle_time >= 60000)
            {
                CmdLCD(DSP_ON_CUR_OFF);
                CmdLCD(CLEAR_LCD);
                return;
            }
            continue;
        }
        else
        {
            idle_time = 0;
        }

        /* NEXT FIELD MANUALLY */
        if(key == '+')
        {
            if(index < 14)
                index++;
        }
        /* PREVIOUS FIELD MANUALLY */
        else if(key == '-')
        {
            if(index > 0)
                index--;
        }
        /* CLEAR FIELDS */
        else if(key == 'C')
        {
            timeStr[0] = '0';
            timeStr[1] = '0';
            timeStr[3] = '0';
            timeStr[4] = '0';
            timeStr[6] = '0';
            timeStr[7] = '0';

            dateStr[0] = '0';
            dateStr[1] = '0';
            dateStr[3] = '0';
            dateStr[4] = '0';
            dateStr[6] = '0';
            dateStr[7] = '0';
            dateStr[8] = '0';
            dateStr[9] = '0';

            dayStr[0]  = '0';

            CmdLCD(GOTO_LINE1_POS0);
            StrLCD(timeStr);
            CmdLCD(GOTO_LINE2_POS0);
            StrLCD(dateStr);
            CmdLCD(GOTO_LINE2_POS0 + 14);
            StrLCD(dayStr);
            index = 0;
        }
        /* SAVE & VALIDATE */
        else if(key == '=')
        {
            u8 hh, mm, ss;
            u8 dd, mon, dow;
            u16 yy;
            u8 err_flags = 0; // Bitmask to capture invalid fields

            hh  = ((timeStr[0] - '0') * 10) + (timeStr[1] - '0');
            mm  = ((timeStr[3] - '0') * 10) + (timeStr[4] - '0');
            ss  = ((timeStr[6] - '0') * 10) + (timeStr[7] - '0');

            dd  = ((dateStr[0] - '0') * 10) + (dateStr[1] - '0');
            mon = ((dateStr[3] - '0') * 10) + (dateStr[4] - '0');
            yy  = ((dateStr[6] - '0') * 1000) +
                  ((dateStr[7] - '0') * 100)  +
                  ((dateStr[8] - '0') * 10)   +
                  (dateStr[9] - '0');

            dow = dayStr[0] - '0';

            /* Validate individual constraints */
            if(hh > 23)                 err_flags |= (1 << 0);
            if(mm > 59)                 err_flags |= (1 << 1);
            if(ss > 59)                 err_flags |= (1 << 2);
            if(dd < 1 || dd > 31)       err_flags |= (1 << 3);
            if(mon < 1 || mon > 12)     err_flags |= (1 << 4);
            if(yy < 2000 || yy > 2999)  err_flags |= (1 << 5);
            if(dow > 6)                 err_flags |= (1 << 6);

            /* If any field failed validation */
            if(err_flags != 0)
            {
                CmdLCD(CLEAR_LCD);
                CmdLCD(GOTO_LINE1_POS0);

                /* All inputs failed */
                if(err_flags == 0x7F)
                {
                    StrLCD("INVALID DATA");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("CHECK INPUTS");
                    index = 0; // Start position
                }
                /* Specific field errors with target index positioning */
                else if(err_flags & (1 << 0))
                {
                    StrLCD("INVALID HOUR");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 00-23");
                    index = 0; // Hour tens position
                }
                else if(err_flags & (1 << 1))
                {
                    StrLCD("INVALID MIN");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 00-59");
                    index = 2; // Minute tens position
                }
                else if(err_flags & (1 << 2))
                {
                    StrLCD("INVALID SEC");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 00-59");
                    index = 4; // Second tens position
                }
                else if(err_flags & (1 << 3))
                {
                    StrLCD("INVALID DATE");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 01-31");
                    index = 6; // Date tens position
                }
                else if(err_flags & (1 << 4))
                {
                    StrLCD("INVALID MONTH");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 01-12");
                    index = 8; // Month tens position
                }
                else if(err_flags & (1 << 5))
                {
                    StrLCD("INVALID YEAR");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 2000-2999");
                    index = 10; // Year thousands position
                }
                else if(err_flags & (1 << 6))
                {
                    StrLCD("INVALID DAY");
                    CmdLCD(GOTO_LINE2_POS0);
                    StrLCD("RANGE: 0-6");
                    index = 14; // Day position
                }

                delay_ms(1500);

                /* Restore the Edit Screen for corrections */
                CmdLCD(CLEAR_LCD);
                CmdLCD(GOTO_LINE1_POS0);
                StrLCD(timeStr);
                CmdLCD(GOTO_LINE2_POS0);
                StrLCD(dateStr);
                CmdLCD(GOTO_LINE2_POS0 + 12);
                StrLCD("D:");
                CmdLCD(GOTO_LINE2_POS0 + 14);
                StrLCD(dayStr);
                CmdLCD(DSP_ON_CUR_BLINK);

                continue; // Repositions the cursor automatically to the updated 'index'
            }

            /* Write back to hardware RTC registers when valid */
            SetRTCTimeInfo(hh, mm, ss);
            SetRTCDateInfo(dd, mon, yy);
            SetRTCDay(dow);

            CmdLCD(DSP_ON_CUR_OFF);
            CmdLCD(CLEAR_LCD);
            CmdLCD(GOTO_LINE1_POS0);
            StrLCD("RTC UPDATED");
            delay_ms(1000);
            return;
        }

        /* ENTER NUMBERS + AUTOMATIC CURSOR ADVANCE */
        else if(key >= '0' && key <= '9')
        {
            u8 p = positions[index];

            /* Write numeric value to buffer and LCD */
            if(p <= 7)
            {
                timeStr[p] = key;
                CmdLCD(GOTO_LINE1_POS0 + p);
                CharLCD(key);
            }
            else if(p <= 17)
            {
                dateStr[p - 8] = key;
                CmdLCD(GOTO_LINE2_POS0 + (p - 8));
                CharLCD(key);
            }
            else
            {
                dayStr[0] = key;
                CmdLCD(GOTO_LINE2_POS0 + 14);
                CharLCD(key);
            }

            /* Auto-advance index to next position automatically */
            if(index < 14)
            {
                index++;
            }
        }
    }
}
