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
#include <time.h>
#include <unistd.h>
#include "rtc.h"


void get_date_time(bool alarm)
{
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    rtc_timer_get(&year, &month, &day, &hour, &minute, &second);
    if (!alarm)
        printf("%4d-%02d-%02d %02d:%02d:%02d\r\n", year, month, day, hour, minute, second);
    else
        printf("Alarm at --> %4d-%02d-%02d %02d:%02d:%02d\r\n", year, month, day, hour, minute, second);
}

int on_timer_interrupt(void *ctx)
{
    get_date_time(false);
    if(rtc_tick_get_interrupt_mode() == RTC_INT_SECOND)
    	printf(" rtc mode is RTC_INT_SECOND\r\n");
    else if(rtc_tick_get_interrupt_mode() == RTC_INT_MINUTE)
    {
    	printf("  rtc mode is RTC_INT_MINUTE\r\n");
    }
	return 0;
}

int on_alarm_interrupt(void *ctx)
{
    get_date_time(true);
   //rtc_alarm_set_mask((rtc_mask_t) {
     //       .second = 0, /* Second mask */
      //      .minute = 0, /* Minute mask */
      //      .hour = 1,   /* Hour mask */
      //      .week = 0,   /* Week mask */
       //     .day = 0,    /* Day mask */
      //      .month = 0,  /* Month mask */
      //      .year = 0,   /* Year mask */
     //   });
    //rtc_tick_set_interrupt_mode(RTC_INT_MINUTE);
    //rtc_alarm_set_interrupt(0);
    if(rtc_tick_get_interrupt())
    	printf(" rtc_tick interrupt enable\r\n");
    else
    {
    	printf(" rtc tick interrupt disable\r\n");
    }
    rtc_tick_set_interrupt(0);
    
	return 0;
}

int main(void)
{
    rtc_init();
    rtc_timer_set(2018, 9, 12, 22, 55, 50);
    rtc_alarm_set(2018, 9, 12, 22, 56, 00);

    printf("RTC Tick and Alarm Test\r\n" "Compiled in " __DATE__ " " __TIME__ "\r\n");

    rtc_tick_irq_register(
        false,
		RTC_INT_SECOND,
        on_timer_interrupt,
        NULL,
        1
    );

    rtc_alarm_irq_register(
        false,
        (rtc_mask_t) {
            .second = 0, /* Second mask */
            .minute = 1, /* Minute mask */
            .hour = 0,   /* Hour mask */
            .week = 0,   /* Week mask */
            .day = 0,    /* Day mask */
            .month = 0,  /* Month mask */
            .year = 0,   /* Year mask */
        },
        on_alarm_interrupt,
        NULL,
        1
    );

    while(1)
    {
        sleep(1);
        // get_date_time();
    }
    return 0;
}
