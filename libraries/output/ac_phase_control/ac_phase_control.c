#include "libraries/output/ac_phase_control/ac_phase_control.h"

#include "drivers/timers/timer2/timer2.h"

static ac_phase_control_group_t* g_active_group = (ac_phase_control_group_t*)0;

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


static void ac_phase_control_apply_group_outputs(
    const ac_phase_control_group_t* group)
{
    uint8_t channel;
    uint8_t scan;
    uint8_t already_processed;
    uint8_t controlled_mask;
    uint8_t high_mask;
    uint8_t lat_value;
    volatile uint8_t* gate_lat;
    const ac_phase_control_channel_t* entry;

    if ((group == (const ac_phase_control_group_t*)0) ||
        (group->channels == (ac_phase_control_channel_t*)0))
    {
        return;
    }

    /*
     * Update every physical LAT register only once.
     *
     * Several logical phase-control channels may share the same LATx
     * register. Building one combined mask prevents one channel update
     * from disturbing another channel on the same port.
     */
    for (channel = 0u; channel < group->channel_count; channel++)
    {
        entry = &group->channels[channel];

        if ((entry->attached == 0u) ||
            (entry->gate_lat == (volatile uint8_t*)0) ||
            (entry->gate_mask == 0u))
        {
            continue;
        }

        gate_lat = entry->gate_lat;
        already_processed = 0u;

        for (scan = 0u; scan < channel; scan++)
        {
            if ((group->channels[scan].attached != 0u) &&
                (group->channels[scan].gate_lat == gate_lat))
            {
                already_processed = 1u;
                break;
            }
        }

        if (already_processed != 0u)
        {
            continue;
        }

        controlled_mask = 0u;
        high_mask = 0u;

        for (scan = channel; scan < group->channel_count; scan++)
        {
            entry = &group->channels[scan];

            if ((entry->attached == 0u) ||
                (entry->gate_lat != gate_lat))
            {
                continue;
            }

            controlled_mask |= entry->gate_mask;

            if ((entry->enabled != 0u) &&
                (entry->pulse_active != 0u))
            {
                high_mask |= entry->gate_mask;
            }
        }

        /*
         * Preserve unrelated pins on the same port and replace only
         * the bits owned by this phase-control group.
         */
        lat_value = *gate_lat;
        lat_value &= (uint8_t)(~controlled_mask);
        lat_value |= high_mask;
        *gate_lat = lat_value;
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

    if ((group == (ac_phase_control_group_t*)0) ||
        (config == (const ac_phase_control_config_t*)0) ||
        (channels == (ac_phase_control_channel_t*)0) ||
        (channel_count == 0u) ||
        (channel_count > (uint8_t)AC_PHASE_CONTROL_MAX_CHANNELS))
    {
        return DRV_STATUS_ERROR;
    }

    if ((timer < AC_PHASE_CONTROL_TIMER0) ||
        (timer > AC_PHASE_CONTROL_TIMER3))
    {
        return DRV_STATUS_ERROR;
    }

    if ((config->half_cycle_us == 0u) ||
        (config->min_delay_us > config->half_cycle_us) ||
        (config->max_delay_us > config->half_cycle_us) ||
        (config->min_delay_us > config->max_delay_us) ||
        (config->gate_pulse_us == 0u))
    {
        return DRV_STATUS_ERROR;
    }

    group->timer = timer;
    group->config = *config;
    group->channels = channels;
    group->channel_count = channel_count;
    group->elapsed_us = 0u;
    group->tick_accumulator_us = 0u;
    group->tick_ms = 0u;
    group->timer_tick_us = 0u;
    group->half_cycle_active = 0u;
    group->initialized = 1u;

    for (index = 0u; index < channel_count; index++)
    {
        channels[index].gate_lat = (volatile uint8_t*)0;
        channels[index].gate_tris = (volatile uint8_t*)0;
        channels[index].gate_mask = 0u;
        channels[index].delay_us = group->config.max_delay_us;
        channels[index].power_percent = 0u;
        channels[index].enabled = 0u;
        channels[index].pulse_active = 0u;
        channels[index].pulse_generated = 0u;
        channels[index].attached = 0u;
    }

    g_active_group = group;
    if (ac_phase_control_timer_init(group) != DRV_STATUS_OK)
    {
        group->initialized = 0u;
        g_active_group = (ac_phase_control_group_t*)0;
        return DRV_STATUS_ERROR;
    }

    ac_phase_control_timer_enable_interrupt(group);
    ac_phase_control_reload_timer(group);
    ac_phase_control_timer_start(group);

    if (group->timer_tick_us == 0u)
    {
        group->initialized = 0u;
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
    entry->gate_lat = (volatile uint8_t*)0;
    entry->gate_tris = (volatile uint8_t*)0;
    entry->gate_mask = 0u;
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

void ac_phase_control_on_zero_cross(ac_phase_control_group_t* group)
{
    uint8_t channel;

    if ((group == (ac_phase_control_group_t*)0) ||
        (group->initialized == 0u))
    {
        return;
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
