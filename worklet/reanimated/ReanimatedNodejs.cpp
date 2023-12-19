//Todo Caf3

#include "ReanimatedNodejs.h"
#include "NativeModules/NativeReanimatedModule.h"
#include "AgilUIScheduler.h"
#include "v8jsi/V8Runtime.h"
#include "Tools/RuntimeDecorator.h"
#include "Fabric/PropsRegistry.h"
#include "../worklet_log.h"
#include "../ui_thread.h"
//#include "ns/NSLog.h"
//#include "heronsdk.h"
//#include "page/UIPage.h"
//#include "Engine.h"
//#include "application/Heron2React.h"
//#include "platform/AgilTrace.h"

#ifdef __AGIL_YUNOS__
#include <uv.h>
#include <v8.h>
#include <node.h>
#include <env-inl.h>
#include <env.h>
#include <memory/MemoryUtils.h>
#else

#include "uv.h"
#include "atomic"
#include "v8.h"

#endif

namespace reanimated {

//agil2::CondVarWaitableImpl mThreadWait;
std::atomic_bool reanimatedFlag = false;

static void DestroyUV(uv_handle_t *handle) {
#ifdef __AGIL_YUNOS__
    YUNOS_DELETE(handle);
#endif
}

static void HandleLock(uv_async_t *handle) {
    v8::Unlocker unlocker(v8::Isolate::GetCurrent());
    auto start = std::chrono::high_resolution_clock::now();
    while (reanimatedFlag) {

    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    NS_LOGD("ReanimatedModuleLock MainThreadLock , %lld", duration.count());
}

ReanimatedNodejs::ReanimatedNodejs(std::shared_ptr<CallInvoker> callInvoker, rnv8::V8Runtime *jsRuntime) {
    this->callInvoker = std::shared_ptr<CallInvoker>(std::move(callInvoker));
    this->jsRuntime = jsRuntime;
#ifdef __AGIL_YUNOS__
    async_lock = YUNOS_NEW(uv_async_t);
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
    uv_loop_t* loop = static_cast<uv_loop_t*>(node::Environment::GetCurrent(isolate)->event_loop());
    uv_async_init(loop, async_lock, HandleLock);
#else
    async_lock = new uv_async_t();
    uv_loop_t *loop = uv_default_loop();
    uv_async_init(loop, async_lock, HandleLock);
#endif
}

ReanimatedNodejs::~ReanimatedNodejs() {
    uv_close((uv_handle_t *) async_lock, DestroyUV);
}

void ReanimatedNodejs::handleRawEvent(
        uint32_t ref,
        const std::string &name,
        const std::string &paramsAsJson) {
    if (mModule == nullptr || mModule->isHandlerEmpty()) {
        return;
    }
    static const std::unordered_map<std::string, std::string> sReactEvents = {
            {"touchstart",  "onTouchStart"},
            {"touchmove",   "onTouchMove"},
            {"touchcancel", "onTouchCancel"},
            {"touchend",    "onTouchEnd"},
            {"scroll",      "onScroll"}
    };
    auto itr = sReactEvents.find(name);
    if (itr == sReactEvents.end()) {
        return;
    }
    auto eventName = std::to_string(ref) + itr->second;
    if (uiRuntime == nullptr || !mModule->isAnyHandlerWaitingForEvent(eventName, 0)) {
        return;
    }
    auto start = std::chrono::high_resolution_clock::now();
    reanimatedFlag = true;
    uv_async_send(this->async_lock);
    v8::Locker locker(rnv8::V8Runtime::nodejsV8Isolate);
    reanimatedFlag = false;
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    NS_LOGD("ReanimatedModuleLock NSThread , %lld", duration.count());
    auto time = std::chrono::system_clock::now();
    auto timeStamps = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
    if (name == "scroll") {
//        auto payload = agilreact::Heron2React::heronScrollInfoToReactPlayload(*uiRuntime, ref, paramsAsJson);
//        mModule->handleEvent(eventName, 0, payload, timeStamps);
    } else {
//        auto payload = agilreact::Heron2React::heronTouchInfoToReactPlayload(*uiRuntime, ref, paramsAsJson);
//        mModule->handleEvent(eventName, 0, payload, timeStamps);
    }
}

void ReanimatedNodejs::installTurboModule() {
    uiRuntime = reanimated::ReanimatedRuntime::make(jsRuntime);
    auto propsRegistry_ = std::make_shared<reanimated::PropsRegistry>();
    auto uiScheudler = std::make_shared<AgilUIScheduler>();
    auto holder = new reanimated::PlatformDepMethodsHolder();
    static reanimated::NativeReanimatedModule *reanimatedModule = nullptr;
    holder->getCurrentTime = []() -> double {
        auto time = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
    };
    holder->maybeFlushUIUpdatesQueueFunction = [&]() {
        NS_LOGD("ReanimatedModule maybeFlushUIUpdatesQueueFunction\n");
        reanimatedModule->performOperations();
    };
    holder->synchronouslyUpdateUIPropsFunction = [](jsi::Runtime &rt, uint32_t tag, const jsi::Object &props) {};
    holder->requestRender = [&](const std::function<void(double)> &frameCallback, jsi::Runtime &rt) {
//        NS_LOGD("ReanimatedModule requestRender");
        if (frameDone) {
            return;
        }
        frameDone = true;
        auto self = this;

        //mock vsync
        GetHeronSDKManager()->postFrameCallback([self, frameCallback]() {
            auto start = std::chrono::high_resolution_clock::now();
            reanimatedFlag = true;
            uv_async_send(self->async_lock);
            v8::Locker locker(rnv8::V8Runtime::nodejsV8Isolate);
            reanimatedFlag = false;
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//        NS_LOGD("ReanimatedModuleLock NSThread , %lld", duration.count());
            self->frameDone = false;
            auto time = std::chrono::system_clock::now();
            auto timeStamps = std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
            frameCallback(timeStamps);
        });
    };
    auto nativeReanimatedModule = std::make_shared<reanimated::NativeReanimatedModule>(
            callInvoker,
            uiScheudler,
            uiRuntime,
            *holder);
    mModule = nativeReanimatedModule.get();
    reanimatedModule = nativeReanimatedModule.get();
    uiScheudler->setRuntimeManager(nativeReanimatedModule->runtimeManager_);
    nativeReanimatedModule->setPropsRegistry(propsRegistry_);
    reanimated::RuntimeDecorator::decorateRNRuntime(*(static_cast<jsi::Runtime *>(jsRuntime)), uiRuntime, false);
    jsRuntime->global().setProperty(
            *jsRuntime,
            jsi::PropNameID::forAscii(*jsRuntime, "__reanimatedModuleProxy"),
            jsi::Object::createFromHostObject(*jsRuntime, nativeReanimatedModule));
}

}
