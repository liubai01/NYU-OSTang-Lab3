#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "task.hpp"

int execTask(
    int argc, 
    char* argv[], 
    pTask out)
{
    int taskFIdx = out->taskFIdx;
    int taskFpos = out->taskFpos;
    int taskSize = out->taskSize;

    FILE *fp = NULL;
    char c;
    int accChar = -1; // accmulated character
    int cnt = 0; // counter of occurence of character
    int outBufferOffset = 0;

    int taskCnt = 0; // halt when exceed
    int isRunning = 1;
    int fidx; // pointer to current fidx

    auto printToResult = [&] () {
        outBufferOffset += sprintf(
            out->buffer + outBufferOffset, 
            OUTFORMAT, 
            accChar, cnt
        );
 
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

    return 0;
}