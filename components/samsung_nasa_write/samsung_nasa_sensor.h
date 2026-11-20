#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"
#include "samsung_nasa_write.h"

namespace esphome {
namespace samsung_nasa_write {

/// Read-only value of one NASA message: value = raw / scale. Updated from
/// broadcast notifications and, with `poll_interval`, by asking the unit
/// (needed for values that are not broadcast, like FSV settings).
class SamsungNasaSensor : public sensor::Sensor, public Component, public NasaBinding {
 public:
  void set_scale(float scale) { this->scale_ = scale; }
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  float scale_{1.0f};
};

}  // namespace samsung_nasa_write
}  // namespace esphome
