// AVISDFRLView.cpp : implementation of the CDFLauncherView class
//

#include "stdafx.h"
#include <errno.h>
#include "AVISDFRL.h"

#include "AVISDFRLView.h"
#include "ResourceDialog.h"
#include "MachineStatistics.h"
#include "MainFrm.h"

#include "DFresource.h"
#include "DFJob.h"
#include "DFMsg.h"
#include "ParamValue.h"
#include "ActivityLogSetting.h"
#include "AutoDisableErrorCount.h"
#include "CookieStatistics.h"
#include "DFDefaults.h"

#pragma warning (disable : 4100)

#include <winsock.h>
#include "AVISSendMail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDFLauncherView

IMPLEMENT_DYNCREATE(CDFLauncherView, CListViewEx)

#define CListView CListViewEx
BEGIN_MESSAGE_MAP(CDFLauncherView, CListView)
#undef CListView
	//{{AFX_MSG_MAP(CDFLauncherView)
	ON_WM_TIMER()
	ON_COMMAND(ID_ACTION_SHOWALMWINDOW, OnActionShowalmwindow)
	ON_COMMAND(ID_ACTIONS_ADD, OnActionsAdd)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ADD, OnUpdateActionsAdd)
	ON_COMMAND(ID_ACTIONS_COPY, OnActionsCopy)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_COPY, OnUpdateActionsCopy)
	ON_COMMAND(ID_ACTIONS_DISABLE, OnActionsDisable)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_DISABLE, OnUpdateActionsDisable)
	ON_COMMAND(ID_ACTIONS_ENABLE, OnActionsEnable)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ENABLE, OnUpdateActionsEnable)
	ON_COMMAND(ID_ACTIONS_MODIFY, OnActionsModify)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_MODIFY, OnUpdateActionsModify)
	ON_COMMAND(ID_ACTIONS_REMOVE, OnActionsRemove)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_REMOVE, OnUpdateActionsRemove)
	ON_COMMAND(ID_ACTIONS_REMOVEALL, OnActionsRemoveall)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_REMOVEALL, OnUpdateActionsRemoveall)
	ON_COMMAND(ID_RESOURCES_SAVE, OnResourcesSave)
	ON_COMMAND(ID_RESOURCES_LOAD, OnResourcesLoad)
	ON_UPDATE_COMMAND_UI(ID_RESOURCES_LOAD, OnUpdateResourcesLoad)
	ON_UPDATE_COMMAND_UI(ID_RESOURCES_SAVE, OnUpdateResourcesSave)
	ON_COMMAND(ID_VIEW_SUMMARY, OnViewSummary)
	ON_COMMAND(ID_VIEW_MACHINESTATISTICS, OnViewMachinestatistics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MACHINESTATISTICS, OnUpdateViewMachinestatistics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SUMMARY, OnUpdateViewSummary)
	ON_COMMAND(ID_ACTIONS_DISABLEALL, OnActionsDisableall)
	ON_COMMAND(ID_ACTIONS_ENABLEALL, OnActionsEnableall)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_DISABLEALL, OnUpdateActionsDisableall)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ENABLEALL, OnUpdateActionsEnableall)
	ON_COMMAND(ID_LOG_FLUSH, OnLogFlush)
	ON_UPDATE_COMMAND_UI(ID_LOG_FLUSH, OnUpdateLogFlush)
	ON_COMMAND(ID_LOG_SETTINGS, OnLogSettings)
	ON_COMMAND(ID_SETUP_SINGLEJOBPERMACHINE, OnSetupSinglejobpermachine)
	ON_UPDATE_COMMAND_UI(ID_SETUP_SINGLEJOBPERMACHINE, OnUpdateSetupSinglejobpermachine)
	ON_COMMAND(ID_ACTIONS_DISABLEMACHINE, OnActionsDisablemachine)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_DISABLEMACHINE, OnUpdateActionsDisablemachine)
	ON_COMMAND(ID_ACTIONS_ENABLEMACHINE, OnActionsEnablemachine)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ENABLEMACHINE, OnUpdateActionsEnablemachine)
	ON_COMMAND(ID_SETUP_ERRORCOUNTFORAUTODISABLE, OnSetupErrorcountforautodisable)
	ON_COMMAND(ID_ACTIONS_ENABLEJOB, OnActionsEnablejob)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ENABLEJOB, OnUpdateActionsEnablejob)
	ON_COMMAND(ID_ACTIONS_DISABLEJOB, OnActionsDisablejob)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_DISABLEJOB, OnUpdateActionsDisablejob)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_SETUP_DISPLAYRESOURCEPROGRESSWINDOW, OnSetupDisplayresourceprogresswindow)
	ON_UPDATE_COMMAND_UI(ID_SETUP_DISPLAYRESOURCEPROGRESSWINDOW, OnUpdateSetupDisplayresourceprogresswindow)
	ON_COMMAND(ID_VIEW_COOKIE_STATISTICS, OnViewCookieStatistics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COOKIE_STATISTICS, OnUpdateViewCookieStatistics)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListViewEx::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListViewEx::OnFilePrint)
    ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked) 
    ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDFLauncherView construction/destruction

//CString g_SampleDirUNCPath;
CString g_ModulePath;
BOOL g_ShowAppWindow;
int WritingStatsToFile = 0;

CDFLauncherView::CDFLauncherView() :
    m_Mutex (FALSE, "ISDFLauncherViewMutex")
{
    m_DFResourceList.SetSize(0,10);
    m_resIndex = 0;
    m_TerminateDLL = FALSE;
    m_ResourceModified = FALSE;
    m_SortedColumn = 0;
    m_SortOrder = SORT_ASCENDING;
    m_pResourceSummaryDlg = NULL;
    m_LogFile = (FILE *) NULL;

    //Configurable Parameters

    m_SingleJobPerMachine = DFR_DEFAULT_SINGLE_JOB_PER_MACHINE;
    m_AutoDisableCount = DFR_DEFAULT_AUTO_DISABLE_COUNT;
    m_MaxLogFileSize = DFR_DEFAULT_MAX_LOG_FILE_SIZE * 1024;
    m_LogBufferSize = DFR_DEFAULT_LOG_BUFFER_SIZE * 1024;
    m_LogFileName = DFR_DEFAULT_LOG_FILE_NAME;
    g_ShowAppWindow = m_ShowAppWindow = DFR_DEFAULT_SHOW_ANALYSIS_APP_WINDOW;

    // Following parameters cannot be configured using the cfg file.
    m_ViewRefreshInterval = DFR_DEFAULT_VIEW_REFRESH_INTERVAL;
    m_StatisticsWriteInterval = DFR_DEFAULT_STATISTICS_WRITE_INTERVAL;

    TCHAR modulePath[512];
    m_ModulePath.Empty();
    if (GetModuleFileName (NULL, modulePath, sizeof(modulePath)))
    {
        CString tmpPath;

        tmpPath = modulePath;
        int pos = tmpPath.ReverseFind('\\');
        if (pos > 0)
        {
            m_ModulePath = tmpPath.Left(pos);
        }
        else
        {
            pos = tmpPath.ReverseFind(':');
            if (pos > 0)
            {
                m_ModulePath = tmpPath.Left(pos+1);
            }
        }

        m_ModulePath += '\\';
    }
    g_ModulePath = m_ModulePath;
    
}

CDFLauncherView::~CDFLauncherView()
{
    int resCount = m_DFResourceList.GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource)
            delete pDFResource;
    }
    m_DFResourceList.RemoveAll();

    if (m_LogFile)
    {
        fflush (m_LogFile);
        fclose (m_LogFile);
    }
}

