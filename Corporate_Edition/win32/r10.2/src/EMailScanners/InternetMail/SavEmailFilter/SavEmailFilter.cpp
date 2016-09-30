// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// SavEmailFilter.cpp
//
// Implementation of CSavEmailFilter -- ccEmailProxy filter plug-in for
// SAV Corporate Edition.
//***************************************************************************

#include "Stdafx.h"

#include "vpcommon.h"
#include "clientreg.h"
#include "SavEmailFilter.h"
#include "ccModuleNames.h"
#include "SymSaferRegistry.h"
#include "vpstrutils.h"

//#include "ccSettingsInterface.h"

#include "SavrtModuleInterface.h"


//*************************************************************************
// CSavEmailFilter::CSavEmailFilter()
//
// CSavEmailFilter::CSavEmailFilter()
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

CSavEmailFilter::CSavEmailFilter()
{
    m_iFilterID          = 0;
    m_pEmailProxy        = NULL;
    m_hEmailProxyDll     = NULL;
    m_pEmailProxyOptions = NULL;

    m_bScanOutgoing = false;
    m_bScanIncoming = false;

    m_hOptionsWatchThread    = NULL;
    m_hOptionsWatchStopEvent = NULL;
} // CSavEmailFilter::CSavEmailFilter()


//*************************************************************************
// CSavEmailFilter::~CSavEmailFilter()
//
// This is our only notification that we're unloading.
// (Well... actually we get Release() calls, but going with
//  SymInterface ISymBaseImpl for ref counting, so we don't see them)
//
// Synchronize cleanup and unload here.
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

CSavEmailFilter::~CSavEmailFilter()
{
    // Stop options watch thread

    if ( m_hOptionsWatchStopEvent )
    {
        if ( m_hOptionsWatchThread )
        {
            SetEvent( m_hOptionsWatchStopEvent );

            WaitForSingleObject( m_hOptionsWatchThread, INFINITE );

            CloseHandle( m_hOptionsWatchThread );
            m_hOptionsWatchThread = NULL;
        }

        CloseHandle( m_hOptionsWatchStopEvent );
        m_hOptionsWatchStopEvent = NULL;
    }

    // Release ccEmailProxy options interface

    FreeOptionsInterface();

    // Deinitialize communications with storage extension

    m_commBuffer.DeInit();
} // CSavEmailFilter::~CSavEmailFilter()


//*************************************************************************
// IEmailFilter methods:
//      Initialize()
//      Notify()
//      Filter()
//*************************************************************************

//*************************************************************************
// CSavEmailFilter::Initialize()
//
// bool CSavEmailFilter::Initialize(int iFilterID, IEmailProxy* pEmailProxy)
//
// ccEmailProxy is loading and initializing this filter.
//
// Parameters:
//      iFilterID                   Assigned filter ID, unique for this session.
//      pEmailProxy                 IEmailProxy interface -- used to select
//                                   which protocols we wish to filter.
//
// Return Values:
//      true on success.
//      false on error, or don't wish to be loaded by ccEmailProxy
//
// Remarks:
//      Note that we will always attempt to initialize/load even if 
//      POP3 scanning is disabled -- There's no second chance to load
//      if POP3 scanning is later enabled....
//
//      IEmailProxy is not a COM interface -- just a struct with a function pointer.
//      No reference counting is done. Pointer is guaranteed valid as long
//      as we're loaded.
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

bool CSavEmailFilter::Initialize(int iFilterID, IEmailProxy* pEmailProxy)
{
    auto    unsigned    uThreadId;

    m_iFilterID = iFilterID;
    m_pEmailProxy = pEmailProxy;

    // Initialize storage extension communication mechanism

    if ( ERROR_SUCCESS != m_commBuffer.Init() )
        return ( false );

    // Get ccEmailProxy options interface

    GetOptionsInterface();

    // Start options watch thread

    m_hOptionsWatchStopEvent = CreateEvent( NULL, FALSE, FALSE, NULL );

    if ( NULL == m_hOptionsWatchStopEvent )
        return ( false );

    m_hOptionsWatchThread = (HANDLE)_beginthreadex( NULL, 0, OptionsWatchProc, this, 0, &uThreadId );

    if ( NULL == m_hOptionsWatchThread )
        return ( false );

    // Update our settings.

    UpdateSettings();

    return ( true );
} // CSavEmailFilter::Initialize()


