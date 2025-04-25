#pragma once
#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    int cnt = 0;
    ThreadPool(size_t threadCount = std::thread::hardware_concurrency())
    {
        isRunning = true;
        for (int i = 0; i < threadCount; ++i)
        {
            workers.emplace_back(
                [this]()
                {
                    while (true)
                    {
                        std::unique_lock<std::mutex> lock(queueMutex);
                        condition.wait(lock, [this] { return !this->isRunning || !this->tasks.empty(); });
                        if (!this->isRunning && this->tasks.empty())
                        {
                            return;
                        }

                        auto task = this->tasks.front();
                        this->tasks.pop();
                        lock.unlock();
                        task();
                    }
                });
        }
    }

    template <class F, class... Args>
    std::future<std::invoke_result_t<F, Args...>> addTask(F && func, Args &&... args)
    {
        using returnType = std::invoke_result_t<F, Args...>;

        auto task = std::make_shared<std::packaged_task<returnType()>>(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        auto future = task->get_future();

        std::unique_lock<std::mutex> lock(queueMutex);

        if (!isRunning)
        {
            throw std::runtime_error("pool already stop");
        }

        tasks.push([task]() { (*task)(); });

        lock.unlock();
        condition.notify_one();
        return future;
    }

    ~ThreadPool()
    {
        queueMutex.lock();
        isRunning = false;
        queueMutex.unlock();
        condition.notify_all();

        for (auto & worker : workers)
        {
            worker.join();
        }
    }

private:
    bool isRunning = false;
    std::queue<std::function<void()>> tasks;
    std::vector<std::thread> workers;

    std::mutex queueMutex;
    std::condition_variable condition;
};