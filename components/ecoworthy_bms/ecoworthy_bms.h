#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/button/button.h"
#include "esphome/components/ecoworthy_modbus/ecoworthy_modbus.h"

namespace esphome {
namespace ecoworthy_bms {

class EcoworthyBms;

// Switch classes for MOS control (JK-BMS naming convention)
class ChargingSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(EcoworthyBms *parent) { this->parent_ = parent; }
  void write_state(bool state) override;
 protected:
  EcoworthyBms *parent_;
};

class DischargingSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(EcoworthyBms *parent) { this->parent_ = parent; }
  void write_state(bool state) override;
 protected:
  EcoworthyBms *parent_;
};

// Button classes for actions
class StandbySleepButton : public button::Button, public Component {
 public:
  void set_parent(EcoworthyBms *parent) { this->parent_ = parent; }
  void press_action() override;
 protected:
  EcoworthyBms *parent_;
};

class DeepSleepButton : public button::Button, public Component {
 public:
  void set_parent(EcoworthyBms *parent) { this->parent_ = parent; }
  void press_action() override;
 protected:
  EcoworthyBms *parent_;
};

class EcoworthyBms : public PollingComponent, public ecoworthy_modbus::EcoworthyModbusDevice {
 public:
  // Binary sensors
  void set_online_status_binary_sensor(binary_sensor::BinarySensor *online_status) {
    online_status_binary_sensor_ = online_status;
  }
  void set_charging_binary_sensor(binary_sensor::BinarySensor *charging) { charging_binary_sensor_ = charging; }
  void set_discharging_binary_sensor(binary_sensor::BinarySensor *discharging) {
    discharging_binary_sensor_ = discharging;
  }
  void set_charging_switch_binary_sensor(binary_sensor::BinarySensor *charging_switch) { charging_switch_binary_sensor_ = charging_switch; }
  void set_discharging_switch_binary_sensor(binary_sensor::BinarySensor *discharging_switch) { discharging_switch_binary_sensor_ = discharging_switch; }
  void set_balancing_binary_sensor(binary_sensor::BinarySensor *balancing) { balancing_binary_sensor_ = balancing; }

  // Voltage sensors
  void set_total_voltage_sensor(sensor::Sensor *total_voltage) { total_voltage_sensor_ = total_voltage; }
  void set_cell_voltage_sensor(uint8_t cell, sensor::Sensor *cell_voltage) {
    this->cells_[cell].cell_voltage_sensor_ = cell_voltage;
  }
  void set_min_cell_voltage_sensor(sensor::Sensor *min_cell_voltage) { min_cell_voltage_sensor_ = min_cell_voltage; }
  void set_max_cell_voltage_sensor(sensor::Sensor *max_cell_voltage) { max_cell_voltage_sensor_ = max_cell_voltage; }
  void set_delta_cell_voltage_sensor(sensor::Sensor *delta_cell_voltage) {
    delta_cell_voltage_sensor_ = delta_cell_voltage;
  }
  void set_average_cell_voltage_sensor(sensor::Sensor *average_cell_voltage) {
    average_cell_voltage_sensor_ = average_cell_voltage;
  }
  void set_min_voltage_cell_sensor(sensor::Sensor *min_voltage_cell) {
    min_voltage_cell_sensor_ = min_voltage_cell;
  }
  void set_max_voltage_cell_sensor(sensor::Sensor *max_voltage_cell) {
    max_voltage_cell_sensor_ = max_voltage_cell;
  }

  // Current and power sensors
  void set_current_sensor(sensor::Sensor *current) { current_sensor_ = current; }
  void set_power_sensor(sensor::Sensor *power) { power_sensor_ = power; }
  void set_charging_power_sensor(sensor::Sensor *charging_power) { charging_power_sensor_ = charging_power; }
  void set_discharging_power_sensor(sensor::Sensor *discharging_power) {
    discharging_power_sensor_ = discharging_power;
  }

  // Temperature sensors
  void set_temperature_sensor(uint8_t temp, sensor::Sensor *temperature) {
    this->temperatures_[temp].temperature_sensor_ = temperature;
  }
  void set_power_tube_temperature_sensor(sensor::Sensor *power_tube_temp) { power_tube_temperature_sensor_ = power_tube_temp; }
  void set_ambient_temperature_sensor(sensor::Sensor *ambient_temp) { ambient_temperature_sensor_ = ambient_temp; }
  void set_min_temperature_sensor(sensor::Sensor *min_temp) { min_temperature_sensor_ = min_temp; }
  void set_max_temperature_sensor(sensor::Sensor *max_temp) { max_temperature_sensor_ = max_temp; }
  void set_avg_temperature_sensor(sensor::Sensor *avg_temp) { avg_temperature_sensor_ = avg_temp; }

