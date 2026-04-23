
#include "Allocator.h"

template <typename T>
Allocator<T>::Allocator() noexcept {}

template <typename T>
Allocator<T>::~Allocator() {}

template <typename T>
Allocator<T>::Allocator(const Allocator&) noexcept {}

template <typename T>
template <typename U>
Allocator<T>::Allocator(const Allocator<U>&) noexcept {}

template <typename T>
T* Allocator<T>::allocate(std::size_t n) {
    return static_cast<T*>(::operator new(n * sizeof(T)));
}

template <typename T>
void Allocator<T>::deallocate(T* p, std::size_t) {
    ::operator delete(p);
}

template <typename T>
template<typename... Args>
void Allocator<T>::construct(T* p, Args&&... args) {
    new(p) T(std::forward<Args>(args)...);
}

template <typename T>
void Allocator<T>::destroy(T* p) {
    p->~T();
}

int main() {
    Allocator<int> intAllocator;
    int* ptr = intAllocator.allocate(5);
    
    for(int i = 0; i < 5; i++) {
        intAllocator.construct(ptr + i, i);
    }

    Allocator<int>::rebind<double>::other doubleALL;
    for(int i = 0; i < 5; i++) {
        std::cout << ptr[i] << " ";
    }
    for(int i = 0; i < 5; i++) {
        intAllocator.destroy(ptr + i);
    }
    intAllocator.deallocate(ptr, 5);
    
    return 0;
}
