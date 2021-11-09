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

    BufferPool* bufPool;

    // threadPool
    std::vector<Worker*> workers;
    sem_t idleWorkerNum;

    sem_t idleWorkerQMutex;
    std::queue<Worker*> idleWorkerQ;
    sem_t isWorking; // wait until all workers finished
    int activeWorkNum;

    // output queue
    sem_t outQMutex;
    std::priority_queue<pTask, std::vector<pTask>, Compare> outQ;
    int taskLastIdx;
    char tailChar;
    int tailCnt;

    TaskQueue(int nJob, int argc, char* argv[]);
    ~TaskQueue();

    // enqueue task to exectute
    void Enqueue(pTask t);
    // enqueue task to outputQueue and print anything if avaliable
    void Output(pTask t);
    void Flush();
};

class Worker
{
public:
    sem_t workerMutex;
    pTask nowTask;
    bool keepRunning;

    Worker(TaskQueue* taskQ);

    void ExecTask(pTask t);
    void Kill();

    pthread_t idx;
    TaskQueue* taskQ;
};


#endif