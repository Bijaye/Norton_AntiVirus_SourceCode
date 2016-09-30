// Copyright 1992-1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/include/VCS/navnet.h_v   1.0   06 Feb 1997 20:51:46   RFULLER  $
//
// Description:
//      This include file defines the header for the Norton AntiVirus
//      for Windows NT NavNet.
//
//************************************************************************
// $Log:   S:/include/VCS/navnet.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:51:46   RFULLER
// Initial revision
// 
//    Rev 1.4   22 Oct 1996 16:18:14   TIVANOV
// export NavOptionsDialog
//
//    Rev 1.3   18 Oct 1996 12:11:18   TIVANOV
// remove the previoce changes I made; new design which does not need them
//
//    Rev 1.2   18 Oct 1996 11:44:10   TIVANOV
// merge the changes from LUIGI
//
//    Rev 1.1   18 Oct 1996 10:37:40   TIVANOV
// new defines for NAVNETNT functionality in common\options.cpp
//
//    Rev 1.0   17 Jul 1996 08:07:36   JWORDEN
// Initial revision.
//
//************************************************************************

#ifndef _N32NET_H
#define _N32NET_H

//==============================================================================
// Return values
//==============================================================================

#define NNR_SUCCESS                        0
#define NNR_INITIALIZATION_FAILURE         1
#define NNR_PATH_IS_NOT_FOUND              2
#define NNR_PATH_IS_INVALID                3
#define NNR_PATH_IS_FILE                   4
#define NNR_FILE_CREATION_ERROR            5
#define NNR_FILE_NOT_FOUND                 6  // Internal use only
#define NNR_FILE_IS_INVALID                7
#define NNR_ERROR_READING_NAVOPTS          8
#define NNR_WORKSTATION_UPDATE_FAILED      9


//==============================================================================
// Prototypes
//==============================================================================

extern "C" WORD SYM_EXPORT WINAPI NavOptionsDialog  // Main entry point
(
    HWND  hWndParent,                    // NEF calling window
    LPSTR lpszInDataPath,                // [in] Path for input data files
    LPSTR lpszOutDataPath                // [in] Path for output data files
);

#endif
