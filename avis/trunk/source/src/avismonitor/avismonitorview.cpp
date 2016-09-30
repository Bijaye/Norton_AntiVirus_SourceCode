// AVISMonitorView.cpp : implementation of the CChildView class
//


#include "stdafx.h"
#include <errno.h>
#include "AVISMonitor.h"
#include "AVISMonitorView.h"
#include "ModuleDialog.h"
#include "mainFrm.h"
#include "ParamValue.h"
#include "MonitorSetupDialog.h"
#include "AVISMonitorLib.h"

#include <winsock.h>
#include "AVISSendMail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorView
IMPLEMENT_DYNCREATE(CAVISMonitorView, CListViewEx)

#define CListView CListViewEx
BEGIN_MESSAGE_MAP(CAVISMonitorView, CListView)
#undef CListView
	//{{AFX_MSG_MAP(CAVISMonitorView)
	ON_COMMAND(ID_ACTIONS_ADD, OnActionsAdd)
	ON_COMMAND(ID_ACTIONS_REMOVE, OnActionsRemove)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_REMOVE, OnUpdateActionsRemove)
	ON_COMMAND(ID_ACTIONS_START, OnActionsStart)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_START, OnUpdateActionsStart)
	ON_COMMAND(ID_ACTIONS_STOP, OnActionsStop)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_STOP, OnUpdateActionsStop)
	ON_COMMAND(ID_SETUP_OPTIONS, OnSetupOptions)
	ON_WM_TIMER()
	ON_COMMAND(ID_MONITOR_LOAD, OnMonitorLoad)
	ON_UPDATE_COMMAND_UI(ID_MONITOR_LOAD, OnUpdateMonitorLoad)
	ON_COMMAND(ID_MONITOR_SAVE, OnMonitorSave)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_ACTIONS_EDIT, OnActionsEdit)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_EDIT, OnUpdateActionsEdit)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_FILE_PRINT, CListViewEx::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListViewEx::OnFilePrint)
END_MESSAGE_MAP()


