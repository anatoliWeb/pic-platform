#include "project_config.h"

#include "core/compiler.h"
#include "drivers/_template/template.h"

/*
 * Template example for PIC18F452.
 *
 * Purpose:
 *   This example is a minimal startup project for a new driver.
 *
 * It verifies that:
 *   - project_config.h is included correctly
 *   - compiler abstraction layer works
 *   - template driver header is available
 *   - template_init() can be called from main()
 *
 * This example does not use any external peripheral.
 * It is only a clean skeleton for future driver examples.
 *
 * Proteus wiring:
 *   VDD pins 11, 32 -> +5V
 *   VSS pins 12, 31 -> GND
 *   MCLR pin 1 -> 10k pull-up to +5V
 *   OSC1 pin 13 / OSC2 pin 14 -> 10 MHz crystal
 *
 * No UART, LEDs, buttons, sensors, or external chips are required.
 */

void main(void)
{
    /*
     * Minimal XC8 startup example.
     *
     * This function should contain only the driver initialization call.
     * Real driver-specific logic should be added later in the main loop.
     */
    template_init();

    while (1)
    {
        /*
         * Driver call flow goes here.
         *
         * This area is intentionally empty.
         *
         * When a real driver is created from this template,
         * typical code flow may look like:
         *
         *   driver_update();
         *   driver_read();
         *   driver_write();
         *
         * For now this loop only keeps the MCU running.
         */
    }
}