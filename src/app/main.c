/******************** (C) COPYRIGHT 2018 Unidoli ********************
* File Name          : main.c
* Author             : hxdyxd
* Version            : V0
* Date               : 26/09/2018
* Description        : Chromatographic analysis capture card
********************************************************************************
* 
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
//STDLIB
#include <stdio.h>
#include <string.h>

//HAL
#include "data_interface_hal.h"

//DEBUG
#include "app_debug.h"

//API
#include "interface_api.h"
#include "gain.h"
#include "channel.h"
#include "soft_timer.h"

#define SOFT_TIMER_POWER_ON_DELAY   0
#define SOFT_TIMER_ACTIVE_DETECT    1
#define SOFT_TIMER_LED_FLASH        4

//两小时保护
#define AUTO_TURN_OFF            (7200*1000)


#undef NO_TIMESTAMP

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#ifdef NO_TIMESTAMP
    #define SINGLE_SAMPLE_NUM         (12)
    #define SINGLE_SAMPLE_SIZE_NUM    (SINGLE_SAMPLE_NUM*4)
#else
    #define SINGLE_SAMPLE_NUM         (6)
    #define SINGLE_SAMPLE_SIZE_NUM    (SINGLE_SAMPLE_NUM*8)
#endif

#define SAMPLE_DECODE(bit24_31, bit16_23, bit8_15, bit0_7)   (((bit24_31) << 18) | ((bit16_23) << 12) | ((bit8_15) << 6) | (bit0_7))

#define ADC_TO_VOLTAGE(v)      (((v)*1.0/0x7fffff-1)*6.25)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static unsigned char buf[512];
static unsigned char cmd_buf[512];

static uint32_t gs_start_adc = 0;
static uint32_t gs_power_on = 0;

static if_api_v10_23_t gs_api_config = {
    .ch =       IF_API_CH1,
    .filter =   IF_API_Filter_SINC4,
    .chop =     IF_API_CHOP_Disable,
    .nodelay =  IF_API_NoDelay_Disable,
    .rej60 =    IF_API_REJ60_0,
    .gain =     IF_API_Gain_10000000,
    .fs =       1,
};

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


void api_set_config(void)
{
    /* 设置通道 */
    channel_t ch_conf;
    ch_conf.ch = gs_api_config.ch;
    ch_conf.sinc3Filter = (gs_api_config.filter == IF_API_Filter_SINC3);
    ch_conf.chop = (gs_api_config.chop == IF_API_CHOP_Enable);
    ch_conf.noDelay = (gs_api_config.nodelay == IF_API_NoDelay);
    ch_conf.rej60 = (gs_api_config.rej60 == IF_API_REJ60_1);
    ch_conf.fs = gs_api_config.fs;
    channel_set(&ch_conf);
}


void power_on_reply_proc(void)
{
    /* 设置增益 */
    switch( gs_api_config.ch ) {
    case IF_API_CH1:
        gain_set(0, gs_api_config.gain);
        break;
    case IF_API_CH2:
        gain_set(1, gs_api_config.gain);
        break;
    default:
        APP_ERROR("gain set filed \r\n");
        return;
    }
    gs_power_on = 1;
}


void connection_turn_of_proc(void)
{
    uint8_t data = 0;
    data_interface_hal_write(HAL_GPIO1)(&data, 1);
    data_interface_hal_write(HAL_GPIO2)(&data, 1);
    gs_power_on = 0;
}


