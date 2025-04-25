#pragma once
#include <gtest/gtest.h>
auto sum = [](int a, int b) -> int { return a + b; };

TEST(example3, add3)
{
    GTEST_ASSERT_EQ(sum(10, 33), 43);
    std::cout << "sum(10, 22) = " << sum(10, 43) << std::endl;
}