# Бібліотека Software PWM

## Опис

Бібліотека software PWM для кількох GPIO-каналів, керованих спільним timer tick.
Це не hardware PWM/CCP, а окремий підхід для гнучкого низькорівневого керування виходами, коли один таймер обслуговує одну PWM-групу.

## API

- `software_pwm_init_group()`
- `software_pwm_set_duty()`
- `software_pwm_enable_channel()`
- `software_pwm_tick()`

## Архітектура

- Один `software_pwm_group_t` відповідає одній частотній PWM-групі.
- Один timer source генерує tick для однієї групи.
- У межах однієї групи може бути багато каналів.
- Усі канали в групі мають одну базову PWM-частоту.
- Кожен канал керує лише своїм duty cycle та станом enable.

## Приклад

```c
static software_pwm_channel_t channels[4] =
{
    { &LATB, &TRISB, (uint8_t)(1u << 0u), 1u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 1u), 3u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 2u), 6u, 1u },
    { &LATB, &TRISB, (uint8_t)(1u << 3u), 9u, 1u }
};

static software_pwm_group_t group;

software_pwm_init_group(&group, SOFTWARE_PWM_TIMER2, 10u, channels, 4u);
```

## Примітки

- `duty = 0` означає завжди OFF.
- `duty >= resolution` означає завжди ON.
- Вибір timer source входить до метаданих групи.
- У поточному прикладі реально використовується `Timer2` як джерело tick.
- `Timer0`, `Timer1` і `Timer3` поки лишаються запланованими джерелами таймера та потребують окремої перевірки.
- Очікуються статичні масиви каналів; динамічна пам'ять не використовується.
- Канали можуть бути на різних портах, якщо вказані правильні адреси регістрів.
- Для PIC18F452 XC8 використовуйте `LATx` разом із відповідним `TRISx`.
  Не використовуйте `PORTx` як latch для software PWM, щоб не ловити read-modify-write проблеми.

## Апаратний PWM проти Software PWM

- Hardware PWM використовує спеціальні периферійні блоки CCP/PWM.
- Software PWM перемикає GPIO програмно з таймерного tick.
- Hardware PWM краще підходить для високих частот і меншого навантаження на CPU.
- Software PWM гнучкіший для багатьох довільних GPIO-виходів, але обмежений навантаженням ISR і частотою таймера.

## Продуктивність

- Один timer відповідає одній frequency group.
- Вища роздільна здатність знижує ефективну частоту PWM при фіксованому timer interrupt rate.
- Більша кількість каналів збільшує навантаження ISR.
- Дуже висока частота + висока роздільна здатність + багато каналів не підходять для PIC18F452.
- Callback таймера має бути коротким; не друкуйте UART у PWM tick path.

## Примітка про керування AC мережею

- Не сприймайте AC 220V керування як звичайний GPIO PWM.
- Phase-angle control потребує zero-cross detector і точного імпульсу керування після zero-cross.
- Для zero-cross SSR зазвичай використовують burst-fire або time-proportional control, а не звичайний PWM.
- Random-fire triac control — окремий і значно більш критичний за безпекою режим.
- Потрібні гальванічна розв'язка, оптопари, захист, creepage/clearance і дотримання правил безпеки для мережі.
- У цьому репозиторії AC mains control лишається окремим майбутнім модулем, не частиною software PWM.

## Статус

Готово до перевірки в Proteus.
