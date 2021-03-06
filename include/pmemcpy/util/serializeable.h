//
// Created by lukemartinlogan on 5/31/21.
//

#ifndef PMEMCPY_ERROR_SERIALIZER_H
#define PMEMCPY_ERROR_SERIALIZER_H

#include <vector>
#include <string>
#include <type_traits>
#include <cstring>

#define NUMBER_SERIAL(type) \
    std::string new_str = std::to_string(num_.type); \
    memcpy(buf, new_str.c_str(), new_str.size()); \
    return new_str.size();

namespace pmemcpy {

    class Serializeable {
    public:
        virtual size_t serialize(char *buf) = 0;
    };

    struct generic_buffer {
        virtual size_t size() const = 0;
        virtual char* c_str() const = 0;
        virtual char& operator[](int i) = 0;
    };

    struct pmem_buffer : public generic_buffer {
        char *buf_;
        size_t size_;

        pmem_buffer() : buf_(nullptr), size_(0) {}
        pmem_buffer(void *buf, size_t size) : buf_((char*)buf), size_(size) {}
        pmem_buffer(const pmem_buffer& old) {
            buf_ = old.c_str();
            size_ = old.size();
        }
        pmem_buffer(pmem_buffer&& old) {
                buf_ = old.c_str();
                size_ = old.size();
                old.buf_ = nullptr;
                old.size_ = 0;
        }
        ~pmem_buffer() {}

        inline void alloc(size_t size) { buf_ = (char*)malloc(size); }
        inline size_t size() const { return size_; }
        inline char *c_str() const { return (char*)buf_; }
        char& operator [](int i) { return buf_[i]; }
    };

    struct malloc_buffer : public generic_buffer {
    public:
        char *buf_;
        size_t size_;

        malloc_buffer() : buf_(nullptr), size_(0) {}
        malloc_buffer(char *buf, size_t size) : size_(size) {
            buf_ = (char*)malloc(size);
            memcpy(buf_, buf, size);
        }
        malloc_buffer(std::string str) : size_(str.size()) {
            buf_ = (char*)malloc(str.size());
            memcpy(buf_, str.c_str(), str.size());
        }
        malloc_buffer(size_t size) { alloc(size); }
        malloc_buffer(const malloc_buffer& old) {
            buf_ = old.c_str();
            size_ = old.size();
        }
        malloc_buffer(malloc_buffer&& old) {
            buf_ = old.c_str();
            size_ = old.size();
            old.buf_ = nullptr;
            old.size_ = 0;
        }
        ~malloc_buffer() {
            if(buf_) {
                free(buf_);
            }
        }

        inline void alloc(size_t size) {
            size_ = size;
            buf_ = (char*)malloc(size);
        }
        inline size_t size() const { return size_; }
        inline char *c_str() const { return (char*)buf_; }
        char& operator [](int i) { return buf_[i]; }

    };

    struct string_buffer : public generic_buffer {
    public:
        std::string buf_;

        string_buffer() {}
        string_buffer(std::string str) : buf_(std::move(str)) {}
        string_buffer(const string_buffer& old) {
            buf_ = old.buf_;
        }
        string_buffer(malloc_buffer&& old) {
            buf_ = std::move(old.buf_);
        }
        ~string_buffer() {}

        inline size_t size() const { return buf_.size(); }
        inline char *c_str() const { return (char*)&buf_[0]; }
        char& operator [](int i) { return buf_[i]; }

    };

    class SizeType : public Serializeable {
    public:
        double num_;
        size_t unit_;

        static const size_t
                BYTES=1,
                KB=(1ul << 10),
                MB=(1ul << 20),
                GB=(1ul << 30),
                TB=(1ul << 40);

        std::string unit_to_str(size_t unit) {
            switch(unit) {
                case BYTES: return "BYTES";
                case KB: return "KB";
                case MB: return "MB";
                case GB: return "GB";
                case TB: return "TB";
            }
            return "";
        }

        SizeType() : num_(0), unit_(0) {}
        SizeType(const SizeType &old_obj) {
            num_ = old_obj.num_;
            unit_ = old_obj.unit_;
        }

