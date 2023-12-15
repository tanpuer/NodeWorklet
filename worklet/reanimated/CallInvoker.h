#ifndef NODEWORKLET_CALLINVOKER_H
#define NODEWORKLET_CALLINVOKER_H

#include "functional"

namespace reanimated {

class CallInvoker {

public:

    void invokeAsync(std::function<void()> func);

};

}

#endif //NODEWORKLET_CALLINVOKER_H
