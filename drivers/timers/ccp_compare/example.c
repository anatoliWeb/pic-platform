/*
 * File: drivers/timers/ccp_compare/example.c
 */

#include "core/compiler.h"
#include "drivers/gpio/gpio.h"
#include "drivers/timers/ccp_compare/ccp_compare.h"

static volatile uint16_t compare_next = 0x2000u;

static void ccp_compare_cb(void)
{
    gpio_toggle(&PORTB, 0u);
    compare_next = (uint16_t)(compare_next + 0x1000u);
    ccp_compare_set_value(compare_next);
}

void main(void)
{
    gpio_set_output(&TRISB, 0u);

    ccp_compare_init();
    ccp_compare_set_value(compare_next);
    ccp_compare_set_callback(ccp_compare_cb);
    ccp_compare_enable();

    while (1)
    {
    }
}
