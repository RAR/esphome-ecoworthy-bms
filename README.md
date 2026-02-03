# ESPHome Ecoworthy BMS Component

> ⚠️ **This component is currently in development and has not been tested with real hardware yet.** Use at your own risk and please report any issues or contribute fixes!

ESPHome component to monitor Ecoworthy Battery Management Systems (BMS) via RS485 Modbus RTU.

This is a native C++ implementation that communicates directly with the Ecoworthy/JBD BMS using a custom Modbus-RTU protocol, providing comprehensive monitoring of your battery pack.

## Supported Devices

- Ecoworthy 48V LiFePO4 Server Rack Batteries (ECO-LFP48100, etc.)
- JBD UP16S series BMS (UP16S010, UP16S015, etc.)
- Other JBD-based BMSes with compatible firmware

## Features

- **Native Modbus implementation** - Direct communication using the JBD protocol
- **Comprehensive monitoring** - Voltage, current, temperature, capacity, and more
- **Individual cell voltages** - Monitor all 16 cells independently
- **Temperature sensors** - MOSFET, ambient, and up to 4 cell temperature sensors
- **Status reporting** - Real-time status, faults, alarms, and balancing status
- **Charge/Discharge limits** - Dynamic CVL, CCL, DVL, DCL values
- **MOS control** - Enable/disable charge and discharge MOSFETs via switches
- **Sleep mode control** - Put BMS into standby or deep sleep via buttons
- **Configuration readout** - Read BMS configuration parameters
- **Multi-BMS support** - Connect multiple BMSes on the same RS485 bus
- **Low resource usage** - Efficient C++ implementation
- **Home Assistant integration** - Automatic device discovery and configuration

## Hardware Requirements

- ESP32 or ESP8266 board (ESP32 recommended for larger buffer size)
- RS485 to TTL converter module (e.g., MAX485, SP3485)
- Ecoworthy/JBD BMS with RS485 port

## Wiring Diagram

```
                  RS485                      UART
┌────────────┐              ┌──────────┐                ┌─────────┐
│            │              │          │<----- RX ----->│         │
│ Ecoworthy  │<-----B- ---->│  RS485   │<----- TX ----->│ ESP32/  │
│   BMS      │<---- A+ ---->│  to TTL  │<----- GND ---->│ ESP8266 │
│            │<--- GND ---->│  module  │<----- 3.3V --->│         │
│            │              │          │                │         │
└────────────┘              └──────────┘                └─────────┘
```

**Important:** Power the RS485 module with 3.3V to match the ESP's logic level.

### Ecoworthy BMS RS485 Connection

The Ecoworthy BMS typically has an RJ45 connector for RS485 communication. Refer to your specific model's documentation for pinout details.

| Signal | Description |
|--------|-------------|
| A+     | RS485 Data+ |
| B-     | RS485 Data- |
| GND    | Ground      |

## Installation

### Option 1: Using External Components (Recommended)

Add to your ESPHome YAML configuration:

```yaml
external_components:
  - source: github://rar/esphome-ecoworthy-bms@main
    refresh: 0s
```

### Option 2: Local Installation

1. Clone this repository
2. Copy the `components` folder to your ESPHome configuration directory
3. Reference it in your YAML:

```yaml
external_components:
  - source: components
```

## Configuration

### Basic Example

```yaml
uart:
  id: uart_0
  baud_rate: 9600
  tx_pin: GPIO16
  rx_pin: GPIO17
  rx_buffer_size: 512

ecoworthy_modbus:
  id: modbus0
  uart_id: uart_0

ecoworthy_bms:
  id: bms0
  address: 0x01  # Battery address (1-15)
  ecoworthy_modbus_id: modbus0
  update_interval: 10s
  battery_count: 1  # Number of batteries (1=master only, 2+=master+slaves)
```

### Multi-Battery Configuration

For systems with multiple batteries connected in parallel, you can poll all batteries from the master:

```yaml
ecoworthy_bms:
  id: bms0
  address: 0x01
  ecoworthy_modbus_id: modbus0
  update_interval: 10s
  battery_count: 3  # Polls batteries at addresses 0x01, 0x02, 0x03

sensor:
  - platform: ecoworthy_bms
    ecoworthy_bms_id: bms0
    total_voltage:
      name: "Battery 1 Voltage"
    # Slave batteries
    batteries:
      2:
        total_voltage:
          name: "Battery 2 Voltage"
        state_of_charge:
          name: "Battery 2 SOC"
      3:
        total_voltage:
          name: "Battery 3 Voltage"
        state_of_charge:
          name: "Battery 3 SOC"

binary_sensor:
  - platform: ecoworthy_bms
    ecoworthy_bms_id: bms0
    online_status:
      name: "Battery 1 Online"
    batteries:
      2:
        online_status:
          name: "Battery 2 Online"
      3:
        online_status:
          name: "Battery 3 Online"
```

