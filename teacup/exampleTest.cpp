#include <gtest/gtest.h>

#include <example.hpp>

TEST(example2, add2)
{
    GTEST_ASSERT_EQ(sum(10, 33), 43);
    std::cout << "sum(10, 22) = " << sum(10, 43) << std::endl;
}