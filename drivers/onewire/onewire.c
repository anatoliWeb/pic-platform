#include "drivers/onewire/onewire.h"

#if defined(DRV_COMPILER_C18)
    #include "../../C18/drivers/onewire/onewire.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../XC8/drivers/onewire/onewire.c"
#else

#include "core/delay.h"
#include "core/bit_utils.h"

static volatile uint8_t* g_onewire_port = (volatile uint8_t*)0;
static volatile uint8_t* g_onewire_tris = (volatile uint8_t*)0;
static uint8_t g_onewire_pin = 0u;

static void onewire_set_low(void)
{
    CLEAR_BIT((*g_onewire_port), g_onewire_pin);
    CLEAR_BIT((*g_onewire_tris), g_onewire_pin);
}

static void onewire_release_line(void)
{
    SET_BIT((*g_onewire_tris), g_onewire_pin);
}

static uint8_t onewire_read_line(void)
{
    return (uint8_t)READ_BIT((*g_onewire_port), g_onewire_pin);
}

void onewire_init(volatile uint8_t* port, volatile uint8_t* tris, uint8_t pin)
{
    g_onewire_port = port;
    g_onewire_tris = tris;
    g_onewire_pin = pin;

    onewire_release_line();
}

uint8_t onewire_reset(void)
{
    uint8_t present;

    onewire_set_low();
    DRV_DELAY_US(480);

    onewire_release_line();
    DRV_DELAY_US(70);

    present = (onewire_read_line() == 0u) ? 1u : 0u;

    DRV_DELAY_US(410);

    return present;
}

#endif
