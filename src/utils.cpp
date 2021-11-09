#include "utils.hpp"
#include "task.hpp"
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int parseNJob(int argc, char* argv[])
{
    char c;
    int njob = 0;
    while ((c = getopt (argc, argv, "j:")) != -1)
    {
        switch (c)
        {
            case 'j':
                njob = atoi(optarg);
                break;
            case ':':
            // requires an option
                return -1;
                break; 
            case '?':
            // unknown option
                return -1;
                break; 
        }
    }

    return njob;
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

inline int getFileSize(int fd)
{
    struct stat s;
    fstat (fd, &s);

    return s.st_size;
}

int seq(int start, int end, char** argv)
{
    int fd;
    char c; // for char by char reading
    int accChar = -1; // accmulated character
    int cnt = 0; // counter of occurence of character

    char* f; // for mmap
    int fileSize;

    for (int fidx = start; fidx < end; ++fidx)
    {

        fd = open(argv[fidx], O_RDONLY);

        fileSize = getFileSize(fd);

        size_t readLen = 0;
        for (off_t readStart = 0; readStart < fileSize; readStart += PGSIZE)
        {
            readLen = readStart + PGSIZE > fileSize ? fileSize - readStart : PGSIZE;

            f = (char *) mmap (0, readLen, PROT_READ, MAP_PRIVATE, fd, readStart);

            for (int i = 0; i < readLen; ++i) 
            {
                c = f[i];
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
        }

        close(fd);
    }
    // output the tail
    if (cnt != 0) {
        printf(OUTFORMAT, accChar, cnt);
    }

    return 0;
}