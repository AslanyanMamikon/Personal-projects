#ifndef MemoryPool_H_
#define MemoryPool_H_

#include <iostream>
#include <vector>

template <typename T>
class MemoryPool {
private:
    struct Block {
        Block* next;
    };

    Block* freeList;
    std::vector<void*> pool;
    size_t blockSize;
    size_t poolSize;

public:
    MemoryPool(size_t size) noexcept
        : blockSize(sizeof(T)), poolSize(size), freeList(nullptr) {

        size_t SIZE = blockSize > sizeof(Block*) ? blockSize : sizeof(Block*);

        void* newBlock = malloc(SIZE * poolSize);
        if (!newBlock) {
            throw std::bad_alloc();
        }
        //pool.resize(blockSize * blockCount);
        pool.push_back(newBlock);

        for (size_t i = 0; i < poolSize; i++) {
            Block* block = reinterpret_cast<Block*>(static_cast<char*>(newBlock) + i * SIZE);
            block->next = freeList;
            freeList = block;
        }
    }
    ~MemoryPool() noexcept {
        for (auto ptr : pool) {
            free(ptr);
        }
    }

    T* allocate() {
        if (!freeList) {
            std::cerr << "Memory Pool exhausted!" << std::endl;
            return nullptr;
        }

        Block* block = freeList;
        freeList = freeList->next;
        return reinterpret_cast<T*> (block); // Block* tipi poxaren veradardzni T*
    }

    void deallocate(T* ptr) {
        if (!ptr) return;

        // Get the Block pointer from the data pointer
        /*Block* block = reinterpret_cast<Block*>(
            static_cast<char*>(ptr) - sizeof(Block)
            );*/

        Block* block = reinterpret_cast<Block*> (ptr);
        block->next = freeList;
        freeList = block;
    }

    template<typename... Args>
    void construct(T* ptr, Args&&... args) {
        new(ptr) T(std::forward<Args>(args)...);
        // new((void*)ptr) T(std::forward<Args>(args)...); // aveli apahov ?
    }

    void destroy(T* ptr) {
        ptr->~T();
    }

    template <typename... Args>
    T* newElement(Args&&... args) {
        T* memory = allocate();
        construct(memory, std::forward<Args>(args)...);
        return memory;
    }

    void deleteElement(T* ptr) {
        if (ptr) {
            destroy(ptr);
            deallocate(ptr);
        }
    }
};

#endif // !MemoryPool_H_
