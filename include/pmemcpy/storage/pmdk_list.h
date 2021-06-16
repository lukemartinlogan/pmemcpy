//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_PMDK_LIST_H
#define PMEMCPY_PMDK_LIST_H

#include <pmemcpy/storage/storage.h>
#include <boost/filesystem.hpp>
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

struct PMEMListArgs {
    size_t id_len;
    TOID(char) id;
    size_t len;
    TOID(char) data;
};
struct PMEMPointer {
    POBJ_LIST_ENTRY(struct PMEMPointer) neighbors;
    uint64_t id_len;
    TOID(char) id;
    uint64_t len;
    TOID(char) data;
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
        ppool_ = pmemobj_create(path.c_str(), POBJ_LAYOUT_NAME(liststore), size, 0777);
        if (ppool_ == nullptr) {
            perror("pmemobj create");
            throw 1;
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
    }

    inline void _open(std::string path) {
        ppool_ = pmemobj_open(path.c_str(), POBJ_LAYOUT_NAME(liststore));
        if (ppool_ == nullptr) {
            perror("pmemobj open");
            throw 1;
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
    }

    static int _add(PMEMobjpool *ppool, void *ptr, void *arg) {
        struct PMEMPointer *bucket_entry = (struct PMEMPointer *)ptr;
        struct PMEMListArgs *info = (struct PMEMListArgs *)arg;
        pmemobj_memcpy_persist(ppool, &bucket_entry->id_len, &info->id_len, sizeof(uint64_t));
        pmemobj_memcpy_persist(ppool, &bucket_entry->id, &info->id, sizeof(TOID(char)));
        pmemobj_memcpy_persist(ppool, &bucket_entry->len, &info->len, sizeof(uint64_t));
        pmemobj_memcpy_persist(ppool, &bucket_entry->data, &info->data, sizeof(TOID(char)));
        return 0;
    }

    static int
    _alloc_str(PMEMobjpool *ppool, void *ptr, void *arg) {
        char *data_dst = (char*)ptr;
        std::string *data_src = (std::string*)arg;
        pmemobj_memcpy_persist(ppool, data_dst, data_src->c_str(), data_src->size());
        return 0;
    }

    TOID(char) _stralloc(std::string *str) {
        TOID(char) id_pmem;
        int ret = POBJ_ALLOC(ppool_, &id_pmem, char, str->size(), _alloc_str, str);
        if(ret < 0) {
            fprintf(stderr, "error: failed to allocate ID: %s\n", pmemobj_errormsg());
            throw 1;
        }
        return id_pmem;
    }

    inline TOID(struct PMEMPointer) _find(std::string id) {
        TOID(struct PMEMPointer) list_entry;
        POBJ_LIST_FOREACH(list_entry, &D_RO(root_)->head, neighbors) {
            uint64_t entry_id_len = D_RO(list_entry)->id_len;
            if(entry_id_len == id.size()) {
                if (strncmp(D_RO(D_RO(list_entry)->id), id.c_str(), entry_id_len) == 0) {
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
        if (not boost::filesystem::exists(path)) {
            _create(path, size == 0 ? PMEMOBJ_MIN_POOL : size);
        } else {
            _open(path);
        }
    }

    void munmap() {
        if (ppool_ != nullptr) {
            pmemobj_close(ppool_);
            ppool_ = nullptr;
        }
    }

    void release(std::string path) {
        remove(path.c_str());
    }

    void store(std::string id, std::string &src) {
        struct PMEMListArgs info;

        //Allocate string and ID in PMEM
        info.id_len = id.size();
        info.id = _stralloc(&id);
        info.len = src.size();
        info.data = _stralloc(&src);

        //Allocate list entry
        PMEMoid oid = POBJ_LIST_INSERT_NEW_HEAD(ppool_, &D_RW(root_)->head, neighbors, sizeof(struct PMEMPointer)+src.size(), _add, &info);
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
            TOID(struct PMEMPointer) bucket_entry = _find(id);
            len = D_RO(bucket_entry)->len;
            buffer = (char*)malloc(len);
            std::memcpy(buffer, D_RO(D_RO(bucket_entry)->data), len);
        } TX_ONABORT {
                fprintf(stderr, "error: reading from pmem aborted\n");
        } TX_END
        return std::string(buffer, len);
    }

    void free(std::string id) {
        TX_BEGIN(ppool_) {
            TX_ADD(root_);
            TOID(struct PMEMPointer) bucket_entry = _find(id);
            POBJ_FREE(&D_RO(bucket_entry)->id);
            POBJ_FREE(&D_RO(bucket_entry)->data);
            POBJ_LIST_REMOVE_FREE(ppool_, &D_RW(root_)->head, bucket_entry, neighbors);
        } TX_ONABORT {
                fprintf(stderr, "error: reading from pmem aborted\n");
        } TX_END
    }
};

}

#endif //PMEMCPY_PMDK_LIST_H
