#ifndef NODEWORKLET_CALLINVOKER_H
#define NODEWORKLET_CALLINVOKER_H

#include "functional"
#include "uv.h"

namespace reanimated {

class CallInvoker {

public:

    CallInvoker();

    ~CallInvoker();

    void invokeAsync(std::function<void()> func);

private:

    uv_async_t *async_lock = nullptr;

};

}

#endif //NODEWORKLET_CALLINVOKER_H
