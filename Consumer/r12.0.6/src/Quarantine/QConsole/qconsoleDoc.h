//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// qconsoleDoc.h : interface of the CQconsoleDoc class
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

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "QuarantineDllLoader.h"

// For CTypedPtrArray
#include <afxtempl.h>


enum RiskLevel
{
	RiskLevel_NotApplicable = 0,
	RiskLevel_Low = 1,
	RiskLevel_Medium,
	RiskLevel_High
};

enum RiskCategory
{
	RiskCategory_Overall = ccEraser::IAnomaly::Last_Prop + 1,
	RiskCategory_Performance = ccEraser::IAnomaly::Performance,
	RiskCategory_Privacy = ccEraser::IAnomaly::Privacy,
	RiskCategory_Removal = ccEraser::IAnomaly::Removal,
	RiskCategory_Stealth = ccEraser::IAnomaly::Stealth
};


enum ColumnMap_ViralView
{
	ColumnMap_ViralView_First = 0
	, ColumnMap_ViralView_RiskName = ColumnMap_ViralView_First
	, ColumnMap_ViralView_DateAdded
	, ColumnMap_ViralView_Category
	, ColumnMap_ViralView_RiskLevel
	, ColumnMap_ViralView_SubmittedDate
	, ColumnMap_ViralView_Status
	, ColumnMap_ViralView_Last = ColumnMap_ViralView_Status
};

enum ColumnMap_ExpandedThreatView
{
	ColumnMap_ExpandedThreatView_First = 0
	, ColumnMap_ExpandedThreatView_RiskName = ColumnMap_ExpandedThreatView_First
	, ColumnMap_ExpandedThreatView_DateAdded
	, ColumnMap_ExpandedThreatView_Category
	, ColumnMap_ExpandedThreatView_RiskLevel
	, ColumnMap_ExpandedThreatView_Dependencies
	, ColumnMap_ExpandedThreatView_SubmittedDate
	, ColumnMap_ExpandedThreatView_Status
	, ColumnMap_ExpandedThreatView_Last = ColumnMap_ExpandedThreatView_Status
};

//const int ColumnMax_ViralView = ColumnMap_ViralView_Last;
const int ColumnCount_ViralView = ColumnMap_ViralView_Last + 1;

//const int ColumnMax_ExpandedThreatView = ColumnMap_ExpandedThreatView_Last;
const int ColumnCount_ExpandedThreatView = ColumnMap_ExpandedThreatView_Last + 1;


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

    int GetSortCol() { return m_iCurSortColumn; }
    void SetSortCol(int iCol) { if(-1 != iCol)
									m_iCurSortColumn = iCol; }

	int MapColumnIDToIndex(int iColumnID);
	int MapColumnIndexToID(int iColumnIndex);

    CString& GetQuarantineFolder(){ return m_sQuarantineFolder; }
    void SetQuarantineFolder(LPCTSTR szFolder){ m_sQuarantineFolder = szFolder; }
    
    int GetItemCount(){ return m_iTotalItems; }
    void SetItemCount(int iNewVal){ m_iTotalItems = iNewVal; }

    int GetQuarantineItemCount(){ return m_iQuarantineItems; }
    void SetQuarantineItemCount(int iNewVal){ m_iQuarantineItems = iNewVal; }

    int GetBackupItemCount(){ return m_iBackupItems; }
    void SetBackupItemCount(int iNewVal){ m_iBackupItems = iNewVal; }

    //int GetSubmittedItemCount(){ return m_iSubmittedItems; }
    //void SetSubmittedItemCount(int iNewVal){ m_iSubmittedItems = iNewVal; }

    //int GetThreatBackupItemCount(){ return m_iThreatBackupItems; }
    //void SetThreatBackupItemCount(int iNewVal){ m_iThreatBackupItems = iNewVal; }

	int GetExpandedThreatItemCount() {return m_iExpandedThreatItems;}
    void SetExpandedThreatItemCount(int iNewVal) {m_iExpandedThreatItems = iNewVal;}

	int GetViralItemCount() {return m_iViralItems;}
    void SetViralItemCount(int iNewVal) {m_iViralItems = iNewVal;}

    BOOL GetStartupScanNeeded(){ return m_bStartupScanNeeded; }

    // Function to get the Original ANSI file name taking into account that it might
    // be an SLFN
    BOOL GetFullANSIFilePath(IQuarantineItem* pQItem, CString& cszOutPath, DWORD dwRemediationIndex = 0);
    BOOL GetANSIFileNameOnly(IQuarantineItem* pQItem, CString& cszOutPath, DWORD dwRemediationIndex = 0);
    BOOL GetANSIFilePathOnly(IQuarantineItem* pQItem, CString& cszOutPath, DWORD dwRemediationIndex = 0);
    
    BOOL GetThreatName(IQuarantineItem* pQItem, CString& cszThreatName);

	DWORD GetRiskLevelNumber(IQuarantineItem* pQItem, DWORD dwCategory);
	bool GetRiskLevelText(DWORD dwRiskLevelNumber, CString& sRiskLevel);
	RiskLevel GetRiskLevel(DWORD dwRiskLevelNumber);
	DWORD GetRiskLevelAverage(IQuarantineItem* pQItem);
	bool GetRiskLevelCategoryText(RiskCategory riskCategory, DWORD dwRiskLevelNumber, CString& sHeader, CString& sDescription);
	DWORD GetDefaultDamageFlagValue(IQuarantineItem* pQuarItem);

    bool IsFileCompressed(LPCTSTR szFileName);


    // Operations
    void RebuildItemArrays();
    CMapPtrToPtr* GetItemList(int iMode = -1);
    BOOL DeleteItemFromLists(IQuarantineItem* pItem, BOOL bAll = TRUE );

    void RecalcItemTypes();

    BOOL IsNT(){ return m_bNT; }
	BOOL IsWinXP() { return m_bWinXP; }

    BOOL IsQConsoleEnabled(){ return m_bQConsoleEnabled; }

	bool GetIntroEnabled();
	bool SetIntroEnabled(bool bEnabled);

	HRESULT LoadExpandedThreatShowSetting();
	bool GetExpandedThreatShowSetting();

	bool GetDefsDate(SYSTEMTIME& stDefsDate);
	bool GetDefsDate(LPTSTR szDefsDate);

