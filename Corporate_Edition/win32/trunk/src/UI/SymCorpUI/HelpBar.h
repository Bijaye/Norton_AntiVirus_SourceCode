#ifndef SYMCORPUI_HELPBAR
#define SYMCORPUI_HELPBAR

#include "stdafx.h"
#include "DialogBarEx.h"
#include "ButtonEx.h"
#include "StaticEx.h"
#include "ColorFontControl.h"

class CHelpBar : public CDialogBarEx
{
public:
    virtual void DoDataExchange( CDataExchange* pDX );
    virtual void OnInitDialogBar();
    virtual void OnPreInitDialogBar();
    // Sets the background bitmap to the specified image (IDR resource)
    void SetBackgroundImage( UINT backgroundID );

    // Message map functions
    afx_msg BOOL OnEraseBkgnd( CDC* paintDC );
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode );
    // Command handlers
    afx_msg void OnBtnGo();
    afx_msg void OnBtnHelpAndSupport();
    afx_msg void OnQuestion();
    afx_msg void OnCommandUI( CCmdUI* thisCommand );

    // Constructor-destructor
    CHelpBar();
    ~CHelpBar();
protected:
    DECLARE_MESSAGE_MAP()
private:
    CButtonEx       goBtn;
    CButtonEx       helpAndSupportBtn;
    CEditColorFont  helpQuestionCtrl;

    CBrush      backgroundBrush;
    CImage      backgroundBitmap;
};

#endif // SYMCORPUI_HELPBAR