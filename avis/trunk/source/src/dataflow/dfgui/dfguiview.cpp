/*----------------------------------------------------------------------------*/
/*                                                                            */
/* FILE:      DFGuiView.cpp                                                   */
/* FUNCTION:  To manage the GUI view                                          */
/*                                                                            */
/* ROUTINES CONTAINED WITHIN FILE:                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*                                                                            */
/* NOTES:                                                                     */
/*----------------------------------------------------------------------------*/
/* HISTORY:  SEPTEMBER, 1998  SNK                                             */
/*----------------------------------------------------------------------------*/
#include "stdafx.h"
#include "afxtempl.h"
#include "afxwin.h"
#include "MainFrm.h" 
#include <iostream>
#include <fstream>
#include "dfpackthread.h"
#include "resource.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"
#include "dfmsg.h" 
#include "dfjob.h"
#include "dfsample.h"
#include "dferror.h"  
#include "dfmatrix.h"
#include "dfdirinfo.h"
#include "dfmanager.h"
#include "avisdfrlimp.h"
#include "listvwex.h"
#include "DFGuiDoc.h"
#include "dfconfiguredialog.h"
#include "dfstatisticsdialog.h"
#include "dfsampleattributes.h"
#include "dfdefaults.h"
#include "paramvalue.h"
#include "DFGuiView.h"
#include "DFGui.h"
#include "prioritydialog.h"
#include "avissendmail.h"
#include "CrashRecover.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFGuiView

IMPLEMENT_DYNCREATE(CDFGuiView, CListViewEx)

BEGIN_MESSAGE_MAP(CDFGuiView, CListViewEx)
	//{{AFX_MSG_MAP(CDFGuiView)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_HOLD, OnUpdateHold)
	ON_UPDATE_COMMAND_UI(ID_STOP, OnUpdateStop)
	ON_UPDATE_COMMAND_UI(ID_RESUME, OnUpdateResume)
	ON_COMMAND(ID_HOLD, OnHold)
	ON_COMMAND(ID_STOP, OnStop)
	ON_COMMAND(ID_RESUME, OnResume)
	ON_WM_NCDESTROY()
	ON_COMMAND(ID_DISABLE, OnDisable)
	ON_UPDATE_COMMAND_UI(ID_DISABLE, OnUpdateDisable)
	ON_UPDATE_COMMAND_UI(ID_ENABLE, OnUpdateEnable)
	ON_COMMAND(ID_ENABLE, OnEnable)
	ON_UPDATE_COMMAND_UI(ID_HOLDALL, OnUpdateHoldall)
	ON_COMMAND(ID_HOLDALL, OnHoldall)
	ON_COMMAND(ID_RESUMEALL, OnResumeall)
	ON_UPDATE_COMMAND_UI(ID_RESUMEALL, OnUpdateResumeall)
	ON_COMMAND(ID_CONFIGURE, OnConfigure)
	ON_UPDATE_COMMAND_UI(ID_CONFIGURE, OnUpdateConfigure)
	ON_COMMAND(ID_STATISTICS, OnStatistics)
	ON_UPDATE_COMMAND_UI(ID_STATISTICS, OnUpdateStatistics)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CHANGEPRIORITY, OnChangepriority)
	ON_UPDATE_COMMAND_UI(ID_CHANGEPRIORITY, OnUpdateChangepriority)
	ON_COMMAND(ID_DEFER, OnDefer)
	ON_UPDATE_COMMAND_UI(ID_DEFER, OnUpdateDefer)
	ON_COMMAND(ID_STOPIMPORT, OnStopimport)
	ON_UPDATE_COMMAND_UI(ID_STOPIMPORT, OnUpdateStopimport)
	ON_COMMAND(ID_STOPUNDEFERRER, OnStopundeferrer)
	ON_UPDATE_COMMAND_UI(ID_STOPUNDEFERRER, OnUpdateStopundeferrer)
	ON_COMMAND(ID_STOPUPDATER, OnStopupdater)
	ON_UPDATE_COMMAND_UI(ID_STOPUPDATER, OnUpdateStopupdater)
	ON_COMMAND(ID_SAMPLEATTRIBUTES, OnSampleattributes)
	ON_UPDATE_COMMAND_UI(ID_SAMPLEATTRIBUTES, OnUpdateSampleattributes)
	ON_COMMAND(ID_STOPATTRCOLLECTOR, OnStopattrcollector)
	ON_UPDATE_COMMAND_UI(ID_STOPATTRCOLLECTOR, OnUpdateStopattrcollector)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListViewEx::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListViewEx::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListViewEx::OnFilePrintPreview)
	ON_COMMAND(ID_LAUNCHER_SHOW, OnLauncherShow)
	ON_MESSAGE( DFMANAGER_BLOCK_PROCESSING  , OnDisable)
	ON_MESSAGE(DFLAUNCHER_POST_JOB_STATUS, OnJobFromLauncher)
	ON_MESSAGE(MSG_TRANSIT, OnJobFromLauncher)
	ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked) 
	ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)	
	ON_MESSAGE(MSG_TRANSIT1, OnAfterThread)
	ON_MESSAGE(MSG_IMMCLOSE, OnImmClose)


END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFGuiView construction/destruction

CDFGuiView::CDFGuiView():
      killFlag(0),
	  request(ENABLE),
      nSortedCol(1), 
	  bSortOrder(DESCENDING),
	  initFlag(0),
	  buildDefFilename(CString()),
	  defBaseDir(CString()),
	  UNCPath(CString()), 
	  sendFlag(0), 
	  logFlag(0),
	  sampleDirPath(""),
	  sortFlag (0)
{
}

CDFGuiView::~CDFGuiView()
{
	if (crashRecoverStatus != CRASH_RECOVER_STOPPED) {
        crashRecoverStatus = CRASH_RECOVER_SIGNALLED_TO_STOP;
        for (int i = 0; i < 15; i++)
        {
            if (crashRecoverStatus != CRASH_RECOVER_STOPPED)
                Sleep (2000);
        }
    }
 	TermView();
}

BOOL CDFGuiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
   cs.style |=LVS_REPORT; 

	return CListViewEx::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDFGuiView drawing

void CDFGuiView::OnDraw(CDC* pDC)
{
	CDFGuiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnInitialUpdate                                       */
/* Description:         Initiate list view, manager, launcher; set timer      */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Note:                                                                      */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void CDFGuiView::OnInitialUpdate()
{
	CListViewEx::OnInitialUpdate();

    CListCtrl& m_ctlListCtrl=GetListCtrl();

	m_ctlListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | 
				 LVS_EX_ONECLICKACTIVATE | 
				 LVS_EX_UNDERLINEHOT | LVS_EX_INFOTIP |
				 LVS_EX_GRIDLINES);


    LV_COLUMN lvc;
    lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem=0;
    CString cookie((LPCSTR) (IDS_LIST_SAMPLE)); 
	lvc.pszText= (LPTSTR) (LPCTSTR) cookie;
    lvc.cx= m_ctlListCtrl.GetStringWidth((LPCTSTR)cookie);
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(0,&lvc);

    lvc.iSubItem=1;
    CString priority((LPCSTR) (IDS_PRIORITY)); 
	lvc.pszText= (LPTSTR) (LPCTSTR) priority;
    lvc.cx= m_ctlListCtrl.GetStringWidth((LPCTSTR)priority) + 60; 
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(1,&lvc);


    lvc.iSubItem=2;
    CString hold((LPCSTR) (IDS_HOLD)); 
	lvc.pszText= (LPTSTR) (LPCTSTR) hold;
    lvc.cx= m_ctlListCtrl.GetStringWidth((LPCTSTR)hold) + 40;
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(2,&lvc);

    lvc.iSubItem=3;
	CString stateIn((LPCSTR) (IDS_LIST_CURRENT_STATE));
    lvc.pszText=(LPTSTR) ((LPCTSTR)stateIn);
    lvc.cx= 7 * m_ctlListCtrl.GetStringWidth((LPCTSTR)stateIn); 
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(3,&lvc);

    lvc.iSubItem=4;
	CString status((LPCSTR) (IDS_LIST_STATUS));
    lvc.pszText=(LPTSTR) ((LPCTSTR)status);
    lvc.cx=4 * m_ctlListCtrl.GetStringWidth((LPCTSTR)status);
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(4,&lvc);

	lvc.iSubItem=5;
    CString done((LPCSTR) (IDS_LIST_JOBS));
    lvc.pszText=(LPTSTR) ((LPCTSTR)done);
    lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)done);
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(5,&lvc);

	int imageItem = 0;
    m_image.Create( IDB_BITMAP1, 11, 3, (COLORREF)-1 ); 
    m_ctlListCtrl.SetImageList(&m_image,LVSIL_SMALL);
    
    
	HDITEM hditem;
	char buffer[256]; 
	CHeaderCtrl* pHeader = (CHeaderCtrl*)m_ctlListCtrl.GetDlgItem(0);
	hditem.mask =  HDI_TEXT | HDI_FORMAT;
    hditem.pszText = buffer;
    hditem.cchTextMax = 256;
	pHeader->GetItem(0, &hditem);
	hditem.mask = HDI_FORMAT | HDI_TEXT;
	hditem.fmt |= HDF_LEFT;
	pHeader->SetItem(0, &hditem);

	hditem.mask =  HDI_TEXT | HDI_FORMAT;
	pHeader->GetItem(1, &hditem);
	hditem.mask = HDI_FORMAT | HDI_TEXT;
//	hditem.fmt |= HDF_LEFT;
	pHeader->SetItem(1, &hditem);

	mybitmap4.LoadMappedBitmap(IDB_BITMAP4);
	mybitmap5.LoadMappedBitmap(IDB_BITMAP5);
//	mybitmap6.LoadMappedBitmap(IDB_BITMAP6);
//	hditem.hbm = (HBITMAP)mybitmap6.GetSafeHandle();
//	pHeader->SetItem(1, &hditem);
//	pHeader->SetItem(0, &hditem);

    AutoSizeColumns( 0 );

    // Add tips to column headers. 
	AddHeaderToolTip(0, CString((LPCSTR) IDS_LIST_SAMPLE_TIP ));
	AddHeaderToolTip(1, CString((LPCSTR) IDS_LIST_PRIORITY));
	AddHeaderToolTip(2, CString((LPCSTR) IDS_LIST_HOLD ));
	AddHeaderToolTip(3, CString((LPCSTR) IDS_LIST_CURRENT_STATE_TIP ));
	AddHeaderToolTip(4, CString((LPCSTR) IDS_LIST_STATUS_TIP ));
//	AddHeaderToolTip(5, CString((LPCSTR) IDS_LIST_NEXT_STATE_TIP ));
	AddHeaderToolTip(5, CString((LPCSTR) IDS_LIST_JOBS_TIP ));

	// Initiate the Manager

	int rc = Manager.Init();
	if (rc == DF_EVALUATOR_FAILED)
		killFlag = 1;
	if (rc == DF_CONFIG_FILE_ERROR)
		killFlag = 2;
	if (rc == DF_CONFIG_FILE_CREATE_ERROR)
		killFlag = 3;
	if (rc == DF_DABASE_OPEN_ERROR)
		killFlag = 4;
	if (rc == DF_DABASE_ADD_NEW_STATE_ERROR)
		killFlag = 5;
	if (rc == DF_CONFIG_FILE_HAS_BEEN_CREATED)
		killFlag = 10;
	if (rc == DF_UNC_EMPTY || rc == DF_BUILD_DEF_FILENAME_EMPTY || rc == DF_BASE_DEF_DIR_EMPTY )
		killFlag = 11;
	if (rc == DF_LATEST_SIGNATURE_ERROR)
		killFlag = 8;

	// adjustment to intervals
	CDFGuiApp *pApp = (CDFGuiApp *) AfxGetApp();

    // Initiate the Launcher
	pApp->DFLauncherWnd = InitDFLauncher(AfxGetApp()->m_pMainWnd->m_hWnd);
    pApp->DFGuiWnd =(HWND)this;
    Manager.SetLauncherHandle(pApp->DFLauncherWnd);
    Manager.SetMainWindowHandle((CWnd*)this);

    Manager.checkTime = CTime::GetCurrentTime();    
    // Set timers
 	(void) CorrectArrivalInterval(Init, Both);
    (void) SetTimer(TIMER_PROCESS_ID, Manager.sampleSubmissionInterval * 1000, NULL);					
    (void) SetTimer(TIMER_ARRIVAL_ID, Manager.sampleArrivalCheckInterval * 1000, NULL);					
    (void) SetTimer(TIMER_DATABASE_RESCAN_ID, Manager.databaseRescanInterval * 1000, NULL);					
