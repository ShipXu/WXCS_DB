#include "io_process.h"


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


std::vector<size_t> _thr_customer_address = {0, 3872693, 7857089, 11841487, 
                                            15825733, 19981852, 24200926, 28420235, 
                                            32638866, 36857318, 41076181, 45294275, 
                                            49512892, 53732300, 57950648, 62169753, 
                                            66388615, 70607393, 74826611, 79046048, 
                                            83264286, 87483140, 91702254, 95921490, 
                                            100140155, 104359178, 108577665, 112796771, 
                                            117015578, 121234175, 125453148, 129672153, 
                                            133890604, 138109385, 142327960, 146546713, 
                                            150766070, 154984301, 159203016, 163421956, 
                                            167640541, 171859966, 176078843, 180375533, 
                                            184828816, 189282031, 193735421, 198188136, 
                                            202641237, 207095005, 211548237, 216001144, 
                                            220454408, 224907467, 229360616, 233814060, 
                                            238267385, 242720529, 247173846, 251627269, 
                                            256080380, 260533612, 264986930, 269439698, 
                                            273891567};

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

std::vector<size_t> _thr_lineitem_address = {0, 260196302, 530256056, 800938386, 
                                            1071622950, 1342304147, 1612986527, 1883667431, 
                                            2154351753, 2425034148, 2695718462, 2969544201, 
                                            3249602231, 3529660567, 3809719123, 4089773902, 
                                            4369832236, 4649888854, 4929947330, 5210006768, 
                                            5490064294, 5770120810, 6050177252, 6330235181, 
                                            6610294260, 6890353425, 7170411407, 7450469016, 
                                            7730526997, 8010585637, 8290644513, 8570702873, 
                                            8850759856, 9130818535, 9410878484, 9690937688, 
                                            9970994520, 10251052373, 10531110254, 10811169454, 
                                            11091226951, 11371285570, 11651344859, 11931402635,
                                            12211460899, 12491520128, 12771579299, 13051638437, 
                                            13331695959, 13611754263, 13891812380, 14171870855, 
                                            14451928522, 14731988064, 15012046667, 15292104787, 
                                            15572163472, 15852223225, 16132281031, 16412338993, 
                                            16692396976, 16972454759, 17252513492, 17532571198, 17812626596};




int GetThrIndex(int ithr, int nthr)
{
    assert(ithr < nthr);
    assert(64 % nthr == 0 && nthr != 0);
    int thr_per_amout = 64 / nthr;
    int thr_index = ithr * thr_per_amout;
    return thr_index;
}

size_t GetStartDivideCustomer(int ithr, int nthr)
{
    int thr_index = GetThrIndex(ithr, nthr);
    return _thr_customer_address[thr_index];
}

size_t GetEndDivideCustomer(int ithr, int nthr)
{
    return GetStartDivideCustomer(ithr + 1, nthr);
}

size_t GetStartDivideOrder(int ithr, int nthr)
{
    int thr_index = GetThrIndex(ithr, nthr);
    return _thr_order_address[thr_index];
}

size_t GetEndDivideOrder(int ithr, int nthr)
{
    return GetStartDivideOrder(ithr + 1, nthr);

}

size_t GetStartDivideLineitem(int ithr, int nthr)
{
    int thr_index = GetThrIndex(ithr, nthr);
    return _thr_lineitem_address[thr_index];
}

size_t GetEndDivideLineitem(int ithr, int nthr)
{
    return GetStartDivideLineitem(ithr + 1, nthr);
}

int GetStartRowid(int ithr, int nthr, int divide_row_amount)
{
    assert(ithr < nthr);
    assert(64 % nthr == 0 && nthr != 0);
    int thr_per_amount = 64 / nthr;
    int ithr_index =  ithr * thr_per_amount;
    int row_id = divide_row_amount * ithr_index + 1;
    return row_id;
}

int GetStartRowidCustomer(int ithr, int nthr)
{
    return GetStartRowid(ithr, nthr, 234376);
}

int GetEndRowidCustomer(int ithr, int nthr)
{
    if (ithr == nthr - 1)
        return 15000001;
    else
        return GetStartRowid(ithr+1, nthr, 234376);
}

int GetStartRowidOrder(int ithr, int nthr)
{
    return GetStartRowid(ithr, nthr, 2343750);
}

int GetEndRowidOrder(int ithr, int nthr)
{
    if (ithr == nthr - 1)
        return 150000001;
    else
        return GetStartRowidOrder(ithr + 1, nthr);
}

int GetStartRowidLineitem(int ithr, int nthr)
{
    return GetStartRowid(ithr, nthr, 9375593);
}

int GetEndRowidLineitem(int ithr, int nthr)
{
    if (ithr == nthr - 1)
        return 600037902;
    else
        return GetStartRowidLineitem(ithr + 1, nthr);
}


//
// FindChar
//
// Desc: find index of the first char equals 'c' and return it's index
//
// In:   input - the input char array
//       c - the char to be finded
// Out:  index - the index of first char equals 'c'
//            
//
int FindChar(char *input, char c)
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
// CharArrayToInt
//
// Desc: change the charArray from now to first char 'c' to long int 
//
// In:   input - the input charArray
//       c - the char to be finded
// Out:  index - the index of first char equals 'c'
//       num - the long int content of the charArray
//
int CharArrayToInt(char * input, char c , int &num)
{
    int index = 0; 
    char *pinput = input;
    num = 0;
    while (*pinput != c)
    {
        index += 1;
        num = num * 10 + (*pinput - '0');
        pinput += 1;
    }
    return index;
}


int GetDay(char* &s_date)
{
    int yy,mm,dd;
    yy = dd = mm = 0;
    int days = 0;
    yy = s_date[3] - '0';
    s_date += 5;

    for (int i = 0; i < 2; ++i)
    {
        mm = mm * 10 + *s_date - '0';
        s_date++;
    }
    s_date++;

    for (int i = 0; i < 2; ++i)
    {
        dd = dd * 10 + *s_date - '0';
        s_date++;
    }

    days = DateToDay(yy, mm, dd);
    return days;
}


//
// Desc: change the date to how many days from 1992-01-01 to input 
//
// In:   yy - which year
//       mm - which month
//       dd - which day
// Out:  days - how many days from 1992-01-01 to input 
//
int DateToDay(int yy, int mm, int dd)
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
void DaysToDate(int days, char *result)
{
    int leap = 0; 
    
    for (int year = 1992; year < 1999; ++year)
    {
        //Determine whether it is a leap year
        if (year == 1992 || year == 1996)
        {
            leap = 1;
        }
        else
        {
            leap = 0;
        };
       
        if (days <= MONTH_DAY[leap][0])
        {
            result[3] = (year % 10) + '0';
            for (int mm = 1; mm < 13; ++mm)
            {
                if (days <= MONTH_DAY[leap][mm])
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
void DateToInt(char *date, int &yy, int &mm, int &dd)
{
    yy = 0;
    mm = 0;
    dd = 0;
    // for (int i = 0; i < 4; ++i)
    // {
    //     yy = (yy * 10) + date[i] - '0';
    // }
    yy = date[3] - '0';
    for (int i = 5; i < 7; ++i)
    {
        mm = (mm * 10) + date[i] - '0';
    }
    for (int i = 8; i < 10; ++i)
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
int OidHash(int oid)
{
    int group_num = oid >> 5;
    group_num = group_num << 3;
    int key = group_num + oid % 8;
    return key;
}

int KeyToOid(int oid_key)
{
    int remainder = oid_key % 8;
    int group_num = oid_key >> 3;
    group_num  = group_num << 5;
    return remainder + group_num;
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
