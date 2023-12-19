#pragma once

#include "functional"
#include "thread"
#include "mutex"
#include "stack"
#include "worklet_log.h"
#include "unistd.h"

class UIThread {

public:

    UIThread() {
        std::thread uiThread(&UIThread::run, this);
        uiThread.detach();
        std::thread workerThread(&UIThread::runWorker, this);
        workerThread.detach();
    }

    void PerformOnNSUIThread(std::function<void()>&& task, bool block = false) {
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

    void runWorker() {
        while (true) {
            std::unique_lock<std::mutex> lock(workerMutex);
            if (this->func == nullptr) {
                workerCondition.wait(lock);
            }
            usleep(15 * 1000);
            std::function<void()> func_ = func;
            this->func = nullptr;
            PerformOnNSUIThread([func_]() {
                func_();
            }, true);
        }
    }

    void postFrameCallback(std::function<void()> &&task) {
        std::unique_lock<std::mutex> lock(workerMutex);
        func = std::move(task);
        workerCondition.notify_all();
    }

private:

    std::mutex mutex;

    std::condition_variable condition;

    std::stack<std::function<void()>> tasks;

    std::mutex workerMutex;

    std::condition_variable workerCondition;

    std::function<void()> func = nullptr;

};

/**
 * 为了后续方便接入HeronSDK
 */
static UIThread *GetHeronSDKManager() {
    static UIThread thread;
    return &thread;
}