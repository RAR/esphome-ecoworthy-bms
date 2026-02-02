# Wiring Guide for Ecoworthy BMS

## Components Needed

1. ESP32 or ESP8266 development board
2. RS485 to TTL converter module (MAX485, SP3485, or similar)
3. Jumper wires
4. (Optional) RJ45 breakout board for easy connection

## Wiring Diagram

```
┌────────────────────────────────────────────────────────────────┐
│                         ECOWORTHY BMS                          │
│                                                                │
│                          RS485 Port                            │
│                         ┌─────────┐                            │
│                         │  RJ45   │                            │
│                         │ ┌─────┐ │                            │
│                         │ │1 2 3│ │                            │
│                         │ │4 5 6│ │                            │
│                         │ │7 8  │ │                            │
│                         │ └─────┘ │                            │
│                         └────┬────┘                            │
│                              │                                 │
└──────────────────────────────┼─────────────────────────────────┘
                               │
          ┌────────────────────┼────────────────────┐
          │                    │                    │
          │ Pin 4 (B-)   Pin 5 (A+)   Pin 3 (GND)  │
          │    │              │            │        │
          │    │              │            │        │
          └────┼──────────────┼────────────┼────────┘
               │              │            │
               │              │            │
        ┌──────┴──────────────┴────────────┴──────┐
        │                                          │
        │           RS485 to TTL Module            │
        │                                          │
        │  ┌────┐  ┌────┐  ┌────┐  ┌────┐  ┌────┐ │
        │  │ A+ │  │ B- │  │GND │  │VCC │  │ DE │ │
        │  └──┬─┘  └──┬─┘  └──┬─┘  └──┬─┘  └────┘ │
        │     │       │       │       │            │
        │  ┌──┴─┐  ┌──┴─┐  ┌──┴─┐  ┌──┴─┐  ┌────┐ │
        │  │ RO │  │ DI │  │GND │  │VCC │  │ RE │ │
        │  └──┬─┘  └──┬─┘  └──┬─┘  └──┬─┘  └────┘ │
        │     │       │       │       │            │
        └─────┼───────┼───────┼───────┼────────────┘
              │       │       │       │
              │       │       │       │
        ┌─────┴───────┴───────┴───────┴─────┐
        │                                    │
        │            ESP32 Board             │
        │                                    │
        │   RX ◄──── RO                     │
        │   TX ────► DI                     │
        │   GND ◄──► GND                    │
        │   3.3V ──► VCC                    │
        │                                    │
        └────────────────────────────────────┘
```

## Pin Connections Summary

### BMS to RS485 Module

| BMS Pin | Signal | RS485 Module |
|---------|--------|--------------|
| 4 (Blue) | B- | B- |
| 5 (Blue/White) | A+ | A+ |
| 3 (Green/White) | GND | GND |

### RS485 Module to ESP32

| RS485 Module | ESP32 | Notes |
|--------------|-------|-------|
| RO | GPIO17 (RX) | Receive data from BMS |
| DI | GPIO16 (TX) | Transmit data to BMS |
| VCC | 3.3V | **Important: Use 3.3V, not 5V** |
| GND | GND | Common ground |
| DE + RE | Connect together, then to GPIO or GND | Flow control (optional) |

## Important Notes

1. **Voltage Level**: Always power the RS485 module with 3.3V to match ESP32 logic levels.

2. **Flow Control**: For half-duplex RS485, DE and RE pins control transmit/receive mode:
   - Tie both together and connect to GPIO pin for automatic flow control
   - Or tie to GND if your module handles this automatically

3. **Cable Length**: For longer cable runs (>10m):
   - Use twisted pair cable (Cat5/Cat6 works well)
   - Add 120Ω termination resistor between A+ and B-

4. **Polarity**: If communication fails, try swapping A+ and B- connections

## ESP32 GPIO Examples

### Standard ESP32

```yaml
uart:
  tx_pin: GPIO16
  rx_pin: GPIO17
```

### ESP32-S3

```yaml
uart:
  tx_pin: GPIO17
  rx_pin: GPIO18
```

### ESP8266

```yaml
uart:
  tx_pin: GPIO1  # TX
  rx_pin: GPIO3  # RX
```

## Troubleshooting

1. **No communication**: Check A+/B- polarity, verify baud rate is 9600
2. **Intermittent data**: Check connections, reduce cable length, add termination
3. **CRC errors**: Verify wiring, check for interference, use shielded cable
