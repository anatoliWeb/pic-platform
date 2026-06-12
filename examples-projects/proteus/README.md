# Proteus Simulations

Proteus simulation folders live here.
Each example project gets one folder named after the project without `.X`.
Firmware HEX files are shared from `examples-projects/hex/` and are not duplicated inside Proteus folders.
Preferred firmware is the XC8 HEX because XC8 builds are already validated. C18 HEX files may exist only for projects that passed C18 build validation.

## Folder Layout

- `examples-projects/proteus/<project>/`
- `README.md` and `README.ua.md` for manual setup notes
- `proteus-version.txt` for the local Proteus version and saved project file name
- real `.pdsprj` / `.dsn` files may be committed when they are created manually

## Shared Firmware Paths

- XC8: `../../hex/xc8/<project>/<project>.hex`
- C18: `../../hex/c18/<project>/<project>.hex` when available

## Rules

- Do not copy HEX files into Proteus folders.
- Do not create fake `.pdsprj` or `.dsn` files.
- Do not commit generated backup/cache files.
- Update `proteus-version.txt` after saving a manual Proteus project.
- Keep manual Proteus notes short and practical.