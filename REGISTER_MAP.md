# Ecoworthy/JBD BMS Register Map

This document describes the Modbus-RTU register map for Ecoworthy and JBD UP16S series Battery Management Systems.

## Protocol Overview

- **Baud Rate**: 9600
- **Data Bits**: 8, Stop Bits: 1, Parity: None
- **Function Code 0x45**: Individual pack status (non-aggregated CCL/DCL)
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

## Individual Pack Status (Function 0x45, 0x0000 - 0x0054)

This command returns non-aggregated (individual) current limits for the battery directly connected to the port. When querying the primary battery with function 0x78, the CCL/DCL values in Pack Status are aggregated across all batteries. Function 0x45 returns the primary's own non-aggregated limits.

**Note:** This command only works for the battery directly connected to the communication port, even on Bluetooth/WiFi UART ports.

| Offset | Size | Description | Unit/Formula |
|--------|------|-------------|--------------|
| 0-95 | 96 | Unused (same as Pack Status) | - |
| 96 | 2 | Individual charge current limit | A = val / 10 |
| 98 | 2 | Individual discharge current limit | A = val / 10 |

## Pack Status (0x1000 - 0x10A0)

| Offset | Size | Description | Unit/Formula |
|--------|------|-------------|--------------|
| 0 | 2 | Pack voltage | V = val / 100 |
| 2 | 2 | Unknown (secondary pack voltage) | V = val / 100 |
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

The protection parameters block contains voltage, current, and temperature protection thresholds. This block is 208 bytes and is only available from the primary battery.

### Voltage Protection (Offsets 0-47)

Cell and pack voltage protection thresholds. Windows app CSV codes shown in parentheses.

| Offset | Size | Description | CSV | Unit/Formula |
|--------|------|-------------|-----|--------------|
| 0 | 2 | Cell OVP alarm | a1 | V = val / 1000 (mV) |
| 2 | 2 | Cell OVP alarm release | a2 | V = val / 1000 (mV) |
| 4 | 2 | Cell OVP alarm delay | a3 | ms |
| 6 | 2 | Cell OVP protect (L1) | a4 | V = val / 1000 (mV) |
| 8 | 2 | Cell OVP protect release | a5 | V = val / 1000 (mV) |
| 10 | 2 | Cell OVP protect delay | a6 | ms |
| 12 | 2 | Cell UVP alarm | b1 | V = val / 1000 (mV) |
| 14 | 2 | Cell UVP alarm release | b2 | V = val / 1000 (mV) |
| 16 | 2 | Cell UVP alarm delay | b3 | ms |
| 18 | 2 | Cell UVP protect (L1) | b4 | V = val / 1000 (mV) |
| 20 | 2 | Cell UVP protect release | b5 | V = val / 1000 (mV) |
| 22 | 2 | Cell UVP protect delay | b6 | ms |
| 24 | 2 | Pack OVP alarm | c1 | V = val / 100 (cV) |
| 26 | 2 | Pack OVP alarm release | c2 | V = val / 100 (cV) |
| 28 | 2 | Pack OVP alarm delay | c3 | ms |
| 30 | 2 | Pack OVP protect (L1) | c4 | V = val / 100 (cV) |
| 32 | 2 | Pack OVP protect release | c5 | V = val / 100 (cV) |
| 34 | 2 | Pack OVP protect delay | c6 | ms |
| 36 | 2 | Pack UVP alarm | d1 | V = val / 100 (cV) |
| 38 | 2 | Pack UVP alarm release | d2 | V = val / 100 (cV) |
| 40 | 2 | Pack UVP alarm delay | d3 | ms |
| 42 | 2 | Pack UVP protect (L1) | d4 | V = val / 100 (cV) |
| 44 | 2 | Pack UVP protect release | d5 | V = val / 100 (cV) |
| 46 | 2 | Pack UVP protect delay | d6 | ms |

### Current Protection (Offsets 48-87)

Charge and discharge overcurrent protection. Values are stored as deciamps (dA), divide by 10 for Amps.

