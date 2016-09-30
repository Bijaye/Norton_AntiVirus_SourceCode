// ALMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "afxtempl.h"
#include "afxwin.h"
#include "stdio.h"
#include "stdlib.h"
#include "io.h"
#include "fstream.h"
#include "strstrea.h"
#include "iomanip.h"
#include "dfevalstatus.h"
#include "dfevalstate.h"
#include "dfmsg.h" 


#include "ALM.h"
#include "DFEvalStatus.h"
#include "DFEvalState.h"
#include "DFJob.h"
#include "DFSample.h"
#include "dferror.h"  
#include "dfmatrix.h"
#include "listctrlex.h"
#include "dflistctrl.h"
#include "DFManager.h"
#include "ALMDlg.h"
 
#include "DFLauncherImp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//#define LOGFILE "alm.log"

const int TIMER=100; 
 
/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CALMDlg dialog

CALMDlg::CALMDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CALMDlg::IDD, pParent),
	selectedObj(NULL)
{
	//{{AFX_DATA_INIT(CALMDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

}

void CALMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CALMDlg)
	DDX_Control(pDX, IDC_LIST1, m_ctlListCtrl);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CALMDlg, CDialog)
	//{{AFX_MSG_MAP(CALMDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP

	ON_MESSAGE(ON_JOBACCEPT, OnJobAccept)
	ON_MESSAGE(ON_JOBEND, OnJobEnd)
	ON_MESSAGE(ON_JOBPOSTPONE, OnJobPostpone)
	ON_MESSAGE(ON_JOBTIMEOUT, OnJobTimeout)

	ON_MESSAGE(DFLAUNCHER_POST_JOB_STATUS, OnJobFromLauncher)
	

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CALMDlg message handlers

BOOL CALMDlg::OnInitDialog()
{
	TRACE("Entered OnInitDialog\n");

	CDialog::OnInitDialog();
	//set the type of row highlighting
    m_ctlListCtrl.SetHighlightType(HIGHLIGHT_ROW);

    LV_COLUMN lvc;
    lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
     

    lvc.iSubItem=0;
    CString cookie("Sample   "); 
    lvc.pszText=(LPTSTR) ((LPCTSTR)cookie);
    lvc.cx= 4* m_ctlListCtrl.GetStringWidth((LPCTSTR)cookie);
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(0,&lvc);

    lvc.iSubItem=1;
	CString state("State   ");
    lvc.pszText=(LPTSTR) ((LPCTSTR)state);
    lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)state) + 30;
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(1,&lvc);

    lvc.iSubItem=2;
	CString status("Status   ");
    lvc.pszText=(LPTSTR) ((LPCTSTR)status);
    lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)status) + 40;
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(2,&lvc);

	lvc.iSubItem=3;
    CString done("Jobs done");
    lvc.pszText=(LPTSTR) ((LPCTSTR)done);
    lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)done) + 30;
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(3,&lvc);

	lvc.iSubItem=4;
    CString successJobs("Jobs successful");
    lvc.pszText=(LPTSTR) ((LPCTSTR)successJobs);
    lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)successJobs) + 30;
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(4,&lvc);

    lvc.iSubItem=5;
    CString stopcond("Stop Cond");
    lvc.pszText=(LPTSTR) ((LPCTSTR)stopcond);
    lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)stopcond) + 15;
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(5,&lvc);

    lvc.iSubItem=6;
    CString initcond("Init Cond");
    lvc.pszText=(LPTSTR) ((LPCTSTR)initcond);
    lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)initcond) + 15;
    lvc.fmt=LVCFMT_LEFT;
    m_ctlListCtrl.InsertColumn(5,&lvc);




	//lvc.iSubItem=4;
    //CString time("Job Status Time   ");
    //lvc.pszText=(LPTSTR) ((LPCTSTR)time);
    //lvc.cx=m_ctlListCtrl.GetStringWidth((LPCTSTR)time) + 40;
    //lvc.fmt=LVCFMT_LEFT;
    //m_ctlListCtrl.InsertColumn(4,&lvc);

    
