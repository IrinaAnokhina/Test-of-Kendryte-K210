#include <unistd.h>
#include "fpioa.h"
#include "plic.h"
#include "sysctl.h"
#include "bsp.h"
#include "i2c_slave.h"
#include "i2c.h"

void io_mux_init(void)
{
    fpioa_set_function(10, FUNC_GPIOHS10);
    fpioa_set_function(11, FUNC_GPIOHS11);
    fpioa_set_function(30, FUNC_I2C0_SCLK);
    fpioa_set_function(31, FUNC_I2C0_SDA);
}

int main(void)
{
	 dmac_init();
	 
    int result = 0;
   // uint8_t index, data_buf[SLAVE_MAX_ADDR];
    printf(" \n test i2c slave mode \r\n");

    io_mux_init();
    plic_init();
    sysctl_enable_irq();
    i2c_slave_init();
    uint8_t data_buf[16] = {0x08, 0xab, 0xec, 0x05};
   
   // i2c_slave_transmit();
   // i2c_slave_receive(1);
    //i2c_master_init();
  // i2c_write_reg(0xE0, &data_buf[0], 1);
   
    //i2c_write_reg(0, &data_buf[0], 4);
   // i2c_write_reg(7, &data_buf[7], 7);

   
   //i2c_read_reg(0, &data_buf[0], 4);
   
    while(1)
    {
    	//i2c_slave_receive(50);
    }
       // ;
    return 0;
}
