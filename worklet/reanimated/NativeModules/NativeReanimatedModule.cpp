#include "NativeReanimatedModule.h"

#include <functional>
#include <memory>
#include <thread>
#include <unordered_map>

#include "../Fabric/PropsRegistry.h"

#include "../Registries/EventHandlerRegistry.h"
#include "../Tools/ReanimatedHiddenHeaders.h"
#include "../Tools/RuntimeDecorator.h"
#include "../SharedItems/Shareables.h"
#include "../Tools/WorkletEventHandler.h"

#ifdef DEBUG

#include "../Tools/JSLogger.h"

#endif

namespace reanimated {

NativeReanimatedModule::NativeReanimatedModule(
        const std::shared_ptr<CallInvoker> &jsInvoker,
        const std::shared_ptr<UIScheduler> &uiScheduler,
        const std::shared_ptr<jsi::Runtime> &rt,
        PlatformDepMethodsHolder platformDepMethodsHolder)
        : runtimeManager_(std::make_shared<RuntimeManager>(
        rt,
        uiScheduler,
        std::make_shared<JSScheduler>(jsInvoker),
        RuntimeType::UI)),
          eventHandlerRegistry(std::make_unique<EventHandlerRegistry>()),
          requestRender(platformDepMethodsHolder.requestRender),
          synchronouslyUpdateUIPropsFunction(
                  platformDepMethodsHolder.synchronouslyUpdateUIPropsFunction) {
    auto requestAnimationFrame = [=](jsi::Runtime &rt, const jsi::Value &fn) {
        auto jsFunction = std::make_shared<jsi::Value>(rt, fn);
        frameCallbacks.push_back([=](double timestamp) {
            runtimeHelper->runOnUIGuarded(*jsFunction, jsi::Value(timestamp));
        });
        maybeRequestRender();
    };

    auto scheduleOnJS = [this](
            jsi::Runtime &rt,
            const jsi::Value &remoteFun,
            const jsi::Value &argsValue) {
        this->scheduleOnJS(rt, remoteFun, argsValue);
    };

    auto makeShareableClone = [this](jsi::Runtime &rt, const jsi::Value &value) {
        return this->makeShareableClone(rt, value, jsi::Value::undefined());
    };

    auto updateDataSynchronously =
            [this](
                    jsi::Runtime &rt,
                    const jsi::Value &synchronizedDataHolderRef,
                    const jsi::Value &newData) {
                return this->updateDataSynchronously(
                        rt, synchronizedDataHolderRef, newData);
            };

    auto updateProps = [this](jsi::Runtime &rt, const jsi::Value &operations) {
        this->updateProps(rt, operations);
    };

    auto removeFromPropsRegistry =
            [this](jsi::Runtime &rt, const jsi::Value &viewTags) {
                this->removeFromPropsRegistry(rt, viewTags);
            };

    auto measure = [this](jsi::Runtime &rt, const jsi::Value &shadowNodeValue) {
        return this->measure(rt, shadowNodeValue);
    };

    auto dispatchCommand = [this](
            jsi::Runtime &rt,
            const jsi::Value &shadowNodeValue,
            const jsi::Value &commandNameValue,
            const jsi::Value &argsValue) {
        this->dispatchCommand(rt, shadowNodeValue, commandNameValue, argsValue);
    };

    RuntimeDecorator::decorateUIRuntime(
            *runtimeManager_->runtime,
            updateProps,
            removeFromPropsRegistry,
            measure,
            dispatchCommand,
            requestAnimationFrame,
            scheduleOnJS,
            makeShareableClone,
            updateDataSynchronously,
            platformDepMethodsHolder.getCurrentTime,
            platformDepMethodsHolder.setGestureStateFunction,
            platformDepMethodsHolder.progressLayoutAnimation,
            platformDepMethodsHolder.endLayoutAnimation,
            platformDepMethodsHolder.maybeFlushUIUpdatesQueueFunction);
    onRenderCallback = [this](double timestampMs) {
        this->renderRequested = false;
        this->onRender(timestampMs);
    };

    subscribeForKeyboardEventsFunction =
            platformDepMethodsHolder.subscribeForKeyboardEvents;
    unsubscribeFromKeyboardEventsFunction =
            platformDepMethodsHolder.unsubscribeFromKeyboardEvents;
}

void NativeReanimatedModule::installCoreFunctions(
        jsi::Runtime &rt,
        const jsi::Value &callGuard,
        const jsi::Value &valueUnpacker) {
    if (!runtimeHelper) {
        // initialize runtimeHelper here if not already present. We expect only one
        // instace of the helper to exists.
        runtimeHelper = std::make_shared<JSRuntimeHelper>(
                &rt,
                runtimeManager_->runtime.get(),
                runtimeManager_->uiScheduler_,
                runtimeManager_->jsScheduler_);
    }
    runtimeHelper->callGuard =
            std::make_unique<CoreFunction>(runtimeHelper.get(), callGuard);
    runtimeHelper->valueUnpacker =
            std::make_unique<CoreFunction>(runtimeHelper.get(), valueUnpacker);
#ifdef DEBUG
    // We initialize jsLogger_ here because we need runtimeHelper
    // to be initialized already
    jsLogger_ = std::make_shared<JSLogger>(runtimeHelper);
#endif
}

NativeReanimatedModule::~NativeReanimatedModule() {
    if (runtimeHelper) {
        runtimeHelper->callGuard = nullptr;
        runtimeHelper->valueUnpacker = nullptr;
        // event handler registry and frame callbacks store some JSI values from UI
        // runtime, so they have to go away before we tear down the runtime
        eventHandlerRegistry.reset();
        frameCallbacks.clear();
        runtimeManager_->runtime.reset();
        // make sure uiRuntimeDestroyed is set after the runtime is deallocated
        runtimeHelper->uiRuntimeDestroyed = true;
    }
}

void NativeReanimatedModule::scheduleOnUI(
        jsi::Runtime &rt,
        const jsi::Value &worklet) {
    auto shareableWorklet = extractShareableOrThrow<ShareableWorklet>(
            rt, worklet, "only worklets can be scheduled to run on UI");
    runtimeManager_->uiScheduler_->scheduleOnUI([=] {
        jsi::Runtime &rt = *runtimeHelper->uiRuntime();
        auto workletValue = shareableWorklet->getJSValue(rt);
        runtimeHelper->runOnUIGuarded(workletValue);
    });
}

void NativeReanimatedModule::scheduleOnJS(
        jsi::Runtime &rt,
        const jsi::Value &remoteFun,
        const jsi::Value &argsValue) {
    auto shareableRemoteFun = extractShareableOrThrow<ShareableRemoteFunction>(
            rt,
            remoteFun,
            "Incompatible object passed to scheduleOnJS. It is only allowed to schedule worklets or functions defined on the React Native JS runtime this way.");
    auto shareableArgs = argsValue.isUndefined()
                         ? nullptr
                         : extractShareableOrThrow<ShareableArray>(
                    rt, argsValue, "args must be an array");
    auto jsRuntime = this->runtimeHelper->rnRuntime();
    runtimeManager_->jsScheduler_->scheduleOnJS([=] {
        jsi::Runtime &rt = *jsRuntime;
        auto remoteFun = shareableRemoteFun->getJSValue(rt);
        if (shareableArgs == nullptr) {
            // fast path for remote function w/o arguments
            remoteFun.asObject(rt).asFunction(rt).call(rt);
        } else {
            auto argsArray = shareableArgs->getJSValue(rt).asObject(rt).asArray(rt);
            auto argsSize = argsArray.size(rt);
            // number of arguments is typically relatively small so it is ok to
            // to use VLAs here, hence disabling the lint rule
            jsi::Value args[argsSize]; // NOLINT(runtime/arrays)
            for (size_t i = 0; i < argsSize; i++) {
                args[i] = argsArray.getValueAtIndex(rt, i);
            }
            remoteFun.asObject(rt).asFunction(rt).call(rt, args, argsSize);
        }
    });
}

jsi::Value NativeReanimatedModule::makeSynchronizedDataHolder(
        jsi::Runtime &rt,
        const jsi::Value &initialShareable) {
    auto dataHolder = std::make_shared<ShareableSynchronizedDataHolder>(
            runtimeHelper, rt, initialShareable);
    return dataHolder->getJSValue(rt);
}

void NativeReanimatedModule::updateDataSynchronously(
        jsi::Runtime &rt,
        const jsi::Value &synchronizedDataHolderRef,
        const jsi::Value &newData) {
    auto dataHolder = extractShareableOrThrow<ShareableSynchronizedDataHolder>(
            rt, synchronizedDataHolderRef);
    dataHolder->set(rt, newData);
}

jsi::Value NativeReanimatedModule::getDataSynchronously(
        jsi::Runtime &rt,
        const jsi::Value &synchronizedDataHolderRef) {
    auto dataHolder = extractShareableOrThrow<ShareableSynchronizedDataHolder>(
            rt, synchronizedDataHolderRef);
    return dataHolder->get(rt);
}

jsi::Value NativeReanimatedModule::makeShareableClone(
        jsi::Runtime &rt,
        const jsi::Value &value,
        const jsi::Value &shouldRetainRemote) {
    std::shared_ptr<Shareable> shareable;
    if (value.isObject()) {
        auto object = value.asObject(rt);
        if (!object.getProperty(rt, "__workletHash").isUndefined()) {
            shareable = std::make_shared<ShareableWorklet>(runtimeHelper, rt, object);
        } else if (!object.getProperty(rt, "__init").isUndefined()) {
            shareable = std::make_shared<ShareableHandle>(runtimeHelper, rt, object);
        } else if (object.isFunction(rt)) {
            auto function = object.asFunction(rt);
            if (function.isHostFunction(rt)) {
                shareable =
                        std::make_shared<ShareableHostFunction>(rt, std::move(function));
            } else {
                shareable = std::make_shared<ShareableRemoteFunction>(
                        runtimeHelper, rt, std::move(function));
            }
        } else if (object.isArray(rt)) {
            if (shouldRetainRemote.isBool() && shouldRetainRemote.getBool()) {
                shareable = std::make_shared<RetainingShareable<ShareableArray>>(
                        runtimeHelper, rt, object.asArray(rt));
            } else {
                shareable = std::make_shared<ShareableArray>(rt, object.asArray(rt));
            }
        } else if (object.isHostObject(rt)) {
            shareable = std::make_shared<ShareableHostObject>(
                    runtimeHelper, rt, object.getHostObject(rt));
        } else {
            if (shouldRetainRemote.isBool() && shouldRetainRemote.getBool()) {
                shareable = std::make_shared<RetainingShareable<ShareableObject>>(
                        runtimeHelper, rt, object);
            } else {
                shareable = std::make_shared<ShareableObject>(rt, object);
            }
        }
    } else if (value.isString()) {
        shareable = std::make_shared<ShareableString>(value.asString(rt).utf8(rt));
    } else if (value.isUndefined()) {
        shareable = std::make_shared<ShareableScalar>();
    } else if (value.isNull()) {
        shareable = std::make_shared<ShareableScalar>(nullptr);
    } else if (value.isBool()) {
        shareable = std::make_shared<ShareableScalar>(value.getBool());
    } else if (value.isNumber()) {
        shareable = std::make_shared<ShareableScalar>(value.getNumber());
    } else if (value.isSymbol()) {
        // TODO: this is only a placeholder implementation, here we replace symbols
        // with strings in order to make certain objects to be captured. There isn't
        // yet any usecase for using symbols on the UI runtime so it is fine to keep
        // it like this for now.
        shareable =
                std::make_shared<ShareableString>(value.getSymbol(rt).toString(rt));
    } else {
        throw std::runtime_error("attempted to convert an unsupported value type");
    }
    return ShareableJSRef::newHostObject(rt, shareable);
}

jsi::Value NativeReanimatedModule::registerEventHandler(
        jsi::Runtime &rt,
        const jsi::Value &eventHash,
        const jsi::Value &worklet) {
    static uint64_t EVENT_HANDLER_ID = 1;

    uint64_t newRegistrationId = EVENT_HANDLER_ID++;
    auto eventName = eventHash.asString(rt).utf8(rt);
    auto handlerShareable = extractShareableOrThrow(rt, worklet);

    runtimeManager_->uiScheduler_->scheduleOnUI([=] {
        jsi::Runtime &rt = *runtimeHelper->uiRuntime();
        auto handlerFunction = handlerShareable->getJSValue(rt);
        auto handler = std::make_shared<WorkletEventHandler>(
                runtimeHelper,
                newRegistrationId,
                eventName,
                std::move(handlerFunction));
        eventHandlerRegistry->registerEventHandler(std::move(handler));
    });

    return jsi::Value(static_cast<double>(newRegistrationId));
}

void NativeReanimatedModule::unregisterEventHandler(
        jsi::Runtime &,
        const jsi::Value &registrationId) {
    uint64_t id = registrationId.asNumber();
    runtimeManager_->uiScheduler_->scheduleOnUI(
            [=] { eventHandlerRegistry->unregisterEventHandler(id); });
}

jsi::Value NativeReanimatedModule::getViewProp(
        jsi::Runtime &rt,
        const jsi::Value &viewTag,
        const jsi::Value &propName,
        const jsi::Value &callback) {
    const int viewTagInt = static_cast<int>(viewTag.asNumber());
    std::string propNameStr = propName.asString(rt).utf8(rt);
    jsi::Function fun = callback.getObject(rt).asFunction(rt);
    std::shared_ptr<jsi::Function> funPtr =
            std::make_shared<jsi::Function>(std::move(fun));

    runtimeManager_->uiScheduler_->scheduleOnUI(
            [&rt, viewTagInt, funPtr, this, propNameStr]() {
                const jsi::String propNameValue =
                        jsi::String::createFromUtf8(rt, propNameStr);
                jsi::Value result = propObtainer(rt, viewTagInt, propNameValue);
                std::string resultStr = result.asString(rt).utf8(rt);

                runtimeManager_->jsScheduler_->scheduleOnJS([&rt, resultStr, funPtr]() {
                    const jsi::String resultValue =
                            jsi::String::createFromUtf8(rt, resultStr);
                    funPtr->call(rt, resultValue);
                });
            });

    return jsi::Value::undefined();
}

jsi::Value NativeReanimatedModule::enableLayoutAnimations(
        jsi::Runtime &,
        const jsi::Value &config) {
    return jsi::Value::undefined();
}

jsi::Value NativeReanimatedModule::configureProps(
        jsi::Runtime &rt,
        const jsi::Value &uiProps,
        const jsi::Value &nativeProps) {
    (void) uiProps; // unused variable on Fabric
    jsi::Array array = nativeProps.asObject(rt).asArray(rt);
    for (size_t i = 0; i < array.size(rt); ++i) {
        std::string name = array.getValueAtIndex(rt, i).asString(rt).utf8(rt);
        nativePropNames_.insert(name);
    }

    return jsi::Value::undefined();
}

jsi::Value NativeReanimatedModule::configureLayoutAnimation(
        jsi::Runtime &rt,
        const jsi::Value &viewTag,
        const jsi::Value &type,
        const jsi::Value &sharedTransitionTag,
        const jsi::Value &config) {
    return jsi::Value::undefined();
}

bool NativeReanimatedModule::isAnyHandlerWaitingForEvent(
        const std::string &eventName,
        const int emitterReactTag) {
    return eventHandlerRegistry->isAnyHandlerWaitingForEvent(
            eventName);
}

bool NativeReanimatedModule::isHandlerEmpty() {
    return eventHandlerRegistry->isHandlerEmpty();
}

void NativeReanimatedModule::maybeRequestRender() {
    if (!renderRequested) {
        renderRequested = true;
        requestRender(onRenderCallback, *runtimeManager_->runtime);
    }
}

void NativeReanimatedModule::onRender(double timestampMs) {
    std::vector<FrameCallback> callbacks = frameCallbacks;
    frameCallbacks.clear();
    for (auto &callback: callbacks) {
        callback(timestampMs);
    }
}

bool NativeReanimatedModule::isThereAnyLayoutProp(
        jsi::Runtime &rt,
        const jsi::Object &props) {
    const jsi::Array propNames = props.getPropertyNames(rt);
    for (size_t i = 0; i < propNames.size(rt); ++i) {
        const std::string propName =
                propNames.getValueAtIndex(rt, i).asString(rt).utf8(rt);
        bool isLayoutProp =
                nativePropNames_.find(propName) != nativePropNames_.end();
        if (isLayoutProp) {
            return true;
        }
    }
    return false;
}

bool NativeReanimatedModule::handleEvent(
        const std::string &eventName,
        const int tag,
        const jsi::Value &payload,
        double currentTime) {
    eventHandlerRegistry->processEvent(
            *runtimeManager_->runtime,
            currentTime,
            eventName,
            tag,
            payload);

    // TODO: return true if Reanimated successfully handled the event
    // to avoid sending it to JavaScript
    return false;
}

bool NativeReanimatedModule::handleRawEvent(
        uint32_t ref, const std::string &name, const std::string &paramsAsJson, double currentTime) {
    //Todo Caf3 根据heron穿过的touch/scroll事件进行处理
//    auto res = handleEvent(name, ref, std::move(payload), currentTime);
    // TODO: we should call performOperations conditionally if event is handled
    // (res == true), but for now handleEvent always returns false. Thankfully,
    // performOperations does not trigger a lot of code if there is nothing to be
    // done so this is fine for now.
    performOperations();
    return true;
}

void NativeReanimatedModule::updateProps(
        jsi::Runtime &rt,
        const jsi::Value &operations) {
    auto array = operations.asObject(rt).asArray(rt);
    size_t length = array.size(rt);
    for (size_t i = 0; i < length; ++i) {
        auto item = array.getValueAtIndex(rt, i).asObject(rt);
        auto shadowNodeWrapper = item.getProperty(rt, "shadowNodeWrapper");
        //Todo Caf3
        auto shadowNode = item.getProperty(rt, "node").asNumber();
        const jsi::Value &updates = item.getProperty(rt, "updates");
        operationsInBatch_.emplace_back(shadowNode, std::make_unique<jsi::Value>(rt, updates));

    }
}

void NativeReanimatedModule::performOperations() {
    if (operationsInBatch_.empty() && tagsToRemove_.empty()) {
        // nothing to do
        return;
    }

    auto copiedOperationsQueue = std::move(operationsInBatch_);
    operationsInBatch_ = std::vector<std::pair<uint32_t, std::unique_ptr<jsi::Value>>>();

    jsi::Runtime &rt = *runtimeManager_->runtime;

    bool hasLayoutUpdates = false;
    for (const auto &[shadowNode, props]: copiedOperationsQueue) {
        if (isThereAnyLayoutProp(rt, props->asObject(rt))) {
            hasLayoutUpdates = true;
            break;
        }
    }

    //Todo Caf3
//    if (!hasLayoutUpdates) {
//        // If there's no layout props to be updated, we can apply the updates
//        // directly onto the components and skip the commit.
//        for (const auto &[shadowNode, props]: copiedOperationsQueue) {
//            Tag tag = shadowNode->getTag();
//            synchronouslyUpdateUIPropsFunction(rt, tag, props->asObject(rt));
//            auto rawProps = new RawProps(rt, *props);
//            auto clonedNode = uiManager_->cloneNode(*shadowNode, nullptr, rawProps);
//            delete rawProps;
//        }
//        if (copiedOperationsQueue.empty() || surfaceId_ < 0) {
//            return;
//        }
//        uiManager_->getDelegate()->uiManagerCompleteSurface(surfaceId_, std::make_shared<ShadowNode::ListOfShared>());
//        return;
//    }
//
//    for (const auto &[shadowNode, props]: copiedOperationsQueue) {
//        auto rawProps = new RawProps(rt, *props);
//        auto clonedNode = uiManager_->cloneNode(*shadowNode, nullptr, rawProps);
//        delete rawProps;
//    }
//    if (copiedOperationsQueue.empty() || surfaceId_ < 0) {
//        return;
//    }
//    uiManager_->getDelegate()->uiManagerCompleteSurface(surfaceId_, std::make_shared<ShadowNode::ListOfShared>());
}

void NativeReanimatedModule::removeFromPropsRegistry(
        jsi::Runtime &rt,
        const jsi::Value &viewTags) {
    auto array = viewTags.asObject(rt).asArray(rt);
    for (size_t i = 0, size = array.size(rt); i < size; ++i) {
        tagsToRemove_.push_back(array.getValueAtIndex(rt, i).asNumber());
    }
}

void NativeReanimatedModule::dispatchCommand(
        jsi::Runtime &rt,
        const jsi::Value &shadowNodeValue,
        const jsi::Value &commandNameValue,
        const jsi::Value &argsValue) {
    //Todo Caf3
}

jsi::Value NativeReanimatedModule::measure(
        jsi::Runtime &rt,
        const jsi::Value &shadowNodeValue) {
    //Todo Caf3
//  auto shadowNode = shadowNodeFromValue(rt, shadowNodeValue);
//  auto pageId = mApp->getPageId(shadowNode->getSurfaceId());
//  auto page = heron::Engine::sharedInstance()->getPage(pageId);
//  auto component = std::static_pointer_cast<heron::UIPage>(page)->getComponentByRef(std::to_string(shadowNode->getTag()));
//  auto layout = component->getViewLayoutRect();
//  auto size = component->getViewLayoutSize();
//  auto origin = component->getViewLayoutRectAbsolute();
//  jsi::Object result(rt);
//  result.setProperty(
//          rt, "x", jsi::Value(static_cast<double>(layout.left)));
//  result.setProperty(
//          rt, "y", jsi::Value(static_cast<double>(layout.top)));
//  result.setProperty(
//          rt, "width", jsi::Value(static_cast<double>(size.width)));
//  result.setProperty(
//          rt, "height", jsi::Value(static_cast<double>(size.height)));
//  result.setProperty(
//          rt, "pageX", jsi::Value(static_cast<double>(origin.left)));
//  result.setProperty(
//          rt, "pageY", jsi::Value(static_cast<double>(origin.top)));
//  return result;

}

void NativeReanimatedModule::setPropsRegistry(
        std::shared_ptr<PropsRegistry> propsRegistry) {
    propsRegistry_ = propsRegistry;
}

jsi::Value NativeReanimatedModule::get(jsi::Runtime &runtime, const jsi::PropNameID &name) {
    auto methodName = name.utf8(runtime);
    if (methodName == "installCoreFunctions") {
        return jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "installCoreFunctions"),
                2,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    this->installCoreFunctions(rt, std::move(args[0]), std::move(args[1]));
                    return jsi::Value::undefined();
                }));
    } else if (methodName == "makeShareableClone") {
        return jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "makeShareableClone"),
                2,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    return this->makeShareableClone(rt, std::move(args[0]), std::move(args[1]));
                }));
    } else if (methodName == "scheduleOnUI") {
        return jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "scheduleOnUI"),
                1,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    this->scheduleOnUI(rt, std::move(args[0]));
                    return jsi::Value::undefined();
                }));
    } else if (methodName == "registerEventHandler") {
        jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "registerEventHandler"),
                2,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    return this->registerEventHandler(rt, std::move(args[0]), std::move(args[1]));
                }));
    } else if (methodName == "getViewProp") {
        jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "getViewProp"),
                3,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    return this->getViewProp(rt, std::move(args[0]), std::move(args[1]), std::move(args[2]));
                }));
    } else if (methodName == "makeSynchronizedDataHolder") {
        return jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "makeSynchronizedDataHolder"),
                1,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    return this->makeSynchronizedDataHolder(rt, std::move(args[0]));
                }));
    } else if (methodName == "getDataSynchronously") {
        return jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "getDataSynchronously"),
                1,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    return this->getDataSynchronously(rt, std::move(args[0]));
                }));
    } else if (methodName == "unregisterEventHandler") {
        return jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "unregisterEventHandler"),
                1,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    this->unregisterEventHandler(rt, std::move(args[0]));
                    return jsi::Value::undefined();
                }));
    } else if (methodName == "configureProps") {
        jsi::Function::createFromHostFunction(
                runtime,
                jsi::PropNameID::forAscii(runtime, "configureProps"),
                2,
                jsi::HostFunctionType([this](jsi::Runtime &rt,
                                             const jsi::Value &thisVal,
                                             const jsi::Value *args,
                                             size_t count) -> jsi::Value {
                    return this->configureProps(rt, std::move(args[0]), std::move(args[1]));
                }));
    }
    return jsi::Value::undefined();
}

} // namespace reanimated