//*************************************************************************
// CSavEmailFilter::Notify()
//
// void CSavEmailFilter::Notify(DWORD dwSessionID, EPXYEVENT Event)
//
// Session notifications -- an email session is beginning or ending, or
// beginning and end of individual message in a session.
//
// Parameters:
//      dwSessionID
//      Event
//
// Return Values:
//
// Remarks:
//      We're handling AP enabling and disabling here, since ccEmlPxy will
//      be handling potentially infected email content for us.
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

void CSavEmailFilter::Notify(DWORD /*dwSessionID*/, EPXYEVENT Event )
{
    switch ( Event )
    {
    case EPXYEVENT_MESSAGE_HANDLE_START:
        // Disable AP around extraction of each message.

        SAVRTUnProtectProcess();
        break;


    case EPXYEVENT_MESSAGE_HANDLE_END:
        // Re-enable AP after each message.

        SAVRTProtectProcess();
        break;

    case EPXYEVENT_SESSION_START:
    case EPXYEVENT_SESSION_FINISH:
    default:
        break;
    }
} // CSavEmailFilter::Notify()


//*************************************************************************
// CSavEmailFilter::Filter()
//
// EPXYRESULT CSavEmailFilter::Filter(const EPXYMESSAGEINFO& MessageInfo)
//
// Parameters:
//      MessageInfo
//
// Return Values:
//      EPXYRESULT_CONTINUE             Allow email to be sent/received.
//                                       Filter may have made changes -- these will
//                                       be passed on.
//      EPXYRESULT_DROP_MESSAGE         Drop this message.
//      EPXYRESULT_DROP_SESSION         Drop entire email session.
//
// Remarks:
//
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

EPXYRESULT CSavEmailFilter::Filter(const EPXYMESSAGEINFO& MessageInfo)
{
    // pass message info up to S.E. for scanning....

    auto    IEM_MAIL_INFO   mailInfo;
    auto    EPXYRESULT  result;

    result = EPXYRESULT_CONTINUE;

    // Translate ccEmailProxy's EPXYMESSAGEINFO to IEM_MAIL_INFO

    memset( &mailInfo, 0, sizeof( mailInfo ) );

    //&? Following two will be filled in by ProcessHookedMessage()
    //&? These two fields are really buffer information, and should be separate from
    //   message information.
    //
    //mailInfo.nState       = IEM_BUFFER_PENDING_SCAN;
    //mailInfo.nBufferIndex = 0;

    _tcsncpy( mailInfo.szFilePath, MessageInfo.szMessageFile, sizeof( mailInfo.szFilePath ) );
    mailInfo.szFilePath[ sizeof( mailInfo.szFilePath ) - 1 ] = _T('\0');

    _tcsncpy( mailInfo.szSubject, MessageInfo.szSubject, sizeof( mailInfo.szSubject ) );
    mailInfo.szSubject[ sizeof( mailInfo.szSubject ) - 1 ] = _T('\0');

    _tcsncpy( mailInfo.szSender, MessageInfo.szSender, sizeof( mailInfo.szSender ) );
    mailInfo.szSender[ sizeof( mailInfo.szSender ) - 1 ] = _T('\0');

    _tcsncpy( mailInfo.szRecipients, MessageInfo.szRecipient, sizeof( mailInfo.szRecipients ) );
    mailInfo.szRecipients[ sizeof( mailInfo.szRecipients ) - 1 ] = _T('\0');

    mailInfo.bOutbound = FALSE;        // Default to false... switch on for known outbound protocols.

    switch ( MessageInfo.Protocol )
    {
    case EPXYPROTOCOL_POP:
        mailInfo.nProtocol = IEM_PROTOCOL_POP3;
        break;

    case EPXYPROTOCOL_SMTP:
        mailInfo.bOutbound = TRUE;
        mailInfo.nProtocol = IEM_PROTOCOL_SMTP;
        break;

    default:
        mailInfo.nProtocol = IEM_PROTOCOL_UNKNOWN;
        break;
    }

    mailInfo.dwClientProcessId = MessageInfo.dwClientPID;

    _tcsncpy( mailInfo.szClientFilePath, MessageInfo.szClientImage, sizeof( mailInfo.szClientFilePath ) );
    mailInfo.szClientFilePath[ sizeof( mailInfo.szClientFilePath ) - 1 ] = _T('\0');

    mailInfo.nResponse = IEM_CONTINUE;

    //
    // Request scan
    //

    if ( ERROR_SUCCESS == m_commBuffer.ProcessHookedMessage( &mailInfo, 120000 ) )       //&? define -- Notes is currently at 30 seconds... too long/short?
    {
        // Check for responses which must be handled by this filter.
        // These are drop a single message, and drop entire email session.
        // As much as possible, functional code is kept in the storage
        // extension proper in order to minimize the size of this filter dll.

        switch ( mailInfo.nResponse )
        {
        case IEM_CONTINUE:
            result = EPXYRESULT_CONTINUE;
            break;

        case IEM_DROP_MESSAGE:
            result = EPXYRESULT_DROP_MESSAGE;
            break;

        case IEM_DROP_SESSION:
            result = EPXYRESULT_DROP_SESSION;
            break;

        default:
            // We should know and handle all valid responses.
            // Why is there a response we don't know?
            SAVASSERT( 0 );    
            break;
        }
    }

    return ( result );
} // CSavEmailFilter::Filter()


