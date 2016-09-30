// AppLauncher.h: interface for the CAppLauncher class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APPLAUNCHER_H__FC313D5E_70DC_463F_874E_2DB16E87FAA4__INCLUDED_)
#define AFX_APPLAUNCHER_H__FC313D5E_70DC_463F_874E_2DB16E87FAA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAppLauncher  
{
public:
	CAppLauncher();
	virtual ~CAppLauncher();
    bool LaunchAppAndWait ( LPCTSTR lpcszFileName, LPCTSTR lpcszParameters );
};

#endif // !defined(AFX_APPLAUNCHER_H__FC313D5E_70DC_463F_874E_2DB16E87FAA4__INCLUDED_)
