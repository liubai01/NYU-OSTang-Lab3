#include "bufferpool.hpp"

BufferPool::BufferPool(int nBuffer)
{
	this->nBuffer = nBuffer;
	for (int i = 0; i < nBuffer; ++i)
	{
		pool.push_back( 
			(char *) malloc( sizeof(char) * ( PGSIZE * 2 + 1 ) )
		);
		pool[i][0] = '\0';
		freeIdx.push(i);
	}
	sem_init(&mutex, 0, 1);
	sem_init(&bufferNum, 0, nBuffer);
}

BufferPool::~BufferPool()
{
	for (int i = 0; i < nBuffer; ++i)
	{
		free(pool[i]);
	}
}

void BufferPool::GetBuffer(Task* t)
{
	sem_wait(&bufferNum);
	sem_wait(&mutex);
	int idx = freeIdx.front();
	freeIdx.pop();

	task2buffer[t->taskIdx] = idx;
	t->buffer = pool[idx];
	sem_post(&mutex);
}

void BufferPool::FreeBuffer(Task* t)
{
	sem_wait(&mutex);
	int bufferIdx = task2buffer[t->taskIdx];
	pool[bufferIdx][0] = '\0';
	task2buffer.erase(bufferIdx);
	freeIdx.push(bufferIdx);
	sem_post(&mutex);
	sem_post(&bufferNum);
}