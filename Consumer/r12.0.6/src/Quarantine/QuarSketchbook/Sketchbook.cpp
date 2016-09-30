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
// Sketchbook.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#define INITIIDS

#include "callback.h"
#include "ccCloneInterface.h"
#include "ccEraserInterface.h"
#include "ccIgnoreWarnings.h"
#include "ccKeyValueCollectionInterface.h"
#include "ccSymKeyValueCollectionImpl.h"
#include "ccModuleNames.cpp"
#include "ccosinfo.h"
#include "ccSerializeInterface.h"
#include "ccSymCrashHandler.h"
#include "ccSymDebugOutput.h"
#include "ccSymDebugOptions.h"
#include "ccSymFileStreamImpl.h"
#include "ccSymStringConvert.h"
#include "ccSymStringImpl.h"
#include "ccSymMemoryImpl.h"
#include "scanitem.h"
#include "Sketchbook.h"
#include "SketchbookCLI.h"
#include "SessionDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// TRACE implementation
ccSym::CDebugOutput g_DebugOutput(_T("QuarSketchbook"));
IMPLEMENT_CCTRACE(g_DebugOutput);
ccSym::CCrashHandler g_CrashHandler;

cc::IModuleLifetimeMgrPtr g_ModuleManager;
IMPLEMENT_MODULEMANAGER(::g_ModuleManager);


// CSketchbookApp

