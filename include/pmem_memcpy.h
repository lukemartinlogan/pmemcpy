//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PM_MEMCPY_PMEM_MEMCPY_H
#define PM_MEMCPY_PMEM_MEMCPY_H

#include <msgpack.hpp>
#include <string>
#include <memory>
#include "libpmemobj.h"

namespace sandia {
    struct PMEMPointer {
        POBJ_LIST_ENTRY(PMEMPointer) neighbors;
        size_t size;
        char *data;
    };

    struct PMEMHeader {
        size_t nbuckets;
        POBJ_LIST_HEAD(PMEMPointerList, PMEMPointer) *buckets;
    };

    POBJ_LAYOUT_BEGIN(objstore);
        POBJ_LAYOUT_ROOT(objstore, PMEMHeader);
        POBJ_LAYOUT_TOID(objstore, PMEMPointer);
        POBJ_LAYOUT_TOID(objstore, char);
    POBJ_LAYOUT_END(objstore);

    class PMEM {
    private:
        PMEMobjpool *ppool_;
        TO_ID(PMEMHeader) root_;
        size_t nbuckets_;

        inline void _create(const std::string path, const size_t size, size_t nbuckets=1000) {
            ppool_ = pmemobj_create(path, POBJ_LAYOUT_NAME(objstore), size, 0666);
            nbuckets_ = nbuckets;
            if (ppool_ == nullptr) {
                perror("pmemobj create");
                throw 1;
            }
            root_ = POBJ_ROOT(pop, PMEMHeader);
            TX_BEGIN(ppool_) {
                TX_ADD(root_);
                D_RW(root_)->nbuckets = nbuckets;
                D_RW(root_)->buckets = TX_ZALLOC(PMEMPointerList, nbuckets);
            } TX_ONABORT {
                    fprintf(stderr, "error: creating objstore aborted\n");
                    ret = (-ECANCELED);
            } TX_END
        }

        inline void _open(const std::string path) {
            ppool_ = pmemobj_open(path, POBJ_LAYOUT_NAME(objstore));
            if (ppool_ == nullptr) {
                perror("pmemobj open");
                throw 1;
            }
            root_ = POBJ_ROOT(pop, PMEMHeader);
        }

        inline void _malloc(const size_t size) {
            return TX_ALLOC(size);
        }

        inline PMEMPointer* _find(const std::string id, size_t &hash) {
            hash = std::hash<std::string>{}(id);
        }

    public:
        PMEM() : ppool_(nullptr) {}

        ~PMEM() {
            if (ppool_ != nullptr) {
                pmem_obj_close(ppool_);
            }
        }

        void mmap(const std::string path, const size_t size = PMEMOBJ_MIN_POOL) {
            if (not std::filesystem::exists(path)) {
                _create(path, size);
            } else {
                _open(path);
            }
        }

        template<typename T>
        static void memcpy(const std::string id, PMEM *dst, T *src) {
            size_t hash;
            std::stringstream buffer;
            msgpack::pack(buffer, *src);
            TX_BEGIN(ppool_) {
                TX_ADD(root_);
                PMEMPointer *existing = _find(id, hash);
                if(!existing) {
                    POBJ_LIST_INSERT_NEW_TAIL(ppool_, &D_RW(root_)->buckets[hash], , neighbors);
                    POBJ_LIST_LAST();
                } else {
                }
            } TX_ONABORT {
                    fprintf(stderr, "error: failed to store data to pmem\n");
                    ret = (-ECANCELED);
            } TX_END
        }

        template<typename T>
        static void memcpy(const std::string id, T *dst, PMEM *src) {
            msgpack::object_handle oh = msgpack::unpack(str.data(), str.size());
            msgpack::object deserialized = oh.get();
            deserialized.convert(*dst);
        }

        static void free(PMEM *pmem, const std::string id) {
        }
    };
}

#endif //PM_MEMCPY_PMEM_MEMCPY_H
