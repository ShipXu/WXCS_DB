//
// File:        pf_hashtable.cc
// Description: PF_HashTable class implementation
// Authors:     Hugo Rivero (rivero@cs.stanford.edu)
//              Dallan Quass (quass@cs.stanford.edu)
//

#include "pf_internal.h"
#include "pf_hashtable.h"

//
// PF_HashTable
//
// Desc: Constructor for PF_HashTable object, which allows search, insert,
//       and delete of hash table entries.
// In:   numBuckets - number of hash table buckets
//
PF_HashTable::PF_HashTable(int _numBuckets)
{
    // Initialize numBuckets local variable from parameter
    this->_numBuckets = _numBuckets;

    // Allocate memory for hash table
    _hashTable = new PF_HashEntry* [_numBuckets];

    // Initialize all buckets to empty
    for (int i = 0; i < _numBuckets; i++)
    {
        _hashTable[i] = NULL;
    }
}

//
// ~PF_HashTable
//
// Desc: Destructor
//
PF_HashTable::~PF_HashTable()
{
    // Clear out all buckets
    for (int i = 0; i < _numBuckets; i++)
    {
        // Delete all entries in the bucket
        PF_HashEntry *entry = _hashTable[i];
        while (entry != NULL)
        {
            PF_HashEntry *next = entry->next;
            delete entry;
            entry = next;
        }
    }
    // Finally delete the hash table
    delete[] _hashTable;
}

//
// find
//
// Desc: Find a hash table entry.
// In:   fd - file descriptor
//       pageNum - page number
// Out:  slot - set to slot associated with fd and pageNum
// Ret:  PF return code
//
RC PF_HashTable::find(int fd, PageNum pageNum, int &slot)
{
    // Get which bucket it should be in
    int bucket_num = hash(fd, pageNum);
    if (bucket_num < 0)
    {
        return (PF_HASHNOTFOUND);
    }

    // Go through the linked list of this bucket
    for (PF_HashEntry *entry = _hashTable[bucket_num]; entry != NULL;
         entry = entry->next)
    {
        if (entry->fd == fd && entry->pageNum == pageNum)
        {
            // Found it
            slot = entry->slot;
            return (0);
        }
    }

    // Didn't find it
    return (PF_HASHNOTFOUND);
}

//
// insert
//
// Desc: Insert a hash table entry
// In:   fd - file descriptor
//       pagenum - page number
//       slot - slot associated with fd and pageNum
// Ret:  PF return code
//
RC PF_HashTable::insert(int fd, PageNum pageNum, int slot)
{
    // Get which bucket it should be in
    int bucket_num = hash(fd, pageNum);

    // Check entry doesn't already exist in the bucket
    PF_HashEntry *entry;
    for (entry = _hashTable[bucket_num]; entry != NULL; entry = entry->next)
    {
        if (entry->fd == fd && entry->pageNum == pageNum)
        {
            return (PF_HASHPAGEEXIST);
        }
    }

    // Allocate memory for new hash entry
    entry = new PF_HashEntry;
    if (entry == NULL)
    {
        return (PF_NOMEM);
    }

    // Insert entry at head of list for this bucket
    entry->fd = fd;
    entry->pageNum = pageNum;
    entry->slot = slot;
    entry->next = _hashTable[bucket_num];
    entry->prev = NULL;
    if (_hashTable[bucket_num] != NULL)
    {
        _hashTable[bucket_num]->prev = entry;
    }
    _hashTable[bucket_num] = entry;

    // Return ok
    return (0);
}

//
// erase
//
// Desc: Delete a hash table entry
// In:   fd - file descriptor
//       pagenum - page number
// Ret:  PF return code
//
RC PF_HashTable::erase(int fd, PageNum pageNum)
{
    // Get which bucket it should be in
    int bucket_num = hash(fd, pageNum);

    // Find the entry is in this bucket
    PF_HashEntry *entry;
    for (entry = _hashTable[bucket_num]; entry != NULL; entry = entry->next)
    {
        if (entry->fd == fd && entry->pageNum == pageNum)
            break;
    }

    // Did we find hash entry?
    if (entry == NULL)
        return (PF_HASHNOTFOUND);

    // Remove this entry
    if (entry == _hashTable[bucket_num])
    {
        _hashTable[bucket_num] = entry->next;
    }
    if (entry->prev != NULL)
    {
        entry->prev->next = entry->next;
    }
    if (entry->next != NULL)
    {
        entry->next->prev = entry->prev;
    }
    delete entry;

    // Return ook
    return (0);
}


