#include<stdio.h>
#include"db_io.h"
#include<ctime>
int main()
{
    int start = clock(); 
    IO_Process io_process;
    char *cust_file, *ord_file, *line_file;
    long int cust_num, ord_num, line_num;
    cust_file = "/home/jiaxuan/test/dataset/customer.txt";
    ord_file = "/home/jiaxuan/test/dataset/orders.txt";
    line_file = "/home/jiaxuan/test/dataset/mini_linetiem.txt";

    cust_num = 15000000;
    ord_num =  150000000;
    line_num = 10000;

    if(-1 == io_process.processOrder(cust_file, cust_num, ord_file, ord_num))
    {
        return -1;
    }
    
    Order *porder;
    long int *department_size = io_process.getDepartmentSize();
    printf("fread%.3lf/n",double(clock()-start)/CLOCKS_PER_SEC);  
    for(int i = 0; i < 5; ++i)
    {
        printf("department: %c %ld \n", DEPARTMENT[i], department_size[i]);
        porder = io_process.getOrder(i);
        for(int j = 0; j < 10; ++j)
        {
            printf("%ld,%ld\n", porder[j].order_id, porder[j].date);
        }
    }
//     if(-1 == io_process.processOrder(cust_file, cust_num, ord_file, ord_num))
//     {
//         return -1;
//     }
//     if(-1 == io_process.processLineitem(line_file, line_num) )
//     {
//         return -1;
//     }
//     LineitemIndex *lineitem_index = io_process.getLineitemIndex();
//     Lineitem *lineitem = io_process.getLineitem();
//     long int lineitem_index_size = io_process.getLineitemIndexSize();
//     long int lineitem_size = io_process.getLineitemSize();
//    // printf("fread%.3lf/n",double(clock()-start)/CLOCKS_PER_SEC);  
//     for(int i = 0; i < lineitem_size; ++i)
//     {
//         printf("%ld，%ld, %f \n", lineitem[i].order_id, 
//         lineitem[i].date, lineitem[i].price);
//     }

    return 0;
}

