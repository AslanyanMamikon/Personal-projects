#include "Pool_Allocator.h"
#include <vector>
#include <iostream>

int main() {
    constexpr size_t POOL_SIZE = 10;

    // Create a memory pool for `int`
    MemoryPool<int> pool(POOL_SIZE);
    PoolAllocator<int> allocator(&pool);

    // Allocate and construct
    int* values[5];
    for (int i = 0; i < 5; ++i) {
        values[i] = allocator.allocate(1);
        allocator.construct(values[i], i * 2); // store even numbers
    }

    for (int i = 0; i < 5; ++i) {
        std::cout << *values[i] << " " << values[i] << "  ";
    }
    std::cout << std::endl;

    // Destroy and deallocate
    for (int i = 0; i < 5; ++i) {
        allocator.destroy(values[i]);
        allocator.deallocate(values[i], 1);
    }

    return 0;
}
