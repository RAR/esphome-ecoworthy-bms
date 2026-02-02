#include "ecoworthy_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ecoworthy_bms {

static const char *const TAG = "ecoworthy_bms";

static const uint8_t FUNCTION_READ = 0x78;
static const uint8_t MAX_NO_RESPONSE_COUNT = 5;

// Ecoworthy/JBD BMS register addresses
// Pack Status: 0x1000 - 0x10A0
static const uint16_t REG_PACK_STATUS_START = 0x1000;
static const uint16_t REG_PACK_STATUS_END = 0x10A0;

void EcoworthyBms::dump_config() {
  ESP_LOGCONFIG(TAG, "Ecoworthy BMS:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_BINARY_SENSOR("  ", "Online Status", this->online_status_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Charging", this->charging_binary_sensor_);
  LOG_BINARY_SENSOR("  ", "Discharging", this->discharging_binary_sensor_);
  LOG_SENSOR("  ", "Total Voltage", this->total_voltage_sensor_);
  LOG_SENSOR("  ", "Current", this->current_sensor_);
  LOG_SENSOR("  ", "Power", this->power_sensor_);
  LOG_SENSOR("  ", "State of Charge", this->state_of_charge_sensor_);
  LOG_TEXT_SENSOR("  ", "Operation Status", this->operation_status_text_sensor_);
}

float EcoworthyBms::get_setup_priority() const { return setup_priority::DATA; }

void EcoworthyBms::update() {
  if (this->no_response_count_ >= MAX_NO_RESPONSE_COUNT) {
    this->publish_device_unavailable_();
    ESP_LOGW(TAG, "No response from BMS (address 0x%02X)", this->address_);
  }

  this->track_online_status_();
  this->no_response_count_++;

  // Request pack status data
  // Frame: addr(1) + func(1) + start_addr(2) + end_addr(2) + data_len(2) + crc(2)
  this->send(FUNCTION_READ, REG_PACK_STATUS_START, REG_PACK_STATUS_END);

  this->update_counter_++;
}

void EcoworthyBms::on_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() < 10) {
    ESP_LOGW(TAG, "Invalid response length: %d", data.size());
    return;
  }

  uint8_t address = data[0];
  uint8_t function = data[1];

  // Check if this response is for this device
  if (address != this->address_) {
    return;  // Not for this device
  }

  // Reset the no-response counter since we got a valid response for this address
  this->reset_online_status_tracker_();

  if (function != FUNCTION_READ) {
    ESP_LOGW(TAG, "Unexpected function code: 0x%02X", function);
    return;
  }

  // Get start/end addresses from response
  uint16_t start_addr = (uint16_t(data[2]) << 8) | uint16_t(data[3]);
  uint16_t end_addr = (uint16_t(data[4]) << 8) | uint16_t(data[5]);
  uint16_t data_length = (uint16_t(data[6]) << 8) | uint16_t(data[7]);

  ESP_LOGD(TAG, "Received response: start=0x%04X, end=0x%04X, len=%d", start_addr, end_addr, data_length);

  if (start_addr == REG_PACK_STATUS_START) {
    this->on_pack_status_data_(data);
  }
}

