#include "samsung_nasa_switch.h"

#include "esphome/core/log.h"

namespace esphome {
namespace samsung_nasa_write {

static const char *const TAG = "samsung_nasa_write.switch";

void SamsungNasaSwitch::setup() {
  if (this->read_back_) {
    this->subscribe_([this](int64_t raw) {
      if (raw == this->on_value_) {
        this->publish_state(true);
      } else if (raw == this->off_value_) {
        this->publish_state(false);
      } else {
        ESP_LOGW(TAG, "0x%04X: value %d is neither on (%d) nor off (%d)", this->message_, (int) raw,
                 (int) this->on_value_, (int) this->off_value_);
      }
    });
  }
  if (this->poll_interval_ > 0) {
    this->set_timeout("nasa_first_poll", this->first_poll_delay_(), [this]() { this->request_read_(); });
    this->set_interval("nasa_poll", this->poll_interval_, [this]() { this->request_read_(); });
  }
}

void SamsungNasaSwitch::dump_config() {
  LOG_SWITCH("", "Samsung NASA switch", this);
  this->dump_binding_config_(TAG);
  ESP_LOGCONFIG(TAG, "  On value: %d, off value: %d", (int) this->on_value_, (int) this->off_value_);
}

void SamsungNasaSwitch::write_state(bool state) {
  const int32_t raw = state ? this->on_value_ : this->off_value_;
  ESP_LOGI(TAG, "0x%04X @ %s: set %s (raw %d)", this->message_, this->address_.c_str(), state ? "ON" : "OFF",
           (int) raw);
  this->write_raw_(raw);
  this->publish_state(state);
}

}  // namespace samsung_nasa_write
}  // namespace esphome
