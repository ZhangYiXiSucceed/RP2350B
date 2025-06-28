#include "main.h"

#define DAC_IO_PORT_MASK  0x3FF0000000
#define DAC_IO_DIR_MASK   0x3FF0000000
#define DAC_IO_LEVEL_HIGH 0x3FF0000000


#define DAC_PORT_NUM  10

#define DAC_PORT_START 28
#define DAC_PORT_END   37

void dac_io_init()
{
    uint8_t i;
    for(i=DAC_PORT_START;i<=DAC_PORT_END;i++)
    {
        gpio_init(i);
        gpio_set_dir(i, 1);
    } 
}



void dac_io_value_set(uint64_t value)
{
    uint8_t bit_value, bit_mask;
    uint8_t i;
    uint8_t dac_port = DAC_PORT_START;
    for(i=0;i<DAC_PORT_NUM;i++)
    {
        bit_value = (value) & 0x1;
        gpio_put(dac_port, bit_value);
        value >>=1;
        dac_port++;
    } 
}

