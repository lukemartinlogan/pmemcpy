//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_SERIALIZER_FACTORY_H
#define PMEMCPY_SERIALIZER_FACTORY_H

#include <memory>
#include <pmemcpy/serialize/msgpack.h>
#include <pmemcpy/serialize/cereal.h>
#include <pmemcpy/serialize/boost.h>
#include <pmemcpy/serialize/boost.h>

namespace pmemcpy {

enum class SerializerType {
    CEREAL,
    MSGPACK,
    BOOST
};

template<typename T>
class SerializerFactory {
public:
    static std::unique_ptr<Serializer<T>> get(SerializerType type) {
        switch(type) {
            case SerializerType::CEREAL: {
                return nullptr;
            }
            case SerializerType::MSGPACK: {
                return std::make_unique<MsgpackSerializer<T>>();
            }
            case SerializerType::BOOST: {
                return nullptr;
            }
        }
        return nullptr;
    }
};

}

#endif //PMEMCPY_SERIALIZER_FACTORY_H
