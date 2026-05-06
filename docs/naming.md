> Навігація: [README (EN)](../README.md) | [README (UA)](../README.ua.md) | [Конвенції](driver-convention.md) | [Компілятори](compiler.md) | [Іменування](naming.md) | [Приклади](examples.md) | [Портинг](porting.md) | [Build Validation](build-validation.md)
# Правила Іменування

## Загальний стиль
- Імена мають бути короткі й однозначні
- Формат: `snake_case`

## Функції
- Формат: `driver_action()`
- Приклади:
  - `uart_init`
  - `gpio_write_high`
  - `adc_read`

## Типи
- Для структур/типів: суфікс `_t`
- Приклади:
  - `button_t`
  - `ring_buffer_t`

## Макроси
- UPPER_CASE
- Префікс драйвера/модуля бажаний

## Заборонено
- Випадкові назви без контексту
- Змішування стилів в одному драйвері
- Project-specific назви в драйверах

