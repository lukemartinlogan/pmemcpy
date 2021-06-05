//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_BOOST_SERIALIZER_H
#define PMEMCPY_BOOST_SERIALIZER_H

#include <pmemcpy/serialize/serializer.h>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <string>

namespace pmemcpy {

template<typename T>
class BoostSerializer : public Serializer<T> {
public:
    inline std::string serialize(const T *src) {
        std::stringstream ss;
        boost::archive::text_oarchive iarch(ss);
        oarch << *src;
        return ss.str();
    }

    inline void deserialize(T *dst, const std::string src) {
        boost::archive::text_iarchive iarch(src);
        iarch >> *dst;
    }
};

}

#endif //PMEMCPY_BOOST_SERIALIZER_H
