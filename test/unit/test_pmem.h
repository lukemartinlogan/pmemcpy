//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PM_MEMCPY_TEST_PMEM_H
#define PM_MEMCPY_TEST_PMEM_H

struct DataStructure {
    int a;
    double b;
    float c;
};
void (*FUNCTION_TYPE)(int,double,float);

class ComplicatedObject {
private:
    DataStructure data_;
    FUNCTION_TYPE *packer_;
public:
    DataStructure get() {
        return data_;
    }
};

namespace clmdep_msgpack {
    MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
            namespace mv1 = clmdep_msgpack::v1;

            namespace adaptor {
                template<>
                struct convert<Parcel> {
                    mv1::object const &operator()(mv1::object const &o, Parcel &input) const {
                        input.id_ = o.via.array.ptr[0].as<CharStruct>();
                        input.position_ = o.via.array.ptr[1].as<size_t>();
                        input.data_size_ = o.via.array.ptr[2].as<size_t>();
                        input.storage_index_ = o.via.array.ptr[3].as<uint16_t>();
                        input.unique_id = o.via.array.ptr[4].as<std::size_t>();
                        return o;
                    }
                };

                template<>
                struct pack<Parcel> {
                    template<typename Stream>
                    packer <Stream> &operator()(mv1::packer<Stream> &o, Parcel const &input) const {
                        o.pack_array(5);
                        o.pack(input.id_);
                        o.pack(input.position_);
                        o.pack(input.data_size_);
                        o.pack(input.storage_index_);
                        o.pack(input.unique_id);
                        return o;
                    }
                };

                template<>
                struct object_with_zone<Parcel> {
                    void operator()(mv1::object::with_zone &o, Parcel const &input) const {
                        o.type = type::ARRAY;
                        o.via.array.size = 5;
                        o.via.array.ptr = static_cast<clmdep_msgpack::object *>(o.zone.allocate_align(
                                sizeof(mv1::object) * o.via.array.size, MSGPACK_ZONE_ALIGNOF(mv1::object)));
                        o.via.array.ptr[0] = mv1::object(input.id_, o.zone);
                        o.via.array.ptr[1] = mv1::object(input.position_, o.zone);
                        o.via.array.ptr[2] = mv1::object(input.data_size_, o.zone);
                        o.via.array.ptr[3] = mv1::object(input.storage_index_, o.zone);
                        o.via.array.ptr[4] = mv1::object(input.unique_id, o.zone);
                    }
                };
            }
    }
}

#endif //PM_MEMCPY_TEST_PMEM_H
