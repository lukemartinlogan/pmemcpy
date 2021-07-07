//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_SERIALIZER_H
#define PMEMCPY_SERIALIZER_H

#include <pmemcpy/util/serializeable.h>
#include <string>
#include <vector>

namespace pmemcpy {

#define PMEMCPY_DIMENSIONS_CONSTRUCT(T) \
    template<typename ...Args>\
    Dimensions(T dim1, Args ...args) : count_(1) {\
        std::vector<T> dims = {dim1, args...};  \
        dims_ = std::vector<size_t>(dims.begin(), dims.end());\
        for(const size_t &dim : dims_) {\
            count_ *= dim;\
        }\
    }
typedef struct Dimensions {
private:
    std::vector<size_t> dims_;
    size_t count_;
public:
    PMEMCPY_DIMENSIONS_CONSTRUCT(int8_t)
    PMEMCPY_DIMENSIONS_CONSTRUCT(int16_t)
    PMEMCPY_DIMENSIONS_CONSTRUCT(int32_t)
    PMEMCPY_DIMENSIONS_CONSTRUCT(int64_t)
    PMEMCPY_DIMENSIONS_CONSTRUCT(uint8_t)
    PMEMCPY_DIMENSIONS_CONSTRUCT(uint16_t)
    PMEMCPY_DIMENSIONS_CONSTRUCT(uint32_t)
    PMEMCPY_DIMENSIONS_CONSTRUCT(uint64_t)

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
