/*
 * File: libraries/output/ac_phase_control/ac_phase_control.c
 *
 * Shared zero-cross synchronized phase-angle control with optional per-channel
 * relay bypass. See ac_phase_control.h for the API contract.
 */

#include "libraries/output/ac_phase_control/ac_phase_control.h"

#include "drivers/timers/timer2/timer2.h"

static ac_phase_control_group_t* g_active_group = (ac_phase_control_group_t*)0;

static uint32_t ac_phase_control_now_us(const ac_phase_control_group_t* group)
{
    return ((uint32_t)group->tick_ms * 1000UL) +
           (uint32_t)group->tick_accumulator_us;
}

static uint8_t ac_phase_control_channel_valid(const ac_phase_control_group_t* group,
                                             uint8_t channel)
{
    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->channels == (ac_phase_control_channel_t*)0) ||
        (channel >= group->channel_count))
    {
        return 0u;
    }

    return 1u;
}

static uint8_t ac_phase_control_mask_valid(uint8_t mask)
{
    if (mask == 0u)
    {
        return 0u;
    }

    if ((mask & (uint8_t)(mask - 1u)) != 0u)
    {
        return 0u;
    }

    return 1u;
}

static uint16_t ac_phase_control_clamp_u16(uint16_t value,
                                           uint16_t min_value,
                                           uint16_t max_value)
{
    if (value < min_value)
    {
        return min_value;
    }

    if (value > max_value)
    {
        return max_value;
    }

    return value;
}

static void ac_phase_control_write_gate(const ac_phase_control_channel_t* channel,
                                        uint8_t high)
{
    if ((channel == (const ac_phase_control_channel_t*)0) ||
        (channel->gate_lat == (volatile uint8_t*)0) ||
        (channel->gate_mask == 0u))
    {
        return;
    }

    if (high != 0u)
    {
        *(channel->gate_lat) |= channel->gate_mask;
    }
    else
    {
        *(channel->gate_lat) &= (uint8_t)(~channel->gate_mask);
    }
}

static void ac_phase_control_relay_write(const ac_phase_control_channel_t* channel,
                                         uint8_t active)
{
    if ((channel == (const ac_phase_control_channel_t*)0) ||
        (channel->relay_lat == (volatile uint8_t*)0) ||
        (channel->relay_mask == 0u))
    {
        return;
    }

    if (active != 0u)
    {
        *(channel->relay_lat) |= channel->relay_mask;
    }
    else
    {
        *(channel->relay_lat) &= (uint8_t)(~channel->relay_mask);
    }
}

static void ac_phase_control_channel_relay_off(ac_phase_control_channel_t* channel)
{
    if (channel == (ac_phase_control_channel_t*)0)
    {
        return;
    }

    channel->relay_state = (uint8_t)AC_PHASE_RELAY_STATE_PHASE;
    channel->relay_active = 0u;
}

/*
 * Collect the combined controlled bit mask and the high-state bit mask that a
 * single LAT register must carry. Gate pulses are suppressed while a channel
 * is in any relay transition or relay-on state to guarantee break-before-make.
 */
static void ac_phase_control_collect_lat(const ac_phase_control_group_t* group,
                                         volatile uint8_t* lat,
                                         uint8_t* controlled,
                                         uint8_t* high)
{
    uint8_t scan;
    uint8_t blocked;
    const ac_phase_control_channel_t* entry;

    *controlled = 0u;
    *high = 0u;

    for (scan = 0u; scan < group->channel_count; scan++)
    {
        entry = &group->channels[scan];

        if (entry->attached == 0u)
        {
            continue;
        }

        if (entry->gate_lat == lat)
        {
            *controlled |= entry->gate_mask;
            blocked = (entry->relay_state != (uint8_t)AC_PHASE_RELAY_STATE_PHASE) ? 1u : 0u;
            if ((entry->enabled != 0u) &&
                (entry->pulse_active != 0u) &&
                (blocked == 0u))
            {
                *high |= entry->gate_mask;
            }
        }

        if (entry->relay_lat == lat)
        {
            *controlled |= entry->relay_mask;
            if ((entry->enabled != 0u) && (entry->relay_active != 0u))
            {
                *high |= entry->relay_mask;
            }
        }
    }
}

