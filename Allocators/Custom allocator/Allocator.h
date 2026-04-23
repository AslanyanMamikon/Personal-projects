#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <iostream>

template <typename T>
class Allocator {
public:

    template <typename U>
    struct rebind {
        using other = Allocator<U>;
    };

    Allocator() noexcept;
    ~Allocator();
    Allocator(const Allocator&) noexcept;
    template <typename U>
    Allocator(const Allocator<U>&) noexcept;

    T* allocate(std::size_t n);
    void deallocate(T* p, std::size_t);

    template<typename... Args>
    void construct(T* p, Args&&... args);

    void destroy(T* p);  
};
#endif // !ALLOC_H_
