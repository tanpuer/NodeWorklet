#include "AgilUIScheduler.h"

#include <utility>
#include "v8.h"
#include "../ui_thread.h"
#include "../worklet_log.h"
#include "future"

void AgilUIScheduler::scheduleOnUI(std::function<void()> job) {
    UIScheduler::scheduleOnUI(job);
    if (!scheduledOnUI_) {
        scheduledOnUI_ = true;
        v8::Unlocker unlocker(v8::Isolate::GetCurrent());

//        NS_LOGD("AgilUIScheduler PerformOnNSUIThread start");
//        GetHeronSDKManager()->PerformOnNSUIThread([this]() {
//            NS_LOGD("AgilUIScheduler PerformOnNSUIThread work");
//            triggerUI();
//        }, true);

        //Todo Caf3
        NS_LOGD("AgilUIScheduler PerformOnNSUIThread start\n");
        std::future<void> future;
        std::promise<void> promise;
        future = promise.get_future();
        GetHeronSDKManager()->PerformOnNSUIThread([this, &promise]() {
            NS_LOGD("AgilUIScheduler PerformOnNSUIThread work\n");
            triggerUI();
            promise.set_value();
        }, true);
        future.wait();
    }
}

void AgilUIScheduler::triggerUI() {
    try {
        UIScheduler::triggerUI();
    } catch (int err) {

    }
}