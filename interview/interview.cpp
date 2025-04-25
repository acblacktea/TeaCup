#include <algorithm>
#include <condition_variable>
#include <list>
#include <map>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <gtest/gtest.h>

class Solution
{
public:
    int countLargestGroup(int n)
    {
        auto countVe = std::vector<int>(100, 0);
        auto maxDigit = 0;
        for (int i = 1; i <= n; ++i)
        {
            auto oneDigitCount = 0;
            auto num = i;
            while (num)
            {
                oneDigitCount += num % 10;
                num /= 10;
            }

            ++countVe[oneDigitCount];
            maxDigit = std::max(maxDigit, countVe[oneDigitCount]);
        }

        auto ans = 0;
        for (int i = 0; i < 100; ++i)
        {
            if (countVe[i] == maxDigit)
            {
                ++ans;
            }
        }

        return ans;
    }
};

TEST(interviewTest, test1)
{
    std::cout << "cpu core count" << std::thread::hardware_concurrency() << std::endl;
}