//*************************************************************************
// End of IEmailFilter methods.
//*************************************************************************

//*************************************************************************
// CSavEmailFilter::UpdateSettings()
//
// void CSavEmailFilter::UpdateSettings()
//
// Utility function for CSavEmailFilter (not part of IEmailFilter interface).
//
// Call to reget options relevant to the filter and reset protocols
// we're registered to filter.
// If we expose this, also set (global to ccEmlPxy) the ports listened on
// for each protocol...
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.07.09 DALLEE - comments.
//*************************************************************************

void CSavEmailFilter::UpdateSettings()
{
    auto    MAIL_OPTIONS    mailOptions;
    auto    bool            bChanged;

    if ( ERROR_SUCCESS == m_commBuffer.ReadOptions( &mailOptions ) )
    {
        bChanged = ( !mailOptions.bOnOff != !m_bScanIncoming );

        m_bScanIncoming = ( 0 != mailOptions.bOnOff );
        m_bScanOutgoing = ( 0 != mailOptions.bOnOff );

        if ( bChanged )
        {
            m_pEmailProxy->SetFilterState(m_iFilterID, EPXYPROTOCOL_POP, m_bScanIncoming );
            m_pEmailProxy->SetFilterState(m_iFilterID, EPXYPROTOCOL_SMTP, m_bScanOutgoing );
        }

        // Reset ccEmailProxy options, if changed

        if ( NULL != m_pEmailProxyOptions )
        {
            auto    bool    bTrayAnimation;
            auto    bool    bOutgoingProgress;
            auto    bool    bPopTlsDetect;
            auto    bool    bSmtpTlsDetect;
            auto    bool    bOutgoingSslMessage;
            auto    bool    bIncomingSslMessage;

            m_pEmailProxyOptions->GetValue( CCEMLPXY_TRAY_ANIMATION,    bTrayAnimation );
            m_pEmailProxyOptions->GetValue( CCEMLPXY_OUTGOING_PROGRESS, bOutgoingProgress );
            m_pEmailProxyOptions->GetValue( CCEMLPXY_DETECT_POP_SSL_STREAM,  bPopTlsDetect );
            m_pEmailProxyOptions->GetValue( CCEMLPXY_DETECT_SMTP_SSL_STREAM, bSmtpTlsDetect );
            m_pEmailProxyOptions->GetValue( CCEMLPXY_OUTGOING_SSL_MESSAGE,  bOutgoingSslMessage );
            m_pEmailProxyOptions->GetValue( CCEMLPXY_INCOMING_SSL_MESSAGE, bIncomingSslMessage );

            if ( !bTrayAnimation != !mailOptions.bProgressIcon )
            {
                m_pEmailProxyOptions->SetValue( CCEMLPXY_TRAY_ANIMATION, 0 != mailOptions.bProgressIcon );
            }

            if ( !bOutgoingProgress != !mailOptions.bProgressWindow )
            {
                m_pEmailProxyOptions->SetValue( CCEMLPXY_OUTGOING_PROGRESS, 0 != mailOptions.bProgressWindow );
            }

            if ( !bPopTlsDetect != !mailOptions.bPopTlsDetect )
            {
                m_pEmailProxyOptions->SetValue( CCEMLPXY_DETECT_POP_SSL_STREAM, 0 != mailOptions.bPopTlsDetect );
            }

            if ( !bSmtpTlsDetect != !mailOptions.bSmtpTlsDetect )
            {
                m_pEmailProxyOptions->SetValue( CCEMLPXY_DETECT_SMTP_SSL_STREAM, 0 != mailOptions.bSmtpTlsDetect );
            }

            // This is correct. Single notification option for progress windows: bProgressWindow
            if ( !bOutgoingSslMessage != !mailOptions.bProgressWindow )
            {
                m_pEmailProxyOptions->SetValue( CCEMLPXY_OUTGOING_SSL_MESSAGE, 0 != mailOptions.bProgressWindow );
            }

            // This is correct. Single notification option for progress windows: bProgressWindow
            if ( !bIncomingSslMessage != !mailOptions.bProgressWindow )
            {
                m_pEmailProxyOptions->SetValue( CCEMLPXY_INCOMING_SSL_MESSAGE, 0 != mailOptions.bProgressWindow );
            }
        }
    }
} // CSavEmailFilter::UpdateSettings()


