#include "ad8400_spi2.h"

#define SPI2_CHN_NUM  (2)

struct sGpio
{
	uint32_t RCC_APB2Periph;
	uint16_t GPIO_Pin;
	GPIO_TypeDef* GPIOx;
};

struct sGpio SPI2_GPIO[SPI2_CHN_NUM] = {
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


/**
  * @brief  使能SPI时钟
  * @retval None
  */
static void SPI_RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	
	for(int i=0;i<SPI2_CHN_NUM;i++) {
		RCC_APB2PeriphClockCmd(SPI2_GPIO[i].RCC_APB2Periph, ENABLE);
	}
}

/**
  * @brief  配置指定SPI的引脚
  * @retval None
  */
static void SPI_GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
	
	//SPI2_IO
    GPIO_InitStruct.GPIO_Pin =  GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_Init(GPIOB, &GPIO_InitStruct);
	
    //EN
	for(int i=0;i<SPI2_CHN_NUM;i++) {
		GPIO_InitStruct.GPIO_Pin =  SPI2_GPIO[i].GPIO_Pin;
		GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(SPI2_GPIO[i].GPIOx , &GPIO_InitStruct);
		GPIO_SetBits(SPI2_GPIO[i].GPIOx, SPI2_GPIO[i].GPIO_Pin );
	}
}

void ad8400_spi2_init(void)
{
    SPI_InitTypeDef SPI_InitStruct;
 
    SPI_RCC_Configuration();
 
    SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
    SPI_InitStruct.SPI_Direction= SPI_Direction_2Lines_FullDuplex;
    SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
    SPI_InitStruct.SPI_DataSize = SPI_DataSize_16b;
    SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStruct.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStruct);
    SPI_SSOutputCmd(SPI2, ENABLE);
    SPI_Cmd(SPI2, ENABLE);
	
	SPI_GPIO_Configuration();
}

void ad8400_spi2_set(uint8_t id, uint16_t val)
{
	GPIO_ResetBits(SPI2_GPIO[id].GPIOx, SPI2_GPIO[id].GPIO_Pin );
	
	SPI_I2S_SendData(SPI2, val);
    while(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
	SPI_I2S_ReceiveData(SPI2);
	
	GPIO_SetBits(SPI2_GPIO[id].GPIOx, SPI2_GPIO[id].GPIO_Pin );
}




