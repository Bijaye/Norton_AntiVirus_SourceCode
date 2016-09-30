// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// MailComm.cpp: implementation of the CMailCommBuffer class.
//
//***************************************************************************

#include "stdafx.h"

#include "vpcommon.h"
#include "clientreg.h"
#include "MailComm.h"
#include "SymSaferStrings.h"
#include "ccEveryoneDacl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


PIEM_MAIL_INFO CMailCommBuffer::GetMailInfoBlockFromIndex( DWORD dwIndex )
{
    auto    PIEM_MAIL_INFO  pMailInfo;

    pMailInfo    = &m_pBuffer->buf[ dwIndex % IEM_MAIL_BUFFER_ENTRIES ];

    // Check the context index to see if it matches 
    // the bufferIndex in the file info and that  
    // the buffer is still owned by the scan context
    // If the buffer is not owned by the scan context, 
    // the mail context may have timed out.
    //
    //&? Bogus bogus.... Notes scheme here is broken, not guaranteed since buffer not locked.

    if( ( dwIndex != pMailInfo->nBufferIndex ) ||
        ( IEM_BUFFER_OWNED_BY_SCAN != pMailInfo->nState ) )
    {
        pMailInfo = NULL;
    }

    return ( pMailInfo );
}

//***************************************************************************
// Local helper functions
//***************************************************************************

// Functions for creating a security descriptor granting access to everyone.

PSECURITY_DESCRIPTOR static AllocAllAccessSecurityDescriptor();
void static FreeAllAccessSecurityDescriptor( PSECURITY_DESCRIPTOR pSD );
static SAV::CEveryoneDACL nullDacl;

//***************************************************************************
// Construction/Destruction
//***************************************************************************

CMailCommBuffer::CMailCommBuffer()
{
    m_hBufferSemaphore      = NULL;
    m_hBufferMutex          = NULL;
    memset( &m_ahBufferEvents, 0, sizeof( m_ahBufferEvents ) );
    m_hBuffer               = NULL;
    m_pBuffer               = NULL;
    m_hOptionsChangedEvent  = NULL;
} // CMailCommBuffer::CMailCommBuffer()

CMailCommBuffer::~CMailCommBuffer()
{
    // Safe even if called a second time... Just ensure all handles, etc., closed.
    DeInit();
} // CMailCommBuffer::~CMailCommBuffer()


//*************************************************************************
// CMailCommBuffer::Init()
//
// DWORD CMailCommBuffer::Init()
//
// Parameters:
//
// Return Values:
//
// Remarks:
//      ERROR_SUCCESS   on success
//      non-zero Windows error code (from GetLastError()) on failure.
//*************************************************************************
// 2003.06.30 DALLEE - from Lotus Notes SE, filefinfo.c, InitIPC()
//*************************************************************************

