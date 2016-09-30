// MachineStatistics.cpp : implementation file
//

#include "stdafx.h"
#include "AVISDFRL.h"
#include "MachineStatistics.h"
#include "Paramvalue.h"
#include "AVISSendMail.h"
#include "DFMsg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMachineStatistics dialog

// Dialog to display the machine statistics of the analysis machines.
// The list of machines is selected from the list of available resourecs
// If NT Machine name is specified, it is assumed as a Windows NT machine
// otherwise it is taken as an AIX machine. The actual task of collecting the
// machine statistics is done in the file PerformanceMonitor.cpp

CMachineStatistics::CMachineStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CMachineStatistics::IDD, pParent) , m_Mutex (FALSE, "ISDFMachineStatisticsMutex")
{
	//{{AFX_DATA_INIT(CMachineStatistics)
	m_RefreshIntervalString = _T("");
	//}}AFX_DATA_INIT
    m_MachineList.SetSize(0, 10);
    m_NTMachineNameList.SetSize(0, 10);
    m_UserList.SetSize(0, 10);
    m_PerfMonList.SetSize(0, 10);
    m_ThreadCount = 0;
    m_AutoSized = FALSE;
    m_RefreshIntervalValue = 1;
	m_RefreshIntervalString.Format("%d", m_RefreshIntervalValue);
}

CMachineStatistics::~CMachineStatistics()
{
    int machineCount = m_PerfMonList.GetSize();
    for (int i = 0; i < machineCount; i++)
    {
        CPerformanceMonitor * pPerfMon;

        pPerfMon = m_PerfMonList[i];
        if (pPerfMon)
            delete pPerfMon;
    }
    m_PerfMonList.RemoveAll();
}

void CMachineStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMachineStatistics)
	DDX_Control(pDX, IDC_LIST_MACHINES, m_MachineListCtrl);
	DDX_Text(pDX, IDC_EDIT_REFRESH_INTERVAL, m_RefreshIntervalString);
	DDV_MaxChars(pDX, m_RefreshIntervalString, 4);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMachineStatistics, CDialog)
	//{{AFX_MSG_MAP(CMachineStatistics)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_REFRESH_INTERVAL, OnChangeEditRefreshInterval)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMachineStatistics message handlers

