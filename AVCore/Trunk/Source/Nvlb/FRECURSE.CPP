//***************************************************************************
// Copyright 1997 Symantec Corporation
//***************************************************************************
//	FILE_NAME:
//***************************************************************************
// $Header:   S:/MAPISEND/VCS/frecurse.cpv   1.0   21 Apr 1998 15:20:20   CBROWN  $
//
// Description:
//
//***************************************************************************
// $Log:   S:/MAPISEND/VCS/frecurse.cpv  $
//
//    Rev 1.0   21 Apr 1998 15:20:20   CBROWN
// Initial revision.
//
//    Rev 1.0   19 Dec 1997 16:42:44   CBROWN
// Initial revision.
//
//***************************************************************************
// credits.cpp : implementation file
//**************************************************************************

#include "stdafx.h"

#include "fRecurse.H"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
CRecurseFolder::CRecurseFolder() :
m_bSubFolders(FALSE),
m_bNewFolder(FALSE),
m_bNoMoeFiles(FALSE)
{

}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
CRecurseFolder::~CRecurseFolder()
{

	CRecurseFolder::Close();
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
void CRecurseFolder::Close()
{
	while( !m_FolderList.IsEmpty() )    // Make sure that queue is empty.
		{
						// Close current handle.
			::FindClose(m_FolderList.GetHead().m_hHandle);
			m_FolderList.RemoveHead();
		}

	CFileFind::Close();
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
BOOL CRecurseFolder::FindFile
    (
        LPCSTR pstrName,
        DWORD  dwUnused,   // = 0,
        BOOL   bSubFolders // = FALSE
    )
{

    UNUSED_ALWAYS(dwUnused);
    Close();
    m_pNextInfo = new WIN32_FIND_DATA;
    m_bGotLast = FALSE;

    m_bSubFolders = bSubFolders;

    if (pstrName == NULL)
        pstrName = _T("*.*");

    _tcscpy(((WIN32_FIND_DATA*) m_pNextInfo)->cFileName, pstrName);

    m_hContext = ::FindFirstFile(pstrName, (WIN32_FIND_DATA*) m_pNextInfo);

    if (m_hContext == INVALID_HANDLE_VALUE)
    {
        DWORD dwTemp = ::GetLastError();
        Close();
        ::SetLastError(dwTemp);
        return FALSE;
    }

    LPTSTR pstrRoot = m_strRoot.GetBufferSetLength(_MAX_PATH);
    LPCTSTR pstr = _tfullpath(pstrRoot, pstrName, _MAX_PATH);

    // passed name isn't a valid path but was found by the API
    ASSERT(pstr != NULL);
    if (pstr == NULL)
    {
        m_strRoot.ReleaseBuffer(-1);
        Close();
        ::SetLastError(ERROR_INVALID_NAME);
        return FALSE;
    }
    else
    {
        // find the last forward or backward whack
        char * pstrBack  = _tcsrchr(pstrRoot, '\\');
        char * pstrFront = _tcsrchr(pstrRoot, '/');

        if (pstrFront != NULL || pstrBack != NULL)
        {
            if (pstrFront == NULL)
                pstrFront = pstrRoot;
            if (pstrBack == NULL)
                pstrBack = pstrRoot;

            // from the start to the last whack is the root

            if (pstrFront >= pstrBack)
                {

				pstrFront++;
                m_strMatchSpec = pstrFront;
                *pstrFront = '\0';

                }
            else
                {

				pstrBack++;
                m_strMatchSpec = pstrBack;
                *pstrBack = '\0';

                }
        }
        m_strRoot.ReleaseBuffer(-1);
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////
BOOL CRecurseFolder::FindNextFile( void )
{
    ASSERT(m_hContext != NULL);

    LPWIN32_FIND_DATA   lpData;
    CString             strName;
	BOOL				bResult = FALSE;

    if( m_hContext == NULL )
	{
        return FALSE;
	}

    if (m_pFoundInfo == NULL)
	{
        m_pFoundInfo = new WIN32_FIND_DATA;
	}

    ASSERT_VALID(this);

	if( TRUE == m_bNoMoeFiles )
	{
		while( FALSE == m_FolderList.IsEmpty() )
		{									// Close current handle.
			::FindClose( m_hContext );
										// Get stored handle and path.
			m_hContext = m_FolderList.GetHead().m_hHandle;
			m_strRoot  = m_FolderList.GetHead().m_strRoot;

			m_FolderList.RemoveHead();	// Pop last retrived info.

			bResult = ::FindNextFile(m_hContext, (LPWIN32_FIND_DATA) m_pNextInfo);

			if( TRUE == bResult )
			{
				m_bNoMoeFiles = FALSE;
				break;
			}

		}//END_WHILE

		if( FALSE == bResult )
		{
            m_bNoMoeFiles = FALSE;
			return bResult;
		}
	}

	void* pTemp  = m_pFoundInfo;

	m_pFoundInfo = m_pNextInfo;
    m_pNextInfo  = pTemp;

    if( FALSE == m_bSubFolders )
	{
        return ::FindNextFile(m_hContext, (LPWIN32_FIND_DATA) m_pNextInfo);
	}

    lpData = (LPWIN32_FIND_DATA) m_pFoundInfo;

    if( (lpData->cFileName[0] == '.' && lpData->cFileName[1] == NULL)||
        (lpData->cFileName[0] == '.' && lpData->cFileName[1] == '.' ) )
	{
		// Do nothing...
	}
    else if( lpData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY  )
	{
     	strName.Format(_T("%s%s\\"), m_strRoot, lpData->cFileName );

        m_pRecurseStruct.m_strRoot = m_strRoot;
        m_pRecurseStruct.m_hHandle = m_hContext;

        m_FolderList.AddHead( m_pRecurseStruct );

        m_strRoot =  strName;

        strName += m_strMatchSpec;

        m_bNewFolder = TRUE;

	}

    if( TRUE == m_bNewFolder )
    {
        m_hContext   = ::FindFirstFile(strName, (WIN32_FIND_DATA*) m_pNextInfo);
        m_bNewFolder = FALSE;
		bResult	     = TRUE;
    }
    else
	{
		bResult = ::FindNextFile(m_hContext, (LPWIN32_FIND_DATA) m_pNextInfo);
	}

	if( FALSE == bResult )
	{
		m_bNoMoeFiles = TRUE;
		bResult		  = TRUE;
	}

    return bResult;
}






