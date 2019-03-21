/* 
 *
 *2018 09 21 & hxdyxd
 *
 */

#include "stm32_spi.h"

#define STM32_SPI_NUM  (2)
#define SPI1_CS_NUM    (0)
#define SPI2_CS_NUM    (2)

struct sGpio
{
	uint32_t RCC_APB2Periph;
	uint16_t GPIO_Pin;
	GPIO_TypeDef* GPIOx;
};

struct sSpi
{
	struct sGpio GPIO;
	uint32_t RCC_APB2Periph;
	SPI_TypeDef* SPIx;
	uint16_t SPI_BaudRatePrescaler;
	uint16_t SPI_DataSize;
	uint16_t SPI_CPOL;
	uint16_t SPI_CPHA; 
	
};

const struct sSpi STM32_SPI[STM32_SPI_NUM] = 
{
	{
		.GPIO = {
			.RCC_APB2Periph = RCC_APB2Periph_GPIOA,
			.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7,
			.GPIOx = GPIOA,
		},
		.RCC_APB2Periph = RCC_APB2Periph_SPI1,
		.SPIx = SPI1,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32,
		.SPI_DataSize = SPI_DataSize_8b,
		.SPI_CPOL = SPI_CPOL_High,
		.SPI_CPHA = SPI_CPHA_2Edge,
	},
	{
		.GPIO = {
			.RCC_APB2Periph = RCC_APB2Periph_GPIOB,
			.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
			.GPIOx = GPIOB,
		},
		.RCC_APB2Periph = RCC_APB1Periph_SPI2,
		.SPIx = SPI2,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128,
		.SPI_DataSize = SPI_DataSize_16b,
		.SPI_CPOL = SPI_CPOL_Low,
		.SPI_CPHA = SPI_CPHA_1Edge,
	},
};

struct sGpio SPI2_CS[SPI2_CS_NUM] = {
	{
		.RCC_APB2Periph = RCC_APB2Periph_GPIOB,
		.GPIOx = GPIOB,
		.GPIO_Pin = GPIO_Pin_8,
	},
	
	{
		.RCC_APB2Periph = RCC_APB2Periph_GPIOB,
		.GPIOx = GPIOB,
		.GPIO_Pin = GPIO_Pin_9,
	},
};


void stm32_spi_init(void)
{
	/* SPI配置 */
	for(int i=0;i<STM32_SPI_NUM;i++) {
		if(STM32_SPI[i].RCC_APB2Periph == RCC_APB1Periph_SPI2) {
			RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2  , ENABLE);
		} else if(STM32_SPI[i].RCC_APB2Periph == RCC_APB2Periph_SPI1) {
			RCC_APB2PeriphClockCmd( RCC_APB2Periph_SPI1  , ENABLE);
		}
		
		RCC_APB2PeriphClockCmd( STM32_SPI[i].GPIO.RCC_APB2Periph  , ENABLE);
		
		SPI_InitTypeDef SPI_InitStruct;
		SPI_InitStruct.SPI_BaudRatePrescaler = STM32_SPI[i].SPI_BaudRatePrescaler ;
		SPI_InitStruct.SPI_DataSize = STM32_SPI[i].SPI_DataSize;
		SPI_InitStruct.SPI_CPOL = STM32_SPI[i].SPI_CPOL;
		SPI_InitStruct.SPI_CPHA = STM32_SPI[i].SPI_CPHA;
		
		/* 共有参数 */
		SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStruct.SPI_CRCPolynomial = 7;
		SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
		SPI_InitStruct.SPI_Mode = SPI_Mode_Master;

		SPI_Init(STM32_SPI[i].SPIx , &SPI_InitStruct);
		SPI_SSOutputCmd(STM32_SPI[i].SPIx , ENABLE);
		SPI_Cmd(STM32_SPI[i].SPIx , ENABLE);
		
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin =  STM32_SPI[i].GPIO.GPIO_Pin;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(STM32_SPI[i].GPIO.GPIOx , &GPIO_InitStruct);
	}
	/* SPI2片选配置 */
	for(int i=0;i<SPI2_CS_NUM;i++) {
		RCC_APB2PeriphClockCmd(SPI2_CS[i].RCC_APB2Periph, ENABLE);
		
		GPIO_InitTypeDef GPIO_InitStruct;
		GPIO_InitStruct.GPIO_Pin =  SPI2_CS[i].GPIO_Pin ;
		GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; 
		GPIO_Init(SPI2_CS[i].GPIOx , &GPIO_InitStruct);
	}
}

void delay_us(int i)
{
	i *= 72;
	while(--i);
}

uint16_t stm32_spi_wr(STM32_SPI_ID id, uint16_t val)
{
	uint16_t rx_data = 0;
	switch(id) {
	case STM32_SPI1:
		SPI_I2S_SendData(SPI1, val);
		while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		rx_data = SPI_I2S_ReceiveData(SPI1);
		break;
	
	case STM32_SPI2_CS1:
		GPIO_ResetBits(SPI2_CS[0].GPIOx, SPI2_CS[0].GPIO_Pin );
		SPI_I2S_SendData(SPI2, val);
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
		rx_data = SPI_I2S_ReceiveData(SPI2);
		GPIO_SetBits(SPI2_CS[0].GPIOx, SPI2_CS[0].GPIO_Pin );
		break;
	
	case STM32_SPI2_CS2:
		GPIO_ResetBits(SPI2_CS[1].GPIOx, SPI2_CS[1].GPIO_Pin );
		SPI_I2S_SendData(SPI2, val);
		while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
		rx_data = SPI_I2S_ReceiveData(SPI2);
		GPIO_SetBits(SPI2_CS[1].GPIOx, SPI2_CS[1].GPIO_Pin );
		break;
	
	default:
		rx_data = 0;
		break;
	}
	return rx_data;
}
