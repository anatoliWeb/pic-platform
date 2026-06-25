/*
 * File: drivers/analog/adc/example.c
 */

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/analog/adc/adc.h"

static void adc_example_potentiometer(void)
{
    uint16_t value = adc_read(0u);

    DBG_PRINT("POT=");
    DBG_PRINT_INT((int)value);
    DBG_PRINTLN("");
}

static void adc_example_thermistor(void)
{
    int16_t temp_c = adc_read_thermistor(1u);

    DBG_PRINT("TEMP=");
    DBG_PRINT_INT((int)temp_c);
    DBG_PRINTLN("C");
}

static void adc_example_buttons(void)
{
    uint8_t button = adc_read_button(2u);

    DBG_PRINT("BTN=");
    DBG_PRINT_INT((int)button);
    DBG_PRINTLN("");
}

static void adc_example_avg(void)
{
    uint16_t filtered = adc_read_avg(0u, 8u);

    DBG_PRINT("AVG=");
    DBG_PRINT_INT((int)filtered);
    DBG_PRINTLN("");
}

void main(void)
{
    uart_init(9600u);
    adc_init();

    while (1)
    {
        adc_example_potentiometer();
        adc_example_thermistor();
        adc_example_buttons();
        adc_example_avg();
        DRV_DELAY_MS(200);
    }
}
