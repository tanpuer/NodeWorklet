#pragma once


#include <unordered_map>
#include <utility>
#include "functional"
#include "mutex"
#include "folly/dynamic.h"

namespace reanimated {

class PropsRegistry {
 public:
  std::lock_guard<std::mutex> createLock() const;
  // returns a lock you need to hold when calling any of the methods below

  void update(const uint32_t tag, folly::dynamic &&props);

  void remove(const uint32_t tag);

 private:
  std::unordered_map<uint32_t , std::pair<uint32_t, folly::dynamic>> map_;

  mutable std::mutex mutex_; // Protects `map_`.
};

} // namespace reanimated