BEGIN_MESSAGE_MAP(CSketchbookApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

SYM_OBJECT_MAP_BEGIN()
SYM_OBJECT_MAP_END() 

// CSketchbookApp construction

CSketchbookApp::CSketchbookApp()
{
	m_pEraser = NULL;
	m_bATReturn = TRUE;
}

// The one and only CSketchbookApp object

CSketchbookApp theApp;

// CSketchbookApp initialization

BOOL CSketchbookApp::StartEraser()
	{
	if(SYM_FAILED(m_ccEraserLoader.CreateObject(GETMODULEMGR(), m_pEraser)))
    {
		::AfxMessageBox(_T("FAILED to Create EraserLoader Object"));
		CCTRACEE(_T("StartEraser():FAILED to Create EraserLoader Object"));
		return FALSE;
	}

	//Initialize the ccEraser engine
	m_pScanner = NULL;
	m_pSink = NULL;
	m_pProperties = NULL;
	m_Scanner.GetScanner(m_pScanner);
	m_Scanner.GetScanSink(m_pSink);
	m_Scanner.GetScanProperties(m_pProperties);

	ccEraser::eResult res;

		res = m_pEraser->Initialize(m_pScanner, m_pSink, m_pProperties, GetSystemDefaultLangID());
	
	if(Failed(res))
	{
		::AfxMessageBox(_T("Failed to initalize ccEraser"));
		CCTRACEE(_T("StartEraser():FAILED to Initialize Eraser"));
		return FALSE;
	}

	return TRUE;
}

int CSketchbookApp::ExitInstance()
	{
	int iRtn = CWinApp::ExitInstance();

	if(m_bATReturn)
	{
		return 3;
	}
	else if(iRtn != 3)
	{
		return iRtn;
	}
	else
	{
		return 1;
	}
}

BOOL CSketchbookApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	g_DebugOutput.LoadOptions();
	g_CrashHandler.LoadOptions();
	
	InitCommonControls();

	CWinApp::InitInstance();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization

	if(S_OK != ::CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY | COINIT_MULTITHREADED ))
	{
		::AfxMessageBox(_T("FAILED to CoInitialize"));
		CCTRACEE(_T("FAILED to CoInitialize"));
		return FALSE;
	}

	CSketchbookCLI cli;

	ParseCommandLine(cli);

	TCHAR szCurDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szCurDir);

	if(cli.m_bUndo)
	{
#ifdef _DEBUG
		::AfxMessageBox(_T("Waiting for debugger"));
#endif

		if(cli.m_strInput != _T("NoUndo"))
		{

			if(!StartEraser())
			{
				return FALSE;
			}

			CStdioFile file;
			file.Open(cli.m_strInput, CFile::modeRead | CFile::typeText);

			CString strFile;
			
			while(file.ReadString(strFile))
			{
				ccSym::CFileStreamImplPtr pActionStream;
				pActionStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
				ccLib::CFile & ActionFile = pActionStream->GetFile();
				if(ActionFile.Open(strFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
				{
				ccEraser::eObjectType type;
				ISymBasePtr pBase;
				ccEraser::eResult res = theApp.m_pEraser->CreateObjectFromStream(pActionStream, &type, pBase);
				ActionFile.Close();
				::DeleteFile(strFile);
				if(Failed(res))
				{
					m_bATReturn = FALSE;
						file.ReadString(strFile);
						::DeleteFile(strFile);
					continue;
				}

				ccEraser::IRemediationActionQIPtr pRemediationAction = pBase; 
				file.ReadString(strFile);

					if(pRemediationAction != NULL)
					{
				ccSym::CFileStreamImplPtr pUndo;
				pUndo.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
				ccLib::CFile & UndoFile = pUndo->GetFile();
						if(UndoFile.Open(strFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))
	{
					cc::IStringPtr pDir;
					cc::IStringPtr pFile;
					pDir.Attach(ccSym::CStringImpl::CreateStringImpl(_T("c:\\FakeUndo")));
					pFile.Attach(ccSym::CStringImpl::CreateStringImpl(_T("FakeUndo.undo")));

					::CreateDirectory(_T("c:\\FakeUndo"), NULL);
							ULONGLONG pos;
							pUndo->GetPosition(pos);
					pRemediationAction->SaveUndoInformation(pDir, pUndo);
							pUndo->SetPosition(pos);
					pRemediationAction->SaveUndoInformation(pDir, pFile, pUndo);
							pUndo->SetPosition(pos);

							DeleteWildCardBack(_T("c:\\FakeUndo"), _T(""), FALSE);

					res = pRemediationAction->Undo(pUndo);
					if(Succeeded(res))
					{
						m_bATReturn = m_bATReturn && TRUE;
					}
					else
					{
						m_bATReturn = FALSE;
					}

							UndoFile.Close();
						}
						else
						{
							m_bATReturn = FALSE;
						}
					}
					else
					{
						m_bATReturn = FALSE;
				}

				::DeleteFile(strFile);
			}
				else
				{
					m_bATReturn = FALSE;
					::DeleteFile(strFile);
					file.ReadString(strFile);
					::DeleteFile(strFile);
				}
			}

			file.Close();
			::DeleteFile(cli.m_strInput);
		}
	}
	else if(cli.m_bBA || cli.m_bGLP)
	{
#ifdef _DEBUG
		::AfxMessageBox(_T("Waiting for debugger"));
		#endif

		ccEraser::eResult res;
		CStdioFile output(cli.m_strOutput, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		CTime time = CTime::GetCurrentTime();
		CString strTime = time.Format(_T("%Y-%m-%d-%H-%M-%S"));
		output.WriteString(strTime);
		output.WriteString(_T("\n"));

		if(cli.m_strInput != _T(""))
		{
			ccLib::CString strPath;
			ccLib::CSplitPath::GetProcessDirectory(strPath);
			strPath += BINFILE;

			if(!::SetFileAttributes(strPath, FILE_ATTRIBUTE_NORMAL))
			{
				output.WriteString(_T("Cannot change existing definition file attribute\n"));
			}
			if(!::DeleteFile(strPath))
			{
				output.WriteString(_T("Cannot delete existing definition file\n"));
			}
			
			ccLib::CSplitPath split;
			split.SplitPath(cli.m_strInput);

			if(::_tcsicmp(split.GetExtension(), _T("bin")) != 0)
			{
				::DeleteFile(strPath);
			
			CString strCmd;
				strCmd.Format(_T("EsrBinCp.exe /X \"%s\" \"%s\""), cli.m_strInput,strPath);

			STARTUPINFO si;
			PROCESS_INFORMATION pi;

			ZeroMemory(&si, sizeof(si));
			si.cb = sizeof(si);
			ZeroMemory(&pi, sizeof(pi));

				::CreateProcess(_T("EsrBinCp.exe"), strCmd.GetBuffer(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
			::WaitForSingleObject(pi.hProcess, INFINITE);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			}
			else
			{
				if(!::CopyFile(cli.m_strInput, strPath, FALSE))
				{
					output.WriteString(_T("Cannot replace existing definition file\n"));
				}
			}
		}

		m_Scanner.m_bDecompose = cli.m_bDecompose;

		if(!StartEraser())
		{
			output.WriteString(_T("FATAL: Cannot start ccEraser engine!!!!!!!\n"));
			output.Close();
			return FALSE;
		}

		if(cli.m_bBA)
		{
			ccEraser::IFilterQIPtr pFilter;
			res = theApp.m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, (void **) &pFilter);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot create new filter object\n"));
			}

			res = pFilter->ClearDetectionFilter();
			if(Failed(res))
			{
				output.WriteString(_T("Cannot clear detection filter\n"));
			}

			res = pFilter->AddDetectionType(ccEraser::BatchDetectionActionType);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot add BatchDetectionActionType to filter\n"));
			}
			res = pFilter->RemoveDetectionType(ccEraser::BatchDetectionActionType);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot remove BatchDetectionActionType to filter\n"));
			}
			

			res = pFilter->ClearAnomalyFilter();
			if(Failed(res))
			{
				output.WriteString(_T("Cannot add Viral to filter\n"));
			}

			res = pFilter->AddAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot add GenericLoadPoint to filter\n"));
			}
			res = pFilter->RemoveAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot remove GenericLoadPoint to filter\n"));
			}
			res = pFilter->AddAnomalyCategory(ccEraser::IAnomaly::Viral);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot add Viral to filter\n"));
			}

			cc::IKeyValueCollectionPtr pSettings;
			res = theApp.m_pEraser->GetSettings(pSettings);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get ccEraser setting\n"));
			}

			if(!pSettings->SetValue(ccEraser::IEraser::InputFilter, pFilter))
			{
				output.WriteString(_T("Cannot set ccEraser filter setting\n"));
			}
		}
		else if(cli.m_bGLP)
		{
			DWORD dwEngineMost;
			DWORD dwEngineLeast;
			res = theApp.m_pEraser->GetEngineVersion(dwEngineMost, dwEngineLeast);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get engine version\n"));
			}

			DWORD dwDataMost;
			DWORD dwDataLeast;
			res = theApp.m_pEraser->GetDataVersion(dwDataMost, dwDataLeast);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get data version\n"));
			}

			CString strVersion;
			strVersion.Format(_T("Engine %u.%u Data %u.%u\n"), dwEngineMost, dwEngineLeast, dwDataMost, dwDataLeast);
			output.WriteString(strVersion);

			ccEraser::IFilterQIPtr pFilter;
			res = theApp.m_pEraser->CreateObject(ccEraser::FilterType, ccEraser::IID_Filter, (void **) &pFilter);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot create new filter object\n"));
			}

			res = pFilter->AddAnomalyCategory(ccEraser::IAnomaly::GenericLoadPoint);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot add GenericLoadPoint to filter\n"));
			}

			cc::IKeyValueCollectionPtr pSettings;
			res = theApp.m_pEraser->GetSettings(pSettings);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get ccEraser setting\n"));
			}

			if(!pSettings->SetValue(ccEraser::IEraser::InputFilter, pFilter))
			{
				output.WriteString(_T("Cannot set ccEraser filter setting\n"));
			}

			ccEraser::IAnomalyListPtr pAnomalyList;
			res = theApp.m_pEraser->GetAnomalies(pAnomalyList);
			if(Succeeded(res))
			{
				size_t dwSize;
				pAnomalyList->GetCount(dwSize);
				CString strCount;
				strCount.Format(_T("Total Anomalies in Binary files: %u\n"), dwSize);
				output.WriteString(strCount);
			}

			GUID clsid;
			ccEraser::IAnomalyPtr pAnomaly;
			WCHAR szCLSID[] = L"{D0505BCB-E1D0-4b3e-854D-811EEE0A1C46}";
			if(NOERROR == CLSIDFromString(szCLSID, &clsid))
			{

				res = m_pEraser->GetAnomaly((REFSYMOBJECT_ID)clsid, pAnomaly);
				if(Failed(res))
				{
					output.WriteString(_T("Unable to get Hardcoded GLP\n"));
				}
			}
		}

		CSymPtr<CCallback> pCallback = new CCallback;
		ccEraser::IContextPtr pContext;

		res = theApp.m_pEraser->Scan(pCallback, pContext);
		if(Failed(res))
		{
			output.WriteString(_T("FATAL: Cannot scan using ccEraser!!!!!!!\n"));
			m_bATReturn = FALSE;
		}

		size_t AnomalyCount = pContext->GetAnomalyCount();
		size_t AnomalyCompleteCount = pContext->GetAnomalyCompleteCount();
		size_t DetectionCount = pContext->GetDetectionCount();
		size_t DetectionCompleteCount = pContext->GetDetectionCompleteCount();

		CString strStatus;
		strStatus.Format(_T("Total Anomaly to Scan: %u; Anomaly Completed: %u; Total Dectection Count: %u; Detection Complete: %u;"),
		AnomalyCount, AnomalyCompleteCount, DetectionCount, DetectionCompleteCount);

		output.WriteString(strStatus + _T("\n"));

		ccEraser::IAnomalyListPtr pAnomalyList;
		pAnomalyList.Attach(pContext->GetDetectedAnomalyList());
		if(pAnomalyList == NULL)
		{
			output.WriteString(_T("Fatal: Returned anomaly list is null.\n"));
			output.Close();
			return FALSE;
		}

		m_bATReturn = RemediateAnomalyList(pAnomalyList, output);
	}
	else if(cli.m_bFullScan) 
	{
#ifdef _DEBUG
		::AfxMessageBox(_T("Waiting for debugger"));
#endif

		ccLib::CString strPath;
		ccLib::CSplitPath::GetProcessDirectory(strPath);
		strPath += BINFILE;

		::SetFileAttributes(strPath, FILE_ATTRIBUTE_NORMAL);
		::CopyFile(cli.m_strInput, strPath, FALSE);

		CStdioFile output(cli.m_strOutput, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		output.SeekToEnd();

		CString strMsg;
		if(!StartEraser())
		{
			return FALSE;
		}

		strMsg.Format(_T("Beginning full scan\n"));
		output.WriteString(strMsg);

		// Build master anomaly list. 
		ccEraser::IAnomalyListPtr pAnomalyList;
		ccEraser::eResult res = theApp.m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &pAnomalyList);
		if(Failed(res))
		{
			strMsg.Format(_T("Fatal: Cannot create empty anomaly list\n"));
			output.WriteString(strMsg);
			output.Close();
			return FALSE;
		}

		DWORD dwAttribute = ::GetFileAttributes(cli.m_strFullScanPath);
		if(dwAttribute & FILE_ATTRIBUTE_DIRECTORY)
		{
		CSymPtr<CScanItem> pScanItem;
		pScanItem = new CScanItem;
		pScanItem->AddRef();
		pScanItem->SetPath(cli.m_strFullScanPath);

		IScanResultsPtr pScanResult;

			SCANSTATUS status = m_pScanner->Scan(pScanItem, m_pProperties, m_pSink, &pScanResult);
		pScanItem = NULL;

		if(status != SCAN_OK)
		{
			strMsg.Format(_T("Scan failed\n"));
			output.WriteString(strMsg);
			output.Close();
			return FALSE;
		}

		int iScan = pScanResult->GetTotalFilesScanned();
		int iInfected = pScanResult->GetTotalInfectionCount();
			strMsg.Format(_T("%d files scanned with %d infected."), iScan, iInfected);
		output.WriteString(strMsg);

		ccEraser::IAnomalyListPtr pTempAnomalyList;
		for(int i=0;i<iInfected;i++)
		{
			IScanInfectionPtr pScanInfection;
			status = pScanResult->GetInfection(i, &pScanInfection);
			if(status != SCAN_OK)
			{
				strMsg.Format(_T("Error: status = pScanResult->GetInfection(i, &pScanInfection); for i=%d\n"), i);
				output.WriteString(strMsg);
				continue;
			}

			ccEraser::eResult res = theApp.m_pEraser->DetectAnomalies(pScanInfection, pTempAnomalyList);
			if(Failed(res))
			{
				strMsg.Format(_T("Scan failed with iInfected: %u\n\n"), i);
				output.WriteString(strMsg);
				continue;
			}

			res = pAnomalyList->Merge(pTempAnomalyList);
			if(Failed(res))
			{
				strMsg.Format(_T("Failed to merge anomaly lists for anomaly i=%u"), i);
				output.WriteString(strMsg);
				continue;
			}
		}
		}
		else
		{
			TCHAR szLongPath[MAX_PATH];
			TCHAR szShortPath[MAX_PATH];
			if(::PathFileExists(cli.m_strFullScanPath))
			{
				::GetLongPathName(cli.m_strFullScanPath, szLongPath, MAX_PATH); 
				::GetShortPathName(cli.m_strFullScanPath, szShortPath, MAX_PATH);
			}
			else
			{
				CFile Target;
				Target.Open(cli.m_strFullScanPath, CFile::modeCreate | CFile::modeReadWrite);
				Target.Close();

				::GetLongPathName(cli.m_strFullScanPath, szLongPath, MAX_PATH); 
				::GetShortPathName(cli.m_strFullScanPath, szShortPath, MAX_PATH);

				::DeleteFile(cli.m_strFullScanPath);
			}

			cc::IStringPtr pFilePath;
			cc::IStringPtr pShortFilePath;
			pFilePath.Attach(ccSym::CStringImpl::CreateStringImpl(szLongPath));
			pShortFilePath.Attach(ccSym::CStringImpl::CreateStringImpl(szShortPath));
	
			ccEraser::eResult res = m_pEraser->DetectAnomalies(pFilePath, pShortFilePath, 18953, pAnomalyList);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot do DetectAnomalies"));
			}
		}

		m_bATReturn = RemediateAnomalyList(pAnomalyList, output);
	}
	else
	{
		CSessionDlg dlg;
		m_pMainWnd = &dlg;
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	}

	::SetCurrentDirectory(szCurDir);

	m_pEraser = NULL;
	m_pScanner = NULL;
	m_pSink = NULL;
	m_pProperties = NULL;

	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}

