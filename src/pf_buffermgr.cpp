#include <cstdio>
#include <unistd.h>
#include <iostream>
#include "pf_buffermgr.h"

using namespace std;

//
// PF_BufferMgr
//
// Desc: Constructor - called by PF_Manager::PF_Manager
//       The buffer manager manages the page buffer.  When asked for a page,
//       it checks if it is in the buffer.  If so, it pins the page (pages
//       can be pinned multiple times).  If not, it reads it from the file
//       and pins it.  If the buffer is full and a new page needs to be
//       inserted, an unpinned page is replaced according to an LRU
// In:   numPages - the number of pages in the buffer
//
// Note: The constructor will initialize the global pStatisticsMgr.  We
//       make it global so that other components may use it and to allow
//       easy access.
//
// Aut2003
// numPages changed to _numPages for to eliminate CC warnings
PF_BufferMgr::PF_BufferMgr(int numPages) : _hashTable(PF_HASH_TBL_SIZE)
{
    // Initialize local variables
    _numPages = numPages;
    _pageSize = PF_PAGE_SIZE + sizeof(PF_PageHdr);

    // Allocate memory for buffer page description table
    _bufTable = new PF_BufPageDesc[_numPages];

    // Initialize the buffer table and allocate memory for buffer pages.
    // Initially, the free list contains all pages
    for (int i = 0; i < _numPages; i++)
    {
        if ((_bufTable[i].pData = new char[_pageSize]) == NULL)
        {
            cerr << "Not enough memory for buffer\n";
            exit(1);
        }
        
        // initiate the buffer value, 
        memset((void *)_bufTable[i].pData, 0, _pageSize);

        _bufTable[i].prev = i - 1;
        _bufTable[i].next = i + 1;
    }

    _bufTable[0].prev = _bufTable[_numPages - 1].next = INVALID_SLOT;

    // set head of the freelist to be first page
    _free = 0;
    _first = this->_last = INVALID_SLOT;
}

//
// ~PF_BufferMgr
//
// Desc: Destructor - called by PF_Manager::~PF_Manager
//
PF_BufferMgr::~PF_BufferMgr()
{
   // Free up buffer pages and tables
   for (int i = 0; i < _numPages; i++)
      delete [] _bufTable[i].pData;

   delete [] _bufTable;
}

//
// getPage
//
// Desc: Get a pointer to a page pinned in the buffer.  If the page is
//       already in the buffer, (re)pin the page and return a pointer
//       to it.  If the page is not in the buffer, read it from the file,
//       pin it, and return a pointer to it.  If the buffer is full,
//       replace an unpinned page.
// In:   fd - OS file descriptor of the file to read
//       pageNum - number of the page to read
//       bMultiplePins - if FALSE, it is an error to ask for a page that is
//                       already pinned in the buffer.
// Out:  ppBuffer - set *ppBuffer to point to the page in the buffer
// Ret:  PF return code
//
RC PF_BufferMgr::getPage(int fd, PageNum pageNum, char **ppBuffer, int bMultiplePins)
{
    RC  rc;     // return code
    int slot;   // buffer slot where page is located

    // Search for page in buffer
    // Not found and error code not equal to PF_HASHNOTFOUND
    if ((rc = _hashTable.find(fd, pageNum, slot)) && (rc != PF_HASHNOTFOUND))
    {
        return rc;                // unexpected error
    }

    // If page not in buffer...
    if (rc == PF_HASHNOTFOUND)
    {
        // Allocate an empty page, this will also promote the newly allocated
        // page to the MRU slot
        if ((rc = internalAlloc(slot)))
        {
            return rc;
        }

        // read the page, insert it into the hash table,
        // and initialize the page description entry
        if ((rc = readPage(fd, pageNum, _bufTable[slot].pData)) ||
            (rc = _hashTable.insert(fd, pageNum, slot)) ||
            (rc = initPageDesc(fd, pageNum, slot)))
        {

            // Put the slot back on the free list before returning the error
            unlink(slot);
            insertFree(slot);
            return rc;
        }
    }
    else
    {
        // Page is in the buffer...
        // Error if we don't want to get a pinned page
        if (!bMultiplePins && _bufTable[slot].pinCount > 0)
        {
            return PF_PAGEPINNED;
        }

        // Page is alredy in memory, just increment pin count
        _bufTable[slot].pinCount++;

        // Make this page the most recently used page
        if ((rc = unlink(slot)) || (rc = linkHead (slot)))
        {
            return rc;
        }
    }

    // Point ppBuffer to page
    *ppBuffer = _bufTable[slot].pData;

    // Return ok
    return OK_RC;
}

