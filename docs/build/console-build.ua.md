[English version](./console-build.md)

# Консольна збірка

## Призначення

Цей документ пояснює, як збирати приклади PIC platform з командного рядка через MPLAB X generated Makefiles і XC8 toolchain.

## Потрібні програми

- MPLAB X IDE
- XC8 compiler
- Microchip PIC18Fxxxx DFP pack
- Git
- Windows Command Prompt або PowerShell

## Типові шляхи

Версії можуть відрізнятися. Типові розташування:

- `C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin`
- `C:\Program Files\Microchip\xc8\v3.10\bin`
- `C:\Program Files\Microchip\MPLABX\v6.30\packs\Microchip\PIC18Fxxxx_DFP\1.7.171`

## Збірка з консолі

Використовуйте MPLAB generated Makefile з папки проєкту `.X`:

```cmd
cd /d E:\_electronic_\microcontroller\pic\mlab-x-ide\pic18f452\pic-platform\examples-projects\xc8\seven_segment\keys_single_line.X
make -f nbproject/Makefile-default.mk SUBPROJECTS= .clean-conf
make -f nbproject/Makefile-default.mk SUBPROJECTS= .build-conf
```

Якщо `make` немає в `PATH`, викликайте повний шлях до `make.exe`.

## Де з'являються HEX-файли

Production HEX пишеться у дерево `dist/` всередині проєкту. У репозиторії зберігаються повторно придатні для Proteus копії тут:

```text
examples-projects/hex/xc8/seven_segment/
```

## Debug vs Production

- `debug` build створює debug artifacts у `dist/default/debug/`
- `production` build створює HEX у `dist/default/production/`
- Для Proteus зазвичай використовуйте `*.production.hex`

## Типові помилки

- `make: Makefile: No such file or directory` означає, що команду запущено не з папки `.X` або пропущено `-f nbproject/Makefile-default.mk`.
- `xc8-cc.exe not found` означає, що XC8 не встановлений або не доданий у `PATH`.
- `DFP pack not found` означає, що PIC18Fxxxx DFP pack відсутній або MPLAB metadata вказує на неправильний pack.

## Скрипти

Наразі окремі build helper scripts у репозиторії не надані.
Консольна збірка виконується через MPLAB X generated Makefiles.