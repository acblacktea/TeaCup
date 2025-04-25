#pragma once
#include <atomic>
#include <iostream>
#include <map>
#include <memory>
class SharedCount
{
public:
    SharedCount() noexcept { };

    void addCount() noexcept { ++count; }
    void reduceCount() noexcept { --count; }
    long getCount() const noexcept { return count; }

private:
    std::atomic<int> count = 1;
};

template <class T>
class SharedPtr
{
public:
    SharedPtr() = delete;
    SharedPtr(T * ptr)
        : _ptr(ptr)
    {
        if (_ptr)
        {
            _sharedCount = new SharedCount();
        }
    };

    ~SharedPtr()
    {
        if (_sharedCount)
        {
            _sharedCount->reduceCount();
            if (_sharedCount->getCount() == 0)
            {
                delete _sharedCount;
                delete _ptr;
            }
        }
    }

    SharedPtr(SharedPtr & sharedPtr) noexcept
    {
        _ptr = sharedPtr._ptr;
        if (_ptr)
        {
            sharedPtr._sharedCount->addCount();
            this->_sharedCount = sharedPtr._sharedCount;
        }
    };

    SharedPtr(SharedPtr && sharedPtr) noexcept
    {
        _ptr = sharedPtr->ptr;
        if (_ptr)
        {
            this->_sharedCount = sharedPtr->_sharedCount;
            sharedPtr->_ptr = nullptr;
        }
    };

    SharedPtr & operator=(SharedPtr sharedPtr) noexcept
    {
        sharedPtr.Swap(*this);
        return *this;
    };

    void Swap(SharedPtr & sharedPtr) noexcept
    {
        swap(this->sharedCount, sharedPtr.sharedCount);
        swap(this->ptr, sharedPtr.ptr);
    };

    T & operator*() const noexcept { return *_ptr; };
    T * operator->() const noexcept { return _ptr; };
    int getCount() const noexcept { return _sharedCount->getCount(); };

private:
    T * _ptr = nullptr;
    SharedCount * _sharedCount = nullptr;
};