//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_SERIALIZER_H
#define PMEMCPY_SERIALIZER_H

#include <pmemcpy/util/serializeable.h>
#include <string>
#include <vector>

namespace pmemcpy {

typedef struct Dimensions {
private:
    std::vector<size_t> dims_;
    size_t count_;
public:
    template<typename ...Args>
    Dimensions(Args ...args) : count_(1) {
        dims_ = {args...};
        for(const size_t &dim : dims_) {
            count_ *= dim;
        }
    }

    size_t count() { return count_; }
    size_t *data() { return dims_.data(); }
    size_t operator [](int i) { return dims_[i]; }
    size_t operator [](size_t i) { return dims_[i]; }
} Dimensions, Offsets, Sizes;

template<typename T>
class Serializer {
public:
    inline virtual size_t est_encoded_size(size_t size) = 0;
    inline virtual size_t serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T &src) = 0;
    inline virtual size_t serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T *src, Dimensions dims) = 0;
    inline virtual std::shared_ptr<pmemcpy::generic_buffer> serialize(T &src) = 0;
    inline virtual std::shared_ptr<pmemcpy::generic_buffer> serialize(T *src, Dimensions dims) = 0;
    inline virtual void deserialize(T &dst, const std::shared_ptr<pmemcpy::generic_buffer> src) = 0;
    inline virtual void deserialize(T *dst, const std::shared_ptr<pmemcpy::generic_buffer> src, Dimensions dims) = 0;
};

}

#endif //PMEMCPY_SERIALIZER_H
