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
#pragma once



// CSketchbookCLI command target

class CSketchbookCLI : public CCommandLineInfo
{
public:
	CSketchbookCLI();
	virtual ~CSketchbookCLI();
	virtual void ParseParam(const TCHAR* pszParam, BOOL bFlag, BOOL bLast);

	BOOL m_bBA;
	BOOL m_bInput;
	BOOL m_bOutput;
	BOOL m_bGLP;
	BOOL m_bUndo;
   BOOL m_bFullScan;
	BOOL m_bDecompose;


	CString m_strInput;
	CString m_strOutput;
   CString m_strFullScanPath;
};


