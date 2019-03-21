#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <stdint.h>
#include "AD7190.h"


typedef struct {
	uint8_t ch;
	uint16_t fs;
	uint8_t chop;
	uint8_t sinc3Filter;
	uint8_t rej60;
	uint8_t noDelay;
}channel_t;

/*******************************************************************************
* Function Name  : channel_init.
* Description    : None.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void channel_init(void);

/*******************************************************************************
* Function Name  : channel_set.
* Description    : …Ë÷√Õ®µ¿.
* Input          : ch, rate.
* Output         : None.
* Return         : None.
*******************************************************************************/
int channel_set(channel_t *conf);



#endif
