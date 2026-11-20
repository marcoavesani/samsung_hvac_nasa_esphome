import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor

from . import (
    CONF_SCALE,
    HAS_LISTENER,
    LISTENER_MISSING,
    binding_schema,
    samsung_nasa_write_ns,
    setup_binding,
)

DEPENDENCIES = ["samsung_ac"]

SamsungNasaSensor = samsung_nasa_write_ns.class_(
    "SamsungNasaSensor", sensor.Sensor, cg.Component
)


def require_listener_always(config):
    if not HAS_LISTENER:
        raise cv.Invalid(LISTENER_MISSING)
    return config


CONFIG_SCHEMA = cv.All(
    sensor.sensor_schema(SamsungNasaSensor)
    .extend(binding_schema(writable=False))
    .extend({cv.Optional(CONF_SCALE, default=1.0): cv.float_})
    .extend(cv.COMPONENT_SCHEMA),
    require_listener_always,
)


async def to_code(config):
    var = await sensor.new_sensor(config)
    await cg.register_component(var, config)
    await setup_binding(var, config, writable=False)
    cg.add(var.set_scale(config[CONF_SCALE]))
