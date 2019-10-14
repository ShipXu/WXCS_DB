
#include "io_process.h"

static omp_lock_t lock;

IO_Process::IO_Process()
{

}

template <typename T>
void IO_Process::_MergeV(T &v1, const T &v2)
{
    for (size_t i = 0; i < v1.size(); i++)
    {
        v1[i].insert(v1[i].end(), v2[i].begin(), v2[i].end());
    }
}

template <typename T>
void IO_Process::MaxVectorSize(T &t_b_date, T &t_a_date,
                 T &t_m_date, T &t_h_date,T &t_f_date)
{
    int max_size = 0;
    for (size_t i = 0; i < t_b_date.size(); ++i)
    {
        if (max_size < t_b_date[i].size())
        {
            max_size = t_b_date[i].size();
        }
    }
    for (size_t i = 0; i < t_a_date.size(); ++i)
    {
        if (max_size < t_a_date[i].size())
        {
            max_size = t_a_date[i].size();
        }
    }
    for (size_t i = 0; i < t_m_date.size(); ++i)
    {
        if (max_size < t_m_date[i].size())
        {
            max_size = t_m_date[i].size();
        }
    }
    for (size_t i = 0; i < t_f_date.size(); ++i)
    {
        if (max_size < t_f_date[i].size())
        {
            max_size = t_f_date[i].size();
        }
    }
    for (size_t i = 0; i < t_h_date.size(); ++i)
    {
        if (max_size < t_h_date[i].size())
        {
            max_size = t_h_date[i].size();
        }
    }
    printf("max_size:%d\n", max_size);
}

void IO_Process::MaxDaysSize(uint32_t *day_size, int length)
{
    uint32_t max_size = 0;
    for (int i = 0; i < length; ++i)
    {
        if (max_size < day_size[i])
        {
            max_size = day_size[i];
        }
        //printf("day_size[i]:%d\n", day_size[i]);
    }
    printf("max_size:%d\n", max_size);
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
int IO_Process::ProcessOrder(const char *custome_file, size_t cust_work_amount, const char *order_file, size_t work_amount)
{
    if(-1 == MapCustomerData(custome_file, cust_work_amount) )
    {
        return -1;
    }

    IO_Mmap io_mmap(order_file);
    char *pcontent = io_mmap.getData();

    _oid_department = (int8_t*)malloc(sizeof(int8_t)*(work_amount+1));
    if (NULL == _oid_department)
    {
        printf("error:fail to allocate memory to _oid_department\n");
        return -1;
    }
    memset(_oid_department, -1, sizeof(int8_t)*(work_amount + 1));
    
    _oid_key_date = (int*)malloc(sizeof(int)*(DEPARTMENT_ORDER_SIZE * 5));

    if (NULL == _oid_key_date)
    {
        printf("error:fail to allocate memory to _oid_key_date\n");
        return -1;
    }
    //memset(_oid_key_date, -1, sizeof(int)*(DEPARTMENT_ORDER_SIZE*5));

    // printf("read order start\n");
    auto ker = [&](const int ithr, const int nthr)
    {
        uint32_t day_size[ORDER_DAYS * 5];
        memset(day_size, 0, sizeof(uint32_t) * (ORDER_DAYS * 5));
        //printf("ProcessOrder done\n");
        size_t start = GetStartDivideOrder(ithr, nthr);

        int start_row_id = GetStartRowidOrder(ithr, nthr);
        int end_row_id = GetEndRowidOrder(ithr, nthr);
    
        auto t_pcontent = pcontent + start;
        size_t item_len{0};
        int8_t department_type = -1;
        int order_id = -1;
        int customer_id = -1;
        int days = -1;
        int oid_key = 0;
        size_t department_address = 0;
        size_t days_address = 0;
        size_t ithr_address = 0;
        size_t this_day_size_address = 0;
        size_t this_order_index = 0;
        
        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = CharArrayToInt(t_pcontent, '|', order_id);
            t_pcontent += item_len + 1;  // pcontent go to the next char of '|'

            item_len = CharArrayToInt(t_pcontent , '|', customer_id);
            t_pcontent += item_len + 1; // pcontent go to the next char of '|'
    
            department_type = _customer[customer_id];       //depend on custome_id , get the department_type
            oid_key = OidHash(order_id);
            _oid_department[oid_key] = department_type;

            days = GetDay(t_pcontent);
            
            department_address = department_type * DEPARTMENT_ORDER_SIZE;
            days_address = days * DAYS_OREDER_SIZE * nthr;
            ithr_address = ithr * DAYS_OREDER_SIZE;
            this_day_size_address = (department_type * ORDER_DAYS) + days;
            this_order_index = department_address + days_address + ithr_address + day_size[this_day_size_address];

            // // printf("%d, %d, %d, %uz\n", department_type, days, day_size[days], index);
            _oid_key_date[this_order_index] = oid_key;
            day_size[this_day_size_address]++;
            t_pcontent++; // pcontent go to the next char of '\n'
            if (*t_pcontent == '\0')
                break;
        }
        //MaxDaysSize(day_size, ORDER_DAYS * 5);
        //MergeOrderRecord(t_b_oid_key_date, t_a_oid_key_date, t_m_oid_key_date, t_h_oid_key_date, t_f_oid_key_date);
    };

    #pragma omp parallel num_threads(THREAD_SIZE)
    {
        ker(omp_get_thread_num(), THREAD_SIZE);
    }
    // ker(7, 8);
    //MaxVectorSize(_b_oid_key_date, _a_oid_key_date, _m_oid_key_date, _h_oid_key_date, _f_oid_key_date);
    printf("ProcessOrder done\n");
    
    _order_size = work_amount;
    return 0;
};

