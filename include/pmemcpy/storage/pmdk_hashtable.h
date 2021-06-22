//
// Created by lukemartinlogan on 5/28/21.
//

#ifndef PMEMCPY_PMDK_HASHTABLE_H
#define PMEMCPY_PMDK_HASHTABLE_H

#include <pmemcpy/util/errors.h>
#include <pmemcpy/util/trace.h>
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
    uint64_t id_len;
    std::string *id;
    uint64_t len;
    TOID(char) data;
};
struct PMEMPointer {
    POBJ_LIST_ENTRY(struct PMEMPointer) neighbors;
    uint64_t id_len;
    char id[512];
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
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::_create path={}, size={}, nbuckets={}", path, SizeType(size, SizeType::MB), nbuckets);
        if(size < sizeof(struct PMEMHeader) + 32*nbuckets*sizeof(struct PMEMPointerList)) {
            PMDK_HASH_POOL_TOO_SMALL.format(size, nbuckets, sizeof(struct PMEMHeader) + 32*nbuckets*sizeof(struct PMEMPointerList));
        }
        ppool_ = pmemobj_create(path.c_str(), POBJ_LAYOUT_NAME(objstore), size, 0777);
        nbuckets_ = nbuckets;
        if (ppool_ == nullptr) {
            throw PMDK_HASH_POOL_FAILED.format(path, SizeType(size, SizeType::MB), nbuckets, std::string(strerror(errno)));
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
        TX_BEGIN(ppool_) {
            TX_ADD(root_);
            D_RW(root_)->table = TX_ZALLOC(struct PMEMPointerTable, sizeof(PMEMPointerTable) + sizeof(PMEMPointerList)*nbuckets);
            ROOT_TABLE_RW(root_)->nbuckets = nbuckets;
            pmemobj_persist(ppool_, &ROOT_TABLE_RW(root_)->nbuckets, sizeof(uint64_t));
        }
        TX_ONABORT {
            release(path);
            throw PMDK_CREATE_HEADER_FAILED.format(path, size, nbuckets, std::string(strerror(errno)));
        } TX_END
    }

    inline void _open(std::string path) {
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::_open path={}", path);
        ppool_ = pmemobj_open(path.c_str(), POBJ_LAYOUT_NAME(objstore));
        if (ppool_ == nullptr) {
            throw PMDK_CANT_OPEN_POOL.format(path, std::string(strerror(errno)));
        }
        root_ = POBJ_ROOT(ppool_, struct PMEMHeader);
        nbuckets_ = ROOT_TABLE_RO(root_)->nbuckets;
    }

    static int _add(PMEMobjpool *ppool, void *ptr, void *arg) {
        struct PMEMPointer *bucket_entry = (struct PMEMPointer *)ptr;
        struct PMEMListArgs *info = (struct PMEMListArgs *)arg;
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::_add id_len={} data_len={}", SizeType(info->id_len, SizeType::MB), SizeType(info->len, SizeType::MB));
        bucket_entry->id_len = info->id_len;
        bucket_entry->len = info->len;
        memcpy(bucket_entry->id, info->id->c_str(), info->id_len);
        bucket_entry->data = info->data;
        pmemobj_persist(ppool, &bucket_entry, sizeof(struct PMEMPointer));
        return 0;
    }

    static int
    _alloc_str(PMEMobjpool *ppool, void *ptr, void *arg) {
        char *data_dst = (char*)ptr;
        pmemcpy::generic_buffer *data_src = (pmemcpy::generic_buffer*)arg;
        pmemobj_memcpy_persist(ppool, data_dst, data_src->c_str(), data_src->size());
        return 0;
    }

    TOID(char) _stralloc(std::shared_ptr<pmemcpy::generic_buffer> &buf) {
        TOID(char) id_pmem;
        int ret = POBJ_ALLOC(ppool_, &id_pmem, char, buf->size(), _alloc_str, buf.get());
        if(ret < 0) {
            throw PMDK_CANT_ALLOCATE_STR.format(SizeType(buf->size(), SizeType::MB), pmemobj_errormsg());
        }
        return id_pmem;
    }

    TOID(char) _stralloc(size_t size) {
        TOID(char) id_pmem;
        int ret = POBJ_ALLOC(ppool_, &id_pmem, char, size, NULL, NULL);
        if(ret < 0) {
            throw PMDK_CANT_ALLOCATE_STR.format(SizeType(size, SizeType::MB), pmemobj_errormsg());
        }
        return id_pmem;
    }

