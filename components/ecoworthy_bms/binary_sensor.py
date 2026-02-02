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
