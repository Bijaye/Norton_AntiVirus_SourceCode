// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header start
// //////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
//
// /////////////////////////////////////////////////////////////////////////////////////////////////
// Symantec copyright header stop
// //////////////////////
#include "StdAfx.h"
#include ".\scanitem.h"

CScanItem::CScanItem(void)
{
	m_strPath = "c:\\";
}

CScanItem::~CScanItem(void)
{
}

int CScanItem::GetScanItemCount() throw()
{
    return 1;
}

LPCSTR CScanItem::GetScanItemPath(int iIndex) throw()
{
	return m_strPath;
}

void CScanItem::SetPath(CString strPath)
{
	m_strPath = CT2A(strPath);
}