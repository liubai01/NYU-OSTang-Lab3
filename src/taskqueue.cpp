#include <stdio.h>
#include <stdlib.h>

#include "taskqueue.hpp"

void *worker(void *vargp)
{
    Worker* w = (Worker *) vargp;

    while (1)
    {
        // wait until taskQueue inform
        sem_wait(&w->workerMutex);
        if(!w->keepRunning)
        {
            break;
        }

        execTask(w->taskQ->argc, w->taskQ->argv, w->nowTask);
        printf("task %d finished: %s\n", w->nowTask->taskIdx, w->nowTask->buffer);
        // output TBD
        w->taskQ->bufPool->FreeBuffer(w->nowTask);
        delete w->nowTask;

        // idle and release the worker
        sem_wait(&w->taskQ->idleWorkerQMutex);
        w->taskQ->idleWorkerQ.push(w);
        if (--w->taskQ->activeWorkNum == 0)
        {
            sem_post(&w->taskQ->isWorking);
        }
        sem_post(&w->taskQ->idleWorkerQMutex);

        // idleWorkNum to unblock the taskQueue
        sem_post(&w->taskQ->idleWorkerNum);
    }
}

Worker::Worker()
{
    sem_init(&workerMutex, 0, 0);
    keepRunning = true;
}

void Worker::AssignTask(pTask t)
{
    nowTask = t;
    sem_post(&workerMutex);
}

TaskQueue::TaskQueue(int nJob, int argc, char* argv[])
{
	this->argc = argc;
	this->argv = argv;
    this->nJob = nJob;

    taskLastIdx = -1;
    tailCnt = 0;
    activeWorkNum = 0;

    bufPool = new BufferPool(nJob);

    sem_init(&idleWorkerNum, 0, nJob);
    sem_init(&isWorking, 0, 1);
    sem_init(&idleWorkerQMutex, 0, 1);

    for (int i = 0; i < nJob; ++i)
    {
        Worker* w = new Worker();
        w->taskQ = this;
        this->workers.push_back(w);
        this->idleWorkerQ.push(w);

        pthread_create(&w->idx, NULL, worker, (void *) w);
    }
    
    
}

TaskQueue::~TaskQueue()
{
    for (int i = 0; i < nJob; ++i)
    {
        workers[i]->keepRunning = false;
        sem_post(&workers[i]->workerMutex);
        pthread_join(workers[i]->idx, NULL);
        delete workers[i];
    }
    delete bufPool;
}


void TaskQueue::enqueue(pTask t)
{
    sem_wait(&idleWorkerNum);

    sem_wait(&idleWorkerQMutex);
    Worker* w = this->idleWorkerQ.front();
    this->idleWorkerQ.pop();
    if (++activeWorkNum == 1)
    {
        sem_wait(&isWorking);
    }
    sem_post(&idleWorkerQMutex);

    bufPool->GetBuffer(t);
    w->AssignTask(t);
}

void TaskQueue::flush()
{
    sem_wait(&isWorking);
    sem_post(&isWorking);
}