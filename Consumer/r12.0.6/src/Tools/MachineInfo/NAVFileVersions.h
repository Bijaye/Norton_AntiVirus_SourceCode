// NAVFileVersions.h: interface for the CNAVFileVersions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NAVFILEVERSIONS_H__BB46D4AB_2569_433E_A3F9_9EFC45643298__INCLUDED_)
#define AFX_NAVFILEVERSIONS_H__BB46D4AB_2569_433E_A3F9_9EFC45643298__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Report.h"

class CNAVFileVersions : public CReport  
{
public:
	CNAVFileVersions();
	virtual ~CNAVFileVersions();
    bool CreateReport ( LPCTSTR lpcszFolderPath );
};

#endif // !defined(AFX_NAVFILEVERSIONS_H__BB46D4AB_2569_433E_A3F9_9EFC45643298__INCLUDED_)
