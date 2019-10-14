
#include<stdio.h>
#include"io_process.h"
#include<ctime>
#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdint.h>


void SearchResult(IO_Process *p_io_process, char *c_mktsegment,
 char *ord_oid_key_date, char *line_oid_key_date,
  uint16_t *oid_date_exist, double *oid_key_price);

void GetTime(struct timeval &start, struct timeval &end);
void SetOrderDateExist(char *order_date, int *date_oid, uint16_t *oid_date_exist, 
                        const int ithr, const int nthr);

void SearchLineitemResult(char *lineitem_date, int *date_lineitem_oid, double *date_lineitem_price,
                        uint16_t *oid_date_exist, double *oid_key_price, 
                        std::vector<int> &result_oid_key, const int ithr, const int nthr);

// void MmapWrite()
// {
//     int fd = open("/home/shipxu/demo_code/WXCS_DB3/index/lineitem_date_oid.txt", O_RDWR | O_CREAT, (mode_t)0600);

//     size_t textsize = sizeof(int) * DEPARTMENT_ORDER_SIZE * 5;//sizeof(double) * DEPARTMENT_LINEITEM_SIZE * 5;

//     lseek(fd, textsize, SEEK_SET);
//     write(fd, "", 1);
//     char *map = static_cast<char *>(mmap(0, textsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));
//     memcpy(map, date_oid, textsize);
//     msync(map, textsize, MS_SYNC);
//     munmap(map, textsize);
//     close(fd);
// }

int main()
{
    struct timeval start;
    struct timeval end;
    gettimeofday(&start,NULL); 

    IO_Process io_process;
    const char *cust_file, *ord_file, *line_file;
    int cust_num, ord_num, line_num;

    cust_file = "/home/shipxu/data/db_competition/customer.txt";
    ord_file = "/home/shipxu/data/db_competition/orders.txt";
    line_file = "/home/shipxu/data/db_competition/lineitem.txt";
    // cust_file = "/home/yangming/dataset/customer.txt";
    // ord_file = "/home/yangming/dataset/orders.txt";
    // line_file = "/home/yangming/dataset/lineitem.txt" ;
    cust_num = 15000000;
    ord_num =  150000000;
    line_num = 600037902;
   
    // if (-1 == io_process.mapCustomerData(cust_file, cust_num))
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

    GetTime(start, end);
    
    double *oid_key_price = (double*)malloc(sizeof(double)*ord_num);
    uint16_t *oid_date_exist = (uint16_t*)malloc(sizeof(uint16_t)* 150000001);

    SearchResult(&io_process, "BUILDING", "1995-02-29", "1995-04-27", oid_date_exist, oid_key_price);
    free(oid_key_price);
    free(oid_date_exist);
    GetTime(start, end);
    
    return 0;
}

void GetTime(struct timeval &start, struct timeval &end)
{
    double time_use=0;
    gettimeofday(&end,NULL);
    time_use=(end.tv_sec-start.tv_sec)*1000000+(end.tv_usec-start.tv_usec);//微秒
    time_use /= 1000000;
    printf("time_use is %.3f\n",time_use);
    start = end;
}

void SetOrderDateExist(char *order_date, int *date_oid, uint16_t *oid_date_exist, 
                        const int ithr, const int nthr)
{
    int order_yy, order_mm, order_dd;
    DateToInt(order_date, order_yy, order_mm, order_dd);
    int order_days = DateToDay(order_yy, order_mm, order_dd);
    //printf("order_days%d\n", order_days);
    
    size_t days_address = 0;
    size_t ithr_address = 0;
    size_t this_day_size_address = 0;
    size_t this_index = 0;

    int oid_key = 0;
    for (uint16_t order_day = 1; order_day < order_days; ++order_day)
    {
        for (size_t this_day = 0; ; ++this_day)
        {
            days_address = order_day * DAYS_OREDER_SIZE * nthr;
            ithr_address = DAYS_OREDER_SIZE * ithr;
            this_index = days_address + ithr_address + this_day;
            oid_key =  date_oid[this_index];
            if (oid_key == 0)
            {
                break;
            }
            //int oid_key = OidHash(order_id);
            oid_date_exist[oid_key] = order_day;
        }
    }
}

void SearchLineitemResult(char *lineitem_date, int *date_lineitem_oid, double *date_lineitem_price,
                        uint16_t *oid_date_exist, double *oid_key_price, 
                        std::vector<int> &result_oid_key, const int ithr, const int nthr)
{
    int lineitem_yy, lineitem_mm, lineitem_dd;
    DateToInt(lineitem_date, lineitem_yy, lineitem_mm, lineitem_dd);
    int lineitem_days = DateToDay(lineitem_yy, lineitem_mm, lineitem_dd);
    //printf("lineitem_days%d\n", lineitem_days);

    int oid_key = 0;
    size_t days_address = 0;
    size_t ithr_address = 0;
    size_t this_day_size_address = 0;
    size_t this_index = 0;
    
    for (uint16_t lineitem_day = lineitem_days+1; lineitem_day < LINEITEM_DAYS; ++lineitem_day)
    {
        for (size_t this_day = 0; ; ++this_day)
        {
            days_address = lineitem_day * DAYS_LINEITEM_SIZE * nthr;
            ithr_address = ithr * DAYS_LINEITEM_SIZE;
            this_index =  days_address + ithr_address + this_day;
            oid_key =  date_lineitem_oid[this_index];
            if (oid_key == 0)
            {
                break;
            }
            if (oid_date_exist[oid_key] > 0)
            {
                if (oid_key_price[oid_key] < 0.001)
                {
                    result_oid_key.push_back(oid_key);
                }
                oid_key_price[oid_key] += date_lineitem_price[this_index];
            }
        }
    }
}

void SearchResult(IO_Process *p_io_process, char *c_mktsegment,
 char *order_date, char *lineitem_date,
  uint16_t *oid_date_exist, double *oid_key_price)
{
    // double time_use=0;
    // struct timeval start;
    // struct timeval end;

    // gettimeofday(&start,NULL);

    memset(oid_date_exist, 0, sizeof(uint16_t)* 150000001);
    memset(oid_key_price, 0, sizeof(double)* 150000001);

    int *date_oid = p_io_process->get_oid_key_date(c_mktsegment[0]);
    int *date_lineitem_oid = p_io_process->get_lineitem_date_oid_key(c_mktsegment[0]);
    double *date_lineitem_price = p_io_process->get_lineitem_date_price(c_mktsegment[0]);

    // GetTime(start, end);
    auto ker = [&](const int ithr, const int nthr)
    {
        //GetTime(start, end);
        SetOrderDateExist(order_date, date_oid, oid_date_exist, ithr, nthr);

        std::vector<int> result_oid_key;

        SearchLineitemResult(lineitem_date, date_lineitem_oid, date_lineitem_price,
            oid_date_exist, oid_key_price, result_oid_key, ithr, nthr);
        Result result;
        char result_date[11] = "1992-01-01";
        for (int i = 0; i < result_oid_key.size(); ++i)
        {
            result.oid_key = result_oid_key[i];
            result.revenue = oid_key_price[result_oid_key[i]];
            if (result.revenue >= 399884.41)
            {
                DaysToDate(oid_date_exist[result_oid_key[i]], result_date);
                printf("%d, %f, %s\n", KeyToOid(result.oid_key), result.revenue, result_date);
            }
        }
    };
    
    #pragma omp parallel num_threads(THREAD_SIZE)
    {
        ker(omp_get_thread_num(), THREAD_SIZE);
    }

    // GetTime(start, end);
}