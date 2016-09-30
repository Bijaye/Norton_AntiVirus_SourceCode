////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// LiveUpdateLogs.h: interface for the CLiveUpdateLogs class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIVEUPDATELOGS_H__C742A3B9_ED96_4EA4_99DA_01337FC16A0C__INCLUDED_)
#define AFX_LIVEUPDATELOGS_H__C742A3B9_ED96_4EA4_99DA_01337FC16A0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Report.h"

class CLiveUpdateLogs : public CReport  
{
public:
	CLiveUpdateLogs();
	virtual ~CLiveUpdateLogs();
    bool CreateReport ( LPCTSTR lpcszFolderPath );

protected:
    TCHAR m_szLiveUpdateDir [MAX_PATH];
};

#endif // !defined(AFX_LIVEUPDATELOGS_H__C742A3B9_ED96_4EA4_99DA_01337FC16A0C__INCLUDED_)
