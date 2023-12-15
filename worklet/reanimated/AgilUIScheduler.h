#ifndef AGILREACT_AGILUISCHEDULER_H
#define AGILREACT_AGILUISCHEDULER_H


#include "Tools/UIScheduler.h"

class AgilUIScheduler : public reanimated::UIScheduler {

public:

    AgilUIScheduler() = default;

    ~AgilUIScheduler() = default;

    void scheduleOnUI(std::function<void()> job) override;

    void triggerUI() override;

};


#endif //AGILREACT_AGILUISCHEDULER_H
