#include "ecoworthy_bms.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ecoworthy_bms {

static const char *const TAG = "ecoworthy_bms";

static const uint8_t FUNCTION_READ = 0x78;
static const uint8_t FUNCTION_WRITE = 0x79;
static const uint8_t MAX_NO_RESPONSE_COUNT = 5;

// Ecoworthy/JBD BMS register addresses
// Pack Status: 0x1000 - 0x10A0
static const uint16_t REG_PACK_STATUS_START = 0x1000;
static const uint16_t REG_PACK_STATUS_END = 0x10A0;

// Pack Configuration Block 1: 0x1C00 - 0x1CA0
static const uint16_t REG_CONFIG_1C00_START = 0x1C00;
static const uint16_t REG_CONFIG_1C00_END = 0x1CA0;

// Pack Configuration Block 2: 0x2000 - 0x2050
static const uint16_t REG_CONFIG_2000_START = 0x2000;
static const uint16_t REG_CONFIG_2000_END = 0x2050;

// Product Information: 0x2810 - 0x283C
static const uint16_t REG_PRODUCT_INFO_START = 0x2810;
static const uint16_t REG_PRODUCT_INFO_END = 0x283C;

// Write addresses
static const uint16_t REG_MOS_CONTROL = 0x2902;
static const uint16_t REG_SLEEP_MODE = 0x2908;

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

  // Cycle through different register blocks
  switch (this->request_step_) {
    case 0:
      // Request pack status data (every update)
      this->send(FUNCTION_READ, REG_PACK_STATUS_START, REG_PACK_STATUS_END);
      break;
    case 1:
      // Request config block 1 (every 5th update)
      if ((this->update_counter_ % 5) == 0) {
        this->send(FUNCTION_READ, REG_CONFIG_1C00_START, REG_CONFIG_1C00_END);
      }
      break;
    case 2:
      // Request config block 2 (every 10th update)
      if ((this->update_counter_ % 10) == 0) {
        this->send(FUNCTION_READ, REG_CONFIG_2000_START, REG_CONFIG_2000_END);
      }
      break;
    case 3:
      // Request product info (once at startup and every 60 updates)
      if (this->update_counter_ == 0 || (this->update_counter_ % 60) == 0) {
        this->send(FUNCTION_READ, REG_PRODUCT_INFO_START, REG_PRODUCT_INFO_END);
      }
      break;
  }

  this->request_step_ = (this->request_step_ + 1) % 4;
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

  if (function == FUNCTION_WRITE) {
    // Write acknowledgment - log success
    ESP_LOGD(TAG, "Write command acknowledged");
    return;
  }

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
  } else if (start_addr == REG_CONFIG_1C00_START) {
    this->on_config_1c00_data_(data);
  } else if (start_addr == REG_CONFIG_2000_START) {
    this->on_config_2000_data_(data);
  } else if (start_addr == REG_PRODUCT_INFO_START) {
    this->on_product_info_data_(data);
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

  // Offset 16: MOSFET (power tube) temperature: °C = (val - 500) / 10
  float power_tube_temp = (get_16bit(16) - 500) / 10.0f;
  this->publish_state_(this->power_tube_temperature_sensor_, power_tube_temp);

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
  
  // Update MOS states for binary sensors
  this->discharge_mos_state_ = (mosfet_status & 0x0001) != 0;
  this->charge_mos_state_ = (mosfet_status & 0x0002) != 0;
  
  this->publish_state_(this->discharging_switch_binary_sensor_, this->discharge_mos_state_);
  this->publish_state_(this->charging_switch_binary_sensor_, this->charge_mos_state_);
  
  // Update switch states (JK-BMS naming: charging/discharging)
  if (this->charging_switch_ != nullptr) {
    this->charging_switch_->publish_state(this->charge_mos_state_);
  }
  if (this->discharging_switch_ != nullptr) {
    this->discharging_switch_->publish_state(this->discharge_mos_state_);
  }

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
  this->publish_state_(this->average_cell_voltage_sensor_, avg_cell_voltage);

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

// Config block 1 (0x1C00) parsing
void EcoworthyBms::on_config_1c00_data_(const std::vector<uint8_t> &data) {
  const uint8_t *payload = &data[8];
  size_t data_length = (uint16_t(data[6]) << 8) | uint16_t(data[7]);

  auto get_16bit = [&](size_t i) -> uint16_t {
    if (i + 1 >= data_length) return 0;
    return (uint16_t(payload[i]) << 8) | uint16_t(payload[i + 1]);
  };

  ESP_LOGV(TAG, "Processing %d bytes of config 0x1C00 data", data_length);

  // Offset 0: Balance open voltage (mV)
  float balance_voltage = get_16bit(0) * 0.001f;
  this->publish_state_(this->balance_voltage_sensor_, balance_voltage);

  // Offset 2: Balance difference voltage (mV)
  float balance_diff = get_16bit(2) * 0.001f;
  this->publish_state_(this->balance_difference_sensor_, balance_diff);

  // Offset 4: Heater start temp °C = (val - 500) / 10
  float heater_start = (get_16bit(4) - 500) / 10.0f;
  this->publish_state_(this->heater_start_temp_sensor_, heater_start);

  // Offset 6: Heater stop temp °C = (val - 500) / 10
  float heater_stop = (get_16bit(6) - 500) / 10.0f;
  this->publish_state_(this->heater_stop_temp_sensor_, heater_stop);

  // Offset 8: Full charge voltage V = val / 100
  float full_chg_v = get_16bit(8) / 100.0f;
  this->publish_state_(this->full_charge_voltage_sensor_, full_chg_v);

  // Offset 10: Full charge current A = val / 10
  float full_chg_a = get_16bit(10) / 10.0f;
  this->publish_state_(this->full_charge_current_sensor_, full_chg_a);

  // Offset 12-27: BMS SN code (16 bytes)
  if (data_length >= 28) {
    std::string bms_sn((char *)&payload[12], 16);
    size_t end = bms_sn.find('\0');
    if (end != std::string::npos) bms_sn = bms_sn.substr(0, end);
    this->publish_state_(this->bms_serial_number_text_sensor_, bms_sn);
  }

  // Offset 28-43: Pack SN code (16 bytes)
  if (data_length >= 44) {
    std::string pack_sn((char *)&payload[28], 16);
    size_t end = pack_sn.find('\0');
    if (end != std::string::npos) pack_sn = pack_sn.substr(0, end);
    this->publish_state_(this->pack_serial_number_text_sensor_, pack_sn);
  }

  // Offset 44-59: Manufacturer code (16 bytes)
  if (data_length >= 60) {
    std::string manufacturer((char *)&payload[44], 16);
    size_t end = manufacturer.find('\0');
    if (end != std::string::npos) manufacturer = manufacturer.substr(0, end);
    this->publish_state_(this->manufacturer_text_sensor_, manufacturer);
  }

  // Offset 60: CAN protocol (0: PYLON, 1: SMA, 2: Victron, 3: Growatt, etc.)
  if (data_length >= 62) {
    uint16_t can_proto = get_16bit(60);
    this->publish_state_(this->can_protocol_text_sensor_, this->decode_can_protocol_(can_proto));
  }

  // Offset 62: RS485 protocol
  if (data_length >= 64) {
    uint16_t rs485_proto = get_16bit(62);
    this->publish_state_(this->rs485_protocol_text_sensor_, this->decode_rs485_protocol_(rs485_proto));
  }

  // Offset 64: Sleep voltage V = val / 100
  if (data_length >= 66) {
    float sleep_v = get_16bit(64) / 100.0f;
    this->publish_state_(this->sleep_voltage_sensor_, sleep_v);
  }

  // Offset 66: Sleep delay (minutes)
  if (data_length >= 68) {
    float sleep_delay = get_16bit(66);
    this->publish_state_(this->sleep_delay_sensor_, sleep_delay);
  }

  // Offset 68: Balance mode (0: voltage, 1: SOC)
  if (data_length >= 70) {
    uint16_t balance_mode = get_16bit(68);
    this->publish_state_(this->balance_mode_text_sensor_, this->decode_balance_mode_(balance_mode));
  }
}

// Config block 2 (0x2000) parsing
void EcoworthyBms::on_config_2000_data_(const std::vector<uint8_t> &data) {
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

  ESP_LOGV(TAG, "Processing %d bytes of config 0x2000 data", data_length);

  // Offset 12: Total charge (4 bytes) Ah = val / 100
  if (data_length >= 16) {
    float total_charge = get_32bit(12) / 100.0f;
    this->publish_state_(this->total_charge_sensor_, total_charge);
  }

  // Offset 16: Total discharge (4 bytes) Ah = val / 100
  if (data_length >= 20) {
    float total_discharge = get_32bit(16) / 100.0f;
    this->publish_state_(this->total_discharge_sensor_, total_discharge);
  }

  // Offset 32: Configured CVL V = val / 10
  if (data_length >= 34) {
    float cvl = get_16bit(32) / 10.0f;
    this->publish_state_(this->configured_cvl_sensor_, cvl);
  }

  // Offset 34: Configured CCL A = val / 10
  if (data_length >= 36) {
    float ccl = get_16bit(34) / 10.0f;
    this->publish_state_(this->configured_ccl_sensor_, ccl);
  }

  // Offset 36: Configured DVL V = val / 10
  if (data_length >= 38) {
    float dvl = get_16bit(36) / 10.0f;
    this->publish_state_(this->configured_dvl_sensor_, dvl);
  }

  // Offset 38: Configured DCL A = val / 10
  if (data_length >= 40) {
    float dcl = get_16bit(38) / 10.0f;
    this->publish_state_(this->configured_dcl_sensor_, dcl);
  }

  // Offset 44: Shunt resistance (μΩ)
  if (data_length >= 46) {
    float rsns = get_16bit(44);
    this->publish_state_(this->shunt_resistance_sensor_, rsns);
  }
}

// Product info (0x2810) parsing
void EcoworthyBms::on_product_info_data_(const std::vector<uint8_t> &data) {
  const uint8_t *payload = &data[8];
  size_t data_length = (uint16_t(data[6]) << 8) | uint16_t(data[7]);

  auto get_16bit = [&](size_t i) -> uint16_t {
    if (i + 1 >= data_length) return 0;
    return (uint16_t(payload[i]) << 8) | uint16_t(payload[i + 1]);
  };

  ESP_LOGV(TAG, "Processing %d bytes of product info data", data_length);

  // Offset 4: Hardware version (as text)
  if (data_length >= 6) {
    uint16_t hw_version = get_16bit(4);
    char hw_str[16];
    snprintf(hw_str, sizeof(hw_str), "v%d.%d", hw_version / 10, hw_version % 10);
    this->publish_state_(this->hardware_version_text_sensor_, std::string(hw_str));
  }

  // Offset 6-8: Firmware version (major.minor.patch)
  if (data_length >= 12) {
    uint16_t fw_major = get_16bit(6);
    uint16_t fw_minor = get_16bit(8);
    uint16_t fw_patch = get_16bit(10);
    char fw_str[32];
    snprintf(fw_str, sizeof(fw_str), "%d.%d.%d", fw_major, fw_minor, fw_patch);
    // Only update firmware from product info if not already set
    if (this->firmware_text_sensor_ != nullptr && !this->firmware_text_sensor_->has_state()) {
      this->publish_state_(this->firmware_text_sensor_, std::string(fw_str));
    }
  }

  // Offset 12-27: BMS model (16 bytes)
  if (data_length >= 28) {
    std::string model((char *)&payload[12], 16);
    size_t end = model.find('\0');
    if (end != std::string::npos) model = model.substr(0, end);
    this->publish_state_(this->bms_model_text_sensor_, model);
  }
}

// MOS control methods
void EcoworthyBms::set_charge_mos(bool state) {
  // MOS control register: bit 1 = charge MOS, bit 0 = discharge MOS
  // We need to preserve the discharge MOS state
  uint8_t mos_value = 0;
  if (this->discharge_mos_state_) mos_value |= 0x01;
  if (state) mos_value |= 0x02;
  
  ESP_LOGI(TAG, "Setting charge MOS to %s (mos_value=0x%02X)", state ? "ON" : "OFF", mos_value);
  
  std::vector<uint8_t> data = {0x00, mos_value};
  this->send_write(REG_MOS_CONTROL, REG_MOS_CONTROL, data);
}

void EcoworthyBms::set_discharge_mos(bool state) {
  // MOS control register: bit 1 = charge MOS, bit 0 = discharge MOS
  // We need to preserve the charge MOS state
  uint8_t mos_value = 0;
  if (state) mos_value |= 0x01;
  if (this->charge_mos_state_) mos_value |= 0x02;
  
  ESP_LOGI(TAG, "Setting discharge MOS to %s (mos_value=0x%02X)", state ? "ON" : "OFF", mos_value);
  
  std::vector<uint8_t> data = {0x00, mos_value};
  this->send_write(REG_MOS_CONTROL, REG_MOS_CONTROL, data);
}

void EcoworthyBms::set_sleep_mode(uint8_t mode) {
  // 0xA501 = standby sleep, 0xA502 = deep sleep
  uint16_t command = 0xA500 | mode;
  ESP_LOGI(TAG, "Setting sleep mode to %s (0x%04X)", mode == 1 ? "standby" : "deep", command);
  
  std::vector<uint8_t> data = {(uint8_t)(command >> 8), (uint8_t)(command & 0xFF)};
  this->send_write(REG_SLEEP_MODE, REG_SLEEP_MODE, data);
}

// Switch implementations (JK-BMS naming convention)
void ChargingSwitch::write_state(bool state) {
  if (this->parent_ != nullptr) {
    this->parent_->set_charge_mos(state);
  }
  // Don't publish state here - wait for BMS to confirm
}

void DischargingSwitch::write_state(bool state) {
  if (this->parent_ != nullptr) {
    this->parent_->set_discharge_mos(state);
  }
  // Don't publish state here - wait for BMS to confirm
}

// Button implementations
void StandbySleepButton::press_action() {
  if (this->parent_ != nullptr) {
    this->parent_->set_sleep_mode(1);  // Standby sleep
  }
}

void DeepSleepButton::press_action() {
  if (this->parent_ != nullptr) {
    this->parent_->set_sleep_mode(2);  // Deep sleep
  }
}

// Decoder methods
std::string EcoworthyBms::decode_balance_mode_(uint16_t mode) {
  switch (mode) {
    case 0: return "Voltage";
    case 1: return "SOC";
    default: return "Unknown";
  }
}

std::string EcoworthyBms::decode_can_protocol_(uint16_t protocol) {
  switch (protocol) {
    case 0: return "PYLON";
    case 1: return "SMA";
    case 2: return "Victron";
    case 3: return "Growatt";
    case 4: return "Goodwe";
    case 5: return "Deye";
    case 6: return "Sofar";
    case 7: return "Solis";
    default: return "Unknown (" + std::to_string(protocol) + ")";
  }
}

std::string EcoworthyBms::decode_rs485_protocol_(uint16_t protocol) {
  switch (protocol) {
    case 0: return "Standard Modbus";
    case 1: return "PACE";
    default: return "Unknown (" + std::to_string(protocol) + ")";
  }
}

}  // namespace ecoworthy_bms
}  // namespace esphome
