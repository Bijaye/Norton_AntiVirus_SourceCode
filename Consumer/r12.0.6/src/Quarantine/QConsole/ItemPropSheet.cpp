//////////////////////////////////////////////////////////////////////////////
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation.
// All rights reserved.
//////////////////////////////////////////////////////////////////////////////
//
// ItemPropSheet.cpp : implementation file
//
/////////////////////////////////////////////////////////////////////////////
// $Log:   S:/QCONSOLE/VCS/itempropsheet.cpv  $
// 
//    Rev 1.3   20 May 1998 17:13:38   DBuches
// 1st pass at help.
// 
//    Rev 1.2   13 Mar 1998 15:20:14   DBuches
// Don't show virus tab if we have no virus.
// 
//    Rev 1.1   06 Mar 1998 11:31:08   DBuches
// Added DisplayProperties.
// 
//    Rev 1.0   27 Feb 1998 15:08:58   DBuches
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "qconsole.h"
#include "mainfrm.h"
#include "ItemPropSheet.h"
#include "qsfields.h"

#include "QItemEZ.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CItemPropSheet

IMPLEMENT_DYNAMIC(CItemPropSheet, CPropertySheet)

CItemPropSheet::CItemPropSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CItemPropSheet::CItemPropSheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

CItemPropSheet::~CItemPropSheet()
{
}


BEGIN_MESSAGE_MAP(CItemPropSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CItemPropSheet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CItemPropSheet message handlers

////////////////////////////////////////////////////////////////////////////
// Description	    : Displays the properties for a given item
// Argument         : IQuarantineItem * pItem - item to display
// Argument         : CImageList* pImageList - pointer to system image list
// Argument         : int iImage - index of image
// 3/3/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
int CItemPropSheet::DisplayProperties(IQuarantineItem* pItem, 
                                      CImageList* pImageList, int iImage)
{
    // Genereral page
    m_GenPage.SetItemPointer(pItem);
	m_GenPage.SetPropSheetCookie(this);
    m_GenPage.m_ImageButton.SetImageList(pImageList);
    m_GenPage.m_ImageButton.SetImageIndex(iImage);
    AddPage(&m_GenPage);

	//  Removed by AHP 12/16/2004 - Client info page

    //
    // Virus info page
    // If there is no virus entry for this Virus ID, don't add the page

    // Get the virus index
	DWORD dwVirID = NULL;
    pItem->GetVirusID(&dwVirID);
    
    CQconsoleDoc* pDoc = (CQconsoleDoc*)((CFrameWnd*)AfxGetMainWnd())->GetActiveDocument();

    if(NULL != dwVirID)
    {
        // Get the Virus info for this VID
        CSymPtr<IScanVirusInfo> pScanVirusInfo;
        pDoc->m_pQuarantine->GetVirusInformation(dwVirID, &pScanVirusInfo);

        // Get categories from the item.
        CString cszThreatCategories;
        QItemEZ::GetString(pItem, QSERVER_ITEM_INFO_CATEGORIES, cszThreatCategories);

        // if this is not a virus, don't add the virus page
        if(pScanVirusInfo && (cszThreatCategories.IsEmpty() || 0 == atol(cszThreatCategories)))
        {
            m_VirusPage.SetItemPointer(pItem);
            m_VirusPage.SetVirusIndex(pScanVirusInfo->GetVirusID());
            AddPage(&m_VirusPage);
        }
    }

	// Risk matrix page
	if(QItemEZ::THREAT_NON_VIRAL == QItemEZ::IsViral(pItem))
	{
		m_RiskMatrixPage.SetItemPointer(pItem);
		AddPage(&m_RiskMatrixPage);
	}

    // Side effect info page - If side effect info is saved into this item
    DWORD dwVal = 0;
    if((SUCCEEDED(pItem->GetDWORD(QSERVER_ITEM_SE_COUNT, &dwVal)) && dwVal > 0))
    {
        m_SEPage.SetItemPointer(pItem);
        m_SEPage.SetVirusIndex(dwVirID);
        AddPage(&m_SEPage);
    }

    // Quarantine Item content page
    m_RemediationPage.SetItemPointer(pItem);
    m_RemediationPage.SetVirusIndex(dwVirID);
    AddPage(&m_RemediationPage);
		
    // Fire off the dialog.
    return DoModal();
}


////////////////////////////////////////////////////////////////////////////
// Description	    : Handles WM_INITDIALOG message.
// 5/20/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL CItemPropSheet::OnInitDialog() 
{
	CPropertySheet::OnInitDialog();
	
	// Hide the OK button and change the text of the cancel button to Close.

    GetDlgItem(IDOK)->ShowWindow(SW_HIDE);

    CString s((LPCTSTR)IDS_CLOSE);
    GetDlgItem(IDCANCEL)->SetWindowText(s);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CItemPropSheet::ActivateRiskMatrixPage(void* pvCookie)
{
	CItemPropSheet* pPropSheet = static_cast<CItemPropSheet*>(pvCookie);

	int iPageIndex = pPropSheet->GetPageIndex(&(pPropSheet->m_RiskMatrixPage));
	if((iPageIndex > 0) && (iPageIndex < pPropSheet->GetPageCount()))
		return pPropSheet->SetActivePage(iPageIndex);
	
	return FALSE;
}