BOOL CMachineStatistics::OnInitDialog() 
{
	CDialog::OnInitDialog();
    CWaitCursor waitCursor;
    LV_COLUMN lvc;
	
	CString MachineString((LPCSTR) IDS_DF_IP_ADDRESS_STRING);
    CString ProcessorBusyString ("CPU Busy");
    CString UserString ("User");
    CString KernelString("Kernel");
    CString MemoryAvailableString ("Mem Available");
    CString CommittedString ("Committed");
    CString MaxCommittedString ("Commit Limit");
    CString StartTime ("Up Time");
    CString DiskFree ("Disk Space Free");

    lvc.mask=LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

    lvc.iSubItem=0;
    lvc.pszText=(LPTSTR) ((LPCTSTR)MachineString);
    lvc.cx=4 * m_MachineListCtrl.GetStringWidth((LPCTSTR)MachineString) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_MachineListCtrl.InsertColumn(0,&lvc);

    lvc.iSubItem=1;
    lvc.pszText=(LPTSTR) ((LPCTSTR)ProcessorBusyString);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)ProcessorBusyString) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    m_MachineListCtrl.InsertColumn(1,&lvc);

    lvc.iSubItem=2;
    lvc.pszText=(LPTSTR) ((LPCTSTR)UserString);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)UserString) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    m_MachineListCtrl.InsertColumn(2,&lvc);

    lvc.iSubItem=3;
    lvc.pszText=(LPTSTR) ((LPCTSTR)KernelString);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)KernelString) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    m_MachineListCtrl.InsertColumn(3,&lvc);

    lvc.iSubItem=4;
    lvc.pszText=(LPTSTR) ((LPCTSTR)MemoryAvailableString);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)MemoryAvailableString) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    m_MachineListCtrl.InsertColumn(4,&lvc);

    lvc.iSubItem=5;
    lvc.pszText=(LPTSTR) ((LPCTSTR)CommittedString);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)CommittedString) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    m_MachineListCtrl.InsertColumn(5,&lvc);

    lvc.iSubItem=6;
    lvc.pszText=(LPTSTR) ((LPCTSTR)MaxCommittedString);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)MaxCommittedString) + 20;
    lvc.fmt=LVCFMT_RIGHT;
    m_MachineListCtrl.InsertColumn(6,&lvc);

    lvc.iSubItem=7;
    lvc.pszText=(LPTSTR) ((LPCTSTR)StartTime);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)StartTime) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_MachineListCtrl.InsertColumn(7,&lvc);

    lvc.iSubItem=8;
    lvc.pszText=(LPTSTR) ((LPCTSTR)DiskFree);
    lvc.cx=m_MachineListCtrl.GetStringWidth((LPCTSTR)DiskFree) + 20;
    lvc.fmt=LVCFMT_LEFT;
    m_MachineListCtrl.InsertColumn(8,&lvc);

    m_MachineListCtrl.AddHeaderToolTip(0, "Immune System machine name");
    m_MachineListCtrl.AddHeaderToolTip(1, "Total procesor utilization");
    m_MachineListCtrl.AddHeaderToolTip(2, "Processor utilization in user mode");
    m_MachineListCtrl.AddHeaderToolTip(3, "Processor utilization in kernel mode");
    m_MachineListCtrl.AddHeaderToolTip(4, "Available physical memory");
    m_MachineListCtrl.AddHeaderToolTip(5, "Total committed memory");
    m_MachineListCtrl.AddHeaderToolTip(6, "Maximum committed memory");
    m_MachineListCtrl.AddHeaderToolTip(7, "Start time of the machine");
    m_MachineListCtrl.AddHeaderToolTip(8, "Free disk space");

    int machineCount = m_MachineList.GetSize();

    for (int i = 0; i < machineCount; i++)
    {
        LV_ITEM lvi;
        int nItem;
        CPerformanceMonitor * pPerfMon;

        lvi.iItem=i;
        lvi.mask=LVIF_TEXT | LVIF_STATE;
        lvi.iSubItem=0;
        lvi.pszText=m_MachineList[i].GetBuffer(1024);
        lvi.stateMask=LVIS_SELECTED | LVIS_FOCUSED;
        lvi.state = 0;

        nItem = m_MachineListCtrl.InsertItem(&lvi);
        m_MachineListCtrl.SetItemData(nItem, (DWORD) i);

        pPerfMon = new CPerformanceMonitor(m_MachineList[i], 
            m_NTMachineNameList[i],
            m_UserList[i]);

        m_PerfMonList.Add(pPerfMon);
    }

    m_MachineListCtrl.AutoSizeColumns( 0 );
    m_MachineListCtrl.AutoSizeColumns( 1 );
    m_MachineListCtrl.AutoSizeColumns( 2 );
    m_MachineListCtrl.AutoSizeColumns( 3 );
    m_MachineListCtrl.AutoSizeColumns( 4 );
    m_MachineListCtrl.AutoSizeColumns( 5 );
    m_MachineListCtrl.AutoSizeColumns( 6 );
    m_MachineListCtrl.AutoSizeColumns( 7 );
    m_MachineListCtrl.AutoSizeColumns( 8 );

    SetTimer (STATISTICS_CHECK_TIMERID, 10, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// The actual time required to collect all the statistics of all the
// available machines may be large. Therefore the process is done in a
// separate thread so that the job processing is not affected. Once the
// statistics check timerid is triggered, a new thread is started to check
// the statistics. A new timer thread_timerid is started to monitor the end
// of the thread. After the thread is completed, the statistics are displayed
// and the check_timerid is reactivated

UINT StartMachineStatistics(LPVOID ptr)
{
    CMachineStatistics * pMachineStatistics = (CMachineStatistics *) ptr;
    pMachineStatistics->m_ThreadCount = 1;

    pMachineStatistics->DFLauncherRefreshStatistics();
    pMachineStatistics->m_StatThread = NULL;
    pMachineStatistics->m_ThreadCount = 0;
    if (pMachineStatistics->m_ThreadStop == TRUE)
        return 1;
    return 0;
}

void CMachineStatistics::OnTimer(UINT nIDEvent) 
{
    if (nIDEvent == STATISTICS_CHECK_TIMERID)
    {
        KillTimer (STATISTICS_CHECK_TIMERID);
        m_ThreadStop = FALSE;
        m_StatThread = AfxBeginThread (StartMachineStatistics, this);
        SetTimer (STATISTICS_THREAD_TIMERID, 2000, NULL);
    }
    if (nIDEvent == STATISTICS_THREAD_TIMERID)
    {
        KillTimer (STATISTICS_THREAD_TIMERID);
        if (m_ThreadCount)
            SetTimer (STATISTICS_THREAD_TIMERID, 1000, NULL);
        else
        {
            RefreshDisplay();
            SetTimer (STATISTICS_CHECK_TIMERID, m_RefreshIntervalValue * 1000, NULL);
        }
    }
}

void CMachineStatistics::DFLauncherRefreshStatistics()
{
    CSingleLock sLock(&m_Mutex, TRUE);
    int machineCount = m_MachineList.GetSize();

    // The statistics are collected twice so that the instance statistics
    // counter are used and not the counter over a longer period of time.
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < machineCount; i++)
        {
            CPerformanceMonitor * pPerfMon;

            pPerfMon = m_PerfMonList[i];

            // Check if the user has pressed cancel to exit the dialog
            if (m_ThreadStop == TRUE)
                return;

            // The actual performance data is collected for each machine
            if (pPerfMon)
                pPerfMon->GetPerformanceData();
        }

        // There should be atleast 1 seconds sleep between the counters
        // else the counter values are not returned correctly.
        if (!j)
            Sleep (1000);
    }

}