void EcoworthyBms::on_pack_status_data_(const std::vector<uint8_t> &data) {
  // Data starts at offset 8 (after header: addr + func + start_addr + end_addr + data_len)
  const uint8_t *payload = &data[8];
  size_t data_length = (uint16_t(data[6]) << 8) | uint16_t(data[7]);

  auto get_16bit = [&](size_t i) -> uint16_t {
    if (i + 1 >= data_length) return 0;
    return (uint16_t(payload[i]) << 8) | uint16_t(payload[i + 1]);
  };

  auto get_32bit = [&](size_t i) -> uint32_t {
    if (i + 3 >= data_length) return 0;
    return (uint32_t(payload[i]) << 24) | (uint32_t(payload[i + 1]) << 16) | 
           (uint32_t(payload[i + 2]) << 8) | uint32_t(payload[i + 3]);
  };

  ESP_LOGV(TAG, "Processing %d bytes of pack status data", data_length);

  // Based on the gist register map for Pack Status (0x1000)
  // Offset 0: pack voltage, V = val / 100
  float total_voltage = get_16bit(0) * 0.01f;
  this->publish_state_(this->total_voltage_sensor_, total_voltage);

  // Offset 2: unknown (slave pack voltage)
  
  // Offset 4: pack current (4 bytes), A = (val - 300000) / 100
  uint32_t current_raw = get_32bit(4);
  float current = ((int32_t)current_raw - 300000) / 100.0f;
  this->publish_state_(this->current_sensor_, current);

  // Calculate power
  float power = total_voltage * current;
  this->publish_state_(this->power_sensor_, power);
  this->publish_state_(this->charging_power_sensor_, power > 0 ? power : 0);
  this->publish_state_(this->discharging_power_sensor_, power < 0 ? -power : 0);

  // Offset 8: state of charge in 0.01% units
  float soc = get_16bit(8) / 100.0f;
  this->publish_state_(this->state_of_charge_sensor_, soc);

  // Offset 10: residual pack capacity, Ah = val / 100
  float remaining_capacity = get_16bit(10) / 100.0f;
  this->publish_state_(this->remaining_capacity_sensor_, remaining_capacity);

  // Offset 12: full pack capacity, Ah = val / 100
  float full_capacity = get_16bit(12) / 100.0f;
  this->publish_state_(this->full_capacity_sensor_, full_capacity);

  // Offset 14: rated pack capacity, Ah = val / 100
  float rated_capacity = get_16bit(14) / 100.0f;
  this->publish_state_(this->rated_capacity_sensor_, rated_capacity);

  // Offset 16: MOSFET temperature: °C = (val - 500) / 10
  float mosfet_temp = (get_16bit(16) - 500) / 10.0f;
  this->publish_state_(this->mosfet_temperature_sensor_, mosfet_temp);

  // Offset 18: ambient temperature: °C = (val - 500) / 10
  float ambient_temp = (get_16bit(18) - 500) / 10.0f;
  this->publish_state_(this->ambient_temperature_sensor_, ambient_temp);

  // Offset 20: Operation status (0: Idle, 1: Charging, 2: Discharging)
  uint16_t operation_status = get_16bit(20);
  this->publish_state_(this->operation_status_text_sensor_, this->decode_operation_status_(operation_status));
  this->publish_state_(this->charging_binary_sensor_, operation_status == 1);
  this->publish_state_(this->discharging_binary_sensor_, operation_status == 2);

  // Offset 22: state of health, %
  float soh = get_16bit(22);
  this->publish_state_(this->state_of_health_sensor_, soh);

  // Offset 24: level 2 protected state fault code (4 bytes)
  uint32_t fault = get_32bit(24);
  this->publish_state_(this->fault_bitmask_sensor_, (float)fault);
  this->publish_state_(this->fault_text_sensor_, this->decode_fault_(fault));

  // Offset 28: level 1 alarm code (4 bytes)
  uint32_t alarm = get_32bit(28);
  this->publish_state_(this->alarm_bitmask_sensor_, (float)alarm);
  this->publish_state_(this->alarm_text_sensor_, this->decode_alarm_(alarm));

  // Offset 32: MOSFET state bitmask
  uint16_t mosfet_status = get_16bit(32);
  this->publish_state_(this->mosfet_status_bitmask_sensor_, (float)mosfet_status);
  this->publish_state_(this->discharge_mos_binary_sensor_, (mosfet_status & 0x0001) != 0);
  this->publish_state_(this->charge_mos_binary_sensor_, (mosfet_status & 0x0002) != 0);

  // Offset 34: bitmask: 0: Charger 1: LOAD 2: SW
  
  // Offset 36: Number of charge cycles
  uint16_t cycle_count = get_16bit(36);
  this->publish_state_(this->cycle_count_sensor_, (float)cycle_count);

  // Offset 38: cell # with highest voltage
  uint16_t max_cell_num = get_16bit(38);
  this->publish_state_(this->max_voltage_cell_sensor_, (float)max_cell_num);

  // Offset 40: highest cell voltage, mV
  float max_cell_voltage = get_16bit(40) * 0.001f;
  this->publish_state_(this->max_cell_voltage_sensor_, max_cell_voltage);

  // Offset 42: cell # with lowest voltage
  uint16_t min_cell_num = get_16bit(42);
  this->publish_state_(this->min_voltage_cell_sensor_, (float)min_cell_num);

  // Offset 44: lowest cell voltage, mV
  float min_cell_voltage = get_16bit(44) * 0.001f;
  this->publish_state_(this->min_cell_voltage_sensor_, min_cell_voltage);

  // Offset 46: avg cell voltage, mV
  float avg_cell_voltage = get_16bit(46) * 0.001f;
  this->publish_state_(this->cell_average_voltage_sensor_, avg_cell_voltage);

  // Calculate delta
  this->publish_state_(this->delta_cell_voltage_sensor_, max_cell_voltage - min_cell_voltage);

  // Offset 48: cell temp sensor # with highest temperature
  // Offset 50: highest cell temperature: °C = (val - 500) / 10
  float max_temp = (get_16bit(50) - 500) / 10.0f;
  this->publish_state_(this->max_temperature_sensor_, max_temp);

  // Offset 52: cell temp sensor # with lowest temperature
  // Offset 54: lowest cell temperature: °C = (val - 500) / 10
  float min_temp = (get_16bit(54) - 500) / 10.0f;
  this->publish_state_(this->min_temperature_sensor_, min_temp);

  // Offset 56: avg cell temperature: °C = (val - 500) / 10
  float avg_temp = (get_16bit(56) - 500) / 10.0f;
  this->publish_state_(this->avg_temperature_sensor_, avg_temp);

  // Offset 58: Charge Voltage Limit ("MAX CHG CV"), V = val / 10
  float cvl = get_16bit(58) / 10.0f;
  this->publish_state_(this->charge_voltage_limit_sensor_, cvl);

  // Offset 60: Charge Current Limit, A = val / 10
  float ccl = get_16bit(60) / 10.0f;
  this->publish_state_(this->charge_current_limit_sensor_, ccl);

  // Offset 62: Discharge Voltage Limit ("MIN DSG DV"), V = val / 10
  float dvl = get_16bit(62) / 10.0f;
  this->publish_state_(this->discharge_voltage_limit_sensor_, dvl);

  // Offset 64: Discharge Current Limit, A = val / 10
  float dcl = get_16bit(64) / 10.0f;
  this->publish_state_(this->discharge_current_limit_sensor_, dcl);

  // Offset 66: number of cells
  uint16_t cell_count = get_16bit(66);
  this->publish_state_(this->cell_count_sensor_, (float)cell_count);

  // Offset 68: cell voltages start (2 bytes each, in mV)
  size_t cell_offset = 68;
  float sum_cell_voltage = 0.0f;
  uint8_t valid_cells = 0;
  
  for (uint8_t i = 0; i < std::min((uint16_t)16, cell_count); i++) {
    uint16_t cell_mv = get_16bit(cell_offset + i * 2);
    if (cell_mv > 0 && cell_mv < 5000) {  // Valid range check
      float cell_voltage = cell_mv * 0.001f;
      this->publish_state_(this->cells_[i].cell_voltage_sensor_, cell_voltage);
      sum_cell_voltage += cell_voltage;
      valid_cells++;
    }
  }

  // Offset after cells: number of temperature sensors
  size_t temp_offset = cell_offset + cell_count * 2;
  if (temp_offset + 2 <= data_length) {
    uint16_t temp_count = get_16bit(temp_offset);
    this->publish_state_(this->temperature_sensor_count_sensor_, (float)temp_count);
    
    // Temperature values
    size_t temp_values_offset = temp_offset + 2;
    for (uint8_t i = 0; i < std::min((uint16_t)4, temp_count); i++) {
      if (temp_values_offset + i * 2 + 2 <= data_length) {
        float temp = (get_16bit(temp_values_offset + i * 2) - 500) / 10.0f;
        this->publish_state_(this->temperatures_[i].temperature_sensor_, temp);
      }
    }

    // After temperatures: balance status bitmask, firmware version, serial number, etc.
    size_t after_temps_offset = temp_values_offset + temp_count * 2;
    
    // Skip unknown (2 bytes), then balance status (2 bytes)
    if (after_temps_offset + 4 <= data_length) {
      uint16_t balance_status = get_16bit(after_temps_offset + 2);
      this->publish_state_(this->balancing_bitmask_sensor_, (float)balance_status);
      this->publish_state_(this->balancing_binary_sensor_, balance_status != 0);
    }

    // Firmware version (2 bytes after balance status)
    if (after_temps_offset + 6 <= data_length) {
      uint16_t fw_raw = get_16bit(after_temps_offset + 4);
      float fw_major = (fw_raw >> 8) & 0xFF;
      float fw_minor = fw_raw & 0xFF;
      char fw_str[16];
      snprintf(fw_str, sizeof(fw_str), "%d.%d", (int)fw_major, (int)fw_minor);
      this->publish_state_(this->firmware_text_sensor_, std::string(fw_str));
    }

    // Serial number (30 bytes after firmware version)
    if (after_temps_offset + 36 <= data_length) {
      std::string serial((char *)&payload[after_temps_offset + 6], 30);
      // Trim null characters
      size_t end = serial.find('\0');
      if (end != std::string::npos) {
        serial = serial.substr(0, end);
      }
      this->publish_state_(this->serial_number_text_sensor_, serial);
    }
  }
}

