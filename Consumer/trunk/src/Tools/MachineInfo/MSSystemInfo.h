////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// MSSystemInfo.h: interface for the MSSystemInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MSSYSTEMINFO_H__10B0B94D_D1D2_46FE_8F94_6C02E23AFD0C__INCLUDED_)
#define AFX_MSSYSTEMINFO_H__10B0B94D_D1D2_46FE_8F94_6C02E23AFD0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Report.h"

class CMSSystemInfo : public CReport
{
public:
	bool CreateReport ( LPCTSTR lpcszFolderPath );
	CMSSystemInfo();
	virtual ~CMSSystemInfo();
protected:
	TCHAR m_szInfoAppName [MAX_PATH];
};

#endif // !defined(AFX_MSSYSTEMINFO_H__10B0B94D_D1D2_46FE_8F94_6C02E23AFD0C__INCLUDED_)
