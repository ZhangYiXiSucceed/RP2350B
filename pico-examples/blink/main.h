#ifndef __MAIN_H__
#define __MAIN_H__
#include "pico/stdlib.h"
#include <stdio.h>
#define SW0      (1<<0)
#define SW1      (1<<1)
#define SW2      (1<<2)

#define SW3      (1<<3)

#define KEY0    (1<<4)  
#define KEY1    (1<<5)
#define KEY2    (1<<6)
#define KEY3    (1<<7)

#define LED3  1
#define LED4  2
#define LED5  3
#define LED6  4
#define LED7  5
#define LED8  6
#define LED9  7
#define LED10 8

#define LED_SINGLE    3

#define SINE_WAVE_SEL          1
#define TRIANGULAR_WAVE_SEL    2
#define SAWTOOTH_WAVE_SEL      3
#define SQUARE_WAVE_SEL        4
#define DC_WAVE_SEL            5

extern unsigned char dc_wave[];
extern unsigned char sine_wave[];
extern unsigned char triangular_wave[];
extern unsigned char sawtooth_wave[];
extern unsigned char square_wave[];

void board_adc_init();
uint16_t board_key_data();
uint16_t ad_read_data();
void show_common_string(uint8_t x,uint8_t y,uint8_t *chr,int font_size);
void core1_entry();
void oled_init();
void ad_init();
uint16_t board_key_check(uint16_t adc_data);
int pico_led_init(void);
void pico_set_led(bool led_on);
void pico_multi_led_control(uint8_t led_num);
void pwm_led_init();
void OLED_Clear(void);
void segment_io_init();
void segment_show(uint16_t value);
uint8_t kxtj3_1057_read_id();
void kxtj3_1057_sensor_init();
int16_t kxtj3_1057_read_data(uint8_t  reg_addr);
void dac_io_init();
void dac_io_value_set(uint64_t value);
void wave_show_select(unsigned char wave_value);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
#endif