//************************************************************************
//
// $Header:   S:/navzip/VCS/mntr.h_v   1.0   06 Feb 1997 21:05:58   RFULLER  $
//
// Description:
//      Class declaration for MyMonitor class used to provide callbacks
//      during ZIP operations.
//
//************************************************************************
// $Log:   S:/navzip/VCS/mntr.h_v  $
// 
//    Rev 1.0   06 Feb 1997 21:05:58   RFULLER
// Initial revision
// 
//    Rev 1.1   01 Jul 1996 15:15:06   JALLEE
// Added bAbort public data member, used to abort scanning on user request.
// 
//    Rev 1.0   24 Jun 1996 14:03:46   JALLEE
// Initial revision.
//************************************************************************

#ifndef __NAVMNTR_H
#define __NAVMNTR_H

#include "arclib.h"
#include "pkarc.h"
#include "symzip.h"

//************************************************************************
// DEFINES
//************************************************************************

#define ENOT_FOUND      9


//************************************************************************
// STRUCTS
//************************************************************************

                                        // Monitor class is provided
                                        // callback from Greenleaf zip
                                        // engine.  It in turn calls the
                                        // ZipCallBack in SCAND project.

class AL_CLASS_TYPE MyMonitor : public ALMonitor {
     public :
          AL_PROTO MyMonitor(LPUNZIP_STRUCT lpUnZipStruct);
          virtual AL_INLINE_PROTO ~MyMonitor(){;};

// Member Functions
     protected :
          virtual void AL_PROTO
          ArchiveOperation( ALArchiveOperation operation,
                            ALArchive AL_DLL_FAR *archive,
                            ALEntry AL_DLL_FAR *job );
// Data
     public:
          BOOL  bAbort;
     protected :
          LPZIPCALLBACK lpCallBack;
          DWORD dwUserData;
          ZIPRECORD ZipRecord;
};
#endif
