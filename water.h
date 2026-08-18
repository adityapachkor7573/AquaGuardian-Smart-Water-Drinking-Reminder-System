
#include "types.h"
extern u8 WaterCount;
extern u8 WaterGoal;

extern volatile u8 DrinkFlag;

extern u8 WaterCount;

extern u8 WaterGoal;
void Water_Init(void);
void Set_Water_Goal(u8 goal);
u8 Get_Water_Goal(void);
void Drink_Water(void);
u8 Get_Water_Count(void);
void Check_Day_Reset(void);
void Get_Water_Status(u8 *count,u8 *goal);
void Water_Goal_Menu(void);
void Display_Number(u16 num);
void Wait_Key_Release(void);
void Increase_Water_Count(void);
void Check_Goal_Completion(void);
void Handle_Missed_Reminder_Increment(void);
void Check_Missed_Reminders(void);
void Load_Glass_Emoji(void);
