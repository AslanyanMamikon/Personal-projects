#ifndef POOL_ALLOCATOR_H
#define POOL_ALLOCATOR_H

#include "Memory_pool.h"
#include <cstddef>
#include <new>
#include <utility>

template <typename T>
class PoolAllocator {
private:
    MemoryPool<T>* pool;

    template <typename U>
    friend class PoolAllocator;

public:
    using value_type = T;

    /*template <typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };*/

    explicit PoolAllocator(MemoryPool<T>* pool) noexcept;

    template <typename U>
    PoolAllocator(const PoolAllocator<U>& other) noexcept;

    T* allocate(std::size_t n);

    void deallocate(T* p, std::size_t n);

    template <typename... Args>
    void construct(T* p, Args&&... args);

    void destroy(T* p);

    MemoryPool<T>* get_pool() const noexcept;

    /*template <typename U>
    bool operator==(const PoolAllocator<U>& other) const noexcept;

    template <typename U>
    bool operator!=(const PoolAllocator<U>& other) const noexcept;*/
};

#endif // POOL_ALLOCATOR_H

template <typename T>
PoolAllocator<T>::PoolAllocator(MemoryPool<T>* pool) noexcept : pool(pool) {}

template <typename T>
template <typename U>
PoolAllocator<T>::PoolAllocator(const PoolAllocator<U>& other) noexcept
    : pool(reinterpret_cast<MemoryPool<T>*>(other.get_pool())) {
}

template <typename T>
T* PoolAllocator<T>::allocate(std::size_t n) {
    if (n != 1) throw std::bad_alloc(); // this pool only allocates one block at a time
    T* ptr = pool->allocate();
    if (!ptr) throw std::bad_alloc();
    return ptr;
}

template <typename T>
void PoolAllocator<T>::deallocate(T* p, std::size_t n) {
    if (n == 1 && p) pool->deallocate(p);
}

template <typename T>
template <typename... Args>
void PoolAllocator<T>::construct(T* p, Args&&... args) {
    pool->construct(p, std::forward<Args>(args)...);
}

template <typename T>
void PoolAllocator<T>::destroy(T* p) {
    pool->destroy(p);
}

template <typename T>
MemoryPool<T>* PoolAllocator<T>::get_pool() const noexcept {
    return pool;
}

//template <typename T>
//template <typename U>
//bool PoolAllocator<T>::operator==(const PoolAllocator<U>& other) const noexcept {
//    return pool == other.get_pool();
//}
//
//template <typename T>
//template <typename U>
//bool PoolAllocator<T>::operator!=(const PoolAllocator<U>& other) const noexcept {
//    return !(*this == other);
//}