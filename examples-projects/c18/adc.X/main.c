#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/analog/adc/adc.h"

void main(void)
{
    uint16_t value;

    uart_init(9600u);
    adc_init();

    while (1)
    {
        value = adc_read(0u);

        DBG_PRINT("C18 ADC0=");
        DBG_PRINT_INT((int)value);
        DBG_PRINT(" mV=");
        DBG_PRINT_INT((int)adc_read_voltage(0u));
        DBG_PRINTLN("");

        DRV_DELAY_MS(250);
    }
}

