/*
 * File: libraries/sensors/tachometer/tachometer.c
 */

#include "libraries/sensors/tachometer/tachometer.h"

static uint32_t tachometer_ms_to_us(uint16_t ms)
{
    return (uint32_t)ms * 1000UL;
}

/*
 * Re-arm the timing session. The stale last_pulse_us timestamp is discarded,
 * RPM is forced back to zero, and the next pulse starts a fresh timing window.
 * The cumulative pulse_count is deliberately preserved: a timeout ends the
 * interval measurement without erasing the accepted-pulse history.
 */
static void tachometer_rearm(tachometer_t* tachometer)
{
    tachometer->session_state = TACHOMETER_SESSION_UNARMED;
    tachometer->last_pulse_us = 0UL;
    tachometer->rpm = 0u;
}

/*
 * Full measurement reset. Unlike a re-arm, it also clears the cumulative
 * statistics, so the next expected-running session counts from zero.
 */
static void tachometer_clear_measurement(tachometer_t* tachometer)
{
    tachometer_rearm(tachometer);
    tachometer->pulse_count = 0UL;
}

/*
 * Convert one accepted pulse interval into RPM.
 *
 *   rpm = 60000000 / (interval_us * pulses_per_revolution)
 *
 * The microsecond interval times the number of pulses per revolution is kept
 * in 64-bit so the multiplication cannot overflow 16-bit or 32-bit math.
 */
static uint16_t tachometer_compute_rpm(const tachometer_t* tachometer,
                                       uint32_t pulse_interval_us)
{
    uint64_t denominator;
    uint64_t rpm;

    if ((pulse_interval_us == 0UL) ||
        (tachometer->config.pulses_per_revolution == 0u))
    {
        return 0u;
    }

    denominator = (uint64_t)pulse_interval_us * (uint64_t)tachometer->config.pulses_per_revolution;
    rpm = 60000000ULL / denominator;

    if (rpm > 65535ULL)
    {
        return 65535u;
    }

    return (uint16_t)rpm;
}

static void tachometer_update_status(tachometer_t* tachometer, uint32_t now_us)
{
    uint32_t grace_us;
    uint32_t timeout_us;
    uint32_t since_start_us;

    if ((tachometer == (tachometer_t*)0) || (tachometer->initialized == 0u))
    {
        return;
    }

    if (tachometer->status == TACHOMETER_STATUS_CONFIG_ERROR)
    {
        return;
    }

    if (tachometer->expected_running == 0u)
    {
        /* Running is not expected: fully idle, discard any pending session. */
        tachometer_rearm(tachometer);
        tachometer->status = TACHOMETER_STATUS_STOPPED;
        return;
    }

    grace_us = tachometer_ms_to_us(tachometer->config.startup_grace_ms);
    since_start_us = (uint32_t)(now_us - tachometer->expected_running_since_us);

    /* Still inside the startup grace window: running is expected but a fresh
     * RPM is not yet meaningful, so report STARTING instead of TOO_SLOW. */
    if (since_start_us < grace_us)
    {
        tachometer->status = TACHOMETER_STATUS_STARTING;
        return;
    }

    if (tachometer->session_state == TACHOMETER_SESSION_UNARMED)
    {
        /* Running is expected but no pulse has ever arrived. */
        tachometer_rearm(tachometer);
        tachometer->status = TACHOMETER_STATUS_NO_SIGNAL;
        return;
    }

    timeout_us = tachometer_ms_to_us(tachometer->config.signal_timeout_ms);
    if ((timeout_us != 0UL) &&
        ((uint32_t)(now_us - tachometer->last_pulse_us) >= timeout_us))
    {
        /* Signal loss: pulses stopped beyond the timeout. Re-arm so the next
         * pulse starts a fresh measurement, keeping the cumulative count. */
        tachometer_rearm(tachometer);
        tachometer->status = TACHOMETER_STATUS_NO_SIGNAL;
        return;
    }

    if (tachometer->session_state != TACHOMETER_SESSION_ACTIVE)
    {
        /* Only the first pulse so far, so no interval/RPM exists yet. */
        tachometer->status = TACHOMETER_STATUS_STARTING;
        return;
    }

    if ((tachometer->rpm < tachometer->config.minimum_rpm) &&
        (tachometer->config.minimum_rpm != 0u))
    {
        /* Pulses are coming in, but slower than the configured minimum. */
        tachometer->status = TACHOMETER_STATUS_TOO_SLOW;
        return;
    }

    tachometer->status = TACHOMETER_STATUS_RUNNING;
}

drv_status_t tachometer_init(tachometer_t* tachometer,
                             const tachometer_config_t* config)
{
    if ((tachometer == (tachometer_t*)0) ||
        (config == (const tachometer_config_t*)0))
    {
        return DRV_STATUS_ERROR;
    }

    tachometer->config = *config;
    tachometer->initialized = 1u;
    tachometer->expected_running = 0u;
    tachometer->expected_running_since_us = 0UL;
    tachometer->status = TACHOMETER_STATUS_STOPPED;
    tachometer_clear_measurement(tachometer);

    if (tachometer->config.pulses_per_revolution == 0u)
    {
        tachometer->status = TACHOMETER_STATUS_CONFIG_ERROR;
        return DRV_STATUS_ERROR;
    }

    return DRV_STATUS_OK;
}

