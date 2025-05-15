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
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <gtest/gtest.h>


TEST(interviewTest, test1)
{
    std::unordered_map<int, int> my_map;

    my_map.max_load_factor(3.0); // Allow more elements per bucket
    my_map.reserve(300);
    std::cout << "Buckets: " << my_map.bucket_count() << ", Load factor: " << my_map.load_factor() << "\n";

    //my_map.max_load_factor(2.0);
    // Insert elements
    for (int i = 0; i < 100; i++)
    {
        my_map[i] = i;
        std::cout << "index: " << i + 1 << ", Buckets: " << my_map.bucket_count() << ", Load factor: " << my_map.load_factor() << ", "
                  << "\n";

        // Rehash happens automatically when:
        // size() / bucket_count() > max_load_factor()
    }

    std::cout << "After inserts - Buckets: " << my_map.bucket_count() << ", Load factor: " << my_map.load_factor() << "\n";

    // Manually control rehashing
    my_map.max_load_factor(2.0); // Allow more elements per bucket
    my_map.rehash(200);
}