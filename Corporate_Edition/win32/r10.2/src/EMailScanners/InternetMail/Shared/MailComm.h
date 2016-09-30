// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
// MailComm.h: interface for the CMailCommBuffer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAILCOMM_H__79E4FC1F_AD50_4258_A88C_E9201258858B__INCLUDED_)
#define AFX_MAILCOMM_H__79E4FC1F_AD50_4258_A88C_E9201258858B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//
// IEM_MAIL_INFO 

// Timeout in ms waiting for IEM_MAIL_INFO_BUFFER mutex.

#define IEM_MAIL_BUFFER_TIMEOUT 1000

// 

typedef enum _IEM_BUFFER_USAGE
{
    IEM_BUFFER_FREE = 0,
    IEM_BUFFER_IN_USE,
    IEM_BUFFER_PENDING_SCAN,
    IEM_BUFFER_OWNED_BY_SCAN,
    IEM_BUFFER_SCANNED
} IEM_BUFFER_USAGE;

#define IEM_MAIL_BUFFER_NAME        _T("Global\\SAVCE_IEM_Buffer")
#define IEM_MAIL_BUFFER_SEMAPHORE   _T("Global\\SAVCE_IEM_Sem")
#define IEM_MAIL_BUFFER_MUTEX       _T("Global\\SAVCE_IEM_Mutex")
#define IEM_MAIL_BUFFER_EVENT       _T("Global\\SAVCE_IEM_Event%d")
#define IEM_MAIL_OPTIONS_EVENT      _T("Global\\SAVCE_IEM_Options")

// Number of buffers available for concurrent scanning.
// Not guaranteed to match # of storage extension RTSWatch threads.

#define IEM_MAIL_BUFFER_ENTRIES       8

//
// IEM_MAIL_INFO Description of individual buffer entry for scanning
//

typedef enum _IEM_PROTOCOL
{
    IEM_PROTOCOL_POP3 = 0,
    IEM_PROTOCOL_SMTP,
    IEM_PROTOCOL_UNKNOWN
} IEM_PROTOCOL;

typedef enum _IEM_RESPONSE
{
    IEM_CONTINUE = 0,
    IEM_DROP_MESSAGE,
    IEM_DROP_SESSION
} IEM_RESPONSE;

#define IEM_MAX_SUBJECT     512
#define IEM_MAX_SENDER      512
#define IEM_MAX_RECIPIENTS  1024

typedef struct _IEM_MAIL_INFO
{
    IEM_BUFFER_USAGE    nState;                     // enum IEM_BUFFER_USAGE, state/owner of buffer
    DWORD               nBufferIndex;               // index of the buffer entry

    //&? Need separate buffer and information structs...
    // members above only apply to buffer, not email info.
    //----------------------------------------------------

    TCHAR           szFilePath[ MAX_PATH ];         // path (and filename) of the extracted file

    TCHAR           szSubject[ IEM_MAX_SUBJECT ];
    TCHAR           szSender[ IEM_MAX_SENDER ];
    TCHAR           szRecipients[ IEM_MAX_RECIPIENTS ];

    IEM_PROTOCOL    nProtocol;                      // POP3, SMTP, ???, etc.
    BOOL            bOutbound;                      // boolean outbound email connection?

    DWORD           dwClientProcessId;              // PID of email client
    TCHAR           szClientFilePath[ MAX_PATH ];   //&? Bad limitation here...
                                                    // We dropping information from ccEmailProxy?
                                                    // Required field for OEH scanning...

    IEM_RESPONSE    nResponse;                      // Response to message. Continue (SE may have
                                                    // made changes to temp message file).
                                                    // Drop this message. Drop entire session/connection.
} IEM_MAIL_INFO, *PIEM_MAIL_INFO;

//
// MAIL_OPTIONS
//
// Internet Email storage extension internal options representation.
// Options format shared by both storage extension and email filter plug-in.
// 

#define MAX_SENDER              256
#define MAX_SUBJECT             256
#define MAX_SELECTED            512
#define MAX_INFECT_INFO         512
#define MAX_RECIPIENTS          512
#define MAX_MAIL_SERVER         256

// note that MAX_MESSAGE should be the longest of the
// consts above, and should NEVER be larger than MAX_MESSAGE_SIZE
#define MAX_MESSAGE             1024


