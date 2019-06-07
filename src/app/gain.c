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
#include <math.h>

#include <app_debug.h>

//HAL
#include "data_interface_hal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define GAIN_MAX_NUM    (5)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
 * 倍数     电压    Rwb     十进制      Rw  Rab
 * 1000     0.3V    12500   63.744      50  50000
 * 10000    0.425V  17700   90.368      50  50000
 * 100000   0.6V    25000   127.744     50  50000
 * 1000000  0.84V   35000   178.944     50  50000
 * 10000000 1.17V   48750   249.344     50  50000
 */
const uint8_t gain_table[GAIN_MAX_NUM] = {  
    64,
    90,
    128,
    179,
    249,
};

#define RES_RAB  50000
#define RES_RW   50
#define RES_REF  1.2
const double gain_kp[4] =  {0.0033,   -0.0161,    0.1149,    0.0099};


/*******************************************************************************
* Function Name  : gain_set.
* Description    : 设置增益.
* Input          : gain.
* Output         : None.
* Return         : None.
*******************************************************************************/
int gain_set(uint8_t ch, uint32_t gain)
{
    if(gain > 10000000) {
        APP_DEBUG("gain failed \r\n");
        return -1;
    }
    
    double gaind = log10(gain);
    double control_voltage = gain_kp[0] * (gaind*gaind*gaind) + gain_kp[1] * (gaind*gaind) +  gain_kp[2] * (gaind) +  gain_kp[3];
    uint8_t res =  (uint8_t)((((control_voltage/RES_REF*RES_RAB) - RES_RW) * 256) / RES_RAB + 0.5);
    APP_DEBUG("gain = %d, control_voltage = %.3f, res = %d\r\n", gain, control_voltage, res);
    
    switch(ch) {
    case 0:
        data_interface_hal_write(HAL_SPI2_CS1)(&res, 1);
        break;
    case 1:
        data_interface_hal_write(HAL_SPI2_CS2)(&res, 1);
        break;
    default:
        ;
    }
    return 0;
}




/******************* (C) COPYRIGHT 2018 Unidoli *****END OF FILE****/
