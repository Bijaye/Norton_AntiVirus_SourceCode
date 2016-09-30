/*
 * PROPRIETARY/CONFIDENTIAL.  use of this product is subject to license terms.
 * Copyright (c) 2005 Symantec Corporation.  All Rights Reserved.
 *
*/

// VirusListView.cpp : implementation file
//

#include "stdafx.h"
#include "qscon.h"
#include <atlsnap.h>
#include "QSConsoleData.h"
#include "QSConsole.h"
#include "VirusListView.h"
//#include "transman.h"
//#include "signature.h"
#include <mscFuncs.h>
//extern CSrvSnapApp theApp;
#ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL g_bContinue = FALSE;
int WaitForObjectsWithMsgPump( HANDLE *lphObjects, int iNumObjs, DWORD dwTimeout /* = INFINITE */ )
{ 
    // The message loop lasts until we get a WM_QUIT message,
    // upon which we shall return from the function.
    while (TRUE)
    {
        // block-local variable 
        DWORD result ; 

        // Wait for any message sent or posted to this queue 
        // or for one of the passed handles be set to signaled.
        result = MsgWaitForMultipleObjects(iNumObjs, lphObjects, 
                 FALSE, dwTimeout, QS_ALLINPUT); 

        // The result tells us the type of event we have.
        if (result == (WAIT_OBJECT_0 + iNumObjs))
        {
            // block-local variable 
            MSG msg ; 

            // Read all of the messages in this next loop, 
            // removing each message as we read it.
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { 
                // If it's a quit message, we're out of here.
                if (msg.message == WM_QUIT)  
                    return 1; 
                // Otherwise, dispatch the message.
                DispatchMessage(&msg); 
            } // End of PeekMessage while loop.
        } 
		else
			break;
    } // End of the always while loop. 

	return 0;
} // End of function.

/////////////////////////////////////////////////////////////////////////////
// CVirusListView dialog

CVirusListView::CVirusListView(CWnd* pParent               /*=NULL*/)
: CDialog(CVirusListView::IDD, pParent)
    {
    //{{AFX_DATA_INIT(CVirusListView)
    m_strPatternFile = _T("");
    //}}AFX_DATA_INIT
    m_pThread = NULL;
    }


void CVirusListView::DoDataExchange(CDataExchange* pDX)
    {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CVirusListView)
    DDX_Control(pDX, IDC_ABOUTVIRUS, m_ctlAboutVirus);
    DDX_Control(pDX, IDOK, m_ctlClose);
    DDX_Control(pDX, IDC_VIRUSLIST, m_ctlVirusList);
    DDX_Text(pDX, IDC_PATTERNFILE, m_strPatternFile);
    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(CVirusListView, CDialog)
//{{AFX_MSG_MAP(CVirusListView)
ON_WM_SIZE()
ON_NOTIFY(LVN_COLUMNCLICK, IDC_VIRUSLIST, OnColumnclickViruslist)
ON_WM_DESTROY()
ON_BN_CLICKED(IDC_ABOUTVIRUS, OnAboutvirus)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVirusListView message handlers

