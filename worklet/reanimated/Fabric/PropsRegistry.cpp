
#include "PropsRegistry.h"

namespace reanimated {

std::lock_guard<std::mutex> PropsRegistry::createLock() const {
  return std::lock_guard<std::mutex>(mutex_);
}

void PropsRegistry::update(
    const uint32_t tag,
    folly::dynamic &&props) {
  const auto it = map_.find(tag);
  if (it == map_.cend()) {
    // we need to store ShadowNode because `ShadowNode::getFamily`
    // returns `ShadowNodeFamily const &` which is non-owning
    map_[tag] = std::make_pair(tag, props);
  } else {
    // no need to update `.first` because ShadowNodeFamily doesn't change
    // merge new props with old props
    it->second.second.update(props);
  }
}

void PropsRegistry::remove(const uint32_t tag) {
  map_.erase(tag);
}

} // namespace reanimated
