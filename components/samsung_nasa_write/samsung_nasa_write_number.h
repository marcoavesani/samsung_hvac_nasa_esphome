#pragma once

#include <cstdint>
#include <string>

#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "samsung_nasa_write.h"

namespace esphome {
namespace samsung_nasa_write {

/// A Home Assistant number that writes one NASA message when changed.
/// Value on the bus = round(value * scale). If `state_sensor` is set, the
/// number mirrors that sensor, so it always shows what the unit reports.
class SamsungNasaWriteNumber : public number::Number, public Component {
 public:
  void set_parent(samsung_ac::Samsung_AC *parent) { this->parent_ = parent; }
  void set_address(const std::string &address) { this->address_ = address; }
  void set_message(uint16_t message) { this->message_ = message; }
  void set_scale(float scale) { this->scale_ = scale; }
  void set_state_sensor(sensor::Sensor *sensor) { this->state_sensor_ = sensor; }
  void set_min_write_interval(uint32_t ms) { this->min_write_interval_ = ms; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void control(float value) override;
  void write_now_(float value);
  float snap_(float value) const;

  samsung_ac::Samsung_AC *parent_{nullptr};
  sensor::Sensor *state_sensor_{nullptr};
  std::string address_;
  uint16_t message_{0};
  float scale_{1.0f};
  uint32_t min_write_interval_{0};
  uint32_t last_write_{0};
  bool has_written_{false};
  float pending_{NAN};
};

}  // namespace samsung_nasa_write
}  // namespace esphome
