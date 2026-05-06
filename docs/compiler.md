> Навігація: [README (EN)](../README.md) | [README (UA)](../README.ua.md) | [Конвенції](driver-convention.md) | [Компілятори](compiler.md) | [Іменування](naming.md) | [Приклади](examples.md) | [Портинг](porting.md) | [Build Validation](build-validation.md)
# Нотатки По Компіляторах (C18 vs XC8)

## Мета
Забезпечити однаковий API драйверів для MPLAB C18 і MPLAB XC8.

## Де ізольована специфіка
- `core/compiler.h` — детекція компілятора та спільні макроси
- `C18/drivers/*` — реалізації для C18
- `XC8/drivers/*` — реалізації для XC8

## Основні відмінності
- Назви/доступ до регістрів можуть відрізнятись
- Синтаксис деяких бітових полів може відрізнятись
- Поведінка оптимізації/inline може відрізнятись

## Практика
- Спільний API тримати в `drivers/<driver>/<driver>.h`
- Спільну логіку — в universal `drivers/<driver>/<driver>.c`
- Специфічні відмінності — в `C18/` та `XC8/`

## Важливо
Не змішувати compiler-specific код з прикладною логікою.