BOOL CDFLauncherView::PreCreateWindow(CREATESTRUCT& cs)
{
    cs.style |= (LVS_REPORT | LVS_SINGLESEL | LVS_EX_FULLROWSELECT);

	return CListViewEx::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDFLauncherView diagnostics

#ifdef _DEBUG
void CDFLauncherView::AssertValid() const
{
	CListViewEx::AssertValid();
}

void CDFLauncherView::Dump(CDumpContext& dc) const
{
	CListViewEx::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDFLauncherView message handlers
char *g_localComputerName;

void CDFLauncherView::OnInitialUpdate() 
{
	CListViewEx::OnInitialUpdate();
	
    LoadAppSettings();

    DWORD compNameSize;

    compNameSize = sizeof (m_ComputerName);
    if (!GetComputerName (m_ComputerName, &compNameSize))
    {
        m_ComputerName[0] = '\0';
    }
    g_localComputerName = m_ComputerName;

    m_AVISDFStartTime = COleDateTime::GetCurrentTime();

    CListCtrl& ListCtrl=GetListCtrl();

    ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | 
        LVS_EX_ONECLICKACTIVATE | 
        LVS_EX_UNDERLINEHOT | 
        LVS_EX_GRIDLINES);

// set image lists

// insert columns

    LV_COLUMN lvc;
	CString JobString((LPCSTR) IDS_DF_JOB_STRING);
	CString IPAddress((LPCSTR) IDS_DF_IP_ADDRESS_STRING);
    CString NTMachineNameString((LPCTSTR) IDS_DF_NTMACHINENAME_STRING);
    CString UserName((LPCSTR) IDS_DF_USER_NAME_STRING);
	CString CookieID((LPCSTR) IDS_DF_COOKIE_ID_STRING);
    CString JobSeq((LPCSTR) IDS_DF_JOB_SEQUENCE);
    CString Status((LPCSTR) IDS_DF_STATUS_STRING);
    CString StartTime((LPCSTR) IDS_DF_START_TIME_STRING);
    CString Timeout((LPCSTR) IDS_DF_TIMEOUT_STRING);
    CString Program ((LPCSTR) IDS_DF_PROGRAM_STRING);
    CString JobCount ((LPCSTR) "Job Count");
    CString BusyTime ((LPCSTR) "Busy %");
    CString ErrorSince ((LPCSTR) "Error Since");
    CString ErrorCount ((LPCSTR) "Error Count");
    CString TimeoutCount ((LPCSTR) "Timeout Count");

    lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem=0;
    lvc.pszText=(LPTSTR) ((LPCTSTR)JobString);
    lvc.cx=4 * ListCtrl.GetStringWidth((LPCTSTR)JobString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(0,&lvc);

    lvc.iSubItem=1;
    lvc.pszText=(LPTSTR) ((LPCTSTR)IPAddress);
    lvc.cx=2 * ListCtrl.GetStringWidth((LPCTSTR)IPAddress) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(1,&lvc);

    lvc.iSubItem=2;
    lvc.pszText=(LPTSTR) ((LPCTSTR)UserName);
    lvc.cx=2 * ListCtrl.GetStringWidth((LPCTSTR)UserName) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(2,&lvc);

    lvc.iSubItem=3;
    lvc.pszText=(LPTSTR) ((LPCTSTR)CookieID);
    lvc.cx=ListCtrl.GetStringWidth((LPCTSTR)CookieID) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(3,&lvc);

    lvc.iSubItem=4;
    lvc.pszText=(LPTSTR) ((LPCTSTR)JobSeq);
    lvc.cx=ListCtrl.GetStringWidth((LPCTSTR)JobSeq) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    ListCtrl.InsertColumn(4,&lvc);

    lvc.iSubItem=5;
    lvc.pszText=(LPTSTR) ((LPCTSTR)Status);
    lvc.cx=2 * ListCtrl.GetStringWidth((LPCTSTR)Status) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(5,&lvc);

    lvc.iSubItem=6;
    lvc.pszText=(LPTSTR) ((LPCTSTR)StartTime);
    lvc.cx=2 * ListCtrl.GetStringWidth((LPCTSTR)StartTime) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(6,&lvc);

    lvc.iSubItem=7;
    lvc.pszText=(LPTSTR) ((LPCTSTR)Timeout);
    lvc.cx=3 * ListCtrl.GetStringWidth((LPCTSTR)Timeout) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(7,&lvc);

    lvc.iSubItem=8;
    lvc.pszText=(LPTSTR) ((LPCTSTR)Program);
    lvc.cx=6 * ListCtrl.GetStringWidth((LPCTSTR)Program) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(8,&lvc);

    lvc.iSubItem=9;
    lvc.pszText=(LPTSTR) ((LPCTSTR)JobCount);
    lvc.cx=ListCtrl.GetStringWidth((LPCTSTR)JobCount) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    ListCtrl.InsertColumn(9,&lvc);

    lvc.iSubItem=10;
    lvc.pszText=(LPTSTR) ((LPCTSTR)BusyTime);
    lvc.cx=ListCtrl.GetStringWidth((LPCTSTR)BusyTime) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    ListCtrl.InsertColumn(10,&lvc);

    lvc.iSubItem=11;
    lvc.pszText=(LPTSTR) ((LPCTSTR)ErrorSince);
    lvc.cx=2 * ListCtrl.GetStringWidth((LPCTSTR)ErrorSince) + 20;
    lvc.fmt=LVCFMT_LEFT;
    ListCtrl.InsertColumn(11,&lvc);

    lvc.iSubItem=12;
    lvc.pszText=(LPTSTR) ((LPCTSTR)ErrorCount);
    lvc.cx=ListCtrl.GetStringWidth((LPCTSTR)ErrorCount) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    ListCtrl.InsertColumn(12,&lvc);

    lvc.iSubItem=13;
    lvc.pszText=(LPTSTR) ((LPCTSTR)TimeoutCount);
    lvc.cx=ListCtrl.GetStringWidth((LPCTSTR)TimeoutCount) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    ListCtrl.InsertColumn(13,&lvc);

    ReadResourceList();

    int resCount = m_DFResourceList.GetSize();

    // Just create the ListView items here. The text for the subitems
    // are created in the OnUpdate() function.

    int i, j = 0;
    for (i = 0; i < resCount; i++)
    {
        LV_ITEM lvi;
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];

        if (pDFResource)
        {
            int nItem;

            lvi.mask=LVIF_TEXT | LVIF_STATE;
            lvi.iItem=j++;
            lvi.iSubItem=0;
            lvi.pszText=pDFResource->m_JobIDString.GetBuffer(1024);
            lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
            lvi.state = 0;

            nItem = ListCtrl.InsertItem(&lvi);
            if (nItem != -1)
                ListCtrl.SetItemData(nItem, (DWORD) pDFResource);
            else
                break;
        }
    }

    ListCtrl.SetRedraw(FALSE);
    ListCtrl.SortItems(CompareFunc, (LPARAM) this);
    ListCtrl.SetRedraw(TRUE);


    SetTimer (TIMEOUT_CHECK_TIMERID, TIMEOUT_CHECK_INTERVAL, NULL);
    WritingStatsToFile = 0;
    SetTimer (STATISTICS_WRITE_TIMERID, 10 * 1000, NULL);
    SetTimer (VIEW_REFRESH_TIMERID, m_ViewRefreshInterval * 1000, NULL);

	OnUpdate(NULL, 1, NULL);

    // Extensions to the ListView class defined in Listvwex.cpp

    AddHeaderToolTip(0, "Data Flow Resource Type");
    AddHeaderToolTip(1, "Remote machine where the job is performed");
    AddHeaderToolTip(2, "Login name of the user on the remote machine");
    AddHeaderToolTip(3, "Issue Tracking number being processed");
    AddHeaderToolTip(4, "Job Sequence number");
    AddHeaderToolTip(5, "Status of the remote machine");
    AddHeaderToolTip(6, "Start time of the job");
    AddHeaderToolTip(7, "Time after which the job is abandoned");
    AddHeaderToolTip(8, "Program being executed on the remote machine");
    AddHeaderToolTip(9, "Number of Jobs executed by the resource");
    AddHeaderToolTip(10, "Percentage of time the resource was busy");
    AddHeaderToolTip(11, "Time since which the resource has error");
    AddHeaderToolTip(12, "Number of consecutive errors");

    AutoSizeColumns( 0 );
    AutoSizeColumns( 1 );
    AutoSizeColumns( 2 );

#ifdef _DEBUG
    FILE *fp;
    fp = fopen (m_ModulePath + "AVISDFRL.Log", "w");
    if (fp)
        fclose (fp);
#endif
    if (m_MaxLogFileSize && !m_LogFileName.IsEmpty())
    {
        m_LogFile = fopen (m_ModulePath + m_LogFileName, "a");
        if (m_LogFile)
        {
            if (m_LogBufferSize)
                setvbuf (m_LogFile, NULL, _IOFBF, m_LogBufferSize);
            fseek (m_LogFile, 0, SEEK_END);
            m_LogFileSize = ftell(m_LogFile);
        }
    }
}

void CDFLauncherView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/) 
{
    // Donot refresh window if the window is not visible or is in iconic state
    if (lHint != 1)
    {
        if (((CMainFrame *) AfxGetMainWnd())->IsIconic())
            return;

        if (IsWindowVisible() == FALSE)
            return;
    }

    CListCtrl& ListCtrl=GetListCtrl();

    int resCount = m_DFResourceList.GetSize();

    // Display all the subitems of the resources

	CString TmpString((LPCSTR) IDS_DF_JOB_STRING);
    CString JobString;

    if (m_ResourceModified)
    {
        JobString = _T("* ");
        JobString += TmpString;
    }
    else
    {
        JobString = TmpString;
    }

    LV_COLUMN lvc;
    lvc.mask=LVCF_TEXT;

    lvc.iSubItem=0;
    lvc.pszText=(LPTSTR) ((LPCTSTR)JobString);
    ListCtrl.SetColumn(0,&lvc);

	int statIndex;
	CString statStr;

	statIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_INDICATOR_COUNTS);
	if (statIndex != -1)
	{
        int downCount = 0;
        int freeCount = 0;
        LONG errorCount = 0;
        LONG timeoutCount = 0;

        for (int i = 0; i < resCount; i++)
        {
    	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource)
            {
                if (pDFResource->m_Status == DF_RESOURCE_DOWN)
                    downCount++;
                if (pDFResource->m_Status == DF_RESOURCE_FREE)
                    freeCount++;
                errorCount += pDFResource->m_ErrorCount;
                timeoutCount += pDFResource->m_TimeoutCount;
            }
        }
		statStr.Format ("%d/%d/%d %ld %ld",
            ListCtrl.GetItemCount(), resCount - downCount, freeCount, errorCount, timeoutCount);
		((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(statIndex, statStr);
    }

	statIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_INDICATOR_UPTIME);
	if (statIndex != -1)
	{
        COleDateTimeSpan upTime = COleDateTime::GetCurrentTime() - m_AVISDFStartTime;

        statStr.Format("%4d-%02d:%02d:%02d", upTime.GetDays(),
            upTime.GetHours(),
            upTime.GetMinutes(),
            upTime.GetSeconds());
		((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(statIndex, statStr);
    }

    // Flicker free drawing
    ListCtrl.SetRedraw(FALSE);
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];

        if (pDFResource)
        {
            int nItem;
            LV_FINDINFO lvf;

            lvf.flags = LVFI_PARAM;
            lvf.lParam = (LPARAM) pDFResource;

            nItem = ListCtrl.FindItem (&lvf, -1);

            if (nItem != -1)
            {
                CString newLabel;
                CString oldStr;

                if (pDFResource->m_Saved != TRUE)
                {
                    newLabel = _T("* ");
                    newLabel += pDFResource->m_JobIDString;
                }
                else
                {
                    newLabel = pDFResource->m_JobIDString;
                }

                // Flicker free drawing
                oldStr = ListCtrl.GetItemText (nItem, 0);
                if (oldStr.Compare(newLabel))
                    ListCtrl.SetItemText (nItem, 0, newLabel);

                oldStr = ListCtrl.GetItemText (nItem, 1);
                if (oldStr.Compare(pDFResource->m_IPAddress))
                    ListCtrl.SetItemText (nItem, 1, pDFResource->m_IPAddress);

                oldStr = ListCtrl.GetItemText (nItem, 2);
                if (oldStr.Compare(pDFResource->m_UserName))
                    ListCtrl.SetItemText (nItem, 2, pDFResource->m_UserName);

                if (pDFResource->m_pDFJob) // Currently processing some job
                {
                    COleDateTime tmpDateTime;
                    TCHAR jobSeq[32];
                    TCHAR fullProgram[1024];

                    oldStr = ListCtrl.GetItemText (nItem, 3);
                    if (oldStr.Compare(pDFResource->m_pDFJob->GetCookieId()))
                        ListCtrl.SetItemText (nItem, 3, pDFResource->m_pDFJob->GetCookieId());

                    sprintf (jobSeq, _T("%d"), pDFResource->m_pDFJob->GetJobSeq());
                    oldStr = ListCtrl.GetItemText (nItem, 4);
                    if (oldStr.Compare(jobSeq))
                        ListCtrl.SetItemText (nItem, 4, jobSeq);

                    oldStr = ListCtrl.GetItemText (nItem, 6);
                    if (oldStr.Compare(pDFResource->m_StartTime.Format("%m/%d/%Y %H:%M:%S")))
                        ListCtrl.SetItemText (nItem, 6, pDFResource->m_StartTime.Format("%m/%d/%Y %H:%M:%S"));

                    COleDateTimeSpan bonusTime (0,0,0,30);
                    tmpDateTime = pDFResource->m_StartTime + pDFResource->m_TimeoutPeriod + bonusTime;
                    oldStr = ListCtrl.GetItemText (nItem, 7);
                    if (oldStr.Compare(tmpDateTime.Format("%m/%d/%Y %H:%M:%S")))
                        ListCtrl.SetItemText (nItem, 7, tmpDateTime.Format("%m/%d/%Y %H:%M:%S"));

                    sprintf (fullProgram, _T("%s --Cookie %s --ProcessId %d"),
                        (LPCTSTR) pDFResource->m_ProgramName,
                        (LPCTSTR) pDFResource->m_pDFJob->GetCookieId(),
                        pDFResource->m_pDFJob->GetJobSeq());

                    CString commandArg = pDFResource->m_pDFJob->GetCommandArg();
                    commandArg.TrimLeft();
                    commandArg.TrimRight();

                    if (!commandArg.IsEmpty())
                    {
                        strcat (fullProgram, " --Parameter \"");
                        strcat (fullProgram, (LPCTSTR) commandArg);
                        strcat (fullProgram, "\"");
                    }

                    CString uncPath = pDFResource->m_pDFJob->GetUNCPath();
                    uncPath.TrimLeft();
                    uncPath.TrimRight();

                    // For AIX machines use / character for UNC path
                    CString UNIXuncPath (uncPath);
                    UNIXuncPath.Replace("\\", "/");

                    if (!uncPath.IsEmpty())
                    {
                        strcat (fullProgram, " --UNC ");
                        if (pDFResource->m_NTMachineName.IsEmpty())
                            strcat (fullProgram, (LPCTSTR) UNIXuncPath);
                        else
                            strcat (fullProgram, (LPCTSTR) uncPath);
                    }

                    oldStr = ListCtrl.GetItemText (nItem, 8);
                    if (oldStr.Compare(fullProgram))
                        ListCtrl.SetItemText (nItem, 8, fullProgram);
                }
                else // Not processing any Job
                {
                    oldStr = ListCtrl.GetItemText (nItem, 3);
                    if (oldStr.Compare(" "))
                        ListCtrl.SetItemText (nItem, 3, _T(" "));

                    oldStr = ListCtrl.GetItemText (nItem, 4);
                    if (oldStr.Compare(" "))
                        ListCtrl.SetItemText (nItem, 4, _T(" "));

                    oldStr = ListCtrl.GetItemText (nItem, 6);
                    if (oldStr.Compare(" "))
                        ListCtrl.SetItemText (nItem, 6, _T(" "));

                    oldStr = ListCtrl.GetItemText (nItem, 7);
                    if (oldStr.Compare(pDFResource->m_TimeoutPeriod.Format("%H:%M:%S")))
                        ListCtrl.SetItemText (nItem, 7, pDFResource->m_TimeoutPeriod.Format("%H:%M:%S"));

                    oldStr = ListCtrl.GetItemText (nItem, 8);
                    if (oldStr.Compare(pDFResource->m_ProgramName))
                        ListCtrl.SetItemText (nItem, 8, pDFResource->m_ProgramName);
                }

                CString dispStr;

                oldStr = ListCtrl.GetItemText (nItem, 5);
                switch (pDFResource->m_Status)
                {
                case DF_RESOURCE_FREE:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_FREE_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;

                case DF_RESOURCE_BUSY:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_BUSY_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;

                case DF_RESOURCE_DOWN:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_DOWN_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;

                case DF_RESOURCE_COMPLETED:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_COMPLETED_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;

                case DF_RESOURCE_IGNORE:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_IGNORE_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;

                case DF_RESOURCE_ERROR:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_ERROR_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;

                case DF_RESOURCE_TIMEDOUT:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_TIMEDOUT_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;

                default:
                    {
                        CString str((LPCTSTR) IDS_DF_RESOURCE_UNKNOWN_STR);
                        if (oldStr.Compare(str))
                            ListCtrl.SetItemText (nItem, 5, str);
                    }
                    break;
                }

                oldStr = ListCtrl.GetItemText (nItem, 9);
                dispStr.Format("%lu", pDFResource->m_JobCount);
                if (oldStr.Compare(dispStr))
                    ListCtrl.SetItemText (nItem, 9, dispStr);

                oldStr = ListCtrl.GetItemText (nItem, 10);
                double totalBusySeconds = pDFResource->m_BusyTime.GetTotalSeconds();
                if (pDFResource->m_Status != DF_RESOURCE_FREE &&
                    pDFResource->m_Status != DF_RESOURCE_DOWN)
                {
                    totalBusySeconds += (COleDateTime::GetCurrentTime() - pDFResource->m_StartTime).GetTotalSeconds();
                }
                double totalElapsedSeconds = (COleDateTime::GetCurrentTime() - pDFResource->m_ResStartTime).GetTotalSeconds();
                if (totalElapsedSeconds == 0.0)
                    totalElapsedSeconds = 1.0;
                int busyPercent = (int) ((totalBusySeconds * 100) / totalElapsedSeconds);
                if (busyPercent < 0)
                    busyPercent = 0;
                if (busyPercent > 100)
                    busyPercent = 100;
                dispStr.Format("%d %%", busyPercent);
                if (oldStr.Compare(dispStr))
                    ListCtrl.SetItemText (nItem, 10, dispStr);

                oldStr = ListCtrl.GetItemText (nItem, 11);
                if (pDFResource->m_ErrorFlag == TRUE)
                    dispStr = pDFResource->m_LastErrorTime.Format("%m/%d/%Y %H:%M:%S");
                else
                    dispStr = " ";
                if (oldStr.Compare(dispStr))
                    ListCtrl.SetItemText (nItem, 11, dispStr);

                oldStr = ListCtrl.GetItemText (nItem, 12);
                if (pDFResource->m_ErrorFlag == TRUE)
                    dispStr.Format("%lu", pDFResource->m_ErrorCount);
                else
                    dispStr = " ";
                if (oldStr.Compare(dispStr))
                    ListCtrl.SetItemText (nItem, 12, dispStr);

                oldStr = ListCtrl.GetItemText (nItem, 13);
                if (pDFResource->m_TimeoutCount)
                    dispStr.Format("%lu", pDFResource->m_TimeoutCount);
                else
                    dispStr = " ";
                if (oldStr.Compare(dispStr))
                    ListCtrl.SetItemText (nItem, 13, dispStr);
            }
        }
    }
    ListCtrl.SetRedraw (TRUE);

    // If the resource summary dialog is being displayed, then refresh
    // the contents of the resource summary dialog also

    if (m_pResourceSummaryDlg)
    {
        int resCount = m_DFResourceList.GetSize();

        m_pResourceSummaryDlg->m_JobIDArray.RemoveAll();
        m_pResourceSummaryDlg->m_TotalCountArray.RemoveAll();
        m_pResourceSummaryDlg->m_FreeCountArray.RemoveAll();
        m_pResourceSummaryDlg->m_AvailableCountArray.RemoveAll();
        m_pResourceSummaryDlg->m_TotalJobDoneArray.RemoveAll();

        m_pResourceSummaryDlg->m_JobIDArray.SetSize (0, 10);
        m_pResourceSummaryDlg->m_TotalCountArray.SetSize (0, 10);
        m_pResourceSummaryDlg->m_AvailableCountArray.SetSize (0, 10);
        m_pResourceSummaryDlg->m_FreeCountArray.SetSize (0, 10);
        m_pResourceSummaryDlg->m_TotalJobDoneArray.SetSize (0, 10);

        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource)
            {
                int jobIDCount = m_pResourceSummaryDlg->m_JobIDArray.GetSize();
                for (int j = 0; j < jobIDCount; j++)
                {
                    if (!pDFResource->m_JobIDString.CompareNoCase(m_pResourceSummaryDlg->m_JobIDArray[j]))
                        break;
                }
                if (j >= jobIDCount)
                {
                    j = m_pResourceSummaryDlg->m_JobIDArray.Add(pDFResource->m_JobIDString);
                    m_pResourceSummaryDlg->m_TotalCountArray.Add(0);
                    m_pResourceSummaryDlg->m_FreeCountArray.Add(0);
                    m_pResourceSummaryDlg->m_AvailableCountArray.Add(0);
                    m_pResourceSummaryDlg->m_TotalJobDoneArray.Add(0);
                }
                m_pResourceSummaryDlg->m_TotalCountArray[j] ++;
                if (pDFResource->m_Status != DF_RESOURCE_DOWN)
                    m_pResourceSummaryDlg->m_AvailableCountArray[j] ++;
                if (pDFResource->m_Status == DF_RESOURCE_FREE)
                    m_pResourceSummaryDlg->m_FreeCountArray[j] ++;
                m_pResourceSummaryDlg->m_TotalJobDoneArray[j] += pDFResource->m_JobCount;
            }
        }
        m_pResourceSummaryDlg->SendMessage (DFLAUNCHER_REFRESH_SUMMARY, 0, 0);
    }
}

