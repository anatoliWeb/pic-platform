# debug_pins_gpio XC8 приклад

## Призначення

Показує `PINS` backend з прямим GPIO-виводом.

## Апаратна частина

- PIC18F452
- Канали на бітах PORTC 0..3
- Один канал використовується як heartbeat

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

Канал 0 дає імпульс кожні 500 мс, а стартовий шаблон виводиться один раз.
