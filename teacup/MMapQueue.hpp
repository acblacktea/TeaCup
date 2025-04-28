// mmap_queue.hpp
#pragma once
#include <atomic>
#include <cstring>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

/*
template <typename T, size_t N>
class MMapQueue
{
public:
    MMapQueue(const std::string & name, bool create = false)
        : name_(name)
        , size_(N * sizeof(T) + 2 * sizeof(std::atomic<uint64_t>))
    {
        int flags = create ? (O_RDWR | O_CREAT) : O_RDWR;
        fd_ = shm_open(name.c_str(), flags, 0666);
        if (fd_ == -1)
            throw std::runtime_error("shm_open failed");

        if (create && ftruncate(fd_, size_) == -1)
            throw std::runtime_error("ftruncate failed");

        void * addr = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
        if (addr == MAP_FAILED)
            throw std::runtime_error("mmap failed");

        buffer_ = reinterpret_cast<T *>(addr);
        head_ = reinterpret_cast<std::atomic<uint64_t> *>(buffer_ + N);
        tail_ = head_ + 1;

        if (create)
        {
            head_->store(0);
            tail_->store(0);
        }
    }

    ~MMapQueue()
    {
        munmap(buffer_, size_);
        close(fd_);
    }

    bool push(const T & item)
    {
        uint64_t head = head_->load(std::memory_order_relaxed);
        uint64_t tail = tail_->load(std::memory_order_acquire);

        if ((head + 1) % N == tail)
            return false; // Queue full

        buffer_[head] = item;
        head_->store((head + 1) % N, std::memory_order_release);
        return true;
    }

    bool pop(T & item)
    {
        uint64_t tail = tail_->load(std::memory_order_relaxed);
        uint64_t head = head_->load(std::memory_order_acquire);

        if (tail == head)
            return false; // Queue empty

        item = buffer_[tail];
        tail_->store((tail + 1) % N, std::memory_order_release);
        return true;
    }

private:
    int fd_;
    std::string name_;
    size_t size_;
    T * buffer_;
    std::atomic<uint64_t> * head_;
    std::atomic<uint64_t> * tail_;
};

*/

template <typename T, size_t N>
class MMapQueue
{
public:
    MMapQueue(std::string && path)
        : fileTotalSize(N * sizeof(T) + 2 * sizeof(std::atomic<int>))
    {
        auto fileExisted = pathExists(path);
        if (!fileExisted)
        {
            fd_ = open(path.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
            ftruncate(fd_, fileTotalSize);
        }
        else
        {
            fd_ = open(path.c_str(), O_RDWR);
        }

        // Memory map the file
        void * addr = mmap(nullptr, fileTotalSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
        if (addr == MAP_FAILED)
        {
            throw std::runtime_error("mmap failed");
        }


        data = reinterpret_cast<T *>(addr);
        head = reinterpret_cast<std::atomic<int> *>(data + N);
        tail = head + 1;

        if (!fileExisted)
        {
            // Initialize header
            head = new std::atomic<int>();
            head->store(0);
            tail = new std::atomic<int>();
            tail->store(0);
        }
    }

    ~MMapQueue()
    {
        munmap(data, fileTotalSize);
        close(fd_);
    }

    bool push(const T & item)
    {
        auto headValue = head->load(std::memory_order_relaxed);
        auto tailValue = tail->load(std::memory_order_acquire);

        if ((headValue + 1) % N == tailValue)
            return false; // Queue full

        data[headValue] = item;
        head->store((headValue + 1) % N, std::memory_order_release);
        return true;
    }

    bool pop(T & item)
    {
        auto tailValue = tail->load(std::memory_order_relaxed);
        auto headValue = head->load(std::memory_order_acquire);

        //std::cout << tailValue << " " << headValue << std::endl;
        if (tailValue == headValue)
            return false; // Queue empty

        item = data[tailValue];
        tail->store((tailValue + 1) % N, std::memory_order_release);
        return true;
    }

private:
    bool pathExists(const std::string & path) { return std::filesystem::exists(path); }

    int fd_;
    size_t fileTotalSize;
    std::atomic<int> * head; // read position
    std::atomic<int> * tail; // write position
    T * data;
};