void CDFLauncherView::OnTimer(UINT nIDEvent) 
{

    if (nIDEvent == TIMEOUT_CHECK_TIMERID)
    {
        COleDateTime curTime = COleDateTime::GetCurrentTime();
        COleDateTimeSpan bonusTime (0,0,0,30);

        int resCount = m_DFResourceList.GetSize();
        for (int i = 0; i < resCount; i++)
        {
            FILETIME currentFileTime;
            LARGE_INTEGER liCurrentFileTime;

            GetSystemTimeAsFileTime (&currentFileTime);
            liCurrentFileTime.LowPart = currentFileTime.dwLowDateTime;
            liCurrentFileTime.HighPart = currentFileTime.dwHighDateTime;

	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if ((pDFResource && pDFResource->m_Status == DF_RESOURCE_BUSY))
            {
                LARGE_INTEGER liResourceStartFileTime;
                /*
                if ((pDFResource->m_StartTime + pDFResource->m_TimeoutPeriod + bonusTime) <
                    curTime)*/
                // Use FILETIME so that the time interval calculation is not affected by the
                // changes due to DST
                liResourceStartFileTime.LowPart = pDFResource->m_StartFileTime.dwLowDateTime;
                liResourceStartFileTime.HighPart = pDFResource->m_StartFileTime.dwHighDateTime;
                LONGLONG resRunTime = liCurrentFileTime.QuadPart - liResourceStartFileTime.QuadPart;
                LONGLONG resTimeoutTime = (LONGLONG) (pDFResource->m_TimeoutPeriod.GetTotalSeconds() + 30) * 10000000;
                if (resRunTime > resTimeoutTime)
                {
                    // Just terminate the process. The thread on completion
                    // will post the completion message. On receiving the
                    // message the resource structure will be updated properly.
                    // This is to ensure that a true completion of the process
                    // is processed properly.

                    if (pDFResource->m_hProcess)
                        TerminateProcess (pDFResource->m_hProcess, TERMINATE_TIMEOUT);
                }
            }
            // Auto enable the resource after 1 hour has elapsed since the time the
            // resource was automatically disabled. This is to take care of some temporary
            // problems which caused the resource to go down and the problem has been fixed
            // since then. If the problem still exists the resource will be again automatically
            // disabled when the enxt job submitted on the resource also fails.
            if ((pDFResource && pDFResource->m_Status == DF_RESOURCE_DOWN))
            {
                if (pDFResource->m_ErrorFlag == TRUE &&
                    m_AutoDisableCount && pDFResource->m_ErrorCount >= m_AutoDisableCount)
                {
                    COleDateTime curTime = COleDateTime::GetCurrentTime();
                    COleDateTimeSpan timeDiff = curTime - pDFResource->m_FinalErrorTime;
                    COleDateTimeSpan checkTime(0, 1, 0, 0);

                    if (timeDiff >= checkTime)
                    {
                        CListCtrl& ListCtrl=GetListCtrl();
                        pDFResource->m_Status = DF_RESOURCE_FREE;
                        pDFResource->m_MarkedForDisable = FALSE;
                        //pDFResource->m_ErrorFlag = FALSE;
                        ListCtrl.SetRedraw(FALSE);
                        ListCtrl.SortItems(CompareFunc, (LPARAM) this);
                        ListCtrl.SetRedraw(TRUE);

                        class CAVISSendMail avisSendMail;
                        CString msgStr;

                        msgStr.Format ("Auto enabling resource <%s> on machine <%s> at <%s>.",
                            pDFResource->m_JobIDString,
                            pDFResource->m_IPAddress,
                            curTime.Format("%m/%d/%Y %H:%M:%S")) ;

                        BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, msgStr);
                        /*
                        if (brc != TRUE)
                        {
                            AfxMessageBox ("Error in sending mail.\n");
                        }
                        else
                        {
                    //        AfxMessageBox ("Mail send successfully.\n");
                        }
                        */
                    }
                }
            }
        }
    }
    if (nIDEvent == STATISTICS_WRITE_TIMERID)
    {
        KillTimer (nIDEvent);
        if (!WritingStatsToFile)
            ShowMachineStatistics (SAVE_TO_FILE);
        SetTimer (STATISTICS_WRITE_TIMERID, m_StatisticsWriteInterval * 60 * 1000, NULL);
    }
    if (nIDEvent == VIEW_REFRESH_TIMERID)
    {
        OnUpdate(NULL, 0, NULL);
        UpdateWindow();
    }
}

void CDFLauncherView::OnActionShowalmwindow() 
{
    CDFLauncherApp * theApp = (CDFLauncherApp *) AfxGetApp();
    CWnd * pALMWnd = CWnd::FromHandle(theApp->m_hALMWnd);

    pALMWnd->ShowWindow(SW_NORMAL);
    pALMWnd->SetForegroundWindow();
}

/* The format of the resource file is as follows
Service : Classifier
Machine : srix.watson.ibm.com
NTMachineName : srix
User : c1sjalan
Timeout : 120
Program : perl -w c:\dataflow\classifier.pl
*/
void CDFLauncherView::ReadResourceList()
{
    FILE *fp;
    CString tmpStr;
    int timeoutVal;
    CString resFile ((LPCTSTR) IDS_RESOURCE_FILE_NAME);
    CString resourceFile = m_ModulePath + resFile;

    CParamValue paramValue;

    fp = fopen (resourceFile, _T("r"));
    if (fp != (FILE *) NULL)
    {
        int numParam;
        CString serviceTag((LPCTSTR) IDS_SERVICE_TAG);
        CString machineTag((LPCTSTR) IDS_MACHINE_TAG);
        CString userTag((LPCTSTR) IDS_USER_TAG);
        CString timeoutTag((LPCTSTR) IDS_TIMEOUT_TAG);
        CString programTag((LPCTSTR) IDS_PROGRAM_TAG);
        CString ntmachineTag((LPCTSTR) IDS_NTMACHINENAME_TAG);

        while (TRUE)
        {
            int defParamSize = paramValue.m_DefaultParam.GetSize();
            for (int i = 0; i < defParamSize; i++)
            {
                CString defParam = paramValue.m_DefaultParam[i];

                if (!defParam.CompareNoCase(serviceTag))
                    paramValue.m_DefaultValue[i].Empty();
                if (!defParam.CompareNoCase(machineTag))
                    paramValue.m_DefaultValue[i].Empty();
                if (!defParam.CompareNoCase(ntmachineTag))
                    paramValue.m_DefaultValue[i].Empty();
            }

            numParam = paramValue.ReadParamValue (fp, TRUE, TRUE);

            if (!numParam)
                break;

            // There should be atleast 5 parameters. The parameter NTMachineName
            // is optional. 
            if (numParam >= 5)
            {
                CString tmpValue;

                CDFResource * pDFResource = new CDFResource();
                int j = 0;

                paramValue.GetValue (ntmachineTag, pDFResource->m_NTMachineName);
                if (TRUE == paramValue.GetValue (serviceTag, pDFResource->m_JobIDString))
                    j++;

                if (TRUE == paramValue.GetValue (machineTag, pDFResource->m_IPAddress))
                    j++;

                if (TRUE == paramValue.GetValue (userTag, pDFResource->m_UserName))
                    j++;

                if (TRUE == paramValue.GetValue (timeoutTag, tmpValue))
                {
                    timeoutVal = atoi (tmpValue);
                    if (timeoutVal < 10)
                        timeoutVal = 10;
                    COleDateTimeSpan tmpPeriod(0,0,0,timeoutVal);
                    pDFResource->m_TimeoutPeriod = tmpPeriod;
                    j++;
                }

                if (TRUE == paramValue.GetValue (programTag, pDFResource->m_ProgramName))
                    j++;

                if (j == 5)
                    pDFResource->m_Index = m_DFResourceList.Add (pDFResource);
                else
                    delete pDFResource;
            }
        }
        
        fclose (fp);
    }
    RemoveDuplicateResources();

}

BOOL g_TerminateDLL = FALSE;
CResourceSummary * g_pResourceSummaryDlg = NULL;

void CDFLauncherView::CleanUp()
{
    int resCount = m_DFResourceList.GetSize();
    BOOL processKilled = 0;
    g_pResourceSummaryDlg = m_pResourceSummaryDlg;
    m_TerminateDLL = TRUE;
    g_TerminateDLL = TRUE;

    CWaitCursor waitCursor;
    CDFLauncherApp * theApp = (CDFLauncherApp *) AfxGetApp();
    CWnd * pALMWnd = CWnd::FromHandle(theApp->m_hALMWnd);
    AfxGetMainWnd()->ShowWindow(SW_HIDE);
    ((CFrameWnd *) pALMWnd)->SetMessageText("Waiting for application termination...");
    pALMWnd->UpdateWindow();

    // Allow for all pending jobs to start so that the process handles
    // are updated properly
    Sleep (3000);
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource && pDFResource->m_hProcess &&
            (pDFResource->m_Status != DF_RESOURCE_FREE))
        {
            CMutex Mutex(FALSE, "ISDFLauncherStartRSH");
            CSingleLock sLock (&Mutex, TRUE);

            pDFResource->m_Status = DF_RESOURCE_IGNORE;
            TerminateProcess (pDFResource->m_hProcess, TERMINATE_EXIT);
            if (pDFResource->m_pDFJob)
            {
                delete pDFResource->m_pDFJob;
                pDFResource->m_pDFJob = NULL;
            }
            if (pDFResource->m_pDFIgnoreJob)
            {
                delete pDFResource->m_pDFIgnoreJob;
                pDFResource->m_pDFIgnoreJob = NULL;
            }
            processKilled ++;
        }
    }
    if (processKilled > 5)
        processKilled = 5;

    // Give some time for the killed process threads to exit.
    if (processKilled)
        Sleep (processKilled * 2000);
}

// Start all analysis jobs on a separate thread
UINT StartRSH(LPVOID ptr)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD exitCode;
    CDFResource * pDFResource = (CDFResource *) ptr;
    TCHAR cmdLine[1024];
    TCHAR tmpLine[1024];
    CString programName((LPCTSTR) IDS_PROGRAM_NAME);
    BOOL processStarted = FALSE;
    // Make sure that we are starting only one process at a time. However once
    // a process is started we don't wait for the completion. 

    CMutex Mutex(FALSE, "ISDFLauncherStartRSH");
    CSingleLock sLock (&Mutex, TRUE);

    if (pDFResource->m_Status != DF_RESOURCE_IGNORE)
    {
        CString commandArg = pDFResource->m_pDFJob->GetCommandArg();
        commandArg.TrimLeft();
        commandArg.TrimRight();

        // If the resource exists on the local machine, then run the program
        // as it is and not to run rsh. rsh is used only for the remote machines
        if (pDFResource->m_IPAddress.CompareNoCase(g_localComputerName) &&
            pDFResource->m_NTMachineName.CompareNoCase(g_localComputerName))
        {
            int strLen = strlen(programName);

            // Replace all single backslash with double backslash as the rsh/rshd command
            // eats up one backslash before invoking the program at the remote machine
            for (int i = 0, j = 0; i < strLen; i++)
            {
                tmpLine[j++] = programName[i];
                if (programName[i] == _T('\\'))
                {
                    tmpLine[j++] = _T('\\');
                }
            }
            tmpLine[j] = _T('\0');

            sprintf (cmdLine, _T("%s %s -l %s %s --Cookie %s --ProcessId %d"),
                (LPCTSTR) tmpLine,
                (LPCTSTR) pDFResource->m_IPAddress,
                (LPCTSTR) pDFResource->m_UserName,
                (LPCTSTR) pDFResource->m_ProgramName,
                (LPCTSTR) pDFResource->m_pDFJob->GetCookieId(),
                pDFResource->m_pDFJob->GetJobSeq());

            if (!commandArg.IsEmpty())
            {
                strcat (cmdLine, " --Parameter \\\"");
                strcat (cmdLine, (LPCTSTR) commandArg);
                strcat (cmdLine, "\\\"");
            }

            CString uncPath = pDFResource->m_pDFJob->GetUNCPath();
            uncPath.TrimLeft();
            uncPath.TrimRight();
            CString UNIXuncPath (uncPath);
            UNIXuncPath.Replace("\\", "/");

            if (!uncPath.IsEmpty())
            {
                strcat (cmdLine, " --UNC ");
                if (pDFResource->m_NTMachineName.IsEmpty())
                    strcat (cmdLine, (LPCTSTR) UNIXuncPath);
                else
                    strcat (cmdLine, (LPCTSTR) uncPath);
            }
        }
        else
        {
            sprintf (cmdLine, _T("%s --Cookie %s --ProcessId %d"),
                (LPCTSTR) pDFResource->m_ProgramName,
                (LPCTSTR) pDFResource->m_pDFJob->GetCookieId(),
                pDFResource->m_pDFJob->GetJobSeq());

            if (!commandArg.IsEmpty())
            {
                strcat (cmdLine, " --Parameter \"");
                strcat (cmdLine, (LPCTSTR) commandArg);
                strcat (cmdLine, "\"");
            }

            CString uncPath = pDFResource->m_pDFJob->GetUNCPath();
            uncPath.TrimLeft();
            uncPath.TrimRight();
            CString UNIXuncPath (uncPath);
            UNIXuncPath.Replace("\\", "/");

            if (!uncPath.IsEmpty())
            {
                strcat (cmdLine, " --UNC ");
                if (pDFResource->m_NTMachineName.IsEmpty())
                    strcat (cmdLine, (LPCTSTR) UNIXuncPath);
                else
                    strcat (cmdLine, (LPCTSTR) uncPath);
            }
        }

        memset (&si, 0, sizeof (STARTUPINFO));
        si.cb = sizeof (STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;

        if (g_ShowAppWindow == TRUE)
            si.wShowWindow = SW_SHOW;
        else
            si.wShowWindow = SW_HIDE;

        si.lpTitle = cmdLine;
        memset (&pi, 0, sizeof (PROCESS_INFORMATION));
        if ( CreateProcess (NULL,
                cmdLine,
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                NULL,
                &si,
                &pi) )
        {
            processStarted = TRUE;

            pDFResource->m_hProcess = pi.hProcess;
            CloseHandle (pi.hThread);
            //Sleep (500);
        }
    }
    sLock.Unlock();

    if (processStarted == TRUE)
    {
        WaitForSingleObject(pi.hProcess, INFINITE);
        GetExitCodeProcess(pi.hProcess, &exitCode);

        // Sleep an additional 3 seconds to allow for all writes etc. by the
        // analysis programs to complete over the network
        // Sleep (3000);
        (AfxGetMainWnd())->PostMessage (DFLAUNCHER_JOB_COMPLETE, exitCode,
            (LPARAM) pDFResource);
    }
    else
    {
        (AfxGetMainWnd())->PostMessage (DFLAUNCHER_JOB_COMPLETE, 1,
            (LPARAM) pDFResource);
    }

    return 0;
}

