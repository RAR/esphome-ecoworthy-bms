# Ecoworthy/JBD BMS Register Map

This document describes the Modbus-RTU register map for Ecoworthy and JBD UP16S series Battery Management Systems.

## Protocol Overview

- **Baud Rate**: 9600
- **Data Bits**: 8, Stop Bits: 1, Parity: None
- **Function Code 0x78**: Read registers
- **Function Code 0x79**: Write registers
- **CRC**: CRC16 with polynomial=0xA001, initial=0xFFFF, LSB first

## Request Frame Format

```
| Byte | Description            |
|------|------------------------|
| 0    | Address (battery #)    |
| 1    | Function code (0x78)   |
| 2-3  | Start address (MSB)    |
| 4-5  | End address (MSB)      |
| 6-7  | Data length (0x0000)   |
| 8-9  | CRC (LSB first)        |
```

## Response Frame Format

```
| Byte | Description            |
|------|------------------------|
| 0    | Address (battery #)    |
| 1    | Function code (0x78)   |
| 2-3  | Start address (MSB)    |
| 4-5  | End address (MSB)      |
| 6-7  | Data length            |
| 8+   | Data payload           |
| N-1  | CRC low byte           |
| N    | CRC high byte          |
```

## Pack Status (0x1000 - 0x10A0)

| Offset | Size | Description | Unit/Formula |
|--------|------|-------------|--------------|
| 0 | 2 | Pack voltage | V = val / 100 |
| 2 | 2 | Unknown (slave pack voltage) | V = val / 100 |
| 4 | 4 | Pack current | A = (val - 300000) / 100 |
| 8 | 2 | State of charge | % = val / 100 |
| 10 | 2 | Residual capacity | Ah = val / 100 |
| 12 | 2 | Full capacity | Ah = val / 100 |
| 14 | 2 | Rated capacity | Ah = val / 100 |
| 16 | 2 | MOSFET temperature | °C = (val - 500) / 10 |
| 18 | 2 | Ambient temperature | °C = (val - 500) / 10 |
| 20 | 2 | Operation status | 0=Idle, 1=Charging, 2=Discharging |
| 22 | 2 | State of health | % |
| 24 | 4 | Level 2 fault code | Bitmask |
| 28 | 4 | Level 1 alarm code | Bitmask |
| 32 | 2 | MOSFET state | Bitmask |
| 34 | 2 | External device state | Bitmask |
| 36 | 2 | Cycle count | Count |
| 38 | 2 | Max voltage cell # | 1-indexed |
| 40 | 2 | Max cell voltage | mV |
| 42 | 2 | Min voltage cell # | 1-indexed |
| 44 | 2 | Min cell voltage | mV |
| 46 | 2 | Average cell voltage | mV |
| 48 | 2 | Max temp sensor # | 1-indexed |
| 50 | 2 | Max cell temperature | °C = (val - 500) / 10 |
| 52 | 2 | Min temp sensor # | 1-indexed |
| 54 | 2 | Min cell temperature | °C = (val - 500) / 10 |
| 56 | 2 | Average cell temperature | °C = (val - 500) / 10 |
| 58 | 2 | Charge voltage limit | V = val / 10 |
| 60 | 2 | Charge current limit | A = val / 10 |
| 62 | 2 | Discharge voltage limit | V = val / 10 |
| 64 | 2 | Discharge current limit | A = val / 10 |
| 66 | 2 | Number of cells | Count |
| 68 | 2×n | Cell voltages | mV |
| 68+2×n | 2 | Number of temp sensors | Count |
| 70+2×n | 2×m | Cell temperatures | °C = (val - 500) / 10 |
| ... | 2 | Unknown | - |
| ... | 2 | Balance status bitmask | Bitmask |
| ... | 2 | Firmware version | Major.Minor |
| ... | 30 | Serial number | ASCII |

## Level 2 Fault Bitmask

