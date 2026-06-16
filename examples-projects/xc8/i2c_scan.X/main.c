#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/i2c/i2c.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

static void print_clock_info(void)
{
#ifdef PIC_PLATFORM_CLOCK_HZ
    DBG_PRINT("PIC_PLATFORM_CLOCK_HZ=");
    DBG_PRINT_INT((int)(PIC_PLATFORM_CLOCK_HZ / 1000000UL));
    DBG_PRINTLN(" MHz");
#endif

#ifdef _XTAL_FREQ
    DBG_PRINT("_XTAL_FREQ=");
    DBG_PRINT_INT((int)(_XTAL_FREQ / 1000000UL));
    DBG_PRINTLN(" MHz");
#endif

#ifdef DRV_XTAL_FREQ
    DBG_PRINT("DRV_XTAL_FREQ=");
    DBG_PRINT_INT((int)(DRV_XTAL_FREQ / 1000000UL));
    DBG_PRINTLN(" MHz");
#endif
}

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
    
    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 I2C scan");
    DBG_PRINTLN("SCL=RC3/pin18, SDA=RC4/pin23");
    
    print_clock_info();
    
    DBG_PRINTLN("");
    /*
     * Use 100 kHz because the scanner already worked at this speed.
     * After this debug test works, try 100 kHz again.
     */
    i2c_init(100000u);
    DBG_PRINTLN("Speed=100 kHz");   
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