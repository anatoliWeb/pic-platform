# AI Instructions Map

These files are for AI agents only. They are not a replacement for human README files and docs.

Use the map, do not read everything by default. Pick the branch that matches the task, then search for existing platform functionality before writing code.

Root entry point: [`AGENTS.md`](../AGENTS.md)

## How to use

1. Identify the task.
2. Read root `AGENTS.md`.
3. Follow the routing map to the smallest useful set of files.
4. Search the repo for an existing reusable solution.
5. Extend platform code if the logic belongs in `pic-platform`.
6. Keep project-local code only for device-specific or board-specific logic.

```mermaid
flowchart TD
    TASK[Identify task] --> MAP[Read root AGENTS.md]
    MAP --> ROUTE[Choose .agents route]
    ROUTE --> SEARCH[Search existing platform functionality]
    SEARCH --> DECIDE{Existing solution?}
    DECIDE -- yes --> USE[Use existing library/module]
    DECIDE -- almost --> EXTEND[Extend reusable platform component]
    DECIDE -- no reusable need --> LOCAL[Project-local logic only if device-specific]
    DECIDE -- reusable need --> NEWLIB[Create reusable platform library]
```
