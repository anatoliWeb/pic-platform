# AGENTS.md

This is the AI entry map for `pic-platform`.

## What this repository is

Reusable PIC18 platform code split into `core/`, `drivers/`, `libraries/`, compiler wrappers, example projects, tests, and documentation.

## Mandatory global rules

- MUST check `git status --short` before reading or changing files.
- MUST protect pre-existing dirty files.
- MUST read this file completely before starting repository work.
- MUST use the routing map below before doing broad repository searches.
- MUST search existing platform functionality before creating project-local code.
- MUST keep reusable code in `pic-platform`.
- MUST keep project-specific logic in project code.
- MUST update EN/UA documentation when behavior changes.
- MUST run targeted builds, tests, and checks only by default.
- MUST NOT use `git add .` or `git add -A`.
- MUST NOT push unless explicitly asked.
- MUST NOT report a task as complete when required commits, tests, builds, or documentation are still missing.

## Mandatory repository navigation

Before recursively searching the repository or designing a new module:

1. Read this file completely.
2. Identify the task domain.
3. Open the matching mapper from the routing table below.
4. Follow the mapper links to the relevant library, driver, example, test, and documentation files.
5. Check whether the requested behavior already exists.
6. Only then use focused `git grep` searches to confirm gaps.
7. Use repository-wide search only when the mapper and focused searches are insufficient.

Do not begin by parsing the whole repository when the agent map already identifies the relevant subsystem.

Do not create a new driver, library, module, HAL, helper, utility, adapter, scheduler, timer abstraction, protocol implementation, or project-local replacement before checking the mapped existing implementation.

## Required first report

Before changing code, report:

- `git status --short`;
- task domain;
- instruction files read;
- relevant agent-map entries;
- existing libraries and drivers checked;
- related examples and tests found;
- one of these decisions:
  - use existing;
  - extend existing;
  - merge existing partial implementations;
  - add reusable platform code;
  - keep project-specific;
- unresolved gaps that require broader search.

Do not modify code before this report unless the user explicitly asks for an immediate trivial edit.

## AI routing map

Start here, then open only the required mapper and its linked files.

| If task is about | Open |
|---|---|
| Repository layout | `.agents/00-map/REPOSITORY-MAP.md` |
| Platform architecture | `.agents/00-map/ARCHITECTURE.md` |
| Global rules | `.agents/10-rules/GLOBAL-RULES.md` |
| Coding rules | `.agents/10-rules/CODING-RULES.md` |
| Configuration rules | `.agents/10-rules/CONFIGURATION-RULES.md` |
| Core helpers | `.agents/core/README.md` |
| Drivers | `.agents/drivers/README.md` |
| Library selection | `.agents/libraries/README.md` |
| Display | `.agents/libraries/display/README.md` |
| Input, buttons, sensors, zero-cross | `.agents/libraries/input/README.md` |
| Outputs, relays, triacs, AC phase control | `.agents/libraries/output/README.md` |
| Actuators and motors | `.agents/libraries/actuator/README.md` |
| Debug and logging | `.agents/libraries/system/README.md` |
| Timers, scheduler, delays, timeouts | `.agents/libraries/time/README.md` |
| Examples | `.agents/examples/README.md` |
| Compiler wrappers | `.agents/compilers/README.md` |
| Build, XC8, HEX | `.agents/build/TESTING.md` |
| Documentation | `.agents/docs/DOCUMENTATION.md` |
| Git workflow and commits | `.agents/workflow/GIT-WORKFLOW.md` |
| Task completion checklists | `.agents/checklists/CHECKLISTS.md` |

If a mapped file does not exist, report that as an agent-map defect before creating or relocating documentation.

## Library selection procedure

For any feature request:

1. Open `.agents/libraries/README.md`.
2. Open the mapped domain README.
3. Check listed:
   - reusable library path;
   - public API;
   - dependencies;
   - compiler support;
   - examples;
   - tests;
   - EN/UA docs;
   - known limitations.
4. Inspect only those files first.
5. Perform a focused search for related symbols and alternate names.
6. Select one decision:
   - existing implementation is sufficient;
   - existing implementation needs a backward-compatible extension;
   - multiple partial implementations should be consolidated;
   - new reusable platform functionality is required;
   - behavior is application-specific and belongs outside the platform.

