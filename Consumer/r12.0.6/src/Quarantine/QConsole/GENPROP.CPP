//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// GenProp.cpp : implementation file for General tab of details dialog.
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/genprop.cpv  $
// 
//    Rev 1.7   08 Jun 1998 14:54:54   DBuches
// Added extra code to classify compressed items.
// 
//    Rev 1.6   04 Jun 1998 15:37:24   DBuches
// Changed to work with GMT times.
// 
//    Rev 1.5   20 May 1998 17:11:18   DBuches
// 1st pass at help.
// 
//    Rev 1.4   07 May 1998 15:07:46   DBuches
// Added help button.
// 
//    Rev 1.3   21 Apr 1998 17:17:54   DBuches
// Added recommendation text.
// 
//    Rev 1.2   13 Mar 1998 15:20:02   DBuches
// Display status text value.
// 
//    Rev 1.1   06 Mar 1998 11:20:24   DBuches
// Updated to use IQuarantineItem pointer to update data.
// 
//    Rev 1.0   27 Feb 1998 15:08:56   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "platform.h"
#include "qconsole.h"
#include "GenProp.h"
#include "RangeStr.h"
#include "xapi.h"
#include "QItemEZ.h"
#include "ItemPropSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CGenProp property page

IMPLEMENT_DYNCREATE(CGenProp, CItemPage)


CGenProp::CGenProp() : CItemPage(CGenProp::IDD)
{
	//{{AFX_DATA_INIT(CGenProp)
	m_sTypeName = _T("");
	m_sFileSize = _T("");
	m_sQuarantineDate = _T("");
	m_sSubDate = _T("");
    m_sStatusInfo = _T("");
	//}}AFX_DATA_INIT
}


CGenProp::~CGenProp()
{
}


void CGenProp::DoDataExchange(CDataExchange* pDX)
{
	CItemPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenProp)
	DDX_Control(pDX, IDC_ICON_BUTTON, m_ImageButton);
	DDX_Control(pDX, IDC_NAME_EDIT, m_linkItemName);
	DDX_Text(pDX, IDC_TYPE_EDIT, m_sTypeName);
	DDX_Text(pDX, IDC_SIZE_EDIT, m_sFileSize);
	DDX_Text(pDX, IDC_QUARANTINE_EDIT, m_sQuarantineDate);
	DDX_Text(pDX, IDC_SUBMITTED_EDIT, m_sSubDate);
    DDX_Text(pDX, IDC_INFORMATION_EDIT, m_sStatusInfo);
	DDX_Control(pDX, IDC_DETAILS_GENERAL_RISK_LEVEL, m_linkRiskLevel);
	DDX_Text(pDX, IDC_DETAILS_GENERAL_DEPENDENCIES, m_sDependencies);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenProp, CItemPage)
	//{{AFX_MSG_MAP(CGenProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CGenProp message handlers

