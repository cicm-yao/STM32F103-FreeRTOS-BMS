# FreeRTOS Design


## Task Overview


| Task | Priority | Function |
|-|-|-|
| OLED Task | 1 | Display update |
| Battery Task | 2 | Battery data processing |
| Temperature Task | 3 | Temperature monitoring |
| Cell Task | 4 | Cell voltage acquisition |
| CAN Task | 4 | Communication |


## Design Concept

Each function is separated into independent tasks.

Benefits:

- Better modularity
- Easier maintenance
- Independent scheduling


## Resource Management

Shared peripherals should be protected when accessed by multiple tasks.

Example:

I2C bus is shared by:

- BQ76920
- OLED