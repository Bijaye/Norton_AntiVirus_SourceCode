////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

/* *******************************************************************************
'	File		: SPTestCase2.cpp
'	Module		: SymProtect
'	Description	: This file has the definitions of the class CSPTestCase2.
				  CSPTestCase2 will do some basic checking of SymProtect functionality.
'	Author		: Ignatius
'	Date		: 22nd April 2006 
'	Modified	: 
********************************************************************************** */
#include "stdafx.h"
#include "SPTestCase2.h"

using namespace ATL;
#include "time.h"

CSPTestCase2::CSPTestCase2(void)
{

}

CSPTestCase2::~CSPTestCase2(void)
{
    m_PolicyEvents.Destroy();
}

bool CSPTestCase2::Initialize(ccUnit::ITestHost *pTestHost) throw()
{

	 if( !m_PolicyEvents.Create(*this) )
    {
        CCTRACEE(_T("CSymProtectControlAppDlg::Connect() - m_PolicyEvents.Create() failed.\n"));
		return false;
    }

	bool bInit = __super::Initialize(pTestHost);	
	return bInit;	
}

bool CSPTestCase2::Execute() throw()
{
	CCUNIT_TEST_CONDITION_RETURN(StartSymProtect());
	CCUNIT_TEST_CONDITION_RETURN(RunProcess());
    return true;
}

void CSPTestCase2::HandleSymProtectEvent(
    const SymProtectEvt::CPolicyEventEx* const pEvent)
{
    SpPolicyEvent(pEvent);
}

