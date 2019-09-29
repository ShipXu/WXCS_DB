#include "db_io.h"


IO_Process::IO_Process()
{
    //resize the vector to become a fixed length array
    _b_date.resize(2408);
    _a_date.resize(2408);
    _m_date.resize(2408);
    _f_date.resize(2408);
    _h_date.resize(2408);
}

//
// processOrder
//
// Desc: get the data of Ordertable and group it by department
//
// In:   file - the path of file
//       number - number of orders in the file
// Out:  _order - the initial address of orders table data
//            
//
int IO_Process::processOrder(char *custome_file, int cust_num, char *order_file, int ord_num)
{
    if(-1 == mapCustomeData(custome_file, cust_num) )
    {
        return -1;
    }
    IO_Mmap io_mmap(order_file);
    char *pcontent = io_mmap.getData();
   
    // _order = (Order*)malloc(sizeof(Order)* ORDER_SZIE);

    // if (NULL == _order)
    // {
    //     printf("error:fail to allocate memory to _order\n");
    //     return -1;
    // }

    // Order* pOrder[5]; //each point one department
    // for(int i = 0; i < 5; ++i)
    // {
    //     pOrder[i] = _order + DEPARTMENT_START_ADDRESS[i];
    // }
    // for(int i = 0; i < 5; ++i)
    // {
    //     _department_size[i] = 0;
    // }

    int len1;
    int num1, num2, num3;
    int department_type;
    int yy,mm,dd;
    int days = 0;
    // printf("read order start\n");
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
        //pOrder[department_type]->order_id = num1;
        //_department_size[department_type] += 1;
       // pOrder[department_type]->date = 0;
        yy = dd = mm = 0;
        for(int i = 0; i < 4; ++i)
        {
             yy = yy*10 + *pcontent - '0';
             pcontent += 1;
        }
        pcontent += 1;
        for(int i = 0; i < 2; ++i)
        {
             mm = mm*10 + *pcontent - '0';
             pcontent += 1;
        }
        pcontent += 1;
        for(int i = 0; i < 2; ++i)
        {
             dd = dd *10 + *pcontent - '0';
             pcontent += 1;
        }
        days = dateToDay(yy,mm,dd);
        addRecord(department_type, days, num1);
        pcontent += 1; // pcontent go to the next char of '\n'
        if(*pcontent == '\0') break;
        // pOrder[department_type] += 1;
    }
    _order_size = ord_num;

    return 0;
};

//
// processLineitem
//
// Desc: get the data of LineitemTable and build an index for it
//
// In:   file - the path of file
//       number - number of orders in the file
// Out:  _lineitem - the initial address of lineitem table data
//       _lineitem_index - the initial address of lineitem table index
//
int IO_Process::processLineitem(char *file, int size)
{
    IO_Mmap io_mmap(file);
    char *pcontent = io_mmap.getData();
    
    _lineitem = (Lineitem*)malloc(sizeof(Lineitem)*size);
    if (NULL == _lineitem)
    {
        printf("error:fail to allocate memory to _lineitem\n");
        return -1;
    }
    _oid_to_address = (int*)malloc(sizeof(int)*(_order_size+1));
    if (NULL == _oid_to_address)
    {
        printf("error:fail to allocate memory to _lineitem_index\n");
        return -1;
    }
    memset(_oid_to_address, -1, sizeof(int)*(_order_size+1));
    Lineitem *p_lineitem= _lineitem;
    //int *p_oid_to_address = (_oid_to_address+1);
    // LineitemIndex *p_lineitem_index = _lineitem_index;
    _limeitem_index_size = 1;
    int len1, pre_rowid;
    int num, pre_orderid;
    double price = 0;
    //LineitemAddress temp;
    pre_rowid = 1; 
    pre_orderid = -1;

    for(int rowid = 1; rowid < size+1; ++rowid)
    {
        len1 = charArrayToInt(pcontent , '|', num);
        //printf("%d\n", num);
        if(pre_orderid != num && _limeitem_index_size < _order_size)
        {
            _oid_to_address[_limeitem_index_size] = rowid;
            pre_orderid = num;
            //pre_rowid = rowid;
            // p_oid_to_address += 1;
            _limeitem_index_size += 1;
        }
        p_lineitem->order_id = num;  
        pcontent += len1+1;  // pcontent go to the next char of '|'

        len1 = charArrayToInt(pcontent , '.', num);
        pcontent += len1+1;
        price = num;

        len1 = charArrayToInt(pcontent , '|', num);
        price += ((double)num/100.0);
        pcontent+=len1+1;
        p_lineitem->price = price;
        
        p_lineitem->date = 0;
        for(int i = 0; i < 4; ++i)
        {
            p_lineitem->date =  p_lineitem->date*10 + *pcontent - '0';
            pcontent += 1;
        }
        pcontent += 1;
        for(int i = 0; i < 2; ++i)
        {
            p_lineitem->date =  p_lineitem->date*10 + *pcontent - '0';
            pcontent += 1;
        }
        pcontent += 1;
        for(int i = 0; i < 2; ++i)
        {
            p_lineitem->date =  p_lineitem->date*10 + *pcontent - '0';
            pcontent += 1;
        }
        pcontent += 1; // pcontent go to the next char of '\n'
        if(*pcontent == '\0') break;
        p_lineitem += 1;
    }

    _limeitem_size = size;
    return 0;
};


