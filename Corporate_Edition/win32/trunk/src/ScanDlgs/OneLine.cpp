// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// OneLine.cpp: implementation of the COneLine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "scandlgs.h"
#include "OneLine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COneLine::COneLine(char *LogLine,LPARAM Data)
{

	m_LogLine = (char *)malloc(strlen(LogLine)+1);
	if (m_LogLine)
		strcpy(m_LogLine,LogLine);

	m_Data = Data;
}

COneLine::~COneLine()
{

	if (m_LogLine)
		free(m_LogLine);
}
