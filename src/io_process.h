#ifndef IO_PROCESS_H
#define IO_PROCESS_H

#include <omp.h>
#include <ctime>  
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "assert.h"
#include "io_mmap.h"
#include <stdint.h>

#define THREAD_SIZE 8
#define ORDER_DAYS 2407
#define DAYS_OREDER_SIZE 1741//12873
#define DEPARTMENT_ORDER_SIZE 33524696//30985311
#define LINEITEM_DAYS 2528
#define DAYS_LINEITEM_SIZE 6570//50757
#define DEPARTMENT_LINEITEM_SIZE 132871680//128313696

struct Result
{
    int oid_key;
    double revenue;
};

// struct Lineitem
// {
//     int order_id;
//     double price;
// };

class IO_Process
{
    public:
        IO_Process(); //construct
        ~IO_Process(); //deconstruct
        int8_t get_index_cutomer(const char first_item, size_t& d_size);
        //map customer table's customer_id to it's department
        int MapCustomerData(const char *file, size_t work_amount);
        void MaxDaysSize(uint32_t *day_size, int length);
        //get the data of ordertable
        int ProcessOrder(const char *custome_file, size_t cust_work_amount, const char *order_file, size_t work_amount); 
         //get the data of lineitemtable and bulid index for in
        int ProcessLineitem(const char *file, size_t work_amount);

        int8_t* get_customer() { return _customer; }
        int8_t* get_oid_department() { return _oid_department; }

        size_t  get_oid_key_date_size() { return _oid_key_date_size; }
        size_t  get_lineitem_date_size() { return _lineitem_date_size; }

        int* get_oid_key_date(const char department) 
        { 
            size_t useless = 0;
            int8_t department_type = get_index_cutomer(department, useless);
            return _oid_key_date + (DEPARTMENT_ORDER_SIZE * department_type); 
        }

        int* get_lineitem_date_oid_key(const char department) 
        { 
            size_t useless = 0;
            int8_t department_type = get_index_cutomer(department, useless);
            return _lineitem_date_oid_key + (DEPARTMENT_LINEITEM_SIZE * department_type); 
        }
        
        double* get_lineitem_date_price(const char department) 
        { 
            size_t useless = 0;
            int8_t department_type = get_index_cutomer(department, useless);
            return _lineitem_date_price + (DEPARTMENT_LINEITEM_SIZE * department_type); 
        }

    private:
        template <typename T>
        void _MergeV(T &v1, const T &v2);

        template <typename T>
        void MaxVectorSize(T &t_b_date, T &t_a_date,
                        T &t_m_date, T &t_h_date,T &t_f_date);

        int8_t *_customer = NULL;
        int8_t *_oid_department = NULL;

        int *_oid_key_date = NULL;
        int *_lineitem_date_oid_key = NULL;
        double *_lineitem_date_price = NULL;

        size_t _oid_key_date_size = 0;
        size_t _lineitem_date_size = 0;
      
};

int GetThrIndex(int ithr, int nthr);
int GetStartRowid(int ithr, int nthr, int divide_row_amount);

size_t GetStartDivideCustomer(int ithr, int nthr);
size_t GetEndDivideCustomer(int ithr, int nthr);
int GetStartRowidCustomer(int ithr, int nthr);
int GetEndRowidCustomer(int ithr, int nthr);

size_t GetStartDivideOrder(int ithr, int nthr);
size_t GetEndDivideOrder(int ithr, int nthr);
int GetStartRowidOrder(int ithr, int nthr);
int GetEndRowidOrder(int ithr, int nthr);

size_t GetStartDivideLineitem(int ithr, int nthr);
size_t GetEndDivideLineitem(int ithr, int nthr);
int GetStartRowidLineitem(int ithr, int nthr);
int GetEndRowidLineitem(int ithr, int nthr);


// find index of the first char equals 'c' and return it's index
int FindChar(char * input, char c);
// change the charArray from now to first char 'c' to long int 
int CharArrayToInt(char * input, char c , int &num);
int GetDay(char* &s_date);
// change the date to how many days from 1992-01-01 to input 
int DateToDay(int yy, int mm, int dd);
// change how many days from 1992-01-01 to now to date 
void DaysToDate(int days, char *result);
// change date char with format of 199x-xx-xx to year,mouth, day
void DateToInt(char *date, int &yy, int &mm, int &dd);
// a hash map order id to rowid
int OidHash(int oid);
int KeyToOid(int oid_key);

#endif  //\s(([A-Z])+[a-z]+)