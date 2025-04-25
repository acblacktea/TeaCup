#pragma once
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <vector>
template <typename T, bool ThreadSafe = true>
class RingQueue
{
public:
    explicit RingQueue(size_t size)
        : _capacity(size)
    {
        _queue = std::vector<T>(_capacity);
    }

    bool push(const T & value)
    {
        if constexpr (ThreadSafe)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            // std::cout << "push: " << value << std::endl;
            _notFullCV.wait(lock, [this]() { return _length < _capacity; });

            _queue[_tail] = value;
            _tail = (_tail + 1) % _capacity;
            ++_length;

            _notEmptyCV.notify_one();
            return true;
        }
        else
        {
            if (_length == _capacity)
            {
                return false;
            }
            _queue[_tail] = value;
            _tail = (_tail + 1) % _capacity;
            ++_length;
            return true;
        }
    }

    bool pop(T & value)
    {
        if constexpr (ThreadSafe)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            _notEmptyCV.wait(lock, [this]() { return _length > 0; });

            value = _queue[_head];
            _head = (_head + 1) % _capacity;
            --_length;

            _notFullCV.notify_one();
            return true;
        }
        else
        {
            if (_length == 0)
            {
                return false;
            }

            value = _queue[_head];
            _head = (_head + 1) % _capacity;
            --_length;
            return true;
        }
    };


    size_t len()
    {
        if constexpr (ThreadSafe)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _length;
        }
        else
        {
            return _length;
        }
    }

private:
    std::condition_variable _notFullCV;
    std::condition_variable _notEmptyCV;
    std::mutex _mutex;

    std::vector<T> _queue;
    int _head = 0;
    int _tail = 0;
    size_t _length = 0;
    size_t _capacity = 0;
};
