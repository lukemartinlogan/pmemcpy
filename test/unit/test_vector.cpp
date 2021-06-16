//
// Created by lukemartinlogan on 6/9/21.
//

#include <pmemcpy/memcpy.h>

int main() {
    int test[5] = {1, 2, 3, 4, 5};
    std::vector<int, pmemcpy::NoAllocator<int>>  temp_(pmemcpy::NoAllocator<int>(test, 5));
    temp_.resize(5);
    printf("%p vs %p\n", test, (temp_).data());
    for(int i = 0; i < 5; ++i) {
        printf("%d ", (temp_)[i]);
    }
    printf("\n");
}