/*******************************************************************************
* Function Name  : usb_callback.
* Description    : USB接收回调函数.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void usb_callback(uint8_t *p, int len)
{
    if(if_api_verify_pack(p, len) < 0) {
        return;
    }
    
    uint8_t data;
    uint8_t cmd_type = p[1];
    /**************************************************************
     *  data farme format
     *
     * [START_CODE] + [DATA] + [CHECKSUM] + [END_CODE]
     **************************************************************/
    int ret;
    switch(cmd_type) {
    //配置
    case IF_API_CMD_TYPE_SET:
        /*
         *       [1]      +   [n]  +     [2]    +    [1]
         *   [START_CODE] + [DATA] + [CHECKSUM] + [END_CODE]
         */
        ret = if_api_data_CMD_SET_parse(p + 1, len - 4, &gs_api_config);
        if(ret < 0) {
            ret = if_api_data_set_pack( cmd_buf, 0, IF_API_CMD_TYPE_SET, IF_API_SPARE_FAILED);
        } else {
            ret = if_api_data_set_pack( cmd_buf, 0, IF_API_CMD_TYPE_SET, IF_API_SPARE_OK);
        }

        data_interface_hal_write(HAL_USB1)(cmd_buf, ret);
        /* 设置参数 */
        api_set_config();
        APP_DEBUG("parametra set success, ch = %d, gain = %d\r\n", gs_api_config.ch, gs_api_config.gain);
        break;
    //開始传输
    case IF_API_CMD_TYPE_ADC_START:
        /* ADC turn on */
        gs_start_adc++;
        APP_DEBUG("adc transfer start\r\n");
        break;
    //结束传输
    case IF_API_CMD_TYPE_ADC_END:
        /* ADC turn off */
        if(gs_start_adc == 0) {
            ret = if_api_data_set_pack( cmd_buf, 0, IF_API_CMD_TYPE_ADC_END, IF_API_SPARE_FAILED);
        } else {
            ret = if_api_data_set_pack( cmd_buf, 0, IF_API_CMD_TYPE_ADC_END, IF_API_SPARE_OK);
        }
        data_interface_hal_write(HAL_USB1)(cmd_buf, ret);
        gs_start_adc = 0;
        APP_DEBUG("adc transfer end\r\n");
        break;
    //开启电源
    case IF_API_CMD_TYPE_POWER_START:
        /* Relay turn on */
        ret = if_api_data_set_pack( cmd_buf, 0, IF_API_CMD_TYPE_POWER_START, IF_API_SPARE_OK);
        data_interface_hal_write(HAL_USB1)(cmd_buf, ret);
        
        //关闭电源
        connection_turn_of_proc();
        //开启通道X电源
        data = 1;
        switch( gs_api_config.ch ) {
        case IF_API_CH1:
            data_interface_hal_write(HAL_GPIO1)(&data, 1);
            break;
        case IF_API_CH2:
            data_interface_hal_write(HAL_GPIO2)(&data, 1);
            break;
        default:
             return;
        }
        
        //更新活跃时间
        soft_timer_create(SOFT_TIMER_ACTIVE_DETECT, 1, 0, connection_turn_of_proc, AUTO_TURN_OFF);
        //delay ，wait power on
        soft_timer_create(SOFT_TIMER_POWER_ON_DELAY, 1, 0, power_on_reply_proc, 1000);
        APP_DEBUG("power on, ch = %d\r\n", gs_api_config.ch);
        break;
    //关闭电源
    case IF_API_CMD_TYPE_POWER_END:
        /* Relay turn off */
        ret = if_api_data_set_pack( cmd_buf, 0, IF_API_CMD_TYPE_POWER_END, IF_API_SPARE_OK);
        data_interface_hal_write(HAL_USB1)(cmd_buf, ret);
    
        //关闭电源
        connection_turn_of_proc();
        soft_timer_delete(SOFT_TIMER_ACTIVE_DETECT);
        APP_DEBUG("power off, ch = %d\r\n", gs_api_config.ch);
        break;
    
    default:
        break;
    }
}

/*******************************************************************************
* Function Name  : adc_conv_prov.
* Description    : ADC数据采集发送.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void adc_conv_proc(void)
{
    static uint8_t gs_last_start_adc = 0;
    if(gs_start_adc) {
        //更新活跃时间
        soft_timer_create(SOFT_TIMER_ACTIVE_DETECT, 1, 0, connection_turn_of_proc, AUTO_TURN_OFF);
#ifdef NO_TIMESTAMP
        AD7190_ContinuousConvRead( SINGLE_SAMPLE_NUM, CONV_PLUS4(buf) );
        uint8_t pack_len = if_api_data_set_pack( buf, SINGLE_SAMPLE_SIZE_NUM, IF_API_CMD_TYPE_ADC_START, IF_API_SPARE_OK);
        data_interface_hal_write(HAL_USB1)( buf, pack_len);
#else
        AD7190_ContinuousConvReadAddTimestamp( SINGLE_SAMPLE_NUM, CONV_PLUS4(buf), (gs_last_start_adc != gs_start_adc) );
        uint8_t pack_len = if_api_data_set_pack( buf, SINGLE_SAMPLE_SIZE_NUM, IF_API_CMD_TYPE_ADC_START, IF_API_SPARE_OK);
        data_interface_hal_write(HAL_USB1)( buf, pack_len);
#endif
    }
    //记录
    gs_last_start_adc = gs_start_adc;
}


void led_flash_proc(void)
{
    static uint8_t led2_status = 0;
    
    if(gs_start_adc && gs_power_on) {
        led2_status = !led2_status;
    } else if(gs_power_on) {
        led2_status = 1;
    } else {
        led2_status = 0;
    }
    data_interface_hal_write(HAL_LED2)(&led2_status, 1);
}


/*******************************************************************************
* Function Name  : main.
* Description    : Main routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
int main(void)
{
    soft_timer_init();
    /* 初始化硬件 */
    data_interface_hal_init();
    APP_DEBUG("Build , %s %s \r\n", __DATE__, __TIME__);
    
    uint8_t led_status;
    if(hal_read_gpio_j5()) {
        led_status = 1;
        data_interface_hal_set(0);
    } else {
        led_status = 0;
        data_interface_hal_set(1);
    }
    if(hal_read_gpio_j7()) {
        // USB/RS485 指示灯
        data_interface_hal_write(HAL_LED1)(&led_status, 1);
        led_status = 1;
        soft_timer_create(SOFT_TIMER_LED_FLASH, 1, 1, led_flash_proc, 250);
    } else {
        led_status = 0;
    }
    data_interface_hal_write(HAL_LED2)(&led_status, 1);

    /* 初始化电源 */
    uint8_t data;
    data = 0;
    data_interface_hal_write(HAL_GPIO1)(&data, 1);
    data = 0;
    data_interface_hal_write(HAL_GPIO2)(&data, 1);
    
    /* 初始化通道 */
    channel_init();
    /* 设置参数 */
    api_set_config();
    
    
    while (1) {
        soft_timer_proc();
        adc_conv_proc();    //adc proc            
        data_interface_hal_read_proc(usb_callback);  //data interface proc
    }
}

/******************* (C) COPYRIGHT 2018 Unidoli *****END OF FILE****/