//
// allocatePage
//
// Desc: Allocate a new page in the buffer and return a pointer to it.
// In:   fd - OS file descriptor of the file associated with the new page
//       pageNum - number of the new page
// Out:  ppBuffer - set *ppBuffer to point to the page in the buffer
// Ret:  PF return code
//
RC PF_BufferMgr::allocatePage(int fd, PageNum pageNum, char **ppBuffer)
{
    RC  rc;     // return code
    int slot;   // buffer slot where page is located

    // If page is already in buffer, return an error
    if (!(rc = _hashTable.find(fd, pageNum, slot)))
    {
        return (PF_PAGEINBUF);
    }

    if (rc != PF_HASHNOTFOUND)
    {
        return (rc);              // unexpected error
    }

    // Allocate an empty page
    if ((rc = internalAlloc(slot)))
    {
        return (rc);
    }

    // Insert the page into the hash table,
    // and initialize the page description entry
    if ((rc = _hashTable.insert(fd, pageNum, slot)) ||
            (rc = initPageDesc(fd, pageNum, slot)))
    {
        // Put the slot back on the free list before returning the error
        unlink(slot);
        insertFree(slot);
        return rc;
    }

    // Point ppBuffer to page
    *ppBuffer = _bufTable[slot].pData;

    // Return ok
    return OK_RC;
}

//
// UnpinPage
//
// Desc: Unpin a page so that it can be discarded from the buffer.
// In:   fd - OS file descriptor of the file associated with the page
//       pageNum - number of the page to unpin
// Ret:  PF return code
//
RC PF_BufferMgr::unpinPage(int fd, PageNum pageNum)
{
    RC  rc;       // return code
    int slot;     // buffer slot where page is located

    // The page must be found and pinned in the buffer
    if ((rc = _hashTable.find(fd, pageNum, slot)))
    {
        if ((rc == PF_HASHNOTFOUND))
        {
            return (PF_PAGENOTINBUF);
        }
        else
        {
            return (rc);              // unexpected error
        }
    }

    if (_bufTable[slot].pinCount == 0)
    {
        return (PF_PAGEUNPINNED);
    }

    // If unpinning the last pin, make it the most recently used page
    _bufTable[slot].pinCount -= 1;
    if (_bufTable[slot].pinCount == 0)
    {
        if ((rc = unlink(slot)) || (rc = linkHead (slot)))
            return (rc);
    }

    // Return ok
    return (0);
}

//
// ClearBuffer
//
// Desc: Remove all entries from the buffer manager.
//       This routine will be called via the system command and is only
//       really useful if the user wants to run some performance
//       comparison starting with an clean buffer.
// In:   Nothing
// Out:  Nothing
// Ret:  Will return an error if a page is pinned and the Clear routine
//       is called.
RC PF_BufferMgr::clearBuffer()
{
    RC rc;
    int slot, next;
    slot = _first;
    while (slot != INVALID_SLOT)
    {
        next = _bufTable[slot].next;
        if (_bufTable[slot].pinCount == 0)
        {
            if ((rc = _hashTable.erase(_bufTable[slot].fd, _bufTable[slot].pageNum)) ||
                (rc = unlink(slot)) ||
                (rc = insertFree(slot)))
            {
                return (rc);
            }
        }
        slot = next;
    }
    return 0;
}

//
// PrintBuffer
//
// Desc: Display all of the pages within the buffer.
//       This routine will be called via the system command.
// In:   Nothing
// Out:  Nothing
// Ret:  Always returns 0
//
RC PF_BufferMgr::printBuffer()
{
    cout << "Buffer contains " << _numPages << " pages of size "
        << _pageSize << "." << endl;
    cout << "Contents in order from most recently used to least recently used."
        << endl;

    int slot, next;
    slot = _first;
    while (slot != INVALID_SLOT)
    {
        next = _bufTable[slot].next;
        cout << slot << " :: " << endl;
        cout << "  fd = " << _bufTable[slot].fd << endl;
        cout << "  pageNum = " << _bufTable[slot].pageNum << endl;
        cout << "  pinCount = " << _bufTable[slot].pinCount << endl;
        slot = next;
    }

    if (_first == INVALID_SLOT)
    {
        cout << "Buffer is empty!" << endl;
    }
    else
    {
        cout << "All remaining slots are free." << endl;
    }

    return 0;
}