//    (void) SetTimer (TIMER_UPDATE_ID, 3 * 1000, NULL);
	if (Manager.statusUpdateInterval != 0 )
        (void) SetTimer (TIMER_STATUS_UPDATE_ID, Manager.statusUpdateInterval * 60 * 1000, NULL);
	if (Manager.undeferrerInterval != 0 )
	    (void) SetTimer (TIMER_UNDEFERRER_ID, Manager.undeferrerInterval * 60 * 1000, NULL);
	if (Manager.defImportInterval != 0 )
	    (void) SetTimer(TIMER_DEFINITION_IMPORT_ID, Manager.defImportInterval * 60 * 1000, NULL);					
	if (Manager.attributeInterval != 0 )
		(void) SetTimer(TIMER_SAMPLEATTRIBUTE_ID, Manager.attributeInterval * 60 * 1000, NULL);					
	(void) SetTimer(TIMER_THREAD_ID, 1000, NULL);					
	(void) SetTimer(TIMER_UPDATELIST_ID, 1000, NULL);					
	// crash recovering
	AfxBeginThread (CleanAfterProgramCrash, Manager.UNCPath);
	Sleep (3000);
}

/////////////////////////////////////////////////////////////////////////////
// CDFGuiView printing

BOOL CDFGuiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDFGuiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDFGuiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}
/////////////////////////////////////////////////////////////////////////////
// CDFGuiView diagnostics

#ifdef _DEBUG
void CDFGuiView::AssertValid() const
{
	CListViewEx::AssertValid();
}

void CDFGuiView::Dump(CDumpContext& dc) const
{
	CListViewEx::Dump(dc);
}

CDFGuiDoc* CDFGuiView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDFGuiDoc)));
	return (CDFGuiDoc*)m_pDocument;
}
#endif //_DEBUG

