//lcd.h
#include "types.h" 
void writeLCD(u8 byte); 
void CmdLCD(u8 cmd); 
void CharLCD(u8 asciiVal); 
void InitLCD(void); 
void StrLCD(s8 *s); 
void u32LCD(u32 n); 
void s32LCD(s32 n); 
void f32LCD(f32 fn, u8 nDP);
void BuildCGRAM(u8 *p, u8 nBytes);
void Display_Animated_Glass(unsigned int percentage);
void Load_Glass_Custom_Chars(void);
