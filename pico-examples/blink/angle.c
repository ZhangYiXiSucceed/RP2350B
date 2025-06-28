#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "main.h"

#define  ANGLE_REG_SCL  23
#define  ANGLE_REG_SDA  22



// By default these devices  are on bus address 0x68
static int addr = 0x68;

static void kxtj3_1057_reset() {
    // Two byte reset. First byte register, second byte data
    // There are a load more options to set up the device in different ways that could be added here
    uint8_t buf[] = {0x6B, 0x80};
    i2c_write_blocking(i2c1, addr, buf, 2, false);
    sleep_ms(100); // Allow device to reset and stabilize

    // Clear sleep mode (0x6B register, 0x00 value)
    buf[1] = 0x00;  // Clear sleep mode by writing 0x00 to the 0x6B register
    i2c_write_blocking(i2c1, addr, buf, 2, false); 
    sleep_ms(10); // Allow stabilization after waking up
}


uint8_t kxtj3_1057_read_id()
{
    uint8_t reg_id;
    uint8_t buf[] = {0x0F,0x00};
    i2c_write_blocking(i2c1, 0x0E, buf, 1, false);
    i2c_read_blocking(i2c1, 0x0E, &reg_id, 1, false);
    return reg_id;
}


int16_t kxtj3_1057_read_data(uint8_t  reg_addr)
{
    int16_t xdata=0;
    uint8_t data;
    uint8_t buf[] = {0x06,0x00};
    buf[0] = reg_addr;
    i2c_write_blocking(i2c1, 0x0E, buf, 1, false);
    i2c_read_blocking(i2c1, 0x0E, &data, 1, false);
    xdata |=(data>>4);

    buf[0] = reg_addr + 1;
    i2c_write_blocking(i2c1, 0x0E, buf, 1, false);
    i2c_read_blocking(i2c1, 0x0E, &data, 1, false);
    xdata |=(data<<8);
    return xdata;
}

void kxtj3_1057_sensor_init() {
    // This example will use I2C0 on the default SDA and SCL pins (4, 5 on a Pico)
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(ANGLE_REG_SDA, GPIO_FUNC_I2C);
    gpio_set_function(ANGLE_REG_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(ANGLE_REG_SDA);
    gpio_pull_up(ANGLE_REG_SCL);
    // Make the I2C pins available to picotool
    bi_decl(bi_2pins_with_func(ANGLE_REG_SDA, ANGLE_REG_SCL, GPIO_FUNC_I2C));

    //kxtj3_1057_reset();

    uint8_t buf[] = {0x1B,0xBC};
    i2c_write_blocking(i2c1, 0x0E, buf, 2, false);
    buf[0] = 0x1d;
    buf[1] = 0x06;
    i2c_write_blocking(i2c1, 0x0E, buf, 2, false);
    buf[0] = 0x21;
    buf[1] = 0x02;
    i2c_write_blocking(i2c1, 0x0E, buf, 2, false);
}
