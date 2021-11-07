#include "bufferpool.hpp"

BufferPool::BufferPool(int nBuffer)
{
	this->nBuffer = nBuffer;
	for (int i = 0; i < nBuffer; ++i)
	{
		pool.push_back( 
			(char *) malloc( sizeof(char) * ( 8096 + 1 ) )
		);
		pool[i][0] = '\0';
		freeIdx.push(i);
	}
	sem_init(&mutex, 0, 1);
}

BufferPool::~BufferPool()
{
	for (int i = 0; i < nBuffer; ++i)
	{
		free(pool[i]);
	}
}

void BufferPool::GetBuffer(pTask r)
{
	sem_wait(&mutex);
	int idx = freeIdx.front();
	freeIdx.pop();

	task2buffer[r->taskIdx] = idx;
	r->buffer = pool[idx];
	sem_post(&mutex);
}

void BufferPool::FreeBuffer(pTask r)
{
	sem_wait(&mutex);
	int bufferIdx = task2buffer[r->taskIdx];
	pool[bufferIdx][0] = '\0';
	task2buffer.erase(bufferIdx);
	freeIdx.push(bufferIdx);
	sem_post(&mutex);
}