#pragma once

#include <memory>
#include "./LoggerInterface.h"

namespace reanimated {

class Logger {
 public:
  template <typename T>
  static void log(T value) {
    static auto logger = std::make_unique<LoggerImpl>();
    logger->log(value);
  }

};

} // namespace reanimated
