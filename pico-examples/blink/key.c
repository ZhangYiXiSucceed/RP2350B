#include "main.h"

#define KEY_NUM  8




#define SW0_ADC_MAX  0xE2A
#define SW0_ADC_MIN  0xE1C

#define SW1_ADC_MAX  0xDEF
#define SW1_ADC_MIN  0xDE0

#define SW2_ADC_MAX  0xDD2
#define SW2_ADC_MIN  0xDC0

#define SW3_ADC_MAX  0xDC2
#define SW3_ADC_MIN  0xDBA

#define KEY0_ADC_MAX  0xCDF
#define KEY0_ADC_MIN  0xCC0

#define KEY1_ADC_MAX  0xBFF
#define KEY1_ADC_MIN  0xBE0

#define KEY2_ADC_MAX  0xA40
#define KEY2_ADC_MIN  0xA20

#define KEY3_ADC_MAX  0x6B0
#define KEY3_ADC_MIN  0x690

typedef struct key_value_range_struct
{
    uint16_t max_value;
    uint16_t min_value;
}key_value_range_t;

key_value_range_t key_value_range_g[KEY_NUM] =
{
    {SW0_ADC_MAX, SW0_ADC_MIN},
    {SW1_ADC_MAX, SW1_ADC_MIN},
    {SW2_ADC_MAX, SW2_ADC_MIN},
    {SW3_ADC_MAX, SW3_ADC_MIN},

    {KEY0_ADC_MAX, KEY0_ADC_MIN},
    {KEY1_ADC_MAX, KEY1_ADC_MIN},
    {KEY2_ADC_MAX, KEY2_ADC_MIN},
    {KEY3_ADC_MAX, KEY3_ADC_MIN},
};


uint16_t board_key_check(uint16_t adc_data)
{
    uint8_t i;
    sleep_ms(5);
    for(i=0;i<KEY_NUM;i++)
    {
        if((adc_data>=key_value_range_g[i].min_value) && (adc_data<=key_value_range_g[i].max_value))
        {
            break;
        }
    }
    return (1<<i);
}