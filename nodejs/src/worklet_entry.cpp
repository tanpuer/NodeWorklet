//
// Created by banma-3412 on 2023/12/8.
//
#include <node.h>
#include <v8.h>

using v8::FunctionCallbackInfo;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

#include "memory"
#include "V8Runtime.h"
#include "ReanimatedNodejs.h"
#include "V8RuntimeConfig.h"
#include "CallInvoker.h"
#include "../ui_thread.h"

static rnv8::V8Runtime *jsRuntime = nullptr;

void Init(const FunctionCallbackInfo<Value> &args) {
    rnv8::V8Runtime::nodejsV8Isolate = args.GetIsolate();
    auto config = std::make_unique<rnv8::V8RuntimeConfig>();
    jsRuntime = new rnv8::V8Runtime(std::move(config));
    auto callInvoker = std::make_shared<reanimated::CallInvoker>();
    auto module = new reanimated::ReanimatedNodejs(callInvoker, jsRuntime);
    module->installTurboModule();

   GetHeronSDKManager();

    args.GetReturnValue().Set(1);
}

void Destroy(const FunctionCallbackInfo<Value> &args) {
    rnv8::V8Runtime::nodejsV8Isolate = nullptr;
}

void Eval(const FunctionCallbackInfo<Value> &args) {
    auto isolate = args.GetIsolate();
    v8::String::Utf8Value value(isolate, args[0]);
    auto codeStr = std::string(*value, value.length());
    v8::String::Utf8Value urlValue(isolate, args[1]);
    auto urlStr = std::string(*urlValue, urlValue.length());
    auto buffer = std::make_shared<facebook::jsi::StringBuffer>(codeStr);
    jsRuntime->evaluateJavaScript(buffer, urlStr);
}

void runUIThread(const FunctionCallbackInfo<Value> &args) {

}

void Initialize(Local<Object> exports) {
    NODE_SET_METHOD(exports, "init", Init);
    NODE_SET_METHOD(exports, "destroy", Destroy);
    NODE_SET_METHOD(exports, "eval", Eval);
    NODE_SET_METHOD(exports, "runUIThread", runUIThread);
}


NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);