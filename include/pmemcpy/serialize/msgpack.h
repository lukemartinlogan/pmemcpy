//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_MSGPACK_SERIALIZER_H
#define PMEMCPY_MSGPACK_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <pmemcpy/util/vector_ptr.h>
#include <msgpack.hpp>
#include <vector>
#include <string>

namespace pmemcpy {

template<typename T>
class MsgpackSerializer : public Serializer<T> {
public:
    inline std::string serialize(T &src) {
        std::stringstream ss;
        msgpack::pack(ss, src);
        return ss.str();
    }

    inline std::string serialize(T *src, Dimensions dims) {
        std::vector<T, NoAllocator<T>> temp_(NoAllocator<T>(src, dims.count()));
        temp_.resize(dims.count());
        std::stringstream ss;
        msgpack::pack(ss, temp_);
        return ss.str();
    }

    inline void deserialize(T &dst, const std::string src) {
        msgpack::object_handle oh = msgpack::unpack(src.c_str(), src.size());
        msgpack::object deserialized = oh.get();
        deserialized.convert(dst);
    }

    inline void deserialize(T *dst, const std::string src, Dimensions dims) {
        std::vector<T, NoAllocator<T>> temp_(NoAllocator<T>(dst, dims.count()));
        temp_.resize(dims.count());
        msgpack::object_handle oh = msgpack::unpack(src.c_str(), src.size());
        msgpack::object deserialized = oh.get();
        deserialized.convert(temp_);
    }
};

}

#endif //PMEMCPY_MSGPACK_SERIALIZER_H
