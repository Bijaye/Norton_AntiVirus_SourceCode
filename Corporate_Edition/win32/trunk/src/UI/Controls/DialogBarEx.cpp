// DialogBarEx.cpp : implementation file
//

#include "stdafx.h"
#include "DialogBarEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define WM_INITDIALOGBAR WM_USER + 1

BEGIN_MESSAGE_MAP(CDialogBarEx,  CDialogBar)
	ON_WM_CREATE()
	ON_MESSAGE(WM_INITDIALOGBAR, InitDialogBarHandler)
END_MESSAGE_MAP()


CDialogBarEx::CDialogBarEx()
{
    // Nothing needed
}

CDialogBarEx::~CDialogBarEx()
{
    // Nothing needed
}

/////////////////////////////////////////////////////////////////////////////
// CDialogBarEx message handlers
int CDialogBarEx::OnCreate( LPCREATESTRUCT lpCreateStruct )
{
    OnPreInitDialogBar();
	PostMessage(WM_INITDIALOGBAR , 0 , 0 );
	return 0;
    // WM_INITDIALOGBAR is then the firstish message processed after we return and
    // the window is created
}

BOOL CDialogBarEx::PreCreateWindow(CREATESTRUCT& cs)
{
    BOOL    returnValBOOL       = FALSE;
    
    returnValBOOL = CDialogBar::PreCreateWindow(cs);
    // Disable AFX borders and the default DS_3DLOOK this adds for dialogs
    // Logically this should be in cs.style, but CDialogBar just pays attention to m_dwStyle
//    cs.style    &= ~(CBRS_BORDER_3D | DS_3DLOOK | CBRS_BORDER_TOP);
    m_dwStyle   &= ~(CBRS_BORDER_3D | DS_3DLOOK | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM);

    return returnValBOOL;
}


LRESULT CDialogBarEx::InitDialogBarHandler( WPARAM wParam, LPARAM lParam )
{
    UpdateData(FALSE);
    OnInitDialogBar();
    return 0;
}

void CDialogBarEx::OnInitDialogBar()
// Like CDialog::OnInitDialog, except the base OnInitDialog has already been called
// Derrived class is expected to implement this
{
    // Nothing needed; derrived class logic goes here
}

void CDialogBarEx::OnPreInitDialogBar()
// As above, but called just before the window is created so derrived classes
// can get functional equivalency to old CDialog::OnInitDialog override, where they
// had control of when they called CDialog::OnInitDialog to create the controls.
{
    // Nothing needed; derrived class logic goes here
}