public:
    // The one quarantineDLL object
	QuarDLL_Loader	m_QuarDllLoader;
	QuarOPTS_Loader	m_QuarOptcLoader;
    IQuarantineDLL* m_pQuarantine;

    // Quarantine options object
    QuarOPTS_Loader m_QuarOptsLoader;
    IQuarantineOpts* m_pOpts;

	static CString m_sProductName;
	static CString m_sAppName;

private:
	// Operations
    void ResetArrays();
    void BuildSubsetList(int iMode);
    BOOL InitVirusDefs();

private:    
    // List view styles - see definitions below
    DWORD   m_dwListViewStyle; 

    // Quarantine folder.
    CString m_sQuarantineFolder;

    // Current view mode (Expanded threats, viral);
    int     m_iViewMode;

    // Virus definitions dir.
    CString m_sDefDir;

    // Arrays of IQuarantine objects.  One for each type of object
    // Data maps for fast access to arrays
    CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>    m_aMapAllItems;
    CTypedPtrMap<CMapPtrToPtr, IQuarantineItem*, IQuarantineItem*>    m_aMapCurrentSubset;

    // Current column we are sorting by
    int     m_iCurSortColumn;

    // Number of each type of item
    int m_iTotalItems;
    int m_iQuarantineItems;
    int m_iBackupItems;
    //int m_iSubmittedItems;
    //int m_iThreatBackupItems;
	int m_iExpandedThreatItems;
	int m_iViralItems;

    // Current definitions date.
    SYSTEMTIME m_stCurDefsDate;
	TCHAR m_szCurDefsDate[13];

    // Previous definitions date.
    SYSTEMTIME  m_stDefsDate;

    // Startup scan needed
    BOOL    m_bStartupScanNeeded;

    // Running on a server?
    //BOOL    m_bCorporateMode;

    // Running on NT?
    BOOL    m_bNT;

	// Running on XP?
	BOOL	m_bWinXP;

    // Are we within our trial period?
	BOOL    m_bQConsoleEnabled;

	bool m_bShowExpandedThreats;
};


/////////////////////////////////////////////////////////////////////////////
// Size arrays should grow when neccessary.
//#define ARRAY_ALLOC_SIZE        0x00000080

/////////////////////////////////////////////////////////////////////////////
// Hints for views of this document
const DWORD LISTVIEW_STYLE_CHANGE = 0x00000001;
const DWORD LISTVIEW_REFRESH      = 0x00000002;
//#define TREEVIEW_SEL_CHANGE     0x00000003
const DWORD SET_VIEW_FILTER       = 0x00000004;
const DWORD LISTVIEW_SORTITEMS    = 0x00000005;
const DWORD LISTVIEW_DELETE_ITEMS = 0x00000006;
const DWORD LISTVIEW_NEW_DEFS     = 0x00000007;

/////////////////////////////////////////////////////////////////////////////
// View mode filters
const DWORD VIEW_MODE_ALL              = 0x00000000;
//#define VIEW_MODE_QUARANTINE    0x00000001
//#define VIEW_MODE_BACKUP        0x00000002
//#define VIEW_MODE_SUBMITTED     0x00000004
//#define VIEW_MODE_THREAT_BACKUP 0x00000005
const DWORD VIEW_MODE_EXPANDED_THREATS = 0x00000006;
const DWORD VIEW_MODE_VIRAL_THREATS    = 0x00000007;


//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_QCONSOLEDOC_H__396DC35F_A5D8_11D1_A51E_0000C06F46D0__INCLUDED_)
