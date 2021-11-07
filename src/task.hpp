#ifndef TASK_H // include guard
#define TASK_H

#define INFINITE -1
#define TASKMAXSIZE 4096

#define OUTFORMAT "%c%c"
// #define OUTFORMAT "[%c %d] " // for debug

struct task {
    int taskIdx;
    char* buffer;
    int taskFIdx; 
    int taskFpos; 
    int taskSize;
};

typedef struct task* pTask;

int execTask(int argc, char* argv[], pTask out);

#endif