static void ac_phase_control_apply_group_outputs(
    const ac_phase_control_group_t* group)
{
    uint8_t channel;
    uint8_t scan;
    uint8_t already_processed;
    uint8_t controlled_mask;
    uint8_t high_mask;
    uint8_t lat_value;
    volatile uint8_t* lat;
    const ac_phase_control_channel_t* entry;

    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->channels == (ac_phase_control_channel_t*)0))
    {
        return;
    }

    /*
     * Update every physical LAT register only once. Several gate/relay
     * channels may share the same LATx register; building one combined mask
     * prevents one channel update from disturbing another on the same port.
     */
    for (channel = 0u; channel < group->channel_count; channel++)
    {
        entry = &group->channels[channel];

        if ((entry->attached == 0u) ||
            (entry->gate_lat == (volatile uint8_t*)0))
        {
            continue;
        }

        already_processed = 0u;

        for (scan = 0u; scan < channel; scan++)
        {
            if ((group->channels[scan].attached != 0u) &&
                ((group->channels[scan].gate_lat == entry->gate_lat) ||
                 (group->channels[scan].relay_lat == entry->gate_lat)))
            {
                already_processed = 1u;
                break;
            }
        }

        if (already_processed != 0u)
        {
            continue;
        }

        lat = entry->gate_lat;
        ac_phase_control_collect_lat(group, lat, &controlled_mask, &high_mask);

        lat_value = *lat;
        lat_value &= (uint8_t)(~controlled_mask);
        lat_value |= high_mask;
        *lat = lat_value;
    }

    /*
     * Relay-only LAT registers (no gate attached on the same register) are
     * written in a second pass.
     */
    for (channel = 0u; channel < group->channel_count; channel++)
    {
        entry = &group->channels[channel];

        if ((entry->attached == 0u) ||
            (entry->relay_lat == (volatile uint8_t*)0))
        {
            continue;
        }

        already_processed = 0u;

        for (scan = 0u; scan < channel; scan++)
        {
            if ((group->channels[scan].attached != 0u) &&
                ((group->channels[scan].gate_lat == entry->relay_lat) ||
                 (group->channels[scan].relay_lat == entry->relay_lat)))
            {
                already_processed = 1u;
                break;
            }
        }

        if (already_processed != 0u)
        {
            continue;
        }

        lat = entry->relay_lat;
        ac_phase_control_collect_lat(group, lat, &controlled_mask, &high_mask);

        lat_value = *lat;
        lat_value &= (uint8_t)(~controlled_mask);
        lat_value |= high_mask;
        *lat = lat_value;
    }
}

static void ac_phase_control_reset_channel_cycle(ac_phase_control_channel_t* channel)
{
    if (channel == (ac_phase_control_channel_t*)0)
    {
        return;
    }

    channel->pulse_active = 0u;
    channel->pulse_generated = 0u;
    ac_phase_control_write_gate(channel, 0u);
}

static void ac_phase_control_hold_until_next_zero_cross(ac_phase_control_group_t* group,
                                                        ac_phase_control_channel_t* channel)
{
    if ((group == (ac_phase_control_group_t*)0) ||
        (channel == (ac_phase_control_channel_t*)0))
    {
        return;
    }

    channel->pulse_active = 0u;
    channel->pulse_generated = 1u;
    ac_phase_control_write_gate(channel, 0u);
}

static uint16_t ac_phase_control_map_percent_to_delay(const ac_phase_control_group_t* group,
                                                      uint8_t percent)
{
    uint16_t delay_range;
    uint32_t scaled;

    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->config.max_delay_us <= group->config.min_delay_us))
    {
        return 0u;
    }

    if (percent >= 100u)
    {
        return group->config.min_delay_us;
    }

    if (percent == 0u)
    {
        return group->config.max_delay_us;
    }

    delay_range = (uint16_t)(group->config.max_delay_us - group->config.min_delay_us);
    scaled = (uint32_t)delay_range * (uint32_t)(100u - percent);
    scaled /= 100u;

    return (uint16_t)(group->config.min_delay_us + (uint16_t)scaled);
}

static uint8_t ac_phase_control_map_delay_to_percent(const ac_phase_control_group_t* group,
                                                     uint16_t delay_us)
{
    uint16_t clamped_delay;
    uint16_t delay_range;
    uint32_t scaled;

    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->config.max_delay_us <= group->config.min_delay_us))
    {
        return 100u;
    }

    clamped_delay = ac_phase_control_clamp_u16(delay_us,
                                               group->config.min_delay_us,
                                               group->config.max_delay_us);

    if (clamped_delay <= group->config.min_delay_us)
    {
        return 100u;
    }

    if (clamped_delay >= group->config.max_delay_us)
    {
        return 0u;
    }

    delay_range = (uint16_t)(group->config.max_delay_us - group->config.min_delay_us);
    scaled = (uint32_t)(clamped_delay - group->config.min_delay_us) * 100u;
    scaled /= delay_range;

    return (uint8_t)(100u - (uint8_t)scaled);
}

