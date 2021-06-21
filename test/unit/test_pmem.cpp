//
// Created by lukemartinlogan on 5/28/21.
//

#include <iostream>
#include <boost/filesystem.hpp>
#include <list>
#include <pmemcpy/memcpy.h>

std::string a_id = "iweljlj;a";
std::string b_id = "asdfklasdfafksdkf";
std::string c_id = "i8w49a";
std::string d_id = "2dgsg4tas";
std::string e_id = "kflkasdl";

void write(pmemcpy::PMEM &pmem) {
    //Write simple type
    double a = 25;
    pmem.store<double>(a_id, a);

    //Write array type
    double data[2][2] = {10, 11, 12, 13};
    pmem.store<double>(b_id, (double*)data, pmemcpy::Dimensions(2u,2u));
}

void read(pmemcpy::PMEM &pmem) {
    //Read simple type
    double a;
    pmem.load<double>(a_id, a);
    std::cout << "a: " << a << std::endl;
    std::cout << std::endl;

    //Read array type?
    double data[2][2];
    pmem.load<double>(b_id, (double*)data, pmemcpy::Dimensions(2u,2u));
    for(int i = 0; i < 2; ++i) {
        std::cout << "b: " << data[i][0] << std::endl;
        std::cout << "b: " << data[i][1] << std::endl;
    }
    std::cout << std::endl;
}

void reset(pmemcpy::PMEM &pmem) {
    pmem.free(a_id);
    //pmem.free(b_id);
    pmem.free(c_id);
}

int main(int argc, char **argv)
{
    pmemcpy::StorageType storage_t;
    pmemcpy::SerializerType serializer_t;
    if(argc != 5) {
        printf("USAGE: test_pmem [pool-path] [i/o mode] [storage_type] [serializer_type]");
        return -1;
    }
    char *path = argv[1];
    int mode = atoi(argv[2]);
    PMEMCPY_ERROR_HANDLE_START()
    storage_t = pmemcpy::StorageTypeConverter::convert(argv[3]);
    serializer_t = pmemcpy::SerializerTypeConverter::convert(argv[4]);
    PMEMCPY_ERROR_HANDLE_END()

    //Test create
    pmemcpy::PMEM pmem(storage_t, serializer_t);
    PMEMCPY_ERROR_HANDLE_START()
    if(mode == 0 && boost::filesystem::exists(path)) {
        pmem.release(path);
    }
    printf("EXISTS: %d\n", boost::filesystem::exists(path));
    pmem.mmap(path, 100*(1<<20));

    //Perform I/O
    switch(mode) {
        case 0: {
            write(pmem);
            break;
        }
        case 1: {
            read(pmem);
            break;
        }
        case 2: {
            reset(pmem);
            try {
                read(pmem);
                throw 1;
            } catch(...) {}
            break;
        }
    }
    PMEMCPY_ERROR_HANDLE_END()
}