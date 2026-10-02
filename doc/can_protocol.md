# CAN Communication


## Overview

CAN communication is used between:


CAN Gateway

    |

    |

BMS Controller



## CAN Gateway

Responsibilities:

- Receive external commands
- Convert UART data
- Send CAN messages


## BMS Controller

Responsibilities:

- Receive CAN commands
- Send battery information


## Frame Format

Standard CAN frame:

- CAN ID
- Data length
- Data bytes