//
// mapCustomeData
//
// Desc: map custome table's custome_id to it's department
//
// In:   file - the path of file
//       number - number of customes in the file
// Out:  _custome - a table map custome table's custome_id to it's department
//            
//
int IO_Process::mapCustomeData(char *file, int number)
{
    IO_Mmap io_mmap(file);
    char *pcontent = io_mmap.getData();
    
    _custome = (int*)malloc(sizeof(int)*(number+1));

    if (NULL == _custome)
    {
        printf("error:fail to allocate memory to _custome\n");
        return -1;
    }

    memset((void *)_custome, -1, number+1);

    int len1;
    int num;

    //map one row's custome_id to it's department
    for(int rowid = 1; rowid <= number; ++rowid)
    {
        len1 = fineChar(pcontent,'|');
        pcontent += len1 + 1; // pcontent go to the next char of '|'
        switch(*pcontent)
        {
            case 'B':
                _custome[rowid] = 0; // custome_id equal to rowid
                break;
            case 'A':
                _custome[rowid] = 1;
                break;
            case 'M':
                _custome[rowid] = 2;
                break;
            case 'H':
                _custome[rowid] = 3;
                break;
            case 'F':
                _custome[rowid] = 4;
                break;
        };

        while(*pcontent != '\n')
        {
            pcontent += 1;
        }

        if(*pcontent == '\0') break;
    }
    _custome_size = number;
    return 0;
};

//
// addRecord
//
// Desc: Put the record in the bucket of the corresponding department and date
//
// In:   department_type - the type of department
//       days - how many from 1992-01-01 to now
//       num1 - the record     
//
void IO_Process::addRecord(int department_type, int days,int num1)
{
    switch (department_type)
        {
            case 0:
                _b_date[days].push_back(num1);
                break;
            case 1:
                _a_date[days].push_back(num1);
                break;
            case 2:
                _m_date[days].push_back(num1);
                break;
            case 3:
                _f_date[days].push_back(num1);
                break;
            case 4:
                _h_date[days].push_back(num1);
                break;
            default:
                break;
        }
}

int* IO_Process::getOidToAddress() 
{
        return _oid_to_address;
};

Lineitem* IO_Process::getLineitem() 
{
    return _lineitem;
}

long int IO_Process::getLineitemIndexSize() 
{
    return _limeitem_index_size;
}    

int IO_Process::getOrderSize() 
{
    return _order_size;
}

int IO_Process::getLineitemSize() 
{
    return _limeitem_size;
}

std::vector< std::vector<int> >  IO_Process::getBDate() 
{
    return _b_date;
}

std::vector< std::vector<int> >  IO_Process::getADate() 
{
    return _a_date;
}

std::vector< std::vector<int> >  IO_Process::getMDate() 
{
    return _m_date;
}

std::vector< std::vector<int> >  IO_Process::getHDate() 
{
    return _h_date;
}

