// NAVEventLogFactory.cpp: implementation of the CNAVEventLogFactory class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "NAVEventLogFactory.h"
#include "AllNAVEvents.h"

#include "NAVSettings.h"

#include "ccSplitPath.h"

#ifdef _DEBUG
    #define new DEBUG_NEW
#endif

using namespace ccLib;
using namespace ccEvtMgr;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
/*
        long m_nEventType;
        LPCTSTR m_szLogName;
        LPCTSTR m_szLogExtension;
        BOOL m_bEncrypt;
        ULONGLONG m_uDefaultMaxSize;
        BOOL m_bDefaultEnabled;
*/
const CNAVEventLogFactory::EventLogInfo CNAVEventLogFactory::m_EventLogInfoArray[] = 
{
    { AV::Event_ID_Threat, _T("AVVirus"), _T("log"), TRUE, LOG_FILE_MAX_SIZE, TRUE },
    { AV::Event_ID_ScanAction, _T("AVApp"), _T("log"), TRUE, LOG_FILE_MAX_SIZE, TRUE },
    { AV::Event_ID_Error, _T("AVError"), _T("log"), TRUE, LOG_FILE_MAX_SIZE, TRUE },
};

CNAVEventLogFactory::CNAVEventLogFactory() 
    : CLogFactoryHelper(m_EventLogInfoArray,
                        CCDIMOF(m_EventLogInfoArray))
{
    loadOptions ();
}

CNAVEventLogFactory::~CNAVEventLogFactory()
{
}

CError::ErrorType CNAVEventLogFactory::SetMaxLogSize(long nEventType, 
                                ULONGLONG uSize)
{
    CError::ErrorType eReturn = ccEvtMgr::CLogFactoryHelper::SetMaxLogSize ( nEventType, uSize );
    saveOptions ();
    return eReturn;
}

CError::ErrorType CNAVEventLogFactory::GetMaxLogSize(long nEventType, 
                                            ULONGLONG& uSize)
{
    loadOptions ();
    CError::ErrorType eReturn = ccEvtMgr::CLogFactoryHelper::GetMaxLogSize ( nEventType, uSize );
    return eReturn;
}


CError::ErrorType CNAVEventLogFactory::SetLogEnabled(long nEventType, 
                                                     bool bEnabled)
{
    CError::ErrorType eReturn = ccEvtMgr::CLogFactoryHelper::SetLogEnabled ( nEventType, bEnabled );
    saveOptions ();
    return eReturn;
}

CError::ErrorType CNAVEventLogFactory::GetLogEnabled(long nEventType, 
                                                     bool& bEnabled)
{
    // This module stays in memory so let's reload the
    // options every time in case the file changes
    // without us knowing.
    //
    loadOptions ();
    CError::ErrorType eReturn = ccEvtMgr::CLogFactoryHelper::GetLogEnabled ( nEventType, bEnabled );
    return eReturn;
}

void CNAVEventLogFactory::loadOptions()
{
 	TRACEHR (h);
	try
	{
        // Load current settings
        //

		ccSettings::ISettingsPtr pSettings = m_ccSettings.GetSettings(_T("Norton AntiVirus\\NAVOPTS.DAT\\ACTIVITY"));

        // Load the max sizes, default to 512K. (pick a number)
        //
        DWORD dwTempSize = 0;

		// ACTIVITY_LogSizeVirus
		if ( SYM_FAILED( pSettings->GetDword(_T("LogSizeVirus"), dwTempSize)) )
			dwTempSize = 1024*512;
        ccEvtMgr::CLogFactoryHelper::SetMaxLogSize ( AV::Event_ID_Threat, dwTempSize );

		// ACTIVITY_LogSizeApp
		if ( SYM_FAILED( pSettings->GetDword(_T("LogSizeApp"), dwTempSize)) )
			dwTempSize = 1024*512;
        ccEvtMgr::CLogFactoryHelper::SetMaxLogSize ( AV::Event_ID_ScanAction, dwTempSize );

		// ACTIVITY_LogSizeError
		if ( SYM_FAILED( pSettings->GetDword(_T("LogSizeError"), dwTempSize)) )
			dwTempSize = 1024*512;
        ccEvtMgr::CLogFactoryHelper::SetMaxLogSize ( AV::Event_ID_Error, dwTempSize );

        // Enabled/disabled
        
		DWORD dwTempEnabled = 0;

		// ACTIVITY_LogEnabledVirus
		if ( SYM_FAILED( pSettings->GetDword(_T("LogEnabledVirus"), dwTempEnabled)) )
			dwTempEnabled = 1;
        ccEvtMgr::CLogFactoryHelper::SetLogEnabled ( AV::Event_ID_Threat, dwTempEnabled );        

		// ACTIVITY_LogEnabledApp
		if ( SYM_FAILED( pSettings->GetDword(_T("LogEnabledApp"), dwTempEnabled)) )
			dwTempEnabled = 1;
        ccEvtMgr::CLogFactoryHelper::SetLogEnabled ( AV::Event_ID_ScanAction, dwTempEnabled );        

		// ACTIVITY_LogEnabledError
		if ( SYM_FAILED( pSettings->GetDword(_T("LogEnabledError"), dwTempEnabled)) )
			dwTempEnabled = 1;
        ccEvtMgr::CLogFactoryHelper::SetLogEnabled ( AV::Event_ID_Error, dwTempEnabled );
	}
    catch(_com_error& err)
    {
		h = err;
    }
}

void CNAVEventLogFactory::saveOptions()
{
	TRACEHR (h);

    try
    {
        // Save all the max sizes
        //

		ccSettings::ISettingsPtr pSettings = m_ccSettings.GetSettings(_T("Norton AntiVirus\\NAVOPTS.DAT\\ACTIVITY"));

        ULONGLONG ullSize = 0;

		// ACTIVITY_LogSizeVirus
        ccEvtMgr::CLogFactoryHelper::GetMaxLogSize ( AV::Event_ID_Threat, ullSize );
		h << pSettings->PutDword(_T("LogSizeVirus"), (DWORD)ullSize);

		// ACTIVITY_LogSizeApp
        ccEvtMgr::CLogFactoryHelper::GetMaxLogSize ( AV::Event_ID_ScanAction, ullSize );
		h << pSettings->PutDword(_T("LogSizeApp"), (DWORD)ullSize);

		// ACTIVITY_LogSizeError
        ccEvtMgr::CLogFactoryHelper::GetMaxLogSize ( AV::Event_ID_Error, ullSize );
		h << pSettings->PutDword(_T("LogSizeError"), (DWORD)ullSize);

        // Enabled/disabled

        bool bEnabled = FALSE;

		// ACTIVITY_LogEnabledVirus
        ccEvtMgr::CLogFactoryHelper::GetLogEnabled ( AV::Event_ID_Threat, bEnabled );
		h << pSettings->PutDword(_T("LogEnabledVirus"), bEnabled? 1 : 0);

		// ACTIVITY_LogEnabledApp
        ccEvtMgr::CLogFactoryHelper::GetLogEnabled ( AV::Event_ID_ScanAction, bEnabled );
		h << pSettings->PutDword(_T("LogEnabledApp"), bEnabled? 1 : 0);
        
		// ACTIVITY_LogEnabledError
        ccEvtMgr::CLogFactoryHelper::GetLogEnabled ( AV::Event_ID_Error, bEnabled );
		h << pSettings->PutDword(_T("LogEnabledError"), bEnabled? 1 : 0);
        
		h << m_ccSettings.PutSettings(pSettings);
    }
    catch(_com_error& err)
    {
		h = err;
    }
}