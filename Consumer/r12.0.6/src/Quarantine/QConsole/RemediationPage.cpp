//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// RemediationPage.cpp : implementation file for remediation tab of details
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "qconres.h"
#include "RemediationPage.h"
#include "qsfields.h"
#include "QItemEZ.h"

#include "ccEraserInterface.h"

#include <map>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum { ICON_REG = 0, ICON_DLL, ICON_BAT, ICON_INI, ICON_DAT, ICON_EXE, ICON_INFECTED, ICON_UNKNOWN };
const TCHAR *g_szExtensions[] = { _T("REG"), // ICON_REG
                                  _T("DLL"), // ICON_DLL
                                  _T("BAT"), // ICON_BAT
                                  _T("INI"), // ICON_INI
                                  _T("DAT"), // ICON_DAT
                                  _T("EXE"), // ICON_EXE
                                  _T("UNKNOWN"), // ICON_UNKNOWN
                                   NULL};


/////////////////////////////////////////////////////////////////////////////
// CRemediationPage property page

IMPLEMENT_DYNCREATE(CRemediationPage, CItemPage)

CRemediationPage::CRemediationPage() : CItemPage(CRemediationPage::IDD)
{
    //{{AFX_DATA_INIT(CRemediationPage)
    m_cszVirusName = _T("");
    //}}AFX_DATA_INIT

    m_ulVID = 0;
}

CRemediationPage::~CRemediationPage()
{
}

void CRemediationPage::DoDataExchange(CDataExchange* pDX)
{
    CItemPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRemediationPage)
    DDX_Text(pDX, IDC_VIRUS_NAME_STATIC, m_cszVirusName);
    DDX_Control(pDX, IDC_LST_COMPONENTS, m_cwndComponentsList);
    DDX_Control(pDX, IDC_LST_DETAILS, m_cwndDetailsList);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRemediationPage, CItemPage)
    //{{AFX_MSG_MAP(CRemediationPage)
    //}}AFX_MSG_MAP
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LST_COMPONENTS, OnComponentListSelChange)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRemediationPage message handlers

////////////////////////////////////////////////////////////////////////////////
// Description	    : Handle WM_INITDIALOG message.
// 2/19/98 - DBUCHES function created / header added
////////////////////////////////////////////////////////////////////////////////
BOOL CRemediationPage::OnInitDialog() 
{
    CItemPage::OnInitDialog();
    
    // Set up virus info
    if(FAILED(m_pDoc->m_pQuarantine->GetVirusInformation(m_ulVID, &m_pScanVirusInfo)))
    {
        CCTRACEE(_T("%s - Failed to get the virus information for vid %d"), __FUNCTION__, m_ulVID);
        return TRUE;
    }

    m_pDoc->GetThreatName(m_pItem, m_cszVirusName);
    
    CRect cRect;
    m_cwndComponentsList.GetWindowRect(&cRect);
    
    DWORD dwWidth = NULL, dwCol1 = NULL, dwCol2 = NULL;
    dwWidth = cRect.Width() - 4; // offset so we don't get a scroll bar
    dwCol1 = (DWORD)((double)dwWidth * .9);
    dwCol2 = (DWORD)((double)dwWidth * .1);

    // Set up component and details list controls
    INT iCol = NULL;
    iCol = m_cwndComponentsList.InsertColumn(0, _T("DESC"), LVCFMT_LEFT, dwCol1);
    iCol = m_cwndComponentsList.InsertColumn(1, _T("COUNT"), LVCFMT_LEFT, dwCol2);
    
    m_cwndDetailsList.GetWindowRect(&cRect);
    dwWidth = cRect.Width() - 4; // offset so we don't get a scroll bar
    dwCol1 = dwWidth;
    iCol = m_cwndDetailsList.InsertColumn(0, _T("DETAIL"), LVCFMT_LEFT, dwCol1);

    // Setup image list
    m_cimgSmallIcons.Create(16, 16, TRUE, 4, 4);
    m_cwndComponentsList.SetImageList(&m_cimgSmallIcons, LVSIL_SMALL);
    FillImageList(m_cimgSmallIcons);
    
    //  Fill in component list
    FillComponentList();

    // Blast data to controls
    UpdateData(FALSE);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}


