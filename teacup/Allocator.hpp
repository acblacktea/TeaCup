#pragma once
template <typename T>
class CustomAllocator
{
public:
    using value_type = T;
    T * allocate(size_t n)
    {
        std::cout << "allocate, size: " << n << ", memory size: " << n * sizeof(T) << std::endl;
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    void deallocate(T * p, size_t n)
    {
        std::cout << "deallocate, size: " << n << ", memory size: " << n * sizeof(T) << std::endl;
        ::operator delete(p);
    }

private:
};