//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_CAPNPROTO_SERIALIZER_H
#define PMEMCPY_CAPNPROTO_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <kj/io.h>
#include <basic_capnp.capnp.h>
#include <vector>
#include <string>
#include <limits>

namespace pmemcpy {

#define CAPNP_PRIM_SERIAL(T, CT)\
    inline std::string _serialize(T &src) {\
        kj::VectorOutputStream output(sizeof(T));\
        capnp::MallocMessageBuilder message;\
        PrimitiveData::Builder num = message.initRoot<PrimitiveData>();\
        num.set##CT(src);\
        capnp::writePackedMessage(output, message.getSegmentsForOutput()); \
        AUTO_TRACE("pmemcpy::capnp::serialize::single size={}", SizeType(output.getArray().size(), SizeType::MB));\
        return std::string((char*)output.getArray().begin(), output.getArray().size());\
    }\
    inline std::string _serialize(T *src, size_t count) {\
        kj::VectorOutputStream output(count*sizeof(T));\
        capnp::MallocMessageBuilder message;\
        PrimitiveData::Builder nums = message.initRoot<PrimitiveData>();\
        nums.set##CT##Arr(kj::ArrayPtr<T>(src, count));\
        capnp::writePackedMessage(output, message.getSegmentsForOutput()); \
        AUTO_TRACE("pmemcpy::capnp::serialize::array size={}", SizeType(output.getArray().size(), SizeType::MB));\
        return std::string((char*)output.getArray().begin(), output.getArray().size());\
    }\
    inline void _deserialize(T &dst, const std::string src) {\
        AUTO_TRACE("pmemcpy::capnp::deserialize::single size={}", SizeType(src.size(), SizeType::MB));\
        kj::ArrayInputStream input(kj::ArrayPtr<const unsigned char>((const unsigned char*)src.c_str(), src.size()));\
        capnp::ReaderOptions opts;\
        opts.traversalLimitInWords = ~0ul;\
        capnp::PackedMessageReader message(input, opts);\
        PrimitiveData::Reader num = message.getRoot<PrimitiveData>();\
        dst = num.get##CT();\
    }\
    inline void _deserialize(T *dst, const std::string src, Dimensions dims) {\
        AUTO_TRACE("pmemcpy::capnp::deserialize::array size={}", SizeType(src.size(), SizeType::MB));\
        kj::ArrayInputStream input(kj::ArrayPtr<const unsigned char>((const unsigned char*)src.c_str(), src.size()));\
        capnp::ReaderOptions opts;\
        opts.traversalLimitInWords = ~0ul;\
        capnp::PackedMessageReader message(input, opts);\
        PrimitiveData::Reader nums = message.getRoot<PrimitiveData>();\
        capnp::List<T>::Reader temp = nums.get##CT##Arr();\
        for(size_t i = 0; i < temp.size(); ++i) { dst[i] = temp[i]; }\
    }

template<typename T>
class CapnProtoSerializer : public Serializer<T> {
private:
    CAPNP_PRIM_SERIAL(int8_t, D8)
    CAPNP_PRIM_SERIAL(int16_t, D16)
    CAPNP_PRIM_SERIAL(int32_t, D32)
    CAPNP_PRIM_SERIAL(int64_t, D64)
    CAPNP_PRIM_SERIAL(uint8_t, U8)
    CAPNP_PRIM_SERIAL(uint16_t, U16)
    CAPNP_PRIM_SERIAL(uint32_t, U32)
    CAPNP_PRIM_SERIAL(uint64_t, U64)
    CAPNP_PRIM_SERIAL(float, F32)
    CAPNP_PRIM_SERIAL(double, F64)

public:
    inline std::string serialize(T &src) {
        return _serialize(src);
    }

    inline std::string serialize(T *src, Dimensions dims) {
        return _serialize(src, dims.count());
    }

    inline void deserialize(T &dst, const std::string src) {
        _deserialize(dst, src);
    }

    inline void deserialize(T *dst, const std::string src, Dimensions dims) {
        _deserialize(dst, src, dims);
    }
};

}

#endif //PMEMCPY_CAPNPROTO_SERIALIZER_H
