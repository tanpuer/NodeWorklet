#pragma once

#include <string>
//Todo Caf3
//#include "ns/NSLog.h"

namespace reanimated {

class LoggerInterface {
 public:
  virtual void log(const char *str) = 0;
  virtual void log(const std::string &str) = 0;
  virtual void log(double d) = 0;
  virtual void log(int i) = 0;
  virtual void log(bool b) = 0;
  virtual ~LoggerInterface() {}
};

class LoggerImpl: public LoggerInterface {
public:
    void log(const char *str) override {
//        NS_LOGD("Reanimated Logger: %s", str);
    }

    void log(const std::string &str) override {
//        NS_LOGD("Reanimated Logger: %s", str.c_str());
    }

    void log(double d) override {
//        NS_LOGD("Reanimated Logger: %lld", d);
    }

    void log(int i) override {
//        NS_LOGD("Reanimated Logger: %d", i);
    }

    void log(bool b) override {
//        NS_LOGD("Reanimated Logger: %d", b);
    }

    ~LoggerImpl() override {}
};

} // namespace reanimated
