LOCAL_PATH:=$(call my-dir)
REACT_ROOT_PATH:=${LOCAL_PATH}/../../
DOUBLE_CONVERTION_PATH:= $(REACT_ROOT_PATH)/third-party-ndk/double-conversion
FMT_PATH:= $(REACT_ROOT_PATH)/third-party-ndk/fmt/include
GLOG_PATH:= $(REACT_ROOT_PATH)/third-party-ndk/glog/exported
FOLLY_PATH:= $(REACT_ROOT_PATH)/third-party-ndk/folly
THIRD_PARTY_PATH:= $(REACT_ROOT_PATH)/third-party-ndk
LIBEVENT_PATH:= $(REACT_ROOT_PATH)/third-party-ndk/libevent/include

# --------------- begin of libv8jsi.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libv8jsi

# macro defs
MACRO_DEFS+= -DFOLLY_NO_CONFIG=1
MACRO_DEFS+= -DFOLLY_HAVE_CLOCK_GETTIME=1
# MACRO_DEFS+= -DFOLLY_USE_LIBCPP=1
MACRO_DEFS+= -DFOLLY_HAVE_RECVMMSG=1
MACRO_DEFS+= -DFOLLY_HAVE_PTHREAD=1
# eng version enable debug flag
ifneq ($(filter $(XMAKE_BUILD_TYPE), eng userdebug),)
    MACRO_DEFS += -DDEBUG
endif

INCLUDES:= \
    $(LOCAL_PATH) \
    $(GLOG_PATH) \
    $(FMT_PATH) \
    $(FOLLY_PATH) \
    $(DOUBLE_CONVERTION_PATH) \
    $(libboost-includes) \
    $(REACT_ROOT_PATH)/ReactCommon/jsi/ \

SRCS:= \
    $(REACT_ROOT_PATH)/ReactCommon/jsi/jsi/jsi.cpp \
    $(REACT_ROOT_PATH)/ReactCommon/jsi/jsi/JSIDynamic.cpp \
    $(REACT_ROOT_PATH)/ReactCommon/jsi/jsi/jsilib-posix.cpp \

# add compile options
# -----begin compiler defs
LOCAL_CXXFLAGS:= -std=c++17 -fvisibility-inlines-hidden -frtti -fexceptions -DLOG_TAG=\"ReactNative\" -Wno-unused-lambda-capture
LOCAL_CXXFLAGS+= -Werror=return-type -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare -g -O2
LOCAL_CXXFLAGS+= -DNDEBUG  -D__AGIL_YUNOS__ $(MACRO_DEFS)
# ------end compiler defs

# source fies
LOCAL_SRC_FILES:= \
    $(SRCS) \

# includes paths
LOCAL_C_INCLUDES:= \
    $(INCLUDES) \

# shared libs
LOCAL_SHARED_LIBRARIES:= \

LOCAL_STATIC_LIBRARIES:= libglog libdouble_conversion libfolly_runtime \

LOCAL_LDFLAGS:= \

include $(BUILD_STATIC_LIBRARY) # build a static lib
# --------- end of libv8jsi.a -------------#

# --------------- begin of libv8runtime_node.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libv8runtime_node

# macro defs
MACRO_DEFS:=
# macro defs
MACRO_DEFS+= -DFOLLY_NO_CONFIG=1
MACRO_DEFS+= -DFOLLY_HAVE_CLOCK_GETTIME=1
# MACRO_DEFS+= -DFOLLY_USE_LIBCPP=1
MACRO_DEFS+= -DFOLLY_HAVE_RECVMMSG=1
MACRO_DEFS+= -DFOLLY_HAVE_PTHREAD=1
MACRO_DEFS+= -DREACT_NATIVE_TARGET_VERSION=70
MACRO_DEFS+= -DRN_ON_MAIN_THREAD=1
# eng version enable debug flag
ifneq ($(filter $(XMAKE_BUILD_TYPE), eng userdebug),)
    MACRO_DEFS += -DDEBUG
endif
ifneq (on, $(strip $(XMAKE_PROD_MODE)))
    MACRO_DEFS += -DNS_DBG_MODE
endif


V8_INCLUDE_DIR:= ${REACT_ROOT_PATH}/third-party-ndk/v8/libs/v8
INCLUDES:= \
    $(LOCAL_PATH) \
    $(FOLLY_PATH) \
    $(GLOG_PATH) \
    $(FMT_PATH) \
    $(DOUBLE_CONVERTION_PATH) \
    $(V8_INCLUDE_DIR) \
    $(REACT_ROOT_PATH)/ReactCommon/jsi \
    $(REACT_ROOT_PATH)/ReactCommon/ \
    $(node-includes)

SRCS:= \
    $(REACT_ROOT_PATH)/ReactHeron/v8jsi/HostProxy.cpp \
    $(REACT_ROOT_PATH)/ReactHeron/v8jsi/JSIV8ValueConverter.cpp \
    $(REACT_ROOT_PATH)/ReactHeron/v8jsi/V8Inspector.cpp \
    $(REACT_ROOT_PATH)/ReactHeron/v8jsi/V8PointerValue.cpp \
    $(REACT_ROOT_PATH)/ReactHeron/v8jsi/V8Runtime.cpp \
    $(REACT_ROOT_PATH)/ReactHeron/v8jsi/V8RuntimeFactory.cpp \
    ${REACT_ROOT_PATH}/ReactCommon/jsinspector/InspectorInterfaces.cpp \

# add compile options
# -----begin compiler defs
LOCAL_CXXFLAGS:= -std=c++17 -fvisibility-inlines-hidden -fexceptions -DLOG_TAG=\"ReactNative\" -Wno-unused-lambda-capture -fpermissive
LOCAL_CXXFLAGS+= -Werror=return-type -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare -g -O2
LOCAL_CXXFLAGS+= -DNDEBUG -D__AGIL_YUNOS__ $(MACRO_DEFS)

V8RUNTIME_BASE_CXXFLAGS:= $(LOCAL_CXXFLAGS)

LOCAL_CXXFLAGS+= -D__NODEJS_MODE__
# ------end compiler defs

# source fies
LOCAL_SRC_FILES:= \
    $(SRCS) \

# includes paths
LOCAL_C_INCLUDES:= \
    $(INCLUDES) \
    $(libboost-includes) \

# shared libs
LOCAL_SHARED_LIBRARIES:= libv8 \

LOCAL_STATIC_LIBRARIES:= libfolly_runtime libfolly_futures libfmt libglog libdouble_conversion  \

LOCAL_LDFLAGS:= \


V8RUNTIME_SRCS:= $(LOCAL_SRC_FILES)
V8RUNTIME_INCLUDES:= $(LOCAL_C_INCLUDES)

include $(BUILD_STATIC_LIBRARY) # build a static lib
# --------- end of libv8runtime.a -------------#


# --------------- begin of libv8runtime.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libv8runtime

LOCAL_CXXFLAGS:= \
    $(V8RUNTIME_BASE_CXXFLAGS) \

LOCAL_SRC_FILES:= \
    $(V8RUNTIME_SRCS) \

LOCAL_C_INCLUDES:= \
    $(V8RUNTIME_INCLUDES) \

# shared libs
LOCAL_SHARED_LIBRARIES:= libv8 \

LOCAL_STATIC_LIBRARIES:= libfolly_runtime libfolly_futures libfmt libglog libdouble_conversion  \

include $(BUILD_STATIC_LIBRARY) # build a static lib