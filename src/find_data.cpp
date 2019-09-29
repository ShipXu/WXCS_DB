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
    char *pcontent = io_mmap.getData();
    
    for(int rowid = 1; rowid < ord_num+1; ++rowid)
    {
        len1 = charArrayToInt(pcontent , '|', num1);
        pcontent += len1 + 1;  // pcontent go to the next char of '|'
        len1 = charArrayToInt(pcontent , '|', num2);
        pcontent += len1 + 1; // pcontent go to the next char of '|'
        // if(num2 < 0 || num2 > 15000000)
        // {
        //     printf("errror : num process,%ld", num2);
        //     return -1;
        // }
        department_type = _custome[num2];       //depend on custome_id , get the department_type
        pOrder[department_type]->order_id = num1;
        _department_size[department_type] += 1;
        pOrder[department_type]->date = 0;
        for(int i = 0; i < 4; ++i)
        {
             pOrder[department_type]->date = pOrder[department_type]->date*10 + *pcontent - '0';
             pcontent += 1;
        }
        pcontent += 1;
        for(int i = 0; i < 2; ++i)
        {
             pOrder[department_type]->date = pOrder[department_type]->date*10 + *pcontent - '0';
             pcontent += 1;
        }
        pcontent += 1;
        for(int i = 0; i < 2; ++i)
        {
             pOrder[department_type]->date = pOrder[department_type]->date*10 + *pcontent - '0';
             pcontent += 1;
        }
        pcontent += 1; // pcontent go to the next char of '\n'
        if(*pcontent == '\0') break;
        pOrder[department_type] += 1;
    }
    return 0;
}