DWORD CMailCommBuffer::Init()
{
    auto    TCHAR   szEvent [80];

    auto    SECURITY_ATTRIBUTES rSA;

    auto    int     i;
    auto    DWORD   dwResult;

    memset( &rSA, 0, sizeof( rSA ) );
    dwResult = (DWORD)-1;

    // Create full-access security descriptor, so Storage Extension (running as a service)
    // can share these objects with the email proxy filter (running as a user)

    rSA.nLength = sizeof( rSA );
    rSA.bInheritHandle = FALSE;
    rSA.lpSecurityDescriptor = AllocAllAccessSecurityDescriptor();

    if ( NULL == rSA.lpSecurityDescriptor )
    {
        dwResult = GetLastError();
        goto BailOut;
    }

    // Create the Buffer pool mutex

	m_hBufferMutex = CreateMutex( &rSA, FALSE, IEM_MAIL_BUFFER_MUTEX );

    if( NULL == m_hBufferMutex )
    {
		// now that we're using a non-null security descriptor, CreateMutex will fail
		// if the mutex was already created by another process, so we need to try open also
		m_hBufferMutex= OpenMutex( SYNCHRONIZE|MUTEX_MODIFY_STATE, true, IEM_MAIL_BUFFER_MUTEX );

		if( NULL == m_hBufferMutex )
		{
			// DebugOut(SM_ERROR, "ERROR: I-Mail: Attempt to create mail buffer Mutex failed.");
			dwResult = GetLastError();
			goto BailOut;
		}
    }

    // Create the file mapping and view - shared memory buffer

    m_hBuffer = CreateFileMapping( (HANDLE)-1,
                                   &rSA,
                                   PAGE_READWRITE,
                                   0, sizeof( IEM_MAIL_INFO_BUFFER ),
                                   IEM_MAIL_BUFFER_NAME );

    if ( NULL == m_hBuffer )
    {
		// now that we're using a non-null security descriptor, CreateFileMapping will fail
		// if the object was already created by another process, so we need to try open also
		m_hBuffer = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, true, IEM_MAIL_BUFFER_NAME);

		if ( NULL == m_hBuffer )
		{
			// DebugOut(SM_ERROR, "ERROR: I-Mail: Attempt to create memory mapped file failed.");
			dwResult = GetLastError();
			goto BailOut;
		}
    }

    m_pBuffer = (PIEM_MAIL_INFO_BUFFER) MapViewOfFile( m_hBuffer,
                                                       FILE_MAP_WRITE,
                                                       0, 0, 0 );

    if ( NULL == m_pBuffer )
    {
        // DebugOut(SM_ERROR, "ERROR: I-Mail: Attempt to create memory mapped file view failed.");
        dwResult = GetLastError();
        goto BailOut;
    }

    // Create the storage extension notification semaphore.

    m_hBufferSemaphore = CreateSemaphore( &rSA, 0, IEM_MAIL_BUFFER_ENTRIES, IEM_MAIL_BUFFER_SEMAPHORE );

    if ( NULL == m_hBufferSemaphore )
    {
		// now that we're using a non-null security descriptor, CreateSemaphore will fail
		// if the object was already created by another process, so we need to try open also
		m_hBufferSemaphore = OpenSemaphore( SYNCHRONIZE|SEMAPHORE_MODIFY_STATE, true, IEM_MAIL_BUFFER_SEMAPHORE);

		if ( NULL == m_hBufferSemaphore )
		{
			// DebugOut(SM_ERROR, "ERROR: I-Mail: Attempt to create mail buffer Semaphore failed.");
			dwResult = GetLastError();
			goto BailOut;
		}
    }

    // Create the scanned file notification events
    for ( i = 0; i < IEM_MAIL_BUFFER_ENTRIES; i++ )
    {
        sssnprintf( szEvent, sizeof(szEvent), IEM_MAIL_BUFFER_EVENT, i );

        m_ahBufferEvents[ i ] = CreateEvent( &rSA, FALSE, FALSE, szEvent );

        if ( NULL == m_ahBufferEvents[ i ] )
        {
			// now that we're using a non-null security descriptor, CreateEvent will fail
			// if the object was already created by another process, so we need to try open also
			m_ahBufferEvents[ i ] = OpenEvent( EVENT_MODIFY_STATE | SYNCHRONIZE, true, szEvent);

			if ( NULL == m_ahBufferEvents[ i ] )
			{
				// DebugOut(SM_ERROR, "ERROR: I-Mail: Attempt to create scanned file event failed.");
				dwResult = GetLastError();
				goto BailOut;
			}
		}
    }

    // Create the options change notification event
    // Manual reset event which will be pulsed to wake up all ccEmlPxy Filter instances.

    m_hOptionsChangedEvent = CreateEvent( &rSA, TRUE, FALSE, IEM_MAIL_OPTIONS_EVENT );

    if ( NULL == m_hOptionsChangedEvent )
    {
		m_hOptionsChangedEvent = OpenEvent( EVENT_MODIFY_STATE | SYNCHRONIZE, true, IEM_MAIL_OPTIONS_EVENT);

		if ( NULL == m_hOptionsChangedEvent )
		{
			// DebugOut(SM_ERROR, "ERROR: I-Mail: Attempt to create options changed event failed.");
			dwResult = GetLastError();
			goto BailOut;
		}
    }

    dwResult = ERROR_SUCCESS;

BailOut:
    if ( ERROR_SUCCESS != dwResult )
        DeInit();

    if ( NULL != rSA.lpSecurityDescriptor )
        FreeAllAccessSecurityDescriptor( rSA.lpSecurityDescriptor );

    return ( dwResult );
} // CMailCommBuffer::Init()


