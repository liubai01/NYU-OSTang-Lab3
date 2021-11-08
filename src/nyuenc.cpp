#include <stdio.h>
#include <stdlib.h>

#include "utils.hpp"
#include "task.hpp"
#include "taskqueue.hpp"
#include <unistd.h>


int main(int argc, char* argv[])
{
    // parse parameters
    int njob = 0;
    char c;
    while ((c = getopt (argc, argv, "j:")) != -1)
    {
        switch (c)
        {
            case 'j':
                njob = atoi(optarg);
                break;
            case ':':
                printf("option needs a value\n"); 
                break; 
            case '?':
                printf("unknown option: %c\n", optopt);
                break; 
        }
    }

    TaskQueue taskQ(njob, argc, argv);

    // sequential version
    if (njob == 0 || njob == 1)
    {
        FILE *fp = NULL;
        char c;
        int accChar = -1; // accmulated character
        int cnt = 0; // counter of occurence of character

        for (int fidx = optind; fidx < argc; ++fidx)
        {
            fp = fopen(argv[fidx], "r");

            if (!fp) {
                printf("File %s not exists!", argv[fidx]);
                return 1;
            }
            
            // read character by character
            while ((c = fgetc(fp)) != EOF)
            {
                if (accChar != c) {
                    if (cnt) {
                        printf(OUTFORMAT, accChar, cnt);
                    }
                    accChar = c;
                    cnt = 1;
                } else {
                    ++cnt;
                }
            }
            fclose(fp);
        }

        // output the tail
        if (cnt) {
            printf(OUTFORMAT, accChar, cnt);
        }


        return 0;
    }

    // Construct tasks
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
        while (taskSize < TASKMAXSIZE)
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
            if (fileRestSize + taskSize > TASKMAXSIZE)
            {
                fileRestSize -= TASKMAXSIZE - taskSize;
                filePos += TASKMAXSIZE - taskSize;
                taskSize = TASKMAXSIZE;
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

            taskQ.enqueue(t);
            taskIdx++;
        }

    }
    taskQ.flush();

    return 0;
    
}