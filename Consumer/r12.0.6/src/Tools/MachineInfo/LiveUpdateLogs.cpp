// LiveUpdateLogs.cpp: implementation of the CLiveUpdateLogs class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "LiveUpdateLogs.h"
#include "NAVInfo.h"
#include "FileIO.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLiveUpdateLogs::CLiveUpdateLogs()
{
    CNAVInfo NAVInfo;
    
    _tcscpy ( m_szLiveUpdateDir, NAVInfo.GetAllUsersAppsDir () );
    _tcscat ( m_szLiveUpdateDir, _T("\\Symantec\\LiveUpdate") );

    if ( 0xFFFFFFFF != GetFileAttributes ( m_szLiveUpdateDir ))
        m_bIsAvailable = true;
}

CLiveUpdateLogs::~CLiveUpdateLogs()
{

}

bool CLiveUpdateLogs::CreateReport ( LPCTSTR lpcszFolderPath )
{
    CFileIO FileIO;
    return FileIO.CopyFiles ( m_szLiveUpdateDir, _T("*.*"), lpcszFolderPath );
}