bool CSPTestCase2::SpPolicyEvent(const SymProtectEvt::CPolicyEventEx* const pEvent)
{
    CStringW msg;

	SYSTEMTIME sysTime = pEvent->GetTimeStamp();
    // Assemble the message
    DWORD dwStrSize = 50;

	wchar_t* pMsg = msg.GetBuffer( dwStrSize );
    VERIFY( pMsg );


	int iCount = GetDateFormat( LOCALE_USER_DEFAULT, 
                            DATE_SHORTDATE, &sysTime, NULL, pMsg, dwStrSize );

    wcscat( pMsg, L" " );

    // add the time string to the end.
    wchar_t* pEnd = &pMsg[iCount];

    // set size to size of unused data in buffer.
    dwStrSize -= (DWORD)(pEnd - pMsg)/sizeof(pMsg[0]);

    // format the time.
    GetTimeFormat( LOCALE_USER_DEFAULT, 0, &sysTime, NULL, pEnd, dwStrSize );

    // release the buffer
    msg.ReleaseBuffer();

    //
    // blocked or logged?
    //
    bool bBlocked;
    VERIFY(SYM_SUCCEEDED(pEvent->WasActionBlocked(bBlocked)));
    if( bBlocked )
        msg += L": BLOCKED: ";
    else
        msg += L": NOTICE: ";

    // Action type?
    SymProtectEvt::SP_ACTION_TYPE type;
    VERIFY(SYM_SUCCEEDED(pEvent->GetAction(type)));
    switch(type)
    {
    case SymProtectEvt::ACTION_API_ZWOPENPROCESS:
        msg += L"OpenProcess";
        break;

    case SymProtectEvt::ACTION_API_ZWOPENTHREAD:
        msg += L"OpenThread";
        break;

    case SymProtectEvt::ACTION_API_ZWDUPLICATEOBJECT:
        msg += L"DuplicateObject";
        break;

    case SymProtectEvt::ACTION_FS_CREATE:
        msg += L"CreateFile";
        break;

    case SymProtectEvt::ACTION_FS_DELETE:
        msg += L"DeleteFile";
        break;

    case SymProtectEvt::ACTION_FS_OPEN:
        msg += L"OpenFile";
        break;

    case SymProtectEvt::ACTION_FS_RENAME:
        msg += L"RenameFile";
        break;

    case SymProtectEvt::ACTION_FS_SETATTRIBUTES:
        msg += L"FileSetAttributes";
        break;

    case SymProtectEvt::ACTION_FS_DIRECTORY_CREATE:
        msg += L"CreateDirectory";
        break;

    case SymProtectEvt::ACTION_FS_DIRECTORY_DELETE:
        msg += L"DeleteDirectory";
        break;

    case SymProtectEvt::ACTION_FS_DIRECTORY_RENAME:
        msg += L"RenameDirectory";
        break;

    case SymProtectEvt::ACTION_FS_DIRECTORY_SETATTRIBUTES:
        msg += L"DirectorySetAttributes";
        break;

    case SymProtectEvt::ACTION_REG_OPEN_KEY:
        msg += L"OpenRegKey";
        break;

    case SymProtectEvt::ACTION_REG_CREATE_KEY:
        msg += L"CreateRegKey";
        break;

    case SymProtectEvt::ACTION_REG_DELETE_KEY:
        msg += L"DeleteRegKey";
        break;

    case SymProtectEvt::ACTION_REG_DELETE_VALUE:
        msg += L"DeleteRegValue";
        break;

    case SymProtectEvt::ACTION_REG_SET_VALUE:
        msg += L"SetRegValue";
        break;

    case SymProtectEvt::ACTION_API_TERMINATE_PROCESS:
        msg += L"TerminateProcess";
        break;

    case SymProtectEvt::ACTION_API_SET_INFO_PROCESS:
        msg += L"SetInfoProcess";
        break;

    case SymProtectEvt::ACTION_API_IMPERSONATE_ANON_TOKEN:
        msg += L"ImpersonateAnonymousToken";
        break;

    case SymProtectEvt::ACTION_API_OPEN_PROCESS_TOKEN:
        msg += L"OpenProcessToken";
        break;

    case SymProtectEvt::ACTION_API_OPEN_THREAD_TOKEN:
        msg += L"OpenThreadToken";
        break;

    case SymProtectEvt::ACTION_API_ALLOC_VIRTUAL_MEM:
        msg += L"AllocVirtualMemory";
        break;

    case SymProtectEvt::ACTION_API_WRITE_VIRTUAL_MEM:
        msg += L"WriteVirtualMemory";
        break;

    case SymProtectEvt::ACTION_API_FREE_VIRTUAL_MEM:
        msg += L"FreeVirtualMemory";
        break;

    case SymProtectEvt::ACTION_API_MAP_VIEW_OF_SECTION:
        msg += L"MapViewOfSection";
        break;

    case SymProtectEvt::ACTION_API_UNMAP_VIEW_OF_SECTION:
        msg += L"UnmapViewOfSection";
        break;

    case SymProtectEvt::ACTION_API_CREATE_THREAD:
        msg += L"CreateThread";
        break;

    case SymProtectEvt::ACTION_API_SET_CONTEXT_THREAD:
        msg += L"SetContextThread";
        break;

    case SymProtectEvt::ACTION_API_SET_INFO_THREAD:
        msg += L"SetInfoThread";
        break;

    case SymProtectEvt::ACTION_API_TERMINATE_THREAD:
        msg += L"TerminateThread";
        break;

    case SymProtectEvt::ACTION_API_SUSPEND_THREAD:
        msg += L"SuspendThread";
        break;

    case SymProtectEvt::ACTION_API_RESUME_THREAD:
        msg += L"ResumeThread";
        break;

    case SymProtectEvt::ACTION_API_ALERT_THREAD:
        msg += L"AlertThread";
        break;

    case SymProtectEvt::ACTION_API_ALERT_RESUME_THREAD:
        msg += L"AlertResumeThread";
        break;

    case SymProtectEvt::ACTION_API_IMPERSONATE_THREAD:
        msg += L"ImpersonateThread";
        break;

    case SymProtectEvt::ACTION_API_CREATE_MUTEX:
        msg += L"CreateMutex";
        break;

    case SymProtectEvt::ACTION_API_OPEN_MUTEX:
        msg += L"OpenMutex";
        break;

    case SymProtectEvt::ACTION_API_CREATE_EVENT:
        msg += L"CreateEvent";
        break;

    case SymProtectEvt::ACTION_API_OPEN_EVENT:
        msg += L"OpenEvent";
        break;

    case SymProtectEvt::ACTION_API_ZWSUSPENDPROCESS:
        msg += L"SuspendProcess";
        break;

    case SymProtectEvt::ACTION_SR_SUCCESS:
        msg += L"System Restore succeeded";
        break;

    case SymProtectEvt::ACTION_SR_FAILURE:
        msg += L"System Restore failed";
        break;

    default:
        msg += L"Unknown Action";
        break;
    }

    msg += L" from ";

    // Actor
    DWORD dwSize = MAX_PATH;
    DWORD dwPID = 0;
    WCHAR szActor[MAX_PATH];
    WCHAR szPID[15];
    VERIFY(SYM_SUCCEEDED(pEvent->GetActor(dwPID, szActor, dwSize)));
    msg += szActor;

    _itow(dwPID, szPID, 10);
    msg += L"(" + CStringW(szPID) + L"), protected resource: ";

    // Target
    dwSize = MAX_PATH;
    WCHAR szTarget[MAX_PATH];
    VERIFY(SYM_SUCCEEDED(pEvent->GetTarget(dwPID, szTarget, dwSize)));
    msg += szTarget;

    _itow(dwPID, szPID, 10);
    msg += L"(" + CStringW(szPID) + L")";

    CCTRACEW(L"%s\n", msg.GetString());

    return 0;
}

bool CSPTestCase2::StartSymProtect()
{
	if (!m_EventTest.SendStartSymProtect())
	{
		CCTRACEW(_T("%s Failed \n"), _T("SendStartSymProtect"));
		return false;
	}

	return true;
}

bool CSPTestCase2::RunProcess()
{
    PROCESS_INFORMATION pi;

	ZeroMemory( &pi, sizeof(pi) );

    if( !CreateProcess( L"AttackAll", NULL, NULL,            
        NULL, FALSE, 0, NULL, NULL, NULL, &pi )) 
    {
		CCTRACEW(L"%s Failed \n", L"CreateProcess failed");
        return false;
    }

    // Wait until child process exits.
    WaitForSingleObject( pi.hProcess, INFINITE );

    // Close process and thread handles. 
    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );

	return true;
}