/*----------------------------------------------------------------------------*/
/* Procedure name:      Update the list view                                  */
/* Description:         To update the list when there are changes in the      */                                                                    
/*                      collection                                            */ 
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int CDFGuiView::UpdateSampleList()
{
	CStringList        deleteList;
		
    LV_ITEM lvi;  
	CListCtrl& m_ctlListCtrl=GetListCtrl();
	DFSample *pSample, *inListSample;
	POSITION pos;
	int i;

	int nCount = m_ctlListCtrl.GetItemCount();
    //  Create a list of samples that have to be deleted from the list
    CString cookie, delCookie;
    for (i = 0; i < nCount; i++) {
		cookie =  m_ctlListCtrl.GetItemText(i, 0);
        CString delCookie(cookie);
        cookie.Remove('*');
		cookie.Remove(' ');

		// a sample is still in the list, but not in the collection 
        POSITION pos;
		pSample =  Manager.FindObjectWithIdInHPList(cookie, pos);
		if (pSample == NULL) {
				deleteList.AddTail(delCookie);
        }
	           
    }
	// Delete the processed samples					
	pos = deleteList.GetHeadPosition();
    while (pos != NULL) {
		CString cookieId = deleteList.GetNext(pos);
		int nItem;
        LV_FINDINFO lvf;

        lvf.flags = LVFI_STRING;
        lvf.psz = (LPTSTR)((LPCTSTR)cookieId);
        nItem = m_ctlListCtrl.FindItem (&lvf, -1);
        if (nItem != -1) {
			m_ctlListCtrl.DeleteItem(nItem);
        }
	}
	if (deleteList.GetCount())
	   deleteList.RemoveAll();
	  
	pos = Manager.sampleHPList.GetTailPosition();
	CString newStr, oldStr;
	int topIndex = m_ctlListCtrl.GetTopIndex();
	int count = m_ctlListCtrl.GetCountPerPage();
    m_ctlListCtrl.SetRedraw(FALSE);
	while (pos != NULL)
	{
	    
		pSample = Manager.sampleHPList.GetPrev(pos);

		int nItem;
        LV_FINDINFO lvf;

        //lvf.flags = LVFI_PARAM;
		//lvf.lParam= (LPARAM) pSample;
		lvf.flags = LVFI_STRING;
        lvf.psz = (LPTSTR)((LPCTSTR)pSample->GetCookieId());
        
		nItem = m_ctlListCtrl.FindItem (&lvf, -1);
        
        if (nItem != -1 ) {
			//if (nItem < topIndex  || nItem > topIndex + count)
			//	continue;
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(nItem);
			//cookieId
			CString newCookieId = pSample->GetCookieId();

			oldStr = m_ctlListCtrl.GetItemText(nItem, 0);
            if (newCookieId.Compare(oldStr))
                    m_ctlListCtrl.SetItemText(nItem, 0, newCookieId);
			//Priority
			char newPrt[256];
			int newPrior = pSample->GetPriority();
			CString newPriority;
			sprintf(newPrt, "%d", newPrior); 
            newPriority = CString(newPrt);  
			oldStr = m_ctlListCtrl.GetItemText(nItem, 1);
            if (newPriority.Compare(oldStr))
                    m_ctlListCtrl.SetItemText(nItem, 1, newPriority);
			

            //Hold
			CString newStr;
			oldStr = m_ctlListCtrl.GetItemText(nItem, 2);
			if ( pSample->GetUserRequest() == DFSample::PAUSE )
				 newStr = CString("Hold");
			else {
		       if ( pSample->GetUserRequest() == DFSample::STOP )
                  newStr  = CString("ImHold");
               else {
				   if ( pSample->GetUserRequest() == DFSample::DEFER ) {
					   if (pSample->GetCurrentStatus() == DFSample::FAIL && pSample->GetCurrentStatus() == DFSample::FINAL) {
						   pSample->SetUserRequest(DFSample::RESUME);
                       } 
                       else {
						   if (pSample->GetCurrentStateStr() != CString("DEFERRER")) {
								   newStr  = CString("RequestToDefer");
						   }
						   else  
	   						   newStr  = CString("");
                           
                       }
                  
				   } 
				   else 
                     newStr  = CString("");
               }
			 
			}
			oldStr = m_ctlListCtrl.GetItemText(nItem, 2);
			
            if (newStr.Compare(oldStr))
                    m_ctlListCtrl.SetItemText(nItem, 2, newStr);
        
            // STATE           
			//current status
			DFSample::SampleStatus status = pSample->GetCurrentStatus();
			CString newStatus  = pSample->GetCurrentStatusStr(status);
	       	CString oldStatusStr = m_ctlListCtrl.GetItemText(nItem,4);
			//current state
			CString newState  = pSample->GetCurrentStateStr();
    
            CString oldStateStr = m_ctlListCtrl.GetItemText(nItem, 3);

			if (status == DFSample::INPROGRESS &&  pSample->GetInProgressJobs() == 0 ) {
				    newStatus = CString((LPCSTR) (IDS_TEMP_STATUS)); 
		    }
			if (status == DFSample::WAIT ||
				pSample->GetCurrentStatus() == DFSample::FAIL)  {
				if (newStatus.Compare(oldStatusStr)) {  
			           m_ctlListCtrl.SetItemText(nItem, 4, newStatus);
                }
				if (newState.Compare(oldStateStr)) {  
			           m_ctlListCtrl.SetItemText(nItem, 3, newState);
                }
				if (newState == Manager.scan) {
                    if (oldStateStr.Compare(CString((LPCSTR) IDS_RESCAN_FILTER))) {
					    m_ctlListCtrl.SetItemText(nItem, 3, CString((LPCSTR) IDS_RESCAN_FILTER));
                    }
                	if (pSample->scanJob) { 
						m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 3, 0, 0, 0 );
				    }
					else 
						m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
                }
				else  {
                	if (pSample->scanJob && (newState == Manager.newSample)) { 
                        if (oldStateStr.Compare(CString((LPCSTR) IDS_INITIAL_RESCAN))) {
		    				m_ctlListCtrl.SetItemText(nItem, 3, CString((LPCSTR) IDS_INITIAL_RESCAN));                
                        }
					} 
			 		m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
				}
            }
			if (status == DFSample::INPROGRESS)  {
				if (newStatus.Compare(oldStatusStr)) {  
			           m_ctlListCtrl.SetItemText(nItem, 4, newStatus);
 				}
				if (newState.Compare(oldStateStr)) {  
  			           m_ctlListCtrl.SetItemText(nItem, 3, newState);
 				}
				m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 0, 0, 0, 0 );
            }

			if (status == DFSample::CriticalError)  {
				if (oldStatusStr.Compare(CString("CriticalError"))) {  
			           m_ctlListCtrl.SetItemText(nItem, 4, "CriticalError");
 				}
				if (newState.Compare(oldStateStr)) {  
  			           m_ctlListCtrl.SetItemText(nItem, 3, newState);
 				}
				m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
            }

			if (status == DFSample::SUCCESS)  {
				CString newNextState  = pSample->GetNextStateStr();
				CString waitStatus = CString((LPCSTR) (IDS_WAIT));
				if (waitStatus.Compare(oldStatusStr)) {  
						//m_ctlListCtrl.SetItem( nItem, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
						m_ctlListCtrl.SetItemText(nItem, 4, waitStatus);
                }
				if (newNextState.Compare(oldStateStr)) {  
  			           m_ctlListCtrl.SetItemText(nItem, 3, newNextState);
 				}
				if (Manager.serialFlag ){
					DFEvalState::StateGroup group = Manager.Matrix.GetStateGroup(pSample->GetNextStateStr());
					if (pSample->scanJob) 
							m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 3, 0, 0, 0 );
					else {
						if (pSample->noBuildFlag && (group == DFEvalState::BUILD || group == DFEvalState::FULLBUILD)) {
							m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 2, 0, 0, 0 );
						}    
						else
							m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
                    }
				}
				else {
					if (pSample->scanJob) 
							m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 3, 0, 0, 0 );
                    else 
				         m_ctlListCtrl.SetItem(nItem, 0, LVIF_IMAGE, NULL, 1, 0, 0, 0 );
                }
			} 
			 
			//jobs
			int initcond;
			int donejobs; 
			int progressjobs; 
			char jobString[256];
			
			initcond = pSample->GetInitCond();
			progressjobs = pSample->GetInProgressJobs();
			donejobs = pSample->GetDoneJobs();
			sprintf(jobString, "%d/%d/%d", initcond, donejobs, progressjobs); 
			CString newJobs = CString(jobString);
			oldStr = m_ctlListCtrl.GetItemText(nItem,5);
       		if (newJobs.Compare(oldStr))
                   m_ctlListCtrl.SetItemText(nItem, 5, newJobs);

             

		}
        else {
			//if (nItem < topIndex  || nItem > topIndex + count)
			//	break;

			CString cookieId = pSample->GetCookieId();

			DFSample::Origin origin = pSample->GetOrigin();
			DFSample::UserRequest request = pSample->GetUserRequest();

			CString strCurrentState;
			
			CString strNextState = pSample->GetNextStateStr();
			
			DFSample::SampleStatus currentStatus = pSample->GetCurrentStatus();

			strCurrentState  = pSample->GetCurrentStateStr();
			if (strCurrentState == Manager.scan)
				strCurrentState = CString((LPCSTR) IDS_RESCAN_FILTER);
            else {
				if (pSample->GetScanRequired() && strCurrentState == Manager.newSample)
					strCurrentState = CString((LPCSTR) IDS_INITIAL_RESCAN);
            }
			CString status = pSample->GetCurrentStatusStr(currentStatus); 
			int initcond;
			int donejobs; 
			int progressjobs; 
			int prior; 
			char jobString[256];
			char priorString[256];
			
			initcond = pSample->GetInitCond();
			progressjobs = pSample->GetInProgressJobs();
			donejobs = pSample->GetDoneJobs();
			sprintf(jobString, "%d/%d/%d", initcond, donejobs, progressjobs); 
			prior = pSample->GetPriority();
			sprintf(priorString, "%d", prior); 
			
			lvi.mask= LVIF_TEXT | LVIF_STATE;
			lvi.state = 0;
			lvi.stateMask = 0xFFFF;

			lvi.iItem=i;
			lvi.iSubItem=0;
			if (status == DFSample::INPROGRESS) {
				lvi.iImage = 0;
            }
			else  {
				if (Manager.serialFlag ){
					DFEvalState::StateGroup group = Manager.Matrix.GetStateGroup(pSample->GetNextStateStr());
					if (pSample->noBuildFlag && (group == DFEvalState::BUILD || group == DFEvalState::FULLBUILD)) {
					       lvi.iImage = 2;
					}
					else
					    lvi.iImage = 1;
				}
				else 
				    lvi.iImage = 1;
			} 

			lvi.pszText= (LPTSTR)((LPCTSTR)cookieId);
		
			int index =  m_ctlListCtrl.InsertItem(&lvi);
			m_ctlListCtrl.SetItemText(i,1, priorString);
			m_ctlListCtrl.SetItemText(i,3, strCurrentState);
			m_ctlListCtrl.SetItemText(i,4, (status) );
			m_ctlListCtrl.SetItemText(i,5, (jobString) );
			

			m_ctlListCtrl.SetItemData(index, (DWORD)pSample);  

		}
	
    }
    m_ctlListCtrl.SetRedraw(TRUE);


    // Fill out the panes
	CString paneString;
	int paneIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_INDICATOR_SAMPLECOUNT);
	if (paneIndex != -1)
	{
		
        int totalSamples = Manager.sampleHPList.GetCount();
        int inprogressSamples = 0;
        LONG waitSamples = 0;
		DFSample *pSample;
		POSITION pos = Manager.sampleHPList.GetHeadPosition();
	    while (pos != NULL)
		{
			pSample = Manager.sampleHPList.GetNext(pos);
			DFSample::SampleStatus status = pSample->GetCurrentStatus();

			if (status == DFSample::INPROGRESS) {
				if( pSample->GetInProgressJobs() == 0 ) 
				    waitSamples++; 
                else
                    inprogressSamples++;
		    }
			if (status == DFSample::WAIT || status == DFSample::SUCCESS) 
			    waitSamples++;           
          
 
        }
		CString string((LPCSTR)IDS_INDICATOR_SAMPLE);
		CString  samplesInfo;
		samplesInfo.Format ("%d/%d/%d",
            totalSamples, waitSamples, inprogressSamples);
		paneString = string + samplesInfo;
	}		
	((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(paneIndex, paneString);

	paneIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_INDICATOR_DATAFLOWSTATUS);
	if (paneIndex != -1 ) {
		if (request == ENABLE)
		  paneString = CString((LPCSTR)IDS_STATUS_ENABLE);
		else
		  paneString = CString((LPCSTR)IDS_STATUS_DISABLE);

		((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(paneIndex, paneString);

    }
	paneIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_SCHED_TASKS_IND1);
	if (paneIndex != -1) {
		CString str((LPCSTR) IDS_SCHED_TASKS_IND);
		CString tmp;
		if (Manager.defImportJob != NULL)
			tmp = CString(" A/");
		else
			tmp = CString(" I/");
		if(Manager.undeferrerJob != NULL) 
			tmp += CString("A/");
		else
			tmp += CString("I/");
		if (Manager.statusUpdaterJob != NULL)  // update status job 
			tmp += CString("A/");
		else
			tmp += CString("I/");
		if (Manager.attributeCollectorJob != NULL)
			tmp += CString("A ");
		else
			tmp += CString("I ");

		paneString = str + tmp;


		((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(paneIndex, paneString);    

    }
    /*
	CPoint point(0,0);
	m_ctlListCtrl.SetItemPosition(topIndex, point);
	m_ctlListCtrl.EnsureVisible(topIndex, 1);
    */

    return 0;
    


}        
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindSampleInList                                      */
/* Description:         To find sample in the list                            */
/*                                                                            */
/* Input:               DFSample *sample - sample object                      */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int CDFGuiView::FindSampleInList(DFSample *sample)
{
    CListCtrl& m_ctlListCtrl=GetListCtrl();

    for (int i = 0; i < m_ctlListCtrl.GetItemCount(); i++)
    {
    
        
		DFSample *pSample = (DFSample*) m_ctlListCtrl.GetItemData(i);
	    if (pSample == sample)
				return i;
	}
	return (-1);
}
int CDFGuiView::GetCurSel(void)
{
    CListCtrl& m_ctlListCtrl=GetListCtrl();
	int SelectedItemIndex = -1;
    int i;

    for (i = 0; i < m_ctlListCtrl.GetItemCount(); i++)
    {
        LV_ITEM lvi;

        lvi.mask = LVIF_STATE;
        lvi.iItem = i;
        lvi.iSubItem = 0;
        lvi.pszText = 0;
        lvi.cchTextMax = 0;
        lvi.stateMask = 0xFFFF;

        m_ctlListCtrl.GetItem(&lvi);

        if (lvi.state & (LVIS_SELECTED | LVIS_FOCUSED) )
		{
			break;
		}
    }
	return i;
}
DFSample *CDFGuiView::FindSampleObject(int& nSel, POSITION& pos)
{
	CListCtrl& m_ctlListCtrl=GetListCtrl();

    if	(m_ctlListCtrl.GetItemCount() == 0)
		return NULL;

	nSel = GetCurSel();
	if (nSel == LB_ERR)
	{
		return NULL;
	}
	
	DFSample* pSample = (DFSample*) m_ctlListCtrl.GetItemData(nSel);
	pos = Manager.sampleHPList.Find(pSample);

	ASSERT(pos != NULL);

	return pSample;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnJobFromLauncher                                     */
/* Description:                                                               */
/*                                                                            */
/* Input:               WPARAM p1, LPARAM p2                                  */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CDFGuiView::OnJobFromLauncher(WPARAM p1, LPARAM p2)
{

	DFJob *pJob = (DFJob*) p2;
	CString id = CString("");
	int seq = 0;
	DFJob::JobStatus status = DFJob::UNDEFINED;
	CString strStatus = CString("");
	if (pJob != NULL) {
	   id = pJob->GetCookieId();
       seq = pJob->GetJobSeq();
       status = pJob->GetStatus();
	   strStatus = Manager.GetCurrentJobStatus(status); 
    }
	int rc;

#ifdef _DEBUG
	(void) Manager.MessageLogging(id, seq, strStatus, 777 );
#endif
	switch(pJob->GetStatus()) {
		case DFJob::COMPLETED:
         rc = Manager.ProcessCompletedJob(pJob);
		 delete pJob;
		 break;
		case DFJob::ACCEPTED:
         rc = Manager.ProcessAcceptedJob(pJob);
		 break;
		case DFJob::TIMEDOUT:
		{
			pJob->SetStatus(DFJob::COMPLETED);
			rc = Manager.ProcessCompletedJob(pJob);
			delete pJob;
			break;
		}
		case DFJob::POSTPONED:
         rc = Manager.ProcessPostponedJob(pJob);
		 delete pJob;
		 break;
		case DFJob::IGNORED:
         rc  = Manager.ProcessIgnoredJob(pJob);
		 delete pJob;
		 break;
    }
	switch (rc) 
    {
	case (DF_DABASE_OPEN_ERROR):         
	case (DF_DABASE_CLOSE_ERROR):                   
	case (DF_DABASE_ADD_NEW_STATE_ERROR):     
	case (DF_DATABASE_UPDATE_RECORD_ERROR):            
	case (DF_DATABASE_GET_RECORD_ERROR):
	case (DF_LATEST_SIGNATURE_ERROR):
		{
		killFlag = 7; 
		returnCode = rc;
		}    
		return 0;
	
    default:
		break;
	}
   //UpdateSampleList();
#ifdef _DEBUG
 	(void) Manager.MessageLogging(id, seq, strStatus, rc );
#endif 

	return 0;

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnTimer                                               */
/* Description:         Get samples, update the list, submit samples          */
/*                                                                            */
/* Input:               UINT nIDEvent                                         */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Note:                                                                      */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
void CDFGuiView::OnTimer(UINT nIDEvent) 
{

	CString message;
	CString title((LPCSTR) (IDS_AVIS_TITLE)); 
			//config file has an empty UNC

	//Added on April/27/2000.
	//Check if we have to do a column sorting on the list control.
	if (nIDEvent == TIMER_THREAD_ID && sortFlag)
	{
		//Sort the requested column now.
		UpdateSampleList();
		SortList ();
		return;
	}

	if (   nIDEvent		== TIMER_PROCESS_ID 
		|| nIDEvent		== TIMER_ARRIVAL_ID 
		|| nIDEvent		== TIMER_DATABASE_RESCAN_ID 
		|| nIDEvent		== TIMER_UPDATE_ID 
		|| nIDEvent		== TIMER_STATUS_UPDATE_ID 
		|| nIDEvent		== TIMER_DEFINITION_IMPORT_ID
		|| nIDEvent		== TIMER_UNDEFERRER_ID   
		|| nIDEvent		== TIMER_UPDATELIST_ID ) {

		//message.Format("Timer Test - %d", nIDEvent);
		//MessageBox (message, title, MB_ICONINFORMATION);
		//return;

		CString string;
		//string.Format("In OnTimer killFlag=%d", killFlag);
		//AfxMessageBox (string, MB_ICONINFORMATION);

		if ( killFlag >= 1 && killFlag <= 8 ) {  
			KillTimer(nIDEvent);
			
			if (killFlag == 1) {
				message = CString((LPCSTR) (IDS_EVALUATOR_ERROR)); 
			}
			if (killFlag ==  2 ) {
					message = CString((LPCSTR) (IDS_CONFIG_FILE_ERROR)); 
			} 
			if (killFlag ==  3 ) {
					message = CString((LPCSTR) (IDS_CONFIG_FILE_CREATE_ERROR)); 
			}
			if (killFlag == 4) {
					message = CString((LPCSTR) (IDS_DATABASE_OPEN_ERROR)); 
			}
			if (killFlag == 5) {
				message = CString((LPCSTR) (IDS_DABASE_ADD_NEW_STATE_ERROR)); 
			}
			if (killFlag == 6) {
				message = CString((LPCSTR) (IDS_DATABASE_UPDATE_PRIORITY_ERROR)); 
			}
			if (killFlag == 8) {
				message = CString((LPCSTR) (IDS_DATABASE_GET_LATEST_SIGNATURE_ERROR)); 
			}

			if (killFlag == 7) {
				CString id;
				switch (returnCode) {
					case (DF_DABASE_OPEN_ERROR):         
						message = CString((LPCSTR) (IDS_DATABASE_OPEN_ERROR));
						break;
					case (DF_DABASE_CLOSE_ERROR):                   
						message = CString((LPCSTR) (IDS_DATABASE_CLOSE_ERROR));
						break;
					case (DF_DABASE_ADD_NEW_STATE_ERROR):     
						message = CString((LPCSTR) (IDS_DATABASE_ADD_NEW_STATE_ERROR));
						break;
					case (DF_DATABASE_UPDATE_RECORD_ERROR):            
						message = CString((LPCSTR) (IDS_DATABASE_UPDATE_RECORD_ERROR));
						break;
					case (DF_DATABASE_GET_RECORD_ERROR):
						message = CString((LPCSTR) (IDS_DATABASE_GET_RECORD_ERROR));
						break;
				}
			}
			MessageBox (message, title, MB_ICONINFORMATION);
			::PostQuitMessage(100);
			return;
		}

		if (killFlag == 10) {
			//config file has been created
			killFlag = 0;
			message = CString((LPCSTR) (IDS_CONFIG_FILE_CREATE_WARNING)); 
			MessageBox (message, title, MB_ICONINFORMATION);
			return;
		}
		if (killFlag == 11) {
			//config file has an empty UNC
			killFlag = 0;
			message = CString((LPCSTR) (IDS_CONFIG_FILE_EMPTY_FIELDS)); 
			MessageBox (message, title, MB_ICONINFORMATION);
			return;
		}
		if (killFlag == 14) {
			killFlag = 0;
		    CString msg((LPCSTR) (IDS_DEFER_MSG)); 
			message.Format(msg, Manager.criticalSample);
			int rc = MessageBox (message, title, MB_ICONQUESTION | MB_YESNO);
			if (rc == IDYES) {

				Manager.deferCriticalSample = 1;
		        POSITION pos;
				DFSample *pSample =  Manager.FindObjectWithIdInHPList(Manager.criticalSample, pos);
				if (pSample != NULL) 
                     pSample->deferCriticalSample = 1;
			    Manager.criticalErrorFlag = 0; 
				sendFlag = 0;
				OnEnable();
				(void) SetTimer(TIMER_PROCESS_ID, Manager.sampleSubmissionInterval * 1000, NULL);					
				(void) SetTimer(TIMER_ARRIVAL_ID, Manager.sampleArrivalCheckInterval * 1000, NULL);					
				(void) SetTimer(TIMER_DATABASE_RESCAN_ID, Manager.databaseRescanInterval * 1000, NULL);					
				if (Manager.statusUpdateInterval != 0 )
					(void) SetTimer (TIMER_STATUS_UPDATE_ID, Manager.statusUpdateInterval * 60 * 1000, NULL);
				if (Manager.undeferrerInterval != 0 )
					(void) SetTimer (TIMER_UNDEFERRER_ID, Manager.undeferrerInterval * 60 * 1000, NULL);
				if (Manager.defImportInterval != 0 )
					(void) SetTimer(TIMER_DEFINITION_IMPORT_ID, Manager.defImportInterval * 60 * 1000, NULL);					
				if (Manager.attributeInterval != 0 )
					(void) SetTimer(TIMER_SAMPLEATTRIBUTE_ID, Manager.attributeInterval * 60 * 1000, NULL);					
            } 
			return;
		}
		if (killFlag == 15) {
			//on critical error 
			killFlag = 0;
		    CString msg((LPCSTR) (IDS_BLOCK_MSG)); 
			message.Format(msg, Manager.criticalSample);
			CTime t = CTime::GetCurrentTime(); 
			CString currentTime= t.Format( "%d %B %Y,  %H:%M:%S" );
			CString boxMessage = "<" + currentTime + ">" + "\n" + message ;
			int rc = MessageBox (boxMessage, title, MB_ICONINFORMATION);
	 		return;
		}
		if (killFlag == 16) {
			// on critical error
			killFlag = 0;
		    CString msg((LPCSTR) (IDS_BLOCK_MSG1)); 
			message.Format(msg, Manager.criticalSample);
			int rc = MessageBox (message, title, MB_ICONINFORMATION);
	 		return;
		}
		if (killFlag == 20) {
			// on defer
			killFlag = 0;
			message = CString((LPCSTR) (IDS_SAMPLE_DEFER_QST)); 
			int rc = MessageBox (message, title, MB_ICONQUESTION | MB_YESNO);
			if (rc == IDYES) {
					(void) SetUserRequest(DFSample::DEFER);
					UpdateSampleList();

            } 
	
			return;
		}

		if (killFlag == 21) {
			//config file has been created
			killFlag = 0;
			message = CString((LPCSTR) (IDS_SELECTION_ERROR)); 
			MessageBox (message, title, MB_ICONINFORMATION);
			return;
		}
		if (killFlag == 22) {
			//config file has been created
			killFlag = 0;
			message = sampleDirPath + CString(": ") +  
				CString((LPCSTR) (IDS_ATTRIBUTE_FILE_ERROR)); 
			MessageBox (message, title, MB_ICONINFORMATION);
			return;
		}
		if (killFlag == 23) {
				// build definition inforamtion message
				killFlag = 0;
				message = CString((LPCSTR) (IDS_WAIT_FOR_NEWDEF_MSG)); 
				MessageBox (message, title, MB_ICONINFORMATION);
			return;
		}


		if ( (strcmp(Manager.UNCPath, "") == 0) || (strcmp(Manager.buildDefFilename, "") == 0) || (strcmp(Manager.defBaseDir, "") == 0) )
			return;
		// critical error while building  a new definition 
		if (Manager.criticalErrorFlag) {
			if (sendFlag  == 0) {
					KillTimer(TIMER_PROCESS_ID);
					KillTimer(TIMER_DATABASE_RESCAN_ID);
					KillTimer(TIMER_ARRIVAL_ID);
					KillTimer(TIMER_UPDATE_ID);
					KillTimer(TIMER_STATUS_UPDATE_ID);
					KillTimer(TIMER_DEFINITION_IMPORT_ID);
					KillTimer(TIMER_UNDEFERRER_ID); 
					class CAVISSendMail avisSendMail;
                    CString msgStr; 
					CString msg = CString((LPCSTR) IDS_MAIL_STRING);
					msgStr.Format(msg, Manager.computerName, Manager.criticalSample); 
					CTime t = CTime::GetCurrentTime(); 
					CString currentTime= t.Format( "%d %B %Y,  %H:%M:%S" );
					CString mailString = "<" + currentTime + ">" + msgStr;
    				BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, mailString);
					killFlag = 15;
					sendFlag = 1;
			}
			UpdateSampleList();
			request = DISABLE;
        }
		// critical error while importing a new definition 
		if (Manager.criticalErrorFlag1) {
			if (sendFlag  == 0) {
					KillTimer(TIMER_PROCESS_ID);
					KillTimer(TIMER_DATABASE_RESCAN_ID);
					KillTimer(TIMER_ARRIVAL_ID);
					KillTimer(TIMER_UPDATE_ID);
					KillTimer(TIMER_STATUS_UPDATE_ID);
					KillTimer(TIMER_DEFINITION_IMPORT_ID);
					KillTimer(TIMER_UNDEFERRER_ID); 
					class CAVISSendMail avisSendMail;
 					CString msgStr = CString((LPCSTR) IDS_MAIL_STRING1);
					CTime t = CTime::GetCurrentTime(); 
					CString currentTime= t.Format( "%d %B %Y,  %H:%M:%S" );
					message.Format(msgStr, Manager.computerName, Manager.secondToken);
					CString mailString = "<" + currentTime + ">" + message;
    				BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, mailString);
					// message about locking the system
				    CString msg1((LPCSTR) (IDS_BLOCK_MSG1)); 
					message.Format(msg1, Manager.secondToken);
					CString boxMessage = "<" + currentTime + ">" + message;
					request = DISABLE;
					sendFlag = 1;
					UpdateSampleList();
					int rc = MessageBox (boxMessage, title, MB_ICONINFORMATION);
				
			}
        }
    }
	if (request == DISABLE) {
			return;
    }
	if ( (strcmp(Manager.UNCPath, "") == 0) || (strcmp(Manager.buildDefFilename, "") == 0) || (strcmp(Manager.defBaseDir, "") == 0) ||  (strcmp(Manager.defImporterDir, "") == 0) )
		return;

#ifdef SCHEDULED_TASK_TEST
// test log
		if (logFlag == 0) {
			strcpy(Manager.testPath, Manager.UNCPath);
			if (strcmp(Manager.testPath, "") != 0) {
				Manager.AppendPath(Manager.testPath, "ScheduledTask.log");
				Manager.logFile = new std::ofstream;
				Manager.logFile->open(Manager.testPath, std::ios::out | std::ios::app);
				Manager.logFile->flush();
				Manager.logFile->close();
				logFlag = 1;
			}
        }
#endif

	int sampleHPCount; 
	int rc = 0;	
	CListCtrl& m_ctlListCtrl=GetListCtrl();
	if (nIDEvent ==  TIMER_ARRIVAL_ID) { 
	// Get new samples for processing
		sampleHPCount = Manager.sampleHPList.GetCount();
		if (Manager.maxSamples != 0 &&  sampleHPCount <  Manager.maxSamples + 1) {
#ifdef _DEBUG
	(void) Manager.LogActivity(0, "Get new samples for processing started", CString());
#endif
			rc = Manager.GetNewSamplesForProcessing(); 
#ifdef _DEBUG
	(void) Manager.LogActivity(0, "Get new samples for rescan ended", CString());
#endif
			if (rc  == DF_SAMPLE_ARRIVED ) {
				//arrivalImportedFlag = 1;
				CorrectArrivalInterval(Back, Imported);
#ifdef _DEBUG
	(void) Manager.LogActivity(0, "UpdateSampleList started", CString());
#endif
				UpdateSampleList();
#ifdef _DEBUG
	(void) Manager.LogActivity(0, "UpdateSampleList ended", CString());
#endif

				if  ( bSortOrder != NONE ) {
					//m_ctlListCtrl.SetRedraw(FALSE);
					//m_ctlListCtrl.SortItems(CompareFunc, (LPARAM) this);
					//m_ctlListCtrl.SetRedraw(TRUE);
					sortFlag = 1;
                }

//				BOOL brc = SortNumericItems( 0, bSortOrder, 0, -1); 
			}
			else {
				if (rc  == DF_GET_SAMPLE_ERROR ) { 
						message = CString((LPCSTR) (IDS_CONFIG_FILE_UNC_EMPTY)); 
						MessageBox ("DB error", title, MB_ICONINFORMATION);
						return;
				} 
				else {
					if (rc == 0) {
						//arrivalImportedFlag = 0;
	    				CorrectArrivalInterval(ToMax, Imported);
                    }
                }
			}	
		}
	}
	if (nIDEvent ==  TIMER_DATABASE_RESCAN_ID) { 
	// Get new samples for rescan
		sampleHPCount = Manager.sampleHPList.GetCount();
		if (Manager.maxSamples != 0 &&  sampleHPCount <  Manager.maxSamples + 1) {
#ifdef _DEBUG
	(void) Manager.LogActivity(0, "GetNewSamplesForRescan started", CString());
#endif
            rc =  Manager.GetNewSamplesForRescan();
#ifdef _DEBUG
	Manager.LogActivity(0, "GetNewSamplesForRescan ended", CString());
#endif
			/*
			if (rc  == DF_SAMPLE_ARRIVED) {
				(void)CorrectArrivalInterval(Back, Rescan);
				UpdateSampleList();
				if  ( bSortOrder != NONE ) {
					m_ctlListCtrl.SetRedraw(FALSE);
					m_ctlListCtrl.SortItems(CompareFunc, (LPARAM) this);
					m_ctlListCtrl.SetRedraw(TRUE);
                }

			}
			else {
				if (rc  == DF_GET_SAMPLE_ERROR ) { 
						message = CString((LPCSTR) (IDS_CONFIG_FILE_UNC_EMPTY)); 
						MessageBox ("DB error", title, MB_ICONINFORMATION);
						return;
				}
				else {
					if (rc == 0)
						(void) CorrectArrivalInterval(ToMax, Rescan );
                }
				
			}
		*/	
		}
		
	}
	if (nIDEvent ==  TIMER_PROCESS_ID) { 
		if(Manager.closeWndRequest == 1) {
				if (Manager.serialFlag == 0 && Manager.packThread == 0) {
       				AfxGetApp()->m_pMainWnd->PostMessage(MSG_REQUEST_CLOSE, 0, 0);
              		Manager.closeWndRequest = 0; 
					return;
                }
			}
		// submit samples for processing 
		sampleHPCount = Manager.sampleHPList.GetCount();
		if (sampleHPCount != 0 ) {
#ifdef _DEBUG
	Manager.LogActivity(0, "Submit samples for processing started", CString());
#endif
			rc = Manager.SubmitSamplesForProcessing(); 
#ifdef _DEBUG
	Manager.LogActivity(0, "Submit samples for processing ended", CString());
#endif

			if (rc != 0) {
//				UpdateSampleList();
			}
		}
	}

    // update priorities in the collection
	if (nIDEvent == TIMER_UPDATE_ID) { 
		int sampleHPCount = Manager.sampleHPList.GetCount();
		if (sampleHPCount != 0 ) {
			int rc = Manager.UpdatePriority();
		    if ( rc == DF_DATABASE_GET_RECORD_ERROR ) 
				killFlag = 6;
        }
    }
    // import new definition
	if (nIDEvent == TIMER_DEFINITION_IMPORT_ID) { 
		if (Manager.serialFlag == 0 ) {
			int rc = Manager.ImportNewDefinitions();
        }
		else 
			Manager. importPending = 1;
    }
    // check deferred samples
	if (nIDEvent == TIMER_UNDEFERRER_ID) { 
			(void)Manager.CheckDeferredSamples();
    }
    // status update 
	if (nIDEvent == TIMER_STATUS_UPDATE_ID ) { 
			(void)Manager.LaunchStatusUpdater();
    }
	if (nIDEvent == TIMER_SAMPLEATTRIBUTE_ID ) { 
		(void) Manager.LaunchSampleAttributeCollector();
    }
	if (nIDEvent == TIMER_UPDATELIST_ID ) { 
#ifdef _DEBUG
	(void) Manager.LogActivity(0, "UpdateSampleList on timer started", CString());
#endif
		    UpdateSampleList();
#ifdef _DEBUG
	(void) Manager.LogActivity(0, "UpdateSampleList on timer ended", CString());
#endif

    }

}
void CDFGuiView::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	CListViewEx::OnClose();
}

int CDFGuiView::SetUserRequest(DFSample::UserRequest request) 
{
	CListCtrl& m_ctlListCtrl=GetListCtrl();
	
	int nCount = m_ctlListCtrl.GetItemCount();
    DFSample *inListSample;
    for (int i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
			
			if (lvi.state & (LVIS_SELECTED )) {
				if (request == DFSample::DEFER || request == DFSample::STOP) { 
					if ( request == DFSample::DEFER && inListSample->GetCurrentStatus() == DFSample::CriticalError)
						     inListSample->SetUserRequest(request);
					DFEvalState::StateGroup group = Manager.Matrix.GetStateGroup(inListSample->GetCurrentStateStr());
					if (group == DFEvalState::FULLBUILD || group == DFEvalState::BUILD) {
					   if (Manager.serialFlag  &&  inListSample->noBuildFlag) 
						     inListSample->SetUserRequest(request);
					}
					else {
						if (group != DFEvalState::ARCHIVE)
							inListSample->SetUserRequest(request);
                    } 
                }  
				else {
					    inListSample->SetUserRequest(request);
                }

			}
    }
	return 0;
}

int CDFGuiView::OnLauncherShow()
{
    CDFGuiApp *pApp = (CDFGuiApp *) AfxGetApp();
    CWnd * pDFLWnd = CWnd::FromHandle(pApp->DFLauncherWnd);

    pDFLWnd->ShowWindow(SW_RESTORE);
    pDFLWnd->SetForegroundWindow();
	return 0;
}

void CDFGuiView::OnNcDestroy() 
{
	CListViewEx::OnNcDestroy();
	
}
void CDFGuiView::TermView()
{    
// test log
//	  testFile->close();

     (void) Manager.TermManager();

}

BOOL CDFGuiView::IsAtLeastOneSelectedNotOnHold() 
{
    CListCtrl& m_ctlListCtrl=GetListCtrl();
	DFSample *inListSample;
	int i;
	int nCount = m_ctlListCtrl.GetItemCount();

    for (i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
             
			if (lvi.state & (LVIS_SELECTED )) {
				if (inListSample->GetUserRequest() != DFSample::PAUSE && inListSample->GetUserRequest() != DFSample::STOP )
					return TRUE;
            }
			
    }
	return FALSE;

}
BOOL CDFGuiView::IsAtLeastOneSelectedOnHold() 
{
    CListCtrl& m_ctlListCtrl=GetListCtrl();
	DFSample  *inListSample;
	int i;
	int nCount = m_ctlListCtrl.GetItemCount();

    for (i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
             
			if (lvi.state & (LVIS_SELECTED )) {
				if (inListSample->GetUserRequest() == DFSample::PAUSE || inListSample->GetUserRequest() == DFSample::STOP)
					return TRUE;
            }
			
    }
	return FALSE;

}
void CDFGuiView::OnUpdateHold(CCmdUI* pCmdUI) 
{ 
	if(Manager.closeWndRequest == 1) {
		  pCmdUI->Enable (FALSE);
          return;	
    }
	// Check if all the items in the selected group can be put on hold

	if (request == ENABLE) {
//		if ( IsAtLeastOneSelectedNotOnHold())
		if ( AreAllSelectedNotOnHold())
			  pCmdUI->Enable (TRUE);
		else 
			  pCmdUI->Enable (FALSE);
	}
	else 
		  pCmdUI->Enable (FALSE);
	
	
}

void CDFGuiView::OnUpdateStop(CCmdUI* pCmdUI) 
{
	// Check if all the items in the selected group can be put on hold
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (FALSE);
/*
	if (request == ENABLE) {
		if ( AreAllSelectedNotOnHold())
			  pCmdUI->Enable (TRUE);
		else 
			  pCmdUI->Enable (FALSE);
    }
	else
			  pCmdUI->Enable (FALSE);
    
*/	
}

void CDFGuiView::OnUpdateResume(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(Manager.closeWndRequest == 1) {
		  pCmdUI->Enable (FALSE);
          return;	
    }

	if (request == ENABLE) {
//		if ( IsAtLeastOneSelectedOnHold())
		if ( AreAllSelectedOnHold())
			  pCmdUI->Enable (TRUE);
		else 
			  pCmdUI->Enable (FALSE);
	}
	else
		  pCmdUI->Enable (FALSE);

}
void CDFGuiView::OnHold() 
{
	// TODO: Add your command handler code here
	(void) SetUserRequest(DFSample::PAUSE);
	UpdateSampleList();
	
}
void CDFGuiView::OnStop() 
{
	// TODO: Add your command handler code here
	(void) SetUserRequest(DFSample::STOP);
	UpdateSampleList();
	
}
void CDFGuiView::OnResume() 
{
	// TODO: Add your command handler code here
	(void) SetUserRequest(DFSample::RESUME);
	UpdateSampleList();
	
}

// Note: Disable option is to stop the submission of all the jobs and to      
// allow all ongoing jobs to end                                              
void CDFGuiView::OnDisable() 
{
	request = DISABLE;
	UpdateSampleList();

}
void CDFGuiView::OnEnable() 
{
	if (Manager.criticalErrorFlag1) {
		if (Manager.criticalErrorFlag == 0) {
		    request = ENABLE;
			(void) SetTimer(TIMER_PROCESS_ID, Manager.sampleSubmissionInterval * 1000, NULL);					
			(void) SetTimer(TIMER_ARRIVAL_ID, Manager.sampleArrivalCheckInterval * 1000, NULL);					
			(void) SetTimer(TIMER_DATABASE_RESCAN_ID, Manager.databaseRescanInterval * 1000, NULL);					
			if (Manager.statusUpdateInterval != 0 )
				(void) SetTimer (TIMER_STATUS_UPDATE_ID, Manager.statusUpdateInterval * 60 * 1000, NULL);
			if (Manager.undeferrerInterval != 0 )
				(void) SetTimer (TIMER_UNDEFERRER_ID, Manager.undeferrerInterval * 60 * 1000, NULL);
			if (Manager.defImportInterval != 0 )
				(void) SetTimer(TIMER_DEFINITION_IMPORT_ID, Manager.defImportInterval * 60 * 1000, NULL);					
			if (Manager.attributeInterval != 0 )
				(void) SetTimer(TIMER_SAMPLEATTRIBUTE_ID, Manager.attributeInterval * 60 * 1000, NULL);					
		}
		UpdateSampleList();
		Manager.criticalErrorFlag1 = 0;
		sendFlag = 0;
		return;
    }

	if (Manager.criticalErrorFlag) {
		killFlag = 14;
	    return;
    }
	else {
		request = ENABLE;
		UpdateSampleList();
		return;
	}


}
void CDFGuiView::OnUpdateDisable(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(Manager.closeWndRequest == 1) {
		  pCmdUI->Enable (FALSE);
          return;	
    }

	if (Manager.sampleHPList.GetCount() == 0 || request == DISABLE)
		  pCmdUI->Enable (FALSE);
	else 
	      pCmdUI->Enable (TRUE);
	
}
void CDFGuiView::OnUpdateEnable(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(Manager.closeWndRequest == 1) {
		  pCmdUI->Enable (FALSE);
          return;	
    }

  if (request == ENABLE)
		  pCmdUI->Enable (FALSE);
  else 
	      pCmdUI->Enable (TRUE);
	
}


void CDFGuiView::OnUpdateHoldall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(Manager.closeWndRequest == 1) {
		  pCmdUI->Enable (FALSE);
          return;	
    }

	if (request == ENABLE) {
		if ( Manager.IsAtLeastOneNotOnHold())
			  pCmdUI->Enable (TRUE);
		else 
			  pCmdUI->Enable (FALSE);
	}
	else 
        pCmdUI->Enable (FALSE);
	
}

void CDFGuiView::OnHoldall() 
{
	// TODO: Add your command handler code here
    Manager.SetupDisableRequest();
	UpdateSampleList();
	
}

void CDFGuiView::OnResumeall() 
{
	// TODO: Add your command handler code here
		Manager.SetupEnableRequest();
		UpdateSampleList();

}

void CDFGuiView::OnUpdateResumeall(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(Manager.closeWndRequest == 1) {
		  pCmdUI->Enable (FALSE);
          return;	
    }

	if (request == ENABLE) {
		if ( Manager.IsAtLeastOneOnHold())
			  pCmdUI->Enable (TRUE);
		else 
			  pCmdUI->Enable (FALSE);
	}
	else 
        pCmdUI->Enable (FALSE);
}

void CDFGuiView::OnConfigure() 
{
	// TODO: Add your command handler code here
	DFConfigureDialog dfConfigureDialog;
                    
//	CString	strCacheInterval; 
//	CString	strDirInterval;
	CString	strMaxSamples;
	CString	strSubmissionInterval;
	CString	strSubmissionIntervalNew;
	CString	strUNCPath; 
	CString	strBuildDefFilename; 
	CString	strDefBaseDir; 
	CString	strDefImporterDir; 
	CString	strArrivalInterval;
	CString	strArrivalIntervalNew;
	CString	strDatabaseRescanIntervalNew;
	CString	strDatabaseRescanInterval;
	CString	strDefImportInterval;
	CString	strDefImportIntervalNew;
	CString	strStatusUpdateInterval;
	CString	strStatusUpdateIntervalNew;
	CString	strUndeferrerInterval;
	CString	strUndeferrerIntervalNew;
	CString	strAttributeInterval;
	CString	strAttributeIntervalNew;
	
	MaxSamples = Manager.maxSamples;
	SubmissionInterval = Manager.sampleSubmissionInterval;
	DefImportInterval = Manager.defImportInterval;
	StatusUpdateInterval = Manager.statusUpdateInterval;
	UndeferrerInterval = Manager.undeferrerInterval;
	AttributeInterval = Manager.attributeInterval;
	UNCPath = CString(Manager.UNCPath); 
	buildDefFilename = CString(Manager.buildDefFilename); 
	defBaseDir = CString(Manager.defBaseDir); 
	defImporterDir = CString(Manager.defImporterDir); 
//	ArrivalInterval = Manager.sampleArrivalCheckInterval;
//	DatabaseRescanInterval = Manager.databaseRescanInterval;

//	strCacheInterval.Format("%d", Manager.cacheCheckInterval); 
//	strDirInterval.Format("%d", Manager.dirCheckInterval);
	strMaxSamples.Format("%d", Manager.maxSamples);
	strSubmissionInterval.Format("%d", Manager.sampleSubmissionInterval);
	strUNCPath = CString(Manager.UNCPath); 
	strBuildDefFilename = CString(Manager.buildDefFilename); 
	strDefBaseDir = CString(Manager.defBaseDir); 
	strDefImporterDir = CString(Manager.defImporterDir); 
	strArrivalInterval.Format("%d", Manager.sampleArrivalCheckInterval);
	strDatabaseRescanInterval.Format("%d", Manager.databaseRescanInterval);
	strDefImportInterval.Format("%d", Manager.defImportInterval);
	strStatusUpdateInterval.Format("%d", Manager.statusUpdateInterval);
	strUndeferrerInterval.Format("%d", Manager.undeferrerInterval);
	strAttributeInterval.Format("%d", Manager.attributeInterval);


//	dfConfigureDialog.m_strCacheInterval = strCacheInterval;
//	dfConfigureDialog.m_strDirInterval =   strDirInterval;
	dfConfigureDialog.m_strMaxNumber =     strMaxSamples;
	dfConfigureDialog.m_strSubmissionInterval =  strSubmissionInterval; 
	dfConfigureDialog.m_strUNCPath =   strUNCPath;
	dfConfigureDialog.m_strBuildDefFilename =   strBuildDefFilename;
	dfConfigureDialog.m_strDefBaseDir =   strDefBaseDir;
	dfConfigureDialog.m_strDefImporterDir =   strDefImporterDir;
	dfConfigureDialog.m_strArrivalInterval =  strArrivalInterval;
	dfConfigureDialog.m_strDatabaseRescanInterval =  strDatabaseRescanInterval;
	dfConfigureDialog.m_strDefImportInterval = strDefImportInterval;
	dfConfigureDialog.m_strStatusUpdateInterval =  strStatusUpdateInterval;
	dfConfigureDialog.m_strUndeferrerInterval =  strUndeferrerInterval;
	dfConfigureDialog.m_strAttributeInterval =  strAttributeInterval;

    int msgFlag = 0;

    int rc = dfConfigureDialog.DoModal();

    if (rc == IDOK)
    {
		// unc path
		UNCPath = dfConfigureDialog.m_strUNCPath;
		//build def filename
		buildDefFilename = dfConfigureDialog.m_strBuildDefFilename;
		//def base name
		defBaseDir = dfConfigureDialog.m_strDefBaseDir;
		// def importer dir 
		defImporterDir = dfConfigureDialog.m_strDefImporterDir;

		if (!UNCPath.IsEmpty()) { 
			UNCPath.TrimLeft();
			UNCPath.TrimRight();
		}

		if ( !buildDefFilename.IsEmpty()) {
			buildDefFilename.TrimLeft();
			buildDefFilename.TrimRight();
        }

 		if( !defBaseDir.IsEmpty()) {
			defBaseDir.TrimLeft();
			defBaseDir.TrimRight();
        }
 		if( !defImporterDir.IsEmpty()) {
			defImporterDir.TrimLeft();
			defImporterDir.TrimRight();
        }

    // submission interval        
		strSubmissionIntervalNew = dfConfigureDialog.m_strSubmissionInterval; 
		if (strSubmissionIntervalNew.CompareNoCase(strSubmissionInterval)) {
			if (!strSubmissionIntervalNew.IsEmpty()) {
					SubmissionInterval = atol((LPTSTR) (LPCTSTR) strSubmissionIntervalNew); 
				if (SubmissionInterval < DEFAULT_SAMPLE_SUBMISSION_INTERVAL || SubmissionInterval > DEFAULT_SAMPLE_SUBMISSION_INTERVAL_MAX )
					SubmissionInterval = (DEFAULT_SAMPLE_SUBMISSION_INTERVAL);
				KillTimer(TIMER_PROCESS_ID);
			   (void) SetTimer(TIMER_PROCESS_ID, SubmissionInterval * 1000, NULL);					

			}
		}
    // arrival interval 
		strArrivalIntervalNew = dfConfigureDialog.m_strArrivalInterval;
		if (strArrivalIntervalNew.IsEmpty()) 
           strArrivalIntervalNew = strArrivalInterval;
		ArrivalInterval = atol((LPTSTR) (LPCTSTR) strArrivalIntervalNew); 
		if ( ArrivalInterval <= 0 || ArrivalInterval > DEFAULT_SAMPLE_ARRIVAL_INTERVAL_MAX )
			ArrivalInterval = (DEFAULT_SAMPLE_ARRIVAL_INTERVAL);  
		KillTimer(TIMER_ARRIVAL_ID);
		(void) SetTimer(TIMER_ARRIVAL_ID, ArrivalInterval * 1000, NULL);					
 

    // database rescan interval 
		strDatabaseRescanIntervalNew = dfConfigureDialog.m_strDatabaseRescanInterval;
		if (strDatabaseRescanIntervalNew.IsEmpty()) 
           strDatabaseRescanIntervalNew = strDatabaseRescanInterval;
		DatabaseRescanInterval = atol((LPTSTR) (LPCTSTR) strDatabaseRescanIntervalNew); 
		if ( DatabaseRescanInterval <= 0 || DatabaseRescanInterval > DEFAULT_SAMPLE_ARRIVAL_INTERVAL_MAX )
			DatabaseRescanInterval = (DEFAULT_DATABASE_RESCAN_INTERVAL);  
		KillTimer(TIMER_DATABASE_RESCAN_ID);
		(void) SetTimer(TIMER_DATABASE_RESCAN_ID, DatabaseRescanInterval * 1000, NULL);					

    // defImportInterval        
		strDefImportIntervalNew = dfConfigureDialog.m_strDefImportInterval; 
		if (strDefImportIntervalNew.CompareNoCase(strDefImportInterval)) {
			if (!strDefImportIntervalNew.IsEmpty()) {
					DefImportInterval = atol((LPTSTR) (LPCTSTR) strDefImportIntervalNew); 
				if (DefImportInterval < DEFAULT_DEF_IMPORT_INTERVAL || DefImportInterval > DEFAULT_DEF_IMPORT_INTERVAL_MAX )
					DefImportInterval = (DEFAULT_DEF_IMPORT_INTERVAL);
				KillTimer(TIMER_DEFINITION_IMPORT_ID);
				if (DefImportInterval != 0)
					(void) SetTimer(TIMER_DEFINITION_IMPORT_ID, DefImportInterval * 60*1000, NULL);					
 

			}
        }
    // status update Interval        
		strStatusUpdateIntervalNew = dfConfigureDialog.m_strStatusUpdateInterval; 
		if (strStatusUpdateIntervalNew.CompareNoCase(strStatusUpdateInterval)) {
			if (!strStatusUpdateIntervalNew.IsEmpty()) {
					StatusUpdateInterval = atol((LPTSTR) (LPCTSTR) strStatusUpdateIntervalNew); 
				if (StatusUpdateInterval < DEFAULT_STATUS_UPDATE_INTERVAL || StatusUpdateInterval > DEFAULT_STATUS_UPDATE_INTERVAL_MAX )
					StatusUpdateInterval = (DEFAULT_STATUS_UPDATE_INTERVAL);
				KillTimer(TIMER_STATUS_UPDATE_ID);
				if (StatusUpdateInterval != 0 )
			      (void) SetTimer(TIMER_STATUS_UPDATE_ID, StatusUpdateInterval * 60 *1000, NULL);					
 			}
		
        }
    // undeferrer Interval        
		strUndeferrerIntervalNew = dfConfigureDialog.m_strUndeferrerInterval; 
		if (strUndeferrerIntervalNew.CompareNoCase(strUndeferrerInterval)) {
			if (!strUndeferrerIntervalNew.IsEmpty()) {
					UndeferrerInterval = atol((LPTSTR) (LPCTSTR) strUndeferrerIntervalNew); 
				if (UndeferrerInterval < DEFAULT_UNDEFERRER_INTERVAL || UndeferrerInterval > DEFAULT_UNDEFERRER_INTERVAL_MAX )
					UndeferrerInterval = (DEFAULT_UNDEFERRER_INTERVAL);
				KillTimer(TIMER_UNDEFERRER_ID);
				if ( UndeferrerInterval != 0 )
				   (void) SetTimer( TIMER_UNDEFERRER_ID	, UndeferrerInterval * 60 * 1000, NULL);					
 			}
		
        }
    // attribute Interval        
		strAttributeIntervalNew = dfConfigureDialog.m_strAttributeInterval; 
		if (strAttributeIntervalNew.CompareNoCase(strAttributeInterval)) {
			if (!strAttributeIntervalNew.IsEmpty()) {
					AttributeInterval = atol((LPTSTR) (LPCTSTR) strAttributeIntervalNew); 
				if (AttributeInterval < DEFAULT_ATTRIBUTE_INTERVAL || AttributeInterval > DEFAULT_ATTRIBUTE_INTERVAL_MAX )
					AttributeInterval = (DEFAULT_ATTRIBUTE_INTERVAL);
				KillTimer(TIMER_SAMPLEATTRIBUTE_ID);
				if ( AttributeInterval != 0 )
				   (void) SetTimer( TIMER_SAMPLEATTRIBUTE_ID	, AttributeInterval * 60 * 1000, NULL);					
 			}
		
        }

   // max number 
		strMaxSamples = dfConfigureDialog.m_strMaxNumber;
        if (!strMaxSamples.IsEmpty()) {
			MaxSamples = atol(strMaxSamples); 
			if ( MaxSamples > DEFAULT_SAMPLES_MAX )
				MaxSamples= DEFAULT_SAMPLES;  
        }
		SaveConfigSettings();
    }
	return;
}

void CDFGuiView::SaveConfigSettings()
{
    FILE * fp;
	char cfgPath[1024];
	char bcfgPath[1024];

	strcpy(cfgPath, Manager.modulePath);
	Manager.AppendPath(cfgPath,"avisdf.cfg");


	strcpy(bcfgPath, cfgPath);
	strcat(bcfgPath, ".bak");
//    unlink (bcfgPath);
//    rename (cfgPath, bcfgPath);
	CopyFile(cfgPath, bcfgPath, FALSE);
    fp = fopen (cfgPath, _T("w"));

	CParamValue paramValue;

    if (fp)
    {

		//CString checkCashIntervalTag((LPCTSTR)IDS_CHECK_CASH_INT );
		//CString checkCashIntervalTag((LPCTSTR)IDS_CHECK_CASH_INT );
		CString submissionIntervalTag((LPCTSTR) IDS_TIMER_INTERVAL);
		CString arrivalIntervalTag((LPCTSTR) IDS_SAMPLE_REQUEST_DELAY);
		CString databaseRescanIntervalTag((LPCTSTR) IDS_DATABASE_RESCAN_INTERVAL);
		CString defImportIntervalTag((LPCTSTR) IDS_DEF_IMPORT_INTERVAL);
		CString statusUpdateIntervalTag((LPCTSTR) IDS_STATUS_UPDATE_INTERVAL);
		CString undeferrerIntervalTag((LPCTSTR) IDS_UNDEFERRER_INTERVAL); 
		CString attributeIntervalTag((LPCTSTR) IDS_ATTRIBUTE_INTERVAL); 
		CString maxSamplesTag((LPCTSTR) IDS_MAX_SAMPLES);
		CString pathTag((LPCTSTR) IDS_UNCPATH); 
		CString buildDefFilenameTag((LPCTSTR) IDS_BUILD_DEF_FILENAME); 
		CString defBaseDirTag((LPCTSTR) IDS_DEF_BASE_DIR); 
		CString defImporterDirTag((LPCTSTR) IDS_DEF_IMPORTER_DIR);
		
		CString cfgProlog((LPCTSTR) IDS_CFG_PROLOG);
		fwrite((LPCTSTR)cfgProlog, cfgProlog.GetLength(), 1, fp);
    
		CString strSubmissionInterval;
		strSubmissionInterval.Format("%d", SubmissionInterval);
		paramValue.AddParamValue (submissionIntervalTag,  strSubmissionInterval );

		CString strArrivalInterval;   
		strArrivalInterval.Format("%d", ArrivalInterval);
		paramValue.AddParamValue (arrivalIntervalTag, strArrivalInterval);

		CString strDatabaseRescanInterval;   
		strDatabaseRescanInterval.Format("%d", DatabaseRescanInterval);
		paramValue.AddParamValue (databaseRescanIntervalTag, strDatabaseRescanInterval);

		CString strDefImportInterval;   
		strDefImportInterval.Format("%d", DefImportInterval);
		paramValue.AddParamValue (defImportIntervalTag, strDefImportInterval);

		CString strStatusUpdateInterval;   
		strStatusUpdateInterval.Format("%d", StatusUpdateInterval);
		paramValue.AddParamValue (statusUpdateIntervalTag, strStatusUpdateInterval);

		CString strUndeferrerInterval;   
		strUndeferrerInterval.Format("%d", UndeferrerInterval);
		paramValue.AddParamValue (undeferrerIntervalTag, strUndeferrerInterval);

		CString strAttributeInterval;   
		strAttributeInterval.Format("%d", AttributeInterval);
		paramValue.AddParamValue (attributeIntervalTag, strAttributeInterval);


		CString strMaxSamples;
		strMaxSamples.Format("%d", MaxSamples );
		paramValue.AddParamValue (maxSamplesTag, strMaxSamples );

		if (!UNCPath.IsEmpty()) { 
			paramValue.AddParamValue (pathTag, UNCPath);
			strcpy(Manager.UNCPath, (LPTSTR) (LPCTSTR) UNCPath);
		}

		if ( !buildDefFilename.IsEmpty()) {
	   		paramValue.AddParamValue (buildDefFilenameTag, buildDefFilename);
			strcpy(Manager.buildDefFilename, (LPTSTR) (LPCTSTR) buildDefFilename);
        }

 		if( !defBaseDir.IsEmpty()) {
			paramValue.AddParamValue (defBaseDirTag, defBaseDir);
			strcpy(Manager.defBaseDir, (LPTSTR) (LPCTSTR) defBaseDir);
        }
 		if( !defImporterDir.IsEmpty()) {
			paramValue.AddParamValue (defImporterDirTag, defImporterDir);
			strcpy(Manager.defImporterDir, (LPTSTR) (LPCTSTR) defImporterDir);
        }
        
		int brc = paramValue.WriteParamValue (fp);
        if (brc == FALSE) {
              AfxMessageBox ("Error in creating config file.");
			  fclose (fp);
			  return;
        }
		fclose(fp);
		//  pass the values to the Manager  
		//Manager.dirCheckInterval = DirInterval; 
		//Manager.cacheCheckInterval = CacheInterval; 
		Manager.sampleSubmissionInterval = SubmissionInterval; 
		Manager.sampleArrivalCheckInterval = ArrivalInterval; 
		Manager.databaseRescanInterval = DatabaseRescanInterval; 
		Manager.defImportInterval = DefImportInterval; 
		Manager.attributeInterval = AttributeInterval; 
		Manager.statusUpdateInterval = StatusUpdateInterval; 
		Manager.undeferrerInterval = UndeferrerInterval;
		Manager.arrivalCount = 0;
		Manager.maxSamples = MaxSamples; 

		enteredArrivalInterval = ArrivalInterval;
		enteredDatabaseRescanInterval = DatabaseRescanInterval;

		return;
	}
}

void CDFGuiView::OnUpdateConfigure(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
}

void CDFGuiView::OnStatistics() 
{
	// TODO: Add your command handler code here
	CDFStatisticsDialog dfStatisticsDialog;
	CTime t = Manager.startTime;
	CString strStartTime = t.Format( "%d %B %Y,  %H:%M:%S" );
	CString strNumberOfSamples;
	long totalSamples = Manager.arrivedSamples - Manager.returnedBackSamples;
	strNumberOfSamples.Format("%d", totalSamples);

	CString strNumberOfSuccessfulSamples;
	strNumberOfSuccessfulSamples.Format("%d", Manager.successfulSamples);

	CString strNumberOfRescannedSamples;
	strNumberOfRescannedSamples.Format("%d", Manager.rescannedSamples);

//	CString strNumberOfCriticalSamples;
//	strNumberOfCriticalSamples.Format("%d", Manager.criticalSamples);

	CString strNumberOfDeferredSamples;
	strNumberOfDeferredSamples.Format("%d", Manager.deferredSamples);

	CString strAverageProcessingTime;
	long h = Manager.averageProcessingTime/3600;
	long sec = Manager.averageProcessingTime - h * 3600;
	long min = sec/60;
	sec = sec - 60* min;
	strAverageProcessingTime.Format("%02d:%02d:%02d", h, min, sec);

	
    dfStatisticsDialog.m_strStartTime = strStartTime;
	dfStatisticsDialog.m_strNumberOfSamples = strNumberOfSamples;
	dfStatisticsDialog.m_strNumberOfSuccessfulSamples = strNumberOfSuccessfulSamples;
	dfStatisticsDialog.m_strRescannedSamples = strNumberOfRescannedSamples;
//	dfStatisticsDialog.m_strCriticalSamples = strNumberOfCriticalSamples;
	dfStatisticsDialog.m_strDeferredSamples = strNumberOfDeferredSamples;
	dfStatisticsDialog.m_strAverageProcessingTime = strAverageProcessingTime; 

	int rc = dfStatisticsDialog.DoModal();

    if (rc == IDOK)
    {
    }
	
}

void CDFGuiView::OnUpdateStatistics(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	
}

void CDFGuiView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
    CPoint local = point;
    ScreenToClient(&local);
    
    CListViewEx::OnRButtonDown(MK_RBUTTON, local);

    CMenu Menu;
    CMenu *subMenu;
    
    Menu.LoadMenu(IDR_MAINFRAME);
    subMenu = Menu.GetSubMenu(2);
    
    if (subMenu)
    {
        subMenu->TrackPopupMenu( TPM_LEFTALIGN,
            point.x,
            point.y,
            AfxGetMainWnd() );
    }
	
}

