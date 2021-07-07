//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_CAPNPROTO_NOCOMPRESS_SERIALIZER_H
#define PMEMCPY_CAPNPROTO_NOCOMPRESS_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <capnp/message.h>
#include <capnp/serialize-packed.h>
#include <kj/io.h>
#include <basic_capnp.capnp.h>
#include <vector>
#include <string>
#include <limits>

namespace pmemcpy {

#define CAPNP_NO_COMP_PRIM_SERIAL(T, CT)\
    inline size_t _serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T &src) {\
        kj::ArrayOutputStream output(kj::ArrayPtr<kj::byte>((kj::byte*)buf->c_str(), buf->size()));\
        capnp::MallocMessageBuilder message;\
        PrimitiveData::Builder num = message.initRoot<PrimitiveData>();\
        num.set##CT(src);\
        capnp::writeMessage(output, message.getSegmentsForOutput()); \
        AUTO_TRACE("pmemcpy::capnp_nocomp::serialize::single buf size={}", SizeType(output.getArray().size(), SizeType::MB)); \
        return output.getArray().size();\
    }\
    inline size_t _serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T *src, size_t count) {\
        kj::ArrayOutputStream output(kj::ArrayPtr<kj::byte>((kj::byte*)buf->c_str(), buf->size()));\
        capnp::MallocMessageBuilder message;\
        PrimitiveData::Builder nums = message.initRoot<PrimitiveData>();\
        nums.set##CT##Arr(kj::ArrayPtr<T>(src, count));\
        capnp::writeMessage(output, message.getSegmentsForOutput()); \
        AUTO_TRACE("pmemcpy::capnp_nocomp::serialize::array buf size={}", SizeType(output.getArray().size(), SizeType::MB));  \
        return output.getArray().size();\
    }\
    inline std::shared_ptr<pmemcpy::generic_buffer> _serialize(T &src) {\
        kj::VectorOutputStream output(est_encoded_size(sizeof(T)));\
        capnp::MallocMessageBuilder message;\
        PrimitiveData::Builder num = message.initRoot<PrimitiveData>();\
        num.set##CT(src);\
        capnp::writeMessage(output, message.getSegmentsForOutput()); \
        AUTO_TRACE("pmemcpy::capnp_nocomp::serialize::single size={}", SizeType(output.getArray().size(), SizeType::MB));\
        return std::shared_ptr<pmemcpy::malloc_buffer>(new pmemcpy::malloc_buffer((char*)output.getArray().begin(), output.getArray().size()));\
    }\
    inline std::shared_ptr<pmemcpy::generic_buffer> _serialize(T *src, size_t count) {\
        kj::VectorOutputStream output(est_encoded_size(count*sizeof(T)));\
        capnp::MallocMessageBuilder message;\
        PrimitiveData::Builder nums = message.initRoot<PrimitiveData>();\
        nums.set##CT##Arr(kj::ArrayPtr<T>(src, count));\
        capnp::writeMessage(output, message.getSegmentsForOutput()); \
        AUTO_TRACE("pmemcpy::capnp_nocomp::serialize::array size={}", SizeType(output.getArray().size(), SizeType::MB));\
        return std::shared_ptr<pmemcpy::malloc_buffer>(new pmemcpy::malloc_buffer((char*)output.getArray().begin(), output.getArray().size()));\
    }\
    inline void _deserialize(T &dst, const std::shared_ptr<pmemcpy::generic_buffer> src) {\
        AUTO_TRACE("pmemcpy::capnp_nocomp::deserialize::single size={}", SizeType(src->size(), SizeType::MB));\
        capnp::ReaderOptions opts;\
        opts.traversalLimitInWords = ~0ul;\
        capnp::FlatArrayMessageReader message(kj::ArrayPtr<const capnp::word>((const capnp::word*)src->c_str(), src->size()), opts);\
        PrimitiveData::Reader num = message.getRoot<PrimitiveData>();\
        dst = num.get##CT();\
    }\
    inline void _deserialize(T *dst, const std::shared_ptr<pmemcpy::generic_buffer> src, Dimensions dims) {\
        AUTO_TRACE("pmemcpy::capnp_nocomp::deserialize::array size={}", SizeType(src->size(), SizeType::MB));\
        capnp::ReaderOptions opts;\
        opts.traversalLimitInWords = ~0ul;\
        capnp::FlatArrayMessageReader message(kj::ArrayPtr<const capnp::word>((const capnp::word*)src->c_str(), src->size()), opts);\
        PrimitiveData::Reader nums = message.getRoot<PrimitiveData>(); \
        capnp::List<T>::Reader temp = nums.get##CT##Arr();\
        for(size_t i = 0; i < temp.size(); ++i) { dst[i] = temp[i]; }\
    }

template<typename T>
class CapnProtoNoCompressSerializer : public Serializer<T> {
private:
    CAPNP_NO_COMP_PRIM_SERIAL(int8_t, D8)
    CAPNP_NO_COMP_PRIM_SERIAL(int16_t, D16)
    CAPNP_NO_COMP_PRIM_SERIAL(int32_t, D32)
    CAPNP_NO_COMP_PRIM_SERIAL(int64_t, D64)
    CAPNP_NO_COMP_PRIM_SERIAL(uint8_t, U8)
    CAPNP_NO_COMP_PRIM_SERIAL(uint16_t, U16)
    CAPNP_NO_COMP_PRIM_SERIAL(uint32_t, U32)
    CAPNP_NO_COMP_PRIM_SERIAL(uint64_t, U64)
    CAPNP_NO_COMP_PRIM_SERIAL(float, F32)
    CAPNP_NO_COMP_PRIM_SERIAL(double, F64)

public:
    inline size_t est_encoded_size(size_t size) {
        return size*1.1 + 128;
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

#endif //PMEMCPY_CAPNPROTO_NOCOMPRESS_SERIALIZER_H