  // Capacity sensors
  void set_remaining_capacity_sensor(sensor::Sensor *remaining_capacity) {
    remaining_capacity_sensor_ = remaining_capacity;
  }
  void set_full_capacity_sensor(sensor::Sensor *full_capacity) { full_capacity_sensor_ = full_capacity; }
  void set_rated_capacity_sensor(sensor::Sensor *rated_capacity) { rated_capacity_sensor_ = rated_capacity; }

  // State sensors
  void set_state_of_charge_sensor(sensor::Sensor *soc) { state_of_charge_sensor_ = soc; }
  void set_state_of_health_sensor(sensor::Sensor *soh) { state_of_health_sensor_ = soh; }
  void set_cycle_count_sensor(sensor::Sensor *cycle_count) { cycle_count_sensor_ = cycle_count; }
  void set_cell_count_sensor(sensor::Sensor *cell_count) { cell_count_sensor_ = cell_count; }
  void set_temperature_sensor_count_sensor(sensor::Sensor *temp_count) { temperature_sensor_count_sensor_ = temp_count; }

  // Charge/Discharge limits
  void set_charge_voltage_limit_sensor(sensor::Sensor *cvl) { charge_voltage_limit_sensor_ = cvl; }
  void set_charge_current_limit_sensor(sensor::Sensor *ccl) { charge_current_limit_sensor_ = ccl; }
  void set_discharge_voltage_limit_sensor(sensor::Sensor *dvl) { discharge_voltage_limit_sensor_ = dvl; }
  void set_discharge_current_limit_sensor(sensor::Sensor *dcl) { discharge_current_limit_sensor_ = dcl; }

  // Bitmask sensors
  void set_fault_bitmask_sensor(sensor::Sensor *fault_bitmask) { fault_bitmask_sensor_ = fault_bitmask; }
  void set_alarm_bitmask_sensor(sensor::Sensor *alarm_bitmask) { alarm_bitmask_sensor_ = alarm_bitmask; }
  void set_mosfet_status_bitmask_sensor(sensor::Sensor *mosfet_bitmask) { mosfet_status_bitmask_sensor_ = mosfet_bitmask; }
  void set_balancing_bitmask_sensor(sensor::Sensor *balancing_bitmask) { balancing_bitmask_sensor_ = balancing_bitmask; }

  // Configuration sensors (from 0x1C00 and 0x2000 blocks)
  void set_balance_voltage_sensor(sensor::Sensor *s) { balance_voltage_sensor_ = s; }
  void set_balance_difference_sensor(sensor::Sensor *s) { balance_difference_sensor_ = s; }
  void set_heater_start_temp_sensor(sensor::Sensor *s) { heater_start_temp_sensor_ = s; }
  void set_heater_stop_temp_sensor(sensor::Sensor *s) { heater_stop_temp_sensor_ = s; }
  void set_full_charge_voltage_sensor(sensor::Sensor *s) { full_charge_voltage_sensor_ = s; }
  void set_full_charge_current_sensor(sensor::Sensor *s) { full_charge_current_sensor_ = s; }
  void set_sleep_voltage_sensor(sensor::Sensor *s) { sleep_voltage_sensor_ = s; }
  void set_sleep_delay_sensor(sensor::Sensor *s) { sleep_delay_sensor_ = s; }
  void set_total_charge_sensor(sensor::Sensor *s) { total_charge_sensor_ = s; }
  void set_total_discharge_sensor(sensor::Sensor *s) { total_discharge_sensor_ = s; }
  void set_configured_cvl_sensor(sensor::Sensor *s) { configured_cvl_sensor_ = s; }
  void set_configured_ccl_sensor(sensor::Sensor *s) { configured_ccl_sensor_ = s; }
  void set_configured_dvl_sensor(sensor::Sensor *s) { configured_dvl_sensor_ = s; }
  void set_configured_dcl_sensor(sensor::Sensor *s) { configured_dcl_sensor_ = s; }
  void set_shunt_resistance_sensor(sensor::Sensor *s) { shunt_resistance_sensor_ = s; }

  // Product info sensors (from 0x2810 block)
  void set_hardware_version_sensor(sensor::Sensor *s) { hardware_version_sensor_ = s; }

