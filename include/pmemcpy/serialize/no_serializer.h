//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_NO_SERIALIZER_H
#define PMEMCPY_NO_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <string>
#include <cstdlib>
#include <cstring>

namespace pmemcpy {

#define NO_SERIALIZER(T, CT)\
    inline size_t _serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T &src) {\
        AUTO_TRACE("pmemcpy::no_serializer::serialize::single buf size={}", SizeType(sizeof(T), SizeType::MB)); \
        memcpy((void*)buf->c_str(), (void*)&src, sizeof(T));                        \
        return sizeof(T);\
    }\
    inline size_t _serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T *src, size_t count) {\
        size_t size = sizeof(T)*count; \
        AUTO_TRACE("pmemcpy::no_serializer::serialize::array buf size={}", SizeType(size, SizeType::MB));\
        memcpy((void*)buf->c_str(), (void*)src, size);                            \
        return size;\
    }\
    inline std::shared_ptr<pmemcpy::generic_buffer> _serialize(T &src) { \
        AUTO_TRACE("pmemcpy::no_serializer::serialize::single size={}", SizeType(sizeof(T), SizeType::MB)); \
        std::shared_ptr<pmemcpy::malloc_buffer> buf(new pmemcpy::malloc_buffer(sizeof(T)));\
        memcpy((void*)buf->c_str(), (void*)&src, sizeof(T)); \
        return buf;\
    }\
    inline std::shared_ptr<pmemcpy::generic_buffer> _serialize(T *src, size_t count) {\
        size_t size = sizeof(T)*count;      \
        AUTO_TRACE("pmemcpy::no_serializer::serialize::array size={}", SizeType(size, SizeType::MB));\
        std::shared_ptr<pmemcpy::malloc_buffer> buf(new pmemcpy::malloc_buffer(size));\
        memcpy((void*)buf->c_str(), (void*)src, size); \
        return buf;\
    }\
    inline void _deserialize(T &dst, const std::shared_ptr<pmemcpy::generic_buffer> src) {\
        AUTO_TRACE("pmemcpy::no_serializer::deserialize::single size={}", SizeType(src->size(), SizeType::MB)); \
        memcpy((void*)&dst, (void*)src->c_str(), src->size());\
    }\
    inline void _deserialize(T *dst, const std::shared_ptr<pmemcpy::generic_buffer> src, Dimensions dims) {\
        AUTO_TRACE("pmemcpy::no_serializer::deserialize::array size={}", SizeType(src->size(), SizeType::MB));             \
        memcpy((void*)dst, (void*)src->c_str(), src->size());\
    }

    template<typename T>
    class NoSerializer : public Serializer<T> {
    private:
        NO_SERIALIZER(int8_t, D8)
        NO_SERIALIZER(int16_t, D16)
        NO_SERIALIZER(int32_t, D32)
        NO_SERIALIZER(int64_t, D64)
        NO_SERIALIZER(uint8_t, U8)
        NO_SERIALIZER(uint16_t, U16)
        NO_SERIALIZER(uint32_t, U32)
        NO_SERIALIZER(uint64_t, U64)
        NO_SERIALIZER(float, F32)
        NO_SERIALIZER(double, F64)

    public:
        inline size_t est_encoded_size(size_t size) {
            return size;
        }

        inline size_t serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T &src) {
            return _serialize(buf, src);
        }

        inline size_t serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T *src, Dimensions dims) {
            return _serialize(buf, src, dims.count());
        }

        inline std::shared_ptr<pmemcpy::generic_buffer> serialize(T &src) {
            return _serialize(src);
        }

        inline std::shared_ptr<pmemcpy::generic_buffer> serialize(T *src, Dimensions dims) {
            return _serialize(src, dims.count());
        }

        inline void deserialize(T &dst, const std::shared_ptr<pmemcpy::generic_buffer> src) {
            _deserialize(dst, src);
        }

        inline void deserialize(T *dst, const std::shared_ptr<pmemcpy::generic_buffer> src, Dimensions dims) {
            _deserialize(dst, src, dims);
        }
    };

}

#endif //PMEMCPY_NO_SERIALIZER_H
