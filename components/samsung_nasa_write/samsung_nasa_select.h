#pragma once

#include <string>
#include <vector>

#include "esphome/components/select/select.h"
#include "esphome/core/component.h"
#include "samsung_nasa_write.h"

namespace esphome {
namespace samsung_nasa_write {

/// A select whose options map to raw NASA values (e.g. 0 = Sunday ... 7 = Every day).
class SamsungNasaSelect : public select::Select, public Component, public NasaBinding {
 public:
  void add_value(int32_t raw) { this->values_.push_back(raw); }
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void control(const std::string &value) override;
  std::vector<int32_t> values_;
};

}  // namespace samsung_nasa_write
}  // namespace esphome
