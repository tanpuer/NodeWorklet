
LOCAL_PATH:=$(call my-dir)

# --------------- begin of libevent.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libevent

# macro defs
MACRO_DEFS:=

# eng version enable debug flag
ifneq ($(filter $(XMAKE_BUILD_TYPE), eng userdebug),)
    MACRO_DEFS += -DDEBUG
endif

INCLUDES:= \
    $(LOCAL_PATH)/include \

SRCS:= \
    $(LOCAL_PATH)/event.c \
    $(LOCAL_PATH)/buffer.c \
    $(LOCAL_PATH)/bufferevent.c \
    $(LOCAL_PATH)/bufferevent_filter.c \
    $(LOCAL_PATH)/bufferevent_pair.c \
    $(LOCAL_PATH)/bufferevent_ratelim.c \
    $(LOCAL_PATH)/bufferevent_sock.c \
    $(LOCAL_PATH)/epoll.c \
    $(LOCAL_PATH)/evmap.c \
    $(LOCAL_PATH)/evthread.c \
    $(LOCAL_PATH)/evthread_pthread.c \
    $(LOCAL_PATH)/evutil.c \
    $(LOCAL_PATH)/evutil_rand.c \
    $(LOCAL_PATH)/evutil_time.c \
    $(LOCAL_PATH)/listener.c \
    $(LOCAL_PATH)/log.c \
    $(LOCAL_PATH)/poll.c \
    $(LOCAL_PATH)/signal.c \
    $(LOCAL_PATH)/strlcpy.c \
    $(LOCAL_PATH)/select.c \

# add compile options
# -----begin compiler defs
LOCAL_CFLAGS:= \
    -std=c11 -Wno-unused-function -Wno-unneeded-internal-declaration -O2 \
    -DHAVE_CONFIG_H \
    -DHAVE_SYS_TIME_H \
    $(MACRO_DEFS) \
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
    -lrt

include $(BUILD_STATIC_LIBRARY) # build a static lib
# --------- end of libevent.a -------------#