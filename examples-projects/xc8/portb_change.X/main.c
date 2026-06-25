/*
 * File: examples-projects/xc8/portb_change.X/main.c
 */

#include "project_config.h"

#include "core/compiler.h"
#include "core/delay.h"

#include "drivers/interrupts/portb_change/portb_change.h"
#include "drivers/communication/uart/uart.h"
#include "libraries/system/uart_debug/uart_debug.h"

#define BUTTON_MASK_RB4_RB7  0xF0u

static volatile uint8_t g_changed_mask = 0u;
static volatile uint8_t g_current_state = 0u;
static volatile uint8_t g_event_flag = 0u;

static void portb_cb(uint8_t changed_mask, uint8_t current_state)
{
    /*
     * Keep ISR callback short.
     * Save data and print later from main loop.
     */
    g_changed_mask = changed_mask;
    g_current_state = current_state;
    g_event_flag = 1u;
}

void __interrupt() isr(void)
{
    portb_change_irq_handler();
}

static void print_bits_rb4_rb7(uint8_t value)
{
    /*
     * Print RB7 RB6 RB5 RB4 as 4 bits.
     */
    DBG_PRINT((value & 0x80u) ? "1" : "0");
    DBG_PRINT((value & 0x40u) ? "1" : "0");
    DBG_PRINT((value & 0x20u) ? "1" : "0");
    DBG_PRINT((value & 0x10u) ? "1" : "0");
}

static void update_leds_from_portb(uint8_t portb_state)
{
    /*
     * LEDs:
     *   RD0 shows RB4
     *   RD1 shows RB5
     *   RD2 shows RB6
     *   RD3 shows RB7
     *
     * Buttons are active-low:
     *   pressed = 0
     *
     * LED ON when button is pressed.
     */
    LATDbits.LATD0 = ((portb_state & 0x10u) == 0u) ? 1u : 0u;
    LATDbits.LATD1 = ((portb_state & 0x20u) == 0u) ? 1u : 0u;
    LATDbits.LATD2 = ((portb_state & 0x40u) == 0u) ? 1u : 0u;
    LATDbits.LATD3 = ((portb_state & 0x80u) == 0u) ? 1u : 0u;
}

void main(void)
{
    uint8_t changed;
    uint8_t current;

    /*
     * PIC18F452 PORTB interrupt-on-change example.
     *
     * IMPORTANT:
     *   PORTB change interrupt works on RB4-RB7 only.
     *
     * Proteus wiring:
     *   RB4 / pin 37 -> button to GND, 10k pull-up to +5V
     *   RB5 / pin 38 -> button to GND, 10k pull-up to +5V
     *   RB6 / pin 39 -> button to GND, 10k pull-up to +5V
     *   RB7 / pin 40 -> button to GND, 10k pull-up to +5V
     *
     * LED indicators:
     *   RD0 / pin 19 -> LED for RB4
     *   RD1 / pin 20 -> LED for RB5
     *   RD2 / pin 21 -> LED for RB6
     *   RD3 / pin 22 -> LED for RB7
     *
     * UART:
     *   RC6/TX / pin 25 -> Virtual Terminal RXD
     */

    uart_init(9600u);

    /*
     * RB4-RB7 as inputs.
     */
    TRISBbits.TRISB4 = 1u;
    TRISBbits.TRISB5 = 1u;
    TRISBbits.TRISB6 = 1u;
    TRISBbits.TRISB7 = 1u;

    /*
     * RD0-RD3 as outputs for LEDs.
     */
    TRISDbits.TRISD0 = 0u;
    TRISDbits.TRISD1 = 0u;
    TRISDbits.TRISD2 = 0u;
    TRISDbits.TRISD3 = 0u;

    LATDbits.LATD0 = 0u;
    LATDbits.LATD1 = 0u;
    LATDbits.LATD2 = 0u;
    LATDbits.LATD3 = 0u;

    DBG_PRINTLN("");
    DBG_PRINTLN("PIC18F452 PORTB change interrupt test");
    DBG_PRINTLN("Inputs: RB4-RB7 / pins 37-40");
    DBG_PRINTLN("LEDs: RD0-RD3 / pins 19-22");
    DBG_PRINTLN("Buttons are active-low");
    DBG_PRINTLN("");

    /*
     * Initialize and enable PORTB change interrupt.
     */
    portb_change_init();
    portb_change_set_callback(portb_cb);
    portb_change_enable();

    /*
     * Initial LED state.
     */
    update_leds_from_portb(PORTB);

    while (1)
    {
        if (g_event_flag != 0u)
        {
            /*
             * Copy volatile data locally.
             */
            g_event_flag = 0u;
            changed = g_changed_mask;
            current = g_current_state;

            update_leds_from_portb(current);

            DBG_PRINT("PORTB change: changed=");
            DBG_PRINT_HEX(changed);

            DBG_PRINT(" current=");
            DBG_PRINT_HEX(current);

            DBG_PRINT(" RB7..RB4=");
            print_bits_rb4_rb7(current);

            DBG_PRINTLN("");
        }

        /*
         * Small delay only for readable UART output.
         */
        DRV_DELAY_MS(10);
    }
}