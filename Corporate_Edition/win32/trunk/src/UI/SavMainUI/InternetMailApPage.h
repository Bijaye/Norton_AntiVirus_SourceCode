#pragma once

#include "stdafx.h"
#include "LdvpSnapinCtrl.h"

// CInternetMailApPage dialog
class CInternetMailApPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CInternetMailApPage)

public:
	CInternetMailApPage();
	virtual ~CInternetMailApPage();

// Dialog Data
	enum { IDD = IDD_CONFIGPAGE_IMAILAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
    CLdvpSnapinCtrl ldvpCtrl;
};
