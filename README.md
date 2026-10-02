# STM32F103 FreeRTOS Battery Management System

A battery management system firmware project based on STM32F103,
FreeRTOS and BQ76920 battery monitor IC.

This project implements battery monitoring, SOC estimation,
OLED display and CAN communication functions.

The system consists of two firmware parts:

- BMS Controller
- CAN Gateway


## Project Overview

The BMS Controller is responsible for:

- Cell voltage acquisition
- Pack voltage measurement
- Current measurement
- NTC temperature monitoring
- SOC estimation
- OLED display
- CAN communication


The CAN Gateway provides:

- UART to CAN communication
- External command forwarding
- CAN message transmission


## Hardware Platform

MCU:

- STM32F103C8T6

Battery Monitor:

- BQ76920

Main Peripherals:

- I2C
- CAN
- UART
- OLED


## Software Architecture

Application Layer

    |
    |

Interface Layer

    |
    |

STM32 HAL

    |
    |

Hardware


## FreeRTOS Task Design

The BMS controller uses multiple tasks:

| Task | Function |
|---|---|
| OLED Task | Display battery information |
| Cell Task | Acquire cell voltage |
| Temperature Task | Process temperature data |
| Battery Task | Process battery state |
| CAN Task | Communication |


## Repository Structure

firmware

├── bms_controller

└── can_gateway


## Development Environment

- STM32CubeMX
- Keil MDK
- VS Code
- Git


## Features

- STM32 HAL based firmware
- FreeRTOS task management
- BQ76920 communication
- CAN communication
- SOC estimation
- OLED user interface


## Hardware Status

Verified functions:

- BQ76920 communication
- Cell voltage measurement
- Temperature measurement
- Current measurement
- OLED display
- CAN communication


## Limitations

This project is an embedded firmware demonstration.

It does not represent a production automotive BMS system.

Additional validation is required for:

- Safety certification
- Long-term reliability testing
- Production-level protection verification