//
// ProcessLineitem
//
// Desc: get the data of LineitemTable and build an index for it
//
// In:   file - the path of file
//       number - number of orders in the file
// Out:  _lineitem - the initial address of lineitem table data
//       _lineitem_index - the initial address of lineitem table index
//
int IO_Process::ProcessLineitem(const char *file, size_t work_amount)
{
    IO_Mmap io_mmap(file);
    char *pcontent = io_mmap.getData();
    int max_size = 0;
    _lineitem_date_oid_key = (int*)malloc(sizeof(int) * DEPARTMENT_LINEITEM_SIZE * 5);
    if (NULL == _lineitem_date_oid_key)
    {
        printf("error:fail to allocate memory to _lineitem_date_oid_key\n");
        return -1;
    }
    _lineitem_date_price = (double*)malloc(sizeof(double) * DEPARTMENT_LINEITEM_SIZE * 5);
    if (NULL == _lineitem_date_price)
    {
        printf("error:fail to allocate memory to _lineitem_date_price\n");
        return -1;
    }

    auto ker = [&](const int ithr, const int nthr)
    {
        uint32_t day_size[LINEITEM_DAYS * 5];
        memset(day_size, 0, sizeof(uint32_t) * (LINEITEM_DAYS * 5));

        size_t start = GetStartDivideLineitem(ithr, nthr);

        int start_row_id = GetStartRowidLineitem(ithr, nthr);
        int end_row_id = GetEndRowidLineitem(ithr, nthr);

        auto t_pcontent = pcontent + start;

        //Lineitem this_lineitem;
        size_t item_len{0};
        int order_id = -1;
        int oid_key = 0;
        double price = 0;
        int price_part = 0;
        int oidHash = -1;
        int8_t department_type = -1;
        int days = 0;
        bool flag = true;
        size_t department_address = 0;
        size_t days_address = 0;
        size_t ithr_address = 0;
        size_t this_day_size_address = 0;
        size_t this_lineitem_index = 0;
       
        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = CharArrayToInt(t_pcontent, '|', order_id);
            oid_key = OidHash(order_id);
            t_pcontent += item_len + 1;  // pcontent go to the next char of '|'

            item_len = CharArrayToInt(t_pcontent, '.', price_part);
            price = price_part;
            t_pcontent += item_len + 1;

            item_len = CharArrayToInt(t_pcontent, '|', price_part);
            price += ((double)price_part / 100.0);
            t_pcontent += item_len + 1;
            
            department_type = _oid_department[oid_key];
            days = GetDay(t_pcontent);
            // if(flag)
            // {
            //     printf("department_type:%d\n", department_type);
            //     flag = false;
            // }

            department_address = department_type * DEPARTMENT_LINEITEM_SIZE;
            days_address = days * DAYS_LINEITEM_SIZE * nthr;
            ithr_address = ithr * DAYS_LINEITEM_SIZE;
            this_day_size_address = (department_type * LINEITEM_DAYS) + (days);
            this_lineitem_index = department_address + days_address + ithr_address + day_size[this_day_size_address];//department_address + days_address + day_size[this_day_size_address];
            //printf("%d, %d, %d, %ld\n", department_type, days, day_size[this_day_size_address], this_lineitem_index);
            _lineitem_date_oid_key[this_lineitem_index] = oid_key;
            _lineitem_date_price[this_lineitem_index] = price;
            day_size[this_day_size_address]++;

            t_pcontent++;
            if (*t_pcontent == '\0')
                break;
        }
        //MaxDaysSize(day_size, LINEITEM_DAYS * 5);
        
        // MergeLineitemRecord(t_b_lineitem_date, t_a_lineitem_date, t_m_lineitem_date, t_h_lineitem_date, t_f_lineitem_date);
    };
    printf("ProcessLineitem done\n");
    #pragma omp parallel num_threads(THREAD_SIZE)
    {
        ker(omp_get_thread_num(), THREAD_SIZE);
    }
    return 0;
};


