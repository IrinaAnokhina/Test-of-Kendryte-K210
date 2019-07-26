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
#include "uarths.h"
#include "gpiohs.h"
#include "sysctl.h"

#define RECV_LENTH  1

#define CLOSLIGHT   0x55
#define OPENLIGHT   0xAA

int release_cmd(char *cmd)
{
    switch(*((int *)cmd)){
        case CLOSLIGHT:
        gpiohs_set_pin(13, GPIO_PV_LOW);
        break;
        case OPENLIGHT:
        gpiohs_set_pin(13, GPIO_PV_HIGH);
        break;
    }
    return 0;
}

void io_mux_init(void)
{

    fpioa_set_function(6, FUNC_UARTHS_RX);
    fpioa_set_function(7, FUNC_UARTHS_TX);
    fpioa_set_function(13, FUNC_GPIOHS13);
}

int main()
{
	//gpio_init();
    io_mux_init();
   // dmac_init();
    plic_init();
    sysctl_enable_irq();
    uarths_init();
    gpiohs_set_drive_mode(13, GPIO_DM_OUTPUT);
    gpio_pin_value_t value = GPIO_PV_HIGH;
    gpiohs_set_pin(13, value);

    uarths_config(115200, UARTHS_STOP_1);
    

    const unsigned char *hel = {"hello world!\r\n"};
    uarths_send_data(hel, strlen(hel));
  
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
    	while(uarths_receive_data(&recv, 1))
        {
    		uarths_send_data(recv, 1);
           /* switch(rec_flag)
            {
                case 0:
                recv == 0x55 ? (rec_flag = 1) : rec_flag;
                uarths_send_data(str0, strlen(str0));
                break;
                case 1:
                	 uarths_send_data(str1, strlen(str1));
                recv == 0xAA ? (rec_flag = 2) : (rec_flag = 0);
               // printf("case 1\r\n");
                break;
                case 2:
                	uarths_send_data(str2, strlen(str2));
                cmd[i++] = recv;
                if(i >= RECV_LENTH)
                {
                	uarths_send_data(str, strlen(str));
                    i = 0;
                    release_cmd(cmd);
                    rec_flag = 0;
                }
                break;
            }
            */
            	
           release_cmd(recv);
        }
        
    }
    while(1);
}

