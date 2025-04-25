#include <algorithm>
#include <mutex>
#include <thread>
#include <gtest/gtest.h>
#include <RingQueue.hpp>
TEST(ringQueue, testSingleThread)
{
    RingQueue<int, false> queue(1000);
    for (int i = 0; i < 700; ++i)
    {
        queue.push(i);
    }

    for (int i = 0; i < 700; ++i)
    {
        int value;
        auto successful = queue.pop(value);
        GTEST_ASSERT_EQ(value, i);
        GTEST_ASSERT_EQ(successful, true);
    }

    for (int i = 0; i < 700; ++i)
    {
        queue.push(i);
    }

    for (int i = 0; i < 700; ++i)
    {
        int value;
        auto successful = queue.pop(value);
        GTEST_ASSERT_EQ(value, i);
        GTEST_ASSERT_EQ(successful, true);
    }

    for (int i = 0; i < 700; ++i)
    {
        queue.push(i);
    }

    for (int i = 0; i < 700; ++i)
    {
        int value;
        auto successful = queue.pop(value);
        GTEST_ASSERT_EQ(value, i);
        GTEST_ASSERT_EQ(successful, true);
    }
}


TEST(ringQueue, testMultiThread)
{
    std::cout << 111 << std::endl;
    RingQueue<int, true> queue(10000);
    std::vector<int> ans;
    std::mutex mutex;

    std::thread readThread(
        [&]()
        {
            for (int i = 0; i < 100000; ++i)
            {
                queue.push(i);
            }
        });

    int count = 0;


    std::thread writeThread(
        [&]()
        {
            for (int i = 0; i < 30000; ++i)
            {
                int value;
                auto successful = queue.pop(value);
                std::lock_guard<std::mutex> lock(mutex);
                ans.emplace_back(value);
            }
        });

    std::thread writeThread2(
        [&]()
        {
            for (int i = 0; i < 30000; ++i)
            {
                int value;
                auto successful = queue.pop(value);
                std::lock_guard<std::mutex> lock(mutex);
                ans.emplace_back(value);
            }
        });

    std::thread writeThread3(
        [&]()
        {
            for (int i = 0; i < 40000; ++i)
            {
                int value;
                auto successful = queue.pop(value);
                std::lock_guard<std::mutex> lock(mutex);
                ans.emplace_back(value);
            }
        });

    readThread.join();
    writeThread.join();
    writeThread2.join();
    writeThread3.join();

    std::sort(ans.begin(), ans.end());
    std::cout << "debug" << ans.size() << std::endl;
    for (int i = 0; i < 100000; ++i)
    {
        GTEST_ASSERT_EQ(ans[i], i);
    }
}