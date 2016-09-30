#include "stdafx.h"
#include "osinfo.h"
#include "globalevent.h"

///////////////////////////////////////////////////////////////////////////////
// CGlobalEvent::CGlobalEvent()

CGlobalEvent::CGlobalEvent() :
	m_hEvent( NULL )
{
	// Intentionally empty.
}

///////////////////////////////////////////////////////////////////////////////
// CGlobalEvent::~CGlobalEvent()

CGlobalEvent::~CGlobalEvent()
{
	if( m_hEvent )
	{
		CloseHandle( m_hEvent );
		m_hEvent = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// @function CGlobalEvent::Create
//
// Create an event object.  
//
// If the event specified by <i>szEventName</i> already exists in the global
// namespace, a handle to it will be opened.  See the Win32 SDK for details.
//
// @param <i>szEventName</i> Name for event object
// @param <i>bManualReset</i> TRUE for manual reset events.
//
// @return  <table cellspacing=1>	
//			<tr><td>TRUE</td><td>On success.</TD>
//			<tr><td>FALSE</td><td>Otherwise.  Call <b>GetLastError()</b> for error info.</TD>
//			</table>
//

BOOL CGlobalEvent::Create( LPCSTR szEventName, BOOL bManualReset /*= TRUE */ )
{
	COSInfo osInfo;

	// First, try to open an existing event.
	m_hEvent = OpenEventA( EVENT_ALL_ACCESS, FALSE, szEventName );
	if( NULL != m_hEvent )
		return TRUE;
	
	// On WinNT systems, create a NULL DACL for the event
	SECURITY_ATTRIBUTES sa; 
	if( COSInfo::IsWinNT() )
	{
		// Setup the descriptor.
		InitializeSecurityDescriptor(&m_sdEvent, SECURITY_DESCRIPTOR_REVISION);

		// Add a null DACL to the security descriptor. 
		SetSecurityDescriptorDacl(&m_sdEvent, TRUE, (PACL) NULL, FALSE);
		sa.nLength = sizeof(sa); 
		sa.lpSecurityDescriptor = &m_sdEvent;
		sa.bInheritHandle = TRUE;

		// On Terminal services systems, prepend "Global\\" to the event 
		// name.
		char szTemp[ MAX_PATH ];
		if( COSInfo::IsTerminalServicesInstalled() )
			wsprintf( szTemp, "Global\\%s", szEventName );
		else
			strncpy( szTemp, szEventName, MAX_PATH );

		// Create the event
		m_hEvent = CreateEventA( &sa, bManualReset, FALSE, szTemp );
	}
	else
	{
		// Just create the event.
		m_hEvent = CreateEventA( NULL, bManualReset, FALSE, szEventName );
	}

	return m_hEvent != NULL;
}

/////////////////////////////////////////////////////////////////////////////
// @function CGlobalEvent::CreateW
//
// Create an event object.  Unicode version.
//
// If the event specified by <i>szEventName</i> already exists in the global
// namespace, a handle to it will be opened.  See the Win32 SDK for details.
//
// Note: This function returns FALSE on Win9x platforms.
//
// @param <i>szEventName</i> Name for event object
// @param <i>bManualReset</i> TRUE for manual reset events.
//
// @return  <table cellspacing=1>	
//			<tr><td>TRUE</td><td>On success.</TD>
//			<tr><td>FALSE</td><td>Otherwise.  Call <b>GetLastError()</b> for error info.</TD>
//			</table>
//

BOOL CGlobalEvent::CreateW( LPCWSTR szEventName, BOOL bManualReset /*= TRUE */ )
{
	COSInfo osInfo;

	// This only works on WINNT systems.
	if( !COSInfo::IsWinNT() )
		return FALSE;

	// First, try to open an existing event.
	m_hEvent = OpenEventW( EVENT_ALL_ACCESS, FALSE, szEventName );
	if( NULL != m_hEvent )
		return TRUE;
	
	// Setup the descriptor.
	SECURITY_ATTRIBUTES sa; 
	InitializeSecurityDescriptor(&m_sdEvent, SECURITY_DESCRIPTOR_REVISION);

	// Add a null DACL to the security descriptor. 
	SetSecurityDescriptorDacl(&m_sdEvent, TRUE, (PACL) NULL, FALSE);
	sa.nLength = sizeof(sa); 
	sa.lpSecurityDescriptor = &m_sdEvent;
	sa.bInheritHandle = TRUE;

	// On Terminal services systems, prepend "Global\\" to the event 
	// name.
	WCHAR szTemp[ MAX_PATH ];
	if( COSInfo::IsTerminalServicesInstalled() )
		wsprintfW( szTemp, L"Global\\%s", szEventName );
	else
		wcsncpy( szTemp, szEventName, MAX_PATH );

	// Create the event
	m_hEvent = CreateEventW( &sa, bManualReset, FALSE, szTemp );

	return m_hEvent != NULL;
}






