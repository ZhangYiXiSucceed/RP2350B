/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>
#include "pico/multicore.h"
#include "hardware/sync/spin_lock.h"
#include "main.h"

// Pico W devices use a GPIO on the WIFI chip for the LED,
// so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

#define LED_IO1   27
#define LED_IO2   26
#define LED_IO3   25
#define LED_IO4   24




// Perform initialisation
int pico_led_init(void) {

    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(LED_IO1);
    gpio_set_dir(LED_IO1, GPIO_OUT);
    gpio_init(LED_IO2);
    gpio_set_dir(LED_IO2, GPIO_OUT);
    gpio_init(LED_IO3);
    gpio_set_dir(LED_IO3, GPIO_OUT);
    gpio_init(LED_IO4);
    gpio_set_dir(LED_IO4, GPIO_OUT);

    gpio_init(LED_SINGLE);
    gpio_set_dir(LED_SINGLE, GPIO_OUT);
    return PICO_OK;
}

// Turn the led on or off
void pico_set_led(bool led_on) {
    gpio_put(LED_SINGLE, led_on);
}


void pico_multi_led_control(uint8_t led_num)
{
    switch(led_num)
    {
        case LED3:
        {
            gpio_set_dir(LED_IO1, GPIO_OUT);
            gpio_set_dir(LED_IO2, GPIO_OUT);
            gpio_set_dir(LED_IO3, GPIO_IN);
            gpio_set_dir(LED_IO4, GPIO_IN);
            gpio_put(LED_IO1, 1);
            gpio_put(LED_IO2, 0);
        }break;
        case LED4:
        {
            gpio_set_dir(LED_IO1, GPIO_IN);
            gpio_set_dir(LED_IO2, GPIO_OUT);
            gpio_set_dir(LED_IO3, GPIO_OUT);
            gpio_set_dir(LED_IO4, GPIO_IN);
            gpio_put(LED_IO2, 1);
            gpio_put(LED_IO3, 0);
        }break;
        case LED5:
        {
            gpio_set_dir(LED_IO1, GPIO_IN);
            gpio_set_dir(LED_IO2, GPIO_IN);
            gpio_set_dir(LED_IO3, GPIO_OUT);
            gpio_set_dir(LED_IO4, GPIO_OUT);
            gpio_put(LED_IO3, 1);
            gpio_put(LED_IO4, 0);
        }break;
        case LED6:
        {
            gpio_set_dir(LED_IO1, GPIO_OUT);
            gpio_set_dir(LED_IO2, GPIO_OUT);
            gpio_set_dir(LED_IO3, GPIO_IN);
            gpio_set_dir(LED_IO4, GPIO_IN);

            gpio_put(LED_IO1, 0);
            gpio_put(LED_IO2, 1);


        }break;
        case LED7:
        {
            gpio_set_dir(LED_IO1, GPIO_IN);
            gpio_set_dir(LED_IO2, GPIO_OUT);
            gpio_set_dir(LED_IO3, GPIO_OUT);
            gpio_set_dir(LED_IO4, GPIO_IN);

            gpio_put(LED_IO2, 0);
            gpio_put(LED_IO3, 1);

        }break;
        case LED8:
        {
            gpio_set_dir(LED_IO1, GPIO_IN);
            gpio_set_dir(LED_IO2, GPIO_IN);
            gpio_set_dir(LED_IO3, GPIO_OUT);
            gpio_set_dir(LED_IO4, GPIO_OUT);

            gpio_put(LED_IO3, 0);
            gpio_put(LED_IO4, 1);
        }break;
         case LED9:
        {
            gpio_set_dir(LED_IO1, GPIO_IN);
            gpio_set_dir(LED_IO2, GPIO_OUT);
            gpio_set_dir(LED_IO3, GPIO_IN);
            gpio_set_dir(LED_IO4, GPIO_OUT);
            
            gpio_put(LED_IO2, 1);
            gpio_put(LED_IO4, 0);
        }break;
         case LED10:
        {
            gpio_set_dir(LED_IO1, GPIO_IN);
            gpio_set_dir(LED_IO2, GPIO_OUT);
            gpio_set_dir(LED_IO3, GPIO_IN);
            gpio_set_dir(LED_IO4, GPIO_OUT);
            
            gpio_put(LED_IO2, 0);
            gpio_put(LED_IO4, 1);
        }break;
        default:
            gpio_put(LED_IO1, 1);
            gpio_put(LED_IO2, 1);
            gpio_put(LED_IO3, 1);
            gpio_put(LED_IO4, 1);
        break;
    }
}

