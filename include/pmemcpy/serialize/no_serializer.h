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
    inline pmemcpy::buffer _serialize(T &src) { \
        AUTO_TRACE("pmemcpy::no_serializer::serialize::single size={}", SizeType(sizeof(T), SizeType::MB)); \
        pmemcpy::buffer buf(sizeof(T));\
        memcpy((void*)buf.c_str(), (void*)&src, sizeof(T));\
        return buf;\
    }\
    inline pmemcpy::buffer _serialize(T *src, size_t count) {\
        size_t size = sizeof(T)*count;      \
        AUTO_TRACE("pmemcpy::no_serializer::serialize::array size={}", SizeType(size, SizeType::MB));\
        T *buf = (T*)malloc(size);\
        memcpy((void*)buf, (void*)src, size); \
        return std::string((char*)buf,size);\
    }\
    inline void _deserialize(T &dst, const pmemcpy::buffer src) {\
        AUTO_TRACE("pmemcpy::no_serializer::deserialize::single size={}", SizeType(src.size(), SizeType::MB)); \
        memcpy((void*)&dst, (void*)src.c_str(), src.size());\
    }\
    inline void _deserialize(T *dst, const pmemcpy::buffer src, Dimensions dims) {\
        AUTO_TRACE("pmemcpy::no_serializer::deserialize::array size={}", SizeType(src.size(), SizeType::MB));             \
        memcpy((void*)dst, (void*)src.c_str(), src.size());\
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
        inline pmemcpy::buffer serialize(T &src) {
            return _serialize(src);
        }

        inline pmemcpy::buffer serialize(T *src, Dimensions dims) {
            return _serialize(src, dims.count());
        }

        inline void deserialize(T &dst, const pmemcpy::buffer src) {
            _deserialize(dst, src);
        }

        inline void deserialize(T *dst, const pmemcpy::buffer src, Dimensions dims) {
            _deserialize(dst, src, dims);
        }
    };

}

#endif //PMEMCPY_NO_SERIALIZER_H