#include "stdafx.h"
#include "ViewEx.h"

BEGIN_MESSAGE_MAP(CViewEx, CFormView)
    // Other
    ON_WM_CREATE()
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()


// An extended view class that supports several MFC customizations.
BOOL CViewEx::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style &= ~stylesToRemove;
    cs.dwExStyle &= ~extendedStylesToRemove;
    return CFormView::PreCreateWindow(cs);
}

int CViewEx::OnCreate( LPCREATESTRUCT createInfo )
{
    // Get the default size of the dialog for later layout calculations.  Can't seem
    // to override InitDialog and by the time InitialUpdate is called, a layout has been
    // performed, destroying this information
    GetClientRect(&defaultSize);
    return CFormView::OnCreate(createInfo);
}

void CViewEx::OnInitialUpdate()
{
    initialUpdateRan = true;
    CFormView::OnInitialUpdate();
}

HBRUSH CViewEx::OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode )
{
    // Specify dialog background color to use
    switch (controlCode)
    {
    case CTLCOLOR_BTN:
    case CTLCOLOR_STATIC:
        drawDC->SetBkMode(TRANSPARENT);
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());
    case CTLCOLOR_DLG:
        return static_cast<HBRUSH>(backgroundBrush.GetSafeHandle());
    }

    return CFormView::OnCtlColor(drawDC, thisWindow, controlCode);
}

void CViewEx::DoDataExchange( CDataExchange* pDX )
{
    CFormView::DoDataExchange(pDX);
}



// Constructor-destructor
CViewEx::CViewEx( UINT dialogID, DWORD newStylesToRemove, DWORD newExtendedStylesToRemove, COLORREF backgroundColor ) : CFormView(dialogID), initialUpdateRan(false), stylesToRemove(newStylesToRemove), extendedStylesToRemove(newExtendedStylesToRemove)
{
    backgroundBrush.CreateSolidBrush(backgroundColor);
}

CViewEx::CViewEx( UINT dialogID, DWORD newStylesToRemove, DWORD newExtendedStylesToRemove ) : CFormView(dialogID), initialUpdateRan(false), stylesToRemove(newStylesToRemove), extendedStylesToRemove(newExtendedStylesToRemove)
{
    // No code needed
}

CViewEx::CViewEx( UINT dialogID, COLORREF backgroundColor ) : CFormView(dialogID), initialUpdateRan(false), stylesToRemove(WS_BORDER | DS_3DLOOK), extendedStylesToRemove(WS_EX_CLIENTEDGE)
{
    backgroundBrush.CreateSolidBrush(backgroundColor);
}

CViewEx::CViewEx( UINT dialogID ) : CFormView(dialogID), initialUpdateRan(false), stylesToRemove(WS_BORDER | DS_3DLOOK), extendedStylesToRemove(WS_EX_CLIENTEDGE)
{
    // No code needed
}

CViewEx::~CViewEx()
{
    // Cleanup
    if (static_cast<HBRUSH>(backgroundBrush) != NULL)
        backgroundBrush.DeleteObject();
}

// Debug support
#ifdef _DEBUG
    void CViewEx::AssertValid() const
    {
        CFormView::AssertValid();
    }
    
    void CViewEx::Dump(CDumpContext& dc) const
    {
        CFormView::Dump(dc);
    }
#endif

#ifndef _DEBUG
    CSymCorpUIDoc* CViewEx::GetDocument() const
    {
        return reinterpret_cast<CSymCorpUIDoc*>(m_pDocument);
    }
#else
    CSymCorpUIDoc* CViewEx::GetDocument() const
    {
        ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSymCorpUIDoc)));
        return (CSymCorpUIDoc*)m_pDocument;
    }
#endif