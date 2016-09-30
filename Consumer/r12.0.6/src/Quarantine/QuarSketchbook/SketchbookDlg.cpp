// /////////////////////////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2004 Symantec Corporation.
// All rights reserved.
// /////////////////////////////////////////////////////////////////////////////////////////////////
//
// SketchbookDlg.cpp : implementation file
//

#include "stdafx.h"

#include "Sketchbook.h"
#include "SketchbookDlg.h"
#include "callback.h"
#include "scanitem.h"
#include "undodlg.h"
#include "SetSettings.h"
#include "detectdialog.h"
#include "detectlistdialog.h"

#include "ccIgnoreWarnings.h"
#include "ccKeyValueCollectionInterface.h"
#include "ccSymStringConvert.h"
#include "ccSymStringImpl.h"
#include "ccSymFileStreamImpl.h"
#include "ccSymIndexValueCollectionImpl.h"
#include ".\sketchbookdlg.h"
#include "ccSymMemoryStreamImpl.h"
#include "Quar32.h"
#include <AtlFile.h>

//QSPak Includes
#include "qspak.h"
#include "qsfields.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Undocumented -- don't use this in production code.
extern "C" WINSHELLAPI BOOL WINAPI RegenerateUserEnvironment(PVOID *, BOOL);

UINT ThreadFullScan(LPVOID lpParameter)
{
	CSketchbookDlg * Dlg = (CSketchbookDlg *)lpParameter;

	TCHAR szPath[MAX_PATH];
	BROWSEINFO info;

	ZeroMemory(&info, sizeof(BROWSEINFO));
	info.lpszTitle = _T("Select the folder which you want to do Full Scan");
	info.ulFlags = BIF_NONEWFOLDERBUTTON | BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN;

	LPITEMIDLIST item = ::SHBrowseForFolder(&info);

	CSymPtr<CScanItem> pScanItem;

	if(item != NULL)
	{
		if(::SHGetPathFromIDList(item, szPath))
		{
			pScanItem  = new CScanItem;
			pScanItem->AddRef();

			pScanItem->SetPath(szPath);
		}
		else
		{
			::AfxMessageBox(_T("Cannot get Directory information"));
			return 0;
		}
	}
	else
	{
		return 0;
	}

	ccEraser::eResult res = Dlg->m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &Dlg->m_pAnomalyList);
	Dlg->setResultWindow(res, _T("Dlg->m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &Dlg->m_pAnomalyList)"));
	if(Failed(res))
	{
		pScanItem = NULL;
		return 0;
	}

	IScanResultsPtr pScanResult;
	DWORD dwScanTick = ::GetTickCount();
	SCANSTATUS status = Dlg->m_pScanner->Scan(pScanItem, Dlg->m_pProperties, Dlg->m_pSink, &pScanResult);
	dwScanTick = ::GetTickCount() - dwScanTick;
	pScanItem = NULL;

	if(status != SCAN_OK)
	{
		::AfxMessageBox(_T("ccScan Error"));
		return 0;
	}

	int iScan = pScanResult->GetTotalFilesScanned();
	int iInfected = pScanResult->GetTotalInfectionCount();
	DWORD dwTick = ::GetTickCount();

	for(int i = 0; i < iInfected; i++)
	{
		IScanInfectionPtr pScanInfection;
		status = pScanResult->GetInfection(i, &pScanInfection);
		if(status != SCAN_OK)
		{
			::AfxMessageBox(_T("Get infection error"));
			continue;
		}

		ccEraser::IAnomalyListPtr pAnomalyList;
		ccEraser::eResult res = Dlg->m_pEraser->DetectAnomalies(pScanInfection, pAnomalyList);
		Dlg->setResultWindow(res, _T("Dlg->m_pEraser->DetectAnomalies(pScanInfection, pAnomalyList)"));

		res = Dlg->m_pAnomalyList->Merge(pAnomalyList);
		Dlg->setResultWindow(res, _T("Dlg->m_pAnomalyList->Merge(pAnomalyList)"));
	}

	CString strScan;
	strScan.Format(_T("Total files scanned: %u; Total infected files: %u; Total time in ccScan: %ums; Total time in ccEraser: %ums;"), iScan, iInfected, dwScanTick, ::GetTickCount() - dwTick);
	Dlg->m_editCtrlScan.SetWindowText(strScan);

	Dlg->SetAnomalyCtrl();

	return 0;
}

UINT ThreadScan(LPVOID lpParameter)
{
	CSketchbookDlg * Dlg = (CSketchbookDlg *)lpParameter;

	CSymPtr<CCallback> pCallback = new CCallback;
	ccEraser::IContextPtr pContext;
	pCallback->m_pDlg = Dlg;

	Dlg->m_buttonCtrlAbort.EnableWindow();
	DWORD dwTick = ::GetTickCount();
	ccEraser::eResult res = Dlg->m_pEraser->Scan(pCallback, pContext);

	dwTick = ::GetTickCount() - dwTick;
	Dlg->m_buttonCtrlAbort.EnableWindow(FALSE);

	Dlg->setResultWindow(res, _T("Dlg->m_pEraser->Scan(pCallback, pContext)"));

	if(Failed(res))
	{
		return 0;
	}

	Dlg->m_pAnomalyList.Attach(pContext->GetDetectedAnomalyList());

	size_t nCount = 0;
	res = Dlg->m_pAnomalyList->GetCount(nCount);
	Dlg->setResultWindow(res, _T("Dlg->m_pAnomalyList->GetCount(nCount)"));

	if(pCallback->m_dwDetectedCalled != nCount)
	{
		::AfxMessageBox(_T("m_dwDetectedCalled != nCount"));
	}

	Dlg->SetAnomalyCtrl();

	CString strTick;
	CString strText;
	pCallback->PutScanStatus(pContext);
	strTick.Format(_T("; Total Scan time: %ums"), dwTick);
	Dlg->m_editCtrlScan.GetWindowText(strText);
	strText += strTick;
	Dlg->m_editCtrlScan.SetWindowText(strText);

	return 0;
}

UINT ThreadGetAnomaly(LPVOID lpParameter)
{
	CSketchbookDlg * Dlg = (CSketchbookDlg *)lpParameter;

	Dlg->m_bGetAnomaly = TRUE;
	Dlg->m_buttonCtrlAbort.EnableWindow();
	DWORD dwTick = ::GetTickCount();
	ccEraser::eResult res = Dlg->m_pEraser->GetAnomalies(Dlg->m_pAnomalyList);
	dwTick = ::GetTickCount() - dwTick;
	Dlg->m_buttonCtrlAbort.EnableWindow(FALSE);
	Dlg->m_bGetAnomaly = FALSE;

	Dlg->setResultWindow(res, _T("Dlg->m_pEraser->GetAnomalies(m_pAnomalyList)"));
	if(Failed(res))
	{
		return 1;
	}

	CString strTick;
	strTick.Format(_T("Definition load time: %u ms"), dwTick);
	Dlg->m_editCtrlScan.SetWindowText(strTick);

	Dlg->SetAnomalyCtrl();

	return 0;
}

// CSketchbookDlg dialog


CSketchbookDlg::CSketchbookDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSketchbookDlg::IDD, pParent)
	, m_bLocalized(TRUE)
	, m_bCache(TRUE)
	, m_bUndo(TRUE)
	, m_bGetAnomaly(FALSE)
	, m_bScanAbort(FALSE)
	, m_pQuarItem(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	// Create quarantine object
	// Create the quarantine server object.
    if(SYM_FAILED(QuarDLL_Loader::CreateObject(GETMODULEMGR(), &m_pQuarantineDLL)) ||
        m_pQuarantineDLL == NULL)
	{
		//AfxMessageBox( IDS_ERROR_CREATING_QUARANTINE_OBJECT, MB_ICONSTOP | MB_OK );
	}

	// Initialize the IQuarantineDLL.
	HRESULT hr = m_pQuarantineDLL->Initialize();
	if(FAILED(hr))
	{
		// ERROR_CREATING_QUARANTINE_OBJECT
	}

    m_bAPToggled = m_APWrapper.EnableAPForThisThread();
}

CSketchbookDlg::~CSketchbookDlg()
{
	if(m_pQuarantineDLL)
    {
        m_pQuarantineDLL->Release();
        m_pQuarantineDLL = NULL;
    }

	if(m_bAPToggled)
    {
        m_APWrapper.DisableAPForThisThread();
    }
}

void CSketchbookDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANOMALY, m_listCtrlAnomaly);
	DDX_Control(pDX, IDC_LIST_DETECTION, m_listCtrlDetections);
	DDX_Control(pDX, IDC_LIST_REMEDIATION, m_listCtrlRemediations);
	DDX_Control(pDX, IDC_RESULT, m_editCtrlResult);
	DDX_Control(pDX, IDC_SCANSTATUS, m_editCtrlScan);
	DDX_Check(pDX, IDC_LOCALIZE, m_bLocalized);
	DDX_Check(pDX, IDC_CACHE, m_bCache);
	DDX_Check(pDX, IDC_SAVEUNDO, m_bUndo);
	DDX_Control(pDX, IDC_ABORT, m_buttonCtrlAbort);
	DDX_Control(pDX, IDC_EDIT_QUAR_ITEM_STATUS, m_editCtrlQuarItem);
}

BEGIN_MESSAGE_MAP(CSketchbookDlg, CDialog)
	//}}AFX_MSG_MAP
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_ANOMALY, OnLvnItemchangedListAnomaly)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_REMEDIATION, OnNMCustomdrawListRemediation)
	ON_BN_CLICKED(IDC_SET_SETTINGS, OnBnClickedSetSettings)
	ON_BN_CLICKED(IDC_UNDO, OnBnClickedUndo)
	ON_BN_CLICKED(IDC_GETANOMALY, OnBnClickedGetanomaly)
	ON_BN_CLICKED(IDC_DET_ANOMALY, OnBnClickedDetAnomaly)
	ON_BN_CLICKED(IDC_REMEDIATE, OnBnClickedRemediate)
	ON_BN_CLICKED(IDC_SCAN, OnBnClickedScan)	
	ON_BN_CLICKED(IDC_DETECT, OnBnClickedDetect)
	ON_BN_CLICKED(IDC_SAVEANOMALY, OnBnClickedSaveanomaly)
	ON_BN_CLICKED(IDC_SAVEREMEDIATION, OnBnClickedSaveremediation)
	ON_BN_CLICKED(IDC_SAVEDETECTION, OnBnClickedSavedetection)
	ON_BN_CLICKED(IDC_LOADOBJECT, OnBnClickedLoadobject)
	ON_BN_CLICKED(IDC_RELOAD, OnBnClickedReload)
	ON_BN_CLICKED(IDC_REMOVEANOMALY, OnBnClickedRemoveanomaly)
	ON_BN_CLICKED(IDC_REMOVEDETECT, OnBnClickedRemovedetect)
	ON_BN_CLICKED(IDC_REMOVEREMEDIATE, OnBnClickedRemoveremediate)
	ON_BN_CLICKED(IDC_LOCALIZE, OnBnClickedLocalize)
	ON_BN_CLICKED(IDC_CACHE, OnBnClickedCache)
	ON_BN_CLICKED(IDC_FULLSCAN, OnBnClickedFullscan)
	ON_BN_CLICKED(IDC_SAVEUNDO, OnBnClickedSaveundo)
    ON_WM_WININICHANGE()
	ON_BN_CLICKED(IDC_ABORT, OnBnClickedAbort)
	ON_BN_CLICKED(IDC_DET_ANOMALY_LIST, OnBnClickedDetAnomalyList)
	ON_BN_CLICKED(IDC_BUTTON_QUAR_ITEM_CREATE_WFILE, OnBnClickedButtonQuarItemCreateWfile)
	ON_BN_CLICKED(IDC_BUTTON_QUAR_ITEM_CREATE_WO_FILE, OnBnClickedButtonQuarItemCreateWithoutFile)
	ON_BN_CLICKED(IDC_BUTTON_VIRUS_FILE_CREATE, OnBnClickedButtonVirusFileCreate)
	ON_BN_CLICKED(IDC_BUTTON_SND_GET_REMEDIATION_DATA, OnBnClickedButtonSNDGetRemediationData)
	ON_BN_CLICKED(IDC_BUTTON_REMEDIATE_ADD_DATA, OnBnClickedButtonRemediateAddData)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_QUAR_ITEM, OnBnClickedButtonLoadQuarItem)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_QUAR_ITEM, OnBnClickedButtonScanQuarItem)
	ON_BN_CLICKED(IDC_BUTTON_REPAIR_QUAR_ITEM, OnBnClickedButtonRepairQuarItem)
	ON_BN_CLICKED(IDC_BUTTON_TAKE_SNAPSHOT, OnBnClickedButtonQuarDLLTakeSnapshot)
	ON_BN_CLICKED(IDC_BUTTON_ADD_SNAPSHOT, OnBnClickedButtonQuarDLLAddSnapshotToQuarItem)	
