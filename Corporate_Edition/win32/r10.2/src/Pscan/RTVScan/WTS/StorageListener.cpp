// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2006 Symantec Corporation, All rights reserved.
//
// CStorageListener implementation.
//
// This class tracks currently logged in and active user.
// Also responsible for notifying storage extensions of changes to current user,
// and updating PONG data.
//
// WTS notifications and their order from Windows
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Listing a few scenarios here, not to provide a complete look at logon/logoff msg handling,
// but to stress a need for research before making changes. Some of the information below
// contradicts the assumptions I had before updating CStorageListener. -DALLEE 2007.07.31.
// (Note that WtsSessionPublisher takes some liberties with following order in UpdateListener() ).
//
// Single local logon/logoff -
//      WTS_CONSOLE_CONNECT         -- this is a logon prompt, no user associated.
//      WTS_SESSION_LOGON           -- user has logged on.
//      WTS_SESSION_LOGOFF          -- user has logged off.
//      WTS_CONSOLE_DISCONNECT      -- Should see this on Vista. On XP and earlier, same session
//                                     may be reused so won't see this or following WTS_CONSOLE_CONNECT
//      WTS_CONSOLE_CONNECT         -- back to logon prompt, no user associated.
//
// Remote logon/logoff behaves the same with WTS_REMOTE_CONNECT/DISCONNECT, if no local user.
//
// Some weird cases:
//
// Local user logged on, remote desktop connects, but doesn't logon -
//      WTS_CONSOLE_CONNECT         -- local logon prompt
//      WTS_SESSION_LOGON           -- local user
//        WTS_REMOTE_CONNECT        -- remote logon prompt
//        WTS_REMOTE_DISCONNECT     -- user close remote desktop, never logged on (may only see DISCONNECT on Vista)
//      ...                         -- local session continues
//
// Windows Vista,
// Remote user A bumped by local user B,
// Local user B logs off
// Remote user A connects on local console
// SESSION  MESSAGE                 NOTES
//  1   WTS_CONSOLE_CONNECT         -- local logon prompt is up
//  2     WTS_REMOTE_CONNECT        -- remote logon prompt
//  2     WTS_SESSION_LOGON         -- user A logs on remote
//      << User B enters logon information locally >>
//  2     WTS_REMOTE_DISCONNECT     -- remote console disconnected
//  1   WTS_SESSION_LOGON           -- user B logged on local
//  1   WTS_SESSION_LOGOFF          -- user B logs off
//  1   WTS_CONSOLE_DISCONNECT      -- local console disconnected
//  3   WTS_CONSOLE_CONNECT         -- new console created for logon prompt
//      << User A enters logon information locally >>
//  3   WTS_CONSOLE_DISCONNECT      -- new session discarded
//  2(!)WTS_CONSOLE_CONNECT         -- session 2 which was remote is now local
//      ...                         -- user A logged in on local console, still on session 2
// Note how user A is associated with session 2. When reconnecting on the console, user A is still
// associated with sessionn 2.
// In the same fashion, session 0 (on XP and earlier) can follow a user from a local console logon
// to a remote session -- it is NOT valid to assume on these OS's that session 0 will always be a local
// session.


#define _WIN32_WINNT 0x0501
#include "storagelistener.h"
#include "pscan.h"
#include "SymSaferStrings.h"
#include "vpexceptionhandling.h"
#include "wtssessionpublisher.h"

CStorageListener CStorageListener::g_objStorageListenerInstance;

CStorageListener& CStorageListener::GetInstance()
{
    CGuard objGuard(g_objStorageListenerInstance.m_objLock);
    if( false == g_objStorageListenerInstance.m_bInitialized )
    {
        CWtsSessionPublisher::Instance()->UpdateListener(g_objStorageListenerInstance);
        g_objStorageListenerInstance.m_bInitialized = true;
    }
    return g_objStorageListenerInstance;
}

CStorageListener::CStorageListener(void): m_dwActiveSessionId(WTS_CURRENT_SESSION), m_bUserConnected(false), m_bInitialized(false)
{
    SetNoActiveUser();
}

CStorageListener::~CStorageListener(void)
{
}

void CStorageListener::SessionChange(const CWtsSessionEvent& objEvent)
{
    switch (objEvent.GetEventType())
	{
		case WTS_SESSION_LOGON:
        	ProcessLogon(objEvent);
			break;

		case WTS_SESSION_LOGOFF:
		    ProcessLogout(objEvent.GetSessionId());
            break;

        case WTS_CONSOLE_DISCONNECT:
            SetSessionInActive(objEvent.GetSessionId());
            break;

        case WTS_CONSOLE_CONNECT:
            SetSessionActive(objEvent.GetSessionId());
            break;

        case WTS_REMOTE_DISCONNECT:
            SetSessionInActive(objEvent.GetSessionId());
            break;

        case WTS_REMOTE_CONNECT:
            SetSessionActive(objEvent.GetSessionId());
            break;

		default:
			// Ignore other types of events that we aren't interested in but log these.
			dprintfTag2(DEBUGSCAN, _T("Storage Listner: Ignoring session change event: %u for session ID: %u\n"), objEvent.GetEventType(), objEvent.GetSessionId());
			break;
	}
}

