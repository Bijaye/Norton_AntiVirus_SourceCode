///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/SEVINST.DEV/VCS/CUsgcnt.h_v   1.1   03 Feb 1998 16:59:58   BHELLST  $
//
//  CUSGCNT.H     - This file contains class definitions for the
//                  CUsageCount Class Objects.
//
//
//
///////////////////////////////////////////////////////////////////////////

#ifndef _CUSGCNT_H_
#define _CUSGCNT_H_


#ifndef _CREGENT_H_
#include "cregent.h"
#endif


#define CUC_HIVE            HKEY_LOCAL_MACHINE
#define CUC_SHARED_DLLS     "Software\\Microsoft\\Windows\\CurrentVersion\\SharedDlls"
#define CUC_INVALID_COUNT   0xFFFFFFFF


class CUsageCount
{
public:

                                        // Construction/Destruction

    CUsageCount( HKEY hHive = CUC_HIVE, LPTSTR lpSubKey = CUC_SHARED_DLLS,
                 LPTSTR lpValue = NULL );
    ~CUsageCount();

                                        // Public Member functions

    void SetValueName( LPTSTR lpValue );
    BOOL CountExists( void );
    DWORD GetCountType( void );
    DWORD GetCount( void );
    LONG SetCount( DWORD dwCount, BOOL bStrCnt = FALSE );
    DWORD IncrementCount( void );
    DWORD DecrementCount( BOOL bDelIfZero = TRUE );
    LONG DeleteCount( void );


protected:

    DWORD StrCountToDword( LPTSTR lpCount );
    BOOL DwordCountToStr( DWORD dwCount, LPTSTR lpCount );


protected:

    DWORD               m_dwCntType;
    LPTSTR              m_lpValue;
    CRegistryEntry     *pRegent;

};



#endif                                  // ifndef _this_file_


///////////////////////////////////////////////////////////////////////////
//  $Log:   S:/SEVINST.DEV/VCS/CUsgcnt.h_v  $
// 
//    Rev 1.1   03 Feb 1998 16:59:58   BHELLST
// 1. Added support for copy only mode if no prod id is passed in
// 2. Added CGlobalInfo class for handling global variables and
//    procedures.
// 3. Add support for VDD under NT.
// 4. Re-design WININIT.INI handling using CWIniFile class.
// 5. Add CCmdLine class for parsing command line
// 6. Set ImagePath for driver under NT.
// 7. Remove SYMEVENT.SYS from the Windows System directory.
// 
//    Rev 1.1   07 Jul 1997 03:11:02   BRUCEH
// 1. Allow CUsageCount to override dword values with string values
// 2. Put in code to make symevent counts match and to override dword
//    usage counts for symevent components in the symantec dir.
//
//
///////////////////////////////////////////////////////////////////////////

