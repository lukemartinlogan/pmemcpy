//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_PMDK_LIST_H
#define PMEMCPY_PMDK_LIST_H

#include <pmemcpy/storage/storage.h>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <memory>
#include <libpmemobj.h>

namespace pmemcpy::pmdk::list {

POBJ_LAYOUT_BEGIN(liststore);
POBJ_LAYOUT_ROOT(liststore, struct PMEMHeader)
POBJ_LAYOUT_TOID(liststore, struct PMEMPointer)
POBJ_LAYOUT_TOID(liststore, struct PMEMPointerTable)
POBJ_LAYOUT_TOID(liststore, char)
POBJ_LAYOUT_END(liststore)

struct PMEMPointer {
    POBJ_LIST_ENTRY(struct PMEMPointer) neighbors;
    uint64_t id_len;
    char *id;
    uint64_t len;
    char *data;
};
POBJ_LIST_HEAD(PMEMPointerList, struct PMEMPointer);
struct PMEMHeader {
    PMEMPointerList head;
};


class PMDKListStorage : public Storage {
private:
    PMEMobjpool *ppool_;
    TOID(struct PMEMHeader) root_;

    inline void _create(std::string path, uint64_t size) {
        ppool_ = pmemobj_create(path.c_str(), POBJ_LAYOUT_NAME(objstore), size, 0777);
        if (ppool_ == nullptr) {
            perror("pmemobj create");
            throw 1;
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
    }

    inline void _open(std::string path) {
        ppool_ = pmemobj_open(path.c_str(), POBJ_LAYOUT_NAME(objstore));
        if (ppool_ == nullptr) {
            perror("pmemobj open");
            throw 1;
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
    }

    inline TOID(struct PMEMPointer) _find(std::string id) {
        TOID(struct PMEMPointer) list_entry;
        POBJ_LIST_FOREACH(list_entry, &D_RO(root_)->head, neighbors) {
            uint64_t entry_id_len = D_RO(list_entry)->id_len;
            if(entry_id_len == id.size()) {
                if (strncmp(D_RO(list_entry)->id, id.c_str(), entry_id_len) == 0) {
                    return list_entry;
                }
            }
        }
        throw 1;
    }

public:
    PMDKListStorage() {}
    ~PMDKListStorage() {}

    void mmap(std::string path, uint64_t size = 0) {
        if (not std::filesystem::exists(path)) {
            _create(path, size == 0 ? PMEMOBJ_MIN_POOL : size);
        } else {
            _open(path);
        }
    }

    void munmap() {
        if (ppool_ != nullptr) {
            pmemobj_close(ppool_);
        }
    }

    void release(std::string path) {
        remove(path.c_str());
    }

    static int _add(PMEMobjpool *ppool, void *ptr, void *arg) {
        struct PMEMPointer *pmem_ptr = (struct PMEMPointer *)ptr;
        std::string *buffer = (std::string*)arg;
        pmemobj_memcpy_persist(ppool, &pmem_ptr->data, buffer->c_str(), buffer->size());
        pmem_ptr->len = buffer->size();
        pmemobj_persist(ppool, &pmem_ptr->len, sizeof(uint64_t));
        return 0;
    }

    void store(std::string id, std::string &src) {
        PMEMoid oid = POBJ_LIST_INSERT_NEW_HEAD(ppool_, &D_RW(root_)->head, neighbors, sizeof(struct PMEMPointer)+src.size(), _add, &src);
        if(OID_IS_NULL(oid)) {
            fprintf(stderr, "failed to allocate entry: %s\n", pmemobj_errormsg());
            throw 1;
        }
    }

    std::string load(std::string id) {
        std::string str;
        char *buffer;
        uint64_t len;
        TX_BEGIN(ppool_) {
            TX_ADD(root_);
            TOID(struct PMEMPointer) ptr = _find(id);
            len = D_RO(ptr)->len;
            buffer = (char*)malloc(len);
            std::memcpy(buffer, D_RO(ptr)->data, len);
        } TX_ONABORT {
            fprintf(stderr, "error: reading from pmem aborted\n");
            pmemobj_errormsg();
        } TX_END
        return std::string(buffer, len);
    }

    void free(std::string id) {
    }
};

}

#endif //PMEMCPY_PMDK_LIST_H
