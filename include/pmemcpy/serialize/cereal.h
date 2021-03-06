//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_CEREAL_SERIALIZER_H
#define PMEMCPY_CEREAL_SERIALIZER_H

#include <pmemcpy/util/errors.h>
#include <pmemcpy/util/trace.h>
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
#include <sstream>

namespace pmemcpy {

template<typename T>
class CerealSerializer : public Serializer<T> {
public:
    inline size_t est_encoded_size(size_t size) {
        return 0;
    }

    inline size_t serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T &src) {
        return 0;
    }

    inline size_t serialize(std::shared_ptr<pmemcpy::generic_buffer> buf, T *src, Dimensions dims) {
        return 0;
    }

    inline std::shared_ptr<pmemcpy::generic_buffer> serialize(T &src) {
        std::stringstream ss;
        cereal::PortableBinaryOutputArchive oarchive(ss);
        oarchive(src);
        AUTO_TRACE("pmemcpy::cereal::serialize::single size={}", SizeType(ss.str().size(), SizeType::MB));
        return std::shared_ptr<pmemcpy::string_buffer>(new pmemcpy::string_buffer(ss.str()));
    }

    inline std::shared_ptr<pmemcpy::generic_buffer> serialize(T *src, Dimensions dims) {
        std::stringstream ss;
        cereal::PortableBinaryOutputArchive oarchive(ss);
        oarchive(cereal::binary_data(src, sizeof(T) * dims.count()));
        AUTO_TRACE("pmemcpy::cereal::serialize::array size={}", SizeType(ss.str().size(), SizeType::MB));
        return std::shared_ptr<pmemcpy::string_buffer>(new pmemcpy::string_buffer(ss.str()));
    }

    inline void deserialize(T &dst, std::shared_ptr<pmemcpy::generic_buffer> src) {
        AUTO_TRACE("pmemcpy::cereal::deserialize::single size={}", SizeType(src->size(), SizeType::MB));
        std::stringstream ss(src->c_str());
        cereal::PortableBinaryInputArchive iarchive(ss);
        iarchive(dst);
    }

    inline void deserialize(T *dst, std::shared_ptr<pmemcpy::generic_buffer> src, Dimensions dims) {
        AUTO_TRACE("pmemcpy::cereal::deserialize::array size={}", SizeType(src->size(), SizeType::MB));
        std::stringstream ss(src->c_str());
        cereal::PortableBinaryInputArchive iarchive(ss);
        iarchive(cereal::binary_data(dst, sizeof(T) * dims.count()));
    }
};

}

#endif //PMEMCPY_CEREAL_SERIALIZER_H
