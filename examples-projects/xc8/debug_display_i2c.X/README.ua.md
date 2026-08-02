# debug_display_i2c XC8 приклад

## Призначення

Показує `DISPLAY` backend з адаптером LCD 2x16 через I2C.

## Апаратна частина

- PIC18F452
- LCD 2x16 з backpack PCF8574 на адресі `0x27`
- I2C лінії RC3/SCL та RC4/SDA

## Збірка

```bash
mplab-make -f examples-projects/xc8/debug_display_i2c.X/Makefile build
```

## Очікуваний результат

Дисплей очищається і показує рядок запуску та лічильник `tick=`.
