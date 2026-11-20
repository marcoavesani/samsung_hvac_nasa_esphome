#include "samsung_nasa_sensor.h"

#include "esphome/core/log.h"

namespace esphome {
namespace samsung_nasa_write {

static const char *const TAG = "samsung_nasa_write.sensor";

void SamsungNasaSensor::setup() {
  this->subscribe_([this](int64_t raw) { this->publish_state((float) raw / this->scale_); });
  if (this->poll_interval_ > 0) {
    this->set_timeout("nasa_first_poll", this->first_poll_delay_(), [this]() { this->request_read_(); });
    this->set_interval("nasa_poll", this->poll_interval_, [this]() { this->request_read_(); });
  }
}

void SamsungNasaSensor::dump_config() {
  LOG_SENSOR("", "Samsung NASA sensor", this);
  this->dump_binding_config_(TAG);
  ESP_LOGCONFIG(TAG, "  Scale: %.3f", this->scale_);
}

}  // namespace samsung_nasa_write
}  // namespace esphome
