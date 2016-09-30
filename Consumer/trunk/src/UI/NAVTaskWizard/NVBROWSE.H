////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//*************************************************************************
//
// $Header:   S:/NAVW/VCS/nvbrowse.h_v   1.2   22 Oct 1998 19:39:12   CEATON  $
//
// Description:
//      Header for NAV Multiple-Selection Folder browse control.
//
// Contains:
//      CNVBrowse
//      CNVBrowseNode
//      CNVBrowseItem
//*************************************************************************
// $Log:   S:/NAVW/VCS/nvbrowse.h_v  $
// 
//    Rev 1.2   22 Oct 1998 19:39:12   CEATON
// Added member function to return number of selected items.
// 
// 
//    Rev 1.1   22 Apr 1998 16:08:24   DALLEE
// Changes to compile for Alpha.
// Had to implement operators for = != > < for CNVBrowseItem and
// CIconID which are stored in STL collections.
// 
//    Rev 1.0   20 Apr 1998 23:12:10   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __NVBROWSE_H
#define __NVBROWSE_H

#ifndef _SHLOBJ_H_
#include "shlobj.h"
#endif

#ifndef __TREECTRL_H
#include "treectrl.h"
#endif

#ifndef _LIST_
#include <list>
#endif

/////////////////////////////////////////////////////////////////////////////
// CNVBrowseItem - selected item from browse control.

class CNVBrowseItem
{
public:
    CNVBrowseItem();
    CNVBrowseItem(const CNVBrowseItem &item);

    BOOL SetName( LPCTSTR lpszName );
    LPCTSTR GetName() const;

    BOOL SetSubDirs( BOOL bSubDirs );
    BOOL GetSubDirs() const;

    virtual ~CNVBrowseItem();

    bool operator==(const CNVBrowseItem& item2) const;
    bool operator!=(const CNVBrowseItem& item2) const;
    bool operator>(const CNVBrowseItem& item2) const;
    bool operator<(const CNVBrowseItem& item2) const;

private:
    LPTSTR  m_lpszName;
    BOOL    m_bSubDirs;
};


/////////////////////////////////////////////////////////////////////////////
// CIconID 
//
// The CIconID class maintains identification information for an icon.
// This is used to track icons have been loaded already for use in
// the folder browse tree control.
//

class CIconID
{
// Construction
public:
    CIconID();
    CIconID(const CIconID& iconID);
    CIconID(LPCTSTR pszSource, int nIndex = 0);

// Attributes
public:
    int     m_nIndex;
    LPTSTR  m_pszSource;

// Operations
public:
    int SetID(LPCTSTR pszSource, int nIndex = 0);

    bool operator==(const CIconID& iconID2) const;
    bool operator!=(const CIconID& iconID2) const;
    bool operator>(const CIconID& iconID2) const;
    bool operator<(const CIconID& iconID2) const;

    const CIconID& operator=(const CIconID& iconIDSrc);

// Implementation
public:
    virtual ~CIconID();
};


class IValidate
{
public:
    virtual void OnItemChanged () = 0;

};

/////////////////////////////////////////////////////////////////////////////
// CNVBrowse - Multiple-selection folder browse control.

class CNVBrowse : public CNVTreeCtrl
{
// Construction
public:
	static int CALLBACK CompareNodes(LPARAM lNode1, LPARAM lNode2, LPARAM lISF);
    CNVBrowse();

// Attributes
public:
    enum STATEINDEX
    {
        NONE,
        UNCHECKED,
        CHECKED,
        GRAYCHECKED
    };

    ::std::list<CNVBrowseItem> m_listSelectedItems;

// Operations
public:
    BOOL Attach( HWND hWnd );
    HWND Detach();

	int LocateImage(CIconID& iconID);
	int AddImage(HICON hIcon, CIconID& iconID);
    int Init( HINSTANCE hResInst );
    int UpdateSelectedList();
    int GetTotalSelectedItems();

// Implementation
public:
    void SetValidatingFunction ( IValidate* pValidateSink );
    virtual ~CNVBrowse();

	HIMAGELIST  m_hImageList;
    HIMAGELIST  m_hStateImageList;

    ::std::list<CIconID> m_listIcons;

    LRESULT OnNotify( WPARAM wParam, LPARAM lParam );

protected:
	void Validate ();
    IValidate* m_pValidateSink;

    static LRESULT CALLBACK SubClassWndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
    WNDPROC m_pfnOrigWndProc;

    void virtual OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
    void virtual OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
    void virtual OnLClick(NMHDR* pNMHDR, LRESULT* pResult);

    void CheckItem( HTREEITEM hItem, int nCheckIndex );
    void CheckItemAndChildren( HTREEITEM hItem, int nCheckIndex );

    void AddSelectedItems( HTREEITEM hItem );
};


/////////////////////////////////////////////////////////////////////////////
// CNVBrowseNode - Individual tree item node. Stored as user data of each
// tree item.

class CNVBrowseNode 
{
// Construction
public:
	DWORD GetName(LPTSTR lpszName, DWORD dwBufferSize);
    CNVBrowseNode();
    
// Attributes
public:
    CNVBrowse       *m_pTree;
    HTREEITEM       m_hItem;

    IShellFolder    *m_pISF;
    IShellFolder    *m_pParentISF;
	LPITEMIDLIST    m_pIDL;

	ULONG           m_ulAttr;

// Operations
public:
    int AddChildren();

// Implementation
public:
    virtual ~CNVBrowseNode();
	int RegisterImage(LPITEMIDLIST pIDL, UINT uFlags);
};

/////////////////////////////////////////////////////////////////////////////

#endif // !__NVBROWSE_H
