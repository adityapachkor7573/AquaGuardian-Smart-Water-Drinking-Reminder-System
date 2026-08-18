//rtc_defines.h

#include <lpc214x.h>

/* Clock Settings  */

#define FOSC    12000000
#define CCLK    (5*FOSC)
#define PCLK    (CCLK/4)

/* RTC Prescaler */

#define PREINT_VAL   ((int)(PCLK/32768)-1)
#define PREFRAC_VAL  (PCLK-((PREINT_VAL+1)*32768))

/* CCR Bits */

#define RTC_ENABLE   (1<<0)
#define RTC_RESET    (1<<1)
#define RTC_CLKSRC   (1<<4)

/* CPU */

#define CPU_LPC2148

/* LCD Positions */

#define TIME_POS   GOTO_LINE1_POS0
#define DAY_POS    (GOTO_LINE1_POS0+10)
#define DATE_POS   GOTO_LINE2_POS0

/* Days */

#define SUN 0
#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6
