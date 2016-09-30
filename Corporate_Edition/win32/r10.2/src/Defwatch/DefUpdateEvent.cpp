// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/////////////////////////////////////////////////////////////////////////////
// DefUpdateEvent.cpp : implementation file
//
// This class manages the "HawkingCheckDefs" named event. This event is 
// created  by a listener (in this case, Defwatch) who will create the event
// and use the handle in any normal wait operation. 
//
// The DefUtils library will open and pulse this event as one of the last
// things to be done when creating a new shared def set.
//
// The listener will return from the wait and continue processing where he 
// will presumably use DefUtils functions to check for new defs.
//
// Note this requires DefUtils.lib from 6/25/2000 or later.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "defwatch.h"
#include "DefUpdateEvent.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CDefUpdateEvent::CDefUpdateEvent()
//
// Description:     Constructor for the CDefUpdateEvent class. Initializes
//                  data members.
//
// Return type:     None
//
///////////////////////////////////////////////////////////////////////////////
// 6/26/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CDefUpdateEvent::CDefUpdateEvent()
{
    // Initialize data members 

    m_hDefUpdateEvent = NULL;
    m_lpGroupSID = NULL;
    m_lpACL = NULL;
	m_pDacl = NULL;

    memset( (LPVOID)&m_SecDesc, 0, sizeof(SECURITY_DESCRIPTOR) );
    memset( (LPVOID)&m_SecAttr, 0, sizeof(SECURITY_ATTRIBUTES) );
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CDefUpdateEvent::~CDefUpdateEvent()
//
// Description:     Destructor for the CDefUpdateEvent class. Makes sure
//                  everything is cleaned up correctly.
//
// Return type:     None
//
///////////////////////////////////////////////////////////////////////////////
// 6/26/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
CDefUpdateEvent::~CDefUpdateEvent()
{
    //
    // Make sure we've cleaned up
    //
    if ( m_hDefUpdateEvent )
    {
        // We're calling our own CloseHandle()
        CloseHandle();
    }

    // 
    // Make sure the buffers are cleaned up
    //
    if (m_lpACL )
    {
        free( m_lpACL );
        m_lpACL = NULL;
    }

    if ( m_lpGroupSID )
    {
        free( m_lpGroupSID );
        m_lpGroupSID = NULL;
    }

	if ( m_pDacl )
	{
		m_nullDacl.Destroy();
		m_pDacl= NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CDefUpdateEvent::GetHandle
//
// Description:     Get a handle to the "HawkingCheckDefs" event for new def  
//                  notifications. Return an existing handle if one already 
//                  exists.
//
// Important Note:  Remember that the handle that gets returned is only 
//                  valid while the instance of the class is within scope.
//
//                  The handle will become invalid when either CloseHandle()
//                  or the destructor gets called.     
//
// Return type:     HANDLE  Returns a handle on success, or NULL on failure
//
///////////////////////////////////////////////////////////////////////////////
// 6/26/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
HANDLE CDefUpdateEvent::GetHandle()
{
    // 
    // Do we already have a valid handle?
    //
    if ( !m_hDefUpdateEvent )
    {
        // 
        // No. Firset set up the security stuff
        //

        if ( ERROR_SUCCESS == SetupSecurityDescriptor() )
        {
            m_SecAttr.bInheritHandle = TRUE;
            m_SecAttr.lpSecurityDescriptor = &m_SecDesc;
            m_SecAttr.nLength = sizeof(SECURITY_ATTRIBUTES);

            //
            // Create the event itself
            //

            m_hDefUpdateEvent = CreateEvent( &m_SecAttr, TRUE, FALSE, DEF_UPDATE_EVENT_NAME );
        }
    }

    return m_hDefUpdateEvent;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CDefUpdateEvent::CloseHandle
//
// Description:     Close the to the "HawkingCheckDefs" event and free the 
//                  buffers we created.
//
// Return type:     DWORD   Returns ERROR_SUCCESS if handle is successfully
//                          closed.
//
///////////////////////////////////////////////////////////////////////////////
// 6/26/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CDefUpdateEvent::CloseHandle()
{
    DWORD   dwError = ERROR_SUCCESS;


    if ( m_hDefUpdateEvent )
    {
        // Close it.

        if ( ::CloseHandle(m_hDefUpdateEvent) )
        {
            //
            // Closed ok..
            //
            m_hDefUpdateEvent = NULL;

            // 
            // Free the access control list buffer
            //
            if (m_lpACL )
            {
                free( m_lpACL );
                m_lpACL = NULL;
            }

            //
            // Free the SID buffer
            //
            if ( m_lpGroupSID )
            {
                free( m_lpGroupSID );
                m_lpGroupSID = NULL;
            }
        }
        else
        {
            dwError = GetLastError();
        }
    }

    return dwError;
}


///////////////////////////////////////////////////////////////////////////////
//
// Function name:   CDefUpdateEvent::SetupSecurityDescriptor
//
// Description:     Set up the security descriptor to be used by the event.
//                  The built-in "Everyone" group will have EVENT_ALL_ACCESS
//                  rights to the event.
//
// Return type:     DWORD   Returns ERROR_SUCCESS if a security descriptor
//                          is successfully created.
//
///////////////////////////////////////////////////////////////////////////////
// 6/26/00: TCashin Function created / header added 
///////////////////////////////////////////////////////////////////////////////
DWORD CDefUpdateEvent::SetupSecurityDescriptor()
{
    DWORD               dwError = ERROR_SUCCESS;
    PSID                pEveryoneGrpSID = NULL;
    PACL                pNewACL = NULL;
    DWORD               dwSize = 0;

    //
    // Initialize the absolute security descriptor
    //
    if ( !InitializeSecurityDescriptor(&m_SecDesc, SECURITY_DESCRIPTOR_REVISION) )
    {
        dwError = GetLastError();
        goto All_Done;
    }


	if (m_pDacl==NULL)
	{
		OSVERSIONINFO VersionInfo;
		ZeroMemory(&VersionInfo, sizeof(VersionInfo));
		VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
		GetVersionEx(&VersionInfo);
		if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if (m_nullDacl.Create() == TRUE)
			{
				m_pDacl= m_nullDacl;
			}
			else
			{
				dwError = GetLastError();
				goto All_Done;
			}
		}
	}

	//
	// set the new DACL in the absolute SD
	//
	if ( !SetSecurityDescriptorDacl(&m_SecDesc, TRUE, m_pDacl, FALSE) )
	{
		dwError = GetLastError();
		goto All_Done;
	}

	//
	// Did we build the new security descriptor correctly?
	//
	if ( !IsValidSecurityDescriptor(&m_SecDesc) )
	{
		dwError = GetLastError();
	}

All_Done:

    if( dwError != ERROR_SUCCESS )
    {
        DefWatchDebugOut( "SetupSecurityDescriptor() failed with error: %X.", dwError);
    }

    return dwError;
}
