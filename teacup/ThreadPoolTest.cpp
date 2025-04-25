#include <algorithm>
#include <future>
#include <mutex>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include <ThreadPool.hpp>

TEST(ThreadPoolTest, test1)
{
    std::vector<std::future<int>> ve;

    auto func = [](int i) { return i; };


    ThreadPool pool;
    for (int i = 0; i < 1000; ++i)
    {
        ve.emplace_back(pool.addTask([](int i) { return i; }, i));
    }

    std::vector<int> ans;
    for (int i = 0; i < 1000; ++i)
    {
        ans.emplace_back(ve[i].get());
    }

    std::sort(ans.begin(), ans.end());
    for (int i = 0; i < 1000; ++i)
    {
        GTEST_ASSERT_EQ(ans[i], i);
    }
}