/*
 * File: examples-projects/xc8/clock.X/main.c
 */

#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/system/clock/clock.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define LED_TRIS    TRISBbits.TRISB0
#define LED_LAT     LATBbits.LATB0

static void app_io_init(void)
{
    /*
     * RB0 = heartbeat LED output.
     * RC6/TX = UART TX.
     */
    ADCON1 = 0x0Fu;

    LED_TRIS = 0u;
    LED_LAT = 0u;
}

static void app_print_clock_info(void)
{
    DBG_PRINTLN("XC8 clock driver example started");

    DBG_PRINT("Fosc=");
    DBG_PRINT_INT((int)clock_get_frequency_khz());
    DBG_PRINTLN(" kHz");

    DBG_PRINT("Instruction clock=");
    DBG_PRINT_INT((int)clock_get_instruction_frequency_khz());
    DBG_PRINTLN(" kHz");

#ifdef _XTAL_FREQ
    DBG_PRINT("_XTAL_FREQ=");
    DBG_PRINT_INT((int)(_XTAL_FREQ / 1000UL));
    DBG_PRINTLN(" kHz");
#endif

#ifdef DRV_XTAL_FREQ
    DBG_PRINT("DRV_XTAL_FREQ=");
    DBG_PRINT_INT((int)(DRV_XTAL_FREQ / 1000UL));
    DBG_PRINTLN(" kHz");
#endif

#ifdef PIC_PLATFORM_CLOCK_HZ
    DBG_PRINT("PIC_PLATFORM_CLOCK_HZ=");
    DBG_PRINT_INT((int)(PIC_PLATFORM_CLOCK_HZ / 1000UL));
    DBG_PRINTLN(" kHz");
#endif

    DBG_PRINTLN("RB0 heartbeat toggles every 500 ms");
}

void main(void)
{
    app_io_init();
    uart_init(9600u);

    /*
     * The oscillator mode is configured by config_bits.c.
     * The clock driver exposes the configured frequency to application code.
     */
    app_print_clock_info();

    while (1)
    {
        LED_LAT = (LED_LAT == 0u) ? 1u : 0u;

        DBG_PRINT("heartbeat RB0=");
        DBG_PRINT_INT((int)LED_LAT);
        DBG_PRINTLN("");

        delay_ms(500u);
    }
}