/////////////////////////////////////////////////////////////////////
// File: CreatMig.cpp
//
// Copyright 1992-1997 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
//
// $Header:   S:/ATMIGRAT/VCS/CREATMIG.CPv   1.0   26 May 1998 14:31:24   SEDWARD  $
//
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ATMIGRAT/VCS/CREATMIG.CPv  $
// 
//    Rev 1.0   26 May 1998 14:31:24   SEDWARD
// Initial revision.
//
/////////////////////////////////////////////////////////////////////



#include    <windows.h>
#include    <windowsx.h>
#include    "CreatMig.h"

// TODO: include your app's migrant class header file here
#include    "NavMig95.h"





// TODO: modify this function to create an instance of your app's migrant object

// ==== CreateMigrant =====================================================
//
//
// ========================================================================
// ========================================================================

CMigrant*   CreateMigrant(void)
{
    auto    CMigrant*       migrantWorker;

    // if necessary, you can be more elaborate in creating the migration object
    // by performing OS - environment checks...

    migrantWorker = new  CNavMigrant95;
    return  (migrantWorker);

}  // end of "CreateMigrant"