import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import ECOWORTHY_BMS_COMPONENT_SCHEMA, CONF_ECOWORTHY_BMS_ID

DEPENDENCIES = ["ecoworthy_bms"]

CONF_ONLINE_STATUS = "online_status"
CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"
# JK-BMS naming convention: charging_switch/discharging_switch for MOS status
CONF_CHARGING_SWITCH = "charging_switch"
CONF_DISCHARGING_SWITCH = "discharging_switch"
CONF_BALANCING = "balancing"
CONF_BATTERIES = "batteries"

# Schema for per-battery binary sensors (secondary batteries - all binary sensors from Pack Status)
BATTERY_BINARY_SENSOR_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:lan-connect",
        ),
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging",
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-minus",
        ),
        cv.Optional(CONF_CHARGING_SWITCH): binary_sensor.binary_sensor_schema(
            icon="mdi:electric-switch",
        ),
        cv.Optional(CONF_DISCHARGING_SWITCH): binary_sensor.binary_sensor_schema(
            icon="mdi:electric-switch",
        ),
        cv.Optional(CONF_BALANCING): binary_sensor.binary_sensor_schema(
            icon="mdi:scale-balance",
        ),
    }
)

CONFIG_SCHEMA = ECOWORTHY_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_ONLINE_STATUS): binary_sensor.binary_sensor_schema(
            icon="mdi:lan-connect",
        ),
        cv.Optional(CONF_CHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-charging",
        ),
        cv.Optional(CONF_DISCHARGING): binary_sensor.binary_sensor_schema(
            icon="mdi:battery-minus",
        ),
        cv.Optional(CONF_CHARGING_SWITCH): binary_sensor.binary_sensor_schema(
            icon="mdi:electric-switch",
        ),
        cv.Optional(CONF_DISCHARGING_SWITCH): binary_sensor.binary_sensor_schema(
            icon="mdi:electric-switch",
        ),
        cv.Optional(CONF_BALANCING): binary_sensor.binary_sensor_schema(
            icon="mdi:scale-balance",
        ),
        # Per-battery binary sensors for secondary batteries (battery_2, battery_3, etc.)
        cv.Optional(CONF_BATTERIES): cv.Schema({
            cv.int_range(min=2, max=16): BATTERY_BINARY_SENSOR_SCHEMA,
        }),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ECOWORTHY_BMS_ID])

    if CONF_ONLINE_STATUS in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_ONLINE_STATUS])
        cg.add(hub.set_online_status_binary_sensor(sens))

    if CONF_CHARGING in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_CHARGING])
        cg.add(hub.set_charging_binary_sensor(sens))

    if CONF_DISCHARGING in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_DISCHARGING])
        cg.add(hub.set_discharging_binary_sensor(sens))

    if CONF_CHARGING_SWITCH in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_CHARGING_SWITCH])
        cg.add(hub.set_charging_switch_binary_sensor(sens))

    if CONF_DISCHARGING_SWITCH in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_DISCHARGING_SWITCH])
        cg.add(hub.set_discharging_switch_binary_sensor(sens))

    if CONF_BALANCING in config:
        sens = await binary_sensor.new_binary_sensor(config[CONF_BALANCING])
        cg.add(hub.set_balancing_binary_sensor(sens))

    # Per-battery binary sensors for secondary batteries
    if CONF_BATTERIES in config:
        for battery_index, battery_config in config[CONF_BATTERIES].items():
            # YAML key is 1-based (secondary_battery_number: 2,3,...) but C++ battery_index
            # from address offset is 0-based (primary=0, secondary1=1, secondary2=2,...)
            cpp_index = battery_index - 1
            if CONF_ONLINE_STATUS in battery_config:
                sens = await binary_sensor.new_binary_sensor(battery_config[CONF_ONLINE_STATUS])
                cg.add(hub.set_secondary_battery_binary_sensor(cpp_index, "online_status", sens))
            if CONF_CHARGING in battery_config:
                sens = await binary_sensor.new_binary_sensor(battery_config[CONF_CHARGING])
                cg.add(hub.set_secondary_battery_binary_sensor(cpp_index, "charging", sens))
            if CONF_DISCHARGING in battery_config:
                sens = await binary_sensor.new_binary_sensor(battery_config[CONF_DISCHARGING])
                cg.add(hub.set_secondary_battery_binary_sensor(cpp_index, "discharging", sens))
            if CONF_CHARGING_SWITCH in battery_config:
                sens = await binary_sensor.new_binary_sensor(battery_config[CONF_CHARGING_SWITCH])
                cg.add(hub.set_secondary_battery_binary_sensor(cpp_index, "charging_switch", sens))
            if CONF_DISCHARGING_SWITCH in battery_config:
                sens = await binary_sensor.new_binary_sensor(battery_config[CONF_DISCHARGING_SWITCH])
                cg.add(hub.set_secondary_battery_binary_sensor(cpp_index, "discharging_switch", sens))
            if CONF_BALANCING in battery_config:
                sens = await binary_sensor.new_binary_sensor(battery_config[CONF_BALANCING])
                cg.add(hub.set_secondary_battery_binary_sensor(cpp_index, "balancing", sens))
