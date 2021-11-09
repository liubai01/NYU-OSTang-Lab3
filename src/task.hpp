#ifndef TASK_H // include guard
#define TASK_H

#define INFINITE -1

#define OUTFORMAT "%c%c"
// #define OUTFORMAT "[%c %d] " // for debug

class Task {
public:
    int taskIdx;
    int taskSize;

    char* buffer;
    char* fmmap; // pointer to the mmap file

    void Exec();

    Task();
    ~Task();
};



#endif