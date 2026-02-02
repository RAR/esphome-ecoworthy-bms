#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"
#include <queue>

namespace esphome {
namespace ecoworthy_modbus {

class EcoworthyModbusDevice;

// Ecoworthy/JBD BMS uses a custom Modbus-like protocol
// Function codes: 0x78 = read, 0x79 = write
// CRC: CRC16 with initial=0xFFFF, polynomial=0xA001, LSB first
struct ModbusRequest {
  uint8_t address;
  uint8_t function;
  uint16_t start_address;
  uint16_t end_address;
  uint16_t data_length;
};

class EcoworthyModbus : public uart::UARTDevice, public Component {
 public:
  EcoworthyModbus() = default;

  void setup() override;
  void loop() override;
  void dump_config() override;

  void register_device(EcoworthyModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  // Ecoworthy uses a custom frame format: addr(1) + func(1) + start_addr(2) + end_addr(2) + data_len(2) + crc(2)
  void send(uint8_t address, uint8_t function, uint16_t start_address, uint16_t end_address);
  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }

 protected:
  GPIOPin *flow_control_pin_{nullptr};

  bool parse_modbus_byte_(uint8_t byte);
  void send_next_request_();
  
  std::vector<uint8_t> rx_buffer_;
  uint32_t last_modbus_byte_{0};
  uint32_t last_send_{0};
  std::vector<EcoworthyModbusDevice *> devices_;
  
  std::queue<ModbusRequest> request_queue_;
  bool waiting_for_response_{false};
};

uint16_t crc16_ecoworthy(const uint8_t *data, uint16_t len);

class EcoworthyModbusDevice {
 public:
  void set_parent(EcoworthyModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_modbus_data(const std::vector<uint8_t> &data) = 0;
  void send(uint8_t function, uint16_t start_address, uint16_t end_address) {
    this->parent_->send(this->address_, function, start_address, end_address);
  }

 protected:
  friend EcoworthyModbus;

  EcoworthyModbus *parent_;
  uint8_t address_;
};

}  // namespace ecoworthy_modbus
}  // namespace esphome
