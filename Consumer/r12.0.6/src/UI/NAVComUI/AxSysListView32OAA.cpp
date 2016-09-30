//******************************************************************************
//******************************************************************************
#include "stdafx.h"
#include "AxSysListView32OAA.h"
#include "CDCOS.h"
#include "AnomalyHelper.h"

#pragma comment(lib, "comctl32.lib")

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CAxSysListView32OAA::CAxSysListView32OAA() : 
    m_ctlSysListView32(_T("SysListView32"), this, 1),
    m_lControlMode(0),
    m_iDescriptionColumn(-1),
    m_pStats(NULL)
{
    m_bWindowOnly = TRUE;
    _tcscpy(m_szDisplay, _T(""));
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
CAxSysListView32OAA::~CAxSysListView32OAA() 
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
BOOL CAxSysListView32OAA::PreTranslateAccelerator(LPMSG pMsg, HRESULT& hRet)
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
LRESULT CAxSysListView32OAA::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    LRESULT lRes = CComControl<CAxSysListView32OAA>::OnSetFocus(uMsg, wParam, lParam, bHandled);
    if( m_bInPlaceActive )
    {
        if(!IsChild(::GetFocus()))
            m_ctlSysListView32.SetFocus();
    }

    return lRes;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32OAA::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    RECT rc;
    GetWindowRect(&rc);
    rc.right -= rc.left;
    rc.bottom -= rc.top;
    rc.top = rc.left = 0;
    InitCommonControls();

    const DWORD style = WS_CHILD | WS_VISIBLE | LVS_OWNERDATA | LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS | WS_TABSTOP;

    m_ctlSysListView32.Create(m_hWnd, rc, NULL, style );

    const DWORD dwLVExStyle = (m_ctlSysListView32.GetExtendedListViewStyle() | LVS_EX_FULLROWSELECT);
    m_ctlSysListView32.SetExtendedListViewStyle( dwLVExStyle );

    SHFILEINFO shInfo;
    ZeroMemory( &shInfo, sizeof(shInfo) );

    const int nFlags = SHGFI_SYSICONINDEX|SHGFI_USEFILEATTRIBUTES|SHGFI_SMALLICON;
    m_imgListSystem = (HIMAGELIST) SHGetFileInfo(_T("C:\\"), FILE_ATTRIBUTE_NORMAL, &shInfo, sizeof(shInfo), nFlags );

    // Set the ListViewCtrl's state image list
    m_ctlSysListView32.SetImageList(m_imgListSystem, LVSIL_SMALL);

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CAxSysListView32OAA::InitListViewColumns()
{
    if( IAxSysListView32OAA_Mode_Top == m_lControlMode )
    {
        // Insert a single column
        CRect Rect;
        m_ctlSysListView32.GetClientRect(&Rect);

        LVCOLUMN lvColumn;
        int iOrder = 0;
        ZeroMemory( &lvColumn, sizeof(lvColumn) );
        lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = Rect.right;
        _tcsncpy(m_szDisplay, _S(IDS_OTHERAREA_AREA_COLUMN_HEADER), MAX_PATH);
        lvColumn.pszText = m_szDisplay;
        lvColumn.cchTextMax = 5;
        lvColumn.iOrder = iOrder;
        m_ctlSysListView32.InsertColumn( iOrder++, &lvColumn );
    }
    else if( IAxSysListView32OAA_Mode_Bottom == m_lControlMode )
    {
        // Insert two columns for the bottom control
        CRect Rect;
        m_ctlSysListView32.GetClientRect(&Rect);
        Rect.right -= GetSystemMetrics(SM_CXHSCROLL);

        LVCOLUMN lvColumn;
        int iOrder = 0;
        ZeroMemory( &lvColumn, sizeof(lvColumn) );

        CommonUIInfectionStatus eStatus = InfectionStatus_Unhandled;
        AnomalyAction eAction = ACTION_REPAIR;
        m_EZAnomaly.GetAnomalyStatus(eAction, eStatus);

        // Description column
        lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
        lvColumn.fmt = LVCFMT_LEFT;
        lvColumn.cx = Rect.right;

        _tcsncpy(m_szDisplay, _S(IDS_OTHERAREA_DETAIL_COLUMN_HEADER), MAX_PATH);
        lvColumn.pszText = m_szDisplay;

        lvColumn.cchTextMax = 11;
        lvColumn.iOrder = iOrder;
        m_iDescriptionColumn = iOrder;
        m_ctlSysListView32.InsertColumn( iOrder++, &lvColumn );
    }

    
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32OAA::OnGetDispInfo( int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled )
{
    NMLVDISPINFO* pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHdr);

    if( pDispInfo->item.mask & LVIF_TEXT )
    {
        if( IAxSysListView32OAA_Mode_Top == m_lControlMode )
        {
            // Validate index
            if( (int)m_vTopItems.size() > pDispInfo->item.iItem )
            {
                _tcsncpy(m_szDisplay, m_vTopItems[pDispInfo->item.iItem].first, MAX_PATH-1);
                pDispInfo->item.pszText = m_szDisplay;
            }
            else
                CCTRACEE(_T("CAxSysListView32OAA::OnGetDispInfo() - %d is out of range for the top control."), pDispInfo->item.iItem);
        }
        else if( IAxSysListView32OAA_Mode_Bottom == m_lControlMode )
        {
            // Validate index
            if( (int)m_vBottomItems.size() > pDispInfo->item.iItem )
            {
                if( pDispInfo->item.iSubItem == m_iDescriptionColumn )
                {
                    // Special case for SLFN paths so we don't cut off the file name
                    if( m_vBottomItems[pDispInfo->item.iItem].first.GetLength() > MAX_PATH )
                    {
                        // Get the file name component.
                        ATL::CAtlString strFileNameTemp = m_vBottomItems[pDispInfo->item.iItem].first;
                        int nSlashPos = strFileNameTemp.ReverseFind(_TCHAR('\\'));

                        if( nSlashPos != -1  && nSlashPos != strFileNameTemp.GetLength() )
                        {
                            strFileNameTemp = strFileNameTemp.Right(strFileNameTemp.GetLength() - (nSlashPos+1));
                            CCTRACEI(_T("CAxSysListView32OAA::OnGetDispInfo() - SLFN path file name component is %s"),(LPCTSTR)strFileNameTemp);

                            // Copy the first MAX_PATH - (length of file name + 5) characters to the display buffer
                            // the +5 comes from our "..." + 1 for the NULL character +1 because we're only copying MAX_PATH-1 chars
                            int nCharsToCopy = MAX_PATH - (strFileNameTemp.GetLength()+5);
                            if( nCharsToCopy > 0 )
                            {
                                ZeroMemory(m_szDisplay, MAX_PATH);
                                _tcsncpy(m_szDisplay, m_vBottomItems[pDispInfo->item.iItem].first.GetBufferSetLength(nCharsToCopy), nCharsToCopy );

                                // Now add the "..."
                                _tcscat(m_szDisplay, _T("..."));

                                CCTRACEI(_T("CAxSysListView32OAA::OnGetDispInfo() - ... path is %s. Length = %d"),m_szDisplay, _tcslen(m_szDisplay));

                                // Re-assign the display string to the modified SLFN path
                                m_vBottomItems[pDispInfo->item.iItem].first = m_szDisplay;
                                m_vBottomItems[pDispInfo->item.iItem].first += strFileNameTemp;
                            }
                        }
                    }

                    // Don't want to give more than max path chars to the control
                    ZeroMemory(m_szDisplay, MAX_PATH);
                    _tcsncpy(m_szDisplay, m_vBottomItems[pDispInfo->item.iItem].first, MAX_PATH-1);

                    pDispInfo->item.pszText = m_szDisplay;
                }
            }
            else
                CCTRACEE(_T("CAxSysListView32OAA::OnGetDispInfo() - %d is out of range for the bottom control."), pDispInfo->item.iItem);
        }
    }

    // If the ListViewCtrl is asking for the icon
    if( pDispInfo->item.mask & LVIF_IMAGE )
    {
        // Using system icons
        SHFILEINFO shf;
        ZeroMemory(&shf, sizeof(SHFILEINFO));

        AffectedTypes typeTemp = typeFile;

        if( IAxSysListView32OAA_Mode_Top == m_lControlMode )
        {
            typeTemp = m_vTopItems[pDispInfo->item.iItem].second;
        }
        else if( IAxSysListView32OAA_Mode_Bottom == m_lControlMode )
        {
            typeTemp = m_BottomMode;
        }

        CString strFile;

        switch( typeTemp )
        {
        case typeFile:
        case typeCOM:
            strFile = _T("*.dll");
            break;
        case typeProcess:
        case typeService:
            strFile = _T("*.exe");
            break;
        case typeBR:
        case typeMBR:
            strFile = _T("C:\\");
            break;
        case typeReg:
            strFile = _T("*.reg");
            break;
        case typeBatch:
            strFile = _T("*.bat");
            break;
        case typeINI:
            strFile = _T("*.ini");
            break;
        case typeLSP:
        case typeHosts:
        default:
            strFile = _T("*.dat");
            break;
        }

        SHGetFileInfo(strFile,  FILE_ATTRIBUTE_NORMAL, &shf, sizeof(SHFILEINFO), SHGFI_USEFILEATTRIBUTES | SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
        pDispInfo->item.iImage = shf.iIcon;
    }

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32OAA::OnItemChanged(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled)
{
    Fire_ItemChanged();
    return 0;
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32OAA::SetObjectRects(LPCRECT prcPos,LPCRECT prcClip)
{
    IOleInPlaceObjectWindowlessImpl<CAxSysListView32OAA>::SetObjectRects(prcPos, prcClip);
    
    const int cx = prcPos->right - prcPos->left;
    const int cy = prcPos->bottom - prcPos->top;

    m_ctlSysListView32.SetWindowPos( NULL, 0, 0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HRESULT CAxSysListView32OAA::FinalConstruct()
{
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CAxSysListView32OAA::FinalRelease() 
{
}


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32OAA::put_Anomaly( VARIANT* newVal )
{
    if( VT_I4 == newVal->vt )
    {
        m_EZAnomaly = reinterpret_cast<ccEraser::IAnomaly*>(newVal->lVal);

        // Populate anomaly data
        m_pStats = m_EZAnomaly.GetRemediationStatistics(_S(IDS_UNKNOWN_REMEDIATION_ACTION_TEXT), _S(IDS_DEFAULT_REGISTRY_KEY_TEXT));

        InitListViewColumns();

        InitializeTopControl();
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void CAxSysListView32OAA::InitializeTopControl()
{
    if( !m_pStats )
    {
        return;
    }

    // Set the top control data based on the statistics
    if( IAxSysListView32OAA_Mode_Top == m_lControlMode )
    {
        AFFECTEDTYPESPAIR typeData;

        for( int nCurType = typeFirst; nCurType <= typeLast ; nCurType++ )
        {
            DWORD dwCount = m_pStats->GetRemediationCount(static_cast<AffectedTypes>(nCurType));
            if( dwCount == 0 )
                continue;

            UINT nResID;
            switch(nCurType)
            {
            case typeFile:
                nResID = (1==dwCount) ? IDS_OTHERAREA_FILE : IDS_OTHERAREA_FILES;
                break;
            case typeProcess:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_PROC : IDS_OTHERAREAS_PROCS;
                break;
            case typeBR:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_BR : IDS_OTHERAREAS_BRS;
                break;
            case typeMBR:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_MBR : IDS_OTHERAREAS_MBRS;
                break;
            case typeReg:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_REGKEY : IDS_OTHERAREAS_REGKEYS;
                break;
            case typeBatch:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_ONE_BATCH : IDS_OTHERAREAS_BATCH;
                break;
            case typeINI:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_ONE_INI : IDS_OTHERAREAS_INI;
                break;
            case typeService:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_SERVICE : IDS_OTHERAREAS_SERVICES;
                break;
            case typeCOM:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_ONE_COM : IDS_OTHERAREAS_COM;
                break;
            case typeHosts:
                nResID = (1==dwCount) ? IDS_OTHERAREA_HOST : IDS_OTHERAREA_HOSTS;
                break;
            case typeLSP:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_LSP : IDS_OTHERAREAS_LSPS;
                break;
            case typeUnkown:
                nResID = (1==dwCount) ? IDS_OTHERAREAS_ONE_UNKNOWN : IDS_OTHERAREAS_PLURAL_UNKOWN;
                break;
            default:
                CCTRACEE(_T("CAxSysListView32OAA::OnCreate() - Unknown affected area type. %d"), typeData.second);
                continue;
            }

            typeData.second = static_cast<AffectedTypes>(nCurType);
            typeData.first.Format(nResID, dwCount);
            m_vTopItems.push_back(typeData);
        }

        // Set the count
        m_ctlSysListView32.SetItemCount(m_vTopItems.size());

        // Select the first item.
        m_ctlSysListView32.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );

        // Fire the item changed event so the bottom window updates to the appropriate type
        Fire_ItemChanged();
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32OAA::get_AffectedType(ULONG* pVal)
{
    // This should only be called for the top mode
    if( IAxSysListView32OAA_Mode_Bottom == m_lControlMode )
    {
        ASSERT(FALSE);
        return S_FALSE;
    }

    // Get the selected row
    size_t nPos = m_ctlSysListView32.GetSelectedIndex();

    if( m_vTopItems.size() > nPos )
    {
        // Return the type define for this row
        *pVal = (m_vTopItems[nPos]).second;
    }
    else
    {
        // If the click occurs somewhere where there is no selected index then we do not
        // want the outer OtherAffectedAreasDlg to change the bottom view so return false
        CCTRACEE(_T("CAxSysListView32OAA::get_AffectedType() - %d is out of range."), nPos);
        return S_FALSE;
    }

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32OAA::put_AffectedType(ULONG newVal)
{
    // This should only be called for the bottom mode
    if( IAxSysListView32OAA_Mode_Top == m_lControlMode )
    {
        ASSERT(FALSE);
        return S_FALSE;
    }

    if( !m_pStats )
        return S_FALSE;

    m_BottomMode = static_cast<AffectedTypes>(newVal);

    m_vBottomItems = m_pStats->GetItems(m_BottomMode);

    // Set the count
    m_ctlSysListView32.SetItemCount(m_vBottomItems.size());

    // Select the first item.
    m_ctlSysListView32.SetItemState( 0, LVIS_SELECTED, LVIS_SELECTED );

    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32OAA::get_ControlMode(ULONG* pVal)
{
    *pVal = m_lControlMode;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
STDMETHODIMP CAxSysListView32OAA::put_ControlMode(ULONG newVal)
{
    m_lControlMode = newVal;
    return S_OK;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32OAA::OnCustomDraw(int nCtrlID, LPNMHDR pNMHdr, BOOL& bHandled)
{
    LRESULT pResult;
    LPNMLVCUSTOMDRAW pLVCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHdr);

    // Take the default processing unless we set this to something else below

    pResult = CDRF_DODEFAULT;

    // First thing - check the draw stage. If it's the control's prepaint
    // stage, then tell Windows we want messages for every item.

    return pResult;

}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
LRESULT CAxSysListView32OAA::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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
LRESULT CAxSysListView32OAA::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = TRUE;
    return TRUE;
}
