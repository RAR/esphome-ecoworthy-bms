import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
    ICON_FLASH,
)

from . import CONF_ECOWORTHY_BMS_ID, EcoworthyBms, ecoworthy_bms_ns

CODEOWNERS = ["@RAR"]

DEPENDENCIES = ["ecoworthy_bms"]

ChargeMosSwitch = ecoworthy_bms_ns.class_("ChargeMosSwitch", switch.Switch, cg.Component)
DischargeMosSwitch = ecoworthy_bms_ns.class_("DischargeMosSwitch", switch.Switch, cg.Component)

CONF_CHARGE_MOS = "charge_mos"
CONF_DISCHARGE_MOS = "discharge_mos"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ECOWORTHY_BMS_ID): cv.use_id(EcoworthyBms),
        cv.Optional(CONF_CHARGE_MOS): switch.switch_schema(
            ChargeMosSwitch,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_FLASH,
        ),
        cv.Optional(CONF_DISCHARGE_MOS): switch.switch_schema(
            DischargeMosSwitch,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_FLASH,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ECOWORTHY_BMS_ID])

    if CONF_CHARGE_MOS in config:
        s = await switch.new_switch(config[CONF_CHARGE_MOS])
        await cg.register_component(s, config[CONF_CHARGE_MOS])
        cg.add(s.set_parent(hub))
        cg.add(hub.set_charge_mos_switch(s))

    if CONF_DISCHARGE_MOS in config:
        s = await switch.new_switch(config[CONF_DISCHARGE_MOS])
        await cg.register_component(s, config[CONF_DISCHARGE_MOS])
        cg.add(s.set_parent(hub))
        cg.add(hub.set_discharge_mos_switch(s))
