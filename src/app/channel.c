/******************** (C) COPYRIGHT 2018 Unidoli ********************
* File Name          : channel.c
* Author             : hxdyxd
* Version            : V0
* Date               : 15/10/2018
* Description        : Chromatographic analysis capture card
********************************************************************************
* 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "AD7190.h"
#include "channel.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define CHANNEL_MAX_NUM    (2)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/



/*******************************************************************************
* Function Name  : channel_init.
* Description    : None.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void channel_init(void)
{
    AD7190_Init();
}


/*******************************************************************************
* Function Name  : channel_set.
* Description    : ÉèÖÃÍ¨µÀ.
* Input          : ch, rate.
* Output         : None.
* Return         : None.
*******************************************************************************/
int channel_set(channel_t *conf)
{
    if(conf->ch >= CHANNEL_MAX_NUM) {
        return -1;
    }
    switch(conf->ch) {
        case 0:
            //¼«ÐÔ¡¢Õ¶²¨¡¢ÔöÒæÅäÖÃ¼Ä´æÆ÷
            AD7190_RangeSetup(0, conf->chop, AD7190_CONF_GAIN_1);
            AD7190_ChannelSelect(AD7190_CH_AIN1P_AIN2M);
            //ËÙÂÊ¡¢ÂË²¨Æ÷¡¢REJ60¡¢ÁãÑÓ³ÙÄ£Ê½¼Ä´æÆ÷
            AD7190_ContinuousConvStart(conf->fs, conf->sinc3Filter, conf->rej60, conf->noDelay);
            break;
        case 1:
            //¼«ÐÔ¡¢Õ¶²¨¡¢ÔöÒæÅäÖÃ¼Ä´æÆ÷
            AD7190_RangeSetup(0, conf->chop, AD7190_CONF_GAIN_1);
            AD7190_ChannelSelect(AD7190_CH_AIN3P_AIN4M);
            //ËÙÂÊ¡¢ÂË²¨Æ÷¡¢REJ60¡¢ÁãÑÓ³ÙÄ£Ê½¼Ä´æÆ÷
            AD7190_ContinuousConvStart(conf->fs, conf->sinc3Filter, conf->rej60, conf->noDelay);
            break;
        default:
            ;
    }
    
    return 0;
}


/******************* (C) COPYRIGHT 2018 Unidoli *****END OF FILE****/