static void ac_phase_control_release_relay(ac_phase_control_channel_t* channel,
                                           uint32_t now_ms)
{
    if (channel == (ac_phase_control_channel_t*)0)
    {
        return;
    }

    channel->relay_active = 0u;
    channel->relay_state = (uint8_t)AC_PHASE_RELAY_STATE_WAIT_OFF;
    channel->relay_off_ms = now_ms;
}

static void ac_phase_control_enter_fault(ac_phase_control_group_t* group)
{
    uint8_t channel;
    ac_phase_control_channel_t* entry;

    group->status = AC_PHASE_STATUS_ZERO_CROSS_LOST;
    group->half_cycle_active = 0u;
    group->elapsed_us = 0u;

    for (channel = 0u; channel < group->channel_count; channel++)
    {
        entry = &group->channels[channel];
        entry->pulse_active = 0u;
        entry->pulse_generated = 0u;
        ac_phase_control_channel_relay_off(entry);
    }

    ac_phase_control_apply_group_outputs(group);
}

/*
 * Advance one channel's relay state machine. Uses the shared tick_ms timebase
 * so no blocking delay is required. enforce On/OFF times, hysteresis and
 * break-before-make are handled here.
 */
static void ac_phase_control_relay_step(ac_phase_control_group_t* group,
                                        ac_phase_control_channel_t* channel,
                                        uint32_t now_ms)
{
    uint8_t requested_on;
    uint16_t break_ms;
    uint16_t min_on_ms;
    uint16_t min_off_ms;

    if ((group == (ac_phase_control_group_t*)0) ||
        (channel == (ac_phase_control_channel_t*)0))
    {
        return;
    }

    break_ms = group->config.relay_break_before_make_ms;
    min_on_ms = group->config.relay_min_on_ms;
    min_off_ms = group->config.relay_min_off_ms;

    if (channel->enabled == 0u)
    {
        if (channel->relay_state == (uint8_t)AC_PHASE_RELAY_STATE_HOLD_ON)
        {
            if ((now_ms - channel->relay_on_ms) >= (uint32_t)min_on_ms)
            {
                ac_phase_control_release_relay(channel, now_ms);
            }
        }
        else if (channel->relay_state == (uint8_t)AC_PHASE_RELAY_STATE_WAIT_OFF)
        {
            if ((now_ms - channel->relay_off_ms) >= (uint32_t)break_ms)
            {
                ac_phase_control_channel_relay_off(channel);
            }
        }
        else
        {
            ac_phase_control_channel_relay_off(channel);
        }
        return;
    }

    requested_on = (channel->power_percent >= group->config.relay_on_threshold_percent) ? 1u : 0u;

    switch (channel->relay_state)
    {
        case AC_PHASE_RELAY_STATE_PHASE:
            if ((requested_on != 0u) &&
                ((now_ms - channel->relay_off_ms) >= (uint32_t)min_off_ms))
            {
                channel->relay_state = (uint8_t)AC_PHASE_RELAY_STATE_WAIT_ON;
                channel->relay_on_ms = now_ms;
            }
            break;

        case AC_PHASE_RELAY_STATE_WAIT_ON:
            if ((now_ms - channel->relay_on_ms) >= (uint32_t)break_ms)
            {
                channel->relay_state = (uint8_t)AC_PHASE_RELAY_STATE_HOLD_ON;
                channel->relay_active = 1u;
                channel->relay_on_ms = now_ms;
            }
            else if (requested_on == 0u)
            {
                channel->relay_state = (uint8_t)AC_PHASE_RELAY_STATE_PHASE;
                channel->relay_active = 0u;
            }
            break;

        case AC_PHASE_RELAY_STATE_HOLD_ON:
            if ((requested_on == 0u) &&
                ((now_ms - channel->relay_on_ms) >= (uint32_t)min_on_ms))
            {
                ac_phase_control_release_relay(channel, now_ms);
            }
            break;

        case AC_PHASE_RELAY_STATE_WAIT_OFF:
            if ((now_ms - channel->relay_off_ms) >= (uint32_t)break_ms)
            {
                ac_phase_control_channel_relay_off(channel);
            }
            break;

        default:
            ac_phase_control_channel_relay_off(channel);
            break;
    }
}

