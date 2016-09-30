#ifndef SYMCORPUI_NAVIGATIONBAR
#define SYMCORPUI_NAVIGATIONBAR

#include "stdafx.h"
#include "DialogBarEx.h"
#include "ButtonEx.h"
#include "StaticEx.h"

class CNavigationBar : public CDialogBarEx
{
public:
    CNavigationBar();
    ~CNavigationBar();

    virtual void DoDataExchange( CDataExchange* pDX );
	virtual void OnInitDialogBar();
    virtual void OnPreInitDialogBar();
protected:
    DECLARE_MESSAGE_MAP()
private:
    CButtonEx   statusBtn;
    CButtonEx   scanBtn;
    CButtonEx   settingsBtn;
    CButtonEx   quarantineBtn;
    CButtonEx   liveUpdateBtn;
    CButtonEx   logsBtn;

    CBrush      backgroundBrush;
    CImage      backgroundImage;

    afx_msg void OnBtnStatus();
    afx_msg void OnBtnScan();
    afx_msg void OnBtnSettings();
    afx_msg void OnBtnLiveUpdate();
    afx_msg void OnBtnQuarantine();
    afx_msg void OnBtnLogs();
    afx_msg HBRUSH OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode );
    afx_msg void OnCommandUI( CCmdUI* thisCommand );
    afx_msg BOOL OnEraseBkgnd( CDC* paintDC );
};

#endif // SYMCORPUI_NAVIGATIONBAR