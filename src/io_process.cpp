
#include "io_process.h"

static omp_lock_t lock;

IO_Process::IO_Process()
{
    //resize the vector to become a fixed length array
    _b_date.resize(2408);
    _a_date.resize(2408);
    _m_date.resize(2408);
    _f_date.resize(2408);
    _h_date.resize(2408);
}

template <typename T>
void IO_Process::_MergeV(T &v1, const T &v2)
{
    for (size_t i = 0; i < v1.size(); i++)
    {
        v1[i].insert(v1[i].end(), v2[i].begin(), v2[i].end());
    }
}

void IO_Process::MergeRecord(std::vector< std::vector<int> > &t_b_date,
                 std::vector< std::vector<int> > &t_a_date,
                 std::vector< std::vector<int> > &t_m_date,
                 std::vector< std::vector<int> > &t_h_date,
                 std::vector< std::vector<int> > &t_f_date)
{
    omp_set_lock(&lock);

    _MergeV(_b_date, t_b_date);
    _MergeV(_a_date, t_a_date);
    _MergeV(_m_date, t_m_date);
    _MergeV(_h_date, t_h_date);
    _MergeV(_f_date, t_f_date);

    omp_unset_lock(&lock);
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

    // printf("read order start\n");
    auto ker = [&](const int ithr, const int nthr)
    {
        size_t start = GetStartDivideOrder(ithr, nthr);

        int start_row_id = GetStartRowidOrder(ithr, nthr);
        int end_row_id = GetEndRowidOrder(ithr, nthr);
    
        auto t_pcontent = pcontent + start;
        size_t item_len{0};
        int department_type = -1;
        int order_id = -1;
        int customer_id = -1;
        int days = -1;

        std::vector< std::vector<int> > t_b_date(2408);  // the second char represent department;
        std::vector< std::vector<int> > t_a_date(2408);
        std::vector< std::vector<int> > t_m_date(2408);
        std::vector< std::vector<int> > t_h_date(2408);
        std::vector< std::vector<int> > t_f_date(2408);

        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = CharArrayToInt(t_pcontent, '|', order_id);
            t_pcontent += item_len + 1;  // pcontent go to the next char of '|'

            item_len = CharArrayToInt(t_pcontent , '|', customer_id);
            t_pcontent += item_len + 1; // pcontent go to the next char of '|'
    
            department_type = _customer[customer_id];       //depend on custome_id , get the department_type

            days = GetOrderDay(t_pcontent);
            switch(department_type)
            {
                case 0:
                    t_b_date[days].push_back(order_id);
                    break;
                case 1:
                    t_a_date[days].push_back(order_id);
                    break;
                case 2:
                    t_m_date[days].push_back(order_id);
                    break;
                case 3:
                    t_f_date[days].push_back(order_id);
                    break;
                case 4:
                    t_h_date[days].push_back(order_id);
                    break;
                default:
                    break;
            }
            t_pcontent++; // pcontent go to the next char of '\n'
            if (*t_pcontent == '\0')
                break;
        }

        MergeRecord(t_b_date, t_a_date, t_m_date, t_h_date, t_f_date);
    };

    #pragma omp parallel num_threads(8)
    {
        ker(omp_get_thread_num(), 8);
    }
    // ker(7, 8);

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

    _lineitem = (Lineitem*)malloc(sizeof(Lineitem)*work_amount);
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
    memset(_oid_to_address, -1, sizeof(int)*(_order_size + 1));

    int len1;
    int num, pre_orderid;
    double price = 0;
    //LineitemAddress temp;
    pre_orderid = -1;
   
     
    auto ker = [&](const int ithr, const int nthr)
    {
        size_t start = GetStartDivideLineitem(ithr, nthr);

        int start_row_id = GetStartRowidLineitem(ithr, nthr);
        int end_row_id = GetEndRowidLineitem(ithr, nthr);

        auto t_pcontent = pcontent + start;
        
        Lineitem *p_lineitem = _lineitem + start_row_id - 1;
        size_t item_len{0};
        int order_id = -1;
        int pre_orderid = -1;
        double price = 0;
        int price_part = 0;
        int oidHash = -1;
        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = CharArrayToInt(t_pcontent, '|', order_id);
           
            if (pre_orderid != order_id)//&& oidHash < _order_size)
            {
                oidHash = OidHash(order_id);
                if (_oid_to_address[oidHash] == -1)
                {
                    _oid_to_address[oidHash] = row_id;
                }else if (row_id < _oid_to_address[oidHash])
                {
                    _oid_to_address[oidHash] = row_id;
                }
                
                pre_orderid = order_id;
            }

            p_lineitem->order_id = order_id;
            t_pcontent += item_len + 1;  // pcontent go to the next char of '|'

            item_len = CharArrayToInt(t_pcontent, '.', price_part);
            price = price_part;
            t_pcontent += item_len + 1;

            item_len = CharArrayToInt(t_pcontent, '|', price_part);
            price += ((double)price_part / 100.0);
            t_pcontent += item_len + 1;
            p_lineitem->price = price;

            p_lineitem->date = 0;
            // for(int i = 0; i < 4; ++i)
            // {
            //     p_lineitem->date = p_lineitem->date * 10 + *t_pcontent - '0';
            //     t_pcontent++;
            // }
             p_lineitem->date = p_lineitem->date * 10 + t_pcontent[3] - '0';
            t_pcontent += 5;
            for(int i = 0; i < 2; ++i)
            {
                p_lineitem->date = p_lineitem->date * 10 + *t_pcontent - '0';
                t_pcontent++;
            }
            t_pcontent++;
            for(int i = 0; i < 2; ++i)
            {
                p_lineitem->date = p_lineitem->date * 10 + *t_pcontent - '0';
                t_pcontent++;
            }

            t_pcontent++; // pcontent go to the next char of '\n'

            if (*t_pcontent == '\0')
                break;

            p_lineitem += 1;
        }
    };

    #pragma omp parallel num_threads(8)
    {
        ker(omp_get_thread_num(), 8);
    }

    _limeitem_size = work_amount;
    return 0;
};