LONG CDFLauncherView::OnALMReqStartJob(WPARAM /* w */, LPARAM l)
{
    CSingleLock sLock(&m_Mutex, TRUE);
    CDFLauncherApp * theApp = (CDFLauncherApp *) AfxGetApp();
    CWnd * pALMWnd = CWnd::FromHandle(theApp->m_hALMWnd);

    class DFJob * pDFJob = (class DFJob *) l;

    if (!pDFJob)
        return 1;

#ifdef _DEBUG
    LogMessage (pDFJob, "Receive Start");
#endif

    CString cookieId = pDFJob->GetCookieId();

    cookieId.TrimLeft();
    cookieId.TrimRight();

    if (cookieId.IsEmpty())
    {
        pDFJob->SetStatus(DFJob::POSTPONED);
#ifdef _DEBUG
        LogMessage (pDFJob, "Send");
#endif
        pALMWnd->PostMessage (DFLAUNCHER_POST_JOB_STATUS, 0, (LPARAM) pDFJob);
        return 1;
    }

    int resCount = m_DFResourceList.GetSize();
    if (m_resIndex >= resCount)
        m_resIndex = 0;

    // Start checking for resources from the last used resource so that all the
    // resources are used and one particular resource is not overloaded if
    // multiple resources are available to do the job.

    COleDateTime tmpTime = COleDateTime::GetCurrentTime();
    //COleDateTimeSpan tmpTimeSpan(0,0,0,3);
    COleDateTimeSpan tmpTimeSpan(0,0,0,0);

    for (int i = m_resIndex, loop = 0; (i < resCount && loop < 2); i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource && pDFResource->m_Status == DF_RESOURCE_FREE)
        {
            if (!pDFResource->m_JobIDString.CompareNoCase(pDFJob->GetJobId()))
            {
                int j = resCount;
                // Check if any other job has been started on the same machine within
                // last 3 seconds. Check only for remote machines.
                if (pDFResource->m_IPAddress.CompareNoCase(g_localComputerName) &&
                    pDFResource->m_NTMachineName.CompareNoCase(g_localComputerName))
                {
                    for (j = 0; j < resCount; j++)
                    {
                        CDFResource * pTmpDFResource = (CDFResource *) m_DFResourceList[j];

                        if (pTmpDFResource && pTmpDFResource != pDFResource && 
                            pTmpDFResource->m_Status != DF_RESOURCE_FREE &&
                            pTmpDFResource->m_Status != DF_RESOURCE_DOWN &&
                            !pTmpDFResource->m_IPAddress.CompareNoCase(pDFResource->m_IPAddress))
                        {
                            if (m_SingleJobPerMachine == TRUE)
                                break;

                            if ((tmpTime - pTmpDFResource->m_StartTime) < tmpTimeSpan)
                            {
                                break;
                            }
                        }
                    }
                }
                if (j >= resCount)
                {
                    pDFResource->m_Status = DF_RESOURCE_BUSY;
                    pDFResource->m_pDFJob = pDFJob;
                    pALMWnd->UpdateWindow();
                    pDFResource->m_hProcess = NULL;
                    pDFResource->m_pWinThread = AfxBeginThread (StartRSH, pDFResource);	
                    pDFResource->m_StartTime = COleDateTime::GetCurrentTime();
                    GetSystemTimeAsFileTime (&(pDFResource->m_StartFileTime));
                    pDFResource->m_TryCount = 1;
                    pDFResource->m_JobCount++;
                    m_resIndex = i+1;
                    break;
                }
            }
        }

        // Start from the beginning once the end of the loop is reached. It is
        // possible that some of the resources are checked for a second time but
        // there is no harm is doing so.
        if (i == (resCount - 1))
        {
            i = -1;
            loop ++;
        }
    }
    if (i >= resCount || loop >= 2)
    {
        pDFJob->SetStatus(DFJob::POSTPONED);
#ifdef _DEBUG
        LogMessage (pDFJob, "Send");
#endif
        pALMWnd->PostMessage (DFLAUNCHER_POST_JOB_STATUS, 0, (LPARAM) pDFJob);
    }
    else
    {
        pDFJob->SetStatus(DFJob::ACCEPTED);
#ifdef _DEBUG
        LogMessage (pDFJob, "Send");
#endif
        pALMWnd->SendMessage (DFLAUNCHER_POST_JOB_STATUS, 0, (LPARAM) pDFJob);
    }

    return 1;
}

LONG CDFLauncherView::OnALMReqIgnoreJob(WPARAM /* w */, LPARAM l)
{
    CSingleLock sLock(&m_Mutex, TRUE);
    class DFJob * pDFJob = (class DFJob *) l;

    if (!pDFJob)
        return 1;

#ifdef _DEBUG
        LogMessage (pDFJob, "Receive Ignore");
#endif

    int resCount = m_DFResourceList.GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource && pDFResource->m_pDFJob &&
            (!(pDFResource->m_pDFJob->GetCookieId()).CompareNoCase (pDFJob->GetCookieId())) &&
            (!(pDFResource->m_pDFJob->GetJobId()).CompareNoCase (pDFJob->GetJobId())) &&
            (pDFResource->m_pDFJob->GetJobSeq() == pDFJob->GetJobSeq()) )
        {
            // Call Terminate process if the remote machine is not an AIX machine.
            if (!pDFResource->m_NTMachineName.IsEmpty())
            {
                pDFResource->m_Status = DF_RESOURCE_IGNORE;
                if (pDFResource->m_hProcess)
                {
                    TerminateProcess (pDFResource->m_hProcess, 1);
                }
                else
                {
                    CMutex Mutex(FALSE, "ISDFLauncherStartRSH");
                    CSingleLock sLock (&Mutex, TRUE);

                    // Mark the resource to be ignored and kill the process.
                    // On receiving the process completion message the result is ignored.
                    if (pDFResource->m_hProcess)
                        TerminateProcess (pDFResource->m_hProcess, 1);
                }

                // Since the job completion status will not be reported to the 
                // main app window, free the memory allocated for the DFJob class
                delete pDFResource->m_pDFJob;
                pDFResource->m_pDFJob = NULL;
                pDFResource->m_pDFIgnoreJob = pDFJob;
            }
            else
            {
                // If the remote machine is an AIX machine, killing the local rsh program does not
                // terminate the remote program, hence just ignore the ignore request and let the
                // program complete normally.
                delete pDFJob;
            }
            break;
        }
    }

    // If the job was not found in the list then simply
    // delete the job object and ignore the message.
    if (i >= resCount)
    {
        delete pDFJob;
    }

    return 1;
}

LONG CDFLauncherView::OnALMReqReloadResources(WPARAM /* w */, LPARAM /* l */)
{
    CSingleLock sLock(&m_Mutex, TRUE);
    CListCtrl& ListCtrl=GetListCtrl();

#ifdef _DEBUG
        LogMessage (NULL, "Reload Resources");
#endif

    // If none of the existing resources are busy then free the list of
    // resources and reload from the resource file.
    int resCount = m_DFResourceList.GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource && 
            pDFResource->m_Status != DF_RESOURCE_FREE &&
            pDFResource->m_Status != DF_RESOURCE_DOWN )
        {
            break;
        }
    }
    if (i < resCount)
    {
        AfxMessageBox (IDP_RELOAD_FAIL_RESOURCE_BUSY, MB_ICONINFORMATION);
    }
    else
    {
        int resCount = m_DFResourceList.GetSize();
        int i, j;

        for (i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource)
                delete pDFResource;
        }
        m_DFResourceList.RemoveAll();
        m_DFResourceList.SetSize (0, 10);
        ListCtrl.DeleteAllItems();
        OnUpdate(NULL, 1, NULL);

        ReadResourceList();

        resCount = m_DFResourceList.GetSize();
        for (i = 0, j = 0; i < resCount; i++)
        {
            LV_ITEM lvi;
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];

            if (pDFResource)
            {
                int nItem;

                lvi.mask=LVIF_TEXT | LVIF_STATE;
                lvi.iItem=j++;
                lvi.iSubItem=0;
                lvi.pszText=pDFResource->m_JobIDString.GetBuffer(1024);
                lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
                lvi.state = 0;

                nItem = ListCtrl.InsertItem(&lvi);
                ListCtrl.SetItemData(nItem, (DWORD) pDFResource);
            }
        }

        ListCtrl.SetRedraw(FALSE);
        ListCtrl.SortItems(CompareFunc, (LPARAM) this);
        ListCtrl.SetRedraw(TRUE);
        OnUpdate(NULL, 1, NULL);

        AutoSizeColumns( 0 );
        AutoSizeColumns( 1 );
        AutoSizeColumns( 2 );
    }

    return 1;
}

LONG CDFLauncherView::OnDFLauncherDisableMachine(WPARAM w, LPARAM l)
{
    char *machineName = (char *) l;

    if (!machineName)
        return 1;

    int resCount = m_DFResourceList.GetSize();

    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource &&
            pDFResource->m_Status != DF_RESOURCE_DOWN &&
            !pDFResource->m_IPAddress.CompareNoCase (machineName) &&
            pDFResource->m_IPAddress.CompareNoCase(g_localComputerName) &&
            pDFResource->m_NTMachineName.CompareNoCase(g_localComputerName))
        {
            if (pDFResource->m_Status == DF_RESOURCE_FREE)
                pDFResource->m_Status = DF_RESOURCE_DOWN;
            else
                pDFResource->m_MarkedForDisable = TRUE;

            COleDateTime curTime = COleDateTime::GetCurrentTime();
            pDFResource->m_ErrorFlag = TRUE;
            pDFResource->m_ErrorCount = m_AutoDisableCount;
            pDFResource->m_FinalErrorTime = curTime;
            pDFResource->m_LastErrorTime = curTime;

            class CAVISSendMail avisSendMail;
            CString msgStr;

            msgStr.Format ("Disabling resource <%s> on machine <%s> at <%s>.\n"
                           "The machine is not responding to the remote command.\n"
                           "Check the status of Ataman rsh service on the machine.\n" ,
                pDFResource->m_JobIDString,
                pDFResource->m_IPAddress,
                curTime.Format("%m/%d/%Y %H:%M:%S")) ;

            BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, msgStr);
        }
    }

    delete machineName;
    return 1;
}


// Job completion message received from the worker thread.

