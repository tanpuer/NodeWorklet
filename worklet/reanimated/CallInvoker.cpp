#include "CallInvoker.h"
#include "../worklet_log.h"
#include "mutex"

static std::mutex sMutex;
static std::vector<std::function<void()>> sFuncVector;

static void HandleLock(uv_async_t *handle) {
    NS_LOGD("HandleLock start\n");
    std::lock_guard<std::mutex> lockGuard(sMutex);
    for (const auto &item: sFuncVector) {
        item();
    }
    sFuncVector.clear();
    NS_LOGD("HandleLock finish function\n");
}

reanimated::CallInvoker::CallInvoker() {
    async_lock = new uv_async_t();
    uv_async_init(uv_default_loop(), async_lock, HandleLock);
}

reanimated::CallInvoker::~CallInvoker() {
    uv_close((uv_handle_t *) async_lock, nullptr);
    sFuncVector.clear();
}

void reanimated::CallInvoker::invokeAsync(std::function<void()> func) {
    //Todo Caf3，暂时完全用uv接口
    std::lock_guard<std::mutex> lockGuard(sMutex);
    sFuncVector.emplace_back(std::move(func));
    uv_async_send(this->async_lock);
}