// CVirusListView sorting function.
int CALLBACK SignatureListCompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParam)
    {
	int iRc=0;
	SAFEARRAY *pa1, *pa2;
	VARIANT vTemp1, vTemp2;
	LONG lIndex=0;
	pa1 = (SAFEARRAY *) lParam1;
	pa2 = (SAFEARRAY *) lParam2;
	CString sTemp1, sTemp2;
	
	USES_CONVERSION;
    CListCtrlEx
    *pLogList = (CListCtrlEx *)lParam;
	
	VariantInit(&vTemp1);
	VariantInit(&vTemp2);
    switch ( pLogList->m_iSortColumn )
        {
        // Name
        case 0:
			lIndex = VDF_NAME;
	        SafeArrayGetElement( pa1, &lIndex, &vTemp1 );
	        SafeArrayGetElement( pa2, &lIndex, &vTemp2 );
			if (vTemp1.vt == VT_BSTR && vTemp2.vt == VT_BSTR)
			{
				sTemp1 =  OLE2T(vTemp1.bstrVal);
				sTemp2 =  OLE2T(vTemp2.bstrVal);

				if ( pLogList->m_iSortOrder == SORT_ASCENDING )
					iRc = sTemp1.CompareNoCase(sTemp2);
				else
					iRc = sTemp2.CompareNoCase(sTemp1);
			}
			break;

        case 1:
			lIndex = VDF_INFECTS;
	        SafeArrayGetElement( pa1, &lIndex, &vTemp1 );
	        SafeArrayGetElement( pa2, &lIndex, &vTemp2 );
			if (vTemp1.vt == VT_BSTR && vTemp2.vt == VT_BSTR)
			{
				sTemp1 =  OLE2T(vTemp1.bstrVal);
				sTemp2 =  OLE2T(vTemp2.bstrVal);

				if ( pLogList->m_iSortOrder == SORT_ASCENDING )
					iRc = sTemp1.CompareNoCase(sTemp2);
				else
					iRc = sTemp2.CompareNoCase(sTemp1);
			}
			break;

        case 2:
			lIndex = VDF_COMMON;
	        SafeArrayGetElement( pa1, &lIndex, &vTemp1 );
	        SafeArrayGetElement( pa2, &lIndex, &vTemp2 );
			if (vTemp1.vt == VT_BSTR && vTemp2.vt == VT_BSTR)
			{
				sTemp1 =  OLE2T(vTemp1.bstrVal);
				sTemp2 =  OLE2T(vTemp2.bstrVal);

				if ( pLogList->m_iSortOrder == SORT_ASCENDING )
					iRc = sTemp1.CompareNoCase(sTemp2);
				else
					iRc = sTemp2.CompareNoCase(sTemp1);
			}
			break;
        }
	VariantClear(&vTemp1);
	VariantClear(&vTemp2);
    return iRc;
    }

