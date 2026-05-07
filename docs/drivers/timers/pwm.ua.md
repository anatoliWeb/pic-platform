[🇬🇧 English version](./pwm.md)

# PWM Драйвер

## Опис
PWM драйвер на базі CCP модулів із Timer2 як часовою основою.

## API
- `pwm_init()`
- `pwm_start()`
- `pwm_stop()`
- `pwm_set_duty()`

## Приклад
```c
pwm_init(1000u);
pwm_start(PWM_CHANNEL_1);
pwm_set_duty(PWM_CHANNEL_1, 512u);
```

## Особливості
- Керує частотою PWM і duty cycle.
- Використовує вибір каналу CCP (`PWM_CHANNEL_1/2`).

## Залежності
- `core/compiler.h`
- `core/types.h`
- Апаратний шлях Timer2/CCP
