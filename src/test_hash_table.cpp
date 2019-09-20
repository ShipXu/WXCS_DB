#include <vector>
#include <iostream>
#include "pf_hashtable.h"

using namespace std;
int main ()
{
    PF_HashTable hashtable(23);
    vector<int> fds;

    for (int i = 0; i <= 20; i++)
    {
        int fd = (rand() % 1000);
        fds.push_back(fd);
        hashtable.insert(fd, i, i + 10);
    }

    int slot = -1;
    if (!hashtable.find(fds[10], 10, slot))
    {
        cout << "founded! " << endl;
        cout << "located in the slot " << slot << endl;
    }
    else
    {
        cout << "Not founded!" << endl;
    }

    hashtable.erase(fds[10], 10);
    if (!hashtable.find(fds[10], 10, slot))
    {
        cout << "founded! " << endl;
        cout << "located in the slot " << slot << endl;
    }
    else
    {
        cout << "Not founded!" << endl;
    }

    return 0;
}