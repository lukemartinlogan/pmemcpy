//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_BOOST_SERIALIZER_H
#define PMEMCPY_BOOST_SERIALIZER_H

#include <pmemcpy/util/errors.h>
#include <pmemcpy/util/trace.h>
#include <pmemcpy/serialize/serializer.h>
#include <boost/serialization/array.hpp>
#include <boost/serialization/bitset.hpp>
#include <boost/serialization/complex.hpp>
#include <boost/serialization/forward_list.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/optional.hpp>
#include <boost/serialization/queue.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/stack.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/unordered_map.hpp>
#include <boost/serialization/unordered_set.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/serialization/valarray.hpp>
#include <boost/serialization/variant.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <string>

#include <iostream>
#include <fstream>

namespace pmemcpy {

template<typename T>
class BoostSerializer : public Serializer<T> {
public:
    inline std::string serialize(T &src) {
        std::stringstream ss;
        boost::archive::binary_oarchive oarch(ss);
        oarch << src;
        AUTO_TRACE("pmemcpy::boost::serialize::single size={}", SizeType(ss.str().size(), SizeType::MB));
        return ss.str();
    }

    inline std::string serialize(T *src, Dimensions dims) {
        std::vector<T, NoAllocator<T>> temp_(NoAllocator<T>(src, dims.count()));
        temp_.resize(dims.count());
        std::stringstream ss;
        boost::archive::binary_oarchive oarch(ss);
        oarch << temp_;
        AUTO_TRACE("pmemcpy::boost::serialize::array size={}", SizeType(ss.str().size(), SizeType::MB));
        return ss.str();
    }

    inline void deserialize(T &dst, const std::string src) {
        AUTO_TRACE("pmemcpy::boost::deserialize::single size={}", SizeType(src.size(), SizeType::MB));
        std::stringstream ss = std::stringstream(src);
        boost::archive::binary_iarchive iarch(ss);
        iarch >> dst;
    }

    inline void deserialize(T *dst, const std::string src, Dimensions dims) {
        AUTO_TRACE("pmemcpy::boost::deserialize::array size={}", SizeType(src.size(), SizeType::MB));
        std::vector<T, NoAllocator<T>> temp_(NoAllocator<T>(dst, dims.count()));
        temp_.resize(dims.count());
        std::stringstream ss(src);
        boost::archive::binary_iarchive iarch(ss);
        iarch >> temp_;
    }
};

}

#endif //PMEMCPY_BOOST_SERIALIZER_H
