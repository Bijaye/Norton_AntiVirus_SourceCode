#pragma once

#include "WizPage.h"

/////////////////////////////////////////////////////////////////////////////
// CSendAddlInfoPropertyPage dialog

class CSendAddlInfoPropertyPage : public CWizardPropertyPage
{
    DECLARE_DYNCREATE(CSendAddlInfoPropertyPage)

    // Construction
public:
    CSendAddlInfoPropertyPage();
    ~CSendAddlInfoPropertyPage();

    // Dialog Data
    //{{AFX_DATA(CSendAddlInfoPropertyPage)
    enum { IDD = IDD_SEND_ADDL_INFO };
    //}}AFX_DATA

    virtual UINT  GetTemplateID(void) { return  IDD; }


    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CSendAddlInfoPropertyPage)
public:
    virtual BOOL OnSetActive();
    virtual LRESULT OnWizardBack();
    virtual LRESULT OnWizardNext();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CSendAddlInfoPropertyPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnChkSendAddlInfoClicked();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

    virtual BOOL UpdateConfigData(void);
    
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
