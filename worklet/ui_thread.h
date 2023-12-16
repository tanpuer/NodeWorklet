#pragma once

#include "functional"
#include "thread"
#include "mutex"
#include "stack"
#include "worklet_log.h"

class UIThread {

public:

    UIThread() {
        std::thread uiThread(&UIThread::run, this);
        uiThread.detach();
    }

    void PerformOnNSUIThread(std::function<void()> task, bool block = false) {
        //todo block js thread
        std::unique_lock<std::mutex> lock(mutex);
        tasks.push(task);
        condition.notify_all();
    }

    void run() {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            if (tasks.empty()) {
                condition.wait(lock);
            }
            auto task = tasks.top();
            tasks.pop();
            task();
        }
    }

private:

    std::mutex mutex;

    std::condition_variable condition;

    std::stack<std::function<void()>> tasks;

};

/**
 * 为了后续方便接入HeronSDK
 */
static UIThread *GetHeronSDKManager() {
    static UIThread thread;
    return &thread;
}