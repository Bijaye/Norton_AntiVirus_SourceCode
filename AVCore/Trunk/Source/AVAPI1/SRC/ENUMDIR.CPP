// Copyright 1997 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVAPI1/VCS/enumdir.cpv   1.1   11 Dec 1997 23:30:16   VGLOSTE  $
//
// ENUMDIR.CPP
//
// CEnumSingleDir definition.
//
// Class for enumerating all files in a single directory.
// Helper class for CEnumFiles.
//
//************************************************************************
// $Log:   S:/AVAPI1/VCS/enumdir.cpv  $
// 
//    Rev 1.1   11 Dec 1997 23:30:16   VGLOSTE
// Revamped the CEnumSingleDir class.  The file security program Cerebrus
// can cause the normal iteration process to break (ugh!) so I have 
// changed this class to read the entire directory into a linked list 
// when asked for the first item.  This makes it impervious to the ravages
// of Cerebrus.  Two small helper classes, CEnumLinkedItem and
// CEnumLinkedList have been added to implement the linked list.
// 
//    Rev 1.0   13 May 1997 18:01:48   DALLEE
// Initial revision.
//************************************************************************

#include "platform.h"
#include "file.h"
#include "enumfile.h"

//************************************************************************
// CEnumLinkedList
//************************************************************************
// This helper class allows us to cache the contents of a directory.
// This is necessary because the Win95 add-on file security product
// Cerebrus breaks the normal depth-first scanning of a drive.  If
// a subdirectory is protected and we attempt to access the files in
// it, this breaks the open iteration handle (gotten from FindFirstFile)
// for the parent directory and leaves the subsequent subdirectories 
// unscanned.  We avoid this by getting the complete list of the contents
// before opening any files and storing the list in the CEnumLinkedList
// object.

//************************************************************************
// CEnumLinkedList::~CEnumLinkedList()
//
// Destructor which deletes all items.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 12/11/1997 VGLOSTE, created.
//************************************************************************

CEnumLinkedList::~CEnumLinkedList()
{	Clear();	}

//************************************************************************
// CEnumLinkedList::Clear()
//
// Function which deletes all items.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 12/11/1997 VGLOSTE, created.
//************************************************************************

void CEnumLinkedList::Clear()
{	while(m_pHead)
	{	m_pCurrent = m_pHead;
		m_pHead = m_pHead->GetNext();
		delete m_pCurrent;
	}
	m_pHead = m_pTail = m_pCurrent = NULL;
} // Clear()

//************************************************************************
// CEnumLinkedList::Next()
//
// Returns the data for the next item in the list and advances
// the pointer to the next item.
//
// Parameters:
//      None.
//
// Returns:
//      Data for the item.
//************************************************************************
// 12/11/1997 VGLOSTE, created.
//************************************************************************

FINDDATA *CEnumLinkedList::Next() 
{	FINDDATA *pData;

	if(m_pCurrent == NULL)
		return NULL;
	pData = m_pCurrent->GetData();
	m_pCurrent = m_pCurrent->GetNext();
	return pData;
} // Next()

//************************************************************************
// CEnumLinkedList::Add()
//
// Adds an item at the end of the list.
//
// Parameters:
//      Item data.
//
// Returns:
//      TRUE for success and FALSE for failure.
//************************************************************************
// 12/11/1997 VGLOSTE, created.
//************************************************************************

BOOL CEnumLinkedList::Add(const FINDDATA *pData) 
{	CEnumLinkedItem *pTemp;

	pTemp = new CEnumLinkedItem(pData);
	if(pTemp == NULL)
		return FALSE;
	if(m_pHead == NULL) // list is empty
		m_pHead = m_pTail = m_pCurrent = pTemp;
	else
	{	m_pTail->SetNext(pTemp);
		m_pTail = pTemp;
	}
	return TRUE;
} // Add()


//************************************************************************
// CEnumSingleDir
//************************************************************************

//************************************************************************
// CEnumSingleDir::CEnumSingleDir()
//
// Constructor which initializes all locals to default empty values.
// Init() function required to set path for class to operate on.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

CEnumSingleDir::CEnumSingleDir()
{
    Clear();
} // CEnumSingleDir()


//************************************************************************
// CEnumSingleDir::~CEnumSingleDir()
//
// Destructor free all mem and other such cleanup.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

CEnumSingleDir::~CEnumSingleDir()
{
    CloseAll();
    Clear();
} // ~CEnumSingleDir()


//************************************************************************
// CEnumSingleDir::Clear()
//
// Set all internal variables to default empty values.
// Note this function ONLY sets empty values. CloseAll() is used to
// free all items before calling this function.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumSingleDir::Clear()
{
    m_pszFileSpec  = NULL;

    memset( (LPFINDDATA)&m_rFind, 0, sizeof( m_rFind ) );
	m_list.Clear();

    return ( NO_ERROR );
} // Clear()


//************************************************************************
// CEnumSingleDir::CloseAll()
//
// Free all handles and allocated memory.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumSingleDir::CloseAll()
{
    if ( NULL != m_pszFileSpec )
    {
        MemFreePtr( m_pszFileSpec );
        m_pszFileSpec = NULL;
    }
    return ( NO_ERROR );
} // CloseAll()