////////////////////////////////////////////////////////////////////////////////
// Description	    : Create group counts and fill component list.
// 11/22/2004 - APIERCE function created
////////////////////////////////////////////////////////////////////////////////
typedef std::map<DWORD, DWORD> MAP_GROUP_COUNTS;
void CRemediationPage::FillComponentList()
{

    DWORD dwRemediationCount = NULL;
    MAP_GROUP_COUNTS mapGroupCounts;

    m_pItem->GetRemediationCount(&dwRemediationCount);

    // Enumerate and count all of the remediation groups
    DWORD dwIndex;
    for (dwIndex = 1; dwIndex <= dwRemediationCount; dwIndex++)
    {
        DWORD dwRemediationType = NULL;

		// If the remediation was unsuccessful or does not have undo information, skip it
		if(!QItemEZ::ShouldShowRemediation(m_pItem, dwIndex))
			continue;
        
        BOOL bRet = QItemEZ::GetRemediationType(m_pItem, dwRemediationType, dwIndex);
        if(!bRet || (NULL == dwRemediationType))
            continue;
        
		if(((DWORD)(ccEraser::InfectionRemediationActionType) == dwRemediationType) ||
			((DWORD)(ccEraser::DirectoryRemediationActionType) == dwRemediationType))
		{
			mapGroupCounts[ccEraser::FileRemediationActionType]++;
		}
		else
		{
			mapGroupCounts[dwRemediationType]++;
		}
    }

    // Hack-o-riffic!!
    // Since the initial file is not represented in the remediation
    //  data, I'll hack it in to appear that way in the UI.

	// Don't count initial file if it is a dummy
	if(!QItemEZ::InitialDummyFile(m_pItem))
		mapGroupCounts[ccEraser::FileRemediationActionType]++;

    // Format the group names and counts, and add them to the components list
    if(mapGroupCounts.size())
    {
        MAP_GROUP_COUNTS::iterator it;
        for(it = mapGroupCounts.begin(); it != mapGroupCounts.end(); it++)
        {
            CString cszDescription;
            INT iIcon = ICON_UNKNOWN;
            switch((*it).first)
            {
                case ccEraser::RegistryRemediationActionType:
                    cszDescription = _S(IDS_REMEDIATION_TYPE_REGISTRY);
                    iIcon = ICON_REG;
                    break;

                case ccEraser::FileRemediationActionType:
                    cszDescription = _S(IDS_REMEDIATION_TYPE_FILE);
                    iIcon = ICON_DLL;
                    break;
                
                case ccEraser::ProcessRemediationActionType:
                    cszDescription = _S(IDS_REMEDIATION_TYPE_RUNNING_PROCESS);
                    iIcon = ICON_EXE;
                    break;

                case ccEraser::BatchRemediationActionType:
                    cszDescription = _S(IDS_REMEDIATION_TYPE_BATCH_FILE);
                    iIcon = ICON_BAT;
                    break;

                case ccEraser::INIRemediationActionType:
                    cszDescription = _S(IDS_REMEDIATION_TYPE_INI_FILE);
                    iIcon = ICON_INI;
                    break;

                case ccEraser::ServiceRemediationActionType:
                    cszDescription = _S(IDS_REMEDIATION_TYPE_RUNNING_SERVICE);
                    iIcon = ICON_EXE;
                    break;

                //case ccEraser::InfectionRemediationActionType:
                //    cszDescription = _S(IDS_REMEDIATION_TYPE_FILE_INFECTION);
                //    iIcon = ICON_DLL;
                //    break;

                case ccEraser::COMRemediationActionType:
                    cszDescription = _S(IDS_REMEDIATION_TYPE_COM_ENTRY);
                    iIcon = ICON_REG;
                    break;

				case ccEraser::HostsRemediationActionType:
					cszDescription = _S(IDS_REMEDIATION_TYPE_HOSTS);
                    //iIcon = ICON_FOLDER;
					break;

				case ccEraser::LSPRemediationActionType:
					cszDescription = _S(IDS_REMEDIATION_TYPE_LSP);
                    //iIcon = ICON_EXE;
					break;
                    
                default:
                    cszDescription.FormatMessage(IDS_DEFAULT_REMEDIATION_TYPE);
                    iIcon = ICON_UNKNOWN;
                    break;
            }

            CString cszCount;
            cszCount.Format(_T("%d"), (*it).second);

            INT iItem = NULL;
            iItem = m_cwndComponentsList.InsertItem(0, cszDescription, iIcon);
            m_cwndComponentsList.SetItemText(iItem, 1, cszCount);
            m_cwndComponentsList.SetItemData(iItem, (*it).first);

        }
    }

    // Set focus to the first item
    m_cwndComponentsList.SetItemState(0, LVIS_FOCUSED , LVIS_FOCUSED);

    return;
}