END_MESSAGE_MAP()

CString CSketchbookDlg::eResult2String(ccEraser::eResult eRes)
{
	CString sz;

    switch (eRes)
    {
    case ccEraser::Success:
        sz = _T("Success");
        break;
    case ccEraser::RebootRequired:
        sz = _T("Reboot Required");
        break;
	case ccEraser::NothingToDo:
		sz = _T("Nothing To Do");
		break;
	case ccEraser::Repaired:
        sz = _T("Repaired");
        break;
    case ccEraser::Deleted:
        sz = _T("Deleted");
        break;
    case ccEraser::False:
        sz = _T("False");
        break;
    case ccEraser::Abort:
        sz = _T("Abort");
        break;
    case ccEraser::Continue:
        sz = _T("Continue");
        break;
    case ccEraser::Fail:
        sz = _T("Fail");
        break;
    case ccEraser::OutOfMemory:
        sz = _T("Out Of Memory");
        break;
    case ccEraser::NotInitialized:
        sz = _T("Not Initialized");
        break;
    case ccEraser::InvalidArgument:
        sz = _T("Invalid Argument");
        break;
    case ccEraser::InsufficientBuffer:
        sz = _T("Insufficient Buffer");
        break;
    case ccEraser::DecryptionError:
        sz = _T("Decryption Error");
        break;
    case ccEraser::FileNotFound:
        sz = _T("File Not Found");
        break;
    case ccEraser::OutOfRange:
        sz = _T("Out Of Range");
        break;
    case ccEraser::COMError:
        sz = _T("COM Error");
        break;
    case ccEraser::PartialFailure:
        sz = _T("Partial Failure");
        break;
    case ccEraser::BadDefinitions:
        sz = _T("Bad Definitions");
        break;
    case ccEraser::InvalidCommand:
        sz = _T("Invalid Command");
        break;
    case ccEraser::NoInterface:
        sz = _T("No Interface");
        break;
    case ccEraser::RSAError:
        sz = _T("RSA Error");
        break;
    case ccEraser::PathNotEmpty:
        sz = _T("Path Not Empty");
        break;
    case ccEraser::InvalidPath:
        sz = _T("Invalid Path");
        break;
    case ccEraser::FileStillPresent:
        sz = _T("File Still Present");
        break;
	case ccEraser::InvalidOS:
        sz = _T("Invalid OS");
        break;
	case ccEraser::NotImplemented:
        sz = _T("Not Implemented");
        break;
	case ccEraser::AccessDenied:
		sz = _T("Access Denied");
		break;
	case ccEraser::ServiceNotStopped:
		sz = _T("Service Not Stopped");
		break;
    default:
        sz = _T("Unknown Error Code");
        break;
    }

	return sz;
}

void CSketchbookDlg::setResultWindow(ccEraser::eResult res, CString strError)
{
	CString strOut;

	if(Failed(res))
	{
		CCTRACEE(_T("%s : %u : %s"), eResult2String(res), res, strError);
	}
	else
	{
		CCTRACEI(_T("%s : %u : %s"), eResult2String(res), res, strError);
	}

	if(res != ccEraser::Success)
	{
		strOut.Format(_T("%s : %u : %s"), eResult2String(res), res, strError);

		CString strCur;
		m_editCtrlResult.GetWindowText(strCur);
		strCur += strOut;
		strCur += _T("\r\n");
		m_editCtrlResult.SetWindowText(strCur);
		m_editCtrlResult.LineScroll(m_editCtrlResult.GetLineCount());
	}
}


void CSketchbookDlg::setQuarantineWindow()
{
	TCHAR szGUID[MAX_PATH] = {0};
	DWORD dwBufSize = MAX_PATH;
	DWORD dwVirusID = 0;
	if(NULL == m_pQuarItem)
	{
		_tcscpy(szGUID, _T("NULL"));
	}
	else
	{
		if(FAILED(m_pQuarItem->GetString(QSERVER_ITEM_INFO_UUID, szGUID, &dwBufSize))){
			_tcscpy(szGUID, _T("Not found"));
			CCTRACEE(_T("Failed to Query for QSERVER_ITEM_INFO_UUID"));
		}

		if(FAILED(m_pQuarItem->GetVirusID(&dwVirusID)))
			CCTRACEI(_T("Not able to Query for VirusID"));
	}

	CString strOut;
	strOut.Format(_T("Current QuarantineItem: %s, VID: %d"), szGUID, dwVirusID);

	m_editCtrlQuarItem.SetWindowText(strOut);
}


void CSketchbookDlg::RefreshDetectionCtrl()
{
	if(m_pDetectionActionList == NULL)
	{
		return;
	}

	size_t dwCount;
	ccEraser::eResult res = m_pDetectionActionList->GetCount(dwCount);
	setResultWindow(res, _T("m_pDetectionActionList->GetCount(dwCount)"));
	if(Failed(res))
	{
		return;
	}

	m_listCtrlDetections.DeleteAllItems();

	for(size_t i = 0; i < dwCount; i++)
	{
		ccEraser::IDetectionActionPtr pDetectionAction;
		res = m_pDetectionActionList->GetItem(i, pDetectionAction);
		setResultWindow(res, _T("m_pDetectionActionList->GetItem(i, pDetectionAction)"));
		if(Failed(res))
		{
			continue;
		}

		// Used only for CC
		cc::IKeyValueCollectionPtr pData;
		res = pDetectionAction->GetUserData(pData);
		//setResultWindow(res, _T("res = pDetectionAction->GetUserData(pData)"));

		m_listCtrlDetections.InsertItem((int)i, _T(""));

		cc::IStringPtr pType;
		res = pDetectionAction->GetType(pType);
		setResultWindow(res, _T("pDetectionAction->GetType(pType)"));
		if(Failed(res))
		{
			continue;
		}

		m_listCtrlDetections.SetItemText((int)i, 0, ccSym::CStringConvert::GetStringT(pType));

		cc::IStringPtr pOperation;
		res = pDetectionAction->GetOperation(pOperation);
		setResultWindow(res, _T("pDetectionAction->GetOperation(pOperation)"));
		if(Failed(res))
		{
			continue;
		}

		m_listCtrlDetections.SetItemText((int)i, 1, ccSym::CStringConvert::GetStringT(pOperation));

		cc::IStringPtr pOperand;
		res = pDetectionAction->GetOperands(pOperand);
		setResultWindow(res, _T("pDetectionAction->GetOperands(pOperand)"));
		if(Failed(res))
		{
			continue;
		}

		m_listCtrlDetections.SetItemText((int)i, 2, ccSym::CStringConvert::GetStringT(pOperand));

		cc::IKeyValueCollectionPtr pProps;
		res = pDetectionAction->GetProperties(pProps);
		setResultWindow(res, _T("pDetectionAction->GetProperties(pProps)"));
		if(Failed(res))
		{
			continue;
		}

		DWORD dwState;
		if(pProps->GetValue(ccEraser::IDetectionAction::State, dwState))
		{
			if(dwState == ccEraser::IDetectionAction::UnknownDetectState)
			{
				m_listCtrlDetections.SetItemText((int)i, 3, _T("UnknownDetectState"));
			}
			else if(dwState == ccEraser::IDetectionAction::Detected)
			{
				m_listCtrlDetections.SetItemText((int)i, 3, _T("Detected"));
			}
			else if(dwState == ccEraser::IDetectionAction::NotDetected)
			{
				m_listCtrlDetections.SetItemText((int)i, 3, _T("NotDetected"));
			}
			else
			{
				m_listCtrlDetections.SetItemText((int)i, 3, _T("Error"));
			}
		}
	}
}

void CSketchbookDlg::RefreshRemediationCtrl()
{
	if(m_pRemediationActionList == NULL)
	{
		return;
	}

	size_t dwCount;
	ccEraser::eResult res = m_pRemediationActionList->GetCount(dwCount);
	setResultWindow(res, _T("m_pRemediationActionList->GetCount(dwCount)"));
	if(Failed(res))
	{
		return;
	}

	m_listCtrlRemediations.DeleteAllItems();

	for(size_t i = 0; i < dwCount; i++)
	{
		ccEraser::IRemediationActionPtr pRemediationAction;
		res = m_pRemediationActionList->GetItem(i, pRemediationAction);
		setResultWindow(res, _T("m_pRemediationActionList->GetItem(i, pRemediationAction)"));
		if(Failed(res))
		{
			continue;
		}

		m_listCtrlRemediations.InsertItem((int)i, _T(""));

		cc::IStringPtr pType;
		res = pRemediationAction->GetType(pType);
		setResultWindow(res, _T("pRemediationAction->GetType(pType)"));
		if(Failed(res))
		{
			continue;
		}

		m_listCtrlRemediations.SetItemText((int)i, 0,ccSym::CStringConvert::GetStringT(pType));

		cc::IStringPtr pOperation;
		res = pRemediationAction->GetOperation(pOperation);
		setResultWindow(res, _T("pRemediationAction->GetOperation(pOperation)"));
		if(Failed(res))
		{
			continue;
		}

		m_listCtrlRemediations.SetItemText((int)i, 1,ccSym::CStringConvert::GetStringT(pOperation));

		cc::IStringPtr pOperand;
		res = pRemediationAction->GetOperands(pOperand);
		setResultWindow(res, _T("pRemediationAction->GetOperands(pOperand)"));
		if(Failed(res))
		{
			continue;
		}

		m_listCtrlRemediations.SetItemText((int)i, 2,ccSym::CStringConvert::GetStringT(pOperand));

		cc::IKeyValueCollectionPtr pProps;
		res = pRemediationAction->GetProperties(pProps);
		setResultWindow(res, _T("pRemediationAction->GetProperties(pProps)"));
		if(Failed(res))
		{
			continue;
		}

		DWORD dwState;
		if(pProps->GetValue(ccEraser::IRemediationAction::State, dwState))
		{
			if(dwState == ccEraser::IRemediationAction::Remediated)
			{
				m_listCtrlRemediations.SetItemText((int)i, 3, _T("Remediated"));
			}
			else if(dwState == ccEraser::IRemediationAction::NotRemediated)
			{
				m_listCtrlRemediations.SetItemText((int)i, 3, _T("NotRemediated"));
			}
			else
			{
				m_listCtrlRemediations.SetItemText((int)i, 3, _T("Error"));
			}
		}
	}
}

