
LOCAL_PATH:=$(call my-dir)

# --------------- begin of libglog.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libglog

# macro defs
MACRO_DEFS:=
MACRO_DEFS+= -DHAVE_PREAD=1
# eng version enable debug flag
ifneq ($(filter $(XMAKE_BUILD_TYPE), eng userdebug),)
    MACRO_DEFS += -DDEBUG
endif

INCLUDES:= \
    $(LOCAL_PATH)/glog-0.3.5/src \
    $(LOCAL_PATH)/exported \

SRCS:= \
    $(LOCAL_PATH)/glog-0.3.5/src/demangle.cc \
    $(LOCAL_PATH)/glog-0.3.5/src/logging.cc \
    $(LOCAL_PATH)/glog-0.3.5/src/raw_logging.cc \
    $(LOCAL_PATH)/glog-0.3.5/src/signalhandler.cc \
    $(LOCAL_PATH)/glog-0.3.5/src/symbolize.cc \
    $(LOCAL_PATH)/glog-0.3.5/src/utilities.cc \
    $(LOCAL_PATH)/glog-0.3.5/src/vlog_is_on.cc \

# add compile options
# -----begin compiler defs
LOCAL_CXXFLAGS:= -std=c++17 -fvisibility-inlines-hidden -fno-exceptions
LOCAL_CXXFLAGS+= -Werror=return-type -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare -g -O2 $(MACRO_DEFS)
# ------end compiler defs

# source fies
LOCAL_SRC_FILES:= \
    $(SRCS) \

# includes paths
LOCAL_C_INCLUDES:= \
    $(INCLUDES) \

# shared libs
LOCAL_SHARED_LIBRARIES:= \

LOCAL_LDFLAGS:= \

include $(BUILD_STATIC_LIBRARY) # build a static lib
# --------- end of libglog.a -------------#