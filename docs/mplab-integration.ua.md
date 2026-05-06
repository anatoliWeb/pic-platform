# Інтеграція MPLAB

[🇬🇧 English version](./mplab-integration.md) | [Головний README](../README.ua.md)

Цей документ пояснює, як підключити `pic-platform` як зовнішню бібліотеку драйверів у PIC18 проєкті MPLAB X.

Основна ідея:

- тримати application-проєкт чистим;
- не копіювати драйвери в кожен новий проєкт;
- використовувати `pic-platform` як окрему reusable бібліотеку;
- використовувати лише відносні шляхи.

---

## 1. Рекомендована структура папок

Тримайте MPLAB-проєкт та бібліотеку платформ поруч.

```text
pic18f452/
├── blink.X/          ← MPLAB application project
└── pic-platform/     ← зовнішня бібліотека драйверів
```

Приклад:

![Структура проєкту](assets/mplab/project-structure.png)

### Правила

- `blink.X` — application project.
- `pic-platform` — reusable driver library.
- Не копіюйте `core/` або `drivers/` всередину `blink.X`.
- Використовуйте лише relative paths.

---

## 2. Що потрібно MPLAB

MPLAB потребує дві речі:

1. **Include paths** для `.h` файлів.
2. **Source files** для `.c` файлів.

Це різні речі.

```text
.h файли → знаходяться через include paths
.c файли → повинні бути додані в Source Files
```

---

## 3. Правильне додавання Source Files

MPLAB НЕ компілює `.c` файли автоматично із зовнішніх папок.

Кожен потрібний `.c` файл треба додати вручну.

### Кроки

1. Натисніть правою кнопкою на **Source Files**
2. Оберіть **Add Existing Item**
3. Виберіть потрібні `.c` файли з `../pic-platform/...`
4. Залиште шлях як **Relative**

Приклад:

![Додавання source files](assets/mplab/add-source-files.png)

### Приклад Source Files

Для простого GPIO blink потрібно додати:

```text
../pic-platform/core/delay.c
../pic-platform/drivers/gpio/gpio.c
```

Для UART також додайте:

```text
../pic-platform/drivers/uart/uart.c
```

Для ADC додайте:

```text
../pic-platform/drivers/adc/adc.c
```

### Важливо

- Додавайте в **Source Files** тільки `.c` файли.
- Не додавайте `.h` файли вручну.
- Header файли підключаються через include paths та `#include`.

---

## 4. Налаштування Include Paths

Відкрийте:

```text
Project → Properties → XC8 Compiler або C18 Compiler → Include directories
```

Додайте:

```text
../pic-platform
../pic-platform/core
../pic-platform/drivers
```

Приклад:

![Include paths](assets/mplab/include-paths.png)

Після цього будуть працювати include-и:

```c
#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"
```

---

## 5. Вибір компілятора

Компілятор вибирається тільки в налаштуваннях MPLAB.

Підтримуються:

- MPLAB XC8
- MPLAB C18

Переключення компілятора НЕ робиться через source code.

```text
MPLAB project settings → select XC8 or C18
```

Бібліотека автоматично адаптується через `core/compiler.h`.

---

## 6. Compiler Abstraction

`core/compiler.h` автоматично визначає активний компілятор та створює внутрішні platform macros:

```text
XC8 → DRV_COMPILER_XC8
C18 → DRV_COMPILER_C18
```

### Важливі правила

- Не форсуйте compiler mode вручну в application code.
- Не визначайте одночасно обидва compiler macros.
- Не підключайте device headers всередині бібліотеки.
- Device headers належать application layer.

---

## 7. Правило Device Header

Тільки application project повинен підключати MCU header.

### XC8 приклад

```c
#include <xc.h>
```

### C18 приклад

```c
#include <p18f452.h>
```

Не підключайте `p18fXXX.h` або `xc.h` всередині:

```text
core/
drivers/
```

---

## 8. Мінімальний GPIO приклад

Цей приклад перевіряє:

- include paths;
- external source files;
- `core/delay`;
- `drivers/gpio`.

```c
#include <p18f452.h>

#include "core/compiler.h"
#include "core/delay.h"
#include "drivers/gpio/gpio.h"

#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF

void main(void)
{
    gpio_set_output(&TRISB, 0);

    while (1)
    {
        gpio_toggle(&PORTB, 0);
        delay_ms(500);
    }
}
```

Потрібні source files:

```text
../pic-platform/core/delay.c
../pic-platform/drivers/gpio/gpio.c
```

---

## 9. Мінімальний UART приклад

```c
#include <p18f452.h>

#include "core/compiler.h"
#include "drivers/uart/uart.h"

#pragma config OSC = HS
#pragma config WDT = OFF
#pragma config LVP = OFF

void main(void)
{
    uart_init(9600u);
    uart_write_string("MPLAB integration OK\r\n");

    while (1)
    {
    }
}
```

Потрібні source files:

```text
../pic-platform/drivers/uart/uart.c
```

Якщо UART залежить від інших модулів у вашій реалізації — також додайте відповідні `.c` файли.

---

## 10. Build Checklist

Перед Build перевірте:

- потрібні `.c` файли додані в **Source Files**;
- `.h` файли не додавались вручну;
- include paths використовують relative paths;
- вибраний XC8 або C18;
- тільки application project підключає MCU header;
- `core/` та `drivers/` не містять device includes;
- build проходить без unresolved symbols.

---

## 11. Типові помилки

### `file not found`

Причина:

```text
Відсутній або неправильний include path.
```

Виправлення:

```text
Додайте:
../pic-platform
../pic-platform/core
../pic-platform/drivers
```

---

### `undefined symbol` або `could not find definition`

Причина:

```text
`.h` файл знайдений, але `.c` реалізація не була додана в Source Files.
```

Виправлення:

```text
Додайте потрібний `.c` файл в Source Files.
```

Приклад:

```text
undefined symbol delay_ms
```

Виправлення:

```text
Add ../pic-platform/core/delay.c
```

---

### `_CONFIG_DECL does not agree`

Причина:

```text
Підключено більше одного device header або device header підключений всередині бібліотеки.
```

Виправлення:

```text
Тільки main.c повинен включати <p18f452.h> або <xc.h>.
Видаліть device includes з core/ та drivers/.
```

---

## 12. Головне правило

```text
Application project = основна логіка
pic-platform = reusable drivers
```

Тримайте їх окремо.

Не перетворюйте бібліотеку в application project.