//************************************************************************
// CEnumSingleDir::Init()
//
// Sets the file specification to enumerate.
// Input should be of the form
//      "C:\Program Files\*.*" to enumerate all files in a directory.
//      "C:\Program Files" will return only one item - the root directory
//          entry for "Program Files".
//
// Parameters:
//      LPCTSTR lpszFileSpec            File specification to enumerate.
//
// Returns:
//      NO_ERROR                        On success.
//      ERROR_NOT_ENOUGH_MEMORY         If out of memory.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumSingleDir::Init( LPCTSTR lpszFileSpec )
{
    auto    int     nResult;

    SYM_ASSERT( NULL != lpszFileSpec );

    nResult = -1;

    CloseAll();
    Clear();

    // Save original filespec.

    m_pszFileSpec = (LPTSTR)MemAllocPtr( GMEM_MOVEABLE, ( STRLEN( lpszFileSpec ) + 1 ) * sizeof( TCHAR ) );

    if ( NULL == m_pszFileSpec )
    {
        goto BailOut;
    }

    STRCPY( m_pszFileSpec, lpszFileSpec );

    nResult = NO_ERROR;

BailOut:
    if ( NO_ERROR != nResult )
    {
#ifdef SYM_WIN32
        nResult = GetLastError();
#endif

        CloseAll();
        Clear();
    }

    return ( nResult );
} // Init()


//************************************************************************
// CEnumSingleDir::Next()
//
// This routine is called repeatedly to step through all files which match
// the input specification.
//
// Parameters:
//      None.
//
// Returns:
//      NO_ERROR                        On success.
//      ERROR_NO_MORE_FILES             If no more files matching spec.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumSingleDir::Next()
{
    auto    int     nResult;

    nResult = -1;

    SYM_ASSERT( NULL != m_pszFileSpec );

    if ( NULL == m_pszFileSpec )
    {
        SetLastError( ERROR_INVALID_DATA );
        goto BailOut;
    }

	// If list is empty, fill it up!

	if(m_list.IsEmpty())
	{	HFIND hFind;

		hFind = FileFindFirst( m_pszFileSpec, (LPFINDDATA)&m_rFind );
        if ( INVALID_HANDLE_VALUE == hFind )
		{	SetLastError(ERROR_NO_MORE_FILES);
			goto BailOut;
		}
		m_list.Add(&m_rFind);
		while(FALSE != FileFindNext(hFind, (LPFINDDATA)&m_rFind ))
			m_list.Add(&m_rFind);
		FileFindClose(hFind);
	}

	// Get first or next matching item
	FINDDATA *pData;
	pData = m_list.Next();
	if(pData == NULL)
	{	SetLastError(ERROR_NO_MORE_FILES);
		goto BailOut;
	}
	memcpy((LPFINDDATA)&m_rFind, pData, sizeof(m_rFind));

    nResult = NO_ERROR;


BailOut:
    if ( NO_ERROR != nResult )
    {
#ifdef SYM_WIN32
        nResult = GetLastError();
#endif
        memset( (LPFINDDATA)&m_rFind, 0, sizeof( m_rFind ) );
    }

    return ( nResult );
} // Next()


//************************************************************************
// CEnumSingleDir::Reset()
//
// This routine restarts the enumeration at the beginning of the current
// file specification.
//
// Parameters:
//      None.
//
// Returns:
//      Nothing.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

int CEnumSingleDir::Reset()
{
    auto    int     nResult;

    nResult = NO_ERROR;

    SYM_ASSERT( m_pszFileSpec );

	m_list.Reset();

    return ( nResult );
} // Reset()


//************************************************************************
// CEnumSingleDir::GetLongFile()
//
// This routine returns the long filename without path of the last item to
// match the file specification.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumSingleDir::GetLongFile()
{
    return ( m_rFind.szFileName );
} // GetLongFile()


//************************************************************************
// CEnumSingleDir::GetShortFile()
//
// This routine returns the short filename without path of the last item to
// match the file specification.
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Name of last file found.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumSingleDir::GetShortFile()
{
    // cAlternateFileName may be empty if there is no short name
    // alias. ( LFN == SFN, or SFN not available on that volume )

    return ( *m_rFind.szAlternateFileName ? m_rFind.szAlternateFileName
                                          : m_rFind.szFileName );
} // GetShortFile()


//************************************************************************
// CEnumSingleDir::GetFileSpec()
//
// This routine gets the filespec currently being enumerated.
// This is the filespec last passed to Init().
//
// Parameters:
//      None.
//
// Returns:
//      LPCTSTR     Filespec currently being enumerated.
//      NULL        Class has not yet been initialized.
//************************************************************************
// 03/13/1997 DALLEE, created.
//************************************************************************

LPCTSTR CEnumSingleDir::GetFileSpec()
{
    return ( NULL != m_pszFileSpec ? m_pszFileSpec
                                   : NULL );
} // GetFileSpec()

