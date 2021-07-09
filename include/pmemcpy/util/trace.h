//
// Created by lukemartinlogan on 5/30/21.
//

#ifndef PMEMCPY_TRACE_H
#define PMEMCPY_TRACE_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <memory>
#include <pmemcpy/util/serializeable.h>
#include <chrono>
#include <thread>

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <unistd.h>

#ifdef DEBUG
#define AUTO_TRACE(...) pmemcpy::AutoTrace __tracepoint__(__VA_ARGS__);
#else
#define AUTO_TRACE(...)
#endif

namespace pmemcpy {
    class AutoTrace {
    private:
        std::chrono::system_clock::time_point start_time_, end_time_;
        std::string fmt_;
        double net_time_;
    public:
        template<typename ...Args>
        AutoTrace(std::string fmt, Args ...args) {
            size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
            fmt_ = Formatter::format(fmt, args...);
            printf("START: %s, pid=%d, tid=%lu\n", fmt_.c_str(), getpid(), tid);
            start_time_ = std::chrono::high_resolution_clock::now();
            fflush(stdout);
        }

        ~AutoTrace() {
            size_t tid = std::hash<std::thread::id>{}(std::this_thread::get_id());
            end_time_ = std::chrono::high_resolution_clock::now();
            net_time_ = std::chrono::duration_cast<std::chrono::milliseconds>(end_time_ - start_time_).count();
            printf("END: %s, pid=%d, tid=%lu, runtime=%lf\n", fmt_.c_str(), getpid(), tid, net_time_);
            fflush(stdout);
        }
    };
}

#endif //PMEMCPY_TRACE_H
