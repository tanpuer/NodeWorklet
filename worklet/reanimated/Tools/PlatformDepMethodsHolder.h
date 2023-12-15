#pragma once

#include <jsi/jsi.h>

#include <string>
#include <utility>
#include <vector>

using namespace facebook;

namespace reanimated {

using SynchronouslyUpdateUIPropsFunction =
    std::function<void(jsi::Runtime &rt, uint32_t tag, const jsi::Object &props)>;
using UpdatePropsFunction =
    std::function<void(jsi::Runtime &rt, const jsi::Value &operations)>;
using RemoveFromPropsRegistryFunction =
    std::function<void(jsi::Runtime &rt, const jsi::Value &viewTags)>;
using DispatchCommandFunction = std::function<void(
    jsi::Runtime &rt,
    const jsi::Value &shadowNodeValue,
    const jsi::Value &commandNameValue,
    const jsi::Value &argsValue)>;
using MeasureFunction = std::function<
    jsi::Value(jsi::Runtime &rt, const jsi::Value &shadowNodeValue)>;


using RequestRender =
    std::function<void(std::function<void(double)>, jsi::Runtime &rt)>;
using TimeProviderFunction = std::function<double(void)>;

using ProgressLayoutAnimationFunction =
    std::function<void(jsi::Runtime &, int, jsi::Object, bool)>;
using EndLayoutAnimationFunction = std::function<void(int, bool)>;

using RegisterSensorFunction =
    std::function<int(int, int, int, std::function<void(double[], int)>)>;
using UnregisterSensorFunction = std::function<void(int)>;
using SetGestureStateFunction = std::function<void(int, int)>;
using ConfigurePropsFunction = std::function<void(
    jsi::Runtime &rt,
    const jsi::Value &uiProps,
    const jsi::Value &nativeProps)>;
using KeyboardEventSubscribeFunction =
    std::function<int(std::function<void(int, int)>, bool)>;
using KeyboardEventUnsubscribeFunction = std::function<void(int)>;
using MaybeFlushUIUpdatesQueueFunction = std::function<void()>;

struct PlatformDepMethodsHolder {
  RequestRender requestRender;
  SynchronouslyUpdateUIPropsFunction synchronouslyUpdateUIPropsFunction;

  TimeProviderFunction getCurrentTime;
  ProgressLayoutAnimationFunction progressLayoutAnimation;
  EndLayoutAnimationFunction endLayoutAnimation;
  RegisterSensorFunction registerSensor;
  UnregisterSensorFunction unregisterSensor;
  SetGestureStateFunction setGestureStateFunction;
  KeyboardEventSubscribeFunction subscribeForKeyboardEvents;
  KeyboardEventUnsubscribeFunction unsubscribeFromKeyboardEvents;
  MaybeFlushUIUpdatesQueueFunction maybeFlushUIUpdatesQueueFunction;
};

} // namespace reanimated
