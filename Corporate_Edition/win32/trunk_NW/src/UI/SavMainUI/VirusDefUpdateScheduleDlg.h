#pragma once

#include "stdafx.h"
#include "LdvpSchedule.h"

// CVirusDefUpdateScheduleDlg dialog
class CVirusDefUpdateScheduleDlg : public CDialog
{
	DECLARE_DYNAMIC(CVirusDefUpdateScheduleDlg)

public:
	CVirusDefUpdateScheduleDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CVirusDefUpdateScheduleDlg();

// Dialog Data
	enum { IDD = IDD_GENERAL_VIRUSDEFUPDATESCHEDULE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual void OnOK();
	DECLARE_MESSAGE_MAP()
private:
    IGenericConfig*     configObject;
    CLDVPSchedule       scheduleCtrl;
};
