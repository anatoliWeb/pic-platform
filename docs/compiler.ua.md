[English version](./compiler.md)

# Нотатки Про Компілятори

## Підтримувані Toolchain
- MPLAB C18
- MPLAB XC8

## Головний Принцип
Зберігати один публічний API та ізолювати compiler-specific реалізації.

## Розташування
- Абстракції: `core/compiler.h`
- C18-код: `C18/drivers/*`
- XC8-код: `XC8/drivers/*`

## Рекомендація
Використовувати універсальний entrypoint у `drivers/<name>/<name>.c` і маршрутизацію через макроси компілятора.
