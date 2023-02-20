#pragma once
#include <napi.h>

#include <chrono>
#include <thread>
#include <pthread.h>
#include "ramp.h"
#include "I2CPeripheral.h"

using namespace Napi;

// Registers/etc:
constexpr uint8_t MODE1              = 0x00;
constexpr uint8_t MODE2              = 0x01;
constexpr uint8_t SUBADR1            = 0x02;
constexpr uint8_t SUBADR2            = 0x03;
constexpr uint8_t SUBADR3            = 0x04;
constexpr uint8_t PRESCALE           = 0xFE;
constexpr uint8_t LED0_ON_L          = 0x06;
constexpr uint8_t LED0_ON_H          = 0x07;
constexpr uint8_t LED0_OFF_L         = 0x08;
constexpr uint8_t LED0_OFF_H         = 0x09;
constexpr uint8_t ALL_LED_ON_L       = 0xFA;
constexpr uint8_t ALL_LED_ON_H       = 0xFB;
constexpr uint8_t ALL_LED_OFF_L      = 0xFC;
constexpr uint8_t ALL_LED_OFF_H      = 0xFD;

// Bits:
constexpr uint8_t RESTART            = 0x80;
constexpr uint8_t SLEEP              = 0x10;
constexpr uint8_t ALLCALL            = 0x01;
constexpr uint8_t INVRT              = 0x10;
constexpr uint8_t OUTDRV             = 0x04;

#define ALL_SERVO 16

class ServoAsyncWorker : public AsyncWorker {
 public:
  ServoAsyncWorker(Function& callback,
    const uint16_t minServoPWM, const uint16_t maxServoPWM, 
    const std::string& device, const uint8_t address, 
    const uint16_t (&pwms)[ALL_SERVO]);
  virtual ~ServoAsyncWorker(){};

  void Execute();
  void OnOK() {};

private:
    void set_pwm_freq(const double freq_hz);
    void set_pwm(const int channel, const uint16_t on, const uint16_t off);
    void set_all_pwm(const uint16_t on, const uint16_t off);
    void set_pwm_ms(const int channel, const double ms);

    std::unique_ptr<I2CPeripheral> i2c_dev;
    double frequency = 200.0;
    bool shutdown = false;

    uint16_t MinServoPWM = 200;
    uint16_t MaxServoPWM = 500;

public:
    void SetPWM(uint16_t servoIndex, uint16_t targetDegrees, uint16_t duration) {
        pthread_mutex_lock(&ramp_mutex);

        // clamp the max degrees to the valid range, 0-255.
        if (targetDegrees > 255) {
            targetDegrees = 255;
        }

        // remap the degrees from 0-255 to MinServoPWM-MaxServoPWM pwm
        uint16_t out_max = MaxServoPWM, out_min = MinServoPWM, in_min = 0, in_max = 255;
        uint16_t pwm = (targetDegrees - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;

        ramps[servoIndex].go(pwm, duration);
        pthread_mutex_unlock(&ramp_mutex);
    };

    void Shutdown() {
        shutdown = true;
    };

private:
    pthread_mutex_t ramp_mutex = PTHREAD_MUTEX_INITIALIZER;

    Ramp ramps[ALL_SERVO] = { 
        Ramp(), Ramp(), Ramp(), Ramp(),
        Ramp(), Ramp(), Ramp(), Ramp(),
        Ramp(), Ramp(), Ramp(), Ramp(),
        Ramp(), Ramp(), Ramp(), Ramp()
    };
};