#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>

#include <vector>
#include <iostream>
#include "pf_buffermgr.h"
using namespace std;


int main ()
{
    int num_buffer = 50;
    PF_BufferMgr pf_buffer = PF_BufferMgr(num_buffer);

    int pagesize = PF_PAGE_SIZE;

    char* filename = "customer.txt";
    int fd = open(filename, O_RDONLY);
    struct stat st;

    fstat(fd, &st);

    int remain_size = st.st_size % pagesize;
    int pageNum = (st.st_size + pagesize - 1) / pagesize;
    // int pageNum = ceil(st.st_size / pagesize);

    // char* content = (char*)mmap(NULL, pageNum * pagesize, PROT_READ, MAP_PRIVATE, fd, pagesize - remain_size);

    // check if MMap succeeded
    // if (content == MAP_FAILED)
    // {
    //     printf("error: read file failed.");
    // }


    cout << "pagenum = "     <<  pageNum      << endl;
    cout << "remain_size = " << remain_size   << endl;

    char **now_ptr = &filename;

    // 1. test allocatePage
    // for (int now_page = 0; now_page < pageNum; ++now_page)
    // {
    //     pf_buffer.allocatePage(fd, now_page, now_ptr);
    // }
    // pf_buffer.printBuffer();

    // 2. test getPage
    for (int now_page = 0; now_page <= pageNum; ++now_page)
    {
        int rc = pf_buffer.getPage(fd, now_page, now_ptr);
        if (!rc)
        {
            cout << *(now_ptr) << endl;
            cout << "-------------------------------newpage-------------------------------" << endl;
        }
        else
        {
            cout << "error code " << rc << endl;
        }

    }

    pf_buffer.printBuffer();
    close(fd);

    return 0;
}