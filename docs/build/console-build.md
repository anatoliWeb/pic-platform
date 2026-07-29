[Ukrainian version](./console-build.ua.md)

# Console Build

## Purpose

This document explains how to build PIC platform example projects from the command line using MPLAB X generated Makefiles and the XC8 toolchain.

## Requirements

- MPLAB X IDE
- XC8 compiler
- Microchip PIC18Fxxxx DFP pack
- Git
- Windows Command Prompt or PowerShell

## Typical Paths

Installations vary by version. Typical locations are:

- `C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin`
- `C:\Program Files\Microchip\xc8\v3.10\bin`
- `C:\Program Files\Microchip\MPLABX\v6.30\packs\Microchip\PIC18Fxxxx_DFP\1.7.171`

## Console Build

Use the MPLAB generated Makefile from the `.X` project folder:

```cmd
cd /d E:\_electronic_\microcontroller\pic\mlab-x-ide\pic18f452\pic-platform\examples-projects\xc8\seven_segment\keys_single_line.X
make -f nbproject/Makefile-default.mk SUBPROJECTS= .clean-conf
make -f nbproject/Makefile-default.mk SUBPROJECTS= .build-conf
```

If `make` is not on `PATH`, call the full path to `make.exe`.

## Where HEX Files Appear

Production HEX output is written by MPLAB to the project `dist/` tree. The repository keeps reusable Proteus-ready copies under:

```text
examples-projects/hex/xc8/seven_segment/
```

## Debug vs Production

- `debug` builds create debug artifacts under `dist/default/debug/`
- `production` builds create HEX output under `dist/default/production/`
- For Proteus, use `*.production.hex`

## Troubleshooting

- `make: Makefile: No such file or directory` means the command was not started in the `.X` project folder or `-f nbproject/Makefile-default.mk` was omitted.
- `xc8-cc.exe not found` means XC8 is not installed or not on `PATH`.
- `DFP pack not found` means the PIC18Fxxxx DFP pack is missing or MPLAB project metadata points to the wrong pack.

## Scripts

No repository build helper scripts are currently provided.
Console builds are performed through MPLAB X generated Makefiles.