static void ac_phase_control_reload_timer(const ac_phase_control_group_t* group)
{
    if (group == (const ac_phase_control_group_t*)0)
    {
        return;
    }

    (void)group;
}

static void ac_phase_control_timer_tick(void);

static drv_status_t ac_phase_control_timer_init(ac_phase_control_group_t* group)
{
    if (group == (ac_phase_control_group_t*)0)
    {
        return DRV_STATUS_ERROR;
    }

    if (group->timer != AC_PHASE_CONTROL_TIMER2)
    {
        group->timer_tick_us = 0u;
        return DRV_STATUS_ERROR;
    }

    group->timer_tick_us = 102u;
    timer2_init(1u);
    timer2_set_callback(ac_phase_control_timer_tick);
    return DRV_STATUS_OK;
}

static void ac_phase_control_timer_start(ac_phase_control_group_t* group)
{
    if (group == (ac_phase_control_group_t*)0)
    {
        return;
    }

    timer2_start();
}

static void ac_phase_control_timer_enable_interrupt(ac_phase_control_group_t* group)
{
    if (group == (ac_phase_control_group_t*)0)
    {
        return;
    }

    timer2_enable_interrupt();
}

static void ac_phase_control_timer_disable_interrupt(ac_phase_control_group_t* group)
{
    if (group == (ac_phase_control_group_t*)0)
    {
        return;
    }

    timer2_disable_interrupt();
}

static void ac_phase_control_timer_stop(ac_phase_control_group_t* group)
{
    if (group == (ac_phase_control_group_t*)0)
    {
        return;
    }

    timer2_stop();
}

static void ac_phase_control_timer_irq_handler(const ac_phase_control_group_t* group)
{
    if (group == (const ac_phase_control_group_t*)0)
    {
        return;
    }

    timer2_irq_handler();
}

static void ac_phase_control_timer_tick(void)
{
    ac_phase_control_group_t* group;
    uint32_t accumulator;

    group = g_active_group;
    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u) ||
        (group->timer_tick_us == 0u))
    {
        return;
    }

    ac_phase_control_update_us(group, group->timer_tick_us);

    accumulator = (uint32_t)group->tick_accumulator_us + (uint32_t)group->timer_tick_us;
    while (accumulator >= 1000u)
    {
        accumulator -= 1000u;
        group->tick_ms++;
    }
    group->tick_accumulator_us = (uint16_t)accumulator;

    ac_phase_control_reload_timer(group);
}

