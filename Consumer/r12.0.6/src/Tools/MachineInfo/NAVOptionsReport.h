// NAVOptionsReport.h: interface for the CNAVOptionsReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVOPTIONSREPORT_H__36414F58_2018_4CD0_913C_85C9A7B10877__INCLUDED_)
#define AFX_NAVOPTIONSREPORT_H__36414F58_2018_4CD0_913C_85C9A7B10877__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Report.h"

class CNAVOptionsReport : public CReport  
{
public:
	CNAVOptionsReport();
	virtual ~CNAVOptionsReport();
    bool CreateReport ( LPCTSTR lpcszFolderPath );
};

#endif // !defined(AFX_NAVOPTIONSREPORT_H__36414F58_2018_4CD0_913C_85C9A7B10877__INCLUDED_)
