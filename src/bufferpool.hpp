#ifndef BUFFERPOOL_H // include guard
#define BUFFERPOOL_H

#include "task.hpp"
#include <queue>
#include <vector>
#include <unordered_map>
#include "semaphore.h"

class BufferPool
{
    sem_t mutex;
public:
    BufferPool(int nBuffer);
    ~BufferPool();

    void GetBuffer(pTask r);
    void FreeBuffer(pTask r);

    int nBuffer;
    std::vector<char*> pool;
    std::queue<int> freeIdx;
    std::unordered_map<int, int> task2buffer;
};


#endif