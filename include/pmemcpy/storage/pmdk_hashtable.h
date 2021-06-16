//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_PMDK_HASHTABLE_H
#define PMEMCPY_PMDK_HASHTABLE_H

#include <pmemcpy/storage/storage.h>
#include <boost/filesystem.hpp>
#include <string>
#include <cstdlib>
#include <memory>
#include <libpmemobj.h>
#include <libpmemlog.h>

namespace pmemcpy::pmdk::hash {

/*DATA STRUCTURES*/
POBJ_LAYOUT_BEGIN(objstore);
POBJ_LAYOUT_ROOT(objstore, struct PMEMHeader)
POBJ_LAYOUT_TOID(objstore, struct PMEMPointer)
POBJ_LAYOUT_TOID(objstore, struct PMEMPointerTable)
POBJ_LAYOUT_TOID(objstore, char)
POBJ_LAYOUT_END(objstore)


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
struct PMEMPointerTable {
    uint64_t nbuckets;
    struct PMEMPointerList buckets[];
};
struct PMEMHeader {
    TOID(struct PMEMPointerTable) table;
    PMEMmutex lock;
};
#define ROOT_TABLE_RW(root) (D_RW(D_RW(root)->table))
#define ROOT_TABLE_RO(root) (D_RO(D_RO(root)->table))
#define BUCKET_RW(root, idx) ((struct PMEMPointerList*)((char*)&ROOT_TABLE_RW(root)->buckets + idx*sizeof(struct PMEMPointerList)))

/*IMPLEMENTATION*/

class PMDKHashtableStorage : public Storage {
private:
    PMEMobjpool *ppool_;
    TOID(struct PMEMHeader) root_;
    uint64_t nbuckets_;
    int nodecomm_, nodesize_, noderank_;

    inline void _create(std::string path, uint64_t size, uint64_t nbuckets=1000) {
        if(size < sizeof(struct PMEMHeader) + 32*nbuckets*sizeof(struct PMEMPointerList)) {
            throw "Not enough space allocated in the pool";
        }
        ppool_ = pmemobj_create(path.c_str(), POBJ_LAYOUT_NAME(objstore), size, 0777);
        nbuckets_ = nbuckets;
        if (ppool_ == nullptr) {
            perror("pmemobj create");
            throw 1;
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
        TX_BEGIN(ppool_) {
            TX_ADD(root_);
            D_RW(root_)->table = TX_ZALLOC(struct PMEMPointerTable, sizeof(PMEMPointerTable) + sizeof(PMEMPointerList)*nbuckets);
            ROOT_TABLE_RW(root_)->nbuckets = nbuckets;
            pmemobj_persist(ppool_, &ROOT_TABLE_RW(root_)->nbuckets, sizeof(uint64_t));
        }
        TX_ONABORT {
            fprintf(stderr, "error: creating objstore aborted\n");
            release(path);
        } TX_END
    }

    inline void _open(std::string path) {
        ppool_ = pmemobj_open(path.c_str(), POBJ_LAYOUT_NAME(objstore));
        if (ppool_ == nullptr) {
            perror("pmemobj open");
            throw 1;
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
        nbuckets_ = ROOT_TABLE_RO(root_)->nbuckets;
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

    inline TOID(struct PMEMPointer) _find(std::string id, uint64_t &hash) {
        hash = _hashfun(id);
        TOID(struct PMEMPointer) list_entry;
        POBJ_LIST_FOREACH(list_entry, BUCKET_RW(root_, hash), neighbors) {
            uint64_t entry_id_len = D_RO(list_entry)->id_len;
            if(entry_id_len == id.size()) {
                if (strncmp(D_RO(D_RO(list_entry)->id), id.c_str(), entry_id_len) == 0) {
                    return list_entry;
                }
            }
        }
        throw 1;
    }

    inline TOID(struct PMEMPointer) _find(std::string id) { uint64_t hash; return _find(id, hash); }

    inline uint64_t _hashfun(std::string &data) {
        return std::hash<std::string>{}(data)%nbuckets_;
    }

public:
    PMDKHashtableStorage() : ppool_(nullptr) {
        /*MPI_Comm_split_type(MPI_COMM_WORLD, MPI_COMM_TYPE_SHARED, rank,
                            MPI_INFO_NULL, &nodecomm_);
        MPI_Comm_size(nodecomm_, &nodesize_);
        MPI_Comm_rank(nodecomm_, &noderank_);*/
    }
    ~PMDKHashtableStorage() { munmap(); }

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
        uint64_t hash = _hashfun(id);
        struct PMEMListArgs info;

        //Allocate string and ID in PMEM
        info.id_len = id.size();
        info.id = _stralloc(&id);
        info.len = src.size();
        info.data = _stralloc(&src);

        //Allocate list entry
        PMEMoid oid = POBJ_LIST_INSERT_NEW_HEAD(ppool_, BUCKET_RW(root_, hash), neighbors, sizeof(struct PMEMPointer)+src.size(), _add, &info);
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
        uint64_t hash;
        TX_BEGIN(ppool_) {
            TX_ADD(root_);
            TOID(struct PMEMPointer) bucket_entry = _find(id, hash);
            POBJ_FREE(&D_RO(bucket_entry)->id);
            POBJ_FREE(&D_RO(bucket_entry)->data);
            POBJ_LIST_REMOVE_FREE(ppool_, BUCKET_RW(root_, hash), bucket_entry, neighbors);
        } TX_ONABORT {
            fprintf(stderr, "error: reading from pmem aborted\n");
        } TX_END
    }
};

}

#endif //PMEMCPY_PMDK_HASHTABLE_H