| Bit | Description |
|-----|-------------|
| 0 | Cell over-voltage |
| 1 | Cell under-voltage |
| 2 | Pack over-voltage |
| 3 | Pack under-voltage |
| 4 | Charge over-current (slow) |
| 5 | Charge over-current (fast) |
| 6 | Discharge over-current (slow) |
| 7 | Discharge over-current (fast) |
| 8 | Charge high temperature |
| 9 | Charge low temperature |
| 10 | Discharge high temperature |
| 11 | Discharge low temperature |
| 12 | MOSFET high temperature |
| 13 | Ambient high temperature |
| 14 | Ambient low temperature |
| 15 | Cell voltage difference too large |
| 16 | Temperature difference too large |
| 17 | SOC too low |
| 18 | Short circuit protection |
| 19 | Cell offline |
| 20 | Temperature sensor failure |
| 21 | Charge MOSFET fault |
| 22 | Discharge MOSFET fault |
| 23 | AFE communication error |

## Level 1 Alarm Bitmask

| Bit | Description |
|-----|-------------|
| 0 | Cell over-voltage |
| 1 | Cell under-voltage |
| 2 | Pack over-voltage |
| 3 | Pack under-voltage |
| 4 | Charge over-current |
| 5 | Discharge over-current |
| 6 | Charge high temperature |
| 7 | Charge low temperature |
| 8 | Discharge high temperature |
| 9 | Discharge low temperature |
| 10 | MOSFET high temperature |
| 11 | Ambient high temperature |
| 12 | Ambient low temperature |
| 13 | Cell voltage difference too large |
| 14 | Temperature difference too large |
| 15 | SOC too low |
| 16 | EEP fault |
| 17 | RTC abnormal |
| 18 | Full charge protection |

## MOSFET State Bitmask

| Bit | Description |
|-----|-------------|
| 0 | Discharge MOSFET on |
| 1 | Charge MOSFET on |
| 2 | Precharge MOSFET on |
| 3 | Heat MOSFET on |
| 4 | Fan on |
| 5 | Dry contact 1 |
| 6 | Dry contact 2 |
| 7 | Limiting |

## Protection Parameters (0x1800 - 0x1900)

The protection parameters block contains voltage and current protection thresholds. This block is 208 bytes and is only available from the master battery.

| Offset | Size | Description | Unit/Formula |
|--------|------|-------------|--------------|
| 0 | 2 | Cell OVP trigger | V = val / 1000 (mV) |
| 2 | 2 | Cell OVP release | V = val / 1000 (mV) |
| 4 | 2 | Unknown | - |
| 6 | 2 | Cell OVP hard limit | V = val / 1000 (mV) |
| 8 | 2 | Unknown | - |
| 10 | 2 | Unknown | - |
| 12 | 2 | Cell UVP trigger | V = val / 1000 (mV) |
| 14 | 2 | Cell UVP release | V = val / 1000 (mV) |
| 16 | 2 | Unknown | - |
| 18 | 2 | Cell UVP hard limit | V = val / 1000 (mV) |
| 20 | 2 | Unknown | - |
| 22 | 2 | Unknown | - |
| 24 | 2 | Pack OVP trigger | V = val / 100 (cV) |
| 26 | 2 | Pack OVP release | V = val / 100 (cV) |
| 28 | 2 | Unknown | - |
| 30 | 2 | Pack OVP hard limit | V = val / 100 (cV) |
| 32 | 2 | Unknown | - |
| 34 | 2 | Unknown | - |
| 36 | 2 | Pack UVP trigger | V = val / 100 (cV) |
| 38 | 2 | Pack UVP release | V = val / 100 (cV) |
| ... | ... | Additional protection parameters | ... |

**Example values for 16S LiFePO4:**
- Cell OVP trigger: 3.600V (0x0E10 = 3600mV)
- Cell OVP release: 3.400V (0x0D48 = 3400mV)
- Cell UVP trigger: 2.700V (0x0A8C = 2700mV)
- Cell UVP release: 2.900V (0x0B54 = 2900mV)
- Pack OVP trigger: 57.60V (0x1680 = 5760cV)
- Pack OVP release: 54.40V (0x1540 = 5440cV)
- Pack UVP trigger: 42.00V (0x1068 = 4200cV)
- Pack UVP release: 48.00V (0x12C0 = 4800cV)

## References

- [DIY Solar Forum - Ecoworthy Thread](https://diysolarforum.com/threads/eco-worthy-48v-100ah-5120wh-lifepo4-server-rack-battery.92299/)
- [dmitrych5's Protocol Gist](https://gist.github.com/dmitrych5/e2fa4ef16b0b483808e4f4089846d0d0)
