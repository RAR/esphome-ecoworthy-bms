import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_CURRENT,
    CONF_ID,
    CONF_POWER,
    CONF_TEMPERATURE,
    CONF_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    UNIT_VOLT,
    UNIT_WATT,
)

from . import ECOWORTHY_BMS_COMPONENT_SCHEMA, CONF_ECOWORTHY_BMS_ID

DEPENDENCIES = ["ecoworthy_bms"]

# Voltage sensors
CONF_TOTAL_VOLTAGE = "total_voltage"
CONF_MIN_CELL_VOLTAGE = "min_cell_voltage"
CONF_MAX_CELL_VOLTAGE = "max_cell_voltage"
CONF_DELTA_CELL_VOLTAGE = "delta_cell_voltage"
CONF_AVERAGE_CELL_VOLTAGE = "average_cell_voltage"
CONF_MIN_VOLTAGE_CELL = "min_voltage_cell"
CONF_MAX_VOLTAGE_CELL = "max_voltage_cell"
# Note: Using average_cell_voltage (not cell_average_voltage) to match JK-BMS naming
CONF_CELL_VOLTAGE_1 = "cell_voltage_1"
CONF_CELL_VOLTAGE_2 = "cell_voltage_2"
CONF_CELL_VOLTAGE_3 = "cell_voltage_3"
CONF_CELL_VOLTAGE_4 = "cell_voltage_4"
CONF_CELL_VOLTAGE_5 = "cell_voltage_5"
CONF_CELL_VOLTAGE_6 = "cell_voltage_6"
CONF_CELL_VOLTAGE_7 = "cell_voltage_7"
CONF_CELL_VOLTAGE_8 = "cell_voltage_8"
CONF_CELL_VOLTAGE_9 = "cell_voltage_9"
CONF_CELL_VOLTAGE_10 = "cell_voltage_10"
CONF_CELL_VOLTAGE_11 = "cell_voltage_11"
CONF_CELL_VOLTAGE_12 = "cell_voltage_12"
CONF_CELL_VOLTAGE_13 = "cell_voltage_13"
CONF_CELL_VOLTAGE_14 = "cell_voltage_14"
CONF_CELL_VOLTAGE_15 = "cell_voltage_15"
CONF_CELL_VOLTAGE_16 = "cell_voltage_16"

# Power sensors
CONF_CHARGING_POWER = "charging_power"
CONF_DISCHARGING_POWER = "discharging_power"

# Temperature sensors (JK-BMS naming convention)
CONF_TEMPERATURE_SENSOR_1 = "temperature_sensor_1"
CONF_TEMPERATURE_SENSOR_2 = "temperature_sensor_2"
CONF_TEMPERATURE_SENSOR_3 = "temperature_sensor_3"
CONF_TEMPERATURE_SENSOR_4 = "temperature_sensor_4"
CONF_POWER_TUBE_TEMPERATURE = "power_tube_temperature"
CONF_AMBIENT_TEMPERATURE = "ambient_temperature"
CONF_MIN_TEMPERATURE = "min_temperature"
CONF_MAX_TEMPERATURE = "max_temperature"
CONF_AVG_TEMPERATURE = "avg_temperature"

# Capacity sensors
CONF_REMAINING_CAPACITY = "remaining_capacity"
CONF_FULL_CAPACITY = "full_capacity"
CONF_RATED_CAPACITY = "rated_capacity"

# State sensors
CONF_STATE_OF_CHARGE = "state_of_charge"
CONF_STATE_OF_HEALTH = "state_of_health"
CONF_CYCLE_COUNT = "cycle_count"
CONF_CELL_COUNT = "cell_count"
CONF_TEMPERATURE_SENSOR_COUNT = "temperature_sensor_count"

# Charge/Discharge limits
CONF_CHARGE_VOLTAGE_LIMIT = "charge_voltage_limit"
CONF_CHARGE_CURRENT_LIMIT = "charge_current_limit"
CONF_DISCHARGE_VOLTAGE_LIMIT = "discharge_voltage_limit"
CONF_DISCHARGE_CURRENT_LIMIT = "discharge_current_limit"

