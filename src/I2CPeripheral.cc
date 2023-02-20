#include "I2CPeripheral.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <system_error>
#include <iostream>
#include "i2c-dev.h"


I2CPeripheral::I2CPeripheral(const std::string& device, const uint8_t address) {
  OpenBus(device);
  ConnectToPeripheral(address);
}

I2CPeripheral::~I2CPeripheral() {
    std::cout << "shutting down via deconstructor.\n";
  close(bus_fd);
}

void I2CPeripheral::CloseConnection() {
    close(bus_fd);
}

void I2CPeripheral::WriteRegisterByte(const uint8_t register_address, const uint8_t value) {
  i2c_smbus_data data;
  data.byte = value;
  const auto err = i2c_smbus_access(bus_fd, I2C_SMBUS_WRITE, register_address, I2C_SMBUS_BYTE_DATA, &data);
  if(err) {
    const auto msg = "Could not write value (" + std::to_string(value) + ") to register " + std::to_string(register_address) + "\n";
    std::cout << msg;
  }
}

uint8_t I2CPeripheral::ReadRegisterByte(const uint8_t register_address) {
  i2c_smbus_data data;
  const auto err = i2c_smbus_access(bus_fd, I2C_SMBUS_READ, register_address, I2C_SMBUS_BYTE_DATA, &data);
  if(err) {
    const auto msg = "Could not read value at register " + std::to_string(register_address) + "\n";
    std::cout << msg;
  }
  return data.byte & 0xFF;
}

void I2CPeripheral::OpenBus(const std::string& device) {
  bus_fd = open(device.c_str(), O_RDWR);
  if(bus_fd < 0) {
    std::cout << "Could not open i2c bus.\n";
  }
}

void I2CPeripheral::ConnectToPeripheral(const uint8_t address) {
  if(ioctl(bus_fd, I2C_SLAVE, address) < 0) {
    std::cout << "Could not set peripheral address.\n";
  }
}