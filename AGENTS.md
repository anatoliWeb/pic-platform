# AGENTS.md

This is the AI entry map for `pic-platform`.

## Repository purpose

Reusable PIC18 platform code organized into `core/`, `drivers/`, `libraries/`, compiler wrappers, examples, tests, and documentation.

## Mandatory rules

- Check `git status --short` before reading or changing files.
- Protect pre-existing dirty files.
- Read this file before starting repository work.
- Use the routing map before broad repository searches.
- Check existing platform functionality before creating new code.
- Keep reusable code in `pic-platform`.
- Keep product-specific behavior in application code.
- Run targeted tests and builds only by default.
- Update EN/UA documentation when behavior changes.
- Do not use `git add .` or `git add -A`.
- Do not push unless explicitly asked.
- Do not report completion while requested tests, builds, documentation, or commits are missing.

## AI workflow skill

Before working on pic-platform, read `.skills/pic-platform/SKILL.md`. The skill defines the AI operating workflow. The `.agents/` tree defines routing maps and module knowledge.

## Required task start

Before changing code:

1. Identify the task domain.
2. Open the matching mapper from the table below.
3. Follow its links to relevant libraries, drivers, examples, tests, and docs.
4. Search existing functionality with focused queries.
5. Decide:
   - use existing;
   - extend existing;
   - consolidate partial implementations;
   - add reusable platform code;
   - keep project-specific.
6. Report the decision and files checked.

Do not parse the whole repository when the agent map already identifies the subsystem.

Do not create a new library, driver, helper, adapter, timer layer, protocol, or project-local replacement before checking the mapped existing implementation.

## Routing map

| Task domain | Open |
|---|---|
| Repository layout | `.agents/00-map/REPOSITORY-MAP.md` |
| Architecture | `.agents/00-map/ARCHITECTURE.md` |
| Global rules | `.agents/10-rules/GLOBAL-RULES.md` |
| Coding rules | `.agents/10-rules/CODING-RULES.md` |
| Configuration | `.agents/10-rules/CONFIGURATION-RULES.md` |
| Core helpers | `.agents/core/README.md` |
| Drivers | `.agents/drivers/README.md` |
| Library selection | `.agents/libraries/README.md` |
| Displays | `.agents/libraries/display/README.md` |
| Inputs, buttons, sensors, zero-cross | `.agents/libraries/input/README.md` |
| Outputs, relays, triacs, AC phase control | `.agents/libraries/output/README.md` |
| Actuators and motors | `.agents/libraries/actuator/README.md` |
| Debug and logging | `.agents/libraries/system/README.md` |
| Timers, scheduler, delays, timeouts | `.agents/libraries/time/README.md` |
| Examples | `.agents/examples/README.md` |
| Compiler wrappers | `.agents/compilers/README.md` |
| Build, XC8, HEX | `.agents/build/TESTING.md` |
| Documentation | `.agents/docs/DOCUMENTATION.md` |
| Git workflow | `.agents/workflow/GIT-WORKFLOW.md` |
| Completion checklists | `.agents/checklists/CHECKLISTS.md` |

If a mapped file is missing, report it as an agent-map defect.

## Existing-first rule

Before creating new functionality:

1. Open `.agents/libraries/README.md`.
2. Open the matching domain mapper.
3. Check listed library paths, APIs, dependencies, examples, tests, docs, and limitations.
4. Inspect only those files first.
5. Use focused `git grep` for alternate names and related symbols.
6. Create new reusable code only when no suitable implementation exists or a clean extension is not possible.

Search by domain and implementation terms, for example:

```text
zero-cross, zero_cross, INT0
phase control, phase angle, triac, burst fire
relay, output channel, bypass
timer, scheduler, timeout, tick
PWM, CCP, capture, compare
```

## Reusable versus project-specific

Reusable behavior belongs in `pic-platform` when it can serve multiple projects, avoid one-board pin binding, and expose hardware through callbacks, descriptors, adapters, or configuration.

Product workflows, board pin binding, machine state, screens, recipes, commands, and business rules belong in application code.

Do not copy reusable code into an example or project.

## Configuration

Before changing project-wide macros, read:

```text
.agents/10-rules/CONFIGURATION-RULES.md
docs/configuration.md
docs/configuration.ua.md
scripts/config_contract.py
```

Do not duplicate Category A values in `project_config.h` when they belong in compiler `-D` or MPLAB `define-macros`.

Prefer runtime arguments or configuration structures for runtime policy values.

## Validation

- Use `xc8` from `PATH`.
- Do not hardcode local compiler paths.
- Use targeted/unit/regression tests only by default.
- Do not call a build verified unless the compiler completed successfully.
- Run `git diff --check` before committing.
- Update relevant agent-map entries when paths, capabilities, examples, tests, dependencies, or limitations change.
- Ukrainian documentation must be UTF-8 Cyrillic, not transliteration.

## Git and completion

Use exact file staging only.

Before commits, follow:

```text
.agents/workflow/GIT-WORKFLOW.md
```

A task requiring commits is not complete until real commit hashes appear in `git log`.

The final report must include:

- decision made;
- files changed;
- targeted tests and builds;
- documentation updated;
- commit hashes, when requested;
- final `git status --short`;
- push performed: yes/no;
- anything not verified and why.