void CMachineStatistics::OnChangeEditRefreshInterval() 
{
    KillTimer (STATISTICS_CHECK_TIMERID);
    KillTimer (STATISTICS_THREAD_TIMERID);
    // The current thread which is running to collect the statistics is
    // abandoned
    m_ThreadStop = TRUE;
    // Wait till the thread is really killed
    while (m_ThreadCount)
    {
        Sleep(500);
    }

    UpdateData(TRUE);
    m_RefreshIntervalValue = atoi (m_RefreshIntervalString);
    // Whatever the value specified for the refresh interval. Get the
    // first value after 5 seconds. After that the refresh interval value 
    // will be honored
    if (m_RefreshIntervalValue > 0)
        SetTimer (STATISTICS_CHECK_TIMERID, 5 * 1000, NULL);
}

// Save the statistics to a file "ISMachineStat.lst". This file is created
// periodically once every 30 minutes as specified in DFLauncherView.cpp
/* Format of this file is as follows
CurrentTime : 03/02/1999 14:47:05

Machine : SRIX2
NT Machine Name : SRIX2
CPU Busy : 2 %
User : 1 %
Kernel : 2 %
Mem Available : 68688 K
Committed : 146660 K
Commit Limit : 259672 K
Up Time : 20-02:37:52
Disk Space Free : C:  275 MB ( 26 %) E:  737 MB ( 74 %) F:  771 MB ( 31 %)
*/
void CMachineStatistics::SaveToFile(CString statFile)
{
    CSingleLock sLock(&m_Mutex, TRUE);
    CParamValue paramValue;
    FILE *fp;

    CString bakStatFile;

    bakStatFile = statFile + ".bak";
//    unlink (bakStatFile);
//    rename (statFile, bakStatFile);

    CopyFile(statFile, bakStatFile, FALSE);
    fp = fopen (statFile, _T("w"));

    if (fp)
    {
        CString curTimeTag ("CurrentTime");
	    CString MachineString((LPCSTR) IDS_DF_IP_ADDRESS_STRING);
        CString NTMachineNameString ("NT Machine Name");
        CString ProcessorBusyString ("CPU Busy");
        CString UserString ("User");
        CString KernelString("Kernel");
        CString MemoryAvailableString ("Mem Available");
        CString CommittedString ("Committed");
        CString MaxCommittedString ("Commit Limit");
        CString StartTime ("Up Time");
        CString DiskFree ("Disk Space Free");

        CParamValue paramValue;
        COleDateTime curTime = COleDateTime::GetCurrentTime();
        CString curTimeString = curTime.Format("%m/%d/%Y %H:%M:%S");

        paramValue.AddParamValue (curTimeTag, curTimeString);
        paramValue.WriteParamValue (fp);

        int machineCount = m_MachineList.GetSize();

        for (int i = 0; i < machineCount; i++)
        {
            CPerformanceMonitor * pPerfMon;

            pPerfMon = new CPerformanceMonitor(m_MachineList[i],
                m_NTMachineNameList[i],
                m_UserList[i]);

            m_PerfMonList.Add(pPerfMon);
        }

        for (i = 0; i < machineCount; i++)
        {
            CPerformanceMonitor * pPerfMon;
            pPerfMon = m_PerfMonList[i];

            pPerfMon->GetPerformanceData();
        }

        Sleep (1000);

        for (i = 0; i < machineCount; i++)
        {
            CPerformanceMonitor * pPerfMon;
            pPerfMon = m_PerfMonList[i];

            pPerfMon->GetPerformanceData();
        }

        for (i = 0; i < machineCount; i++)
        {
            CParamValue paramValue;

            CPerformanceMonitor * pPerfMon;
            pPerfMon = m_PerfMonList[i];

            paramValue.AddParamValue (MachineString, m_MachineList[i]);
            paramValue.AddParamValue (NTMachineNameString, m_NTMachineNameList[i]);

            if (pPerfMon->m_ErrorCode == ERROR_SUCCESS)
            {
                CString dispStr;

                dispStr.Format ("%d %%", pPerfMon->processorTime);
                paramValue.AddParamValue (ProcessorBusyString, dispStr);

                dispStr.Format ("%d %%", pPerfMon->userTime);
                paramValue.AddParamValue (UserString, dispStr);

                dispStr.Format ("%d %%", pPerfMon->kernelTime);
                paramValue.AddParamValue (KernelString, dispStr);

                dispStr.Format ("%ld K", (long) (pPerfMon->availableMemoryCount/1024));
                paramValue.AddParamValue (MemoryAvailableString, dispStr);

                dispStr.Format ("%ld K", (long) (pPerfMon->commitedMemoryCount/1024));
                paramValue.AddParamValue (CommittedString, dispStr);

                dispStr.Format ("%ld K", (long) (pPerfMon->maxCommitedMemoryCount/1024));
                paramValue.AddParamValue (MaxCommittedString, dispStr);

                paramValue.AddParamValue (StartTime, pPerfMon->systemUpTimeStr);

                paramValue.AddParamValue (DiskFree, pPerfMon->diskFreeString);
            }
            else
            {
                CString dispStr = _T("N/A");

                paramValue.AddParamValue (ProcessorBusyString, dispStr);
                paramValue.AddParamValue (UserString, dispStr);
                paramValue.AddParamValue (KernelString, dispStr);
                paramValue.AddParamValue (MemoryAvailableString, dispStr);
                paramValue.AddParamValue (CommittedString, dispStr);
                paramValue.AddParamValue (MaxCommittedString, dispStr);
                paramValue.AddParamValue (StartTime, dispStr);
                paramValue.AddParamValue (DiskFree, dispStr);
            }
            paramValue.WriteParamValue (fp);

            if (pPerfMon->diskSpaceLow == TRUE)
            {
                class CAVISSendMail avisSendMail;
                CString msgStr;
                COleDateTime curTime = COleDateTime::GetCurrentTime();

                msgStr.Format ("Disk space low on machine <%s> at <%s>.\n"
                               "Free disk space for all drives: <%s>.\n" ,
                    m_MachineList[i],
                    curTime.Format("%m/%d/%Y %H:%M:%S"),
                    pPerfMon->diskFreeString) ;

                BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, msgStr);
            }
            if (pPerfMon->memoryLow == TRUE)
            {
                class CAVISSendMail avisSendMail;
                CString msgStr;
                COleDateTime curTime = COleDateTime::GetCurrentTime();

                msgStr.Format ("Memory low on machine <%s> at <%s>.\n"
                               "%s: <%ld K>.\n"
                               "%s: <%ld K>.\n" ,
                    m_MachineList[i],
                    curTime.Format("%m/%d/%Y %H:%M:%S"),
                    MaxCommittedString,
                    (long) (pPerfMon->maxCommitedMemoryCount/1024),
                    CommittedString,
                    (long) (pPerfMon->commitedMemoryCount/1024)) ;

                BOOL brc = avisSendMail.AVISSendMail (AVISSENDMAIL_PROFILE_DATAFLOW_TYPE_INDEX, msgStr);
            }
            if (!(m_NTMachineNameList[i].IsEmpty()))
            {
                if (CheckRSHStatus (m_MachineList[i], m_UserList[i]) == FALSE)
                {
                    char *machineName = new char[256];

                    machineName[0] = '\0';
                    strcpy (machineName, m_MachineList[i]);

                    (AfxGetMainWnd())->PostMessage (DFLAUNCHER_DISABLE_MACHINE, 0,
                        (LPARAM) machineName);
                }
            }
        }
        fclose (fp);
    }
}

