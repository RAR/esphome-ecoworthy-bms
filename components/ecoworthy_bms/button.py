import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
    ICON_POWER,
)

from . import CONF_ECOWORTHY_BMS_ID, EcoworthyBms, ecoworthy_bms_ns

CODEOWNERS = ["@RAR"]

DEPENDENCIES = ["ecoworthy_bms"]

StandbySleepButton = ecoworthy_bms_ns.class_("StandbySleepButton", button.Button, cg.Component)
DeepSleepButton = ecoworthy_bms_ns.class_("DeepSleepButton", button.Button, cg.Component)

CONF_STANDBY_SLEEP = "standby_sleep"
CONF_DEEP_SLEEP = "deep_sleep"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_ECOWORTHY_BMS_ID): cv.use_id(EcoworthyBms),
        cv.Optional(CONF_STANDBY_SLEEP): button.button_schema(
            StandbySleepButton,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_POWER,
        ),
        cv.Optional(CONF_DEEP_SLEEP): button.button_schema(
            DeepSleepButton,
            entity_category=ENTITY_CATEGORY_CONFIG,
            icon=ICON_POWER,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ECOWORTHY_BMS_ID])

    if CONF_STANDBY_SLEEP in config:
        b = await button.new_button(config[CONF_STANDBY_SLEEP])
        await cg.register_component(b, config[CONF_STANDBY_SLEEP])
        cg.add(b.set_parent(hub))
        cg.add(hub.set_standby_sleep_button(b))

    if CONF_DEEP_SLEEP in config:
        b = await button.new_button(config[CONF_DEEP_SLEEP])
        await cg.register_component(b, config[CONF_DEEP_SLEEP])
        cg.add(b.set_parent(hub))
        cg.add(hub.set_deep_sleep_button(b))