void CSketchbookDlg::SetAnomalyCtrl()
{
	if(m_pAnomalyList == NULL)
	{
		return;
	}

	size_t dwSize;
	ccEraser::eResult res = m_pAnomalyList->GetCount(dwSize);
	setResultWindow(res, _T("m_pAnomalyList->GetCount(dwSize)"));
	if(Failed(res))
	{
		return;
	}

	m_listCtrlAnomaly.DeleteAllItems();
	m_listCtrlDetections.DeleteAllItems();
	m_listCtrlRemediations.DeleteAllItems();

	ccEraser::IAnomalyPtr pAnomaly;
	for(size_t i = 0; i < dwSize; i++)
	{
		res = m_pAnomalyList->GetItem(i, pAnomaly);
		setResultWindow(res, _T("m_pAnomalyList->GetItem(i, pAnomaly)"));
		if(Failed(res))
		{
			continue;
		}

		// Use only for CC
		const cc::IKeyValueCollection *  pConstProps = NULL;
		res = pAnomaly->GetProperties(pConstProps);
		setResultWindow(res, _T("pAnomaly->GetProperties(pConstProps)"));
		if(Failed(res))
		{
			return;
		}

		m_listCtrlAnomaly.InsertItem((int)i, _T(""));

		ISymBasePtr pBase;
		if(pConstProps->GetValue(ccEraser::IAnomaly::Name, pBase))
		{
			cc::IStringQIPtr pName = pBase;
			m_listCtrlAnomaly.SetItemText((int)i, 0,ccSym::CStringConvert::GetStringT(pName));
		}

		const_cast<cc::IKeyValueCollection*>(pConstProps)->Release();

		cc::IKeyValueCollectionPtr pProps;
		res = pAnomaly->GetProperties(pProps);
		setResultWindow(res, _T("pAnomaly->GetProperties(pProps)"));
		if(Failed(res))
		{
			return;
		}

		if(pProps->GetValue(ccEraser::IAnomaly::ID, pBase))
		{
			cc::IStringQIPtr pGUID = pBase;
			m_listCtrlAnomaly.SetItemText((int)i, 1, ccSym::CStringConvert::GetStringT(pGUID));
		}

		if(pProps->GetValue(ccEraser::IAnomaly::Categories, pBase))
		{
			cc::IIndexValueCollectionQIPtr pCategories = pBase;
			CString strCategories = _T("");

			for(size_t j = 0; j < pCategories->GetCount(); j++)
			{
				DWORD dwType;

				if(pCategories->GetValue(j, dwType))
				{
					switch(dwType)
					{
					case ccEraser::IAnomaly::Viral:
						strCategories += _T("Viral; ");
						break;
					case ccEraser::IAnomaly::Malicious:
						strCategories += _T("Malicious; ");
						break;
					case ccEraser::IAnomaly::ReservedMalicious:
						strCategories += _T("Malicious; ");
						break;
					case ccEraser::IAnomaly::Heuristic:
						strCategories += _T("Heuristic; ");
						break;
					case ccEraser::IAnomaly::SecurityRisk:
						strCategories += _T("SecurityRisk; ");
						break;
					case ccEraser::IAnomaly::Hacktool:
						strCategories += _T("Hacktool; ");
						break;
					case ccEraser::IAnomaly::SpyWare:
						strCategories += _T("SpyWare; ");
						break;
					case ccEraser::IAnomaly::Trackware:
						strCategories += _T("Trackware; ");
						break;
					case ccEraser::IAnomaly::Dialer:
						strCategories += _T("Dialer; ");
						break;
					case ccEraser::IAnomaly::RemoteAccess:
						strCategories += _T("RemoteAccess; ");
						break;
					case ccEraser::IAnomaly::Adware:
						strCategories += _T("Adware; ");
						break;
					case ccEraser::IAnomaly::Joke:
						strCategories += _T("Joke; ");
						break;
					case ccEraser::IAnomaly::ClientCompliancy:
						strCategories += _T("ClientCompliancy; ");
						break;
					case ccEraser::IAnomaly::GenericLoadPoint:
						strCategories += _T("GenericLoadPoint; ");
						break;
					default:
						strCategories += _T("Unknown Anomaly Category; ");
						break;
					}
				}
			}

			m_listCtrlAnomaly.SetItemText((int)i, 2, strCategories);
		}

		DWORD dwVID;
		if(pProps->GetValue(ccEraser::IAnomaly::VID, dwVID))
		{
			CString strVID;
			strVID.Format(_T("%u"), dwVID);
			m_listCtrlAnomaly.SetItemText((int)i, 3, strVID);
		}
		
		// Retrieve and display   Stealth value from the Anomaly if present
		bool bAutoCreated; 
		if(pProps->GetValue(ccEraser::IAnomaly::AutoCreated, bAutoCreated))
		{
			CString strAuto = bAutoCreated ? _T("TRUE") : _T("FALSE");
			m_listCtrlAnomaly.SetItemText((int)i, 4, strAuto);
		}

		// Retrieve and display   Stealth value from the Anomaly if present
		bool bHonorAVDelete; 
		if(pProps->GetValue(ccEraser::IAnomaly::HonorAVCanDelete, bHonorAVDelete))
		{
			CString strHonorAV = bHonorAVDelete ? _T("TRUE") : _T("FALSE");
			m_listCtrlAnomaly.SetItemText((int)i, 5, strHonorAV);
		}
			
		// Retrieve and display   Stealth value from the Anomaly if present
		DWORD dwStealth; 
		if(pProps->GetValue(ccEraser::IAnomaly::Stealth, dwStealth))
		{
			CString strStealth;
			strStealth.Format(_T("%d"), dwStealth);
			m_listCtrlAnomaly.SetItemText((int)i, 6, strStealth);
		}

		// Retrieve and display   Removal value from the Anomaly if present
		DWORD dwRemoval; 
		if(pProps->GetValue(ccEraser::IAnomaly::Removal, dwRemoval))
		{
			CString strRemoval;
			strRemoval.Format(_T("%d"), dwRemoval);
			m_listCtrlAnomaly.SetItemText((int)i, 7, strRemoval);
		}

		// Retrieve and display  Performance value from the Anomaly if present
		DWORD dwPerformance; 
		if(pProps->GetValue(ccEraser::IAnomaly::Performance, dwPerformance))
		{
			CString strPerformace;
			strPerformace.Format(_T("%d"), dwPerformance);
			m_listCtrlAnomaly.SetItemText((int)i, 8, strPerformace);
		}
		
		// Retrieve and display   Privalcy value from the Anomaly if present
		DWORD dwPrivacy; 
		if(pProps->GetValue(ccEraser::IAnomaly::Privacy, dwPrivacy))
		{
			CString strPrivacy;
			strPrivacy.Format(_T("%d"), dwPrivacy);
			m_listCtrlAnomaly.SetItemText((int)i, 9, strPrivacy);
		}
		
		// Retrieve and display   Dependency value from the Anomaly if present
		DWORD dwDependency; 
		if(pProps->GetValue(ccEraser::IAnomaly::Dependency, dwDependency))
		{
			CString strDependency;
			strDependency.Format(_T("%d"), dwDependency);
			m_listCtrlAnomaly.SetItemText((int)i, 10, strDependency);
		}
	}
}

CString CSketchbookDlg::GetTimeStampFile(CString strPath, CString strExtension)
{
	CString strOut;
	SYSTEMTIME time;
	::GetLocalTime(&time);
	strOut.Format(_T("%s\\%04d-%02d-%02d-%02d-%02d-%02d-%03d.%s"), strPath, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds++, strExtension);

	CFileStatus status;
	while( CFile::GetStatus(strOut, status) )
	{
		strOut.Format(_T("%s\\%04d-%02d-%02d-%02d-%02d-%02d-%03d.%s"), strPath, time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds++, strExtension);
	}

	return strOut;
}

// CSketchbookDlg message handlers

BOOL CSketchbookDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
/*
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
*/
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//Set the styles for the list control objects
	m_listCtrlAnomaly.SetExtendedStyle( LVS_EX_FULLROWSELECT | m_listCtrlAnomaly.GetExtendedStyle() );
	m_listCtrlDetections.SetExtendedStyle( LVS_EX_FULLROWSELECT | m_listCtrlDetections.GetExtendedStyle() );
	m_listCtrlRemediations.SetExtendedStyle( LVS_EX_FULLROWSELECT | m_listCtrlRemediations.GetExtendedStyle() );

	// Add columns to the lists.
	CRect rect;
	m_listCtrlAnomaly.GetClientRect( &rect );
	m_listCtrlAnomaly.InsertColumn( 0, _T("Name"), LVCFMT_LEFT, rect.Width() / 20 * 6 , 0 );
	m_listCtrlAnomaly.InsertColumn( 1, _T("GUID"), LVCFMT_LEFT, rect.Width() / 20 * 7 , 0 );
	m_listCtrlAnomaly.InsertColumn( 2, _T("Category"), LVCFMT_LEFT, rect.Width() / 20 * 5 , 0 );
	m_listCtrlAnomaly.InsertColumn( 3, _T("VID"), LVCFMT_LEFT, rect.Width() / 20 * 2, 0 );
	m_listCtrlAnomaly.InsertColumn( 4, _T("AutoCreated"), LVCFMT_LEFT, rect.Width() / 10 * 1 , 0 );
	m_listCtrlAnomaly.InsertColumn( 5, _T("HonorAVCanDelete"), LVCFMT_LEFT, rect.Width() / 10 * 1 , 0 );
	m_listCtrlAnomaly.InsertColumn( 6, _T("Stealth"), LVCFMT_LEFT, rect.Width() / 10 * 1 , 0 );
	m_listCtrlAnomaly.InsertColumn( 7, _T("Removal"), LVCFMT_LEFT, rect.Width() / 10 * 1 , 0 );
	m_listCtrlAnomaly.InsertColumn( 8, _T("Performance"), LVCFMT_LEFT, rect.Width() / 10 * 1 , 0 );
	m_listCtrlAnomaly.InsertColumn( 9, _T("Privacy"), LVCFMT_LEFT, rect.Width() / 10 * 1 , 0 );
	m_listCtrlAnomaly.InsertColumn( 10, _T("Dependency"), LVCFMT_LEFT, rect.Width() / 10 * 1 , 0 );

	m_listCtrlDetections.GetClientRect( &rect );
	m_listCtrlDetections.InsertColumn( 0, _T("Type"), LVCFMT_LEFT, rect.Width() / 20 * 2 , 0 );
	m_listCtrlDetections.InsertColumn( 1, _T("Operation"), LVCFMT_LEFT, rect.Width() / 20 * 2 , 0 );
	m_listCtrlDetections.InsertColumn( 2, _T("Operands"), LVCFMT_LEFT, rect.Width() / 10 * 7 , 0 );
	m_listCtrlDetections.InsertColumn( 3, _T("Status"), LVCFMT_LEFT, rect.Width() / 20 * 2 , 0 );


	m_listCtrlRemediations.GetClientRect( &rect );
	m_listCtrlRemediations.InsertColumn( 0, _T("Type"), LVCFMT_LEFT, rect.Width() / 20 * 2 , 0 );
	m_listCtrlRemediations.InsertColumn( 1, _T("Operation"), LVCFMT_LEFT, rect.Width() / 20 * 2 , 0 );
	m_listCtrlRemediations.InsertColumn( 2, _T("Operands"), LVCFMT_LEFT, rect.Width() / 10 * 7 , 0 );
	m_listCtrlRemediations.InsertColumn( 3, _T("Status"), LVCFMT_LEFT, rect.Width() / 20 * 2 , 0 );

	m_RepairOpt = ccEraser::IRemediationAction::RepairAndDelete;

	OnBnClickedReload();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSketchbookDlg::PostNcDestroy()
{
	CDialog::PostNcDestroy();
	CWnd * wndOwner = GetOwner();

	if(wndOwner != NULL)
	{
		wndOwner->PostMessage(WM_MODELESS_CLOSED, 0, (LPARAM)this);
		delete this;
	}
	}

void CSketchbookDlg::OnCancel()
	{
	DestroyWindow();
}

void CSketchbookDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


