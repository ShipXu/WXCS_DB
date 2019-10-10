#ifndef IO_MMAP_H
#define IO_MMAP_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>

class IO_Mmap
{
    public:
        IO_Mmap(const char *file); // Constructor
        ~IO_Mmap();          // Destructor

        // get pointer pointed to the head of the open file object
        inline char *getData()
        {
            return _content;
        }

        // get the length of the file
        inline long int getLength()
        {
            return _sb.st_size;
        }

    private:
        char *_content; 
        struct stat _sb;
        int _fd;
};
#endif  // IO_MMAP_H