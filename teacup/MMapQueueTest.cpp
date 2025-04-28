#include <vector>
#include <unistd.h>
#include <gtest/gtest.h>
#include <sys/wait.h>
#include <MMapQueue.hpp>
struct testBlock
{
    int a;
    char b;
    double c;
    int value;
};


TEST(mmapQueue, test1)
{
    auto dataSize = 100000000;
    // create mmap;
    MMapQueue<testBlock, 2048> mpQueue("./mmapQueue.txt");
    auto process1PID = fork();
    if (process1PID == 0)
    {
        MMapQueue<testBlock, 2048> mpQueue("./mmapQueue.txt");
        for (int i = 0; i < dataSize; ++i)
        {
            testBlock data;
            data.value = i;
            while (!mpQueue.push(data))
            {
            }
        }
        return;
    }

    auto process2PID = fork();
    if (process2PID == 0)
    {
        std::vector<int> ans;
        MMapQueue<testBlock, 2048> mpQueue("./mmapQueue.txt");
        for (int i = 0; i < dataSize; ++i)
        {
            testBlock data;
            while (!mpQueue.pop(data))
            {
            }

            ans.emplace_back(data.value);
        }

        //std::cout << "size  " << ans.size() << std::endl;
        for (int i = 0; i < dataSize; ++i)
        {
            //std::cout << ans[i] << std::endl;
            GTEST_ASSERT_EQ(ans[i], i);
        }

        return;
    }

    waitpid(process1PID, nullptr, 0);
    waitpid(process2PID, nullptr, 0);
}