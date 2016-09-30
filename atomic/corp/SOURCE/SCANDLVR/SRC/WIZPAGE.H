/////////////////////////////////////////////////////////////////////////////
// $Header:   S:/SCANDLVR/VCS/WIZPAGE.H_v   1.6   26 May 1998 18:28:20   SEDWARD  $
/////////////////////////////////////////////////////////////////////////////
//
// WizPage.h: interface for the CWizardPropertyPage class.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/SCANDLVR/VCS/WIZPAGE.H_v  $
// 
//    Rev 1.6   26 May 1998 18:28:20   SEDWARD
// Added 'm_' to the 'bitmap' member variables.
//
//    Rev 1.5   26 May 1998 18:18:22   SEDWARD
// Added support for swapping 16/256 color bitmaps at runtime.
//
//    Rev 1.4   25 May 1998 13:52:28   SEDWARD
// Added m_staticBitmap, DisplayableColors(), OnInitDialog() and
// WizMessageBox().
//
//    Rev 1.3   26 Apr 1998 17:25:44   SEDWARD
// Added GetTemplateID().
//
//    Rev 1.2   24 Apr 1998 18:23:24   SEDWARD
// Added logfile headers.
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WIZPAGE_H__7522D800_C674_11D1_A7C8_0000E8D34392__INCLUDED_)
#define AFX_WIZPAGE_H__7522D800_C674_11D1_A7C8_0000E8D34392__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "ScanDeliverDLL.h"


/////////////////////////////////////////////////////////////////////////////
// CWizardPropertyPage dialog

class CWizardPropertyPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CWizardPropertyPage)

// Construction
public:
    CWizardPropertyPage();
    CWizardPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0);
    CWizardPropertyPage(CScanDeliverDLL*  pDLL, UINT nIDTemplate, UINT nIDCaption = 0);
    ~CWizardPropertyPage();

// Dialog Data
    //{{AFX_DATA(CWizardPropertyPage)
	enum { IDD = IDD_WIZDLG };
	//}}AFX_DATA

    HBITMAP     m_hBitmap;
    UINT        m_bitmap16;
    UINT        m_bitmap256;

    virtual UINT  GetTemplateID(void) { return  IDD; }
    int WizMessageBox(UINT  nMessageID, UINT  nCaptionID = IDS_CAPTION_STANDARD_MESGBOX
                                                        , UINT  nFlags = MB_ICONWARNING);

// Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CWizardPropertyPage)
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
    virtual BOOL UpdateConfigData(void);
    // Generated message map functions
    //{{AFX_MSG(CWizardPropertyPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    CScanDeliverDLL*    m_pScanDeliverDLL;

    DWORD   DisplayableColors(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZPAGE_H__7522D800_C674_11D1_A7C8_0000E8D34392__INCLUDED_)