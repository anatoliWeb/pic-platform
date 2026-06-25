/*
 * File: examples-projects/c18/delay.X/main.c
 */

#include "project_config.h"
#include "core/compiler.h"
#include "core/delay.h"

void main(void)
{
    /* Example pin: RB0 (output). */
    TRISBbits.TRISB0 = 0u;

    while (1)
    {
        /* ms-level blocking delay example (LED blink). */
        PORTBbits.RB0 = 1u;
        delay_ms(250u);

        PORTBbits.RB0 = 0u;
        delay_ms(250u);

        /* us-level short pulse example. */
        PORTBbits.RB0 = 1u;
        delay_us(10u);
        PORTBbits.RB0 = 0u;
        delay_us(10u);
    }
}
