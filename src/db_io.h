#ifndef DB_IO_H
#define DB_IO_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <ctime>  
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#define ORDER_SZIE 150000000
#define CUSTOME_SZIE 15000000
#define LINEITEM_SZIE 600000000

//
// represent department e.g
// 0 : BUILDING  1 : AUTOMOBILE
// 2 : MACHINERY  3 : HOUSEHOLD 4 : FURNITURE
//
const char DEPARTMENT[5] = {'B','A','M','H','F'} ;
//const long int department_SIZE[5] = {29990185,30000899,29993332,29993462,30022122};
//the start address of each department
const int DEPARTMENT_START_ADDRESS[5] = {0,29990185,59991084,89984416,119977878};


const int MONTH_DAY[2][13] = 	
{
	{365, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{366, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

struct Order
{
    int order_id;
    int date; // date, format example:19960102
};



// struct LineitemIndex
// {
//     long int order_id;
//     long int lineitem_rowid;
//     int num; //the number of linitem in this order_id
// };
// struct LineitemAddress
// {
//     int lineitem_rowid;
//     int num; //the number of linitem in this order_id
// };

struct Lineitem
{
    int order_id;
    double price;
    int date; // date, format example:19960102
};

struct Result
{
    int order_id;
    char date[11] = "1992-01-01";
    double revenue;
};

class IO_Mmap
{
    public:
        IO_Mmap(char *file); // Constructor
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

class IO_Process
{
    public:
        IO_Process(); //construct
        ~IO_Process(); //deconstruct
        //get the data of ordertable
        int processOrder(char *custome_file, int cust_num, char *order_file, int ord_num); 
         //get the data of lineitemtable and bulid index for in
        int processLineitem(char *file, int number);

        Order* getOrder(int department) 
        {
            return _order+DEPARTMENT_START_ADDRESS[department];
        }   

        // int* getDepartmentSize() 
        // {
        //     return _department_size;
        // }     

        // LineitemIndex* getLineitemIndex() 
        // {
        //     return _lineitem_index;
        // }
        int* getOidToAddress();
        Lineitem* getLineitem();
        long int getLineitemIndexSize();
        int getOrderSize();
        int getLineitemSize();
        std::vector< std::vector<int> >  getBDate();
        std::vector< std::vector<int> >  getADate();
        std::vector< std::vector<int> >  getMDate();
        std::vector< std::vector<int> >  getHDate();
        std::vector< std::vector<int> >  getFDate();

    private:
        //map custome table's custome_id to it's department
        int mapCustomeData(char *file, int number);

        //Put the record in the bucket of the corresponding department and date
        void addRecord(int department_type, int days,int num1);

        int *_custome = NULL;
        Order *_order = NULL;
        // int _department_size[5];
        int _lineitem_index;
        int *_oid_to_address = NULL;
        Lineitem *_lineitem = NULL;

        int _custome_size;
        int _order_size;        // LineitemIndex size equal to order size
        int _limeitem_index_size;        // LineitemIndex size equal to order size
        int _limeitem_size;

        std::vector< std::vector<int> > _b_date;  // the second char represent department;
        std::vector< std::vector<int> > _a_date;
        std::vector< std::vector<int> > _m_date;
        std::vector< std::vector<int> > _h_date;
        std::vector< std::vector<int> > _f_date;
};

// find index of the first char equals 'c' and return it's index
int fineChar(char * input, char c);
// change the charArray from now to first char 'c' to long int 
int charArrayToInt(char * input, char c , int &num);
// change the date to how many days from 1992-01-01 to input 
int dateToDay(int yy, int mm, int dd);
// change how many days from 1992-01-01 to now to date 
void daysToDate(int days, char *result);
// change date char with format of 199x-xx-xx to year,mouth, day
void dateToInt(char *date, int &yy, int &mm, int &dd);
// a hash map order id to rowid
int oidHash(int oid);
#endif  // DB_IO_H