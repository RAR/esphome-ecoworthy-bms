#include "ecoworthy_modbus.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ecoworthy_modbus {

static const char *const TAG = "ecoworthy_modbus";

// Ecoworthy/JBD custom function codes
static const uint8_t FUNCTION_READ = 0x78;
static const uint8_t FUNCTION_WRITE = 0x79;

static const uint16_t ECOWORTHY_RESPONSE_TIMEOUT = 2000;
static const uint16_t ECOWORTHY_MIN_MSG_LEN = 10;  // addr + func + start(2) + end(2) + len(2) + crc(2)

void EcoworthyModbus::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
}

void EcoworthyModbus::loop() {
  const uint32_t now = millis();

  // Read incoming bytes
  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_modbus_byte_(byte)) {
      this->last_modbus_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }

  // Check for timeout on current response
  if (this->waiting_for_response_ && !this->rx_buffer_.empty() && 
      (now - this->last_modbus_byte_ > ECOWORTHY_RESPONSE_TIMEOUT)) {
    ESP_LOGW(TAG, "Response timeout");
    this->rx_buffer_.clear();
    this->waiting_for_response_ = false;
  }

  // Check for complete timeout (no response at all)
  if (this->waiting_for_response_ && this->rx_buffer_.empty() && 
      (now - this->last_send_ > ECOWORTHY_RESPONSE_TIMEOUT)) {
    ESP_LOGW(TAG, "No response received");
    this->waiting_for_response_ = false;
  }

  // Send next request if not waiting for a response
  if (!this->waiting_for_response_) {
    this->send_next_request_();
  }
}

void EcoworthyModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "Ecoworthy Modbus:");
  ESP_LOGCONFIG(TAG, "  Flow control pin: %s", YESNO(this->flow_control_pin_ != nullptr));
}

float EcoworthyModbus::get_setup_priority() const { return setup_priority::DATA; }

