#include <napi.h>
#include "servoAsyncWorker.h"

using namespace Napi;

std::unique_ptr<ServoAsyncWorker> _ServoAsyncWorker = NULL;

void DoNothing(const CallbackInfo& info) {}

// minPWM, maxPWM, i2c device, i2c address.
void Initialize(const CallbackInfo& info) {
    Napi::Env env = info.Env();

    auto minPWM = info[0].As<Number>().Uint32Value();
    auto maxPWM = info[1].As<Number>().Uint32Value();

    auto device = info[2].As<String>();
    auto address = info[3].As<Number>().Uint32Value();

    
    uint16_t pwms[ALL_SERVO];
    Array startDegrees = info[4].As<Array>();

    if (startDegrees.Length() != ALL_SERVO) {
        Napi::TypeError::New(env, "Invalid startDegrees length, must be 16 u32 values between 0-255.")
            .ThrowAsJavaScriptException();
        std::cout << "invalid startDegrees length";
        return;
    }
    
    for (uint32_t i=0; i < startDegrees.Length(); i++) {
        Value v = startDegrees[i];
        if (v.IsNumber()) {
            pwms[i] = v.As<Number>().Uint32Value();
        } else {
            Napi::TypeError::New(env, "Invalid startDegrees length, must be 16 u32 values between 0-255.")
                .ThrowAsJavaScriptException();
            return;
        }
    }
    
    Function callback = Function::New<DoNothing>(env);
    if (_ServoAsyncWorker == NULL) {
        _ServoAsyncWorker = std::make_unique<ServoAsyncWorker>(callback, minPWM, maxPWM, device, address, pwms);
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