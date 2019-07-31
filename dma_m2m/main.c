#include <stdio.h>
#include <stddef.h>
#include "stdlib.h"
#include <string.h>
#include "fpioa.h"
#include "uart.h"
#include "gpiohs.h"
#include "sysctl.h"
#include "io.h"
#include "platform.h"
#include "utils.h"

#define SIZE_BUF 128
#define UART_NUM    UART_DEVICE_1

uint8_t buf_src[SIZE_BUF] = {"Hello! I`m a test driver for dma_m2m function\r\n"};
uint8_t buf_dst[SIZE_BUF];

void send_dma_m2m(const uint8_t *buffer, size_t buf_len)
{
	 uint32_t *v_send_buf = malloc(buf_len * sizeof(uint32_t));
	    configASSERT(v_send_buf != NULL);
	    for(uint32_t i = 0; i < buf_len; i++)
	        v_send_buf[i] = buffer[i];
	   // sysctl_dma_select(SYSCTL_DMA_CHANNEL_0, SYSCTL_DMA_SELECT_UART1_TX_REQ + uart_channel * 2);
	    dmac_set_single_mode(SYSCTL_DMA_CHANNEL_0, v_send_buf, buf_dst, DMAC_ADDR_INCREMENT, DMAC_ADDR_INCREMENT,
	    		DMAC_MSIZE_1, DMAC_TRANS_WIDTH_8, buf_len);
	    dmac_wait_done(SYSCTL_DMA_CHANNEL_0);
	    free((void *)v_send_buf);
}

int main()
{
	printf("hello\r\n");
	dmac_init();
	plic_init();
	sysctl_enable_irq();
	    
	fpioa_set_function(6, FUNC_UART1_RX + UART_NUM * 2);
	fpioa_set_function(7, FUNC_UART1_TX + UART_NUM * 2);
	   
	 uart_init(UART_NUM);
	 uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);
	     
	    const unsigned char *hel = {"hello world!\r\n"};
	    uart_send_data(UART_NUM, hel, strlen(hel));
	     
	   /* for(uint8_t i = 0; i < SIZE_BUF; i++)
	    		 buf_src[i] = i + 1;*/
	    
	 send_dma_m2m(buf_src, SIZE_BUF);
	 
	 for(uint8_t i = 0; i < 10; i++)
	 {
		 uart_send_data(UART_NUM, &buf_dst[i], 1);
	 }
	 
	 while(1);
}

