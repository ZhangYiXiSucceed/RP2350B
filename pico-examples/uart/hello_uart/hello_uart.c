/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

/// \tag::hello_uart[]

#define UART_ID uart1
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 4
#define UART_RX_PIN 5

/// \tag::timer_example[]
volatile bool timer_fired = false;

int64_t alarm_callback(alarm_id_t id, __unused void *user_data) {
    uart_puts(UART_ID, "Timer fired!\r\n");
    timer_fired = true;
    // Can return a value here in us to fire in the future
    return 0;
}

bool repeating_timer_callback(__unused struct repeating_timer *t) {
    uart_puts(UART_ID, "Repeat Timer end\r\n");
    return true;
}

int main() {
    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_TX_PIN));
    gpio_set_function(UART_RX_PIN, UART_FUNCSEL_NUM(UART_ID, UART_RX_PIN));

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART

    // // Send out a character without any conversions
    // uart_putc_raw(UART_ID, 'A');

    // // Send out a character but do CR/LF conversions
    // uart_putc(UART_ID, 'B');

    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\r\n");

    // Call alarm_callback in 2 seconds
    add_alarm_in_ms(2000, alarm_callback, NULL, false);
    struct repeating_timer timer;
    add_repeating_timer_ms(500, repeating_timer_callback, NULL, &timer);
    while(1)
    {
        uart_puts(UART_ID, "Hello, UART!\r\n");
        sleep_ms(1000);
    }
    return 0;
}

/// \end::hello_uart[]
