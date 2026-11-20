#pragma once

#include <cmath>
#include <cstdint>

#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "samsung_nasa_write.h"

namespace esphome {
namespace samsung_nasa_write {

/// A number that writes one NASA message when changed.
/// Value on the bus = round(value * scale). The state follows, in order of
/// preference: `state_sensor` if set, else the value read back from the bus
/// (needs the samsung_ac listener hook), else the last written value.
class SamsungNasaWriteNumber : public number::Number, public Component, public NasaBinding {
 public:
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

  sensor::Sensor *state_sensor_{nullptr};
  float scale_{1.0f};
  uint32_t min_write_interval_{0};
  uint32_t last_write_{0};
  bool has_written_{false};
  float pending_{NAN};
};

}  // namespace samsung_nasa_write
}  // namespace esphome