A new implementation is forbidden until this procedure is completed and reported.

## Existing-first rule

Before creating anything new, search by both domain language and implementation language.

Examples:

- zero-cross, zero cross, `zero_cross`, `INT0`;
- AC phase control, phase angle, triac, thyristor, burst fire;
- relay, output channel, bypass;
- timer, scheduler, timeout, tick;
- display, LCD, HD44780, PCF8574;
- UART, RS485, protocol, CRC;
- buttons, debounce, encoder;
- PWM, CCP, capture, compare.

Prefer mapped paths and focused `git grep` over full recursive parsing.

## Reusable versus project-specific decision

Reusable behavior belongs in `pic-platform` when it:

- can be used by more than one project;
- does not depend on one board pinout;
- can expose pins through callbacks, descriptors, adapters, or configuration;
- represents a driver, protocol, timing primitive, display, input, output, actuator, safety helper, or control algorithm;
- can be tested independently.

Project-specific behavior belongs in the application when it:

- encodes one product workflow;
- depends on one machine state model;
- binds platform APIs to one board pinout;
- contains product-specific policy, screens, recipes, commands, or business logic.

Do not copy reusable code into an example or application.

## Configuration contract

Before adding or moving compile-time macros, read:

```text
.agents/10-rules/CONFIGURATION-RULES.md
docs/configuration.md
docs/configuration.ua.md
scripts/config_contract.py
```

Project-wide Category A defines must follow the canonical configuration contract.

Do not duplicate Category A values in `project_config.h` when they belong in compiler `-D` or MPLAB `define-macros`.

Runtime values should remain function arguments or runtime configuration fields when possible.

## Build and test rules

- Use only targeted/unit/regression tests related to changed code.
- Do not run the full suite unless explicitly requested or required by a mapped instruction.
- Use `xc8` from `PATH`.
- Do not hardcode a local compiler path.
- Prefer tracked project metadata and repository build helpers.
- Do not call a build verified unless the compiler actually completed successfully.
- Report commands, results, warnings, ROM, RAM, and HEX paths for requested XC8 builds.
- Run `git diff --check` before commits.

## Documentation rules

When public behavior, API, configuration, architecture, or examples change:

- update English documentation;
- update Ukrainian documentation;
- keep Ukrainian documentation in UTF-8 Cyrillic, not transliteration;
- update agent-map entries when paths, capabilities, dependencies, examples, tests, or limitations change;
- keep the agent map concise and link to detailed files instead of duplicating entire documentation.

## Git and completion rules

Before committing:

```bash
git status --short
git diff --name-status
git diff --stat
git diff --check
```

Use exact file staging only.

Before each commit:

```bash
git diff --cached --name-status
git diff --cached --stat
git diff --cached --check
```

After each commit:

```bash
git log -1 --oneline
git status --short
```

A task is complete only when all requested acceptance criteria are satisfied.

If the task requires commits, do not write `Task complete` until real commit hashes appear in `git log`.

The final report must state:

- decision made;
- files changed;
- tests run;
- builds run;
- documentation updated;
- commit hashes, if requested;
- final `git status --short`;
- push performed: yes/no;
- anything not verified and why.

## Start every task with

1. Check `git status --short`.
2. Identify the task type and domain.
3. Read this file completely.
4. Open only the required instruction files from the routing map.
5. Follow mapped links to existing libraries, drivers, examples, tests, and docs.
6. Search existing platform functionality with focused queries.
7. Decide: use existing, extend existing, consolidate, add reusable platform code, or keep project-specific.
8. Report the decision before changing code.
9. Implement the smallest correct change.
10. Run targeted validation.
11. Update docs and agent maps when behavior changes.
12. Commit only when requested and only with exact staging.

## Hard rules

- Before creating a new driver, module, HAL, helper, utility, adapter, protocol, scheduler, timer wrapper, control layer, or project-local implementation, check whether the functionality already exists in `pic-platform`, can be safely extended, can be consolidated from partial implementations, or should be added as reusable platform code.
- Do not duplicate reusable code inside a project.
- Do not silently replace or rewrite verified timer, compiler, or platform infrastructure.
- Do not hide malformed tracked configuration by repairing it heuristically at runtime.
- Do not push without a direct command.
- Do not run broad builds by default.
- Do not claim completion while required work remains.
