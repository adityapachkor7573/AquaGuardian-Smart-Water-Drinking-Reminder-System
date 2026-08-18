
#include "types.h" // Ensures u8, u16, u32 types are available

// Fixed password length (e.g., 4-digit PIN)
#define PASS_LENGTH 4

void Password_Init(void);
u8 Password_Verify(void);
void Password_Change(void);
void RTC_SubMenu(void);