void CDFGuiView::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult)
{
    HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

    if( phdn->iButton == 0 )   // left button pressed NMHEADER message
    {
		if( phdn->iItem == 0 )
        {
			nSortedCol  = 0;
            switch (bSortOrder)
            {
            case ASCENDING:
                bSortOrder = DESCENDING;
                break;

            case DESCENDING:
            default:
                bSortOrder = ASCENDING;
                break;

            }
        }
        if( phdn->iItem == 1 )
        {
			nSortedCol = 1;
            switch (bSortOrder)
            {
            case ASCENDING:
                bSortOrder = DESCENDING;
                break;

            case DESCENDING:
            default:
                bSortOrder = ASCENDING;
                break;

            }
        }

	}

	*pResult = 0;
	//Set the sortFlag so that in the timer callback function, we can
	//call the SortList method safely.
	sortFlag = 1;
}

//Added the following function on April/27/2000.
//The code below was mainly moved from the OnHeaderClicked method.
//This method will be called from the timer callback (OnTimer) method.
void CDFGuiView::SortList(void)
{
	//Set the sortFlag to 0 so that timer callback method won't call this method in a loop.
	sortFlag = 0;
	CListCtrl& ListCtrl=GetListCtrl();
	HDITEM hditem;
	char buffer[256]; 

	CHeaderCtrl* pHeader = (CHeaderCtrl*)ListCtrl.GetDlgItem(0);
	//hditem.mask =  HDI_TEXT | HDI_FORMAT;
    hditem.pszText = buffer;
    hditem.cchTextMax = 256;

    if (nSortedCol == 0) 
    {
        ListCtrl.SetRedraw(FALSE);
		SortTextItems( 0, bSortOrder, 0, -1);
//		ListCtrl.SortItems(CompareFunc, (LPARAM) this);
		hditem.mask =  HDI_FORMAT;
		pHeader->GetItem(0, &hditem);
		hditem.mask = HDI_TEXT | HDI_FORMAT;
		hditem.mask = HDI_BITMAP | HDI_FORMAT;
		hditem.fmt |= HDF_LEFT;
		hditem.fmt |= HDF_BITMAP;

		if( bSortOrder == ASCENDING ) {
				hditem.hbm = (HBITMAP)mybitmap4.GetSafeHandle();
        }
		else  {
				hditem.hbm = (HBITMAP)mybitmap5.GetSafeHandle();

		}
		pHeader->SetItem(0, &hditem);


		hditem.mask =  HDI_FORMAT;
		pHeader->GetItem(1, &hditem);
		hditem.fmt &= ~HDF_BITMAP;
		pHeader->SetItem(1, &hditem);
        ListCtrl.SetRedraw(TRUE);

    }
	if (nSortedCol == 1) {
	    ListCtrl.SetRedraw(FALSE);
		SortTextItems( 1, bSortOrder, 0, -1);
//		ListCtrl.SortItems(CompareFunc, (LPARAM) this);
		hditem.mask =  HDI_FORMAT;
		pHeader->GetItem(1, &hditem);
		hditem.mask = HDI_TEXT | HDI_FORMAT;
		hditem.mask = HDI_BITMAP | HDI_FORMAT;
		hditem.fmt |= HDF_LEFT;
		hditem.fmt |= HDF_BITMAP;

		if( bSortOrder == ASCENDING ) {
				hditem.hbm = (HBITMAP)mybitmap4.GetSafeHandle();
        }
		else  {
				hditem.hbm = (HBITMAP)mybitmap5.GetSafeHandle();
		}
		pHeader->SetItem(1, &hditem);

		hditem.mask =  HDI_FORMAT;
		pHeader->GetItem(0, &hditem);
		hditem.fmt &= ~HDF_BITMAP;
		pHeader->SetItem(0, &hditem);
        ListCtrl.SetRedraw(TRUE);
    }
}


