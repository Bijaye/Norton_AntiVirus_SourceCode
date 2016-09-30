// FileEnum.cpp: implementation of the CFileEnum class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MachineInfo.h"
#include "FileEnum.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define FILEENUM_FIRST_TIME 0
#define FILEENUM_NORMAL     1
#define FILEENUM_DONE       2

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFileEnum::CFileEnum( LPCTSTR lpcszSourcePath,
                      LPCTSTR lpcszFileMask)
{
    // Make our own copy in case the client get cute and changes the values.
    //
    if ( lpcszSourcePath[0] != '\0' )
    {
        _tcscpy ( m_szSourcePath, lpcszSourcePath );
        
        if ( lpcszFileMask[0] == '\0' )
            _tcscpy ( m_szFileMask, _T("*.*"));
        else
            _tcscpy ( m_szFileMask, lpcszFileMask );
    
        _stprintf ( m_szSearchPath, _T("%s\\%s"), lpcszSourcePath, lpcszFileMask );
        
        m_hFindHandle = 0;

        Restart ();
    }
}

CFileEnum::~CFileEnum()
{
    FindClose ( m_hFindHandle );
}

bool CFileEnum::NextFile(LPTSTR lpszFileName )
{
    // If there's nothing to look for, or we are done, return.
    //
    if ( m_szSearchPath[0] == '\0' ||
         m_iState == FILEENUM_DONE )
        return false;

    // Normal operation, get the next file
    //
    if ( m_iState == FILEENUM_NORMAL )
        if ( !FindNextFile ( m_hFindHandle, &m_FindFileData ))
        {
            m_iState = FILEENUM_DONE;
            return false;
        }

    // First time through, we already have the first file.
    //
    if ( m_iState == FILEENUM_FIRST_TIME )
        m_iState = FILEENUM_NORMAL;

    if ( INVALID_HANDLE_VALUE == m_hFindHandle )
    {
        m_iState = FILEENUM_DONE;
        return false;
    }

    TCHAR szFileName [MAX_PATH] = {0};

    while ( m_FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY && m_iState != FILEENUM_DONE )
    {
        if ( !FindNextFile ( m_hFindHandle, &m_FindFileData ))
            m_iState = FILEENUM_DONE;
    }
    
    // Finally got a file!
    //
    _stprintf ( lpszFileName, "%s\\%s", m_szSourcePath, m_FindFileData.cFileName );

    return true;

}

void CFileEnum::Restart()
{
    if ( INVALID_HANDLE_VALUE != m_hFindHandle )
        FindClose ( m_hFindHandle );

    m_hFindHandle = FindFirstFile ( m_szSearchPath, &m_FindFileData );
    m_iState = FILEENUM_FIRST_TIME;
}

LPCTSTR CFileEnum::GetCurrentFileName()
{
    return m_FindFileData.cFileName;
}
