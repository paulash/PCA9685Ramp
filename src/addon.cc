#include <napi.h>
#include "servoAsyncWorker.h"

using namespace Napi;

std::unique_ptr<ServoAsyncWorker> _ServoAsyncWorker = NULL;

void Initalize(const CallbackInfo& info) {
    auto device = info[0].As<String>();
    auto address = info[1].As<Number>().Uint32Value();

    Function callback = info[2].As<Function>();
    if (_ServoAsyncWorker == NULL) {
        _ServoAsyncWorker = std::make_unique<ServoAsyncWorker>(callback, 250, 500, device, address);
        _ServoAsyncWorker->Queue();
    }
}

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
  exports["Initalize"] = Function::New(env, Initalize, std::string("Initalize"));
  exports["Shutdown"] = Function::New(env, Shutdown, std::string("Shutdown"));
  return exports;
}

NODE_API_MODULE(addon, Init)