int IO_Process::get_index_cutomer(const char first_item, size_t& d_size)
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
    
    _customer = (int*)malloc(sizeof(int)*(work_amount + 1));

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
            int temp = get_index_cutomer(*t_pcontent, item_len);
            _customer[row_id] = temp;

            //printf("%d %d\n", row_id, temp);
            t_pcontent += item_len + 1; // jump over the department item
            if (*t_pcontent == '\0')
                break;
        }
    };

    #pragma omp parallel num_threads(8)
    {
        ker(omp_get_thread_num(), 8);
    }

    _customer_size = work_amount;
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
void IO_Process::AddRecord(int department_type, int days, int order_id)
{
    omp_set_lock(&lock);
    switch(department_type)
    {
        case 0:
            _b_date[days].push_back(order_id);
            break;
        case 1:
            _a_date[days].push_back(order_id);
            break;
        case 2:
            _m_date[days].push_back(order_id);
            break;
        case 3:
            _f_date[days].push_back(order_id);
            break;
        case 4:
            _h_date[days].push_back(order_id);
            break;
        default:
            break;
    }
    omp_unset_lock(&lock);
}

int* IO_Process::get_custome() 
{
    return _customer;
};

int* IO_Process::get_oid_to_address() 
{
    return _oid_to_address;
};

Lineitem* IO_Process::get_lineitem() 
{
    return _lineitem;
}

size_t IO_Process::get_oid_to_address_size() 
{
    return _oid_to_address_size;
}    


size_t IO_Process::get_limeitem_size() 
{
    return _limeitem_size;
}

std::vector< std::vector<int> >  IO_Process::get_b_date() 
{
    return _b_date;
}

std::vector< std::vector<int> >  IO_Process::get_a_date() 
{
    return _a_date;
}

std::vector< std::vector<int> >  IO_Process::get_m_date() 
{
    return _m_date;
}

std::vector< std::vector<int> >  IO_Process::get_h_date() 
{
    return _h_date;
}

std::vector< std::vector<int> >  IO_Process::get_f_date() 
{
    return _f_date;
}

IO_Process::~IO_Process()
{
    // if(_order != NULL)
    //     free(_order);
    if(_customer != NULL)
        free(_customer);
    if(_lineitem != NULL)
        free(_lineitem);
    if(_oid_to_address != NULL)
        free(_oid_to_address);
    // if(_lineitem_index != NULL)
    //     free(_lineitem_index);
}