IMPLEMENT_DYNCREATE(CMStatDialogThread, CWinThread)

BOOL CMStatDialogThread::InitInstance()
{
    CMachineStatistics MStatDialog;
    MStatDialog.DoModal();
    return FALSE;
}

void CMachineStatistics::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
    KillTimer (STATISTICS_CHECK_TIMERID);
    KillTimer (STATISTICS_THREAD_TIMERID);
    // Signal the thread to terminate
    m_ThreadStop = TRUE;
    // Wait for the thread to actually terminate
    while (m_ThreadCount)
    {
        Sleep(500);
    }
	CDialog::OnClose();
}

void CMachineStatistics::OnOK() 
{
	// TODO: Add extra validation here
	
    KillTimer (STATISTICS_CHECK_TIMERID);
    KillTimer (STATISTICS_THREAD_TIMERID);
    m_ThreadStop = TRUE;
    while (m_ThreadCount)
    {
        Sleep(500);
    }
	CDialog::OnOK();
}

// Display the statistics on screen
void CMachineStatistics::RefreshDisplay()
{
    int itemCount = m_MachineListCtrl.GetItemCount();

    m_MachineListCtrl.SetRedraw (FALSE);
    for (int i = 0; i < itemCount; i++)
    {
        int nItem;
        int nMachine;
        CPerformanceMonitor * pPerfMon;

        nMachine = m_MachineListCtrl.GetItemData(i);
        nItem = i;

        pPerfMon = m_PerfMonList[nMachine];
        if (pPerfMon)
        {
            CString origStr;

            // The data is marked valid only after all the required
            // statistics are obtained
            if (pPerfMon->m_DataValid == TRUE)
            {
                if (pPerfMon->m_ErrorCode == ERROR_SUCCESS)
                {
                    CString dispStr;

                    dispStr.Format ("%d %%", pPerfMon->processorTime);
                    origStr = m_MachineListCtrl.GetItemText(nItem, 1);
                    if (origStr.Compare(dispStr))
                        m_MachineListCtrl.SetItemText(nItem, 1, dispStr.GetBuffer(1024));

                    dispStr.Format ("%d %%", pPerfMon->userTime);
                    origStr = m_MachineListCtrl.GetItemText(nItem, 2);
                    if (origStr.Compare(dispStr))
                        m_MachineListCtrl.SetItemText(nItem, 2, dispStr.GetBuffer(1024));

                    dispStr.Format ("%d %%", pPerfMon->kernelTime);
                    origStr = m_MachineListCtrl.GetItemText(nItem, 3);
                    if (origStr.Compare(dispStr))
                        m_MachineListCtrl.SetItemText(nItem, 3, dispStr.GetBuffer(1024));

                    dispStr.Format ("%ld K", (long) pPerfMon->availableMemoryCount/1024);
                    origStr = m_MachineListCtrl.GetItemText(nItem, 4);
                    if (origStr.Compare(dispStr))
                        m_MachineListCtrl.SetItemText(nItem, 4, dispStr.GetBuffer(1024));

                    dispStr.Format ("%ld K", (long) pPerfMon->commitedMemoryCount/1024);
                    origStr = m_MachineListCtrl.GetItemText(nItem, 5);
                    if (origStr.Compare(dispStr))
                        m_MachineListCtrl.SetItemText(nItem, 5, dispStr.GetBuffer(1024));

                    dispStr.Format ("%ld K", (long) pPerfMon->maxCommitedMemoryCount/1024);
                    origStr = m_MachineListCtrl.GetItemText(nItem, 6);
                    if (origStr.Compare(dispStr))
                        m_MachineListCtrl.SetItemText(nItem, 6, dispStr.GetBuffer(1024));

                    origStr = m_MachineListCtrl.GetItemText(nItem, 7);
                    if (origStr.Compare(pPerfMon->systemUpTimeStr))
                        m_MachineListCtrl.SetItemText(nItem, 7, pPerfMon->systemUpTimeStr.GetBuffer(1024));

                    origStr = m_MachineListCtrl.GetItemText(nItem, 8);
                    if (origStr.Compare(pPerfMon->diskFreeString))
                        m_MachineListCtrl.SetItemText(nItem, 8, pPerfMon->diskFreeString.GetBuffer(4096));
                }
                else
                {
                    CString dispStr = _T("N/A");

                    for (int j = 1; j < 9; j++)
                    {
                        origStr = m_MachineListCtrl.GetItemText(nItem, j);
                        if (origStr.Compare(dispStr))
                            m_MachineListCtrl.SetItemText(nItem, j, dispStr.GetBuffer(1024));
                    }
                }
            }
        }
    }

    // Auto size the columns for the very first time only
    if (m_AutoSized == FALSE)
    {
        m_MachineListCtrl.AutoSizeColumns( 1 );
        m_MachineListCtrl.AutoSizeColumns( 2 );
        m_MachineListCtrl.AutoSizeColumns( 3 );
        m_MachineListCtrl.AutoSizeColumns( 4 );
        m_MachineListCtrl.AutoSizeColumns( 5 );
        m_MachineListCtrl.AutoSizeColumns( 6 );
        m_MachineListCtrl.AutoSizeColumns( 7 );
        m_MachineListCtrl.AutoSizeColumns( 8 );
        m_AutoSized = TRUE;
    }

    m_MachineListCtrl.SetRedraw (TRUE);
}