    inline TOID(struct PMEMPointer) _find(std::string id, uint64_t &hash) {
        hash = _hashfun(id);
        TOID(struct PMEMPointer) list_entry;
        POBJ_LIST_FOREACH(list_entry, BUCKET_RW(root_, hash), neighbors) {
            uint64_t entry_id_len = D_RO(list_entry)->id_len;
            if(entry_id_len == id.size()) {
                if (strncmp(D_RO(list_entry)->id, id.c_str(), entry_id_len) == 0) {
                    return list_entry;
                }
            }
        }
        throw PMDK_CANT_FIND_KEY.format(id);
    }
    inline TOID(struct PMEMPointer) _find(std::string id) { uint64_t hash; return _find(id, hash); }

    inline uint64_t _hashfun(std::string &data) {
        return std::hash<std::string>{}(data)%nbuckets_;
    }

public:
    PMDKHashtableStorage() : ppool_(nullptr) {}
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
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::release path={}", path);
        remove(path.c_str());
    }

    std::shared_ptr<pmemcpy::generic_buffer> alloc(std::string id, size_t size) {
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::alloc id={} size={}", id, SizeType(size, SizeType::MB));
        uint64_t hash = _hashfun(id);
        struct PMEMListArgs info;

        //Allocate string and ID in PMEM
        info.id_len = id.size();
        info.id = &id;
        info.len = size;
        info.data = _stralloc(size);

        //Allocate list entry
        PMEMoid oid = POBJ_LIST_INSERT_NEW_HEAD(ppool_, BUCKET_RW(root_, hash), neighbors, sizeof(struct PMEMPointer), _add, &info);
        if(OID_IS_NULL(oid)) {
            PMDK_CANT_ALLOCATE_OBJ.format(id, size, pmemobj_errormsg());
        }

        return std::shared_ptr<pmemcpy::pmem_buffer>(new pmemcpy::pmem_buffer(D_RW(info.data), info.len));
    }

    void store(std::string id, std::shared_ptr<pmemcpy::generic_buffer> &src) {
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::store id={} src_len={}", id, SizeType(src->size(), SizeType::MB));
        uint64_t hash = _hashfun(id);
        struct PMEMListArgs info;

        //Allocate string and ID in PMEM
        info.id_len = id.size();
        info.id = &id;
        info.len = src->size();
        info.data = _stralloc(src);

        //Allocate list entry
        PMEMoid oid = POBJ_LIST_INSERT_NEW_HEAD(ppool_, BUCKET_RW(root_, hash), neighbors, sizeof(struct PMEMPointer), _add, &info);
        if(OID_IS_NULL(oid)) {
            PMDK_CANT_ALLOCATE_OBJ.format(id, SizeType(src->size(), SizeType::MB), pmemobj_errormsg());
        }
    }

    std::shared_ptr<pmemcpy::generic_buffer> find(std::string id) {
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::store id={}", id);
        uint64_t len;
        TOID(struct PMEMPointer) bucket_entry = _find(id);
        len = D_RO(bucket_entry)->len;
        return std::shared_ptr<pmemcpy::pmem_buffer>(new pmemcpy::pmem_buffer(D_RW(D_RW(bucket_entry)->data), len));
    }

    std::shared_ptr<pmemcpy::generic_buffer> load(std::string id) {
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::store id={}", id);
        std::shared_ptr<pmemcpy::malloc_buffer> buf(new pmemcpy::malloc_buffer());
        uint64_t len;
        TOID(struct PMEMPointer) bucket_entry = _find(id);
        len = D_RO(bucket_entry)->len;
        buf->alloc(len);
        std::memcpy(buf->c_str(), D_RO(D_RO(bucket_entry)->data), len);
        return buf;
    }

    void free(std::string id) {
        AUTO_TRACE("pmemcpy::PMDKHashtableStorage::free id={}", id);
        uint64_t hash;
        TOID(struct PMEMPointer) bucket_entry = _find(id, hash);
        POBJ_FREE(&D_RO(bucket_entry)->data);
        POBJ_LIST_REMOVE_FREE(ppool_, BUCKET_RW(root_, hash), bucket_entry, neighbors);
    }
};

}

#endif //PMEMCPY_PMDK_HASHTABLE_H
