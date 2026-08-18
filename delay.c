//delay.c
#include "delay.h"
#include "types.h"

void delay_us(u32 dly_us)
{
	for(dly_us*=12;dly_us>0;dly_us--);
}
void delay_ms(u32 dly_ms)
{
		for(dly_ms*=12000;dly_ms>0;dly_ms--);
}
void delay_S(u32 dly_s)
{
	for(dly_s*=12000000;dly_s>0;dly_s--);
}