BOOL CMachineStatistics::CheckRSHStatus(CString &machine, CString &user)
{
    BOOL rc = FALSE;
    CString programName((LPCTSTR) IDS_PROGRAM_NAME);
    TCHAR cmdLine[1024];
    TCHAR tmpLine[1024];

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

    sprintf (cmdLine, _T("%s %s -l %s dir c:\\"),
        (LPCTSTR) tmpLine,
        (LPCTSTR) machine,
        (LPCTSTR) user);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hReadPipe;
    HANDLE hWritePipe;  
    SECURITY_ATTRIBUTES saPipe;
    BOOL    fSuccess;

    saPipe.nLength = sizeof( SECURITY_ATTRIBUTES );
    saPipe.lpSecurityDescriptor = NULL;
    saPipe.bInheritHandle       = TRUE;

    fSuccess = CreatePipe(
        &hReadPipe,      // read handle
        &hWritePipe,     // write handle
        &saPipe,         // security descriptor
        0 );             // use default pipe buffer size

    if (fSuccess)
    {
        memset (&si, 0, sizeof (STARTUPINFO));
        si.cb = sizeof (STARTUPINFO);
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

        si.wShowWindow = SW_HIDE;
        si.lpTitle = cmdLine;
        si.hStdOutput  = hWritePipe;
        si.hStdError   = hWritePipe;

        memset (&pi, 0, sizeof (PROCESS_INFORMATION));
        if ( CreateProcess (NULL,
                cmdLine,
                NULL,
                NULL,
                TRUE,
                0,
                NULL,
                NULL,
                &si,
                &pi) )
        {
            fSuccess = CloseHandle( hWritePipe );
            // Check for atleast 7 lines of output data
            int lineCnt = 0;
            unsigned long cbReadBuffer;
            char bReadBuffer; 
            
            while( fSuccess = ReadFile(
                hReadPipe,          // read handle
                &bReadBuffer,       // buffer for incoming data
                1,                  // number of bytes to read
                &cbReadBuffer,      // number of bytes actually read
                NULL))
            {
                if (bReadBuffer == '\n')
                {
                    lineCnt++;
                    if (lineCnt == 7)
                    {
                        rc = TRUE;
                        break;
                    }
                }
            }
            fSuccess = CloseHandle( hReadPipe );
			TerminateProcess (pi.hProcess, 0);
            CloseHandle (pi.hThread);
            CloseHandle (pi.hProcess);
        }
        else
        {
            fSuccess = CloseHandle( hWritePipe );
            fSuccess = CloseHandle( hReadPipe );
        }
    }

    return rc;
}
