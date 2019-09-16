#ifndef RF_H
#define RF_H

#include "base.h"
#include "../pf/pf.h"
#include "rm_rid.h"

namespace wxcs {
namespace rf {
    using namespace base;

    class RM_Manager {
        public:
            RM_Manager  (pf::PF_Manager &pfm); // Constructor
            ~RM_Manager (); // Destructor
            RC CreateFile  (const char *fileName, int recordSize); // Create a new file
            RC DestroyFile (const char *fileName); // Destroy a file
            RC OpenFile    (const char *fileName, RM_FileHandle &fileHandle); // Open a file
            RC CloseFile   (RM_FileHandle &fileHandle); // Close a file
    };

    class RM_FileHandle {
        public:
            RM_FileHandle  (); // Constructor
            ~RM_FileHandle (); // Destructor
            RC GetRec         (const rf::RID &rid, RM_Record &rec) const; // Get a record
    };

    class RM_FileScan {
        public:
            RM_FileScan  ();                                // Constructor
            ~RM_FileScan ();                                // Destructor
            RC OpenScan     (const RM_FileHandle &fileHandle,  // Initialize file scan
                                    AttrType      attrType,
                                    int           attrLength,
                                    int           attrOffset,
                                    CompOp        compOp,
                                    void          *value,
                                    ClientHint    pinHint = NO_HINT);
            RC GetNextRec   (RM_Record &rec);                  // Get next matching record
            RC CloseScan    ();                                // Terminate file scan
    };

    class RM_Record {
        public:
            RM_Record  ();                     // Constructor
            ~RM_Record ();                     // Destructor

            RC GetData    (char *&pData) const;   // Set pData to point to
                                                //   the record's contents
            RC GetRid     (RID &rid) const;       // Get the record id
    };
}
}
#endif //RF_H