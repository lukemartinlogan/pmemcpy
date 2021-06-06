//
// Created by lukemartinlogan on 5/28/21.
//

#include <iostream>
#include <filesystem>
#include <list>
#include <pmemcpy/memcpy.h>
#include "test_pmem.h"

std::string a_id = "iweljlj;a";
std::string b_id = "asdfklasdfafksdkf";
std::string c_id = "i8w49a";

void write(pmemcpy::PMEM &pmem) {
    //Write simple type
    double a = 25;
    pmem.store<double>(a_id, &a);

    //Write array type?
    char data[150];

    //Write linked list
    std::list<double> c;
    c.push_back(1);
    c.push_back(2);
    c.push_back(3);
    pmem.store<std::list<double>>(c_id, &c);
}

void read(pmemcpy::PMEM &pmem) {
    //Read simple type
    double a;
    pmem.load<double>(a_id, &a);
    std::cout << "a: " << a << std::endl;

    //Read array type?
    //char data[150];

    //Read linked list
    std::list<double> c;
    pmem.load<std::list<double>>(c_id, &c);
    for(double num : c) {
        std::cout << "c: " << num << std::endl;
    }
}

void reset(pmemcpy::PMEM &pmem) {
    pmem.free(a_id);
    //pmem.free(b_id);
    pmem.free(c_id);
}

int main(int argc, char **argv)
{
    pmemcpy::PMEM pmem(pmemcpy::StorageType::PMDK_LIST, pmemcpy::SerializerType::CEREAL);
    if(argc != 3) {
        printf("USAGE: test_pmem [pool-path] [i/o mode]");
        return -1;
    }
    char *path = argv[1];
    int mode = atoi(argv[2]);

    //Test create
    if(mode == 0 && std::filesystem::exists(path)) {
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