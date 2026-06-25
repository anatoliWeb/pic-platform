/*
 * File: libraries/output/software_pwm/software_pwm.c
 */

#include "libraries/output/software_pwm/software_pwm.h"

static void software_pwm_write_channel(const software_pwm_channel_t* channel,
                                       uint8_t high)
{
    if ((channel == (const software_pwm_channel_t*)0) ||
        (channel->port == (volatile uint8_t*)0))
    {
        return;
    }

    if (high != 0u)
    {
        *(channel->port) |= channel->bit_mask;
    }
    else
    {
        *(channel->port) &= (uint8_t)(~channel->bit_mask);
    }
}

static void software_pwm_configure_channel(const software_pwm_channel_t* channel)
{
    if ((channel == (const software_pwm_channel_t*)0) ||
        (channel->tris == (volatile uint8_t*)0))
    {
        return;
    }

    *(channel->tris) &= (uint8_t)(~channel->bit_mask);
}

static uint16_t software_pwm_clamp_duty(const software_pwm_group_t* group,
                                        uint16_t duty)
{
    if (group == (const software_pwm_group_t*)0)
    {
        return 0u;
    }

    if (duty >= group->resolution)
    {
        return group->resolution;
    }

    return duty;
}

static void software_pwm_apply_channel(const software_pwm_group_t* group,
                                       software_pwm_channel_t* channel,
                                       uint16_t phase)
{
    if ((group == (const software_pwm_group_t*)0) ||
        (channel == (software_pwm_channel_t*)0))
    {
        return;
    }

    if ((channel->enabled == 0u) || (channel->duty == 0u))
    {
        software_pwm_write_channel(channel, 0u);
        return;
    }

    if (channel->duty >= group->resolution)
    {
        software_pwm_write_channel(channel, 1u);
        return;
    }

    if (phase < channel->duty)
    {
        software_pwm_write_channel(channel, 1u);
    }
    else
    {
        software_pwm_write_channel(channel, 0u);
    }
}

void software_pwm_init_group(software_pwm_group_t* group,
                             software_pwm_timer_t timer,
                             uint16_t resolution,
                             software_pwm_channel_t* channels,
                             uint8_t channel_count)
{
    uint8_t index;

    if (group == (software_pwm_group_t*)0)
    {
        return;
    }

    group->timer = timer;
    group->resolution = (resolution == 0u) ? 1u : resolution;
    group->counter = 0u;
    group->channels = channels;
    group->channel_count = channel_count;

    for (index = 0u; index < channel_count; index++)
    {
        software_pwm_configure_channel(&channels[index]);
        channels[index].duty = software_pwm_clamp_duty(group, channels[index].duty);
        software_pwm_apply_channel(group, &channels[index], 0u);
    }
}

void software_pwm_set_duty(software_pwm_group_t* group,
                           uint8_t channel_index,
                           uint16_t duty)
{
    if ((group == (software_pwm_group_t*)0) ||
        (group->channels == (software_pwm_channel_t*)0) ||
        (channel_index >= group->channel_count))
    {
        return;
    }

    group->channels[channel_index].duty = software_pwm_clamp_duty(group, duty);
    software_pwm_apply_channel(group,
                               &group->channels[channel_index],
                               group->counter);
}

void software_pwm_enable_channel(software_pwm_group_t* group,
                                 uint8_t channel_index,
                                 uint8_t enabled)
{
    if ((group == (software_pwm_group_t*)0) ||
        (group->channels == (software_pwm_channel_t*)0) ||
        (channel_index >= group->channel_count))
    {
        return;
    }

    group->channels[channel_index].enabled = (enabled != 0u) ? 1u : 0u;
    software_pwm_apply_channel(group,
                               &group->channels[channel_index],
                               group->counter);
}

void software_pwm_tick(software_pwm_group_t* group)
{
    uint8_t index;
    uint16_t phase;

    if ((group == (software_pwm_group_t*)0) ||
        (group->channels == (software_pwm_channel_t*)0) ||
        (group->channel_count == 0u))
    {
        return;
    }

    phase = group->counter;

    for (index = 0u; index < group->channel_count; index++)
    {
        software_pwm_apply_channel(group, &group->channels[index], phase);
    }

    phase++;
    if (phase >= group->resolution)
    {
        phase = 0u;
    }

    group->counter = phase;
}
