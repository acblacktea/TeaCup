#include <gtest/gtest.h>
#include <SharedPtr.hpp>

struct PackedButAlignedMember
{
    char a;
    alignas(8) int b[3];
    int c;
};

struct alignas(16) PackedButAlignedMember2
{
    int a[5];
};

TEST(SharedPtrTest, test1)
{
    std::cout << sizeof(PackedButAlignedMember2) << std::endl;

    auto value = SharedPtr<int>(new int(5));
    GTEST_ASSERT_EQ(*value, 5);
    GTEST_ASSERT_EQ(value.getCount(), 1);


    {
        auto value2 = value;
        GTEST_ASSERT_EQ(*value, 5);
        GTEST_ASSERT_EQ(value.getCount(), 2);
        GTEST_ASSERT_EQ(*value2, 5);
        GTEST_ASSERT_EQ(value2.getCount(), 2);

        auto value3 = value;
        GTEST_ASSERT_EQ(*value, 5);
        GTEST_ASSERT_EQ(value.getCount(), 3);
        GTEST_ASSERT_EQ(*value2, 5);
        GTEST_ASSERT_EQ(value2.getCount(), 3);
        GTEST_ASSERT_EQ(*value3, 5);
        GTEST_ASSERT_EQ(value3.getCount(), 3);

        *value3 = 10;
        GTEST_ASSERT_EQ(*value, 10);
        GTEST_ASSERT_EQ(value.getCount(), 3);
        GTEST_ASSERT_EQ(*value2, 10);
        GTEST_ASSERT_EQ(value2.getCount(), 3);
        GTEST_ASSERT_EQ(*value3, 10);
        GTEST_ASSERT_EQ(value3.getCount(), 3);
    }


    GTEST_ASSERT_EQ(*value, 10);
    GTEST_ASSERT_EQ(value.getCount(), 1);
}