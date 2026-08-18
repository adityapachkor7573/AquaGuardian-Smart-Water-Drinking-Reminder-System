//interrupt.h
#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__
#include "types.h"
extern volatile unsigned char MenuFlag;
extern volatile u8 DrinkFlag;

void EINT0_Init(void);
void Water_Button_Init(void);
#endif
