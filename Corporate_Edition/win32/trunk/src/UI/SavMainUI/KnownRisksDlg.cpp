// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2004, 2005, Symantec Corporation, All Rights Reserved.

#include "stdafx.h"
#include "KnownRisksDlg.h"
#include <algorithm>
#include "htmlhelpids.h"
#include "vpcommon.h"

// CKnownRisksDlg dialog

#define CHECKBOX_COLUMN_WIDTH  21


const static struct
{
	UINT nColName;
	UINT nColWidth;
	int  iFormat;
} anColumnInfo[evAfterLastColumn] =
	// The index of an element of this array must correspond to the
	// ERiskConfigColumn value for which the element holds data.
{
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_THREATNAME_NAME,
      IDS_THREATCAT_TAB_EXCLUSION_COLUMN_THREATNAME_WIDTH,
	  LVCFMT_LEFT },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_FIRSTACTION_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_FIRSTACTION_WIDTH,
	  LVCFMT_LEFT },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_SECONDACTION_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_SECONDACTION_WIDTH,
	  LVCFMT_LEFT },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_OVERALLRATING_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_OVERALLRATING_WIDTH,
	  LVCFMT_CENTER },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_PRIVACYRATING_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_PRIVACYRATING_WIDTH,
	  LVCFMT_CENTER },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_PERFORMANCERATING_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_PERFORMANCERATING_WIDTH,
	  LVCFMT_CENTER },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_STEALTHRATING_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_STEALTHRATING_WIDTH,
	  LVCFMT_CENTER },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_REMOVALRATING_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_REMOVALRATING_WIDTH,
	  LVCFMT_CENTER },
	{ IDS_THREATCAT_TAB_EXCLUSION_COLUMN_DEPENDENT_NAME,
	  IDS_THREATCAT_TAB_EXCLUSION_COLUMN_DEPENDENT_WIDTH,
	  LVCFMT_CENTER }
};


IMPLEMENT_DYNAMIC(CKnownRisksDlg, CDialog)
CKnownRisksDlg::CKnownRisksDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKnownRisksDlg::IDD, pParent)
{
    m_bInRisksListNotify = false;
}


void CKnownRisksDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_AVAILABLE_PVID_LIST, m_cRisksList);
}


BEGIN_MESSAGE_MAP(CKnownRisksDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBtnClickOk)
    ON_BN_CLICKED(IDCANCEL, OnBtnClickCancel)

    ON_NOTIFY(LVN_ITEMCHANGED, IDC_AVAILABLE_PVID_LIST, OnItemchangedRisksList)
    ON_NOTIFY(LVN_COLUMNCLICK, IDC_AVAILABLE_PVID_LIST, OnColumnClickedRisksList)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_AVAILABLE_PVID_LIST, OnCustomDrawRisksList)
    ON_COMMAND(ID_HELP, OnHelp)	
	ON_WM_SIZE()
END_MESSAGE_MAP()


BOOL CKnownRisksDlg::OnInitDialog()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CDialog::OnInitDialog();

    // Clear the data vectors.
    m_moRisks.clear();
    VERIFY(m_cRisksList.DeleteAllItems());
	// Insert a checkbox column and set up the risk list.
	VERIFY(m_cRisksList.InsertColumn(0, _T(""), LVCFMT_LEFT, CHECKBOX_COLUMN_WIDTH) == 0);
	CKnownRisksDlg::SetupRiskListControl(m_cRisksList, CKnownRisksDlg::ERiskConfigColumnToSubitemIndex, LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);
	// Disable the control while we insert items into the list controls.
	// This is to prevent users from sorting the view while the list
	// is loading.
	m_cRisksList.EnableWindow(FALSE);

	FillListThread(this);

	// Update the list boxes.
    m_cRisksList.SortItems(ExceptionListCompareFunc, reinterpret_cast<DWORD_PTR>(this));
	m_cRisksList.EnableWindow();
	m_cRisksList.Invalidate();
    // Set wizard buttons accordingly.
	UpdateWizardButtons();

    return TRUE;
}