void CSketchbookDlg::OnBnClickedSetSettings()
{
	// TODO: Add your control notification handler code here

	cc::IKeyValueCollectionPtr pSettings;
	ccEraser::eResult res = m_pEraser->GetSettings(pSettings);
	setResultWindow(res, _T("m_pEraser->GetSettings(pSettings)"));
	if(Failed(res))
	{
		return;
	}

	SetSettings m_depthDlg(this);
	m_depthDlg.m_pDlg = this;

	if(!pSettings->GetValue(ccEraser::IEraser::InputFilter, m_depthDlg.m_pInputFilter))
	{
		// Comment out because Get/Set NULL value returnan an error.  But ccEraser can handle that internally.
	//	::AfxMessageBox(_T("Unable to get Input Filter settings"));
	//	return;
	}

	if(!pSettings->GetValue(ccEraser::IEraser::OutputFilter, m_depthDlg.m_pOutputFilter))
	{
		// Comment out because Get/Set NULL value returnan an error.  But ccEraser can handle that internally.
	//	::AfxMessageBox(_T("Unable to get Output Filter settings"));
	//	return;
	}

	if(!pSettings->GetValue(ccEraser::IEraser::Depth, m_depthDlg.m_dwSettingsDepth))
	{
		::AfxMessageBox(_T("Unable to get Depth settings"));
		return;
	}

	if(!pSettings->GetValue(ccEraser::IEraser::ScanDlls, m_depthDlg.m_bScanDll))
	{
		::AfxMessageBox(_T("Unable to get ScanDlls settings"));
		return;
	}
	
	DWORD Domain;
	if(!pSettings->GetValue(ccEraser::IEraser::ScanDomain, Domain))
	{
		::AfxMessageBox(_T("Unable to get ScanDomain settings"));
		return;
	}

	switch(Domain)
	{
	case ccEraser::LocalHost:
		m_depthDlg.m_dwDomain = 1;
		break;
	case ccEraser::LocalHostAndNetwork:
		m_depthDlg.m_dwDomain = 0;
		break;
	}
	
	switch(m_RepairOpt)
	{
	case ccEraser::IRemediationAction::RepairAndDelete:
		m_depthDlg.m_dwRepair = 0;
		break;
	case ccEraser::IRemediationAction::RepairOnly:
		m_depthDlg.m_dwRepair = 1;
		break;
	case ccEraser::IRemediationAction::DeleteOnly:
		m_depthDlg.m_dwRepair = 2;
		break;
	}
	
	//Display the set depth dialog box
	if(IDOK == m_depthDlg.DoModal())
	{
		if(m_depthDlg.m_pInputFilter && !pSettings->SetValue(ccEraser::IEraser::InputFilter, m_depthDlg.m_pInputFilter))
		{
			// Comment out because Get/Set NULL value returnan an error.  But ccEraser can handle that internally.
//			::AfxMessageBox(_T("Unable to set Input Filter settings"));
//			return;
		}

		if(m_depthDlg.m_pOutputFilter && !pSettings->SetValue(ccEraser::IEraser::OutputFilter, m_depthDlg.m_pOutputFilter))
		{
			// Comment out because Get/Set NULL value returnan an error.  But ccEraser can handle that internally.
//			::AfxMessageBox(_T("Unable to set Output Filter settings"));
//			return;
		}

		if(!pSettings->SetValue(ccEraser::IEraser::Depth, m_depthDlg.m_dwSettingsDepth))
		{
			::AfxMessageBox(_T("Unable to set Depth settings"));
			return;
		}

		if(!pSettings->SetValue(ccEraser::IEraser::ScanDlls, m_depthDlg.m_bScanDll))
		{
			::AfxMessageBox(_T("Unable to set ScanDlls settings"));
			return;
		}

		switch(m_depthDlg.m_dwDomain)
		{
		case 1:
			Domain = ccEraser::LocalHost;
			break;
		case 0:
			Domain = ccEraser::LocalHostAndNetwork;
			break;
		}

		switch(m_depthDlg.m_dwRepair)
		{
		case 0:
			m_RepairOpt = ccEraser::IRemediationAction::RepairAndDelete;
			break;
		case 1:
			m_RepairOpt = ccEraser::IRemediationAction::RepairOnly;
			break;
		case 2:
			m_RepairOpt = ccEraser::IRemediationAction::DeleteOnly;
			break;
		}

		if(!pSettings->SetValue(ccEraser::IEraser::ScanDomain, Domain))
		{
			::AfxMessageBox(_T("Unable to set ScanDomain settings"));
			return;
		}
	}
}

void CSketchbookDlg::OnBnClickedUndo()
{
	// TODO: Add your control notification handler code here
	
	CFileDialog dlgFile(TRUE, NULL, _T("*.undo"), OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR, _T("Undo List Files (*.undo)|*.undo||"), this);

	if (dlgFile.DoModal() == IDOK)
	{
		ccEraser::eResult res = m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pRemediationActionList);
		setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pRemediationActionList)"));
		if(Failed(res))
		{
			return;
		}

		CStdioFile file;
		if(file.Open(dlgFile.GetPathName(), CFile::modeRead | CFile::typeText))
		{
		CString strFile;
		
		while(file.ReadString(strFile))
		{
                ccSym::CFileStreamImplPtr pActionStream;
                pActionStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
			ccLib::CFile & ActionFile = pActionStream->GetFile();
			ActionFile.Open(strFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			ccEraser::eObjectType type;
			ISymBasePtr pBase;
				ccEraser::eResult res = m_pEraser->CreateObjectFromStream(pActionStream, &type, pBase);
				setResultWindow(res, _T("m_pEraser->CreateObjectFromStream(pActionStream, &type, pBase)"));
			if(Failed(res))
			{
				if(IDNO == ::AfxMessageBox(_T("Warning: Cannot create undo required object, undo result might be unstable, continue at your own risk, click yes to continue"), MB_YESNO))
				{
					ActionFile.Close();
					return;
				}
				else
				{
					ActionFile.Close();
					file.ReadString(strFile);
					continue;
				}
			}

			ActionFile.Close();

			ccEraser::IRemediationActionQIPtr pRemediationAction = pBase; 

			file.ReadString(strFile);

                ccSym::CFileStreamImplPtr pUndo;
                pUndo.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
			ccLib::CFile & UndoFile = pUndo->GetFile();
			UndoFile.Open(strFile, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

			if(pRemediationAction != NULL)
			{
				ccEraser::eObjectType type = ccEraser::FirstRemediationType;
				res = pRemediationAction->GetType(type);
				setResultWindow(res, _T("pRemediationAction->GetType(type)"));

					m_pRemediationActionList->Add(pRemediationAction);

					CUndoDlg Dlg(this);
				if(Succeeded(res) && (type == ccEraser::FileRemediationActionType || type == ccEraser::InfectionRemediationActionType))
				{
					Dlg.DoModal();

					if(Dlg.m_type == CUndoDlg::SaveTo)
					{
						cc::IStringPtr pDir;
                            pDir.Attach(ccSym::CStringImpl::CreateStringImpl(Dlg.strDir));
                     
						res = pRemediationAction->SaveUndoInformation(pDir, pUndo);
						cc::IStringPtr pDescription;
						ccEraser::eResult resDes = pRemediationAction->GetDescription(pDescription);
						if(Failed(resDes))
						{
							setResultWindow(res, _T("pRemediationAction->SaveUndoInformation(pDir, pUndo)"));
						}
						else
						{
							setResultWindow(res, CString(_T("SaveUndoTo ")) + ccSym::CStringConvert::GetStringT(pDescription));
						}

						if(Succeeded(res))
						{
							UndoFile.Close();
							continue;
						}
					}
					else if(Dlg.m_type == CUndoDlg::SaveAs)
					{
                            cc::IStringPtr pDir, pFile;
                            pDir.Attach(ccSym::CStringImpl::CreateStringImpl(Dlg.strDir));
                            pFile.Attach(ccSym::CStringImpl::CreateStringImpl(Dlg.strFile));
                             
						res = pRemediationAction->SaveUndoInformation(pDir, pFile, pUndo);
						cc::IStringPtr pDescription;
						ccEraser::eResult resDes = pRemediationAction->GetDescription(pDescription);
						if(Failed(resDes))
						{
							setResultWindow(res, _T("pRemediationAction->SaveUndoInformation(pDir, PFile, pUndo)"));
						}
						else
						{
							setResultWindow(res, CString(_T("SaveUndoAs ")) + ccSym::CStringConvert::GetStringT(pDescription));
						}

						if(Succeeded(res))
						{
							UndoFile.Close();
							continue;
						}
					}
				}

				res = pRemediationAction->Undo(pUndo);

				cc::IStringPtr pDescription;
				ccEraser::eResult resDes = pRemediationAction->GetDescription(pDescription);
				if(Failed(resDes))
				{
					setResultWindow(res, _T("pRemediationAction->Undo(pUndo)"));
				}
				else
				{
					setResultWindow(res, CString(_T("Undo ")) + ccSym::CStringConvert::GetStringT(pDescription));
				}				
			}

			UndoFile.Close();
		}

		file.Close();
	}
	}

	RefreshRemediationCtrl();
}

void CSketchbookDlg::OnBnClickedGetanomaly()
{
	// TODO: Add your control notification handler code here

	m_listCtrlAnomaly.DeleteAllItems();

	::AfxBeginThread(ThreadGetAnomaly, this);
}

void CSketchbookDlg::OnBnClickedScan()
{
	// TODO: Add your control notification handler code here

	m_editCtrlScan.SetWindowText(_T(""));

	::AfxBeginThread(ThreadScan, this);
}

void CSketchbookDlg::OnBnClickedDetAnomaly()
{
	// TODO: Add your control notification handler code here
	CDetectDialog m_detectDlg(this);
	if(IDOK == m_detectDlg.DoModal())
	{
        cc::IStringPtr pFilePath, pShortFilePath;
        pFilePath.Attach(ccSym::CStringImpl::CreateStringImpl(m_detectDlg.m_szLongPath));
        pShortFilePath.Attach(ccSym::CStringImpl::CreateStringImpl(m_detectDlg.m_szShortPath));

        ccEraser::eResult res = m_pEraser->DetectAnomalies(pFilePath, pShortFilePath, m_detectDlg.m_dwVID, m_pAnomalyList);
		setResultWindow(res, _T("m_pEraser->DetectAnomalies(pFilePath, pShortFilePath, m_detectDlg.m_dwVID, m_pAnomalyList)"));
		if(Failed(res))
		{
			return;
		}

		SetAnomalyCtrl();
	}
}

