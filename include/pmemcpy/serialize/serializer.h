//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_SERIALIZER_H
#define PMEMCPY_SERIALIZER_H

#include <string>

namespace pmemcpy {

template<typename T>
class Serializer {
public:
    inline virtual std::string serialize(const T *src) = 0;
    inline virtual void deserialize(T *dst, const std::string src) = 0;
};

}

#endif //PMEMCPY_SERIALIZER_H
