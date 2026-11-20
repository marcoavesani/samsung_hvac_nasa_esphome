import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch

from . import (
    CONF_OFF_VALUE,
    CONF_ON_VALUE,
    binding_schema,
    require_listener,
    samsung_nasa_write_ns,
    setup_binding,
)

DEPENDENCIES = ["samsung_ac"]

SamsungNasaSwitch = samsung_nasa_write_ns.class_(
    "SamsungNasaSwitch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = cv.All(
    switch.switch_schema(SamsungNasaSwitch, default_restore_mode="DISABLED")
    .extend(binding_schema(writable=True))
    .extend(
        {
            cv.Optional(CONF_ON_VALUE, default=1): cv.int_,
            cv.Optional(CONF_OFF_VALUE, default=0): cv.int_,
        }
    )
    .extend(cv.COMPONENT_SCHEMA),
    require_listener,
)


async def to_code(config):
    var = await switch.new_switch(config)
    await cg.register_component(var, config)
    await setup_binding(var, config, writable=True)
    cg.add(var.set_on_value(config[CONF_ON_VALUE]))
    cg.add(var.set_off_value(config[CONF_OFF_VALUE]))
