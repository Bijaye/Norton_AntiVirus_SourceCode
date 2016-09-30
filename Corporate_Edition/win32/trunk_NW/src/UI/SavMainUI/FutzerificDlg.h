#pragma once
#include "resource.h"
#include "LDVPSnapInCtrl.h"

// CFutzerificDlg dialog

class CFutzerificDlg : public CDialog
{
	DECLARE_DYNAMIC(CFutzerificDlg)

public:
	CFutzerificDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFutzerificDlg();

// Dialog Data
	enum { IDD = IDD_FUTZERIFIC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
private:
    CLdvpSnapinCtrl     uiCtrl;
};