int8_t IO_Process::get_index_cutomer(const char first_item, size_t& d_size)
{
    if (first_item == 'B')
    {
        d_size = 8;
        return 0;
    }
    else if (first_item == 'A')
    {
        d_size = 10;
        return 1;
    }
    else if (first_item == 'M')
    {
        d_size = 9;
        return 2;
    }
    else if (first_item == 'H')
    {
        d_size = 9;
        return 3;
    }
    else if (first_item == 'F')
    {
        d_size = 9;
        return 4;
    }
    else
        throw "customer doesn't belong to any department";
}

//
// MapCustomerData
//
// Desc: map customer table's customer_id to it's department
//
// In:   file - the path of file
//       work_amount - number of customes in the file
// Out:  _custome - a table map custome table's custome_id to it's department
//            
//
int IO_Process::MapCustomerData(const char *file, size_t work_amount)
{
    IO_Mmap io_mmap(file);
    char *pcontent = io_mmap.getData();
    
    _customer = (int8_t*)malloc(sizeof(int8_t)*(work_amount + 1));

    if (NULL == _customer)
    {
        printf("error:fail to allocate memory to _customer\n");
        return -1;
    }

    memset((void*)_customer, -1, work_amount + 1);
    auto ker = [&](const int ithr, const int nthr)
    {
        size_t start = GetStartDivideCustomer(ithr, nthr);
        
        int start_row_id = GetStartRowidCustomer(ithr, nthr);
        int end_row_id = GetEndRowidCustomer(ithr, nthr);

        auto t_pcontent = pcontent + start;
        size_t item_len{0};

        //printf("%d %d\n", start_row_id, end_row_id);
        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = FindChar(t_pcontent, '|');
            t_pcontent += item_len + 1; // pcontent go to the next char of '|'

            // get the type of department by its first letter
            // set _customer[row_id] = index of its type,
            // ,and change item_len to the department length 
            int8_t temp = get_index_cutomer(*t_pcontent, item_len);
            _customer[row_id] = temp;

            //printf("%d %d\n", row_id, temp);
            t_pcontent += item_len + 1; // jump over the department item
            if (*t_pcontent == '\0')
                break;
        }
    };

    #pragma omp parallel num_threads(THREAD_SIZE)
    {
        ker(omp_get_thread_num(), THREAD_SIZE);
    }
    printf("MapCustomerData done\n");
    _customer_size = work_amount;
    return 0;
};


IO_Process::~IO_Process()
{
    if(_customer != NULL)
        free(_customer);
    if(_oid_department != NULL)
        free(_oid_department);
    if(_oid_key_date != NULL)
        free(_oid_key_date);
    if(_lineitem_date_oid_key != NULL)
        free(_lineitem_date_oid_key);
    if(_lineitem_date_price != NULL)
        free(_lineitem_date_price);
}