#define DEFAULT_WIDTH  100  // Use this width if we can't parse a _WIDTH string.

/** Sets up a list control to display risks (inserts columns, etc).
  * @param cRiskList The list control to add columns to.
  * It should not have any columns already.
  * @param fERiskConfigColumnToSubitemIndex A function that maps ERiskConfig
  * values to subitem indexes for this list control.  It should return -1 if the
  * list control should not have a certain column.
  * For now, we must have fERisk...(x) > fERisk...(y) when x > y.
  * If that has to change, this function must be enhanced to handle it.
  * @param dwExtendedStyle extended style of the list control
  */
void CKnownRisksDlg::SetupRiskListControl(CListCtrl &cRiskList,
											int fERiskConfigColumnToSubitemIndex(ERiskConfigColumn) /*= UseAllColumns*/,
											DWORD dwExtendedStyle /*= LVS_EX_FULLROWSELECT*/)
{
    // Setup columns.
	int iColsAdded = 0; // the # of columns added so far

	for (int iColumn = 0; iColumn < sizeof(anColumnInfo)/sizeof(anColumnInfo[0]); iColumn++)
	{
		// Get the subitem index and see if we should add this column.
		int iSubitemIndex = fERiskConfigColumnToSubitemIndex(
								static_cast<ERiskConfigColumn>(iColumn));

		if (-1 == iSubitemIndex)
			continue;
		// Get the column name and width.
		// We get the width from a string resource so that internationalization
		// can change it.
		CString columnName;
		CString columnWidth;
		int     columnWidthNo = 0;

		try
		{
			columnName.LoadString(anColumnInfo[iColumn].nColName);
			columnWidth.LoadString(anColumnInfo[iColumn].nColWidth);
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
		columnWidthNo = _tstoi((LPCTSTR) columnWidth);
		if (columnWidthNo <= 0)
			columnWidthNo = DEFAULT_WIDTH;
		// Add the column.
		VERIFY(cRiskList.InsertColumn(iSubitemIndex,
									  columnName,
									  anColumnInfo[iColumn].iFormat,
									  columnWidthNo)
				   == iSubitemIndex);
			// See cppdoc comments above on fERiskConfigColumnToSubitemIndex.
		iColsAdded++;
	}
	// Set the extended style.
    cRiskList.SetExtendedStyle(dwExtendedStyle);
}


void CKnownRisksDlg::UpdateRiskInListview(CListCtrl &cRiskList, 
										  int iRiskItem, 
										  const CExpandedThreat &oRisk, 
										  int fERiskConfigColumnToSubitemIndex(ERiskConfigColumn) /*= UseAllColumns*/)
{
	// Set the risk name with a URL (if the vid is not zero).
	CString strUrlTemplate;
	CString strUrl;

	if (oRisk.GetPermanentVirusId() != 0)
	{
		try
		{
			strUrlTemplate.LoadString(IDS_RESPONSE_URL);
				// TODO:  Move IDS_RESPONSE_URL from all .rc files into acta.str.
			strUrl.FormatMessage(strUrlTemplate, oRisk.GetPermanentVirusId());
		}
		VP_CATCH_MEMORYEXCEPTIONS(;)
	}
	VERIFY(cRiskList.SetItemText(iRiskItem,
								 fERiskConfigColumnToSubitemIndex(evColThreatName),
								 oRisk.GetName()));
	// Set rating subitems.
	for (int eRating = 0; eRating < CExpandedThreat::evAfterLastRating; eRating++)
	{
		// Get the rating description.
		stlTstring strRatingDesc;

		oRisk.GetRating((CExpandedThreat::ERatingType)eRating, strRatingDesc);
		// Set the rating text.
		ERiskConfigColumn eCol;
		
		eCol = CKnownRisksDlg::ERatingTypeToERiskConfigColumn(static_cast<CExpandedThreat::ERatingType>(eRating));
		if (evInvalidColumn == eCol)
			continue;
		VERIFY(cRiskList.SetItemText(iRiskItem,
									 fERiskConfigColumnToSubitemIndex(eCol),
									 strRatingDesc.c_str()));
	}
	// Set the overall rating.
	double	   dAvgRating = GetOverallRating(oRisk);
	stlTstring strOverallRatingDesc;

	CExpandedThreat::FindRatingDesc(dAvgRating, strOverallRatingDesc);
	VERIFY(cRiskList.SetItemText(iRiskItem,
								 fERiskConfigColumnToSubitemIndex(evColOverallRating),
								 strOverallRatingDesc.c_str()));
	// Set the dependency subitem.
	DWORD	dwDependencyFlag = oRisk.GetDependency();
	CString strDependencyText;

	if (TRUE == dwDependencyFlag)
		strDependencyText.LoadString(IDS_YES);
	else if (FALSE == dwDependencyFlag)
		strDependencyText.LoadString(IDS_NO);
//	else if (CExpandedThreat::RATING_UNRATED == dwDependencyFlag)
//		strDependencyText.LoadString(IDS_RISK_RATING_UNRATED);
	else
	{
		strDependencyText.LoadString(IDS_RISK_RATING_UNSET);
	}
	VERIFY(cRiskList.SetItemText(iRiskItem,
								 fERiskConfigColumnToSubitemIndex(evColDependent),
								 strDependencyText));
}


// For use with LVM_SORTITEMSEX, not LVM_SORTITEMS
// Combining this with CThreatCatTabExceptions::ExceptionListCompareFunc() just
// required too much weird generalization or refactoring.
// TODO this right, we probably need to put a base class under
// CThreatCatTabExceptions and CKnownRisksDlg with pure virtual GetRisk()
// functions etc.
int CALLBACK CKnownRisksDlg::ExceptionListCompareFunc(LPARAM lItem1, LPARAM lItem2, LPARAM lThis)
{
	return 0;
}
	/*
	// Validate parameters
	CKnownRisksDlg*	pThis = reinterpret_cast<CKnownRisksDlg*>(lThis);
    CListCtrl*	pcExceptionsCtrl = NULL;
	int				iCol;

	if (pThis != NULL)
		pcExceptionsCtrl = &pThis->m_cRisksList;
    if (NULL == pcExceptionsCtrl)
		return 0;
	iCol = pcExceptionsCtrl->m_iSortColumn;
	if (iCol < 0 ||
		iCol >= evAfterLastColumn + 1 - (evAfterLastActionColumn - evFirstActionColumn))
	{
		ASSERT(false); // Unknown column!
		return 0;
	}
    // Get item text and sort order.
	CString	itemText1 = pcExceptionsCtrl->GetItemText(lItem1, iCol);
	CString	itemText2 = pcExceptionsCtrl->GetItemText(lItem2, iCol);
	int		iSortOrderMultiplier =
				(pcExceptionsCtrl->m_iSortOrder == SORT_ASCENDING) ? 1 : -1;
				// Multiplying a comparison result by this will produce a result
				// that correctly sorts items in ascending or descending order.
	// Sort checkboxes.
	if (0 == iCol)
	{
		BOOL bItem1Checked = pcExceptionsCtrl->GetCheck(lItem1);
		BOOL bItem2Checked = pcExceptionsCtrl->GetCheck(lItem2);

		if (bItem1Checked == bItem2Checked)
			return 0;
		else
			return bItem1Checked < bItem2Checked
					   ? iSortOrderMultiplier
					   : -iSortOrderMultiplier;
	}
	// The same text means they have the same sort position.
	if (itemText1.CompareNoCase(itemText2) == 0)
		return 0;
	// Convert the column number into a risk config column.
	ERiskConfigColumn eCol = evInvalidColumn;

	if (iCol > 0)
	{
		if (iCol - 1 < evFirstActionColumn)
			eCol = static_cast<ERiskConfigColumn>(iCol - 1);
		else
			eCol = static_cast<ERiskConfigColumn>(iCol - 1 + (evAfterLastActionColumn - evFirstActionColumn));

		ASSERT(CKnownRisksDlg::ERiskConfigColumnToSubitemIndex(eCol) == iCol);
			// This logic is bad.  Create SubitemIndexToERiskConfigColumn().
	}
	// A few columns require special handling.
	if (eCol >= evFirstRatingColumn && eCol < evAfterLastRatingColumn || // Ratings!
		evColDependent == eCol) // similar logic to ratings
	{
		// Find the risks.
		DWORD dwPVID1 = pcExceptionsCtrl->GetItemData(lItem1);
		DWORD dwPVID2 = pcExceptionsCtrl->GetItemData(lItem2);
		RiskMap::const_iterator iterItem1 = pThis->m_moRisks.find(dwPVID1);
		RiskMap::const_iterator iterItem2 = pThis->m_moRisks.find(dwPVID2);

		if (pThis->m_moRisks.end() == iterItem1 ||
			pThis->m_moRisks.end() == iterItem2)
		{
			ASSERT(false); // A pvid is invalid?
			return 0;
		}
		// Get the ratings.
		double dRating1 = 0;
		double dRating2 = 0;

		if (evColDependent == eCol)
		{
			dRating1 = iterItem1->second.GetDependency();
			dRating2 = iterItem2->second.GetDependency();
		}
		else if (evColOverallRating == eCol)
		{
			dRating1 = GetOverallRating(iterItem1->second);
			dRating2 = GetOverallRating(iterItem2->second);
		}
		else
		{
			CExpandedThreat::ERatingType eRatingSorted = ERiskConfigColumnToERatingType(eCol);

			dRating1 = iterItem1->second.GetRating(eRatingSorted);
			dRating2 = iterItem2->second.GetRating(eRatingSorted);
		}
		// Sort unset as the last, unrated next to last, and then "High"
		// third last, because we don't want unset/unrated values to escape
		// the user's attention.
		if (CExpandedThreat::RATING_UNSET == (DWORD)dRating1)
			return iSortOrderMultiplier;
		if (CExpandedThreat::RATING_UNSET == (DWORD)dRating2)
			return -iSortOrderMultiplier;
		if (CExpandedThreat::RATING_UNRATED == (DWORD)dRating1)
			return iSortOrderMultiplier;
		if (CExpandedThreat::RATING_UNRATED == (DWORD)dRating2)
			return -iSortOrderMultiplier;

		// Both risks are rated, so just compare them.
		return (dRating1 < dRating2) ? -iSortOrderMultiplier : iSortOrderMultiplier;
	}
	else
	{
	    // Most columns should just be sorted alphabetically.
		return itemText1.CompareNoCase(itemText2) * iSortOrderMultiplier;
	}
}
*/

int CKnownRisksDlg::ERiskConfigColumnToSubitemIndex(ERiskConfigColumn eCol)
{
	// Skip the action columns.
	if (eCol < evFirstActionColumn)
		return eCol + 1; // Add 1 for the checkbox column.
	else if (eCol >= evAfterLastActionColumn)
		return eCol + 1 - (evAfterLastActionColumn - evFirstActionColumn);
			// Add 1 for the checkbox column and subtract some amount for the
			// action columns we're skipping.
	else
		return -1; // Indicate that we don't have this column.
}



HRESULT CKnownRisksDlg::UpdateWizardButtons()
{
	// Enable the next button if a risk is checked.
	for (int iItem = 0; iItem < m_cRisksList.GetItemCount(); iItem++)
		if (m_cRisksList.GetCheck(iItem))
		{
//	        pPropertySheet->SetWizardButtons( PSWIZB_NEXT );
			return S_OK;
		}
	// No risks were checked.  Disable the Next button.
//    pPropertySheet->SetWizardButtons( 0 );

	return S_OK;
}

void CKnownRisksDlg::OnBtnClickOk()
{	
    if (UpdateData(TRUE) == FALSE)
		return;

	// Close the dialog.
	OnOK();
}

void CKnownRisksDlg::OnBtnClickCancel()
{	
	// Close the dialog.
	OnCancel();
}


/** Given list control item's state, returns the item's check state.
  * @param uiState an item's state, as in NM_LISTVIEW's uOldState and uNewState.
  * @return -1 for no check state set, 0 for unchecked, and 1 for checked.
  */
#define GET_CHECK_STATE(uiState)  \
	((BOOL)((((uiState) & LVIS_STATEIMAGEMASK)>>12)-1))
	// This macro is adapted from ListView_GetCheckState.

void CKnownRisksDlg::OnItemchangedRisksList(NMHDR *pNMHDR, LRESULT *pResult)
{
	// Tell Windows we didn't fully handle this message
	// so it will do its normal handling.
	*pResult = 0;
	// See if a check state changed and should affect other selected rows.
	NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

	if (GET_CHECK_STATE(pNMListView->uOldState) == GET_CHECK_STATE(pNMListView->uNewState))
	{
		// Something other than the check state changed.
		return;
	}

	if (!m_cRisksList.IsWindowEnabled())
		return; // We're not done setting up.

	if (!m_cRisksList.GetItemState(pNMListView->iItem, LVIS_SELECTED))
	{
		// The row is not selected.
		// MS default behavior is to not select a row when the user clicks a
		// checkbox.  If we didn't return here, it could be confusing for the
		// user if we also change the checked state of the selected row(s).
		// It could also be strange if we select this row, because that's not
		// default MS behavior.
		// There was a checkbox state change, so we still need to update buttons.
		UpdateWizardButtons();
		return;
	}

    // Calling SetCheck causes a recursive call to this function, which in combination with the following loop,
    // and a large risk list can cause a stack overflow.
    if (m_bInRisksListNotify)
		return;
    m_bInRisksListNotify = true;

	// Iterate through all selected rows, changing their check states to match
	// the current item's.
	POSITION posSelectedItem = m_cRisksList.GetFirstSelectedItemPosition();

	while (posSelectedItem != NULL)
	{
		int iSelection = m_cRisksList.GetNextSelectedItem(posSelectedItem);

		if (iSelection == pNMListView->iItem)
			continue; // Don't need to re-check / re-uncheck the current item.
		m_cRisksList.SetCheck(iSelection, GET_CHECK_STATE(pNMListView->uNewState));
	}
	// Update buttons in case they checked an item.
	UpdateWizardButtons();
    m_bInRisksListNotify = false;
}


void CKnownRisksDlg::OnColumnClickedRisksList( NMHDR* pNMHDR, LRESULT* pResult )
{
    NM_LISTVIEW *lvNotification = (NM_LISTVIEW*) pNMHDR;

    m_cRisksList.SortItems(ExceptionListCompareFunc, reinterpret_cast<LPARAM>(this));
 
	*pResult = 0;
}






void CKnownRisksDlg::OnHelp()
{
    CWinApp* winApp = NULL;

	winApp = AfxGetApp();

    if (winApp != NULL)
        winApp->WinHelpInternal(THREAT_EXCLUSION_WIZARD_PAGE_SELECT);
}


void CKnownRisksDlg::AddExpandedThreat(const CExpandedThreat &oRisk)
{
	int itemNo = 0;
	// See if we already have the risk.
	if (m_moRisks.find(oRisk.GetPermanentVirusId()) != m_moRisks.end())
		return;
	// Store the risk.
	try
	{
		m_moRisks.insert(RiskMap::value_type(oRisk.GetPermanentVirusId(), oRisk));

        itemNo = m_cRisksList.InsertItem((int)m_moRisks.size(), _T(""));
		VERIFY(m_cRisksList.SetItemData(itemNo, oRisk.GetPermanentVirusId()));
		CKnownRisksDlg::UpdateRiskInListview(m_cRisksList, itemNo, oRisk, CKnownRisksDlg::ERiskConfigColumnToSubitemIndex);
	}
	VP_CATCH_MEMORYEXCEPTIONS
	(
		return;
	)
}


void CKnownRisksDlg::OnCustomDrawRisksList(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (NULL == pNMHDR || NULL == pResult)
	{
		ASSERT(false);
		return;
	}
	// For some reason, CListCtrl::DrawItem() does not get called on our list
	// control, so we need to alter the drawing logic here.
	// Let the control draw itself (unless we decide otherwise).
	*pResult = CDRF_DODEFAULT;
	// See what drawing stage we're in.
	NMLVCUSTOMDRAW *pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	switch (pLVCD->nmcd.dwDrawStage)
	{
		case CDDS_PREPAINT: // We always get this notification before anything is painted.
			// Request per-item notifications.
			*pResult = CDRF_NOTIFYITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT: // About to draw an item...
			// Request sub-item notifications.
			*pResult = CDRF_NOTIFYSUBITEMDRAW;
			break;
		case CDDS_ITEMPREPAINT | CDDS_SUBITEM:
			// Request post-drawing sub-item notifications.
			*pResult = CDRF_NOTIFYPOSTPAINT;
			// Let the control set font/color for this sub-item.
			/*
			if (m_cRisksList.SetLinkFontColor(pLVCD->nmcd.dwItemSpec,
											  pLVCD->iSubItem,
											  CDC::FromHandle(pLVCD->nmcd.hdc),
											  pLVCD->clrText))
			{
				*pResult |= CDRF_NEWFONT;
			}
			*/
			break;
		case CDDS_ITEMPOSTPAINT | CDDS_SUBITEM:
			// Asking for this case is unnecessary, but it seems to lessen an MFC bug.
			// The bug is in CDDS_ITEMPREPAINT | CDDS_SUBITEM above.  The list
			// control will sometimes apply the font/color set for one item to other
			// sub-items.  This causes the list control to look as if many sub-items
			// had links.
			break;
		default:
			ASSERT(false); // We shouldn't get messages in any other state.
			break;
	}
}

ERiskConfigColumn CKnownRisksDlg::ERatingTypeToERiskConfigColumn(CExpandedThreat::ERatingType eRating)
{
	switch (eRating)
	{
		case CExpandedThreat::evPrivacyRating:	   return evColPrivacyRating;
		case CExpandedThreat::evPerformanceRating: return evColPerformanceRating;
		case CExpandedThreat::evStealthRating:	   return evColStealthRating;
		case CExpandedThreat::evRemovalRating:	   return evColRemovalRating;
		default:
			ASSERT(false); // Invalid rating!
			return evInvalidColumn;
	}
}


double CKnownRisksDlg::GetOverallRating(const CExpandedThreat &oRisk)
{
	return 0;
}
/*
	// Get the total of all ratings.
	DWORD dwTotalRating = 0;

	for (int eRating = 0; eRating < CExpandedThreat::evAfterLastRating; eRating++)
	{
		if (dwTotalRating != CExpandedThreat::RATING_UNRATED)
		{
			DWORD dwRating = oRisk.GetRating((CExpandedThreat::ERatingType)eRating);

			if (CExpandedThreat::RATING_UNSET	== dwRating ||
				CExpandedThreat::RATING_UNRATED == dwRating)
			{
				dwTotalRating = CExpandedThreat::RATING_UNRATED;
			}
			else
				dwTotalRating += dwRating;
		}
		// else if one rating is unset, the total will be as well.
	}
	// Return the overall rating.
	if (CExpandedThreat::RATING_UNRATED == dwTotalRating)
		return CExpandedThreat::RATING_UNRATED;
	else
		return ((double)dwTotalRating) / CExpandedThreat::evAfterLastRating;
}
*/

CExpandedThreat::ERatingType CKnownRisksDlg::ERiskConfigColumnToERatingType(ERiskConfigColumn eColumn)
{
	switch (eColumn)
	{
	case evColPrivacyRating:	 return CExpandedThreat::evPrivacyRating;
	case evColPerformanceRating: return CExpandedThreat::evPerformanceRating;
	case evColStealthRating:	 return CExpandedThreat::evStealthRating;
	case evColRemovalRating:	 return CExpandedThreat::evRemovalRating;
	default:
		ASSERT(false); // Invalid rating!
		return CExpandedThreat::evInvalidRating;
	}
}


// Read the expanded threat list, then populate the "Available Threats" and "Selected Threats" list controls.
// This thread runs asynchronous to the UI and so must handle situations where the UI windows is destroyed before
// it starts
UINT CKnownRisksDlg::FillListThread( LPVOID pVoid )
{
#if defined _USRDLL
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
    HRESULT                 hr              = S_OK;
    DWORD                   cc              = ERROR_SUCCESS;
    HWND                    hwnd  = NULL;
    CComPtr<IThreatEnum>    ptrThreatEnum;
    
    // Validate parameters
    if (pVoid == NULL)
        return 1;
        
	CKnownRisksDlg* pkrd	= reinterpret_cast<CKnownRisksDlg*>(pVoid);
	hwnd					= pkrd->GetSafeHwnd();

    // Cache some data.
	TCHAR szHost[MAX_PATH] = _T("localhost");
    DWORD       category = 0;
 
    // Initialize COM.
    if( FAILED(hr = CoInitialize( NULL )) )
        goto ThreatEnum_End;

    if( (hr = CoCreateLDVPObject( CLSID_Transman, IID_IThreatEnum, (void**)&ptrThreatEnum )) != S_OK )
        goto ThreatEnum_End;

	if( (cc = ptrThreatEnum->Open( (const char *)szHost, category )) != ERROR_SUCCESS )
    {
        hr = E_FAIL;
        goto ThreatEnum_End;
    }

    // Iterate.
    try
    {
        EXPANDED_THREAT_INFO stThreat;

        do
        {
            if( (cc = ptrThreatEnum->GetNext( &stThreat )) == ERROR_SUCCESS )
            {
                CExpandedThreat item( stThreat.nPVID,
                                      (LPCTSTR)stThreat.szName,
                                      stThreat.nCategory,
                                      stThreat.nStealthRating,
                                      stThreat.nRemovalRating,
                                      stThreat.nPerformanceRating,
                                      stThreat.nPrivacyRating,
                                      stThreat.nDependency);

                // If this is in new threat mode, the "selected" risk list is
                // actually a list of risks the user has configured already.
                // In this case, we shouldn't add selected risks to the select risks page.
				AddExpandedThreat(item);
            }
        }
        while( cc == ERROR_SUCCESS &&  ::IsWindow(hwnd));

        // Translate EOF to success.
        if( cc == ERROR_NO_MORE )
        {
            cc = ERROR_SUCCESS;
        }
        else if( cc != ERROR_SUCCESS )
        {
            // On fail, skip to the end.
            hr = E_FAIL;
            goto ThreatEnum_End;
        }
    }
    catch( std::bad_alloc& )
    {
        hr = E_OUTOFMEMORY;
        goto ThreatEnum_End;
    }

ThreatEnum_End:

    // Explicitly release the COM object so that we can CoUninitialize() later.
    ptrThreatEnum.Release();

    CoUninitialize();

    return( hr );
}