//
// ResizeBuffer
//
// Desc: Resizes the buffer manager to the size passed in.
//       This routine will be called via the system command.
// In:   The new buffer size
// Out:  Nothing
// Ret:  0 for success or,
//       Some other PF error (probably PF_NOBUF)
//
// Notes: This method attempts to copy all the old pages which I am
// unable to kick out of the old buffer manager into the new buffer
// manager.  This obviously cannot always be successfull!
//
RC PF_BufferMgr::resizeBuffer(int iNewSize)
{
    RC rc;

    // First try and clear out the old buffer!
    clearBuffer();

    // Allocate memory for a new buffer table
    PF_BufPageDesc *pNewBufTable = new PF_BufPageDesc[iNewSize];

    // Initialize the new buffer table and allocate memory for buffer
    // pages.  Initially, the free list contains all pages
    for (int i = 0; i < iNewSize; i++)
    {
        if ((pNewBufTable[i].pData = new char[_pageSize]) == NULL)
        {
            cerr << "Not enough memory for buffer\n";
            exit(1);
        }

        memset ((void *)pNewBufTable[i].pData, 0, _pageSize);

        pNewBufTable[i].prev = i - 1;
        pNewBufTable[i].next = i + 1;
    }
    pNewBufTable[0].prev = pNewBufTable[iNewSize - 1].next = INVALID_SLOT;

    // Now we must remember the old first and last slots and (of course)
    // the buffer table itself.  Then we use insert methods to insert
    // each of the entries into the new buffertable
    int oldFirst = _first;
    PF_BufPageDesc *pOldBufTable = _bufTable;

    // Setup the new number of pages,  first, last and free
    _numPages = iNewSize;
    _first = _last = INVALID_SLOT;
    _free = 0;

    // Setup the new buffer table
    _bufTable = pNewBufTable;

    // We must first remove from the hashtable any possible entries
    int slot, next, newSlot;
    slot = oldFirst;
    while (slot != INVALID_SLOT) {
        next = pOldBufTable[slot].next;

        // Must remove the entry from the hashtable from the
        if ((rc = _hashTable.erase(pOldBufTable[slot].fd, pOldBufTable[slot].pageNum)))
            return (rc);
        slot = next;
    }

    // Now we traverse through the old buffer table and copy any old
    // entries into the new one
    slot = oldFirst;
    while (slot != INVALID_SLOT)
    {
        next = pOldBufTable[slot].next;
        // Allocate a new slot for the old page
        if ((rc = internalAlloc(newSlot)))
            return rc;

        // Insert the page into the hash table,
        // and initialize the page description entry
        if ((rc = _hashTable.insert(pOldBufTable[slot].fd,
                pOldBufTable[slot].pageNum, newSlot)) ||
                (rc = initPageDesc(pOldBufTable[slot].fd,
                pOldBufTable[slot].pageNum, newSlot)))
            return (rc);

        // Put the slot back on the free list before returning the error
        unlink(newSlot);
        insertFree(newSlot);

        slot = next;
    }

    // Finally, delete the old buffer table
    delete [] pOldBufTable;

    return 0;
}

//
// GetBlockSize
//
// Return the size of the block that can be allocated.  This is simply
// just the size of the page since a block will take up a page in the
// buffer pool.
//
RC PF_BufferMgr::getBlockSize(int &length) const
{
   length = _pageSize;
   return OK_RC;
}

//------------------------------------------------------------------------------
// Private Methods
//------------------------------------------------------------------------------

//
// insertFree
//
// Desc: Internal.  Insert a slot at the head of the free list
// In:   slot - slot number to insert
// Ret:  PF return code
//
RC PF_BufferMgr::insertFree(int slot)
{
   _bufTable[slot].next = _free;
   _free = slot;

   // Return ok
   return OK_RC;
}

//
// linkHead
//
// Desc: Internal.  Insert a slot at the head of the used list, making
//       it the most-recently used slot.
// In:   slot - slot number to insert
// Ret:  PF return code
//
RC PF_BufferMgr::linkHead(int slot)
{
    // Set next and prev pointers of slot entry
    _bufTable[slot].next = _first;
    _bufTable[slot].prev = INVALID_SLOT;

    // If list isn't empty, point old first back to slot
    if (_first != INVALID_SLOT)
        _bufTable[_first].prev = slot;

    _first = slot;

    // if list was empty, set last to slot
    if (_last == INVALID_SLOT)
        _last = _first;

    // Return ok
    return OK_RC;
}