std::string EcoworthyBms::decode_operation_status_(uint16_t status) {
  switch (status) {
    case 0: return "Idle";
    case 1: return "Charging";
    case 2: return "Discharging";
    default: return "Unknown";
  }
}

std::string EcoworthyBms::decode_fault_(uint32_t fault) {
  if (fault == 0) return "None";
  
  std::string result;
  if (fault & (1 << 0)) result += "Cell OV;";
  if (fault & (1 << 1)) result += "Cell UV;";
  if (fault & (1 << 2)) result += "Pack OV;";
  if (fault & (1 << 3)) result += "Pack UV;";
  if (fault & (1 << 4)) result += "Charge OC Slow;";
  if (fault & (1 << 5)) result += "Charge OC Fast;";
  if (fault & (1 << 6)) result += "Discharge OC Slow;";
  if (fault & (1 << 7)) result += "Discharge OC Fast;";
  if (fault & (1 << 8)) result += "Charge HT;";
  if (fault & (1 << 9)) result += "Charge LT;";
  if (fault & (1 << 10)) result += "Discharge HT;";
  if (fault & (1 << 11)) result += "Discharge LT;";
  if (fault & (1 << 12)) result += "MOS HT;";
  if (fault & (1 << 13)) result += "Ambient HT;";
  if (fault & (1 << 14)) result += "Ambient LT;";
  if (fault & (1 << 15)) result += "Cell V Diff;";
  if (fault & (1 << 16)) result += "Temp Diff;";
  if (fault & (1 << 17)) result += "SOC Low;";
  if (fault & (1 << 18)) result += "Short Circuit;";
  if (fault & (1 << 19)) result += "Cell Offline;";
  if (fault & (1 << 20)) result += "Temp Sensor Fail;";
  if (fault & (1 << 21)) result += "Charge MOS Fault;";
  if (fault & (1 << 22)) result += "Discharge MOS Fault;";
  if (fault & (1 << 23)) result += "AFE Comm Error;";
  
  if (!result.empty() && result.back() == ';') result.pop_back();
  return result;
}