BOOL CSketchbookApp::RemediateAnomalyList(ccEraser::IAnomalyListPtr &pAnomalyList, CStdioFile &output)
{
	if(pAnomalyList == NULL)
	{  
		output.WriteString(_T("Anomaly list is null\n"));
		output.Close();
		return FALSE;
	}

	ccEraser::eResult res;

	cc::IClonePtr pCopy;
	m_pEraser->CreateObject(ccEraser::AnomalyListType, cc::IID_Clone, (void **) &pCopy);
	pCopy->Copy(pAnomalyList);
	ccEraser::IAnomalyListQIPtr pCopyDest = pCopy;

	cc::ICloneQIPtr pCloneSrc = pAnomalyList;
	ISymBasePtr pCloneDest;
	pCloneSrc->Clone(pCloneDest);
	ccEraser::IAnomalyListQIPtr pCloneDest2 = pCloneDest;

	res = pCopyDest->Merge(pCloneDest2);
	if(Failed(res))
	{
		output.WriteString(_T("Fatal: Cloning and copy failed\n"));
	}
	else
	{
//		pAnomalyList = pCopyDest;
	}

	
	ccSym::CFileStreamImplPtr pOutStream;
    pOutStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
    ccLib::CFile & OutFile = pOutStream->GetFile();
	OutFile.Open(_T("c:\\serialize.data"), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	cc::ISerializeQIPtr pSerialize = pAnomalyList;
	pSerialize->SetDirty(true);
	pSerialize->Save(pOutStream);
	if(!pSerialize->GetDirty())
	{
		output.WriteString(_T("Fatal: Dirty bit didn't change to false\n"));
	}
	OutFile.Close();

	ccSym::CFileStreamImplPtr pInStream;
    pInStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
    ccLib::CFile & InFile = pInStream->GetFile();
	InFile.Open(_T("c:\\serialize.data"), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	ccEraser::eObjectType type;
	ISymBasePtr pBase;
	res = m_pEraser->CreateObjectFromStream(pInStream, &type, pBase);
	if(Failed(res))
	{
		output.WriteString(_T("Fatal: Serialize doesn't work\n"));
	}
	/*
	else
	{
		ccEraser::IAnomalyListQIPtr pAnomalySerialize = pBase;
		pAnomalyList = pAnomalySerialize;
	}
	*/

	OutFile.Close();

	BOOL bRetVal = TRUE;

		size_t dwSize;
		res = pAnomalyList->GetCount(dwSize);
		if(Failed(res))
		{
		output.WriteString(_T("Fatal: Cannot get return anomaly list count.\n"));
		bRetVal = FALSE;
		}

		CString strFiles = _T("");

		ccEraser::IAnomalyPtr pAnomaly;
		for(size_t i = 0; i < dwSize; i++)
		{
			res = pAnomalyList->GetItem(i, pAnomaly);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get Anomaly item\n"));
				continue;
			}

			cc::IKeyValueCollectionPtr pProps;
			res = pAnomaly->GetProperties(pProps);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get Anomaly property\n"));
			}

			CString strAnomaly;
			ISymBasePtr pBase;
			if(pProps->GetValue(ccEraser::IAnomaly::Name, pBase))
			{
				cc::IStringQIPtr pAnomalyName = pBase;
				strAnomaly = ccSym::CStringConvert::GetStringT(pAnomalyName);
			}

			CString strID;
			if(pProps->GetValue(ccEraser::IAnomaly::ID, pBase))
			{
				cc::IStringQIPtr pGUID = pBase;
				strID = ccSym::CStringConvert::GetStringT(pGUID);
			}

			output.WriteString( _T("ANOMALY:\n") );
			output.WriteString( strAnomaly + _T(" ") + strID);
		output.WriteString( _T("\n\nACTIONS:\n") );

			ccEraser::IDetectionActionListPtr pDetectionActionList;
			res = pAnomaly->GetDetectionActions(pDetectionActionList);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get Detection List\n"));
				continue;
			}

		cc::IClonePtr pCopy;
		m_pEraser->CreateObject(ccEraser::DetectionActionListType, cc::IID_Clone, (void **) &pCopy);
		pCopy->Copy(pDetectionActionList);
		ccEraser::IDetectionActionListQIPtr pCopyDest = pCopy;

		cc::ICloneQIPtr pCloneSrc = pDetectionActionList;
		ISymBasePtr pCloneDest;
		pCloneSrc->Clone(pCloneDest);
		ccEraser::IDetectionActionListQIPtr pCloneDest2 = pCloneDest;

		res = pCopyDest->Merge(pCloneDest2);
		if(Failed(res))
		{
			output.WriteString(_T("Fatal: Cloning and copy failed\n"));
		}
		else
		{
			// pDetectionActionList = pCopyDest;
		}

			size_t dwCount;
			ccEraser::eResult res = pDetectionActionList->GetCount(dwCount);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get Detection List count\n"));
				continue;
			}

			for(size_t j = 0; j < dwCount; j++)
			{
				ccEraser::IDetectionActionPtr pDetectionAction;
				res = pDetectionActionList->GetItem(j, pDetectionAction);
				if(Failed(res))
				{
					output.WriteString(_T("Cannot get Detection item\n"));
					continue;
				}

				CString strDescription;
				cc::IStringPtr pDescription;
				res = pDetectionAction->GetDescription(pDescription);
				if(Succeeded(res))
				{
					strDescription = ccSym::CStringConvert::GetStringT(pDescription);
				}

			CString strOperands;
			cc::IStringPtr pOperands;
			res = pDetectionAction->GetOperands(pOperands);
			if(Succeeded(res))
			{
				strOperands = ccSym::CStringConvert::GetStringT(pOperands);
			}

			CString strOperations;
			cc::IStringPtr pOperations;
			res = pDetectionAction->GetOperation(pOperations);
			if(Succeeded(res))
			{
				strOperations = ccSym::CStringConvert::GetStringT(pOperations);
			}

			CString strType;
			cc::IStringPtr pType;
			res = pDetectionAction->GetType(pType);
			if(Succeeded(res))
			{
				strType = ccSym::CStringConvert::GetStringT(pType);
			}

			if(strDescription.Find(strOperands) == -1 ||
				strDescription.Find(strOperations) == -1 ||
				strDescription.Find(strType) == -1)
			{
				output.WriteString(_T("Cannot find matching string\n"));
			}

			cc::IKeyValueCollectionPtr pCollection;
			pCollection.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
			res = pDetectionAction->GetUserData(pCollection);
			if(Failed(res))
			{
//				output.WriteString(_T("Cannot get userdata\n"));
			}

				CString strOut;
				res = pDetectionAction->GetProperties(pProps);
				if(Succeeded(res))
				{
					DWORD dwState;
					if(pProps->GetValue(ccEraser::IDetectionAction::State, dwState))
					{
						if(dwState == ccEraser::IDetectionAction::Detected)
						{
							strOut.Format(_T("POSITIVE : %u : %s\n"), res, strDescription);
						bRetVal = bRetVal && TRUE;
						}
						else
						{
							strOut.Format(_T("NEGATIVE : %u : %s\n"), res, strDescription);
						bRetVal = FALSE;
						}
					}
				}
		
				output.WriteString(strOut);
				output.WriteString( _T("\n"));
			}

			ccEraser::IRemediationActionListPtr pRemediationActionList;
			res = pAnomaly->GetRemediationActions(pRemediationActionList);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get Remediation List\n"));
				continue;
			}

		cc::IClonePtr pCopyR;
		m_pEraser->CreateObject(ccEraser::RemediationActionListType, cc::IID_Clone, (void **) &pCopyR);
		pCopyR->Copy(pRemediationActionList);
		ccEraser::IRemediationActionListQIPtr pCopyDestR = pCopyR;

		cc::ICloneQIPtr pCloneSrcR = pRemediationActionList;
		ISymBasePtr pCloneDestR;
		pCloneSrcR->Clone(pCloneDestR);
		ccEraser::IRemediationActionListQIPtr pCloneDest2R = pCloneDestR;

		res = pCopyDestR->Merge(pCloneDest2R);
		if(Failed(res))
		{
			output.WriteString(_T("Fatal: Cloning and copy of remediation failed\n"));
		}
		else
		{
			pRemediationActionList = pCopyDestR;
		}

			res = pRemediationActionList->GetCount(dwCount);
			if(Failed(res))
			{
				output.WriteString(_T("Cannot get Remediation List count\n"));
				continue;
			}

			::CreateDirectory(_T("undo"), NULL);

			for(size_t j = 0; j < dwCount; j++)
			{
				ccEraser::IRemediationActionPtr pRemediationAction;
				res = pRemediationActionList->GetItem(j, pRemediationAction);
				if(Failed(res))
				{
					output.WriteString(_T("Cannot get Remediation item\n"));
					continue;
				}

				bool bPresent = true;
				res = pRemediationAction->IsPresent(bPresent);
				if(Succeeded(res) && !bPresent)
				{
					continue;
				}

				CString strDescription;
				cc::IStringPtr pDescription;
				res = pRemediationAction->GetDescription(pDescription);
				if(Succeeded(res))
				{
					strDescription = ccSym::CStringConvert::GetStringT(pDescription);
				}

			CString strOperands;
			cc::IStringPtr pOperands;
			res = pRemediationAction->GetOperands(pOperands);
			if(Succeeded(res))
			{
				strOperands = ccSym::CStringConvert::GetStringT(pOperands);
			}

			CString strOperations;
			cc::IStringPtr pOperations;
			res = pRemediationAction->GetOperation(pOperations);
			if(Succeeded(res))
			{
				strOperations = ccSym::CStringConvert::GetStringT(pOperations);
			}

			CString strType;
			cc::IStringPtr pType;
			res = pRemediationAction->GetType(pType);
			if(Succeeded(res))
			{
				strType = ccSym::CStringConvert::GetStringT(pType);
			}

			if(strDescription.Find(strOperands) == -1 ||
				strDescription.Find(strOperations) == -1 ||
				strDescription.Find(strType) == -1)
			{
				output.WriteString(_T("Cannot find matching string\n"));
			}

			cc::IKeyValueCollectionPtr pCollection;
			pCollection.Attach(ccSym::CKeyValueCollectionImpl::CreateKeyValueCollectionImpl());
			res = pRemediationAction->GetUserData(pCollection);
			if(Failed(res))
			{
				//output.WriteString(_T("Cannot get userdata\n"));
			}

				bool bUndo = false;
				res = pRemediationAction->SupportsUndo(bUndo);
			if(!bUndo && Succeeded(res))
			{
				pRemediationAction->GetUndoInformation(NULL);
				pRemediationAction->Undo(NULL);
				pRemediationAction->SaveUndoInformation(NULL, NULL);
				pRemediationAction->SaveUndoInformation(NULL, NULL, NULL);
			}

				if(bUndo && Succeeded(res))
				{
					CString strUndo = CSketchbookDlg::GetTimeStampFile(_T("undo"), _T("data"));
				ccSym::CFileStreamImplPtr pUndo;
				pUndo.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
					ccLib::CFile & UndoFile = pUndo->GetFile();
					UndoFile.Open(strUndo, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

					res = pRemediationAction->GetUndoInformation(pUndo);
					UndoFile.Close();
					if(Succeeded(res))
					{
						CString strAction = CSketchbookDlg::GetTimeStampFile(_T("undo"), _T("data"));
					ccSym::CFileStreamImplPtr pActionStream;
					pActionStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
						ccLib::CFile & ActionFile = pActionStream->GetFile();
						ActionFile.Open(strAction, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

						cc::ISerializeQIPtr pSerialize = pRemediationAction;
						pSerialize->Save(pActionStream);
						ActionFile.Close();

						CString strPath = strAction;
						strPath += _T("\n");
						strPath += strUndo;

						strFiles.Insert(0, strPath + _T("\n"));
					}
					else
					{
						::DeleteFile(strUndo);
						bUndo = false;
					}
				}

				CString strOut;
				res = pRemediationAction->Remediate();
				if(Failed(res))
				{
					strOut.Format(_T("NEGATIVE : %u : %s\n"), res, strDescription);
				bRetVal = FALSE;
				}
				else
				{
					res = pRemediationAction->GetProperties(pProps);
					if(Succeeded(res))
					{
						DWORD dwState;
						if(pProps->GetValue(ccEraser::IRemediationAction::State, dwState))
						{
							if(dwState == ccEraser::IRemediationAction::Remediated)
							{
								strOut.Format(_T("POSITIVE : %u : %s\n"), res, strDescription);
							bRetVal = bRetVal && TRUE;
							}
							else
							{
								strOut.Format(_T("NEGATIVE : %u : %s\n"), res, strDescription);
							bRetVal = FALSE;
							}
						}
					}
				}

				output.WriteString(strOut);
				output.WriteString( _T("\n"));
			}
/*
			TCHAR szPath[MAX_PATH];
			CString strName;
			::GetModuleFileName(NULL, szPath, MAX_PATH);
			_tcscpy(_tcsrchr(szPath, _T('\\')) + 1, _T(""));
			strName = CSketchbookDlg::GetTimeStampFile(szPath, _T("undo"));
*/
			CStdioFile file;
		file.Open(_T("auto.undo"), CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeText);
			file.WriteString(strFiles);
			file.Close();

		output.WriteString( _T("\n\n"));
		}

	return bRetVal;
}

