# Library Integration

## Steps

1. Read the matching detailed card in `.agents/libraries/...`.
2. Read the human docs for that library.
3. Add the required `.c` and `.h` files to the MPLAB project.
4. Add compiler wrapper files if the project style requires them.
5. Configure `project_config.h`.
6. Check `core/pic_platform_config.h` defaults.
7. Bind pins in project config or application code.
8. Check timer and peripheral conflicts.
9. Build the targeted example or project.
10. Update docs or README if the integration is reusable for others.