| Offset | Size | Description | CSV | Unit/Formula |
|--------|------|-------------|-----|--------------|
| 48 | 2 | Charge OC alarm (L1) | e1 | A = val / 10 |
| 50 | 2 | Reserved | - | - |
| 52 | 2 | Charge OC alarm delay | e3 | ms |
| 54 | 2 | Charge OC protect (L1) | e4 | A = val / 10 |
| 56 | 2 | Charge OC protect delay | e5 | ms |
| 58 | 2 | Charge OC recover delay | e6 | s = val / 1000 |
| 60 | 2 | Reserved | - | - |
| 62 | 2 | Charge OC2 protect (L2) | f1 | A = val / 10 |
| 64 | 2 | Charge OC2 protect delay | f2 | ms |
| 66 | 2 | Reserved | - | - |
| 68 | 2 | Discharge OC alarm (L1) | g1 | A = val / 10 |
| 70 | 2 | Reserved | - | - |
| 72 | 2 | Discharge OC alarm delay | g3 | ms |
| 74 | 2 | Discharge OC protect (L1) | g4 | A = val / 10 |
| 76 | 2 | Discharge OC protect delay | g5 | ms |
| 78 | 2 | Discharge OC recover delay | g6 | s = val / 1000 |
| 80 | 2 | Reserved | - | - |
| 82 | 2 | Discharge OC2 protect (L2) | h1 | A = val / 10 |
| 84 | 2 | Discharge OC2 protect delay | h2 | ms |
| 86 | 2 | Reserved | - | - |

### Temperature Protection (Offsets 88-135)

Temperature formula: `°C = (raw - 500) / 10`
Delay formula: `seconds = raw / 1000`

| Offset | Size | Description | CSV | Unit/Formula |
|--------|------|-------------|-----|--------------|
| 88 | 2 | Charge OT alarm | i1 | °C = (val - 500) / 10 |
| 90 | 2 | Charge OT alarm release | i2 | °C = (val - 500) / 10 |
| 92 | 2 | Charge OT alarm delay | i3 | s = val / 1000 |
| 94 | 2 | Charge OT protect | i4 | °C = (val - 500) / 10 |
| 96 | 2 | Charge OT protect release | i5 | °C = (val - 500) / 10 |
| 98 | 2 | Charge OT protect delay | i6 | s = val / 1000 |
| 100 | 2 | Charge UT alarm | j1 | °C = (val - 500) / 10 |
| 102 | 2 | Charge UT alarm release | j2 | °C = (val - 500) / 10 |
| 104 | 2 | Charge UT alarm delay | j3 | s = val / 1000 |
| 106 | 2 | Charge UT protect | j4 | °C = (val - 500) / 10 |
| 108 | 2 | Charge UT protect release | j5 | °C = (val - 500) / 10 |
| 110 | 2 | Charge UT protect delay | j6 | s = val / 1000 |
| 112 | 2 | Discharge OT alarm | k1 | °C = (val - 500) / 10 |
| 114 | 2 | Discharge OT alarm release | k2 | °C = (val - 500) / 10 |
| 116 | 2 | Discharge OT alarm delay | k3 | s = val / 1000 |
| 118 | 2 | Discharge OT protect | k4 | °C = (val - 500) / 10 |
| 120 | 2 | Discharge OT protect release | k5 | °C = (val - 500) / 10 |
| 122 | 2 | Discharge OT protect delay | k6 | s = val / 1000 |
| 124 | 2 | Discharge UT alarm | l1 | °C = (val - 500) / 10 |
| 126 | 2 | Discharge UT alarm release | l2 | °C = (val - 500) / 10 |
| 128 | 2 | Discharge UT alarm delay | l3 | s = val / 1000 |
| 130 | 2 | Discharge UT protect | l4 | °C = (val - 500) / 10 |
| 132 | 2 | Discharge UT protect release | l5 | °C = (val - 500) / 10 |
| 134 | 2 | Discharge UT protect delay | l6 | s = val / 1000 |

### Example Values for 16S 100Ah LiFePO4

**Voltage protection:**
- Cell OVP alarm: 3.55V, protect: 3.60V
- Cell UVP alarm: 2.75V, protect: 2.70V
- Pack OVP alarm: 56.80V, protect: 57.60V
- Pack UVP alarm: 44.00V, protect: 42.00V