drv_status_t ac_phase_control_init_group(ac_phase_control_group_t* group,
                                         ac_phase_control_timer_t timer,
                                         const ac_phase_control_config_t* config,
                                         ac_phase_control_channel_t* channels,
                                         uint8_t channel_count)
{
    uint8_t index;
    zero_cross_config_t zero_cross_config;
    uint32_t min_half_cycle;
    uint32_t max_half_cycle;

    if ((group == (ac_phase_control_group_t*)0) ||
        (config == (const ac_phase_control_config_t*)0) ||
        (channels == (ac_phase_control_channel_t*)0) ||
        (channel_count == 0u) ||
        (channel_count > (uint8_t)AC_PHASE_CONTROL_MAX_CHANNELS))
    {
        return DRV_STATUS_ERROR;
    }

    if (timer > AC_PHASE_CONTROL_TIMER3)
    {
        return DRV_STATUS_ERROR;
    }

    if ((config->half_cycle_us == 0u) ||
        (config->min_delay_us > config->half_cycle_us) ||
        (config->max_delay_us > config->half_cycle_us) ||
        (config->min_delay_us > config->max_delay_us) ||
        (config->gate_pulse_us == 0u))
    {
        group->status = AC_PHASE_STATUS_CONFIG_ERROR;
        return DRV_STATUS_ERROR;
    }

    group->timer = timer;
    group->config = *config;

    if (group->config.relay_on_threshold_percent == 0u)
    {
        group->config.relay_on_threshold_percent = AC_PHASE_CONTROL_DEFAULT_RELAY_ON_THRESHOLD;
    }
    if (group->config.relay_off_threshold_percent == 0u)
    {
        group->config.relay_off_threshold_percent = AC_PHASE_CONTROL_DEFAULT_RELAY_OFF_THRESHOLD;
    }
    if (group->config.relay_break_before_make_ms == 0u)
    {
        group->config.relay_break_before_make_ms = AC_PHASE_CONTROL_DEFAULT_RELAY_BREAK_MAKE_MS;
    }
    if (group->config.relay_min_on_ms == 0u)
    {
        group->config.relay_min_on_ms = AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_ON_MS;
    }
    if (group->config.relay_min_off_ms == 0u)
    {
        group->config.relay_min_off_ms = AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_OFF_MS;
    }

    if ((group->config.relay_on_threshold_percent > 100u) ||
        (group->config.relay_off_threshold_percent > 100u) ||
        (group->config.relay_on_threshold_percent <= group->config.relay_off_threshold_percent))
    {
        group->status = AC_PHASE_STATUS_CONFIG_ERROR;
        return DRV_STATUS_ERROR;
    }

    /*
     * Seed the shared zero-cross detector from the group timing config. The
     * half-cycle acceptance window tolerates a 60 Hz source (8333 us) when the
     * configured target is 50 Hz (10000 us).
     */
    min_half_cycle = ((uint32_t)config->half_cycle_us * 80u) / 100u;
    max_half_cycle = ((uint32_t)config->half_cycle_us * 125u) / 100u;

    zero_cross_config.timeout_ms = group->config.zero_cross_timeout_ms;
    zero_cross_config.min_half_cycle_us = (uint16_t)min_half_cycle;
    zero_cross_config.max_half_cycle_us = (uint16_t)max_half_cycle;
    zero_cross_config.glitch_reject_us = AC_PHASE_CONTROL_DEFAULT_GLITCH_REJECT_US;
    zero_cross_config.recovery_event_count = AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS;

    if (zero_cross_init(&group->zero_cross, &zero_cross_config) != DRV_STATUS_OK)
    {
        group->status = AC_PHASE_STATUS_CONFIG_ERROR;
        return DRV_STATUS_ERROR;
    }

    group->channels = channels;
    group->channel_count = channel_count;
    group->elapsed_us = 0u;
    group->tick_accumulator_us = 0u;
    group->tick_ms = 0u;
    group->timer_tick_us = 0u;
    group->half_cycle_active = 0u;
    group->status = AC_PHASE_STATUS_OK;
    group->initialized = 1u;

    for (index = 0u; index < channel_count; index++)
    {
        channels[index].gate_lat = (volatile uint8_t*)0;
        channels[index].gate_tris = (volatile uint8_t*)0;
        channels[index].gate_mask = 0u;
        channels[index].relay_lat = (volatile uint8_t*)0;
        channels[index].relay_tris = (volatile uint8_t*)0;
        channels[index].relay_mask = 0u;
        channels[index].delay_us = group->config.max_delay_us;
        channels[index].power_percent = 0u;
        channels[index].enabled = 0u;
        channels[index].pulse_active = 0u;
        channels[index].pulse_generated = 0u;
        channels[index].attached = 0u;
        channels[index].relay_state = (uint8_t)AC_PHASE_RELAY_STATE_PHASE;
        channels[index].relay_active = 0u;
        channels[index].relay_on_ms = 0UL;
        channels[index].relay_off_ms = 0UL;
    }

    g_active_group = group;
    if (ac_phase_control_timer_init(group) != DRV_STATUS_OK)
    {
        group->initialized = 0u;
        group->status = AC_PHASE_STATUS_CONFIG_ERROR;
        g_active_group = (ac_phase_control_group_t*)0;
        return DRV_STATUS_ERROR;
    }

    ac_phase_control_timer_enable_interrupt(group);
    ac_phase_control_reload_timer(group);
    ac_phase_control_timer_start(group);

    if (group->timer_tick_us == 0u)
    {
        group->initialized = 0u;
        group->status = AC_PHASE_STATUS_CONFIG_ERROR;
        g_active_group = (ac_phase_control_group_t*)0;
        return DRV_STATUS_ERROR;
    }

    return DRV_STATUS_OK;
}

