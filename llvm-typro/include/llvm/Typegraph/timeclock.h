#ifndef LLVM_TYPEGRAPHS_TIMECLOCK_H
#define LLVM_TYPEGRAPHS_TIMECLOCK_H

#include <chrono>
#include <iostream>

namespace {
class TimeClock {
  std::chrono::high_resolution_clock::time_point tp;
  std::chrono::high_resolution_clock::time_point stopped;

public:
  TimeClock() { restart(); }

  void restart() { tp = std::chrono::high_resolution_clock::now(); }

  void report(const std::string &msg) {
    auto d = std::chrono::high_resolution_clock::now() - tp;
    auto d2 = std::chrono::duration_cast<std::chrono::milliseconds>(d).count() / 1000.0;
    fprintf(stderr, "[TIME] %7.3fs | %s\n", d2, msg.c_str());
  }

  void stop() {
    stopped = std::chrono::high_resolution_clock::now();
  }

  void cont() {
    tp += (std::chrono::high_resolution_clock::now() - stopped);
  }
};
} // namespace

#endif // LLVM_TYPEGRAPHS_TIMECLOCK_H
