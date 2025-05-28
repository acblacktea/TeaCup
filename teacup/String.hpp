#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>

class String
{
private:
    static constexpr size_t SSO_CAPACITY = 16;
    union
    {
        struct
        {
            size_t capacity;
            char * data;
        } large;
        char small[SSO_CAPACITY];
    };

    bool isSmall() const { return size_ < SSO_CAPACITY; }

    void setNullTerminator(char * s, size_t size) { s[size] = '\0'; }

    size_t size_ = 0;

public:
    String() { }
    explicit String(const char * s)
        : size_(strlen(s))
    {
        if (isSmall())
        {
            std::copy_n(s, size_, small);
            setNullTerminator(small, size_);
        }
        else
        {
            large.capacity = size_;
            large.data = new char[size_ + 1];
            std::copy_n(s, size_, large.data);
            setNullTerminator(large.data, size_);
        }
    }

    String(String & s)
        : size_(s.size_)
    {
        if (isSmall())
        {
            std::copy_n(s.small, size_, small);
            setNullTerminator(small, size_);
        }
        else
        {
            large.capacity = size_;
            large.data = new char[size_ + 1];
            std::copy_n(s.large.data, size_, large.data);
            setNullTerminator(large.data, size_);
        }
    }
    String(String && s) noexcept
        : size_(s.size_)
    {
        if (isSmall())
        {
            std::copy_n(s.small, size_, small);
            setNullTerminator(small, size_);
        }
        else [[likely]]
        {
            large = s.large;
            s.large.capacity = 0;
            s.size_ = 0;
            s.large.data = nullptr;
        }
    }

    ~String()
    {
        if (!isSmall())
        {
            delete[] large.data;
        }
    }

    String & operator=(String & other)
    {
        if (this != &other)
        {
            auto isSmallFlag = isSmall();
            if (other.size_ < SSO_CAPACITY)
            {
                if (!isSmallFlag)
                {
                    delete[] large.data;
                    large.capacity = 0;
                }

                std::copy_n(other.small, other.size_, small);
                setNullTerminator(small, other.size_);
            }
            else
            {
                if (isSmallFlag)
                {
                    large.data = new char[other.size_ + 1];
                    large.capacity = other.size_;
                }
                else if (other.size_ > size_)
                {
                    delete large.data;
                    large.data = new char[other.size_ + 1];
                    large.capacity = other.size_;
                }

                std::copy_n(other.large.data, other.large.capacity, large.data);
                setNullTerminator(large.data, other.size_);
            }

            size_ = other.size_;
        }

        return *this;
    }

    String & operator=(String && other) noexcept
    {
        if (this != &other)
        {
            auto isSmallFlag = isSmall();
            if (other.size_ < SSO_CAPACITY)
            {
                size_ = other.size_;
                if (!isSmallFlag)
                {
                    delete[] large.data;
                    large.capacity = 0;
                }

                std::copy_n(other.small, other.size_, small);
                setNullTerminator(small, other.size_);
            }
            else
            {
                if (!isSmallFlag)
                {
                    delete[] large.data;
                }

                large = other.large;
                other.large.capacity = 0;
                other.large.data = nullptr;
                other.size_ = 0;
            }
        }

        return *this;
    }

    String & operator+=(const char * s)
    {
        auto appendSize = strlen(s);
        auto newSize = size_ + appendSize;
        if (newSize < SSO_CAPACITY)
        {
            std::copy_n(s, appendSize, small + size_);
            setNullTerminator(small, newSize);
        }
        else
        {
            auto isSmallFlag = isSmall();
            if (!isSmallFlag && newSize < large.capacity)
            {
                std::copy_n(s, appendSize, large.data + size_);
            }
            else
            {
                auto capacity = std::max(newSize + 1, size_ * 2);
                char * newData = new char[capacity];
                if (!isSmall())
                {
                    std::copy_n(large.data, size_, newData);
                    delete large.data;
                }
                else
                {
                    std::copy_n(small, size_, newData);
                }

                std::copy_n(s, appendSize, newData);
                large.capacity = capacity;
                large.data = newData;
            }

            setNullTerminator(large.data, newSize);
        }

        size_ = newSize;

        return *this;
    }

    String & operator+=(char c)
    {
        auto appendSize = 1;
        auto newSize = size_ + appendSize;
        if (newSize < SSO_CAPACITY)
        {
            std::copy_n(&c, appendSize, small + size_);
            setNullTerminator(small, newSize);
        }
        else
        {
            auto isSmallFlag = isSmall();
            if (!isSmallFlag && newSize < large.capacity)
            {
                std::copy_n(&c, appendSize, large.data + size_);
            }
            else
            {
                auto capacity = std::max(newSize + 1, size_ * 2);
                char * newData = new char[capacity];
                if (!isSmall())
                {
                    std::copy_n(large.data, size_, newData);
                    delete large.data;
                }
                else
                {
                    std::copy_n(small, size_, newData);
                }

                std::copy_n(&c, appendSize, newData);
                large.capacity = capacity;
                large.data = newData;
            }

            setNullTerminator(large.data, newSize);
        }

        size_ = newSize;

        return *this;
    }

    const char * cStr() { return isSmall() ? small : large.data; }
};