/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::CAVISMonitorView 
*   Description: 
*       The constructor function. Iniatialize all values to default values 
*   Parameters: 
*       None 
*   Return: 
*       None 
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
CAVISMonitorView::CAVISMonitorView()
{
    m_ModuleCheckInterval = 1;      // Minutes
    m_ModuleTerminateErrorCount = 3;
//    m_ModuleAutoRestart = FALSE;
    m_ModuleStartOnStartup = TRUE;

    m_ViewRefreshInterval = 1;       // Seconds
    m_SortedColumn = 0;
    m_SortOrder = SORT_ASCENDING;
    m_ModuleModified = FALSE;

    m_ModuleList.RemoveAll();
    m_StartTime = COleDateTime::GetCurrentTime();
    m_NextCheckTime = COleDateTime::GetCurrentTime();

    TCHAR modulePath[512];
    // Get the name of the directory from which the program is executed. The module list file 
    // should be present in the same directory.
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
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::~CAVISMonitorView 
*   Description: 
*       The destructor funtion for the view. Clean up the memory allocated  
*       for the view 
*   Parameters: 
*       None 
*   Return: 
*       None 
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
CAVISMonitorView::~CAVISMonitorView()
{
    int moduleCount = m_ModuleList.GetSize();
    for (int i = 0; i < moduleCount; i++)
    {
	    CAVISModule * pModule = (CAVISModule *) m_ModuleList[i];
        if (pModule)
            delete pModule;
    }
    m_ModuleList.RemoveAll();

}


/////////////////////////////////////////////////////////////////////////////
// CAVISMonitorView message handlers

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::PreCreateWindow 
*   Description: 
*       Force the list view to REPORT style and initialize other listview  
*       styles 
*   Parameters: 
*       CREATESTRUCT& cs -  
*   Return: 
*       BOOL -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
BOOL CAVISMonitorView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
    cs.style |= (LVS_REPORT | LVS_SINGLESEL | LVS_EX_FULLROWSELECT);

	if (!CListViewEx::PreCreateWindow(cs))
		return FALSE;

	//cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
	//	::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

char *g_localComputerName;
/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnInitialUpdate 
*   Description: 
*       Called ehen the list view is initially displayed 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnInitialUpdate() 
{
	CListViewEx::OnInitialUpdate();

    DWORD compNameSize;

    compNameSize = sizeof (m_ComputerName);
    // Get the name of the local computer. This is used in messages send to the administrator.
    if (!GetComputerName (m_ComputerName, &compNameSize))
    {
        m_ComputerName[0] = '\0';
    }
    g_localComputerName = m_ComputerName;
    // Read the configuration settings for the program.
    ReadConfigFile();

    CListCtrl& ListCtrl=GetListCtrl();

    ListCtrl.SetExtendedStyle(LVS_EX_FULLROWSELECT | 
        LVS_EX_ONECLICKACTIVATE | 
        LVS_EX_UNDERLINEHOT | 
        LVS_EX_GRIDLINES);
	
    LV_COLUMN lvc;

    CString NameString((LPCSTR) "Module Name");
    CString FilePathString((LPCSTR) "Program File Name");
    CString StartTimeString((LPCSTR) "Start Time");
    CString StopTimeString((LPCSTR) "Stop Time");
    CString LastCheckTimeString((LPCSTR) "Last Check Time");
    CString StatusString((LPCSTR) "Status");
    CString CurrMemUsage((LPCSTR) "Curr Memory");
    CString CurrVMSize((LPCSTR) "VM Size");
    CString OrigMemUsage((LPCSTR) "Orig Memory");
    CString OrigVMSize((LPCSTR) "VM Size");
    CString PeakMemUsage((LPCSTR) "Peak Memory");
    CString PeakVMSize((LPCSTR) "VM Size");
    CString AutoRestart((LPCSTR) "Auto Restart");
    CString AutoStopTime((LPCSTR) "Auto Stop Time");
    CString RestartTime((LPCSTR) "Restart Time");
    CString SendMail((LPCSTR) "Send Mail");
//    CString ErrorCountString((LPCSTR) "Error Count");

    lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem = 0;
    lvc.pszText = (LPTSTR) ((LPCTSTR) NameString);
    lvc.cx = 2* ListCtrl.GetStringWidth ((LPCTSTR) NameString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (0, &lvc);

    lvc.iSubItem = 1;
    lvc.pszText = (LPTSTR) ((LPCTSTR) FilePathString);
    lvc.cx = 2* ListCtrl.GetStringWidth ((LPCTSTR) FilePathString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (1, &lvc);

    lvc.iSubItem = 2;
    lvc.pszText = (LPTSTR) ((LPCTSTR) StatusString);
    lvc.cx = 2* ListCtrl.GetStringWidth ((LPCTSTR) StatusString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (2, &lvc);

    lvc.iSubItem = 3;
    lvc.pszText = (LPTSTR) ((LPCTSTR) StartTimeString);
    lvc.cx = 2* ListCtrl.GetStringWidth ((LPCTSTR) StartTimeString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (3, &lvc);

    lvc.iSubItem = 4;
    lvc.pszText = (LPTSTR) ((LPCTSTR) StopTimeString);
    lvc.cx = 2* ListCtrl.GetStringWidth ((LPCTSTR) StartTimeString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (4, &lvc);

    lvc.iSubItem = 5;
    lvc.pszText = (LPTSTR) ((LPCTSTR) LastCheckTimeString);
    lvc.cx = 2* ListCtrl.GetStringWidth ((LPCTSTR) StartTimeString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (5, &lvc);

    lvc.iSubItem = 6;
    lvc.pszText = (LPTSTR) ((LPCTSTR) CurrMemUsage);
    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) CurrMemUsage) + 20;
    lvc.fmt = LVCFMT_RIGHT;
    ListCtrl.InsertColumn (6, &lvc);

    lvc.iSubItem = 7;
    lvc.pszText = (LPTSTR) ((LPCTSTR) CurrVMSize);
    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) CurrVMSize) + 20;
    lvc.fmt = LVCFMT_RIGHT;
    ListCtrl.InsertColumn (7, &lvc);

    lvc.iSubItem = 8;
    lvc.pszText = (LPTSTR) ((LPCTSTR) OrigMemUsage);
    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) OrigMemUsage) + 20;
    lvc.fmt = LVCFMT_RIGHT;
    ListCtrl.InsertColumn (8, &lvc);

    lvc.iSubItem = 9;
    lvc.pszText = (LPTSTR) ((LPCTSTR) OrigVMSize);
    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) OrigVMSize) + 20;
    lvc.fmt = LVCFMT_RIGHT;
    ListCtrl.InsertColumn (9, &lvc);

    lvc.iSubItem = 10;
    lvc.pszText = (LPTSTR) ((LPCTSTR) PeakMemUsage);
    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) PeakMemUsage) + 20;
    lvc.fmt = LVCFMT_RIGHT;
    ListCtrl.InsertColumn (10, &lvc);

    lvc.iSubItem = 11;
    lvc.pszText = (LPTSTR) ((LPCTSTR) PeakVMSize);
    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) PeakVMSize) + 20;
    lvc.fmt = LVCFMT_RIGHT;
    ListCtrl.InsertColumn (11, &lvc);

    lvc.iSubItem = 12;
    lvc.pszText = (LPTSTR) ((LPCTSTR) AutoRestart);
    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) AutoRestart) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (12, &lvc);

    lvc.iSubItem = 13;
    lvc.pszText = (LPTSTR) ((LPCTSTR) AutoStopTime);
    lvc.cx = 2 * ListCtrl.GetStringWidth ((LPCTSTR) StartTimeString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (13, &lvc);

    lvc.iSubItem = 14;
    lvc.pszText = (LPTSTR) ((LPCTSTR) RestartTime);
    lvc.cx = 2 * ListCtrl.GetStringWidth ((LPCTSTR) StartTimeString) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (14, &lvc);

    lvc.iSubItem = 15;
    lvc.pszText = (LPTSTR) ((LPCTSTR) SendMail);
    lvc.cx = 2 * ListCtrl.GetStringWidth ((LPCTSTR) SendMail) + 20;
    lvc.fmt = LVCFMT_LEFT;
    ListCtrl.InsertColumn (15, &lvc);

//    lvc.iSubItem = 16;
//    lvc.pszText = (LPTSTR) ((LPCTSTR) ErrorCountString);
//    lvc.cx = ListCtrl.GetStringWidth ((LPCTSTR) ErrorCountString) + 20;
//    lvc.fmt = LVCFMT_LEFT;
//    ListCtrl.InsertColumn (16, &lvc);

	OnUpdate(NULL, 1, NULL);

    AddHeaderToolTip(0, "AVIS Module Name");
    AddHeaderToolTip(1, "AVIS Module EXE File Path");
    AddHeaderToolTip(2, "Program Status");
    AddHeaderToolTip(3, "Program Start Time");
    AddHeaderToolTip(4, "Program Stop Time");
    AddHeaderToolTip(5, "Last status check time");
    AddHeaderToolTip(6, "Current Memory Usage");
    AddHeaderToolTip(7, "Current VM Size");
    AddHeaderToolTip(8, "Original Memory Usage");
    AddHeaderToolTip(9, "Original VM Size");
    AddHeaderToolTip(10, "Peak Memory Usage");
    AddHeaderToolTip(11, "Peak VM Size");
    AddHeaderToolTip(12, "Auto Restart Stopped Program");
    AddHeaderToolTip(13, "Auto Stop Time");
    AddHeaderToolTip(14, "Restart Time");
    AddHeaderToolTip(15, "Send mail after module is externally stopped");
//    AddHeaderToolTip(16, "Number of cycles for which the program is not responding");

    // Read the list of modules from the Module list file.
    ReadModuleList();

    int modCount = m_ModuleList.GetSize();

    // Just create the ListView items here. The text for the subitems
    // are created in the OnUpdate() function.

    for (int i = 0, j = 0; i < modCount; i++)
    {
        LV_ITEM lvi;
	    CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];

        if (pAVISModule)
        {
            int nItem;

            lvi.mask=LVIF_TEXT | LVIF_STATE;
            lvi.iItem=j++;
            lvi.iSubItem=0;
            lvi.pszText=pAVISModule->m_ModuleName.GetBuffer(1024);
            lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
            lvi.state = 0;

            nItem = ListCtrl.InsertItem(&lvi);
            ListCtrl.SetItemData(nItem, (DWORD) pAVISModule);
        }
    }

    ListCtrl.SetRedraw(FALSE);
    ListCtrl.SortItems(CompareFunc, (LPARAM) this);
    ListCtrl.SetRedraw(TRUE);

	OnUpdate(NULL, 1, NULL);

    AutoSizeColumns( 0 );
    AutoSizeColumns( 1 );

    COleDateTimeSpan checkInterval(0, 0, m_ModuleCheckInterval, 0);
    m_NextCheckTime = COleDateTime::GetCurrentTime() + checkInterval;

    // Timer to check the status of each module. Modules which are started externally are only
    // recognized during this timer.
    SetTimer (MODULE_CHECK_TIMER, m_ModuleCheckInterval * 60 * 1000, NULL);

    // Refresh the view on every second
    SetTimer (VIEW_REFRESH_TIMERID, 1000, NULL);

    // Check to see if the module needs to be stopped. The modules are not stopped at the 
    // instant the interval that is specified in the confoguration. There may be a delay of
    // upto two minutes depending upon the following timer value.
    SetTimer (MODULE_AUTO_STOP_TIMERID, 2 * 60 * 1000, NULL);   // Two minute timer

    // Check to see if any module needs to be restarted.
    SetTimer (MODULE_RESTART_TIMERID, 1 * 60 * 1000, NULL);   // One minute timer

    // If the list of modules need to be started, then start them after an interval of 2 seconds.
    if (m_ModuleStartOnStartup == TRUE)
        SetTimer (MODULE_AUTO_START_TIMERID, 2 * 1000, NULL);   // Start all the modules after 2 seconds
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnUpdate 
*   Description: 
*       Update the display of the list of modules being monitored by the  
*       program. Also updates the status bar 
*   Parameters: 
*       CView* pSender -  Not required
*       LPARAM lHint -  
*       CObject* pHint -  Not required
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnUpdate(CView* /*pSender*/, LPARAM lHint, CObject* /*pHint*/) 
{
    // Donot refresh window if the window is not visible or is in iconic state
    // The window is always refreshed when lHint is 1 as is the case when this
    // function is called from OnInitDialog()
    if (lHint != 1)
    {
        if (((CMainFrame *) AfxGetMainWnd())->IsIconic())
            return;

        if (IsWindowVisible() == FALSE)
            return;
    }

    CListCtrl& ListCtrl=GetListCtrl();

    int moduleCount = m_ModuleList.GetSize();

    // Display all the subitems of the resources

    CString TmpString((LPCSTR) "Module Name");
    CString NameString;

    if (m_ModuleModified)
    {
        NameString = _T("* ");
        NameString += TmpString;
    }
    else
    {
        NameString = TmpString;
    }

    LV_COLUMN lvc;
    lvc.mask=LVCF_TEXT;

    lvc.iSubItem=0;
    lvc.pszText=(LPTSTR) ((LPCTSTR)NameString);
    ListCtrl.SetColumn(0,&lvc);

	int statIndex;
	CString statStr;
    int modCount = m_ModuleList.GetSize();

    // Update the status bar values
	statIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_INDICATOR_NEXT_CHECK_TIME);
	if (statIndex != -1)
	{
        COleDateTimeSpan checkAfter = m_NextCheckTime - COleDateTime::GetCurrentTime();
		statStr = _T("Check After 00:00:00");
        if (checkAfter.GetStatus() == COleDateTime::valid)
        {
            statStr.Format("Check After %d:%02d:%02d",
                checkAfter.GetHours(),
                checkAfter.GetMinutes(),
                checkAfter.GetSeconds());
        }
		((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(statIndex, statStr);
	}
    
	statIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_INDICATOR_COUNTS);
	if (statIndex != -1)
	{
        int activeCount = 0;
        LONG errorCount = 0;

        for (int i = 0; i < modCount; i++)
        {
    	    CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
            if (pAVISModule)
            {
                if (pAVISModule->m_ModuleStatus == MODULE_STARTED)
                    activeCount++;
                if (pAVISModule->m_ModuleErrorCount)
                    errorCount ++;
            }
        }
        /*
		statStr.Format ("Total %d, Active %d, Error %d",
            modCount, activeCount, errorCount);
        */
		statStr.Format ("Total %d, Active %d",
            modCount, activeCount);
		((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(statIndex, statStr);
    }

	statIndex = ((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.CommandToIndex(IDS_INDICATOR_UPTIME);
	if (statIndex != -1)
	{
        COleDateTimeSpan upTime = COleDateTime::GetCurrentTime() - m_StartTime;

        statStr.Format("System Up Time %d-%02d:%02d:%02d", upTime.GetDays(),
            upTime.GetHours(),
            upTime.GetMinutes(),
            upTime.GetSeconds());
		((CMainFrame *) AfxGetMainWnd())->m_wndStatusBar.SetPaneText(statIndex, statStr);
    }

    // Flicker free drawing
    // Do not redraw as and when the values are changed. Only redraw at the end when all
    // the values have been updated.
    ListCtrl.SetRedraw(FALSE);
    for (int i = 0; i < moduleCount; i++)
    {
	    CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];

        if (pAVISModule)
        {
            int nItem;
            LV_FINDINFO lvf;

            lvf.flags = LVFI_PARAM;
            lvf.lParam = (LPARAM) pAVISModule;

            nItem = ListCtrl.FindItem (&lvf, -1);

            if (nItem != -1)
            {
                CString newLabel;
                CString oldStr;
                CString tmpStr;

                if (pAVISModule->m_Saved != TRUE)
                {
                    newLabel = _T("* ");
                    newLabel += pAVISModule->m_ModuleName;
                }
                else
                {
                    newLabel = pAVISModule->m_ModuleName;
                }

                // Flicker free drawing
                // First get the old display value. If the new value is different than the old
                // value, then only call to update the value
                oldStr = ListCtrl.GetItemText (nItem, 0);
                if (oldStr.Compare(newLabel))
                    ListCtrl.SetItemText (nItem, 0, newLabel);

                oldStr = ListCtrl.GetItemText (nItem, 1);
                if (oldStr.Compare(pAVISModule->m_ModuleFilePath + " " + pAVISModule->m_ModuleArguments))
                    ListCtrl.SetItemText (nItem, 1, pAVISModule->m_ModuleFilePath + " " + pAVISModule->m_ModuleArguments);

                oldStr = ListCtrl.GetItemText (nItem, 2);
                switch (pAVISModule->m_ModuleStatus)
                {
                case MODULE_STARTED:
                    if (pAVISModule->m_ModuleExternalStarted == TRUE)
                    {
                        tmpStr = "Active (External)";
                    }
                    else
                        tmpStr = "Active";
                    break;

                case MODULE_STOPPED:
                    {
                        if (pAVISModule->m_ModuleStopTime.GetStatus() == COleDateTime::valid)
                            tmpStr = "Stopped";
                        else
                            tmpStr = "Not Started";
                    }
                    break;

                case MODULE_UNKNOWN:
                default:
                    tmpStr = "Unknown";
                    break;
                }
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 2, tmpStr);

                oldStr = ListCtrl.GetItemText (nItem, 3);
                if (pAVISModule->m_ModuleStatus != MODULE_STOPPED)
                {
                    tmpStr = pAVISModule->m_ModuleStartTime.Format("%m/%d/%Y %H:%M:%S");
                    // Since the module was started externally, the exact start time for
                    // the module cannot be determined and so an estimated time is displayed
                    // which is the time when AVISMonitor first detected the presence of the 
                    // module
                    if (pAVISModule->m_ModuleExternalStarted == TRUE)
                    {
                        tmpStr += " (Estimated)";
                    }
                }
                else
                    tmpStr = " ";
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 3, tmpStr);

                oldStr = ListCtrl.GetItemText (nItem, 4);
                if (pAVISModule->m_ModuleStatus == MODULE_STOPPED)
                {
                    if (pAVISModule->m_ModuleStopTime.GetStatus() == COleDateTime::valid)
                        tmpStr = pAVISModule->m_ModuleStopTime.Format("%m/%d/%Y %H:%M:%S");
                    else
                        tmpStr = " ";
                }
                else
                    tmpStr = " ";
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 4, tmpStr);

                oldStr = ListCtrl.GetItemText (nItem, 5);
                tmpStr = pAVISModule->m_ModuleLastCheckTime.Format("%m/%d/%Y %H:%M:%S");
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 5, tmpStr);

                // The memory statistics of the module are displayed only for the active
                // modules. The initial values are refresshed only after 2 timer cycles so that
                // a proper estimate of the initial memory used by the module can be estimated.
                if (pAVISModule->m_ModuleStatus != MODULE_STOPPED && pAVISModule->m_OrigWorkingSetSize)
                {
                    oldStr = ListCtrl.GetItemText (nItem, 6);
                    tmpStr.Format ("%ld K", (long) (pAVISModule->m_MemCounter.WorkingSetSize / 1024));
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 6, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 7);
                    tmpStr.Format ("%ld K", (long) (pAVISModule->m_MemCounter.PagefileUsage / 1024));
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 7, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 8);
                    tmpStr.Format ("%ld K", (long) (pAVISModule->m_OrigWorkingSetSize / 1024));
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 8, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 9);
                    tmpStr.Format ("%ld K", (long) (pAVISModule->m_OrigVMSize / 1024));
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 9, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 10);
                    tmpStr.Format ("%ld K", (long) (pAVISModule->m_MemCounter.PeakWorkingSetSize / 1024));
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 10, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 11);
                    tmpStr.Format ("%ld K", (long) (pAVISModule->m_MemCounter.PeakPagefileUsage / 1024));
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 11, tmpStr);
                }
                else
                {
                    tmpStr = " ";

                    oldStr = ListCtrl.GetItemText (nItem, 6);
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 6, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 7);
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 7, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 8);
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 8, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 9);
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 9, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 10);
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 10, tmpStr);

                    oldStr = ListCtrl.GetItemText (nItem, 11);
                    if (oldStr.Compare(tmpStr))
                        ListCtrl.SetItemText (nItem, 11, tmpStr);
                }
                oldStr = ListCtrl.GetItemText (nItem, 12);
                tmpStr.Format ("%s", ((pAVISModule->m_AutoRestart == TRUE) ? "Yes" : "No"));
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 12, tmpStr);

                oldStr = ListCtrl.GetItemText (nItem, 13);
                if (pAVISModule->m_ModuleStatus != MODULE_STOPPED && pAVISModule->m_AutoStopTime > 0)
                {
                    COleDateTime moduleAutoStopTime;
                    COleDateTimeSpan stopTimeSpan (0, 0, pAVISModule->m_AutoStopTime, 0);

                    moduleAutoStopTime = pAVISModule->m_ModuleStartTime + stopTimeSpan;
                    tmpStr = moduleAutoStopTime.Format("%m/%d/%Y %H:%M:%S");
                }
                else
                    tmpStr = " ";
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 13, tmpStr);

                oldStr = ListCtrl.GetItemText (nItem, 14);
                if (pAVISModule->m_ModuleStatus == MODULE_STOPPED && 
                    pAVISModule->m_ModuleStopTime.GetStatus() == COleDateTime::valid &&
                    pAVISModule->m_AutoRestart == TRUE)
                {
                    COleDateTime moduleRestartTime;
                    COleDateTimeSpan restartTimeSpan (0, 0, pAVISModule->m_RestartTime, 0);

                    moduleRestartTime = pAVISModule->m_ModuleStopTime + restartTimeSpan;
                    tmpStr = moduleRestartTime.Format("%m/%d/%Y %H:%M:%S");
                }
                else
                    tmpStr = " ";
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 14, tmpStr);

                oldStr = ListCtrl.GetItemText (nItem, 15);
                tmpStr.Format ("%s", ((pAVISModule->m_SendMail == TRUE) ? "Yes" : "No"));
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 15, tmpStr);

                /*
                oldStr = ListCtrl.GetItemText (nItem, 16);
                if (pAVISModule->m_ModuleErrorCount)
                    tmpStr.Format ("%d", pAVISModule->m_ModuleErrorCount);
                else
                    tmpStr = " ";
                if (oldStr.Compare(tmpStr))
                    ListCtrl.SetItemText (nItem, 16, tmpStr);
                */
            }
        }
    }
    ListCtrl.SetRedraw (TRUE);

}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnActionsAdd 
*   Description: 
*       Add a new module to the list of modules being monitored  
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnActionsAdd() 
{
    CModuleDialog moduleDialog;
    int rc;

    rc = moduleDialog.DoModal();

    // Add a new module information. However the module is not started automatically
    // The user has to start the module manually.
    if (rc == IDOK)
    {
        if (!moduleDialog.m_ModuleName.IsEmpty() &&
            !moduleDialog.m_ModuleFilePath.IsEmpty())
        {
            CAVISModule * pModule = (CAVISModule *) new (CAVISModule);
            CListCtrl & ListCtrl = GetListCtrl();

            pModule->m_ModuleName = moduleDialog.m_ModuleName;
            pModule->m_ModuleFilePath = moduleDialog.m_ModuleFilePath;
            pModule->m_ModuleArguments = moduleDialog.m_ModuleArguments;
            pModule->m_AutoRestart = moduleDialog.m_AutoRestart;
            pModule->m_AutoStopTime = moduleDialog.m_AutoStopTime;
            pModule->m_RestartTime = moduleDialog.m_RestartTime;
            pModule->m_SendMail = moduleDialog.m_SendMail;

            m_ModuleList.Add(pModule);

            LV_ITEM lvi;
            int nItem;

            lvi.mask=LVIF_TEXT | LVIF_STATE;
            lvi.iItem=ListCtrl.GetItemCount();
            lvi.iSubItem=0;
            lvi.pszText=pModule->m_ModuleName.GetBuffer(1024);
            lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
            lvi.state = 0;

            nItem = ListCtrl.InsertItem(&lvi);
            ListCtrl.SetItemData(nItem, (DWORD) pModule);

            pModule->m_Saved = FALSE;
            pModule->m_ModuleStatus = MODULE_STOPPED;
            m_ModuleModified = TRUE;

            ListCtrl.SetRedraw(FALSE);
            ListCtrl.SortItems(CompareFunc, (LPARAM) this);
            ListCtrl.SetRedraw(TRUE);
            OnUpdate(NULL, 0, NULL);
            UpdateWindow();
        }
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnActionsRemove 
*   Description: 
*       Remove the module from the list of modules being monitored 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnActionsRemove() 
{
    CListCtrl& ListCtrl=GetListCtrl();
    CAVISModule * pModule;

    pModule = GetCurrentSelection();

    if (pModule && pModule->m_ModuleStatus == MODULE_STOPPED)
    {
        int rc;
        CString msgStr;

        msgStr.GetBuffer(1024);
        msgStr.Format ("Remove Module <%s>.\n\nAre You Sure ?\n",
            (LPCTSTR) pModule->m_ModuleName);

        rc = AfxMessageBox (msgStr, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
        if (rc != IDYES)
            return;
    }

    // Make sure that the module is still stopped. It could have been started by the
    // timer while the user was responding to the dialog message.
    if (pModule && pModule->m_ModuleStatus == MODULE_STOPPED)
    {
        int modCount = m_ModuleList.GetSize();
        int itemCount = ListCtrl.GetItemCount();
        int i;

        for (i = 0; i < modCount; i++)
        {
            if (pModule == (CAVISModule *) m_ModuleList[i])
            {
                m_ModuleList.RemoveAt(i);
                break;
            }
        }

        for (i = 0; i < itemCount; i++)
        {
            if (pModule == (CAVISModule *) ListCtrl.GetItemData(i))
            {
                ListCtrl.DeleteItem(i);
                break;
            }
        }

        delete pModule;
        m_ModuleModified = TRUE;
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnUpdateActionsRemove 
*   Description: 
*       Allow to remove the module from the list if it is currently stopped 
*   Parameters: 
*       CCmdUI* pCmdUI -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnUpdateActionsRemove(CCmdUI* pCmdUI) 
{
    CAVISModule * pModule;

    pModule = GetCurrentSelection();

    if (pModule && pModule->m_ModuleStatus == MODULE_STOPPED)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable(FALSE);
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnActionsStart 
*   Description: 
*       Manually start the module 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnActionsStart() 
{
    CAVISModule * pModule;

    pModule = GetCurrentSelection();

    if (pModule && pModule->m_ModuleStatus == MODULE_STOPPED)
    {
        int rc;
        CString msgStr;

        msgStr.GetBuffer(1024);
        msgStr.Format ("Start Module <%s>.\n\nAre You Sure ?\n",
            (LPCTSTR) pModule->m_ModuleName);

        rc = AfxMessageBox (msgStr, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
        if (rc != IDYES)
            return;
        StartModule (pModule);
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnUpdateActionsStart 
*   Description: 
*       Allow to manually start the module if it is currently stopped 
*   Parameters: 
*       CCmdUI* pCmdUI -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnUpdateActionsStart(CCmdUI* pCmdUI) 
{
    CAVISModule * pModule;

    pModule = GetCurrentSelection();

    if (pModule && pModule->m_ModuleStatus == MODULE_STOPPED)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable(FALSE);
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnActionsStop 
*   Description: 
*       Manually stop the active module by calling TerminateProcess 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnActionsStop() 
{
    CAVISModule * pModule;
    BOOL processTerminated = FALSE;

    pModule = GetCurrentSelection();

    CMutex Mutex(FALSE, "AVISMonitorThreadStartModule");
    CSingleLock sLock (&Mutex, TRUE);
    if (pModule && pModule->m_hProcess &&
        (pModule->m_ModuleStatus != MODULE_STOPPED))
    {
        int rc;
        CString msgStr;

        msgStr.GetBuffer(1024);
        msgStr.Format ("Stop Module <%s>.\n\nAre You Sure ?\n",
            (LPCTSTR) pModule->m_ModuleName);

        rc = AfxMessageBox (msgStr, MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
        if (rc == IDYES)
        {
            // Modify code to use WM_CLOSE followed by terminate process

            TerminateProcess (pModule->m_hProcess, TERMINATE_USER_STOP);
            processTerminated = TRUE;
        }
    }
    sLock.Unlock();
    if (processTerminated == TRUE)
        Sleep (1000);
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnUpdateActionsStop 
*   Description: 
*       Allow to stop the selected module manually only if the module is  
*       currently active 
*   Parameters: 
*       CCmdUI* pCmdUI -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnUpdateActionsStop(CCmdUI* pCmdUI) 
{
    CAVISModule * pModule;

    pModule = GetCurrentSelection();

    if (pModule && pModule->m_hProcess &&
        (pModule->m_ModuleStatus != MODULE_STOPPED))
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable (FALSE);
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnSetupOptions 
*   Description: 
*       Change the AVISModule program options 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnSetupOptions() 
{
    CMonitorSetupDialog monitorSetupDlg;
    int rc;

//    monitorSetupDlg.m_autoRestart = m_ModuleAutoRestart;
    monitorSetupDlg.m_AutoTerminateErrorCount = m_ModuleTerminateErrorCount;
    monitorSetupDlg.m_StatusCheckInterval = m_ModuleCheckInterval;
    monitorSetupDlg.m_startOnMonitorStartup = m_ModuleStartOnStartup;

    rc = monitorSetupDlg.DoModal();

    if (rc == IDOK)
    {
        BOOL valueChanged = FALSE;

//        if (m_ModuleAutoRestart != monitorSetupDlg.m_autoRestart)
//        {
//            m_ModuleAutoRestart = monitorSetupDlg.m_autoRestart;
//            valueChanged = TRUE;
//        }

        if (m_ModuleStartOnStartup != monitorSetupDlg.m_startOnMonitorStartup)
        {
            m_ModuleStartOnStartup = monitorSetupDlg.m_startOnMonitorStartup;
            valueChanged = TRUE;
        }

        if (m_ModuleTerminateErrorCount != monitorSetupDlg.m_AutoTerminateErrorCount)
        {
            m_ModuleTerminateErrorCount = monitorSetupDlg.m_AutoTerminateErrorCount;
            valueChanged = TRUE;
        }

        if (m_ModuleCheckInterval != monitorSetupDlg.m_StatusCheckInterval)
        {
            m_ModuleCheckInterval = monitorSetupDlg.m_StatusCheckInterval;
            KillTimer (MODULE_CHECK_TIMER);
            COleDateTimeSpan checkInterval(0, 0, 0, 10);
            m_NextCheckTime = COleDateTime::GetCurrentTime() + checkInterval;
            SetTimer (MODULE_CHECK_TIMER, 10000, NULL);
            valueChanged = TRUE;
        }

        // If any of the values are changed then save the new values in the configuration file.
        if (valueChanged == TRUE)
            SaveConfigFile();
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnTimer 
*   Description: 
*       Timer handling function 
*   Parameters: 
*       UINT nIDEvent -  Specifies which timer event to process
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnTimer(UINT nIDEvent) 
{
    switch (nIDEvent)
    {
    case MODULE_CHECK_TIMER:
        {
            KillTimer (MODULE_CHECK_TIMER);
            int modCount = m_ModuleList.GetSize();
            int i;

            for (i = 0; i < modCount; i++)
            {
	            CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
                if (pAVISModule)
                {
                    pAVISModule->m_ModuleLastCheckTime = COleDateTime::GetCurrentTime();
                    if (pAVISModule->m_ModuleStatus != MODULE_STOPPED)
                    {
                        // Check status of all ongoing modules
                        if (pAVISModule->m_OrigWorkingSetSize != 0)
                        {
                            // Get Current Process Memory Usage Information
                            GetProcessMemoryInfo (
                                pAVISModule->m_hProcess,
                                &pAVISModule->m_MemCounter,
                                sizeof (pAVISModule->m_MemCounter));
                        }
                        else
                        {
                            // Get the original working memory only after atleast 30 seconds since the
                            // first time the program was detected to be active
                            COleDateTimeSpan memoryCheckOffset(0, 0, 0, 30);
                            if (pAVISModule->m_ModuleLastCheckTime - pAVISModule->m_ModuleStartTime >= memoryCheckOffset)
                            {
                                PROCESS_MEMORY_COUNTERS memCounter;
                                BOOL brc = GetProcessMemoryInfo (
                                    pAVISModule->m_hProcess,
                                    &memCounter,
                                    sizeof (memCounter));
                                if (brc)
                                {
                                    pAVISModule->m_OrigWorkingSetSize = memCounter.WorkingSetSize;
                                    pAVISModule->m_OrigVMSize = memCounter.PagefileUsage;
                                    GetProcessMemoryInfo (
                                        pAVISModule->m_hProcess,
                                        &pAVISModule->m_MemCounter,
                                        sizeof (pAVISModule->m_MemCounter));
                                }
                            }
                        }
                        ModuleCheckStatus (pAVISModule);
                    }
                    else
                    {
                        ModuleCheckExternalStatus(pAVISModule);
                        /*
                        // Start all the modules which are stopped
                        if (pAVISModule->m_ModuleStatus == MODULE_STOPPED &&
                            pAVISModule->m_AutoRestart == TRUE &&
                            pAVISModule->m_ModuleStopTime.GetStatus() == COleDateTime::valid)
                            StartModule (pAVISModule);
                        */
                    }
                }
            }

            // Write the statistics about the modules to a file on disk
            WriteModuleInfoToFile();

            COleDateTimeSpan checkInterval(0, 0, m_ModuleCheckInterval, 0);
            m_NextCheckTime = COleDateTime::GetCurrentTime() + checkInterval;
            SetTimer (MODULE_CHECK_TIMER, m_ModuleCheckInterval * 60 * 1000, NULL);
        }
        break;

    case VIEW_REFRESH_TIMERID:
        OnUpdate(NULL, 0, NULL);
        UpdateWindow();
        break;

    case MODULE_AUTO_START_TIMERID:
        {
            // This timer is called only once at the beginning of the program to start
            // the modules on program startup
            CWaitCursor waitCursor;
            KillTimer (MODULE_AUTO_START_TIMERID);
            int modCount = m_ModuleList.GetSize();
            int i;

            for (i = 0; i < modCount; i++)
            {
	            CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
                if (pAVISModule)
                {
                    StartModule (pAVISModule);
                }
            }
        }
        break;

    case MODULE_AUTO_STOP_TIMERID:
        {
            // Auto terminate the program after the specified period. This is to allow the
            // programs to restart thereby resetting network connections etc.

            // Use the mutex to make sure that no module is being started or being stopped at
            // this time by another thread.

            CMutex Mutex(FALSE, "AVISMonitorThreadStartModule");
            CSingleLock sLock (&Mutex, TRUE);
            CWaitCursor waitCursor;
            KillTimer (MODULE_AUTO_STOP_TIMERID);
            int modCount = m_ModuleList.GetSize();
            int i;

            for (i = 0; i < modCount; i++)
            {
                // Check if any module needs to be automatically stopped
	            CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
                if (pAVISModule && 
                    pAVISModule->m_hProcess &&
                    pAVISModule->m_ModuleStatus != MODULE_STOPPED && 
                    pAVISModule->m_AutoStopTime > 0)
                {
                    COleDateTime moduleAutoStopTime;
                    COleDateTimeSpan stopTimeSpan (0, 0, pAVISModule->m_AutoStopTime, 0);

                    moduleAutoStopTime = pAVISModule->m_ModuleStartTime + stopTimeSpan;
                    COleDateTime curTime = COleDateTime::GetCurrentTime();

                    if (curTime > moduleAutoStopTime)
                    {
                        if (pAVISModule->m_WMCloseMessageSent == TRUE)
                        {
                            // If WM_CLOSE message has been sent to the module and the
                            // module has still not terminated then just terminate the
                            // process.
                            TerminateProcess (pAVISModule->m_hProcess, TERMINATE_AUTO_STOP);
                        }
                        else
                        {
                            // First send a WM_CLOSE message to the module and mark the state as
                            // WM_CLOSE message sent.
                            SendWMCloseMessage (pAVISModule);
                            pAVISModule->m_WMCloseMessageSent = TRUE;
                        }
                    }
                }
            }
            SetTimer (MODULE_AUTO_STOP_TIMERID, 2 * 60 * 1000, NULL);   // Two minute timer
        }
        break;
    case MODULE_RESTART_TIMERID:
        {
            CWaitCursor waitCursor;
            KillTimer (MODULE_RESTART_TIMERID);
            int modCount = m_ModuleList.GetSize();
            int i;

            for (i = 0; i < modCount; i++)
            {
                // Check if any module needs to be automatically restarted
	            CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
                if (pAVISModule && 
                    pAVISModule->m_ModuleStatus == MODULE_STOPPED && 
                    pAVISModule->m_ModuleStopTime.GetStatus() == COleDateTime::valid &&
                    pAVISModule->m_AutoRestart == TRUE)
                {
                    COleDateTime moduleRestartTime;
                    COleDateTimeSpan restartTimeSpan (0, 0, pAVISModule->m_RestartTime, 0);

                    moduleRestartTime = pAVISModule->m_ModuleStopTime + restartTimeSpan;
                    COleDateTime curTime = COleDateTime::GetCurrentTime();

                    if (curTime > moduleRestartTime)
                    {
                        StartModule (pAVISModule);
                    }
                }
            }
            SetTimer (MODULE_RESTART_TIMERID, 1 * 60 * 1000, NULL);   // One minute timer
        }
        break;

    default:
		CListView::OnTimer(nIDEvent);
        break;
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::CompareFunc 
*   Description: 
*       Compare function for sorting the Module list view 
*   Parameters: 
*       LPARAM lParam1 -  ListCtrl Data for the first element
*       LPARAM lParam2 -  ListCtrl Data for the second element
*       LPARAM lParamSort -  this pointer passed as lParamSort so that the static
*                            function can access the class members
*   Return: 
*       int -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
int CAVISMonitorView::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
    CAVISMonitorView * pAVISMonitorView = (CAVISMonitorView *) lParamSort;
    CAVISModule * pAVISModule1 = (CAVISModule *) lParam1;
    CAVISModule * pAVISModule2 = (CAVISModule *) lParam2;

    if (pAVISMonitorView && pAVISModule1 && pAVISModule2)
    {
        int rc = 0;

        switch (pAVISMonitorView->m_SortOrder)
        {
        case SORT_DESCENDING:
            if (pAVISMonitorView->m_SortedColumn == 0)
            {
                rc = pAVISModule2->m_ModuleName.CompareNoCase(pAVISModule1->m_ModuleName);
            }

            return rc;
            break;

        default:
        case SORT_ASCENDING:
            if (pAVISMonitorView->m_SortedColumn == 0)
            {
                rc = pAVISModule1->m_ModuleName.CompareNoCase(pAVISModule2->m_ModuleName);
            }

            return rc;
            break;
        }
    }
    return 0;
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::GetCurrentSelection 
*   Description: 
*       Return the currently selected module pointer 
*   Parameters: 
*       None 
*   Return: 
*       CAVISModule * -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
CAVISModule * CAVISMonitorView::GetCurrentSelection()
{
    CListCtrl& ListCtrl=GetListCtrl();
	CAVISModule * SelectedModule = NULL;
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
			SelectedModule = (CAVISModule *) ListCtrl.GetItemData(i);
			break;
		}
    }
	return (SelectedModule);
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnMonitorLoad 
*   Description: 
*       Reload the list of modules to be monitored from the file stored on  
*       disk 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnMonitorLoad() 
{
    int rc;

    rc = AfxMessageBox ("Reload modules from file saved on disk.\n\nAre You Sure ?\n",
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (rc != IDYES)
        return;

    m_ModuleModified = FALSE;

    CListCtrl& ListCtrl=GetListCtrl();

    // If none of the existing modules are busy then free the list of
    // modules and reload from the module file.
    int modCount = m_ModuleList.GetSize();
    for (int i = 0; i < modCount; i++)
    {
	    CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
        if (pAVISModule && 
            pAVISModule->m_ModuleStatus != MODULE_STOPPED )
        {
            break;
        }
    }
    if (i < modCount)
    {
        AfxMessageBox ("Error in loading modules list as some modules are busy.", MB_ICONINFORMATION);
    }
    else
    {
        int modCount = m_ModuleList.GetSize();
        int i, j;

        for (i = 0; i < modCount; i++)
        {
	        CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
            if (pAVISModule)
                delete pAVISModule;
        }
        m_ModuleList.RemoveAll();
        m_ModuleList.SetSize (0, 10);
        ListCtrl.DeleteAllItems();
        OnUpdate(NULL, 1, NULL);

        ReadModuleList();

        modCount = m_ModuleList.GetSize();
        for (i = 0, j = 0; i < modCount; i++)
        {
            LV_ITEM lvi;
	        CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];

            if (pAVISModule)
            {
                int nItem;

                lvi.mask=LVIF_TEXT | LVIF_STATE;
                lvi.iItem=j++;
                lvi.iSubItem=0;
                lvi.pszText=pAVISModule->m_ModuleName.GetBuffer(1024);
                lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
                lvi.state = 0;

                nItem = ListCtrl.InsertItem(&lvi);
                ListCtrl.SetItemData(nItem, (DWORD) pAVISModule);
            }
        }

        ListCtrl.SetRedraw(FALSE);
        ListCtrl.SortItems(CompareFunc, (LPARAM) this);
        ListCtrl.SetRedraw(TRUE);
        OnUpdate(NULL, 1, NULL);

        AutoSizeColumns( 0 );
        AutoSizeColumns( 1 );
    }

    return;
}


/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnUpdateMonitorLoad 
*   Description: 
*       Reload modules from disk file only if there are no modules currently  
*       active 
*   Parameters: 
*       CCmdUI* pCmdUI -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnUpdateMonitorLoad(CCmdUI* pCmdUI) 
{
    int modCount = m_ModuleList.GetSize();
    int i;

    for (i = 0; i < modCount; i++)
    {
	    CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
        if (pAVISModule && pAVISModule->m_ModuleStatus != MODULE_STOPPED)
        {
            pCmdUI->Enable (FALSE);
            return;
        }
    }
    pCmdUI->Enable (TRUE);
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnMonitorSave 
*   Description: 
*       Save the current list of modules to the file on disk 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnMonitorSave() 
{
    CString bakMonFile ((LPCTSTR) "AVISMonitor.lst.bak");
    CString monFile ((LPCTSTR) "AVISMonitor.lst");
    CString bakMonitorFile = m_ModulePath + bakMonFile;
    CString monitorFile = m_ModulePath + monFile;
    CListCtrl& ListCtrl=GetListCtrl();
    BOOL saveFile = TRUE;

    int rc;

    rc = AfxMessageBox ("Save modules to file on disk.\n\nAre You Sure ?\n",
        MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

    if (rc != IDYES)
        return;

    unlink (bakMonitorFile);

    // BAckup the cuurent disk file
    rc = rename (monitorFile, bakMonitorFile);
    if (rc)
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

        fp = fopen (monitorFile, _T("w"));

        if (fp)
        {
            CString moduleNameTag((LPCTSTR) "Module Name");
            CString moduleFilePathTag((LPCTSTR) "Module File Path");
            CString moduleArgumentsTag((LPCTSTR) "Module Arguments");
            CString moduleAutoStartTag((LPCSTR) "Module Auto Restart");
            CString moduleAutoStopTimeTag((LPCSTR) "Module Auto Stop Time");
            CString moduleRestartTimeTag((LPCSTR) "Module Restart Time");
            CString moduleSendMailTag((LPCSTR) "Send Mail");
			CString prologForListFile((LPCSTR) IDS_LSTFILE_PROLOG);

			//Added on May/17/2000. Write the prolog for the list file.
			fwrite ((LPCSTR )prologForListFile, prologForListFile.GetLength(), 1, fp);

            m_ModuleModified = FALSE;

            for ( i = 0; i < itemCount; i++)
            {
        	    CAVISModule * pModule = (CAVISModule *) ListCtrl.GetItemData(i);
                pModule->m_Saved = FALSE;
            }

            for ( i = 0; i < itemCount; i++)
            {
                CParamValue paramValue;
        	    CAVISModule * pAVISModule = (CAVISModule *) ListCtrl.GetItemData(i);
                if (pAVISModule)
                {
                    BOOL brc;

                    paramValue.AddParamValue (moduleNameTag, pAVISModule->m_ModuleName);
                    paramValue.AddParamValue (moduleFilePathTag, pAVISModule->m_ModuleFilePath);
                    paramValue.AddParamValue (moduleArgumentsTag, pAVISModule->m_ModuleArguments);
                    CString tmpStr;
                    tmpStr = (pAVISModule->m_AutoRestart == TRUE) ? CString("Yes") : CString("No");
                    paramValue.AddParamValue (moduleAutoStartTag, tmpStr);
                    tmpStr.Format ("%ld", pAVISModule->m_AutoStopTime);
                    paramValue.AddParamValue (moduleAutoStopTimeTag, tmpStr);
                    tmpStr.Format ("%ld", pAVISModule->m_RestartTime);
                    paramValue.AddParamValue (moduleRestartTimeTag, tmpStr);
                    tmpStr = (pAVISModule->m_SendMail == TRUE) ? CString("Yes") : CString("No");
                    paramValue.AddParamValue (moduleSendMailTag, tmpStr);

                    brc = paramValue.WriteParamValue (fp);

                    if (brc == FALSE)
                    {
                        AfxMessageBox ("Error in creating module file.");
                        m_ModuleModified = TRUE;
                        break;
                    }
                    else
                    {
                        pAVISModule->m_Saved = TRUE;
                    }
                }
            }
            fclose (fp);
        }
        else
        {
            AfxMessageBox ("Error in creating module file.");
            m_ModuleModified = TRUE;
        }
    }
}

/* The format of the module file is as follows
Module Name : Test
Module File Path : c:\dir\test.exe
Module Arguments : Test Argument
Module Auto Restart : Yes
Module Auto Stop Time : 0
Send Mail : Yes
*/
/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::ReadModuleList 
*   Description: 
*       Read the list of modules from the disk file 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::ReadModuleList()
{
    FILE *fp;
    CString tmpStr;
    CString modFile ((LPCTSTR) "AVISMonitor.lst");
    CString moduleFile = m_ModulePath + modFile;

    CParamValue paramValue;

    fp = fopen (moduleFile, _T("r"));
    if (fp != (FILE *) NULL)
    {
        int numParam;
        CString moduleNameTag((LPCTSTR) "Module Name");
        CString moduleFilePathTag((LPCTSTR) "Module File Path");
        CString moduleArgumentsTag((LPCTSTR) "Module Arguments");
        CString moduleAutoStartTag((LPCSTR) "Module Auto Restart");
        CString moduleAutoStopTimeTag((LPCSTR) "Module Auto Stop Time");
        CString moduleRestartTimeTag((LPCSTR) "Module Restart Time");
        CString moduleSendMailTag((LPCSTR) "Send Mail");

        while (TRUE)
        {
            int defParamSize = paramValue.m_DefaultParam.GetSize();
            for (int i = 0; i < defParamSize; i++)
            {
                CString defParam = paramValue.m_DefaultParam[i];

                if (!defParam.CompareNoCase(moduleNameTag))
                    paramValue.m_DefaultValue[i].Empty();
                if (!defParam.CompareNoCase(moduleFilePathTag))
                    paramValue.m_DefaultValue[i].Empty();
                if (!defParam.CompareNoCase(moduleArgumentsTag))
                    paramValue.m_DefaultValue[i].Empty();
// The default value for automatically starting the module is FALSE
                if (!defParam.CompareNoCase(moduleAutoStartTag))
                    paramValue.m_DefaultValue[i] = "No";
// The default value for automatically stopping the module is FALSE
                if (!defParam.CompareNoCase(moduleAutoStopTimeTag))
                    paramValue.m_DefaultValue[i] = "0";
// The default value for restart time of the module is 0
                if (!defParam.CompareNoCase(moduleRestartTimeTag))
                    paramValue.m_DefaultValue[i] = "0";
// The default value for sending mail is TRUE
                if (!defParam.CompareNoCase(moduleSendMailTag))
                    paramValue.m_DefaultValue[i] = "Yes";
            }

            numParam = paramValue.ReadParamValue (fp, TRUE, TRUE);

            if (!numParam)
                break;

            // There should be atleast 2 parameters.

            if (numParam >= 2)
            {
                CString tmpValue;

                CAVISModule * pAVISModule = (CAVISModule *) new (CAVISModule);
                int j = 0;

                if (TRUE == paramValue.GetValue (moduleNameTag, pAVISModule->m_ModuleName))
                    j++;

                if (TRUE == paramValue.GetValue (moduleFilePathTag, pAVISModule->m_ModuleFilePath))
                    j++;

                paramValue.GetValue (moduleArgumentsTag, pAVISModule->m_ModuleArguments);
                if (TRUE == paramValue.GetValue (moduleAutoStartTag, tmpValue))
                {
                    if (!tmpValue.CompareNoCase("Yes"))
                        pAVISModule->m_AutoRestart = TRUE;
                    else
                        pAVISModule->m_AutoRestart = FALSE;
                }
                else
                {
                    pAVISModule->m_AutoRestart = FALSE;
                }

                if (TRUE == paramValue.GetValue (moduleAutoStopTimeTag, tmpValue))
                {
                    long autoStopTime = atol (tmpValue);
                    if (autoStopTime >= 0 && autoStopTime <= 40320)
                        pAVISModule->m_AutoStopTime = autoStopTime;
                    else
                        pAVISModule->m_AutoStopTime = 0;
                }
                else
                {
                    pAVISModule->m_AutoStopTime = 0;
                }

                if (TRUE == paramValue.GetValue (moduleRestartTimeTag, tmpValue))
                {
                    long restartTime = atol (tmpValue);
                    if (restartTime >= 0 && restartTime <= 1440)
                        pAVISModule->m_RestartTime = restartTime;
                    else
                        pAVISModule->m_RestartTime = 0;
                }
                else
                {
                    pAVISModule->m_RestartTime = 0;
                }

                if (TRUE == paramValue.GetValue (moduleSendMailTag, tmpValue))
                {
                    if (!tmpValue.CompareNoCase("Yes"))
                        pAVISModule->m_SendMail = TRUE;
                    else
                        pAVISModule->m_SendMail = FALSE;
                }
                else
                {
                    pAVISModule->m_SendMail = TRUE;
                }

                if (j == 2)
                    m_ModuleList.Add (pAVISModule);
                else
                    delete pAVISModule;
            }
        }
        
        fclose (fp);
    }
}

/*-----------------------------------------------------------------------------
*   Function: MonitorSendAlert  
*   Description: 
*       Send mail alert to the system administrator 
*   Parameters: 
*       CAVISModule * pAVISModule -  
*       DWORD exitCode -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void MonitorSendAlert (CAVISModule * pAVISModule, DWORD exitCode)
{
    if (pAVISModule->m_SendMail != TRUE)
        return;

    CString msgStr;
    CString stopTime;

    // Do not send alert if the module was manually stopped by the user using the GUI
    // or the module was stopped because of AVISMonitor program termination
    if (exitCode == TERMINATE_USER_STOP || exitCode == TERMINATE_EXIT)
        return;

    stopTime = pAVISModule->m_ModuleStopTime.Format("%m/%d/%Y %H:%M:%S");

    switch (exitCode)
    {
    case TERMINATE_USER_STOP:
        msgStr.Format("Module <%s> on machine <%s> stopped by user at <%s>",
            pAVISModule->m_ModuleName,
            g_localComputerName,
            stopTime);
        break;

    case TERMINATE_AUTO_STOP:
        msgStr.Format("Module <%s> on machine <%s> stopped automatically at <%s>",
            pAVISModule->m_ModuleName,
            g_localComputerName,
            stopTime);
        break;

    case TERMINATE_INACTIVE:
        msgStr.Format("Module <%s> on machine <%s> is not responding, terminated at <%s>.",
            pAVISModule->m_ModuleName,
            g_localComputerName,
            stopTime);
        break;

    case TERMINATE_EXIT:
        msgStr.Format("Module <%s> on machine <%s> terminated at <%s> as AVISMonitor is exiting.",
            pAVISModule->m_ModuleName,
            g_localComputerName,
            stopTime);
        break;

    default:
        msgStr.Format("Module <%s> on machine <%s> exited at <%s> with exit code <%ld>.",
            pAVISModule->m_ModuleName,
            g_localComputerName,
            stopTime,
            (LONG) exitCode);
        break;
    }

    class CAVISSendMail avisSendMail;

    BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, msgStr);
    if (brc != TRUE)
    {
//        AfxMessageBox ("Error in sending mail.\n");
    }
    else
    {
//        AfxMessageBox ("Mail send successfully.\n");
    }
}

/*-----------------------------------------------------------------------------
*   Function: ThreadStartModule 
*   Description: 
*       Start all AVIS Modules on a separate thread 
*   Parameters: 
*       LPVOID ptr -  
*   Return: 
*       UINT -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
UINT ThreadStartModule(LPVOID ptr)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD exitCode;
    CAVISModule * pAVISModule = (CAVISModule *) ptr;

    // Use a mutex to make sure that 2 or more modules are not being started at
    // the same time.
    CMutex Mutex(FALSE, "AVISMonitorThreadStartModule");
    CSingleLock sLock (&Mutex, TRUE);

    // Make sure that the module is really stopped before attempting to start the module
    if (pAVISModule->m_ModuleStatus == MODULE_STOPPED)
    {
        memset (&si, 0, sizeof (STARTUPINFO));
        si.cb = sizeof (STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW;

        si.wShowWindow = SW_SHOWMINNOACTIVE;

        si.lpTitle = (LPSTR) ((LPCTSTR) pAVISModule->m_ModuleName);
        memset (&pi, 0, sizeof (PROCESS_INFORMATION));
        if ( CreateProcess (NULL,
                (LPSTR) ((LPCTSTR) (pAVISModule->m_ModuleFilePath + " " + pAVISModule->m_ModuleArguments)),
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                NULL,
                &si,
                &pi) )
        {
            // Store the process handle which is used to terminate the process if required
            pAVISModule->m_hProcess = pi.hProcess;
            // Store the processID  which is used to get the handles of the top level windows
            // of the module
            pAVISModule->m_ProcessID = pi.dwProcessId;
            pAVISModule->m_ModuleStatus = MODULE_STARTED;
            pAVISModule->m_WMCloseMessageSent = FALSE;
            pAVISModule->m_ModuleStartTime = COleDateTime::GetCurrentTime();
            pAVISModule->m_ModuleErrorCount = 0;
            pAVISModule->m_ModuleLastCheckTime = COleDateTime::GetCurrentTime();
            pAVISModule->m_ModuleCheckCount = 0;
            CloseHandle (pi.hThread);

            sLock.Unlock();
            // Wait forrever for the process to terminate, either naturally or by a call
            // to Terminate process
            WaitForSingleObject(pAVISModule->m_hProcess, INFINITE);
            sLock.Lock();

            GetExitCodeProcess(pAVISModule->m_hProcess, &exitCode);
            CloseHandle (pAVISModule->m_hProcess);
            pAVISModule->m_ModuleStopTime = COleDateTime::GetCurrentTime();
            pAVISModule->m_ModuleLastCheckTime = COleDateTime::GetCurrentTime();
            MonitorSendAlert (pAVISModule, exitCode);
            pAVISModule->m_hProcess = NULL;
            pAVISModule->m_ProcessID = 0;
            pAVISModule->m_ModuleStatus = MODULE_STOPPED;
            pAVISModule->m_ModuleExternalStarted = FALSE;
            pAVISModule->m_WMCloseMessageSent = FALSE;
            pAVISModule->m_OrigWorkingSetSize = 0;
            pAVISModule->m_OrigVMSize = 0;
            memset (&pAVISModule->m_MemCounter, 0, sizeof (pAVISModule->m_MemCounter));
        }
        else
        {
            pAVISModule->m_hProcess = NULL;
            pAVISModule->m_ProcessID = 0;
        }
    }
    sLock.Unlock();
    pAVISModule->m_pWinThread = NULL;
    return 0;
}

/*-----------------------------------------------------------------------------
*   Function: ThreadAttachModule 
*   Description: 
*       Attach to an existing process which has been started externally 
*   Parameters: 
*       LPVOID ptr -  
*   Return: 
*       UINT -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
UINT ThreadAttachModule(LPVOID ptr)
{
    CAVISModule * pAVISModule = (CAVISModule *) ptr;
    DWORD exitCode;

    CMutex Mutex(FALSE, "AVISMonitorThreadStartModule");
    CSingleLock sLock (&Mutex, FALSE);

    WaitForSingleObject(pAVISModule->m_hProcess, INFINITE);
    sLock.Lock();

    GetExitCodeProcess(pAVISModule->m_hProcess, &exitCode);
    CloseHandle (pAVISModule->m_hProcess);
    pAVISModule->m_ModuleStopTime = COleDateTime::GetCurrentTime();
    pAVISModule->m_ModuleLastCheckTime = COleDateTime::GetCurrentTime();
    MonitorSendAlert (pAVISModule, exitCode);
    pAVISModule->m_hProcess = NULL;
    pAVISModule->m_ProcessID = 0;
    pAVISModule->m_ModuleStatus = MODULE_STOPPED;
    pAVISModule->m_ModuleExternalStarted = FALSE;
    pAVISModule->m_WMCloseMessageSent = FALSE;
    pAVISModule->m_OrigWorkingSetSize = 0;
    pAVISModule->m_OrigVMSize = 0;
    memset (&pAVISModule->m_MemCounter, 0, sizeof (pAVISModule->m_MemCounter));
    sLock.Unlock();

    return 0;
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::StartModule 
*   Description: 
*       Start the module process in a seperate thread 
*   Parameters: 
*       CAVISModule *pAVISModule -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::StartModule(CAVISModule *pAVISModule)
{
    if (pAVISModule && pAVISModule->m_ModuleStatus == MODULE_STOPPED)
    {
        pAVISModule->m_pWinThread = AfxBeginThread (ThreadStartModule, pAVISModule);
        Sleep (1000);   // Wait for the thread to start
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::ModuleCheckStatus 
*   Description: 
*       Check the status of the ongoing module 
*   Parameters: 
*       CAVISModule *pAVISModule -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::ModuleCheckStatus(CAVISModule *pAVISModule)
{
    if (pAVISModule && pAVISModule->m_ModuleStatus != MODULE_STOPPED)
    {
#ifdef USE_FILE_BASED_CHECK
        // Check for the existancce of the moduole check file name which is the same as
        // the module name followed by ".lck". If the file is present, it is deleted
        // The module periodically recreates the file to signal that it is alive.
        // If during continuous check upto the limit specified by m_ModuleTerminateErrorCount
        // the file is not created by the module, it is assumed that the module is hung
        // and so it is terminated to be restarted during the next check time.
        if (pAVISModule->m_ModuleCheckFileName.IsEmpty())
        {
            pAVISModule->m_ModuleCheckFileName = pAVISModule->m_ModuleFilePath + ".lck";
        }

        FILE *fp;

        fp = fopen (pAVISModule->m_ModuleCheckFileName, "r");
        if (fp == (FILE *) NULL)
        {
            pAVISModule->m_ModuleStatus = MODULE_UNKNOWN;
            pAVISModule->m_ModuleErrorCount ++;
            if (pAVISModule->m_ModuleErrorCount >= m_ModuleTerminateErrorCount)
            {
                TerminateProcess (pAVISModule->m_hProcess, TERMINATE_INACTIVE);
                Sleep (1000);
            }
        }
        else
        {
            fclose (fp);
            pAVISModule->m_ModuleStatus = MODULE_STARTED;
            pAVISModule->m_ModuleErrorCount = 0;
            unlink (pAVISModule->m_ModuleCheckFileName);
        }
#else //USE_FILE_BASED CHECK
        // Use the functions from AVISMonitor lib to check if the module is
        // alive. This uses a semaphore to verify that during subsequent calls
        // to the function, the semaphore values does get changed signalling
        // that the module is alive

        // None of the AVIS modules are currently using this mechanism
        // and so the status returned is always active

        int checkRC;
        LONG newCount;

        checkRC =  AVISMonitorCheck((LPTSTR) ((LPCTSTR)pAVISModule->m_ModuleFilePath),
            pAVISModule->m_ModuleCheckCount, &newCount);

        switch (checkRC)
        {
        case AVIS_MONITOR_ACTIVE :
            pAVISModule->m_ModuleStatus = MODULE_STARTED;
            pAVISModule->m_ModuleErrorCount = 0;
            pAVISModule->m_ModuleCheckCount = newCount;
            break;

        case AVIS_MONITOR_ERROR :
        case AVIS_MONITOR_INACTIVE :
        case AVIS_MONITOR_UNKNOWN :
        default:
            pAVISModule->m_ModuleStatus = MODULE_UNKNOWN;
            pAVISModule->m_ModuleErrorCount ++;
            pAVISModule->m_ModuleCheckCount = newCount;
            // Automatic termination of the module if it is detected as inactive is 
            // currently disabled
            /*
            if (pAVISModule->m_ModuleErrorCount >= m_ModuleTerminateErrorCount)
            {
                TerminateProcess (pAVISModule->m_hProcess, TERMINATE_INACTIVE);
                Sleep (1000);
            }
            */
        }

#endif //USE_FILE_BASED CHECK
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::ModuleCheckExternalStatus 
*   Description: 
*       Check to see if the module has been started externally by the user 
*   Parameters: 
*       CAVISModule *pAVISModule -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::ModuleCheckExternalStatus(CAVISModule *pAVISModule)
{
    if (pAVISModule && pAVISModule->m_ModuleStatus == MODULE_STOPPED)
    {
        char *moduleBaseName;

        moduleBaseName = strrchr (pAVISModule->m_ModuleFilePath, '\\');
        if (!moduleBaseName)
            moduleBaseName = strrchr (pAVISModule->m_ModuleFilePath, ':');

        if (!moduleBaseName)
            moduleBaseName = (LPTSTR) ((LPCTSTR)pAVISModule->m_ModuleFilePath);
        else
            moduleBaseName++;

		//The following change for PERL module check was added on
		//June/02/2000. If we start some PERL (1.pl, 2.pl etc.) programs
		//through the AVIS monitor program, it may wrongly attach the
		//idle perl programs to any other actively running perl programs.
		//Because, the EnumProcess function will return PERL.EXE as the
		//active process name and not the 1.pl or 2.pl etc. It will match
		//with the module name specified for those programs.
		//(e-g: d:\perl\bin\perl.exe d:\avis\avisdbexport.pl). Because of
		//the match, it will randomly and wrongly attach to some other
		//perl process. Hence, we decided not to do this check for Perl
		//programs.
		char tmpModuleBaseName [5024];
		strcpy (tmpModuleBaseName, moduleBaseName);
		strupr (tmpModuleBaseName);

		if (strstr (tmpModuleBaseName, "PERL"))
		{
			//ModuleBaseName contains a string "PERL" in it.
			//Let us not bother about attaching to the externally started PERL module.
			return;
		}

        DWORD aProcesses[2048];
        DWORD cbNeeded;
        DWORD cProcesses;
        UINT i;

        // Get the list of all ongoing processes and look for the name of the process
        // If there is a process already running with the same name then attach to the
        // process
        if ( !EnumProcesses (aProcesses, sizeof (aProcesses), &cbNeeded) )
            return;

        cProcesses = cbNeeded / sizeof (DWORD);

        for (i = 0; i < cProcesses; i++)
        {
            char szProcessName[MAX_PATH] = "unknown";

            HANDLE hProcess = OpenProcess (
                PROCESS_ALL_ACCESS,
                FALSE,
                aProcesses[i]);

            if (hProcess)
            {
                HMODULE hMod;

                if (EnumProcessModules (hProcess, &hMod, sizeof (hMod), &cbNeeded))
                {
                    GetModuleBaseName (hProcess, hMod, szProcessName, sizeof(szProcessName));
                    if (!stricmp (szProcessName, moduleBaseName))
                    {
                        pAVISModule->m_ModuleExternalStarted = TRUE;
                        pAVISModule->m_ModuleStatus = MODULE_STARTED;
                        pAVISModule->m_WMCloseMessageSent = FALSE;
                        pAVISModule->m_ModuleErrorCount = 0;
                        pAVISModule->m_ModuleStartTime = COleDateTime::GetCurrentTime();
                        pAVISModule->m_ModuleLastCheckTime = COleDateTime::GetCurrentTime();
                        pAVISModule->m_ModuleCheckCount = 0;
                        pAVISModule->m_hProcess = hProcess;
                        pAVISModule->m_ProcessID = aProcesses[i];
                        pAVISModule->m_pWinThread = AfxBeginThread (ThreadAttachModule, pAVISModule);
                        return;
                    }
                }
                CloseHandle (hProcess);
            }
        }
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::TerminateAllOngoingProcess 
*   Description: 
*       Terminate all the ongoing process as the AVISMonitor program is  
*       exiting 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::TerminateAllOngoingProcess()
{
    CWaitCursor waitCursor;
    int modCount = m_ModuleList.GetSize();
    int i;
    BOOL someProcessTerminated = FALSE;

    KillTimer(MODULE_CHECK_TIMER);
    KillTimer(VIEW_REFRESH_TIMERID);
    KillTimer(MODULE_AUTO_START_TIMERID);
    KillTimer(MODULE_AUTO_STOP_TIMERID);
    for (i = 0; i < modCount; i++)
    {
	    CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];
        if (pAVISModule)
        {
            BOOL processTerminated = FALSE;
            CMutex Mutex(FALSE, "AVISMonitorThreadStartModule");
            CSingleLock sLock (&Mutex, TRUE);
            if (pAVISModule->m_ModuleStatus != MODULE_STOPPED &&
                pAVISModule->m_hProcess)
            {
                TerminateProcess (pAVISModule->m_hProcess, TERMINATE_EXIT);
                processTerminated = TRUE;
                someProcessTerminated = TRUE;
            }
            sLock.Unlock();
            if (processTerminated == TRUE)
            {
                while (pAVISModule->m_ModuleStatus != MODULE_STOPPED)
                    Sleep (1000);
            }
        }
    }
    if (someProcessTerminated == TRUE)
        Sleep(3000);    // Allow all process threads to die
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::WriteModuleInfoToFile 
*   Description: 
*       Write current statistical info about all the modules to the file on  
*       disk 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::WriteModuleInfoToFile()
{
    CString bakModInfoFile ((LPCTSTR) "AVISMonitorStat.lst.bak");
    CString modInfoFile ((LPCTSTR) "AVISMonitorStat.lst");
    CString bakModuleInfoFile = m_ModulePath + bakModInfoFile;
    CString moduleInfoFile = m_ModulePath + modInfoFile;

    unlink (bakModuleInfoFile);
    rename (moduleInfoFile, bakModuleInfoFile);

    FILE *fp;

    fp = fopen (moduleInfoFile, _T("w"));

    if (fp)
    {
        CString curTimeTag ("Current Time");
        CString NameString((LPCSTR) "Module Name");
        CString FilePathString((LPCSTR) "Program File Name");
        CString StartTimeString((LPCSTR) "Start Time");
        CString StopTimeString((LPCSTR) "Stop Time");
        CString LastCheckTimeString((LPCSTR) "Last Check Time");
        CString StatusString((LPCSTR) "Status");
//        CString ErrorCountString((LPCSTR) "Error Count");

        CString tmpStr;

        CParamValue paramValue;
        /*
        COleDateTime curTime = COleDateTime::GetCurrentTime();
        CString curTimeString = curTime.Format("%m/%d/%Y %H:%M:%S");

        paramValue.AddParamValue (curTimeTag, curTimeString);
        paramValue.WriteParamValue (fp);

        paramValue.ResetValues();
        */

        int moduleCount = m_ModuleList.GetSize();
        for (int i = 0; i < moduleCount; i++)
        {
	        CAVISModule * pAVISModule = (CAVISModule *) m_ModuleList[i];

            if (pAVISModule)
            {
                CString moduleCommand;
                moduleCommand = pAVISModule->m_ModuleFilePath + " " + pAVISModule->m_ModuleArguments;
                paramValue.AddParamValue(NameString, pAVISModule->m_ModuleName);
                paramValue.AddParamValue(FilePathString, moduleCommand);
                switch (pAVISModule->m_ModuleStatus)
                {
                case MODULE_STARTED:
                    tmpStr = "Active";
                    break;

                case MODULE_STOPPED:
                    if (pAVISModule->m_ModuleStopTime.GetStatus() == COleDateTime::valid)
                        tmpStr = "Stopped";
                    else
                        tmpStr = "Not Started";
                    break;

                case MODULE_UNKNOWN:
                default:
                    tmpStr = "Unknown";
                    break;
                }
                paramValue.AddParamValue(StatusString, tmpStr);

                if (pAVISModule->m_ModuleStatus != MODULE_STOPPED)
                    tmpStr = pAVISModule->m_ModuleStartTime.Format("%m/%d/%Y %H:%M:%S");
                else
                    tmpStr.Empty();
                paramValue.AddParamValue(StartTimeString, tmpStr);

                if (pAVISModule->m_ModuleStatus == MODULE_STOPPED)
                {
                    if (pAVISModule->m_ModuleStopTime.GetStatus() == COleDateTime::valid)
                        tmpStr = pAVISModule->m_ModuleStopTime.Format("%m/%d/%Y %H:%M:%S");
                    else
                        tmpStr.Empty();
                }
                else
                    tmpStr.Empty();
                paramValue.AddParamValue(StopTimeString, tmpStr);

                tmpStr = pAVISModule->m_ModuleLastCheckTime.Format("%m/%d/%Y %H:%M:%S");
                paramValue.AddParamValue(LastCheckTimeString, tmpStr);

                /*
                tmpStr.Format ("%d", pAVISModule->m_ModuleErrorCount);
                paramValue.AddParamValue(ErrorCountString, tmpStr);
                */

                paramValue.WriteParamValue (fp);

                paramValue.ResetValues();
            }
        }
        fclose (fp);
    }

}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnContextMenu 
*   Description: 
*       Display the actions popup menu on right click 
*   Parameters: 
*       CWnd* pWnd -  Not required
*       CPoint point -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnContextMenu(CWnd* /*pWnd*/, CPoint point) 
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

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::ReadConfigFile 
*   Description: 
*       Read AVISMonitor configuration values from the file 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::ReadConfigFile()
{
    CParamValue paramValue;
    FILE * fp;
    CString moduleCheckInterval ("Module Check Interval (minutes)");
//    CString moduleAutoRestart ("Restart Stopped Module");
//    CString moduleAutoRestartCount ("Error Count To Terminate Module");
    CString moduleStartOnStartup ("Start Modules On Monitor Startup");

    CString confFile ("AVISMonitor.cfg");
    CString configFile = m_ModulePath + confFile;

    fp = fopen (configFile, _T("r"));
    if (fp != (FILE *) NULL)
    {
        int numParam = paramValue.ReadParamValue (fp, FALSE, FALSE);

        if (numParam)
        {
            CString tmpValue;

            if (TRUE == paramValue.GetValue (moduleCheckInterval, tmpValue))
                m_ModuleCheckInterval = atoi (tmpValue);
            else
                m_ModuleCheckInterval = 1;

//            if (TRUE == paramValue.GetValue (moduleAutoRestartCount, tmpValue))
//                m_ModuleTerminateErrorCount = atoi (tmpValue);
//            else
//                m_ModuleTerminateErrorCount = 3;

//            if (TRUE == paramValue.GetValue (moduleAutoRestart, tmpValue))
//            {
//                if (!tmpValue.CompareNoCase ("TRUE"))
//                    m_ModuleAutoRestart = TRUE;
//                else
//                    m_ModuleAutoRestart = FALSE;
//            }
//            else
//                m_ModuleAutoRestart = FALSE;

            if (TRUE == paramValue.GetValue (moduleStartOnStartup, tmpValue))
            {
                if (!tmpValue.CompareNoCase ("TRUE"))
                    m_ModuleStartOnStartup = TRUE;
                else
                    m_ModuleStartOnStartup = FALSE;
            }
            else
                m_ModuleStartOnStartup = TRUE;
        }

        if (m_ModuleCheckInterval < 0)
            m_ModuleCheckInterval = 0;
        if (m_ModuleCheckInterval > 30)
            m_ModuleCheckInterval = 30;

//        if (m_ModuleTerminateErrorCount < 0)
//            m_ModuleTerminateErrorCount = 0;
//        if (m_ModuleTerminateErrorCount > 32000)
//            m_ModuleTerminateErrorCount = 32000;

        fclose (fp);
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::SaveConfigFile 
*   Description: 
*       Save AVISMonitor Configuration values to the file 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::SaveConfigFile()
{
    CParamValue paramValue;
    FILE * fp;
    CString moduleCheckInterval ("Module Check Interval (minutes)");
//    CString moduleAutoRestart ("Restart Stopped Module");
//    CString moduleAutoRestartCount ("Error Count To Terminate Module");
    CString moduleStartOnStartup ("Start Modules On Monitor Startup");

    CString confFile ("AVISMonitor.cfg");
    CString configFile = m_ModulePath + confFile;
    CString bakConfigFile;

    bakConfigFile = configFile + ".Bak";

    unlink (bakConfigFile);

    rename (configFile, bakConfigFile);

    fp = fopen (configFile, _T("w"));

    if (fp)
    {
        CString tmpString;
		CString prologForCfgFile((LPCSTR) IDS_CFGFILE_PROLOG);

		//Added on May/17/2000. Write the prolog for the list file.
		fwrite ((LPCSTR )prologForCfgFile, prologForCfgFile.GetLength(), 1, fp);

        tmpString.Format("%d", m_ModuleCheckInterval);
        paramValue.AddParamValue (moduleCheckInterval, tmpString);

//        tmpString.Format("%d", m_ModuleTerminateErrorCount);
//        paramValue.AddParamValue (moduleAutoRestartCount, tmpString);

//        if (m_ModuleAutoRestart == TRUE)
//            tmpString = "TRUE";
//        else
//            tmpString = "FALSE";
//        paramValue.AddParamValue (moduleAutoRestart, tmpString);

        if (m_ModuleStartOnStartup == TRUE)
            tmpString = "TRUE";
        else
            tmpString = "FALSE";
        paramValue.AddParamValue (moduleStartOnStartup, tmpString);

        paramValue.WriteParamValue (fp);

        fclose (fp);
    }
}

/*-----------------------------------------------------------------------------
*   Function: CALLBACK SendAppWMClose 
*   Description: 
*       Send WM_CLOSE message to the top level windows of the module 
*   Parameters: 
*       HWND hwnd -  
*       LPARAM lParam -  
*   Return: 
*       BOOL -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
BOOL CALLBACK SendAppWMClose( HWND hwnd, LPARAM lParam )
{
    DWORD dwID ;

    GetWindowThreadProcessId(hwnd, &dwID) ;

    if(dwID == (DWORD)lParam)
    {
        PostMessage(hwnd, WM_CLOSE, 0, 0) ;
    }

    return TRUE ;
}


/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::SendWMCloseMessage 
*   Description: 
*       Call EnumWindows to enumerate all the top level windows on the system  
*       and try to send WM_CLOSE to the required process 
*   Parameters: 
*       CAVISModule *pAVISModule -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::SendWMCloseMessage(CAVISModule *pAVISModule)
{
    EnumWindows ((WNDENUMPROC)SendAppWMClose, (LPARAM) pAVISModule->m_ProcessID) ;
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnActionsEdit 
*   Description: 
*       Edit the module information 
*   Parameters: 
*       None 
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnActionsEdit() 
{
    CAVISModule * pModule;

    pModule = GetCurrentSelection();
    if (!pModule)
        return;

    CModuleDialog moduleDialog;
    int rc;

    moduleDialog.m_ModuleName = pModule->m_ModuleName;
    moduleDialog.m_ModuleFilePath = pModule->m_ModuleFilePath;
    moduleDialog.m_ModuleArguments = pModule->m_ModuleArguments;
    moduleDialog.m_AutoRestart = pModule->m_AutoRestart;
    moduleDialog.m_AutoStopTime = pModule->m_AutoStopTime;
    moduleDialog.m_RestartTime = pModule->m_RestartTime;
    moduleDialog.m_SendMail = pModule->m_SendMail;

    rc = moduleDialog.DoModal();

    if (rc == IDOK)
    {
        if (!moduleDialog.m_ModuleName.IsEmpty() &&
            !moduleDialog.m_ModuleFilePath.IsEmpty())
        {
            CListCtrl & ListCtrl = GetListCtrl();

            pModule->m_ModuleName = moduleDialog.m_ModuleName;
            pModule->m_ModuleFilePath = moduleDialog.m_ModuleFilePath;
            pModule->m_ModuleArguments = moduleDialog.m_ModuleArguments;
            pModule->m_AutoRestart = moduleDialog.m_AutoRestart;
            pModule->m_AutoStopTime = moduleDialog.m_AutoStopTime;
            pModule->m_RestartTime = moduleDialog.m_RestartTime;
            pModule->m_SendMail = moduleDialog.m_SendMail;

            pModule->m_Saved = FALSE;
            m_ModuleModified = TRUE;

            ListCtrl.SetRedraw(FALSE);
            ListCtrl.SortItems(CompareFunc, (LPARAM) this);
            ListCtrl.SetRedraw(TRUE);
            OnUpdate(NULL, 0, NULL);
            UpdateWindow();
        }
    }
}

/*-----------------------------------------------------------------------------
*   Function: CAVISMonitorView::OnUpdateActionsEdit 
*   Description: 
*       None 
*   Parameters: 
*       CCmdUI* pCmdUI -  
*   Return: 
*       void -  
*   Designed by: Srikant jalan
-----------------------------------------------------------------------------*/
void CAVISMonitorView::OnUpdateActionsEdit(CCmdUI* pCmdUI) 
{
    CAVISModule * pModule;

    pModule = GetCurrentSelection();

    if (pModule)
        pCmdUI->Enable (TRUE);
    else
        pCmdUI->Enable (FALSE);
}
