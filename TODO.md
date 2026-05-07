# TODO

> [Українська версія](TODO.ua.md)

## Structure Refactor
- [x] Split source tree into `/drivers` and `/libraries`
- [x] Move low-level modules to grouped driver folders
- [x] Move high-level modules to grouped library folders
- [x] Move compiler-specific C18/XC8 sources to mirrored grouped folders
- [x] Move driver/library docs to `/docs/drivers` and `/docs/libraries`
- [x] Update include paths and internal source routing paths

## Validation
- [x] No old flat source folders used in code includes
- [ ] Validate MPLAB XC8 build after file moves
- [ ] Validate MPLAB C18 build after file moves
- [ ] Validate examples-projects `.X` projects with updated source references
