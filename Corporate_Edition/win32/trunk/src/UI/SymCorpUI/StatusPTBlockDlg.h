#pragma once

#include "stdafx.h"
#include "StaticEx.h"
#include "ButtonEx.h"
#define PROTECTIONPROVIDER_HELPERTYPES_WITHNAMESPACE
#import "ProtectionProvider.tlb" raw_interfaces_only exclude("wireHWND", "_RemotableHandle", "__MIDL_IWinTypes_0009")
#include "ProtectionProvider.h"
#include <atlimage.h>

// CStatusPTBlockDlg dialog
class CStatusPTBlockDlg : public CDialog
{
public:
    // ** FUNCTION MEMBERS **
    // Updates display with our current data
    HRESULT RefreshDisplay( void );

    // ** DATA MEMBERS **
    // Pointer to the protection provider.  Lifetime is managed by app and must exceed
    // that of this dialog.
    ProtectionProviderLib::IProtection_Provider*        protectionProvider;

    // MFC Support
    // Constructor-destructor
	CStatusPTBlockDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStatusPTBlockDlg();
    // MFC dialog Data
	enum { IDD = IDD_STATUS_PTBLOCK };
protected:
    // MFC Message handlers
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    afx_msg BOOL OnEraseBkgnd( CDC* paintDC );
    afx_msg HBRUSH OnCtlColor( CDC* drawDC, CWnd* thisWindow, UINT controlCode );
    BOOL PreTranslateMessage( MSG* pMsg );
    afx_msg void OnBtnOptions();

	DECLARE_DYNAMIC(CStatusPTBlockDlg)
	DECLARE_MESSAGE_MAP()
private:
    // ** FUNCTION MEMBERS **
    HRESULT SetProtectionStatusDisplay( ProtectionProviderLib::ProtectionStatus status );

    //** DATA MEMBERS **
    // Miscellaneous
    CImage          backgroundImage;
    CBrush          backgroundBrush;
    HBITMAP         splashBitmapHandle;
    // Controls
    CStaticEx       graphicCtrl;
    CStaticEx       onoffGraphicCtrl;
    CStaticEx       nameCtrl;
    CStaticEx       descriptionCtrl;
    CStaticEx       defsNameCtrl;
    CStaticEx       defsDateRevCtrl;
    CStaticEx       onOffCtrl;
    CButtonEx       optionsCtrl;
    // On-Off related
    DialogFont      onFont;
    DialogFont      offFont;
    DialogFont      errorFont;
    CString         onText;
    CString         offText;
    CString         errorText;
};

typedef std::list<CStatusPTBlockDlg*> CStatusPTBlockDlgPtrList;