#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"
#include "drivers/system/reset/reset.h"

static void print_reset_cause(reset_cause_t cause)
{
    /*
     * Print numeric and text reset reason.
     *
     * Enum values are defined in:
     *   drivers/system/reset/reset.h
     */
    DBG_PRINT("Reset cause code=");
    DBG_PRINT_INT((int)cause);
    DBG_PRINT(" ");

    switch (cause)
    {
        case RESET_CAUSE_POWER_ON:
            DBG_PRINTLN("POWER_ON");
            break;

        case RESET_CAUSE_BROWN_OUT:
            DBG_PRINTLN("BROWN_OUT");
            break;

        case RESET_CAUSE_WATCHDOG:
            DBG_PRINTLN("WATCHDOG");
            break;

        case RESET_CAUSE_EXTERNAL:
            /*
             * For PIC18F452 this is usually external reset through MCLR.
             */
            DBG_PRINTLN("EXTERNAL_MCLR");
            break;

        case RESET_CAUSE_SOFTWARE:
            DBG_PRINTLN("SOFTWARE");
            break;

        case RESET_CAUSE_UNKNOWN:
        default:
            DBG_PRINTLN("UNKNOWN");
            break;
    }
}

void main(void)
{
    reset_cause_t cause;

    /*
     * PIC18F452 reset cause test.
     *
     * Proteus wiring:
     *   MCLR / pin 1 -> 10k pull-up -> +5V
     *   MCLR / pin 1 -> push button -> GND
     *
     * UART debug:
     *   RC6/TX / pin 25 -> Virtual Terminal RXD
     *   RC7/RX / pin 26 <- Virtual Terminal TXD
     *
     * Virtual Terminal:
     *   9600 baud
     *   8N1
     *
     * Expected:
     *   - On first simulation start: POWER_ON or UNKNOWN depending on simulator flags.
     *   - After pressing MCLR button: EXTERNAL_MCLR.
     */

    uart_init(9600u);

    /*
     * Initialize reset driver.
     * The driver stores or prepares reset flag state.
     */
    reset_init();

    /*
     * Read reset cause before clearing reset flags.
     */
    cause = reset_get_cause();

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 Reset cause test");
    DBG_PRINTLN("MCLR: pin 1 -> 10k -> +5V");
    DBG_PRINTLN("MCLR button: pin 1 -> button -> GND");
    DBG_PRINTLN("");

    print_reset_cause(cause);

    /*
     * Clear reset flags after reading them.
     *
     * Important:
     *   If flags are cleared before reset_get_cause(),
     *   the reset reason can be lost.
     */
    reset_clear_flags();

    DBG_PRINTLN("");
    DBG_PRINTLN("Reset flags cleared.");
    DBG_PRINTLN("Press MCLR button in Proteus to test external reset.");
    DBG_PRINTLN("");

    while (1)
    {
        /*
         * Nothing else to do.
         * The next visible event is another reset.
         */
        DRV_DELAY_MS(1000);
    }
}