////////////////////////////////////////////////////////////////////////////////
// Description	    : Fill details list with detail from type passed in.
// Arguments        : INT iComponentGroup - group of details to fill
// 11/22/2004 - APIERCE function created
////////////////////////////////////////////////////////////////////////////////
void CRemediationPage::FillDetailsList(INT iComponentGroup)
{
    m_cwndDetailsList.DeleteAllItems();
        
    DWORD dwRemediationCount = NULL;
    m_pItem->GetRemediationCount(&dwRemediationCount);

    DWORD dwIndex;
    for (dwIndex = 1; dwIndex <= dwRemediationCount; dwIndex++)
    {
		DWORD dwRemediationType = NULL;
        BOOL bRet = QItemEZ::GetRemediationType(m_pItem, dwRemediationType, dwIndex);
        if(!bRet || (NULL == dwRemediationType))
            continue;

		// Lumping Infection and directory remediations into File category.
		// If we want to do this more extensively, should have a better method of
		// handling this.
		if(((DWORD)(ccEraser::InfectionRemediationActionType) == dwRemediationType) ||
			((DWORD)(ccEraser::DirectoryRemediationActionType) == dwRemediationType))
			dwRemediationType = ccEraser::FileRemediationActionType;

        // Determine if this item is in the remediation group we are interested in
        if(iComponentGroup == dwRemediationType)
        {
			// If the remediation was unsuccessful or does not have undo information, skip it
			if(!QItemEZ::ShouldShowRemediation(m_pItem, dwIndex))
				continue;

            // Pull remediation description from Quarantine item
            CString cszDescription;
            QItemEZ::GetRemediationDescription(m_pItem, cszDescription, dwIndex);
            
            // Pull remediation operation from Quarantine item
            //DWORD dwOperation = NULL;
            //QItemEZ::GetRemediationOperation(m_pItem, dwOperation, dwIndex);

            // Format and add the entry to the details list
            if(cszDescription.GetLength())
            {
                INT iItem = m_cwndDetailsList.InsertItem(0, cszDescription);
                m_cwndDetailsList.SetColumnWidth(0, LVSCW_AUTOSIZE);
            }
        }
    }

    // Hack-o-riffic!!
    // Since the initial file is not represented in the remediation
    //  data, I'll hack it in to appear that way in the UI.
    if(ccEraser::FileRemediationActionType == iComponentGroup)
	{
		// Skip adding the initial file if it is a dummy file
		if(!QItemEZ::InitialDummyFile(m_pItem))
		{
			CString cszFileName;
			QItemEZ::GetFullANSIFilePath(m_pItem, cszFileName);
			if(cszFileName.GetLength())
			{
				INT iItem = m_cwndDetailsList.InsertItem(0, cszFileName);
                m_cwndDetailsList.SetColumnWidth(0, LVSCW_AUTOSIZE);
			}
		}
	}

    // Set focus to the first item
    m_cwndDetailsList.SetItemState(0, LVIS_FOCUSED , LVIS_FOCUSED);

    return;
}


////////////////////////////////////////////////////////////////////////////////
// Description	    : Fill image list with appropriate icons.
// Arguments        : INT iComponentGroup - group of details to fill
// 11/22/2004 - APIERCE function created
////////////////////////////////////////////////////////////////////////////////
void CRemediationPage::FillImageList(CImageList &cImageList) 
{
    // init image list
    CImageList *pImgList = NULL;
    SHFILEINFO shf;
    ZeroMemory(&shf, sizeof(SHFILEINFO));
    pImgList = CImageList::FromHandle((HIMAGELIST)SHGetFileInfo(_T("*.dat"),  FILE_ATTRIBUTE_NORMAL, &shf, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON));

    INT iCount = 0;
    for(iCount = 0; g_szExtensions[iCount]; iCount++)
    {
        // Build temp file name from template
        CString cszTempFileName;
        cszTempFileName.Format(_T("qcontest.%s"), g_szExtensions[iCount]);

        // Request icon from system for temp file
        ZeroMemory(&shf, sizeof(SHFILEINFO));
        SHGetFileInfo(cszTempFileName,  FILE_ATTRIBUTE_NORMAL, &shf, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);

        //  Store off icon if available
        if(shf.iIcon && pImgList)
            cImageList.Add(pImgList->ExtractIcon(shf.iIcon));
    }

    return;
}


////////////////////////////////////////////////////////////////////////////////
// Description	    : Handle LVN_ITEMCHANGED notification from Component List
// Arguments        : See MFC docs for details
// 11/22/2004 - APIERCE function created
////////////////////////////////////////////////////////////////////////////////
void CRemediationPage::OnComponentListSelChange(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    FillDetailsList(pNMLV->lParam);

    *pResult = 0;
}
