//
// Created by lukemartinlogan on 5/30/21.
//

#ifndef PMEMCPY_ERROR_H
#define PMEMCPY_ERROR_H

#include <iostream>
#include <string>
#include <cstdlib>
#include <memory>
#include <pmemcpy/util/serializeable.h>

#define PMEMCPY_ERROR_HANDLE_START() try {
#define PMEMCPY_ERROR_HANDLE_END() } catch(std::shared_ptr<pmemcpy::Error> &err) { err->print(); exit(1); }

namespace pmemcpy {
    class Error {
    private:
        int code_;
        std::string fmt_;
        std::string msg_;
    public:
        Error() : code_(0), fmt_(nullptr) {}
        Error(int code, const std::string &fmt) : code_(code), fmt_(fmt) {}
        ~Error() {}

        int get_code() { return code_; }

        template<typename ...Args>
        std::shared_ptr<Error> format(Args ...args) const {
            std::shared_ptr<Error> err = std::make_shared<Error>(code_, fmt_);
            err->msg_ = Formatter::format(fmt_, args...);
            return err;
        }

        void print() {
            std::cout << msg_ << std::endl;
        }
    };
}

#endif //PMEMCPY_ERROR_H
