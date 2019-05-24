#include "usart_rx.h"
#include "systick.h"
#include "app_debug.h"
#include "kfifo.h"


static uint8_t usart_fifo_buffer[USART_RX_MAX_NUM];
static struct __kfifo uart_fifo;

void usart_rx_isr_init(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
    __kfifo_init(&uart_fifo, usart_fifo_buffer, USART_RX_MAX_NUM, 1);
    
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //开启串口接受中断
}



static uint8_t uart_rx_stat  = 0;

static uint8_t uart_rx_cbbuf_out[USART_RX_MAX_NUM];
static int uart_rx_cbbuf_out_counter = 0;

void usart_rx_proc( void (* usart_rx_callback)(uint8_t *, int) )
{
    uint8_t byte;
    while(  __kfifo_out(&uart_fifo, &byte, 1) == 1 ) {
        switch(uart_rx_stat) {
        case 0:
            if(byte == 0x82) {
                uart_rx_stat = 1;
                uart_rx_cbbuf_out_counter = 0;
                uart_rx_cbbuf_out[uart_rx_cbbuf_out_counter++] = byte;
            }
            break;
        case 1:
            uart_rx_cbbuf_out[uart_rx_cbbuf_out_counter++] = byte;
            if(byte == 0x83) {
                //timeout detect
                usart_rx_callback(uart_rx_cbbuf_out, uart_rx_cbbuf_out_counter);
                uart_rx_stat = 0;
            } else if(uart_rx_cbbuf_out_counter >= USART_RX_MAX_NUM) {
                uart_rx_stat = 0;
            }
            break;
        default:
            ;
        }
    }
}


void USART3_IRQHandler(void)                    //串口1中断服务程序
{
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
    {
        uint8_t byte = USART3->DR;
        __kfifo_in(&uart_fifo, &byte, 1);
        
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
    
}
