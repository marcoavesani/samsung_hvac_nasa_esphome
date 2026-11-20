#pragma once

#include "esphome/components/button/button.h"
#include "esphome/core/component.h"
#include "samsung_nasa_write.h"

namespace esphome {
namespace samsung_nasa_write {

/// A button writing a fixed raw value to one NASA message.
class SamsungNasaButton : public button::Button, public Component, public NasaBinding {
 public:
  void set_value(int32_t value) { this->value_ = value; }
  void dump_config() override;

 protected:
  void press_action() override;
  int32_t value_{0};
};

}  // namespace samsung_nasa_write
}  // namespace esphome