//    int rc = Manager.LoadSampleCollection();
/*     
	POSITION pos;
	int nCount;
    int itemVisible = -1;
	nCount = (WORD)Manager.sampleList.GetCount();
    
	LV_ITEM lvi;
	int i = 0;
	pos = Manager.sampleList.GetHeadPosition();
         
	while (pos != NULL)
	{
		DFSample* pSample = Manager.sampleList.GetNext(pos);
		CString cookieId = pSample->GetCookieId();
		CString strCurrentState = pSample->GetCurrentStateStr();
		CString currentStatus = pSample->GetCurrentStatus();

        int initcond = pSample->GetInitCond();
		char initCond[20];
		_itoa(initcond, initCond, 20);

		int stopcond = pSample->GetStopCond();
		char stopCond[20];
		_itoa(stopcond, stopCond, 20);

		int donejobs = pSample->GetDoneJobs();
		char doneJobs[20];
		_itoa(donejobs, doneJobs, 20);

		int successjobs= pSample->GetSuccessJobs();
		char successJobs[20];
		_itoa(successjobs, successJobs, 20);


		lvi.mask=LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
	   	lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
				
		if(!i) {
           lvi.state =LVIS_SELECTED | LVIS_FOCUSED ;
		   itemVisible = i;
            }  
		else 
           lvi.state = 0;
			   

      
		lvi.iItem=i;
		lvi.iSubItem=0;
		lvi.pszText= (LPTSTR)((LPCTSTR)cookieId);
		//UINT image = pScanObject->GetImageNumber();
		//lvi.iImage = image;


        
		int index =  m_ctlListCtrl.InsertItem(&lvi);
		m_ctlListCtrl.SetItemText(i,1, (strCurrentState) );
		m_ctlListCtrl.SetItemText(i,2, (currentStatus) );
		m_ctlListCtrl.SetItemText(i,3, (doneJobs) );
		m_ctlListCtrl.SetItemText(i,4, (successJobs) );
		m_ctlListCtrl.SetItemText(i,5, (initCond) );
		m_ctlListCtrl.SetItemText(i,6, (stopCond) );

		//CString s = submissionTime.Format( "%B, %d %H:%M:%S" );
		
		//m_ctlListCtrl.SetItemText(i,4,s);
		
        m_ctlListCtrl.SetItemData(index, (DWORD)pSample);  

		nCount--;
		i++;
	}
	m_ctlListCtrl.EnsureVisible(itemVisible, 0);

*/

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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
// Initialize the paths
    if (Manager.InitDirPaths() != 0)
		return(-1);
#ifdef LAUNCHER
// Launcher 
	 DFLauncherWnd = InitDFLauncher(m_hWnd);
     Manager.SetLauncherHandle(DFLauncherWnd);

	 CWnd * pDFLWnd = CWnd::FromHandle(DFLauncherWnd);
     pDFLWnd->ShowWindow(SW_RESTORE);
	 pDFLWnd->SetForegroundWindow();
#endif
      SetTimer(TIMER, TIMER_INTERVAL, NULL);					
// This is for testing
// 1.
//     OnManualTimer();					

		return TRUE;  // return TRUE  unless you set the focus to a control
}

void CALMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CALMDlg::OnPaint() 
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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CALMDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CALMDlg::OnTimer(UINT Id)
{  

   int rc;	
   int sampleCount = Manager.sampleList.GetCount();
   int sampleHPCount = Manager.sampleHPList.GetCount();

   if (sampleCount >= MAX_SAMPLES && sampleHPCount >=MAX_SAMPLES )
	    return;
   int updateFlag = 0;
//   if (Manager.GetNewCookiesFromQueues()) { 
   if (Manager.GetNewSamples()) { 
       UpdateSampleList();
       //Manager.MirrorSampleCollection();
   } 
   sampleHPCount = Manager.sampleHPList.GetCount();

   if (sampleHPCount != 0) {
     rc = Manager.SubmitSamplesForProcessing(DFSample::HIGH); 
	 if (rc == 2)
		 selectedObj = NULL;
	 if (rc != 0)
		UpdateSampleList();
	 
//       Manager.MirrorSampleCollection();
   }
   else {
		  sampleCount = Manager.sampleList.GetCount();
          if (sampleCount != 0) {
    	     rc = Manager.SubmitSamplesForProcessing(DFSample::LOW);				 ;
			 if ( rc == 2)
				 selectedObj = NULL;
			 if (rc != 0)
				UpdateSampleList();
          }
	 
   }

//old code

/*
   int sampleCount = Manager.sampleList.GetCount();
   int sampleHPCount = Manager.sampleHPList.GetCount();

   if (sampleCount >= MAX_SAMPLES && sampleHPCount >=MAX_SAMPLES )
	    return;
   int updateFlag = 0;
//   if (Manager.GetNewCookiesFromQueues()) { 
   if (Manager.GetNewSamples()) { 
       UpdateSampleList();
       //Manager.MirrorSampleCollection();
   } 
   sampleHPCount = Manager.sampleHPList.GetCount();

   if (sampleHPCount != 0) {
     if (Manager.SubmitSamplesForProcessing(DFSample::HIGH)) {
//       Manager.MirrorSampleCollection();
	 }
	 else {
		  sampleCount = Manager.sampleList.GetCount();
          if (sampleCount != 0) {
    	     if (Manager.SubmitSamplesForProcessing(DFSample::LOW)) {
			 }
          }
	 }
   }
   //if (updateFlag == 1)
   //	   UpdateSampleList();
//  

 //  Beep(200, 200);

 */  


}
void CALMDlg::OnManualTimer()
{  
// check if the input queue has cookies     
   int rc;	
   int sampleCount = Manager.sampleList.GetCount();
   int sampleHPCount = Manager.sampleHPList.GetCount();

   if (sampleCount >= MAX_SAMPLES && sampleHPCount >=MAX_SAMPLES )
	    return;
   int updateFlag = 0;
//   if (Manager.GetNewCookiesFromQueues()) { 
   if (Manager.GetNewSamples()) { 
       UpdateSampleList();
       //Manager.MirrorSampleCollection();
   } 
   sampleHPCount = Manager.sampleHPList.GetCount();

   if (sampleHPCount != 0) {
     rc = Manager.SubmitSamplesForProcessing(DFSample::HIGH); 
	 if (rc == 2)
		 selectedObj = NULL;
     UpdateSampleList();
	 
//       Manager.MirrorSampleCollection();
   }
   else {
		  sampleCount = Manager.sampleList.GetCount();
          if (sampleCount != 0) {
    	     rc = Manager.SubmitSamplesForProcessing(DFSample::LOW);				 ;
			 if ( rc == 2)
				 selectedObj = NULL;
			 UpdateSampleList();
          }
	 
   }


}


