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
// SketchbookCLI.cpp : implementation file
//

#include "stdafx.h"
#include "Sketchbook.h"
#include "SketchbookCLI.h"


// CSketchbookCLI

CSketchbookCLI::CSketchbookCLI()
{
	m_bBA = FALSE;
	m_bInput = FALSE;
	m_bOutput = FALSE;
	m_bGLP = FALSE;
	m_bUndo = FALSE;
   m_bFullScan = FALSE;
	m_bDecompose = FALSE;

	m_strInput = _T("");
	m_strOutput = _T("ccEraser.txt");
}

CSketchbookCLI::~CSketchbookCLI()
{
}


// CSketchbookCLI member functions
void CSketchbookCLI::ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast)
{
	if(bFlag)
	{
		if(_tcsicmp(pszParam, _T("ba")) == 0)
		{
			m_bBA = TRUE;
		}
		else if(_tcsicmp(pszParam, _T("i")) == 0)
		{
			m_bInput = TRUE;
		}
		else if(_tcsicmp(pszParam, _T("o")) == 0)
		{
			m_bOutput = TRUE;
		}
		if(_tcsicmp(pszParam, _T("per")) == 0)
		{
			m_bGLP = TRUE;
		}
		if(_tcsicmp(pszParam, _T("undo")) == 0)
		{
			m_bUndo = TRUE;
		}
		if(_tcsicmp(pszParam, _T("full")) == 0)
		{
			m_bFullScan = TRUE;
		}
		if(_tcsicmp(pszParam, _T("decompose")) == 0)
      {
			m_bDecompose = TRUE;
      }
	}
	else
	{
		if(m_bInput)
		{
			m_strInput = pszParam;
		}
		else if(m_bOutput)
		{
			m_strOutput = pszParam;
		}
      else if(m_bFullScan)
      {
         m_strFullScanPath = pszParam;
      }
		m_bInput = FALSE;
		m_bOutput = FALSE;
	}

	CCommandLineInfo::ParseParam(pszParam, bFlag, bLast);
}