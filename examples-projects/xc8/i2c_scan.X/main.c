#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/i2c/i2c.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * I2C scan example
 *
 * Notes:
 *   - Prints the number of detected I2C devices.
 *   - Keep the source list external to the project.
 */

void main(void)
{
    uint8_t found[8];
    uint8_t count;
    uint8_t i;

    uart_init(9600u);
    i2c_init(100000u);

    DBG_PRINTLN("I2C scan example ready");

    while (1)
    {
        count = i2c_scan(found, 8u);

        DBG_PRINT("I2C devices = ");
        DBG_PRINT_INT((int)count);
        DBG_PRINTLN("");

        for (i = 0u; i < count; i++)
        {
            DBG_PRINT("  0x");
            DBG_PRINT_HEX(found[i]);
            DBG_PRINTLN("");
        }

        delay_ms(1000u);
    }
}