/*----------------------------------------------------------------------------*/
/* Procedure name:      Update the list                                       */
/* Description:         To update the list                                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int CALMDlg::UpdateSampleList()
{
	int initcond;
	char initCond[20];
	int stopcond; 
	char stopCond[20];
	int donejobs; 
	char doneJobs[20];
	int successjobs;
	char successJobs[20];

    LV_ITEM lvi;  
   	
	m_ctlListCtrl.DeleteAllItems();

	POSITION pos = Manager.sampleHPList.GetHeadPosition();
	int i = 0;
	int itemVisible = -1;
	int nCount = (WORD)Manager.sampleHPList.GetCount();
	
	while (pos != NULL)
	{
		DFSample* pSample = Manager.sampleHPList.GetNext(pos);
		CString cookieId = pSample->GetCookieId();
		CString strCurrentState = pSample->GetCurrentStateStr();
		DFSample::SampleStatus currentStatus = pSample->GetCurrentStatus();
		CString status = pSample->GetCurrentStatusStr(currentStatus); 

        initcond = pSample->GetInitCond();
		_itoa(initcond, initCond, 20);

		stopcond = pSample->GetStopCond();
		_itoa(stopcond, stopCond, 20);

		donejobs = pSample->GetDoneJobs();
		_itoa(donejobs, doneJobs, 20);

		successjobs= pSample->GetSuccessJobs();
		_itoa(successjobs, successJobs, 20);

    	lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
		lvi.mask=LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
		if (selectedObj != NULL) {
		   if (pSample == selectedObj) {
         	   	lvi.state =LVIS_SELECTED | LVIS_FOCUSED ;
    	    	itemVisible = i;  
		   }
           else
	        	lvi.state = 0;
        }
		else  { 
			if(!i) {
               lvi.state =LVIS_SELECTED | LVIS_FOCUSED ;
			   itemVisible = i;
            }  
			else 
               lvi.state = 0;
			   
    	    selectedObj = pSample;	
        } 



		lvi.iItem=i;
		lvi.iSubItem=0;
		lvi.pszText= (LPTSTR)((LPCTSTR)cookieId);
	

        
		int index =  m_ctlListCtrl.InsertItem(&lvi);
		m_ctlListCtrl.SetItemText(i,1, (strCurrentState) );
		m_ctlListCtrl.SetItemText(i,2, (status) );
		m_ctlListCtrl.SetItemText(i,3, (doneJobs) );
		m_ctlListCtrl.SetItemText(i,4, (successJobs) );
		m_ctlListCtrl.SetItemText(i,5, (initCond) );
		m_ctlListCtrl.SetItemText(i,6, (stopCond) );


//		CString s = submissionTime.Format( "%B, %d %H:%M:%S" );
//		m_ctlListCtrl.SetItemText(i,4,s);
		
        m_ctlListCtrl.SetItemData(index, (DWORD)pSample);  

		nCount--;
		i++;
	}

	pos = Manager.sampleList.GetHeadPosition();
	//int i = 0;
	//int itemVisible = -1;
	
	nCount = (WORD)Manager.sampleList.GetCount();
	
	while (pos != NULL)
	{
		DFSample* pSample = Manager.sampleList.GetNext(pos);
		CString cookieId = pSample->GetCookieId();
		CString strCurrentState = pSample->GetCurrentStateStr();
		DFSample::SampleStatus currentStatus = pSample->GetCurrentStatus();
		CString status = pSample->GetCurrentStatusStr(currentStatus); 

        initcond = pSample->GetInitCond();
		_itoa(initcond, initCond, 20);

		stopcond = pSample->GetStopCond();
		_itoa(stopcond, stopCond, 20);

		donejobs = pSample->GetDoneJobs();
		_itoa(donejobs, doneJobs, 20);

		successjobs= pSample->GetSuccessJobs();
		_itoa(successjobs, successJobs, 20);



		//CTime submissionTime = pSample->GetSubmissionTime();
		
		lvi.mask=LVIF_TEXT | LVIF_STATE | LVIF_IMAGE;
	   	lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
				
		if(!i) {
           lvi.state =LVIS_SELECTED | LVIS_FOCUSED ;
		   itemVisible = i;
            }  
		else 
           lvi.state = 0;
			   

      
		lvi.iItem=i;
		lvi.iSubItem=0;
		lvi.pszText= (LPTSTR)((LPCTSTR)cookieId);
	

        
		int index =  m_ctlListCtrl.InsertItem(&lvi);
		m_ctlListCtrl.SetItemText(i,1, (strCurrentState) );
		m_ctlListCtrl.SetItemText(i,2, (status) );
		m_ctlListCtrl.SetItemText(i,3, (doneJobs) );
		m_ctlListCtrl.SetItemText(i,4, (successJobs) );
		m_ctlListCtrl.SetItemText(i,5, (initCond) );
		m_ctlListCtrl.SetItemText(i,6, (stopCond) );


//		CString s = submissionTime.Format( "%B, %d %H:%M:%S" );
//		m_ctlListCtrl.SetItemText(i,4,s);
		
        m_ctlListCtrl.SetItemData(index, (DWORD)pSample);  

		nCount--;
		i++;
	}

	m_ctlListCtrl.EnsureVisible(itemVisible, 0);
    return 0;
}    
/*----------------------------------------------------------------------------*/
/* Procedure name:      SubmitJob                                             */
/* Description:         To submit a job to the Job Locator                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::SubmitJob(CString cookieId, int state)
{
	return 0;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      IgnoreJob                                             */