void CSketchbookDlg::OnBnClickedRemediate()
{
	if(m_pRemediationActionList == NULL)
	{
		return;
	}

	CString strFiles = _T("");

	TCHAR szPath[MAX_PATH];
	::GetModuleFileName(NULL, szPath, MAX_PATH);
	_tcscpy(_tcsrchr(szPath, _T('\\')) + 1, _T("undo"));
	::CreateDirectory(szPath, NULL);

	for(int i = 0 ; i < m_listCtrlRemediations.GetItemCount(); i++)
	{
		if(m_listCtrlRemediations.GetSelectedCount() == 0 || LVIS_SELECTED == m_listCtrlRemediations.GetItemState(i, LVIS_SELECTED))
		{
			ccEraser::IRemediationActionPtr pRemediationAction;
			ccEraser::eResult res = m_pRemediationActionList->GetItem(i, pRemediationAction);
			setResultWindow(res, _T("m_pRemediationActionList->GetItem(i, pRemediationAction)"));
			if(Failed(res))
			{
				return;
			}

			ccEraser::eObjectType type = ccEraser::FirstRemediationType;
			res = pRemediationAction->GetType(type);
			setResultWindow(res, _T("pRemediationAction->GetType(type)"));

			if(type == ccEraser::InfectionRemediationActionType)
			{
				cc::IKeyValueCollectionPtr pProps;
				res = pRemediationAction->GetProperties(pProps);
				setResultWindow(res, _T("pRemediationAction->GetProperties(pProps)"));
				pProps->SetValue(ccEraser::IRemediationAction::HandleThreatOperation, (DWORD)m_RepairOpt);
			}

			bool bUndo = false;

			CString strUndo;
			CString strAction;

			if(m_bUndo)
			{
				res = pRemediationAction->SupportsUndo(bUndo);
				setResultWindow(res, _T("pRemediationAction->SupportsUndo(bUndo)"));
			}

			if(bUndo)
			{
				strUndo = GetTimeStampFile(szPath, _T("data"));
                ccSym::CFileStreamImplPtr pUndo;
                pUndo.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
				ccLib::CFile & UndoFile = pUndo->GetFile();
				UndoFile.Open(strUndo, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
				res = pRemediationAction->GetUndoInformation(pUndo);

				cc::IStringPtr pDescription;
				ccEraser::eResult resDes = pRemediationAction->GetDescription(pDescription);
				if(Failed(resDes))
				{
					setResultWindow(res, _T("pRemediationAction->GetUndoInformation(pUndo)"));
				}
				else
				{
					setResultWindow(res, CString(_T("GetUndo ")) + ccSym::CStringConvert::GetStringT(pDescription));
				}

				UndoFile.Close();

				if(Failed(res))
				{
					::DeleteFile(strUndo);
					bUndo = false;
				}
			}

			res = pRemediationAction->Remediate();

			cc::IStringPtr pDescription;
			ccEraser::eResult resDes = pRemediationAction->GetDescription(pDescription);
			if(Failed(resDes))
			{
				setResultWindow(res, _T("pRemediationAction->Remediate()"));
			}
			else
			{
				setResultWindow(res, CString(_T("Remediate ")) + ccSym::CStringConvert::GetStringT(pDescription));
			}

			if(Failed(res))
			{
				::DeleteFile(strUndo);
			}
			else if(bUndo)
			{
				strAction = GetTimeStampFile(szPath, _T("data"));
                ccSym::CFileStreamImplPtr pActionStream;
                pActionStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
				ccLib::CFile & ActionFile = pActionStream->GetFile();
				ActionFile.Open(strAction, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

				cc::ISerializeQIPtr pSerialize = pRemediationAction;
				pSerialize->Save(pActionStream);
				ActionFile.Close();

				// Used only for CC
				bool bDirty = pSerialize->GetDirty();
				pSerialize->SetDirty(bDirty);

				CString strPath = strAction;
				strPath += _T("\n");
				strPath += strUndo;

				strFiles.Insert(0, strPath + _T("\n"));
			}
			else
			{
				::DeleteFile(strUndo);
			}
		}
	}

	if(strFiles != _T(""))
	{
		TCHAR szPath[MAX_PATH];
		CString strName;
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		_tcscpy(_tcsrchr(szPath, _T('\\')) + 1, _T(""));
		strName = GetTimeStampFile(szPath, _T("undo"));

		CStdioFile file;
		file.Open(strName, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
		file.WriteString(strFiles);
		file.Close();
	}

	RefreshDetectionCtrl();
	RefreshRemediationCtrl();
}

void CSketchbookDlg::OnLvnItemchangedListAnomaly(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if(pNMLV->uNewState == 0)
	{
		return;
	}

	if(m_listCtrlAnomaly.GetSelectedCount() > 1)
	{
		return;
	}

	ccEraser::IAnomalyPtr pAnomaly;

	if(m_bCache)
	{
		ccEraser::eResult res = m_pAnomalyList->GetItem(pNMLV->iItem, pAnomaly);
		setResultWindow(res, _T("m_pAnomalyList->GetItem(pNMLV->iItem, pAnomaly)"));
		if(Failed(res))
		{
			return;
		}
	}
	else
	{
		GUID clsid;
		CString strGUID = m_listCtrlAnomaly.GetItemText(pNMLV->iItem, 1);
		strGUID.Insert(0, _T('{'));
		strGUID.AppendChar(_T('}'));

		CT2W szGUID(strGUID.GetBuffer());
		if(NOERROR == CLSIDFromString(szGUID, &clsid))
		{

			ccEraser::eResult res = m_pEraser->GetAnomaly((REFSYMOBJECT_ID)clsid, pAnomaly);
			setResultWindow(res, _T("m_pEraser->GetAnomaly((REFSYMOBJECT_ID)clsid, pAnomaly)"));
			if(Failed(res))
			{
				return;
			}
		}
	}

	ccEraser::eResult res = pAnomaly->GetDetectionActions(m_pDetectionActionList);
	setResultWindow(res, _T("pAnomaly->GetDetectionActions(m_pDetectionActionList)"));
	if(Failed(res))
	{
		return;
	}

	res = pAnomaly->GetRemediationActions(m_pRemediationActionList);
	setResultWindow(res, _T("pAnomaly->GetRemediationActions(m_pRemediationActionList)"));
	if(Failed(res))
	{
		return;
	}

	res = m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pDynamicRemediationActionList);
	setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pDynamicRemediationActionList)"));
	if(Failed(res))
	{
		return;
	}

	RefreshDetectionCtrl();
	RefreshRemediationCtrl();
}

void CSketchbookDlg::OnBnClickedDetect()
{
	// TODO: Add your control notification handler code here

	if(m_pDetectionActionList == NULL)
	{
		return;
	}

	for(int i = 0 ; i < m_listCtrlDetections.GetItemCount(); i++)
	{
		if(m_listCtrlDetections.GetSelectedCount() == 0 || LVIS_SELECTED == m_listCtrlDetections.GetItemState(i, LVIS_SELECTED))
		{
			ccEraser::IDetectionActionPtr pDetectionAction;
			ccEraser::eResult res = m_pDetectionActionList->GetItem(i, pDetectionAction);
			setResultWindow(res, _T("m_pDetectionActionList->GetItem(i, pDetectionAction)"));
			if(Failed(res))
			{
				continue;
			}

			ccEraser::IRemediationActionListPtr pRemediationActionList;

			res = pDetectionAction->Detect(pRemediationActionList);

			cc::IStringPtr pDescription;
			ccEraser::eResult resDes = pDetectionAction->GetDescription(pDescription);
			if(Failed(resDes))
			{
				setResultWindow(res, _T("pDetectionAction->Detect(pRemediationActionList)"));
			}
			else
			{
				setResultWindow(res, CString(_T("Detect ")) + ccSym::CStringConvert::GetStringT(pDescription));
			}
			
			if(Failed(res))
			{
				continue;
			}

			res = m_pDynamicRemediationActionList->Merge(pRemediationActionList);
			setResultWindow(res, _T("m_pRemediationActionList->Merge(pRemediationActionList)"));
			if(Failed(res))
			{
				continue;
			}

			res = m_pRemediationActionList->Merge(pRemediationActionList);
			setResultWindow(res, _T("m_pRemediationActionList->Merge(pRemediationActionList)"));
			if(Failed(res))
			{
				continue;
			}
		}
	}

	RefreshDetectionCtrl();
	RefreshRemediationCtrl();

	//m_pDynamicRemediationActionList = NULL;
}

void CSketchbookDlg::OnBnClickedSaveanomaly()
{
	// TODO: Add your control notification handler code here

	if(m_listCtrlAnomaly.GetItemCount() == 0)
	{
		return;
	}

	cc::ISerializeQIPtr pSerialize;

	if(m_listCtrlAnomaly.GetSelectedCount() == 1)
	{
		ccEraser::IAnomalyPtr pAnomaly;
		POSITION pos = m_listCtrlAnomaly.GetFirstSelectedItemPosition();

		ccEraser::eResult res = m_pAnomalyList->GetItem(m_listCtrlAnomaly.GetNextSelectedItem(pos), pAnomaly);
		setResultWindow(res, _T("m_pAnomalyList->GetItem(m_listCtrlAnomaly.GetNextSelectedItem(pos), pAnomaly)"));
		if(Failed(res))
		{
			return;
		}

		pSerialize = pAnomaly;
	}
	else
	{
		ccEraser::IAnomalyListPtr pAnomalyList;
		ccEraser::eResult res = m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &pAnomalyList);
		setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &pAnomalyList)"));
		if(Failed(res))
		{
			return;
		}

		for(int i = 0 ; i < m_listCtrlAnomaly.GetItemCount(); i++)
		{
			if(m_listCtrlAnomaly.GetSelectedCount() == 0 || LVIS_SELECTED == m_listCtrlAnomaly.GetItemState(i, LVIS_SELECTED))
			{

				ccEraser::IAnomalyPtr pAnomaly;
				ccEraser::eResult res = m_pAnomalyList->GetItem(i, pAnomaly);
				setResultWindow(res, _T("m_pAnomalyList->GetItem(i, pAnomaly)"));
				if(Failed(res))
				{
					return;
				}

				res = pAnomalyList->Add(pAnomaly);
			}
		}

		pSerialize = pAnomalyList;
	}

	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, NULL, this);

	if(FileDlg.DoModal() == IDOK)
	{
        ccSym::CFileStreamImplPtr pStream;
        pStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
		ccLib::CFile & File = pStream->GetFile();
		File.Open(FileDlg.GetPathName(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		pSerialize->Save(pStream);
		File.Close();
	}
}

void CSketchbookDlg::OnBnClickedSaveremediation()
{
	// TODO: Add your control notification handler code here

	if(m_listCtrlRemediations.GetItemCount() == 0)
	{
		return;
	}

	cc::ISerializeQIPtr pSerialize;

	if(m_listCtrlRemediations.GetSelectedCount() == 1)
	{
		POSITION pos = m_listCtrlRemediations.GetFirstSelectedItemPosition();
		ccEraser::IRemediationActionPtr pRemediationAction;
		ccEraser::eResult res = m_pRemediationActionList->GetItem(m_listCtrlRemediations.GetNextSelectedItem(pos), pRemediationAction);
		setResultWindow(res, _T("m_pRemediationActionList->GetItem(i, pRemediationAction)"));
		if(Failed(res))
		{
			return;
		}

		pSerialize = pRemediationAction;
	}
	else
	{
		ccEraser::IRemediationActionListPtr pRemediationActionList;
		ccEraser::eResult res = m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &pRemediationActionList);
		setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &pRemediationActionList)"));
		if(Failed(res))
		{
			return;
		}

		for(int i = 0 ; i < m_listCtrlRemediations.GetItemCount(); i++)
		{
			if(m_listCtrlRemediations.GetSelectedCount() == 0 || LVIS_SELECTED == m_listCtrlRemediations.GetItemState(i, LVIS_SELECTED))
			{
				ccEraser::IRemediationActionPtr pRemediationAction;
				ccEraser::eResult res = m_pRemediationActionList->GetItem(i, pRemediationAction);
				setResultWindow(res, _T("m_pRemediationActionList->GetItem(i, pRemediationAction)"));
				if(Failed(res))
				{
					return;
				}

				res = pRemediationActionList->Add(pRemediationAction);
				setResultWindow(res, _T("pRemediationActionList->Add(pRemediationAction)"));
				if(Failed(res))
				{
					return;
				}
			}
		}

		pSerialize = pRemediationActionList;
	}

	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, NULL, this);

	if(FileDlg.DoModal() == IDOK)
	{
		ccSym::CFileStreamImplPtr pStream;
        pStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
		ccLib::CFile & File = pStream->GetFile();
		File.Open(FileDlg.GetPathName(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		pSerialize->Save(pStream);
		File.Close();
	}
}

void CSketchbookDlg::OnBnClickedSavedetection()
{
	// TODO: Add your control notification handler code here

	if(m_listCtrlDetections.GetItemCount() == 0)
	{
		return;
	}

	cc::ISerializeQIPtr pSerialize;

	if(m_listCtrlDetections.GetSelectedCount() == 1)
	{
		POSITION pos = m_listCtrlDetections.GetFirstSelectedItemPosition();
		ccEraser::IDetectionActionPtr pDetectionAction;
		ccEraser::eResult res = m_pDetectionActionList->GetItem(m_listCtrlDetections.GetNextSelectedItem(pos), pDetectionAction);
		setResultWindow(res, _T("m_pDetectionActionList->GetItem(m_listCtrlDetections.GetNextSelectedItem(pos), pDetectionAction)"));
		if(Failed(res))
		{
			return;
		}

		pSerialize = pDetectionAction;
	}
	else
	{
		ccEraser::IDetectionActionListPtr pDetectionActionList;
		ccEraser::eResult res = m_pEraser->CreateObject(ccEraser::DetectionActionListType, ccEraser::IID_DetectionActionList, (void **) &pDetectionActionList);
		setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::DetectionActionListType, ccEraser::IID_DetectionActionList, (void **) &pDetectionActionList)"));
		if(Failed(res))
		{
			return;
		}

		for(int i = 0 ; i < m_listCtrlDetections.GetItemCount(); i++)
		{
			if(m_listCtrlDetections.GetSelectedCount() == 0 || LVIS_SELECTED == m_listCtrlDetections.GetItemState(i, LVIS_SELECTED))
			{
				ccEraser::IDetectionActionPtr pDetectionAction;
				res = m_pDetectionActionList->GetItem(i, pDetectionAction);
				setResultWindow(res, _T("m_pDetectionActionList->GetItem(i, pDetectionAction)"));
				if(Failed(res))
				{
					continue;
				}

				res = pDetectionActionList->Add(pDetectionAction);
				setResultWindow(res, _T("pDetectionActionList->Add(pDetectionAction)"));
				if(Failed(res))
				{
					return;
				}
			}
		}

		pSerialize = pDetectionActionList;
	}

	CFileDialog FileDlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, NULL, this);

	if(FileDlg.DoModal() == IDOK)
	{
		ccSym::CFileStreamImplPtr pStream;
        pStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
		ccLib::CFile & File = pStream->GetFile();
		File.Open(FileDlg.GetPathName(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		pSerialize->Save(pStream);
		File.Close();
	}
}

void CSketchbookDlg::OnBnClickedLoadobject()
{
	// TODO: Add your control notification handler code here

	CFileDialog FileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_NOCHANGEDIR, NULL, this);

	if(FileDlg.DoModal() != IDOK)
	{
		return;
	}

	m_listCtrlDetections.DeleteAllItems();
	m_listCtrlRemediations.DeleteAllItems();
	m_listCtrlAnomaly.DeleteAllItems();

	ccSym::CFileStreamImplPtr pStream;
    pStream.Attach(ccSym::CFileStreamImpl::CreateFileStreamImpl());
	ccLib::CFile & File = pStream->GetFile();
	File.Open(FileDlg.GetPathName(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	ccEraser::eObjectType type;
	ISymBasePtr pBase;
	ccEraser::eResult res = m_pEraser->CreateObjectFromStream(pStream, &type, pBase);
	setResultWindow(res, _T("m_pEraser->CreateObjectFromStream(pStream, &type, pBase)"));	
	if(Failed(res))
	{
		return;
	}

	File.Close();

	if(type == ccEraser::RemediationActionListType)
	{
		ccEraser::IRemediationActionListQIPtr pRemediationActionList = pBase;
		m_pRemediationActionList = pRemediationActionList;

		RefreshRemediationCtrl();
	}
	else if(type == ccEraser::DetectionActionListType)
	{
		ccEraser::IDetectionActionListQIPtr pDetectionActionList = pBase;
		m_pDetectionActionList = pDetectionActionList;

		RefreshDetectionCtrl();
	}
	else if(type == ccEraser::AnomalyListType)
	{
		ccEraser::IAnomalyListQIPtr pAnomalyList = pBase;
		m_pAnomalyList = pAnomalyList;

		SetAnomalyCtrl();
	}
	else if(type == ccEraser::AnomalyType)
	{
		ccEraser::IAnomalyQIPtr pAnomaly = pBase;

		ccEraser::eResult res = m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &m_pAnomalyList);
		setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &m_pAnomalyList)"));
		if(Failed(res))
		{
			return;
		}

		res = m_pAnomalyList->Add(pAnomaly);
		setResultWindow(res, _T("m_pAnomalyList->Add(pAnomaly)"));
		if(Failed(res))
		{
			return;
		}

		SetAnomalyCtrl();
	}
	else if(type >= ccEraser::FirstDetectionType && type <= ccEraser::LastDetectionType)
	{
		ccEraser::IDetectionActionQIPtr pDetection = pBase;

		ccEraser::eResult res = m_pEraser->CreateObject(ccEraser::DetectionActionListType, ccEraser::IID_DetectionActionList, (void **) &m_pDetectionActionList);
		setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::DetectionActionListType, ccEraser::IID_DetectionActionList, (void **) &m_pDetectionActionList)"));
		if(Failed(res))
		{
			return;
		}

		res = m_pDetectionActionList->Add(pDetection);
		setResultWindow(res, _T("m_pDetectionActionList->Add(pDetection)"));
		if(Failed(res))
		{
			return;
		}

		RefreshDetectionCtrl();
	}
	else if(type >= ccEraser::FirstRemediationType && type <= ccEraser::LastRemediationType)
	{
		ccEraser::IRemediationActionQIPtr pRemediation = pBase;

		ccEraser::eResult res = m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pRemediationActionList);
		setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pRemediationActionList)"));
		if(Failed(res))
		{
			return;
		}

		res = m_pRemediationActionList->Add(pRemediation);
		setResultWindow(res, _T("m_pRemediationActionList->Add(pRemediation)"));
		if(Failed(res))
		{
			return;
		}

		RefreshRemediationCtrl();
	}

}

