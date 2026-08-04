/*
 * File: libraries/input/digital_input/example.c
 *
 * Integration outline for the digital input helper. The application owns the
 * digital_input_t instance, feeds raw GPIO samples with a millisecond time
 * base, and owns any E-STOP / door reaction triggered by the consuming edge
 * flags.
 */

#include "libraries/input/digital_input/digital_input.h"

static digital_input_t g_input;

/*
 * 20 ms debounce, active-low (raw 0 = active), initial raw level released (1),
 * latch enabled so an active event is held until the application clears it.
 */
static const digital_input_config_t g_input_config =
{
    20u,
    0u,
    1u,
    1u
};

void digital_input_example_init(void)
{
    (void)digital_input_init(&g_input, &g_input_config);
}

/* Feed the raw GPIO sample here, at a periodic rate fast enough relative to
 * debounce_ms. The sample is the electrical pin level (0 or 1). */
void digital_input_example_update(uint8_t raw_level, uint32_t now_ms)
{
    digital_input_update(&g_input, raw_level, now_ms);
}

uint8_t digital_input_example_active(void)
{
    return digital_input_is_active(&g_input);
}