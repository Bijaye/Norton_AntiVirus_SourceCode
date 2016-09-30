// CookieStatistics.cpp : implementation file
//

#include "stdafx.h"
#include "AVISDFRL.h"
#include "CookieStatistics.h"
#include <afxmt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCookieStatistics dialog

// Dialog to display the Issue Tracking Number based statistics. The information is read from
// the activity log files (backup and the current files). The current file is
// flushed just before the dialog is displayed. However during the time the
// dialog is being displayed, the log files are flushed as defined by the
// log file settings. If you select a Tracking Number which is currently being processed,
// the information displayed may not be current. The backup file is reused
// once the current file grows to the size as specified in the log file settings
// and the information in the previous backup file is lost. The statistics are
// displayed only for the information available in the log files and may be
// incomplete if some of the initial information was present in the file which
// was reused. The status of the job is the completion status of the analysis
// programs. The analysis program may complete successfully and the result of the
// program may be that the job failed. However, the status displayed in the 
// statistics will say success as the program was executed successfully.

CCookieStatistics::CCookieStatistics(CWnd* pParent /*=NULL*/)
	: CDialog(CCookieStatistics::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCookieStatistics)
	m_CookieID = _T("");
	m_CookieStatistics = _T("");
	//}}AFX_DATA_INIT

    m_CookieLogList.RemoveAll();
    m_CookieLogStatList.RemoveAll();
}


CCookieStatistics::~CCookieStatistics()
{
    int logCount;
    int i;

    logCount = m_CookieLogList.GetSize();
    for (i = 0; i < logCount; i++)
    {
	    CCookieLog * pCookieLog = (CCookieLog *) m_CookieLogList[i];
        if (pCookieLog)
            delete pCookieLog;
    }
    m_CookieLogList.RemoveAll();

    logCount = m_CookieLogStatList.GetSize();
    for (i = 0; i < logCount; i++)
    {
	    CCookieLogStat * pCookieLogStat = (CCookieLogStat *) m_CookieLogStatList[i];
        if (pCookieLogStat)
            delete pCookieLogStat;
    }
    m_CookieLogStatList.RemoveAll();
}


void CCookieStatistics::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCookieStatistics)
	DDX_Control(pDX, IDC_EDITCOOKIE_ID, m_CookieIDCtrl);
	DDX_Text(pDX, IDC_EDITCOOKIE_ID, m_CookieID);
	DDX_Text(pDX, IDC_EDIT_COOKIE_STATISTICS, m_CookieStatistics);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCookieStatistics, CDialog)
	//{{AFX_MSG_MAP(CCookieStatistics)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCookieStatistics message handlers

