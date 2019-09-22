#include "db_io.h"
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
int IO_Process::processOrder(char *custome_file, long int cust_num, char *order_file, long int ord_num)
{
    if(-1 == mapCustomeData(custome_file, cust_num) )
    {
        return -1;
    }
    IO_Mmap io_mmap(order_file);
    char *pcontent = io_mmap.getData();
   
    _order = (Order*)malloc(sizeof(Order)* ORDER_SZIE);

    if (NULL == _order)
    {
        printf("error:fail to allocate memory to _order\n");
        return -1;
    }

    Order* pOrder[5]; //each point one department
    for(int i = 0; i < 5; ++i)
    {
        pOrder[i] = _order + DEPARTMENT_START_ADDRESS[i];
    }
    for(int i = 0; i < 5; ++i)
    {
        _department_size[i] = 0;
    }

    int len1;
    long int num1, num2, num3;
    int department_type;
    // printf("read order start\n");
    for(long int rowid = 1; rowid < ord_num+1; ++rowid)
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
int IO_Process::processLineitem(char *file, long int size)
{
    IO_Mmap io_mmap(file);
    char *pcontent = io_mmap.getData();
    
    _lineitem = (Lineitem*)malloc(sizeof(Lineitem)*size);
    if (NULL == _lineitem)
    {
        printf("error:fail to allocate memory to _lineitem\n");
        return -1;
    }
    _lineitem_index = (LineitemIndex*)malloc(sizeof(LineitemIndex)*_order_size);
    if (NULL == _lineitem_index)
    {
        printf("error:fail to allocate memory to _lineitem_index\n");
        return -1;
    }

    Lineitem *p_lineitem = _lineitem;
    LineitemIndex *p_lineitem_index = _lineitem_index;
    _limeitem_index_size = 0;
    int len1, pre_rowid;
    long int num, pre_orderid;
    double price = 0;

    pre_rowid = 1; 
    pre_orderid = -1;

    for(long int rowid = 1; rowid < size+1; ++rowid)
    {
        len1 = charArrayToInt(pcontent , '|', num);
        if(pre_orderid == -1)
        {
            pre_orderid = num;
        }
        // printf("%d\n", num);
        if(pre_orderid != -1 && pre_orderid != num && _limeitem_index_size < _order_size)
        {
            p_lineitem_index->order_id = pre_orderid;
            p_lineitem_index->lineitem_rowid = pre_rowid;
            p_lineitem_index->num = rowid - pre_rowid;
            pre_orderid = num;
            pre_rowid = rowid;
            p_lineitem_index += 1;
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
int IO_Process::mapCustomeData(char *file, long int number)
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
    long int num;

    //map one row's custome_id to it's department
    for(long int rowid = 1; rowid <= number; ++rowid)
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

IO_Process::~IO_Process()
{
    if(_order != NULL)
        free(_order);
    if(_custome != NULL)
        free(_custome);
    if(_lineitem != NULL)
        free(_lineitem);
    if(_lineitem_index != NULL)
        free(_lineitem_index);
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
int charArrayToInt(char * input, char c , long int &num)
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

