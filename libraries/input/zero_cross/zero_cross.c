/*
 * File: libraries/input/zero_cross/zero_cross.c
 *
 * Implementation of the reusable zero-cross detector. See zero_cross.h for
 * the API contract.
 *
 * Time model: the caller provides a monotonic microsecond time in now_us.
 * The library never owns a timer or a clock; it only compares edge timestamps.
 * All state lives in the caller-owned zero_cross_t instance, so any number of
 * consumers can share the same detector by dispatching the produced event.
 */

#include "libraries/input/zero_cross/zero_cross.h"

#ifdef ZERO_CROSS_ENABLE_FREQUENCY_DETECTION
#define ZERO_CROSS_50HZ_HALF_CYCLE_US   10000U
#define ZERO_CROSS_60HZ_HALF_CYCLE_US   8333U
#define ZERO_CROSS_FREQ_BOUNDARY_US     \
    ((uint16_t)((ZERO_CROSS_50HZ_HALF_CYCLE_US + ZERO_CROSS_60HZ_HALF_CYCLE_US) / 2u))
#endif

drv_status_t zero_cross_init(zero_cross_t* zc,
                             const zero_cross_config_t* config)
{
    if ((zc == (zero_cross_t*)0) ||
        (config == (const zero_cross_config_t*)0))
    {
        return DRV_STATUS_ERROR;
    }

    if ((config->recovery_event_count == 0u) ||
        (config->min_half_cycle_us >= config->max_half_cycle_us))
    {
        zc->status = ZERO_CROSS_STATUS_CONFIG_ERROR;
        zc->initialized = 0u;
        return DRV_STATUS_ERROR;
    }

    zc->config = *config;
    zc->status = ZERO_CROSS_STATUS_WAITING;
    zc->last_edge_us = 0UL;
    zc->sequence = 0UL;
    zc->half_cycle_us = 0u;
    zc->frequency = ZERO_CROSS_FREQUENCY_UNKNOWN;
    zc->recovery_count = 0u;
    zc->initialized = 1u;

    return DRV_STATUS_OK;
}

uint8_t zero_cross_on_edge(zero_cross_t* zc,
                           uint32_t now_us,
                           zero_cross_event_t* event)
{
    uint32_t half_cycle;

    if ((zc == (zero_cross_t*)0) ||
        (zc->initialized == 0u) ||
        (zc->status == ZERO_CROSS_STATUS_CONFIG_ERROR))
    {
        return 0u;
    }

#ifdef ZERO_CROSS_ENABLE_GLITCH_FILTER
    if ((zc->config.glitch_reject_us != 0u) &&
        (zc->last_edge_us != 0UL) &&
        ((now_us - zc->last_edge_us) <
         (uint32_t)zc->config.glitch_reject_us))
    {
        return 0u;
    }
#endif

    if (zc->last_edge_us == 0UL)
    {
        /* First edge only arms the detector; half-cycle needs two edges. */
        zc->last_edge_us = now_us;
        zc->status = ZERO_CROSS_STATUS_WAITING;
        return 0u;
    }

    half_cycle = now_us - zc->last_edge_us;

    if ((half_cycle < (uint32_t)zc->config.min_half_cycle_us) ||
        (half_cycle > (uint32_t)zc->config.max_half_cycle_us))
    {
        return 0u;
    }

    zc->last_edge_us = now_us;
    zc->half_cycle_us = (uint16_t)half_cycle;
    zc->sequence++;

#ifdef ZERO_CROSS_ENABLE_FREQUENCY_DETECTION
    zc->frequency = (half_cycle >= (uint32_t)ZERO_CROSS_FREQ_BOUNDARY_US)
                        ? ZERO_CROSS_FREQUENCY_50_HZ
                        : ZERO_CROSS_FREQUENCY_60_HZ;
#endif

    if (zc->status == ZERO_CROSS_STATUS_LOST)
    {
        zc->recovery_count++;
        if (zc->recovery_count < zc->config.recovery_event_count)
        {
            return 0u;
        }
        zc->recovery_count = 0u;
    }

    zc->status = ZERO_CROSS_STATUS_ALIVE;

    if (event != (zero_cross_event_t*)0)
    {
        event->timestamp_us = now_us;
        event->half_cycle_us = (uint16_t)half_cycle;
        event->sequence = zc->sequence;
        event->frequency = zc->frequency;
    }

    return 1u;
}

void zero_cross_process(zero_cross_t* zc, uint32_t now_us)
{
    if ((zc == (zero_cross_t*)0) ||
        (zc->initialized == 0u))
    {
        return;
    }

#ifdef ZERO_CROSS_ENABLE_TIMEOUT
    if ((zc->config.timeout_ms != 0u) &&
        (zc->last_edge_us != 0UL) &&
        (zc->status == ZERO_CROSS_STATUS_ALIVE))
    {
        if ((now_us - zc->last_edge_us) >
            ((uint32_t)zc->config.timeout_ms * 1000u))
        {
            zc->status = ZERO_CROSS_STATUS_LOST;
            zc->recovery_count = 0u;
        }
    }
#endif
}

uint8_t zero_cross_is_alive(const zero_cross_t* zc)
{
    if ((zc == (const zero_cross_t*)0) ||
        (zc->initialized == 0u))
    {
        return 0u;
    }

    return (zc->status == ZERO_CROSS_STATUS_ALIVE) ? 1u : 0u;
}

zero_cross_status_t zero_cross_get_status(const zero_cross_t* zc)
{
    if ((zc == (const zero_cross_t*)0) ||
        (zc->initialized == 0u))
    {
        return ZERO_CROSS_STATUS_NOT_INITIALIZED;
    }

    return zc->status;
}

zero_cross_frequency_t zero_cross_get_frequency(const zero_cross_t* zc)
{
    if ((zc == (const zero_cross_t*)0) ||
        (zc->initialized == 0u))
    {
        return ZERO_CROSS_FREQUENCY_UNKNOWN;
    }

    return zc->frequency;
}

uint16_t zero_cross_get_half_cycle_us(const zero_cross_t* zc)
{
    if ((zc == (const zero_cross_t*)0) ||
        (zc->initialized == 0u))
    {
        return 0u;
    }

    return zc->half_cycle_us;
}

uint32_t zero_cross_get_sequence(const zero_cross_t* zc)
{
    if ((zc == (const zero_cross_t*)0) ||
        (zc->initialized == 0u))
    {
        return 0UL;
    }

    return zc->sequence;
}

void zero_cross_reset(zero_cross_t* zc)
{
    if ((zc == (zero_cross_t*)0) ||
        (zc->initialized == 0u))
    {
        return;
    }

    zc->status = ZERO_CROSS_STATUS_WAITING;
    zc->last_edge_us = 0UL;
    zc->sequence = 0UL;
    zc->half_cycle_us = 0u;
    zc->frequency = ZERO_CROSS_FREQUENCY_UNKNOWN;
    zc->recovery_count = 0u;
}
