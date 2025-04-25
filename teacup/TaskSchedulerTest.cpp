#include <algorithm>
#include <mutex>
#include <thread>
#include <vector>
#include <gtest/gtest.h>
#include <TaskScheduler.hpp>
TEST(TaskSchedulerTest, test1)
{
    std::vector<int> ans;
    TaskScheduler scheduler(10);
    std::mutex mutex;
    auto addTasks = std::thread(
        [&]()
        {
            for (int i = 0; i < 1000; ++i)
            {
                scheduler.addTask(
                    [&mutex, &ans, i]()
                    {
                        std::lock_guard<std::mutex> lock(mutex);
                        ans.emplace_back(i);
                    },
                    i * 10);
            }
        });

    addTasks.detach();

    auto execuateTasks = std::thread([&]() { scheduler.run(); });

    execuateTasks.detach();
    std::this_thread::sleep_for(std::chrono::seconds(15));

    std::sort(ans.begin(), ans.end());
    GTEST_ASSERT_EQ(ans.size(), 1000);
    for (int i = 0; i < 1000; ++i)
    {
        GTEST_ASSERT_EQ(ans[i], i);
    }
}