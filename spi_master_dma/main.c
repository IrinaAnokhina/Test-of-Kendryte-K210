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
#include "bsp.h"
#include "sysctl.h"
#include "plic.h"
#include "uarths.h"
#include "spi.h"
#include "spi_slave.h"
#include "spi_master.h"
#include "board_config.h"
#define MASTER_MODE
#define PLL0_OUTPUT_FREQ 800000000UL
static void io_set_power(void)
{
    sysctl_set_power_mode(SYSCTL_POWER_BANK0, POWER_BANK0_SELECT);
    sysctl_set_power_mode(SYSCTL_POWER_BANK1, POWER_BANK1_SELECT);
    sysctl_set_power_mode(SYSCTL_POWER_BANK2, POWER_BANK2_SELECT);
    sysctl_set_power_mode(SYSCTL_POWER_BANK3, POWER_BANK3_SELECT);
    sysctl_set_power_mode(SYSCTL_POWER_BANK4, POWER_BANK4_SELECT);
    sysctl_set_power_mode(SYSCTL_POWER_BANK5, POWER_BANK5_SELECT);
    sysctl_set_power_mode(SYSCTL_POWER_BANK6, POWER_BANK6_SELECT);
    sysctl_set_power_mode(SYSCTL_POWER_BANK7, POWER_BANK7_SELECT);
}
#ifdef MASTER_MODE
static uint8_t test_data[1024];
#else
static uint8_t slave_cfg[32];
static uint8_t test_data_tmp[1024];
uint32_t buf[128];
#endif
int main(void)
{
	 printf(" in main\r\n");
    sysctl_pll_set_freq(SYSCTL_PLL0, PLL0_OUTPUT_FREQ);
    dmac_init();
    uarths_init();
    io_set_power();
    plic_init();
#ifdef MASTER_MODE
    printf("spi master init\r\n");
    spi_master_init();
#else
    printf("spi slave init\r\n");
    *(uint32_t *)(&slave_cfg[8]) = (uint32_t)test_data_tmp;
    spi_slave_init(slave_cfg, 32);
#endif
    /* enable global interrupt */
    sysctl_enable_irq();
    /* system start */
    printf("System start\r\n");
#ifdef MASTER_MODE
    for (uint32_t i = 0; i < 7; i++)
            test_data[i] = i + 1;
    spi_send_data(test_data, 8);
    send_dma(test_data, 1);
           receive_dma(test_data, 1);

           for (uint32_t i = 0; i < 7; i++)
                       test_data[i] = i + 1;
               spi_send_data(test_data, 8);
              
           printf("test finish\r\n");
       #endif
           while (1)
               ;
       }