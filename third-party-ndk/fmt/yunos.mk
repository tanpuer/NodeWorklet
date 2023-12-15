
LOCAL_PATH:=$(call my-dir)

# --------------- begin of libfmt.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libfmt

# macro defs
MACRO_DEFS:=
MACRO_DEFS+= -DHAVE_PREAD=1
# eng version enable debug flag
ifneq ($(filter $(XMAKE_BUILD_TYPE), eng userdebug),)
    MACRO_DEFS += -DDEBUG
endif

INCLUDES:= \
    $(LOCAL_PATH) \
    $(LOCAL_PATH)/include \

SRCS:= \
    $(LOCAL_PATH)/src/format.cc

# add compile options
# -----begin compiler defs
LOCAL_CXXFLAGS:= -std=c++11 -fexceptions
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
# --------- end of libfmt.a -------------#