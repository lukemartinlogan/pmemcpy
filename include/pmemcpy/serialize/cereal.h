//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_CEREAL_SERIALIZER_H
#define PMEMCPY_CEREAL_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <cereal/archives/binary.hpp>
#include <string>

namespace pmemcpy {

template<typename T>
class CerealSerializer : public Serializer<T> {
public:
    inline std::string serialize(const T *src) {
        std::stringstream ss;
        cereal::BinaryOutputArchive oarchive(ss);
        oarchive(*src)
        return ss.str();
    }

    inline void deserialize(T *dst, const std::string src) {
        cereal::BinaryInputArchive iarchive(src);
        iarchive(*dst);
    }
};

}

#endif //PMEMCPY_CEREAL_SERIALIZER_H
