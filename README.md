# samsung_hvac_nasa_esphome

An [ESPHome](https://esphome.io) external component to **read and write
arbitrary Samsung NASA messages** from Home Assistant, on top of the
[`samsung_ac`](https://github.com/omerfaruk-aran/esphome_samsung_hvac_bus)
component (F1/F2 bus, NASA protocol).

`samsung_ac` can read broadcast messages through `custom_sensor`, but it can
only write a fixed set of values, and it ignores the replies to read requests.
This component adds, for any NASA message:

| Platform / action | What it does |
|---|---|
| `number` | Writes a scaled value (e.g. water law offset `0x4248`) |
| `select` | Writes one of several raw values with your own labels (e.g. disinfection day) |
| `switch` | Writes an on / off value |
| `button` | Writes a fixed value |
| `sensor` | Reads a value, also one that is never broadcast (FSV settings) |
| `samsung_nasa_write.write` | Action: write a raw value (templatable) |
| `samsung_nasa_write.read` | Action: ask the unit for a value |
| `write_message()` / `read_message()` | C++ helpers for lambdas |

It reuses the bus, the packet encoder and the packet types of `samsung_ac`;
the NASA protocol is not reimplemented.

## Reading values back: the listener hook

Writing works with any `samsung_ac`. **Reading** (the `sensor` platform,
`poll_interval`, and showing on number/select/switch the value the unit
reports) needs a `samsung_ac` that lets other components see every decoded
NASA message, including `Response` packets. That is a small patch
(`add_nasa_message_listener`, ~40 lines) available on the
[`main` branch of this fork](https://github.com/marcoavesani/esphome_samsung_hvac_bus)
and proposed upstream. The component detects it automatically; without it,
read options are rejected at config time with a clear error.

```yaml
external_components:
  # samsung_ac with the listener hook
  - source: github://marcoavesani/esphome_samsung_hvac_bus@main
    components: [samsung_ac]
  - source: github://marcoavesani/samsung_hvac_nasa_esphome@main
    components: [samsung_nasa_write]

samsung_ac:
  id: samsung_ac_bus
  # ...
```

With upstream `samsung_ac` (write only):

```yaml
  - source: github://omerfaruk-aran/esphome_samsung_hvac_bus@<commit>
    components: [samsung_ac]
```

## Common options (all platforms)

| Option | Required | Default | Description |
|---|---|---|---|
| `samsung_ac_id` | no | the only `samsung_ac` | ID of the `samsung_ac` component |
| `address` | yes | | NASA address of the device, e.g. `"20.00.00"` (hydro unit) |
| `message` | yes | | NASA message number, e.g. `0x4248` |
| `data_type` | no | `request` | `request` (what `samsung_ac` uses for its own writes) or `write`. Not for `sensor` |
| `poll_interval` | no | | Ask the unit for the value periodically (needs the hook). Needed for values that are not broadcast |
| `read_back` | no | `true` with the hook | Show the value the unit reports. Not for `sensor` / `button` |
| `signed` | no | `true` | Treat 2 and 4 byte values as signed. 1 byte (ENUM) values are always unsigned |

The payload size (1 byte ENUM, 2 byte VAR, 4 byte LONG) follows from the
message number (`(message & 0x0600) >> 9`), as in the NASA protocol.

### `number`

`scale` (bus value = round(value × scale)), `min_value`, `max_value`, `step`
(values are clamped and snapped), `min_write_interval` (newer requests are
delayed, not dropped), `state_sensor` (mirror an existing sensor instead of
read-back) and the standard [number](https://esphome.io/components/number/) options.

### `sensor`

`scale` (value = raw / scale) and the standard sensor options.

### `select`

`options`: map of raw value → label.

### `switch`

`value_on` (default `1`), `value_off` (default `0`). Restore mode defaults to
`DISABLED`, so nothing is written at boot.

### `button`

`value`: raw value written on press.

### Actions

```yaml
- samsung_nasa_write.write:
    address: "20.00.00"
    message: 0x4248
    value: !lambda return -45;   # raw, already scaled: -4.5 °C
    data_type: request           # optional
- samsung_nasa_write.read:
    address: "20.00.00"
    message: 0x4269
```

### Lambdas

```cpp
esphome::samsung_nasa_write::write_message(id(samsung_ac_bus), "20.00.00", 0x4248, -45);
esphome::samsung_nasa_write::read_message(id(samsung_ac_bus), "20.00.00", 0x4269);
```

## Useful messages (Samsung EHS hydro unit, `20.00.00`)

| Message | Meaning | Values | Notes |
|---|---|---|---|
| `0x4248` | Water law offset | −5…+5 °C, ×10, signed | Tested: written value is reported back by the unit |
| `0x427F` | Water law target flow temperature | °C ×10 | Read only, broadcast |
| `0x4099` | FSV 3041 disinfection on/off | 0 / 1 | Untested |
| `0x409A` | FSV 3042 disinfection day | 0 Sun … 6 Sat, 7 every day | Untested |
| `0x4269` | FSV 3043 disinfection start hour | 0…23 | Untested |
| `0x4095` | FSV 2091 external thermostat zone 1 | 0 no, 1–4 | Installer setting, see caveats |
| `0x406F` | Temperature reference | 0 room, 1 water out | |

Sources: NASA protocol notes of the `samsung_ac` project and community
message lists. Check every message on your wired controller before relying on it.

## Caveats

- **Use at your own risk.** Writing a wrong message can change installer
  settings. Start with values you can see and revert on the wired controller.
- Samsung manuals say to **power cycle the unit after changing the
  disinfection FSVs (3041–3046)**; a write over the bus may only take effect then.
- FSV values may be stored in non-volatile memory. Do not write them in
  loops: write only on change and use `min_write_interval`.
- Some FSVs may only accept `data_type: write`; if a `request` write is not
  reported back, try `write`.
- Changing FSV 2091 changes how the unit is controlled (external thermostats,
  water law); do it on the wired controller.

## Tests

The listener hook comes with a host test in the `samsung_ac` fork
(`test/main_test_nasa.cpp`, `test_nasa_message_listener`). This component
is validated with `esphome config` and code generation on ESPHome 2026.10
and 2026.11, with and without the hook.

## Credits

Built on [omerfaruk-aran/esphome_samsung_hvac_bus](https://github.com/omerfaruk-aran/esphome_samsung_hvac_bus)
(derived from lanwin's `esphome_samsung_ac`).

## License

MIT
