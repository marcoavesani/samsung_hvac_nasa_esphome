# Changelog

## 0.2.1 (2026-11-20)

- Read requests are sent once (`publish_data_once`): the unit answers with a
  Response, not an Ack, so `publish_data()` retried every read and logged
  "Packet sending timeout", and each value arrived twice.

## 0.2.0 (2026-11-20)

- New platforms: `sensor` (read only), `select`, `switch`, `button`.
- New actions: `samsung_nasa_write.write` (templatable value) and
  `samsung_nasa_write.read`; C++ helper `read_message()`.
- Read-back: entities show the value the unit reports, and `poll_interval`
  asks the unit for values that are never broadcast (FSV settings). Needs the
  `samsung_ac` NASA message listener hook; detected automatically.
- `data_type: request | write` on writable entities.
- `signed` option; 1 byte (ENUM) values are always unsigned.
- Addresses are normalised to lower case hex, as printed by `samsung_ac`.

## 0.1.0 (2026-11-20)

- `number` platform writing one NASA message (`0x4248` water law offset tested).
