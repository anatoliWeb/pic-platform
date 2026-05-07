#include "core/compiler.h"
#include "drivers/communication/i2c/i2c.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

static void i2c_scan_example(void)
{
    uint8_t found[16];
    uint8_t count;
    uint8_t i;

    count = i2c_scan(found, 16u);
    DBG_PRINT("I2C devices=");
    DBG_PRINT_INT((int)count);
    DBG_PRINTLN("");

    for (i = 0u; i < count && i < 16u; i++)
    {
        DBG_PRINT("ADDR=");
        DBG_PRINT_HEX(found[i]);
        DBG_PRINTLN("");
    }
}

static void i2c_sensor_read_example(void)
{
    uint8_t sensor_val = 0u;

    if (i2c_read_register(0x48u, 0x00u, &sensor_val) == 0u)
    {
        DBG_PRINT("SENSOR=");
        DBG_PRINT_HEX(sensor_val);
        DBG_PRINTLN("");
    }
}

static void i2c_lcd_like_example(void)
{
    i2c_write_register(0x27u, 0x00u, 0x01u);
    i2c_write_register(0x27u, 0x40u, 0x41u);
}

void main(void)
{
    uart_init(9600u);
    i2c_init(100000u);

    i2c_scan_example();
    i2c_sensor_read_example();
    i2c_lcd_like_example();

    while (1)
    {
    }
}
