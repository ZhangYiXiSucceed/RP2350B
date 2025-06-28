#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

#define ADC_AVG_NUM   20

void board_adc_init()
{
    adc_init();
    // Make sure GPIO is high-impedance, no pullups etc
    adc_gpio_init(47);
    // Select ADC input 7 (GPIO47)
    adc_select_input(7);
}

uint16_t board_key_data()
{
    uint16_t curr_value, max_value, min_value;
    uint32_t sum;
    uint8_t i;
    curr_value = adc_read();
    max_value = curr_value;
    min_value = curr_value;
    sum+=curr_value;
    for(i=0;i<(ADC_AVG_NUM-1);i++)
    {
       curr_value = adc_read();
       if(curr_value > max_value)
       {
           max_value = curr_value;
       }
       if(curr_value < min_value)
       {
           min_value = curr_value;
       }
       sum+=curr_value;
    }
    sum-=min_value;
    sum-=max_value;
    return (sum/(ADC_AVG_NUM-2));
}   