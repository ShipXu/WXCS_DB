#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <vector>

using namespace std;

typedef struct{
    long id;
    char name[10];
} Customer;

char* _get_line(char* buf, int* len)
{
	char* tmp = buf;
	while(*tmp != '\n')
        ++tmp;
	*len = tmp - buf;
 
	if (*tmp == 0)
        return NULL;
 
    // skip New-Line char
    if (*tmp == 0x0d){ // Windows style New-Line 0x0d 0x0a
        tmp += 2;
    }//else Unix style New-Line 0x0a
    else{
       ++tmp;
    }
    return tmp;
}

int main()
{
    int fd = open("customer.txt", O_RDONLY);
    if (fd == -1) {
        printf("open:%m\n");
        exit(-1);
    }

    int filelen = lseek(fd, 0L, SEEK_END);
    printf("file length : %d;\n", filelen);

    char *mbuf = (char *) mmap(NULL, filelen, PROT_READ, MAP_PRIVATE, fd, 0);
    printf("open ok! can read;\n");

    // process data
    // vector<Customer> vec;
    // for (int i = 0; i < 5; i++){
    //     auto cus = new Customer();
    //     read(fd, &cus, sizeof(cus));
    //     vec.push_back(cus);
    // }
    printf("%s\n", mbuf);
    int len = 0;
    for (char *p = mbuf; *p && p - mbuf < filelen;) {
        char* str = _get_line(p, &len);
        printf("%s\n", str);
        p += len;
    }

    // close file
    close(fd);
    return 0;
}