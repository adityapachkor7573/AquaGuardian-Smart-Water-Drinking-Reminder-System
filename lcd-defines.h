//lcd-defines.h

#define LCD_DATA 8 /* DB0-DB7 at P0.8-P0.15 */
#define LCD_RS 19 /* P0.16 - Register Select */
#define LCD_RW 20 /* P0.17 - Read/Write */
#define LCD_EN 21 /* P0.18 - Enable */ 

/* HD44780 Commands */
#define CLEAR_LCD 0x01 
#define RET_CUR_HOME 0x02 
#define SHIFT_CUR_RIGHT 0x06
#define SHIFT_CUR_LEFT 0x07
#define DSP_OFF 0x08 
#define DSP_ON_CUR_OFF 0x0C 
#define DSP_ON_CUR_ON 0x0E
#define DSP_ON_CUR_BLINK 0x0F 
#define SHIFT_DISP_LEFT 0x16
#define SHIFT_DISP_RIGHT 0x14 
#define MODE_8BIT_2LINE 0x38
#define MODE_4BIT_2LINE 0x28 
#define GOTO_LINE1_POS0 0x80 
#define GOTO_LINE2_POS0 0xC0
#define GOTO_LINE3_POS0 0x94
#define GOTO_LINE4_POS0 0xD4 
#define GOTO_CGRAM_START 0x40