//*************************************************************************
// CMailCommBuffer::DeInit()
//
// DWORD CMailCommBuffer::DeInit()
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.30 DALLEE - created.
//*************************************************************************

DWORD CMailCommBuffer::DeInit()
{
    // Close the options change notification event

    if ( m_hOptionsChangedEvent )
    {
        CloseHandle( m_hOptionsChangedEvent );
        m_hOptionsChangedEvent = NULL;
    }

    // Close the scanned file notification events

    for ( int i = 0; i < IEM_MAIL_BUFFER_ENTRIES; ++i )
    {
        if ( NULL != m_ahBufferEvents[ i ] )
        {
            CloseHandle( m_ahBufferEvents[ i ] );
            m_ahBufferEvents[ i ] = NULL;
        }
    }

    // Close the storage extension notification semaphore

    if ( m_hBufferSemaphore )
    {
        CloseHandle( m_hBufferSemaphore );
        m_hBufferSemaphore = NULL;
    }

    // Unmap view of shared data

    if ( NULL != m_pBuffer )
    {
        UnmapViewOfFile( m_pBuffer );
        m_pBuffer = NULL;
    }

    if ( NULL != m_hBuffer )
    {
        CloseHandle( m_hBuffer );
        m_hBuffer = NULL;
    }

    // Close the buffer pool mutex

    if ( NULL != m_hBufferMutex )
    {
        CloseHandle( m_hBufferMutex );
        m_hBufferMutex = NULL;
    }

    return ( ERROR_SUCCESS );
} // CMailCommBuffer::DeInit()


//*************************************************************************
// CMailCommBuffer::ProcessHookedMessage()
//
// DWORD CMailCommBuffer::ProcessHookedMessage( PIEM_MAIL_INFO pMail, DWORD dwTimeout )
//
// Scan the email message described in pMail. Request is sent to the
// internet email storage extension.
//
// Parameters:
//      pMail
//      dwTimeout
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.30 DALLEE - loosely from Notes ext ProcessHookedNote().
//*************************************************************************

