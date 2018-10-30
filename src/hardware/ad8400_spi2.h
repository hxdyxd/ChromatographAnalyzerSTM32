/* hxdyxd 2018 09 17 */

#ifndef _ad8400_spi2_H_
#define _ad8400_spi2_H_


#include "stm32f10x.h"


void ad8400_spi2_init(void);
void ad8400_spi2_set(uint8_t id, uint16_t val);


#endif
