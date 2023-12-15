#include "ReanimatedRuntime.h"

#include <memory>
#include <utility>
#include <v8jsi/V8RuntimeFactory.h>

namespace reanimated {

std::shared_ptr<jsi::Runtime> ReanimatedRuntime::make(
        jsi::Runtime *rnRuntime) {
    (void) rnRuntime;
    auto config = std::make_unique<rnv8::V8RuntimeConfig>();
    config->enableInspector = false;
    config->appName = "reanimated";
    return rnv8::createSharedV8Runtime(rnRuntime, std::move(config));
}

} // namespace reanimated