DWORD CMailCommBuffer::ProcessHookedMessage( PIEM_MAIL_INFO pMail, DWORD dwTimeout )
{
    auto    PIEM_MAIL_INFO  pMailInfo;
    auto    DWORD   dwWaitResult;
    auto    DWORD   dwResult;

    pMailInfo = NULL;
    dwResult = (DWORD)-1; //&?

    //&? Check on whether S.E. actually present.
    //   Verify against real-time scanning threads reported in shared buffer, once implemented.

    //&? Old-style... GetAvailableFileInfoBlock() ->
    // - Locks buffer mutex
    // - Finds unused block
    // - marks block to single state of in-use-by-hook/read-to-scan
    // - increments producer index, save in block
    // - release mutex

    pMailInfo = GetAvailableMailInfoBlock();

    if ( NULL != pMailInfo )
    {
        //&? Separate buffer and info structs...
        // copy info into buffer structs....

        pMail->nState = IEM_BUFFER_PENDING_SCAN;
        pMail->nBufferIndex = pMailInfo->nBufferIndex;

        memcpy( pMailInfo, pMail, sizeof( *pMailInfo ) );

        // We manually reset the event in case a previous
        // scan context timed out...

        ResetEvent( m_ahBufferEvents[ pMailInfo->nBufferIndex % IEM_MAIL_BUFFER_ENTRIES ] );

        // signal the scan context that an item is ready.

        if ( !ReleaseSemaphore( m_hBufferSemaphore, 1, NULL ) )
        {
            // we don't really need to set the nBufferIndex to 0,
            // but just in case...

            pMailInfo->nBufferIndex = 0;
            // DebugOut(SM_WARNING, "NLNVP: unable to signal RTVSCAN, skipping attachment \"%s\"", pMailInfo->szAnsiFileName);
            // dwResult = ?? error signaling storage extension ??;
        }
        else
        {

            // DebugOut(SM_GENERAL, "NLNVP: scanning attachment \"%s\" as \"%s\"", pMailInfo->szAnsiFileName, pMailInfo->szFilePath);

            //  Wait for the scanned file event to be signaled.
            //  If X seconds passes, allow notes to continue

            dwWaitResult = WaitForSingleObject( m_ahBufferEvents[ pMailInfo->nBufferIndex % IEM_MAIL_BUFFER_ENTRIES ],
                                                dwTimeout );

            switch ( dwWaitResult )
            {
            case WAIT_OBJECT_0:
                // Work done... back to caller with success...
                //&? Separate buffer and info structs...
                // copy info into buffer structs....

                memcpy( pMail, pMailInfo, sizeof( *pMail ) );

                dwResult = ERROR_SUCCESS;
                break;

            default:
                // Timeout on scan completed event.... attempt best-case cleanup.
                //&? Man this is ugly....

                // we wait for the mutex to avoid stealing the fileinfo
                // buffer from the scanning process...

                dwWaitResult = WaitForSingleObject( m_hBufferMutex, IEM_MAIL_BUFFER_TIMEOUT );

                switch ( dwWaitResult )
                {
                case WAIT_OBJECT_0:
                case WAIT_ABANDONED_0:
                    // check to see if the state changed during
                    // the time we waited for the mutex

                    if ( IEM_BUFFER_SCANNED == pMailInfo->nState )
                    {
                        // Work done... back to caller with success...
                        //&? Separate buffer and info structs...
                        // copy info into buffer structs....

                        memcpy( pMail, pMailInfo, sizeof( *pMail ) );

                        dwResult = ERROR_SUCCESS;
                    }
                    else
                    {
                        // Still not scanned...

                        pMailInfo->nBufferIndex = 0;
                        //&? dwResult = ?? not scanned ??;
                    }

                    ReleaseMutex( m_hBufferMutex );

                    break;

                default:
                    // If we didn't get the mutex, we will still
                    // check to see if the scan got completed

                    if ( IEM_BUFFER_SCANNED == pMailInfo->nState )
                    {
                        // Work done... back to caller with success...
                        //&? Separate buffer and info structs...
                        // copy info into buffer structs....

                        memcpy( pMail, pMailInfo, sizeof( *pMail ) );

                        dwResult = ERROR_SUCCESS;
                    }
                    else
                    {
                        // Still not scanned...

                        pMailInfo->nBufferIndex = 0;
                        //&? dwResult = ?? not scanned ??;
                    }

                    break;
                } // switch ( wait on buffer mutex )

                break;
            } // switch ( wait on scan complete event )
        }

        ReleaseMailInfoBlock( pMailInfo );
    }
    else
    {
        // DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: unable to get info block");
    }

    return ( dwResult );
} // CMailCommBuffer::ProcessHookedMessage()


//*************************************************************************
// CMailCommBuffer::GetAvailableMailInfoBlock()
//
// PIEM_MAIL_INFO CMailCommBuffer::GetAvailableMailInfoBlock()
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.01 DALLEE - created.
//*************************************************************************

PIEM_MAIL_INFO CMailCommBuffer::GetAvailableMailInfoBlock()
{
    auto    PIEM_MAIL_INFO  pMailInfo;
    auto    DWORD           dwNewIndex;
    auto    int             i;

    pMailInfo = NULL;

    // General scheme:
    // - Lock access to shared ring buffer
    // - Search from last used producer index for free block
    // - Mark block in-use and update producer index
    // - Unlock buffer
    // - return pointer to block

    switch ( WaitForSingleObject( m_hBufferMutex, IEM_MAIL_BUFFER_TIMEOUT ) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED_0:
        // Find first free buffer

        for ( i = 0, dwNewIndex = m_pBuffer->producerIndex;
              ( NULL == pMailInfo ) && ( i < IEM_MAIL_BUFFER_ENTRIES );
              ++i, ++dwNewIndex )
        {
            // Don't allow producer index we're going to write in a block to equal 0,
            // this is used to indicate a timeout. Using IEM_MAIL_BUFFER_ENTRIES here,
            // so same entry in the ring buffer will be checked/used.

            if ( 0 == dwNewIndex )
                dwNewIndex = IEM_MAIL_BUFFER_ENTRIES;

            if ( IEM_BUFFER_FREE == m_pBuffer->buf[ dwNewIndex % IEM_MAIL_BUFFER_ENTRIES ].nState )
            {
                // Mark found buffer in-use, update producer index, and done...

                pMailInfo = &m_pBuffer->buf[ dwNewIndex % IEM_MAIL_BUFFER_ENTRIES ];

                memset( pMailInfo, 0, sizeof( *pMailInfo ) );
                pMailInfo->nState = IEM_BUFFER_IN_USE;
                pMailInfo->nBufferIndex = dwNewIndex;

                m_pBuffer->producerIndex = dwNewIndex + 1;
            }
        }

        ReleaseMutex( m_hBufferMutex );

        break;

    default:
        // Couldn't lock access to mail info buffer.
        break;
    }

    return ( pMailInfo );
} // CMailCommBuffer::GetAvailableMailInfoBlock()


