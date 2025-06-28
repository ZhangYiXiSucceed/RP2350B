#include "stdio.h"
#include "pico/multicore.h"
#include "hardware/uart.h"
#include "hardware/sync/spin_lock.h"
#include "main.h"
spin_lock_t *spin_lock_core1_g;
uint32_t ad_data_g = 0x3FF;
uint32_t key_value;
uint32_t last_value = 0xFF;
uint32_t fre = 100;
uint32_t wave_select = 0xFF;

uint16_t sine_wave_data[] = 
{
    0x0,   0xC,   0x19,  0x25,  0x32,  0x3E,  0x4B,  0x57,  
    0x63,  0x70,  0x7C,  0x88,  0x94,  0xA0,  0xAC,  0xB8,  
    0xC3,  0xCF,  0xDA,  0XE6,  0xF1,  0XFC,  0x107, 0x111, 
    0x11C, 0x126, 0x130, 0x13A, 0x144, 0x14E, 0x157, 0x161,   
    0x16A, 0x172, 0x17B, 0x183, 0x18B, 0x193, 0x19B, 0x1A2,
    0x1A9, 0x1B0, 0x1B7, 0x1BD, 0x1C3, 0x1C9, 0x1CE, 0x1D4,
    0x1D9, 0x1DD, 0x1E2, 0x1E6, 0x1E9, 0x1ED, 0x1F0, 0x1F3,   
    0x1F6, 0x1F8, 0x1FA, 0x1FC, 0x1FD, 0x1FE, 0x1FF, 0x1FF,
}; 
uint16_t triangular_wave_data[] = 
{
    0x0,    0x20,  0x40,  0x60,  0x80,  0xA0,  0xC0,  0xE0,
};
uint16_t sawtooth_wave_data[] = {0,128,256,374,512,640,768,896,1024};


void core1_fifo_irq() {
    // Just record the latest entry
    uint32_t value;
    while (multicore_fifo_rvalid())   
    {
        value = multicore_fifo_pop_blocking();
        ad_data_g = ((value & 0xFF) << 2);
        wave_select = (value >> 8) & 0xFF;
        fre  = (value>>16);
    }
    multicore_fifo_clear_irq();
}

void generate_wave()
{
    switch(wave_select)
    {
        case SINE_WAVE_SEL:
        {
            int8_t i;
            for(i=0;i<64;i++)
            {
                dac_io_value_set((0x1FF + sine_wave_data[i])*ad_data_g/1024);
                sleep_us(fre);
            }
            for(i=63;i>=0;i--)
            {
                dac_io_value_set((0x1FF + sine_wave_data[i])*ad_data_g/1024);
                sleep_us(fre);
            }
            for(i=0;i<64;i++)
            {
                dac_io_value_set((0x1FF - sine_wave_data[i])*ad_data_g/1024);
                sleep_us(fre);
            }
            for(i=63;i>=0;i--)
            {
                dac_io_value_set((0x1FF - sine_wave_data[i])*ad_data_g/1024);
                sleep_us(fre);
            }
        }break;
        case SQUARE_WAVE_SEL:
        {
            dac_io_value_set(ad_data_g);
            sleep_us(fre);
            dac_io_value_set(0);
            sleep_us(fre);
        }break;
        case TRIANGULAR_WAVE_SEL:
        {
            int8_t i;
            uint16_t value = 0;
            uint16_t avg = ad_data_g>>5;
            for(i=0;i<32;i++)
            {
                dac_io_value_set(value);
                sleep_us(fre);
                value+=avg;
            }
            for(i=0;i<32;i++)
            {
                value-=avg;
                dac_io_value_set(value);
                sleep_us(fre);
            }
        }break;
        case SAWTOOTH_WAVE_SEL:
        {
            int8_t i;
            uint16_t value = 0;
            uint16_t avg = ad_data_g>>5;
            for(i=0;i<32;i++)
            {
                dac_io_value_set(value);
                sleep_us(fre);
                value+=avg;
            }
        }break;
        case DC_WAVE_SEL:
        {
            dac_io_value_set(ad_data_g);
        }break;
        default:
            dac_io_value_set(ad_data_g);
            sleep_us(fre);
            dac_io_value_set(0);
            sleep_us(fre);
        break;
    }
}

void uart_core1_printf(char* fmt_str, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt_str);
    vsnprintf(buf, 256, fmt_str, ap);
    uint32_t int_save =  spin_lock_blocking(spin_lock_core1_g);
    uart_puts(uart1, buf);
    spin_unlock(spin_lock_core1_g, int_save);
    va_end(ap);
}


void core1_entry() {

    multicore_fifo_clear_irq();
    irq_set_exclusive_handler(SIO_FIFO_IRQ_NUM(1), core1_fifo_irq);
    irq_set_enabled(SIO_FIFO_IRQ_NUM(1), true);
    multicore_fifo_push_blocking(123);
    spin_lock_core1_g = spin_lock_instance(0);
    while (ad_data_g != (213<<2))
    {
        uart_core1_printf("Hmm, that's not right on core 1!\r\n");;
    }
    
    {
        uart_core1_printf("Its gone  on core 1!\r\n");
    }
    char buf1[128];
    dac_io_init();
    uart_core1_printf("dac io init!\r\n");
    while (1)
    {
        generate_wave();
        // int32_t int_save =  spin_lock_blocking(spin_lock_core1_g);
        // sprintf(buf1,"core1 bit_shit=%d, key_value=%d\r\n", bit_shit, key_value);
        // uart_puts(uart1, buf1);
        // spin_unlock(spin_lock_core1_g, int_save);
        // sleep_ms(1000);
    }
}