LONG CDFLauncherView::OnDFLauncherJobComplete(WPARAM exitCode, LPARAM l)
{
    // If the process has been killed due to DLL termination then do not
    // worry about processing the return conditions.

    if (exitCode == TERMINATE_EXIT || g_TerminateDLL == TRUE)
        return 1;

    CDFLauncherApp * theApp = (CDFLauncherApp *) AfxGetApp();
    CWnd * pALMWnd = CWnd::FromHandle(theApp->m_hALMWnd);
    CDFResource * pDFResource = (CDFResource *) l;
    if (!pDFResource)
    {
        return 1;
    }

    if (pDFResource->m_hProcess)
    {
        CloseHandle (pDFResource->m_hProcess);
        pDFResource->m_hProcess = 0;
    }
    pDFResource->m_pWinThread = NULL;

    // Make sure that the reource is really busy. If the results are to
    // be ignored it is handled here

    LogCompletionMessage (pDFResource, exitCode);
    if (pDFResource->m_Status == DF_RESOURCE_BUSY)
    {
        switch (exitCode)
        {
        case TERMINATE_TIMEOUT: // The remote program did not complete in the specified time
            pDFResource->m_pDFJob->SetStatus(DFJob::TIMEDOUT);
            pDFResource->m_Status = DF_RESOURCE_TIMEDOUT;
            Sleep (0);
#ifdef _DEBUG
            LogMessage (pDFResource->m_pDFJob, "Send");
#endif
            pALMWnd->PostMessage (DFLAUNCHER_POST_JOB_STATUS, 0,
                (LPARAM) pDFResource->m_pDFJob);
            pDFResource->m_ErrorFlag = FALSE;
            pDFResource->m_ErrorCount = 0;
            pDFResource->m_TimeoutCount++;
            break;

        case 0:  // The remote program completed without an error.
        case 2: // Temporary success as returned by Ataman version of rsh
            // The actual success or failure of the job is indetermined at
            // this time.
            pDFResource->m_Status = DF_RESOURCE_COMPLETED;
            pDFResource->m_pDFJob->SetStatus(DFJob::COMPLETED);
            Sleep (0);
#ifdef _DEBUG
            LogMessage (pDFResource->m_pDFJob, "Send");
#endif
            pALMWnd->PostMessage (DFLAUNCHER_POST_JOB_STATUS, 0,
                (LPARAM) pDFResource->m_pDFJob);
            pDFResource->m_ErrorFlag = FALSE;
            pDFResource->m_ErrorCount = 0;

            break;

        default: // The remote program execution failed due to some error such as
            // Invalid program name, invalid user, connection error etc. If the 
            // remote program is successfully executed and it either completes
            // normally or even if it crashes abnormally, the return status is 0
            // and the main app loop should handle the case of abnormal completion.
            pDFResource->m_Status = DF_RESOURCE_ERROR;
            pDFResource->m_JobCount--;
            pDFResource->m_ErrorCount ++;
            pDFResource->m_FinalErrorTime = COleDateTime::GetCurrentTime();
            if (pDFResource->m_ErrorFlag == FALSE)
            {
                pDFResource->m_ErrorFlag = TRUE;
                pDFResource->m_LastErrorTime = COleDateTime::GetCurrentTime();
            }
            Sleep (0);
            {
                int resTypeCount = 0;

                for (int i = 0; i < m_DFResourceList.GetSize(); i++)
                {
                    if (m_DFResourceList[i] &&
                        !((CDFResource *) (m_DFResourceList[i]))->m_JobIDString.
                            CompareNoCase(pDFResource->m_JobIDString))
                        resTypeCount++;
                }

                // If the local rsh program was terminated by the user or all the
                // available resources for the given job type has been tried then
                // inform the main app loop to retry the job at a later time.
                if (exitCode == CONTROL_C_EXIT ||
                    pDFResource->m_TryCount >= resTypeCount)
                {
                    pDFResource->m_pDFJob->SetStatus(DFJob::POSTPONED);
#ifdef _DEBUG
                    LogMessage (pDFResource->m_pDFJob, "Send");
#endif
                    pALMWnd->PostMessage (DFLAUNCHER_POST_JOB_STATUS, 0,
                        (LPARAM) pDFResource->m_pDFJob);

                }
                else
                {
                    // Retry the job on some other resource
                    COleDateTime tmpTime = COleDateTime::GetCurrentTime();
                    //COleDateTimeSpan tmpTimeSpan(0,0,0,3);
                    COleDateTimeSpan tmpTimeSpan(0,0,0,0);
                    DFJob * pDFJob = pDFResource->m_pDFJob;
                    int resCount = m_DFResourceList.GetSize();
                    m_resIndex = pDFResource->m_Index + 1;
                    if (m_resIndex >= resCount)
                        m_resIndex = 0;
                    for (int i = m_resIndex, loop = 0;
                        (i < resCount && loop < 2); i++)
                    {
	                    CDFResource * pNewDFResource = (CDFResource *) m_DFResourceList[i];
                        if (pNewDFResource &&
                            pNewDFResource->m_Status == DF_RESOURCE_FREE)
                        {
                            if (!pNewDFResource->m_JobIDString.CompareNoCase
                                (pDFJob->GetJobId()))
                            {
                                for (int j = 0; j < resCount; j++)
                                {
                                    CDFResource * pTmpDFResource = (CDFResource *) m_DFResourceList[j];

                                    if (pTmpDFResource && pTmpDFResource != pDFResource && 
                                        pTmpDFResource->m_Status != DF_RESOURCE_FREE &&
                                        pTmpDFResource->m_Status != DF_RESOURCE_DOWN &&
                                        !pTmpDFResource->m_IPAddress.CompareNoCase(pDFResource->m_IPAddress))
                                    {
                                        if (m_SingleJobPerMachine == TRUE)
                                            break;

                                        if ((tmpTime - pTmpDFResource->m_StartTime) < tmpTimeSpan)
                                        {
                                            break;
                                        }
                                    }
                                }
                                if (j >= resCount)
                                {
                                    pNewDFResource->m_Status = DF_RESOURCE_BUSY;
                                    pNewDFResource->m_pDFJob = pDFJob;
                                    pNewDFResource->m_hProcess = NULL;
                                    pNewDFResource->m_pWinThread =
                                        AfxBeginThread (StartRSH, pNewDFResource);	
                                    pNewDFResource->m_StartTime =
                                        COleDateTime::GetCurrentTime();
                                    GetSystemTimeAsFileTime (&(pNewDFResource->m_StartFileTime));
                                    pNewDFResource->m_TryCount =
                                        pDFResource->m_TryCount + 1;
                                    pNewDFResource->m_JobCount++;
                                    m_resIndex = i+1;
                                    break;
                                }
                            }
                        }
                        if (i == (resCount - 1))
                        {
                            i = -1;
                            loop ++;
                        }
                    }

                    if (i >= resCount || loop >= 2)
                    {
                        pDFJob->SetStatus(DFJob::POSTPONED);
#ifdef _DEBUG
                        LogMessage (pDFResource->m_pDFJob, "Send");
#endif
                        pALMWnd->PostMessage (DFLAUNCHER_POST_JOB_STATUS, 0,
                            (LPARAM) pDFResource->m_pDFJob);
                    }
                }
            }
            break;
        }
    }
    else
    if (pDFResource->m_Status == DF_RESOURCE_IGNORE)
    {
        pDFResource->m_pDFIgnoreJob->SetStatus(DFJob::IGNORED);
        CDFLauncherApp * theApp = (CDFLauncherApp *) AfxGetApp();
        CWnd * pALMWnd = CWnd::FromHandle(theApp->m_hALMWnd);
#ifdef _DEBUG
        LogMessage (pDFResource->m_pDFIgnoreJob, "Send");
#endif
        pALMWnd->PostMessage (DFLAUNCHER_POST_JOB_STATUS, 0, (LPARAM) pDFResource->m_pDFIgnoreJob);
    }

    pDFResource->m_Status = DF_RESOURCE_FREE;
    pDFResource->m_BusyTime += COleDateTime::GetCurrentTime() - pDFResource->m_StartTime;
    pDFResource->m_pDFJob = NULL;
    pDFResource->m_pDFIgnoreJob = NULL;

    CListCtrl& ListCtrl=GetListCtrl();
    if (m_AutoDisableCount && pDFResource->m_ErrorCount >= m_AutoDisableCount)
    {
        pDFResource->m_Status = DF_RESOURCE_DOWN;
        ListCtrl.SetRedraw(FALSE);
        ListCtrl.SortItems(CompareFunc, (LPARAM) this);
        ListCtrl.SetRedraw(TRUE);

        class CAVISSendMail avisSendMail;
        CString msgStr;
        COleDateTime curTime = COleDateTime::GetCurrentTime();

        msgStr.Format ("Disabling resource <%s> on machine <%s> at <%s>.\n"
                       "The resource is having errors since <%s>.\n"
                       "<%d> error attempts have been made on the resource.\n" ,
            pDFResource->m_JobIDString,
            pDFResource->m_IPAddress,
            curTime.Format("%m/%d/%Y %H:%M:%S"),
            pDFResource->m_LastErrorTime.Format("%m/%d/%Y %H:%M:%S"),
            pDFResource->m_ErrorCount) ;

        BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, msgStr);
        /*
        if (brc != TRUE)
        {
            AfxMessageBox ("Error in sending mail.\n");
        }
        else
        {
    //        AfxMessageBox ("Mail send successfully.\n");
        }
        */
    }
    // Process delayed disabling of the resource
    if (pDFResource->m_MarkedForDisable == TRUE)
    {
        pDFResource->m_MarkedForDisable = FALSE;
        pDFResource->m_Status = DF_RESOURCE_DOWN;
        ListCtrl.SetRedraw(FALSE);
        ListCtrl.SortItems(CompareFunc, (LPARAM) this);
        ListCtrl.SetRedraw(TRUE);
    }
    return 1;
}

// Pass the notification message to the ListViewEx Class to handle the header
// resize message for the list control.

BOOL CDFLauncherView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	return CListViewEx::OnNotify(wParam, lParam, pResult);
}

void CDFLauncherView::OnActionsAdd() 
{
    CResourceDialog resourceDialog;
    CListCtrl& ListCtrl=GetListCtrl();
    int rc;

    resourceDialog.m_pDFResourceList = &m_DFResourceList;
    resourceDialog.m_DialogType = ADD_RESOURCE;

    rc = resourceDialog.DoModal();

    if (rc == IDOK)
    {
        if (!resourceDialog.m_JobID.IsEmpty() &&
            !resourceDialog.m_Machine.IsEmpty() &&
            !resourceDialog.m_Program.IsEmpty() &&
            !resourceDialog.m_Timeout.IsEmpty() &&
            !resourceDialog.m_User.IsEmpty() )
        {
            CDFResource * pDFResource = new CDFResource();
            int timeoutVal;

            int itemCount = ListCtrl.GetItemCount();
            int i;

            // If a resource is added then make sure that all resources on the
            // specified machine has the same NT Machine name and userid.
            for (i = 0; i < itemCount; i++)
            {
                CDFResource * pDFResource = (CDFResource *) ListCtrl.GetItemData(i);

                if (pDFResource &&
                    !pDFResource->m_IPAddress.CompareNoCase(resourceDialog.m_Machine))
                {
                    if (pDFResource->m_NTMachineName.CompareNoCase(resourceDialog.m_ntMachineName))
                    {
                        pDFResource->m_NTMachineName = resourceDialog.m_ntMachineName;
                        pDFResource->m_Saved = FALSE;
                        m_ResourceModified = TRUE;
                    }
                    if (pDFResource->m_UserName.CompareNoCase(resourceDialog.m_User))
                    {
                        pDFResource->m_UserName = resourceDialog.m_User;
                        pDFResource->m_Saved = FALSE;
                        m_ResourceModified = TRUE;
                    }
                }
            }

            pDFResource->m_JobIDString = resourceDialog.m_JobID;
            pDFResource->m_IPAddress = resourceDialog.m_Machine;
            pDFResource->m_NTMachineName = resourceDialog.m_ntMachineName;
            pDFResource->m_UserName = resourceDialog.m_User;
            pDFResource->m_ProgramName = resourceDialog.m_Program;

            timeoutVal = atoi (resourceDialog.m_Timeout);
            COleDateTimeSpan tmpPeriod(0,0,0,timeoutVal);
            pDFResource->m_TimeoutPeriod = tmpPeriod;

            if (FALSE == IsResourceTypePresent(pDFResource))
            {
                pDFResource->m_Index = m_DFResourceList.Add (pDFResource);
                RemoveDuplicateResources();


                LV_ITEM lvi;
                int nItem;

                lvi.mask=LVIF_TEXT | LVIF_STATE;
                lvi.iItem=ListCtrl.GetItemCount();
                lvi.iSubItem=0;
                lvi.pszText=pDFResource->m_JobIDString.GetBuffer(1024);
                lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
                lvi.state = 0;

                nItem = ListCtrl.InsertItem(&lvi);
                ListCtrl.SetItemData(nItem, (DWORD) pDFResource);

                pDFResource->m_Saved = FALSE;
                m_ResourceModified = TRUE;
            }
            else
            {
                CString msgString;

                msgString.Format("Resource not added.\nOnly one NT and one AIX machine are allowed.");
                AfxMessageBox(msgString, MB_ICONSTOP | MB_OK);
                delete pDFResource;
            }
            ListCtrl.SetRedraw(FALSE);
            ListCtrl.SortItems(CompareFunc, (LPARAM) this);
            ListCtrl.SetRedraw(TRUE);
        }
    }
}

void CDFLauncherView::OnUpdateActionsAdd(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable (TRUE);
}

void CDFLauncherView::OnActionsCopy() 
{
    CResourceDialog resourceDialog;
    int rc;
    CListCtrl& ListCtrl=GetListCtrl();

    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    resourceDialog.m_pDFResourceList = &m_DFResourceList;
    resourceDialog.m_DialogType = COPY_RESOURCE;

    if (selectedResource != NULL)
    {
        resourceDialog.m_Machine = selectedResource->m_IPAddress;
        resourceDialog.m_ntMachineName = selectedResource->m_NTMachineName;
        resourceDialog.m_Program = selectedResource->m_ProgramName;
        resourceDialog.m_User = selectedResource->m_UserName;
        resourceDialog.m_JobID = selectedResource->m_JobIDString;
        resourceDialog.m_Timeout.Format("%ld",
            (long) selectedResource->m_TimeoutPeriod.GetTotalSeconds());
    }

    rc = resourceDialog.DoModal();

    if (rc == IDOK)
    {
        if (!resourceDialog.m_JobID.IsEmpty() &&
            !resourceDialog.m_Machine.IsEmpty() &&
            !resourceDialog.m_Program.IsEmpty() &&
            !resourceDialog.m_Timeout.IsEmpty() &&
            !resourceDialog.m_User.IsEmpty() )
        {
            CDFResource * pDFResource = new CDFResource();
            int timeoutVal;
            int itemCount = ListCtrl.GetItemCount();
            int i;

            // If a resource is added then make sure that all resources on the
            // specified machine has the same NT Machine name and userid.
            for (i = 0; i < itemCount; i++)
            {
                CDFResource * pDFResource = (CDFResource *) ListCtrl.GetItemData(i);

                if (pDFResource &&
                    !pDFResource->m_IPAddress.CompareNoCase(resourceDialog.m_Machine))
                {
                    if (pDFResource->m_NTMachineName.CompareNoCase(resourceDialog.m_ntMachineName))
                    {
                        pDFResource->m_NTMachineName = resourceDialog.m_ntMachineName;
                        pDFResource->m_Saved = FALSE;
                        m_ResourceModified = TRUE;
                    }
                    if (pDFResource->m_UserName.CompareNoCase(resourceDialog.m_User))
                    {
                        pDFResource->m_UserName = resourceDialog.m_User;
                        pDFResource->m_Saved = FALSE;
                        m_ResourceModified = TRUE;
                    }
                }
            }

            pDFResource->m_JobIDString = resourceDialog.m_JobID;
            pDFResource->m_IPAddress = resourceDialog.m_Machine;
            pDFResource->m_NTMachineName = resourceDialog.m_ntMachineName;
            pDFResource->m_UserName = resourceDialog.m_User;
            pDFResource->m_ProgramName = resourceDialog.m_Program;

            timeoutVal = atoi (resourceDialog.m_Timeout);
            COleDateTimeSpan tmpPeriod(0,0,0,timeoutVal);
            pDFResource->m_TimeoutPeriod = tmpPeriod;

            if (FALSE == IsResourceTypePresent(pDFResource))
            {
                pDFResource->m_Index = m_DFResourceList.Add (pDFResource);
                RemoveDuplicateResources();


                LV_ITEM lvi;
                int nItem;

                lvi.mask=LVIF_TEXT | LVIF_STATE;
                lvi.iItem=ListCtrl.GetItemCount();
                lvi.iSubItem=0;
                lvi.pszText=pDFResource->m_JobIDString.GetBuffer(1024);
                lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
                lvi.state = 0;

                nItem = ListCtrl.InsertItem(&lvi);
                ListCtrl.SetItemData(nItem, (DWORD) pDFResource);

                pDFResource->m_Saved = FALSE;
                m_ResourceModified = TRUE;
            }
            else
            {
                CString msgString;

                msgString.Format("Resource not added.\nOnly one NT and one AIX machine are allowed.");
                AfxMessageBox(msgString, MB_ICONSTOP | MB_OK);
                delete pDFResource;
            }
            ListCtrl.SetRedraw(FALSE);
            ListCtrl.SortItems(CompareFunc, (LPARAM) this);
            ListCtrl.SetRedraw(TRUE);
        }
    }
}

void CDFLauncherView::OnUpdateActionsCopy(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    if (selectedResource != NULL)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnActionsDisable() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    if (selectedResource->m_Status == DF_RESOURCE_DOWN)
        return;

    if (selectedResource != NULL)
    {
        // If the resource is curently busy then it is just marked for disable and
        // as soon as the current job is finished on the resource, it is disabled.
        if (selectedResource->m_Status == DF_RESOURCE_FREE)
            selectedResource->m_Status = DF_RESOURCE_DOWN;
        else
            selectedResource->m_MarkedForDisable = TRUE;

        ListCtrl.SetRedraw(FALSE);
        ListCtrl.SortItems(CompareFunc, (LPARAM) this);
        ListCtrl.SetRedraw(TRUE);
    }
}

void CDFLauncherView::OnUpdateActionsDisable(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    if (selectedResource != NULL && selectedResource->m_Status != DF_RESOURCE_DOWN)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnActionsEnable() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    if (selectedResource != NULL)
    {
        if (selectedResource->m_Status == DF_RESOURCE_DOWN)
            selectedResource->m_Status = DF_RESOURCE_FREE;
        selectedResource->m_MarkedForDisable = FALSE;

        ListCtrl.SetRedraw(FALSE);
        ListCtrl.SortItems(CompareFunc, (LPARAM) this);
        ListCtrl.SetRedraw(TRUE);
    }
}

