# BQ76920 Driver


## Overview

The BMS controller communicates with BQ76920 through I2C.


## Functions


Implemented:

- Register read/write
- CRC8 verification
- Voltage acquisition
- Current acquisition
- Temperature acquisition


## Data Flow



BQ76920

↓

I2C Interface

↓

BQ76920 Driver

↓

Battery Data Processing

↓

Application



## SOC Estimation

SOC is estimated using voltage based lookup table.


Voltage

↓

OCV Table

↓

SOC