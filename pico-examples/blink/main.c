#include "main.h"
#include "hardware/uart.h"
#include <stdio.h>
#include <stdarg.h>
#include "pico/multicore.h"
#include "hardware/sync/spin_lock.h"


/// \tag::hello_uart[]

#define UART_ID uart1
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5

#define ANGLE_DATA_AVG_NUM  4

spin_lock_t *spin_lock_g;
repeating_timer_t timer_g;
uint8_t count_500ms_g = 0;
repeating_timer_t led_show_timer_g;
uint8_t led_show_flag_g;
uint8_t key_last_value;
uint8_t wave_set = SINE_WAVE_SEL;
int16_t wave_fre=100;

int16_t angle_x_data[ANGLE_DATA_AVG_NUM];
uint16_t angle_x_index = 0;
int16_t angle_y_data[ANGLE_DATA_AVG_NUM];
uint16_t angle_y_index = 0;

bool led_blink(repeating_timer_t* timer)
{
    if(0 == (count_500ms_g % 2))
    {
        pico_set_led(true);
    }
    else
    {
        pico_set_led(false);
    }
    count_500ms_g++;
    return 1;
}

bool oled_show(repeating_timer_t* timer)
{
    led_show_flag_g = 1;
    return 1;
}

void uart_printf(char* fmt_str, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt_str);
    vsnprintf(buf, 256, fmt_str, ap);
    uint32_t int_save =  spin_lock_blocking(spin_lock_g);
    uart_puts(UART_ID, buf);
    spin_unlock(spin_lock_g, int_save);
    va_end(ap);
}
void board_key_process_for_wave(uint16_t key_value)
{
    if(key_last_value != key_value)
    {
        if((key_value>=SW0) && (key_value<=KEY3) && (key_value != SW3))
        {
            uart_printf("key=%x \r\n", key_value);
        }
        key_last_value = key_value;
        switch(key_value)
        {
            case KEY0:
            {
                pico_multi_led_control(LED3);
                wave_set = SAWTOOTH_WAVE_SEL;
            }
            break;
            case KEY1:
            {
                pico_multi_led_control(LED4);
                wave_set = DC_WAVE_SEL;
            }
            break;
            case KEY2:
            {
                pico_multi_led_control(LED5);
                if(wave_fre>1000)
                {
                    wave_fre+=1000;
                }
                else if((wave_fre>=100) && (wave_fre<=1000))
                {
                    wave_fre+=100;
                }
                else if((wave_fre>=10) && (wave_fre<100))
                {
                    wave_fre+=10;
                }
                else if((wave_fre>=1) && (wave_fre<10))
                {
                    wave_fre+=1;
                }
            }
            break;
            case KEY3:
            {
                pico_multi_led_control(LED6);
                if(wave_fre>1000)
                {
                    wave_fre-=1000;
                }
                else if((wave_fre>100) && (wave_fre<=1000))
                {
                    wave_fre-=100;
                }
                else if((wave_fre>10) && (wave_fre<=100))
                {
                    wave_fre-=10;
                }
                else if((wave_fre>1) && (wave_fre<=10))
                {
                    wave_fre-=1;
                }
                else
                {
                    wave_fre = 1;
                }
            }
            break;
            case SW0:
            {
                pico_multi_led_control(LED7);
                wave_set = SQUARE_WAVE_SEL;
            }
            break;
            case SW1:
            {
                pico_multi_led_control(LED8);
                wave_set = SINE_WAVE_SEL;
            }
            break;
            case SW2:
            {
                pico_multi_led_control(LED9);
                wave_set = TRIANGULAR_WAVE_SEL;
            }
            break;
            case SW3:
            {
                pico_multi_led_control(LED10);
            }
            break;
            default:
            break;
        }
    }
}

