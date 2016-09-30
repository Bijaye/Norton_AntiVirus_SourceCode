// ActivityLogReport.h: interface for the CActivityLogReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIVITYLOGREPORT_H__7D9BD92E_3CC1_458C_B8A3_FAC6FA2DCDF4__INCLUDED_)
#define AFX_ACTIVITYLOGREPORT_H__7D9BD92E_3CC1_458C_B8A3_FAC6FA2DCDF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Report.h"

class CActivityLogReport : public CReport 
{
public:
	bool CreateReport (LPCTSTR lpcszFolderPath);
	CActivityLogReport();
	virtual ~CActivityLogReport();

protected:
	TCHAR m_szActivityLog [MAX_PATH];
};

#endif // !defined(AFX_ACTIVITYLOGREPORT_H__7D9BD92E_3CC1_458C_B8A3_FAC6FA2DCDF4__INCLUDED_)
