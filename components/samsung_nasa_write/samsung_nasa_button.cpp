#include "samsung_nasa_button.h"

#include "esphome/core/log.h"

namespace esphome {
namespace samsung_nasa_write {

static const char *const TAG = "samsung_nasa_write.button";

void SamsungNasaButton::dump_config() {
  LOG_BUTTON("", "Samsung NASA button", this);
  this->dump_binding_config_(TAG);
  ESP_LOGCONFIG(TAG, "  Value: %d", (int) this->value_);
}

void SamsungNasaButton::press_action() {
  ESP_LOGI(TAG, "0x%04X @ %s: write %d", this->message_, this->address_.c_str(), (int) this->value_);
  this->write_raw_(this->value_);
}

}  // namespace samsung_nasa_write
}  // namespace esphome