/** void ProcessLogon(const CWtsSessionEvent& objEvent) **/
void CStorageListener::ProcessLogon(const CWtsSessionEvent& objEvent)
{
    CAccessToken objToken;
    objToken.Attach(objEvent.GetToken(), CAccessToken::TOKEN_WTS);
    CSessionInfo objSessionInfo(objToken,true);

    m_bUserConnected = true;
    m_dwActiveSessionId = objEvent.GetSessionId();

    try
    {
        CGuard objGuard( m_mapLock );

        m_mapSessionInfo[objEvent.GetSessionId()] = objSessionInfo;
        m_strUserName = objSessionInfo.GetUserName();
    }
    VP_CATCH_MEMORYEXCEPTIONS
    (
        SetNoActiveUser();
    )

    ReStartPongEngine(TRUE);
    NotifyStoragesOfNewUser(m_strUserName.c_str());
}

/** void ProcessLogout(const CWtsSessionEvent& objEvent) **/
void CStorageListener::ProcessLogout(const DWORD dwSessionId)
{
    //cleanup the session info
    {
        CGuard objGuard( m_mapLock );

        if( m_mapSessionInfo.find(dwSessionId) != m_mapSessionInfo.end() )
        {
            m_mapSessionInfo.erase(dwSessionId);
        }
    }

    if ( dwSessionId == m_dwActiveSessionId )
    {
        SetNoActiveUser();

        ReStartPongEngine(TRUE);
        NotifyStoragesOfNewUser(m_strUserName.c_str());
    }
}

/** Set no active logged on user **/
void CStorageListener::SetNoActiveUser()
{
    char szUserName[NAME_SIZE];
	DWORD size = sizeof(szUserName);

    if (::GetUserName(szUserName,&size) == FALSE) // BUG BUG, this ALWAYS returns "SYSTEM" . But that ok here   CC:)
    {
		ssStrnCpy(szUserName,LS(IDS_USER_NOT_LOGGED_IN), sizeof(szUserName));
    }

    try
    {
        m_strUserName = szUserName;
    }
    VP_CATCH_MEMORYEXCEPTIONS(;)

    m_dwActiveSessionId = WTS_CURRENT_SESSION;
    m_bUserConnected = false;
}

/** Gets the access token from the class 
 *
 * @returns If the user is logged in the access token
 * @returns NULL if the user is logged out
**/
HANDLE CStorageListener::GetToken()
{
    HANDLE  hToken = NULL;

    if ( m_bUserConnected )
    {
        try
        {
            CGuard objGuard( m_mapLock );

            hToken = m_mapSessionInfo[m_dwActiveSessionId].GetToken();
        }
        VP_CATCH_MEMORYEXCEPTIONS(;)
    }

    return hToken;
}

/** Sets the active session **/
//
// Note: Will only set session as active session if a user is logged in.
//
// It is possible (and in fact standard) to receive WTS_CONSOLE_CONNECT messages
// for the session displaying the logon prompt with no user attached.
void CStorageListener::SetSessionActive(const DWORD dwSessionId)
{
    bool    bUserReconnected = false;

    {
        CGuard objGuard( m_mapLock );

        // Check if this is an existing session which is reconnecting to a console.

        if( m_mapSessionInfo.find(dwSessionId) != m_mapSessionInfo.end() )
        {
            //make sure we set the user connected
            m_bUserConnected = true;
            bUserReconnected = true;

            m_mapSessionInfo[dwSessionId].SetActive();
            m_dwActiveSessionId = dwSessionId;

            try
            {
                m_strUserName = m_mapSessionInfo[dwSessionId].GetUserName();
            }
            VP_CATCH_MEMORYEXCEPTIONS
            (
                SetNoActiveUser();
            )
        }
        else
        {
            // There is no user associated with this session -- most likely a logon prompt.
            // Once a user logs on, we'll receive a WTS_SESSION_LOGON message for this session.
            //
            // Not marking a session as active session unless there's a user logged on.
        }
    }

    if ( bUserReconnected )
    {
        // We changed state of current user/session, notify storages and parent.
        ReStartPongEngine(TRUE);
        NotifyStoragesOfNewUser(m_strUserName.c_str());
    }
}

void CStorageListener::SetSessionInActive(const DWORD dwSessionId)
{
    if( WTS_CURRENT_SESSION == m_dwActiveSessionId )
        return;

    // Scope for lock on session map.
    {
        CGuard objGuard( m_mapLock );

        // Note: Pre-checking for existence here, since map::operator[]() will create
        // a default entry at the key if one does not exist.
        if( m_mapSessionInfo.find(dwSessionId) != m_mapSessionInfo.end() )
            m_mapSessionInfo[dwSessionId].SetInActive();
    }

    // If this is the current session going inactive, make sure we set set the user is not connected.
    if ( dwSessionId == m_dwActiveSessionId )
    {
        SetNoActiveUser();

        ReStartPongEngine(TRUE);
        NotifyStoragesOfNewUser(m_strUserName.c_str());
    }
}

DWORD CStorageListener::GetLoggedInUserCount()
{
	CGuard objGuard( m_mapLock );

	return m_mapSessionInfo.size();
}

bool CStorageListener::GetIsUserLoggedIn()
{
	return ( GetLoggedInUserCount() > 0 ) ? true : false;
}
