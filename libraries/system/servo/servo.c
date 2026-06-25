/*
 * File: libraries/system/servo/servo.c
 */

#include "libraries/system/servo/servo.h"

static uint16_t servo_clamp_u16(uint16_t v, uint16_t lo, uint16_t hi)
{
    if (v < lo)
    {
        return lo;
    }
    if (v > hi)
    {
        return hi;
    }
    return v;
}

static uint16_t servo_angle_to_pulse(const servo_t* servo, uint16_t angle_deg)
{
    uint32_t num;
    uint32_t den;
    uint32_t pulse;

    if (servo == (const servo_t*)0)
    {
        return 1500u;
    }

    angle_deg = servo_clamp_u16(angle_deg, servo->cfg.angle_min_deg, servo->cfg.angle_max_deg);

    den = (uint32_t)(servo->cfg.angle_max_deg - servo->cfg.angle_min_deg);
    if (den == 0u)
    {
        return servo->cfg.pulse_min_us;
    }

    num = (uint32_t)(angle_deg - servo->cfg.angle_min_deg) *
          (uint32_t)(servo->cfg.pulse_max_us - servo->cfg.pulse_min_us);

    pulse = (uint32_t)servo->cfg.pulse_min_us + (num / den);
    return (uint16_t)pulse;
}

static uint16_t servo_pulse_to_angle(const servo_t* servo, uint16_t pulse_us)
{
    uint32_t num;
    uint32_t den;
    uint32_t angle;

    if (servo == (const servo_t*)0)
    {
        return 0u;
    }

    pulse_us = servo_clamp_u16(pulse_us, servo->cfg.pulse_min_us, servo->cfg.pulse_max_us);

    den = (uint32_t)(servo->cfg.pulse_max_us - servo->cfg.pulse_min_us);
    if (den == 0u)
    {
        return servo->cfg.angle_min_deg;
    }

    num = (uint32_t)(pulse_us - servo->cfg.pulse_min_us) *
          (uint32_t)(servo->cfg.angle_max_deg - servo->cfg.angle_min_deg);

    angle = (uint32_t)servo->cfg.angle_min_deg + (num / den);
    return (uint16_t)angle;
}

static void servo_apply_pwm(servo_t* servo)
{
    if ((servo == (servo_t*)0) || (servo->pwm_apply_cb == (servo_pwm_apply_cb_t)0))
    {
        return;
    }

    servo->pwm_apply_cb(servo->pwm_ctx, servo->cfg.period_us, servo->current_pulse_us);
}

uint8_t servo_init(servo_t* servo, const servo_config_t* cfg)
{
    servo_config_t local_cfg;

    if (servo == (servo_t*)0)
    {
        return 0u;
    }

    if (cfg == (const servo_config_t*)0)
    {
        local_cfg.pulse_min_us = 1000u;
        local_cfg.pulse_max_us = 2000u;
        local_cfg.period_us = 20000u;
        local_cfg.angle_min_deg = 0u;
        local_cfg.angle_max_deg = 180u;
        cfg = &local_cfg;
    }

    servo->cfg = *cfg;

    if (servo->cfg.pulse_min_us >= servo->cfg.pulse_max_us)
    {
        servo->cfg.pulse_min_us = 1000u;
        servo->cfg.pulse_max_us = 2000u;
    }
    if (servo->cfg.angle_min_deg >= servo->cfg.angle_max_deg)
    {
        servo->cfg.angle_min_deg = 0u;
        servo->cfg.angle_max_deg = 180u;
    }
    if (servo->cfg.period_us == 0u)
    {
        servo->cfg.period_us = 20000u;
    }

    servo->current_angle_deg = (uint16_t)((servo->cfg.angle_min_deg + servo->cfg.angle_max_deg) / 2u);
    servo->target_angle_deg = servo->current_angle_deg;
    servo->current_pulse_us = servo_angle_to_pulse(servo, servo->current_angle_deg);

    servo->enabled = 0u;
    servo->smooth_active = 0u;
    servo->moving = 0u;

    servo->speed_deg_per_step = 1u;
    servo->step_interval_ms = 20u;
    servo->last_step_ms = 0u;

    servo->pwm_apply_cb = (servo_pwm_apply_cb_t)0;
    servo->pwm_enable_cb = (servo_pwm_enable_cb_t)0;
    servo->pwm_disable_cb = (servo_pwm_disable_cb_t)0;
    servo->pwm_ctx = (void*)0;

    return 1u;
}

void servo_set_backend(servo_t* servo,
                       servo_pwm_apply_cb_t apply_cb,
                       servo_pwm_enable_cb_t enable_cb,
                       servo_pwm_disable_cb_t disable_cb,
                       void* ctx)
{
    if (servo == (servo_t*)0)
    {
        return;
    }

    servo->pwm_apply_cb = apply_cb;
    servo->pwm_enable_cb = enable_cb;
    servo->pwm_disable_cb = disable_cb;
    servo->pwm_ctx = ctx;
}

