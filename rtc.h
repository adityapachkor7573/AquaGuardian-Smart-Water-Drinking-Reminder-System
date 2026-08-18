//rtc.h
#include "types.h"

/* Function Prototypes */

void RTC_Init(void);

void GetRTCTimeInfo(s32 *, s32 *, s32 *);
void SetRTCTimeInfo(u32, u32, u32);
void DisplayRTCTime(u32, u32, u32);

void GetRTCDateInfo(s32 *, s32 *, s32 *);
void SetRTCDateInfo(u32, u32, u32);
void DisplayRTCDate(u32, u32, u32);

void GetRTCDay(s32 *);
void SetRTCDay(u32);
void DisplayRTCDay(u32);
void RTC_Edit(void);
