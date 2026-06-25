/*
 * File: libraries/system/servo/servo.h
 */

#ifndef LIBRARIES_SYSTEM_SERVO_H
#define LIBRARIES_SYSTEM_SERVO_H

#include "core/compiler.h"
#include "core/types.h"

typedef struct
{
    uint16_t pulse_min_us;
    uint16_t pulse_max_us;
    uint16_t period_us;
    uint16_t angle_min_deg;
    uint16_t angle_max_deg;
} servo_config_t;

typedef void (*servo_pwm_apply_cb_t)(void* ctx, uint16_t period_us, uint16_t pulse_us);
typedef void (*servo_pwm_enable_cb_t)(void* ctx);
typedef void (*servo_pwm_disable_cb_t)(void* ctx);

typedef struct
{
    servo_config_t cfg;

    uint16_t current_angle_deg;
    uint16_t target_angle_deg;
    uint16_t current_pulse_us;

    uint8_t enabled;
    uint8_t smooth_active;
    uint8_t moving;

    uint16_t speed_deg_per_step;
    uint32_t step_interval_ms;
    uint32_t last_step_ms;

    servo_pwm_apply_cb_t pwm_apply_cb;
    servo_pwm_enable_cb_t pwm_enable_cb;
    servo_pwm_disable_cb_t pwm_disable_cb;
    void* pwm_ctx;
} servo_t;

uint8_t servo_init(servo_t* servo, const servo_config_t* cfg);

void servo_set_backend(servo_t* servo,
                       servo_pwm_apply_cb_t apply_cb,
                       servo_pwm_enable_cb_t enable_cb,
                       servo_pwm_disable_cb_t disable_cb,
                       void* ctx);

void servo_enable(servo_t* servo);
void servo_disable(servo_t* servo);

void servo_set_angle(servo_t* servo, uint16_t angle_deg);
uint16_t servo_get_angle(const servo_t* servo);

void servo_set_pulse_us(servo_t* servo, uint16_t pulse_us);

void servo_move_smooth(servo_t* servo,
                       uint16_t target_angle_deg,
                       uint16_t speed_deg_per_step,
                       uint32_t step_interval_ms,
                       uint32_t now_ms);

void servo_update(servo_t* servo, uint32_t now_ms);
uint8_t servo_is_moving(const servo_t* servo);

#endif /* LIBRARIES_SYSTEM_SERVO_H */
