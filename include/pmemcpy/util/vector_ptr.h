//
// Created by lukemartinlogan on 6/9/21.
//

#ifndef PM_VECTOR_PTR_H
#define PM_VECTOR_PTR_H

#include <memory>

namespace pmemcpy {

template<typename T>
struct NoAllocatorData {
    T *ptr_;
    size_t count_;
};

template<typename T>
class NoAllocator : public std::allocator<T> {
private:
    T *ptr_;
    size_t count_;
public:
    NoAllocator() : ptr_(nullptr), count_(0) {}
    NoAllocator(T *ptr, size_t count) : ptr_(ptr), count_(count) {}
    template <class U> NoAllocator(const NoAllocator<U>& old_){ old_.get_ptr(ptr_, count_); }
    ~NoAllocator() {}

    template <typename U>
    struct rebind
    {
        typedef NoAllocator other;
    };

    void set_ptr(T *ptr, size_t count) {
        ptr_ = ptr;
        count_ = count;
    }
    void get_ptr(T *&ptr, size_t &count) {
        ptr = ptr_;
        count = count_;
    }

    T* address(T &x) const {return &x;}
    const T* address(const T &x) const {return &x;}
    size_t max_size() const throw() { return count_; }
    T* allocate(size_t n, T* hint = 0) { return ptr_; }
    void deallocate(T *p, size_t n) {}
    void construct(T *p, const T& val) {}
    void construct(T *p) {}
    void destroy(T *p) {}
};

}

#endif //PM_VECTOR_PTR_H
