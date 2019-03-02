/* 
 *
 *2018 09 21 & hxdyxd
 *
 */


#ifndef __data_interface_hal_H__
#define __data_interface_hal_H__

#include <stdint.h>

typedef enum
{
	HAL_SPI1 = 0,
	HAL_SPI2_CS1 = 1,
	HAL_SPI2_CS2 = 3,
	HAL_GPIO1 = 4,
	HAL_GPIO2 = 5,
	HAL_USB1 = 6,
	HAL_USB2 = 7,
}HAL_WRITE_ID;

/*******************************************************************************
* Function Name  : data_interface_hal_init.
* Description    :Hardware adaptation layer initialization.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void data_interface_hal_init(void);

/*******************************************************************************
* Function Name  : data_interface_hal_write.
* Description    : Hardware read and write adaptation layer.
* Input          : HAL_WRITE_ID.
* Output         : None.
* Return         : void (*)(uint8_t *p, uint8_t len).
*******************************************************************************/
inline void (*data_interface_hal_write(HAL_WRITE_ID id))(uint8_t *p, uint8_t len);

/*******************************************************************************
* Function Name  : data_interface_hal_read_proc.
* Description    : Hardware read adaptation layer.
* Input          : void (*callback_func)(uint8_t *, uint8_t).
* Output         : None.
* Return         : None.
*******************************************************************************/
void data_interface_hal_read_proc(void (*callback_func)(uint8_t *, uint8_t));

int get_connection_state(void);

inline uint64_t hal_read_TickCounter(void);
inline uint8_t hal_read_gpio_spi1_miso(void);

#endif
