// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// IMailStorage.cpp:
//
// Implementation of the CIMailStorage class.
// This is the Internet Mail (POP3/SMTP) Storage Extension.
//
//*************************************************************************

#include "stdafx.h"
#include <new>
#include <assert.h>
#include <io.h>
#include <fcntl.h>
#include <share.h>

#include "vpcommon.h"
#include "clientreg.h"
#include "veapi.h"

#include "MailComm.h"
#include "IMailStorage.h"
#include <exception>
#include "DAPI.h"
#include "vpstrutils.h"

//for dec2text integration for I-EMAIL 
#include "decinterface.h"

//*************************************************************************
// Static helpers
//*************************************************************************

static size_t GetOriginalAttachmentName( LPTSTR lpszAttachment, PEVENTBLOCK eb, size_t cchAttachment );
static void GetFakeSID( mSID * SID, LPCTSTR lpszUser );

//*************************************************************************
// Static class data
//*************************************************************************
//
// Lack of context information on some callbacks provided to the storage manager
// requires minimally a static pointer to the current CIMailStorage instance.
//
// &? May also want to make unchanging callback and information structures
// static to class...
//*************************************************************************

CIMailStorage *  CIMailStorage::s_pStorage = NULL;


//*************************************************************************
// Class Implementation
//*************************************************************************

//*************************************************************************
// ImStorageInit()
//
// DWORD STORAGEAPI ImStorageInit ( DWORD flags, PSTORAGEINFO * pInfo, HANDLE * phSEInstance, PSSFUNCTIONS pSSFuns )
//
// ImStorageInit is called by the storage manager to initialize the storage extension.
//
// Parameters:
//      flags               [in] Any combination of S_WANT_CONFIG, S_WANT_WALK, S_WANT_RTS
//                               (Must include S_WANT_RTS for our purposes)
//      pInfo               [out] Storage extension information
//      phSEInstance        [out] Module handle (arbitrary value)
//      pSSFuns             [in] Functions provided by the storage manager
//
// Return Values:
//      ERROR_SUCCESS on success.
//      non-zero ERROR_??? on error (see vpcommon.h for return codes)
//
// Remarks:
//      This exported function is the (only) entry point for the storage
//      extension. It must be registered under
//      [<<REGHEADER>>\Storages\InternetMail]
//          ServiceDllEntryPoint="ImStorageInit"
//          ServiceDllName="IMail.dll"
//          ServiceDllPath="<<directory of this dll>>"
//          
//      For Win32, REGHEADER is HKEY_LOCAL_MACHINE\SOFTWARE\INTEL\LANDesk\VirusProtect6\CurrentVersion
//*************************************************************************
// 2003.06.19 DALLEE - created.
//*************************************************************************

__declspec( dllexport ) DWORD STORAGEAPI ImStorageInit ( DWORD flags, PSTORAGEINFO * pInfo, HANDLE * phSEInstance, PSSFUNCTIONS pSSFuns )
{
    auto    CIMailStorage   *pStorage;
    auto    DWORD           dwResult;

    dwResult = ERROR_MEMORY;

    try
    {
        pStorage = new CIMailStorage;

        if ( NULL != pStorage )
        {
            dwResult = pStorage->StorageInit( flags, pInfo, phSEInstance, pSSFuns );

            if ( ERROR_SUCCESS != dwResult )
            {
                delete pStorage;
            }
        }
    }
    catch (std::bad_alloc &) {}
    return ( dwResult );
} // ImStorageInit()


DWORD CIMailStorage::StorageInit ( DWORD flags, PSTORAGEINFO * pInfo, HANDLE * phSEInstance, PSSFUNCTIONS pSSFuns )
{
    auto    DWORD   dwResult = ERROR_SUCCESS;
    auto    HKEY    hKey = NULL;

    // Only support RTS functionality

    if ( 0 == ( flags & S_WANT_RTS ) )
    {
        dwResult = ERROR_FUNCTION_NOT_SUPPORTED;
        goto BailOut;
    }

    //initialize callbacks for decomposer
    //you must initialize file system before IOService
    CDecInterface::NewIOService();
    CDecInterface::NewDecomposer();

    // Initialize communications with email proxy filter

    if ( ERROR_SUCCESS != m_commBuffer.Init() )
    {
        dwResult = ERROR_NO_COMM;
        goto BailOut;
    }

    // Save storage manager provided utility functions.

    m_pfnsStorageManager = pSSFuns;


    // Open RTS configuration key

    m_pfnsStorageManager->Open( reinterpret_cast<HKEY>( HKEY_VP_STORAGE_ROOT ), szReg_Key_Storage_InternetMail, &hKey );

    if ( NULL == hKey )
    {
        dwResult = ERROR_NO_KEY;
        goto BailOut;
    }

    m_pfnsStorageManager->Open( hKey, szReg_Key_Storage_RealTime, &m_storageInfo.hRTSConfigKey );

    if ( NULL == m_storageInfo.hRTSConfigKey )
    {
        dwResult = ERROR_NO_KEY;
        goto BailOut;
    }

    // Manual reset stop event for RTS threads

    m_hStopRtsWatch = CreateEvent( NULL, TRUE, FALSE, NULL );

    if ( NULL == m_hStopRtsWatch )
    {
        if( m_storageInfo.hRTSConfigKey != NULL )
            CloseHandle( m_storageInfo.hRTSConfigKey );

        dwResult = ERROR_MEMORY;
        goto BailOut;
    }

    // Save global pointer for context

    SAVASSERT( NULL == s_pStorage );
    s_pStorage = this;

    // Initialize the registry settings

    sf_ReloadRTSConfig();

    // All good, just returning data from here on out

    *pInfo = &m_storageInfo;
    *phSEInstance = reinterpret_cast<HANDLE>( this );
    
BailOut:
    //&? config key, and event on error....

    if ( NULL != hKey )
        m_pfnsStorageManager->Close( hKey );

    return ( dwResult );
} // CIMailStorage::StorageInit()


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIMailStorage::CIMailStorage()
{
    // SSFUNCTIONS - Utility functions provided by Storage Manager

    m_pfnsStorageManager = NULL;

    m_hStopRtsWatch = NULL;

    //&? static for those of these that make sense?
    // m_fnsStorage?

    // STORAGEINFO - Top-level storage information returned in StorageInit().

    m_storageInfo.Size              = sizeof( STORAGEINFO );
    m_storageInfo.Version           = 0x01;
    m_storageInfo.InstanceCount     = 1;
    m_storageInfo.InstanceBlocks    = &m_instanceDefinition;
    m_storageInfo.Functions         = &m_fnsStorage;
    m_storageInfo.hRTSConfigKey     = NULL;             // Fill in on StorageInit()...
    m_storageInfo.Status            = &m_storageStatus;
    strcpy( m_storageInfo.Name, IMAIL_STORAGE_NAME );   // Not for display. Must match Storages sub-key.
    m_storageInfo.InfFilename[0]    = '\0';
    m_storageInfo.Type              = IT_MAIL | IT_CAN_RTS;
    m_storageInfo.RTSData           = &m_storageData;
    m_storageInfo.MaxThreads        = IMAIL_MAX_REALTIME_THREADS;

    // IDEF - description of each logical volume/instance supported by this storage.
    // Only one instance in order to support RTS.

    m_instanceDefinition.InstanceID     = IMAIL_STORAGE_INSTANCE;
    m_instanceDefinition.Type           = IT_MAIL | IT_CAN_RTS;
    strcpy( m_instanceDefinition.DisplayName, "" );     // Unused for non-walk storages.
    strcpy( m_instanceDefinition.VolumeName, "" );      // Unused for non-walk storages.

    // SFUNCTIONS implementation
    //&? static?

    m_fnsStorage.FindFirstNode      = sf_FindFirstNode;
    m_fnsStorage.FindNextNode       = sf_FindNextNode;
    m_fnsStorage.CreateSNode        = sf_CreateSNode;
    m_fnsStorage.FindClose          = sf_FindClose;
    m_fnsStorage.Deinit             = sf_DeInit;
    m_fnsStorage.BeginRTSWatch      = sf_BeginRTSWatch;
    m_fnsStorage.StopRTSWatches     = sf_StopRTSWatches;
    m_fnsStorage.RefreshInstanceData= sf_RefreshInstanceData;
    m_fnsStorage.ReloadRTSConfig    = sf_ReloadRTSConfig;
    m_fnsStorage.Reinit             = sf_ReInit;
    m_fnsStorage.ProcessPacket      = sf_ProcessPacket;
    m_fnsStorage.FormatMessage      = sf_FormatMessage;
    m_fnsStorage.ChangeUser         = sf_ChangeUser;

    // SFILETABLE implementation -- SNODE IO table
    //&? Same handling as SFUNCTIONS...

    m_fnsNodeIo.open            = io_open;
    m_fnsNodeIo.close           = io_close;
    m_fnsNodeIo.read            = io_read;
    m_fnsNodeIo.write           = io_write;
    m_fnsNodeIo.lseek           = io_lseek;
    m_fnsNodeIo.access          = io_access;
    m_fnsNodeIo.GetState        = io_GetState;
    m_fnsNodeIo.SetState        = io_SetState;
    m_fnsNodeIo.GetExtendedData = io_GetExtendedData;
    m_fnsNodeIo.SetExtendedData = io_SetExtendedData;
    m_fnsNodeIo.GetFullKey      = io_GetFullKey;

    // NODEFUNCTIONS implementation -- Specialized SNODE functions.
    // Most importantly, NodeHasViruses() callback function.

    m_fnsNode.ReleaseSNode   = nf_ReleaseSNode;
    m_fnsNode.NodeHasViruses = nf_NodeHasViruses;
    m_fnsNode.RenameNode     = nf_RenameNode;
    m_fnsNode.RemoveNode     = nf_RemoveNode;

    // STORAGESTATUS - unused.

    m_storageStatus.Flags   = 0xDEADBEEF;   //&? Unused (I believe). Change to 0 after confirming.

    // STORAGEDATA - high-level storage configuration
    
    m_storageData.ZipDepth      = 3;
    m_storageData.FileType      = 0;
    m_storageData.Trap          = 0;
    m_storageData.CheckSum      = 0;
    m_storageData.Types         = 6;
    m_storageData.Exts[0]       = '\0';
    m_storageData.ZipExts[0]    = '\0';
    m_storageData.TrapExts[0]   = '\0';
    m_storageData.ZipFile       = 1;
    m_storageData.Softmice      = 0;

    // MAIL_OPTIONS - real-time options from the registry

    memset( &m_options, 0, sizeof( m_options ) );    // Will be filled in on StorageInit()
    memset( m_szCurrentUser, 0, sizeof( m_szCurrentUser ) );
}