# Bitmask sensors
CONF_FAULT_BITMASK = "fault_bitmask"
CONF_ALARM_BITMASK = "alarm_bitmask"
CONF_MOSFET_STATUS_BITMASK = "mosfet_status_bitmask"
CONF_BALANCING_BITMASK = "balancing_bitmask"

# Configuration sensors (from 0x1C00 and 0x2000 blocks)
CONF_BALANCE_VOLTAGE = "balance_voltage"
CONF_BALANCE_DIFFERENCE = "balance_difference"
CONF_HEATER_START_TEMP = "heater_start_temp"
CONF_HEATER_STOP_TEMP = "heater_stop_temp"
CONF_FULL_CHARGE_VOLTAGE = "full_charge_voltage"
CONF_FULL_CHARGE_CURRENT = "full_charge_current"
CONF_SLEEP_VOLTAGE = "sleep_voltage"
CONF_SLEEP_DELAY = "sleep_delay"
CONF_TOTAL_CHARGE = "total_charge"
CONF_TOTAL_DISCHARGE = "total_discharge"
CONF_CONFIGURED_CVL = "configured_cvl"
CONF_CONFIGURED_CCL = "configured_ccl"
CONF_CONFIGURED_DVL = "configured_dvl"
CONF_CONFIGURED_DCL = "configured_dcl"
CONF_SHUNT_RESISTANCE = "shunt_resistance"

UNIT_AMPERE_HOURS = "Ah"
UNIT_MINUTES = "min"
UNIT_MICROOHM = "μΩ"

CELL_VOLTAGE_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_VOLT,
    accuracy_decimals=3,
    device_class=DEVICE_CLASS_VOLTAGE,
    state_class=STATE_CLASS_MEASUREMENT,
)

TEMPERATURE_SCHEMA = sensor.sensor_schema(
    unit_of_measurement=UNIT_CELSIUS,
    accuracy_decimals=1,
    device_class=DEVICE_CLASS_TEMPERATURE,
    state_class=STATE_CLASS_MEASUREMENT,
)