//*************************************************************************
// CSavEmailFilter::OptionsWatch()
//
// Monitor options change notifications from the storage extension.
//
// unsigned CSavEmailFilter::OptionsWatch()
//
// Parameters:
//
// Return Values:
//      0 always
//
// Remarks:
//      Notified of options changes by m_commBuffer.m_hOptionsChangedEvent.
//      Thread exits on m_hOptionsWatchStopEvent.
//*************************************************************************
// 2003.08.03 DALLEE - created.
//*************************************************************************

unsigned CSavEmailFilter::OptionsWatch()
{
    auto    HANDLE  ahWaitObjects [ 2 ];
    auto    bool    bExit;

    bExit = false;

    ahWaitObjects[0] = m_commBuffer.m_hOptionsChangedEvent;
    ahWaitObjects[1] = m_hOptionsWatchStopEvent;

    while ( false == bExit )
    {
        switch ( WaitForMultipleObjects( 2, ahWaitObjects, FALSE, INFINITE ) )
        {
        // case WAIT_ABANDONED_0:
        //     Not waiting on any mutexes, but just here as a reminder.
        case WAIT_OBJECT_0:
            UpdateSettings();
            break;

        case WAIT_OBJECT_0 + 1:
        default:
            bExit = true;
            break;
        }
    }

    return ( 0 );
} // CSavEmailFilter::OptionsWatch()


//*************************************************************************
// CSavEmailFilter::OptionsWatchProc()
//
// ThreadProc for options watch. Thin wrapper around CSavEmailFilter::OptionsWatch().
//
// static unsigned __stdcall CSavEmailFilter::OptionsWatchProc( void *pSavEmailFilter )
//
// Parameters:
//      *pSavEmailFilter
//
// Return Values:
//      0 always
//
// Remarks:
//
//*************************************************************************
// 2003.08.03 DALLEE - created.
//*************************************************************************