/* Description:         To ignore a job                                       */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::IgnoreJob(CString cookieId, int state)
{
	return 0;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      Request the Job Status                               */
/* Description:         To update the list                                    */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::RequestJobStatus(CString cookieId , int state)
{
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      FindSampleInList                                      */
/* Description:         To find sample in the list                            */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int CALMDlg::FindSampleInList(DFSample *sample)
{

    for (int i = 0; i < m_ctlListCtrl.GetItemCount(); i++)
    {
    
        
		DFSample *pSample = (DFSample*) m_ctlListCtrl.GetItemData(i);
	    if (pSample == sample)
				return i;
	}
	return (-1);
}

int CALMDlg::GetCurSel(void)
{
    //CListCtrl& ListCtrl=GetListCtrl();
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
			//SelectedItemIndex = (int) m_ctlListCtrl.GetItemData(i);
			break;
		}
    }
	return i;
}

DFSample *CALMDlg::FindSampleObject(int& nSel, POSITION& pos)
{
    if	(m_ctlListCtrl.GetItemCount() == 0)
		return NULL;

	nSel = GetCurSel();
	if (nSel == LB_ERR)
	{
	//	AfxMessageBox(IDS_SELECT_ENTRY);
		return NULL;
	}
    //CWnd *parent = GetParent();
    //DFSampleList  &sampleList = parent->sampleList;
	
	// The AvScanObject pointer was saved as the list control's data item.
	DFSample* pSample = (DFSample*) m_ctlListCtrl.GetItemData(nSel);
//SNK --- add all lists HERE!!!                                           
	// Find the AvScanObject pointer in the CTypedPtrList.
	pos = Manager.sampleHPList.Find(pSample);
	if (pos == NULL)
		pos = Manager.sampleList.Find(pSample);

	ASSERT(pos != NULL);

	return pSample;
}

BOOL CALMDlg::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
    int nID = (int) LOWORD(wParam);
	
	switch (nID)
	{
    
    case ID_ACCEPT:
		JobAccept();
		break;
    case ON_JOBACCEPT:
    	OnJobAccept(wParam, lParam);
		break;
    case ID_END:
		JobEnd();
		break;
    case ON_JOBEND:
    	OnJobEnd(wParam, lParam);
		break;
	case ID_POSTPONE:
		JobPostpone();
		break;
    case ON_JOBPOSTPONE:
    	OnJobPostpone(wParam, lParam);
		break;
	case ID_TIMEOUT:
		JobTimeout();
		break;
    case ON_JOBTIMEOUT:
    	OnJobTimeout(wParam, lParam);
		break;
    	
		
	}


	
	return CDialog::OnCommand(wParam, lParam);
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      JobAccept                                             */
/* Description:         To accept a job (test program)                        */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:  The method is uded for testing                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::JobAccept()
{
    int nSel;
	POSITION pos;
	DFSample *pSample = FindSampleObject(nSel, pos);
	if (pSample == NULL)
		return 0;
	selectedObj = pSample;
	pos = pSample->jobList.GetHeadPosition();
	
	DFJob* pJob;

	while (pos != NULL)
	{
    	if (pSample->stopSatisfied) {
		   pSample->stopSatisfied = 0;
		   break;
		} 

	  pJob = pSample->jobList.GetNext(pos);
      DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), CString());

      SendMessage(ON_JOBACCEPT,(WPARAM)transitJob, 0);


	}
	    
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnJobAccept                                           */
/* Description:         On job acceptence                                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::OnJobAccept(WPARAM p1, LPARAM p2	)
{

    //find a sample in a collection      
	DFJob *pJob = (DFJob*) p1;

	CString cookieId = pJob->GetCookieId();
	POSITION pos;
    DFSample *pSample = Manager.FindObjectWithIdInHPList(cookieId, pos);
	if (pSample == NULL)
		pSample = Manager.FindObjectWithIdInList(cookieId, pos);
    if (pSample == NULL) {
		delete pJob;
		return(-1);
    }
    int seq = pJob->GetJobSeq();
    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL) {
		delete pJob;
	    return( -1);  
    } 
//	DFJob::JobStatus status = pJob->GetJobStatus();
	if (jobObj->GetStatus() != DFJob::SUBMITTED) {
		delete pJob;
	    return (-1); 
    }
    jobObj->SetStatus(DFJob::ACCEPTED);
	// delete the transit object.
	delete pJob;
	(void) Manager.DetermineSampleStatus(pSample);
	UpdateSampleList();
#ifndef LAUNCHER
	OnManualTimer();
#endif
    return 0;
}



/*----------------------------------------------------------------------------*/
/* Procedure name:      End                                                   */
/* Description:         To end a job (test program)                           */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes: The method is used for testing                                      */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::JobEnd()
{

    int nSel;
	POSITION pos;
	DFSample *pSample = FindSampleObject(nSel, pos);
	if (pSample == NULL)
		return 0;

	selectedObj = pSample;

	pos = pSample->jobList.GetHeadPosition();
	
	DFJob* pJob;
	while (pos != NULL)
	{
		if (pSample->stopSatisfied) {
		  pSample->stopSatisfied = 0;
		  break;
        } 
		pJob = pSample->jobList.GetNext(pos);
		DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), CString());

      SendMessage(ON_JOBEND,(WPARAM)transitJob, 0);


	}

	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnJobAccept                                           */