void CDFLauncherView::OnUpdateActionsEnable(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    if (selectedResource != NULL && selectedResource->m_Status == DF_RESOURCE_DOWN)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnActionsModify() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    CResourceDialog resourceDialog;
    int rc;

    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();


    if (selectedResource != NULL && selectedResource->m_Status == DF_RESOURCE_DOWN)
    {
        resourceDialog.m_pDFResourceList = &m_DFResourceList;
        resourceDialog.m_DialogType = MODIFY_RESOURCE;
        resourceDialog.m_Machine = selectedResource->m_IPAddress;
        resourceDialog.m_ntMachineName = selectedResource->m_NTMachineName;
        resourceDialog.m_Program = selectedResource->m_ProgramName;
        resourceDialog.m_User = selectedResource->m_UserName;
        resourceDialog.m_JobID = selectedResource->m_JobIDString;
        resourceDialog.m_Timeout.Format("%ld",
            (long) selectedResource->m_TimeoutPeriod.GetTotalSeconds());
        resourceDialog.m_ErrorCount = selectedResource->m_ErrorCount;
        resourceDialog.m_TimeoutCount = selectedResource->m_TimeoutCount;
        rc = resourceDialog.DoModal();
        if (rc != IDOK)
            return;;
    }

    if ( selectedResource != NULL && selectedResource->m_Status == DF_RESOURCE_DOWN)
    {
        if (!resourceDialog.m_JobID.IsEmpty() &&
            !resourceDialog.m_Machine.IsEmpty() &&
            !resourceDialog.m_Program.IsEmpty() &&
            !resourceDialog.m_Timeout.IsEmpty() &&
            !resourceDialog.m_User.IsEmpty() )
        {
            int timeoutVal;
            int itemCount = ListCtrl.GetItemCount();
            int i;

            // If a resource is modified then make sure that all resources on the
            // specified machine has the same NT Machine name and userid.
            
            for (i = 0; i < itemCount; i++)
            {
                CDFResource * pDFResource = (CDFResource *) ListCtrl.GetItemData(i);

                if (pDFResource &&
                    pDFResource != selectedResource &&
                    !pDFResource->m_IPAddress.CompareNoCase(resourceDialog.m_Machine))
                {
                    if (pDFResource->m_NTMachineName.CompareNoCase(resourceDialog.m_ntMachineName))
                    {
                        pDFResource->m_NTMachineName = resourceDialog.m_ntMachineName;
                        pDFResource->m_Saved = FALSE;
                        m_ResourceModified = TRUE;
                    }
                    if (pDFResource->m_UserName.CompareNoCase(resourceDialog.m_User))
                    {
                        pDFResource->m_UserName = resourceDialog.m_User;
                        pDFResource->m_Saved = FALSE;
                        m_ResourceModified = TRUE;
                    }
                }
            }
            

            CDFResource DFResource;
            DFResource.m_IPAddress = resourceDialog.m_Machine;
            DFResource.m_NTMachineName = resourceDialog.m_ntMachineName;
            DFResource.m_UserName = resourceDialog.m_User;
            DFResource.m_ProgramName = resourceDialog.m_Program;

            if (FALSE == IsResourceTypePresent(&DFResource))
            {
                if (selectedResource->m_IPAddress != resourceDialog.m_Machine)
                {
                    selectedResource->m_IPAddress = resourceDialog.m_Machine;
                    selectedResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }
                if (selectedResource->m_NTMachineName != resourceDialog.m_ntMachineName)
                {
                    selectedResource->m_NTMachineName = resourceDialog.m_ntMachineName;
                    selectedResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }
                if (selectedResource->m_UserName != resourceDialog.m_User)
                {
                    selectedResource->m_UserName = resourceDialog.m_User;
                    selectedResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }
                if (selectedResource->m_ProgramName != resourceDialog.m_Program)
                {
                    selectedResource->m_ProgramName = resourceDialog.m_Program;
                    selectedResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }
                timeoutVal = atoi (resourceDialog.m_Timeout);
                COleDateTimeSpan tmpPeriod(0,0,0,timeoutVal);
                if (selectedResource->m_TimeoutPeriod != tmpPeriod)
                {
                    selectedResource->m_TimeoutPeriod = tmpPeriod;
                    selectedResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }

                selectedResource->m_ErrorCount = resourceDialog.m_ErrorCount;
                if (!selectedResource->m_ErrorCount)
                    selectedResource->m_ErrorFlag = FALSE;
                selectedResource->m_TimeoutCount = resourceDialog.m_TimeoutCount;
                RemoveDuplicateResources();

            }
            else
            {
                CString msgString;

                msgString.Format("Resource not added.\nOnly one NT and one AIX machine are allowed.");
                AfxMessageBox(msgString, MB_ICONSTOP | MB_OK);
            }
            ListCtrl.SetRedraw(FALSE);
            ListCtrl.SortItems(CompareFunc, (LPARAM) this);
            ListCtrl.SetRedraw(TRUE);
        }
    }
}

void CDFLauncherView::OnUpdateActionsModify(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    if (selectedResource != NULL && selectedResource->m_Status == DF_RESOURCE_DOWN)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnActionsRemove() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();
    if (selectedResource && selectedResource->m_Status == DF_RESOURCE_DOWN)
    {
        int rc;
        CString msgStr;

        msgStr.GetBuffer(1024);
        msgStr.Format ("Remove Resource <%s> on machine <%s>.\n\nAre You Sure ?\n",
            (LPCTSTR) selectedResource->m_JobIDString,
            (LPCTSTR) selectedResource->m_IPAddress);

        rc = AfxMessageBox (msgStr, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
        if (rc != IDYES)
            return;;
    }

    if (selectedResource && selectedResource->m_Status == DF_RESOURCE_DOWN)
    {
        int resCount = m_DFResourceList.GetSize();
        int itemCount = ListCtrl.GetItemCount();
        int i;

        for (i = 0; i < resCount; i++)
        {
            if (selectedResource == (CDFResource *) m_DFResourceList[i])
            {
                m_DFResourceList.RemoveAt(i);
                break;
            }
        }

        for (i = 0; i < itemCount; i++)
        {
            if (selectedResource == (CDFResource *) ListCtrl.GetItemData(i))
            {
                ListCtrl.DeleteItem(i);
                break;
            }
        }

        delete selectedResource;
        m_ResourceModified = TRUE;
    }
}

void CDFLauncherView::OnUpdateActionsRemove(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    
    selectedResource = GetCurrentSelection();

    if (selectedResource != NULL && selectedResource->m_Status == DF_RESOURCE_DOWN)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnActionsRemoveall() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    int resCount = m_DFResourceList.GetSize();
    int i;
    int rc;

    rc = AfxMessageBox ("Remove All Resources.\n\nAre You Sure ?\n",
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (rc != IDYES)
        return;

    for (i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource &&
            pDFResource->m_Status != DF_RESOURCE_FREE &&
            pDFResource->m_Status != DF_RESOURCE_DOWN)
            break;
    }

    if (i < resCount)
    {
        AfxMessageBox ("Cannot remove resources as some resource is busy\n");
        return;
    }


    for (i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource)
            delete pDFResource;
    }
    m_DFResourceList.RemoveAll();
    m_DFResourceList.SetSize (0, 10);
    ListCtrl.DeleteAllItems();
    m_ResourceModified = TRUE;
}

void CDFLauncherView::OnUpdateActionsRemoveall(CCmdUI* pCmdUI) 
{
    int resCount = m_DFResourceList.GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource &&
            pDFResource->m_Status != DF_RESOURCE_DOWN )
            break;
    }
    if (!resCount || i < resCount)
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
}

void CDFLauncherView::OnResourcesSave() 
{
    CString bakResFile ((LPCTSTR) IDS_BAK_RESOURCE_FILE_NAME);
    CString resFile ((LPCTSTR) IDS_RESOURCE_FILE_NAME);
    CString bakResourceFile = m_ModulePath + bakResFile;
    CString resourceFile = m_ModulePath + resFile;
    CListCtrl& ListCtrl=GetListCtrl();
    BOOL saveFile = TRUE;

    int rc;

    rc = AfxMessageBox ("Save resources to file on disk.\n\nAre You Sure ?\n",
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (rc != IDYES)
        return;

//    unlink (bakResourceFile);

//    rc = rename (resourceFile, bakResourceFile);
	rc = CopyFile(resourceFile, bakResourceFile, FALSE);

    if (!rc)
    {
        if (errno != ENOENT)
        {
            rc = AfxMessageBox ("Failed to create backup file.\n\nContinue saving file ?\n",
                MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

            if (rc != IDYES)
                saveFile = FALSE;
            else
                saveFile = TRUE;
        }
        else
            saveFile = TRUE;
    }

    if (saveFile == TRUE)
    {
        int itemCount = ListCtrl.GetItemCount();
        int i;
        FILE *fp;

        fp = fopen (resourceFile, _T("w"));

        if (fp)
        {
            CString serviceTag((LPCTSTR) IDS_SERVICE_TAG);
            CString machineTag((LPCTSTR) IDS_MACHINE_TAG);
            CString userTag((LPCTSTR) IDS_USER_TAG);
            CString timeoutTag((LPCTSTR) IDS_TIMEOUT_TAG);
            CString programTag((LPCTSTR) IDS_PROGRAM_TAG);
            CString ntmachineTag((LPCTSTR) IDS_NTMACHINENAME_TAG);
			CString prologForListFile((LPCSTR) IDS_LSTFILE_PROLOG);

			//Added on May/17/2000. Write the prolog for the list file.
			fwrite ((LPCSTR )prologForListFile, prologForListFile.GetLength(), 1, fp);


            m_ResourceModified = FALSE;

            for ( i = 0; i < itemCount; i++)
            {
        	    CDFResource * pDFResource = (CDFResource *) ListCtrl.GetItemData(i);
                pDFResource->m_Saved = FALSE;
            }

            for ( i = 0; i < itemCount; i++)
            {
                CParamValue paramValue;
        	    CDFResource * pDFResource = (CDFResource *) ListCtrl.GetItemData(i);
                if (pDFResource)
                {
                    BOOL brc;

                    paramValue.AddParamValue (serviceTag, pDFResource->m_JobIDString);
                    paramValue.AddParamValue (machineTag, pDFResource->m_IPAddress);
                    if (!pDFResource->m_NTMachineName.IsEmpty())
                    {
                        paramValue.AddParamValue (ntmachineTag, pDFResource->m_NTMachineName);
                    }
                    paramValue.AddParamValue (userTag, pDFResource->m_UserName);

                    CString tmpStr;
                    tmpStr.Format ("%ld", 
                        (long) pDFResource->m_TimeoutPeriod.GetTotalSeconds());
                    paramValue.AddParamValue (timeoutTag, tmpStr);
                    paramValue.AddParamValue (programTag, pDFResource->m_ProgramName);

                    brc = paramValue.WriteParamValue (fp);

                    if (brc == FALSE)
                    {
                        AfxMessageBox ("Error in creating resource file.");
                        m_ResourceModified = TRUE;
                        break;
                    }
                    else
                    {
                        pDFResource->m_Saved = TRUE;
                    }
                }
            }
            fclose (fp);
        }
        else
        {
            AfxMessageBox ("Error in creating resource file.");
            m_ResourceModified = TRUE;
        }
    }
}

CDFResource * CDFLauncherView::GetCurrentSelection(void)
{
    CListCtrl& ListCtrl=GetListCtrl();
	CDFResource * SelectedResource = NULL;
    int i;

    for (i = 0; i < ListCtrl.GetItemCount(); i++)
    {
        LV_ITEM lvi;

        lvi.mask = LVIF_STATE;
        lvi.iItem = i;
        lvi.iSubItem = 0;
        lvi.pszText = 0;
        lvi.cchTextMax = 0;
        lvi.stateMask = 0xFFFF;

        ListCtrl.GetItem(&lvi);

        if (lvi.state & (LVIS_SELECTED | LVIS_FOCUSED) )
		{
			SelectedResource = (CDFResource *) ListCtrl.GetItemData(i);
			break;
		}
    }
	return (SelectedResource);
}

void CDFLauncherView::OnResourcesLoad() 
{
    int rc;

    rc = AfxMessageBox ("Reload resources from file saved on disk.\n\nAre You Sure ?\n",
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (rc != IDYES)
        return;

    m_ResourceModified = FALSE;
    OnALMReqReloadResources(0, 0);
}

void CDFLauncherView::OnUpdateResourcesLoad(CCmdUI* pCmdUI) 
{
    int resCount = m_DFResourceList.GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource && pDFResource->m_Status != DF_RESOURCE_FREE &&
            pDFResource->m_Status != DF_RESOURCE_DOWN)
            break;
    }
    if (i < resCount)
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
}

void CDFLauncherView::OnUpdateResourcesSave(CCmdUI* pCmdUI) 
{
    pCmdUI->Enable(TRUE);
}

void CDFLauncherView::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult) 
{
    CListCtrl& ListCtrl=GetListCtrl();
    HD_NOTIFY *phdn = (HD_NOTIFY *) pNMHDR;

    if( phdn->iButton == 0 )
    {
        // User clicked on header using left mouse button
        if( phdn->iItem == m_SortedColumn )
        {
            switch (m_SortOrder)
            {
            case SORT_ASCENDING:
                m_SortOrder = SORT_DESCENDING;
                break;

            case SORT_DESCENDING:
            default:
                m_SortOrder = SORT_ASCENDING;
                break;

            }
        }
        else
        {
            m_SortOrder = SORT_ASCENDING;
        }

        m_SortedColumn = phdn->iItem;
        if (m_SortedColumn == 0 || m_SortedColumn == 1 || m_SortedColumn == 5)
        {
            ListCtrl.SetRedraw(FALSE);
            ListCtrl.SortItems(CompareFunc, (LPARAM) this);
            ListCtrl.SetRedraw(TRUE);
        }
    }
    *pResult = 0;
}