void CSketchbookDlg::OnBnClickedReload()
{
	// TODO: Add your control notification handler code here	

	m_pEraser = NULL;

	m_listCtrlAnomaly.DeleteAllItems();
	m_listCtrlDetections.DeleteAllItems();
	m_listCtrlRemediations.DeleteAllItems();
	m_editCtrlResult.SetWindowText(_T(""));

	if(SYM_FAILED(theApp.m_ccEraserLoader.CreateObject(GETMODULEMGR(), m_pEraser)))
    {
		::AfxMessageBox(_T("FAILED to Create EraserLoader Object"));
		CCTRACEE(_T("StartEraser():FAILED to Create EraserLoader Object"));
		return;
	}

	//Initialize the ccEraser engine
	m_pScanner = NULL;
	m_pSink = NULL;
	m_pProperties = NULL;
	m_Scanner.GetScanner(m_pScanner);
	m_Scanner.GetScanSink(m_pSink);
	m_Scanner.GetScanProperties(m_pProperties);

	ccEraser::eResult res;

	if(m_bLocalized)
	{
		res = m_pEraser->Initialize(m_pScanner, m_pSink, m_pProperties);
	}
	else
	{
		res = m_pEraser->Initialize(m_pScanner, m_pSink, m_pProperties, GetSystemDefaultLangID());
	}

	setResultWindow(res, _T("m_pEraser->Initialize(pScanner, pSink, pProperties)"));
	if(Failed(res))
	{
		::AfxMessageBox(_T("Failed to initalize ccEraser"));
		return;
	}

	DWORD dwEngineMost;
	DWORD dwEngineLeast;
	res = m_pEraser->GetEngineVersion(dwEngineMost, dwEngineLeast);
	setResultWindow(res, _T("m_pEraser->GetEngineVersion(dwEngineMost, dwEngineLeast)"));
	if(Failed(res))
	{
		return;
	}

	DWORD dwDataMost;
	DWORD dwDataLeast;
	res = m_pEraser->GetDataVersion(dwDataMost, dwDataLeast);
	CString strVersion;
	setResultWindow(res, _T("m_pEraser->GetDataVersion(dwDataMost, dwDataLeast)"));
	if(Failed(res))
	{
		return;
	}

	strVersion.Format(_T("Engine %u.%u Data %u.%u"), dwEngineMost, dwEngineLeast, dwDataMost, dwDataLeast);
	m_editCtrlScan.SetWindowText(strVersion);

	res = m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &m_pAnomalyList);
	setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::AnomalyListType, ccEraser::IID_AnomalyList, (void **) &m_pAnomalyList)"));
	if(Failed(res))
	{
		return;
	}

	res = m_pEraser->CreateObject(ccEraser::DetectionActionListType, ccEraser::IID_DetectionActionList, (void **) &m_pDetectionActionList);
	setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::DetectionActionListType, ccEraser::IID_DetectionActionList, (void **) &m_pDetectionActionList)"));
	if(Failed(res))
	{
		return;
	}

	res = m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pRemediationActionList);
	setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pRemediationActionList)"));
	if(Failed(res))
	{
		return;
	}

	res = m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pDynamicRemediationActionList);
	setResultWindow(res, _T("m_pEraser->CreateObject(ccEraser::RemediationActionListType, ccEraser::IID_RemediationActionList, (void **) &m_pDynamicRemediationActionList)"));
	if(Failed(res))
	{
		return;
	}
}

void CSketchbookDlg::OnBnClickedRemoveanomaly()
{
	// TODO: Add your control notification handler code here

	if(m_pAnomalyList == NULL)
	{
		return;
	}

	if(m_listCtrlAnomaly.GetSelectedCount() != 1)
	{
		::AfxMessageBox(_T("Need to select one and only one item"));
		return;
	}

	POSITION pos = m_listCtrlAnomaly.GetFirstSelectedItemPosition();
	ccEraser::eResult res = m_pAnomalyList->Remove(m_listCtrlAnomaly.GetNextSelectedItem(pos));
	setResultWindow(res, _T("m_pAnomalyList->Remove(m_listCtrlAnomaly.GetNextSelectedItem(pos))"));
	if(Failed(res))
	{
		return;
	}

	m_pDetectionActionList = NULL;
	m_pRemediationActionList = NULL;

	SetAnomalyCtrl();
}

void CSketchbookDlg::OnBnClickedRemovedetect()
{
	// TODO: Add your control notification handler code here

	if(m_pDetectionActionList == NULL)
	{
		return;
	}

	if(m_listCtrlDetections.GetSelectedCount() != 1)
	{
		::AfxMessageBox(_T("Need to select one and only one item"));
		return;
	}

	POSITION pos = m_listCtrlDetections.GetFirstSelectedItemPosition();
	ccEraser::eResult res = m_pDetectionActionList->Remove(m_listCtrlDetections.GetNextSelectedItem(pos));
	setResultWindow(res, _T("m_pDetectionActionList->Remove(m_listCtrlDetections.GetNextSelectedItem(pos))"));
	if(Failed(res))
	{
		return;
	}

	RefreshDetectionCtrl();
}

void CSketchbookDlg::OnBnClickedRemoveremediate()
{
	// TODO: Add your control notification handler code here

	if(m_pRemediationActionList == NULL)
	{
		return;
	}

	if(m_listCtrlRemediations.GetSelectedCount() != 1)
	{
		::AfxMessageBox(_T("Need to select one and only one item"));
		return;
	}

	POSITION pos = m_listCtrlRemediations.GetFirstSelectedItemPosition();
	ccEraser::eResult res = m_pRemediationActionList->Remove(m_listCtrlRemediations.GetNextSelectedItem(pos));
	setResultWindow(res, _T("m_pRemediationActionList->Remove(m_listCtrlRemediations.GetNextSelectedItem(pos))"));
	if(Failed(res))
	{
		return;
	}

	RefreshRemediationCtrl();
}