typedef struct _MAIL_OPTIONS
{
    DWORD   bOnOff;                 //  Signifies whether realtime scanning is enabled
    DWORD   bReads;                 //  Signifies whether reads are enabled
    DWORD   bWrites;                //  Signifies whether writes are enables
    DWORD   bZipFile;               //  Controls scan of pkzip and lha files
    DWORD   ScanFilesType;          //  0=Scan all, 1=by ext // removed: 2=by type
    DWORD   bOehOnOff;              //  Signifies whether Outbound E-mail Heuristics scanning is enabled
    DWORD   bPopTlsDetect;          //  Detect and bypass filtering for encrypted POP traffic
    DWORD   bSmtpTlsDetect;         //  Detect and bypass filtering for encrypted SMTP traffic
    
    TCHAR   Exts[EXT_LIST_SIZE];    // valid extension filenames to be scanned
    TCHAR   SkipExts[EXT_LIST_SIZE];// Extensions to be skipped
    TCHAR   ZipExts[EXT_LIST_SIZE]; // valid extension filenames to be scanned

    DWORD   bProgressWindow;        // Display outgoing progress window
    DWORD   bProgressIcon;          // Display tray icon while scanning

    // Notification flags and buffers
    
    DWORD   bInsertWarning;         // Insert Warning message into infected message
    DWORD   bChangeMessageSubject;  // Change subject of infected message
    DWORD   bEmailSender;           // Send infection message to sender
    DWORD   bEmailSelected;         // Send infection message to selected list
    
    TCHAR   WarningSubject[MAX_SUBJECT];            // Body of unformatted message subject
    TCHAR   WarningMessage[MAX_MESSAGE];            // Body of unformatted message warning
    TCHAR   WarningInfectionInfo[MAX_INFECT_INFO];  // Body of unformatted infection info
    
    TCHAR   SenderSubject[MAX_SUBJECT];             // Body of unformatted message subject
    TCHAR   SenderMessage[MAX_MESSAGE];             // Body of unformatted message warning
    TCHAR   SenderInfectionInfo[MAX_INFECT_INFO];   // Body of unformatted infection info
    TCHAR   SenderMailServer[MAX_MAIL_SERVER];      // Mail server for sending alert emails.
    TCHAR   SenderReversePath[MAX_SENDER];          // SMTP "MAIL FROM:" reverse-path
    WORD    SenderMailPort;

    TCHAR   SelectedRecipients[MAX_SELECTED];       // List of selected email recipients
    TCHAR   SelectedSubject[MAX_SUBJECT];           // Body of unformatted subject
    TCHAR   SelectedMessage[MAX_MESSAGE];           // Body of unformatted message
    TCHAR   SelectedInfectionInfo[MAX_INFECT_INFO]; // Body of unformatted infection info
    TCHAR   SelectedMailServer[MAX_MAIL_SERVER];    // Mail server for sending alert emails.
    TCHAR   SelectedReversePath[MAX_SENDER];        // SMTP "MAIL FROM:" reverse-path
    WORD    SelectedMailPort;
} MAIL_OPTIONS, *PMAIL_OPTIONS;


//
// IEM_MAIL_INFO_BUFFER Shared memory buffer for scan requests
//

typedef struct _IEM_MAIL_INFO_BUFFER
{
    //&? Signature? versioning? size?
    unsigned int    producerIndex;              // Hook index counter.
    unsigned int    consumerIndex;              // Storage extension index counter.
    long            realTimeThreads;            // Count of running realtime threads.
    IEM_MAIL_INFO   buf[ IEM_MAIL_BUFFER_ENTRIES ];     // An array of IEM_MAIL_INFO blocks for scanning.

    MAIL_OPTIONS    options;                    // 
} IEM_MAIL_INFO_BUFFER, * PIEM_MAIL_INFO_BUFFER;


// Class for controlling access to iem file info buffer...
//&? Fix access issues... Lotes scheme too hands on.

class CMailCommBuffer  
{
public:
    CMailCommBuffer();
    virtual ~CMailCommBuffer();

public:
    DWORD   Init();                             // Initialize IPC
    DWORD   DeInit();                           // DeInitialize IPC

    DWORD   ProcessHookedMessage( PIEM_MAIL_INFO pMail, DWORD dwTimeout );

    PIEM_MAIL_INFO GetAvailableMailInfoBlock();
    void ReleaseMailInfoBlock( PIEM_MAIL_INFO pMailInfo );

    PIEM_MAIL_INFO GetWaitingMailInfoBlock();

    PIEM_MAIL_INFO GetMailInfoBlockFromIndex( DWORD dwIndex );

    DWORD   ReadOptions( PMAIL_OPTIONS pOptions );
    DWORD   WriteOptions( const PMAIL_OPTIONS pOptions );

    HANDLE      m_hBufferSemaphore;             // Count of buffers awaiting scan 
    HANDLE      m_hBufferMutex;                 // Access to m_pBuffer
    HANDLE      m_ahBufferEvents[ IEM_MAIL_BUFFER_ENTRIES ];    // Scan completed event per buffer entry
    HANDLE      m_hBuffer;                      // Handle to shared mem for communication
    IEM_MAIL_INFO_BUFFER *m_pBuffer;            // Shared mem for communication

    HANDLE      m_hOptionsChangedEvent;         // Manual reset, pulsed by WriteOptions()
};


//&? end IEM_MAIL_INFO
//*****************************************************************************


#endif // !defined(AFX_MAILCOMM_H__79E4FC1F_AD50_4258_A88C_E9201258858B__INCLUDED_)

