// AVISModule.cpp: implementation of the CAVISModule class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "avismonitor.h"
#include "AVISModule.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAVISModule::CAVISModule()
{
    m_hProcess = NULL;
    m_ModuleArguments.Empty();
    m_ModuleCheckCount = 0L;
    m_ModuleCheckFileName.Empty();
    m_ModuleEnabled = TRUE;
    m_ModuleErrorCount = 0;
    m_ModuleExternalStarted = FALSE;
    m_ModuleFilePath.Empty();
    m_ModuleLastCheckTime = COleDateTime::GetCurrentTime();
    m_ModuleName.Empty();
    m_ModuleStartTime.SetStatus (COleDateTime::null);
    m_ModuleStatus = MODULE_STOPPED;
    m_ModuleStopTime.SetStatus (COleDateTime::null);
    m_pWinThread = NULL;
    m_Saved = TRUE;
    m_OrigWorkingSetSize = 0;
    m_OrigVMSize = 0;
    m_AutoRestart = FALSE;
    m_AutoStopTime = 0;
    m_RestartTime = 0;
    m_SendMail = TRUE;
    memset (&m_MemCounter, 0, sizeof (m_MemCounter));
}

CAVISModule::~CAVISModule()
{

}
