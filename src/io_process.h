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

struct Result
{
    int order_id;
    char date[11] = "1992-01-01";
    double revenue;
};

struct Lineitem
{
    int order_id;
    double price;
    int date; // date, format example:19960102
};


class IO_Process
{
    public:
        IO_Process(); //construct
        ~IO_Process(); //deconstruct
        
        //map customer table's customer_id to it's department
        int MapCustomerData(const char *file, size_t work_amount);

        //get the data of ordertable
        int ProcessOrder(const char *custome_file, size_t cust_work_amount, const char *order_file, size_t work_amount); 
         //get the data of lineitemtable and bulid index for in
        int ProcessLineitem(const char *file, size_t work_amount);

        int* get_custome();
        int* get_oid_to_address();
        Lineitem* get_lineitem();

        size_t get_oid_to_address_size();
        size_t get_limeitem_size();

        std::vector< std::vector<int> >  get_b_date();
        std::vector< std::vector<int> >  get_a_date();
        std::vector< std::vector<int> >  get_m_date();
        std::vector< std::vector<int> >  get_h_date();
        std::vector< std::vector<int> >  get_f_date();

    private:
        //Put the record in the bucket of the corresponding department and date
        void AddRecord(int department_type, int days,int num1);

        template <typename T>
        void _MergeV(T &v1, const T &v2);

        void MergeRecord(std::vector< std::vector<int> > &t_b_date,
                         std::vector< std::vector<int> > &t_a_date,
                         std::vector< std::vector<int> > &t_m_date,
                         std::vector< std::vector<int> > &t_h_date,
                         std::vector< std::vector<int> > &t_f_date);

        int get_index_cutomer(const char first_item, size_t& d_size);
        int *_customer = NULL;
        // int _department_size[5];
        int _lineitem_index;
        int *_oid_to_address = NULL;
        Lineitem *_lineitem = NULL;

        size_t _customer_size;
        size_t _order_size = 150000000;        // LineitemIndex size equal to order size
        size_t _oid_to_address_size = 150000001;        // LineitemIndex size equal to order size
        size_t _limeitem_size;

        std::vector< std::vector<int> > _b_date;  // the second char represent department;
        std::vector< std::vector<int> > _a_date;
        std::vector< std::vector<int> > _m_date;
        std::vector< std::vector<int> > _h_date;
        std::vector< std::vector<int> > _f_date;
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
int GetOrderDay(char* &s_date);
// change the date to how many days from 1992-01-01 to input 
int DateToDay(int yy, int mm, int dd);
// change how many days from 1992-01-01 to now to date 
void DaysToDate(int days, char *result);
// change date char with format of 199x-xx-xx to year,mouth, day
void DateToInt(char *date, int &yy, int &mm, int &dd);
// a hash map order id to rowid
int OidHash(int oid);

#endif  //\s(([A-Z])+[a-z]+)