**Note:** Per the protocol documentation, only Pack Status is available for slave batteries via RS485/RS232. Configuration parameters are only read from the master.

### Full Example

See [esp32-example.yaml](esp32-example.yaml) for a complete configuration with all available sensors.

## Available Sensors

### Binary Sensors

| Sensor | Description |
|--------|-------------|
| `online_status` | BMS communication status |
| `charging` | Battery is currently charging |
| `discharging` | Battery is currently discharging |
| `charging_switch` | Charge MOSFET is enabled |
| `discharging_switch` | Discharge MOSFET is enabled |
| `balancing` | Cells are currently being balanced |

### Voltage Sensors

| Sensor | Unit | Description |
|--------|------|-------------|
| `total_voltage` | V | Total pack voltage |
| `min_cell_voltage` | V | Lowest cell voltage |
| `max_cell_voltage` | V | Highest cell voltage |
| `delta_cell_voltage` | V | Difference between max and min cell voltage |
| `average_cell_voltage` | V | Average cell voltage |
| `cell_voltage_1` - `cell_voltage_16` | V | Individual cell voltages |

### Current and Power Sensors

| Sensor | Unit | Description |
|--------|------|-------------|
| `current` | A | Current (positive=charging, negative=discharging) |
| `power` | W | Power (voltage × current) |
| `charging_power` | W | Power when charging (0 when discharging) |
| `discharging_power` | W | Power when discharging (0 when charging) |

### Temperature Sensors

| Sensor | Unit | Description |
|--------|------|-------------|
| `temperature_sensor_1` - `temperature_sensor_4` | °C | Cell temperature sensors |
| `power_tube_temperature` | °C | Power tube (MOSFET) temperature |
| `ambient_temperature` | °C | Ambient temperature |
| `min_temperature` | °C | Lowest temperature reading |
| `max_temperature` | °C | Highest temperature reading |
| `avg_temperature` | °C | Average temperature |

### Capacity Sensors

| Sensor | Unit | Description |
|--------|------|-------------|
| `remaining_capacity` | Ah | Remaining capacity |
| `full_capacity` | Ah | Full capacity |
| `rated_capacity` | Ah | Rated/designed capacity |

### State Sensors

| Sensor | Unit | Description |
|--------|------|-------------|
| `state_of_charge` | % | State of charge (SOC) |
| `state_of_health` | % | State of health (SOH) |
| `cycle_count` | - | Number of charge cycles |
| `cell_count` | - | Number of cells in pack |
| `temperature_sensor_count` | - | Number of temperature sensors |

### Charge/Discharge Limit Sensors

| Sensor | Unit | Description |
|--------|------|-------------|
| `charge_voltage_limit` | V | Maximum charge voltage (CVL) |
| `charge_current_limit` | A | Maximum charge current (CCL) |
| `discharge_voltage_limit` | V | Minimum discharge voltage (DVL) |
| `discharge_current_limit` | A | Maximum discharge current (DCL) |

### Text Sensors

| Sensor | Description |
|--------|-------------|
| `operation_status` | Current operation status (Idle/Charging/Discharging) |
| `fault` | Active fault conditions |
| `alarm` | Active alarm conditions |
| `serial_number` | Battery serial number |
| `firmware_version` | BMS firmware version |
| `bms_serial_number` | BMS board serial number |
| `pack_serial_number` | Battery pack serial number |
| `manufacturer` | Manufacturer name |
| `bms_model` | BMS model identifier |
| `balance_mode` | Balance mode (Voltage/SOC) |
| `can_protocol` | Configured CAN protocol |
| `rs485_protocol` | Configured RS485 protocol |

### Configuration Sensors

| Sensor | Unit | Description |
|--------|------|-------------|
| `balance_voltage` | V | Balance start voltage threshold |
| `balance_difference` | V | Balance difference threshold |
| `heater_start_temp` | °C | Heater activation temperature |
| `heater_stop_temp` | °C | Heater deactivation temperature |
| `full_charge_voltage` | V | Full charge voltage threshold |
| `full_charge_current` | A | Full charge current threshold |
| `sleep_voltage` | V | Sleep mode voltage threshold |
| `sleep_delay` | min | Sleep mode delay |
| `total_charge` | Ah | Total charge accumulated |
| `total_discharge` | Ah | Total discharge accumulated |
| `configured_cvl` | V | Configured charge voltage limit |
| `configured_ccl` | A | Configured charge current limit |
| `configured_dvl` | V | Configured discharge voltage limit |
| `configured_dcl` | A | Configured discharge current limit |
| `shunt_resistance` | μΩ | Current shunt resistance |
| `hardware_version` | - | Hardware version |

