//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_SERIALIZER_FACTORY_H
#define PMEMCPY_SERIALIZER_FACTORY_H

#include <memory>
#include <pmemcpy/util/errors.h>
#include <pmemcpy/serialize/msgpack.h>
#include <pmemcpy/serialize/cereal.h>
#include <pmemcpy/serialize/boost.h>
#include <pmemcpy/serialize/capnproto.h>
#include <pmemcpy/serialize/capnproto_nocompress.h>
#include <pmemcpy/serialize/no_serializer.h>

namespace pmemcpy {

enum class SerializerType {
    CEREAL,
    MSGPACK,
    BOOST,
    CAPNPROTO,
    CAPNPROTO_NOCOMPRESS,
    NO_SERIALIZER
};

class SerializerTypeConverter {
public:
    static SerializerType convert(std::string name) {
        if(name == "CEREAL") return SerializerType::CEREAL;
        if(name == "MSGPACK") return SerializerType::MSGPACK;
        if(name == "BOOST") return SerializerType::BOOST;
        if(name == "CAPNPROTO") return SerializerType::CAPNPROTO;
        if(name == "CAPNPROTO_NOCOMPRESS") return SerializerType::CAPNPROTO_NOCOMPRESS;
        if(name == "NO_SERIALIZER") return SerializerType::NO_SERIALIZER;
        throw INVALID_SERIALIZER_TYPE.format(name);
    }
    static std::string convert(SerializerType id) {
        switch(id) {
            case SerializerType::CEREAL: return "CEREAL";
            case SerializerType::MSGPACK: return "MSGPACK";
            case SerializerType::BOOST: return "BOOST";
            case SerializerType::CAPNPROTO: return "CAPNPROTO";
            case SerializerType::CAPNPROTO_NOCOMPRESS: return "CAPNPROTO_NOCOMPRESS";
            case SerializerType::NO_SERIALIZER: return "NO_SERIALIZER";
        }
    }
};

template<typename T>
class SerializerFactory {
public:
    static std::unique_ptr<Serializer<T>> get(SerializerType type) {
        switch(type) {
            case SerializerType::CEREAL: { return std::unique_ptr<CerealSerializer<T>>(new CerealSerializer<T>()); }
            case SerializerType::MSGPACK: { return std::unique_ptr<MsgpackSerializer<T>>(new MsgpackSerializer<T>()); }
            case SerializerType::BOOST: { return std::unique_ptr<BoostSerializer<T>>(new BoostSerializer<T>()); }
            case SerializerType::CAPNPROTO: { return std::unique_ptr<CapnProtoSerializer<T>>(new CapnProtoSerializer<T>()); }
            case SerializerType::CAPNPROTO_NOCOMPRESS: return std::unique_ptr<CapnProtoNoCompressSerializer<T>>(new CapnProtoNoCompressSerializer<T>());
            case SerializerType::NO_SERIALIZER: return std::unique_ptr<NoSerializer<T>>(new NoSerializer<T>());
        }
        return nullptr;
    }
};

}

#endif //PMEMCPY_SERIALIZER_FACTORY_H
