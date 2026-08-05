# pic-platform AI workflow

## Purpose

This skill teaches AI how to work with the `pic-platform` reusable PIC18 library repository. It defines the operating method, not the module catalog.

The repository instruction layers have distinct roles:

- `AGENTS.md` is the repository entry point.
- This `SKILL.md` defines the AI operating workflow.
- `.agents/` contains routing maps and module-specific knowledge.
- Tests define executable behavior.
- Examples show integration lifecycle.
- Documentation explains behavior for humans.
- Source files contain the implementation.

## Use this skill when

- A task mentions PIC18F452, PIC18, XC8, MPLAB X, or `pic-platform`.
- A task uses or references `pic-platform` modules.
- A task asks for a driver, library, helper, protocol, or peripheral implementation.
- A task asks whether reusable functionality already exists.
- A task modifies an application that includes `pic-platform`.
- A task audits PIC firmware against platform conventions.
- A task adds or extends a reusable module, example, test, configuration, or documentation.

## Do not use this skill when

- The task is unrelated to PIC firmware.
- The task concerns another MCU platform and `pic-platform` is not involved.
- The task is purely schematic or mechanical and has no firmware implications.

## Mandatory startup workflow

Before any code change:

```text
1. Run git status --short.
2. Locate the repository root and confirm AGENTS.md is present.
3. Read AGENTS.md.
4. Route through the matching .agents/ map for the task domain.
5. Inspect the exact module card.
6. Inspect the module public header.
7. Inspect example.c and the standalone XC8 example project.
8. Inspect targeted tests.
9. Inspect EN/UA documentation.
10. Decide: reuse / extend / consolidate / new reusable / application-local.
11. Report the decision before changing code.
```

Do not skip these steps. Do not scan the whole repository when the agent map identifies the relevant subsystem.

## Existing-first decision model

```text
reuse existing
    ↓
extend existing with an API-compatible addition
    ↓
consolidate partial implementations
    ↓
is the functionality reusable across projects?
    ├── yes → add a new reusable platform module
    └── no  → keep it application-local
```

Before modifying a stable existing module, identify the exact missing behavior or defect. Preserve backward compatibility unless a breaking change is explicitly approved.

Never create a new library, driver, helper, adapter, timer layer, or protocol before checking the mapped existing implementation.

## Platform vs application

### Platform code belongs in pic-platform

Platform code is:

- Reusable across projects.
- Hardware-agnostic where reasonable.
- Free of product-specific state machines.
- Free of hardcoded board pins unless the module is explicitly board-specific.
- Caller-owned in state and lifecycle.
- Heap-free.
- Non-blocking by default.
- Based on wrap-safe timing where timestamps may overflow.
- Complete with header, source, `example.c`, XC8 example project, targeted tests, EN/UA documentation, `.agents` module card, mapper entry, and manifest entry.

### Application code belongs outside pic-platform

Application code includes:

- Product behavior and device-specific logic.
- Board pin bindings and project configuration.
- Machine state.
- Screens and UI flows.
- Safety policy and command handling.
- Relationships such as “if the fan stops, disable the heater”.
- Recipes and product-specific rules.

Do not copy reusable platform code into an example or application project. Do not place product policy in the platform.

## Source-of-truth hierarchy

| Source | Role |
|---|---|
| `AGENTS.md` | Repository entry point |
| This `SKILL.md` | AI operating workflow |
| `.agents/` | Routing maps and module cards |
| Module header (`.h`) | Public API contract |
| Tests | Executable behavior specification |
| `example.c` and XC8 project | Integration lifecycle reference |
| `docs/` | Human explanation |
| Source (`.c`) | Implementation |

When documentation, tests, and code conflict:

1. Do not guess.
2. Report the mismatch.
3. Use executable tests and current code as evidence of actual behavior.
4. Confirm intended behavior before changing implementation.
5. Update stale documentation only after behavior is verified.

## How to use examples

- Examples are reference integrations, not production templates.
- They show lifecycle, source lists, configuration, ISR wiring, process cadence, and ownership.
- Do not copy simulation shortcuts into production code.
- Do not treat `example.c` alone as proof of complete MPLAB project integration.
- Inspect the standalone XC8 project under `examples-projects/xc8/` for real integration and tracked build metadata.
- Keep hardware adapters and project policy outside reusable library internals.

## How to use tests

- Tests are executable behavior specifications.
- Passing structural source-text checks does not prove runtime behavior.
- State machines require executable behavior coverage where practical.
- Prefer runtime harnesses and XC8 compile/build checks.
- Add targeted regression tests after every behavior change.
- Run only targeted tests by default.
- Never run the full suite unless explicitly requested.

## Configuration contract

Before changing project-wide macros, read:

```text
.agents/10-rules/CONFIGURATION-RULES.md
docs/configuration.md
docs/configuration.ua.md
scripts/config_contract.py
```

Rules:

