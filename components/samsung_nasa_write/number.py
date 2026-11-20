import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number, sensor
from esphome.const import CONF_MAX_VALUE, CONF_MIN_VALUE, CONF_STEP

from . import (
    CONF_MIN_WRITE_INTERVAL,
    CONF_SCALE,
    CONF_STATE_SENSOR,
    binding_schema,
    require_listener,
    samsung_nasa_write_ns,
    setup_binding,
)

DEPENDENCIES = ["samsung_ac"]

SamsungNasaWriteNumber = samsung_nasa_write_ns.class_(
    "SamsungNasaWriteNumber", number.Number, cg.Component
)


def validate_range(config):
    if config[CONF_MIN_VALUE] >= config[CONF_MAX_VALUE]:
        raise cv.Invalid("min_value must be lower than max_value")
    return config


CONFIG_SCHEMA = cv.All(
    number.number_schema(SamsungNasaWriteNumber)
    .extend(binding_schema(writable=True))
    .extend(
        {
            cv.Optional(CONF_SCALE, default=1.0): cv.float_,
            cv.Required(CONF_MIN_VALUE): cv.float_,
            cv.Required(CONF_MAX_VALUE): cv.float_,
            cv.Optional(CONF_STEP, default=1.0): cv.positive_float,
            cv.Optional(CONF_STATE_SENSOR): cv.use_id(sensor.Sensor),
            cv.Optional(
                CONF_MIN_WRITE_INTERVAL, default="0s"
            ): cv.positive_time_period_milliseconds,
        }
    )
    .extend(cv.COMPONENT_SCHEMA),
    validate_range,
    require_listener,
)


async def to_code(config):
    var = await number.new_number(
        config,
        min_value=config[CONF_MIN_VALUE],
        max_value=config[CONF_MAX_VALUE],
        step=config[CONF_STEP],
    )
    await cg.register_component(var, config)
    await setup_binding(var, config, writable=True)
    cg.add(var.set_scale(config[CONF_SCALE]))
    cg.add(var.set_min_write_interval(config[CONF_MIN_WRITE_INTERVAL]))
    if CONF_STATE_SENSOR in config:
        sens = await cg.get_variable(config[CONF_STATE_SENSOR])
        cg.add(var.set_state_sensor(sens))
