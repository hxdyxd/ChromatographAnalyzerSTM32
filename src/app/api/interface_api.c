/******************** (C) COPYRIGHT 2018 Unidoli ********************
* File Name          : main.c
* Author             : hxdyxd
* Version            : V0
* Date               : 26/09/2018
* Description        : Chromatographic analysis capture card
********************************************************************************
* 
*******************************************************************************/

#include "interface_api.h"


#pragma  pack(1)
typedef struct {
    uint8_t ch_2;
    uint8_t filter_sinc4;
    uint8_t chop_on;
    uint8_t nodelay_off;
    uint8_t rej60_1;
    uint8_t gain[4];
    uint8_t fs_bit98;
    uint8_t fs_bit7654321;
}if_api_pack;
#pragma pack()

#define SAMPLE_DECODE(bit24_31, bit16_23, bit8_15, bit0_7)   (((bit24_31) << 21) | ((bit16_23) << 14) | ((bit8_15) << 7) | (bit0_7))


int if_api_calculate_checksum(void *buf, unsigned char len)
{
    unsigned char *p = (unsigned char *)buf;
    unsigned int sum = 0;
    unsigned char i;
    for(i=0;i<len;i++,p++)
        sum += *p;
#ifdef DEBUG_CHECKSUM
    printf("sum = %d \r\n", sum);
#endif
    sum = (sum % 65536) ^ 0xffff;
#ifdef DEBUG_CHECKSUM
    printf("rev.hex = 0x%04x \r\n", sum);
#endif
    *p = (sum & 0x7f00) >> 8;
    p++;
    *p = sum & 0x7f;
    return len + 2;
}

/*
 *  api farme format
 * [1] + [n] + [2] + [1]
 * [START_CODE] + [DATA] + [CHECKSUM] + [END_CODE]
 */
int if_api_verify_pack(uint8_t *in_buffer, int in_len)
{
    if( in_len < 7 ){
        return -1;
    }
    
    if( in_buffer[0] != IF_API_PACKET_START_CODE || in_buffer[in_len-1] != IF_API_PACKET_END_CODE ) {
        return -1;
    }
    
    uint8_t chechsum_low = in_buffer[in_len-2];
    uint8_t chechsum_high = in_buffer[in_len-3];
    
    in_buffer[in_len-2] = 0;
    in_buffer[in_len-3] = 0;
    
    if_api_calculate_checksum(in_buffer+1, in_len-4);  //数据长度， 数据指针
    
    if(chechsum_low != in_buffer[in_len-2] || chechsum_high != in_buffer[in_len-3]) {
        return -1;
    }
    
    return 0;
}

/*
 *  [DATA] farme format
 * [1] + [1] + [1] + [n]
 * [CMD_TYPE] + [SPARE] + [LENGTH] + [DATA]
 */
int if_api_data_CMD_SET_parse(uint8_t *in_buffer, int in_len, if_api_v10_23_t *out)
{
    
    if( in_buffer[0] != IF_API_CMD_TYPE_SET ) {
        return -1;
    }
    
    if(in_buffer[2] < sizeof(if_api_pack) ){
        return -1;
    }
    
    if_api_pack *api_pack = (if_api_pack *)(&in_buffer[3]);
    
    out->ch = (api_pack->ch_2 == 0x2) ? IF_API_CH2 : IF_API_CH1;
    out->filter = (api_pack->filter_sinc4 == 0x2) ? IF_API_Filter_SINC4 : IF_API_Filter_SINC3;
    out->chop = (api_pack->chop_on == 0x2) ? IF_API_CHOP_Enable : IF_API_CHOP_Disable;
    out->nodelay = (api_pack->nodelay_off == 0x2) ? IF_API_NoDelay_Disable : IF_API_NoDelay;
    out->rej60 = (api_pack->rej60_1 == 0x2) ? IF_API_REJ60_1 : IF_API_REJ60_0;
    out->gain = SAMPLE_DECODE(api_pack->gain[0], api_pack->gain[1], api_pack->gain[2], api_pack->gain[3]);
    uint16_t fs = (api_pack->fs_bit98<<7) | api_pack->fs_bit7654321;
    if(fs != 0) {
        out->fs = fs;
    }
    
    return 0;
}

/*
 *
 * 序号   0     [  1          2       3        4     ]    5       6
 * 字节   1     [  1          1       1      LENGTH  ]    2       1
 * 含义 0x82    [ CMD       SPARE   LENGTH    DATA   ] CheckSum  0x83
 *
 */
int if_api_data_set_pack(uint8_t *in_buffer_plus_4, uint8_t in_len, uint8_t cmd_type, uint8_t spare)
{
    *in_buffer_plus_4 = IF_API_PACKET_START_CODE;
    *(in_buffer_plus_4 + 1) = cmd_type & 0x7f;
    *(in_buffer_plus_4 + 2) = spare & 0x7f;
    *(in_buffer_plus_4 + 3) = in_len & 0x7f;
    
    if_api_calculate_checksum(in_buffer_plus_4 + 1, in_len + 3);
    
    *(in_buffer_plus_4 + 4 + in_len + IF_API_CHECKSUM_LEN) = IF_API_PACKET_END_CODE;
    return (in_len + 7);
}


/******************* (C) COPYRIGHT 2018 Unidoli *****END OF FILE****/