int CDFLauncherView::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CDFLauncherView * pDFLauncherView = (CDFLauncherView *) lParamSort;
    CDFResource * pDFResource1 = (CDFResource *) lParam1;
    CDFResource * pDFResource2 = (CDFResource *) lParam2;

    if (pDFLauncherView && pDFResource1 && pDFResource2)
    {
        int rc = 0;

        switch (pDFLauncherView->m_SortOrder)
        {
        case SORT_DESCENDING:
            if (pDFLauncherView->m_SortedColumn == 0)
            {
                rc = pDFResource2->m_JobIDString.CompareNoCase(pDFResource1->m_JobIDString);
            }
            else
            {
                if (pDFLauncherView->m_SortedColumn == 1)
                    rc = pDFResource2->m_IPAddress.CompareNoCase(pDFResource1->m_IPAddress);
                else
                {
                    rc = 0;
                    if (pDFResource2->m_Status < pDFResource1->m_Status)
                        rc = -1;
                    if (pDFResource2->m_Status > pDFResource1->m_Status)
                        rc = 1;
                }
            }

            return rc;
            break;

        default:
        case SORT_ASCENDING:
            if (pDFLauncherView->m_SortedColumn == 0)
            {
                rc = pDFResource1->m_JobIDString.CompareNoCase(pDFResource2->m_JobIDString);
            }
            else
            {
                if (pDFLauncherView->m_SortedColumn == 1)
                    rc = pDFResource1->m_IPAddress.CompareNoCase(pDFResource2->m_IPAddress);
                else
                {
                    rc = 0;
                    if (pDFResource1->m_Status < pDFResource2->m_Status)
                        rc = -1;
                    if (pDFResource1->m_Status > pDFResource2->m_Status)
                        rc = 1;
                }
            }

            return rc;
            break;
        }
    }
    return 0;
}

void CDFLauncherView::OnViewSummary() 
{
    m_pResourceSummaryDlg = new (CResourceSummary);
    int resCount = m_DFResourceList.GetSize();

    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource)
        {
            int jobIDCount = m_pResourceSummaryDlg->m_JobIDArray.GetSize();
            for (int j = 0; j < jobIDCount; j++)
            {
                if (!pDFResource->m_JobIDString.CompareNoCase(m_pResourceSummaryDlg->m_JobIDArray[j]))
                    break;
            }
            if (j >= jobIDCount)
            {
                j = m_pResourceSummaryDlg->m_JobIDArray.Add(pDFResource->m_JobIDString);
                m_pResourceSummaryDlg->m_TotalCountArray.Add(0);
                m_pResourceSummaryDlg->m_FreeCountArray.Add(0);
                m_pResourceSummaryDlg->m_AvailableCountArray.Add(0);
                m_pResourceSummaryDlg->m_TotalJobDoneArray.Add(0);
            }
            m_pResourceSummaryDlg->m_TotalCountArray[j] ++;
            if (pDFResource->m_Status != DF_RESOURCE_DOWN)
                m_pResourceSummaryDlg->m_AvailableCountArray[j] ++;
            if (pDFResource->m_Status == DF_RESOURCE_FREE)
                m_pResourceSummaryDlg->m_FreeCountArray[j] ++;
            m_pResourceSummaryDlg->m_TotalJobDoneArray[j] += pDFResource->m_JobCount;
        }
    }
    m_pResourceSummaryDlg->DoModal();
    if (g_TerminateDLL != TRUE)
    {
        delete (m_pResourceSummaryDlg);
        m_pResourceSummaryDlg = NULL;
    }
    else
    {
        if (g_pResourceSummaryDlg)
            delete (g_pResourceSummaryDlg);
    }
}

#ifdef _DEBUG
void CDFLauncherView::LogMessage(DFJob * pDFJob, CString msg)
{
    FILE *fp;
    COleDateTime curTime = COleDateTime::GetCurrentTime();
    CString timeStr;
    CString msgStr;

    timeStr = curTime.Format("%m/%d/%Y %H:%M:%S");

    fp = fopen (m_ModulePath + "AVISDFRL.Log", "ac");
    if (fp)
    {
        if (pDFJob)
        {
            CString jobStatus;

            switch (pDFJob->GetStatus())
            {
            case DFJob::UNDEFINED:
                jobStatus = "UNDEFINED";
                break;

            case DFJob::ACCEPTED:
                jobStatus = "ACCEPTED";
                break;

            case DFJob::SUBMITTED:
                jobStatus = "SUBMITTED";
                break;

            case DFJob::POSTPONED:
                jobStatus = "POSTPONED";
                break;

            case DFJob::SUCCESS:
                jobStatus = "SUCCESS";
                break;

            case DFJob::FAIL:
                jobStatus = "FAIL";
                break;

            case DFJob::TIMEDOUT:
                jobStatus = "TIMEDOUT";
                break;

            case DFJob::IGNORED:
                jobStatus = "IGNORED";
                break;

            case DFJob::COMPLETED:
                jobStatus = "COMPLETED";
                break;

            default:
                jobStatus = "UNKNOWN";
                break;

            }
            fprintf (fp, "%s: \"%s\" <%s> <%s> <%d> <%s>\n", timeStr, msg, pDFJob->GetJobId(), pDFJob->GetCookieId(), pDFJob->GetJobSeq(), jobStatus);
            msgStr.Format ("%s: \"%s\" <%s> <%s> <%d> <%s>\n", timeStr, msg, pDFJob->GetJobId(), pDFJob->GetCookieId(), pDFJob->GetJobSeq(), jobStatus);
            OutputDebugString (msgStr);
        }
        else
        {
            fprintf (fp, "%s: \"%s\"\n", timeStr, msg);
            msgStr.Format ("%s: \"%s\"\n", timeStr, msg);
            OutputDebugString (msgStr);
        }
        fflush (fp);
        fclose (fp);
    }
}
#endif

void CDFLauncherView::OnViewMachinestatistics() 
{
    ShowMachineStatistics(SHOW_DIALOG);
}

void CDFLauncherView::OnUpdateViewMachinestatistics(CCmdUI* pCmdUI) 
{
    int resCount = m_DFResourceList.GetSize();
	
    if (resCount)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnUpdateViewSummary(CCmdUI* pCmdUI) 
{
    int resCount = m_DFResourceList.GetSize();

    if (resCount)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

UINT SaveMachineStatToFile(LPVOID ptr)
{
    WritingStatsToFile = 1;
    CMachineStatistics * machineStatistics = (CMachineStatistics *) ptr;
    machineStatistics->SaveToFile(g_ModulePath + "ISMachineStat.lst");
    delete machineStatistics;
    WritingStatsToFile = 0;
    return 0;
}

void CDFLauncherView::ShowMachineStatistics(int showType)
{
    int resCount = m_DFResourceList.GetSize();
    if (resCount)
    {
        CMachineStatistics * machineStatistics = new CMachineStatistics();

        if (m_ComputerName[0])
        {
            machineStatistics->m_MachineList.Add(m_ComputerName);
            machineStatistics->m_UserList.Add("Local");
            machineStatistics->m_NTMachineNameList.Add(m_ComputerName);
        }
        else
        {
            machineStatistics->m_MachineList.Add("Local");
            machineStatistics->m_UserList.Add("Local");
            machineStatistics->m_NTMachineNameList.Add("LOCAL");
        }

        for (int i = 0; i < resCount; i++)
        {
    	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            int machineCount = machineStatistics->m_MachineList.GetSize();

            if (pDFResource)
            {
                for (int j = 0; j < machineCount; j++)
                {
                    if (!machineStatistics->m_MachineList[j].CompareNoCase(pDFResource->m_IPAddress))
                    {
                        break;
                    }
                    if (!machineStatistics->m_NTMachineNameList[j].IsEmpty() &&
                        !machineStatistics->m_NTMachineNameList[j].CompareNoCase(pDFResource->m_NTMachineName))
                    {
                        break;
                    }
                }
                if (j >= machineCount)
                {
                    machineStatistics->m_MachineList.Add(pDFResource->m_IPAddress);
                    machineStatistics->m_UserList.Add(pDFResource->m_UserName);
                    machineStatistics->m_NTMachineNameList.Add(pDFResource->m_NTMachineName);
                }
            }
        }

        if (showType == SHOW_DIALOG)
        {
            machineStatistics->DoModal();
            //AfxBeginThread ( RUNTIME_CLASS(CMStatDialogThread) );
            delete machineStatistics;
        }
        if (showType == SAVE_TO_FILE)
        {
            m_StatThread = AfxBeginThread(SaveMachineStatToFile, machineStatistics);
        }
    }
}

void CDFLauncherView::OnActionsDisableall() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    int resCount = m_DFResourceList.GetSize();
    int i;

    for (i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource)
        {
            if (pDFResource->m_Status == DF_RESOURCE_FREE)
                pDFResource->m_Status = DF_RESOURCE_DOWN;
            else
                pDFResource->m_MarkedForDisable = TRUE;
        }
    }
    ListCtrl.SetRedraw(FALSE);
    ListCtrl.SortItems(CompareFunc, (LPARAM) this);
    ListCtrl.SetRedraw(TRUE);
}

void CDFLauncherView::OnActionsEnableall() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    int resCount = m_DFResourceList.GetSize();
    int i;

    for (i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource)
        {
            if (pDFResource->m_Status == DF_RESOURCE_DOWN)
                pDFResource->m_Status = DF_RESOURCE_FREE;
            pDFResource->m_MarkedForDisable = FALSE;
        }
    }
    ListCtrl.SetRedraw(FALSE);
    ListCtrl.SortItems(CompareFunc, (LPARAM) this);
    ListCtrl.SetRedraw(TRUE);
}