void wave_info_show()
{
    uint8_t buf[128];
    uint32_t ad_data = ad_read_data();
    sprintf(buf,"Vol:%.2f V", (float)ad_data*3.0/254);
    show_common_string(40, 4, buf, 16);
    switch(wave_set)
    {
        case SQUARE_WAVE_SEL:
        {
            uint16_t fre = 1000000/wave_fre/2;
            if(fre>1000)
                sprintf(buf,"Fre:%.1f kHz", (float)fre/1000.0);
            else
                sprintf(buf,"Fre:%04d Hz", fre);
            OLED_DrawBMP(0,0, 40, 64, square_wave);
        }break;
        case SINE_WAVE_SEL:
        {
            uint16_t fre = 1000000/wave_fre/256;
            sprintf(buf,"Fre:%04d Hz", fre);
            OLED_DrawBMP(0,0, 40, 64, sine_wave);
        }break;
        case TRIANGULAR_WAVE_SEL:
        {
            uint16_t fre = 1000000/wave_fre/64;
            sprintf(buf,"Fre:%04d Hz", fre);
            OLED_DrawBMP(0,0, 40, 64, triangular_wave);
        }break;
        case SAWTOOTH_WAVE_SEL:
        {
            uint16_t fre = 1000000/wave_fre/32;
            sprintf(buf,"Fre:%04d Hz", fre);
            OLED_DrawBMP(0,0, 40, 64, sawtooth_wave);
        }break;
        case DC_WAVE_SEL:
        {
            sprintf(buf,"Fre:%04d Hz", 0);
            OLED_DrawBMP(0,0, 40, 64, dc_wave);
        }break;
        default:
        break;
    }
    show_common_string(40, 1, buf, 16);
    multicore_fifo_push_blocking((wave_fre<<16) | (wave_set<<8) | (ad_data));
}

