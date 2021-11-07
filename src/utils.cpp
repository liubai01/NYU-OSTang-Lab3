#include "utils.hpp"

int getFileSize(const char* fileName)
{
    int ret;
    FILE* fp = fopen(fileName, "r");
    fseek(fp, 0L, SEEK_END);
    ret = ftell(fp);
    fclose(fp);

    return ret;
}