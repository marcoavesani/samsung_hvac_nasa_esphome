#include "samsung_nasa_write.h"

#include "esphome/components/samsung_ac/protocol_nasa.h"
#include "esphome/core/log.h"

namespace esphome {
namespace samsung_nasa_write {

static const char *const TAG = "samsung_nasa_write";

void write_message(samsung_ac::Samsung_AC *bus, const std::string &address, uint16_t message, int32_t raw_value,
                   uint8_t data_type) {
  if (bus == nullptr) {
    ESP_LOGE(TAG, "samsung_ac bus not set, cannot write 0x%04X", message);
    return;
  }
  auto type = data_type == NASA_DATA_TYPE_WRITE ? samsung_ac::DataType::Write : samsung_ac::DataType::Request;
  auto packet = samsung_ac::Packet::create(samsung_ac::Address::parse(address), type,
                                           static_cast<samsung_ac::MessageNumber>(message), (int) raw_value);
  ESP_LOGD(TAG, "%s %s 0x%04X = %d", data_type == NASA_DATA_TYPE_WRITE ? "Write" : "Request", address.c_str(),
           message, (int) raw_value);
  bus->publish_data(packet.command.packetNumber, packet.encode());
}

void read_message(samsung_ac::Samsung_AC *bus, const std::string &address, uint16_t message) {
  if (bus == nullptr) {
    ESP_LOGE(TAG, "samsung_ac bus not set, cannot read 0x%04X", message);
    return;
  }
  auto packet = samsung_ac::Packet::create(samsung_ac::Address::parse(address), samsung_ac::DataType::Read,
                                           static_cast<samsung_ac::MessageNumber>(message), 0);
  ESP_LOGV(TAG, "Read %s 0x%04X", address.c_str(), message);
  // The unit answers a Read with a Response, not with an Ack. publish_data()
  // would wait for an Ack, retry and log a timeout, so send it once instead
  // (same as samsung_ac does for its own display-light reads).
  bus->publish_data_once(packet.command.packetNumber, packet.encode(), []() {});
}

int64_t decode_value(uint16_t message, long raw, bool is_signed) {
  const uint8_t type = (message & 0x0600) >> 9;  // 0 ENUM, 1 VAR, 2 LONG
  switch (type) {
    case 0:
      return (uint8_t) (raw & 0xFF);
    case 1:
      return is_signed ? (int64_t) (int16_t) (raw & 0xFFFF) : (int64_t) (uint16_t) (raw & 0xFFFF);
    case 2:
      return is_signed ? (int64_t) (int32_t) (raw & 0xFFFFFFFF) : (int64_t) (uint32_t) (raw & 0xFFFFFFFF);
    default:
      return raw;
  }
}

bool NasaBinding::subscribe_(std::function<void(int64_t)> &&on_value) {
#ifdef USE_SAMSUNG_AC_NASA_MESSAGE_LISTENER
  if (this->parent_ == nullptr)
    return false;
  this->parent_->add_nasa_message_listener(
      [this, on_value = std::move(on_value)](const std::string &source, const std::string &dest, uint8_t data_type,
                                             uint16_t message_number, long value) {
        if (message_number != this->message_ || source != this->address_)
          return;
        if (data_type != NASA_DATA_TYPE_NOTIFICATION && data_type != NASA_DATA_TYPE_RESPONSE)
          return;
        on_value(decode_value(this->message_, value, this->signed_));
      });
  return true;
#else
  (void) on_value;
  return false;
#endif
}

void NasaBinding::write_raw_(int32_t raw) { write_message(this->parent_, this->address_, this->message_, raw, this->data_type_); }

void NasaBinding::request_read_() { read_message(this->parent_, this->address_, this->message_); }

void NasaBinding::dump_binding_config_(const char *tag) const {
  ESP_LOGCONFIG(tag, "  Address: %s", this->address_.c_str());
  ESP_LOGCONFIG(tag, "  Message: 0x%04X", this->message_);
  ESP_LOGCONFIG(tag, "  Data type: %s", this->data_type_ == NASA_DATA_TYPE_WRITE ? "write" : "request");
  ESP_LOGCONFIG(tag, "  Read back: %s%s", this->read_back_ ? "yes" : "no",
                has_message_listener() ? "" : " (samsung_ac without listener hook)");
  if (this->poll_interval_ > 0)
    ESP_LOGCONFIG(tag, "  Poll interval: %u ms", (unsigned) this->poll_interval_);
}

}  // namespace samsung_nasa_write
}  // namespace esphome