// SortTextItems	- Sort the list based on column text
// Returns		- Returns true for success
// nCol			- column that contains the text to be sorted
// bAscending		- indicate sort order
// low			- row to start scanning from - default row is 0
// high			- row to end scan. -1 indicates last row
BOOL CDFGuiView::SortTextItems(int nCol, BOOL bAscending, int low, int high)
{
	CListCtrl& ListCtrl=GetListCtrl();

	if( nCol >= ((CHeaderCtrl*)ListCtrl.GetDlgItem(0))->GetItemCount() )
		return FALSE;

	if( high == -1 ) high = ListCtrl.GetItemCount() - 1;

	int lo = low;
	int hi = high;

	if( hi <= lo ) return FALSE;

	unsigned int midItem = atoi (ListCtrl.GetItemText( (lo+hi)/2, nCol ));

	// loop through the list until indices cross
	while( lo <= hi )
	{
		// rowText will hold all column text for one row
		CStringArray rowText;

		// find the first element that is greater than or equal to 
		// the partition element starting from the left Index.
		if( bAscending )
			while( ( lo < high ) && ( atoi(ListCtrl.GetItemText(lo, nCol)) < midItem ) )
				++lo;
		else
			while( ( lo < high ) && ( atoi(ListCtrl.GetItemText(lo, nCol)) > midItem ) )
				++lo;

		// find an element that is smaller than or equal to 
		// the partition element starting from the right Index.
		if( bAscending )
			while( ( hi > low ) && ( atoi(ListCtrl.GetItemText(hi, nCol)) > midItem ) )
				--hi;
		else
			while( ( hi > low ) && ( atoi(ListCtrl.GetItemText(hi, nCol)) < midItem ) )
				--hi;

		// if the indexes have not crossed, swap
		// and if the items are not equal
		if( lo <= hi )
		{
			// swap only if the items are not equal
			if( ListCtrl.GetItemText(lo, nCol) != ListCtrl.GetItemText(hi, nCol))
			{
				// swap the rows
				LV_ITEM lvitemlo, lvitemhi;
				int nColCount = 
					((CHeaderCtrl*)ListCtrl.GetDlgItem(0))->GetItemCount();
				rowText.SetSize( nColCount );
				int i;
				for( i=0; i<nColCount; i++)
					rowText[i] = ListCtrl.GetItemText(lo, i);
				lvitemlo.mask = LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
				lvitemlo.iItem = lo;
				lvitemlo.iSubItem = 0;
				lvitemlo.stateMask = LVIS_CUT | LVIS_DROPHILITED | 
						LVIS_FOCUSED |  LVIS_SELECTED | 
						LVIS_OVERLAYMASK | LVIS_STATEIMAGEMASK;

				lvitemhi = lvitemlo;
				lvitemhi.iItem = hi;

				ListCtrl.GetItem( &lvitemlo );
				ListCtrl.GetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					ListCtrl.SetItemText(lo, i, ListCtrl.GetItemText(hi, i));

				lvitemhi.iItem = lo;
				ListCtrl.SetItem( &lvitemhi );

				for( i=0; i<nColCount; i++)
					ListCtrl.SetItemText(hi, i, rowText[i]);

				lvitemlo.iItem = hi;
				ListCtrl.SetItem( &lvitemlo );
			}

			++lo;
			--hi;
		}
	}

	// If the right index has not reached the left side of array
	// must now sort the left partition.
	if( low < hi )
		SortTextItems( nCol, bAscending , low, hi);

	// If the left index has not reached the right side of array
	// must now sort the right partition.
	if( lo < high )
		SortTextItems( nCol, bAscending , lo, high );

	return TRUE;
}

