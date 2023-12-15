#include "AgilUIScheduler.h"

#include <utility>
#include "v8.h"

void AgilUIScheduler::scheduleOnUI(std::function<void()> job) {
    UIScheduler::scheduleOnUI(job);
    if (!scheduledOnUI_) {
        scheduledOnUI_ = true;
        v8::Unlocker unlocker(v8::Isolate::GetCurrent());
        //Todo Caf3
//        NS_LOGD("AgilUIScheduler PerformOnNSUIThread start");
//        GetHeronSDKManager()->PerformOnNSUIThread([this]() {
//            NS_LOGD("AgilUIScheduler PerformOnNSUIThread work");
//            triggerUI();
//        }, true);
    }
}

void AgilUIScheduler::triggerUI() {
    try {
        UIScheduler::triggerUI();
    } catch (int err) {

    }
}