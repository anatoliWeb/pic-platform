#ifndef LIBRARIES_OUTPUT_SOFTWARE_PWM_H
#define LIBRARIES_OUTPUT_SOFTWARE_PWM_H

#include "core/compiler.h"
#include "core/types.h"

typedef enum
{
    SOFTWARE_PWM_TIMER0 = 0,
    SOFTWARE_PWM_TIMER1,
    SOFTWARE_PWM_TIMER2,
    SOFTWARE_PWM_TIMER3
} software_pwm_timer_t;

typedef struct
{
    volatile uint8_t* port;
    volatile uint8_t* tris;
    uint8_t bit_mask;
    uint16_t duty;
    uint8_t enabled;
} software_pwm_channel_t;

typedef struct
{
    software_pwm_timer_t timer;
    uint16_t resolution;
    uint16_t counter;
    software_pwm_channel_t* channels;
    uint8_t channel_count;
} software_pwm_group_t;

void software_pwm_init_group(software_pwm_group_t* group,
                             software_pwm_timer_t timer,
                             uint16_t resolution,
                             software_pwm_channel_t* channels,
                             uint8_t channel_count);

void software_pwm_set_duty(software_pwm_group_t* group,
                           uint8_t channel_index,
                           uint16_t duty);

void software_pwm_enable_channel(software_pwm_group_t* group,
                                 uint8_t channel_index,
                                 uint8_t enabled);

void software_pwm_tick(software_pwm_group_t* group);

#endif /* LIBRARIES_OUTPUT_SOFTWARE_PWM_H */
