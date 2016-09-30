/////////////////////////////////////////////////////////////////////
// ATListPg.h : header file
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/ALERTUI/VCS/ATLISTPG.H_v   1.1   25 Apr 1997 11:04:42   JBRENNA  $
//
// Description:
//      This class is used to handle most of the UI tasks associated with
//      the alert target property pages for NAV.  Since it is inherited
//      from "CAlertPropertyPage", it automatically contains functionality
//      needed to interface with the "CAlertOptions" class.  This class
//      does most of the "grunt-work" for UI, providing virtual functions
//      to be implemented by derived classes for more detailed tasks.
//
//      In order for a derived class to effectively use this class, the derived
//      class needs to do the following:
//
//          *) initialize the alert type data member
//          *) define help map arrays and initialize the context help map pointers
//          *) initialize the "how to" index for this dialog by calling
//             SetHowToMenuIndex()
//          *) initialize some control IDs
//             -- m_listCtrlContextMenuID
//             -- m_listCtrlWhatsThisMenuID
//             -- m_listCtrlWhatsThisHelpID
//             -- m_listCtrlID
//          *) initialize pointers to some base controls
//             -- m_listCtrlPtr
//             -- m_addButtonPtr
//             -- m_modifyButtonPtr
//             -- m_removeButtonPtr
//          *) initialize the column header arrays
//
//      See any of the derived alert target property pages for examples.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/ALERTUI/VCS/ATLISTPG.H_v  $
// 
//    Rev 1.1   25 Apr 1997 11:04:42   JBRENNA
// Ported from Tesla.
// 
//    Rev 1.0   15 Apr 1997 20:08:48   sedward
// Initial revision.
//
//
/////////////////////////////////////////////////////////////////////


#ifndef ALERT_LIST_PROPERTY_PAGE_HEADER
#define ALERT_LIST_PROPERTY_PAGE_HEADER


#include <afxtempl.h>
#include "ATPropPg.h"


#define LIST_CTRL_WIDTH_PIXELS      280
#define MAX_LIST_SUBSTRING_LEN      100

/////////////////////////////////////////////////////////////////////////////
//
// CAlertListPropertyPage
//
/////////////////////////////////////////////////////////////////////////////

class CAlertListPropertyPage : public CAlertPropertyPage
{
    DECLARE_DYNCREATE(CAlertListPropertyPage)

// Construction
public:
    CAlertListPropertyPage();
    CAlertListPropertyPage(UINT  dlgID);
    ~CAlertListPropertyPage();

// Dialog Data
    //{{AFX_DATA(CAlertListPropertyPage)
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CAlertListPropertyPage)
	public:
    virtual void OnOK(void);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
    //{{AFX_MSG(CAlertListPropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClickAlertsTargetsList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAddButton();
	afx_msg void OnModifyButton();
	afx_msg void OnRemoveButton();
    afx_msg void OnHandleAdd(void);
    afx_msg void OnHandleModify(void);
    afx_msg void OnHandleRemove(void);
	afx_msg void OnDblclkAlertsTargetsList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Data members
public:
    CListCtrl*                  m_listCtrlPtr;
    CButton*                    m_removeButtonPtr;
    CButton*                    m_modifyButtonPtr;
    CButton*                    m_addButtonPtr;

protected:
    CArray<CString, CString&>   m_columnHeaders;
    CImageList                  m_imageList;
    int                         m_clickedItemIndex;
    int                         m_totalDisplayStrings;
    UINT                        m_listCtrlID;
    UINT                        m_listCtrlContextMenuID;
    UINT                        m_listCtrlWhatsThisHelpID;
    UINT                        m_listCtrlWhatsThisMenuID;

// Member functions
public:
    //virtual int     AddTargetToListCtrl(CAlertTarget*  pAlertTarget);
    virtual BOOL    GetDisplayString(CAlertTarget*  pAlertTarget
                                                        , CString&  displayStr
                                                        , int  index);
    virtual BOOL    HandleTargetEdit(CAlertTarget*  pAlertTarget, int  mode);
    virtual void    HandleRclickListCtrl(void);

    int             AddTargetToListCtrl(CAlertTarget*  pAlertTarget
                                                    , int  editMode = EDIT_ADD);
    BOOL            GetClickedItemIndex(void);
    void            HandleListCtrlWhatsThis(void);
    int             InitListbox(void);
    int             RemoveSelectedListCtrlItems(void);
    void            SynchronizeEditButtonsAndContextMenu(CMenu  *menuPtr);
    int             SynchronizeListCtrlAndEditButtons(void);
    BOOL            ToggleListItemCheckbox(int  itemIndex);
};

#endif  // ALERT_LIST_PROPERTY_PAGE_HEADER