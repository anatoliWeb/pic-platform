/*
 * File: drivers/timers/pwm/pwm.h
 */

#ifndef DRIVERS_PWM_PWM_H
#define DRIVERS_PWM_PWM_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    PWM_CHANNEL_1 = 0,
    PWM_CHANNEL_2
} pwm_channel_t;

void pwm_init(uint32_t frequency);

void pwm_start(pwm_channel_t channel);
void pwm_stop(pwm_channel_t channel);

void pwm_set_duty(pwm_channel_t channel, uint16_t duty);

#endif /* DRIVERS_PWM_PWM_H */
