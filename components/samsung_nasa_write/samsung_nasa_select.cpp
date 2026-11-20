#include "samsung_nasa_select.h"

#include "esphome/core/log.h"

namespace esphome {
namespace samsung_nasa_write {

static const char *const TAG = "samsung_nasa_write.select";

void SamsungNasaSelect::setup() {
  if (this->read_back_) {
    this->subscribe_([this](int64_t raw) {
      for (size_t i = 0; i < this->values_.size(); i++) {
        if (this->values_[i] == raw) {
          this->publish_state(std::string(this->option_at(i)));
          return;
        }
      }
      ESP_LOGW(TAG, "0x%04X: value %d has no matching option", this->message_, (int) raw);
    });
  }
  if (this->poll_interval_ > 0) {
    this->set_timeout("nasa_first_poll", this->first_poll_delay_(), [this]() { this->request_read_(); });
    this->set_interval("nasa_poll", this->poll_interval_, [this]() { this->request_read_(); });
  }
}

void SamsungNasaSelect::dump_config() {
  LOG_SELECT("", "Samsung NASA select", this);
  this->dump_binding_config_(TAG);
}

void SamsungNasaSelect::control(const std::string &value) {
  auto index = this->index_of(value);
  if (!index.has_value() || index.value() >= this->values_.size()) {
    ESP_LOGW(TAG, "0x%04X: unknown option '%s'", this->message_, value.c_str());
    return;
  }
  const int32_t raw = this->values_[index.value()];
  ESP_LOGI(TAG, "0x%04X @ %s: set '%s' (raw %d)", this->message_, this->address_.c_str(), value.c_str(), (int) raw);
  this->write_raw_(raw);
  this->publish_state(value);
}

}  // namespace samsung_nasa_write
}  // namespace esphome
