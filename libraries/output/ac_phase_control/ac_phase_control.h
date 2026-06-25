/*
 * File: libraries/output/ac_phase_control/ac_phase_control.h
 */

#ifndef LIBRARIES_OUTPUT_AC_PHASE_CONTROL_H
#define LIBRARIES_OUTPUT_AC_PHASE_CONTROL_H

#include "core/compiler.h"
#include "core/types.h"

#ifndef AC_PHASE_CONTROL_MAX_CHANNELS
#define AC_PHASE_CONTROL_MAX_CHANNELS 4U
#endif

typedef enum
{
    AC_PHASE_CONTROL_TIMER0 = 0,
    AC_PHASE_CONTROL_TIMER1,
    AC_PHASE_CONTROL_TIMER2,
    AC_PHASE_CONTROL_TIMER3
} ac_phase_control_timer_t;

typedef struct
{
    uint16_t half_cycle_us;
    uint16_t min_delay_us;
    uint16_t max_delay_us;
    uint16_t gate_pulse_us;
} ac_phase_control_config_t;

typedef struct
{
    volatile uint8_t* gate_lat;
    volatile uint8_t* gate_tris;
    uint8_t gate_mask;
    uint16_t delay_us;
    uint8_t power_percent;
    uint8_t enabled;
    uint8_t pulse_active;
    uint8_t pulse_generated;
    uint8_t attached;
} ac_phase_control_channel_t;

typedef struct
{
    ac_phase_control_timer_t timer;
    ac_phase_control_config_t config;
    ac_phase_control_channel_t* channels;
    uint8_t channel_count;
    uint16_t elapsed_us;
    uint16_t tick_accumulator_us;
    uint32_t tick_ms;
    uint16_t timer_tick_us;
    uint8_t half_cycle_active;
    uint8_t initialized;
} ac_phase_control_group_t;

drv_status_t ac_phase_control_init_group(ac_phase_control_group_t* group,
                                         ac_phase_control_timer_t timer,
                                         const ac_phase_control_config_t* config,
                                         ac_phase_control_channel_t* channels,
                                         uint8_t channel_count);

/* Bit mask example: (1U << 0U) for RD0. */
drv_status_t ac_phase_control_attach_channel(ac_phase_control_group_t* group,
                                             uint8_t channel,
                                             volatile uint8_t* gate_lat,
                                             volatile uint8_t* gate_tris,
                                             uint8_t gate_mask);

drv_status_t ac_phase_control_detach_channel(ac_phase_control_group_t* group,
                                             uint8_t channel);

drv_status_t ac_phase_control_set_power_percent(ac_phase_control_group_t* group,
                                                uint8_t channel,
                                                uint8_t percent);

drv_status_t ac_phase_control_set_delay_us(ac_phase_control_group_t* group,
                                           uint8_t channel,
                                           uint16_t delay_us);

drv_status_t ac_phase_control_enable_channel(ac_phase_control_group_t* group,
                                             uint8_t channel,
                                             uint8_t enabled);

uint8_t ac_phase_control_get_power_percent(const ac_phase_control_group_t* group,
                                          uint8_t channel);
uint8_t ac_phase_control_is_channel_enabled(const ac_phase_control_group_t* group,
                                            uint8_t channel);

void ac_phase_control_on_zero_cross(ac_phase_control_group_t* group);
void ac_phase_control_update_us(ac_phase_control_group_t* group,
                                uint16_t elapsed_us);
void ac_phase_control_stop_group(ac_phase_control_group_t* group);

void ac_phase_control_irq_handler(void);
uint32_t ac_phase_control_get_tick_ms(const ac_phase_control_group_t* group);
uint8_t ac_phase_control_is_any_channel_active(const ac_phase_control_group_t* group);

#endif /* LIBRARIES_OUTPUT_AC_PHASE_CONTROL_H */