CIMailStorage::~CIMailStorage()
{
    //&? Verify cleanup happened during DeInit()
}


//*****************************************************************************
// Storage Extension Functions (SFUNCTIONS implementations)
//*****************************************************************************

//*************************************************************************
// Find node functions:
//
//  CIMailStorage::sf_FindFirstNode()
//  CIMailStorage::sf_FindNextNode()
//  CIMailStorage::sf_CreateSNode()
//  CIMailStorage::sf_FindClose()
//
// Remarks:
//      Find node functions not supported. These are for Storage Extensions with
//      real traversable storages.
//      Internet mail storage extension only supports real-time scanning.
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_FindFirstNode ( HANDLE /*hParent*/, DWORD /*dwInstance*/, const char * /*pszPath*/, PQNODE /*pQNode*/, PSNODE /*pSNode*/, HANDLE * phFind )
{
    if ( NULL != phFind )
    {
        *phFind = INVALID_HANDLE_VALUE;
    }

    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::sf_FindFirstNode()

DWORD STORAGEAPI CIMailStorage::sf_FindNextNode ( HANDLE /*hFind*/, PQNODE /*pQNode*/, PSNODE /*pSNode*/ )
{
    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::sf_FindNextNode()

DWORD STORAGEAPI CIMailStorage::sf_CreateSNode ( DWORD /*dwInstance*/, const char * /*pszPath*/, PSNODE /*pSNode*/ )
{
    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::sf_CreateSNode()

DWORD STORAGEAPI CIMailStorage::sf_FindClose ( HANDLE /*hFind*/ )
{
    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::sf_FindClose()


//*************************************************************************
// CIMailStorage::sf_DeInit()
//
// DWORD STORAGEAPI CIMailStorage::sf_DeInit ( HANDLE hStorage )
//
// Parameters:
//      hStorage
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_DeInit ( HANDLE hStorage )
{
    auto    CIMailStorage   *pStorage;
    auto    DWORD           dwResult;

    pStorage = reinterpret_cast<CIMailStorage *>( hStorage );

    if ( pStorage == s_pStorage )
    {
        // Change shared options to disabled state + fire options change event to disable filter

        pStorage->m_options.bOnOff = 0;
        pStorage->m_commBuffer.WriteOptions( &pStorage->m_options );

        // Internal storage extension cleanup

        if ( NULL != pStorage->m_storageInfo.hRTSConfigKey )
        {
            pStorage->m_pfnsStorageManager->Close( pStorage->m_storageInfo.hRTSConfigKey );
        }

        //&? object deletion? stop rts event deletion?
        // Already synchronized with stop of RTS watches, or need to handle it myself?
        // **Need to handle it myself** There's a wait on running RTS threads during the
        // StopStorageRTS() call, but one second delay only and not guaranteed.
        // Best to give in and keep one global CIMailStorage object?
        // Or keep usage counts -- 1 for generic use by Storage Manager, 1 for each RTSWatch.

        if ( NULL != pStorage->m_hStopRtsWatch )
        {
            CloseHandle( pStorage->m_hStopRtsWatch );
            pStorage->m_hStopRtsWatch = NULL;
        }

        pStorage->m_commBuffer.DeInit();

        CDecInterface::DeleteIOService();
        CDecInterface::DeleteDecomposer();

        delete pStorage;
        s_pStorage = NULL;

        dwResult = ERROR_SUCCESS;

    }
    else
    {
        SAVASSERT( pStorage == s_pStorage );
        dwResult = ERROR_BAD_PARAM;
    }

    return ( dwResult );
} // CIMailStorage::sf_DeInit()


//*************************************************************************
// CIMailStorage::sf_BeginRTSWatch()
//
// DWORD STORAGEAPI CIMailStorage::sf_BeginRTSWatch ( PROCESSRTSNODE   ProcessRTSNode,
//                                                    PSNODE           pSNode,
//                                                    void *           pStorageManagerContext )
//
// Parameters:
//      ProcessRTSNode
//      pSNode
//      pStorageManagerContext
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_BeginRTSWatch ( PROCESSRTSNODE   ProcessRTSNode,
                                                   PSNODE           pSNode,
                                                   void *           pStorageManagerContext )
{
    //&? Getting ugly in here referencing Storage from global pointer...
    // Transfer to non-static member func, or...?

    auto    IEM_NODE_CONTEXT    nodeContext;
    auto    PIEM_MAIL_INFO      pMailInfo;

    auto    HANDLE  ahWaitObjects [ 2 ];
    auto    bool    bExit;
    MSG     msg;

    bExit = false;

    CoInitialize( NULL );

    ahWaitObjects[0] = s_pStorage->m_commBuffer.m_hBufferSemaphore;
    ahWaitObjects[1] = s_pStorage->m_hStopRtsWatch;

    //&? RTS thread count?  Why is that kept in Lotes?
    // Lotes keeps count in shared memory. Hook verifies there are
    // threads running BeginRTSWatch() before sending messages for scanning.
    // Use thread count to delay CIMailStorage deletion on deinit.

    do
    {
        //&? Wait for item to scan/exit...
        // Notes method mirrored here is no good. Need to hide implementation details
        // of wait for message, return status of scan...

        switch ( MsgWaitForMultipleObjects( 2, ahWaitObjects, FALSE, INFINITE, QS_ALLINPUT) )
        {
        // case WAIT_ABANDONED_0:
        //     Not waiting on any mutexes, but just here as a reminder.

        case WAIT_OBJECT_0:
            pMailInfo = s_pStorage->m_commBuffer.GetWaitingMailInfoBlock();

            if ( NULL != pMailInfo )
            {
                // Message to scan.
                // Fill in SNODE from file info block and call ProcessRTSNode()

                //&? Temp debug message
                {
                    auto    TCHAR   szMessage [ 2048 ];

                    sssnprintf( szMessage,
                               sizeof(szMessage),
                               _T("Protocol: %s\n"
                                  "Client PID: %08X\n"
                                  "Client Image: %s\n"
                                  "Subject: %s\n"
                                  "Sender: %s\n"
                                  "Recipients: %s\n"),
                               pMailInfo->nProtocol == IEM_PROTOCOL_POP3 ? _T("POP3") : _T("SMTP"),
                               pMailInfo->dwClientProcessId,
                               pMailInfo->szClientFilePath,
                               pMailInfo->szSubject,
                               pMailInfo->szSender,
                               pMailInfo->szRecipients );

                    s_pStorage->m_pfnsStorageManager->dPrintf( szMessage );
                }
                //&?

                // Initialize the node structure 

                memset( pSNode, 0, sizeof( *pSNode ) );
                memset( &nodeContext, 0, sizeof( nodeContext ) );

                nodeContext.dwMailInfoBufferIndex = pMailInfo->nBufferIndex;
                pSNode->Context = static_cast<void *>( &nodeContext );

                pSNode->IO = &s_pStorage->m_fnsNodeIo;
                pSNode->Flags = N_RTSNODE | N_MAILNODE;
                pSNode->InternalPath = pMailInfo->szFilePath;
                pSNode->UID = pMailInfo->nBufferIndex;
                pSNode->Functions = &s_pStorage->m_fnsNode;
                pSNode->InstanceID = IMAIL_STORAGE_INSTANCE;
                pSNode->Operations = FA_READ | FA_WRITE | FA_FILE_NEEDS_SCAN | FA_AFTER_OPEN;

                // Request Outbound E-mail Heuristic (OEH) scanning, when the
                // e-mail is outbound and the user wants OEH scanning.
                if ( pMailInfo->bOutbound && s_pStorage && s_pStorage->m_options.bOehOnOff != 0 )
                     pSNode->Operations |= FA_EMAIL_HEURISTIC_SCAN;

                // Fake SID creation, pastes in computer and user name in format RTVScan understands.
                GetFakeSID( &pSNode->Sid, s_pStorage->m_szCurrentUser );

                // Using email subject for description
                _tcsncpy( pSNode->Description, pMailInfo->szSubject, sizeof( pSNode->Description ) / sizeof( *pSNode->Description ) );
                pSNode->Description[ sizeof( pSNode->Description ) / sizeof( *pSNode->Description ) - 1 ] = _T('\0');

                // Don't have real extension info, since this is the body of the email itself.
                // Arbitrarily picking ".eml" that MS uses for saving emails.
                _tcscpy( pSNode->Ext, _T(".eml") );

                // Normally name of attachment with extension...
                _tcsncpy( pSNode->Name, pMailInfo->szSubject, sizeof( pSNode->Name ) / sizeof( *pSNode->Name ) );
                pSNode->Name[ sizeof( pSNode->Name ) / sizeof( *pSNode->Name ) - 1 ] = _T('\0');

                // Setup the Outbound E-mail Heuristics extra info when we are
                // doing OEH.
                if (pSNode->Operations & FA_EMAIL_HEURISTIC_SCAN)
                {
                    pSNode->sz2ndBinPath = pMailInfo->szClientFilePath;
                    pSNode->dwPID        = pMailInfo->dwClientProcessId;
                }

                // Scan this node...
                // Note: Notification comes back in SNode's CIMailStorage::nf_NodeHasViruses()
                // before ProcessRTSNode() returns.

                ProcessRTSNode( pSNode, pStorageManagerContext );

                // Alerts

                if ( NULL != nodeContext.pAttachmentInfo )
                {
                    doNotifications( pSNode );
                }

                // Clean up and attachment infection information

                DeleteAttachmentInfo( &nodeContext );

                // Done scanning, notify the proxy filter.

                switch ( WaitForSingleObject( s_pStorage->m_commBuffer.m_hBufferMutex, IEM_MAIL_BUFFER_TIMEOUT ) )
                {
                case WAIT_ABANDONED_0:
                case WAIT_OBJECT_0:
                    //&?? Checking to be sure we haven't timed out...
                    // Yet another synch bug in Lotes comm mechanism... Fix me, Fix me, Fix me!
                    if( pSNode->UID == pMailInfo->nBufferIndex )
                    {
                        pMailInfo->nState = IEM_BUFFER_SCANNED;
                        SetEvent( s_pStorage->m_commBuffer.m_ahBufferEvents[ pMailInfo->nBufferIndex % IEM_MAIL_BUFFER_ENTRIES ] );
                    }

                    ReleaseMutex( s_pStorage->m_commBuffer.m_hBufferMutex );
                    break;

                default:
                    break;
                }
            }
            break;

        case WAIT_OBJECT_0 + 2:
            // There is a window message available. Dispatch it.
            while(PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            break;

        case WAIT_OBJECT_0 + 1:
        default:
            bExit = true;
            break;
        }
    } while ( false == bExit );

    //&? RTS thread count?  Why is that kept in Lotes?

    CoUninitialize();

    return ( ERROR_SUCCESS );
} // CIMailStorage::sf_BeginRTSWatch()


//*************************************************************************
// CIMailStorage::sf_StopRTSWatches()
//
// DWORD STORAGEAPI CIMailStorage::sf_StopRTSWatches ( )
//
// Parameters:
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_StopRTSWatches ( )
{
    SetEvent( s_pStorage->m_hStopRtsWatch );

    return ( ERROR_SUCCESS );
} // CIMailStorage::sf_StopRTSWatches()


//*************************************************************************
// CIMailStorage::sf_RefreshInstanceData()
//
// DWORD STORAGEAPI CIMailStorage::sf_RefreshInstanceData ( )
//
// Take new snapshot of instances/logical volumes in a storage.
// (e.g. For file system storage this is the list of logical drives )
//
// Dummy implementation for Internet Mail -- we have one and only one instance
// definition for real-time scanning of all POP3/SMTP mail.
//
// Return Values:
//      ERROR_SUCCESS
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_RefreshInstanceData ( )
{
    return ( ERROR_SUCCESS );
} // CIMailStorage::sf_RefreshInstanceData()


//*************************************************************************
// CIMailStorage::sf_ReloadRTSConfig()
//
// Notification function called by Storage Manager (RTVScan) when options
// for this storage extension have changed.
//
// This routine updates shared email options, and signals the change to
// the ccEmlPxy filter plug-in.
//
// DWORD STORAGEAPI CIMailStorage::sf_ReloadRTSConfig ( )
//
// Parameters:
//
// Return Values:
//
// Remarks:
//    Kulgery: Zip scanning is always enabled and 1 is added to zip depth -->
//      We begin scanning at the level of the original message and attachments
//      are the first level of decomposer scanning. However, expectation is 
//      that we behave like other email extensions which begin scanning at
//      top-level attachments with ZipDepth referring to sub-items in attached
//      containers.
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_ReloadRTSConfig ( )
{
    auto    CIMailStorage   *pStorage;
    auto    MAIL_OPTIONS    options;
    auto    DWORD           dwResult;

    pStorage = s_pStorage;
    dwResult = ERROR_SUCCESS;

    memset( &options, 0, sizeof( options ) );

    // Read options to temp structure.

    options.bOnOff = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_RTSScanOnOff, 0);
    options.bReads = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_ScanOnAccess, 1);
    options.bWrites = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_ScanOnModify, 0);
    options.bZipFile = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_ScanZipFile, 1);

    options.ScanFilesType = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_FileTypes, 0);
    options.bOehOnOff = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_RTSOehScanOnOff, 0);
    options.bPopTlsDetect = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_PopTlsDetect, REG_DEFAULT_PopTlsDetect);
    options.bSmtpTlsDetect = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SmtpTlsDetect, REG_DEFAULT_SmtpTlsDetect);

    options.bInsertWarning = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_InsertWarning, 1);
    options.bChangeMessageSubject = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_ChangeSubject, 1);
    options.bEmailSender = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_EmailSender, 1);
    options.bEmailSelected = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_EmailSelected, 0);
    
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_Extensions, options.Exts, EXT_LIST_SIZE, "");

    // if we're scanning by extension and if there are no extensions, switch to scan-all-files

    if ((options.ScanFilesType == 1) && (!_tcslen(options.Exts)))
        options.ScanFilesType = 0;

    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SkipExts, options.SkipExts, EXT_LIST_SIZE, "");

    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_ZipExts, options.ZipExts, EXT_LIST_SIZE, DEFAULT_ARC_EXTS);

    options.bProgressWindow = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_EmailProgressWindow, TRUE);
    options.bProgressIcon   = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_EmailProgressIcon, TRUE);

    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_WarningSubject, options.WarningSubject, MAX_SUBJECT, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_WarningMessage, options.WarningMessage, MAX_MESSAGE, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_WarningInfectionInfo, options.WarningInfectionInfo, MAX_INFECT_INFO, "");

    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SenderSubject, options.SenderSubject, MAX_SUBJECT, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SenderMessage, options.SenderMessage, MAX_MESSAGE, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SenderInfectionInfo, options.SenderInfectionInfo, MAX_INFECT_INFO, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_AlertSenderServerName, options.SenderMailServer, MAX_INFECT_INFO, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_AlertSenderServerReversePath, options.SenderReversePath, MAX_INFECT_INFO, "");
    options.SenderMailPort = (WORD)pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_AlertSenderServerPort, 25);

    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_Recipients, options.SelectedRecipients, MAX_SELECTED, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SelectedSubject, options.SelectedSubject, MAX_SUBJECT, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SelectedMessage, options.SelectedMessage, MAX_MESSAGE, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_SelectedInfectionInfo, options.SelectedInfectionInfo, MAX_INFECT_INFO, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_AlertSelectedServerName, options.SelectedMailServer, MAX_MAIL_SERVER, "");
    pStorage->m_pfnsStorageManager->GetStr(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_AlertSelectedServerReversePath, options.SelectedReversePath, MAX_MAIL_SERVER, "");
    options.SelectedMailPort = (WORD)pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_AlertSelectedServerPort, 25);

    // Update the STORAGEDATA structure

    //*** Zip/Decomposer options kluge ***
    // Fudging value of zip scan to on, and increasing depth by 1. See Remarks in header....
    if ( options.bZipFile )
    {
        pStorage->m_storageInfo.RTSData->ZipDepth = 1 + pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_ZipDepth, 3);
    }
    else
    {
        // If zip scanning is not selected in the UI, force depth to 1 so we pick up the
        // attachments only.
        pStorage->m_storageInfo.RTSData->ZipDepth = 1;
    }

    options.bZipFile = 1;

    pStorage->m_storageInfo.RTSData->FileType = options.ScanFilesType;
    // removed: pStorage->m_storageInfo.RTSData->Types = pStorage->m_pfnsStorageManager->GetVal(pStorage->m_storageInfo.hRTSConfigKey, szReg_Val_ScanByType, 6);
    pStorage->m_storageInfo.RTSData->ZipFile = options.bZipFile;

    _tcscpy(pStorage->m_storageInfo.RTSData->Exts, options.Exts);
    _tcscpy(pStorage->m_storageInfo.RTSData->ZipExts, options.ZipExts);

    //&? Synchronize access to S.E.'s copy of options?

    memcpy( &pStorage->m_options, &options, sizeof( pStorage->m_options ) );

    // Update options in shared memory, will automatically fire options changed event for ccEmlPxy filter

    //&?
    pStorage->m_pfnsStorageManager->dPrintf( "CIMailStorage::sf_ReloadRTSConfig( )\n" );

    pStorage->m_commBuffer.WriteOptions( &options );

    return ( dwResult );
} // CIMailStorage::sf_ReloadRTSConfig()


