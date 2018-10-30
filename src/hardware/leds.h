
#ifndef __LEDS_H__
#define __LEDS_H__

#include "stm32f10x.h"

struct sGpio
{
	uint32_t RCC_APB2Periph;
	uint16_t GPIO_Pin;
	GPIO_TypeDef* GPIOx;
};


#define PMT_POWER_OFF(id) led_on(id)
#define PMT_POWER_ON(id) led_off(id)


void leds_init(void);
void led_on(int id);
void led_off(int id);
void led_rev(int id);

#endif