/*
//This callback function doesn't work reliably on a dynamically updated list.
//We get illegal addresses in lParam1.
static int CALLBACK CDFGuiView::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
 
    CDFGuiView * cdfGuiView = (CDFGuiView *) lParamSort;
    DFSample * pSample1 = (DFSample *) lParam1;
    DFSample * pSample2 = (DFSample*) lParam2;

    if (cdfGuiView && pSample1 && pSample2)
    {
        int rc = 0;
		long sample1, sample2, priority1, priority2;

        switch (cdfGuiView->bSortOrder)
        {
        case DESCENDING:
            if (cdfGuiView->nSortedCol == 0)
            {
				sample1 = atoi(pSample1->GetCookieId());
				sample2 = atoi(pSample2->GetCookieId());
				if (sample2 < sample1) rc = -1;
				if (sample2 > sample1) rc = 1;
            
            }
            if (cdfGuiView->nSortedCol == 1)
            {
					priority1 = pSample1->GetPriority();
					priority2 = pSample2->GetPriority();
					if (priority2 < priority1 ) rc = -1;
					if (priority2 > priority1 ) rc =  1;

            }

            return rc;
        case ASCENDING:
            if (cdfGuiView->nSortedCol == 0)
            {
				sample1 = atoi(pSample1->GetCookieId());
				sample2 = atoi(pSample2->GetCookieId());
				if (sample1 < sample2) rc = -1;
				if (sample1 > sample2) rc = 1;
            }
            if (cdfGuiView->nSortedCol == 1) {
					priority1 = pSample1->GetPriority();
					priority2 = pSample2->GetPriority();
					if (priority1 < priority2 ) rc = -1;
					if (priority1 > priority2 ) rc =  1;
            }

            return rc;
        }
    }
    return 0;
}
*/
void CDFGuiView::OnChangepriority() 
{
	// TODO: Add your command handler code here
	CPriorityDialog priorityDialog;
                    
	CString	strTrackingNumber; 
	CString	strPriority;
	CListCtrl& m_ctlListCtrl=GetListCtrl();

	int nCount = m_ctlListCtrl.GetItemCount();
    DFSample *inListSample;
	int selectedFlag = 0;
    for (int i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
             
			if (lvi.state & (LVIS_SELECTED )) {
				//inListSample->SetUserRequest(DFSample::PAUSE);
				selectedFlag = 1;
				break;
			}				
    }
	CString cookieId;
    int priority;
    if (selectedFlag) {
		cookieId = inListSample->GetCookieId();
		int priority = inListSample->GetPriority();
		strPriority.Format("%d", priority);

	    priorityDialog.m_priority = strPriority;
		priorityDialog.m_trackingNumber = cookieId;
    }
    int rc = priorityDialog.DoModal();
	
    if (rc == IDOK)
    {
		strPriority = priorityDialog.m_priority;
		if (!strPriority.IsEmpty()) {
			strPriority.TrimLeft();
			strPriority.TrimRight();
			char prior[256]; 
			strcpy(prior, (LPTSTR) (LPCTSTR) strPriority);
			priority = atoi(prior); 
			
        }
        cookieId = priorityDialog.m_trackingNumber;
		if (!cookieId.IsEmpty()) {
			cookieId.TrimLeft();
			cookieId.TrimRight();
			
        }
        // find the sample in the collection 
        POSITION pos;
		DFSample *pSample =  Manager.FindObjectWithIdInHPList(cookieId, pos);
		if (pSample != NULL) {
				//pSample->SetPriority(priority);
			      Manager.ChangeCookiePriority(pSample);
        }
	    
        
    }

	return;
	
}

