/////////////////////////////////////////////////////////////////////
// NETBRWSD.H - Created - 11/14/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/NETBRWSD.H_v   1.1   28 Aug 1998 17:49:40   JBRENNA  $
//
// Description:
//    Network Browser dialog header file.  This file defines the
//    CNetBrowsDlg class.  This class will be able to browse the
//    network for a network resource.  The type of resource, and the
//    items that you able to select will be selectable before this
//    browser is called.  This dialog will then be able to screen
//    off any items that you do not want to display, or will not
//    allow you to browse beneath certain items in the tree.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/NETBRWSD.H_v  $
// 
//    Rev 1.1   28 Aug 1998 17:49:40   JBRENNA
// Port changes from QAKN branch to trunk:
// 1. Rev 1.0.1.1   07 Jul 1998 12:06:04   DBuches
//    Added members to deal with the help button.
// 
//    Rev 1.0   21 Nov 1997 11:53:02   KSACKIN
// Initial revision.
// 
//    Rev 1.0   24 Apr 1997 20:33:36   JBRENNA
// Initial revision.
// 
//    Rev 1.0   13 Mar 1997 15:10:42   RFULLER
// Initial revision
// 
//    Rev 1.5   09 Jan 1997 03:55:32   ksackin
// Added an OnHelp function for the network browser dialog's help button.
// 
//    Rev 1.4   20 Dec 1996 20:12:02   ksackin
// Added a new variable to the network browser dialog to support the getting and
// setting of the Browser Information static text field.
// 
//    Rev 1.3   16 Dec 1996 23:48:44   ksackin
// Made some changes to the network browser dialog to handle the size grip in
// NT 3.51.  Also added some fixes for Novell Client 32.
// 
//    Rev 1.2   09 Dec 1996 23:42:04   ksackin
// Added some features to the browser, and a couple of member function for
// screening off certain types of network resources.
// 
//    Rev 1.1   25 Nov 1996 00:40:32   ksackin
// Added minimum and maximum size information to the class so that we can
// restrict the sizing of the dialog.
// 
//    Rev 1.0   20 Nov 1996 02:30:30   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__NETBRWSD_H)
#define __NETBRWSD_H


/////////////////////////////////////////////////////////////////////
// Include Files.

#include "NResCont.h"
#include "NetTree.h"
#include "wndsize.h"

/////////////////////////////////////////////////////////////////////
// Function Prototypes

void TrackPopupMenuWithID(
    UINT  uMenuID,
    CWnd  *lpcCommandWindow
    );

/////////////////////////////////////////////////////////////////////
// Browser Types

#define BROWSER_TYPE_DEFAULT         1

#define BROWSER_TYPE_NLM             2
#define BROWSER_TYPE_FORWARD         3
#define BROWSER_TYPE_NETWORK_MESSAGE 4


/////////////////////////////////////////////////////////////////////////////
// CNetBrowsDlg dialog

class AFX_EXT_CLASS CNetBrowsDlg : public CDialog
{
// Construction
public:
	void DrawOverlapForSizeGrip( void );
	void SetClippingForSizeGrip( void );
                                        // standard constructor
	CNetBrowsDlg( DWORD dwBrowserType = BROWSER_TYPE_DEFAULT, CWnd* pParent = NULL);	
	~CNetBrowsDlg( void );

                                        // Access function to retrieve the network
                                        // tree's display item flags.
	void GetDialogTitle( CString *lpcReturnString );
	void GetInitialBrowserInformationText( CString *lpcReturnString );

    DWORD GetDisplayItemFlags( void );
	DWORD GetBrowseBeneathFlags( void );
	DWORD GetValidSelectionFlags( void );

    LPCTSTR GetSelectedItemName( void ) {return m_cszSelectedItemName;};
    DWORD   GetSelectedItemType( void ) {return m_dwSelectedItemType;};

	void SetBrowserType( DWORD dwBrowserType );

                                        // Access function to set the network
                                        // tree's display item flags.
	void SetDialogTitle( LPCTSTR cszNewTitle );
	void SetInitialBrowserInformationText( LPCTSTR cszInitialBrowserInformationText );
	void SetInitialBrowseItemTitle( LPCTSTR cszInitialBrowseItemTitle );
	void SetInitialBrowseItemName( LPCTSTR cszInitialBrowseItemName );




                                        // Access function to set the network
                                        // tree's display item flags.
    void SetDisplayItemFlags( DWORD dwNewDisplayItemFlags );
	void SetBrowseBeneathFlags( DWORD dwNewBrowseBeneathFlags );
	void SetValidSelectionFlags( DWORD dwNewValidSelectionFlags );

                                        // Determine if the item that is selected
                                        // is a valid item to return from this
                                        // dialog.
    BOOL    ValidResourceTypeSelected( void );

                                        // Set up help filename and topic ID
    void SetHelpFileInfo( int iTopicID, LPCTSTR cszHelpFileName );
    
// Dialog Data
	//{{AFX_DATA(CNetBrowsDlg)
//	enum { IDD = IDD_NETBROWS_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetBrowsDlg)
	public:
	virtual int DoModal();
	virtual BOOL DestroyWindow();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void InitializeBrowserDialog( void );
	HICON                   m_hIcon;
    HANDLE                  m_hEnumResource;
    CWindowResizer          m_cWindowResizer;
    CNetworkTreeCtrl        m_cNetTreeCtrl;

    int m_nMinTrackSizeX;               // Minimum window track size Horizontally
    int m_nMinTrackSizeY;               // Minimum window track size Vertically.

                                        // Initial dialog state variables.
    CString m_cszDialogTitle;
    CString m_cszInitialBrowseItemTitle;
    CString m_cszInitialBrowseItemName;
    CString m_cszInitialBrowserInformationText;

                                        // Browser type.
    DWORD   m_dwBrowserType;
                                        // Browser display item flags.
    DWORD   m_dwValidSelectionFlags;
    DWORD   m_dwBrowseBeneathFlags;
    DWORD   m_dwDisplayItemFlags;

                                        // These items are for the return
                                        // values.  You will be able to query
                                        // these after the dialog exits.
    CString m_cszSelectedItemName;
    DWORD   m_dwSelectedItemType;
                                        // Help file info.
                                        //
    int     m_iHelpButtonTopicID;
    CString m_sHelpFileName;

	// Generated message map functions
	//{{AFX_MSG(CNetBrowsDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnItemexpandingNetTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDblclkNetTree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnSelchangedNetTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnChangeServerName();
	afx_msg void OnRclickNetTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void CalculateMinMax( void );
};


#endif // if !defined(__NETBRWSD_H)
