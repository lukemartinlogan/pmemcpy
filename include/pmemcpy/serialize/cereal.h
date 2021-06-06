//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_CEREAL_SERIALIZER_H
#define PMEMCPY_CEREAL_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <cereal/types/array.hpp>
#include <cereal/types/atomic.hpp>
#include <cereal/types/bitset.hpp>
#include <cereal/types/chrono.hpp>
#include <cereal/types/complex.hpp>
#include <cereal/types/forward_list.hpp>
#include <cereal/types/functional.hpp>
#include <cereal/types/list.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/optional.hpp>
#include <cereal/types/queue.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/stack.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/tuple.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/unordered_set.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/valarray.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <string>

namespace pmemcpy {

template<typename T>
class CerealSerializer : public Serializer<T> {
public:
    inline std::string serialize(const T *src) {
        std::stringstream ss;
        cereal::PortableBinaryOutputArchive oarchive(ss);
        oarchive(*src);
        return ss.str();
    }

    inline void deserialize(T *dst, const std::string src) {
        std::stringstream ss(src);
        cereal::PortableBinaryInputArchive iarchive(ss);
        iarchive(*dst);
    }
};

}

#endif //PMEMCPY_CEREAL_SERIALIZER_H
