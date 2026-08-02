# debug_disabled XC8 приклад

## Призначення

Показує, що debug API компілюється при `DRV_DEBUG_ENABLE=0`.

## Апаратна частина

- PIC18F452
- Debug hardware не потрібне

## Збірка

```bash
mplab-make -f examples-projects/xc8/debug_disabled.X/Makefile build
```

## Очікуваний результат

Проєкт збирається, а всі debug-виклики стають no-op.
