////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// ActivityLogReport.cpp: implementation of the CActivityLogReport class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "ActivityLogReport.h"
#include "NAVInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActivityLogReport::CActivityLogReport()
{
    CNAVInfo NAVInfo;
    _stprintf (m_szActivityLog, "%s\\Activity.log", NAVInfo.GetNAVDir ());

    
    if ( 0xFFFFFFFF != GetFileAttributes ( m_szActivityLog ))
        m_bIsAvailable = true;
}

CActivityLogReport::~CActivityLogReport()
{
}

bool CActivityLogReport::CreateReport(LPCTSTR lpcszFolderPath)
{
    TCHAR szNewFile [MAX_PATH] = {0};

    _stprintf (szNewFile, "%s\\Activity.log", lpcszFolderPath);

    return ( TRUE == CopyFile ( m_szActivityLog, szNewFile, FALSE ));
}
