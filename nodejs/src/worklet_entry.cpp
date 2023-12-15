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
#include "ReanimatedModule.h"
#include "V8RuntimeConfig.h"
#include "CallInvoker.h"

void Init(const FunctionCallbackInfo<Value> &args) {
    rnv8::V8Runtime::nodejsV8Isolate = args.GetIsolate();
    auto config = std::make_unique<rnv8::V8RuntimeConfig>();
    auto jsRuntime = new rnv8::V8Runtime(std::move(config));
//    jsRuntime->evaluateJavaScript()
    auto callInvoker = std::make_shared<reanimated::CallInvoker>();
    auto module = new reanimated::ReanimatedModule(callInvoker, jsRuntime);
    module->installTurboModule();
    args.GetReturnValue().Set(1);
}

void Destroy(const FunctionCallbackInfo<Value> &args) {
    rnv8::V8Runtime::nodejsV8Isolate = nullptr;
}

void Initialize(Local<Object> exports) {
    NODE_SET_METHOD(exports, "init", Init);
    NODE_SET_METHOD(exports, "destroy", Destroy);
}


NODE_MODULE(NODE_GYP_MODULE_NAME, Initialize);