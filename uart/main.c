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

#define RECV_LENTH  1

#define CLOSLIGHT   0x55
#define OPENLIGHT   0xAA

#define UART_NUM    UART_DEVICE_1

int release_cmd(char *cmd)
{
    switch(*((int *)cmd)){
        case CLOSLIGHT:
        gpiohs_set_pin(24, GPIO_PV_LOW);
        break;
        case OPENLIGHT:
        gpiohs_set_pin(24, GPIO_PV_HIGH);
        break;
    }
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
	printf("hello\r\n");
	//gpio_init();
    io_mux_init();
    dmac_init();
    plic_init();
    sysctl_enable_irq();

    gpiohs_set_drive_mode(24, GPIO_DM_OUTPUT);
    gpio_pin_value_t value = GPIO_PV_HIGH;
    gpiohs_set_pin(24, value);

    uart_init(UART_NUM);
    uart_configure(UART_NUM, 115200, 8, UART_STOP_1, UART_PARITY_NONE);
    printf("hello\r\n");
    const unsigned char *hel = {"hello world!\r\n"};
   // uart_send_data(UART_NUM, hel, strlen(hel));
    uart_send_data_dma(UART_NUM, DMAC_CHANNEL0, hel, strlen(hel));
    uart_send_data_dma(UART_NUM, DMAC_CHANNEL0, hel, strlen(hel));
  
    char *str = {"i >= RECV_LENTH\r\n"};
    char *str0 = {"case 0\r\n"};
    char *str1 = {"case 1\r\n"};
    char *str2 = {"case 2\r\n"};
    char *str3 = {"uart didn`t receive\r\n"};
    char recv = 0;
    int rec_flag = 0;
    char cmd[8];
    int i = 0;
    while (1)
    {
    	//while(uart_receive_data(UART_NUM, &recv, 1))
    	//{
    	uart_receive_data_dma(UART_NUM, DMAC_CHANNEL1, &recv, 1);
    	if(recv == 0)
    		uart_send_data(UART_NUM, str3, strlen(str3));
        while(recv != 0)
        {        
        	 //uart_send_data(UART_NUM, recv, strlen(recv));
    	//uart_send_data(UART_NUM, hel, strlen(hel));
        	if(recv == 0x55)
        		 gpiohs_set_pin(24, GPIO_PV_LOW);
        	else if(recv == 0x12)
        		 gpiohs_set_pin(24, GPIO_PV_HIGH);
        }
        
    }
    while(1);
}

