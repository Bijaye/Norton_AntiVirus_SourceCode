// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// OneLine.h: interface for the COneLine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ONELINE_H__919B98F0_61FD_11D1_AB13_00A0C90F8F6F__INCLUDED_)
#define AFX_ONELINE_H__919B98F0_61FD_11D1_AB13_00A0C90F8F6F__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class COneLine  
{
public:
	char * m_LogLine;
	LPARAM m_Data;
	COneLine(char *LogLine,LPARAM Data=NULL);
	virtual ~COneLine();

};

#endif // !defined(AFX_ONELINE_H__919B98F0_61FD_11D1_AB13_00A0C90F8F6F__INCLUDED_)