std::string EcoworthyBms::decode_alarm_(uint32_t alarm) {
  if (alarm == 0) return "None";
  
  std::string result;
  if (alarm & (1 << 0)) result += "Cell OV;";
  if (alarm & (1 << 1)) result += "Cell UV;";
  if (alarm & (1 << 2)) result += "Pack OV;";
  if (alarm & (1 << 3)) result += "Pack UV;";
  if (alarm & (1 << 4)) result += "Charge OC;";
  if (alarm & (1 << 5)) result += "Discharge OC;";
  if (alarm & (1 << 6)) result += "Charge HT;";
  if (alarm & (1 << 7)) result += "Charge LT;";
  if (alarm & (1 << 8)) result += "Discharge HT;";
  if (alarm & (1 << 9)) result += "Discharge LT;";
  if (alarm & (1 << 10)) result += "MOS HT;";
  if (alarm & (1 << 11)) result += "Ambient HT;";
  if (alarm & (1 << 12)) result += "Ambient LT;";
  if (alarm & (1 << 13)) result += "Cell V Diff;";
  if (alarm & (1 << 14)) result += "Temp Diff;";
  if (alarm & (1 << 15)) result += "SOC Low;";
  if (alarm & (1 << 16)) result += "EEP Fault;";
  if (alarm & (1 << 17)) result += "RTC Abnormal;";
  if (alarm & (1 << 18)) result += "Full Charge Prot;";
  
  if (!result.empty() && result.back() == ';') result.pop_back();
  return result;
}

void EcoworthyBms::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor != nullptr) {
    binary_sensor->publish_state(state);
  }
}

void EcoworthyBms::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor != nullptr && !std::isnan(value)) {
    sensor->publish_state(value);
  }
}

void EcoworthyBms::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor != nullptr && !state.empty()) {
    text_sensor->publish_state(state);
  }
}

void EcoworthyBms::reset_online_status_tracker_() {
  this->no_response_count_ = 0;
  this->publish_state_(this->online_status_binary_sensor_, true);
}

void EcoworthyBms::track_online_status_() {
  if (this->no_response_count_ < MAX_NO_RESPONSE_COUNT) {
    this->publish_state_(this->online_status_binary_sensor_, true);
  }
}

void EcoworthyBms::publish_device_unavailable_() {
  this->publish_state_(this->online_status_binary_sensor_, false);
}

}  // namespace ecoworthy_bms
}  // namespace esphome
