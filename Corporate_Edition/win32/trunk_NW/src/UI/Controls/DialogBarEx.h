#if !defined(SYMCORPUI_DIALOGBAREX)
#define SYMCORPUI_DIALOGBAREX

#if _MSC_VER >= 1000
#pragma once
#endif

// An Improved CDialogBar that allows us to associated variables and subclass controls
class CDialogBarEx : public CDialogBar 
{
// Construction
public:
	CDialogBarEx();
    virtual ~CDialogBarEx();

// Implementation
public:
    // Like CDialog::OnInitDialog, except the base OnInitDialog has already been called
    // PROBLEM WARNING!!
    // For some reason, owner draw controls in a toolbar (but not a dialog!) don't call DrawItem
    // when initially created/drawn, but they draw normally otherwise.
    // No idea why.  Debug shows that parent window OnDrawItem correctly
    // tries to reflect the message to the control, but can't find it in
    // the map (???), and so the messages withers up and dies.  This is fixed
    // later...somehow.  To workaround, invalidate the controls in OnInitDialogBar
	virtual void OnInitDialogBar();
    // As above, but called just before the window is created so derrived classes
    // can get functional equivalency to old CDialog::OnInitDialog override, where they
    // had control of when they called CDialog::OnInitDialog to create the controls.
	virtual void OnPreInitDialogBar();

protected:
	afx_msg LRESULT InitDialogBarHandler( WPARAM wParam, LPARAM lParam );
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif // SYMCORPUI_DIALOGBAREX
