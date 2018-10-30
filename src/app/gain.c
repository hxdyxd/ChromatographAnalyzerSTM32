/******************** (C) COPYRIGHT 2018 Unidoli ********************
* File Name          : gain.c
* Author             : hxdyxd
* Version            : V0
* Date               : 15/10/2018
* Description        : Chromatographic analysis capture card
********************************************************************************
* 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

//HAL
#include "data_interface_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define GAIN_MAX_NUM    (5)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
 * 倍数	    电压	Rwb	    十进制		Rw	Rab
 * 1000	    0.3V	12500	63.744		50	50000
 * 10000	0.425V	17700	90.368		50	50000
 * 100000	0.6V	25000	127.744		50	50000
 * 1000000	0.84V	35000	178.944		50	50000
 * 10000000	1.17V	48750	249.344		50	50000
 */
const uint8_t gain_table[GAIN_MAX_NUM] = {  
	64,
	90,
	128,
	179,
	249,
};

/*******************************************************************************
* Function Name  : gain_set.
* Description    : 设置增益.
* Input          : gain.
* Output         : None.
* Return         : None.
*******************************************************************************/
int gain_set(uint8_t gain)
{
	if(gain >= GAIN_MAX_NUM) {
		return -1;
	}
	uint8_t data;
	data = gain_table[gain];
	data_interface_hal_write(HAL_SPI2_CS1)(&data, 1);
	data = gain_table[gain];
	data_interface_hal_write(HAL_SPI2_CS2)(&data, 1);
	return 0;
}




/******************* (C) COPYRIGHT 2018 Unidoli *****END OF FILE****/
