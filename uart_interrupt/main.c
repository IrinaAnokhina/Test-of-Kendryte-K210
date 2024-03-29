/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdio.h>
#include "fpioa.h"
#include <string.h>
#include "uart.h"
#include "gpiohs.h"
#include "sysctl.h"
#include "dmac.h"

#define CMD_LENTH  4

#define CLOSLIGHT   0x55555555
#define OPENLIGHT   0xAAAAAAAA

#define UART_NUM    UART_DEVICE_1

uint8_t in_buf[0];

int release_cmd(char *cmd)
{
    switch(*((int *)cmd))
    {
        case CLOSLIGHT:
        gpiohs_set_pin(24, GPIO_PV_LOW);
        break;
        case OPENLIGHT:
        gpiohs_set_pin(24, GPIO_PV_HIGH);
        break;
    }
    return 0;
}

int on_uart_send(void *ctx)
{
    uint8_t v_uart = *((uint32_t *)ctx) + 1 + 0x30;
    uart_irq_unregister(UART_NUM, UART_SEND);
    char *v_send_ok = "Send ok Uart";
    uart_send_data(UART_NUM, v_send_ok,strlen(v_send_ok));
    uart_send_data(UART_NUM, (char *)&v_uart,1);
    return 0;
}

volatile uint32_t recv_flag = 0;
char g_cmd[4];
volatile uint8_t g_cmd_cnt = 0;
int on_uart_recv(void *ctx)
{
    //char in_buf[1];
    int ret =  uart_receive_data(UART_NUM, in_buf, 1);
   
    if(in_buf[0] == 0x55)
    	gpiohs_set_pin(24, GPIO_PV_LOW);
    else if(in_buf[0] == 0x12)
    	gpiohs_set_pin(24, GPIO_PV_HIGH);
    return 0;
}

void io_mux_init(void)
{
    fpioa_set_function(6, FUNC_UART1_RX + UART_NUM * 2);
    fpioa_set_function(7, FUNC_UART1_TX + UART_NUM * 2);
    fpioa_set_function(24, FUNC_GPIOHS24);
}

int main()
{
    io_mux_init();
    plic_init();
    sysctl_enable_irq();
    dmac_init();
    
    gpiohs_set_drive_mode(24, GPIO_DM_OUTPUT);
    gpio_pin_value_t value = GPIO_PV_HIGH;
    gpiohs_set_pin(24, value);

    uart_init(UART_NUM);
    uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);

    uart_set_receive_trigger(UART_NUM, UART_RECEIVE_FIFO_8);
    uart_irq_register(UART_NUM, UART_RECEIVE, on_uart_recv, NULL, 2);

    uart_set_send_trigger(UART_NUM, UART_SEND_FIFO_0);
    uint32_t v_uart_num = UART_NUM;
    uart_irq_register(UART_NUM, UART_SEND, on_uart_send, &v_uart_num, 2);

    char *hel = {"hello world!\n"};
   // uart_send_data(UART_NUM, hel, strlen(hel));
   uart_send_data_dma_irq(UART_NUM, DMAC_CHANNEL0, hel, strlen(hel), on_uart_send, &v_uart_num, 2);
   //uart_receive_data_dma_irq(UART_NUM, DMAC_CHANNEL1, in_buf, strlen(in_buf), on_uart_recv, &v_uart_num, 2);
    while(1)
    	{
    	//uart_receive_data_dma(UART_NUM, DMAC_CHANNEL1, in_buf, 1);
    	  uart_receive_data_dma_irq(UART_NUM, DMAC_CHANNEL1, in_buf, strlen(in_buf), on_uart_recv, NULL, 3);
    	/*if(in_buf[0] == 0x55)
    	    	gpiohs_set_pin(24, GPIO_PV_LOW);
    	    else if(in_buf[0] == 0x12)
    	    	gpiohs_set_pin(24, GPIO_PV_HIGH);*/
    	}
}