drv_status_t ac_phase_control_attach_channel(ac_phase_control_group_t* group,
                                             uint8_t channel,
                                             volatile uint8_t* gate_lat,
                                             volatile uint8_t* gate_tris,
                                             uint8_t gate_mask)
{
    ac_phase_control_channel_t* entry;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u) ||
        (ac_phase_control_channel_valid(group, channel) == 0u) ||
        (gate_lat == (volatile uint8_t*)0) ||
        (gate_tris == (volatile uint8_t*)0) ||
        (ac_phase_control_mask_valid(gate_mask) == 0u))
    {
        return DRV_STATUS_ERROR;
    }

    entry = &group->channels[channel];
    entry->gate_lat = gate_lat;
    entry->gate_tris = gate_tris;
    entry->gate_mask = gate_mask;
    entry->delay_us = group->config.max_delay_us;
    entry->power_percent = 0u;
    entry->enabled = 0u;
    entry->pulse_active = 0u;
    entry->pulse_generated = 0u;
    entry->attached = 1u;

    *(entry->gate_tris) &= (uint8_t)(~entry->gate_mask);
    ac_phase_control_write_gate(entry, 0u);

    return DRV_STATUS_OK;
}

drv_status_t ac_phase_control_attach_channel_relay(ac_phase_control_group_t* group,
                                                   uint8_t channel,
                                                   volatile uint8_t* relay_lat,
                                                   volatile uint8_t* relay_tris,
                                                   uint8_t relay_mask)
{
    ac_phase_control_channel_t* entry;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u) ||
        (ac_phase_control_channel_valid(group, channel) == 0u) ||
        (relay_lat == (volatile uint8_t*)0) ||
        (relay_tris == (volatile uint8_t*)0) ||
        (ac_phase_control_mask_valid(relay_mask) == 0u))
    {
        return DRV_STATUS_ERROR;
    }

    entry = &group->channels[channel];
    entry->relay_lat = relay_lat;
    entry->relay_tris = relay_tris;
    entry->relay_mask = relay_mask;
    entry->relay_state = (uint8_t)AC_PHASE_RELAY_STATE_PHASE;
    entry->relay_active = 0u;
    entry->relay_on_ms = 0UL;
    entry->relay_off_ms = 0UL;

    *(entry->relay_tris) &= (uint8_t)(~entry->relay_mask);
    ac_phase_control_relay_write(entry, 0u);

    return DRV_STATUS_OK;
}

drv_status_t ac_phase_control_detach_channel(ac_phase_control_group_t* group,
                                             uint8_t channel)
{
    ac_phase_control_channel_t* entry;

    if (ac_phase_control_channel_valid(group, channel) == 0u)
    {
        return DRV_STATUS_ERROR;
    }

    entry = &group->channels[channel];
    ac_phase_control_reset_channel_cycle(entry);
    entry->relay_active = 0u;
    entry->relay_state = (uint8_t)AC_PHASE_RELAY_STATE_PHASE;
    ac_phase_control_relay_write(entry, 0u);

    entry->gate_lat = (volatile uint8_t*)0;
    entry->gate_tris = (volatile uint8_t*)0;
    entry->gate_mask = 0u;
    entry->relay_lat = (volatile uint8_t*)0;
    entry->relay_tris = (volatile uint8_t*)0;
    entry->relay_mask = 0u;
    entry->delay_us = group->config.max_delay_us;
    entry->power_percent = 0u;
    entry->enabled = 0u;
    entry->attached = 0u;

    return DRV_STATUS_OK;
}

drv_status_t ac_phase_control_set_power_percent(ac_phase_control_group_t* group,
                                                uint8_t channel,
                                                uint8_t percent)
{
    ac_phase_control_channel_t* entry;

    if (ac_phase_control_channel_valid(group, channel) == 0u)
    {
        return DRV_STATUS_ERROR;
    }

    if (percent > 100u)
    {
        percent = 100u;
    }

    entry = &group->channels[channel];
    entry->power_percent = percent;
    entry->delay_us = ac_phase_control_map_percent_to_delay(group, percent);

    if (percent == 0u)
    {
        entry->enabled = 0u;
        ac_phase_control_reset_channel_cycle(entry);
        return DRV_STATUS_OK;
    }

    entry->enabled = 1u;
    ac_phase_control_hold_until_next_zero_cross(group, entry);

    return DRV_STATUS_OK;
}

drv_status_t ac_phase_control_set_delay_us(ac_phase_control_group_t* group,
                                           uint8_t channel,
                                           uint16_t delay_us)
{
    ac_phase_control_channel_t* entry;

    if (ac_phase_control_channel_valid(group, channel) == 0u)
    {
        return DRV_STATUS_ERROR;
    }

    entry = &group->channels[channel];
    entry->delay_us = ac_phase_control_clamp_u16(delay_us,
                                                 group->config.min_delay_us,
                                                 group->config.max_delay_us);
    entry->enabled = 1u;
    entry->power_percent = ac_phase_control_map_delay_to_percent(group, entry->delay_us);
    ac_phase_control_hold_until_next_zero_cross(group, entry);

    return DRV_STATUS_OK;
}