unsigned __stdcall CSavEmailFilter::OptionsWatchProc( void *pSavEmailFilter )
{
    return ( ((CSavEmailFilter *)pSavEmailFilter)->OptionsWatch() );
} // CSavEmailFilter::OptionsWatchProc()


//*************************************************************************
// CSavEmailFilter::GetOptionsInterface()
//
// Load the IEmailOptions options interface from ccEmlPxy.dll.
//
// void CSavEmailFilter::GetOptionsInterface()
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.11.12 DALLEE - created.
//*************************************************************************

void CSavEmailFilter::GetOptionsInterface()
{
    auto    pfnGETEMAILOPTIONSOBJECTID  pGetEmailOptionsObjectID;
    auto    pfnGETFACTORY               pGetFactory;
    auto    SYMOBJECT_ID                rOptionsID = {0};
    auto    CSymPtr<ISymFactory>        pFactory;

    SAVASSERT( NULL == m_hEmailProxyDll );
    SAVASSERT( NULL == m_pEmailProxyOptions );

    pGetEmailOptionsObjectID = NULL;
    pGetFactory              = NULL;

    // Load ccEmlPxy.dll

	char szSymSharedPath[IMAX_PATH] = "\0";

	// Get the common client directory
	HKEY hkey;
	if (RegOpenKey(HKEY_LOCAL_MACHINE,szReg_Key_Sym_InstalledApps,&hkey) == ERROR_SUCCESS) 
	{
		DWORD size = sizeof(szSymSharedPath);
		SymSaferRegQueryValueEx(hkey,_T("Common Client"),0,NULL,(PBYTE)szSymSharedPath ,&size);
		RegCloseKey(hkey);
	}

	vpstrnappendfile (szSymSharedPath, _T("ccEmlPxy.dll"), sizeof (szSymSharedPath));

	
	m_hEmailProxyDll = LoadLibrary( szSymSharedPath );

    if ( NULL == m_hEmailProxyDll )
        goto BailOut;

    // Get the ccEmailProxy exports we need...

    pGetEmailOptionsObjectID = (pfnGETEMAILOPTIONSOBJECTID) GetProcAddress( m_hEmailProxyDll, "GetEmailOptionsObjectID" );

    if ( NULL == pGetEmailOptionsObjectID )
        goto BailOut;

    pGetFactory = (pfnGETFACTORY) GetProcAddress( m_hEmailProxyDll, "GetFactory" );

    if ( NULL == pGetFactory )
        goto BailOut;

    // Get ID of ccEmailProxy Options object

    if ( FAILED( pGetEmailOptionsObjectID( &rOptionsID ) ) )
        goto BailOut;

    // Get the ccEmailProxy factory

    if ( SYM_FAILED( pGetFactory( rOptionsID, &pFactory ) ) )
        goto BailOut;

    // Get ccEmailProxy Options object

    if ( SYM_FAILED( pFactory->CreateInstance( IID_EmailOptions, (void**) &m_pEmailProxyOptions ) ) )
        goto BailOut;

BailOut:
    ;
} // CSavEmailFilter::GetOptionsInterface()


//*************************************************************************
// CSavEmailFilter::FreeOptionsInterface()
//
// Release the ccEmailProxy options interface
//
// void CSavEmailFilter::FreeOptionsInterface()
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.11.12 DALLEE - created.
//*************************************************************************

void CSavEmailFilter::FreeOptionsInterface()
{
    if ( NULL != m_pEmailProxyOptions )
    {
        m_pEmailProxyOptions->Release();
        m_pEmailProxyOptions = NULL;
    }

    if ( NULL != m_hEmailProxyDll )
    {
        FreeLibrary( m_hEmailProxyDll );
        m_hEmailProxyDll = NULL;
    }
} // CSavEmailFilter::FreeOptionsInterface()
