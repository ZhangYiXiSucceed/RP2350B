#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;

#define PIN_MISO 14
#define PIN_CS   16
#define PIN_SCK  15

#define ad_cs_clr() 	            gpio_put(PIN_CS, 0)
#define ad_cs_set()                 gpio_put(PIN_CS, 1)

#define ad_sdo_in()                 gpio_get(PIN_MISO)//DC


#define ad_sclk_clr()               gpio_put(PIN_SCK, 0)//CLK
#define ad_sclk_set()               gpio_put(PIN_SCK, 1)


u16 ad_read_data()
{	
    u16 data=0;
    ad_cs_clr();
    int i;
    for(i=0;i<11;i++)
    {			  
      ad_sclk_clr();
      data |= ad_sdo_in();
      ad_sclk_set();
      data<<=1;   
    }
    ad_cs_set();	
    return data;			 		   	
} 


void ad_init() {
 
    gpio_init(PIN_SCK);
    gpio_set_dir(PIN_SCK, GPIO_OUT);
    gpio_put(PIN_SCK, 1);
	
	gpio_init(PIN_MISO);
    gpio_set_dir(PIN_MISO, GPIO_IN);
    gpio_put(PIN_MISO, 1);

    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
}