drv_status_t ac_phase_control_enable_channel(ac_phase_control_group_t* group,
                                             uint8_t channel,
                                             uint8_t enabled)
{
    ac_phase_control_channel_t* entry;

    if (ac_phase_control_channel_valid(group, channel) == 0u)
    {
        return DRV_STATUS_ERROR;
    }

    entry = &group->channels[channel];
    entry->enabled = (enabled != 0u) ? 1u : 0u;

    if (entry->enabled == 0u)
    {
        ac_phase_control_reset_channel_cycle(entry);
        return DRV_STATUS_OK;
    }

    ac_phase_control_hold_until_next_zero_cross(group, entry);

    return DRV_STATUS_OK;
}

uint8_t ac_phase_control_get_power_percent(const ac_phase_control_group_t* group,
                                          uint8_t channel)
{
    if (ac_phase_control_channel_valid(group, channel) == 0u)
    {
        return 0u;
    }

    return group->channels[channel].power_percent;
}

uint8_t ac_phase_control_is_channel_enabled(const ac_phase_control_group_t* group,
                                            uint8_t channel)
{
    if (ac_phase_control_channel_valid(group, channel) == 0u)
    {
        return 0u;
    }

    return group->channels[channel].enabled;
}

uint8_t ac_phase_control_is_channel_in_relay_mode(const ac_phase_control_group_t* group,
                                                  uint8_t channel)
{
    if (ac_phase_control_channel_valid(group, channel) == 0u)
    {
        return 0u;
    }

    return (group->channels[channel].relay_state == (uint8_t)AC_PHASE_RELAY_STATE_HOLD_ON) ? 1u : 0u;
}

void ac_phase_control_on_zero_cross_event(ac_phase_control_group_t* group,
                                          const zero_cross_event_t* event)
{
    uint8_t channel;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u) ||
        (event == (const zero_cross_event_t*)0))
    {
        return;
    }

    /*
     * Do not arm the half-cycle before a valid sync or while the zero-cross
     * stream is LOST: recovery events are only dispatched once the detector is
     * ALIVE again, so this keeps all outputs OFF on lost sync.
     */
    if (zero_cross_is_alive(&group->zero_cross) == 0u)
    {
        return;
    }

    if (group->status == AC_PHASE_STATUS_ZERO_CROSS_LOST)
    {
        group->status = AC_PHASE_STATUS_OK;
    }

    group->elapsed_us = 0u;
    group->half_cycle_active = 1u;

    for (channel = 0u; channel < group->channel_count; channel++)
    {
        group->channels[channel].pulse_active = 0u;
        group->channels[channel].pulse_generated = 0u;
    }

    ac_phase_control_apply_group_outputs(group);
}

void ac_phase_control_on_zero_cross(ac_phase_control_group_t* group)
{
    zero_cross_event_t event;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return;
    }

    if (zero_cross_on_edge(&group->zero_cross,
                           ac_phase_control_now_us(group),
                           &event) != 0u)
    {
        ac_phase_control_on_zero_cross_event(group, &event);
    }
}

void ac_phase_control_update_us(ac_phase_control_group_t* group,
                                uint16_t elapsed_us)
{
    uint8_t channel;
    uint32_t total_elapsed;
    uint32_t pulse_elapsed;
    ac_phase_control_channel_t* entry;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u) ||
        (group->half_cycle_active == 0u) ||
        (elapsed_us == 0u))
    {
        return;
    }

    total_elapsed = (uint32_t)group->elapsed_us + (uint32_t)elapsed_us;

    if (total_elapsed >= (uint32_t)group->config.half_cycle_us)
    {
        for (channel = 0u; channel < group->channel_count; channel++)
        {
            group->channels[channel].pulse_active = 0u;
            group->channels[channel].pulse_generated = 0u;
        }

        ac_phase_control_apply_group_outputs(group);

        group->elapsed_us = 0u;
        group->half_cycle_active = 0u;
        return;
    }

    group->elapsed_us = (uint16_t)total_elapsed;

    for (channel = 0u; channel < group->channel_count; channel++)
    {
        entry = &group->channels[channel];

        if ((entry->attached == 0u) ||
            (entry->enabled == 0u) ||
            (entry->power_percent == 0u))
        {
            entry->pulse_active = 0u;
            continue;
        }

        if (entry->relay_state != (uint8_t)AC_PHASE_RELAY_STATE_PHASE)
        {
            entry->pulse_active = 0u;
            entry->pulse_generated = 1u;
            continue;
        }

        if ((entry->pulse_generated == 0u) &&
            (total_elapsed >= (uint32_t)entry->delay_us))
        {
            entry->pulse_generated = 1u;
            entry->pulse_active = 1u;
        }

        if (entry->pulse_active != 0u)
        {
            pulse_elapsed = total_elapsed - (uint32_t)entry->delay_us;

            if (pulse_elapsed >= (uint32_t)group->config.gate_pulse_us)
            {
                entry->pulse_active = 0u;
            }
        }
    }

    /*
     * Commit all channel states with one masked write per LAT register.
     */
    ac_phase_control_apply_group_outputs(group);
}