void CVirusListView::OnSize(UINT nType, int cx, int cy)
    {
    if ( m_ctlClose )
        {
        m_ctlClose.SetWindowPos(NULL, cx - 85, cy - 34, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }

    if ( m_ctlVirusList )
        {
//              rect.left = 0;
//              rect.top = 0;
//              rect.right = 4;
//              rect.bottom = 8;
//          MapDialogRect(&rect);
//          int baseUnitY = rect.bottom;
//          int baseUnitX = rect.right;
//              int xOffSet = 7 * 4;
//              int yOffSet = 28 * 4;


        m_ctlVirusList.SetWindowPos(NULL, 0, 0, cx - 22, cy - 80, SWP_NOMOVE | SWP_NOZORDER);
//              m_LogList.SetWindowPos(NULL, 0, 0, cx - xOffSet, cy - yOffSet, SWP_NOMOVE | SWP_NOZORDER);
        }
    }

BOOL CVirusListView::OnInitDialog()
    {
    CDialog::OnInitDialog();


//	m_strPatternFile =  ((CQSConsoleData*)m_pSnapin)->m_sCurrentDefDate;
	GetDlgItem(IDC_PATTERNFILE)->SetWindowText(m_strPatternFile);


    // Create the column headers.
    LV_COLUMN lvc;
    CString strTitle;
    AFX_MANAGE_STATE(AfxGetStaticModuleState());

    CWaitCursor wait;
    m_ctlVirusList.m_iSortColumn = 0;
    m_ctlVirusList.OnCreate(NULL);
    // Set the style for the listbox to show selection always.
    m_ctlVirusList.ModifyStyle(0, LVS_SHOWSELALWAYS);

    // Setup the listctrl's columns.
    lvc.mask = LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    // Add the column headers for these columns

    lvc.iSubItem = 0;
    strTitle.LoadString(IDS_COLUMN_VIRUSLIST_NAME);
    lvc.pszText = const_cast< LPTSTR >( ( LPCTSTR )strTitle );
    lvc.cx = 165;
    m_ctlVirusList.InsertColumn(0, &lvc);

    lvc.iSubItem = 1;
    strTitle.LoadString(IDS_COLUMN_VIRUSLIST_INFECTS);
    lvc.pszText = const_cast< LPTSTR >( ( LPCTSTR )strTitle );
    lvc.cx = 235;
    m_ctlVirusList.InsertColumn(1, &lvc);

    lvc.iSubItem = 2;
    strTitle.LoadString(IDS_COLUMN_VIRUSLIST_SPREAD);
    lvc.pszText = const_cast< LPTSTR >( ( LPCTSTR )strTitle );
    lvc.cx = 125;
    m_ctlVirusList.InsertColumn(2, &lvc);


    ReadVirusList();

    return TRUE;                                           // return TRUE unless you set the focus to a control
                                                           // EXCEPTION: OCX Property Pages should return FALSE
    }


UINT FillListThread( LPVOID pVoid )
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
	HRESULT hr=S_OK;
    CVirusListView* pVirusList = (CVirusListView*)pVoid;
    CListCtrlEx* pListCtl = &pVirusList->m_ctlVirusList;
    DWORD    iCounter = 10;
	ULONG fetched = 0;
	ULONG ultempcounter = 0;
	SAFEARRAY *pa;
	LONG lIndex=0;
	int iIndex=0;
    USES_CONVERSION;
	VARIANT vTemp;
    CString strNumPatterns;

	VariantInit(&vTemp);
	
	CoInitialize( NULL );
	
	VARIANT items[NO_VIRUS_READ];

	_ASSERTE (((CQSConsoleData*)pVirusList->m_pSnapin)->m_cQSInterfaces);
	if(((CQSConsoleData*)pVirusList->m_pSnapin)->m_cQSInterfaces == NULL)
		return E_UNEXPECTED;
    // 
    // Unmarshal server interface.
    // 
    CoGetInterfaceAndReleaseStream( pVirusList->m_pServerStream, __uuidof( IEnumSavVirusInfo ), (LPVOID*)&pVirusList->m_pEnum );
    ((CQSConsoleData*)pVirusList->m_pSnapin)->m_cQSInterfaces->m_pQServer.DCO_SetProxyBlanket( pVirusList->m_pEnum );


    CString strLoading;
    strLoading.LoadString ( IDS_LOADING_SIGS );

	// Disable the header when we insert our virus list into the listview
	// This is to prevent users from sorting the view while the list
	// is loading.
	CListCtrl* pList = &pVirusList->m_ctlVirusList;

	pList->GetHeaderCtrl()->EnableWindow(FALSE);

    pVirusList->GetDlgItem(IDC_NUMPATTERNS)->SetWindowText(strLoading);

	while(  pVirusList->m_pEnum->Next( NO_VIRUS_READ, items, &fetched )== S_OK &&g_bContinue)
	{
  
		strNumPatterns.Format(_T("%d"), ultempcounter);
		pVirusList->GetDlgItem(IDC_NUMPATTERNS)->SetWindowText(strNumPatterns);

		for( ULONG i = 0; i < fetched; i++ )
		{
			pa = items[i].parray;
			// 
			// Initialize safe array construct
			// 
			lIndex = VDF_NAME;
	        SafeArrayGetElement( pa, &lIndex, &vTemp );
            iIndex = pListCtl->InsertItem(ultempcounter++, OLE2T(vTemp.bstrVal));
			VariantClear(&vTemp);

			lIndex = VDF_INFECTS;
	        SafeArrayGetElement( pa, &lIndex, &vTemp );
            pListCtl->SetItemText(iIndex, 1, OLE2T(vTemp.bstrVal));
			VariantClear(&vTemp);

			lIndex = VDF_COMMON;
	        SafeArrayGetElement( pa, &lIndex, &vTemp );
            pListCtl->SetItemText(iIndex, 2, OLE2T(vTemp.bstrVal));
			VariantClear(&vTemp);

            pListCtl->SetItemData(iIndex, (long)items[i].parray);
			
		}


            //If the item is visible, I need to re-draw the list
		if( g_bContinue	&&
			(0 == iCounter++ % 10) ||
			( ( iIndex >= pListCtl->GetTopIndex() ) && 
			  ( iIndex <= (pListCtl->GetTopIndex() + pListCtl->GetCountPerPage() ) ) ) )
			{
			
            pListCtl->SendMessage( WM_SETREDRAW, 1, 0 );
            pVirusList->GetDlgItem(IDC_NUMPATTERNS)->SetWindowText(strLoading);
			pListCtl->UpdateWindow();
			pListCtl->SendMessage( WM_SETREDRAW, 0, 0 );
			}

	}
        

		//Re-enable the listview header.
		pList->GetHeaderCtrl()->EnableWindow(TRUE);


    strNumPatterns.Format(_T("%d"), pListCtl->GetItemCount());
    pVirusList->GetDlgItem(IDC_NUMPATTERNS)->SetWindowText(strNumPatterns);
	CoUninitialize();
    if ( g_bContinue )
        pListCtl->SortItems(SignatureListCompareFunc, (LPARAM)&pVirusList->m_ctlVirusList, 0);
        pListCtl->SendMessage( WM_SETREDRAW, 1, 0 );
		pListCtl->UpdateWindow();

    return TRUE;

}

