# Copyright (c) Meta Platforms, Inc. and affiliates.
#
# This source code is licensed under the MIT license found in the
# LICENSE file in the root directory of this source tree.

##################
###    jsi     ###
##################
set(AGIL_REACT_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/../../)

cmake_minimum_required(VERSION 3.13)
set(CMAKE_VERBOSE_MAKEFILE on)
link_directories("/usr/local/lib")
add_compile_options(
        -fexceptions
        -frtti
        -O3
        -Wno-unused-lambda-capture
        -DLOG_TAG=\"ReactNative\")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

file(GLOB jsi_SRC CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/../jsi/jsi/*.cpp)
add_library(jsi STATIC ${jsi_SRC})
link_directories("/usr/local/lib")
target_include_directories(jsi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../jsi/ /usr/local/include)

target_link_libraries(jsi
        folly_runtime
        double-conversion
        glog)

##################
### jscruntime ###
##################

add_definitions(-DREACT_NATIVE_TARGET_VERSION=70)
file(GLOB SOURCES_V8RUNTIME  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_library(jscruntime STATIC
        ${SOURCES_V8RUNTIME}
        )
if(AGILREACT_BUILD_NODEJS)
add_definitions(-DNODE_JS_VERSION=16)
add_definitions(-DRN_ON_MAIN_THREAD=1)
set(V8_INCLUDE_DIR "${AGIL_REACT_ROOT}/third-party-ndk/v8/libs/v8_node_v16.20.0")
else()
set(V8_INCLUDE_DIR "${AGIL_REACT_ROOT}/third-party-ndk/v8/libs/v8")
endif()
set(REACT_INCLUDE_DIR "${AGIL_REACT_ROOT}/ReactCommon/")
target_include_directories(jscruntime PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../jsi ${V8_INCLUDE_DIR} ${REACT_INCLUDE_DIR})
link_directories("/usr/local/lib")
if(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "arm64")
        target_link_directories(jscruntime PUBLIC "${AGIL_REACT_ROOT}/third-party-ndk/v8/libs/mac/arm64")
elseif(CMAKE_HOST_SYSTEM_PROCESSOR MATCHES "x86_64")
        target_link_directories(jscruntime PUBLIC "${AGIL_REACT_ROOT}/third-party-ndk/v8/libs/mac/x86")
endif()
target_link_libraries(jscruntime folly_runtime v8_libbase v8_libplatform v8_monolith glog)

# TODO: Remove this flag when ready.
# Android has this enabled by default, but the flag is still needed for iOS.
target_compile_options(jscruntime PRIVATE -DRN_FABRIC_ENABLED)
