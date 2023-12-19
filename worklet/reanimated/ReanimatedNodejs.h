#pragma once

#include "NativeModules/NativeReanimatedModule.h"
#include "CallInvoker.h"
#include "uv.h"
#include "V8Runtime.h"
#include "atomic"

namespace reanimated {

class ReanimatedNodejs {

public:

    ReanimatedNodejs(std::shared_ptr<CallInvoker> callInvoker, rnv8::V8Runtime *jsRuntime);

    ~ReanimatedNodejs();

    reanimated::NativeReanimatedModule *mModule;

    void handleRawEvent(uint32_t ref, const std::string &name, const std::string &paramsAsJson);

    void installTurboModule();

private:

    bool frameDone = false;

    uv_async_t *async_lock = nullptr;

    std::shared_ptr<jsi::Runtime> uiRuntime = nullptr;

    std::shared_ptr<CallInvoker> callInvoker = nullptr;

    rnv8::V8Runtime *jsRuntime = nullptr;

};
}