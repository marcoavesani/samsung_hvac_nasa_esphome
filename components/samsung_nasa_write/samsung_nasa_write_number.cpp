#include "samsung_nasa_write_number.h"

#include <cmath>

#include "esphome/core/hal.h"
#include "esphome/core/log.h"

namespace esphome {
namespace samsung_nasa_write {

static const char *const TAG = "samsung_nasa_write.number";

void SamsungNasaWriteNumber::setup() {
  if (this->state_sensor_ != nullptr) {
    this->state_sensor_->add_on_state_callback([this](float value) {
      if (!std::isnan(value))
        this->publish_state(value);
    });
    if (this->state_sensor_->has_state() && !std::isnan(this->state_sensor_->state))
      this->publish_state(this->state_sensor_->state);
  } else if (this->read_back_) {
    this->subscribe_([this](int64_t raw) { this->publish_state((float) raw / this->scale_); });
  }
  if (this->poll_interval_ > 0) {
    this->set_timeout("nasa_first_poll", this->first_poll_delay_(), [this]() { this->request_read_(); });
    this->set_interval("nasa_poll", this->poll_interval_, [this]() { this->request_read_(); });
  }
}

void SamsungNasaWriteNumber::dump_config() {
  LOG_NUMBER("", "Samsung NASA number", this);
  this->dump_binding_config_(TAG);
  ESP_LOGCONFIG(TAG, "  Scale: %.3f", this->scale_);
  ESP_LOGCONFIG(TAG, "  Min write interval: %u ms", (unsigned) this->min_write_interval_);
  if (this->state_sensor_ != nullptr)
    LOG_SENSOR("  ", "State sensor", this->state_sensor_);
}

float SamsungNasaWriteNumber::snap_(float value) const {
  const float min_v = this->traits.get_min_value();
  const float max_v = this->traits.get_max_value();
  const float step = this->traits.get_step();
  if (step > 0.0f)
    value = min_v + std::round((value - min_v) / step) * step;
  if (value < min_v)
    value = min_v;
  if (value > max_v)
    value = max_v;
  return value;
}

void SamsungNasaWriteNumber::control(float value) {
  if (std::isnan(value))
    return;
  value = this->snap_(value);

  const uint32_t now = millis();
  const uint32_t elapsed = now - this->last_write_;
  if (this->has_written_ && this->min_write_interval_ > 0 && elapsed < this->min_write_interval_) {
    // Too soon: keep only the latest request and send it when allowed.
    this->pending_ = value;
    this->set_timeout("pending_write", this->min_write_interval_ - elapsed, [this]() {
      if (!std::isnan(this->pending_)) {
        float v = this->pending_;
        this->pending_ = NAN;
        this->write_now_(v);
      }
    });
    ESP_LOGD(TAG, "0x%04X: write of %.2f delayed by rate limit", this->message_, value);
    return;
  }
  this->write_now_(value);
}

void SamsungNasaWriteNumber::write_now_(float value) {
  const int32_t raw = (int32_t) std::lround(value * this->scale_);
  ESP_LOGI(TAG, "0x%04X @ %s: set %.2f (raw %d)", this->message_, this->address_.c_str(), value, (int) raw);
  this->write_raw_(raw);
  this->last_write_ = millis();
  this->has_written_ = true;
  // Optimistic; replaced by the value the unit reports (state sensor or read-back).
  this->publish_state(value);
}

}  // namespace samsung_nasa_write
}  // namespace esphome
