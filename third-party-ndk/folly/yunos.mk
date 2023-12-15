
LOCAL_PATH:=$(call my-dir)
DOUBLE_CONVERTION_PATH:= $(LOCAL_PATH)/../double-conversion
FMT_PATH:= $(LOCAL_PATH)/../fmt/include
GLOG_PATH:= $(LOCAL_PATH)/../glog/exported
THIRD_PARTY_PATH:= $(LOCAL_PATH)/../
LIBEVENT_PATH:= $(LOCAL_PATH)/../libevent/include
# --------------- begin of libfolly_runtime.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libfolly_runtime

# macro defs
MACRO_DEFS:=
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
    $(LOCAL_PATH)/folly \
    $(THIRD_PARTY_PATH) \
    $(DOUBLE_CONVERTION_PATH) \
    $(FMT_PATH) \
    $(GLOG_PATH) \
    $(LIBEVENT_PATH) \

SRCS:= \
    $(LOCAL_PATH)/folly/SharedMutex.cpp \
    $(LOCAL_PATH)/folly/concurrency/CacheLocality.cpp \
    $(LOCAL_PATH)/folly/detail/Futex.cpp \
    $(LOCAL_PATH)/folly/lang/SafeAssert.cpp \
    $(LOCAL_PATH)/folly/lang/ToAscii.cpp \
    $(LOCAL_PATH)/folly/synchronization/ParkingLot.cpp \
    $(LOCAL_PATH)/folly/system/ThreadId.cpp \
    $(LOCAL_PATH)/folly/system/ThreadName.cpp \
    $(LOCAL_PATH)/folly/json.cpp \
    $(LOCAL_PATH)/folly/Unicode.cpp \
    $(LOCAL_PATH)/folly/Conv.cpp \
    $(LOCAL_PATH)/folly/Demangle.cpp \
    $(LOCAL_PATH)/folly/memory/detail/MallocImpl.cpp \
    $(LOCAL_PATH)/folly/String.cpp \
    $(LOCAL_PATH)/folly/dynamic.cpp \
    $(LOCAL_PATH)/folly/FileUtil.cpp \
    $(LOCAL_PATH)/folly/Format.cpp \
    $(LOCAL_PATH)/folly/net/NetOps.cpp \
    $(LOCAL_PATH)/folly/json_pointer.cpp \
    $(LOCAL_PATH)/folly/lang/CString.cpp \
    $(LOCAL_PATH)/folly/detail/UniqueInstance.cpp \
    $(LOCAL_PATH)/folly/hash/SpookyHashV2.cpp \
    $(LOCAL_PATH)/folly/container/detail/F14Table.cpp \
    $(LOCAL_PATH)/folly/ScopeGuard.cpp \
    $(LOCAL_PATH)/folly/portability/SysUio.cpp \
    $(LOCAL_PATH)/folly/lang/Assume.cpp \

# add compile options
# -----begin compiler defs
LOCAL_CXXFLAGS:= -std=c++17 -fvisibility-inlines-hidden -fexceptions -fno-omit-frame-pointer -frtti
LOCAL_CXXFLAGS+= -Werror=return-type -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare -g -O2 $(MACRO_DEFS)
# ------end compiler defs

# source fies
LOCAL_SRC_FILES:= \
    $(SRCS) \

# includes paths
LOCAL_C_INCLUDES:= \
    $(INCLUDES) \
    $(libboost-includes) \

# shared libs
LOCAL_SHARED_LIBRARIES:= \
    libboost_context \
    libboost_filesystem \
    libboost_program_options \
    libboost_regex \
    libboost_system \
    libboost_thread \

LOCAL_STATIC_LIBRARIES:= libglog libdouble_conversion libfmt \

LOCAL_LDFLAGS:= \

include $(BUILD_STATIC_LIBRARY) # build a static lib
# --------- end of libfolly_runtime.a -------------#


# --------------- begin of libfolly_futures.a -----------
include $(CLEAR_VARS)
LOCAL_MODULE:= libfolly_futures

# macro defs
MACRO_DEFS:=
MACRO_DEFS+= -DFOLLY_NO_CONFIG=1
MACRO_DEFS+= -DFOLLY_HAVE_CLOCK_GETTIME=1
# MACRO_DEFS+= -DFOLLY_USE_LIBCPP=1
MACRO_DEFS+= -DFOLLY_HAVE_PTHREAD=1
# eng version enable debug flag
ifneq ($(filter $(XMAKE_BUILD_TYPE), eng userdebug),)
    MACRO_DEFS += -DDEBUG
endif