**Current protection:**
- Charge OC alarm: 105A (e1), L1 protect: 120A (e4), L2 protect: 130A (f1)
- Discharge OC alarm: 105A (g1), L1 protect: 120A (g4), L2 protect: 140A (h1)
- Short circuit: 1236A threshold (z028), 125μs delay (z029) - stored in 0x2000

**Temperature protection:**
- Charge OT: 55°C alarm, 65°C protect
- Charge UT: 5°C alarm, 0°C protect
- Discharge OT: 60°C alarm, 65°C protect
- Discharge UT: -15°C alarm, -20°C protect

## Configuration Block 1 (0x1C00 - 0x1CA0)

Configuration and identification data. This block is 136 bytes and is only available from the primary battery.

**Note:** Ecoworthy offsets are shifted by 4 bytes compared to some EG4 documentation.

| Offset | Size | Description | Unit/Formula |
|--------|------|-------------|--------------|
| 0-3 | 4 | Unknown/Reserved | - |
| 4 | 2 | Balance trigger voltage | V = val / 1000 (mV) |
| 6 | 2 | Balance difference voltage | V = val / 1000 (mV) |
| 8 | 2 | Heater start temp | °C = (val - 500) / 10 |
| 10 | 2 | Heater stop temp | °C = (val - 500) / 10 |
| 12 | 2 | Full charge voltage | V = val / 100 |
| 14 | 2 | Full charge current | A = val / 100 |
| 16 | 26 | Serial number | ASCII string |
| 46 | 2 | Manufacturing year | Year |
| 48 | 2 | Manufacturing month | Month (1-12) |
| 50 | 2 | Manufacturing day | Day (1-31) |
| 52 | 12 | Manufacturer/Model code | ASCII string |
| 64+ | ... | Reserved/Unknown | - |

**Example values:**
- Balance trigger: 3.35V (0x0D16 = 3350mV)
- Balance diff: 0.010V (0x000A = 10mV)
- Heater start: 0°C (0x01F4 = 500 raw)
- Heater stop: 10°C (0x0258 = 600 raw)
- Full charge voltage: 56.80V (0x1630 = 5680cV)
- Full charge current: 15.0A (0x05DC = 1500cA)
- Serial: "UP16S0190001242250713001"
- Mfg date: 2025-07-29
- Manufacturer: "JBD481000000"

## Write Registers

### MOS Control (0x2902)

Controls the charge and discharge MOSFETs.

| Bit | Description |
|-----|-------------|
| 0 | Discharge MOSFET |
| 1 | Charge MOSFET |
| 2 | Precharge MOSFET |
| 3 | Heat MOSFET |
| 4 | Fan |
| 5 | Charge Limit |

### Dry Contact / Trip Control (0x2904)

Controls dry contacts and emergency trip function.

| Bit | Description |
|-----|-------------|
| 0 | Dry contact 1 |
| 1 | Dry contact 2 |
| 2 | ADDR OUT |
| 3 | **Trip** (emergency disconnect) |

**Example command to trip the breaker:**
```
TX: 01 79 29 04 29 06 00 06 26 4A 42 44 00 08 [CRC]
    │  │  │     │     │     │              └── Value: 0x0008 (bit 3 = trip)
    │  │  │     │     │     └── Identifier: 0x26 'J' 'B' 'D'
    │  │  │     │     └── Data length: 6 bytes
    │  │  │     └── End address: 0x2906
    │  │  └── Start address: 0x2904
    │  └── Function: 0x79 (write)
    └── Address: 0x01 (battery #1)

RX: 01 79 29 04 29 06 00 00 [CRC]  (acknowledgment)
```

### Sleep Mode (0x2908)

| Value | Description |
|-------|-------------|
| 0xA501 | Standby sleep |
| 0xA502 | Deep sleep |

## References

- [DIY Solar Forum - Ecoworthy Thread](https://diysolarforum.com/threads/eco-worthy-48v-100ah-5120wh-lifepo4-server-rack-battery.92299/)
- [dmitrych5's Protocol Gist](https://gist.github.com/dmitrych5/e2fa4ef16b0b483808e4f4089846d0d0)