//*************************************************************************
// CIMailStorage::sf_ReInit()
//
// Called by the Storage Manager (RTVScan) to initailize us with different
// flags ( S_WANT_RTS, S_WANT_WALK, S_WANT_CONFIG ).
//
// DWORD STORAGEAPI CIMailStorage::sf_ReInit ( DWORD flags )
//
// Parameters:
//      flags
//
// Return Values:
//
// Remarks:
//      We really only expect to be initialized with S_WANT_RTS and
//      don't care about other settings.
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_ReInit ( DWORD /*flags*/ )
{
    return ( ERROR_SUCCESS );
} // CIMailStorage::sf_ReInit()


//*************************************************************************
// CIMailStorage::sf_ProcessPacket()
//
// DWORD STORAGEAPI CIMailStorage::sf_ProcessPacket ( WORD         FunctionCode,
//                                                    BYTE *       sendBuffer,
//                                                    DWORD        sendBufferSize,
//                                                    BYTE *       replyBuffer,
//                                                    DWORD *      replyBufferSize,
//                                                    CBA_Addr *   address )
//
// Storage extension custom communication mechanism.
// Not used for Internet Mail storage extension.
//
// Return Values:
//      ERROR_FUNCTION_NOT_SUPPORTED
//
// Remarks:
//      Old Intel documentation states it's for communication between a configuration
//      snap-in and its related storage extension.
//
//      Currently used by RTVScan to request log information from SymProtect storage.
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_ProcessPacket ( WORD         /*FunctionCode*/,
                                                   BYTE *       /*sendBuffer*/,
                                                   DWORD        /*sendBufferSize*/,
                                                   BYTE *       /*replyBuffer*/,
                                                   DWORD *      /*replyBufferSize*/,
                                                   CBA_Addr *   /*address*/ )
{
    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::sf_ProcessPacket()


//*************************************************************************
//  CIMailStorage::formatMailString formats message text which may contain mail specific
//  format specifiers.  These include EmailMessageSubject, InfectedAttachmentName,
//  EmailSender.
//
//  Parameters:
//      pMsgBuf         [out] A buffer which will contain formatted message text.
//      nNumMsgBufBytes [in]  Size of pMsgBuf in bytes including space for EOS.
//      pMsgFmt         [in]  A pointer to a format string.
//      eb              [in]  A pointer to an EVENTBLOCK structure.
//
//  Returns:
//      ERROR_SUCCESS
//*************************************************************************
// 2003.07.20 DALLEE pulled from Notes extension scancontext.c
//*************************************************************************

DWORD CIMailStorage::formatMailString( LPTSTR pMsgBuf, size_t nNumMsgBufBytes, LPCTSTR pMsgFmt, PEVENTBLOCK eb, DWORD userParam )
{
    TCHAR szTempText[ MAX_PATH ];

    userParam;

    // Check for space for at least one char for EOS

    if ( nNumMsgBufBytes < sizeof(pMsgBuf[0]) )
    {
        return ( ERROR_GENERAL );
    }

    // Initialize output buffer

    pMsgBuf[0] = _T('\0');

    char szMyMsgFmt[MAX_MESSAGE_SIZE];
    vpstrncpy (szMyMsgFmt, pMsgFmt, sizeof (szMyMsgFmt));

    LPTSTR pLast    = szMyMsgFmt;
    LPTSTR pBegin   = szMyMsgFmt;
    DWORD  dwResult = ERROR_GENERAL;

    PIEM_NODE_CONTEXT pNodeContext = static_cast<PIEM_NODE_CONTEXT>( eb->so->Node->Context );
    PIEM_MAIL_INFO    pMailInfo    = s_pStorage->m_commBuffer.GetMailInfoBlockFromIndex( pNodeContext->dwMailInfoBufferIndex );

    if ( NULL != pMailInfo )
    {
        for ( pBegin = _tcschr( szMyMsgFmt, _T('~') ); NULL != pBegin; pBegin = _tcschr( pBegin, _T('~') ) )
        {
            LPTSTR pTmp  = _tcsinc(pBegin);
            LPTSTR pText = NULL;

            switch(*pTmp)
            {
            case 'u':   // EmailMessageSubject
            case 'U':
                pText = pMailInfo->szSubject;
                break;

            case 'o':   // OriginalAttachmentName
            case 'O':
                *szTempText = _T('\0');
                GetOriginalAttachmentName( szTempText, eb, sizeof( szTempText ) / sizeof( *szTempText ) );
                pText = szTempText;
                break;

            case 'd':   // EmailSender
            case 'D':
                pText = pMailInfo->szSender;
                break;

            case 'i':   // EmailRecipients
            case 'I':
                pText = pMailInfo->szRecipients;
                break;
            }

            if(pText)
            {
                *pBegin = 0;

                // Copy everything up to the ~
                vpstrnappend(pMsgBuf, pLast, nNumMsgBufBytes);

                // Copy the new text
                vpstrnappend(pMsgBuf, pText, nNumMsgBufBytes);

                pLast = _tcsinc(pTmp);
            }

            pBegin = _tcsinc(pBegin);
        }

    	vpstrnappend(pMsgBuf, pLast, nNumMsgBufBytes);

        dwResult = ERROR_SUCCESS;
    }

    return ( dwResult );
} // CIMailStorage::formatMailString()


//*************************************************************************
// CIMailStorage::sf_FormatMessage()
//
// DWORD STORAGEAPI CIMailStorage::sf_FormatMessage ( char *out, size_t nNumOutBytes, const char *format, PEVENTBLOCK eb )
//
// Parameters:
//      *out
//      *format
//      eb
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_FormatMessage ( char *out, size_t nNumOutBytes, const char *format, PEVENTBLOCK eb, DWORD userParam )
{
    auto    DWORD   dwResult;

    dwResult = ERROR_GENERAL;

    if ( ( NULL != out ) && ( NULL != format ) )
    {
        dwResult = formatMailString( out, nNumOutBytes, format, eb, userParam );
    }

    return ( dwResult );
} // CIMailStorage::sf_FormatMessage()


//*************************************************************************
// CIMailStorage::sf_ChangeUser()
//
// DWORD STORAGEAPI CIMailStorage::sf_ChangeUser ( char *pszUser, HANDLE hAccessToken )
//
// Parameters:
//      *pszUser
//      hAccessToken
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.12 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::sf_ChangeUser ( char *pszUser, HANDLE hAccessToken )
{
    hAccessToken;

    _tcsncpy( s_pStorage->m_szCurrentUser, pszUser, sizeof( s_pStorage->m_szCurrentUser ) / sizeof( *s_pStorage->m_szCurrentUser ) - 1 );
    s_pStorage->m_szCurrentUser[ sizeof( s_pStorage->m_szCurrentUser ) / sizeof( *s_pStorage->m_szCurrentUser ) - 1 ] = _T('\0');

    return ( ERROR_SUCCESS );
} // CIMailStorage::sf_ChangeUser()


//*****************************************************************************
// Storage Node IO Functions (SFILETABLE implementations)
//*****************************************************************************

//*************************************************************************
// CIMailStorage::io_open()
//
// DWORD STORAGEAPI CIMailStorage::io_open ( PSNODE pNode, DWORD dwAccess )
//
// Parameters:
//      pNode
//      dwAccess
//
// Return Values:
//      On success, a valid file handle which can be used with other FILETABLE functions.
//      INVALID_HANDLE_VALUE on error.
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_open ( PSNODE pNode, DWORD dwAccess )
{
    auto    PIEM_NODE_CONTEXT   pNodeContext;
    auto    PIEM_MAIL_INFO      pFileInfo;
    auto    DWORD               dwResult;

    auto    int         nOpenMode;
    auto    int         nShareMode;

    dwResult = reinterpret_cast<DWORD>( INVALID_HANDLE_VALUE );

    pNodeContext = static_cast<PIEM_NODE_CONTEXT>( pNode->Context );
    pFileInfo = s_pStorage->m_commBuffer.GetMailInfoBlockFromIndex( pNodeContext->dwMailInfoBufferIndex );

    if ( NULL != pFileInfo )
    {
        nOpenMode  = \
        nShareMode = 0;

        // Somewhat odd translation here for open mode:
        // FTO_RDONLY and FTO_WRONLY can be combined (and form FTO_RDWR).
        // Standard run-time _O_RDONLY, _O_WRONLY, and _O_RDWR are mutually exclusive.

        switch ( dwAccess & ( FTO_RDWR | FTO_RDONLY | FTO_WRONLY ) )
        {
        default:
        case FTO_RDONLY:    nOpenMode |= _O_RDONLY; break;
        case FTO_WRONLY:    nOpenMode |= _O_WRONLY; break;
        case FTO_RDWR:      nOpenMode |= _O_RDWR;   break;
        }

        if ( dwAccess & FTO_APPEND )    nOpenMode |= _O_APPEND;
        if ( dwAccess & FTO_TRUNC )     nOpenMode |= _O_TRUNC;
        if ( dwAccess & FTO_BINARY )    nOpenMode |= _O_BINARY;
        if ( dwAccess & FTO_TEXT )      nOpenMode |= _O_TEXT;

        if ( dwAccess & FTSH_DENYNONE ) nShareMode |= _SH_DENYNO;

        //&?
        nShareMode |= _SH_DENYNO;

        dwResult = _sopen( pFileInfo->szFilePath, nOpenMode, nShareMode );
    }

    return ( dwResult );
} // CIMailStorage::io_open()


//*************************************************************************
// CIMailStorage::io_close()
//
// DWORD STORAGEAPI CIMailStorage::io_close ( DWORD hFile )
//
// Parameters:
//      hFile
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_close ( DWORD hFile )
{
    return ( _close( hFile ) );
} // CIMailStorage::io_close()


//*************************************************************************
// CIMailStorage::io_read()
//
// DWORD STORAGEAPI CIMailStorage::io_read ( DWORD hFile, void *pvData, DWORD dwLength )
//
// Parameters:
//      hFile
//      *pvData
//      dwLength
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_read ( DWORD hFile, void *pvData, DWORD dwLength )
{
    return ( _read( hFile, pvData, dwLength ) );
} // CIMailStorage::io_read()


//*************************************************************************
// CIMailStorage::io_write()
//
// DWORD STORAGEAPI CIMailStorage::io_write ( DWORD hFile, void *pvData, DWORD dwLength )
//
// Parameters:
//      hFile
//      *pvData
//      dwLength
//
// Return Values:
//      Number of bytes written on success. (DWORD) -1 on error.
//
// Remarks:
//      Will truncate the file at the current position if a request to write 0 bytes is made.
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_write ( DWORD hFile, void *pvData, DWORD dwLength )
{
    auto DWORD dwResult;

    if ( 0 == dwLength )
    {
        auto    long    lPosition;

        lPosition = _lseek( hFile, 0, SEEK_CUR );

        if ( -1 != lPosition )
            dwResult = _chsize( hFile, lPosition );
        else
            dwResult = -1;
    }
    else
    {
        dwResult = _write( hFile, pvData, dwLength );
    }

    return ( dwResult );
} // CIMailStorage::io_write()


//*************************************************************************
// CIMailStorage::io_lseek()
//
// DWORD STORAGEAPI CIMailStorage::io_lseek ( DWORD hFile, long lOffset, DWORD dwOrigin )
//
// Parameters:
//      hFile
//      lOffset
//      dwOrigin
//
// Return Values:
//      New offset of file pointer.
//      On error, (DWORD)-1.
//
// Remarks:
//      Assuming that origin flags for dwOrigin match those in standard run-time
//      _lseek(). This is the implementation used in Notes and MS Exchange
//      extensions.
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_lseek ( DWORD hFile, long lOffset, DWORD dwOrigin )
{
    return ( _lseek( hFile, lOffset, dwOrigin ) );
} // CIMailStorage::io_lseek()


//*************************************************************************
// CIMailStorage::io_access()
//
// DWORD STORAGEAPI CIMailStorage::io_access ( PSNODE pNode, int nAccess )
//
// Determine access to an SNODE.
//
// Parameters:
//      pNode
//      nAccess     access to verify, may be 00 Existence only
//                                           02 Write permission
//                                           04 Read permission
//                                           06 Read and write permission
//
// Return Values:
//      0 if specified access is available to the SNODE.
//      Non-zero otherwise.
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_access ( PSNODE pNode, int nAccess )
{
    auto    PIEM_NODE_CONTEXT   pNodeContext;
    auto    PIEM_MAIL_INFO      pFileInfo;
    auto    DWORD               dwResult;

    dwResult = ERROR_BAD_PARAM;

    pNodeContext = static_cast<PIEM_NODE_CONTEXT>( pNode->Context );
    pFileInfo = s_pStorage->m_commBuffer.GetMailInfoBlockFromIndex( pNodeContext->dwMailInfoBufferIndex );

    if ( NULL != pFileInfo )
    {
        dwResult = _access( pFileInfo->szFilePath, nAccess );
    }

    return ( dwResult );
} // CIMailStorage::io_access()


//*************************************************************************
// CIMailStorage::io_GetState()
//
// DWORD STORAGEAPI CIMailStorage::io_GetState ( PSNODE pNode, BYTE *pData )
//
// Save file time and attribute information about a node.
// The storage manager (RTVScan) calls this function for walk scans on the
// file system storage -- and not for real-time scans.
//
// VirusBin calls this function when used to create a backup before repair.
// This call *is* made for real-time scans, but a full implementation is
// apparently not required.
//
// Matching non-implementations for this function and io_SetState()
// for internet email real-time scan.
//
// Return Values:
//      ERROR_FUNCTION_NOT_SUPPORTED
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_GetState ( PSNODE /*pNode*/, BYTE * /*pData*/ )
{
    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::io_GetState()


//*************************************************************************
// CIMailStorage::io_SetState()
//
// DWORD STORAGEAPI CIMailStorage::io_SetState ( PSNODE pNode, BYTE *pData )
//
// Restore file time and attribute information previously saved by GetState().
// The storage manager (RTVScan) calls this function for walk scans on the
// file system storage -- and not for real-time scans.
//
// VirusBin calls this function when used to create a backup before repair.
// This call *is* made for real-time scans, but a full implementation is
// apparently not required.
//
// Matching non-implementations for this function and io_GetState()
// for internet email real-time scan.
//
// Return Values:
//      ERROR_FUNCTION_NOT_SUPPORTED
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_SetState ( PSNODE /*pNode*/, BYTE * /*pData*/ )
{
    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::io_SetState()


//*************************************************************************
// CIMailStorage::io_GetExtendedData()
//
// DWORD STORAGEAPI CIMailStorage::io_GetExtendedData ( PSNODE pNode, char *KeyName, BYTE *pData, DWORD dwLength )
//
// No documentation available for this function. As implemented in the file
// system storage, this function reads alternate data streams from a file.
//
// Not implemented for Internet Mail extension.
//
// Return Values:
//      ERROR_FUNCTION_NOT_SUPPORTED
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_GetExtendedData ( PSNODE /*pNode*/, char * /*KeyName*/, BYTE * /*pData*/, DWORD /*dwLength*/ )
{
    // Assertion just to be sure this is not being used.
    SAVASSERT( 0 );

    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::io_GetExtendedData()


//*************************************************************************
// CIMailStorage::io_SetExtendedData()
//
// DWORD STORAGEAPI CIMailStorage::io_SetExtendedData ( PSNODE pNode, char *KeyName, BYTE *pData, DWORD dwLength )
//
// No documentation available for this function. As implemented in the file
// system storage, this function writes data to an alternate stream in a file.
//
// Not implemented for Internet Mail extension.
//
// Return Values:
//      ERROR_FUNCTION_NOT_SUPPORTED
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_SetExtendedData ( PSNODE /*pNode*/, char * /*KeyName*/, BYTE * /*pData*/, DWORD /*dwLength*/ )
{
    // Assertion just to be sure this is not being used.
    SAVASSERT( 0 );

    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::io_SetExtendedData()


//*************************************************************************
// CIMailStorage::io_GetFullKey()
//
// DWORD STORAGEAPI CIMailStorage::io_GetFullKey ( PSNODE pNode, char *Buffer, DWORD dwLength )
//
// This function should return the full path excluding volume (instance) information
// for the given SNODE.
//
// Not implemented for Internet Mail extension -- no reasonable representation
// for "path" of email attachments.
//
// Return Values:
//      ERROR_FUNCTION_NOT_SUPPORTED
//*************************************************************************
// 2003.06.16 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::io_GetFullKey ( PSNODE /*pNode*/, char * /*Buffer*/, DWORD /*dwLength*/ )
{
    //&? Find if this gets called for mail storage extensions, and where exactly.

    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // // CIMailStorage::io_GetFullKey()


//*************************************************************************
// Storage Node Functions (NODEFUNCTIONS implementations)
//*************************************************************************

//*************************************************************************
// CIMailStorage::nf_ReleaseSNode()
//
// DWORD STORAGEAPI CIMailStorage::nf_ReleaseSNode ( PSNODE pNode )
//
// Notification from Storage Manager that it is done processing this node.
//
// Parameters:
//      pNode
//
// Return Values:
//      ERROR_SUCCESS
//
// Remarks:
//      No cleanup of this extension's data here -- will take care of it in
//      post-processing in BeginRTSWatch().
//*************************************************************************
// 2003.06.17 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::nf_ReleaseSNode ( PSNODE /*pNode*/ )
{
    return ( ERROR_SUCCESS );
} // CIMailStorage::nf_ReleaseSNode()


//*************************************************************************
// CIMailStorage::nf_NodeHasViruses()
//
// Callback from Storage Manager (RTVScan) when infections are found
// in a storage node passed to ProcessRTSNode().
//
// Saves infection information and response to take for the email session
// (whether to drop the message and/or entire email session).
//
// DWORD STORAGEAPI CIMailStorage::nf_NodeHasViruses ( PSNODE pNode, PEVENTBLOCK pEventBlock )
//
// Parameters:
//      pNode
//      pEventBlock
//
// Return Values:
//      ERROR_SUCCESS
//
// Remarks:
//      This routine is potentially called multiple times for storage nodes
//      describing containers -- once per infection found, and once per container.
//
//      Saved information acted on when scanning is complete and control
//      returns to BeginRTSWatch().
//*************************************************************************
// 2003.06.17 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::nf_NodeHasViruses ( PSNODE pNode, PEVENTBLOCK pEventBlock )
{
    auto    PIEM_NODE_CONTEXT           pNodeContext;
    auto    IEM_ATTACHMENT_INFO_LIST    attachmentInfo;

    auto    TCHAR   szTemp [ MAX_MESSAGE ];

    // If this is an actual infection notification (rather than a container notification),
    // fill in the attachment information and add to list in node context.

    if ( pEventBlock->VirusName && *pEventBlock->VirusName )
    {
        pNodeContext = static_cast<PIEM_NODE_CONTEXT>( pNode->Context );

        // Need to do formatting of subject and message body text here while we still have
        // access to an EVENTBLOCK.

        // Insert Warning
        s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.WarningSubject, pEventBlock, 0 );
        formatMailString( pNodeContext->szWarningSubject, sizeof (pNodeContext->szWarningSubject), szTemp, pEventBlock, 0 );

        s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.WarningMessage, pEventBlock, 0 );
        formatMailString( pNodeContext->szWarningMessage, sizeof (pNodeContext->szWarningMessage), szTemp, pEventBlock, 0 );

        // Email Sender for non-Outbound E-mail Heuristics based threats.
        // It is foolish to e-mail the sender for OEH. The sender will likely
        // be manufactured by the worm.
        if ( pEventBlock->VirusID != VE_OEH_VIRUS_ID )
        {
            pNodeContext->bAlertSender = s_pStorage->m_options.bEmailSender? true : false;

            s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.SenderSubject, pEventBlock, 0 );
            formatMailString( pNodeContext->szSenderSubject, sizeof (pNodeContext->szSenderSubject), szTemp, pEventBlock, 0 );

            s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.SenderMessage, pEventBlock, 0 );
            formatMailString( pNodeContext->szSenderMessage, sizeof (pNodeContext->szSenderMessage), szTemp, pEventBlock, 0 );
        }

        // Email Selected
        s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.SelectedSubject, pEventBlock, 0 );
        formatMailString( pNodeContext->szSelectedSubject, sizeof (pNodeContext->szSelectedSubject), szTemp, pEventBlock, 0 );

        s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.SelectedMessage, pEventBlock, 0 );
        formatMailString( pNodeContext->szSelectedMessage, sizeof (pNodeContext->szSelectedMessage), szTemp, pEventBlock, 0 );

        // Fill in attachment information - name and formatted infection messages.

        memset( &attachmentInfo, 0, sizeof( attachmentInfo ) );

        GetOriginalAttachmentName( attachmentInfo.szAttachmentName, pEventBlock, sizeof( attachmentInfo.szAttachmentName ) / sizeof( *attachmentInfo.szAttachmentName ) );

        s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.WarningInfectionInfo, pEventBlock, 0 );
        formatMailString( attachmentInfo.szWarningInfectionInfo, sizeof (attachmentInfo.szWarningInfectionInfo), szTemp, pEventBlock, 0 );

        s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.SenderInfectionInfo, pEventBlock, 0 );
        formatMailString( attachmentInfo.szSenderInfectionInfo, sizeof (attachmentInfo.szSenderInfectionInfo), szTemp, pEventBlock, 0 );

        s_pStorage->m_pfnsStorageManager->FormatMessage( szTemp, sizeof (szTemp), s_pStorage->m_options.SelectedInfectionInfo, pEventBlock, 0 );
        formatMailString( attachmentInfo.szSelectedInfectionInfo, sizeof (attachmentInfo.szSelectedInfectionInfo), szTemp, pEventBlock, 0 );

        // Add/update attachment information stored in node context

        AddAttachmentInfo( pNodeContext, &attachmentInfo );

        // If this method is dealing with a Outbound E-mail Heuristics
        // detection and the user wanted something done to prevent the
        // spread, drop the entire SMTP session.
        if (pEventBlock->VirusID == VE_OEH_VIRUS_ID &&
            pEventBlock->WantedAction[0] != AC_NOTHING)
        {
            SetResponse (pNode, IEM_DROP_SESSION);
        }
    }

    return ( ERROR_SUCCESS );
} // CIMailStorage::nf_NodeHasViruses()