//*************************************************************************
// CMailCommBuffer::ReleaseMailInfoBlock()
//
// void CMailCommBuffer::ReleaseMailInfoBlock( PIEM_MAIL_INFO pMailInfo )
//
// Parameters:
//      pMailInfo
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.01 DALLEE - created.
//*************************************************************************

void CMailCommBuffer::ReleaseMailInfoBlock( PIEM_MAIL_INFO pMailInfo )
{
    switch ( WaitForSingleObject( m_hBufferMutex, IEM_MAIL_BUFFER_TIMEOUT ) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED_0:
        pMailInfo->nState = IEM_BUFFER_FREE;

        ReleaseMutex( m_hBufferMutex );

        break;

    default:
        // Couldn't lock access to mail info buffer.
        // Per Notes implementation, simply marking it free anyway.

        pMailInfo->nState = IEM_BUFFER_FREE;

        break;
    }
} // CMailCommBuffer::ReleaseMailInfoBlock()


//*************************************************************************
// CMailCommBuffer::GetWaitingMailInfoBlock()
//
// PIEM_MAIL_INFO CMailCommBuffer::GetWaitingMailInfoBlock ( )
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.08 DALLEE - created.
//*************************************************************************

PIEM_MAIL_INFO CMailCommBuffer::GetWaitingMailInfoBlock ( )
{
    auto    PIEM_MAIL_INFO  pMailInfo;
    auto    DWORD           dwNewIndex;
    auto    int             i;

    pMailInfo = NULL;

    // General scheme:
    // - Lock access to shared ring buffer
    // - Search from last used consumer index for block awaiting scan
    // - Mark block in-use and update consumer index
    // - Unlock buffer
    // - return pointer to block

    switch ( WaitForSingleObject( m_hBufferMutex, IEM_MAIL_BUFFER_TIMEOUT ) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED_0:
        // Find first free buffer

        for ( i = 0, dwNewIndex = m_pBuffer->consumerIndex;
              ( NULL == pMailInfo ) && ( i < IEM_MAIL_BUFFER_ENTRIES );
              ++i, ++dwNewIndex )
        {
            if ( IEM_BUFFER_PENDING_SCAN == m_pBuffer->buf[ dwNewIndex % IEM_MAIL_BUFFER_ENTRIES ].nState )
            {
                // Mark found buffer in-use, update consumer index, and done...

                pMailInfo = &m_pBuffer->buf[ dwNewIndex % IEM_MAIL_BUFFER_ENTRIES ];

                pMailInfo->nState = IEM_BUFFER_OWNED_BY_SCAN;

                m_pBuffer->consumerIndex = dwNewIndex + 1;
            }
        }

        ReleaseMutex( m_hBufferMutex );

        break;

    default:
        // Couldn't lock access to mail info buffer.
        break;
    }

    return ( pMailInfo );
} // CMailCommBuffer::GetWaitingMailInfoBlock()


//*************************************************************************
// CMailCommBuffer::ReadOptions()
//
// Read a copy of the current options from shared memory.
// Options locked around read.
//
// DWORD CMailCommBuffer::ReadOptions( PMAIL_OPTIONS pOptions )
//
// Parameters:
//      pOptions            [out] receives copy of current options on success.
//
// Return Values:
//      ERROR_SUCCESS on success, otherwise non-zero
//
// Remarks:
//
//*************************************************************************
// 2003.08.03 DALLEE - created.
//*************************************************************************

