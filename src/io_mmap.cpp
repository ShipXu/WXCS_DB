#include "io_mmap.h"


IO_Mmap::IO_Mmap(const char *file)
{
    // open file and get the size of file
    _fd = open(file, O_RDONLY);
    fstat(_fd, &_sb);

    _content = (char*)mmap(NULL, _sb.st_size, PROT_READ, MAP_PRIVATE, _fd, 0);

    // check if Map succeeded
    if (_content == MAP_FAILED)
    {
        printf("error: read file failed.:%s\n", file);
    }
}

IO_Mmap::~IO_Mmap()
{
    munmap(_content, _sb.st_size);
    close(_fd);
}