//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_MSGPACK_SERIALIZER_H
#define PMEMCPY_MSGPACK_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <msgpack.hpp>
#include <string>

namespace pmemcpy {

template<typename T>
class MsgpackSerializer : public Serializer<T> {
public:
    inline std::string serialize(const T *src) {
        std::stringstream ss;
        msgpack::pack(ss, *src);
        return ss.str();
    }

    inline void deserialize(T *dst, const std::string src) {
        msgpack::object_handle oh = msgpack::unpack(src.c_str(), src.size());
        msgpack::object deserialized = oh.get();
        deserialized.convert(*dst);
    }
};

}

#endif //PMEMCPY_MSGPACK_SERIALIZER_H