//*************************************************************************
// CIMailStorage::nf_RenameNode()
//
// DWORD STORAGEAPI CIMailStorage::nf_RenameNode ( PSNODE pNode, char * szNewExt )
//
// Change the extension of the file specified by the SNODE.
// This response option is no longer available.
//
// Not used (and not implemented here) for real-time email scanning.
//
// Return Values:
//      ERROR_FUNCTION_NOT_SUPPORTED
//*************************************************************************
// 2003.06.17 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::nf_RenameNode ( PSNODE /*pNode*/, char * /*szNewExt*/ )
{
    // Assertion just to be sure this is not being used.
    SAVASSERT( 0 );

    return ( ERROR_FUNCTION_NOT_SUPPORTED );
} // CIMailStorage::nf_RenameNode()

//*************************************************************************
// CIMailStorage::SetResponse()
//
// Called to set the response. This method ensures that there is an ever
// escalating response and never a decreased response. In other words, 
// IEM_DROP_MESSAGE can override IEM_CONTINUE. IEM_DROP_SESSION overrides
// IEM_CONTINUE and IEM_DROP_MESSAGE.
//
// DWORD CIMailStorage::SetResponse( PSNODE pNode, IEM_RESPONSE eNew )
//
// Parameters:
//      pNode
//      eNew
//
// Return Values:
//      ERROR_SUCCESS
//*************************************************************************
// 2003.09.10 JBRENNAN - created.
//*************************************************************************
DWORD CIMailStorage::SetResponse( PSNODE pNode, IEM_RESPONSE eNew )
{
    // Flag message or attachment for removal...

    auto    PIEM_NODE_CONTEXT   pNodeContext;
    auto    PIEM_MAIL_INFO      pMailInfo;

    auto    DWORD               dwResult = ERROR_NO_COMM;

    pNodeContext = static_cast<PIEM_NODE_CONTEXT>( pNode->Context );
    pMailInfo = s_pStorage->m_commBuffer.GetMailInfoBlockFromIndex( pNodeContext->dwMailInfoBufferIndex );

    if ( NULL != pMailInfo )
    {
        // Only record escalated results.
        if (eNew > pMailInfo->nResponse)
            pMailInfo->nResponse = eNew;
        dwResult = ERROR_SUCCESS;
    }

    return ( dwResult );
}

