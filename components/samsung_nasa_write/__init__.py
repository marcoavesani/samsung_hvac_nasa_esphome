"""Write arbitrary Samsung NASA messages through the samsung_ac component.

Companion component for https://github.com/omerfaruk-aran/esphome_samsung_hvac_bus
(samsung_ac). It reuses that component's UART bus and NASA packet encoder, so
it must be loaded together with it.
"""

import esphome.codegen as cg

CODEOWNERS = ["@marcoavesani"]
DEPENDENCIES = ["samsung_ac"]

CONF_SAMSUNG_AC_ID = "samsung_ac_id"
CONF_MESSAGE = "message"
CONF_SCALE = "scale"
CONF_STATE_SENSOR = "state_sensor"
CONF_MIN_WRITE_INTERVAL = "min_write_interval"

samsung_nasa_write_ns = cg.esphome_ns.namespace("samsung_nasa_write")
