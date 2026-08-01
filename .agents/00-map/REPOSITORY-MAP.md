# Repository Map

This is a navigation map for the real repository layout. Read only the branch that matches the task.

| Path | Purpose | Contains | Must not contain | Used by | Related instructions |
|---|---|---|---|---|---|
| `core/` | Platform core helpers | compiler abstraction, types, delay, config, device clock glue, bit utils | project-specific business logic | drivers, libraries, examples | `10-rules/CODING-RULES.md`, `10-rules/CONFIGURATION-RULES.md` |
| `drivers/` | Low-level reusable MCU drivers | GPIO, UART, timers, ADC, communication, memory, interrupts, system drivers | library-level UI or application logic | libraries, examples | `10-rules/GLOBAL-RULES.md`, `10-rules/CODING-RULES.md` |
| `libraries/` | Reusable higher-level platform components | display, input, actuator, LED, sensor, system, time, UI libraries | board pin binding, project-only orchestration | examples, project code | `20-libraries/*`, `30-projects/*` |
| `XC8/` | XC8 compiler wrappers and backends | XC8-specific driver/library implementations | C18 sources, project-local code | top-level wrappers in `drivers/` and `libraries/` | `50-compilers/COMPILER-COMPATIBILITY.md` |
| `C18/` | C18 compiler wrappers and backends | C18-specific driver/library implementations | XC8 sources, project-local code | top-level wrappers in `drivers/` and `libraries/` | `50-compilers/COMPILER-COMPATIBILITY.md` |
| `examples-projects/` | MPLAB wrapper projects and artifacts | XC8/C18 example projects, HEX, Proteus docs | reusable platform source duplication | README, docs, AI routing | `40-examples/EXAMPLES.md`, `60-build/HEX-ARTIFACTS.md` |
| `examples-projects/hex/` | Tracked HEX artifacts | repository-copied production HEX files | source code, project metadata | Proteus folders, release validation | `60-build/HEX-ARTIFACTS.md` |
| `examples-projects/proteus/` | Proteus simulation docs and projects | wiring notes, saved Proteus projects, version notes | copied HEX files, build output, caches | example validation | `30-projects/BOARD-INTEGRATION.md`, `40-examples/EXAMPLES.md` |
| `docs/` | Human documentation | architecture, build, compiler, naming, porting, module docs | AI-only instruction files | README, maintainers, AI routing | `70-docs/DOCUMENTATION.md` |
| `examples/` | Shared example support | common example assets | platform libraries or drivers | example projects | `40-examples/EXAMPLES.md` |
| `prompts/` | Reusable prompt contexts for chat tools | prompt templates and notes | source code | docs/prompts and AI users | `70-docs/DOCUMENTATION.md` |
| `scripts/` | Helper scripts | automation helpers | platform runtime code | maintainers | `80-workflow/GIT-WORKFLOW.md` |
| `templates/` | Project templates | config templates and starter files | generated build artifacts | example/project setup | `10-rules/CONFIGURATION-RULES.md` |
| `docs/architecture/` | Structural docs | dependency graphs, example matrix, module inventory, generation workflow | code | AI audit and humans | `00-map/ARCHITECTURE.md` |
| `docs/libraries/` | Human library docs | per-library reference docs in EN and UA | AI instructions | library selection and integration | `20-libraries/LIBRARY-CATALOG.md`, `70-docs/DOCUMENTATION.md` |
| `docs/build/` | Build docs | console build and validation notes | platform source | build workflow | `60-build/TESTING.md`, `60-build/HEX-ARTIFACTS.md` |
