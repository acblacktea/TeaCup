#pragma once
#include <chrono>
#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>
#include <vector>

inline long long getNowMilliseconds()
{
    auto now = std::chrono::system_clock::now();
    auto time_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    return time_ms.time_since_epoch().count();
}

class Task
{
public:
    Task(std::function<void()> _func, long long delayMS)
    {
        func = _func;
        executionStarsTime = getNowMilliseconds() + delayMS;
    }

    bool operator>(const Task & task) const { return executionStarsTime > task.executionStarsTime; }

    std::function<void()> func;
    long long executionStarsTime = 0;
};

class TaskScheduler
{
public:
    explicit TaskScheduler(int _parallelCount) noexcept
        : parallelCount(_parallelCount)
    {
    }
    void addTask(std::function<void()> _func, long long delayMS) noexcept
    {
        std::unique_lock<std::mutex> lock(mutex);
        taskQueue.emplace(_func, delayMS);
        cv.notify_one();
    }

    void run() noexcept
    {
        std::vector<std::thread> threads;
        for (int i = 0; i < parallelCount; ++i)
        {
            threads.emplace_back(&TaskScheduler::runPerThread, this);
        }

        for (auto & thread : threads)
        {
            thread.join();
        }
    }


    void runPerThread()
    {
        while (true)
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return taskQueue.size(); });
            auto task = taskQueue.top();
            auto nowMS = getNowMilliseconds();
            if (task.executionStarsTime <= nowMS)
            {
                task.func();
                taskQueue.pop();
            }
            else
            {
                cv.wait_until(lock, std::chrono::system_clock::time_point(std::chrono::milliseconds(task.executionStarsTime)));
            }
        }
    }

private:
    std::priority_queue<Task, std::vector<Task>, std::greater<Task>> taskQueue;
    std::condition_variable cv;
    std::mutex mutex;
    int parallelCount = 1;
};