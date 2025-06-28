#include "main.h"

#include "pico/time.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#define PWM_IO  6

void on_pwm_wrap() {
    static int fade = 0;
    static bool going_up = true;
    // Clear the interrupt flag that brought us here
    pwm_clear_irq(pwm_gpio_to_slice_num(PWM_IO));

    // if (going_up) {
    //     ++fade;
    //     if (fade > 255) {
    //         fade = 255;
    //         going_up = false;
    //     }
    // } else {
    //     --fade;
    //     if (fade < 0) {
    //         fade = 0;
    //         going_up = true;
    //     }
    // }
    // Square the fade value to make the LED's brightness appear more linear
    // Note this range matches with the wrap value
    pwm_set_gpio_level(PWM_IO, 128*128);
}


void pwm_led_init() {

    // Tell the LED pin that the PWM is in charge of its value.
    gpio_set_function(PWM_IO, GPIO_FUNC_PWM);
    // Figure out which slice we just connected to the LED pin
    uint slice_num = pwm_gpio_to_slice_num(PWM_IO);

    // // Mask our slice's IRQ output into the PWM block's single interrupt line,
    // // and register our interrupt handler
    // pwm_clear_irq(slice_num);
    // pwm_set_irq_enabled(slice_num, true);
    // irq_set_exclusive_handler(PWM_DEFAULT_IRQ_NUM(), on_pwm_wrap);
    // irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);

    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 4);
    //pwm_config_set_wrap(&config, 255);
    pwm_set_gpio_level(PWM_IO, 128*128);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);

}