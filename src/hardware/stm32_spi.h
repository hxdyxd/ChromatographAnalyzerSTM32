/* 
 *
 *2018 09 21 & hxdyxd
 *
 */

#ifndef __STM32_H__
#define __STM32_H__

#include "stm32f10x.h"

typedef enum
{
	STM32_SPI1 = 0,
	STM32_SPI2_CS1 = 1,
	STM32_SPI2_CS2 = 3,
}STM32_SPI_ID;

void stm32_spi_init(void);
uint16_t stm32_spi_wr(STM32_SPI_ID id, uint16_t val);


#endif
