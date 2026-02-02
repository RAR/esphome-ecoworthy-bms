import esphome.codegen as cg
from esphome.components import ecoworthy_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["ecoworthy_modbus", "binary_sensor", "sensor", "text_sensor"]
CODEOWNERS = ["@rar"]
MULTI_CONF = True

CONF_ECOWORTHY_BMS_ID = "ecoworthy_bms_id"

DEFAULT_ADDRESS = 0x01

ecoworthy_bms_ns = cg.esphome_ns.namespace("ecoworthy_bms")
EcoworthyBms = ecoworthy_bms_ns.class_("EcoworthyBms", cg.PollingComponent, ecoworthy_modbus.EcoworthyModbusDevice)

ECOWORTHY_BMS_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ECOWORTHY_BMS_ID): cv.use_id(EcoworthyBms),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(EcoworthyBms),
        }
    )
    .extend(cv.polling_component_schema("10s"))
    .extend(ecoworthy_modbus.ecoworthy_modbus_device_schema(DEFAULT_ADDRESS))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await ecoworthy_modbus.register_ecoworthy_modbus_device(var, config)
