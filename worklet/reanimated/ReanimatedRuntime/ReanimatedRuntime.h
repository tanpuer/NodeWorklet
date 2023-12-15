#pragma once

#include <jsi/jsi.h>

#include <memory>

using namespace facebook;

namespace reanimated {

class ReanimatedRuntime {
public:
    static std::shared_ptr<jsi::Runtime> make(
            jsi::Runtime *rnRuntime);
};

} // namespace reanimated
