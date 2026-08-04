/*
 * File: libraries/output/ac_phase_control/ac_phase_control.h
 *
 * Multi-channel zero-cross synchronized phase-angle control with optional
 * per-channel relay-bypass for full-power mode.
 *
 * One shared zero-cross event starts a half-cycle and drives all channels.
 * One shared timer performs the gate-pulse scheduling. Relay transitions are
 * handled by a non-blocking state machine advanced from ac_phase_control_process().
 *
 * Zero-cross detection, timeout and recovery live in the reusable
 * libraries/input/zero_cross library. The group owns one zero_cross_t instance
 * as its shared sync domain; the caller feeds edges and dispatches the produced
 * event to this group (and optionally to other consumers).
 */

#ifndef LIBRARIES_OUTPUT_AC_PHASE_CONTROL_H
#define LIBRARIES_OUTPUT_AC_PHASE_CONTROL_H

#include "core/compiler.h"
#include "core/types.h"
#include "libraries/input/zero_cross/zero_cross.h"

#ifndef AC_PHASE_CONTROL_MAX_CHANNELS
#define AC_PHASE_CONTROL_MAX_CHANNELS 4U
#endif

#define AC_PHASE_CONTROL_DEFAULT_RELAY_ON_THRESHOLD   98U
#define AC_PHASE_CONTROL_DEFAULT_RELAY_OFF_THRESHOLD  96U
#define AC_PHASE_CONTROL_DEFAULT_RELAY_BREAK_MAKE_MS  50U
#define AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_ON_MS      200U
#define AC_PHASE_CONTROL_DEFAULT_RELAY_MIN_OFF_MS     200U

#define AC_PHASE_CONTROL_ZERO_CROSS_RECOVERY_EVENTS   2U

#define AC_PHASE_CONTROL_DEFAULT_GLITCH_REJECT_US     500U

typedef enum
{
    AC_PHASE_CONTROL_TIMER0 = 0,
    AC_PHASE_CONTROL_TIMER1,
    AC_PHASE_CONTROL_TIMER2,
    AC_PHASE_CONTROL_TIMER3
} ac_phase_control_timer_t;

typedef enum
{
    AC_PHASE_STATUS_OK = 0,
    AC_PHASE_STATUS_NOT_INITIALIZED,
    AC_PHASE_STATUS_CONFIG_ERROR,
    AC_PHASE_STATUS_ZERO_CROSS_LOST
} ac_phase_status_t;

typedef enum
{
    AC_PHASE_RELAY_STATE_PHASE = 0,
    AC_PHASE_RELAY_STATE_WAIT_ON,
    AC_PHASE_RELAY_STATE_HOLD_ON,
    AC_PHASE_RELAY_STATE_WAIT_OFF
} ac_phase_relay_state_t;

typedef struct
{
    uint16_t half_cycle_us;
    uint16_t min_delay_us;
    uint16_t max_delay_us;
    uint16_t gate_pulse_us;
    uint8_t relay_on_threshold_percent;
    uint8_t relay_off_threshold_percent;
    uint16_t relay_break_before_make_ms;
    uint16_t relay_min_on_ms;
    uint16_t relay_min_off_ms;
    uint16_t zero_cross_timeout_ms;
} ac_phase_control_config_t;

typedef struct
{
    volatile uint8_t* gate_lat;
    volatile uint8_t* gate_tris;
    uint8_t gate_mask;
    volatile uint8_t* relay_lat;
    volatile uint8_t* relay_tris;
    uint8_t relay_mask;
    uint16_t delay_us;
    uint8_t power_percent;
    uint8_t enabled;
    uint8_t pulse_active;
    uint8_t pulse_generated;
    uint8_t attached;
    uint8_t relay_state;
    uint8_t relay_active;
    uint32_t relay_on_ms;
    uint32_t relay_off_ms;
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
    zero_cross_t zero_cross;
    ac_phase_status_t status;
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

/* Optional relay bypass output. Channel must be attached with a gate first. */
drv_status_t ac_phase_control_attach_channel_relay(ac_phase_control_group_t* group,
                                                   uint8_t channel,
                                                   volatile uint8_t* relay_lat,
                                                   volatile uint8_t* relay_tris,
                                                   uint8_t relay_mask);

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
uint8_t ac_phase_control_is_channel_in_relay_mode(const ac_phase_control_group_t* group,
                                                  uint8_t channel);

void ac_phase_control_on_zero_cross(ac_phase_control_group_t* group);
void ac_phase_control_on_zero_cross_event(ac_phase_control_group_t* group,
                                          const zero_cross_event_t* event);
void ac_phase_control_update_us(ac_phase_control_group_t* group,
                                uint16_t elapsed_us);
void ac_phase_control_process(ac_phase_control_group_t* group);
void ac_phase_control_all_off(ac_phase_control_group_t* group);
void ac_phase_control_stop_group(ac_phase_control_group_t* group);

void ac_phase_control_irq_handler(void);
uint32_t ac_phase_control_get_tick_ms(const ac_phase_control_group_t* group);
uint8_t ac_phase_control_is_any_channel_active(const ac_phase_control_group_t* group);

ac_phase_status_t ac_phase_control_get_status(const ac_phase_control_group_t* group);
uint8_t ac_phase_control_is_zero_cross_alive(const ac_phase_control_group_t* group);

#endif /* LIBRARIES_OUTPUT_AC_PHASE_CONTROL_H */
