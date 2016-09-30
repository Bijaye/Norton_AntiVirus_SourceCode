#include "StdAfx.h"
#include "ccEventUtil.h"
#include "HPPLogFactory.h"
#include "HPPEventsInterface.h"

#include <shlobj.h>

using namespace ccEvtMgr;

const LPCSTR PATH_NAME_SYMANTEC = _T("Symantec");

const CHPPLogFactory::EventLogInfo CHPPLogFactory::m_EventTypeInfoArray[] = 
{
    { HPP::Event_ID_LogEntry_HomePageActivity, _T("HPPHomePageActivity"), _T("log"), FALSE, 65536, TRUE },
    { HPP::Event_ID_LogEntry_AppActivity, _T("HPPAppActivity"), _T("log"), FALSE, 65536, TRUE }
};

CHPPLogFactory::CHPPLogFactory(void)
    : CLogFactoryHelper(m_EventTypeInfoArray, CCDIMOF(m_EventTypeInfoArray))

{
}

CHPPLogFactory::~CHPPLogFactory(void)
{
}

BOOL CHPPLogFactory::Register(long nModuleId,
                           CLogManagerExPtr pLogManager,
						   CEventFactoryExPtr pEventFactory)
{
    // Register.

    BOOL bRet;
    CString cszAppDataPath = _T("c:\\");

    bRet = SHGetSpecialFolderPath(GetDesktopWindow(), cszAppDataPath.GetBuffer(MAX_PATH), CSIDL_COMMON_APPDATA, TRUE);
    cszAppDataPath.ReleaseBuffer();
    if(bRet)
    {
        //
        //  Add "SYMANTEC" to the App Data path
        ::PathAppend(cszAppDataPath.GetBuffer(MAX_PATH), CString(PATH_NAME_SYMANTEC));
        cszAppDataPath.ReleaseBuffer();
        if(-1 == ::GetFileAttributes(cszAppDataPath))
        {
            bRet = ::CreateDirectory(cszAppDataPath, NULL);
            if(!bRet)
            {
                DWORD dwErr = GetLastError();
                CCTRACEE(_T("CHPPLogFactory::Register() - CreateDirectory(\"%s\") failed. GetLastError() = 0x%08X"), cszAppDataPath, dwErr);
            }
        }
    }
    else
    {
        DWORD dwErr = GetLastError();
        CCTRACEE(_T("CHPPLogFactory::Register() - SHGetSpecialFolderPath(CSIDL_APPDATA) failed. GetLastError() = 0x%08X"), dwErr);
    }
    
    CCTRACEI(_T("CHPPLogFactory::Register() - Saving log files to \"%s\"."), cszAppDataPath);
	return CLogFactoryHelper::Register(nModuleId,
                                       pLogManager,
									   pEventFactory,
                                       cszAppDataPath);
}
