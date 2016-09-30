//******************************************************************************
//******************************************************************************
#include "stdafx.h"
#include "AxSysListView32.h"
#include "CDCOS.h"
#include "CommCtrl.h"
#include "AnomalyHelper.h"
#include "ccResourceLoader.h"
#include "StartBrowser.h"

// Resource Loader
extern cc::CResourceLoader g_ResLoader;

#include "Utils.h"

#pragma comment(lib, "comctl32.lib")

static const int LOW_DAMAGE_FLAG_VALUE = 1;
static const int MED_DAMAGE_FLAG_VALUE = 3;

bool CAxSysListView32::Compare_ByThreatName(ccEraser::IAnomaly* pAnom1, ccEraser::IAnomaly* pAnom2)
{
    CEZAnomaly ezAnom1(pAnom1);
    CEZAnomaly ezAnom2(pAnom2);

    cc::IStringPtr pName1;
    cc::IStringPtr pName2;

    if( ezAnom1.GetName(pName1) && ezAnom2.GetName(pName2) )
    {
        if (g_bThreatNameAscending)
        {
            // Sort ascending
            if (_tcsicmp(pName1->GetStringA(), pName2->GetStringA()) < 0)
                return true;
        }
        else
        {
            // Sort descending
            if (_tcsicmp(pName2->GetStringA(), pName1->GetStringA()) < 0)
                return true;		
        }
    }

    return false;
}

bool CAxSysListView32::Compare_ByThreatImpact(ccEraser::IAnomaly* pAnom1, ccEraser::IAnomaly* pAnom2)
{
    CEZAnomaly ezAnom1(pAnom1);
    CEZAnomaly ezAnom2(pAnom2);

    DWORD dwAvg1 = ezAnom1.GetDamageFlagAverage();
    DWORD dwAvg2 = ezAnom2.GetDamageFlagAverage();

    if (g_bThreatImpactAscending == true)
    {
        // Sort ascending
          
        // Viral always above non-viral
        if( ezAnom2.IsNonViral() && ezAnom1.IsViral() ) {
            return true;
        }
        else if( ezAnom2.IsNonViral() && ezAnom1.IsNonViral() && 
                 dwAvg1 > dwAvg2 ) {
            return true;
        }
    }
    else
    {
        if( ezAnom1.IsNonViral() && ezAnom2.IsViral() ) {
            return true;
        }
        else if( ezAnom1.IsNonViral() && ezAnom2.IsNonViral() && 
                 dwAvg2 > dwAvg1 ) {
            return true;
        }
    }

    return false;
}

bool CAxSysListView32::Compare_ByThreatType(ccEraser::IAnomaly* pAnom1, ccEraser::IAnomaly* pAnom2)
{
    CEZAnomaly ezAnom1(pAnom1);
    CEZAnomaly ezAnom2(pAnom2);

    if( g_bThreatTypeAscending )
    {
        if( ezAnom1.IsViral() && ezAnom2.IsNonViral() ) {
            return true;
        }
        else if( ezAnom1.IsNonViral() && ezAnom2.IsNonViral() ) {
            CString strCat1, strCat2;
            CThreatCatInfo threatInfo;
            threatInfo.GetCategoryText(ezAnom1.GetCategories(), 
                                       strCat1.GetBuffer(256), 256);
            strCat1.ReleaseBuffer();
            threatInfo.GetCategoryText(ezAnom2.GetCategories(), 
                                       strCat2.GetBuffer(256), 256);
            strCat2.ReleaseBuffer();

            if ( strCat1 > strCat2 ) {
                return true;
            }
        }
    }
    else
    {
        if( ezAnom1.IsNonViral() && ezAnom2.IsViral() ) {
            return true;
        }
        else if( ezAnom1.IsNonViral() && ezAnom2.IsNonViral() )
        {
            CString strCat1, strCat2;
            CThreatCatInfo threatInfo;
            threatInfo.GetCategoryText(ezAnom1.GetCategories(), 
                                       strCat1.GetBuffer(256), 256);
            strCat1.ReleaseBuffer();
            threatInfo.GetCategoryText(ezAnom2.GetCategories(), 
                                       strCat2.GetBuffer(256), 256);
            strCat2.ReleaseBuffer();

            if ( strCat1 < strCat2 ) {
                return true;
            }
        }
    }

    return false;
}


bool CAxSysListView32::Compare_ByThreatWarning(ccEraser::IAnomaly* pAnom1, 
                                               ccEraser::IAnomaly* pAnom2)
{
    CEZAnomaly ezAnom1(pAnom1);
    CEZAnomaly ezAnom2(pAnom2);

    CString pWarning1;
    CString pWarning2;

    GetWarning(ezAnom1, pWarning1);
    GetWarning(ezAnom2, pWarning2);

    if (g_bThreatWarningAscending == true)
    {
        if (_tcsicmp(pWarning1, pWarning2) < 0)
            return true;
    }
    else
    {
        if (_tcsicmp(pWarning2, pWarning1) < 0)
            return true;		
    }


    return false;
}

bool CAxSysListView32::Compare_ByThreatStatus(ccEraser::IAnomaly* pAnom1, 
                                              ccEraser::IAnomaly* pAnom2)
{
    CEZAnomaly ezAnom1(pAnom1);
    CEZAnomaly ezAnom2(pAnom2);

    AnomalyAction eAction1, eAction2;
    CommonUIInfectionStatus eStatus1, eStatus2;

    if( ezAnom1.GetAnomalyStatus(eAction1, eStatus1) && 
        ezAnom2.GetAnomalyStatus(eAction2, eStatus2) ) {
        if( g_bThreatStatusAscending ) {
            // Sort ascending (lower values indicate higher position)
            if ( eStatus1 < eStatus2 ) {
                return true;
            }
        }
        else {
            // Sort descending
            if ( eStatus1 > eStatus2 ) {
                return true;
            }
        }
    }

    return false;
}

