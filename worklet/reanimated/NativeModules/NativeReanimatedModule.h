#pragma once

#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>
#include "jsi/jsi.h"

#include "../Tools/PlatformDepMethodsHolder.h"
#include "../Tools/RuntimeDecorator.h"
#include "../SharedItems/RuntimeManager.h"
#include "../Tools/UIScheduler.h"
#include "../Tools/JSLogger.h"
#include "../SharedItems/JSRuntimeHelper.h"

#include "../Fabric/PropsRegistry.h"
#include "../CallInvoker.h"

namespace reanimated {

using FrameCallback = std::function<void(double)>;

class EventHandlerRegistry;

class NativeReanimatedModule : public facebook::jsi::HostObject {
public:
    NativeReanimatedModule(
            const std::shared_ptr<CallInvoker> &jsInvoker,
            const std::shared_ptr<UIScheduler> &uiScheduler,
            const std::shared_ptr<jsi::Runtime> &rt,
            PlatformDepMethodsHolder platformDepMethodsHolder);

    ~NativeReanimatedModule();

    std::shared_ptr<RuntimeManager> runtimeManager_;
    std::shared_ptr<JSRuntimeHelper> runtimeHelper;

    void installCoreFunctions(
            jsi::Runtime &rt,
            const jsi::Value &callGuard,
            const jsi::Value &valueUnpacker);

    jsi::Value makeShareableClone(
            jsi::Runtime &rt,
            const jsi::Value &value,
            const jsi::Value &shouldRetainRemote);

    jsi::Value makeSynchronizedDataHolder(
            jsi::Runtime &rt,
            const jsi::Value &initialShareable);

    jsi::Value getDataSynchronously(
            jsi::Runtime &rt,
            const jsi::Value &synchronizedDataHolderRef);

    void updateDataSynchronously(
            jsi::Runtime &rt,
            const jsi::Value &synchronizedDataHolderRef,
            const jsi::Value &newData);

    void scheduleOnUI(jsi::Runtime &rt, const jsi::Value &worklet);

    void scheduleOnJS(
            jsi::Runtime &rt,
            const jsi::Value &remoteFun,
            const jsi::Value &argsValue);

    jsi::Value registerEventHandler(
            jsi::Runtime &rt,
            const jsi::Value &eventHash,
            const jsi::Value &worklet);

    void unregisterEventHandler(
            jsi::Runtime &rt,
            const jsi::Value &registrationId);

    jsi::Value getViewProp(
            jsi::Runtime &rt,
            const jsi::Value &viewTag,
            const jsi::Value &propName,
            const jsi::Value &callback);

    jsi::Value enableLayoutAnimations(jsi::Runtime &rt, const jsi::Value &config);

    jsi::Value configureProps(
            jsi::Runtime &rt,
            const jsi::Value &uiProps,
            const jsi::Value &nativeProps);

    jsi::Value configureLayoutAnimation(
            jsi::Runtime &rt,
            const jsi::Value &viewTag,
            const jsi::Value &type,
            const jsi::Value &sharedTransitionTag,
            const jsi::Value &config);

    void onRender(double timestampMs);

    bool isAnyHandlerWaitingForEvent(
            const std::string &eventName,
            const int emitterReactTag);

    bool isHandlerEmpty();

    void maybeRequestRender();

    UpdatePropsFunction updatePropsFunction;

    bool handleEvent(
            const std::string &eventName,
            const int emitterReactTag,
            const jsi::Value &payload,
            double currentTime);

    bool handleRawEvent(uint32_t ref, const std::string &name, const std::string &paramsAsJson, double currentTime);

    void updateProps(jsi::Runtime &rt, const jsi::Value &operations);

    void removeFromPropsRegistry(jsi::Runtime &rt, const jsi::Value &viewTags);

    void performOperations();

    void dispatchCommand(
            jsi::Runtime &rt,
            const jsi::Value &shadowNodeValue,
            const jsi::Value &commandNameValue,
            const jsi::Value &argsValue);

    jsi::Value measure(jsi::Runtime &rt, const jsi::Value &shadowNodeValue);

    void setPropsRegistry(std::shared_ptr<PropsRegistry> propsRegistry);

    jsi::Value get(facebook::jsi::Runtime &, const facebook::jsi::PropNameID &name) override;



private:
    bool isThereAnyLayoutProp(jsi::Runtime &rt, const jsi::Object &props);

    std::unique_ptr<EventHandlerRegistry> eventHandlerRegistry;
    std::function<void(FrameCallback &, jsi::Runtime &)> requestRender;
    std::vector<FrameCallback> frameCallbacks;
    bool renderRequested = false;

    std::function<jsi::Value(jsi::Runtime &, const int, const jsi::String &)

    >
            propObtainer;
    std::function<void(double)> onRenderCallback;
    ConfigurePropsFunction configurePropsPlatformFunction;

    SynchronouslyUpdateUIPropsFunction synchronouslyUpdateUIPropsFunction;

    std::vector<std::tuple<std::string&, uint32_t, std::unique_ptr<jsi::Value>>>
            operationsInBatch_;

    std::shared_ptr<PropsRegistry> propsRegistry_;

    std::vector<uint32_t> tagsToRemove_; // from `propsRegistry_`

    std::unordered_set<std::string> nativePropNames_; // filled by configureProps

    KeyboardEventSubscribeFunction subscribeForKeyboardEventsFunction;
    KeyboardEventUnsubscribeFunction unsubscribeFromKeyboardEventsFunction;

#ifdef DEBUG
    std::shared_ptr<JSLogger> jsLogger_;
#endif


};

} // namespace reanimated
