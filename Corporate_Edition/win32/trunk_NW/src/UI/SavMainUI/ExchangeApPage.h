#pragma once

#include "stdafx.h"
#include "LdvpSnapinCtrl.h"

// ExchangeApPage dialog
class CExchangeApPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CExchangeApPage)

public:
	CExchangeApPage();
	virtual ~CExchangeApPage();

// Dialog Data
	enum { IDD = IDD_CONFIGPAGE_EXCHANGEAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
    CLdvpSnapinCtrl ldvpCtrl;
};
