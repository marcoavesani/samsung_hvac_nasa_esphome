"""Read and write arbitrary Samsung NASA messages through the samsung_ac component.

Companion component for https://github.com/omerfaruk-aran/esphome_samsung_hvac_bus
(``samsung_ac``). It reuses that component's UART bus and NASA packet encoder.

Writing works with any ``samsung_ac``. Reading values back (``sensor``
platform, ``poll_interval``, read-back on number/select/switch) needs a
``samsung_ac`` that exposes the NASA message listener hook
(``add_nasa_message_listener``), see the README.
"""

from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import samsung_ac as samsung_ac_component
from esphome.components.samsung_ac import Samsung_AC
from esphome.const import CONF_ADDRESS, CONF_VALUE

CODEOWNERS = ["@marcoavesani"]
DEPENDENCIES = ["samsung_ac"]

CONF_SAMSUNG_AC_ID = "samsung_ac_id"
CONF_MESSAGE = "message"
CONF_SCALE = "scale"
CONF_STATE_SENSOR = "state_sensor"
CONF_MIN_WRITE_INTERVAL = "min_write_interval"
CONF_DATA_TYPE = "data_type"
CONF_POLL_INTERVAL = "poll_interval"
CONF_SIGNED = "signed"
CONF_READ_BACK = "read_back"
CONF_ON_VALUE = "value_on"
CONF_OFF_VALUE = "value_off"

# True when the samsung_ac in use exposes the NASA message listener hook.
HAS_LISTENER = bool(getattr(samsung_ac_component, "NASA_MESSAGE_LISTENER", False))

LISTENER_MISSING = (
    "Reading NASA values needs a samsung_ac component with the NASA message "
    "listener hook (add_nasa_message_listener). Use a samsung_ac version that "
    "includes it, or remove this option."
)

samsung_nasa_write_ns = cg.esphome_ns.namespace("samsung_nasa_write")
NasaWriteAction = samsung_nasa_write_ns.class_("NasaWriteAction", automation.Action)
NasaReadAction = samsung_nasa_write_ns.class_("NasaReadAction", automation.Action)

# NASA DataType values (protocol_nasa.h)
DATA_TYPES = {"write": 2, "request": 3}


def validate_nasa_address(value):
    value = cv.string_strict(value)
    parts = value.split(".")
    if len(parts) != 3:
        raise cv.Invalid("NASA address must look like '20.00.00'")
    out = []
    for part in parts:
        try:
            byte = int(part, 16)
        except ValueError as err:
            raise cv.Invalid(f"'{part}' is not a hex byte") from err
        if not 0 <= byte <= 0xFF:
            raise cv.Invalid(f"'{part}' is out of range 00..ff")
        out.append(f"{byte:02x}")
    # samsung_ac prints addresses in lower case hex
    return ".".join(out)


def require_listener(config):
    if HAS_LISTENER:
        return config
    if config.get(CONF_POLL_INTERVAL) is not None:
        raise cv.Invalid(LISTENER_MISSING, path=[CONF_POLL_INTERVAL])
    if config.get(CONF_READ_BACK) is True:
        raise cv.Invalid(LISTENER_MISSING, path=[CONF_READ_BACK])
    return config


def binding_schema(*, writable: bool):
    schema = {
        cv.GenerateID(CONF_SAMSUNG_AC_ID): cv.use_id(Samsung_AC),
        cv.Required(CONF_ADDRESS): validate_nasa_address,
        cv.Required(CONF_MESSAGE): cv.hex_uint16_t,
        cv.Optional(CONF_SIGNED, default=True): cv.boolean,
        cv.Optional(CONF_POLL_INTERVAL): cv.positive_time_period_milliseconds,
    }
    if writable:
        schema[cv.Optional(CONF_DATA_TYPE, default="request")] = cv.enum(
            DATA_TYPES, lower=True
        )
        # Default: read back whenever the hook is available.
        schema[cv.Optional(CONF_READ_BACK)] = cv.boolean
    return cv.Schema(schema)


async def setup_binding(var, config, *, writable: bool, read_back=None):
    parent = await cg.get_variable(config[CONF_SAMSUNG_AC_ID])
    cg.add(var.set_parent(parent))
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_message(config[CONF_MESSAGE]))
    cg.add(var.set_signed(config[CONF_SIGNED]))
    if config.get(CONF_POLL_INTERVAL) is not None:
        cg.add(var.set_poll_interval(config[CONF_POLL_INTERVAL]))
    if writable:
        cg.add(var.set_data_type(config[CONF_DATA_TYPE]))
    if read_back is None:
        read_back = config.get(CONF_READ_BACK, HAS_LISTENER) if writable else True
    cg.add(var.set_read_back(read_back))


# ---------------------------------------------------------------------------
# Actions
# ---------------------------------------------------------------------------
WRITE_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SAMSUNG_AC_ID): cv.use_id(Samsung_AC),
        cv.Required(CONF_ADDRESS): validate_nasa_address,
        cv.Required(CONF_MESSAGE): cv.hex_uint16_t,
        cv.Required(CONF_VALUE): cv.templatable(cv.int_),
        cv.Optional(CONF_DATA_TYPE, default="request"): cv.enum(DATA_TYPES, lower=True),
    }
)


@automation.register_action(
    "samsung_nasa_write.write", NasaWriteAction, WRITE_ACTION_SCHEMA, synchronous=True
)
async def nasa_write_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_SAMSUNG_AC_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_message(config[CONF_MESSAGE]))
    cg.add(var.set_data_type(config[CONF_DATA_TYPE]))
    if cg.is_template(config[CONF_VALUE]):
        templ = await cg.templatable(config[CONF_VALUE], args, cg.int32)
        cg.add(var.set_value_lambda(templ))
    else:
        cg.add(var.set_value(config[CONF_VALUE]))
    return var


READ_ACTION_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SAMSUNG_AC_ID): cv.use_id(Samsung_AC),
        cv.Required(CONF_ADDRESS): validate_nasa_address,
        cv.Required(CONF_MESSAGE): cv.hex_uint16_t,
    }
)


@automation.register_action(
    "samsung_nasa_write.read", NasaReadAction, READ_ACTION_SCHEMA, synchronous=True
)
async def nasa_read_action_to_code(config, action_id, template_arg, args):
    parent = await cg.get_variable(config[CONF_SAMSUNG_AC_ID])
    var = cg.new_Pvariable(action_id, template_arg, parent)
    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_message(config[CONF_MESSAGE]))
    return var
