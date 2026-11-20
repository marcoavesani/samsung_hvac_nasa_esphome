#pragma once

#include <functional>
#include <string>

#include "esphome/core/automation.h"
#include "samsung_nasa_write.h"

namespace esphome {
namespace samsung_nasa_write {

/// samsung_nasa_write.write: send a raw (already scaled) value.
template<typename... Ts> class NasaWriteAction : public Action<Ts...> {
 public:
  explicit NasaWriteAction(samsung_ac::Samsung_AC *parent) : parent_(parent) {}
  void set_address(const std::string &address) { this->address_ = address; }
  void set_message(uint16_t message) { this->message_ = message; }
  void set_data_type(uint8_t data_type) { this->data_type_ = data_type; }
  void set_value(int32_t value) {
    this->value_ = value;
    this->value_lambda_ = nullptr;
  }
  void set_value_lambda(std::function<int32_t(Ts...)> f) { this->value_lambda_ = std::move(f); }

  void play(const Ts &...x) override {
    const int32_t value = this->value_lambda_ ? this->value_lambda_(x...) : this->value_;
    write_message(this->parent_, this->address_, this->message_, value, this->data_type_);
  }

 protected:
  samsung_ac::Samsung_AC *parent_;
  std::string address_;
  uint16_t message_{0};
  uint8_t data_type_{NASA_DATA_TYPE_REQUEST};
  int32_t value_{0};
  std::function<int32_t(Ts...)> value_lambda_{nullptr};
};

/// samsung_nasa_write.read: ask the unit for the current value of a message.
template<typename... Ts> class NasaReadAction : public Action<Ts...> {
 public:
  explicit NasaReadAction(samsung_ac::Samsung_AC *parent) : parent_(parent) {}
  void set_address(const std::string &address) { this->address_ = address; }
  void set_message(uint16_t message) { this->message_ = message; }

  void play(const Ts &...x) override { read_message(this->parent_, this->address_, this->message_); }

 protected:
  samsung_ac::Samsung_AC *parent_;
  std::string address_;
  uint16_t message_{0};
};

}  // namespace samsung_nasa_write
}  // namespace esphome
