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

ChargingSwitch = ecoworthy_bms_ns.class_("ChargingSwitch", switch.Switch, cg.Component)
DischargingSwitch = ecoworthy_bms_ns.class_("DischargingSwitch", switch.Switch, cg.Component)

# JK-BMS naming convention: charging/discharging (not charge_mos/discharge_mos)
CONF_CHARGING = "charging"
CONF_DISCHARGING = "discharging"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ECOWORTHY_BMS_ID): cv.use_id(EcoworthyBms),
        cv.Optional(CONF_CHARGING): switch.switch_schema(
            ChargingSwitch,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_FLASH,
        ),
        cv.Optional(CONF_DISCHARGING): switch.switch_schema(
            DischargingSwitch,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_FLASH,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ECOWORTHY_BMS_ID])

    if CONF_CHARGING in config:
        s = await switch.new_switch(config[CONF_CHARGING])
        await cg.register_component(s, config[CONF_CHARGING])
        cg.add(s.set_parent(hub))
        cg.add(hub.set_charging_switch(s))

    if CONF_DISCHARGING in config:
        s = await switch.new_switch(config[CONF_DISCHARGING])
        await cg.register_component(s, config[CONF_DISCHARGING])
        cg.add(s.set_parent(hub))
        cg.add(hub.set_discharging_switch(s))
