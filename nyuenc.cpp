#include <stdio.h>
#include <stdlib.h>

#include "src/utils.hpp"
#include "src/task.hpp"
#include "src/taskqueue.hpp"
#include <unistd.h>


int main(int argc, char* argv[])
{
    // Parse parameters
    int njob = parseNJob(argc, argv);

    if (njob < 0)
    {
        return 1;
    }

    TaskQueue taskQ(njob, argc, argv);

    // Sequential Version
    if (njob == 0 || njob == 1)
    {
        return seq(optind, argc, argv);
    }

    // Parallel Version
    // -> Dispatch tasks
    int taskIdx = 0; // the index for task, from 0 to taskNum
    int taskSize = 0; // the size of current task
    int taskFIdx = 0; // the file name index for the start of the task
    int taskFPos = 0; // the file offset

    int fidx = optind - 1;
    int fileRestSize = 0;
    int filePos = 0;
    int isRunning = 1;

    while (isRunning)
    {
        taskSize = 0;
        if (fileRestSize == 0)
        {
            taskFIdx = fidx + 1;
            taskFPos = 0;
        } else {
            taskFIdx = fidx;
            taskFPos = filePos;
        }

        // try allocate until reach maxsize
        while (taskSize < PGSIZE)
        {
            // if curret file has been allocated
            if (!fileRestSize)
            {
                // try seek for new file for help
                if (++fidx == argc)
                {
                    isRunning = 0;
                    break;
                }
                fileRestSize = getFileSize(argv[fidx]);
                filePos = 0;
            }

            // allocate size
            if (fileRestSize + taskSize > PGSIZE)
            {
                fileRestSize -= PGSIZE - taskSize;
                filePos += PGSIZE - taskSize;
                taskSize = PGSIZE;
                break;
            } else {
                taskSize += fileRestSize;
                fileRestSize = 0;
            }
        }

        if (taskSize)
        {
            pTask t = new struct task();
            t->taskIdx = taskIdx;
            t->taskSize = taskSize;
            t->taskFIdx = taskFIdx;
            t->taskFpos = taskFPos;

            taskQ.Enqueue(t);
            taskIdx++;
        }

    }
    taskQ.Flush();

    return 0;
    
}