void CSketchbookDlg::OnBnClickedLocalize()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSketchbookDlg::OnBnClickedCache()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSketchbookDlg::OnNMCustomdrawListRemediation(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pNMLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: Add your control notification handler code here

	if(m_pDynamicRemediationActionList == NULL)
	{
		*pResult = 1;
		return;
	}

	if(pNMLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
		return;
	}

	if(pNMLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
	{
		*pResult = CDRF_NEWFONT;

		ccEraser::IRemediationActionPtr pRemediationAction;
		ccEraser::eResult res = m_pRemediationActionList->GetItem(pNMLVCD->nmcd.dwItemSpec, pRemediationAction);
		setResultWindow(res, _T("m_pRemediationActionList->GetItem(pNMLVCD->nmcd.dwItemSpec, pRemediationAction)"));
		if(Failed(res))
		{
			return;
		}

		size_t dwCount;
		res = m_pDynamicRemediationActionList->GetCount(dwCount);
		setResultWindow(res, _T("m_pDynamicRemediationActionList->GetCount(dwCount)"));
		if(Failed(res))
		{
			return;
		}

		pNMLVCD->clrText = RGB(0, 0, 0);

		for(size_t i = 0; i < dwCount; i++)
		{
			ccEraser::IRemediationActionPtr pRemediationAction2;
			ccEraser::eResult res = m_pDynamicRemediationActionList->GetItem(i, pRemediationAction2);
			setResultWindow(res, _T("m_pDynamicRemediationActionList->GetItem(i, pRemediationAction2)"));
			if(Failed(res))
			{
				return;
			}

			if(pRemediationAction2 == pRemediationAction)
			{
				pNMLVCD->clrText = RGB(255, 0, 0);
				break;
			}
		}

		//display grey backgraound for remediation actions that are not present on the system
		bool bPresent = true;
		res = pRemediationAction->IsPresent(bPresent);
		//get remediation action state 
		cc::IKeyValueCollectionPtr pProps;
		pRemediationAction->GetProperties(pProps);
		DWORD dwState;
		if(!pProps->GetValue(ccEraser::IRemediationAction::State, dwState))
		{
			return;
		}

		if(Succeeded(res) && !bPresent && (dwState != ccEraser::IRemediationAction::Remediated))
		{
			pNMLVCD->clrTextBk=RGB(220, 220, 220);
			
		}
		if (res!=ccEraser::NotImplemented)
			setResultWindow(res, _T("_pRemediationAction->IsPresent(bPresent)"));
		return;
	}

	*pResult = 0;
	return;
}

void CSketchbookDlg::OnBnClickedFullscan()
{
	// TODO: Add your control notification handler code here

	::AfxBeginThread(ThreadFullScan, this);
}

void CSketchbookDlg::OnBnClickedSaveundo()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
}

void CSketchbookDlg::OnWinIniChange(LPCTSTR lpszSection)
{
    CDialog::OnWinIniChange(lpszSection);

    if (lpszSection &&
        lstrcmpi(lpszSection, _T("Environment")) == 0)
    {
        // Defect 1-38DJWL:
        // The user changed the environment using the "System" control panel.
        // Rebuild our environment -- in order to pick up changes
        // to the PATH in particular -- so that the user doesn't have
        // to re-open Sketchbook.

        // Undocumented -- don't use this in production code.
        PVOID pEnv = NULL;
        RegenerateUserEnvironment(&pEnv, TRUE);
    }
}

void CSketchbookDlg::OnBnClickedAbort()
{
	// TODO: Add your control notification handler code here

	if(m_bGetAnomaly)
	{
		ccEraser::eResult res = m_pEraser->RequestAbort();
		setResultWindow(res, _T("m_pEraser->RequestAbort()"));
		if(Failed(res))
		{
			return;
		}
	}
	else
	{
		m_bScanAbort = TRUE;
	}
}

void CSketchbookDlg::OnBnClickedDetAnomalyList()
{
	// TODO: Add your control notification handler code here

	CDetectListDialog m_detectDlg(this);
	if(IDOK == m_detectDlg.DoModal())
	{
		cc::IIndexValueCollectionPtr pCollection;
		pCollection.Attach(ccSym::CIndexValueCollectionImpl::CreateIndexValueCollectionImpl());
		
		for(POSITION pos = m_detectDlg.m_stringListFile.GetHeadPosition(); pos != NULL;)
		{
			cc::IStringPtr pFilePath;
			pFilePath.Attach(ccSym::CStringImpl::CreateStringImpl(m_detectDlg.m_stringListFile.GetNext(pos)));
			pCollection->AddValue(pFilePath);
		}

		ccEraser::eResult res = m_pEraser->DetectAnomalies(pCollection, m_detectDlg.m_dwVID, m_pAnomalyList);
		setResultWindow(res, _T("m_pEraser->DetectAnomalies(pCollection, m_detectDlg.m_dwVID, m_pAnomalyList)"));
		if(Failed(res))
		{
			return;
		}

		SetAnomalyCtrl();
	}
}


void CSketchbookDlg::OnBnClickedButtonQuarItemCreateWfile()
{
	if(m_pRemediationActionList == NULL)
	{
		return;
	}

	if(m_listCtrlRemediations.GetSelectedCount() != 1)
	{
		::MessageBox(NULL, "Pick 1 file based item", "Wrong number of items selected", MB_OK);
		return;
	}

	for(int i = 0 ; i < m_listCtrlRemediations.GetItemCount(); i++)
	{
		if(LVIS_SELECTED == m_listCtrlRemediations.GetItemState(i, LVIS_SELECTED))
		{
			ccEraser::IRemediationActionPtr pRemediationAction;
			ccEraser::eResult res = m_pRemediationActionList->GetItem(i, pRemediationAction);
			setResultWindow(res, _T("m_pRemediationActionList->GetItem(i, pRemediationAction)"));
			if(Failed(res))
			{
				return;
			}

			// if a file, put in QuarItem
			cc::IKeyValueCollectionPtr props;

			// Get the properties for the two actions
			if(ccEraser::Failed(pRemediationAction->GetProperties(props)))
			{
				// Error getting properties
				break;
			}

			IScanInfectionPtr pScanInfection;
			if(!props->GetValue(ccEraser::IRemediationAction::ScanInfection, reinterpret_cast<ISymBase*&>(pScanInfection)))
			{
				// Error or not IScanInfection
				break;
			}

			IScanFileInfectionPtr pFileInfection;
			if(SYM_FAILED(pScanInfection->QueryInterface(IID_ScanFileInfection, reinterpret_cast<void**>(&pFileInfection)))
				|| pFileInfection == NULL)
			{
				// Error or not IScanFileInfection
				break;
			}

			LPSTR szAFileName = (LPSTR)pFileInfection->GetLongFileName();

			HRESULT hr = m_pQuarantineDLL->CreateNewQuarantineItemFromFile(szAFileName, 
				QFILE_TYPE_NORMAL,
				QFILE_STATUS_QUARANTINED,
				&m_pQuarItem);

			if(FAILED(hr) || (NULL == m_pQuarItem))
			{
				// Let the user know we could not add the file to quarantine
				//ReportFatalError(IDS_ERROR_ADDING_TO_QUARANTINE);
				return;
			}


			/////////// Set Anomaly info
			if(m_listCtrlAnomaly.GetItemCount() > 0)
			{
				if(m_listCtrlAnomaly.GetSelectedCount() == 1)
				{
					ccEraser::IAnomalyPtr pAnomaly;
					POSITION pos = m_listCtrlAnomaly.GetFirstSelectedItemPosition();

					ccEraser::eResult res = m_pAnomalyList->GetItem(m_listCtrlAnomaly.GetNextSelectedItem(pos), pAnomaly);
					setResultWindow(res, _T("m_pAnomalyList->GetItem(m_listCtrlAnomaly.GetNextSelectedItem(pos), pAnomaly)"));
					if(Failed(res))
						return;

					cc::IKeyValueCollectionPtr pProps;
					res = pAnomaly->GetProperties(pProps);
					setResultWindow(res, _T("pAnomaly->GetProperties(pProps)"));
					if(Failed(res))
						return;

					cc::IIndexValueCollectionPtr pCategories;
					if(pProps->GetValue(ccEraser::IAnomaly::Categories, reinterpret_cast<ISymBase*&>(pCategories)))
					{
						m_pQuarItem->SetAnomalyCategories(pCategories);
					}

					cc::IStringQIPtr pName;
					if(pProps->GetValue(ccEraser::IAnomaly::Name, reinterpret_cast<ISymBase*&>(pName)))
					{						
						CString sName = ccSym::CStringConvert::GetStringT(pName);
						m_pQuarItem->SetAnomalyName(sName.GetBuffer());
					}

					cc::IStringQIPtr pID;
					if(pProps->GetValue(ccEraser::IAnomaly::ID, reinterpret_cast<ISymBase*&>(pID)))
					{
						CString sID = ccSym::CStringConvert::GetStringT(pID);
						m_pQuarItem->SetAnomalyID(sID.GetBuffer());
					}
				}
			}


			setQuarantineWindow();

			res = pRemediationAction->Remediate();

			cc::IStringPtr pDescription;
			ccEraser::eResult resDes = pRemediationAction->GetDescription(pDescription);
			if(Failed(resDes))
			{
				setResultWindow(res, _T("pRemediationAction->Remediate()"));
			}
			else
			{
				setResultWindow(res, CString(_T("Remediate ")) + ccSym::CStringConvert::GetStringT(pDescription));
			}

			break;
		}
	}

	RefreshDetectionCtrl();
	RefreshRemediationCtrl();
}


void CSketchbookDlg::OnBnClickedButtonQuarItemCreateWithoutFile()
{
	HRESULT hr = m_pQuarantineDLL->CreateNewQuarantineItemWithoutFile(QFILE_STATUS_THREAT_BACKUP, &m_pQuarItem);
	if(FAILED(hr) || (NULL == m_pQuarItem))
	{
		// failed to CreateNewQuarantineItemWithoutFile()
	}
	else
	{
		setQuarantineWindow();

		if(m_listCtrlAnomaly.GetItemCount() == 0)
			return;

		if(m_listCtrlAnomaly.GetSelectedCount() != 1)
			return;

		ccEraser::IAnomalyPtr pAnomaly;
		POSITION pos = m_listCtrlAnomaly.GetFirstSelectedItemPosition();

		ccEraser::eResult res = m_pAnomalyList->GetItem(m_listCtrlAnomaly.GetNextSelectedItem(pos), pAnomaly);
		setResultWindow(res, _T("m_pAnomalyList->GetItem(m_listCtrlAnomaly.GetNextSelectedItem(pos), pAnomaly)"));
		if(Failed(res))
			return;


		cc::IKeyValueCollectionPtr pProps;
		res = pAnomaly->GetProperties(pProps);
		setResultWindow(res, _T("pAnomaly->GetProperties(pProps)"));
		if(Failed(res))
			return;

		cc::IIndexValueCollectionPtr pCategories;
		if(pProps->GetValue(ccEraser::IAnomaly::Categories, reinterpret_cast<ISymBase*&>(pCategories)))
		{
			m_pQuarItem->SetAnomalyCategories(pCategories);
		}

		cc::IStringQIPtr pName;
		if(pProps->GetValue(ccEraser::IAnomaly::Name, reinterpret_cast<ISymBase*&>(pName)))
		{						
			CString sName = ccSym::CStringConvert::GetStringT(pName);
			m_pQuarItem->SetAnomalyName(sName.GetBuffer());
		}

		cc::IStringQIPtr pID;
		if(pProps->GetValue(ccEraser::IAnomaly::ID, reinterpret_cast<ISymBase*&>(pID)))
		{
			CString sID = ccSym::CStringConvert::GetStringT(pID);
			m_pQuarItem->SetAnomalyID(sID.GetBuffer());
		}
	}

	setQuarantineWindow();
}


void CSketchbookDlg::OnBnClickedButtonVirusFileCreate()
{
	CFileDialog dlgFile(FALSE, NULL, _T("*.*"), OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, NULL, this);

	if (dlgFile.DoModal() == IDOK)
	{
		//TCHAR szPath[MAX_PATH];
		if(dlgFile.GetPathName().GetLength() > 0)
		{
			CAtlFile File;
			if(FAILED(File.Create(dlgFile.GetPathName(),GENERIC_WRITE,0,CREATE_ALWAYS)) 
				|| FAILED(File.Write(g_szEicar,sizeof(g_szEicar)-1)))
			{
				// Error
			}
			File.Close();
		}
	}
}


