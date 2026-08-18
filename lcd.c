//lcd.c
#include <lpc214x.h>
#include "lcd.h"
#include "lcd-defines.h"
#include "delay.h"


void writeLCD(u8 byte) 
{ 

	IO0CLR = 1 << LCD_RW; 
	WRITEBYTE(IO0PIN, LCD_DATA, byte); 
	IO0SET = 1 << LCD_EN;

	delay_us(1);
	IO0CLR = 1 << LCD_EN; 
	/* EN=LOW (falling edge -> LCD reads) */ 
	delay_ms(2); 
}

void CmdLCD(u8 cmd) 
{
	IO0CLR = 1 << LCD_RS; 
	/* RS=0: command register */
	writeLCD(cmd); 
}

void CharLCD(u8 asciiVal)
{
	IO0SET = 1 << LCD_RS;
	/* RS=1: data register */
	writeLCD(asciiVal); 
}

void InitLCD(void)
{
	WRITEBYTE(IO0DIR, LCD_DATA, 0xFF);
	SETBIT(IO0DIR, LCD_RS); 
	SETBIT(IO0DIR, LCD_RW); 
	SETBIT(IO0DIR, LCD_EN);
	delay_ms(15);
	CmdLCD(0x30);
	delay_ms(4);
	CmdLCD(0x30); 
	delay_us(100);
	CmdLCD(0x30);
	CmdLCD(MODE_8BIT_2LINE);
	CmdLCD(DSP_ON_CUR_BLINK);
	CmdLCD(CLEAR_LCD); 
	delay_ms(2);
	CmdLCD(SHIFT_CUR_RIGHT);
}

void StrLCD(s8 *s) 
{
	while(*s) CharLCD(*s++);
} 

void u32LCD(u32 n) 
{
	s32 i = 0; 
	u8 a[10];
	if(n == 0)
	{
		CharLCD('0'); 
	return;
	}
	while(n > 0)
	{
		a[i++] = (n % 10) + 48; 
		n /= 10;
	} 
	for(i -= 1; i >= 0; i--)
						CharLCD(a[i]);
}

void s32LCD(s32 n)
{ 
	if(n < 0)
	{	
		CharLCD('-');
		n = -n; 
	}
	u32LCD(n);
}

void f32LCD(f32 fn, u8 nDP)
{ 
	u32 n, i;
	if(fn < 0.0)
	{
		CharLCD('-');
		fn = -fn;
	}
	n = fn; 
	u32LCD(n);
	CharLCD('.'); 
	for(i = 0; i < nDP; i++)
	{
		fn = (fn - n) * 10; 
		n = fn;
		CharLCD(n + 48);
	}
}

void BuildCGRAM(u8 *p, u8 nBytes)
{ 
	u32 i; 
	CmdLCD(GOTO_CGRAM_START); 
	IO0CLR = 1 << LCD_RW;
	IO0SET = 1 << LCD_RS; 
	for(i = 0; i < nBytes; i++) 
			writeLCD(p[i]); 
	CmdLCD(GOTO_LINE1_POS0); 
}

/* 5 Custom Glass Byte Maps (Empty to Full) */
const unsigned char Glass_Frames[5][8] = {
    { 0x1F, 0x11, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x00 }, // 0% Empty
    { 0x1F, 0x11, 0x11, 0x11, 0x11, 0x1F, 0x1F, 0x00 }, // 25%
    { 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F, 0x00 }, // 50%
    { 0x1F, 0x11, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00 }, // 75%
    { 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00 }  // 100% Full
};

/* Load custom glass graphics into CGRAM */
void Load_Glass_Custom_Chars(void)
{
    unsigned char i, j;
    CmdLCD(0x40); // CGRAM address
    for(i = 0; i < 5; i++)
    {
        for(j = 0; j < 8; j++)
        {
            CharLCD(Glass_Frames[i][j]);
        }
    }
}

/* Print Custom Glass Frame matching current percentage */
void Display_Animated_Glass(unsigned int percentage)
{
    if(percentage >= 100)  
		{			
			CharLCD(4);
		}
    else if(percentage >= 75) 
		{
			CharLCD(3);
		}
    else if(percentage >= 50) 
		{
			CharLCD(2);
		}
    else if(percentage >= 25) 
		{
			CharLCD(1);
		}
    else                      
		{
			CharLCD(0);
		}
}
