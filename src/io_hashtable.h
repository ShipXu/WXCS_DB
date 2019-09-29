#ifndef IO_HASHTABLE_H
#define IO_HASHTABLE_H

//
// HashEntry - Hash table bucket entries
//
struct IO_hash {
    int strat_address;
    int len;
};

//
// PF_HashTable - allow search, insertion, and deletion of hash table entries
//
class IO_hash 
{
    public:
        PF_HashTable (int numBuckets);                 // Constructor
        ~PF_HashTable();                               // Destructor
        RC find (int fd, PageNum pageNum, int &slot);  // Set slot to the hash table entry for fd and pageNum
        RC insert (int fd, PageNum pageNum, int slot); // Insert a hash table entry
        RC erase (int fd, PageNum pageNum);            // Erase a hash table entry

    private:
        // Hash function
        int hash (int fd, PageNum pageNum) const
        {
            return ((fd + pageNum) % _numBuckets);
        }
        int _numBuckets;                               // Number of hash table buckets
        PF_HashEntry **_hashTable;                     // Hash table
};

#endif //IO_HASHTABLE_H
