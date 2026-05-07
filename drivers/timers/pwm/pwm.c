#include "drivers/timers/pwm/pwm.h"

#if defined(DRV_COMPILER_C18)
    #include "../../../C18/drivers/timers/pwm/pwm.c"
#elif defined(DRV_COMPILER_XC8)
    #include "../../../XC8/drivers/timers/pwm/pwm.c"
#else

#include "core/device.h"

static uint8_t pwm_prescaler_bits = 0u;

static uint8_t pwm_calc_prescaler(uint32_t frequency, uint8_t* pr2_value)
{
    uint32_t pr2;

    if (frequency == 0u)
    {
        frequency = 1000u;
    }

    pr2 = (_XTAL_FREQ / (4u * 1u * frequency)) - 1u;
    if (pr2 <= 255u)
    {
        *pr2_value = (uint8_t)pr2;
        return 0u;
    }

    pr2 = (_XTAL_FREQ / (4u * 4u * frequency)) - 1u;
    if (pr2 <= 255u)
    {
        *pr2_value = (uint8_t)pr2;
        return 1u;
    }

    pr2 = (_XTAL_FREQ / (4u * 16u * frequency)) - 1u;
    if (pr2 > 255u)
    {
        pr2 = 255u;
    }

    *pr2_value = (uint8_t)pr2;
    return 2u;
}

void pwm_init(uint32_t frequency)
{
    uint8_t pr2_val;

    pwm_prescaler_bits = pwm_calc_prescaler(frequency, &pr2_val);

    PR2 = pr2_val;

    T2CONbits.T2CKPS0 = (uint8_t)(pwm_prescaler_bits & 0x01u);
    T2CONbits.T2CKPS1 = (uint8_t)((pwm_prescaler_bits >> 1u) & 0x01u);
    T2CONbits.TMR2ON = 1u;

    CCP1CON = 0x0Cu;
    CCP2CON = 0x0Cu;

    pwm_set_duty(PWM_CHANNEL_1, 0u);
    pwm_set_duty(PWM_CHANNEL_2, 0u);
}

void pwm_start(pwm_channel_t channel)
{
    if (channel == PWM_CHANNEL_1)
    {
        CCP1CONbits.CCP1M = 0x0Cu;
    }
    else
    {
        CCP2CONbits.CCP2M = 0x0Cu;
    }
}

void pwm_stop(pwm_channel_t channel)
{
    if (channel == PWM_CHANNEL_1)
    {
        CCP1CONbits.CCP1M = 0x00u;
    }
    else
    {
        CCP2CONbits.CCP2M = 0x00u;
    }
}

void pwm_set_duty(pwm_channel_t channel, uint16_t duty)
{
    if (duty > 1023u)
    {
        duty = 1023u;
    }

    if (channel == PWM_CHANNEL_1)
    {
        CCPR1L = (uint8_t)(duty >> 2u);
        CCP1CONbits.DC1B0 = (uint8_t)(duty & 0x01u);
        CCP1CONbits.DC1B1 = (uint8_t)((duty >> 1u) & 0x01u);
    }
    else
    {
        CCPR2L = (uint8_t)(duty >> 2u);
        CCP2CONbits.DC2B0 = (uint8_t)(duty & 0x01u);
        CCP2CONbits.DC2B1 = (uint8_t)((duty >> 1u) & 0x01u);
    }
}

#endif
