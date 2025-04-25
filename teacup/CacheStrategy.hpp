#include <algorithm>
#include <condition_variable>
#include <list>
#include <map>
#include <mutex>
#include <optional>
#include <queue>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>

template <typename T>
concept CanHash = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};


template <typename T1, typename T2>
class CacheStrategy
{
public:
    explicit CacheStrategy(int capacity)
        : capacity_(capacity)
    {
    }

protected:
    size_t capacity_;
};


template <typename T1, typename T2>
class LRU : public CacheStrategy<T1, T2>
{
public:
    explicit LRU(int capacity)
        : CacheStrategy<T1, T2>(capacity)
    {
    }

    std::optional<T2> get(T1 & key)
    {
        if (cacheNodeUnorderedMap.contains(key))
        {
            auto iter = cacheNodeUnorderedMap[key];
            auto ans = (*iter).second;
            auto key = (*iter).first;
            cacheList.erase(iter);

            insertValue(key, ans);
            return ans;
        }

        return std::nullopt;
    }

    void put(T1 & key, T2 & value)
    {
        if (cacheNodeUnorderedMap.contains(key))
        {
            auto iter = cacheNodeUnorderedMap[key];
            cacheList.erase(iter);
        }
        else if (cacheList.size() == this->capacity_)
        {
            auto endNodeKey = (*cacheList.rbegin()).first;
            cacheNodeUnorderedMap.erase(endNodeKey);
            cacheList.pop_back();
        }

        insertValue(key, value);
    }

private:
    void insertValue(T1 & key, T2 & value)
    {
        cacheList.push_front({key, value});
        cacheNodeUnorderedMap[key] = cacheList.begin();
    }

    using iterator = std::list<std::pair<T1, T2>>::iterator;
    std::unordered_map<T1, iterator> cacheNodeUnorderedMap;
    std::list<std::pair<T1, T2>> cacheList;
};


template <typename T1, typename T2>
class LFU : public CacheStrategy<T1, T2>
{
public:
    explicit LFU(int capacity)
        : CacheStrategy<T1, T2>(capacity)
    {
    }

    std::optional<T2> get(T1 & key)
    {
        if (keyWithFrequencyAndCache.contains(key))
        {
            //std::cout << key << "&&&&" <<  miniumFrequency << std::endl;
            auto [frequency, iter] = keyWithFrequencyAndCache[key];
            auto result = (*iter).second;

            frequencyWithCache[frequency].erase(iter);
            insert(frequency + 1, key, result);
            if (frequency == miniumFrequency && frequencyWithCache[miniumFrequency].empty())
            {
                ++miniumFrequency;
            }

            return result;
        }

        return std::nullopt;
    }

    void put(T1 & key, T2 & value)
    {
        if (keyWithFrequencyAndCache.contains(key))
        {
            auto [frequency, iter] = keyWithFrequencyAndCache[key];
            frequencyWithCache[frequency].erase(iter);
            insert(frequency + 1, key, value);
            if (frequency == miniumFrequency && frequencyWithCache[miniumFrequency].empty())
            {
                ++miniumFrequency;
            }
        }
        else
        {
            if (size == this->capacity_)
            {
                keyWithFrequencyAndCache.erase((*frequencyWithCache[miniumFrequency].rbegin()).first);
                frequencyWithCache[miniumFrequency].pop_back();
                --size;
            }

            insert(1, key, value);
            miniumFrequency = 1;
            ++size;
        }
    }

private:
    void insert(size_t frequency, auto & key, auto & value)
    {
        frequencyWithCache[frequency].push_front({key, value});
        keyWithFrequencyAndCache[key] = {frequency, frequencyWithCache[frequency].begin()};
    }

    using iterator = std::list<std::pair<T1, T2>>::iterator;
    std::unordered_map<size_t, std::list<std::pair<T1, T2>>> frequencyWithCache;
    std::unordered_map<T1, std::pair<size_t, iterator>> keyWithFrequencyAndCache;
    int miniumFrequency = 0;
    int size = 0;
};