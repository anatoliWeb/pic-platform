# Testing

## Policy

- Run targeted builds by default.
- Do not run the full suite unless explicitly requested.
- Run regression builds only for affected shared components.
- If the change is docs-only, a C build is not required unless the docs describe generated artifacts that must be verified.

## Console build example

```cmd
cd /d examples-projects\xc8\actuator\position_drive_adc.X
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject/Makefile-default.mk SUBPROJECTS= .clean-conf
"C:\Program Files\Microchip\MPLABX\v6.30\gnuBins\GnuWin32\bin\make.exe" -f nbproject/Makefile-default.mk SUBPROJECTS= .build-conf
```

The MPLAB installation path may differ on the local machine.
