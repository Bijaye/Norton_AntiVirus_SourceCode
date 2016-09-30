/* Copyright 1997 Symantec, Peter Norton Computing Group                */
//////////////////////////////////////////////////////////////////////////
//
// $Header:   S:/INCLUDE/VCS/dirtree.h_v   1.1   24 Apr 1997 20:44:36   JBRENNA  $
//
//////////////////////////////////////////////////////////////////////////
// $Log:   S:/INCLUDE/VCS/dirtree.h_v  $
// 
//    Rev 1.1   24 Apr 1997 20:44:36   JBRENNA
// Add VCS $Log ...$ line so "chkin"s have the comments inserted here.
//
// Created by: DBUCHES   03-07-97
//////////////////////////////////////////////////////////////////////////

// DirTree.h : header file
//
#ifndef _CDIRTREE_H
#define _CDIRTREE_H

#ifdef BUILDING_EXTDLL
#define DECLSPEC_DIRECTORYTREE _declspec( dllexport )
#else
#define DECLSPEC_DIRECTORYTREE _declspec( dllimport )
#endif


#include "shlobj.h"

/////////////////////////////////////////////////////////////////////////////
// CDirTree Class.
//
//  NOTE: This class is the base class implemtation.  Users should not
//  derive from this class.  Instead, derive from the CDirectoryTree class
//  declared below.
//
/////////////////////////////////////////////////////////////////////////////

class DECLSPEC_DIRECTORYTREE CDirTree : public CTreeCtrl
{
// Construction
public:
    CDirTree();

// Attributes
public:
	CObList m_xItemsList;

// Operations
public:
    // Public methods.

    // Ititalizes the tree.
    BOOL InitTree();

    // Resets the tree.
    void Reset();

    // Gets path from selected item.
    void GetPathFromItem(HTREEITEM hItem, CString &sPath);

    // Get/Set Tree flags
    DWORD GetFlags();
    DWORD SetFlags(DWORD dwFlags);

    // Selection list functions.  
	
    // Builds constructs the m_xItemsList member, adding an item
    // for each checked item in the list.  Returns number of items 
    // added to the list.
    int BuildSelectedList();
    
    // Deletes the list.
    void DeleteListItems();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDirTree)
	public:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDirTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDirTree)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	void AddCheckedItems(HTREEITEM hItem);
    void CheckChildren(HTREEITEM hItem, int iChecked);
	void AddDir(HTREEITEM hItem, CString &sFileName);
	DWORD m_dwFlags;


protected:
	HTREEITEM m_hRoot;
	CImageList m_xCheckImages;
    CImageList m_xDriveImages;
	DWORD GetItemFlags(HTREEITEM hItem);
	void SetItemFlags(HTREEITEM hItem, DWORD dwFlag);
    void CheckItems(HTREEITEM hItem, BOOL bChecked);

protected:
	// Overridables
	virtual void InitTopLevel();
    virtual void AddChildren(HTREEITEM hItem);
	virtual int GetNumChildren(HTREEITEM hItem);
	virtual int FindNumChildren(HTREEITEM hItem, CString &sPath);
	virtual void FindChildren(HTREEITEM hItem, CString &sPath);
};

/////////////////////////////////////////////////////////////////////////////
// CDirectoryTree Class

class DECLSPEC_DIRECTORYTREE CDirectoryTree : public CDirTree
{
public:
	// Constructors/Destructors
    CDirectoryTree();
    ~CDirectoryTree();

private:
    // Internal data members.
    LPMALLOC m_pMalloc;
    BOOL m_bUseFindFirst;
    int m_iFolderIcon;

private:
    BOOL InitImages();
	int AddImageIcon(LPSHELLFOLDER pFolder, LPITEMIDLIST pidl);
	void CStringFromStrRet(STRRET* p, LPITEMIDLIST pidl, CString &s);
	void PopulateTree(LPITEMIDLIST pidlDrivesFolder);
	void InitTopLevel();
    void FindChildren(HTREEITEM hItem, CString& sPath);
    void AddNode(HTREEITEM hParent, LPSHELLFOLDER pFolder, LPITEMIDLIST pidlDrivesFolder, BOOL bRoot = FALSE);
};

////////////////////////////////////////////////////////////////////////////
// CListItem -
//
////////////////////////////////////////////////////////////////////////////
class CListItem : public CObject
{
public:
	CListItem(){ m_bScanSubDirs = FALSE; };
	~CListItem(){};

public:
	CString m_sPathName;
	BOOL m_bScanSubDirs;

	BOOL ScanSubDirs(){ return m_bScanSubDirs; }
};


/////////////////////////////////////////////////////////////////////////////
// Internal tree data structure.
//
/////////////////////////////////////////////////////////////////////////////
typedef struct _TREEDATA {
    DWORD dwFlags;
    TCHAR dl;
    } TREEDATA, *LPTREEDATA;


/////////////////////////////////////////////////////////////////////////////
//
// The following constants define indexes into the image lists.
/////////////////////////////////////////////////////////////////////////////
#define MY_COMPUTER     0
#define REMOVABLE       1
#define FIXED           2
#define CDROM           3
#define REMOTE          4
#define FOLDER          5
#define OPEN_FOLDER     6

#define FOLDER_IMAGE        0
#define MY_COMPUTER_IMAGE   1


#define NOT_CHECKED     0x00000001
#define CHECKED         0x00000002
#define GRAY_CHECKED    0x00000004


// Flags defining tree behavior
#define DIRTREE_LOCAL_ONLY      0x00000001

// User flags.
#define ITEM_ALREADY_SCANNED    0x00000001      // Item has already been traversed.
#define ITEM_ROOT_ITEM          0x00000002      // This is a top level item (drive)


#endif
