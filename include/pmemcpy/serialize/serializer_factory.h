//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_SERIALIZER_FACTORY_H
#define PMEMCPY_SERIALIZER_FACTORY_H

#include <memory>

#include <pmemcpy/serialize/msgpack.h>
#include <pmemcpy/serialize/cereal.h>
#include <pmemcpy/serialize/boost.h>

#ifdef VECTOR_TEST
#else
#include <pmemcpy/serialize/capnproto.h>
#endif

namespace pmemcpy {

enum class SerializerType {
    CEREAL,
    MSGPACK,
    BOOST,
    CAPNPROTO
};

template<typename T>
class SerializerFactory {
public:
    static std::unique_ptr<Serializer<T>> get(SerializerType type) {
        switch(type) {
            case SerializerType::CEREAL: {
                return std::make_unique<CerealSerializer<T>>();
            }
            case SerializerType::MSGPACK: {
                return std::make_unique<MsgpackSerializer<T>>();
            }
            case SerializerType::BOOST: {
                return std::make_unique<BoostSerializer<T>>();
            }
            case SerializerType::CAPNPROTO: {
#ifdef VECTOR_TEST
                return nullptr;
#else
                return std::make_unique<CapnProtoSerializer<T>>();
#endif
            }
        }
        return nullptr;
    }
};

}

#endif //PMEMCPY_SERIALIZER_FACTORY_H
