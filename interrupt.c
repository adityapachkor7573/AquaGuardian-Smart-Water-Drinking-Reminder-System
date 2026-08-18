//interrupt.c
#include <lpc214x.h>
#include "delay.h"
#include "interrupt.h"    
extern volatile u8 DrinkFlag;
volatile unsigned char MenuFlag = 0;

/* EINT0 ISR */
__irq void EINT0_ISR(void)
{
    MenuFlag=1;          
    EXTINT = (1<<0);      
    VICVectAddr = 0;     
}

void EINT0_Init(void)
{
    /* P0.1 as EINT0 */
    PINSEL0 |= 0x0000000C;
		PINSEL0 |=  (1<<2);

    EXTMODE |= (1<<0);

    EXTPOLAR &= ~(1<<0);

    EXTINT = (1<<0);
    VICIntSelect &= ~(1<<14);
    VICVectAddr0 = (unsigned long)EINT0_ISR;
    VICVectCntl0 = 0x20 | 14;
    VICIntEnable = (1<<14);
}

/* ISR for Button 2 on P0.30 (EINT3) */
void EINT3_ISR(void) __irq {
    DrinkFlag = 1;          
    EXTINT = (1 << 3);      
    VICVectAddr = 0;        
}
void Water_Button_Init(void) {
    PINSEL1 &= ~(3 << 28);
    PINSEL1 |=  0x25400000;;  // P0.30 = EINT3

    EXTMODE  |= (1 << 3);   
    EXTPOLAR &= ~(1 << 3);  
    EXTINT    = (1 << 3);   

    VICIntSelect &= ~(1 << 17);     
    VICVectAddr1  = (unsigned)EINT3_ISR;
    VICVectCntl1  = (1 << 5) | 17; 
    VICIntEnable |= (1 << 17);
}
