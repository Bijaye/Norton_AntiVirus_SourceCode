/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/QCONSOLE/VCS/qconsoledoc.h_v   1.13   11 Jun 1998 15:48:28   DBuches  $
/////////////////////////////////////////////////////////////////////////////
//
// qconsoleDoc.h : interface of the CQconsoleDoc class
//
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/qconsoledoc.h_v  $
// 
//    Rev 1.13   11 Jun 1998 15:48:28   DBuches
// Added support for trial ware.
// 
//    Rev 1.12   12 May 1998 15:20:52   DBuches
// Added code for NT security support.
// 
//    Rev 1.11   11 May 1998 15:45:08   DBuches
// Added corporate repair folder.
// 
//    Rev 1.10   27 Apr 1998 16:10:44   DBuches
// Hooked up corporate mode registry keys.
// 
//    Rev 1.9   07 Apr 1998 16:49:40   DBuches
// 1st pass at startup scanning.
// 
//    Rev 1.8   03 Apr 1998 13:28:34   DBuches
// 1st pass at options.
// 
//    Rev 1.7   31 Mar 1998 15:02:04   DBuches
// Switched to using MFC data maps for keeping track of quarantine items.
// 
//    Rev 1.6   13 Mar 1998 15:22:18   DBuches
// Added item classification stuff.
// 
//    Rev 1.5   11 Mar 1998 15:16:20   DBuches
// Added DataMap objects for fast access to item lists.
// 
//    Rev 1.4   06 Mar 1998 17:16:58   DBuches
// Added DeleteItemFromList().
// 
//    Rev 1.3   06 Mar 1998 11:21:04   DBuches
// Added more data members.
// 
//    Rev 1.2   03 Mar 1998 17:04:24   DBuches
// Checked in more work in progress.
// 
//    Rev 1.1   02 Mar 1998 15:26:12   DBuches
// Added AVAPI context to document data structure.
// 
//    Rev 1.0   27 Feb 1998 15:10:26   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_QCONSOLEDOC_H__396DC35F_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
#define AFX_QCONSOLEDOC_H__396DC35F_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_

#include "iquaran.h"
#include "avapi.h"

// For CTypedPtrArray
#include <afxtempl.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CQconsoleDoc : public CDocument
{
protected: // create from serialization only
	CQconsoleDoc();
	DECLARE_DYNCREATE(CQconsoleDoc)

// Attributes
public:
                             
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CQconsoleDoc)
	public:
	virtual void OnCloseDocument();
	virtual BOOL OnNewDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CQconsoleDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CQconsoleDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
    // Accessors
    DWORD GetListViewStyle() { return m_dwListViewStyle; }
    void SetListViewStyle(DWORD dwStyle) { m_dwListViewStyle = dwStyle; }
    int GetViewMode() { return m_iViewMode; }
    void SetViewMode(int iNewMode) { m_iViewMode = iNewMode; }
    const CString GetVirusDefsDir() { return m_sDefDir; }
    HVCONTEXT GetAVAPIContext() { return m_hAVAPIContext; }
    int GetSortCol() { return m_iCurSortColumn; }
    void SetSortCol(int iCol ) { m_iCurSortColumn = iCol; }
    CString& GetQuarantineFolder(){ return m_sQuarantineFolder; }
    void SetQuarantineFolder( LPCTSTR szFolder ){ m_sQuarantineFolder = szFolder; }
    
    int GetItemCount(){ return m_iTotalItems; }
    void SetItemCount(int iNewVal){ m_iTotalItems = iNewVal; }
    int GetQuarantineItemCount(){ return m_iQuarantineItems; }
    void SetQuarantineItemCount(int iNewVal){ m_iQuarantineItems = iNewVal; }
    int GetBackupItemCount(){ return m_iBackupItems; }
    void SetBackupItemCount(int iNewVal){ m_iBackupItems = iNewVal; }
    int GetSubmittedItemCount(){ return m_iSubmittedItems; }
    void SetSubmittedItemCount(int iNewVal){ m_iSubmittedItems = iNewVal; }
    BOOL IsPasswordGood() { return m_bPasswordGood; }
    void SetPasswordGood( BOOL bGood = TRUE) { m_bPasswordGood = bGood; }
    BOOL GetStartupScanNeeded(){ return m_bStartupScanNeeded; }
    BOOL GetCorpMode() { return m_bCorporateMode; }
    CString GetRepairFolder() { return m_sRepairFolder; }

    // Operations
    void RebuildItemArrays();
    CMapPtrToPtr* GetItemList( int iMode = -1 );
    BOOL DeleteItemFromLists( IQuarantineItem* pItem, BOOL bAll = TRUE  );
    void RecalcItemTypes();
    BOOL IsNT(){ return m_bNT; }
    BOOL IsTrialValid(){ return m_bTrialValid; }

public:
    // The one quarantineDLL object
    IQuarantineDLL* m_pQuarantine;

    // Quarantine options object
    IQuarantineOpts* m_pOpts;

private:
	// Operations
    void ResetArrays();
    void BuildSubsetList( int iMode );
    BOOL InitVirusEngine();

private:    
    // List view styles - see definitions below
    DWORD   m_dwListViewStyle; 

    // Quarantine folder.
    CString m_sQuarantineFolder;

    // Corporate mode.
    CString m_sRepairFolder;

    // Current list view mode (Icon, Report, etc);
    int     m_iViewMode;

    // Virus definitions dir.
    CString m_sDefDir;

    // AVAPI context;
    HVCONTEXT m_hAVAPIContext;
    
    // Arrays of IQuarantine objects.  One for each type of object
    // Data maps for fast access to arrays
    CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>    m_aMapAllItems;
    CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>    m_aMapCurrentSubset;

    // Current column we are sorting by
    int     m_iCurSortColumn;

    // Number of each type of item
    int     m_iTotalItems;
    int     m_iQuarantineItems;
    int     m_iBackupItems;
    int     m_iSubmittedItems;

    // Password OK flag
    BOOL    m_bPasswordGood;
    
    // Current definitions date.
    SYSTEMTIME  m_stDefsDate;    

    // Startup scan needed
    BOOL    m_bStartupScanNeeded;

    // Running on a server?
    BOOL    m_bCorporateMode;

    // Running on NT?
    BOOL    m_bNT;

    // Are we within our trial period?
    BOOL    m_bTrialValid;


};

/////////////////////////////////////////////////////////////////////////////
// Size arrays should grow when neccessary.
#define ARRAY_ALLOC_SIZE        0x00000080

/////////////////////////////////////////////////////////////////////////////
// Hints for views of this document
#define LISTVIEW_STYLE_CHANGE   0x00000001
#define LISTVIEW_REFRESH        0x00000002
#define TREEVIEW_SEL_CHANGE     0x00000003
#define SET_VIEW_FILTER         0x00000004
#define LISTVIEW_SORTITEMS      0x00000005
#define LISTVIEW_DELETE_ITEMS   0x00000006
#define LISTVIEW_NEW_DEFS       0x00000007

/////////////////////////////////////////////////////////////////////////////
// View mode filters
#define VIEW_MODE_ALL           0x00000000
#define VIEW_MODE_QUARANTINE    0x00000001
#define VIEW_MODE_BACKUP        0x00000002
//#define VIEW_MODE_REPAIRED      0x00000003
#define VIEW_MODE_SUBMITTED     0x00000004



//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONSOLEDOC_H__396DC35F_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
