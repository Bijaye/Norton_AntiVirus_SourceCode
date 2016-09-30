////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"

#include "ccVariantArray.h"
#include "NAVEventCommon.h"

//#include "TestEvent4.h"
//#include "TestEvent5.h"

#include "TestLog.h"
#include "TestLogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace ccLib;
using namespace ccEvtMgr;

CTestLogDlg::CTestLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestLogDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTestLogDlg)
	m_nEventCount = 0;
	m_nEventType = AV_Event_ID_VirusAlert;
	m_nMaxSize = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTestLogDlg)
	DDX_Control(pDX, IDC_EVENT_LIST, m_EventList);
	DDX_Text(pDX, IDC_EVENT_COUNT_EDIT, m_nEventCount);
	DDX_Text(pDX, IDC_EVENT_TYPE_EDIT, m_nEventType);
	DDX_Text(pDX, IDC_MAX_SIZE_EDIT, m_nMaxSize);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestLogDlg, CDialog)
	//{{AFX_MSG_MAP(CTestLogDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, OnOk)
	ON_BN_CLICKED(IDC_CONNECT_BUTTON, OnConnectButton)
	ON_BN_CLICKED(IDC_SEARCH_BUTTON, OnSearchButton)
	ON_BN_CLICKED(IDC_LOAD_BUTTON, OnLoadButton)
	ON_BN_CLICKED(IDC_CLEAR_BUTTON, OnClearButton)
	ON_BN_CLICKED(IDC_SEARCHLOAD_BUTTON, OnSearchloadButton)
	ON_BN_CLICKED(IDC_GET_SIZE_BUTTON, OnGetSizeButton)
	ON_BN_CLICKED(IDC_SET_SIZE_BUTTON, OnSetSizeButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTestLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
    GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(TRUE);
    GetDlgItem(IDC_SEARCH_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_SEARCHLOAD_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_LOAD_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_CLEAR_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_GET_SIZE_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_SET_SIZE_BUTTON)->EnableWindow(FALSE);

    m_EventList.InsertColumn(EventTypeColumn, _T("Type"));
    ResizeColumn(EventTypeColumn, _T("Type"));
    m_EventList.InsertColumn(EventIndexColumn, _T("Index"));
    ResizeColumn(EventIndexColumn, _T("Index"));
    m_EventList.InsertColumn(EventIndexCheckColumn, _T("Index Check"));
    ResizeColumn(EventIndexCheckColumn, _T("Index Check"));
    m_EventList.InsertColumn(EventTimeStampColumn, _T("Time Stamp"));
    ResizeColumn(EventTimeStampColumn, _T("Time Stamp"));
    m_EventList.InsertColumn(EventSequenceColumn, _T("Sequence Number"));
    ResizeColumn(EventSequenceColumn, _T("Sequence Number"));
    m_EventList.InsertColumn(EventValueColumn, _T("Value"));
    ResizeColumn(EventValueColumn, _T("Value"));
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestLogDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

HCURSOR CTestLogDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CTestLogDlg::OnOk() 
{
	// Nothing
}

void CTestLogDlg::OnCancel() 
{
    if (m_piLogManager != NULL)
    {
        m_piLogManager.Release();
    }
	
	CDialog::OnCancel();
}

void CTestLogDlg::OnConnectButton() 
{
    if (m_piLogManager != NULL)
    {
        return;
    }

    HRESULT hRes;
    hRes = m_piLogManager.CreateInstance(__uuidof(EVENTMANAGERLib::LogManager));
    if (FAILED(hRes))
    {
        MessageBox(_T("Failed to connect to Log Manager"));
        return;
    }

    GetDlgItem(IDC_CONNECT_BUTTON)->EnableWindow(FALSE);
    GetDlgItem(IDC_SEARCH_BUTTON)->EnableWindow(TRUE);
    GetDlgItem(IDC_SEARCHLOAD_BUTTON)->EnableWindow(TRUE);
    GetDlgItem(IDC_CLEAR_BUTTON)->EnableWindow(TRUE);
    GetDlgItem(IDC_GET_SIZE_BUTTON)->EnableWindow(TRUE);
    GetDlgItem(IDC_SET_SIZE_BUTTON)->EnableWindow(TRUE);
}

void CTestLogDlg::OnSearchButton() 
{
    if (m_piLogManager == NULL)
    {
        return;
    }

    UpdateData(TRUE);

    long error = CError::eNoError;
    m_EventInfoVector.clear();
    m_nEventCount = 0;
    try
    {
        // Search for events
        DATE dtStart = 0, dtStop = 0;
        long nEventCount = 0, nContextId = 0;
        _variant_t vIndex, vIndexCheck, vTimeStamp, vSequenceNumber;
        error = m_piLogManager->SearchEvents(m_nEventType, 
                                             dtStart, 
                                             dtStop,
                                             VARIANT_FALSE,
                                             0,
                                             &nContextId,
                                             &vIndex,
                                             &vIndexCheck,
                                             &vTimeStamp,
                                             &vSequenceNumber,
                                             &m_nEventCount);
        if (error == CError::eNoError)
        {
            // Convert to vector
            if (Convert(vIndex,
                        vIndexCheck,
                        vTimeStamp,
                        vSequenceNumber,
                        m_EventInfoVector) == FALSE)
            {
                error = CError::eFatalError;
            }
        }
    }
    catch(...)
    {
        error = CError::eFatalError;
    }
    if (error != CError::eNoError)
    {
        m_EventInfoVector.clear();
        m_nEventCount = 0;
        MessageBox(_T("Failed to search for events"));
    }

    GetDlgItem(IDC_LOAD_BUTTON)->EnableWindow(m_nEventCount > 0);

    UpdateData(FALSE);
}

void CTestLogDlg::OnLoadButton() 
{
	if (m_EventInfoVector.size() == 0 ||
        m_piLogManager == NULL)
    {
        return;
    }

    m_EventList.DeleteAllItems();
	
    UpdateData(TRUE);

    long error = CError::eNoError;
    try
    {
        // Load events
        EVENTMANAGERLib::IEventExPtr piEvent;
        int nItem = 0;
        ULARGE_INTEGER uIndex;
        EventInfoVector::iterator pos;
        for (pos = m_EventInfoVector.begin();
             pos != m_EventInfoVector.end();
             pos ++)
        {
            // Load event
            uIndex.QuadPart = pos->m_uIndex;
            ASSERT(uIndex.HighPart == 0);
            error = m_piLogManager->LoadEvent(m_nEventType, 
                                              uIndex.LowPart,
                                              pos->m_dwIndexCheck,
                                              0,
                                              &piEvent);
            if (error != CError::eNoError ||
                piEvent == NULL)
            {
                TRACE(_T("CTestLogDlg::OnLoadButton() : piEvent == NULL\n"));
                continue;
            }

            InsertEvent(nItem, *pos, piEvent);
        }
    }
    catch (...)
    {
        MessageBox(_T("Failed to load events"));
    }

    UpdateData(FALSE);
}

void CTestLogDlg::ResizeColumn(int nColumn, LPCTSTR szString)
{
    int nStringWidth = m_EventList.GetStringWidth(szString);
    nStringWidth += 16;
    int nColumnWidth = m_EventList.GetColumnWidth(nColumn);
    if (nColumnWidth < nStringWidth)
    {
        m_EventList.SetColumnWidth(nColumn, nStringWidth);
    }
}
void CTestLogDlg::OnClearButton() 
{
	if (m_piLogManager == NULL)
    {
        return;
    }
	
    UpdateData(TRUE);

    long error = CError::eNoError;
    try
    {
        error = m_piLogManager->ClearLog(m_nEventType);
    }
    catch(...)
    {
        error = CError::eFatalError;
    }
    if (error != CError::eNoError)
    {
        MessageBox(_T("Failed to clear events"));
    }
}

void CTestLogDlg::OnSearchloadButton() 
{
    if (m_piLogManager == NULL)
    {
        return;
    }

    UpdateData(TRUE);

    GetDlgItem(IDC_LOAD_BUTTON)->EnableWindow(FALSE);
    m_EventInfoVector.clear();

    long error = CError::eNoError;
    m_EventList.DeleteAllItems();
    m_nEventCount = 0;
    EventInfoVector eventInfoVector;
    try
    {
        // Search for events
        DATE dtStart = 0, dtStop = 0;
        long nEventCount = 0, nContextId = 0;
        _variant_t vIndex, vIndexCheck, vTimeStamp, vSequenceNumber;
        error = m_piLogManager->SearchEvents(m_nEventType, 
                                             dtStart, 
                                             dtStop,
                                             VARIANT_TRUE,
                                             0,
                                             &nContextId,
                                             &vIndex,
                                             &vIndexCheck,
                                             &vTimeStamp,
                                             &vSequenceNumber,
                                             &m_nEventCount);
        if (error == CError::eNoError &&
            Convert(vIndex,
                    vIndexCheck,
                    vTimeStamp,
                    vSequenceNumber,
                    eventInfoVector) != FALSE)
        {
            // Load events
            EVENTMANAGERLib::IEventExPtr piEvent;
            int nItem = 0;
            ULARGE_INTEGER uIndex;
            EventInfoVector::iterator pos;
            for (pos = eventInfoVector.begin();
                 pos != eventInfoVector.end();
                 pos ++)
            {
                // Load event
                uIndex.QuadPart = pos->m_uIndex;
                ASSERT(uIndex.HighPart == 0);
                error = m_piLogManager->LoadEvent(m_nEventType, 
                                                  uIndex.LowPart,
                                                  pos->m_dwIndexCheck,
                                                  nContextId,
                                                  &piEvent);
                if (error != CError::eNoError ||
                    piEvent == NULL)
                {
                    TRACE(_T("CTestLogDlg::OnSearchLoadButton() : piEvent == NULL\n"));
                    continue;
                }

                InsertEvent(nItem, *pos, piEvent);
            }
        }

        // Release context
        if (nContextId != 0)
        {
            m_piLogManager->DeleteSearchContext(m_nEventType, nContextId);
        }
    }
    catch(...)
    {
        error = CError::eFatalError;
    }
    if (error != CError::eNoError)
    {
        m_EventInfoVector.clear();
        m_nEventCount = 0;
        MessageBox(_T("Failed to search and load events"));
    }

    UpdateData(FALSE);
}

void CTestLogDlg::OnGetSizeButton() 
{
	if (m_piLogManager == NULL)
    {
        return;
    }
	
    UpdateData(TRUE);
	
    long error = CError::eNoError;
    try
    {
        error = m_piLogManager->GetMaxLogSize(m_nEventType, &m_nMaxSize);
    }
    catch(...)
    {
        error = CError::eFatalError;
    }
    if (error != CError::eNoError)
    {
        MessageBox(_T("Failed to get max size"));
    }

    UpdateData(FALSE);
}

void CTestLogDlg::OnSetSizeButton() 
{
	if (m_piLogManager == NULL)
    {
        return;
    }
	
    UpdateData(TRUE);
	
    long error = CError::eNoError;
    try
    {
        error = m_piLogManager->SetMaxLogSize(m_nEventType, m_nMaxSize);
    }
    catch(...)
    {
        error = CError::eFatalError;
    }
    if (error != CError::eNoError)
    {
        MessageBox(_T("Failed to set max size"));
    }
}

BOOL CTestLogDlg::Convert(const VARIANT& vIndex,
                          const VARIANT& vIndexCheck,
                          const VARIANT& vTimeStamp,
                          const VARIANT& vSequenceNumber,
                          EventInfoVector& EventInfos)
{
    try
    {
        // Convert to vector
        CVariantArray indexArray(vIndex);
        CVariantArray indexCheckArray(vIndexCheck);
        CVariantArray timeStampArray(vTimeStamp);
        CVariantArray sequenceNumberArray(vSequenceNumber);
        long i;
        ULARGE_INTEGER uIndex;
        CLogFactoryEx::EventInfo eventInfo;
        for (i = 0; i < indexArray.GetSize(); i++)
        {
            uIndex.LowPart = long(indexArray.GetAt(i));
            uIndex.HighPart = 0;
            eventInfo.m_uIndex = uIndex.QuadPart;
            eventInfo.m_dwIndexCheck = long(indexCheckArray.GetAt(i));
            eventInfo.m_nEventType = m_nEventType;
            eventInfo.m_dwSequenceNumber = long(sequenceNumberArray.GetAt(i));
            VariantTimeToSystemTime(DATE(timeStampArray.GetAt(i)), &eventInfo.m_TimeStamp);

            EventInfos.push_back(eventInfo);
        }
    }
    catch (...)
    {
        return FALSE;
    }
    return TRUE;
}


BOOL CTestLogDlg::InsertEvent(int& nItem, 
                              CLogFactoryEx::EventInfo& EventInfo,
                              EVENTMANAGERLib::IEventExPtr& piEvent)
{
    nItem = m_EventList.InsertItem(nItem + 1, _T(""));

    ULARGE_INTEGER uIndex;
    uIndex.QuadPart = EventInfo.m_uIndex;
    ASSERT(uIndex.HighPart == 0);
    CString sString;
    sString.Format(_T("%lu"), uIndex.LowPart);
    m_EventList.SetItemText(nItem, EventIndexColumn, sString);
    ResizeColumn(EventIndexColumn, sString);

    sString.Format(_T("%ld"), EventInfo.m_nEventType);
    m_EventList.SetItemText(nItem, EventTypeColumn, sString);
    ResizeColumn(EventTypeColumn, sString);

    sString.Format(_T("%lu"), EventInfo.m_dwIndexCheck);
    m_EventList.SetItemText(nItem, EventIndexCheckColumn, sString);
    ResizeColumn(EventIndexCheckColumn, sString);

    DATE dtDate;
    _variant_t vValue;
    _bstr_t bString;
    SystemTimeToVariantTime(&EventInfo.m_TimeStamp, &dtDate);
    vValue.vt = VT_DATE;
    vValue.date = dtDate;
    vValue.ChangeType(VT_BSTR);
    bString = vValue;
    sString.Format(_T("%s"), LPCTSTR(bString));
    m_EventList.SetItemText(nItem, EventTimeStampColumn, sString);
    ResizeColumn(EventTimeStampColumn, sString);

    sString.Format(_T("%lu"), EventInfo.m_dwSequenceNumber);
    m_EventList.SetItemText(nItem, EventSequenceColumn, sString);
    ResizeColumn(EventSequenceColumn, sString);

    NAVEVENTSLib::INAVEventCommonIntPtr piTestEvent;

    switch (piEvent->GetType())
    {
    case AV_Event_ID_VirusAlert:
        piTestEvent = piEvent;
        //sString.Format(_T("%ld"), piTestEvent->GetTestValue());
        break;
/*    case CTestEvent5::TypeId:
        piTestEvent = piEvent;
        sString.Format(_T("%ld"), piTestEvent->GetTestValue());
        break;*/
    default:
        sString = _T("Unknown");
    }
    m_EventList.SetItemText(nItem, EventValueColumn, sString);
    ResizeColumn(EventValueColumn, sString);

    return TRUE;
}