void CVirusListView::ReadVirusList()
    {
    g_bContinue = TRUE;
    m_pThread = AfxBeginThread( (AFX_THREADPROC)FillListThread, this );

	if( m_pThread )
		m_pThread->m_bAutoDelete = TRUE;

    return;
    }

void CVirusListView::OnColumnclickViruslist(NMHDR* pNMHDR, LRESULT* pResult)
    {
    NM_LISTVIEW
    *pNMListView = (NM_LISTVIEW*)pNMHDR;

    m_ctlVirusList.SortItems(SignatureListCompareFunc, (LPARAM)&m_ctlVirusList, pNMListView->iSubItem);
    *pResult = 0;
    }

void CVirusListView::OnDestroy()
    {

    SetCapture();
    // Make CDialog::OnDestroy() occurs outside the scope of the wait cursor.
    {
	    CWaitCursor	wait;
		SAFEARRAY *pa;

	    TRACE(_T("Terminating Thread\n") );
	    g_bContinue = FALSE;
	    //Wait for my thread to go away
	    if( m_pThread )
	    {
		    WaitForObjectsWithMsgPump( &(m_pThread->m_hThread), 1 );
	    }
        // Free all the data associated with my listctrl;
        int
        i,
        iNumItems = m_ctlVirusList.GetItemCount();


        for ( i = 0; i < iNumItems; i++ )
            {
            
            pa = (SAFEARRAY *)m_ctlVirusList.GetItemData(i);
			
			SafeArrayDestroy(pa);
            }
        m_ctlVirusList.DeleteAllItems();
    }
    ReleaseCapture();

    CDialog::OnDestroy();
    }

void CVirusListView::OnAboutvirus()
    {
    int    i, iCount = m_ctlVirusList.GetItemCount();
    CString    strName = _T("");

	VARIANT vTemp;
	VariantInit (&vTemp);
	SAFEARRAY *pa;
	CString sTemp;
	USES_CONVERSION;
    for ( i = 0; i < iCount; i++ )
        {
        if ( m_ctlVirusList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED )
            {
            // Get the virus name to use as a lookup into the encyclopedia
            pa = (SAFEARRAY *)m_ctlVirusList.GetItemData(i);
			
			LONG lIndex = VDF_NAME;
	        SafeArrayGetElement( pa, &lIndex, &vTemp );
			sTemp = OLE2T(vTemp.bstrVal);

            int iLen = sTemp.GetLength();

            if ( sTemp .GetAt(iLen - 1) == _T('*') )
                strName = sTemp.Left(iLen - 1);
            else
                strName = sTemp ;

            break;
            }
        }
//    theApp.WinHelp( reinterpret_cast<DWORD>((LPCTSTR)strName), HELP_PARTIALKEY );
    }
