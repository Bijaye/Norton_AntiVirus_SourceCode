///////////////////////////////////////////////////////////////////////////
// Copyright (c)1997 - Bruce Hellstrom All Rights Reserved
///////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/CUsgcnt.h_v   1.0   18 Jul 1997 11:23:32   JALLEE  $
//
//  CUSGCNT.H     - This file contains class definitions for the
//                  CUsageCount Class Objects.
//
///////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/CUsgcnt.h_v  $
// 
//    Rev 1.0   18 Jul 1997 11:23:32   JALLEE
// Initial revision.
//
//    Rev 1.4   07 Jul 1997 03:07:36   BRUCEH
// 1. Allow CUsageCount to override dword values with string values
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
//  $Log:   S:/ISNU/VCS/CUsgcnt.h_v  $
//
//    Rev 1.4   07 Jul 1997 03:07:36   BRUCEH
// 1. Allow CUsageCount to override dword values with string values
//
//
///////////////////////////////////////////////////////////////////////////