std::vector< std::vector<int> >  IO_Process::getFDate() 
{
    return _f_date;
}

IO_Process::~IO_Process()
{
    if(_order != NULL)
        free(_order);
    if(_custome != NULL)
        free(_custome);
    if(_lineitem != NULL)
        free(_lineitem);
    // if(_lineitem_index != NULL)
    //     free(_lineitem_index);
}



//
// finechar
//
// Desc: find index of the first char equals 'c' and return it's index
//
// In:   input - the input char array
//       c - the char to be finded
// Out:  index - the index of first char equals 'c'
//            
//
int fineChar(char *input, char c)
{
    int index = 0; 
    char *ptr_input = input;
    while(*ptr_input != c)
    {
        ptr_input += 1;
        index += 1;
    }
    return index;
}

//
// finechar
//
// Desc: change the charArray from now to first char 'c' to long int 
//
// In:   input - the input charArray
//       c - the char to be finded
// Out:  index - the index of first char equals 'c'
//       num - the long int content of the charArray
//
int charArrayToInt(char * input, char c , int &num)
{
    int index = 0; 
    char *pinput = input;
    num = 0;
    while(*pinput != c)
    {
        index += 1;
        num = num*10 + (*pinput - '0');
        pinput += 1;
    }
    return index;
}

//
// Desc: change the date to how many days from 1992-01-01 to input 
//
// In:   yy - which year
//       mm - which month
//       dd - which day
// Out:  days - how many days from 1992-01-01 to input 
//
int dateToDay(int yy, int mm, int dd)
{
    int year = yy - 1992;
    int days = dd + year * 365;
    if(year >= 5)
    {
        days += 2;
    } 
    else if( year >= 1) 
    {   
        days += 1;
    }
    int leap = 0;
    //Determine whether it is a leap year
    if(yy == 1992 || yy == 1996)
    {
        leap = 1;
    }
    for(int i = 1; i < mm ; ++i)
    {
        days += MONTH_DAY[leap][i];
    }
    return days;
}


//
// Desc: change how many days from 1992-01-01 to now to date 
//
// In:   days - how many days from 1992-01-01 to now to date 
//       result - an char array of this format 199x-xx-xx
// Out:  result - the date 
//
void daysToDate(int days, char *result)
{
    int leap = 0; 
    
    for(int year = 1992; year < 1999; ++year)
    {
        //Determine whether it is a leap year
        if(year == 1992 || year == 1996)
        {
            leap = 1;
        }
        else
        {
            leap = 0;
        };
       
        if(days <= MONTH_DAY[leap][0])
        {
            result[3] = (year % 10) + '0';
            for(int mm = 1; mm < 13; ++mm)
            {
                if(days <= MONTH_DAY[leap][mm])
                {
                    result[5] = (mm / 10) + '0';
                    result[6] = (mm % 10) + '0';
                    result[8] = (days / 10) + '0';
                    result[9] = (days % 10) + '0';
                    return;
                }
                else
                {
                    days -= MONTH_DAY[leap][mm];
                }
            }
        }
        else
        {
            days -= MONTH_DAY[leap][0];
        }
    }
}

//
// Desc: change date char with format of 199x-xx-xx to year,mouth, day
//
// In:   date - date char with format of 199x-xx-xx
// Out:  yy - year 
//       mm - mouth 
//       dd - day 
//
void dateToInt(char *date, int &yy, int &mm, int &dd)
{
    yy = 0;
    mm = 0;
    dd = 0;
    for(int i = 0; i < 4; ++i)
    {
        yy = (yy * 10) + date[i] - '0';
    }
    
    for(int i = 5; i < 7; ++i)
    {
        mm = (mm * 10) + date[i] - '0';
    }
    for(int i = 8; i < 10; ++i)
    {
        dd = (dd * 10) + date[i] - '0';
    }

}

//
// Desc: a hash map order id to rowid
//
// In:   oid - order id
// Out:  rowids 
//
int oidHash(int oid)
{
    int rowids = oid >> 5;
    rowids = rowids << 3;
    rowids += oid % 8;
    return rowids;
}