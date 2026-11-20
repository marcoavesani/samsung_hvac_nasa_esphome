# samsung_hvac_nasa_esphome

A small [ESPHome](https://esphome.io) external component that lets you **write
arbitrary Samsung NASA messages** from Home Assistant, on top of the
[`samsung_ac`](https://github.com/omerfaruk-aran/esphome_samsung_hvac_bus)
component (F1/F2 bus, NASA protocol).

`samsung_ac` can read any message through `custom_sensor`, but it can only
write a fixed set of values (power, mode, target temperatures, water outlet
target, ...). This component adds:

- **`number` platform `samsung_nasa_write`**: a Home Assistant number that
  sends one NASA message when you change it, with scaling, range/step
  clamping, rate limiting and an optional state sensor to mirror the value
  the unit actually reports.
- **`write_message()`** C++ helper, callable from lambdas.

It reuses the bus, the packet encoder and the packet type (`Request`) that
`samsung_ac` itself uses for its own writes, so nothing is reimplemented.

## Main use case: water law offset (Samsung EHS)

On Samsung EHS heat pumps running on the weather curve ("water law"), the
leaving water temperature set by the user is ignored; what you can change is
the **water law offset** (NASA `0x4248`, ±5 °C on the wired controller). The
resulting target flow temperature is reported in `0x427F`.

Exposing `0x4248` as a number allows Home Assistant automations to modulate
the heat pump (for example to follow photovoltaic surplus) without switching
it on and off.

See [`example.yaml`](example.yaml).

## Installation

```yaml
external_components:
  - source: github://omerfaruk-aran/esphome_samsung_hvac_bus@<commit>
    components: [samsung_ac]
  - source: github://marcoavesani/samsung_hvac_nasa_esphome@main
    components: [samsung_nasa_write]

samsung_ac:
  id: samsung_ac_bus
  # ...
```

## `number` options

| Option | Required | Default | Description |
|---|---|---|---|
| `samsung_ac_id` | no | the only `samsung_ac` | ID of the `samsung_ac` component |
| `address` | yes | | NASA address of the target device, e.g. `"20.00.00"` |
| `message` | yes | | NASA message number, e.g. `0x4248` |
| `scale` | no | `1` | Value sent on the bus = `round(value × scale)` |
| `min_value` / `max_value` | yes | | Allowed range (values are clamped) |
| `step` | no | `1` | Values are snapped to this step |
| `state_sensor` | no | | Sensor whose value the number mirrors (recommended) |
| `min_write_interval` | no | `0s` | Minimum time between two writes; newer requests are delayed, not dropped |

Plus all the standard [number](https://esphome.io/components/number/) options
(`name`, `mode`, `unit_of_measurement`, `icon`, ...).

The payload size (ENUM / VAR / LONG) is derived from the message number by
the `samsung_ac` encoder; negative values are sent as two's complement.

## Lambda helper

```cpp
esphome::samsung_nasa_write::write_message(id(samsung_ac_bus), "20.00.00", 0x4248, -45);  // -4.5 °C
```

`raw_value` is the value as it travels on the bus (already scaled).

## Caveats

- **Use at your own risk.** Writing the wrong message to a heat pump can
  change installer settings. Start with values you can also see and revert on
  the wired controller.
- Avoid writing continuously: use `min_write_interval` and only write when
  the value actually changes. It is not known whether the unit stores these
  values in non-volatile memory.
- Developed for `0x4248` (water law offset) on a Samsung EHS TDM+ hydro
  unit (address `20.00.00`). Other messages are untested: check them on the
  wired controller before relying on them.

## Credits

Built on [omerfaruk-aran/esphome_samsung_hvac_bus](https://github.com/omerfaruk-aran/esphome_samsung_hvac_bus)
(itself derived from lanwin's `esphome_samsung_ac`). NASA message meanings from
the community protocol notes.

## License

MIT
