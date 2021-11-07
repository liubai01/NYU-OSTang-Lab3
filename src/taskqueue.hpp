#ifndef TASKQUEUE_H // include guard
#define TASKQUEUE_H

#include "task.hpp"
#include "bufferpool.hpp"
#include <queue>
#include <vector>
#include "semaphore.h"

class Compare
{
public:
    bool operator() (pTask a, pTask b)
    {
        return a->taskIdx > b->taskIdx;
    }
};

class Worker;

class TaskQueue
{
public:
    int argc;
    char** argv;
    int nJob;

    int taskLastIdx;
    char tailChar;
    int tailCnt;

    BufferPool* bufPool;

    // threadPool
    std::vector<Worker*> workers;
    sem_t idleWorkerNum;

    sem_t idleWorkerQMutex;
    std::queue<Worker*> idleWorkerQ;
    sem_t isWorking; // wait until all workers finished
    int activeWorkNum;

    // priority queue for output
    std::priority_queue<pTask, std::vector<pTask>, Compare> outQ;

    TaskQueue(int nJob, int argc, char* argv[]);
    ~TaskQueue();

    void enqueue(pTask t);
    void flush();
};

class Worker
{
public:
    sem_t workerMutex;
    pTask nowTask;
    bool keepRunning;

    Worker();
    void AssignTask(pTask t);

    pthread_t idx;
    TaskQueue* taskQ;
};


#endif