void CCookieStatistics::OnOK() 
{
    UpdateData (TRUE);
    m_CookieStatistics.Empty();

    m_CookieID.TrimLeft();
    m_CookieID.TrimRight();

    if (!m_CookieID.IsEmpty())
    {
        // Make sure that while the log file is being read, the LogCompletionMessage
        // function in DFLauncherView class is not writing any new log information.
        CMutex Mutex(FALSE, "ISDFLauncherLogCompletionMessage");
        CSingleLock sLock (&Mutex, TRUE);

        CString bakFileName = m_LogFileName + ".Bak";

        FILE * logFile;

        // Load all the information of the specified Tracking Number in the collection
        // m_CookieLogList. The information is loaded in the order in which
        // individual jobs completion time.
        logFile = fopen (bakFileName, "r");
        if (logFile != (FILE *) NULL)
        {
            LoadCookieLog (logFile);
            fclose (logFile);
        }

        logFile = fopen (m_LogFileName, "r");
        if (logFile != (FILE *) NULL)
        {
            LoadCookieLog (logFile);
            fclose (logFile);
        }

        m_CookieStatistics.Format("Issue Tracking Number : %s", m_CookieID);
        int logCount;
        int i;

        // Summarize the information in m_CookieLogList into m_CookieLogStatList
        // where a record is created for each individual resource type and 
        // statistics for the resource is collected.
        logCount = m_CookieLogList.GetSize();
        for (i = 0; i < logCount; i++)
        {
	        CCookieLog * pCookieLog = (CCookieLog *) m_CookieLogList[i];
            if (pCookieLog)
            {
                int statCount = m_CookieLogStatList.GetSize();
                int j;
                CCookieLogStat * pCookieLogStat = (CCookieLogStat *) NULL;

                for (j = 0; j < statCount; j++)
                {
	                CCookieLogStat * pTmpCookieLogStat =
                        (CCookieLogStat *) m_CookieLogStatList[j];
                    if (pTmpCookieLogStat)
                    {
                        if (!pCookieLog->m_JobType.CompareNoCase(pTmpCookieLogStat->m_JobType))
                            pCookieLogStat = pTmpCookieLogStat;
                    }
                }

                COleDateTime startTime;
                COleDateTime endTime;

                startTime.ParseDateTime (pCookieLog->m_JobStartTime);
                endTime.ParseDateTime (pCookieLog->m_JobEndTime);

                COleDateTimeSpan runTime = endTime - startTime;

                if (pCookieLogStat)
                {
                    pCookieLogStat->m_JobCount++;
                    pCookieLogStat->m_JobTime += runTime;
                }
                else
                {
                    pCookieLogStat = (CCookieLogStat *) new CCookieLogStat;

                    pCookieLogStat->m_JobType = pCookieLog->m_JobType;
                    pCookieLogStat->m_JobCount++;
                    pCookieLogStat->m_JobTime = runTime;

                    m_CookieLogStatList.Add(pCookieLogStat);
                }
            }
        }

        m_CookieStatistics += "\r\n\r\nStatistics :\r\n====================\r\n";
        m_CookieStatistics += "[Resource : Days-Hours:Minutes:Seconds (Number of iterations)]\r\n\r\n";

        // Display individual resource statistics
        logCount = m_CookieLogStatList.GetSize();
        COleDateTimeSpan totalTime;
        CString tmpStr;

        for (i = 0; i < logCount; i++)
        {
	        CCookieLogStat * pCookieLogStat =
                (CCookieLogStat *) m_CookieLogStatList[i];
            if (pCookieLogStat)
            {

                if (pCookieLogStat->m_JobCount > 1)
                {
                    tmpStr.Format("%s : %d-%02d:%02d:%02d (%d)\r\n",
                        pCookieLogStat->m_JobType,
                        pCookieLogStat->m_JobTime.GetDays(),
                        pCookieLogStat->m_JobTime.GetHours(),
                        pCookieLogStat->m_JobTime.GetMinutes(),
                        pCookieLogStat->m_JobTime.GetSeconds(),
                        pCookieLogStat->m_JobCount);
                }
                else
                {
                    tmpStr.Format("%s : %d-%02d:%02d:%02d\r\n",
                        pCookieLogStat->m_JobType,
                        pCookieLogStat->m_JobTime.GetDays(),
                        pCookieLogStat->m_JobTime.GetHours(),
                        pCookieLogStat->m_JobTime.GetMinutes(),
                        pCookieLogStat->m_JobTime.GetSeconds());
                }
                m_CookieStatistics += tmpStr;
                totalTime += pCookieLogStat->m_JobTime;
                delete pCookieLogStat;
            }
        }
        tmpStr.Format("\r\nTotal Processing Time : %d-%02d:%02d:%02d\r\n",
            totalTime.GetDays(),
            totalTime.GetHours(),
            totalTime.GetMinutes(),
            totalTime.GetSeconds());
        m_CookieStatistics += tmpStr;

        m_CookieStatistics += "\r\nDetails :\r\n====================\r\n";
        m_CookieStatistics += "[\"Resource\" \"Machine\" \"Job Sequence\" \"Start Time\" \"End Time\" \"Status\"]\r\n\r\n";

        // Display details. The jobs are sorted according to the time the job
        // is completed.
        logCount = m_CookieLogList.GetSize();
        for (i = 0; i < logCount; i++)
        {
	        CCookieLog * pCookieLog = (CCookieLog *) m_CookieLogList[i];
            if (pCookieLog)
            {
                tmpStr.Format("%s %s %s \"%s\" \"%s\" %s\r\n",
                    pCookieLog->m_JobType,
                    pCookieLog->m_MachineName,
                    pCookieLog->m_JobSequence,
                    pCookieLog->m_JobStartTime,
                    pCookieLog->m_JobEndTime,
                    pCookieLog->m_JobStatus);

                m_CookieStatistics += tmpStr;
                delete pCookieLog;
            }
        }
        m_CookieLogList.RemoveAll();
        m_CookieLogStatList.RemoveAll();
    }
    UpdateData (FALSE);
    m_CookieIDCtrl.SetFocus();
}

void CCookieStatistics::LoadCookieLog(FILE *logFile)
{
    char logBuf[8*1024];

    // Skip first 3 lines. These are the information lines on the log file
    // which explains the format of the log
    fgets (logBuf, sizeof(logBuf), logFile);
    fgets (logBuf, sizeof(logBuf), logFile);
    fgets (logBuf, sizeof(logBuf), logFile);

    while (fgets (logBuf, sizeof(logBuf), logFile))
    {
        CCookieLog *pCookieLog = (CCookieLog *) new CCookieLog;

        LogParseString (logBuf, pCookieLog);

        if (pCookieLog->m_CookieID.CompareNoCase(m_CookieID))
        {
            delete pCookieLog;
        }
        else
        {
            m_CookieLogList.Add(pCookieLog);
        }
    }
}

void CCookieStatistics::LogParseString(char *logBuf, CCookieLog *pCookieLog)
{
// "Machine Name" "Job Type" "Issue Tracking Number" "Job Sequence" "Job Start Time"
// "Job End Time" "Job Status"

    int strLen = strlen (logBuf);
    int index = 0;
    char fieldVal[4*1024];
    int fieldPos = 0;
    BOOL fieldStarted = FALSE;

    for (int i = 0; i < strLen; i++)
    {
        if (logBuf[i] == '\"')
        {
            if (fieldStarted == FALSE)
            {
                fieldPos = 0;
                fieldStarted = TRUE;
            }
            else
            {
                fieldVal[fieldPos] = '\0';
                fieldStarted = FALSE;
                switch (index)
                {
                case 0:
                    pCookieLog->m_MachineName = fieldVal;
                    break;

                case 1:
                    pCookieLog->m_JobType = fieldVal;
                    break;

                case 2:
                    pCookieLog->m_CookieID = fieldVal;
                    break;

                case 3:
                    pCookieLog->m_JobSequence = fieldVal;
                    break;

                case 4:
                    pCookieLog->m_JobStartTime = fieldVal;
                    break;

                case 5:
                    pCookieLog->m_JobEndTime = fieldVal;
                    break;

                case 6:
                    pCookieLog->m_JobStatus = fieldVal;
                    break;

                default:
                    break;
                }
                index ++;
            }
        }
        else
        {
            if (fieldStarted == TRUE && fieldPos < sizeof(fieldVal))
            {
                fieldVal[fieldPos++] = logBuf[i];
            }
        }
    }
}
