// QuarantinedDlg.h: interface for the CQuarantinedDlg class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUARANTINEDDLG_H__D27EEE1D_CD12_41DA_9FB0_B07AD908C698__INCLUDED_)
#define AFX_QUARANTINEDDLG_H__D27EEE1D_CD12_41DA_9FB0_B07AD908C698__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseAlertUI.h"

class CQuarantinedDlg : public CBaseAlertUI
{
public:
	CQuarantinedDlg();
	virtual ~CQuarantinedDlg();
	
    int DoModal ();

protected:
	// Enable Quarantine.
	bool m_bQuarEnabled;
};

#endif // !defined(AFX_QUARANTINEDDLG_H__D27EEE1D_CD12_41DA_9FB0_B07AD908C698__INCLUDED_)