  // Text sensors
  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status) { operation_status_text_sensor_ = operation_status; }
  void set_fault_text_sensor(text_sensor::TextSensor *fault) { fault_text_sensor_ = fault; }
  void set_alarm_text_sensor(text_sensor::TextSensor *alarm) { alarm_text_sensor_ = alarm; }
  void set_serial_number_text_sensor(text_sensor::TextSensor *serial) { serial_number_text_sensor_ = serial; }
  void set_firmware_version_text_sensor(text_sensor::TextSensor *firmware) { firmware_text_sensor_ = firmware; }
  void set_bms_serial_number_text_sensor(text_sensor::TextSensor *s) { bms_serial_number_text_sensor_ = s; }
  void set_pack_serial_number_text_sensor(text_sensor::TextSensor *s) { pack_serial_number_text_sensor_ = s; }
  void set_manufacturer_text_sensor(text_sensor::TextSensor *s) { manufacturer_text_sensor_ = s; }
  void set_bms_model_text_sensor(text_sensor::TextSensor *s) { bms_model_text_sensor_ = s; }
  void set_balance_mode_text_sensor(text_sensor::TextSensor *s) { balance_mode_text_sensor_ = s; }
  void set_can_protocol_text_sensor(text_sensor::TextSensor *s) { can_protocol_text_sensor_ = s; }
  void set_rs485_protocol_text_sensor(text_sensor::TextSensor *s) { rs485_protocol_text_sensor_ = s; }

  // Switches for MOS control (JK-BMS naming convention)
  void set_charging_switch(ChargingSwitch *s) { charging_switch_ = s; }
  void set_discharging_switch(DischargingSwitch *s) { discharging_switch_ = s; }

  // Buttons for sleep modes
  void set_standby_sleep_button(StandbySleepButton *b) { standby_sleep_button_ = b; }
  void set_deep_sleep_button(DeepSleepButton *b) { deep_sleep_button_ = b; }

  void on_modbus_data(const std::vector<uint8_t> &data) override;

  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

  // MOS control methods (called by switches)
  void set_charge_mos(bool state);
  void set_discharge_mos(bool state);
  void set_sleep_mode(uint8_t mode);  // 0xA501 = standby, 0xA502 = deep

  // Current MOS state (for switches)
  bool get_charge_mos_state() const { return charge_mos_state_; }
  bool get_discharge_mos_state() const { return discharge_mos_state_; }

 protected:
  // Binary sensors
  binary_sensor::BinarySensor *online_status_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *charging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *discharging_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *charging_switch_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *discharging_switch_binary_sensor_{nullptr};
  binary_sensor::BinarySensor *balancing_binary_sensor_{nullptr};

  // Voltage sensors
  sensor::Sensor *total_voltage_sensor_{nullptr};
  sensor::Sensor *min_cell_voltage_sensor_{nullptr};
  sensor::Sensor *max_cell_voltage_sensor_{nullptr};
  sensor::Sensor *delta_cell_voltage_sensor_{nullptr};
  sensor::Sensor *average_cell_voltage_sensor_{nullptr};
  sensor::Sensor *min_voltage_cell_sensor_{nullptr};
  sensor::Sensor *max_voltage_cell_sensor_{nullptr};

  // Current and power sensors
  sensor::Sensor *current_sensor_{nullptr};
  sensor::Sensor *power_sensor_{nullptr};
  sensor::Sensor *charging_power_sensor_{nullptr};
  sensor::Sensor *discharging_power_sensor_{nullptr};

  // Temperature sensors
  sensor::Sensor *power_tube_temperature_sensor_{nullptr};
  sensor::Sensor *ambient_temperature_sensor_{nullptr};
  sensor::Sensor *min_temperature_sensor_{nullptr};
  sensor::Sensor *max_temperature_sensor_{nullptr};
  sensor::Sensor *avg_temperature_sensor_{nullptr};

  // Capacity sensors
  sensor::Sensor *remaining_capacity_sensor_{nullptr};
  sensor::Sensor *full_capacity_sensor_{nullptr};
  sensor::Sensor *rated_capacity_sensor_{nullptr};

  // State sensors
  sensor::Sensor *state_of_charge_sensor_{nullptr};
  sensor::Sensor *state_of_health_sensor_{nullptr};
  sensor::Sensor *cycle_count_sensor_{nullptr};
  sensor::Sensor *cell_count_sensor_{nullptr};
  sensor::Sensor *temperature_sensor_count_sensor_{nullptr};

  // Charge/Discharge limits
  sensor::Sensor *charge_voltage_limit_sensor_{nullptr};
  sensor::Sensor *charge_current_limit_sensor_{nullptr};
  sensor::Sensor *discharge_voltage_limit_sensor_{nullptr};
  sensor::Sensor *discharge_current_limit_sensor_{nullptr};

  // Bitmask sensors
  sensor::Sensor *fault_bitmask_sensor_{nullptr};
  sensor::Sensor *alarm_bitmask_sensor_{nullptr};
  sensor::Sensor *mosfet_status_bitmask_sensor_{nullptr};
  sensor::Sensor *balancing_bitmask_sensor_{nullptr};

  // Configuration sensors
  sensor::Sensor *balance_voltage_sensor_{nullptr};
  sensor::Sensor *balance_difference_sensor_{nullptr};
  sensor::Sensor *heater_start_temp_sensor_{nullptr};
  sensor::Sensor *heater_stop_temp_sensor_{nullptr};
  sensor::Sensor *full_charge_voltage_sensor_{nullptr};
  sensor::Sensor *full_charge_current_sensor_{nullptr};
  sensor::Sensor *sleep_voltage_sensor_{nullptr};
  sensor::Sensor *sleep_delay_sensor_{nullptr};
  sensor::Sensor *total_charge_sensor_{nullptr};
  sensor::Sensor *total_discharge_sensor_{nullptr};
  sensor::Sensor *configured_cvl_sensor_{nullptr};
  sensor::Sensor *configured_ccl_sensor_{nullptr};
  sensor::Sensor *configured_dvl_sensor_{nullptr};
  sensor::Sensor *configured_dcl_sensor_{nullptr};
  sensor::Sensor *shunt_resistance_sensor_{nullptr};
  sensor::Sensor *hardware_version_sensor_{nullptr};

  // Text sensors
  text_sensor::TextSensor *operation_status_text_sensor_{nullptr};
  text_sensor::TextSensor *fault_text_sensor_{nullptr};
  text_sensor::TextSensor *alarm_text_sensor_{nullptr};
  text_sensor::TextSensor *serial_number_text_sensor_{nullptr};
  text_sensor::TextSensor *firmware_text_sensor_{nullptr};
  text_sensor::TextSensor *bms_serial_number_text_sensor_{nullptr};
  text_sensor::TextSensor *pack_serial_number_text_sensor_{nullptr};
  text_sensor::TextSensor *manufacturer_text_sensor_{nullptr};
  text_sensor::TextSensor *bms_model_text_sensor_{nullptr};
  text_sensor::TextSensor *balance_mode_text_sensor_{nullptr};
  text_sensor::TextSensor *can_protocol_text_sensor_{nullptr};
  text_sensor::TextSensor *rs485_protocol_text_sensor_{nullptr};

  // Switches
  ChargingSwitch *charging_switch_{nullptr};
  DischargingSwitch *discharging_switch_{nullptr};

  // Buttons
  StandbySleepButton *standby_sleep_button_{nullptr};
  DeepSleepButton *deep_sleep_button_{nullptr};

  struct Cell {
    sensor::Sensor *cell_voltage_sensor_{nullptr};
  } cells_[16];

  struct Temperature {
    sensor::Sensor *temperature_sensor_{nullptr};
  } temperatures_[4];

  uint8_t no_response_count_{0};
  uint32_t update_counter_{0};
  uint8_t request_step_{0};

  // Current MOS states
  bool charge_mos_state_{false};
  bool discharge_mos_state_{false};

  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  
  void on_pack_status_data_(const std::vector<uint8_t> &data);
  void on_config_1c00_data_(const std::vector<uint8_t> &data);
  void on_config_2000_data_(const std::vector<uint8_t> &data);
  void on_product_info_data_(const std::vector<uint8_t> &data);
  
  void reset_online_status_tracker_();
  void track_online_status_();
  void publish_device_unavailable_();
  
  std::string decode_operation_status_(uint16_t status);
  std::string decode_fault_(uint32_t fault);
  std::string decode_alarm_(uint32_t alarm);
  std::string decode_balance_mode_(uint16_t mode);
  std::string decode_can_protocol_(uint16_t protocol);
  std::string decode_rs485_protocol_(uint16_t protocol);
};

}  // namespace ecoworthy_bms
}  // namespace esphome
