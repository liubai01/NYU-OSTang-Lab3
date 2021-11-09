#include <stdio.h>
#include <stdlib.h>

#include "src/utils.hpp"
#include "src/task.hpp"
#include "src/taskqueue.hpp"

#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>


#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(int argc, char* argv[])
{
    // Parse parameters
    int njob = parseNJob(argc, argv);

    if (njob < 0)
    {
        return 1;
    }

    TaskQueue taskQ(njob);


    // Sequential Version
    if (njob == 0 || njob == 1)
    {
        return seq(optind, argc, argv);
    }

    // Parallel Version
    // -> Dispatch tasks
    int taskIdx = 0; // the index for task, from 0 to taskNum
    
    int fd;
    int fileSize;
    char* fmmap; // for mmap

    for (int fidx = optind; fidx < argc; ++fidx)
    {

        fd = open(argv[fidx], O_RDONLY);
        fileSize = getFileSize(fd);

        // mmap once
        fmmap = (char *) mmap (nullptr, fileSize, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0);
        if (fmmap == MAP_FAILED)
        {
            handle_error("mmap");
        }

        size_t readLen = 0;
        for (off_t readStart = 0; readStart < fileSize; readStart += PGSIZE)
        {
            readLen = readStart + PGSIZE > fileSize ? fileSize - readStart : PGSIZE;

            Task* task = new Task();
            // mmap page by page (legacy with )
            // fmmap = (char *) mmap (nullptr, readLen, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, readStart);
            task->fmmap = readStart + fmmap;
            task->taskIdx = taskIdx;
            task->taskSize = readLen;
            ++taskIdx;

            taskQ.Enqueue(task);
        }

        close(fd);
    }

    taskQ.Flush();

    return 0;
    
}