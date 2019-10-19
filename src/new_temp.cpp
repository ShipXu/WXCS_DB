#include "assert.h"
// #include "db_io.h"
std::vector<size_t> _thr_customer_address = {0,1880938,3872693,5864702,
                                    7857089,9849104,11841487,13833542,
                                    15825733,17872741,19981852,22091292,
                                    24200926,26310801,28420235,30529753,
                                    32638866,34748078,36857318,38966856,
                                    41076181,43184995,45294275,47403689,
                                    49512892,51622606,53732300,55841372,
                                    57950648,60060075,62169753,64278911,
                                    66388615,68498320,70607393,72716888,
                                    74826611,76936615,79046048,81155161,
                                    83264286,85373944,87483140,89592720,
                                    91702254,93811880,95921490,98030946,
                                    100140155,102249729,104359178,106468498,
                                    108577665,110687009,112796771,114906289,
                                    117015578,119124856,121234175,123343498,
                                    125453148,127562534,129672153,131781460,
                                    133890604,135999901,138109385,140218660,
                                    142327960,144437324,146546713,148656507,
                                    150766070,152875051,154984301,157093769,
                                    159203016,161312413,163421956,165531008,
                                    167640541,169750436,171859966,173969484,
                                    176078843,178187837,180375533,182602387,
                                    184828816,187055349,189282031,191508930,
                                    193735421,195961489,198188136,200414728,
                                    202641237,204868204,207095005,209321683,
                                    211548237,213774772,216001144,218227620,
                                    220454408,222680839,224907467,227133922,
                                    229360616,231587397,233814060,236040701,
                                    238267385,240493701,242720529,244947053,
                                    247173846,249400473,251627269,253853530,
                                    256080380,258306993,260533612,262760370,
                                    264986930,267213274,269439698,271666284,273891567};

std::vector<size_t> _thr_order_address = {0, 63612070, 129688089, 195920356, 
                                        262154166, 328384205, 394615985, 460848888, 
                                        527081411, 593314921, 659546980, 726560638, 
                                        795137209, 863714656, 932291564, 1000867368, 
                                        1069444407, 1138021345, 1206597422, 1275173789, 
                                        1343751323, 1412327166, 1480905424, 1549484325, 
                                        1618061615, 1686639011, 1755214347, 1823789834, 
                                        1892367012, 1960942716, 2029518945, 2098095685, 
                                        2166670580, 2235247154, 2303822463, 2372399775, 
                                        2440975332, 2509551303, 2578127614, 2646704570, 
                                        2715280667, 2783857949, 2852434744, 2921011679, 
                                        2989587726, 3058164173, 3126740542, 3195317745, 
                                        3263893972, 3332470542, 3401047767, 3469624954, 
                                        3538201160, 3606778031, 3675354637, 3743930982, 
                                        3812507997, 3881084813, 3949662111, 4018238227, 
                                        4086814786, 4155392494, 4223968904, 4292543890, 4361120079};

std::vector<size_t> _thr_lineitem_address = {0, 260179798, 530221796, 800887011, 
                                            1071554457, 1342218546, 1612883773, 1883547616, 
                                            2154214764, 2424880104, 2695547262, 2969349332, 
                                            3249389699, 3529430250, 3809471087, 4089508200, 
                                            4369548815, 4649587742, 4929628512, 5209670281, 
                                            5489710062, 5769748883, 6049787551, 6329827787, 
                                            6609869151, 6889910530, 7169950790, 7449990718, 
                                            7730030997, 8010071935, 8290113092, 8570153653, 
                                            8850193060, 9130234043, 9410276210, 9690317786, 
                                            9970356835, 10250396943, 10530437217, 10810478495, 
                                            11090518375, 11370559315, 11650600897, 11930640841, 
                                            12210681598, 12490722938, 12770764541, 13050805908, 
                                            13330845705, 13610886324, 13890926654, 14170967431, 
                                            14451007439, 14731049331, 15011090160, 15291130595, 
                                            15571171452, 15851213642, 16131253655, 16411293846, 
                                            16691334154, 16971374429, 17251415364, 17531455146,17812626596};

IO_Process::IO_Process()
{
    //resize the vector to become a fixed length array
    _b_date.resize(2408);
    _a_date.resize(2408);
    _m_date.resize(2408);
    _f_date.resize(2408);
    _h_date.resize(2408);
}

