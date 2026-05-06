#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/uart/uart.h"
#include "drivers/uart_debug/uart_debug.h"
#include "drivers/adc/adc.h"

void main(void)
{
    uint16_t value;

    uart_init(9600u);
    adc_init();

    while (1)
    {
        value = adc_read(0u);

        DBG_PRINT("XC8 ADC0=");
        DBG_PRINT_INT((int)value);
        DBG_PRINT(" mV=");
        DBG_PRINT_INT((int)adc_read_voltage(0u));
        DBG_PRINTLN("");

        DRV_DELAY_MS(250);
    }
}

