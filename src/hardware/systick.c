/* Systick 2018 06 27 END */
/* By hxdyxd */

#include "systick.h"

//#define CLOCKS_PER_SEC (1000)
uint64_t clk_count = 0;



void systick_init(void)
{
	if( SysTick_Config( SystemCoreClock / 1000 ) )
    { 
        /* Capture error */ 
        while (1);
    }
}

void SysTick_Handler(void)
{
	/* System Tick Counter */
	clk_count++;
}
