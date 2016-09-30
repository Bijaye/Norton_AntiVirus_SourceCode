// Copyright 1998 Symantec Corporation. 
//*************************************************************************
// TREECTRL.H - created 04/08/1998 1:14:32 PM
//
// $Header:   S:/NAVW/VCS/treectrl.h_v   1.0   20 Apr 1998 23:12:46   DALLEE  $
//
// Description:
//      Header for tree control.
//
// Contains:
//      Declaration of CNVTreeCtrl - non-MFC generic tree control.
//*************************************************************************
// $Log:   S:/NAVW/VCS/treectrl.h_v  $
// 
//    Rev 1.0   20 Apr 1998 23:12:46   DALLEE
// Initial revision.
//*************************************************************************

#ifndef __TREECTRL_H
#define __TREECTRL_H

#ifndef _INC_COMMCTRL
#include <commctrl.h>
#endif


//
// CNVTreeCtrl - Generic Non-MFC tree control
//

class CNVTreeCtrl
{
// Construction
public:
    CNVTreeCtrl();
    BOOL Create( HINSTANCE hInst, DWORD dwStyle, LPCRECT pRect, HWND hParent, UINT uID );

// Attributes
public:
    HWND    m_hWnd;
    operator HWND() const;

    HIMAGELIST GetImageList( int nType ) const;
    HIMAGELIST SetImageList( HIMAGELIST hImageList, int nType );
    HIMAGELIST GetNormalImageList() const;
    HIMAGELIST SetNormalImageList( HIMAGELIST hImageList );
    HIMAGELIST GetStateImageList() const;
    HIMAGELIST SetStateImageList( HIMAGELIST hImageList );
    BOOL GetItem( LPTV_ITEM pItem ) const;
    BOOL SetItem( const TV_ITEM *pItem );
    LPARAM GetItemData( HTREEITEM hItem ) const;
    BOOL SetItemData( HTREEITEM hItem, LPARAM lData );
    UINT GetItemState( HTREEITEM hItem, UINT uMask ) const;
    BOOL SetItemState( HTREEITEM hItem, UINT uState, UINT uMask );
    HTREEITEM GetNextItem( HTREEITEM hItem, int nRelation ) const;
    HTREEITEM GetChildItem( HTREEITEM hItem ) const;
    HTREEITEM GetNextSiblingItem( HTREEITEM hItem ) const;
    HTREEITEM GetParentItem( HTREEITEM hItem ) const;
    HTREEITEM GetPrevSiblingItem( HTREEITEM hItem ) const;
    HTREEITEM GetRootItem() const;
    HTREEITEM GetSelectedItem() const;
    BOOL GetItemRect( HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly ) const;
    HTREEITEM HitTest( LPTV_HITTESTINFO pHitTestInfo ) const;

// Operations
public:
    BOOL Attach( HWND hWnd );
    HWND Detach();

    HTREEITEM InsertItem( const TV_INSERTSTRUCT *pInsert );
    BOOL DeleteAllItems();
    BOOL DeleteItem( HTREEITEM hItem );
    BOOL Expand( HTREEITEM hItem, int nAction );
    BOOL SortChildren( HTREEITEM hItem );
    BOOL SortChildrenCB( LPTV_SORTCB pSortInfo );

    virtual BOOL DestroyWindow();

// Implementation
public:
    virtual ~CNVTreeCtrl();

protected:
    LONG m_lOldUserData;
};


#endif // !__TREECTRL_H
