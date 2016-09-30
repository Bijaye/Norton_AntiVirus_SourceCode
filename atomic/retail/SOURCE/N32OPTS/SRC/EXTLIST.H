/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/N32OPTS/VCS/extlist.h_v   1.2   09 Nov 1998 11:42:04   rchinta  $
/////////////////////////////////////////////////////////////////////////////
//
//
// ExtList.h: interface for the CExtensionList class.
//
//
//
//
//
//
//
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/N32OPTS/VCS/extlist.h_v  $
// 
//    Rev 1.2   09 Nov 1998 11:42:04   rchinta
// Reverting to rev 1.0 since the previous fix is incomplete and
// was not included in the functional spec of Barstow.
// 
//    Rev 1.0   14 May 1997 15:02:30   DBUCHES
// Initial revision.
/////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXTLIST_H__8E35FFDD_C586_11D0_8AD6_0060977341AC__INCLUDED_)
#define AFX_EXTLIST_H__8E35FFDD_C586_11D0_8AD6_0060977341AC__INCLUDED_

#include <afxtempl.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CExtensionList
{

public:
	CExtensionList();
    CExtensionList(CString& sExts);
	virtual ~CExtensionList();



public:
	int IsItemValid(CString & sItem);
	void RemoveItem(CString & sItem);
	BOOL AddItem(CString &sItem);
    void Init(CString & sExts);

    // List of items
    CList<CString, CString &> m_extItems;

private:
	int m_iTotalSize;
};


#define ITEM_OK                 0
#define ITEM_INVALID_NAME       1
#define ITEM_DUPLICATE_NAME     2



#endif // !defined(AFX_EXTLIST_H__8E35FFDD_C586_11D0_8AD6_0060977341AC__INCLUDED_)