        SizeType(int8_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(int16_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(int32_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(int64_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(uint8_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(uint16_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(uint32_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(uint64_t bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(float bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}
        SizeType(double bytes, size_t unit) : num_(((double)bytes)/unit), unit_(unit) {}

        size_t serialize(char *buf) {
            std::string serial = std::to_string(num_) + unit_to_str(unit_);
            memcpy(buf, serial.c_str(), serial.size());
            return serial.size();
        }
    };

    class Arg : Serializeable {
    private:
        union {
            int d32;
            long d64;
            long long d128;
            unsigned u32;
            unsigned long u64;
            unsigned long long u128;
            float f32;
            double f64;
            long double f96;
        } num_;
        std::string str_;
        Serializeable *obj_;
        std::shared_ptr<Serializeable> obj_shared_;
        int type_;
    public:
        Arg(int num) : type_(0) { num_.d32 = num; }
        Arg(long num) : type_(1) { num_.d64 = num; }
        Arg(long long num) : type_(2) { num_.d128 = num; }
        Arg(unsigned int num) : type_(3) { num_.u32 = num; }
        Arg(unsigned long num) : type_(4) { num_.u64 = num; }
        Arg(unsigned long long num) : type_(5) { num_.u128 = num; }
        Arg(float num) : type_(6) { num_.f32 = num; }
        Arg(double num) : type_(7) { num_.f64 = num; }
        Arg(long double num) : type_(8) { num_.f96 = num; }
        Arg(char *str) : type_(9) { if(str) str_ = std::string(str); else str_ = ""; }
        Arg(const char *str) : type_(9) { if(str) str_ = std::string(str); else str_ = ""; }
        Arg(std::string str) : type_(9) { str_ = str; }
        Arg(Serializeable *obj) : type_(10) { obj_ = obj; }
        Arg(std::shared_ptr<Serializeable> obj) : type_(11) { obj_shared_ = obj; }
        template<typename T>
        Arg(T obj) : type_(11) { obj_shared_ = std::shared_ptr<Serializeable>(new T(obj)); }

        size_t serialize(char *buf) {
            switch(type_) {
                case 0: {
                    NUMBER_SERIAL(d32)
                }
                case 1: {
                    NUMBER_SERIAL(d64)
                }
                case 2: {
                    NUMBER_SERIAL(d128)
                }
                case 3: {
                    NUMBER_SERIAL(u32)
                }
                case 4: {
                    NUMBER_SERIAL(u64)
                }
                case 5: {
                    NUMBER_SERIAL(u128)
                }
                case 6: {
                    NUMBER_SERIAL(f32)
                }
                case 7: {
                    NUMBER_SERIAL(f64)
                }
                case 8: {
                    NUMBER_SERIAL(f96)
                }
                case 9: {
                    memcpy(buf, str_.c_str(), str_.size());
                    return str_.size();
                }
                case 10: {
                    return obj_->serialize(buf);
                }
                case 11: {
                    return obj_shared_->serialize(buf);
                }
            }
            return 0;
        }
    };

    class ArgPacker {
    private:
        std::vector<Arg> args_;
    public:
        template<typename ...Args>
        ArgPacker(Args ...args) {
            args_ = {args...};
        }
        Arg& operator [](int pos) { return args_[pos]; }

        std::vector<Arg>::iterator begin() { return args_.begin(); }
        std::vector<Arg>::iterator end() { return args_.end(); }
        std::vector<Arg>::reverse_iterator rbegin() { return args_.rbegin(); }
        std::vector<Arg>::reverse_iterator rend() { return args_.rend(); }
    };

    class Formatter {
    public:
        template<typename ...Args>
        static std::string format(std::string fmt, Args ...args) {
            ArgPacker params(args...);
            char *buffer = (char*)calloc(8192, 1);
            size_t off = 0;
            int arg = 0;
            for (int i = 0; i < fmt.size(); ++i) {
                if (fmt[i] == '{' && fmt[i + 1] == '}') {
                    off += params[arg++].serialize(buffer + off);
                    ++i;
                    continue;
                }
                buffer[off++] = fmt[i];
            }
            return std::string(buffer, off);
        }
    };
}

#endif //PMEMCPY_ERROR_SERIALIZER_H