void  angle_data_process(int16_t* xdata, int16_t *ydata)
{
    static uint8_t angel_select = false;
    if(angel_select)
    {
        angel_select=false;
        int16_t temp_ydata=kxtj3_1057_read_data(0x8);
        angle_y_data[angle_y_index++] = temp_ydata;
        if(angle_y_index>=ANGLE_DATA_AVG_NUM)
        {
            angle_y_index=0;
        }
        int16_t curr_value =  angle_y_data[0];
        int16_t max_value = curr_value;
        int16_t min_value = curr_value;
        int16_t sum=0;
        uint8_t i;
        sum+=angle_y_data[0];
        for(i=1;i<(ANGLE_DATA_AVG_NUM-1);i++)
        {
        curr_value = angle_y_data[i];
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
        int16_t res = (sum/(ANGLE_DATA_AVG_NUM-2));
        
        *ydata = res;
    }
    else
    {
        angel_select=true;
        int16_t temp_xdata=kxtj3_1057_read_data(0x6);
        angle_x_data[angle_x_index++] = temp_xdata;
        if(angle_x_index>=ANGLE_DATA_AVG_NUM)
        {
            angle_x_index=0;
        }
        int16_t curr_value =  angle_x_data[0];
        int16_t max_value = curr_value;
        int16_t min_value = curr_value;
        int16_t sum=0;
        uint8_t i;
        sum+=angle_x_data[0];
        for(i=1;i<(ANGLE_DATA_AVG_NUM-1);i++)
        {
        curr_value = angle_x_data[i];
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
        int16_t res = (sum/(ANGLE_DATA_AVG_NUM-2));
        *xdata = res;
    }
    uart_printf("xdata=%d ydata=%d\r\n",*xdata, *ydata);
    // int16_t ydata=kxtj3_1057_read_data(0x8);
    // int16_t zdata=kxtj3_1057_read_data(0xA);
    // uart_printf("core0 ad test, ad_data=%d, id=%d,xdata=%d,ydata=%d,zdata=%d\r\n",ad_read_data(),id,xdata,ydata,zdata);
}

void angle_for_wave(int16_t xdata, int16_t ydata)
{
    static uint8_t last_xdir=0, last_ydir=0;
    static uint8_t curr_xdir = 0, curr_ydir=0;
    if(xdata>600)
    {
        curr_xdir=1;
    }
    else if(xdata<-600)
    {
        curr_xdir=2;
    }
    else if((xdata>-200) && (xdata<200))
    {
        curr_xdir=4;
    }

    if(last_xdir != curr_xdir)
    {
        last_xdir = curr_xdir;
        if(curr_xdir == 1)
        {
            wave_set++;
            if(wave_set>5)
            {
                wave_set=5;
            }
        }
        if(curr_xdir == 2)
        {
            wave_set--;
            if(wave_set<1)
            {
                wave_set=1;
            }
        }
    }

    if(ydata>600)
    {
        curr_ydir=1;
    }
    else if(ydata<-600)
    {
        curr_ydir=2;
    }
    else if((ydata>-200) && (ydata<200))
    {
        curr_ydir=4;
    }

    if(last_ydir != curr_ydir)
    {
        last_ydir = curr_ydir;
        if(curr_ydir == 1)
        {
            if(wave_fre>1000)
            {
                wave_fre+=1000;
            }
            else if((wave_fre>=100) && (wave_fre<=1000))
            {
                wave_fre+=100;
            }
            else if((wave_fre>=10) && (wave_fre<100))
            {
                wave_fre+=10;
            }
            else if((wave_fre>=1) && (wave_fre<10))
            {
                wave_fre+=1;
            }
        }
        if(curr_ydir == 2)
        {
            if(wave_fre>1000)
            {
                wave_fre-=1000;
            }
            else if((wave_fre>100) && (wave_fre<=1000))
            {
                wave_fre-=100;
            }
            else if((wave_fre>10) && (wave_fre<=100))
            {
                wave_fre-=10;
            }
            else if((wave_fre>1) && (wave_fre<=10))
            {
                wave_fre-=1;
            }
            else
            {
                wave_fre = 1;
            }
        }
    }
}

int main() {
    spin_lock_g = spin_lock_init(0);
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));
    //gpio_put(LED2, 1);
    //gpio_put(LED3, 1);
    oled_init();
    ad_init();
    board_adc_init();
    segment_io_init();
    //pwm_led_init();
    kxtj3_1057_sensor_init();
    uint8_t id = kxtj3_1057_read_id();
    if(id != 0x35)
    {
        uart_printf("angle sensor init failed, id=0x%x\r\n", id);
    }
    else
    {
        uart_printf("angle sensor init ok, id=0x%x\r\n", id);
    }
    multicore_launch_core1(core1_entry);
    // Wait for it to start up
    uint32_t flag = multicore_fifo_pop_blocking();
    if (flag != 123)
    {
        uart_printf("Hmm, that's not right on core 0!\n");
        return 0;
    }
    else {
        multicore_fifo_push_blocking(213);
        uart_printf("It's gone on core 0!\r\n");
    }
    if(!add_repeating_timer_ms(500, (repeating_timer_callback_t)led_blink, NULL, &timer_g))
    {
        uart_printf("add repeat timer failed\r\n");
        return 0;
    }
    if(!add_repeating_timer_ms(100, (repeating_timer_callback_t)oled_show, NULL, &led_show_timer_g))
    {
        uart_printf("add repeat timer failed\r\n");
        return 0;
    }
    const float conversion_factor = 3.3f / (1 << 12);
    uint8_t last_count_500ms = 0;
    int16_t xdata, ydata;
    while (true) {       
        uint16_t result = board_key_data();
        uint16_t key_value = board_key_check(result);
        board_key_process_for_wave(key_value);
        segment_show(wave_set);
        if(last_count_500ms != count_500ms_g)
        {
            last_count_500ms=count_500ms_g;
            uart_printf("Raw value: 0x%03x, voltage: %f \r\n", result, result * conversion_factor);
        }
        if(led_show_flag_g)
        {
            led_show_flag_g=0;
            wave_info_show();
            angle_data_process(&xdata, &ydata);
        }
        angle_for_wave(xdata, ydata);
    }
}
