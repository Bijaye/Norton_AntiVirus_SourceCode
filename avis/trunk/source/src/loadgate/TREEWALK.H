//===========================================================================
//
//                            TREEWALK.H
//
// Copyright (c) 1997  MENSK Technologies Corporation. All Rights Reserved.
// 
//
//===========================================================================

#ifndef   _TREEWALK_H_
#define   _TREEWALK_H_

#include <time.h>
#include <io.h>


//---------------------------------------------------------------------------
// Return codes from PerDir() callback.
//---------------------------------------------------------------------------

#define  DIR_FOUND            (1)      // To be passed as second param. to PerDir().
#define  DIR_DONE             (2)      // To be passed as second param. to PerDir().

#define  TW_SUCCESS           (0)
#define  TW_ERROR             (-1)
#define  TW_DEFAULT           (0)      // Proceed normal traversing with
                                       // default actions:
                                       // - search subdirs.
                                       // - search for files in subdirs.

#define  TW_NO_DEEPER         (2)      // Do not search for subdirs.
#define  TW_CUT_SEARCH        (4)      // Do not proceed any search in current dir.
#define  TW_EXIT              (256)    // Terminate search and exit.


int   TreeWalk (
         const char     *pszTreeSpec,                                          // Starting tree.
         void           (*pFnLevelBack)   (DWORD, char *pathname),             // NULL|callback: -- dir level.
         void           (*pFnLevelAdded)  (DWORD, char *pathname, char *name), // NULL|callback: ++ dir level.
         int            (*pFnPerDir)      (DWORD, int DirWhat, char *pathname, char *name), // NULL|dir  found callback.
         int            (*pFnPerFile)     (DWORD, char *pathname, char *name, WIN32_FIND_DATA *pData), // NULL|file found callback.
         DWORD          ulParam1                                               // Passed to all callback functions.
         );


#define  MAX_PATH_LEN         MAX_PATH          // in STDLIB.H
#define  MAX_DIR_NUM          (MAX_PATH_LEN/2)  // 125 for NT in my exp.


#endif   // _TREEWALK_H_
