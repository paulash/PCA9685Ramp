#include <napi.h>
#include "servoAsyncWorker.h"

using namespace Napi;

std::unique_ptr<ServoAsyncWorker> _ServoAsyncWorker = NULL;

// minPWM, maxPWM, i2c device, i2c address.
void Initialize(const CallbackInfo& info) {
    auto minPWM = info[0].As<Number>().Uint32Value();
    auto maxPWM = info[1].As<Number>().Uint32Value();

    auto device = info[2].As<String>();
    auto address = info[3].As<Number>().Uint32Value();

    Function callback = info[4].As<Function>();
    if (_ServoAsyncWorker == NULL) {
        _ServoAsyncWorker = std::make_unique<ServoAsyncWorker>(callback, minPWM, maxPWM, device, address);
        _ServoAsyncWorker->Queue();
    }
}

// servoIndex, targetDegrees [between 0-255 remapped to the minPWM/maxPWM], duration in milliseconds to arrive at the new degrees.
void SetPWM(const CallbackInfo& info) {
    uint16_t servoIndex = info[0].As<Number>().Uint32Value();
    uint16_t targetDegrees = info[1].As<Number>().Uint32Value();
    uint32_t duration = info[2].As<Number>().Uint32Value();

    _ServoAsyncWorker->SetPWM(servoIndex, targetDegrees, duration);
}

void Shutdown(const CallbackInfo& info) {
    _ServoAsyncWorker->Shutdown();
    _ServoAsyncWorker = NULL;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports["SetPWM"] = Function::New(env, SetPWM, std::string("SetPWM"));
  exports["Initialize"] = Function::New(env, Initialize, std::string("Initialize"));
  exports["Shutdown"] = Function::New(env, Shutdown, std::string("Shutdown"));
  return exports;
}

NODE_API_MODULE(addon, Init)