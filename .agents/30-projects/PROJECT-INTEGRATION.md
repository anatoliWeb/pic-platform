# Project Integration

## Allowed in a project

- device scenario;
- board-specific pin mapping;
- configuration values;
- orchestration of existing platform components;
- minimal glue code.

## Not allowed in a project

- duplicate GPIO, ADC, UART, or timer drivers;
- duplicate display library code;
- duplicate button or key-scanner logic;
- duplicate actuator logic that belongs in `position_drive` or another library;
- reusable protocol parser hidden in project code.

## Reporting rule

If project-local code is added, explain why it is not reusable platform code.
