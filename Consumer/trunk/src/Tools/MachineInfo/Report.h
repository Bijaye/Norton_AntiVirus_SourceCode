////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

// Report.h: interface for the CReport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REPORT_H__42D3E4E3_FA15_4491_89A3_CE9BAB619064__INCLUDED_)
#define AFX_REPORT_H__42D3E4E3_FA15_4491_89A3_CE9BAB619064__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CReport  
{
public:
	virtual bool CreateReport ( LPCTSTR lpcszFolderPath ) = 0;
	virtual bool IsAvailable();
	CReport();
	virtual ~CReport();

protected:
	bool m_bIsAvailable;
};

#endif // !defined(AFX_REPORT_H__42D3E4E3_FA15_4491_89A3_CE9BAB619064__INCLUDED_)