void CSketchbookDlg::OnBnClickedButtonSNDGetRemediationData()
{
	setQuarantineWindow();
	if(m_pQuarItem == NULL)
	{
		setResultWindow(ccEraser::Success, _T("NULL QuarantineItem pointer"));
		return;
	}

	TCHAR szListing[2000] = {0};
	TCHAR szLine[2000] = {0};

	TCHAR szDataDestinationFolder[MAX_PATH] = _T("C:\\Program Files\\Norton AntiVirus\\Quarantine\\TEST_SND_INTEGRATION");
	TCHAR szRemediationActionName[MAX_PATH] = {0};
	DWORD dwBufferSizeAction = MAX_PATH;
	TCHAR szRemediationUndoName[MAX_PATH] = {0};
	DWORD dwBufferSizeUndo = MAX_PATH;

	DWORD dwRemediationCount = 0;
	HRESULT hResult = m_pQuarItem->GetRemediationCount(&dwRemediationCount);
	for(DWORD dwRemediationIndex = 1; dwRemediationIndex <= dwRemediationCount; dwRemediationIndex++)
	{

		hResult = m_pQuarItem->GetRemediationData(szDataDestinationFolder, dwRemediationIndex, szRemediationActionName, &dwBufferSizeAction, szRemediationUndoName, &dwBufferSizeUndo);
		if(FAILED(hResult))
		{
			CCTRACEE(_T("CSketchbookDlg::OnBnClickedButtonSNDGetRemediationData() - FAILED(m_pQuarItem->GetRemediationData(..., %d, ...)) = 0x%08X"), dwRemediationIndex, hResult);
		}
		sprintf(szLine, _T("%d: %s, %s\r\n"), dwRemediationIndex, szRemediationActionName, szRemediationUndoName);
		_tcscat(szListing, szLine);
	}

	::MessageBox(NULL, szListing, _T("Listing of Remediations"), MB_OK);

}

void CSketchbookDlg::OnBnClickedButtonRemediateAddData()
{
	setQuarantineWindow();

	if(m_pQuarItem == NULL)
	{
		setResultWindow(ccEraser::Success, _T("NULL QuarantineItem pointer"));
		return;
	}

	if(m_pRemediationActionList == NULL)
	{
		return;
	}

	if(m_listCtrlRemediations.GetSelectedCount() != 1)
	{
		::MessageBox(NULL, "Pick only 1 item", "Wrong number of items selected", MB_OK);
		return;
	}

	POSITION pos = m_listCtrlRemediations.GetFirstSelectedItemPosition();
	if (pos == NULL)
	{
		return;
	}
	else
	{
		int iItem = m_listCtrlRemediations.GetNextSelectedItem(pos);

		ccEraser::IRemediationActionPtr pRemediationAction;
		ccEraser::eResult res = m_pRemediationActionList->GetItem(iItem, pRemediationAction);
		setResultWindow(res, _T("m_pRemediationActionList->GetItem(iItem, pRemediationAction)"));
		if(Failed(res))
		{
			return;
		}
		
		bool bSupportsUndo = false;
		res = pRemediationAction->SupportsUndo(bSupportsUndo);
		if(ccEraser::Failed(res) || !bSupportsUndo)
		{
			setResultWindow(res, _T("Does not support undo, or failed to get property."));
			return;
		}
		else
		{
			setResultWindow(res, _T("Support undo."));
		}

		ccSym::CMemoryStreamImplPtr pUndo;
		pUndo.Attach(ccSym::CMemoryStreamImpl::CreateMemoryStreamImpl());
		res = pRemediationAction->GetUndoInformation(pUndo);

		cc::IStringPtr pDescription;
		ccEraser::eResult resDes = pRemediationAction->GetDescription(pDescription);
		if(ccEraser::Failed(res))
		{
			setResultWindow(res, _T("pRemediationAction->GetUndoInformation(pUndo)"));
			return;
		}
		else
		{
			setResultWindow(res, CString(_T("GetUndoInformation ")) + ccSym::CStringConvert::GetStringT(pDescription));
		}

		res = pRemediationAction->Remediate();
		resDes = pRemediationAction->GetDescription(pDescription);
		if(ccEraser::Failed(res))
		{
			setResultWindow(res, _T("pRemediationAction->Remediate()"));
		}
		else
		{
			setResultWindow(res, CString(_T("Remediate ")) + ccSym::CStringConvert::GetStringT(pDescription));
		}

		HRESULT hResult = m_pQuarItem->AddRemediationData(pRemediationAction, pUndo);
		if(FAILED(hResult))
		{
			setResultWindow(ccEraser::eResult::Fail, _T("m_pQuarItem->AddRemediationData()"));
		}
	}

	setQuarantineWindow();
	RefreshDetectionCtrl();
	RefreshRemediationCtrl();
}

void CSketchbookDlg::OnBnClickedButtonLoadQuarItem()
{
	setQuarantineWindow();

	CFileDialog dlgFile(FALSE, NULL, _T("*.*"), OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, NULL, this);
	if (dlgFile.DoModal() == IDOK)
	{
		int iFileLen = dlgFile.GetPathName().GetLength() + 1;
		if(iFileLen > 0)
		{
			TCHAR* szFile = new TCHAR[iFileLen];
			_tcscpy(szFile, dlgFile.GetPathName());

			DWORD dwVirusID = 11101;

			HRESULT hResult = m_pQuarantineDLL->LoadQuarantineItemFromFilenameVID(szFile, dwVirusID, &m_pQuarItem);
			if(FAILED(hResult) || (NULL == m_pQuarItem))
			{
				m_pQuarItem = NULL;
				setResultWindow(ccEraser::eResult::Fail, _T("m_pQuarItem->m_pQuarantineDLL->LoadQuarantineItemFromFilenameVID()"));
			}
			else
			{
				setResultWindow(ccEraser::Success, _T("m_pQuarItem->m_pQuarantineDLL->LoadQuarantineItemFromFilenameVID()"));
			}

			delete[] szFile;
		}
	}

	setQuarantineWindow();
	RefreshDetectionCtrl();
	RefreshRemediationCtrl();
}


// Only acts on QuarantineItem Remediations
void CSketchbookDlg::OnBnClickedButtonScanQuarItem()
{
	setQuarantineWindow();

	if(m_pQuarItem == NULL)
	{
		setResultWindow(ccEraser::Success, _T("NULL QuarantineItem pointer"));
		return;
	}

	DWORD dwRemediationCount = 0;
	HRESULT hResult = m_pQuarItem->GetRemediationCount(&dwRemediationCount);
	for(DWORD dwRemediationIndex = 1; dwRemediationIndex <= dwRemediationCount; dwRemediationIndex++)
	{
		char szQueryName[MAX_PATH] = {0};
		DWORD dwRemediationActionType;
		sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_TYPE, dwRemediationIndex);
		hResult = m_pQuarItem->GetDWORD(szQueryName, &dwRemediationActionType);
		if(FAILED(hResult))
		{
			setResultWindow(ccEraser::Fail, _T("FAILED(m_pQuarItem->GetDWORD(QSERVER_ITEM_REMEDIATION_ACTION_TYPE))"));
			CCTRACEE(_T("CQuarantineItem::AddRemediationData() FAILED(m_pQuarItem->GetDWORD(%s))"), szQueryName);
			return;
		}

		ccEraser::eObjectType eType = static_cast<ccEraser::eObjectType>(dwRemediationActionType);

		if((eType != ccEraser::FileRemediationActionType) && (eType != ccEraser::InfectionRemediationActionType))
			continue;

		hResult = m_pQuarItem->ScanFileRemediation(dwRemediationIndex);
		if(SUCCEEDED(hResult) && ((S_OK == hResult) || (S_NO_VIRUS_FOUND == hResult)))
		{
			TCHAR szErrorTemp[MAX_PATH] = {0};
			sprintf(szErrorTemp, _T("SUCCEEDED(ScanFileRemediation()), hr=0x%08X"), hResult);
			setResultWindow(ccEraser::Success, szErrorTemp);
		}
		else
		{
			TCHAR szErrorTemp[MAX_PATH] = {0};
			sprintf(szErrorTemp, _T("FAILED(ScanFileRemediation()), hr=0x%08X"), hResult);
			setResultWindow(ccEraser::Fail, szErrorTemp);
		}
	}

	setQuarantineWindow();
}


// Only acts on QuarantineItem Remediations
void CSketchbookDlg::OnBnClickedButtonRepairQuarItem()
{
	setQuarantineWindow();

	if(m_pQuarItem == NULL)
	{
		setResultWindow(ccEraser::Success, _T("NULL QuarantineItem pointer"));
		return;
	}

	DWORD dwRemediationCount = 0;
	HRESULT hResult = m_pQuarItem->GetRemediationCount(&dwRemediationCount);
	for(DWORD dwRemediationIndex = 1; dwRemediationIndex <= dwRemediationCount; dwRemediationIndex++)
	{
		char szQueryName[MAX_PATH] = {0};
		DWORD dwRemediationActionType;
		sprintf(szQueryName, QSERVER_ITEM_REMEDIATION_ACTION_TYPE, dwRemediationIndex);
		hResult = m_pQuarItem->GetDWORD(szQueryName, &dwRemediationActionType);
		if(FAILED(hResult))
		{
			setResultWindow(ccEraser::Fail, _T("FAILED(m_pQuarItem->GetDWORD(QSERVER_ITEM_REMEDIATION_ACTION_TYPE))"));
			CCTRACEE(_T("CQuarantineItem::AddRemediationData() FAILED(m_pQuarItem->GetDWORD(%s))"), szQueryName);
			return;
		}

		ccEraser::eObjectType eType = static_cast<ccEraser::eObjectType>(dwRemediationActionType);

		if((eType != ccEraser::FileRemediationActionType) && (eType != ccEraser::InfectionRemediationActionType))
			continue;

		// Will receive temp location values
		TCHAR szDestinationFolder[MAX_PATH] = {0};
		DWORD dwDestFolderSize = MAX_PATH;
		TCHAR szDestinationFilename[MAX_PATH] = {0};
		DWORD dwDestFilenameSize = MAX_PATH;

		hResult = m_pQuarItem->RepairAndRestoreFileRemediationToTemp(dwRemediationIndex, 
														szDestinationFolder,
														&dwDestFolderSize,
														szDestinationFilename,
														&dwDestFilenameSize);
		if(SUCCEEDED(hResult) && ((S_OK == hResult) || (S_NO_VIRUS_FOUND == hResult)))
		{
			TCHAR szErrorTemp[MAX_PATH] = {0};
			sprintf(szErrorTemp, _T("SUCCEEDED(RepairAndRestoreFileRemediationToTemp()), hr=0x%08X"), hResult);
			setResultWindow(ccEraser::Success, szErrorTemp);

			CFileDialog dlgFile(FALSE, NULL, _T("*.*"), OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT, NULL, this);
			if (dlgFile.DoModal() == IDOK)
			{
				if(dlgFile.GetPathName().GetLength() > 0)
				{
					CString sTempPath = szDestinationFolder;
					sTempPath += "\\";
					sTempPath += szDestinationFilename;
					::CopyFile(sTempPath, dlgFile.GetPathName(), FALSE);
				}
			}
		}
		else
		{
			TCHAR szErrorTemp[MAX_PATH] = {0};
			sprintf(szErrorTemp, _T("FAILED(RepairAndRestoreFileRemediationToTemp()), hr=0x%08X"), hResult);
			setResultWindow(ccEraser::Fail, szErrorTemp);
		}
	}

	setQuarantineWindow();
}


void CSketchbookDlg::OnBnClickedButtonQuarDLLTakeSnapshot()
{
	HRESULT hr = m_pQuarantineDLL->TakeSystemSnapshot();
	if(FAILED(hr))
		setResultWindow(ccEraser::Fail, _T("FAILED m_pQuarantineDLL->TakeSystemSnapshot()"));
	else
		setResultWindow(ccEraser::Success, _T("SUCCEEDED m_pQuarantineDLL->TakeSystemSnapshot()"));

	setQuarantineWindow();
}

void CSketchbookDlg::OnBnClickedButtonQuarDLLAddSnapshotToQuarItem()
{
	setQuarantineWindow();

	if(m_pQuarItem == NULL)
	{
		setResultWindow(ccEraser::Success, _T("NULL QuarantineItem pointer"));
		return;
	}

	HRESULT hResult = m_pQuarantineDLL->AddSnapshotToQuarantineItem(m_pQuarItem);
	if(FAILED(hResult))
	{
		setResultWindow(ccEraser::Fail, _T("FAILED(m_pQuarantineDLL->AddSnapshotToQuarantineItem())"));
		CCTRACEE(_T("%s - FAILED(m_pQuarantineDLL->AddSnapshotToQuarantineItem())"), __FUNCTION__);
		return;
	}

	setQuarantineWindow();
}