void servo_enable(servo_t* servo)
{
    if (servo == (servo_t*)0)
    {
        return;
    }

    servo->enabled = 1u;
    if (servo->pwm_enable_cb != (servo_pwm_enable_cb_t)0)
    {
        servo->pwm_enable_cb(servo->pwm_ctx);
    }
    servo_apply_pwm(servo);
}

void servo_disable(servo_t* servo)
{
    if (servo == (servo_t*)0)
    {
        return;
    }

    servo->enabled = 0u;
    servo->smooth_active = 0u;
    servo->moving = 0u;

    if (servo->pwm_disable_cb != (servo_pwm_disable_cb_t)0)
    {
        servo->pwm_disable_cb(servo->pwm_ctx);
    }
}

void servo_set_angle(servo_t* servo, uint16_t angle_deg)
{
    if (servo == (servo_t*)0)
    {
        return;
    }

    servo->smooth_active = 0u;
    servo->moving = 0u;

    servo->current_angle_deg = servo_clamp_u16(angle_deg, servo->cfg.angle_min_deg, servo->cfg.angle_max_deg);
    servo->target_angle_deg = servo->current_angle_deg;
    servo->current_pulse_us = servo_angle_to_pulse(servo, servo->current_angle_deg);

    if (servo->enabled != 0u)
    {
        servo_apply_pwm(servo);
    }
}

uint16_t servo_get_angle(const servo_t* servo)
{
    if (servo == (const servo_t*)0)
    {
        return 0u;
    }

    return servo->current_angle_deg;
}

void servo_set_pulse_us(servo_t* servo, uint16_t pulse_us)
{
    if (servo == (servo_t*)0)
    {
        return;
    }

    servo->smooth_active = 0u;
    servo->moving = 0u;

    servo->current_pulse_us = servo_clamp_u16(pulse_us, servo->cfg.pulse_min_us, servo->cfg.pulse_max_us);
    servo->current_angle_deg = servo_pulse_to_angle(servo, servo->current_pulse_us);
    servo->target_angle_deg = servo->current_angle_deg;

    if (servo->enabled != 0u)
    {
        servo_apply_pwm(servo);
    }
}

void servo_move_smooth(servo_t* servo,
                       uint16_t target_angle_deg,
                       uint16_t speed_deg_per_step,
                       uint32_t step_interval_ms,
                       uint32_t now_ms)
{
    if (servo == (servo_t*)0)
    {
        return;
    }

    servo->target_angle_deg = servo_clamp_u16(target_angle_deg, servo->cfg.angle_min_deg, servo->cfg.angle_max_deg);
    servo->speed_deg_per_step = (uint16_t)(speed_deg_per_step == 0u ? 1u : speed_deg_per_step);
    servo->step_interval_ms = (uint32_t)(step_interval_ms == 0u ? 1u : step_interval_ms);
    servo->last_step_ms = now_ms;

    servo->smooth_active = 1u;
    servo->moving = (uint8_t)(servo->current_angle_deg != servo->target_angle_deg ? 1u : 0u);
}

void servo_update(servo_t* servo, uint32_t now_ms)
{
    uint16_t step;
    uint16_t next;

    if ((servo == (servo_t*)0) || (servo->enabled == 0u))
    {
        return;
    }

    if ((servo->smooth_active == 0u) || (servo->moving == 0u))
    {
        return;
    }

    if ((uint32_t)(now_ms - servo->last_step_ms) < servo->step_interval_ms)
    {
        return;
    }

    servo->last_step_ms = now_ms;

    step = servo->speed_deg_per_step;

    if (servo->current_angle_deg < servo->target_angle_deg)
    {
        next = (uint16_t)(servo->current_angle_deg + step);
        if (next > servo->target_angle_deg)
        {
            next = servo->target_angle_deg;
        }
    }
    else
    {
        if (servo->current_angle_deg > step)
        {
            next = (uint16_t)(servo->current_angle_deg - step);
        }
        else
        {
            next = 0u;
        }

        if (next < servo->target_angle_deg)
        {
            next = servo->target_angle_deg;
        }
    }

    servo->current_angle_deg = next;
    servo->current_pulse_us = servo_angle_to_pulse(servo, servo->current_angle_deg);
    servo_apply_pwm(servo);

    if (servo->current_angle_deg == servo->target_angle_deg)
    {
        servo->moving = 0u;
        servo->smooth_active = 0u;
    }
}

uint8_t servo_is_moving(const servo_t* servo)
{
    if (servo == (const servo_t*)0)
    {
        return 0u;
    }

    return servo->moving;
}
