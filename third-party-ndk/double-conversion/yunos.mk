
LOCAL_PATH:=$(call my-dir)

# --------------- begin of libdouble_conversion.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libdouble_conversion

# macro defs
MACRO_DEFS:=
MACRO_DEFS+= -DHAVE_PREAD=1
# eng version enable debug flag
ifneq ($(filter $(XMAKE_BUILD_TYPE), eng userdebug),)
    MACRO_DEFS += -DDEBUG
endif

INCLUDES:= \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/double-conversion \

SRCS:= \
    $(LOCAL_PATH)/double-conversion/bignum.cc \
    $(LOCAL_PATH)/double-conversion/bignum-dtoa.cc \
    $(LOCAL_PATH)/double-conversion/cached-powers.cc \
    $(LOCAL_PATH)/double-conversion/diy-fp.cc \
    $(LOCAL_PATH)/double-conversion/double-conversion.cc \
    $(LOCAL_PATH)/double-conversion/fast-dtoa.cc \
    $(LOCAL_PATH)/double-conversion/fixed-dtoa.cc \
    $(LOCAL_PATH)/double-conversion/strtod.cc \

# add compile options
# -----begin compiler defs
LOCAL_CXXFLAGS:= -std=c++17 -fvisibility-inlines-hidden
LOCAL_CXXFLAGS+= -Wno-unused-variable -Wno-unused-local-typedefs $(MACRO_DEFS)
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
# --------- end of libdouble_conversion.a -------------#