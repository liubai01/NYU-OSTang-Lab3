#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "taskqueue.hpp"

// work thread function
void *worker(void *vargp)
{
    Worker* w = (Worker *) vargp;

    while (1)
    {
        // 1. wait until taskQueue inform
        sem_wait(&w->workerMutex);
        if(!w->keepRunning)
        {
            break;
        }

        // 2. execuate and output task
        w->nowTask->Exec();
        w->taskQ->Output(w->nowTask);

        // 3. idle and release the worker
        sem_wait(&w->taskQ->idleWorkerQMutex);
        w->taskQ->idleWorkerQ.push(w);
        if (--w->taskQ->activeWorkNum == 0)
        {
            sem_post(&w->taskQ->isWorking);
        }
        sem_post(&w->taskQ->idleWorkerQMutex);

        // 4. idleWorkNum to unblock the taskQueue
        sem_post(&w->taskQ->idleWorkerNum);
    }

    return NULL;
}

Worker::Worker(TaskQueue* taskQ)
{
    sem_init(&workerMutex, 0, 0);
    keepRunning = true;
    this->taskQ = taskQ;

    pthread_create(&idx, NULL, worker, (void *) this);
}

void Worker::ExecTask(Task* t)
{
    nowTask = t;
    taskQ->bufPool->GetBuffer(t);
    sem_post(&workerMutex); // unfreeze the task thread
}

void Worker::Kill()
{
    this->keepRunning = false;
    sem_post(&this->workerMutex);
    pthread_join(this->idx, NULL);
}


TaskQueue::TaskQueue(int nJob)
{
    // record for execute
    this->nJob = nJob;

    // output queue
    sem_init(&outQMutex, 0, 1);
    taskLastIdx = -1;
    tailCnt = 0;
    activeWorkNum = 0;

    // 2x nJob here since output may not be in order
    // could occupy more than nJob's memory
    bufPool = new BufferPool(nJob * 4); 

    // idleWorkerNum: let main thread freeze if there is no idle worker (assign new task)
    sem_init(&idleWorkerNum, 0, nJob);
    // isWorking: let main thread wait if there is still works
    sem_init(&isWorking, 0, 1);
    // idleWorkerQMutex: atmoic idleWorkerQueue
    sem_init(&idleWorkerQMutex, 0, 1);

    for (int i = 0; i < nJob; ++i)
    {
        Worker* w = new Worker(this);

        this->workers.push_back(w);
        this->idleWorkerQ.push(w);
    }
    
    
}

TaskQueue::~TaskQueue()
{
    for (int i = 0; i < nJob; ++i)
    {
        workers[i]->Kill();
        delete workers[i];
    }
    delete bufPool;
}

void TaskQueue::Enqueue(Task* t)
{
    // wait if there is no idle worker
    sem_wait(&idleWorkerNum);

    // enter crtical section for idleWorkerQ
    // goal: get an idle worker
    sem_wait(&idleWorkerQMutex);
    Worker* w = this->idleWorkerQ.front();
    this->idleWorkerQ.pop();
    if (++activeWorkNum == 1)
    {
        sem_wait(&isWorking);
    }
    sem_post(&idleWorkerQMutex);
    // leave crtical section for idleWorkerQ

    // assign task to the worker
    w->ExecTask(t);
}

void TaskQueue::Output(Task* t)
{
    sem_wait(&outQMutex);
    outQ.push(t);
    // if task output is contigous, output latest task
    while(!outQ.empty() && outQ.top()->taskIdx == taskLastIdx + 1)
    {
        Task* tNow = outQ.top();
        outQ.pop();

        char* buf = tNow->buffer;
        int lenBuf = strlen(buf);
        char nowChar;
        int nowCnt;

        // output and stitch
        if (tailCnt == 0) 
        // no previous output, directly output and enqueue last char
        {
            tailChar = buf[lenBuf - 2];
            tailCnt = buf[lenBuf - 1];
            buf[lenBuf - 2] = '\0';
            printf("%s", buf);
        } else {
            nowChar = buf[0];
            nowCnt = buf[1];
            buf = buf + 2;
            lenBuf -= 2;
            if (tailChar != nowChar)
            // tail not match, output tail and output the rest of them
            {
                printf("%c%c", tailChar, tailCnt);
                printf("%c%c", nowChar, nowCnt);
                // enqueue&remove tail
                if (lenBuf > 0)
                {
                    tailChar = buf[lenBuf - 2];
                    tailCnt = buf[lenBuf - 1];
                    buf[lenBuf - 2] = '\0';
                    printf("%s", buf);
                } else {
                    tailCnt = 0;
                }

            } else {
                // tail match, update tail according to first char
                tailCnt += nowCnt;
                if (lenBuf > 0)
                {
                    printf("%c%c", tailChar, tailCnt);
                    // enqueue&remove tail
                    tailChar = buf[lenBuf - 2];
                    tailCnt = buf[lenBuf - 1];
                    buf[lenBuf - 2] = '\0';
                    // omit first char and output the rest
                    printf("%s", buf);
                }
                
            }
        }
        bufPool->FreeBuffer(tNow);
        delete tNow;
        
        ++taskLastIdx;
    }
    sem_post(&outQMutex);
}


void TaskQueue::Flush()
{
    sem_wait(&isWorking);
    sem_post(&isWorking);
    // no thread is working here
    if (tailCnt != 0)
    {
        printf("%c%c", tailChar, tailCnt);
    }
}