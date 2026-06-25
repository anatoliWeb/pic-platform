# Нотатки Proteus: multiplex_timer

## Призначення

Нотатки для перевірки мінімального діагностичного прикладу seven-segment дисплея в timer-owned режимі.

## Проєкт прикладу

- `examples-projects/xc8/seven_segment/multiplex_timer.X`

## Що Перевірити

- Дисплей показує постійне `1234`.
- Після `seven_segment_init()` refresh від Timer2 належить бібліотеці.
- Глобальний ISR лише передає керування через `seven_segment_irq_handler()`.
- `seven_segment_process()` у timer mode не потрібен.
- `RE0` залишається LOW у нормальному режимі.
- `RE1` змінюється, коли змінюється timer-owned refresh counter.
- `RE2` перемикається й показує, що головний цикл живий.

## Діагностична Таблиця

| Стан сигналу | Значення |
|---|---|
| `RE0 = 1` | помилка init або API |
| `RE1 = 1` | змінюється timer refresh counter |
| `RE2 toggles` | головний цикл живий |
| дисплей темний + `RE1 = 0` | збій Timer2 / ISR backend |
| дисплей темний + `RE1 = 1` | проблема GPIO refresh / polarity |

## Статус

Готово до перевірки в Proteus.
