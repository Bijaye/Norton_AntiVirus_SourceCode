// Enumerator.cpp: implementation of the CEnumerator class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Enumerator.h"
#include "qdefs.h"
#include "qspak.h"
#include "comdef.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CEnumerator::CEnumerator
//
// Description:
//
// Return type:
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/1/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
CEnumerator::CEnumerator( LPCTSTR pszFolder ) :
m_ulCount( 0 )
{
    //
    // Copy data and set up defaults.
    //
    lstrcpyn( m_szFolder, pszFolder, MAX_PATH );

    m_pListHead = m_pCurrentPosition = NULL;

}


///////////////////////////////////////////////////////////////////////////////
//
// Function name: CEnumerator::~CEnumerator
//
// Description:
//
// Return type:
//
//
///////////////////////////////////////////////////////////////////////////////
// 12/1/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
CEnumerator::~CEnumerator()
{
    //
    // Cleanup list
    //
    while( m_pListHead )
        {
        m_pCurrentPosition = m_pListHead->pNext;
        delete m_pListHead;
        m_pListHead = m_pCurrentPosition;
        }
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumerator::Enumerate
//
// Description   : This routine will enumerate all items in the quarantine
//                 folder.
//
// Return type   : BOOL
//
// Note:  This routine will throw a memory exception if an allocation fails.
//
///////////////////////////////////////////////////////////////////////////////
// 12/2/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
BOOL CEnumerator::Enumerate()
{
USES_CONVERSION;
    //
    // Build search path.
    //
    TCHAR szTemp[ MAX_PATH + 5 ];
    lstrcpyn( szTemp, m_szFolder, MAX_PATH );
    lstrcat( szTemp, _T("\\*.*") );

    //
    // Fire off find operation.
    //
    HANDLE hFindFirst;
    WIN32_FIND_DATA findData;
    hFindFirst = FindFirstFile( szTemp, &findData );
    if( hFindFirst == INVALID_HANDLE_VALUE )
        {
        return FALSE;
        }

    do
        {
        //
        // Skip other folders.
        //
        if( findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
            continue;

        //
        // Make sure this is a quarantine file.
        //
        wsprintf( szTemp, _T( "%s\\%s" ), m_szFolder, findData.cFileName );
        if( QSPAKSTATUS_OK != QsPakIsQserverFile( T2A(szTemp) ) )
            continue;

        //
        // Bump item count.
        //
        m_ulCount++;

        //
        // Allocate and populate this item
        //
        CQItem *p = new CQItem( findData.cFileName );

        //
        // Insert this item into the list
        //
        if( m_pListHead == NULL )
            {
            m_pListHead = p;
            m_pCurrentPosition = p;
            }
        else
            {
            m_pCurrentPosition->pNext = p;
            m_pCurrentPosition = p;
            }
        }
    while( FindNextFile( hFindFirst, &findData ) );

    //
    // Set current position to begining of list
    //
    Reset();

    //
    // Cleanup
    //
    FindClose( hFindFirst );

    return TRUE;
}



///////////////////////////////////////////////////////////////////////////////
//
// Function name : CEnumerator::GetNext
//
// Description   : This routine will retrieve the next item in the enumeration.
//
//
// Return type   : BOOL
//
// Argument      : LPTSTR pszFileName - MAX_PATH sized buffer to recieve filename.
//
///////////////////////////////////////////////////////////////////////////////
// 12/2/98 - DBUCHES: Function created / header added
///////////////////////////////////////////////////////////////////////////////
BOOL CEnumerator::GetNext( LPTSTR pszFileName )
{
    //
    // Q: Are we at the end of the list?  If so, then no more elements
    //
    if( m_pCurrentPosition == NULL )
        return FALSE;

    //
    // Copy data.
    //
    lstrcpy( pszFileName, *m_pCurrentPosition );
    m_pCurrentPosition = m_pCurrentPosition->pNext;

    //
    // Success!
    //
    return TRUE;
}