void tachometer_set_expected_running(tachometer_t* tachometer,
                                     uint8_t expected_running,
                                     uint32_t now_us)
{
    if ((tachometer == (tachometer_t*)0) ||
        (tachometer->initialized == 0u) ||
        (tachometer->status == TACHOMETER_STATUS_CONFIG_ERROR))
    {
        return;
    }

    expected_running = (uint8_t)(expected_running != 0u ? 1u : 0u);
    if (tachometer->expected_running == expected_running)
    {
        return;
    }

    tachometer->expected_running = expected_running;
    tachometer->expected_running_since_us = now_us;
    /* Any transition of the running expectation starts a fresh measurement
     * session, because the previous RPM / interval belong to the old run. */
    tachometer_clear_measurement(tachometer);
    tachometer->status = (expected_running != 0u)
                             ? TACHOMETER_STATUS_STARTING
                             : TACHOMETER_STATUS_STOPPED;
}

uint8_t tachometer_on_pulse(tachometer_t* tachometer, uint32_t now_us)
{
    uint32_t interval_us;
    uint32_t timeout_us;

    if ((tachometer == (tachometer_t*)0) ||
        (tachometer->initialized == 0u) ||
        (tachometer->status == TACHOMETER_STATUS_CONFIG_ERROR) ||
        (tachometer->expected_running == 0u))
    {
        return 0u;
    }

    if (tachometer->session_state != TACHOMETER_SESSION_UNARMED)
    {
        timeout_us = tachometer_ms_to_us(tachometer->config.signal_timeout_ms);
        if ((timeout_us != 0UL) &&
            ((uint32_t)(now_us - tachometer->last_pulse_us) >= timeout_us))
        {
            /* A pulse can arrive before process() runs, so the timeout is
             * re-checked here. A pulse after the timeout starts a fresh
             * session instead of using the stale interval. */
            tachometer_rearm(tachometer);
            tachometer->status = TACHOMETER_STATUS_NO_SIGNAL;
        }
    }

    if (tachometer->session_state == TACHOMETER_SESSION_UNARMED)
    {
        /* First pulse of a session: only arm the measurement, no RPM yet.
         * The interval needs a second pulse, so RPM is forced to zero. */
        tachometer->session_state = TACHOMETER_SESSION_FIRST_PULSE;
        tachometer->last_pulse_us = now_us;
        tachometer->pulse_count++;
        tachometer->rpm = 0u;
        tachometer_update_status(tachometer, now_us);
        return 1u;
    }

    interval_us = (uint32_t)(now_us - tachometer->last_pulse_us);
    if ((tachometer->config.minimum_pulse_interval_us != 0u) &&
        (interval_us < (uint32_t)tachometer->config.minimum_pulse_interval_us))
    {
        /* Too-short interval is treated as noise: reject without updating the
         * timestamp or counters, so a burst of bounce pulses cannot skew RPM. */
        return 0u;
    }

    tachometer->last_pulse_us = now_us;
    /* Second and later accepted pulses compute RPM. The session phase
     * saturates at ACTIVE; it is not a cumulative counter, so a long pulse
     * stream cannot overflow it. */
    tachometer->session_state = TACHOMETER_SESSION_ACTIVE;
    tachometer->pulse_count++;
    tachometer->rpm = tachometer_compute_rpm(tachometer, interval_us);
    tachometer_update_status(tachometer, now_us);
    return 1u;
}

void tachometer_process(tachometer_t* tachometer, uint32_t now_us)
{
    if ((tachometer == (tachometer_t*)0) ||
        (tachometer->initialized == 0u))
    {
        return;
    }

    tachometer_update_status(tachometer, now_us);
}

uint16_t tachometer_get_rpm(const tachometer_t* tachometer)
{
    if ((tachometer == (const tachometer_t*)0) ||
        (tachometer->initialized == 0u))
    {
        return 0u;
    }

    return tachometer->rpm;
}

tachometer_status_t tachometer_get_status(const tachometer_t* tachometer)
{
    if (tachometer == (const tachometer_t*)0)
    {
        return TACHOMETER_STATUS_NOT_INITIALIZED;
    }

    if (tachometer->initialized == 0u)
    {
        return TACHOMETER_STATUS_NOT_INITIALIZED;
    }

    return tachometer->status;
}

uint32_t tachometer_get_pulse_count(const tachometer_t* tachometer)
{
    if ((tachometer == (const tachometer_t*)0) ||
        (tachometer->initialized == 0u))
    {
        return 0UL;
    }

    return tachometer->pulse_count;
}

void tachometer_reset(tachometer_t* tachometer)
{
    if ((tachometer == (tachometer_t*)0) ||
        (tachometer->initialized == 0u) ||
        (tachometer->status == TACHOMETER_STATUS_CONFIG_ERROR))
    {
        return;
    }

    tachometer->expected_running = 0u;
    tachometer->expected_running_since_us = 0UL;
    tachometer->status = TACHOMETER_STATUS_STOPPED;
    /* Reset clears runtime state only; the validated config is preserved. */
    tachometer_clear_measurement(tachometer);
}
