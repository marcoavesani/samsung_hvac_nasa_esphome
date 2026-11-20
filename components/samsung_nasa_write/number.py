import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number, sensor
from esphome.components.samsung_ac import Samsung_AC
from esphome.const import (
    CONF_ADDRESS,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_STEP,
)

from . import (
    CONF_MESSAGE,
    CONF_MIN_WRITE_INTERVAL,
    CONF_SAMSUNG_AC_ID,
    CONF_SCALE,
    CONF_STATE_SENSOR,
    samsung_nasa_write_ns,
)

DEPENDENCIES = ["samsung_ac"]

SamsungNasaWriteNumber = samsung_nasa_write_ns.class_(
    "SamsungNasaWriteNumber", number.Number, cg.Component
)


def validate_nasa_address(value):
    value = cv.string_strict(value)
    parts = value.split(".")
    if len(parts) != 3:
        raise cv.Invalid("NASA address must look like '20.00.00'")
    for part in parts:
        try:
            byte = int(part, 16)
        except ValueError as err:
            raise cv.Invalid(f"'{part}' is not a hex byte") from err
        if not 0 <= byte <= 0xFF:
            raise cv.Invalid(f"'{part}' is out of range 00..FF")
    return value


def validate_range(config):
    if config[CONF_MIN_VALUE] >= config[CONF_MAX_VALUE]:
        raise cv.Invalid("min_value must be lower than max_value")
    return config


CONFIG_SCHEMA = cv.All(
    number.number_schema(SamsungNasaWriteNumber)
    .extend(
        {
            cv.GenerateID(CONF_SAMSUNG_AC_ID): cv.use_id(Samsung_AC),
            cv.Required(CONF_ADDRESS): validate_nasa_address,
            cv.Required(CONF_MESSAGE): cv.hex_uint16_t,
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
)


async def to_code(config):
    var = await number.new_number(
        config,
        min_value=config[CONF_MIN_VALUE],
        max_value=config[CONF_MAX_VALUE],
        step=config[CONF_STEP],
    )
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SAMSUNG_AC_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_message(config[CONF_MESSAGE]))
    cg.add(var.set_scale(config[CONF_SCALE]))
    cg.add(var.set_min_write_interval(config[CONF_MIN_WRITE_INTERVAL]))

    if CONF_STATE_SENSOR in config:
        sens = await cg.get_variable(config[CONF_STATE_SENSOR])
        cg.add(var.set_state_sensor(sens))
