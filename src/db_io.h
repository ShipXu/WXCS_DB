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
const long int DEPARTMENT_START_ADDRESS[5] = {0,29990185,59991084,89984416,119977878};


struct Order
{
    long int order_id;
    long int date; // date, format example:19960102
};

struct LineitemIndex
{
    long int order_id;
    long int lineitem_rowid;
    int num; //the number of linitem in this order_id
};

struct Lineitem
{
    long int order_id;
    double price;
    long int date; // date, format example:19960102
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

class IO_Process{

    public:
        ~IO_Process(); //deconstruct
        //get the data of ordertable
        int processOrder(char *custome_file, long int cust_num, char *order_file, long int ord_num); 
         //get the data of lineitemtable
        int processLineitem(char *file, long int number);

        Order* getOrder(long int department) 
        {
            return _order+DEPARTMENT_START_ADDRESS[department];
        }   

        long int* getDepartmentSize() 
        {
            return _department_size;
        }     

        LineitemIndex* getLineitemIndex() 
        {
            return _lineitem_index;
        }

        Lineitem* getLineitem() 
        {
            return _lineitem;
        }

        long int getLineitemIndexSize() 
        {
            return _limeitem_index_size;
        }    

        long int getOrderSize() 
        {
            return _order_size;
        }

        long int getLineitemSize() 
        {
            return _limeitem_size;
        }

    private:
        //map custome table's custome_id to it's department
        int mapCustomeData(char *file, long int number);

        int *_custome = NULL;
        Order *_order = NULL;
        long int _department_size[5];
        LineitemIndex *_lineitem_index = NULL;
        Lineitem *_lineitem = NULL;

        long int _custome_size;
        long int _order_size;        // LineitemIndex size equal to order size
        long int _limeitem_index_size;        // LineitemIndex size equal to order size
        long int _limeitem_size;
};

// Desc: find index of the first char equals 'c' and return it's index
int fineChar(char * input, char c);
// Desc: change the charArray from now to first char 'c' to long int 
int charArrayToInt(char * input, char c , long int &num);

#endif  // DB_IO_H