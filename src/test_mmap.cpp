#include<stdio.h>
#include"db_io.h"
#include<ctime>
int main()
{
    int start = clock();  
    char *cust_file, *ord_file, *line_file;
    long int cust_num, ord_num, line_num;
    cust_file = "/home/jiaxuan/test/dataset/customer.txt";
    ord_file = "/home/jiaxuan/test/dataset/orders.txt";
    line_file = "/home/jiaxuan/test/dataset/lineitem.txt";

    cust_num = 15000000;
    ord_num =  10000;
    line_num = 10000;
    IO_Mmap io_mmap(ord_file);
    char *content = io_mmap.getData();
    long int size = io_mmap.getLength();
    
    // while(*content != '\0')
    // {
    //     content += 1;
    // }
    
    // for(long int i = 0; i < size && *content != '\0'; ++i)
    // {
    //     ++content;
    // }

    for(long int i = 0; i < size; ++i)
    {
        if(*content != '\0')
        {
            content += 1;
        }
    }

    printf("read%.3lf\n",double(clock()-start)/CLOCKS_PER_SEC); 
    return 0;
}