/* Description:         On job acceptence                                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::OnJobEnd(WPARAM p1, LPARAM p2	)
{
    //find a sample in a collection      
	// I have to read result files or to contact the Database at this point to know what is the status of 
	// the ended job! 
	// Let's put status "Success" for testing
	//pJob->SetStatus(status);	
	POSITION pos;
	DFJob *pJob = (DFJob*) p1;

	CString cookieId = pJob->GetCookieId();
    DFSample *pSample = Manager.FindObjectWithIdInHPList(cookieId, pos);
	if (pSample == NULL)
		pSample = Manager.FindObjectWithIdInList(cookieId, pos);
    if (pSample == NULL) { 
		delete pJob;
		return(-1);
    }
    int seq = pJob->GetJobSeq();
//	if (nextState != 0 )
//	    pSample->SetNextStateStr(nextState);
    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL ) {
         delete pJob;    //deletion of a flying job
	     return (-1);
    }

	CString nextStep; 
	
	int seqId = pJob->GetJobSeq(); 
	cookieId = pJob->GetCookieId();
	// Read the result of the completed job 
	int rc = Manager.LoadSampleData(pJob); 
	if (rc == -1) {
		delete pJob;
		return rc;
	}
    // Read stoping condition after replication driver completed its work  
	if (pJob->GetJobId() == Manager.Matrix.GetReplDriverName()) {
        int rc = Manager.ReadStopCondition(pJob);	
        if (rc == -1) {
	    	delete pJob;
	    	return rc;
		}
	}
	
	(void) Manager.DetermineSampleStatus(pSample);
		 UpdateSampleList();
		
    
	delete pJob;

#ifndef LAUNCHER
	OnManualTimer();
#endif

	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      JobPostpone                                           */
/* Description:         To postpone a job (test program)                      */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:  The method is uded for testing                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::JobPostpone()
{
    int nSel;
	POSITION pos;
	DFSample *pSample = FindSampleObject(nSel, pos);
	if (pSample == NULL)
		return 0;

	selectedObj = pSample;
	pos = pSample->jobList.GetHeadPosition();
	
	DFJob* pJob;

	while (pos != NULL)
	{
    	if (pSample->stopSatisfied) {
		   pSample->stopSatisfied = 0;
		   break;
		} 

	  pJob = pSample->jobList.GetNext(pos);
      DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), CString());

      SendMessage(ON_JOBPOSTPONE,(WPARAM)transitJob, 0);


	}
	    
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnJobPostpone                                         */
/* Description:         On job postponed                                      */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::OnJobPostpone(WPARAM p1, LPARAM p2	)
{

    //find a sample in a collection      
	DFJob *pJob = (DFJob*) p1;

	CString cookieId = pJob->GetCookieId();
	POSITION pos;
    DFSample *pSample = Manager.FindObjectWithIdInHPList(cookieId, pos);
	if (pSample == NULL)
		pSample = Manager.FindObjectWithIdInList(cookieId, pos);
    if (pSample == NULL) {
		delete pJob;
		return(-1);
    }
    int seq = pJob->GetJobSeq();
    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL) {
		delete pJob;
	    return( -1);  
    }
	
	if (jobObj->GetStatus() != DFJob::SUBMITTED) {
	    return (-1); 
    }

    jobObj->SetStatus(DFJob::POSTPONED);
	// delete the transit object.
	delete pJob;
	(void) Manager.DetermineSampleStatus(pSample);
	UpdateSampleList();

