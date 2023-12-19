cmake_minimum_required(VERSION 3.21.2)

message(STATUS "************ node-worklet ${CMAKE_BUILD_TYPE} Build ************")
project(NodeWorklet)

set(NODEWORKLET_LIBRARY 1)
set(NODEWORKLET_LIBRARY_NAME nodeworklet)
set(NODEWORKLET_BUILD_SHARED 1)
add_definitions(-DNODEWORKLET_LIBRARY=1)
add_definitions(-DNODEWORKLET_LIBRARY_NAME=nodeworklet)
add_definitions(-DRN_ON_MAIN_THREAD=1)
add_definitions(-DRCT_NEW_ARCH_ENABLED=1)
add_definitions(-DJS_RUNTIME_V8=1)

set(WORKLET_INCLUDES
        /usr/local/include
        third-party-ndk/glog
        third-party-ndk/double-conversion
        third-party-ndk/fmt
        third-party-ndk/folly
        third-party-ndk/jsi
        third-party-ndk/v8jsi
        third-party-ndk/v8/libs/v8_node_v16.20.0
        third-party-ndk
)

message(STATUS "####make sure you have installed boost libevent sdl2 with brew####")
message(STATUS "#### build node worklet with macos : ${CMAKE_HOST_SYSTEM_PROCESSOR} ####")
set(AGIL_MACOS ON)
set(AGIL_NS_DEBUG ON)
add_subdirectory(third-party-ndk/glog)
add_subdirectory(third-party-ndk/double-conversion)
add_subdirectory(third-party-ndk/fmt)
add_subdirectory(third-party-ndk/folly)
#add_subdirectory(third-party-ndk/jsi)
add_subdirectory(third-party-ndk/v8jsi)
if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
    link_directories("/opt/homebrew/lib")
    link_directories("/opt/homebrew/opt/openssl@1.1/lib")
    link_directories("/opt/homebrew/opt/curl/lib")
    link_directories("${AGIL_PLATFORM_DIR}/third_party/angle/lib/mac/arm64/")
    link_directories("/usr/local/lib")
    link_directories("/opt/homebrew/opt/jpeg-turbo/lib")
    link_directories("/opt/homebrew/opt/icu4c/lib")
elseif (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    link_directories("/usr/local/lib")
    link_directories("/usr/local/opt/openssl@1.1/lib")
    link_directories("/usr/local/opt/curl/lib")
    link_directories("${AGIL_PLATFORM_DIR}/third_party/angle/lib/mac/x86/")
    link_directories("/usr/local/opt/jpeg-turbo/lib")
    link_directories("/usr/local/opt/icu4c/lib")
endif ()
set(WORKLET_INCLUDES
        ${WORKLET_INCLUDES}
        /usr/local/include
        include/node/
        worklet/reanimated
)

set(WORKLET_SOURCES
        worklet/reanimated/Fabric/PropsRegistry.cpp
        worklet/reanimated/NativeModules/NativeReanimatedModule.cpp
        worklet/reanimated/ReanimatedRuntime/ReanimatedRuntime.cpp
        worklet/reanimated/Registries/EventHandlerRegistry.cpp
        worklet/reanimated/SharedItems/Shareables.cpp
        worklet/reanimated/Tools/JSISerializer.cpp
        worklet/reanimated/Tools/JSLogger.cpp
        worklet/reanimated/Tools/JSScheduler.cpp
        worklet/reanimated/Tools/ReanimatedVersion.cpp
        worklet/reanimated/Tools/RuntimeDecorator.cpp
        worklet/reanimated/Tools/UIScheduler.cpp
        worklet/reanimated/Tools/WorkletEventHandler.cpp
        worklet/reanimated/AgilUIScheduler.cpp
        worklet/reanimated/CallInvoker.cpp
        worklet/reanimated/ReanimatedModule.cpp
        worklet/reanimated/ReanimatedNodejs.cpp
)

if (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fvisibility-inlines-hidden -fstrict-aliasing -fPIC -fvisibility=default -pthread -Wall -Wextra -Wno-sign-compare -Wno-mismatched-tags -Wno-inconsistent-missing-override -Wno-pessimizing-move -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-const-variable -Wno-unused-private-field -Wno-missing-braces -Wno-unused-lambda-capture -Wno-overloaded-virtual -fno-unwind-tables")
elseif (CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fvisibility-inlines-hidden -fstrict-aliasing -fPIC -fvisibility=default -pthread -Wall -Wextra -Wno-sign-compare -Wno-mismatched-tags -Wno-inconsistent-missing-override -Wno-pessimizing-move -Wno-unused-command-line-argument -Wno-unused-parameter -Wno-unused-const-variable -Wno-unused-private-field -Wno-missing-braces -Wno-unused-lambda-capture -Wno-overloaded-virtual -fno-unwind-tables")
endif ()


if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(WORKLET_DEFS -DNDEBUG)
    message(STATUS "************CMake Release Build************")
else ()
    set(WORKLET_DEFS -DDEBUG -DRN_DEBUG_STRING_CONVERTIBLE)
    message(STATUS "************CMake ${CMAKE_BUILD_TYPE} Build************")
endif ()

message(STATUS "************ NodeWorklet Compiler C++ Flags ************")
message(STATUS ${CMAKE_CXX_FLAGS})

#########nodeworklet lib#########
set(WORKLET_LIB_NAME nodeworklet)
add_library(${WORKLET_LIB_NAME} STATIC ${WORKLET_SOURCES})
target_include_directories(${WORKLET_LIB_NAME} PUBLIC ${WORKLET_INCLUDES})
set(agildefs -D__AGIL_MACOS__)
target_link_libraries(${WORKLET_LIB_NAME} jsi event)

target_compile_definitions(${WORKLET_LIB_NAME} PUBLIC ${agildefs} ${WORKLET_DEFS})