void CDFGuiView::OnUpdateChangepriority(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	  pCmdUI->Enable (FALSE);

/*       int numberOfSamples = Manager.sampleHPList.GetCount();
	   if (numberOfSamples != 0)  
	 	  pCmdUI->Enable (TRUE);
		else 
		  pCmdUI->Enable (FALSE);
*/	
}

void CDFGuiView::OnDefer() 
{
	// TODO: Add your command handler code here
	killFlag = 20;
//	(void) SetUserRequest(DFSample::DEFER);
//	UpdateSampleList();
	
}

void CDFGuiView::OnUpdateDefer(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if(Manager.closeWndRequest == 1) {
		  pCmdUI->Enable (FALSE);
          return;	
    }

	if (request == ENABLE) {
		if ( AreAllSelectedNotInDefer())
			  pCmdUI->Enable (TRUE);
		else 
			  pCmdUI->Enable (FALSE);
	}
	else 
        pCmdUI->Enable (FALSE);
	
}
BOOL CDFGuiView::AreAllSelectedNotOnHold() 
{
    CListCtrl& m_ctlListCtrl=GetListCtrl();
	DFSample *inListSample;
	int i;
	int nCount = m_ctlListCtrl.GetItemCount();
	if (nCount == 0)
		return FALSE;

    int flag = 0, flagSelected = 0; 
    for (i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
             
			if (lvi.state & (LVIS_SELECTED )) {
				flagSelected = 1;
				if (inListSample->GetUserRequest() == DFSample::PAUSE || inListSample->GetUserRequest() == DFSample::STOP )
					flag = 1;
            }
			
    }
	if (flagSelected == 0)
		return FALSE;

	if (flag == 0 && flagSelected == 1)  
		return TRUE;

    return FALSE; 

}
BOOL CDFGuiView::AreAllSelectedOnHold() 
{
    CListCtrl& m_ctlListCtrl=GetListCtrl();
	DFSample  *inListSample;
	int i;
	int nCount = m_ctlListCtrl.GetItemCount();
	if (nCount == 0)
		return FALSE;
    int flag = 0, flagSelected = 0; 
    for (i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
             
			if (lvi.state & (LVIS_SELECTED )) {
				    flagSelected = 1;
				if (inListSample->GetUserRequest() != DFSample::PAUSE && inListSample->GetUserRequest() != DFSample::STOP)
					flag = 1;
            }
			
    }
	if (flagSelected == 0)
		return FALSE;

	if (flag == 0 && flagSelected == 1) 
	 return TRUE;
    else
     return FALSE;
}
BOOL CDFGuiView::AreAllSelectedNotInDefer() 
{
    CListCtrl& m_ctlListCtrl=GetListCtrl();
	DFSample *inListSample;
	int i;
	int nCount = m_ctlListCtrl.GetItemCount();
	if (nCount == 0)
		return FALSE;

    int flag = 0, flagSelected = 0; 
    for (i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
             
			if (lvi.state & (LVIS_SELECTED )) {
				flagSelected = 1;
				if ( inListSample->GetUserRequest() == DFSample::DEFER )
					flag = 1;
            }
	}
	if (flagSelected == 0)
		return FALSE;

	if (flag == 0 && flagSelected == 1)  
		return TRUE;

    return FALSE; 
}
int CDFGuiView::CorrectArrivalInterval(ArrivalType type, TimerType timerType)
{
  	if (type == Init) {
// test log
//		char logPath[MAX_PATH];
//      strcpy(logPath, Manager.UNCPath);
//		if (strcmp(Manager.UNCPath, "") != 0) {
//		  Manager.AppendPath(logPath, "IntervalTest.log");
//		  testFile = new std::ofstream;
//		  testFile->open(logPath, std::ios::out | std::ios::app);
//		  testFile->flush();
//        }
//
		ArrivalInterval = Manager.sampleArrivalCheckInterval;
		DatabaseRescanInterval = Manager.databaseRescanInterval; 
		enteredArrivalInterval = ArrivalInterval;
		enteredDatabaseRescanInterval = DatabaseRescanInterval; 
		return 0;
    }
   	if (type == Back) {
		if (timerType == Imported) {
			if (ArrivalInterval != enteredArrivalInterval) {
				ArrivalInterval = enteredArrivalInterval;
				KillTimer(TIMER_ARRIVAL_ID);
				(void) SetTimer(TIMER_ARRIVAL_ID, ArrivalInterval * 1000, NULL);					
//test log
//		CTime t = CTime::GetCurrentTime();
//		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
//
//		*(testFile) << (LPTSTR) (LPCTSTR) time << "Back, Imported:" <<  ArrivalInterval <<"\n";
//			testFile->flush();
//

            }
		}	
		else  {
			if (timerType == Rescan) {
				if (DatabaseRescanInterval != enteredDatabaseRescanInterval) {
					DatabaseRescanInterval = enteredDatabaseRescanInterval;
					KillTimer(TIMER_DATABASE_RESCAN_ID);
					(void) SetTimer(TIMER_DATABASE_RESCAN_ID, DatabaseRescanInterval * 1000, NULL);					
//test
//		CTime t = CTime::GetCurrentTime();
//		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
//
//		*(testFile) << (LPTSTR) (LPCTSTR) time <<"Back, Rescan:" <<  DatabaseRescanInterval <<"\n";
//			testFile->flush();
//

				}
			}	
        }
		return 0;
    }
   	if (type == ToMax) {
		if (timerType == Imported) {
			if (ArrivalInterval == enteredArrivalInterval) {
				ArrivalInterval = DEFAULT_SAMPLE_ARRIVAL_INTERVAL_MAX; 
				KillTimer(TIMER_ARRIVAL_ID);
				(void) SetTimer(TIMER_ARRIVAL_ID, ArrivalInterval * 1000, NULL);					
//test log
//		CTime t = CTime::GetCurrentTime();
//		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
//
//		*(testFile) << (LPTSTR) (LPCTSTR) time << "ToMax, Imported:" <<  ArrivalInterval <<"\n";
//			testFile->flush();
//

            } 
        }
		else  {
			if (timerType == Rescan) {
					if (DatabaseRescanInterval == enteredDatabaseRescanInterval) {
						DatabaseRescanInterval = DEFAULT_DATABASE_RESCAN_INTERVAL_MAX; 
						KillTimer(TIMER_DATABASE_RESCAN_ID);
						(void) SetTimer(TIMER_DATABASE_RESCAN_ID, DatabaseRescanInterval * 1000, NULL);					
//test
//		CTime t = CTime::GetCurrentTime();
//		CString time = t.Format( "%d %B %Y,  %H:%M:%S" );
//
//
//		*(testFile) << (LPTSTR) (LPCTSTR) time << "ToMax, Rescan:" <<  DatabaseRescanInterval <<"\n";
//		testFile->flush();
//

					}
            }
        }
		return 0;
    }
	return 0;
}

