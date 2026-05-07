#include "drivers/timers/tick/tick.h"
#include "drivers/timers/timer1/timer1.h"

#include "core/device.h"

#define TICK_PRESCALER 8u

static volatile uint32_t g_tick_ms = 0u;
static uint16_t g_tick_reload = 0u;

static void tick_timer1_cb(void)
{
    timer1_set(g_tick_reload);
    g_tick_ms++;
}

void tick_init(void)
{
    uint32_t counts_per_ms;

    counts_per_ms = (_XTAL_FREQ / 4u) / TICK_PRESCALER / 1000u;
    if (counts_per_ms == 0u)
    {
        counts_per_ms = 1u;
    }
    if (counts_per_ms > 65535u)
    {
        counts_per_ms = 65535u;
    }

    g_tick_reload = (uint16_t)(65536u - (uint16_t)counts_per_ms);
    g_tick_ms = 0u;

    timer1_init(TICK_PRESCALER);
    timer1_set(g_tick_reload);
    timer1_set_callback(tick_timer1_cb);
    timer1_enable_interrupt();
    timer1_start();
}

uint32_t tick_get(void)
{
    return g_tick_ms;
}

void tick_delay(uint32_t ms)
{
    uint32_t start = tick_get();

    while ((tick_get() - start) < ms)
    {
    }
}
