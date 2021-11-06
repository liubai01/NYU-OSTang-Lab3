#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define INFINITE -1
#define TASKMAXSIZE 7

// #define OUTFORMAT "%c%c"
#define OUTFORMAT "[%c %d] " // for debug

struct taskResult {
    int taskIdx;
    char buffer[4096];
    char headChar;
    int headCnt = 0;
    char tailChar;
    int tailCnt = 0;
};

int task(
    int argc, 
    char* argv[], 
    int taskFIdx, 
    int taskFpos, 
    int taskSize,
    struct taskResult* out)
{
    FILE *fp = NULL;
    char c;
    int accChar = -1; // accmulated character
    int cnt = 0; // counter of occurence of character
    int outBufferOffset = 0;

    int taskCnt = 0; // halt when exceed
    int isRunning = 1;
    int fidx; // pointer to current fidx

    auto printToResult = [&] () {
        // add to head if not exists
        if (out->headCnt == 0)
        {
            out->headChar = accChar;
            out->headCnt = cnt;
            return;
        }
        // flush tailChar to buffer if exists
        if (out->tailCnt != 0)
        {
            outBufferOffset += sprintf(
                out->buffer + outBufferOffset, 
                OUTFORMAT, 
                out->tailChar, out->tailCnt
            );
        }
        // set to the tail
        out->tailChar = accChar;
        out->tailCnt = cnt;  
    };

    for (fidx = taskFIdx; fidx < argc && isRunning; ++fidx)
    {
        fp = fopen(argv[fidx], "r");
        if (fidx == taskFIdx)
        {
             fseek(fp, taskFpos, SEEK_SET);  
        }

        if (!fp) {
            printf("File %s not exists!", argv[fidx]);
            return 1;
        }
        
        // read character by character
        while ((c = fgetc(fp)) != EOF)
        {
            if (accChar != c) {
                if (cnt) {
                    printToResult();
                }
                accChar = c;
                cnt = 1;
            } else {
                ++cnt;
                // reset counter when reach 16
                if (cnt == 16)
                {
                    printToResult();
                    accChar = -1;
                    cnt = 0;
                }
            }
            if (++taskCnt == taskSize) 
            {
                isRunning = 0;
                break;
            }
        }
        fclose(fp);
    }

    // output the tail
    if (cnt) {
        printToResult();
    }

    // if tail is hex full
    if (out->tailCnt == 16)
    {
        outBufferOffset += sprintf(
            out->buffer + outBufferOffset, 
            OUTFORMAT, 
            out->tailChar, out->tailCnt
        );
        out->tailChar = 0;
        out->tailCnt = 0;
    }

    return 0;
}

int getFileSize(const char* fileName)
{
    int ret;
    FILE* fp = fopen(fileName, "r");
    fseek(fp, 0L, SEEK_END);
    ret = ftell(fp);
    fclose(fp);

    return ret;
}

int main(int argc, char* argv[])
{
    // parse parameters
    int njob = 1;
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

    if (njob == 1) {
        // sequential
        struct taskResult result;
        return task(argc, argv, optind, 0, INFINITE, &result);
    } else {
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
                struct taskResult result = {};
                result.taskIdx = taskIdx;
                printf(
                    "task %d: fName[%s] taskSize[%d] taskFIdx[%d] taskFPos[%d]\n", 
                    taskIdx++, 
                    argv[taskFIdx], 
                    taskSize, 
                    taskFIdx, 
                    taskFPos
                );

                task(argc, argv, taskFIdx, taskFPos, taskSize, &result);
                printf("head: [%c %d]\n", result.headChar, result.headCnt);
                printf("%s\n", result.buffer);
                printf("tail: [%c %d]\n", result.tailChar, result.tailCnt);
                printf("\n");
            }

        }
    }

    return 0;
    
}