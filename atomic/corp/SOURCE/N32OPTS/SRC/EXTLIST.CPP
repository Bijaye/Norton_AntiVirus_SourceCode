/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/extlist.cpv   1.3   09 Nov 1998 11:42:58   rchinta  $
/////////////////////////////////////////////////////////////////////////////
//
//
// ExtList.cpp: implementation of the CExtensionList class.
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/extlist.cpv  $
// 
//    Rev 1.3   09 Nov 1998 11:42:58   rchinta
// Reverting to rev 1.1 since the previous changes/fix was not
// included in the functional spec of Barstow.
// (The change is also incomplete).
// 
//    Rev 1.1   16 May 1997 18:34:12   DBUCHES
// Fixed bug with RemoveItem() function.
//
//    Rev 1.0   14 May 1997 14:58:14   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "n32opts.h"
#include "ExtList.h"
#include "navoptx.h"


#include "options.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CExtensionList::CExtensionList()
{
    m_extItems.RemoveAll();
    m_iTotalSize = 0;
}

CExtensionList::CExtensionList(CString& sExts)
{
    Init(sExts, FALSE);
}

CExtensionList::CExtensionList(CString & sExts, BOOL bScanNoExtFile)
{
    Init(sExts, m_bScanNoExtFile);
}


void CExtensionList::Init(CString & sExts, BOOL bScanNoExtFile)
{
    CString s;
    LPTSTR pTok;

    // Start fresh
    m_extItems.RemoveAll();
    m_iTotalSize = 0;


    // Copy list to scratch buffer
    LPTSTR szExts = new TCHAR[sExts.GetLength() + sizeof(TCHAR)];
    ASSERT(szExts);
    _tcscpy(szExts, sExts);

    // Add each extension to list
    pTok = _tcstok(szExts, _T(" "));
    while( pTok != NULL )
        {
        s = pTok;
        // Figure out how big we are.
		// 1 is added since the extensions are retained
		// within the file with a space in between
        m_iTotalSize += s.GetLength() + 1;

        // Add to list
        m_extItems.AddTail(s);

        // Get next
        pTok = _tcstok(NULL, _T(" "));
        }

    delete [] szExts;

    m_bScanNoExtFile = bScanNoExtFile;
}

CExtensionList::~CExtensionList()
{
    m_extItems.RemoveAll();
}

BOOL CExtensionList::AddItem(CString & sItem)
{
    if((sItem.GetLength() + 1 + m_iTotalSize) <= MAX_EXTENSION_LIST_SIZE)
        {
        // Convert to upper case.
        sItem.MakeUpper();

        // Add to list
        m_extItems.AddTail(sItem);

        // adjust total size.
        m_iTotalSize += sItem.GetLength() + 1;
        return TRUE;
        }

    return FALSE;
}

void CExtensionList::RemoveItem(CString & sItem)
{
    // Find our item, and nuke it.
    POSITION tPos = m_extItems.GetHeadPosition();
    POSITION tOldPos = NULL;
    CString s = _T("NonMatchingItem");

    while(tPos != NULL)
        {
	// Get next item.
	tOldPos = tPos;
	s = m_extItems.GetNext(tPos);

	// Check for match.
        if(s == sItem)
            {
            m_extItems.RemoveAt(tOldPos);
            break;
	    }

        }

}

int CExtensionList::IsItemValid(CString & sItem)
{
    // Make sure this item is unique;
    POSITION tPos = m_extItems.GetHeadPosition();
    while(tPos != NULL)
        {
        CString s = m_extItems.GetNext(tPos);

        if(s == sItem)
            {
            return ITEM_DUPLICATE_NAME;
            }
        }

    // Make sure this item is valid
    // Make sure there are no illegal MSDOS characters.
    if(sItem.FindOneOf(_T(" /\\+\"|.,;:'><")) != -1)
        return ITEM_INVALID_NAME;

    // Need to make sure that the item does not contain *ONLY* wildcards
    int iNumWildCardChars = 0;
    int iLength = sItem.GetLength();

    // Check first char.  If not wild, everything is cool.
    if( sItem.Find(_T("*")) == 0  ||
        sItem.Find(_T("?")) == 0 )
        {
        // Found a wild char.  Now, since we know the first
        // character is NOT a DBCS lead byte, move to the
        // next char, and check it.
        iNumWildCardChars++;
        LPTSTR szBuffer = sItem.GetBuffer(0);
        for(int i = 1; i < iLength; i++)
            {
            CString sTemp = &szBuffer[i];
            if( sTemp.Find(_T("*")) == 0  ||
                sTemp.Find(_T("?")) == 0 )
                {
                iNumWildCardChars++;
                }
            }
        }

    // Make sure we are not all wild chars
    if(iNumWildCardChars >= iLength)
        return ITEM_INVALID_NAME;

    return ITEM_OK;
}