#ifndef LAUNCHER
	OnManualTimer();
#endif
    return 0;
}

/*----------------------------------------------------------------------------*/
/* Procedure name:      JobTimeout                                            */
/* Description:         To timeout a job (test program)                       */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:  The method is uded for testing                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::JobTimeout()
{
    int nSel;
	POSITION pos;
	DFSample *pSample = FindSampleObject(nSel, pos);
	if (pSample == NULL)
		return 0;

	selectedObj = pSample;
	pos = pSample->jobList.GetHeadPosition();
	
	DFJob* pJob;

	while (pos != NULL)
	{
    	if (pSample->stopSatisfied) {
		   pSample->stopSatisfied = 0;
		   break;
		} 

	  pJob = pSample->jobList.GetNext(pos);
      DFJob *transitJob = new DFJob(pJob->GetJobSeq(), pJob->GetJobId(), pSample->GetCookieId(), CString());

      SendMessage(ON_JOBTIMEOUT,(WPARAM)transitJob, 0);


	}
	    
	return 0;
}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnJobTimeout                                          */
/* Description:         On job timeout                                        */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::OnJobTimeout(WPARAM p1, LPARAM p2	)
{

    //find a sample in a collection      
	DFJob *pJob = (DFJob*) p1;

	CString cookieId = pJob->GetCookieId();
	POSITION pos;
    DFSample *pSample = Manager.FindObjectWithIdInHPList(cookieId, pos);
	if (pSample == NULL)
		pSample = Manager.FindObjectWithIdInList(cookieId, pos);
    if (pSample == NULL) {
		delete pJob;
		return(-1);
    }
    int seq = pJob->GetJobSeq();
    DFJob *jobObj = pSample->FindJobWithSeq(seq, pos);         
	if ( jobObj == NULL) {
		delete pJob;
	    return( -1);  
    } 
//	DFJob::JobStatus status = pJob->GetJobStatus();
    jobObj->SetStatus(DFJob::TIMEDOUT);
	// delete the transit object.
	delete pJob;
	(void) Manager.DetermineSampleStatus(pSample);
	UpdateSampleList();

#ifndef LAUNCHER
	OnManualTimer();
#endif
    return 0;
}


void CALMDlg::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
#ifdef LAUNCHER
//Launcher
    TermDFLauncher();	
#endif
	CDialog::OnClose();
}

BOOL CALMDlg::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
    BOOL rc;
	rc =  CDialog::PreTranslateMessage(pMsg);
	if ( !rc )  {
#ifdef LAUNCHER
// Launcher
	if (DFLauncherWnd)
             rc = FilterDFLauncherMessage(pMsg);
#endif
	
    }

    return rc;
	
//	return CDialog::PreTranslateMessage(pMsg);

}
/*----------------------------------------------------------------------------*/
/* Procedure name:      OnJobAccept                                           */
/* Description:         On job acceptence                                     */
/*                                                                            */
/* Input:                                                                     */
/*                                                                            */
/* Output:              None                                                  */
/* Return:                                                                    */
/* Global References:   None                                                  */
/* Notes:                                                                     */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/* History:                                                                   */
/*----------------------------------------------------------------------------*/
int  CALMDlg::OnJobFromLauncher(WPARAM p1, LPARAM p2)
{

	DFJob *pJob = (DFJob*) p2;
	if (pJob->GetStatus() == DFJob::COMPLETED) {
            Manager.ProcessCompletedJob(pJob);        
    }
	if (pJob->GetStatus() == DFJob::ACCEPTED) {
            Manager.ProcessAcceptedJob(pJob);        
    }
	if (pJob->GetStatus() == DFJob::TIMEDOUT) {
            Manager.ProcessTimedoutJob(pJob);        
    }
    if (pJob->GetStatus() == DFJob::POSTPONED) {
		   Manager.ProcessPostponedJob(pJob);        
    }
    UpdateSampleList();
	// testing
//	OnManualTimer();
	return 0;

}
    
