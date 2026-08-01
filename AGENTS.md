# AGENTS.md

This is the AI entry map for `pic-platform`.

## What this repository is

Reusable PIC18 platform code split into `core/`, `drivers/`, `libraries/`, compiler wrappers, example projects, and docs.

## Mandatory global rules

- MUST check `git status` first.
- MUST protect pre-existing dirty files.
- MUST search existing platform functionality before creating project-local code.
- MUST keep reusable code in `pic-platform`.
- MUST keep project-specific logic in project code.
- MUST update docs when behavior changes.
- MUST run targeted builds/checks only by default.
- MUST NOT use `git add .`.
- MUST NOT push unless explicitly asked.

## AI map

Start here, then open only the needed mapper.

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
| Input/buttons | `.agents/libraries/input/README.md` |
| Actuators/motors | `.agents/libraries/actuator/README.md` |
| Debug/logging | `.agents/libraries/system/README.md` |
| Timers/timeouts | `.agents/libraries/time/README.md` |
| Examples | `.agents/examples/README.md` |
| Compiler wrappers | `.agents/compilers/README.md` |
| Build/HEX | `.agents/build/TESTING.md` |
| Documentation | `.agents/docs/DOCUMENTATION.md` |
| Git workflow | `.agents/workflow/GIT-WORKFLOW.md` |
| Checklists | `.agents/checklists/CHECKLISTS.md` |

## Start every task with

1. Check `git status`.
2. Identify the task type.
3. Read only the required instruction files from the routing map.
4. Search existing platform functionality.
5. Decide: use existing, extend existing, add reusable platform code, or keep it project-local.
6. Report the decision before changing code.

## Hard rules

- Before creating a new driver, module, HAL, helper, utility, adapter, or project-local implementation, check whether the functionality already exists in `pic-platform`, can be safely extended in `pic-platform`, or should be implemented as a reusable platform component.
- Do not duplicate reusable code inside a project.
- Do not push without a direct command.
- Do not run broad builds by default.
