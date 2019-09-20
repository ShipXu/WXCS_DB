//
// File:        pf_hashtable.h
// Description: PF_HashTable class interface
// Authors:     Hugo Rivero (rivero@cs.stanford.edu)
//              Dallan Quass (quass@cs.stanford.edu)
//

#ifndef PF_HASHTABLE_H
#define PF_HASHTABLE_H

#include "pf_internal.h"

//
// HashEntry - Hash table bucket entries
//
struct PF_HashEntry {
    PF_HashEntry *next;   // next hash table element or NULL
    PF_HashEntry *prev;   // prev hash table element or NULL
    int          fd;      // file descriptor
    PageNum      pageNum; // page number
    int          slot;    // slot of this page in the buffer
};

//
// PF_HashTable - allow search, insertion, and deletion of hash table entries
//
class PF_HashTable {
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

#endif
