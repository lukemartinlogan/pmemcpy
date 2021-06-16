//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_SERIALIZER_H
#define PMEMCPY_SERIALIZER_H

#include <string>
#include <vector>

namespace pmemcpy {

typedef struct Dimensions {
private:
    std::vector<size_t> dims_;
    std::vector<size_t> offs_;
    size_t count_;
public:
    template<typename ...Args>
    Dimensions(Args ...args) : count_(1) {
        dims_ = {args...};
        for(const size_t &dim : dims_) {
            count_ *= dim;
        }
        /*size_t i = dims_.size() - 1;
        size_t off = 1;
        for(auto dim = dims_.rbegin(); dim != dims_.rend(); ++dim) {
            off *= *dim;
            offs_[i--] = off;
        }*/
    }
    size_t count() { return count_; }
    size_t get_off(int dim) { return offs_[dim]; }
    size_t *data() { return dims_.data(); }
    size_t operator [](int i) { return dims_[i]; }
    size_t operator [](size_t i) { return dims_[i]; }
} Dimensions, Offsets, Sizes;

template<typename T>
class Serializer {
public:
    inline virtual std::string serialize(T &src) = 0;
    inline virtual std::string serialize(T *src, Dimensions dims) = 0;
    inline virtual void deserialize(T &dst, const std::string src) = 0;
    inline virtual void deserialize(T *dst, const std::string src, Dimensions dims) = 0;
};

}

#endif //PMEMCPY_SERIALIZER_H
