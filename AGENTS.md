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

## Task routing map

| Task | Read first |
|---|---|
| Understand repository layout | `.agents/00-map/REPOSITORY-MAP.md` |
| Understand platform architecture | `.agents/00-map/ARCHITECTURE.md` |
| Follow global rules | `.agents/10-rules/GLOBAL-RULES.md` |
| Follow coding style | `.agents/10-rules/CODING-RULES.md` |
| Follow configuration rules | `.agents/10-rules/CONFIGURATION-RULES.md` |
| Add or extend library | `.agents/20-libraries/LIBRARY-SELECTION.md`, `.agents/20-libraries/LIBRARY-DEVELOPMENT.md` |
| Integrate library into a project | `.agents/20-libraries/LIBRARY-INTEGRATION.md`, `.agents/30-projects/PROJECT-INTEGRATION.md` |
| Work with board wiring | `.agents/30-projects/BOARD-INTEGRATION.md` |
| Add an example | `.agents/40-examples/EXAMPLES.md` |
| Work with compiler wrappers | `.agents/50-compilers/COMPILER-COMPATIBILITY.md` |
| Run targeted checks | `.agents/60-build/TESTING.md` |
| Regenerate HEX | `.agents/60-build/HEX-ARTIFACTS.md` |
| Write docs | `.agents/70-docs/DOCUMENTATION.md` |
| Follow git workflow | `.agents/80-workflow/GIT-WORKFLOW.md` |
| Use checklists | `.agents/90-checklists/CHECKLISTS.md` |

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
