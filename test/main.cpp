#include <algorithm>
#include <gtest/gtest.h>
#include <CacheStrategy.hpp>
#include <FileReader.hpp>
#include <MMapQueue.hpp>
#include <RingQueue.hpp>
#include <SharedPtr.hpp>
#include <TaskScheduler.hpp>
#include <ThreadPool.hpp>
int main(int argc, char ** argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}