void CDFLauncherView::OnUpdateActionsDisableall(CCmdUI* pCmdUI) 
{
    int resCount = m_DFResourceList.GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource &&
            pDFResource->m_Status != DF_RESOURCE_DOWN )
            break;
    }
    if (i < resCount)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnUpdateActionsEnableall(CCmdUI* pCmdUI) 
{
    int resCount = m_DFResourceList.GetSize();
    for (int i = 0; i < resCount; i++)
    {
	    CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
        if (pDFResource &&
            pDFResource->m_Status == DF_RESOURCE_DOWN )
            break;
    }
    if (i < resCount)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::LogCompletionMessage(CDFResource *pDFResource, LONG exitCode)
{
    CMutex Mutex(FALSE, "ISDFLauncherLogCompletionMessage");
    CSingleLock sLock (&Mutex, TRUE);

    if (!m_LogFile)
        return;

    CString status;
    DFJob * pDFJob;

    if (pDFResource->m_Status == DF_RESOURCE_BUSY)
    {
        pDFJob = pDFResource->m_pDFJob;
        switch (exitCode)
        {
        case TERMINATE_TIMEOUT: // The remote program did not complete in the specified time
            status = "Timeout";
            break;

        case 0:  // The remote program completed without an error.
            status = "Success";
            break;

        default: // The remote program execution failed due to some error such as
            status = "Error";
            break;
        }
    }
    else
    {
        status = "Ignored";
        pDFJob = pDFResource->m_pDFIgnoreJob;
    }

    if (m_LogFileSize >= m_MaxLogFileSize)
    {
        CString bakLogFileName = m_ModulePath + m_LogFileName + ".Bak";
        fflush (m_LogFile);
        fclose (m_LogFile);
        m_LogFile = (FILE *) NULL;
//        unlink (bakLogFileName);

//        int rc = rename (m_LogFileName, bakLogFileName);

		int rc = CopyFile(m_LogFileName, bakLogFileName, FALSE);

        if (rc)
            m_LogFile = fopen (m_ModulePath + m_LogFileName, "w");

        if (m_LogFile)
        {
            if (m_LogBufferSize)
                setvbuf (m_LogFile, NULL, _IOFBF, m_LogBufferSize);
            fseek (m_LogFile, 0, SEEK_END);
            m_LogFileSize = ftell(m_LogFile);
        }
    }

    if (m_LogFile)
    {
        CString logString;

        logString.GetBuffer(2048);

        if (!m_LogFileSize)
        {
            logString.Format("Immune System Resources Activity Log\n"
                             "Format :\n"
                             "\"Machine Name\" \"Job Type\" \"Issue Tracking Number\" "
                             "\"Job Sequence\" \"Job Start Time\" "
                             "\"Job End Time\" \"Job Status\"\n");

            m_LogFileSize += logString.GetLength();

            fprintf (m_LogFile, "%s", (LPCTSTR) logString);
            fflush (m_LogFile);
        }

        logString.Format("\"%s\" \"%s\" \"%s\" \"%ld\" \"%s\" \"%s\" \"%s\"\n",
            pDFResource->m_IPAddress,
            pDFResource->m_JobIDString,
            pDFJob->GetCookieId(),
            (LONG) pDFJob->GetJobSeq(),
            (LPCTSTR) pDFResource->m_StartTime.Format("%m/%d/%Y %H:%M:%S"),
            (LPCTSTR) COleDateTime::GetCurrentTime().Format("%m/%d/%Y %H:%M:%S"),
            (LPCTSTR) status);

        m_LogFileSize += logString.GetLength();

        fprintf (m_LogFile, "%s", (LPCTSTR) logString);
        if (!m_LogBufferSize)
            OnLogFlush();
    }

}

void CDFLauncherView::OnLogFlush() 
{
    if (m_LogFile)
    {
        fflush (m_LogFile);
        fclose (m_LogFile);

        m_LogFile = fopen (m_ModulePath + m_LogFileName, "a");

        if (m_LogFile)
        {
            if (m_LogBufferSize)
                setvbuf (m_LogFile, NULL, _IOFBF, m_LogBufferSize);
            fseek (m_LogFile, 0, SEEK_END);
            m_LogFileSize = ftell(m_LogFile);
        }
    }
}

void CDFLauncherView::OnUpdateLogFlush(CCmdUI* pCmdUI) 
{
    if (m_LogFile)
        pCmdUI->Enable(TRUE);
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnLogSettings() 
{
    CActivityLogSetting actLogSetting;
    int rc;

    actLogSetting.m_LogFileName = m_LogFileName;
    actLogSetting.m_LogBufferSize = m_LogBufferSize / 1024;
    actLogSetting.m_MaxLogFileSize = m_MaxLogFileSize / 1024;

    rc = actLogSetting.DoModal();

    if (rc == IDOK)
    {
        m_LogFileName = actLogSetting.m_LogFileName;
        m_LogBufferSize = actLogSetting.m_LogBufferSize * 1024;
        m_MaxLogFileSize = actLogSetting.m_MaxLogFileSize * 1024;

        m_LogFileName.TrimLeft();
        m_LogFileName.TrimRight();
        if (m_LogFileName.GetLength() && m_LogFileName.Right(4).CompareNoCase(_T(".log")))
            m_LogFileName += _T(".Log");


        if (m_LogFile)
        {
            fflush (m_LogFile);
            fclose (m_LogFile);
            m_LogFile = NULL;
        }

        if (m_MaxLogFileSize && !m_LogFileName.IsEmpty())
        {
            m_LogFile = fopen (m_ModulePath + m_LogFileName, "a");

            if (m_LogFile)
            {
                if (m_LogBufferSize)
                    setvbuf (m_LogFile, NULL, _IOFBF, m_LogBufferSize);
                fseek (m_LogFile, 0, SEEK_END);
                m_LogFileSize = ftell(m_LogFile);
            }
        }
        SaveAppSettings();
    }
}

void CDFLauncherView::SaveAppSettings()
{
    CString confFile ("AVISDFRL.cfg");
    CString configFile = m_ModulePath + confFile;
    CString bakConfigFile;
    FILE * fp;

    bakConfigFile = configFile + ".Bak";

//    unlink (bakConfigFile);

//    rename (configFile, bakConfigFile);

	CopyFile(configFile, bakConfigFile, FALSE);

    fp = fopen (configFile, _T("w"));

    if (fp)
    {
        CString maxLogFileSizeTag ("Max Log File Size (KB)");
        CString logBufferSizeTag ("Log Buffer Size (KB)");
        CString logFileNameTag ("Log File Name");
        CString singleJobPerMachine ("Single Job Per Machine");
        CString autoDisableCountTag ("Auto Disable Error Count");
        CString showAppWindowTag ("Show Resource Window");
        CParamValue paramValue;
        CString tmpString;
		CString prologForCfgFile((LPCSTR) IDS_CFGFILE_PROLOG);

		//Added on May/17/2000. Write the prolog for the config file.
		fwrite ((LPCSTR )prologForCfgFile, prologForCfgFile.GetLength(), 1, fp);

        tmpString.Format("%d", m_MaxLogFileSize / 1024);
        paramValue.AddParamValue (maxLogFileSizeTag, tmpString);

        tmpString.Format("%d", m_LogBufferSize / 1024);
        paramValue.AddParamValue (logBufferSizeTag, tmpString);

        paramValue.AddParamValue (logFileNameTag, m_LogFileName);

        if (m_SingleJobPerMachine == TRUE)
            tmpString = "TRUE";
        else
            tmpString = "FALSE";
        paramValue.AddParamValue (singleJobPerMachine, tmpString);

        tmpString.Format("%ld", m_AutoDisableCount);
        paramValue.AddParamValue (autoDisableCountTag, tmpString);

        if (m_ShowAppWindow == TRUE)
            tmpString = "TRUE";
        else
            tmpString = "FALSE";
        paramValue.AddParamValue (showAppWindowTag, tmpString);

        paramValue.WriteParamValue (fp);
        fclose (fp);
    }
}

void CDFLauncherView::LoadAppSettings()
{
    CParamValue paramValue;
    FILE * fp;
    CString maxLogFileSizeTag ("Max Log File Size (KB)");
    CString logBufferSizeTag ("Log Buffer Size (KB)");
    CString logFileNameTag ("Log File Name");
    CString confFile ("AVISDFRL.cfg");
    CString configFile = m_ModulePath + confFile;
    CString singleJobPerMachine ("Single Job Per Machine");
    CString autoDisableCountTag ("Auto Disable Error Count");
    CString showAppWindowTag ("Show Resource Window");

    fp = fopen (configFile, _T("r"));
    if (fp != (FILE *) NULL)
    {
        int numParam = paramValue.ReadParamValue (fp, FALSE, FALSE);

        if (numParam)
        {
            CString tmpValue;

            if (TRUE == paramValue.GetValue (maxLogFileSizeTag, tmpValue))
                m_MaxLogFileSize = atoi (tmpValue);
            else
                m_MaxLogFileSize = 1024;

            if (TRUE == paramValue.GetValue (logBufferSizeTag, tmpValue))
                m_LogBufferSize = atoi (tmpValue);
            else
                m_LogBufferSize = 16;

            if (TRUE == paramValue.GetValue (logFileNameTag, tmpValue))
                m_LogFileName = tmpValue;
            else
                m_LogFileName.Empty();

            if (TRUE == paramValue.GetValue (singleJobPerMachine, tmpValue))
            {
                if (!tmpValue.CompareNoCase ("TRUE"))
                    m_SingleJobPerMachine = TRUE;
                else
                    m_SingleJobPerMachine = FALSE;
            }
            else
                m_SingleJobPerMachine = FALSE;

            if (TRUE == paramValue.GetValue (autoDisableCountTag, tmpValue))
                m_AutoDisableCount = atol (tmpValue);
            else
                m_AutoDisableCount = 200;

            if (TRUE == paramValue.GetValue (showAppWindowTag, tmpValue))
            {
                if (!tmpValue.CompareNoCase ("TRUE"))
                    m_ShowAppWindow = TRUE;
                else
                    m_ShowAppWindow = FALSE;
            }
            else
                m_ShowAppWindow = FALSE;

            g_ShowAppWindow = m_ShowAppWindow;

        }

        if (m_MaxLogFileSize < 0)
            m_MaxLogFileSize = 0;
        if (m_MaxLogFileSize > 10240)
            m_MaxLogFileSize = 10240;

        m_MaxLogFileSize *= 1024;

        if (m_LogBufferSize < 0)
            m_LogBufferSize = 0;
        if (m_LogBufferSize > 32)
            m_LogBufferSize = 32;

        m_LogBufferSize *= 1024;

        m_LogFileName.TrimLeft();
        m_LogFileName.TrimRight();

        if (m_AutoDisableCount < 0)
            m_AutoDisableCount = 0;

        if (m_AutoDisableCount > 10000)
            m_AutoDisableCount = 10000;

        if (m_LogFileName.GetLength() && m_LogFileName.Right(4).CompareNoCase(_T(".log")))
            m_LogFileName += _T(".Log");

        fclose (fp);
    }
}

void CDFLauncherView::OnSetupSinglejobpermachine() 
{
	if (m_SingleJobPerMachine == TRUE)
        m_SingleJobPerMachine = FALSE;
    else
        m_SingleJobPerMachine = TRUE;

    SaveAppSettings();
}

void CDFLauncherView::OnUpdateSetupSinglejobpermachine(CCmdUI* pCmdUI) 
{
    if (m_SingleJobPerMachine == TRUE)
        pCmdUI->SetCheck(1);
    else
        pCmdUI->SetCheck(0);
}

void CDFLauncherView::OnActionsDisablemachine() 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_IPAddress.CompareNoCase (selectedResource->m_IPAddress))
            {
                if (pDFResource->m_Status == DF_RESOURCE_FREE)
                    pDFResource->m_Status = DF_RESOURCE_DOWN;
                else
                    pDFResource->m_MarkedForDisable = TRUE;
            }
        }
    }
}

void CDFLauncherView::OnUpdateActionsDisablemachine(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_IPAddress.CompareNoCase (selectedResource->m_IPAddress) &&
                pDFResource->m_Status != DF_RESOURCE_DOWN )
                break;
        }
        if (i < resCount)
            pCmdUI->Enable(TRUE);
        else
            pCmdUI->Enable(FALSE);
    }
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnActionsEnablemachine() 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_IPAddress.CompareNoCase (selectedResource->m_IPAddress))
            {
                if (pDFResource->m_Status == DF_RESOURCE_DOWN)
                    pDFResource->m_Status = DF_RESOURCE_FREE;
                pDFResource->m_MarkedForDisable = FALSE;
            }
        }
    }
}

void CDFLauncherView::OnUpdateActionsEnablemachine(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_IPAddress.CompareNoCase (selectedResource->m_IPAddress) &&
                pDFResource->m_Status == DF_RESOURCE_DOWN )
                break;
        }
        if (i < resCount)
            pCmdUI->Enable(TRUE);
        else
            pCmdUI->Enable(FALSE);
    }
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnSetupErrorcountforautodisable() 
{
    CAutoDisableErrorCount autoDisableErrorCount;

    autoDisableErrorCount.m_ErrorCount = m_AutoDisableCount;

    int rc = autoDisableErrorCount.DoModal();

    if (rc == IDOK)
    {
        m_AutoDisableCount = autoDisableErrorCount.m_ErrorCount;
        SaveAppSettings();
    }
}

void CDFLauncherView::OnActionsEnablejob() 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_JobIDString.CompareNoCase (selectedResource->m_JobIDString))
            {
                if (pDFResource->m_Status == DF_RESOURCE_DOWN)
                    pDFResource->m_Status = DF_RESOURCE_FREE;
                pDFResource->m_MarkedForDisable = FALSE;
            }
        }
    }
}

void CDFLauncherView::OnUpdateActionsEnablejob(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_JobIDString.CompareNoCase (selectedResource->m_JobIDString) &&
                pDFResource->m_Status == DF_RESOURCE_DOWN )
                break;
        }
        if (i < resCount)
            pCmdUI->Enable(TRUE);
        else
            pCmdUI->Enable(FALSE);
    }
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnActionsDisablejob() 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_JobIDString.CompareNoCase (selectedResource->m_JobIDString))
            {
                if (pDFResource->m_Status == DF_RESOURCE_FREE)
                    pDFResource->m_Status = DF_RESOURCE_DOWN;
                else
                    pDFResource->m_MarkedForDisable = TRUE;
            }
        }
    }
}

void CDFLauncherView::OnUpdateActionsDisablejob(CCmdUI* pCmdUI) 
{
    CDFResource * selectedResource;
    int resCount = m_DFResourceList.GetSize();
    
    selectedResource = GetCurrentSelection();

    if (selectedResource)
    {
        for (int i = 0; i < resCount; i++)
        {
	        CDFResource * pDFResource = (CDFResource *) m_DFResourceList[i];
            if (pDFResource &&
                !pDFResource->m_JobIDString.CompareNoCase (selectedResource->m_JobIDString) &&
                pDFResource->m_Status != DF_RESOURCE_DOWN )
                break;
        }
        if (i < resCount)
            pCmdUI->Enable(TRUE);
        else
            pCmdUI->Enable(FALSE);
    }
    else
        pCmdUI->Enable(FALSE);
}

void CDFLauncherView::OnContextMenu(CWnd* /* pWnd */, CPoint point) 
{
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

void CDFLauncherView::OnSetupDisplayresourceprogresswindow() 
{
	if (m_ShowAppWindow == TRUE)
        m_ShowAppWindow = FALSE;
    else
        m_ShowAppWindow = TRUE;

    g_ShowAppWindow = m_ShowAppWindow;

    SaveAppSettings();
}

void CDFLauncherView::OnUpdateSetupDisplayresourceprogresswindow(CCmdUI* pCmdUI) 
{
    if (m_ShowAppWindow == TRUE)
        pCmdUI->SetCheck(1);
    else
        pCmdUI->SetCheck(0);
}

void CDFLauncherView::OnViewCookieStatistics() 
{
    CCookieStatistics cookieStatistics;

    cookieStatistics.m_LogFileName = m_ModulePath + m_LogFileName;
    OnLogFlush();
    cookieStatistics.DoModal();
}

void CDFLauncherView::OnUpdateViewCookieStatistics(CCmdUI* pCmdUI) 
{
    if (m_LogFileName.IsEmpty())
        pCmdUI->Enable(FALSE);
    else
        pCmdUI->Enable(TRUE);
}

// Allow only one NT and one AIX machine
//#define ALLOW_SINGLE_MACHINE_OF_A_TYPE

void CDFLauncherView::RemoveDuplicateResources()
{
#ifdef ALLOW_SINGLE_MACHINE_OF_A_TYPE
    for (int i = 0; i < m_DFResourceList.GetSize(); i++)
    {
        CDFResource *pDFResource = (CDFResource *) m_DFResourceList[i];
        for (int j = i + 1; j < m_DFResourceList.GetSize(); j++)
        {
            CDFResource *pNewDFResource = (CDFResource *) m_DFResourceList[j];
            /*
            if (!pDFResource->m_JobIDString.CompareNoCase(pNewDFResource->m_JobIDString))
            {
                delete pNewDFResource;
                m_DFResourceList.RemoveAt(j);
                i = -1;
                break;
            }
            */
            if (!pNewDFResource->m_NTMachineName.CompareNoCase(
                pDFResource->m_NTMachineName))
            {
                if (pNewDFResource->m_IPAddress.CompareNoCase(
                    pDFResource->m_IPAddress))
                {
                    pNewDFResource->m_IPAddress = pDFResource->m_IPAddress;
                    pNewDFResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }
            }
            if (!pNewDFResource->m_IPAddress.CompareNoCase(
                pDFResource->m_IPAddress))
            {
                if (pNewDFResource->m_NTMachineName.CompareNoCase(
                    pDFResource->m_NTMachineName))
                {
                    pNewDFResource->m_NTMachineName = pDFResource->m_NTMachineName;
                    pNewDFResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }
                if (pNewDFResource->m_UserName.CompareNoCase(
                    pDFResource->m_UserName))
                {
                    pNewDFResource->m_UserName = pDFResource->m_UserName;
                    pNewDFResource->m_Saved = FALSE;
                    m_ResourceModified = TRUE;
                }
            }
            if (!pDFResource->m_NTMachineName.IsEmpty() &&
                !pNewDFResource->m_NTMachineName.IsEmpty() &&
                pDFResource->m_NTMachineName.CompareNoCase(pNewDFResource->m_NTMachineName))
            {
                delete pNewDFResource;
                m_ResourceModified = TRUE;
                m_DFResourceList.RemoveAt(j);
                i = -1;
                break;
            }
        }
    }
#endif
}

BOOL CDFLauncherView::IsResourceTypePresent(CDFResource *pNewDFResource)
{
#ifdef ALLOW_SINGLE_MACHINE_OF_A_TYPE
    for (int i = 0; i < m_DFResourceList.GetSize(); i++)
    {
        CDFResource *pDFResource = (CDFResource *) m_DFResourceList[i];
        /*
        if (!pDFResource->m_JobIDString.
                CompareNoCase(pNewDFResource->m_JobIDString))
        */
        if (!pNewDFResource->m_NTMachineName.CompareNoCase(
            pDFResource->m_NTMachineName))
        {
            if (pNewDFResource->m_IPAddress.CompareNoCase(
                pDFResource->m_IPAddress))
            {
                pNewDFResource->m_IPAddress = pDFResource->m_IPAddress;
            }
        }
        if (!pNewDFResource->m_IPAddress.CompareNoCase(
            pDFResource->m_IPAddress))
        {
            if (pNewDFResource->m_NTMachineName.CompareNoCase(
                pDFResource->m_NTMachineName))
            {
                pNewDFResource->m_NTMachineName = pDFResource->m_NTMachineName;
                pNewDFResource->m_Saved = FALSE;
                m_ResourceModified = TRUE;
            }
            if (pNewDFResource->m_UserName.CompareNoCase(
                pDFResource->m_UserName))
            {
                pNewDFResource->m_UserName = pDFResource->m_UserName;
                pNewDFResource->m_Saved = FALSE;
                m_ResourceModified = TRUE;
            }
        }

        if (!pDFResource->m_NTMachineName.IsEmpty() &&
            !pNewDFResource->m_NTMachineName.IsEmpty() &&
            pDFResource->m_NTMachineName.CompareNoCase(pNewDFResource->m_NTMachineName))
            return TRUE;
    }
#endif
    return FALSE;
}
