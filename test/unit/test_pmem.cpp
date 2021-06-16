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
    double data[5] = {10, 11, 12, 13, 14};
    pmem.store(b_id, data, pmemcpy::Dimensions(5u));

    //Write linked list
    /*std::list<double> c;
    c.push_back(1);
    c.push_back(2);
    c.push_back(3);
    pmem.store<std::list<double>>(c_id, c);*/
}

void read(pmemcpy::PMEM &pmem) {
    //Read simple type
    double a;
    pmem.load<double>(a_id, a);
    std::cout << "a: " << a << std::endl;
    std::cout << std::endl;

    //Read array type?
    double data[5];
    pmem.load(b_id, data, pmemcpy::Dimensions(5u));
    for(int i = 0; i < 5; ++i) {
        std::cout << "b: " << data[i] << std::endl;
    }
    std::cout << std::endl;

    //Read linked list
    /*std::list<double> c;
    pmem.load<std::list<double>>(c_id, c);
    for(double num : c) {
        std::cout << "c: " << num << std::endl;
    }
    std::cout << std::endl;*/
}

void reset(pmemcpy::PMEM &pmem) {
    pmem.free(a_id);
    //pmem.free(b_id);
    pmem.free(c_id);
}

int main(int argc, char **argv)
{
    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_LIST, pmemcpy::SerializerType::CAPNPROTO);
    if(argc != 3) {
        printf("USAGE: test_pmem [pool-path] [i/o mode]");
        return -1;
    }
    char *path = argv[1];
    int mode = atoi(argv[2]);

    //Test create
    if(mode == 0 && boost::filesystem::exists(path)) {
        pmem.release(path);
    }
    pmem.mmap(argv[1], 100*(1<<20));

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
}