[English version](./naming.md)

# Правила Іменування

## Функції
- Формат: `driver_action()`
- Приклади: `uart_init`, `gpio_set_output`, `adc_read`

## Типи
- Структури: `<driver>_t`
- Enum: `<driver>_<name>_t`

## Макроси
- Префікс: `DRV_`
- Стиль: `UPPER_SNAKE_CASE`

## Файли
- Нижній регістр в іменах файлів
- Імена `.h/.c` відповідають назві драйвера
