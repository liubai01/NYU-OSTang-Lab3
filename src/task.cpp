#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "task.hpp"
#include "utils.hpp"

Task::Task()
{
    buffer = (char *) malloc( sizeof(char) * ( PGSIZE * 2 + 1 ) );
}

Task::~Task()
{
    free(buffer);
}

void Task::Exec()
{
    char c; // for char by char reading
    int accChar = -1; // accmulated character
    int cnt = 0; // counter of occurence of character
    int outBufferOffset = 0;

    for (int i = 0; i < taskSize; ++i) 
    {
        c = fmmap[i];
        if (accChar != c) {
            if (cnt != 0) {
                outBufferOffset += sprintf(
                    buffer + outBufferOffset, 
                    OUTFORMAT, 
                    accChar, cnt
                );
            }
            accChar = c;
            cnt = 1;
        } else {
            ++cnt;
        }
    }

    // output the tail
    if (cnt != 0)
    {
        outBufferOffset += sprintf(
            buffer + outBufferOffset, 
            OUTFORMAT, 
            accChar, cnt
        );
    }

    // munmap(fmmap, taskSize);

}