//*************************************************************************
// CIMailStorage::nf_RemoveNode()
//
// Called by the storage manager when scan options indicate that the
// node should be deleted.
//
// This may occur when there is an infection in the actual message text or
// header and options are set to delete. Or when the storage manager wants to 
// move then entire email into Quarantine.
//
// DWORD STORAGEAPI CIMailStorage::nf_RemoveNode ( PSNODE pNode )
//
// Parameters:
//      pNode
//
// Return Values:
//      ERROR_SUCCESS
//
// Remarks:
//      Relies on later processing by ccEmailProxy to drop the message,
//      but assuming communication with our filter plug-in works ccEmailProxy's
//      drop message should be guaranteed.
//*************************************************************************
// 2003.06.17 DALLEE - created.
//*************************************************************************

DWORD STORAGEAPI CIMailStorage::nf_RemoveNode ( PSNODE pNode )
{
    // Flag message or attachment for removal...
    return SetResponse (pNode, IEM_DROP_MESSAGE);
} // CIMailStorage::nf_RemoveNode()


//*************************************************************************
// AddAttachmentInfo()
//
// This routine adds or updates the infection information for a given
// attachment to our storage extension's SNODE context information.
//
// This is infection message information built and saved during
// CIMailStorage::nf_NodeHasViruses().
//
// void AddAttachmentInfo( PIEM_NODE_CONTEXT pNodeContext, PIEM_ATTACHMENT_INFO_LIST pAttachmentInfo )
//
// Parameters:
//      pNodeContext
//      pAttachmentInfo
//
// Return Values:
//
// Remarks:
//      Before deleting our node context, DeleteAttachmentInfo() must be called
//      to free memory allocated by this routine.
//*************************************************************************
// 2003.08.24 DALLEE - created.
//*************************************************************************

