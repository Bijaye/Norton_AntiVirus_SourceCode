#pragma once

#include "stdafx.h"
#include "SymCorpUIDoc.h"


// An extended view class that supports the following customizations:
// 1.  Custom background color
// 2.  Removal of styles/extended styles during PreCreateWindow to eliminate Afx borders
// 3.  Stores default dialog size in defaultSize
// 4.  Sets initialUpdateRan flag once InitialUpdate has run
// 5.  GetDocument defined for CSymCorpUIDoc
// 6.  Dummy AssertValid/Dumps defined
class CViewEx : public CFormView
{
public:
    CSymCorpUIDoc* GetDocument() const;

    // MFC
protected:
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    afx_msg HBRUSH OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode );
    afx_msg int OnCreate( LPCREATESTRUCT createInfo );
    virtual void DoDataExchange( CDataExchange* pDX );
    DECLARE_MESSAGE_MAP()

    // Constructor-destructor
    // Solid color background
    CViewEx( UINT dialogID, DWORD newStylesToRemove, DWORD newExtendedStylesToRemove, COLORREF backgroundColor );
    // No background brush
    CViewEx( UINT dialogID, DWORD newStylesToRemove, DWORD newExtendedStylesToRemove );
    // Custom background color, default style removes
    CViewEx( UINT dialogID, COLORREF backgroundColor );
    // No background brush, default style removals
    CViewEx( UINT dialogID );
    virtual ~CViewEx();
    // Debug support
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    bool                        initialUpdateRan;
    CRect                       defaultSize;
    CBrush                      backgroundBrush;
private:
    DWORD                       stylesToRemove;
    DWORD                       extendedStylesToRemove;
};