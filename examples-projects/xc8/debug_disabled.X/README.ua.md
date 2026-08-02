# debug_disabled XC8 приклад

## Призначення

Показує, що debug API компілюється при `DRV_DEBUG_ENABLE=0`.

## Апаратна частина

- PIC18F452
- Debug hardware не потрібне

## Збірка

```cmd
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject\Makefile-default.mk SUBPROJECTS= .build-conf
```

## Очікуваний результат

Проєкт збирається, а всі debug-виклики стають no-op.
