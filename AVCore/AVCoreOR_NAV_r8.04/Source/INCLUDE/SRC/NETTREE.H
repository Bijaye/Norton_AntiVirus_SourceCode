/////////////////////////////////////////////////////////////////////
// NETTREE.H - Created - 11/20/96
//
// Copyright 1992-1996 Symantec, Peter Norton Product Group
/////////////////////////////////////////////////////////////////////
// $Header:   S:/INCLUDE/VCS/nettree.h_v   1.5   07 Jan 1998 15:50:52   DBuches  $
//
// Description:
//      Network Browser Tree Contol class implementation file.
//    This class is a class derived from a CTreeCtrl.  It will be
//    responsable for enumerating the network and filling the tree
//    control with those enumerated item.  This was made into it's
//    own class so that it would not be tied into the dialog that
//    uses it.  In this fashion, we can drop this control into
//    any other dialog seamlessly without having to worry about the
//    ties to the current one.
//      This class is, however, dependent upon another class.  That
//    class is the CNetResrouceContainer class.  This class is used
//    in all of the branches of this Network Tree Class.  Each branch
//    has one of these classes allocated as the extra data member.
//    this design was neccessary in order to do just-in-time network
//    enumeration only when you expand a particular branch.  The
//    Branch container saves such state information as the current
//    network enumeration position, and the buffer for the Network
//    enumeration API call.
//
//    See Also : NResCont.h for definition of the Netowrk Resoruce
//                   container class.
//
/////////////////////////////////////////////////////////////////////
//
// $Log:   S:/INCLUDE/VCS/nettree.h_v  $
// 
//    Rev 1.5   07 Jan 1998 15:50:52   DBuches
// Added OnChar handler so Spacebar now checks and unchecks items.
//
//    Rev 1.4   07 Jan 1998 15:44:06   DBuches
// Added OnChar handler so Spacebar now checks and unchecks items.
//
//    Rev 1.3   07 Dec 1997 17:04:44   DBuches
// No change.
//
//    Rev 1.0   21 Nov 1997 11:53:04   KSACKIN
// Initial revision.
//
//    Rev 1.1   30 Jul 1997 16:10:12   KSACKIN
// Added support for a Multi-Select network browser.
//
//    Rev 1.0   24 Apr 1997 20:32:50   JBRENNA
// Initial revision.
//
//    Rev 1.0   13 Mar 1997 15:10:44   RFULLER
// Initial revision
//
//    Rev 1.1   05 Dec 1996 02:07:20   ksackin
// Made modifications to the network tree to allow for screening off certain
// types of network resources.
//
//    Rev 1.0   20 Nov 1996 02:27:48   ksackin
// Initial revision.
//
/////////////////////////////////////////////////////////////////////

#if !defined(__NETTREE_H)
#define __NETTREE_H


/////////////////////////////////////////////////////////////////////////////
// CNetworkTreeCtrl window

class AFX_EXT_CLASS CNetworkTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CNetworkTreeCtrl();
	virtual ~CNetworkTreeCtrl();

    InitTreeControl( void );

                                        // Display and Selection Flag
                                        // Access functions.
	void SetValidSelectionFlags( DWORD dwValidSelectionFlags );
	void SetBrowseBeneathFlags( DWORD dwValidSelectionFlags );
	void SetDisplayItemFlags( DWORD dwValidSelectionFlags );

	DWORD GetValidSelectionFlags( void ) { return m_dwValidSelectionFlags;};
	DWORD GetBrowseBeneathFlags( void ) { return m_dwValidSelectionFlags;};
	DWORD GetDisplayItemFlags( void ) { return m_dwValidSelectionFlags;};

	BOOL IsValidSelectionType( DWORD dwType );
	BOOL IsValidBrowseBeneathType( DWORD dwType );
	BOOL IsValidDisplayType( DWORD dwType );

    BOOL GetSelectedItemInformation(
        CString *lpcszSelectedItemName,
        DWORD *lpdwResourceType = NULL
        );

    BOOL GetSelectedItemInformation(
        CObArray *lpcSelectedItemList
        );

    BOOL AddSelectedItems(
        HTREEITEM hBranchToAdd,
        CObArray *lpcSelectedItemArray
        );


	void SetMultiSelectState( BOOL bUseMultiSelect );
    BOOL GetMultiSelectState( void ) { return m_bUseMultiSelect; };

                                        // Destroys a single network branch
                                        // from this network tree control.
    void DestroyNetworkBranch(
        HTREEITEM hBranchToDelete
        );

                                        // Destroys the entire network tree.
    void DestroyNetworkTree( void );

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetworkTreeCtrl)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:

    void CheckItems(HTREEITEM hItem, BOOL bChecked);
    void CheckChildren(HTREEITEM hItem, int iChecked);
    void UpdateBranchSelectionState( HTREEITEM hBranchToUpdate );

    DWORD m_dwValidSelectionFlags;
    DWORD m_dwBrowseBeneathFlags;
    DWORD m_dwDisplayItemFlags;

	//{{AFX_MSG(CNetworkTreeCtrl)
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	CImageList m_xCheckImages;
	BOOL m_bUseMultiSelect;
};


/////////////////////////////////////////////////////////////////////////////
// CNetworkSelection class

class CNetworkSelection : public CObject
{
public:
    CNetworkSelection( LPCTSTR cszResourceName, DWORD dwResourceType );

    LPCTSTR GetResourceName( void ) { return (LPCTSTR)m_cszResourceName; };
    DWORD   GetResourceType( void ) { return m_dwResourceType; };

    void SetResourceName( LPCTSTR cszResourceName );
    void SetResourceType( DWORD   dwResourceType );

private:
    CString m_cszResourceName;
    DWORD   m_dwResourceType;
};

#endif // if !defined(__NETTREE_H)