void CSketchbookApp::DeleteWildCardBack (CString dirPath, CString searchFile, BOOL subDirSearch)
{
	//no arguments validity check

	TCHAR szDir[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, szDir);
	BOOL bDir = ::SetCurrentDirectory(dirPath);
	if(bDir) {
	
		CFileFind cfindFile;
		CString strFileName;
		searchFile = searchFile.MakeLower();
		BOOL bFoundFile = cfindFile.FindFile(_T("*.*"));

		//go through each file
		while(bFoundFile)
		{
			bFoundFile = cfindFile.FindNextFile();
			// skip . and .. files; otherwise, we'd recur infinitely!
			if (cfindFile.IsDots())
				continue;

			// if it's a directory, recursively search it
			if (cfindFile.IsDirectory() && subDirSearch)
			{
				CString newDirPath = dirPath + _T("\\") + cfindFile.GetFileName();
				DeleteWildCardBack (newDirPath, searchFile, subDirSearch);
			}
			//if it's not a directory
			else if (!cfindFile.IsDirectory())
			{
				//Tries to match the substring, searchFile
				strFileName = cfindFile.GetFileName();
				strFileName = strFileName.MakeLower();
	
				if (searchFile == _T("") || strFileName.Find (searchFile, 0) != -1)
				{
					//File Found, delete it...
					::SetFileAttributes(strFileName, FILE_ATTRIBUTE_NORMAL);
					::DeleteFile (strFileName);
				}
			}

		}//while
		cfindFile.Close();
	} //if bDir
	::SetCurrentDirectory(szDir);
}