////////////////////////////////////////////////////////////////////////////
// Description	    : Handles WM_INITDIALOG message
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CGenProp::OnInitDialog()
{
	CItemPage::OnInitDialog();

	// Set up controls.

	// Get categories from the item.
	BOOL bRet = QItemEZ::GetThreatCategoryList(m_pItem, m_sTypeName);

	// If it's empty (no Categories) set default categories string
	if((FALSE == bRet) || m_sTypeName.IsEmpty())
		m_sTypeName.LoadString(IDS_UNKNOWN_THREAT_CATEGORY);

	// Get threat name from doc, which uses "unknown" text if can't get a value.
	// FALSE return value means an empty string. Already tried to get default, so
	// we'll just use the string reguardless.
	CString sItemName;
	bRet = m_pDoc->GetThreatName(m_pItem, sItemName);

	DWORD dwVirID = -1;
	if(SUCCEEDED(m_pItem->GetVirusID(&dwVirID)) && (dwVirID != -1))
	{
		CString sURL;
		sURL.Format(IDS_ONLINE_VIRUS_INFO_FORMAT, dwVirID);
		m_linkItemName.SetLink(sItemName, sURL);

		// Show these if expanded threat only
		if(QItemEZ::THREAT_NON_VIRAL == QItemEZ::IsViral(m_pItem))
		{
			// Get Risk level average and set up link
			DWORD dwRiskLevel = -1;
			dwRiskLevel = m_pDoc->GetRiskLevelAverage(m_pItem);
			if(-1 != dwRiskLevel)
			{
				CString sRiskLevel;
				bool bGotText = m_pDoc->GetRiskLevelText(dwRiskLevel, sRiskLevel);
				if(bGotText)
				{
					m_linkRiskLevel.SetLink(sRiskLevel, LinkCallback, static_cast<void*>(this), m_pvPropSheetCookie);
					GetDlgItem(IDC_DETAILS_GENERAL_RISK_LEVEL)->ShowWindow(SW_SHOW);
					GetDlgItem(IDC_DETAILS_GENERAL_HEADING_RISK_LEVEL)->ShowWindow(SW_SHOW);
				}
			}

			// Get Dependencies flag and set text for link
			DWORD dwHasDependencies = 0;
			m_pItem->GetAnomalyDependencyFlag(&dwHasDependencies);
			if(1 == dwHasDependencies)
				m_sDependencies.LoadString(IDS_DEPENDENCIES_YES);
			else
				m_sDependencies.LoadString(IDS_DEPENDENCIES_NO);

			GetDlgItem(IDC_DETAILS_GENERAL_DEPENDENCIES)->ShowWindow(SW_SHOW);
			GetDlgItem(IDC_DETAILS_GENERAL_HEADING_DEPENDENCIES)->ShowWindow(SW_SHOW);
		}
	}
	else // Handle if no VID (i.e. manually added item or error)
	{
		m_linkItemName.SetLink(sItemName);
	}

	// Get size.
	DWORD dwFileSize;
	m_pItem->GetOriginalFilesize(&dwFileSize);
	CRangedString rs(dwFileSize);
	m_sFileSize = rs;

	// Get date strings
	TCHAR szDate[128];
	TCHAR szTime[128];
	SYSTEMTIME tm;
	m_pItem->GetDateQuarantined(&tm);

	// Convert to local time
	FILETIME ft, lft;
	SystemTimeToFileTime(&tm, &ft);
	FileTimeToLocalFileTime(&ft, &lft);
	FileTimeToSystemTime(&lft, &tm);

	GetDateFormat(NULL, DATE_LONGDATE, &tm, NULL, szDate, 128);
	GetTimeFormat(NULL, 0, &tm, NULL, szTime, 128);

	m_sQuarantineDate = szDate;
	m_sQuarantineDate += _T(" ");
	m_sQuarantineDate += szTime;

	m_pItem->GetDateSubmittedToSARC(&tm);

	// Check to make sure this was actually submitted.
	if(tm.wYear == 0)
	{
		m_sSubDate.LoadString(IDS_NOT_SUBMITTED);
	}
	else
	{
		// Convert to local time
		SystemTimeToFileTime(&tm, &ft);
		FileTimeToLocalFileTime(&ft, &lft);
		FileTimeToSystemTime(&lft, &tm);

		GetDateFormat(NULL, DATE_LONGDATE, &tm, NULL, szDate, 128);
		GetTimeFormat(NULL, 0, &tm, NULL, szTime, 128);

		m_sSubDate = szDate;
		m_sSubDate += _T(" ");
		m_sSubDate += szTime;
	}

	// Get status text
	DWORD dwStatus;
	int iStrInfoID;

	if(SUCCEEDED(m_pItem->GetFileStatus(&dwStatus)))
	{
		if(dwStatus & QFILE_STATUS_QUARANTINED)
		{
			iStrInfoID = IDS_RECOMMEND_QUARANTINE;
		}
		else if(dwStatus & QFILE_STATUS_BACKUP_FILE)
		{
			iStrInfoID = IDS_RECOMMEND_BACKUP;
		}
		else if(dwStatus & QFILE_STATUS_THREAT_BACKUP)
		{
			iStrInfoID = IDS_RECOMMEND_THREAT_BACKUP;
		}

		CString sFilePath;
		bRet = QItemEZ::GetFullANSIFilePath(m_pItem, sFilePath);
		if((FALSE != bRet) && FileIsCompressed(sFilePath))
			iStrInfoID = IDS_RECOMMEND_COMPRESSED_FILE;

		m_sStatusInfo.LoadString(iStrInfoID);
	}

	// Blast data to controls
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////
// 6/8/98 DBUCHES, created - header added.
////////////////////////////////////////////////////////////////////////////
BOOL CGenProp::FileIsCompressed(LPCTSTR szFileName)
{
	const char szCompressedExtList[] = "ZIP.LHA.LZH.ARJ.CAB.MIM.MME.UUE";
	auto  char szExt[MAX_PATH];

	NameReturnExtensionEx(szFileName, szExt, MAX_PATH);

	strupr(szExt);

	if((strlen(szExt) == 3) &&
		((strstr(szCompressedExtList, szExt)) ||
		(szExt[2] == '_')))          // LZ extentions: ".??_"
	{
		return TRUE;
	}

	return FALSE;
}


BOOL CGenProp::LinkCallback(void* pData, void* pParam)
{
	// Call prop sheet to switch to risk matrix page
	return CItemPropSheet::ActivateRiskMatrixPage(pParam);
}