int GetOrderDay(char* &s_date)
{
    int yy,mm,dd;
    yy = dd = mm = 0;

    yy = s_date[3] - '0';
    s_date += 5;

    for(int i = 0; i < 2; ++i)
    {
        mm = mm * 10 + *s_date - '0';
        s_date++;
    }
    s_date++;

    for(int i = 0; i < 2; ++i)
    {
        dd = dd * 10 + *pcontent - '0';
        s_date++;
    }

    days = dateToDay(yy, mm, dd);
    return days;
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
int IO_Process::processOrder(char *custome_file, int cust_num, char *order_file, size_t work_amount)
{
    if(-1 == mapCustomeData(custome_file, cust_num) )
    {
        return -1;
    }

    IO_Mmap io_mmap(order_file);
    const char *pcontent = io_mmap.getData();

    // printf("read order start\n");
    auto ker = [&](const int ithr, const int nthr)
    {
        size_t start = get_start_divide_order(ithr, nthr);

        int start_row_id = get_start_rowid_order(ithr, nthr);
        int end_row_id = get_end_rowid_order(ithr, nthr);

        auto t_pcontent = pcontent + start;
        size_t item_len{0};
        int department_type = -1;
        int order_id = -1;

        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = charArrayToInt(t_pcontent, '|', order_id);
            t_pcontent += item_len + 1;  // pcontent go to the next char of '|'

            item_len = charArrayToInt(t_pcontent , '|', customer_id);
            t_pcontent += item_len + 1; // pcontent go to the next char of '|'
    
            department_type = _customer[customer_id];       //depend on custome_id , get the department_type

            days = GetOrderDay(t_pcontent);
            addRecord(department_type, days, order_id);

            t_pcontent++; // pcontent go to the next char of '\n'
            if (*t_pcontent == '\0')
                break;
        }
    };

    #pragma omp parallel num_threads(8)
    {
        ker(omp_get_thread_num(), 8);
    }

    _order_size = work_amount;
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
int IO_Process::processLineitem(char *file, size_t work_amount)
{
    IO_Mmap io_mmap(file);
    const char *pcontent = io_mmap.getData();

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
    memset(_oid_to_address, -1, sizeof(int)*(_order_size + 1));


    _limeitem_index_size = 1;

    int len1;
    int num, pre_orderid;
    double price = 0;
    //LineitemAddress temp;
    pre_orderid = -1;

    auto ker = [&](const int ithr, const int nthr)
    {
        size_t start = get_start_divide_order(ithr, nthr);

        int start_row_id = get_start_rowid_order(ithr, nthr);
        int end_row_id = get_end_rowid_order(ithr, nthr);

        auto t_pcontent = pcontent + start;
        Lineitem *p_lineitem = _lineitem + row_id - 1;
        size_t item_len{0};
        int order_id = -1;
        int pre_orderid = -1;
        double price = 0;
        int price_part = 0;

        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = charArrayToInt(t_pcontent, '|', order_id);

            if (pre_orderid != order_id && _limeitem_index_size < _order_size)
            {
                _oid_to_address[_limeitem_index_size] = row_id;
                pre_orderid = order_id;
                _limeitem_index_size += 1;
            }

            p_lineitem->order_id = order_id;
            t_pcontent += item_len + 1;  // pcontent go to the next char of '|'

            item_len = charArrayToInt(t_pcontent, '.', price_part);
            price = price_part;
            t_pcontent += item_len + 1;

            item_len = charArrayToInt(t_pcontent, '|', price_part);
            price += ((double)price_part / 100.0);
            t_pcontent += item_len + 1;
            p_lineitem->price = price;

            p_lineitem->date = 0;
            for(int i = 0; i < 4; ++i)
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

    _limeitem_size = size;
    return 0;
};

size_t get_start_divide_customer(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    size_t index = 0;
    int thr_amout = 128 / nthr;
    index = _thr_customer_address[ithr * thr_amout];
    return index;
}

size_t get_end_divide_customer(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    size_t index = 0;
    int thr_amout = 128 / nthr;
    index = _thr_customer_address[(ithr + 1) * thr_amout];
    return index;
}

int get_start_rowid_customer(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    int divide_row_amount = 117188;
    int thr_amount = 128 / nthr;
    int this_amount =  ithr * thr_amount;
    int row_id = divide_row_amount * this_amount + 1;
    return row_id;
}

int get_end_rowid_customer(int ithr, int nthr)
{
    assert(ithr < nthr);
    if(ithr == nthr - 1)
        return 15000001;
    else
        return get_start_rowid(ithr + 1, nthr);
}

size_t get_start_divide_order(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    size_t index = 0;
    int thr_amout = 64 / nthr;
    index = _thr_order_address[ithr * thr_amout];
    return index;
}

size_t get_end_divide_order(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    size_t index = 0;
    int thr_amout = 128 / nthr;
    index = _thr_order_address[(ithr + 1) * thr_amout];
    return index;
}

int get_start_rowid_order(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    int divide_row_amount = 2343750;
    int thr_amount = 64 / nthr;
    int this_amount =  ithr * thr_amount;
    int row_id = divide_row_amount * this_amount + 1;
    return row_id;
}

int get_end_rowid_order(int ithr, int nthr)
{
    assert(ithr < nthr);
    if(ithr == nthr - 1)
        return 150000001;
    else
        return get_start_rowid(ithr + 1, nthr);
}

size_t get_start_divide_lineitem(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    size_t index = 0;
    int thr_amout = 128 / nthr;
    index = _thr_address[ithr * thr_amout];
    return index;
}

size_t get_end_divide_lineitem(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    size_t index = 0;
    int thr_amout = 128 / nthr;
    index = _thr_address[(ithr + 1) * thr_amout];
    return index;
}

int get_start_rowid_lineitem(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(nthr % 8 == 0 && nthr != 0);
    int divide_row_amount = 117188;
    int thr_amount = 128 / nthr;
    int this_amount =  ithr * thr_amount;
    int row_id = divide_row_amount * this_amount + 1;
    return row_id;
}

int get_end_rowid_lineitem(int ithr, int nthr)
{
    assert(ithr < nthr);
    if(ithr == nthr - 1)
        return 15000001;
    else
        return get_start_rowid(ithr + 1, nthr);
}

int get_index_cutomer(const char first_item, size_t& d_size)
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
// mapCustomeData
//
// Desc: map custome table's custome_id to it's department
//
// In:   file - the path of file
//       work_amount - number of customes in the file
// Out:  _custome - a table map custome table's custome_id to it's department
//            
//
int IO_Process::mapCustomeData(char *file, size_t work_amount)
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
        size_t start = get_start_divide_customer(ithr, nthr);
        
        int start_row_id = get_start_rowid(ithr, nthr);
        int end_row_id = get_end_rowid(ithr, nthr);

        auto t_pcontent = pcontent + start;
        size_t item_len{0};

        //printf("%d %d\n", start_row_id, end_row_id);
        for (int row_id = start_row_id; row_id < end_row_id; row_id++)
        {
            item_len = findChar(t_pcontent, '|');
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
void IO_Process::addRecord(int department_type, int days,int num1)
{
    switch(department_type)
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

int* IO_Process::getCustome() 
{
    return _customer;
};

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
    if(_customer != NULL)
        free(_customer);
    if(_lineitem != NULL)
        free(_lineitem);
    if(_oid_to_address != NULL)
        free(_oid_to_address);
    // if(_lineitem_index != NULL)
    //     free(_lineitem_index);
}



//
// findChar
//
// Desc: find index of the first char equals 'c' and return it's index
//
// In:   input - the input char array
//       c - the char to be finded
// Out:  index - the index of first char equals 'c'
//            
//
int findChar(char *input, char c)
{
    int index = 0; 
    char *ptr_input = input;
    while (*ptr_input != c)
    {
        ptr_input += 1;
        index += 1;
    }
    return index;
}

//
// charArrayToInt
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
        num = num * 10 + (*pinput - '0');
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
    int year = yy - 2;
    int days = dd + year * 365;
    if (year >= 5)
    {
        days += 2;
    } 
    else if (year >= 1) 
    {   
        days += 1;
    }

    int leap = 0;
    // Determine whether it is a leap year
    if (yy == 2 || yy == 6)
    {
        leap = 1;
    }

    for (int i = 1; i < mm; ++i)
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