void CDFGuiView::OnStopimport() 
{
	// TODO: Add your command handler code here
    CString message((LPCSTR) (IDS_STOP_IMPORTER)); 
	CString title((LPCSTR) (IDS_AVIS_TITLE)); 
	int rc = MessageBox (message, title, MB_ICONQUESTION | MB_YESNO);
	if (rc == IDYES) {
	    (void) Manager.StopImporter();
	} 


}

void CDFGuiView::OnUpdateStopimport(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (Manager.defImportJob != NULL) {
			  pCmdUI->Enable (TRUE);
	}
	else 
        pCmdUI->Enable (FALSE);
	
	
}

void CDFGuiView::OnStopundeferrer() 
{
	// TODO: Add your command handler code here
    CString message((LPCSTR) (IDS_STOP_UNDEFERRER)); 
	CString title((LPCSTR) (IDS_AVIS_TITLE)); 
	int rc = MessageBox (message, title, MB_ICONQUESTION | MB_YESNO);
	if (rc == IDYES) {
		(void) Manager.StopUndeferrer();
	} 

	
}

void CDFGuiView::OnUpdateStopundeferrer(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (Manager.undeferrerJob != NULL) {
			  pCmdUI->Enable (TRUE);
	}
	else 
        pCmdUI->Enable (FALSE);
	

}

void CDFGuiView::OnStopupdater() 
{
    CString message((LPCSTR) (IDS_STOP_UPDATER1)); 
	CString title((LPCSTR) (IDS_AVIS_TITLE)); 
	int rc = MessageBox (message, title, MB_ICONQUESTION | MB_YESNO);
	if (rc == IDYES) {
		(void) Manager.StopUpdater();
	} 

	
}

void CDFGuiView::OnUpdateStopupdater(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (Manager.statusUpdaterJob != NULL) {
			  pCmdUI->Enable (TRUE);
	}
	else 
        pCmdUI->Enable (FALSE);
	
	
}

void CDFGuiView::OnSampleattributes() 
{

	// TODO: Add your command handler code here
	CListCtrl& m_ctlListCtrl=GetListCtrl();
	
	int nCount = m_ctlListCtrl.GetItemCount();
    DFSample *inListSample;
	CString cookieId;
	char filePath[MAX_LENGTH];
	int selectFlag = 0;
    for (int i = 0; i < nCount; i++) {
			inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
			// a sample is still in the list, but not in the collection 
            if (!inListSample)
				continue;

	        LV_ITEM lvi;

		    lvi.mask = LVIF_STATE;
			lvi.iItem = i;
			lvi.iSubItem = 0;
			lvi.pszText = 0;
			lvi.cchTextMax = 0;
			lvi.stateMask = 0xFFFF;

			m_ctlListCtrl.GetItem(&lvi);
			if (lvi.state & (LVIS_SELECTED )) {
				inListSample = (DFSample *) m_ctlListCtrl.GetItemData(i);
                cookieId = inListSample->GetCookieId();
                selectFlag = 1;
     			break;       
			}
    }
	if (selectFlag == 0) {
		killFlag = 21; 
	    return;
    }
	strcpy(filePath, Manager.UNCPath);
	char cookie[100];
	strcpy(cookie, (LPTSTR) (LPCTSTR) cookieId);
	CString modId(cookieId); 
	while (modId.GetLength() < 8)
	   modId.Insert(0, '0');
	Manager.AppendPath(filePath, (LPTSTR) (LPCTSTR) modId);
	Manager.AppendPath(filePath, "SampleAttributes");
	sampleDirPath = filePath;
	Manager.AppendPath(filePath, "attributes.txt");

	DFSampleAttributes dfSampleAttrDialog;
	if (!dfSampleAttrDialog.SetFileText(filePath, cookieId)) {
		killFlag = 22; 
	    return;
	}	
	int rc = dfSampleAttrDialog.DoModal();

    if (rc == IDOK)
    {

    }
	

	
}

void CDFGuiView::OnUpdateSampleattributes(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (Manager.sampleHPList.GetCount() == 0)
		  pCmdUI->Enable (FALSE);
	else 
	      pCmdUI->Enable (TRUE);
}

void CDFGuiView::OnStopattrcollector() 
{
	// TODO: Add your command handler code here
    CString message((LPCSTR) (IDS_STOP_ATTRIBUTECOLLECTOR)); 
	CString title((LPCSTR) (IDS_AVIS_TITLE)); 
	int rc = MessageBox (message, title, MB_ICONQUESTION | MB_YESNO);
	if (rc == IDYES) {
		(void) Manager.StopAttrCollector();
	} 
	
}

void CDFGuiView::OnUpdateStopattrcollector(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	if (Manager.attributeCollectorJob != NULL) {
			  pCmdUI->Enable (TRUE);
	}
	else 
        pCmdUI->Enable (FALSE);
	
	
}

void CDFGuiView::OnDestroy() 
{

//	Manager.packThread->Kill();
	CListViewEx::OnDestroy();
	
	// TODO: Add your message handler code here
	
}
int  CDFGuiView::OnAfterThread(WPARAM p1, LPARAM p2)
{

	DFSample *pSample = (DFSample*) p1;
	if (pSample == NULL)
		return 0;
	(void) Manager.FinishPackaging(pSample);
	if(Manager.packSample) {
		delete Manager.packSample;
		Manager.packSample = NULL;
	} 
	if (Manager.packThread) {
		delete Manager.packThread;
		Manager.packThread = NULL;
    }
	if (Manager.closeWndRequest) {
		AfxGetApp()->m_pMainWnd->PostMessage(MSG_REQUEST_CLOSE, 0, 0);
	    Manager.closeWndRequest = 0;
    }
	return 0;
}

int  CDFGuiView::OnImmClose(WPARAM p1, LPARAM p2)
{

	Manager.closeWndRequest = 1; 

	KillTimer(TIMER_PROCESS_ID);
	KillTimer(TIMER_DATABASE_RESCAN_ID);
	KillTimer(TIMER_ARRIVAL_ID);
	KillTimer(TIMER_UPDATE_ID);
	KillTimer(TIMER_STATUS_UPDATE_ID);
	KillTimer(TIMER_DEFINITION_IMPORT_ID);
	KillTimer(TIMER_UPDATELIST_ID);
	KillTimer(TIMER_UNDEFERRER_ID); 
	if (request == ENABLE) {
		(void) Manager.SubmitSamplesForProcessing(); 
		if (Manager.serialFlag) {
#ifdef _DEBUG
			(void) Manager.LogActivity(2, "From Gui -stop1   ", CString());
#endif

			if (Manager.bgtrap == 0) {
#ifdef _DEBUG
			(void) Manager.LogActivity(2, "From Gui -stop   2", CString());
#endif

				(void) SetTimer(TIMER_PROCESS_ID, Manager.sampleSubmissionInterval * 1000, NULL);					
				(void) SetTimer(TIMER_ARRIVAL_ID, Manager.sampleArrivalCheckInterval * 1000, NULL);					
				(void) SetTimer(TIMER_DATABASE_RESCAN_ID, Manager.databaseRescanInterval * 1000, NULL);					
				if (Manager.statusUpdateInterval != 0 )
					(void) SetTimer (TIMER_STATUS_UPDATE_ID, Manager.statusUpdateInterval * 60 * 1000, NULL);
				if (Manager.undeferrerInterval != 0 )
					(void) SetTimer (TIMER_UNDEFERRER_ID, Manager.undeferrerInterval * 60 * 1000, NULL);
				if (Manager.defImportInterval != 0 )
					(void) SetTimer(TIMER_DEFINITION_IMPORT_ID, Manager.defImportInterval * 60 * 1000, NULL);					
				if (Manager.attributeInterval != 0 )
						(void) SetTimer(TIMER_SAMPLEATTRIBUTE_ID, Manager.attributeInterval * 60 * 1000, NULL);					
				(void) SetTimer(TIMER_UPDATELIST_ID, 1000, NULL);					
				killFlag = 23;
				return 1; 
			}
		
		}
    } 
	AfxGetApp()->m_pMainWnd->PostMessage(MSG_REQUEST_CLOSE, 0, 0);
	Manager.closeWndRequest = 0; 
    return 0;
}