DWORD CMailCommBuffer::ReadOptions( PMAIL_OPTIONS pOptions )
{
    auto    DWORD   dwResult;

    switch ( WaitForSingleObject( m_hBufferMutex, IEM_MAIL_BUFFER_TIMEOUT ) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED_0:
        memcpy( pOptions, &m_pBuffer->options, sizeof( *pOptions ) );
        ReleaseMutex( m_hBufferMutex );

        dwResult = ERROR_SUCCESS;
        break;

    default:
        // Couldn't lock access to mail info buffer.

        dwResult = ERROR_COMM_TIMEOUT;
        break;
    }

    return ( dwResult );
} // CMailCommBuffer::ReadOptions()


//*************************************************************************
// CMailCommBuffer::WriteOptions()
//
// Update options in shared memory. Options locked around write.
//
// DWORD CMailCommBuffer::WriteOptions( const PMAIL_OPTIONS pOptions )
//
// Parameters:
//      pOptions
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.03 DALLEE - created.
//*************************************************************************

DWORD CMailCommBuffer::WriteOptions( const PMAIL_OPTIONS pOptions )
{
    auto    DWORD   dwResult;

    switch ( WaitForSingleObject( m_hBufferMutex, IEM_MAIL_BUFFER_TIMEOUT ) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED_0:
        memcpy( &m_pBuffer->options, pOptions, sizeof( m_pBuffer->options ) );
        ReleaseMutex( m_hBufferMutex );

        PulseEvent( m_hOptionsChangedEvent );

        dwResult = ERROR_SUCCESS;
        break;

    default:
        // Couldn't lock access to mail info buffer.

        dwResult = ERROR_COMM_TIMEOUT;
        break;
    }

    return ( dwResult );
} // CMailCommBuffer::WriteOptions()

//***************************************************************************
// Local helper functions
//***************************************************************************

//************************************************************************
// AllocAllAccessSecurityDescriptor()
//
// This function allocates a SECURITY_DESCRIPTOR granting all access
// to everyone. It will have no owner, and no access control lists.
// The SECURITY_DESCRIPTOR can be used when creating kernel objects which
// need to be shared system-wide.
// Call FreeAllAccessSecurityDescriptor() to release the descriptor when done.
//
// Parameters:
//      None
//
// Returns:
//      On success, a pointer to a full-access SECURITY_DESCRIPTOR.
//      On error, NULL.
//************************************************************************
// 12/06/95 DALLEE, created.
// 2003.03.28 DALLEE, pulled from SYMSTAT with a couple changes
//************************************************************************

PSECURITY_DESCRIPTOR static AllocAllAccessSecurityDescriptor ( )
{
    auto    PSECURITY_DESCRIPTOR    pSD;
	PACL pDacl= NULL;

    // Allocate minimum size absolute security descriptor.

    pSD = (PSECURITY_DESCRIPTOR)malloc( sizeof(SECURITY_DESCRIPTOR) );

    if (NULL != pSD)
    {
	    OSVERSIONINFO VersionInfo;
		ZeroMemory(&VersionInfo, sizeof(VersionInfo));
		VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
		GetVersionEx(&VersionInfo);
		if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if (nullDacl.Create() == TRUE)
				pDacl= nullDacl;
		}
	
        if ( !InitializeSecurityDescriptor( pSD, SECURITY_DESCRIPTOR_REVISION ) ||
             !SetSecurityDescriptorDacl( pSD, TRUE, pDacl, FALSE ) )
        {
            // On error, just go ahead w/ NULL for default security descriptor.
            free( pSD );
            pSD = NULL;
        }
    }
    else
    {
        SetLastError( ERROR_NOT_ENOUGH_MEMORY );
    }

    return (pSD);
} // AllocAllAccessSecurityDescriptor()


//*************************************************************************
// FreeAllAccessSecurityDescriptor()
//
// Free the security descriptor allocated by a call to
// AllocAllAccessSecurityDescriptor().
//
// void static FreeAllAccessSecurityDescriptor ( PSECURITY_DESCRIPTOR pSD )
//
// Parameters:
//      pSD
//
// Return Values:
//*************************************************************************
// 2003.03.28 DALLEE - created.
//*************************************************************************

void static FreeAllAccessSecurityDescriptor ( PSECURITY_DESCRIPTOR pSD )
{
    if ( NULL != pSD )
    {
        free( pSD );
    }

	nullDacl.Destroy();

} // FreeAllAccessSecurityDescriptor()
