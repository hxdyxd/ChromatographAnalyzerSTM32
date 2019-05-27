/******************** (C) COPYRIGHT 2018 Unidoli ********************
* File Name          : main.c
* Author             : hxdyxd
* Version            : V0
* Date               : 26/09/2018
* Description        : Chromatographic analysis capture card
********************************************************************************
* 
*******************************************************************************/

#ifndef __INTERFACE_API_H__
#define __INTERFACE_API_H__

#include <stdint.h>


#define IF_API_CMD_TYPE_SET           0x41      /* A */
#define IF_API_CMD_TYPE_ADC_START     0x42      /* B */
#define IF_API_CMD_TYPE_ADC_END       0x43      /* C */
#define IF_API_CMD_TYPE_POWER_START   0x44      /* D */
#define IF_API_CMD_TYPE_POWER_END     0x45      /* E */

#define IF_API_PACKET_START_CODE      0x82
#define IF_API_PACKET_END_CODE        0x83

#define IF_API_SPARE_DEFAULT          0x00
#define IF_API_SPARE_OK               0x41      /* A */
#define IF_API_SPARE_FAILED           0x42      /* B */

#define IF_API_CHECKSUM_LEN            2
#define CONV_PLUS4(A)                 ((A)+4)

typedef enum {
	IF_API_CH1 = 0, 
	IF_API_CH2 = 1,
}IF_API_Set_CHx_t;

typedef enum {
	IF_API_Filter_SINC3   = 0, 
	IF_API_Filter_SINC4   = 1,
}IF_API_Set_Filter_t;

typedef enum {
	IF_API_CHOP_Disable   = 0, 
	IF_API_CHOP_Enable    = 1,
}IF_API_Set_CHOP_t;

typedef enum {
	IF_API_NoDelay           = 0, 
	IF_API_NoDelay_Disable   = 1,
}IF_API_Set_NoDelay_t;

typedef enum {
	IF_API_REJ60_0  = 0,
	IF_API_REJ60_1  = 1,
}IF_API_Set_REJ60_t;

typedef enum {
	IF_API_Gain_1000       = 0, 
	IF_API_Gain_10000      = 1,
	IF_API_Gain_100000     = 2,
	IF_API_Gain_1000000    = 3,
	IF_API_Gain_10000000   = 4,
}IF_API_Set_Gain_t;

typedef struct {
	IF_API_Set_CHx_t      ch;
	IF_API_Set_Filter_t   filter;
	IF_API_Set_CHOP_t     chop;
	IF_API_Set_NoDelay_t  nodelay;
	IF_API_Set_REJ60_t    rej60;
	IF_API_Set_Gain_t     gain;
	uint16_t              fs;
}if_api_v10_23_t;


/*******************************************************************************
* Function Name  : if_api_verify_pack.
* Description    : Verify checksum.
* Input          : None.
* Output         : None.
* Return         : 0 success.
				  -1 failed.
*******************************************************************************/
int if_api_verify_pack(uint8_t *in_buffer, int in_len);

/*******************************************************************************
* Function Name  : if_api_calculate_checksum.
* Description    : Calculate checksum. 校验会被添加到数据后两字节内
* Input          : 数据长度， 数据指针.
* Output         : None.
* Return         : None.
*******************************************************************************/
int if_api_calculate_checksum(void *buf, unsigned char len);

/*******************************************************************************
* Function Name  : if_api_data_CMD_SET_parse.
* Description    : Parsing setup parameters.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int if_api_data_CMD_SET_parse(uint8_t *in_buffer, int in_len, if_api_v10_23_t *out);

/*******************************************************************************
* Function Name  : if_api_data_set_pack.
* Description    : set pack.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int if_api_data_set_pack(uint8_t *in_buffer_plus_4, uint8_t in_len, uint8_t cmd_type, uint8_t spare);






#endif
/******************* (C) COPYRIGHT 2018 Unidoli *****END OF FILE****/
