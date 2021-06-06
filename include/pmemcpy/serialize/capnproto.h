//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_CAPNPROTO_SERIALIZER_H
#define PMEMCPY_CAPNPROTO_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>

namespace pmemcpy {

template<typename T>
class CapnProtoSerializer : public Serializer<T> {
public:
    inline std::string serialize(const T *src) {
        return nullptr;
    }

    inline void deserialize(T *dst, const std::string src) {
    }
};

}

#endif //PMEMCPY_CAPNPROTO_SERIALIZER_H
