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
// Sketchbook.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

#include "ccSymModuleLifetimeMgrHelper.h"
#include "ccSymPathProvider.h"
#include "ccEraserInterface.h"
#include "TestScanner.h"

// CSketchbookApp:
// See Sketchbook.cpp for the implementation of this class
//

class CSketchbookApp : public CWinApp
{
public:
	CSketchbookApp();

   // Starts the eraser engine
	BOOL StartEraser();

   // Receives a list of anomalies and handle to log file.
   // Retrieves detection and remediation action lists and remediates each remediation action obtained.
   // This function does not close the handle to the output log.
	BOOL RemediateAnomalyList(ccEraser::IAnomalyListPtr &pAnomalyList, CStdioFile &output);

	/*
	DeleteWildCardBack()
	Usage: deleteWildCardBack (_T("C:\\test"), _T("abc"), true);
	dirPath			: Path to the directory where the search is to begin
	searchFile		: File name to be searched, including substring
	subDirSearch	: true or false, whether to recursively search through sub-directories
	*/
	void DeleteWildCardBack (CString dirPath, CString searchFile, BOOL subDirSearch);

	CTestScanner m_Scanner;

	/* Initialize the ccEraser Engine Loader */
	CSymInterfaceManagedDLLHelper<&cc::sz_ccEraser_dll,
                           ccSym::CModulePathProvider, 
		cc::CSymInterfaceTrustedMgdLoader,
							      ccEraser::IEraser, 
							      &ccEraser::IID_Eraser,
                           &ccEraser::IID_Eraser> m_ccEraserLoader;

	IScannerPtr m_pScanner;
	IScanSinkPtr m_pSink;
	IScanPropertiesPtr m_pProperties;
	ccEraser::IEraserPtr m_pEraser;

	BOOL m_bATReturn;

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CSketchbookApp theApp;