// CRC16 with initial_value=0xFFFF and polynomial=0xA001, LSB first
uint16_t crc16_ecoworthy(const uint8_t *data, uint16_t len) {
  uint16_t crc = 0xFFFF;
  for (uint16_t i = 0; i < len; i++) {
    crc ^= data[i];
    for (uint8_t j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc = (crc >> 1) ^ 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

void EcoworthyModbus::send(uint8_t address, uint8_t function, uint16_t start_address, uint16_t end_address) {
  // Add request to queue instead of sending immediately
  ModbusRequest request;
  request.address = address;
  request.function = function;
  request.start_address = start_address;
  request.end_address = end_address;
  request.is_write = false;
  
  this->request_queue_.push(request);
  
  ESP_LOGV(TAG, "Queued read request for address 0x%02X, start=0x%04X, end=0x%04X, queue size: %d", 
           address, start_address, end_address, this->request_queue_.size());
}

void EcoworthyModbus::send_write(uint8_t address, uint16_t start_address, uint16_t end_address, const std::vector<uint8_t> &data) {
  // Add write request to queue
  ModbusRequest request;
  request.address = address;
  request.function = FUNCTION_WRITE;
  request.start_address = start_address;
  request.end_address = end_address;
  request.is_write = true;
  
  // Write commands include 0x114A4244 prefix ("JBD" with 0x11 prefix)
  request.data.push_back(0x11);
  request.data.push_back(0x4A);  // 'J'
  request.data.push_back(0x42);  // 'B'
  request.data.push_back(0x44);  // 'D'
  // Append actual data
  for (uint8_t b : data) {
    request.data.push_back(b);
  }
  
  this->request_queue_.push(request);
  
  ESP_LOGV(TAG, "Queued write request for address 0x%02X, start=0x%04X, end=0x%04X, data_len=%d, queue size: %d", 
           address, start_address, end_address, request.data.size(), this->request_queue_.size());
}

void EcoworthyModbus::send_next_request_() {
  if (this->request_queue_.empty() || this->waiting_for_response_) {
    return;
  }

  ModbusRequest request = this->request_queue_.front();
  this->request_queue_.pop();

  if (request.is_write) {
    // Write frame format: addr(1) + func(1) + start_addr(2) + end_addr(2) + data_len(2) + data(n) + crc(2)
    size_t frame_size = 8 + request.data.size() + 2;
    std::vector<uint8_t> frame(frame_size);
    
    frame[0] = request.address;
    frame[1] = request.function;
    frame[2] = request.start_address >> 8;
    frame[3] = request.start_address & 0xFF;
    frame[4] = request.end_address >> 8;
    frame[5] = request.end_address & 0xFF;
    frame[6] = (request.data.size() >> 8) & 0xFF;
    frame[7] = request.data.size() & 0xFF;
    
    // Copy data
    for (size_t i = 0; i < request.data.size(); i++) {
      frame[8 + i] = request.data[i];
    }
    
    uint16_t crc = crc16_ecoworthy(frame.data(), frame_size - 2);
    frame[frame_size - 2] = crc & 0xFF;        // LSB first
    frame[frame_size - 1] = (crc >> 8) & 0xFF;

    if (this->flow_control_pin_ != nullptr) {
      this->flow_control_pin_->digital_write(true);
    }

    this->write_array(frame.data(), frame_size);
    this->flush();

    if (this->flow_control_pin_ != nullptr) {
      this->flow_control_pin_->digital_write(false);
    }

    ESP_LOGD(TAG, "Sent write: %s", format_hex_pretty(frame.data(), std::min(frame_size, (size_t)32)).c_str());
  } else {
    // Read frame format: addr(1) + func(1) + start_addr(2) + end_addr(2) + data_len(2) + crc(2)
    uint8_t frame[10];
    frame[0] = request.address;
    frame[1] = request.function;
    frame[2] = request.start_address >> 8;
    frame[3] = request.start_address & 0xFF;
    frame[4] = request.end_address >> 8;
    frame[5] = request.end_address & 0xFF;
    frame[6] = 0x00;
    frame[7] = 0x00;

    uint16_t crc = crc16_ecoworthy(frame, 8);
    frame[8] = crc & 0xFF;        // LSB first
    frame[9] = (crc >> 8) & 0xFF;

    if (this->flow_control_pin_ != nullptr) {
      this->flow_control_pin_->digital_write(true);
    }

    this->write_array(frame, 10);
    this->flush();

    if (this->flow_control_pin_ != nullptr) {
      this->flow_control_pin_->digital_write(false);
    }

    ESP_LOGV(TAG, "Sent read: %s", format_hex_pretty(frame, 10).c_str());
  }
  
  this->last_send_ = millis();
  this->waiting_for_response_ = true;
}

bool EcoworthyModbus::parse_modbus_byte_(uint8_t byte) {
  const uint32_t now = millis();
  
  // Start of new frame if buffer is empty or timeout occurred
  if (this->rx_buffer_.empty()) {
    this->rx_buffer_.push_back(byte);
    return true;
  }

  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];
  const size_t len = this->rx_buffer_.size();

  // Wait for minimum message length (header)
  if (len < ECOWORTHY_MIN_MSG_LEN) {
    return true;
  }

  // Check if we have a complete frame
  // Response format: addr(1) + func(1) + start_addr(2) + end_addr(2) + data_len(2) + data(n) + crc(2)
  uint8_t address = raw[0];
  uint8_t function = raw[1];

  // Get data length from header
  uint16_t data_length = (uint16_t(raw[6]) << 8) | uint16_t(raw[7]);
  
  // Expected total length: 8 (header) + data_length + 2 (crc)
  size_t expected_len = 8 + data_length + 2;
  
  // Sanity check on data length
  if (data_length > 512) {
    ESP_LOGW(TAG, "Invalid data length: %d", data_length);
    this->rx_buffer_.clear();
    return false;
  }

  if (len < expected_len) {
    return true;  // Need more data
  }

  if (len >= expected_len) {
    // Verify CRC (calculated over entire frame minus CRC itself)
    uint16_t crc_calc = crc16_ecoworthy(raw, expected_len - 2);
    uint16_t crc_recv = raw[expected_len - 2] | (raw[expected_len - 1] << 8);  // LSB first

    if (crc_calc != crc_recv) {
      ESP_LOGW(TAG, "CRC check failed! Calculated: 0x%04X, Received: 0x%04X", crc_calc, crc_recv);
      this->rx_buffer_.clear();
      return false;
    }

    ESP_LOGV(TAG, "Received %d bytes: %s", expected_len, format_hex_pretty(raw, std::min(expected_len, (size_t)32)).c_str());

    // Dispatch to devices
    for (auto *device : this->devices_) {
      device->on_modbus_data(this->rx_buffer_);
    }

    this->rx_buffer_.clear();
    this->waiting_for_response_ = false;  // Ready for next request
    return true;
  }

  return true;
}

}  // namespace ecoworthy_modbus
}  // namespace esphome
