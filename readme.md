## worklet_addon

worklet的介绍可参考react-native-reanimated

//Todo Caf3
1. 只支持Mac平台！先安装依赖arch -arm64 brew install boost libevent。
2. 使用nvm将nodejs版本切成v16.20.0。
3. 基于nodejs的v8环境运行，在主线程运行js，起一个子线程作为UI线程。
4. 之前已经将react-native-reanimated移植到AgilReact中，采取同样的方案，在渲染线程使用同一个v8
   isolate运行部分js代码，这是实现worklet的原理。
5. 基于worklet封装一些动画/手势模块。
6. worklet_entry.cpp是入口。
7. 第一运行执行cd nodejs, sh compile.sh。
8. 以后执行sh make.sh && node index.js。
9. 第一步依赖jsi，后续剥离jsi，直接调用v8接口完成即可。