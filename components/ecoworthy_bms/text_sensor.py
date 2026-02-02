import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv

from . import ECOWORTHY_BMS_COMPONENT_SCHEMA, CONF_ECOWORTHY_BMS_ID

DEPENDENCIES = ["ecoworthy_bms"]

CONF_OPERATION_STATUS = "operation_status"
CONF_FAULT = "fault"
CONF_ALARM = "alarm"
CONF_SERIAL_NUMBER = "serial_number"
CONF_FIRMWARE_VERSION = "firmware_version"
CONF_BMS_SERIAL_NUMBER = "bms_serial_number"
CONF_PACK_SERIAL_NUMBER = "pack_serial_number"
CONF_MANUFACTURER = "manufacturer"
CONF_BMS_MODEL = "bms_model"
CONF_BALANCE_MODE = "balance_mode"
CONF_CAN_PROTOCOL = "can_protocol"
CONF_RS485_PROTOCOL = "rs485_protocol"

CONFIG_SCHEMA = ECOWORTHY_BMS_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATION_STATUS): text_sensor.text_sensor_schema(
            icon="mdi:battery-heart-variant",
        ),
        cv.Optional(CONF_FAULT): text_sensor.text_sensor_schema(
            icon="mdi:alert-circle-outline",
        ),
        cv.Optional(CONF_ALARM): text_sensor.text_sensor_schema(
            icon="mdi:alert-outline",
        ),
        cv.Optional(CONF_SERIAL_NUMBER): text_sensor.text_sensor_schema(
            icon="mdi:identifier",
        ),
        cv.Optional(CONF_FIRMWARE_VERSION): text_sensor.text_sensor_schema(
            icon="mdi:chip",
        ),
        cv.Optional(CONF_BMS_SERIAL_NUMBER): text_sensor.text_sensor_schema(
            icon="mdi:identifier",
        ),
        cv.Optional(CONF_PACK_SERIAL_NUMBER): text_sensor.text_sensor_schema(
            icon="mdi:identifier",
        ),
        cv.Optional(CONF_MANUFACTURER): text_sensor.text_sensor_schema(
            icon="mdi:factory",
        ),
        cv.Optional(CONF_BMS_MODEL): text_sensor.text_sensor_schema(
            icon="mdi:chip",
        ),
        cv.Optional(CONF_BALANCE_MODE): text_sensor.text_sensor_schema(
            icon="mdi:scale-balance",
        ),
        cv.Optional(CONF_CAN_PROTOCOL): text_sensor.text_sensor_schema(
            icon="mdi:bus-electric",
        ),
        cv.Optional(CONF_RS485_PROTOCOL): text_sensor.text_sensor_schema(
            icon="mdi:serial-port",
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ECOWORTHY_BMS_ID])

    if CONF_OPERATION_STATUS in config:
        sens = await text_sensor.new_text_sensor(config[CONF_OPERATION_STATUS])
        cg.add(hub.set_operation_status_text_sensor(sens))

    if CONF_FAULT in config:
        sens = await text_sensor.new_text_sensor(config[CONF_FAULT])
        cg.add(hub.set_fault_text_sensor(sens))

    if CONF_ALARM in config:
        sens = await text_sensor.new_text_sensor(config[CONF_ALARM])
        cg.add(hub.set_alarm_text_sensor(sens))

    if CONF_SERIAL_NUMBER in config:
        sens = await text_sensor.new_text_sensor(config[CONF_SERIAL_NUMBER])
        cg.add(hub.set_serial_number_text_sensor(sens))

    if CONF_FIRMWARE_VERSION in config:
        sens = await text_sensor.new_text_sensor(config[CONF_FIRMWARE_VERSION])
        cg.add(hub.set_firmware_version_text_sensor(sens))

    if CONF_BMS_SERIAL_NUMBER in config:
        sens = await text_sensor.new_text_sensor(config[CONF_BMS_SERIAL_NUMBER])
        cg.add(hub.set_bms_serial_number_text_sensor(sens))

    if CONF_PACK_SERIAL_NUMBER in config:
        sens = await text_sensor.new_text_sensor(config[CONF_PACK_SERIAL_NUMBER])
        cg.add(hub.set_pack_serial_number_text_sensor(sens))

    if CONF_MANUFACTURER in config:
        sens = await text_sensor.new_text_sensor(config[CONF_MANUFACTURER])
        cg.add(hub.set_manufacturer_text_sensor(sens))

    if CONF_BMS_MODEL in config:
        sens = await text_sensor.new_text_sensor(config[CONF_BMS_MODEL])
        cg.add(hub.set_bms_model_text_sensor(sens))

    if CONF_BALANCE_MODE in config:
        sens = await text_sensor.new_text_sensor(config[CONF_BALANCE_MODE])
        cg.add(hub.set_balance_mode_text_sensor(sens))

    if CONF_CAN_PROTOCOL in config:
        sens = await text_sensor.new_text_sensor(config[CONF_CAN_PROTOCOL])
        cg.add(hub.set_can_protocol_text_sensor(sens))

    if CONF_RS485_PROTOCOL in config:
        sens = await text_sensor.new_text_sensor(config[CONF_RS485_PROTOCOL])
        cg.add(hub.set_rs485_protocol_text_sensor(sens))