INCLUDES:= \
    $(LOCAL_PATH)/folly \
    $(THIRD_PARTY_PATH) \
    $(DOUBLE_CONVERTION_PATH) \
    $(FMT_PATH) \
    $(GLOG_PATH) \
    $(LIBEVENT_PATH) \

SRCS:= \
    $(LOCAL_PATH)/folly/ExceptionWrapper.cpp \
    $(LOCAL_PATH)/folly/ExceptionString.cpp \
    $(LOCAL_PATH)/folly/Executor.cpp \
    $(LOCAL_PATH)/folly/Singleton.cpp \
    $(LOCAL_PATH)/folly/Try.cpp \
    $(LOCAL_PATH)/folly/concurrency/CacheLocality.cpp \
    $(LOCAL_PATH)/folly/detail/AsyncTrace.cpp \
    $(LOCAL_PATH)/folly/detail/AtFork.cpp \
    $(LOCAL_PATH)/folly/detail/MemoryIdler.cpp \
    $(LOCAL_PATH)/folly/detail/SingletonStackTrace.cpp \
    $(LOCAL_PATH)/folly/detail/StaticSingletonManager.cpp \
    $(LOCAL_PATH)/folly/detail/ThreadLocalDetail.cpp \
    $(LOCAL_PATH)/folly/fibers/Baton.cpp \
    $(LOCAL_PATH)/folly/fibers/FiberManager.cpp \
    $(LOCAL_PATH)/folly/fibers/Fiber.cpp \
    $(LOCAL_PATH)/folly/fibers/GuardPageAllocator.cpp \
    $(LOCAL_PATH)/folly/futures/detail/Core.cpp \
    $(LOCAL_PATH)/folly/futures/Future.cpp \
    $(LOCAL_PATH)/folly/futures/ThreadWheelTimekeeper.cpp \
    $(LOCAL_PATH)/folly/executors/ExecutorWithPriority.cpp \
    $(LOCAL_PATH)/folly/executors/InlineExecutor.cpp \
    $(LOCAL_PATH)/folly/executors/TimedDrivableExecutor.cpp \
    $(LOCAL_PATH)/folly/executors/QueuedImmediateExecutor.cpp \
    $(LOCAL_PATH)/folly/io/async/AsyncTimeout.cpp \
    $(LOCAL_PATH)/folly/io/async/EventBase.cpp \
    $(LOCAL_PATH)/folly/io/async/EventBaseBackendBase.cpp \
    $(LOCAL_PATH)/folly/io/async/EventBaseLocal.cpp \
    $(LOCAL_PATH)/folly/io/async/EventHandler.cpp \
    $(LOCAL_PATH)/folly/io/async/HHWheelTimer.cpp \
    $(LOCAL_PATH)/folly/io/async/Request.cpp \
    $(LOCAL_PATH)/folly/io/async/TimeoutManager.cpp \
    $(LOCAL_PATH)/folly/io/async/VirtualEventBase.cpp \
    $(LOCAL_PATH)/folly/lang/Exception.cpp \
    $(LOCAL_PATH)/folly/memory/MallctlHelper.cpp \
    $(LOCAL_PATH)/folly/portability/SysMembarrier.cpp \
    $(LOCAL_PATH)/folly/synchronization/AsymmetricMemoryBarrier.cpp \
    $(LOCAL_PATH)/folly/synchronization/Hazptr.cpp \
    $(LOCAL_PATH)/folly/synchronization/WaitOptions.cpp \
    $(LOCAL_PATH)/folly/synchronization/detail/Sleeper.cpp \
    $(LOCAL_PATH)/folly/system/Pid.cpp \

# add compile options
# -----begin compiler defs
LOCAL_CXXFLAGS:= -std=c++17 -fvisibility-inlines-hidden -fexceptions -fno-omit-frame-pointer -frtti
LOCAL_CXXFLAGS+= -Werror=return-type -Wwrite-strings -Woverloaded-virtual -Wno-sign-compare -Wno-unused-variable -g -O2 $(MACRO_DEFS)
# ------end compiler defs

# source fies
LOCAL_SRC_FILES:= \
    $(SRCS) \

# includes paths
LOCAL_C_INCLUDES:= \
    $(INCLUDES) \
    $(libboost-includes) \

# shared libs
LOCAL_SHARED_LIBRARIES:=  \
    libboost_context \
    libboost_filesystem \
    libboost_program_options \
    libboost_regex \
    libboost_system \
    libboost_thread \
    libevent \

LOCAL_STATIC_LIBRARIES:= libfolly_runtime libglog libdouble_conversion libfmt \


LOCAL_LDFLAGS:= \

include $(BUILD_STATIC_LIBRARY) # build a static lib
# --------- end of libfolly_futures.a -------------#