- Category A contains project-wide compiler defines passed through `-D` flags or MPLAB `define-macros`.
- Category B contains runtime arguments and configuration structures.
- Category C contains CONFIG-bit helper flags.
- Library `.c` translation units do not automatically see `project_config.h`.
- Do not create duplicate configuration sources of truth.
- Reusable libraries must not hardcode project pins.
- Add a new Category A define only when it must be visible to all relevant translation units.

## Compiler and build rules

- Use `xc8` from `PATH`.
- Use `scripts/build_xc8_project.py` when available.
- Do not hardcode a local compiler path.
- Build only touched examples or projects by default.
- Do not claim build success without actual compiler output.
- Report the command, result, ROM, RAM, warnings, and HEX path.
- Do not rewrite verified timer or peripheral drivers merely to simplify one example.
- Preserve tracked MPLAB project metadata and source isolation.

## Git rules

- Protect pre-existing dirty files.
- Use exact file staging only.
- Never use `git add .` or `git add -A`.
- Do not push without an explicit instruction.
- Create commits only when requested.
- A task that requires commits is not complete until real commit hashes appear in `git log`.
- Run `git diff --check` before committing.
- Report final `git status --short`.

## Comments-in-code policy

C code must contain meaningful comments where behavior is not obvious.

Comments should explain:

- Design intent and trade-offs.
- State semantics and transitions.
- Ownership and lifecycle.
- Units such as ms, us, percent, raw ADC, or bitmask.
- Blocking, non-blocking, ISR, and polling expectations.
- Wrap-safe arithmetic.
- Recovery and re-arm behavior.
- Callback behavior and event consumption.
- Fail-safe behavior.
- Non-obvious validation.
- Why a field is intentionally preserved or reset.

Do not write comments that merely restate the code, such as:

```c
/* Increment counter. */
counter++;
```

## New module completeness

A new reusable platform module normally requires:

```text
library header (.h)
library source (.c)
example.c
standalone XC8 example project
targeted executable behavior tests
verified XC8 build
EN documentation
UA documentation
.agents module card
.agents mapper entry
agents_catalog_manifest.json entry
example matrix or inventory update when required
```

Do not report a new reusable module as complete until the full module contract is satisfied.

A partial implementation is allowed only when explicitly requested. It must be clearly reported as incomplete, including missing tests, examples, builds, documentation, or integration work.

## Safety

For power-line, relay, triac, heater, and motor-control modules:

- Firmware does not provide galvanic isolation.
- Platform examples and documentation must never imply that an MCU pin can connect directly to mains voltage.
- Mains sensing and switching require suitable isolation, drivers, protection, and board-level safety measures.
- Fail-safe outputs must turn OFF on invalid, lost-control, or uninitialized states unless a documented hardware contract requires otherwise.
- Do not create hidden fallback between physically different control modes.
- Do not assume relay, triac, heater, motor, or load behavior without an explicit hardware contract.
- Keep device-specific emergency policy in the application while exposing safe reusable mechanisms from the platform.

## Common AI mistakes

- Scans the whole repository instead of using the agent map.
- Creates a duplicate module.
- Places reusable code in an application project.
- Places product policy in the platform.
- Ignores examples or tests.
- Trusts documentation without checking tests and code.
- Treats structural tests as proof of runtime behavior.
- Uses `project_config.h` as a global source for library translation units.
- Adds blocking delays to reusable library code.
- Hardcodes board pins in reusable modules.
- Rewrites stable modules without identifying a real missing behavior or defect.
- Rewrites verified timer drivers without explicit need.
- Breaks backward compatibility without approval.
- Reports completion without tests, builds, documentation, or required commits.
- Adds noisy or obvious comments.
- Copies simulation behavior into production code.

## Decision tree

```text
Task
  → classify the domain
  → open the matching .agents/ mapper
  → does a suitable module exist?
       yes → use it after inspecting header, example, tests, and docs
       partial → extend or consolidate it
       no → is the functionality reusable across projects?
               yes → add a complete reusable platform module
               no → keep it application-local
```

## Using this skill from an application repository

When working in an external PIC18 application that uses `pic-platform`:

1. Locate the `pic-platform` checkout or submodule.
2. Read `pic-platform/AGENTS.md`.
3. Follow its reference to `pic-platform/.skills/pic-platform/SKILL.md`.
4. Use `pic-platform/.agents/` routing to locate existing reusable code.
5. Inspect the selected module header, example, tests, and documentation.
6. Search existing modules before adding application-local implementation.
7. Keep board bindings, machine state, safety policy, and product behavior in the application.
8. Do not duplicate platform modules in the application tree.

## Completion checklist

```text
[ ] git status checked
[ ] repository skill and agent route followed
[ ] existing reusable module searched
[ ] module card, header, example, tests, and docs inspected
[ ] reuse / extend / consolidate / new reusable / application-local decision reported
[ ] backward compatibility considered for stable modules
[ ] executable behavior tests passed where practical
[ ] targeted XC8 builds passed
[ ] documentation and agent map updated
[ ] configuration contract respected
[ ] comments explain non-obvious behavior
[ ] git diff --check clean
[ ] exact staging used
[ ] commit hash reported when requested
[ ] push not performed unless requested
```