void AddAttachmentInfo( PIEM_NODE_CONTEXT pNodeContext, PIEM_ATTACHMENT_INFO_LIST pAttachmentInfo )
{
    auto    PIEM_ATTACHMENT_INFO_LIST   pNewNode;
    auto    PIEM_ATTACHMENT_INFO_LIST   pListNode;
    auto    bool    bFound;

    // Check if we already have information for this attachment

    bFound = false;

    pListNode = pNodeContext->pAttachmentInfo;

    while ( pListNode )
    {
        if ( 0 == _tcsicmp( pAttachmentInfo->szAttachmentName, pListNode->szAttachmentName ) )
        {
            bFound = true;
            break;
        }

        pListNode = pListNode->pNext;
    }

    // Didn't find it. Add new node.

    try
    {
        if ( !bFound )
        {
            pNewNode = new IEM_ATTACHMENT_INFO_LIST;

            *pNewNode = *pAttachmentInfo;

            pNewNode->pNext = pNodeContext->pAttachmentInfo;
            pNodeContext->pAttachmentInfo = pNewNode;
        }
    }
    catch (std::bad_alloc &){}
} // AddAttachmentInfo()

//*************************************************************************
// DeleteAttachmentInfo()
//
// This routine frees the memory allocated by AddAttachmentInfo().
// Must be called before deleting out node context.
//
// void DeleteAttachmentInfo( PIEM_NODE_CONTEXT pNodeContext )
//
// Parameters:
//      pNodeContext
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.24 DALLEE - created.
//*************************************************************************

