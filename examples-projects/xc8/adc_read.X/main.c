/*
 * File: examples-projects/xc8/adc_read.X/main.c
 */

#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/analog/adc/adc.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

/*
 * ADC read example
 *
 * Notes:
 *   - Reads ADC channel 0.
 *   - Debug output is kept simple for MPLAB project integration.
 */

void main(void)
{
    uint16_t value;

    uart_init(9600u);
    adc_init();

    DBG_PRINTLN("ADC read example ready");

    while (1)
    {
        value = adc_read(0u);
        DBG_PRINT("ADC0 raw = ");
        DBG_PRINT_INT((int)value);
        DBG_PRINTLN("");
        delay_ms(1000u);
    }
}