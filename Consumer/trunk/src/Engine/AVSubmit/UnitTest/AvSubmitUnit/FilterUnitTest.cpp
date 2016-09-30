////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include ".\filterunittest.h"
#include "resource.h"
#include "AVSubmissionInterface.h"
#include "AVSubmissionLoader.h"
#include "ccSymModuleLifetimeMgrHelper.h"
#include "md5.h"

namespace AVSubmit {
		//managed loader
	typedef CMgdInterfaceHelperStatic<AVSubmit_dll,
									ccSym::CNAVPathProvider,
									cc::CSymInterfaceTrustedCacheMgdLoader,
									IAVSubmission,
									&CLSID_AVSampleSubmission,
									&IID_AVSubmission> AVSubmit_IAVSubmissionSampleMgd;
}

CFilterUnitTest::CFilterUnitTest(void) : ccUnit::CTestCaseImpl(_T("FilterUnitTest"), _T("Content Filtering Unit Test Cases"))
{
}

CFilterUnitTest::~CFilterUnitTest(void)
{
}

bool CFilterUnitTest::Execute()
{
	ABORT_ON_FAIL(TestWordMacro());
	ABORT_ON_FAIL(TestExcelMacro());
	ABORT_ON_FAIL(TestDLL());
	return true;
}

bool CFilterUnitTest::TestWordMacro()
{
	ccLib::CString sPath, sOutPath;
	ABORT_ON_FAIL(BinResToFile(_T("word_doc_input.doc"), sPath, IDR_WORDDOC));
	ABORT_ON_FAIL(DoFilter(sPath, sOutPath));
	return true;
}

bool CFilterUnitTest::TestExcelMacro()
{
#pragma message("TODO: Test Excel Macros")
#if 0
	ccLib::CString sPath, sOutPath;
	ABORT_ON_FAIL(BinResToFile(_T("excel_input.xls"), sPath, IDR_SPREADSHEET));
	ABORT_ON_FAIL(DoFilter(sPath, sOutPath));
#endif
	return true;
}

bool CFilterUnitTest::TestDLL()
{
	//content should not be stripped
	ccLib::CString sPath, sOutPath;
	ABORT_ON_FAIL(BinResToFile(_T("test_dll.dll"), sPath, IDR_TESTDLL));
	ABORT_ON_FAIL(DoFilter(sPath, sOutPath));

	crypto::CMD5 md5In, md5Out;
	CCUNIT_TEST_CONDITION_RETURN(FileMD5(sPath, md5In));
	CCUNIT_TEST_CONDITION_RETURN(FileMD5(sOutPath, md5Out));
	CCUNIT_TEST_CONDITION_RETURN(wcscmp(md5In.GetString(), md5Out.GetString()) == 0);
	return true;
}

bool CFilterUnitTest::FileMD5(const ccLib::CString& sPath, crypto::CMD5& md5)
{
	ccLib::CMemory mem;
	ccLib::CFile file;
	ULONGLONG ullLength = 0;
	CCUNIT_TEST_CONDITION_RETURN(file.Open(sPath, GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL) != FALSE);
	CCUNIT_TEST_CONDITION_RETURN(file.GetLength(ullLength) != FALSE);
	CCUNIT_TEST_CONDITION_RETURN(mem.NewAlloc(SIZE_T(ullLength)) != NULL);
	CCUNIT_TEST_CONDITION_RETURN(file.Read(mem, DWORD(ullLength)) != FALSE);
	md5.HashMem((BYTE*)mem.GetMemory(), UINT(ullLength));
	return true;
}

bool CFilterUnitTest::DoFilter(const ccLib::CString& sPath, ccLib::CString& sOutPath)
{
	cc::IStringPtr pInFile;
	cc::IStringQIPtr pOutFile;
	ISymBasePtr pBase;
	AVSubmit::IAVSubmissionPtr pAvSubmission;
	AVSubmit::IContentFilterQIPtr pContentFilter;
	pInFile.Attach(ccSym::CStringImpl::CreateStringImpl(sPath));
	CCUNIT_TEST_CONDITION_RETURN(pInFile!=NULL);
	CCUNIT_TEST_CONDITION_RETURN(AVSubmit::AVSubmit_IAVSubmissionSampleMgd::CreateObject(GETMODULEMGR(), pAvSubmission.m_p) == SYM_OK);
	pContentFilter = pAvSubmission;
	CCUNIT_TEST_CONDITION_RETURN(pContentFilter != NULL);
	CCUNIT_TEST_CONDITION_RETURN(pContentFilter->FilterContent(pInFile, pBase.m_p) == S_OK);
	pOutFile = pBase;
	CCUNIT_TEST_CONDITION_RETURN(pOutFile != NULL);

	ccLib::CString sMessage;
	ccLib::CStringConvert::Format(sMessage, _T("*****************\r\n\r\nTest succeeded!\r\nPlease verify manually the contents of: \r\n%s\r\nAND\r\n%s\r\n\r\n***************\r\n"), sPath, pOutFile->GetStringW());
	//::MessageBox(0, sMessage, _T(__FUNCTION__), 0);
	::_tprintf(sMessage);

#ifdef UNICODE
	ccLib::CStringConvert::Copy(pOutFile->GetStringW(), sOutPath);
#else
	ccLib::CStringConvert::Copy(pOutFile->GetStringA(), sOutPath);
#endif
	return true;
}

bool CFilterUnitTest::BinResToFile(LPCTSTR pszInFile, ccLib::CString& sInFile, int nRes)
{
	HMODULE hModule = (HMODULE)&__ImageBase;
	HRSRC hResource = FindResource(hModule, MAKEINTRESOURCE(nRes), _T("BINARY"));
	ABORT_ON_FAIL(hResource != NULL);
	HGLOBAL hGlobal = LoadResource(hModule, hResource);
	ABORT_ON_FAIL(hGlobal != NULL);
	DWORD dwResSize = SizeofResource(hModule, hResource);
	LPVOID pResData = LockResource(hGlobal);
	ABORT_ON_FAIL(pResData != NULL);

	TCHAR szPath[MAX_PATH];
	size_t nSize = MAX_PATH;
	ccLib::CFile fInput;

	ABORT_ON_FAIL(ccSym::CModulePathProvider::GetPath(szPath, nSize));
	ABORT_ON_FAIL(ccLib::CStringConvert::FormatPath(sInFile, szPath, _T("%s"), pszInFile));
	ABORT_ON_FAIL(fInput.Open(sInFile, GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL) != FALSE);
	ABORT_ON_FAIL(fInput.Write(pResData, dwResSize) != FALSE);
	return true;
}