//
// unlink
//
// Desc: Internal.  Unlink the slot from the used list.  Assume that
//       slot is valid.  Set prev and next pointers to INVALID_SLOT.
//       The caller is responsible to either place the unlinked page into
//       the free list or the used list.
// In:   slot - slot number to unlink
// Ret:  PF return code
//
RC PF_BufferMgr::unlink(int slot)
{
    // If slot is at head of list, set first to next element
    if (_first == slot)
        _first = _bufTable[slot].next;

    // If slot is at end of list, set last to previous element
    if (_last == slot)
        _last = _bufTable[slot].prev;

    // If slot not at end of list, point next back to previous
    if (_bufTable[slot].next != INVALID_SLOT)
        _bufTable[_bufTable[slot].next].prev = _bufTable[slot].prev;

    // If slot not at head of list, point prev forward to next
    if (_bufTable[slot].prev != INVALID_SLOT)
        _bufTable[_bufTable[slot].prev].next = _bufTable[slot].next;

    // Set next and prev pointers of slot entry
    _bufTable[slot].prev = _bufTable[slot].next = INVALID_SLOT;

    // Return ok
    return OK_RC;
}

//
// internalAlloc
//
// Desc: Internal.  Allocate a buffer slot.  The slot is inserted at the
//       head of the used list.  Here's how it chooses which slot to use:
//       If there is something on the free list, then use it.
//       Otherwise, choose a victim to replace.  If a victim cannot be
//       chosen (because all the pages are pinned), then return an error.
// Out:  slot - set to newly-allocated slot
// Ret:  PF_NOBUF if all pages are pinned, other PF return code otherwise
//
RC PF_BufferMgr::internalAlloc(int &slot)
{
    RC  rc;       // return code

    // If the free list is not empty, choose a slot from the free list
    if (_free != INVALID_SLOT)
    {
        slot = _free;
        _free = _bufTable[slot].next;
    }
    else
    {
        // Choose the least-recently used page that is unpinned : reverse finding
        for (slot = _last; slot != INVALID_SLOT; slot = _bufTable[slot].prev)
        {
            if (_bufTable[slot].pinCount == 0)
                break;
        }

        // Return error if all buffers were pinned
        if (slot == INVALID_SLOT)
        {
            return PF_NOBUF;
        }

        // Remove page from the hash table and slot from the used buffer list
        if ((rc = _hashTable.erase(_bufTable[slot].fd, _bufTable[slot].pageNum)) || (rc = unlink(slot)))
        {
            return rc;
        }
    }

    // Link slot at the head of the used list
    if ((rc = linkHead(slot)))
    {
        return rc;
    }

    // Return ok
    return OK_RC;
}

//
// readPage
//
// Desc: Read a page from disk
//
// In:   fd - OS file descriptor
//       pageNum - number of page to read
//       dest - pointer to buffer in which to read page
// Out:  dest - buffer contains page contents
// Ret:  PF return code
//
RC PF_BufferMgr::readPage(int fd, PageNum pageNum, char *dest)
{
    // seek to the appropriate place (cast to long for PC's)
    // long offset = pageNum * (long)_pageSize + PF_FILE_HDR_SIZE;
    long offset = pageNum * PF_PAGE_SIZE;

    if (lseek(fd, offset, L_SET) < 0)
    {
        return PF_UNIX;
    }

    // Read the data
    int numBytes = read(fd, dest, PF_PAGE_SIZE);
    if (numBytes < 0)
    {
        return PF_UNIX;
    }
    else if (numBytes != PF_PAGE_SIZE)
    {
        return PF_INCOMPLETEREAD;
    }
    else
    {
        return OK_RC;
    }
}

//
// initPageDesc
//
// Desc: Internal.  Initialize PF_BufPageDesc to a newly-pinned page
//       for a newly pinned page
// In:   fd - file descriptor
//       pageNum - page number
// Ret:  PF return code
//
RC PF_BufferMgr::initPageDesc(int fd, PageNum pageNum, int slot)
{
   // set the slot to refer to a newly-pinned page
   _bufTable[slot].fd       = fd;
   _bufTable[slot].pageNum  = pageNum;
   _bufTable[slot].pinCount = 1;

   // Return ok
   return OK_RC;
}