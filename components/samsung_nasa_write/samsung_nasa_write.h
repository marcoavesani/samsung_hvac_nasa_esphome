#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "esphome/core/defines.h"
#include "esphome/components/samsung_ac/samsung_ac.h"

namespace esphome {
namespace samsung_nasa_write {

// NASA DataType values (see samsung_ac protocol_nasa.h)
static const uint8_t NASA_DATA_TYPE_READ = 1;
static const uint8_t NASA_DATA_TYPE_WRITE = 2;
static const uint8_t NASA_DATA_TYPE_REQUEST = 3;
static const uint8_t NASA_DATA_TYPE_NOTIFICATION = 4;
static const uint8_t NASA_DATA_TYPE_RESPONSE = 5;

/// Send one NASA message to a device on the bus.
/// `raw_value` is the value as it travels on the bus (already scaled, e.g.
/// -4.5 °C on a 0.1 °C message is -45). The payload size (ENUM / VAR / LONG)
/// is derived from the message number by the samsung_ac encoder.
/// `data_type` is Request (default, what samsung_ac uses for its own writes) or Write.
/// Usable from lambdas:
///   esphome::samsung_nasa_write::write_message(id(samsung_ac_bus), "20.00.00", 0x4248, -45);
void write_message(samsung_ac::Samsung_AC *bus, const std::string &address, uint16_t message, int32_t raw_value,
                   uint8_t data_type = NASA_DATA_TYPE_REQUEST);

/// Ask a device for the current value of a message (NASA Read). The answer
/// arrives as a Response packet; it is only visible to read-back entities when
/// samsung_ac has the NASA message listener hook.
void read_message(samsung_ac::Samsung_AC *bus, const std::string &address, uint16_t message);

/// Interpret a raw value as received from samsung_ac: ENUM (1 byte) is always
/// unsigned, VAR (2 bytes) and LONG (4 bytes) are sign-extended when `is_signed`.
int64_t decode_value(uint16_t message, long raw, bool is_signed);

/// True when this build can observe NASA messages (samsung_ac listener hook).
constexpr bool has_message_listener() {
#ifdef USE_SAMSUNG_AC_NASA_MESSAGE_LISTENER
  return true;
#else
  return false;
#endif
}

/// Shared address/message binding for the entities of this component.
class NasaBinding {
 public:
  void set_parent(samsung_ac::Samsung_AC *parent) { this->parent_ = parent; }
  void set_address(const std::string &address) { this->address_ = address; }
  void set_message(uint16_t message) { this->message_ = message; }
  void set_data_type(uint8_t data_type) { this->data_type_ = data_type; }
  void set_poll_interval(uint32_t ms) { this->poll_interval_ = ms; }
  void set_signed(bool is_signed) { this->signed_ = is_signed; }
  void set_read_back(bool read_back) { this->read_back_ = read_back; }

 protected:
  /// Call `on_value` (decoded raw value) for every Notification or Response
  /// from `address` carrying `message`. Returns false without the hook.
  bool subscribe_(std::function<void(int64_t)> &&on_value);
  void write_raw_(int32_t raw);
  void request_read_();
  void dump_binding_config_(const char *tag) const;
  /// Delay of the first poll after boot, staggered per message.
  uint32_t first_poll_delay_() const { return 15000 + (this->message_ % 16) * 700; }

  samsung_ac::Samsung_AC *parent_{nullptr};
  std::string address_;
  uint16_t message_{0};
  uint8_t data_type_{NASA_DATA_TYPE_REQUEST};
  uint32_t poll_interval_{0};
  bool signed_{true};
  bool read_back_{false};
};

}  // namespace samsung_nasa_write
}  // namespace esphome
