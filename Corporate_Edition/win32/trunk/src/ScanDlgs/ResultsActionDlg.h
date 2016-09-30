#pragma once

#include "Resource.h"

// CResultsActionDlg dialog

class CResultsActionDlg : public CDialog
{
	DECLARE_DYNAMIC(CResultsActionDlg)

public:
	enum EAction
	{
		evInvalidAction = -1,
		evFirstValidAction = 0,
		evRemoveRisks = evFirstValidAction,
		evReboot,
		evRemoveRisksAndReboot,
		evAfterLastRealAction,
		evNoAction = evAfterLastRealAction, // This should be after any real actions
		evAfterLastValidAction
		
	};

	CResultsActionDlg(EAction eAction, CWnd* pParent = NULL);
	virtual ~CResultsActionDlg();

	EAction GetChosenAction() const;

// Dialog Data
	enum { IDD = IDD_RESULTS_ACTION_REQUIRED };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

private:
	const EAction m_eActionRequired;
	EAction		  m_eActionChosen;

	EAction GetActionForRadioButton(UINT idcButton) const;
protected:
	virtual void OnOK();
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedHelp();
};
