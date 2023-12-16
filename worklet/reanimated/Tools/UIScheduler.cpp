#include "UIScheduler.h"
#include "ReanimatedRuntime/ReanimatedRuntime.h"
#include "SharedItems/RuntimeManager.h"

#include <utility>

namespace reanimated {

void UIScheduler::scheduleOnUI(std::function<void()> job) {
  uiJobs_.push(std::move(job));
}

void UIScheduler::triggerUI() {
  scheduledOnUI_ = false;
  while (uiJobs_.getSize()) {
    const auto job = uiJobs_.pop();
    job();
  }
}

void UIScheduler::setRuntimeManager(
    const std::shared_ptr<RuntimeManager> &runtimeManager) {
  weakRuntimeManager_ = runtimeManager;
}

UIScheduler::~UIScheduler() {}

} // namespace reanimated
