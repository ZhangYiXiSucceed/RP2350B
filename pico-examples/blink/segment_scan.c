#include "main.h"


#define SER_IO    0 
#define RCLK_IO   1
#define SCLK_IO   2

const uint8_t segment[16] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};
    // seg[0]	=	7'h3f;   // 0
    // seg[1]	=	7'h06;   // 1
    // seg[2]	=	7'h5b;   // 2
    // seg[3]	=	7'h4f;   // 3
    // seg[4]	=	7'h66;   // 4
    // seg[5]	=	7'h6d;   // 5
    // seg[6]	=	7'h7d;   // 6
    // seg[7]	=	7'h07;   // 7
    // seg[8]	=	7'h7f;   // 8
    // seg[9]	=	7'h6f;   // 9
	// seg[10]	=	7'h77;   // A
    // seg[11]	=	7'h7c;   // b
    // seg[12]	=	7'h39;   // C
    // seg[13]	=	7'h5e;   // d
    // seg[14]	=	7'h79;   // E
    // seg[15]	=	7'h71;   // F


void segment_io_init()
{
    gpio_init(SER_IO);
    gpio_set_dir(SER_IO, GPIO_OUT);

    gpio_init(RCLK_IO);
    gpio_set_dir(RCLK_IO, GPIO_OUT);

    gpio_init(SCLK_IO);
    gpio_set_dir(SCLK_IO, GPIO_OUT);
}


void segment_show(uint16_t value)
{
    static bool show_one_flag = 1;
    if(show_one_flag)
    {
        show_one_flag=0;
        uint16_t show_one_value = (0xFE<<8|segment[((value/10)%10)]);

        uint8_t i;
        gpio_put(SCLK_IO, 0);
        gpio_put(RCLK_IO, 0);
        for(i=0;i<16;i++)
        {
            if(show_one_value&0x8000)
            {
                gpio_put(SER_IO, 1);
            }
            else
            {
                gpio_put(SER_IO, 0);
            }
            show_one_value<<=1;
            gpio_put(SCLK_IO, 1);
            sleep_us(1);
            gpio_put(SCLK_IO, 0);
        }
        gpio_put(RCLK_IO, 1);
        sleep_us(1);
        gpio_put(RCLK_IO, 0);

    }
    else
    {
        show_one_flag=1;
        uint16_t show_two_value = (0xFD<<8|segment[value%10]);

        uint8_t i;
        gpio_put(SCLK_IO, 0);
        gpio_put(RCLK_IO, 0);
        for(i=0;i<16;i++)
        {
            if(show_two_value&0x8000)
            {
                gpio_put(SER_IO, 1);
            }
            else
            {
                gpio_put(SER_IO, 0);
            }
            show_two_value<<=1;
            gpio_put(SCLK_IO, 1);
            sleep_us(1);
            gpio_put(SCLK_IO, 0);
        }
        gpio_put(RCLK_IO, 1);
        sleep_us(1);
        gpio_put(RCLK_IO, 0);
    }
}