### Slave Battery Sensors

For multi-battery setups, **all Pack Status data** is available for each slave battery. The following sensors can be configured per slave:

#### Slave Sensors

| Category | Available Sensors |
|----------|-------------------|
| **Voltage** | `total_voltage`, `min_cell_voltage`, `max_cell_voltage`, `delta_cell_voltage`, `average_cell_voltage`, `min_voltage_cell`, `max_voltage_cell`, `cell_voltage_1` through `cell_voltage_16` |
| **Current/Power** | `current`, `power`, `charging_power`, `discharging_power` |
| **Temperature** | `power_tube_temperature`, `ambient_temperature`, `min_temperature`, `max_temperature`, `avg_temperature`, `temperature_sensor_1` through `temperature_sensor_4` |
| **Capacity** | `state_of_charge`, `state_of_health`, `remaining_capacity`, `full_capacity`, `rated_capacity`, `cycle_count` |
| **Limits** | `charge_voltage_limit`, `charge_current_limit`, `discharge_voltage_limit`, `discharge_current_limit` |
| **Status** | `cell_count`, `temperature_sensor_count`, `fault_bitmask`, `alarm_bitmask`, `mosfet_status_bitmask`, `balancing_bitmask` |

#### Slave Binary Sensors

| Available Binary Sensors |
|-------------------------|
| `online_status`, `charging`, `discharging`, `charging_switch`, `discharging_switch`, `balancing` |

#### Slave Text Sensors

| Available Text Sensors |
|-----------------------|
| `operation_status`, `fault`, `alarm`, `serial_number`, `firmware_version` |

> **Note:** Configuration parameters from 0x1C00, 0x2000, and 0x2810 blocks (e.g., balance settings, manufacturer, BMS model, etc.) are only available for the master battery when using RS485.

### Switches (MOS Control)

| Switch | Description |
|--------|-------------|
| `charging` | Enable/disable charge MOSFET |
| `discharging` | Enable/disable discharge MOSFET |

> ⚠️ **Warning:** Disabling MOSFETs will disconnect the battery from the load/charger. Use with caution!

### Buttons (Sleep Control)

| Button | Description |
|--------|-------------|
| `standby_sleep` | Put BMS into standby sleep mode |
| `deep_sleep` | Put BMS into deep sleep mode |

> ⚠️ **Warning:** Deep sleep mode requires physical button press or charger connection to wake the BMS!

## Protocol Information

This component uses the JBD/Ecoworthy Modbus-RTU protocol:

- **Baud Rate**: 9600 (default)
- **Data Bits**: 8
- **Stop Bits**: 1
- **Parity**: None
- **Function Code 0x78**: Read registers
- **Function Code 0x79**: Write registers (with 0x114A4244 prefix)
- **CRC**: CRC16 with polynomial 0xA001, initial value 0xFFFF, LSB first

### Register Blocks

| Block | Address Range | Description |
|-------|---------------|-------------|
| Pack Status | 0x1000 - 0x10A0 | Real-time battery status |
| Config Block 1 | 0x1C00 - 0x1CA0 | Configuration parameters |
| Config Block 2 | 0x2000 - 0x2050 | Additional configuration |
| Product Info | 0x2810 - 0x283C | Product information |
| MOS Control | 0x2902 | Enable/disable MOSFETs |
| Sleep Mode | 0x2908 | Sleep mode control |

### Pack Status Registers (0x1000 - 0x10A0)

The main data is read from the Pack Status register block which contains:
- Pack voltage and current
- State of charge and health
- Cell voltages
- Temperature readings
- Fault and alarm status
- MOSFET status
- Charge/discharge limits
- Serial number and firmware version

## Troubleshooting

### No Response from BMS

1. Check wiring connections (A+, B-, GND)
2. Verify baud rate is 9600
3. Try swapping A+ and B- connections
4. Ensure the RS485 module is powered with 3.3V
5. Check that the correct address is configured

### CRC Errors

1. Check for loose connections
2. Reduce cable length or use shielded cable
3. Add termination resistor (120Ω) if using long cables

### Intermittent Communication

1. Increase `update_interval` to reduce bus traffic
2. Check power supply stability
3. Add decoupling capacitors near the RS485 module

## Credits

- Protocol documentation based on community research from [DIY Solar Forum](https://diysolarforum.com/)
- Register map from [dmitrych5's gist](https://gist.github.com/dmitrych5/e2fa4ef16b0b483808e4f4089846d0d0)

## License

MIT License - See [LICENSE](LICENSE) file for details.
