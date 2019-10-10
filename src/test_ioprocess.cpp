
#include<stdio.h>
#include"io_process.h"
#include<ctime>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>

void searchResult(IO_Process *p_io_process, char *c_mktsegment, char *ord_date, char *line_date);

int main()
{
    double time_use=0;
    struct timeval start;
    struct timeval end;
    gettimeofday(&start,NULL); 
    IO_Process io_process;
    const char *cust_file, *ord_file, *line_file;
    int cust_num, ord_num, line_num;

    cust_file = "/opt/demo_code/dataset/customer.txt";
    ord_file = "/opt/demo_code/dataset/orders.txt";
    line_file = "/opt/demo_code/dataset/lineitem.txt";

    cust_num = 15000000;
    ord_num =  150000000;
    line_num = 600000000;

    // if(-1 == io_process.mapCustomerData(cust_file, cust_num))
    // {
    //     return -1;
    // }
    if (-1 == io_process.ProcessOrder(cust_file, cust_num, ord_file, ord_num))
    {
        return -1;
    }
    if (-1 == io_process.ProcessLineitem(line_file, line_num))
    {
        return -1;
    }
    

  
    searchResult(&io_process, "BUILDING", "1995-02-29", "1995-04-27");
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    time_use /= 1000000;
    printf("time_use is %.3f\n",time_use);

    //printf("read %.3lf\n",double(clock()-start)/CLOCKS_PER_SEC); 
    return 0;
}

void searchResult(IO_Process *p_io_process, char *c_mktsegment, char *ord_date, char *line_date)
{
    int line_yy, line_mm, line_dd;
    DateToInt(line_date, line_yy, line_mm, line_dd);
    int line_date_int = 0;
    line_date_int = line_yy * 10000 + line_mm * 100 + line_dd;
    //printf("line_date_int%d\n", line_date_int);

    int *oid_to_address = p_io_process->get_oid_to_address();
    Lineitem *lineitem = p_io_process->get_lineitem();
    int lineitem_size = p_io_process->get_limeitem_size();

    std::vector< std::vector<int> > date_oid;
    switch (c_mktsegment[0])
    {
        case 'B' :
            date_oid = p_io_process->get_b_date();
            break;
        case 'A' :
            date_oid = p_io_process->get_a_date();
            break;
        case 'M' :
            date_oid = p_io_process->get_m_date();
            break;
        case 'F' :
            date_oid = p_io_process->get_f_date();
            break;
        case 'H' :
            date_oid = p_io_process->get_h_date();
            break; 
    }
    int yy, mm, dd;
    DateToInt(ord_date, yy, mm, dd);
    int days = DateToDay(yy, mm, dd);
    //printf("days%d\n", days);
    
    Result result;
    bool flag = false;
    int order_id = -1;

    //traverse all the oid with date smaller than ord_date
    for (int day = 1; day < days; ++day)
    {
        for (size_t i = 0; i < date_oid[day].size(); ++i)
        {
            order_id =  date_oid[day][i];
            int rowid = OidHash(order_id);
            //printf("rowid%d\n", rowid);
            //map order_id to the corresponding rowid int lineitem table
            int lineIndex = oid_to_address[rowid] - 1;
            //if this order_id have no record in lineitem table, continue
            if (lineIndex < 0)
            {
               continue;
            }
            //printf("lineIndex%d\n", lineIndex);
            flag = false;
            result.revenue = 0;
            //traverse the corresponding lineitem
            for (int line_index = lineIndex; line_index < lineitem_size 
            && lineitem[line_index].order_id == order_id; ++line_index)
            {
                if (lineitem[line_index].date > line_date_int)
                {
                    flag = true;
                    result.revenue += lineitem[line_index].price;
                }
            }
            if (flag)
            {
                result.order_id = order_id;
                DaysToDate(day, result.date);
                if (result.revenue >= 399884.41)
                {
                    printf("%d, %s, %f\n", result.order_id, result.date, result.revenue);
                }
                
            }
        }
    }
    
}