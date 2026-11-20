#pragma once

#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"
#include "samsung_nasa_write.h"

namespace esphome {
namespace samsung_nasa_write {

/// A switch writing `on_value` / `off_value` (default 1 / 0) to one NASA message.
class SamsungNasaSwitch : public switch_::Switch, public Component, public NasaBinding {
 public:
  void set_on_value(int32_t value) { this->on_value_ = value; }
  void set_off_value(int32_t value) { this->off_value_ = value; }
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void write_state(bool state) override;
  int32_t on_value_{1};
  int32_t off_value_{0};
};

}  // namespace samsung_nasa_write
}  // namespace esphome
