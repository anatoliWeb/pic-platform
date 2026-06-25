[English version](./segment_keys.md)

# Бібліотека segment_keys

## Опис

Декодер shared-line кнопок для кнопок, підключених до шини сегментів семисегментного індикатора.

## API

- `segment_keys_init()`
- `segment_keys_update()`
- `segment_keys_is_pressed()`
- `segment_keys_get_raw_mask()`
- `segment_keys_is_unknown()`
- `segment_keys_get_key()`

## Конфігурація

`segment_keys_config_t` містить:

- `display` - цільовий `seven_segment_t`
- `keys` - таблицю точних масок сегментів
- `key_count`
- `active_level` - активний електричний рівень під час сканування

Кожен `segment_keys_entry_t` містить:

- `segment_mask`
- `key`
- необов'язковий `button_t*`

## Модель сканування

- Перед зчитуванням вимикаються всі digit lines.
- Піни сегментів тимчасово переводяться у вхід.
- Бібліотека читає точну маску сегментів, яку формує схема кнопки.
- Після зчитування виходи сегментів відновлюються.
- `segment_keys` не керує оновленням дисплея. Застосунок має окремо продовжувати викликати `seven_segment_process()` або `seven_segment_refresh()`.

## Інтеграція з button

- `segment_keys` сам не реалізує debounce, click або hold.
- Якщо в записі кнопки є `button != NULL`, декодований стан pressed або released передається через `button_set_raw_state()` і `button_update()`.
- Так click, double-click, hold і hold-repeat лишаються всередині вже існуючої бібліотеки `button`.
- Рекомендований шлях інтеграції:
  - `button_init_external()`
  - `button_set_raw_state()`
  - `button_update()`

## Підтримуване підключення кнопок

- Одна кнопка на одній лінії сегмента
- Одна кнопка, закодована кількома лініями сегментів через діоди
- Лише exact-mask декодування

## Сімейства прикладів

- `examples-projects/xc8/seven_segment/keys_single_line.X`
- `examples-projects/xc8/seven_segment/keys_diode_coded.X`

## Приклад

```c
button_t key_button;
segment_keys_t keys;

button_init_external(&key_button, 1u);
segment_keys_init(&keys, &cfg);

while (1)
{
    segment_keys_update(&keys);

    if (button_clicked(&key_button))
    {
        /* handle click */
    }
}
```

## Примітки

- Невідомі або неоднозначні маски доступні через `segment_keys_is_unknown()`.
- Бібліотека призначена для схем з малою кількістю пінів, де індикатор і кнопки ділять одну шину.
- Електрична схема pull-up або pull-down має відповідати `active_level`.
- Exact-mask декодування робить бібліотеку простою і передбачуваною; перекривні маски краще вирішувати на рівні апаратної схеми, а не приховувати в декодері.

## Статус

- Бібліотека реалізована.
- Групування прикладів завершено.
- Ручна перевірка в Proteus ще очікується.

## Залежності

- `libraries/display/seven_segment`
- `libraries/input/button`
- `drivers/gpio/gpio.h`
