import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.components.samsung_ac import Samsung_AC
from esphome.const import CONF_ADDRESS, CONF_VALUE

from . import (
    CONF_DATA_TYPE,
    CONF_MESSAGE,
    CONF_SAMSUNG_AC_ID,
    DATA_TYPES,
    samsung_nasa_write_ns,
    setup_binding,
    validate_nasa_address,
)

DEPENDENCIES = ["samsung_ac"]

SamsungNasaButton = samsung_nasa_write_ns.class_(
    "SamsungNasaButton", button.Button, cg.Component
)

CONFIG_SCHEMA = (
    button.button_schema(SamsungNasaButton)
    .extend(
        {
            cv.GenerateID(CONF_SAMSUNG_AC_ID): cv.use_id(Samsung_AC),
            cv.Required(CONF_ADDRESS): validate_nasa_address,
            cv.Required(CONF_MESSAGE): cv.hex_uint16_t,
            cv.Optional(CONF_DATA_TYPE, default="request"): cv.enum(DATA_TYPES, lower=True),
            cv.Required(CONF_VALUE): cv.int_,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await button.new_button(config)
    await cg.register_component(var, config)
    config.setdefault("signed", True)
    await setup_binding(var, config, writable=True, read_back=False)
    cg.add(var.set_value(config[CONF_VALUE]))
