//
// File:        pf_buffermgr.h
// Description: PF_BufferMgr class interface
//
// 1997: When requesting a page from the buffer manager the page requested
// is now promoted to the MRU slot.
// 1998: Allow chunks from the buffer manager to not be associated with
// a particular file.  Allows students to use main memory chunks that
// are associated with (and limited by) the buffer.
//

#ifndef PF_BUFFERMGR_H
#define PF_BUFFERMGR_H

#include "pf_internal.h"
#include "pf_hashtable.h"

//
// Defines

// INVALID_SLOT is used within the PF_BufferMgr class which tracks a list
// of PF_BufPageDesc.  Inside the PF_BufPageDesc are integer "pointers" to
// next and prev items.  INVALID_SLOT is used to indicate no previous or
// next.
#define INVALID_SLOT  (-1)

//
// PF_BufPageDesc - struct containing data about a page in the buffer
//
struct PF_BufPageDesc {
    char       *pData;      // page contents
    int        next;        // next in the linked list of buffer pages
    int        prev;        // prev in the linked list of buffer pages
    short int  pinCount;    // pin count
    PageNum    pageNum;     // page number for this page
    int        fd;          // OS file descriptor of this page
};

//
// PF_BufferMgr - manage the page buffer
//
class PF_BufferMgr {
public:
    PF_BufferMgr     (int numPages);             // Constructor - allocate numPages buffer pages
    ~PF_BufferMgr    ();                         // Destructor

    // Read pageNum into buffer, point *ppBuffer to location
    RC getPage      (int fd, PageNum pageNum, char **ppBuffer, int bMultiplePins = TRUE);
    // Allocate a new page in the buffer, point *ppBuffer to its location
    RC allocatePage (int fd, PageNum pageNum, char **ppBuffer);

    RC unpinPage    (int fd, PageNum pageNum);  // Unpin page from the buffer

    // Remove all entries from the Buffer Manager.
    RC clearBuffer  ();
    // Display all entries in the buffer
    RC printBuffer   ();

    // Attempts to resize the buffer to the new size
    RC resizeBuffer  (int iNewSize);

    // Return the size of the block that can be allocated.
    RC getBlockSize  (int &length) const;

private:
    RC  insertFree   (int slot);                 // Insert slot at head of free
    RC  linkHead     (int slot);                 // Insert slot at head of used
    RC  unlink       (int slot);                 // Unlink slot
    RC  internalAlloc(int &slot);                // Get a slot to use

    // Read a page
    RC  readPage     (int fd, PageNum pageNum, char *dest);

    // Init the page desc entry
    RC  initPageDesc (int fd, PageNum pageNum, int slot);

    PF_BufPageDesc *_bufTable;                     // info on buffer pages
    PF_HashTable   _hashTable;                     // Hash table object
    int            _numPages;                      // # of pages in the buffer
    int            _pageSize;                      // Size of pages in the buffer
    int            _first;                         // MRU page slot
    int            _last;                          // LRU page slot
    int            _free;                          // head of free list
};
#endif