void DeleteAttachmentInfo( PIEM_NODE_CONTEXT pNodeContext )
{
    auto    PIEM_ATTACHMENT_INFO_LIST   pLast;

    while ( pNodeContext->pAttachmentInfo )
    {
        pLast = pNodeContext->pAttachmentInfo;

        pNodeContext->pAttachmentInfo = pLast->pNext;

        delete pLast;
    }
} // DeleteAttachmentInfo()


//*************************************************************************
// GetOriginalAttachmentName()
//
// This routine retrieves the name of the top level email attachment
// described by the specified event.
// 
//
// static size_t GetOriginalAttachmentName( lpszAttachment, PEVENTBLOCK eb, size_t cchAttachment )
//
// Parameters:
//      lpszAttachment                  Buffer to receive attachment name
//      eb                              Description of event (infection)
//      cchAttachment                   Size of lpszAttachment in characters
//
// Return Values:
//      0 on error, or
//      Number of characters copied to lpszAttachment excluding EOS, or required size of
//      buffer including EOS.
//
// Remarks:
//      Since we begin scanning at the message body level, the attachment
//      name is the first section of the nesting path from decomposer.
//      If present, copy the underlined portion from eb->pdfi->lpszNestingPath:
//      lpszNestingPath ">>Top-level decomposed item (original attachment)>>subitem>>subitem"
//                         ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//      If this is an event associated with the body text (not attachments) of
//      the email message, the name will instead be taken as eb->Description.
//*************************************************************************
// 2003.08.20 DALLEE - created.
//*************************************************************************

