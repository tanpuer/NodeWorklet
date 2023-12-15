#pragma once

#include <memory>
#include <functional>
#include "../CallInvoker.h"

namespace reanimated {

class JSScheduler {
 public:
  explicit JSScheduler(
      const std::shared_ptr<CallInvoker> &jsCallInvoker)
      : jsCallInvoker_(jsCallInvoker) {}
  void scheduleOnJS(std::function<void()> job);

 protected:
  const std::shared_ptr<CallInvoker> jsCallInvoker_;
};

} // namespace reanimated
