#pragma once

#include "stdafx.h"
#include "LdvpSnapinCtrl.h"

// CNotesApPage dialog
class CNotesApPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CNotesApPage)

public:
	CNotesApPage();
	virtual ~CNotesApPage();

// Dialog Data
	enum { IDD = IDD_CONFIGPAGE_NOTESAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
    CLdvpSnapinCtrl ldvpCtrl;
};