void ac_phase_control_process(ac_phase_control_group_t* group)
{
    uint8_t channel;
    uint32_t now_ms;
    uint32_t now_us;
    ac_phase_control_channel_t* entry;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return;
    }

    now_ms = group->tick_ms;
    now_us = ac_phase_control_now_us(group);

    zero_cross_process(&group->zero_cross, now_us);

    if (zero_cross_get_status(&group->zero_cross) == ZERO_CROSS_STATUS_LOST)
    {
        ac_phase_control_enter_fault(group);
        return;
    }

    for (channel = 0u; channel < group->channel_count; channel++)
    {
        entry = &group->channels[channel];

        if (entry->attached == 0u)
        {
            continue;
        }

        if ((entry->relay_lat == (volatile uint8_t*)0) ||
            (entry->relay_tris == (volatile uint8_t*)0) ||
            (entry->relay_mask == 0u))
        {
            ac_phase_control_channel_relay_off(entry);
            continue;
        }

        ac_phase_control_relay_step(group, entry, now_ms);
    }

    ac_phase_control_apply_group_outputs(group);
}

void ac_phase_control_all_off(ac_phase_control_group_t* group)
{
    uint8_t channel;
    ac_phase_control_channel_t* entry;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return;
    }

    for (channel = 0u; channel < group->channel_count; channel++)
    {
        entry = &group->channels[channel];

        if (entry->attached != 0u)
        {
            entry->enabled = 0u;
            entry->pulse_active = 0u;
            entry->pulse_generated = 0u;
            ac_phase_control_channel_relay_off(entry);
        }
    }

    group->elapsed_us = 0u;
    group->half_cycle_active = 0u;

    ac_phase_control_apply_group_outputs(group);
}

void ac_phase_control_stop_group(ac_phase_control_group_t* group)
{
    uint8_t channel;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return;
    }

    for (channel = 0u; channel < group->channel_count; channel++)
    {
        if (group->channels[channel].attached != 0u)
        {
            group->channels[channel].enabled = 0u;
            group->channels[channel].power_percent = 0u;
            group->channels[channel].pulse_active = 0u;
            group->channels[channel].pulse_generated = 0u;
            ac_phase_control_channel_relay_off(&group->channels[channel]);
        }
    }

    ac_phase_control_apply_group_outputs(group);

    ac_phase_control_timer_disable_interrupt(group);
    ac_phase_control_timer_stop(group);

    if (g_active_group == group)
    {
        g_active_group = (ac_phase_control_group_t*)0;
    }

    group->elapsed_us = 0u;
    group->half_cycle_active = 0u;
}

void ac_phase_control_irq_handler(void)
{
    ac_phase_control_timer_irq_handler(g_active_group);
}

uint32_t ac_phase_control_get_tick_ms(const ac_phase_control_group_t* group)
{
    if (group == (const ac_phase_control_group_t*)0)
    {
        return 0UL;
    }

    return group->tick_ms;
}

uint8_t ac_phase_control_is_any_channel_active(const ac_phase_control_group_t* group)
{
    uint8_t channel;

    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return 0u;
    }

    for (channel = 0u; channel < group->channel_count; channel++)
    {
        if (group->channels[channel].pulse_active != 0u)
        {
            return 1u;
        }
    }

    return 0u;
}

ac_phase_status_t ac_phase_control_get_status(const ac_phase_control_group_t* group)
{
    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return AC_PHASE_STATUS_NOT_INITIALIZED;
    }

    return group->status;
}

uint8_t ac_phase_control_is_zero_cross_alive(const ac_phase_control_group_t* group)
{
    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return 0u;
    }

    return zero_cross_is_alive(&group->zero_cross);
}
