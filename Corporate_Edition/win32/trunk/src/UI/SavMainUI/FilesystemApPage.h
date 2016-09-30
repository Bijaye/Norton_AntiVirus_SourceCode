#pragma once

#include "stdafx.h"
#include "LdvpSnapinCtrl.h"

// CFilesystemApPage dialog
class CFilesystemApPage : public CPropertyPage
{
	DECLARE_DYNAMIC(CFilesystemApPage)

public:
	CFilesystemApPage();
	virtual ~CFilesystemApPage();

// Dialog Data
	enum { IDD = IDD_CONFIGPAGE_FILESYSTEMAP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()

private:
    CLdvpSnapinCtrl ldvpCtrl;
};
