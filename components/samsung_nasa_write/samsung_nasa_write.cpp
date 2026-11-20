#include "samsung_nasa_write.h"

#include "esphome/components/samsung_ac/protocol_nasa.h"
#include "esphome/core/log.h"

namespace esphome {
namespace samsung_nasa_write {

static const char *const TAG = "samsung_nasa_write";

void write_message(samsung_ac::Samsung_AC *bus, const std::string &address, uint16_t message, int raw_value) {
  if (bus == nullptr) {
    ESP_LOGE(TAG, "samsung_ac bus not set, cannot write 0x%04X", message);
    return;
  }
  auto packet = samsung_ac::Packet::create(samsung_ac::Address::parse(address), samsung_ac::DataType::Request,
                                           static_cast<samsung_ac::MessageNumber>(message), raw_value);
  ESP_LOGD(TAG, "Write %s 0x%04X = %d", address.c_str(), message, raw_value);
  bus->publish_data(packet.command.packetNumber, packet.encode());
}

}  // namespace samsung_nasa_write
}  // namespace esphome
