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
#define _GNU_SOURCE
#include <unistd.h>

#ifdef DEBUG
#define AUTO_TRACE(...) pmemcpy::AutoTrace(__VA_ARGS__);
#else
#define AUTO_TRACE(...)
#endif

namespace pmemcpy {
    class AutoTrace {
    public:
        template<typename ...Args>
        AutoTrace(std::string fmt, Args ...args) {
            //std::cout << Formatter::format(fmt, args...) + "\n";
            printf("%s, pid=%d\n", Formatter::format(fmt, args...).c_str(), getpid());
            fflush(stdout);
        }
    };
}

#endif //PMEMCPY_TRACE_H
