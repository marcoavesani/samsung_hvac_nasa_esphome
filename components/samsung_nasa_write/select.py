import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import select
from esphome.const import CONF_OPTIONS

from . import binding_schema, require_listener, samsung_nasa_write_ns, setup_binding

DEPENDENCIES = ["samsung_ac"]

SamsungNasaSelect = samsung_nasa_write_ns.class_(
    "SamsungNasaSelect", select.Select, cg.Component
)


def validate_options(value):
    value = cv.Schema({cv.int_: cv.string_strict})(value)
    if not value:
        raise cv.Invalid("At least one option is required")
    labels = list(value.values())
    if len(set(labels)) != len(labels):
        raise cv.Invalid("Option labels must be unique")
    return value


CONFIG_SCHEMA = cv.All(
    select.select_schema(SamsungNasaSelect)
    .extend(binding_schema(writable=True))
    .extend({cv.Required(CONF_OPTIONS): validate_options})
    .extend(cv.COMPONENT_SCHEMA),
    require_listener,
)


async def to_code(config):
    options = config[CONF_OPTIONS]
    var = await select.new_select(config, options=list(options.values()))
    await cg.register_component(var, config)
    await setup_binding(var, config, writable=True)
    for raw in options:
        cg.add(var.add_value(raw))
