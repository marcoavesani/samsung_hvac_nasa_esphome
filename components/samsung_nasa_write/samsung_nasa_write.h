#pragma once

#include <cstdint>
#include <string>

#include "esphome/components/samsung_ac/samsung_ac.h"

namespace esphome {
namespace samsung_nasa_write {

/// Send one NASA message to a device on the bus, using the same packet type
/// (DataType::Request) that samsung_ac uses for its own writes.
/// `raw_value` is the value as it travels on the bus (already scaled, e.g.
/// -4.5 °C on a 0.1 °C message is -45). The payload size (ENUM / VAR / LONG)
/// is derived from the message number by the samsung_ac encoder.
/// Usable from lambdas:
///   esphome::samsung_nasa_write::write_message(id(samsung_ac_bus), "20.00.00", 0x4248, -45);
void write_message(samsung_ac::Samsung_AC *bus, const std::string &address, uint16_t message, int raw_value);

}  // namespace samsung_nasa_write
}  // namespace esphome