static size_t GetOriginalAttachmentName( LPTSTR lpszAttachment, PEVENTBLOCK eb, size_t cchAttachment )
{
    auto    size_t  nReturn = 0;

    // Looking first for decomposer info (this is an attachment).
    // Then for top-level description (this is an event about the message body...)
    // Else error...

    if ( ( NULL != eb->pdfi ) &&
         ( NULL != eb->pdfi->lpszNestingPath ) )
    {
        // An attachment -- copy the underlined portion from eb->pdfi->lpszNestingPath:
        // lpszNestingPath ">>Top-level decomposed item (original attachment)>>subitem>>subitem"
        //                    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        auto    LPCTSTR     lpszStartAttachment;
        auto    LPCTSTR     lpszEndAttachment;
        auto    size_t      nChars;

        // lpszStartAttachment -- Start of original attachment name
        // lpszEndAttachment -- Character after original attachment name. Either '>' or EOS.

        lpszStartAttachment = eb->pdfi->lpszNestingPath;

        while ( _T('>') == *lpszStartAttachment )
        {
            lpszStartAttachment = _tcsinc( lpszStartAttachment );
        }

        lpszEndAttachment = _tcschr( lpszStartAttachment, _T('>') );

        if ( NULL == lpszEndAttachment )
        {
            lpszEndAttachment = lpszStartAttachment + _tcslen( lpszStartAttachment );
        }

        // Copy attachment name

        nChars = lpszEndAttachment - lpszStartAttachment;

        if ( cchAttachment > 0 )
        {
            _tcsncpy( lpszAttachment, lpszStartAttachment, min( nChars, cchAttachment ) );
            lpszAttachment[ min( nChars, cchAttachment - 1 ) ] = _T('\0');
        }

        // Return number of characters copied
        // Or required buffer size if buffer was too small for entire name

        nReturn = nChars;

        if ( nReturn >= cchAttachment )
        {
            ++nReturn;
        }
    }
    else if ( NULL != eb->Description )
    {
        // Refers to body of email message -- return description of message

        if ( cchAttachment > 0 )
        {
            _tcsncpy( lpszAttachment, eb->Description, cchAttachment );
            lpszAttachment[ cchAttachment - 1 ] = _T('\0');
        }

        // Return number of characters copied
        // Or required buffer size if buffer was too small for entire name

        nReturn = _tcslen( eb->Description );

        if ( nReturn >= cchAttachment )
        {
            ++nReturn;
        }
    }
    else
    {
        // Unexpected...

        if ( cchAttachment > 0 )
        {
            *lpszAttachment = _T('\0');
        }
    }

    return ( nReturn );
} // GetOriginalAttachmentName()


//*************************************************************************
// GetFakeSID()
//
// GetFakeSID formats a static "fake" SID for identification purposes.
// the SID is used as an indentifier for an SNODE.
//
// static void GetFakeSID( mSID * SID, LPCTSTR lpszUser )
//
// Parameters:
//      SID
//      lpszUser
//
// Return Values:
//
// Remarks:
//
//*************************************************************************
// 2003.08.26 DALLEE - Based on Notes extension GetFakeSID() in notesext.c
//*************************************************************************

static void GetFakeSID( mSID * SID, LPCTSTR lpszUser )
{
    // note that SID_SIZE is defined as 64, and that a SID is
    // defined as an array of DWORD [SID_SIZE], so the actual
    // number of TCHARs in the array is a multiple of SID_SIZE;
    // 4*SID_SIZE for chars, and 2*SID_SIZE for wchars.

    auto    DWORD       maxLen = SID_SIZE * (sizeof(DWORD) / sizeof(TCHAR));
    auto    mSID        mySID = {0};
    auto    LPTSTR      lpszStringData;

    const   TCHAR       szSep[] = _T("\x01");

    mySID.stuff[0] = 0xFFFFFFFF;
    maxLen -= sizeof(DWORD) / sizeof(TCHAR);

    // Copy in User and Computer data

    lpszStringData = reinterpret_cast<LPTSTR>( &mySID.stuff[1] );

    // Copy user name up to size of buffer - szSep - EOS
    // strncat( d, s, count ) will copy count characters + EOS(always)

    *lpszStringData = _T('\0');
    _tcsncat( lpszStringData, lpszUser, maxLen - _tcslen( szSep ) - 1 );
    _tcscat( lpszStringData, szSep );

    maxLen -= _tcslen( lpszStringData );

    GetComputerName( lpszStringData + _tcslen( lpszStringData ), &maxLen );

    *SID = mySID;
} // GetFakeSID()