bool CAxSysListView32::Compare_ByAction(ccEraser::IAnomaly* pAnom1, 
                                        ccEraser::IAnomaly* pAnom2)
{
    CEZAnomaly ezAnom1(pAnom1);
    CEZAnomaly ezAnom2(pAnom2);

    AnomalyAction eAction1, eAction2;
    CommonUIInfectionStatus eStatus1, eStatus2;

    if ( ezAnom1.GetAnomalyStatus(eAction1, eStatus1) && 
         ezAnom2.GetAnomalyStatus(eAction2, eStatus2) ) {
        if( g_bThreatStatusAscending ) {
            // Sort ascending (lower values indicate higher position)
            if ( eAction1 < eAction2 ) {
                return true;
            }
        }
        else {
            // Sort descending
            if ( eAction1 > eAction2 ) {
                return true;
            }
        }
    }

    return false;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CAxSysListView32::CAxSysListView32() :
    m_iThreatNameColumn(-1),
    m_iThreatTypeColumn(-1),
    m_iThreatImpactColumn(-1),
    m_iThreatWarningColumn(-1),
    m_iRepairActionColumn(-1),
    m_bThreatNameAscending(false)
{
    m_bWindowOnly = TRUE;

    _tcsncpy(REMOVE_ACTION, 
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_DELETE)), 
             MAX_PATH-1);
    _tcsncpy(REPAIR_ACTION, 
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_REPAIR)), 
             MAX_PATH-1);
    _tcsncpy(REPAIR_THEN_QUARANTINE_ACTION,
             _S(CAnomalyHelper::
                            GetActionToTakeVerb(ACTION_REPAIRTHENQUARANTINE)), 
             MAX_PATH-1);
    _tcsncpy(REPAIR_THEN_REMOVE_ACTION,
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_REPAIRTHENDELETE)), 
             MAX_PATH-1);
    _tcsncpy(REVIEW_ACTION, 
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_REVIEW)), 
             MAX_PATH-1);
    _tcsncpy(IGNORE_ACTION, 
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_IGNORE)), 
             MAX_PATH-1);
    _tcsncpy(EXCLUDE_ACTION, 
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_EXCLUDE)), 
             MAX_PATH-1);
    _tcsncpy(QUARANTINE_ACTION, 
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_QUARANTINE)), 
             MAX_PATH-1);
    _tcsncpy(COMPLETE_ACTION, 
             _S(CAnomalyHelper::GetActionToTakeVerb(ACTION_COMPLETE)), 
             MAX_PATH-1);
    _tcsncpy(NO_OPERATIONS_ACTION,
             _S(CAnomalyHelper::
                           GetActionToTakeVerb(ACTION_NOOPERATIONSAVAILABLE)), 
             MAX_PATH-1);


    const CString strRecommended = _S(IDS_RECOMMENDED_ABBREV);
    const int RECOMMEND_LENGTH = strRecommended.GetLength();

    _tcsncpy(REMOVE_ACTION_RECOMMENDED, REMOVE_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(REMOVE_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(REPAIR_ACTION_RECOMMENDED, REPAIR_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(REPAIR_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(REVIEW_ACTION_RECOMMENDED, REVIEW_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(REVIEW_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(REPAIR_THEN_QUARANTINE_ACTION_RECOMMENDED, 
             REPAIR_THEN_QUARANTINE_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(REPAIR_THEN_QUARANTINE_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(REPAIR_THEN_REMOVE_ACTION_RECOMMENDED, 
             REPAIR_THEN_REMOVE_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(REPAIR_THEN_REMOVE_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(IGNORE_ACTION_RECOMMENDED, IGNORE_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(IGNORE_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(EXCLUDE_ACTION_RECOMMENDED, EXCLUDE_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(EXCLUDE_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(QUARANTINE_ACTION_RECOMMENDED, QUARANTINE_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(QUARANTINE_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(COMPLETE_ACTION_RECOMMENDED, COMPLETE_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(COMPLETE_ACTION_RECOMMENDED, _T(strRecommended));

    _tcsncpy(NO_OPERATIONS_ACTION_RECOMMENDED, NO_OPERATIONS_ACTION, 
             MAX_PATH - RECOMMEND_LENGTH);
    _tcscat(NO_OPERATIONS_ACTION_RECOMMENDED, _T(strRecommended));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CAxSysListView32::~CAxSysListView32()
{
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, 
                                   LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    RECT rc;
    GetWindowRect(&rc);
    rc.right -= rc.left;
    rc.bottom -= rc.top;
    rc.top = rc.left = 0;
    InitCommonControls();

    const DWORD style = WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT | 
                        LVS_SINGLESEL | LVS_SHOWSELALWAYS | WS_TABSTOP;

    m_ctlSysListView32.Create(m_hWnd, rc, NULL, style );

    const DWORD dwLVExStyle = (m_ctlSysListView32.GetExtendedListViewStyle() | 
                                LVS_EX_FULLROWSELECT);
    m_ctlSysListView32.SetExtendedListViewStyle( dwLVExStyle );
    
    CClientDC dc( m_hWnd );
    m_fontFaked = CUtils::CreateFontEz(dc, IDS_FONTEZ_LISTBOXDETAIL_FAKE, NULL,
                                       _Module.GetResourceInstance());
    m_ctlSysListView32.SetFont(m_fontFaked);
    m_ctlSysListView32.SetHeaderFont();

    //InitListViewColumns();
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32::InitColumns(BOOL bManualRemoval)
{
    m_bManualRemoval = bManualRemoval;

    //
    // KLUDGE WORKAROUND
    //
    // In the CustomCellListView, for some reason moving the cursor over
    // any row causes paint messages to be sent to the item corresponding
    // to the row over which the cursor is moving.  Further, the paint 
    // messages are specific *only* the first column of that item.  The
    // result is that this cursor movement causes that particular cell
    // (the first column of whatever row the cursor is over) to flicker
    // as the cursor is moved since that movement causes that cell to 
    // paint itself over and over again.
    //
    // At this point I don't know why this occurs, so I don't know how to
    // stop.  Thus, I'm working around it by makeing the first column of
    // the list view a dummy column that will be of 0 width and thus not
    // visible to the user.  This invisible dummy column can then be the 
    // receptacle of all of these unwanted paint messages.  Hopefully, in
    // the future the cause of the messages can be determined and
    // addressed to stop the messages altogether from occurring, and thus
    // eliminating the need for this kludge column.
    //
    // -- MMW 07/27/2005
    //
    LVCOLUMN lvColumn;

    lvColumn.cx = 0;
    lvColumn.iOrder = 0;
    m_ctlSysListView32.InsertColumn(0, &lvColumn);
    m_ctlSysListView32.SetColumnType(0,
                                     CCustomCellListView::CELLTYPE_NORMAL);
    m_ctlSysListView32.SetColumnWidth(0, 0);

    if (m_bManualRemoval) {
        //
        // Usability thinks manual removal panel should look more like
        // the scan results page.  Still needs to be a list view, though,
        // in case there are more in the list han the screen can hold.
        // Make the following changes for visual appeal:
        //
        // 1. No alternating colors on rows.   
        // 2. Use header font (bold) for the threat names in list.
        // 3. Background of headers is white.
        //
        m_ctlSysListView32.SetAlternating(FALSE);        
        m_ctlSysListView32.SetNormalFont(IDS_FONTEZ_LISTBOXHEADER);        
        m_ctlSysListView32.GetCustomHeader().
                                         SetBackgroundColor(RGB(255,255,255));

        return InitMRColumns();
    }

    int iOrder = 1;
    ZeroMemory( &lvColumn, sizeof(lvColumn) );

    // Load the threat name column header text
    CString strThreatName;
    g_ResLoader.LoadString(IDS_THREATNAME_COL,strThreatName);

    // Load the threat warnning column header text
    CRect rectThreatWarning;
    CString strWarning;
    CString strWarningSz;
    g_ResLoader.LoadString(IDS_THREATWARNING_COL, strWarning);
    g_ResLoader.LoadString(IDS_MANUAL_REMOVAL_WARNING, strWarningSz);

    // Insert the repair action column
    CRect rectThreatAction;
    CString strRepairAction;
    CString strRepairActionSz;
    g_ResLoader.LoadString(IDS_REPAIRACTION_COL, strRepairAction);
    g_ResLoader.LoadString(IDS_ACTION_QUARANTINE, strRepairActionSz);
    strRepairActionSz += _S( IDS_RECOMMENDED_ABBREV );

    // Threat impact column
    CRect rectThreatImpact;
    CString strThreatImpact;
    CString strThreatImpactSz;
    g_ResLoader.LoadString(IDS_THREATIMPACT_COL, strThreatImpact);
    g_ResLoader.LoadString(IDS_THREAT_MATRIX_MED, strThreatImpactSz);


    // Threat type column
    CRect rectThreatType;
    CString strThreatType;
    CString strThreatTypeSz = "Remote Access";  // hard-coded but will NOT be
                                                // displayed.  Just using to
                                                // approximate column width.
    g_ResLoader.LoadString(IDS_TYPE_COL, strThreatType);

    // Calc the appropriate sizes of cols...
    CClientDC dc( m_hWnd );
    dc.DrawText( strThreatTypeSz, -1,  &rectThreatType, 
                 DT_CALCRECT | DT_SINGLELINE );
    dc.DrawText( strWarningSz, -1,  &rectThreatWarning, 
                 DT_CALCRECT | DT_SINGLELINE );
    dc.DrawText( strThreatImpactSz, -1,  &rectThreatImpact, 
                 DT_CALCRECT | DT_SINGLELINE );
    dc.DrawText( strRepairActionSz, -1,  &rectThreatAction, 
                 DT_CALCRECT | DT_SINGLELINE );

    int nWarningCX = rectThreatWarning.Width();
    const int nImpactCX = rectThreatImpact.Width();
    const int nActionCX = rectThreatAction.Width() +
                          GetSystemMetrics(SM_CYVSCROLL);
    const int nTypeCX = rectThreatType.Width();

    CRect rectClient;
    GetClientRect( rectClient );

    int cx = rectClient.Width() - nWarningCX - nImpactCX  - nActionCX -
                                  nTypeCX - GetSystemMetrics(SM_CYVSCROLL);

    //
    // Ensure that the Warning column is not *so* large as to shrink 8
    // the Name column down too far.  Never allow the Warning column to 
    // come out wider than the Name column
    //
    if (nWarningCX > cx) {
        int nAdjust = (nWarningCX - cx) / 2;

        cx += nAdjust;
        nWarningCX -= nAdjust;
    }

    // Insert the Filename column
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    lvColumn.fmt = LVCFMT_LEFT;
    _tcsncpy(m_szDisplay, strThreatName, MAX_PATH-1);
    lvColumn.pszText = m_szDisplay;
    lvColumn.cchTextMax = strThreatName.GetLength();
    lvColumn.cx = cx;
    lvColumn.iOrder = iOrder;
    m_iThreatNameColumn = iOrder;
    m_ctlSysListView32.InsertColumn( iOrder++, &lvColumn );
    m_ctlSysListView32.SetColumnType(m_iThreatNameColumn,
                                     CCustomCellListView::CELLTYPE_HYPERLINK);


    // Insert the threat impact column
    _tcsncpy(m_szDisplay, strThreatImpact, MAX_PATH-1);
    lvColumn.pszText = m_szDisplay;
    lvColumn.cchTextMax = strThreatImpact.GetLength();
    lvColumn.cx = nImpactCX;
    lvColumn.iOrder = iOrder;
    m_iThreatImpactColumn = iOrder;	
    m_ctlSysListView32.InsertColumn(iOrder++, &lvColumn);
    m_ctlSysListView32.SetColumnType(m_iThreatImpactColumn,
                                     CCustomCellListView::CELLTYPE_HYPERLINK);

    // Insert the warnings column
    _tcsncpy(m_szDisplay, strWarning, MAX_PATH-1);
    lvColumn.pszText = m_szDisplay;
    lvColumn.cchTextMax = strWarning.GetLength();
    lvColumn.cx = nWarningCX;
    lvColumn.iOrder = iOrder;
    m_iThreatWarningColumn = iOrder;	
    m_ctlSysListView32.InsertColumn(iOrder++, &lvColumn);
    m_ctlSysListView32.SetColumnTextColor(m_iThreatWarningColumn, 
                                          RGB(255,0,0));

    // Insert the threat type column
    _tcsncpy(m_szDisplay, strThreatType, MAX_PATH-1);
    lvColumn.pszText = m_szDisplay;
    lvColumn.cchTextMax = strThreatType.GetLength();
    lvColumn.cx = nTypeCX;
    lvColumn.iOrder = iOrder;
    m_iThreatTypeColumn = iOrder;	
    m_ctlSysListView32.InsertColumn(iOrder++, &lvColumn);
    m_ctlSysListView32.SetColumnType(m_iThreatTypeColumn,
                                     CCustomCellListView::CELLTYPE_HYPERLINK);

    // Insert the repair action column
    _tcsncpy(m_szDisplay, strRepairAction, MAX_PATH-1);
    lvColumn.pszText = m_szDisplay;
    lvColumn.cchTextMax = strRepairAction.GetLength();
    lvColumn.cx = nActionCX;
    lvColumn.iOrder = iOrder;
    m_iRepairActionColumn = iOrder;
    m_ctlSysListView32.InsertColumn(iOrder++, &lvColumn);

    return TRUE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CAxSysListView32::InitMRColumns()
{
    LVCOLUMN lvColumn;
    int iOrder = 1;
    ZeroMemory( &lvColumn, sizeof(lvColumn) );

    // Load the threat name column header text
    CString strThreatName;
    strThreatName.LoadString( IDS_THREATNAME_COL );

    // Insert the repair action column
    CRect rectThreatAction;
    CString strRepairAction;
    CString strRepairActionSz;
    strRepairAction.LoadString( IDS_REPAIRACTION_COL );
    strRepairActionSz.LoadString( IDS_VIEW_REMOVAL_INSTRUCTIONS );

    // Calc the sizes of columns
    CClientDC dc( m_hWnd );
    dc.DrawText( strRepairActionSz, -1,  &rectThreatAction, 
                 DT_CALCRECT | DT_SINGLELINE );

    const int nActionCX = rectThreatAction.Width();

    CRect rectClient;
    GetClientRect( rectClient );

    int cx = rectClient.Width() - nActionCX - GetSystemMetrics(SM_CYVSCROLL);

    // Insert the Filename column
    lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
    lvColumn.fmt = LVCFMT_LEFT;
    _tcsncpy(m_szDisplay, strThreatName, MAX_PATH-1);
    lvColumn.pszText = m_szDisplay;
    lvColumn.cchTextMax = strThreatName.GetLength();
    lvColumn.cx = cx;
    lvColumn.iOrder = iOrder;
    m_iThreatNameColumn = iOrder;
    m_ctlSysListView32.InsertColumn( iOrder++, &lvColumn );

    // Insert the repair action column
    _tcsncpy(m_szDisplay, strRepairAction, MAX_PATH-1);
    lvColumn.pszText = m_szDisplay;
    lvColumn.cchTextMax = strRepairAction.GetLength();
    lvColumn.cx = nActionCX;
    lvColumn.iOrder = iOrder;
    m_iRepairActionColumn = iOrder;
    m_ctlSysListView32.InsertColumn(iOrder++, &lvColumn);
    m_ctlSysListView32.SetColumnType(m_iRepairActionColumn,
                                     CCustomCellListView::CELLTYPE_HYPERLINK);

    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, 
                                     BOOL& bHandled)
{
    LRESULT lRes = CComControl<CAxSysListView32>::OnSetFocus(uMsg, wParam, 
                                                             lParam, bHandled);
    
    if ( m_bInPlaceActive ) {
        if (!IsChild(::GetFocus())) {
            m_ctlSysListView32.SetFocus();
        }
    }

    return lRes;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnGetDispInfo( int nCtrlID, LPNMHDR pNMHdr, 
                                         BOOL& bHandled )
{
    if (m_bManualRemoval) {
       return OnMRGetDispInfo( nCtrlID, pNMHdr, bHandled );
    }

    bool bPastTense;

    NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHdr);

    std::vector<ccEraser::IAnomalyPtr>* pVec = NULL;
    if ( IAxSysListView32_Filter_Infected == m_eFilterType ) {
        pVec = &m_vListRemaining;
        bPastTense = FALSE;
    }
    else {
        pVec = &m_vListRepaired;
        bPastTense = TRUE;
    }

    // Make sure we have at least 1 item in the infection vector and what the 
    // ListViewCtrl is asking for is within the infection vector
    if ( pVec->empty() ) {
        CCTRACEI(_T("CAxSysListView32::OnGetDispInfo() - The vector of items is empty."));
        return 0;
    }

    if( pDispInfo->item.iItem >= (int)pVec->size() ) {
        CCTRACEE(_T("CAxSysListView32::OnGetDispInfo() - Item %d is out of range."), pDispInfo->item.iItem);
        return 0;
    }

    // Clear old text
    ZeroMemory(m_szDisplay,MAX_PATH);

    m_EZAnomaly = pVec->at(pDispInfo->item.iItem);

    AnomalyAction eAction;
    CommonUIInfectionStatus eStatus;
    m_EZAnomaly.GetAnomalyStatus(eAction, eStatus);
    bool bViral = m_EZAnomaly.IsViral();

    if ( pDispInfo->item.mask & LVIF_TEXT ) {
        cc::IStringPtr pString;

        if (pDispInfo->item.iSubItem == m_iThreatNameColumn) {
            if ( m_EZAnomaly.GetName(pString) ) {
                _tcsncpy(m_szDisplay, pString->GetStringA(), MAX_PATH-1);
                pDispInfo->item.pszText = m_szDisplay;
            }
            else {
                CCTRACEE(_T("CAxSysListView32::OnGetDispInfo() - Failed to get threat name for item %d."), pDispInfo->item.iItem);
            }
        }
        else if ( pDispInfo->item.iSubItem == m_iThreatWarningColumn ) {
            // Get the warning text
            CString strWarning;
            GetWarning(m_EZAnomaly, strWarning, bPastTense);
            _tcsncpy(m_szDisplay, (LPCSTR)strWarning, MAX_PATH-1);

            pDispInfo->item.pszText = m_szDisplay;
        }
        else if ( pDispInfo->item.iSubItem == m_iThreatTypeColumn ) {
           // Get the categories text
            m_ThreatInfo.GetCategoryText(m_EZAnomaly.GetCategories(), 
                                         m_szDisplay, MAX_PATH);
            pDispInfo->item.pszText = m_szDisplay;
        }
        else if ( pDispInfo->item.iSubItem == m_iThreatImpactColumn )
        {
            // Get the threat impact text based on the damage value
            if ( bViral ) {
                _tcsncpy(m_szDisplay, _S(IDS_THREAT_MATRIX_HIGH), MAX_PATH-1);
                pDispInfo->item.pszText = m_szDisplay;
            }
            else {
                DWORD dwDamageValue = m_EZAnomaly.GetDamageFlagAverage();
                if ( dwDamageValue <= LOW_DAMAGE_FLAG_VALUE ) {
                    _tcsncpy(m_szDisplay, _S(IDS_THREAT_MATRIX_LOW), 
                             MAX_PATH-1);
                    pDispInfo->item.pszText = m_szDisplay;
                }
                else if ( dwDamageValue <= MED_DAMAGE_FLAG_VALUE ) {
                    _tcsncpy(m_szDisplay, _S(IDS_THREAT_MATRIX_MED), 
                             MAX_PATH-1);
                    pDispInfo->item.pszText = m_szDisplay;
                }
                else {
                    _tcsncpy(m_szDisplay, _S(IDS_THREAT_MATRIX_HIGH), 
                             MAX_PATH-1);
                    pDispInfo->item.pszText = m_szDisplay;
                }
            }
        }
        else if ( pDispInfo->item.iSubItem == m_iRepairActionColumn ) {
            // If this is the repaired view or there are no more actions 
            // available display the status of the item. Otherwise show 
            // the next action to take.
            //
            if ( IAxSysListView32_Filter_Repaired == m_eFilterType ) {
                UINT nResID = 0;
                // Check if this is an ignored and resolved item
                if( eStatus == InfectionStatus_Unhandled && 
                    eAction == ACTION_COMPLETE ) {
                    nResID = IDS_ACTION_IGNORED;
                }
                else {
                    nResID = CAnomalyHelper::GetStatusVerb(eStatus, bViral);
                }

                _tcsncpy(m_szDisplay, _S(nResID), MAX_PATH-1);
                         pDispInfo->item.pszText = m_szDisplay;
            }
            else {
                AnomalyAction eRecommended;
                int iActionIndex = eAction;

                if( m_EZAnomaly.GetRecommendedAction(eRecommended) 
                    &&
                    eRecommended == eAction ) {
                    pDispInfo->item.pszText = pszActions[eAction + 
                                                         RECOMMEND_CONSTANT];
                }
                else {
                    pDispInfo->item.pszText = pszActions[eAction];
                }

                pDispInfo->item.lParam = eAction;
                m_ctlSysListView32.SetItemData(pDispInfo->item.iItem, eAction);
            }
        }
    }
    
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnMRGetDispInfo( int nCtrlID, LPNMHDR pNMHdr, 
                                           BOOL& bHandled )
{
    NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHdr);

    std::vector<ccEraser::IAnomalyPtr>* pVec = NULL;

    pVec = &m_vListRemaining;

    // Make sure we have at least 1 item in the infection vector and what the 
    // ListViewCtrl is asking for is within the infection vector
    if( pVec->empty() )
    {
        CCTRACEI(_T("CAxSysListView32::OnMRGetDispInfo() - The vector of items is empty."));
        return 0;
    }

    if( pDispInfo->item.iItem >= (int)pVec->size() )
    {
        CCTRACEE(_T("CAxSysListView32::OnMRGetDispInfo() - Item %d is out of range."), pDispInfo->item.iItem);
        return 0;
    }

    // Clear old text
    ZeroMemory(m_szDisplay, MAX_PATH);

    m_EZAnomaly = pVec->at(pDispInfo->item.iItem);

    AnomalyAction eAction;
    CommonUIInfectionStatus eStatus;
    m_EZAnomaly.GetAnomalyStatus(eAction, eStatus);

    if( pDispInfo->item.mask & LVIF_TEXT )
    {
        cc::IStringPtr pString;

        if (pDispInfo->item.iSubItem == m_iThreatNameColumn)
        {
            if( m_EZAnomaly.GetName(pString) )
            {
                _tcsncpy(m_szDisplay, pString->GetStringA(), MAX_PATH-1);
                pDispInfo->item.pszText = m_szDisplay;
            }
            else
            {
                CCTRACEE(_T("CAxSysListView32::OnGetDispInfo() - Failed to get threat name for item %d."), pDispInfo->item.iItem);
            }
        }
        else if( pDispInfo->item.iSubItem == m_iRepairActionColumn )
        {
            _tcsncpy(m_szDisplay, _S(IDS_VIEW_REMOVAL_INSTRUCTIONS), 
                     MAX_PATH-1);

            pDispInfo->item.pszText = m_szDisplay;
        }
    }
    
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnColumnClick(int nCtrlID, LPNMHDR pNMHdr, 
                                        BOOL& bHandled)
{

    LPNMLISTVIEW pListView = reinterpret_cast<LPNMLISTVIEW>(pNMHdr);

    // Check if users click on the header

    if (pListView->iItem == -1)
    {
        CCTRACEI(_T("CAxSysListView32::OnColumnClick() - Column %d header clicked"), pListView->iSubItem);

        std::vector<ccEraser::IAnomalyPtr>* pVec = NULL;
        if( IAxSysListView32_Filter_Infected == m_eFilterType )
        {
            pVec = &m_vListRemaining;
        }
        else
        {
            pVec = &m_vListRepaired;
        }

        if (pListView->iSubItem == m_iThreatNameColumn)
        {
            // Clicked on the threat name column
            //
            // Must keep tracking of the ascending/descending change
            // separately if this is the manual removal window.
            //
            if (m_bManualRemoval) { 
                m_bThreatNameAscending = !m_bThreatNameAscending;
                g_bThreatNameAscending = m_bThreatNameAscending;
            }
            else {
                g_bThreatNameAscending = !g_bThreatNameAscending;
            }

            CCTRACEI(_T("CAxSysListView32::OnColumnClick() - Sorting by %s risk name"), g_bThreatNameAscending ? "ascending" : "descending");

            sort(pVec->begin(), pVec->end(), Compare_ByThreatName);

            //
            // Must undo the change to the global if this is the manual
            // removal window.
            // 
            if (m_bManualRemoval) {
                g_bThreatNameAscending = !g_bThreatNameAscending;
            }

            m_ctlSysListView32.SetItemCount( pVec->size() );
        }
        else if( pListView->iSubItem == m_iThreatImpactColumn )
        {
            g_bThreatImpactAscending = !g_bThreatImpactAscending;

            CCTRACEI(_T("CAxSysListView32::OnColumnClick() - Sorting by %s risk level"), g_bThreatImpactAscending ? "ascending" : "descending");

            sort(pVec->begin(), pVec->end(), Compare_ByThreatImpact);
            m_ctlSysListView32.SetItemCount( pVec->size() );
        }
        else if( pListView->iSubItem == m_iThreatTypeColumn )
        {
            g_bThreatTypeAscending = !g_bThreatTypeAscending;

            CCTRACEI(_T("CAxSysListView32::OnColumnClick() - Sorting by %s risk type"), g_bThreatTypeAscending ? "ascending" : "descending");

            sort(pVec->begin(), pVec->end(), Compare_ByThreatType);
            m_ctlSysListView32.SetItemCount( pVec->size() );
        }
        else if( pListView->iSubItem == m_iThreatWarningColumn )
        {
            g_bThreatWarningAscending = !g_bThreatWarningAscending;

            CCTRACEI(_T("CAxSysListView32::OnColumnClick() - Sorting by %s risk warning"), g_bThreatWarningAscending ? "ascending" : "descending");

            sort(pVec->begin(), pVec->end(), Compare_ByThreatWarning);
            m_ctlSysListView32.SetItemCount( pVec->size() );
        }
        else if( pListView->iSubItem == m_iRepairActionColumn )
        {
            g_bThreatStatusAscending = !g_bThreatStatusAscending;

            CCTRACEI(_T("CAxSysListView32::OnColumnClick() - Sorting by %s repair action"), g_bThreatStatusAscending ? "ascending" : "descending");

            if ( IAxSysListView32_Filter_Infected == m_eFilterType ) {
                sort(pVec->begin(), pVec->end(), Compare_ByAction);
            }
            else {
                sort(pVec->begin(), pVec->end(), Compare_ByThreatStatus);
            }

            m_ctlSysListView32.SetItemCount( pVec->size() );
        }

        Fire_ItemChanged();
    }

    return 0;
}



//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnItemChanged(int nCtrlID, LPNMHDR pNMHdr, 
                                        BOOL& bHandled)
{
    //
    // Not sure why anything more than Fire_ItemChanged() is necessary below.
    // But when an list item is selected, then the parent window becomes no
    // longer the active window, then the parent window is restored, and
    // then a new list item is selected, the *old* selected item is not
    // redrawn.  Thus two different items in the list appear highlighted.
    //
    // Don't know why that happens, but the below fixes it by forcing the
    // old item to redraw.
    //
    LPNMLISTVIEW lpNMListView = (LPNMLISTVIEW) pNMHdr;

    int iItem = lpNMListView->iItem;

    Fire_ItemChanged();

    if (iItem >= 0) {
       m_ctlSysListView32.RedrawItems(iItem, iItem);
    }

    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnHeaderBeginTrack(int idCtrl, LPNMHDR pnmh, 
                                             BOOL& bHandled)
{
    // 
    // Don't let the kludge dummy first column resize.
    //
    if (0 == ((LPNMHEADER)pnmh)->iItem) 
    {
        bHandled = TRUE;
        return 1;
    }
    
    bHandled = FALSE;
    return FALSE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnDividerDblClick(int idCtrl, LPNMHDR pnmh, 
                                            BOOL& bHandled)
{
    // 
    // Don't let the kludge dummy first column auto-resize.
    //
    if (0 == ((LPNMHEADER)pnmh)->iItem) 
    {
        bHandled = TRUE;
        return 1;
    }
    
    bHandled = FALSE;
    return FALSE;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnLvComboItemActivate(int nCtrlID, LPNMHDR pNMHdr, 
                                                BOOL& bHandled)
{
    //
    // Populate list item vector with the appropriate items for this 
    // list view item.
    //
    CCustomCellListView::LPLVCOMBOITEMACTIVATE plvcmbActivate = 
                          (CCustomCellListView::LPLVCOMBOITEMACTIVATE) pNMHdr;

    int iItem = plvcmbActivate->itemActivate.iItem;
    int iSubItem = plvcmbActivate->itemActivate.iSubItem;

    DWORD_PTR dwData = (DWORD_PTR)plvcmbActivate->itemActivate.lParam;
    

    std::vector<ccEraser::IAnomalyPtr>* pVec = NULL;

    if ( IAxSysListView32_Filter_Infected == m_eFilterType ) {
        pVec = &m_vListRemaining;
    }
    else {
        pVec = &m_vListRepaired;
    }

    // Make sure we have at least 1 item in the infection vector 
    if ( pVec->empty() ) {
        CCTRACEI(_T("CAxSysListView32::OnLVComboItemActivate() - The vector of items is empty."));
        return 0;
    }

    if ( iItem >= (int)pVec->size() ) {
        CCTRACEE(_T("CAxSysListView32::OnLVComboItemActivate() - Item %d is out of range."), iItem);
        return 0;
    }

    // Clear old text
    ZeroMemory(m_szDisplay,MAX_PATH);

    m_EZAnomaly = pVec->at(iItem);

    AnomalyAction eAction;
    CommonUIInfectionStatus eStatus;

    if ( !m_EZAnomaly.GetAnomalyStatus(eAction, eStatus) ) {
        CCTRACEE(_T("AxSysListView32::OnLvComboItemActivate() - Failed to anomaly status"));
        eStatus = InfectionStatus_Unhandled;
        eAction = ACTION_NOOPERATIONSAVAILABLE;        
    } 

    plvcmbActivate->nCurrentSelection = eAction;

    if ( IAxSysListView32_Filter_Repaired == m_eFilterType ) {
        UINT nResID;

        // Check if this is an ignore once item
        if( eStatus == InfectionStatus_Unhandled && 
            eAction == ACTION_COMPLETE ) {
            nResID = IDS_ACTION_IGNORED;
        }
        else {
            nResID = CAnomalyHelper::GetStatusVerb(eStatus, 
                                                   m_EZAnomaly.IsViral());
        }

        plvcmbActivate->
                listItems.push_back( new CCustomCellSelectListItem( _S(nResID),
                                                                    eAction ));
        return 0;
    }

    AnomalyAction eRecommended;

    if ( !m_EZAnomaly.GetRecommendedAction(eRecommended) ) {
        CCTRACEE(_T("AxSysListView32::OnLvComboItemActivate() - Failed to get recommended action"));
        eRecommended = ACTION_NOOPERATIONSAVAILABLE;        
    }

    plvcmbActivate->
        listItems.push_back( 
                new CCustomCellSelectListItem( pszActions[eRecommended + 
                                                          RECOMMEND_CONSTANT],
                                               eRecommended ) );

    switch (eRecommended) {
        case ACTION_REPAIR:
            // Don't allow delete or quarantine action for boot record or 
            // MBR infections, nor for infections that ccEraser marked as
            // being RepairOnly.
            if ( !m_EZAnomaly.IsBootRecordAnomaly() && 
                 !m_EZAnomaly.IsMBRAnomaly() &&
                 !m_EZAnomaly.IsRepairOnlyAnomaly() ) {
                plvcmbActivate->
                    listItems.push_back( 
                          new CCustomCellSelectListItem( QUARANTINE_ACTION,
                                                         ACTION_QUARANTINE ) );

                plvcmbActivate->
                    listItems.push_back( 
                              new CCustomCellSelectListItem( REMOVE_ACTION,
                                                             ACTION_DELETE ) );
            }

            break;

        case ACTION_QUARANTINE:
            plvcmbActivate->
                listItems.push_back( 
                              new CCustomCellSelectListItem( REMOVE_ACTION,
                                                             ACTION_DELETE ) );
            break;

        case ACTION_EXCLUDE:
            if (InfectionStatus_CantDelete != eStatus && 
                InfectionStatus_Delete_Failed != eStatus ) {
                plvcmbActivate->
                    listItems.push_back( 
                              new CCustomCellSelectListItem( REMOVE_ACTION,
                                                             ACTION_DELETE ) );
            }
            break;

        case ACTION_DELETE:
        case ACTION_REVIEW:

            // Delete and review recommendatiosn could be viral or 
            // non-viral.  Additional choices besides review are
            // only possible for non-virals.
            //
            if ( !m_EZAnomaly.IsViral() ) 
            {
                // If this was a review recommendation due to a delete
                // failing on quarantine, then allow delete option.
                if ( ACTION_REVIEW == eRecommended && 
                     InfectionStatus_Quarantine_Failed == eStatus )
                {
                    plvcmbActivate->
                        listItems.push_back( 
                              new CCustomCellSelectListItem( REMOVE_ACTION,
                                                             ACTION_DELETE ) );
                }

                // Only allow exclude option on low or medium risks in 
                // non-email scans. 
                if ( !m_bEmailScan && 
                     m_EZAnomaly.GetDamageFlagAverage() <= 
                                                        MED_DAMAGE_FLAG_VALUE ) 
                {
                    plvcmbActivate->
                        listItems.push_back( 
                             new CCustomCellSelectListItem( EXCLUDE_ACTION,
                                                            ACTION_EXCLUDE ) );
                }
            }

        default:
            break;
    }

    //
    // Ignore is the last option for medium and low non-virals, never 
    // recommended.
    //
    if ( ACTION_IGNORE != eRecommended ) {
        if ( !m_EZAnomaly.IsViral() &&
             m_EZAnomaly.GetDamageFlagAverage() <= MED_DAMAGE_FLAG_VALUE ) {
            plvcmbActivate->
                listItems.push_back( 
                             new CCustomCellSelectListItem( IGNORE_ACTION,
                                                            ACTION_IGNORE ) );
        }
    }
    else {
        CCTRACEE(_T("AxSysListView32::OnLvComboItemActivate() - Recommended action is ignore"));
    }

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnLvComboItemChange(int nCtrlID, LPNMHDR pNMHdr, 
                                              BOOL& bHandled)
{
    CCustomCellListView::LPLVCOMBOITEMCHANGE plvcmbChange = 
                             (CCustomCellListView::LPLVCOMBOITEMCHANGE) pNMHdr;
    
    int iItem = plvcmbChange->LVItem.iItem;
    int iSubItem = plvcmbChange->LVItem.iSubItem;
    DWORD_PTR dwData = (DWORD_PTR)plvcmbChange->LVItem.lParam;

    if ((AnomalyAction) plvcmbChange->LBItem.lParam < 0) {
        return 0;
    }

    std::vector<ccEraser::IAnomalyPtr>* pVec = NULL;
    if( IAxSysListView32_Filter_Infected == m_eFilterType )
    {
        pVec = &m_vListRemaining;
    }
    else
    {
        pVec = &m_vListRepaired;
    }

    // Make sure we have at least 1 item in the infection vector and that the 
    // ListViewCtrl is asking for is within the infection vector
    if( pVec->empty() )
    {
        CCTRACEI(_T("CAxSysListView32::OnLvComboItemChange() - The vector of items is empty."));
        return 0;
    }

    if( iItem >= (int)pVec->size() )
    {
        CCTRACEE(_T("CAxSysListView32::OnLvComboItemChange() - Item %d is out of range."), iItem);
        return 0;
    }

    m_EZAnomaly = pVec->at(iItem);

    AnomalyAction eAction;
    CommonUIInfectionStatus eStatus;
    m_EZAnomaly.GetAnomalyStatus(eAction, eStatus);

    m_EZAnomaly.SetAnomalyStatus((AnomalyAction) plvcmbChange->LBItem.lParam, 
                                 eStatus);

    m_ctlSysListView32.SetItemData(iItem, 
                                   (DWORD_PTR) plvcmbChange->LBItem.lParam);

    m_ctlSysListView32.RedrawItems(iItem, iItem);

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnLvHyperlinkActivate(int nCtrlID, LPNMHDR pNMHdr, 
                                                BOOL& bHandled)
{
    //
    // Populate the List Box with the appropriate items for this list view item.
    //
    LPNMITEMACTIVATE plvhActivate = (LPNMITEMACTIVATE)pNMHdr;

    int iItem = plvhActivate->iItem;
    int iSubItem = plvhActivate->iSubItem;

    m_ctlSysListView32.SelectItem(iItem);

    if (m_bManualRemoval) {
        if (iSubItem == m_iRepairActionColumn) {
            Fire_ThreatNameClick();
        }
    }
    else {
        if (iSubItem == m_iThreatNameColumn) {
            Fire_ThreatNameClick();
        }
        else if (iSubItem == m_iThreatImpactColumn) {
            Fire_ThreatRiskClick();
        }
        else if (iSubItem == m_iThreatTypeColumn) {
            Fire_ThreatTypeClick();
        }
    }
 
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CAxSysListView32::PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet)
{
    if(pMsg->message == WM_KEYDOWN)
    {
        switch(pMsg->wParam)
        {
        case VK_LEFT:
        case VK_RIGHT:
        case VK_UP:
        case VK_DOWN:
        case VK_HOME:
        case VK_END:
        case VK_NEXT:
        case VK_PRIOR:
            hRet = S_FALSE;
            return TRUE;
        }
    }
 
    return FALSE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32::SetObjectRects(LPCRECT prcPos,LPCRECT prcClip)
{
    IOleInPlaceObjectWindowlessImpl<CAxSysListView32>::SetObjectRects(prcPos, prcClip);
    
    const int cx = (prcPos->right - prcPos->left);
    const int cy = (prcPos->bottom - prcPos->top);

    m_ctlSysListView32.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CAxSysListView32::OnEnabledChanged()
{
    CCTRACEI(_T("CAxSysListView32::OnEnabledChanged\n"));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CAxSysListView32::FinalConstruct()
{
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CAxSysListView32::FinalRelease() 
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32::put_AnomalyList( VARIANT* newVal )
{
    if( VT_I4 == newVal->vt )
    {
        m_spAnomalyList = NULL;
        m_spAnomalyList = reinterpret_cast<ccEraser::IAnomalyList*>(newVal->lVal);
        ATLASSERT( m_spAnomalyList );

        if( !m_spAnomalyList )
            return E_FAIL;
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32::Initialize(BOOL bEmailScan)
{
    m_bEmailScan = bEmailScan;

    
    if( !m_spAnomalyList )
    {
        CCTRACEE(_T("CAxSysListView32::Initialize() - The anomaly list is null."));
        return E_FAIL;
    }

    // clear our vectors
    m_vListRemaining.clear();
    m_vListRepaired.clear();

    ccEraser::eResult eRes = ccEraser::Success;
    size_t nCount = 0;
    eRes = m_spAnomalyList->GetCount(nCount);
    if( ccEraser::Failed(eRes) )
    {
        CCTRACEE(_T("CAxSysListView32::Initialize() - Failed to get the anomaly list count. eResult = %d"), eRes);
        return E_FAIL;
    }

    if( nCount > 0 )
    {
        CCTRACEI(_T("CAxSysListView32::Initialize() - There are %d anomalies in the list."), nCount);
        // Get each anomaly
        for( size_t nCur = 0; nCur < nCount; nCur++ )
        {
            ccEraser::IAnomalyPtr pAnomaly;
            eRes = m_spAnomalyList->GetItem(nCur, pAnomaly);
            if( ccEraser::Succeeded(eRes) )
            {
                m_EZAnomaly = pAnomaly;
                AnomalyAction eAction;
                CommonUIInfectionStatus eStatus;
                m_EZAnomaly.GetAnomalyStatus(eAction, eStatus);

                CCTRACEI(_T("CAxSysListView32::Initialize() - Action to take for anomaly %d is %d"), nCur, eAction);
                
                if (m_bManualRemoval) {
                   if ( ACTION_REVIEW == eAction) {
                      m_vListRemaining.push_back(pAnomaly);
                   } 
                }
                else {
                    // If the action to take is complete then this goes in 
                    // the repaired list otherwise it goes in the remaining 
                    // list
                    if( eAction == ACTION_COMPLETE ) {
                    m_vListRepaired.push_back(pAnomaly);
                }
                    else {
                    m_vListRemaining.push_back(pAnomaly);
                }
            }
            }
            else
            {
                CCTRACEE(_T("CAxSysListView32::Initialize() - Failed to get anomaly %d from the list. eResult = %d"), nCur, eRes);
            }
        }

        // Sort by risk impact initially
        CCTRACEI("CAxSysListView32::Initialize() - Doing initial sort by risk level");
        g_bThreatImpactAscending = true;

        sort(m_vListRemaining.begin(), m_vListRemaining.end(), 
             Compare_ByThreatImpact);
        sort(m_vListRepaired.begin(), m_vListRepaired.end(), 
             Compare_ByThreatImpact);
    }
    else
    {
        CCTRACEW(_T("CAxSysListView32::Initialize() - No Anomalies in the list"));
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32::put_Filter(LONG newVal)
{
    switch( newVal )
    {
    case IAxSysListView32_Filter_Infected:
        {
            const DWORD dwLVExStyle = 
                             ( m_ctlSysListView32.GetExtendedListViewStyle() | 
                               LVS_EX_FULLROWSELECT );
            m_ctlSysListView32.SetExtendedListViewStyle( dwLVExStyle );
 
            m_eFilterType = IAxSysListView32_Filter_Infected;
            m_ctlSysListView32.SetItemCount( m_vListRemaining.size() );

            if ( m_bManualRemoval ) {
                m_ctlSysListView32.SetColumnType( 
                                      m_iRepairActionColumn,
                                      CCustomCellListView::CELLTYPE_HYPERLINK);
            }
            else {
                m_ctlSysListView32.SetColumnType( 
                                       m_iRepairActionColumn,
                                       CCustomCellListView::CELLTYPE_COMBOBOX);
            }

            Fire_ItemChanged();

            for ( unsigned int counter = 0; counter < m_vListRemaining.size();
                  counter++) {
                CEZAnomaly ezAnomaly( m_vListRemaining.at(counter) );

                if ( !ezAnomaly.IsViral() ) {
                    AnomalyAction eNextAction;
                    CommonUIInfectionStatus eStatus = 
                                                     InfectionStatus_Unhandled;

                    if ( !ezAnomaly.GetAnomalyStatus(eNextAction, eStatus) ) {
                         CCTRACEE( _T("CAxSysListView32::GetWarning() - Failed to get the status values for anomaly %d\n"), counter );
                    }

                    if ( InfectionStatus_CantDelete != eStatus &&
                         InfectionStatus_Delete_Failed != eStatus ) {
                        Fire_DisplayQuarantineNote();

                        break;
                    }
                }
            }
        }
        break;

    case IAxSysListView32_Filter_Repaired:
        {
            ATLASSERT(!m_bManualRemoval);

            m_ctlSysListView32.SetView(LV_VIEW_DETAILS);
            m_ctlSysListView32.SetCallbackMask( 0 );
 
            m_eFilterType = IAxSysListView32_Filter_Repaired;

            m_ctlSysListView32.SetItemCount( m_vListRepaired.size() );

            m_ctlSysListView32.SetColumnType( 
                                        m_iRepairActionColumn,
                                        CCustomCellListView::CELLTYPE_NORMAL);

            Fire_ItemChanged();

            for ( unsigned int counter = 0; counter < m_vListRepaired.size();
                  counter++ ) {
                CEZAnomaly ezAnomaly( m_vListRepaired.at(counter) );

                if ( !ezAnomaly.IsViral() ) {
                    AnomalyAction eAction;
                    CommonUIInfectionStatus eStatus = InfectionStatus_Unhandled;

                    if ( !ezAnomaly.GetAnomalyStatus(eAction, eStatus) ) {
                        CCTRACEE( _T("CAxSysListView32::put_Filter() - Failed to get the status values for anomaly %d\n"), counter );
                    }

                    if ( InfectionStatus_Deleted == eStatus ) {
                        Fire_DisplayQuarantineNote();
                        break;
                    }
                }
            }
        }
        break;

    default: 
        ATLASSERT(FALSE);
        break;
    }

    m_ctlSysListView32.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );

    SCROLLBARINFO scrollBarInfo;

    scrollBarInfo.cbSize = sizeof(SCROLLBARINFO);

    //
    // Check to see if the scroll bar is absent, and if it is, see if 
    // there's any unused space as a result.  If there is, expand the 
    // name column to fill in that space.
    //
    if (GetScrollBarInfo(m_ctlSysListView32, OBJID_VSCROLL, &scrollBarInfo) &&
        scrollBarInfo.rgstate[0] & STATE_SYSTEM_INVISIBLE) {
        CRect rectClient;
        GetClientRect( rectClient );

        int nExpandWidth = 0;

        if (m_bManualRemoval) {
            nExpandWidth = 
                    rectClient.Width() -
                    m_ctlSysListView32.GetColumnWidth( m_iThreatNameColumn ) -
                    m_ctlSysListView32.GetColumnWidth( m_iRepairActionColumn ); 
        }
        else {
            nExpandWidth = 
                  rectClient.Width() -
                  m_ctlSysListView32.GetColumnWidth( m_iThreatNameColumn ) -
                  m_ctlSysListView32.GetColumnWidth( m_iThreatImpactColumn ) -
                  m_ctlSysListView32.GetColumnWidth( m_iThreatWarningColumn ) -
                  m_ctlSysListView32.GetColumnWidth( m_iThreatTypeColumn ) -
                  m_ctlSysListView32.GetColumnWidth( m_iRepairActionColumn);
        }
                          
        if (nExpandWidth > 0) {
            m_ctlSysListView32.SetColumnWidth ( 
                  m_iThreatNameColumn,
                  ( m_ctlSysListView32.GetColumnWidth( m_iThreatNameColumn ) +
                    nExpandWidth ) );
        }
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnPaint(UINT uMsg, WPARAM wParam, 
                                  LPARAM lParam, BOOL& bHandled)
{
    //
    // [CBROWN] Code used to remove redraw flicker....
    //
    CPaintDC dcTmp( m_hWnd ); // device context for painting

    CRect rectClient;
    GetClientRect(rectClient);

    CDCOS dc( &dcTmp, rectClient );
    dc.FillSolidRect( rectClient, ::GetSysColor(COLOR_WINDOW) );

    DefWindowProc(WM_PRINTCLIENT, (WPARAM)dc.m_hDC, 0);
    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam,
                                       BOOL& bHandled)
{
    bHandled = TRUE;
    return TRUE;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32::get_Anomaly( VARIANT* pVal )
{
    HRESULT hr = E_FAIL;

    std::vector<ccEraser::IAnomalyPtr>* pVec = 
        (IAxSysListView32_Filter_Infected == m_eFilterType) ? 
                                                           &m_vListRemaining : 
                                                           &m_vListRepaired;
 
    const int nIdx = m_ctlSysListView32.GetSelectedIndex();

    if( LB_ERR != nIdx && nIdx < (int)pVec->size() )
    {
        ccEraser::IAnomalyPtr ptrAnomaly = pVec->at(nIdx);
        _variant_t var = (long) reinterpret_cast<DWORD>(ptrAnomaly.m_p);

        *pVal = var;
        hr = S_OK;
    }

    return hr;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32::Refresh(void)
{
    if( m_ctlSysListView32.IsWindow() )
    {
        const int nIdx = m_ctlSysListView32.GetSelectedIndex();
        m_ctlSysListView32.RedrawItems(nIdx, nIdx);
    }

    return S_OK;
}

// non-static
void CAxSysListView32::GetWarning(const CEZAnomaly & ezAnom, 
                                  CString & strWarning,
                                  const bool bPastTense)
{
    CEZAnomaly ezAnomaly = (CEZAnomaly) ezAnom;

    AnomalyAction eNextAction;
    CommonUIInfectionStatus eStatus = InfectionStatus_Unhandled;

    if (!ezAnomaly.GetAnomalyStatus(eNextAction, eStatus)) {
        CCTRACEE(_T("CAxSysListView32::GetWarning() - Failed to get the status values\n"));
    }

    if ( InfectionStatus_CantDelete == eStatus ||
         InfectionStatus_Delete_Failed == eStatus ) {
        strWarning.LoadString( IDS_MANUAL_REMOVAL_WARNING );
    }
    else if (ezAnomaly.KnownToHaveDependencies()) {
        if (bPastTense && (InfectionStatus_Quarantined == eStatus ||
                           InfectionStatus_Repaired == eStatus || 
                           InfectionStatus_Deleted == eStatus)) {
            strWarning.LoadString( IDS_DEPENDENCY_WARNING_PAST_TENSE );
        }
        else { 
            strWarning.LoadString( IDS_DEPENDENCY_WARNING );
        }
    }
    else if ( !ezAnomaly.IsViral() && 
              InfectionStatus_Quarantine_Failed == eStatus )
    {
        strWarning.LoadString( IDS_CANT_RESTORE_WARNING );
    }
    else {
        strWarning.Empty();
    }
}

// static
void CAxSysListView32::GetWarning(const CEZAnomaly & ezAnom, 
                                  CString & strWarning)
{
    CEZAnomaly ezAnomaly = (CEZAnomaly) ezAnom;

    AnomalyAction eNextAction;
    CommonUIInfectionStatus eStatus = InfectionStatus_Unhandled;

    if (!ezAnomaly.GetAnomalyStatus(eNextAction, eStatus)) {
        CCTRACEE(_T("CAxSysListView32::GetWarning() - Failed to get the status values\n"));
    }

    if ( InfectionStatus_CantDelete == eStatus ||
         InfectionStatus_Delete_Failed == eStatus ) {
        g_ResLoader.LoadString(IDS_MANUAL_REMOVAL_WARNING, strWarning);
    }
    else if (ezAnomaly.KnownToHaveDependencies()) {
        g_ResLoader.LoadString(IDS_DEPENDENCY_WARNING, strWarning);
    }
    else if ( !ezAnomaly.IsViral() && 
              InfectionStatus_Quarantine_Failed == eStatus )
    {
        strWarning.LoadString( IDS_CANT_RESTORE_WARNING );
    }
    else {
        strWarning.Empty();
    }
}