CONFIG_SCHEMA = ECOWORTHY_BMS_COMPONENT_SCHEMA.extend(
    {
        # Voltage sensors
        cv.Optional(CONF_TOTAL_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_MIN_CELL_VOLTAGE): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_MAX_CELL_VOLTAGE): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_DELTA_CELL_VOLTAGE): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_AVERAGE_CELL_VOLTAGE): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_MIN_VOLTAGE_CELL): sensor.sensor_schema(
            accuracy_decimals=0,
        ),
        cv.Optional(CONF_MAX_VOLTAGE_CELL): sensor.sensor_schema(
            accuracy_decimals=0,
        ),
        cv.Optional(CONF_CELL_VOLTAGE_1): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_2): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_3): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_4): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_5): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_6): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_7): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_8): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_9): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_10): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_11): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_12): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_13): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_14): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_15): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_CELL_VOLTAGE_16): CELL_VOLTAGE_SCHEMA,
        # Current and power sensors
        cv.Optional(CONF_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGING_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGING_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Temperature sensors (JK-BMS naming convention)
        cv.Optional(CONF_TEMPERATURE_SENSOR_1): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_TEMPERATURE_SENSOR_2): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_TEMPERATURE_SENSOR_3): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_TEMPERATURE_SENSOR_4): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_POWER_TUBE_TEMPERATURE): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_AMBIENT_TEMPERATURE): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_MIN_TEMPERATURE): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_MAX_TEMPERATURE): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_AVG_TEMPERATURE): TEMPERATURE_SCHEMA,
        # Capacity sensors
        cv.Optional(CONF_REMAINING_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_FULL_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_RATED_CAPACITY): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # State sensors
        cv.Optional(CONF_STATE_OF_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=2,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:battery-70",
        ),
        cv.Optional(CONF_STATE_OF_HEALTH): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CYCLE_COUNT): sensor.sensor_schema(
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CELL_COUNT): sensor.sensor_schema(
            accuracy_decimals=0,
        ),
        cv.Optional(CONF_TEMPERATURE_SENSOR_COUNT): sensor.sensor_schema(
            accuracy_decimals=0,
        ),
        # Charge/Discharge limits
        cv.Optional(CONF_CHARGE_VOLTAGE_LIMIT): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CHARGE_CURRENT_LIMIT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGE_VOLTAGE_LIMIT): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_DISCHARGE_CURRENT_LIMIT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        # Bitmask sensors
        cv.Optional(CONF_FAULT_BITMASK): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:alert-circle-outline",
        ),
        cv.Optional(CONF_ALARM_BITMASK): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:alert-outline",
        ),
        cv.Optional(CONF_MOSFET_STATUS_BITMASK): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:chip",
        ),
        cv.Optional(CONF_BALANCING_BITMASK): sensor.sensor_schema(
            accuracy_decimals=0,
            icon="mdi:scale-balance",
        ),
        # Configuration sensors (from 0x1C00 and 0x2000 blocks)
        cv.Optional(CONF_BALANCE_VOLTAGE): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_BALANCE_DIFFERENCE): CELL_VOLTAGE_SCHEMA,
        cv.Optional(CONF_HEATER_START_TEMP): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_HEATER_STOP_TEMP): TEMPERATURE_SCHEMA,
        cv.Optional(CONF_FULL_CHARGE_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_FULL_CHARGE_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SLEEP_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SLEEP_DELAY): sensor.sensor_schema(
            unit_of_measurement=UNIT_MINUTES,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:timer-outline",
        ),
        cv.Optional(CONF_TOTAL_CHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:battery-plus",
        ),
        cv.Optional(CONF_TOTAL_DISCHARGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE_HOURS,
            accuracy_decimals=2,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:battery-minus",
        ),
        cv.Optional(CONF_CONFIGURED_CVL): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CONFIGURED_CCL): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CONFIGURED_DVL): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_CONFIGURED_DCL): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_SHUNT_RESISTANCE): sensor.sensor_schema(
            unit_of_measurement=UNIT_MICROOHM,
            accuracy_decimals=0,
            state_class=STATE_CLASS_MEASUREMENT,
            icon="mdi:resistor",
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ECOWORTHY_BMS_ID])

    # Voltage sensors
    if CONF_TOTAL_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_TOTAL_VOLTAGE])
        cg.add(hub.set_total_voltage_sensor(sens))
    
    if CONF_MIN_CELL_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_MIN_CELL_VOLTAGE])
        cg.add(hub.set_min_cell_voltage_sensor(sens))
    
    if CONF_MAX_CELL_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_MAX_CELL_VOLTAGE])
        cg.add(hub.set_max_cell_voltage_sensor(sens))
    
    if CONF_DELTA_CELL_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_DELTA_CELL_VOLTAGE])
        cg.add(hub.set_delta_cell_voltage_sensor(sens))
    
    if CONF_AVERAGE_CELL_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_AVERAGE_CELL_VOLTAGE])
        cg.add(hub.set_average_cell_voltage_sensor(sens))
    
    if CONF_MIN_VOLTAGE_CELL in config:
        sens = await sensor.new_sensor(config[CONF_MIN_VOLTAGE_CELL])
        cg.add(hub.set_min_voltage_cell_sensor(sens))
    
    if CONF_MAX_VOLTAGE_CELL in config:
        sens = await sensor.new_sensor(config[CONF_MAX_VOLTAGE_CELL])
        cg.add(hub.set_max_voltage_cell_sensor(sens))

    # Cell voltages
    for i in range(1, 17):
        conf_name = f"cell_voltage_{i}"
        if conf_name in config:
            sens = await sensor.new_sensor(config[conf_name])
            cg.add(hub.set_cell_voltage_sensor(i - 1, sens))

    # Current and power sensors
    if CONF_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_CURRENT])
        cg.add(hub.set_current_sensor(sens))
    
    if CONF_POWER in config:
        sens = await sensor.new_sensor(config[CONF_POWER])
        cg.add(hub.set_power_sensor(sens))
    
    if CONF_CHARGING_POWER in config:
        sens = await sensor.new_sensor(config[CONF_CHARGING_POWER])
        cg.add(hub.set_charging_power_sensor(sens))
    
    if CONF_DISCHARGING_POWER in config:
        sens = await sensor.new_sensor(config[CONF_DISCHARGING_POWER])
        cg.add(hub.set_discharging_power_sensor(sens))

    # Temperature sensors (JK-BMS naming convention)
    for i in range(1, 5):
        conf_name = f"temperature_sensor_{i}"
        if conf_name in config:
            sens = await sensor.new_sensor(config[conf_name])
            cg.add(hub.set_temperature_sensor(i - 1, sens))

    if CONF_POWER_TUBE_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_POWER_TUBE_TEMPERATURE])
        cg.add(hub.set_power_tube_temperature_sensor(sens))
    
    if CONF_AMBIENT_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_AMBIENT_TEMPERATURE])
        cg.add(hub.set_ambient_temperature_sensor(sens))
    
    if CONF_MIN_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MIN_TEMPERATURE])
        cg.add(hub.set_min_temperature_sensor(sens))
    
    if CONF_MAX_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_MAX_TEMPERATURE])
        cg.add(hub.set_max_temperature_sensor(sens))
    
    if CONF_AVG_TEMPERATURE in config:
        sens = await sensor.new_sensor(config[CONF_AVG_TEMPERATURE])
        cg.add(hub.set_avg_temperature_sensor(sens))

    # Capacity sensors
    if CONF_REMAINING_CAPACITY in config:
        sens = await sensor.new_sensor(config[CONF_REMAINING_CAPACITY])
        cg.add(hub.set_remaining_capacity_sensor(sens))
    
    if CONF_FULL_CAPACITY in config:
        sens = await sensor.new_sensor(config[CONF_FULL_CAPACITY])
        cg.add(hub.set_full_capacity_sensor(sens))
    
    if CONF_RATED_CAPACITY in config:
        sens = await sensor.new_sensor(config[CONF_RATED_CAPACITY])
        cg.add(hub.set_rated_capacity_sensor(sens))

    # State sensors
    if CONF_STATE_OF_CHARGE in config:
        sens = await sensor.new_sensor(config[CONF_STATE_OF_CHARGE])
        cg.add(hub.set_state_of_charge_sensor(sens))
    
    if CONF_STATE_OF_HEALTH in config:
        sens = await sensor.new_sensor(config[CONF_STATE_OF_HEALTH])
        cg.add(hub.set_state_of_health_sensor(sens))
    
    if CONF_CYCLE_COUNT in config:
        sens = await sensor.new_sensor(config[CONF_CYCLE_COUNT])
        cg.add(hub.set_cycle_count_sensor(sens))
    
    if CONF_CELL_COUNT in config:
        sens = await sensor.new_sensor(config[CONF_CELL_COUNT])
        cg.add(hub.set_cell_count_sensor(sens))
    
    if CONF_TEMPERATURE_SENSOR_COUNT in config:
        sens = await sensor.new_sensor(config[CONF_TEMPERATURE_SENSOR_COUNT])
        cg.add(hub.set_temperature_sensor_count_sensor(sens))

    # Charge/Discharge limits
    if CONF_CHARGE_VOLTAGE_LIMIT in config:
        sens = await sensor.new_sensor(config[CONF_CHARGE_VOLTAGE_LIMIT])
        cg.add(hub.set_charge_voltage_limit_sensor(sens))
    
    if CONF_CHARGE_CURRENT_LIMIT in config:
        sens = await sensor.new_sensor(config[CONF_CHARGE_CURRENT_LIMIT])
        cg.add(hub.set_charge_current_limit_sensor(sens))
    
    if CONF_DISCHARGE_VOLTAGE_LIMIT in config:
        sens = await sensor.new_sensor(config[CONF_DISCHARGE_VOLTAGE_LIMIT])
        cg.add(hub.set_discharge_voltage_limit_sensor(sens))
    
    if CONF_DISCHARGE_CURRENT_LIMIT in config:
        sens = await sensor.new_sensor(config[CONF_DISCHARGE_CURRENT_LIMIT])
        cg.add(hub.set_discharge_current_limit_sensor(sens))
    
    # Bitmask sensors
    if CONF_FAULT_BITMASK in config:
        sens = await sensor.new_sensor(config[CONF_FAULT_BITMASK])
        cg.add(hub.set_fault_bitmask_sensor(sens))
    
    if CONF_ALARM_BITMASK in config:
        sens = await sensor.new_sensor(config[CONF_ALARM_BITMASK])
        cg.add(hub.set_alarm_bitmask_sensor(sens))
    
    if CONF_MOSFET_STATUS_BITMASK in config:
        sens = await sensor.new_sensor(config[CONF_MOSFET_STATUS_BITMASK])
        cg.add(hub.set_mosfet_status_bitmask_sensor(sens))
    
    if CONF_BALANCING_BITMASK in config:
        sens = await sensor.new_sensor(config[CONF_BALANCING_BITMASK])
        cg.add(hub.set_balancing_bitmask_sensor(sens))

    # Configuration sensors
    if CONF_BALANCE_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_BALANCE_VOLTAGE])
        cg.add(hub.set_balance_voltage_sensor(sens))

    if CONF_BALANCE_DIFFERENCE in config:
        sens = await sensor.new_sensor(config[CONF_BALANCE_DIFFERENCE])
        cg.add(hub.set_balance_difference_sensor(sens))

    if CONF_HEATER_START_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_HEATER_START_TEMP])
        cg.add(hub.set_heater_start_temp_sensor(sens))

    if CONF_HEATER_STOP_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_HEATER_STOP_TEMP])
        cg.add(hub.set_heater_stop_temp_sensor(sens))

    if CONF_FULL_CHARGE_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_FULL_CHARGE_VOLTAGE])
        cg.add(hub.set_full_charge_voltage_sensor(sens))

    if CONF_FULL_CHARGE_CURRENT in config:
        sens = await sensor.new_sensor(config[CONF_FULL_CHARGE_CURRENT])
        cg.add(hub.set_full_charge_current_sensor(sens))

    if CONF_SLEEP_VOLTAGE in config:
        sens = await sensor.new_sensor(config[CONF_SLEEP_VOLTAGE])
        cg.add(hub.set_sleep_voltage_sensor(sens))

    if CONF_SLEEP_DELAY in config:
        sens = await sensor.new_sensor(config[CONF_SLEEP_DELAY])
        cg.add(hub.set_sleep_delay_sensor(sens))

    if CONF_TOTAL_CHARGE in config:
        sens = await sensor.new_sensor(config[CONF_TOTAL_CHARGE])
        cg.add(hub.set_total_charge_sensor(sens))

    if CONF_TOTAL_DISCHARGE in config:
        sens = await sensor.new_sensor(config[CONF_TOTAL_DISCHARGE])
        cg.add(hub.set_total_discharge_sensor(sens))

    if CONF_CONFIGURED_CVL in config:
        sens = await sensor.new_sensor(config[CONF_CONFIGURED_CVL])
        cg.add(hub.set_configured_cvl_sensor(sens))

    if CONF_CONFIGURED_CCL in config:
        sens = await sensor.new_sensor(config[CONF_CONFIGURED_CCL])
        cg.add(hub.set_configured_ccl_sensor(sens))

    if CONF_CONFIGURED_DVL in config:
        sens = await sensor.new_sensor(config[CONF_CONFIGURED_DVL])
        cg.add(hub.set_configured_dvl_sensor(sens))

    if CONF_CONFIGURED_DCL in config:
        sens = await sensor.new_sensor(config[CONF_CONFIGURED_DCL])
        cg.add(hub.set_configured_dcl_sensor(sens))

    if CONF_SHUNT_RESISTANCE in config:
        sens = await sensor.new_sensor(config[CONF_SHUNT_RESISTANCE])
        cg.add(hub.set_shunt_resistance_sensor(sens))
