#ifndef BUFFERPOOL_H // include guard
#define BUFFERPOOL_H

#include "task.hpp"
#include "utils.hpp"
#include <queue>
#include <vector>
#include <unordered_map>
#include "semaphore.h"

class BufferPool
{
    sem_t mutex;
    sem_t bufferNum;
public:
    BufferPool(int nBuffer);
    ~BufferPool();

    void GetBuffer(Task* t);
    void FreeBuffer(Task* t);

    int nBuffer;
    std::vector<char*> pool;
    std::queue<int> freeIdx;
    std::unordered_map<int, int> task2buffer;
};


#endif