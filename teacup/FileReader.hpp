#include <algorithm>
#include <fstream>
#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <gtest/gtest.h>

class FileReader
{
public:
    FileReader(std::string & filePath, int threadCount)
    {
        if (threadCount == 0)
        {
            throw std::runtime_error("thread count cannot be zero");
        }

        auto file = std::ifstream(filePath, std::ios::binary);
        if (file.fail())
        {
            throw std::runtime_error("open file failed");
        }

        _filePath = filePath;
        _threadCount = threadCount;
        file.seekg(0, std::ios::end);
        _fileSize = file.tellg();
    }

    std::string read()
    {
        if (_fileSize == 0)
        {
            return "";
        }

        auto chunkSize = _fileSize / _threadCount;
        auto threads = std::vector<std::thread>();
        auto index = 0;
        auto chunks = std::vector<std::pair<int, std::string>>();
        for (int i = 0; i < _threadCount; ++i)
        {
            auto currentChunkSize = chunkSize + (i < _fileSize % _threadCount);
            threads.emplace_back(&FileReader::readChunk, this, index, currentChunkSize, std::ref(chunks));
            index += currentChunkSize;
        }

        for (auto & thread : threads)
        {
            thread.join();
        }

        std::sort(chunks.begin(), chunks.end());
        std::string ans;
        for (auto & chunk : chunks)
        {
            ans += chunk.second;
        }

        return ans;
    }

private:
    void readChunk(int startIndex, int size, std::vector<std::pair<int, std::string>> & chunks)
    {
        auto file = std::ifstream(_filePath);
        if (file.fail())
        {
            throw std::runtime_error("open file failed");
        }

        char * chars = new char[size];
        file.seekg(startIndex);
        file.read(chars, size);


        {
            std::lock_guard<std::mutex> lock(_mutex);
            chunks.emplace_back(startIndex, chars);
        }
    }


    int _threadCount = 0;
    int _fileSize = 0;
    std::string _filePath;
    std::vector<std::tuple<std::string, int, int>> chunckVector; // start, end
    std::mutex _mutex;
};


TEST(FileReader, test1)
{
    std::string fileName = "./test1.txt";
    FileReader f = FileReader(fileName, 7);

    std::cout << "start test" << std::endl;
    std::cout << f.read() << std::endl;
}