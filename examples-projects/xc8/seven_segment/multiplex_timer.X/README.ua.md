# multiplex_timer.X

[English version](./README.md)

## Призначення

Мінімальний діагностичний приклад чотирирозрядного seven-segment дисплея для PIC18F452 у timer-owned режимі.
Мета прикладу — підтвердити, що бібліотека справді виконує refresh через Timer2 без ручного refresh-виклику в `main()`.

## Що Демонструє

- `SEVEN_SEGMENT_REFRESH_TIMER`
- `SEVEN_SEGMENT_TIMER2`
- внутрішнє налаштування Timer2 бібліотекою
- `seven_segment_irq_handler()` з глобального ISR
- діагностику через timer-side refresh counter
- фіксований вміст дисплея: `1234`

## Source Files

- `main.c`
- `config_bits.c`
- `../../../../core/delay.c`
- `../../../../drivers/gpio/gpio.c`
- `../../../../drivers/timers/timer2/timer2.c`
- `../../../../libraries/display/seven_segment/seven_segment.c`

## Власник Refresh

- Власник refresh: бібліотека
- Вибраний таймер: Timer2
- Запитаний період: `1000 us`
- Виклик refresh у main loop: не потрібен у timer mode

## Вимога До ISR

```c
void __interrupt() isr(void)
{
    seven_segment_irq_handler();
}
```

Застосунок усе ще має увімкнути `PEIE` і `GIE` після успішного `seven_segment_init()`.

## Піни

- RD0..RD6: спільні сегменти A..G
- RD7: спільна десяткова крапка
- RC0..RC3: лінії вибору розрядів
- RE0: індикатор помилки init/API
- RE1: індикатор активності refresh counter
- RE2: heartbeat головного циклу

## Очікувана Поведінка

- Дисплей постійно показує `1234`.
- У timer mode дисплей має refresh-итися без `seven_segment_process()` або `seven_segment_refresh()`.
- `RE0` залишається LOW у нормальному режимі.
- `RE1` переходить у HIGH, коли змінюється `refresh_count`.
- `RE2` перемикається в головному циклі, показуючи, що застосунок живий.

## Діагностичні Примітки

- `APP_DIAGNOSTIC_MANUAL_FALLBACK = 0` залишає приклад у чистому timer-owned режимі.
- `APP_DIAGNOSTIC_MANUAL_FALLBACK = 1` перемикає приклад у manual refresh лише для порівняння.
- `seven_segment_process()` і `seven_segment_refresh()` навмисно не використовуються у timer mode.
- Якщо дисплей темний і `RE1 = 0`, спочатку перевіряйте Timer2 backend, callback routing і interrupt gates.
- Якщо дисплей темний і `RE1 = 1`, перевіряйте GPIO дисплея або polarity.

## Статус

Готово до перевірки в Proteus.
