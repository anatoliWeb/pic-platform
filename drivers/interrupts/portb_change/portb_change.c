#include "drivers/interrupts/portb_change/portb_change.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/interrupts/portb_change/portb_change.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/interrupts/portb_change/portb_change.c"
#else

static volatile uint8_t g_last_state = 0u;
static volatile uint8_t g_changed_mask = 0u;
static portb_change_callback_t g_callback = 0;

void portb_change_init(void)
{
    TRISB |= 0xF0u;
    g_last_state = PORTB;
    g_changed_mask = 0u;
    INTCONbits.RBIF = 0u;
}

void portb_change_enable(void)
{
    INTCONbits.RBIF = 0u;
    INTCONbits.RBIE = 1u;
}

void portb_change_disable(void)
{
    INTCONbits.RBIE = 0u;
}

uint8_t portb_change_read_port(void)
{
    return PORTB;
}

uint8_t portb_change_get_last_state(void)
{
    return g_last_state;
}

uint8_t portb_change_get_changed_mask(void)
{
    return g_changed_mask;
}

void portb_change_set_callback(portb_change_callback_t cb)
{
    g_callback = cb;
}

void portb_change_irq_handler(void)
{
    uint8_t current;

    if (INTCONbits.RBIF == 0u)
    {
        return;
    }

    current = PORTB;
    g_changed_mask = (uint8_t)((current ^ g_last_state) & 0xF0u);
    g_last_state = current;

    INTCONbits.RBIF = 0u;

    if (g_callback != 0)
    {
        g_callback(g_changed_mask, current);
    }
}

#endif
