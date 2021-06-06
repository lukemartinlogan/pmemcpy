//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_BOOST_SERIALIZER_H
#define PMEMCPY_BOOST_SERIALIZER_H

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
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>

namespace pmemcpy {

template<typename T>
class BoostSerializer : public Serializer<T> {
public:
    inline std::string serialize(const T *src) {
        std::stringstream ss;
        boost::archive::text_oarchive oarch(ss);
        oarch << *src;
        return ss.str();
    }

    inline void deserialize(T *dst, const std::string src) {
        std::stringstream ss = std::stringstream(src);
        boost::archive::text_iarchive iarch(ss);
        iarch >> *dst;
    }
};

}

#endif //PMEMCPY_BOOST_SERIALIZER_H
