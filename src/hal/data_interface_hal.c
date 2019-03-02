/* 
 *
 *2018 09 21 & hxdyxd
 *
 */

#include "data_interface_hal.h"
#include "stm32f10x.h"

//TICKET
#include "systick.h"

//GPIO
#include "leds.h"

//USB
#include "usb_lib.h"
#include "usb_desc.h"
#include "hw_config.h"
#include "usb_pwr.h"

//SPI
#include "stm32_spi.h"


uint64_t timer;

static void usb_port_set(u8 enable)
{  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	if(enable)_SetCNTR(_GetCNTR()&(~(1<<1)));//�˳��ϵ�ģʽ
	else
	{	  
		_SetCNTR(_GetCNTR()|(1<<1));  // �ϵ�ģʽ
		GPIOA->CRH&=0XFFF00FFF;
		GPIOA->CRH|=0X00033000;
		GPIO_ResetBits(GPIOA, GPIO_Pin_12);
	}
}

/* some low level platform function */

static void data_interface_hal_empty(uint8_t *p, uint8_t len)
{
}

static void data_interface_hal_spi1(uint8_t *p, uint8_t len)
{
	*p = stm32_spi_wr(STM32_SPI1, *p);
}

static void data_interface_hal_spi2_cs1(uint8_t *p, uint8_t len)
{
	*p = stm32_spi_wr(STM32_SPI2_CS1, *p);
}

static void data_interface_hal_spi2_cs2(uint8_t *p, uint8_t len)
{
	*p = stm32_spi_wr(STM32_SPI2_CS2, *p);
}

static void data_interface_hal_gpio1(uint8_t *p, uint8_t len)
{
	if(*p) {
		led_off(0);
	} else {
		led_on(0);
	}
}

static void data_interface_hal_gpio2(uint8_t *p, uint8_t len)
{
	if(*p) {
		led_off(1);
	} else {
		led_on(1);
	}
}

/*	
 * ��Ҫע�⼸�㣺
 * 1)TXǰ��Ҫ�ж�    if(bDeviceState == CONFIGURED)����USBû��Ļ������USB TX�Ͷ�����
 * 2)tx����ǰ����Ҫ�ж�            while (GetEPTxStatus(ENDP1) == EP_TX_VALID); //0x30�����Ƿ�TX����
 * 3)ÿ�����64�ֽڣ������������ǡ����64�ֽڣ�TX֮��PC�����ղ����ģ����뷢һ��EOF���ܡ�������ǰ64���ֽڡ�����֪���㲻��������bug����
	����һ�ַ�������ÿ����һ֡���ݣ��ж������Ƿ�ǡ����64�ֽڣ�����ǣ��ͷ���һ��EOF����һ�ַ�������ÿ��TX��������ݳ�����63���ֽڣ��ܹ�64.
*/
static void data_interface_hal_usb1(uint8_t *p, uint8_t len)
{
	if(bDeviceState == CONFIGURED) {
		while (GetEPTxStatus(ENDP1) == EP_TX_VALID);
		UserToPMABufferCopy(p, ENDP1_TXADDR, len);
		SetEPTxCount(ENDP1, len);
		SetEPTxValid(ENDP1);
	}
	
}

static void data_interface_hal_usb2(uint8_t *p, uint8_t len)
{
	
	if(bDeviceState == CONFIGURED) {
		while (GetEPTxStatus(ENDP4) == EP_TX_VALID);
		UserToPMABufferCopy(p, ENDP4_TXADDR, len);
		SetEPTxCount(ENDP4, len);
		SetEPTxValid(ENDP4);
	}
	
}



/* public hal function */

void data_interface_hal_init(void)
{
	//CLOCK INTERFACE
	systick_init();
	//GPIO INTERFACE
	leds_init();
	
	//USB INTERFACE
	usb_port_set(0);
	timer =  clk_count;
	while(  clk_count - timer < 300 );
	usb_port_set(1);
	//Set_System();
	Set_USBClock();
	USB_Interrupts_Config();
	USB_Init();
	
	//SPI INTERFACE
	stm32_spi_init();
}


extern uint8_t buffer_out[VIRTUAL_COM_PORT_DATA_SIZE];
extern __IO uint32_t count_out;

void data_interface_hal_read_proc(void (*callback_func)(uint8_t *, uint8_t))
{
	if(callback_func != NULL && count_out != 0) {
		callback_func(buffer_out, count_out);
		count_out = 0;
		SetEPRxValid(ENDP3);
	}
}

int get_connection_state(void)
{
	return (bDeviceState == CONFIGURED);
}


void (*data_interface_hal_write(HAL_WRITE_ID id))(uint8_t *p, uint8_t len)
{
	switch(id) {
	case HAL_SPI1:
		return data_interface_hal_spi1;
	
	case HAL_SPI2_CS1:
		return data_interface_hal_spi2_cs1;
	
	case HAL_SPI2_CS2:
		return data_interface_hal_spi2_cs2;
	
	case HAL_GPIO1:
		return data_interface_hal_gpio1;
	
	case HAL_GPIO2:
		return data_interface_hal_gpio2;
	
	case HAL_USB1:
		return data_interface_hal_usb1;
	
	case HAL_USB2:
		return data_interface_hal_usb2;
	
	default:
		return data_interface_hal_empty;
	}
}

uint64_t hal_read_TickCounter(void)
{
	return clk_count;
}

uint8_t hal_read_gpio_spi1_miso(void)
{
	return (uint8_t)GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
}


/******************* (C) COPYRIGHT 2018 hxdyxd *****END OF FILE****/
