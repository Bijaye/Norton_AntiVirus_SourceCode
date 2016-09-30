// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
// IMailStorage.h:
//
// Declaration of the CIMailStorage class.
// This is the Internet Mail (POP3/SMTP) Storage Extension.
//
//*************************************************************************

#if !defined(AFX_IMAILSTORAGE_H__0402DDA6_2435_41B5_9D24_C56C63394C1A__INCLUDED_)
#define AFX_IMAILSTORAGE_H__0402DDA6_2435_41B5_9D24_C56C63394C1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//
// Functions passed to and from RTVScan all use _cdecl calling convention
//

#define STORAGEAPI  _cdecl


#ifdef __cplusplus
extern "C" {
#endif

__declspec( dllexport ) DWORD STORAGEAPI ImStorageInit( DWORD flags, PSTORAGEINFO * pInfo, HANDLE * hSEInstance, PSSFUNCTIONS pSSFuns );

#ifdef __cplusplus
} // extern "C"
#endif

//
// IEM_ATTACHMENT_INFO_LIST list for saving per attachment infection information.
//

typedef struct _IEM_ATTACHMENT_INFO_LIST
{
    TCHAR   szAttachmentName[MAX_PATH];

    TCHAR   szWarningInfectionInfo[MAX_INFECT_INFO];
    TCHAR   szSenderInfectionInfo[MAX_INFECT_INFO];
    TCHAR   szSelectedInfectionInfo[MAX_INFECT_INFO];

    struct _IEM_ATTACHMENT_INFO_LIST    *pNext;
} IEM_ATTACHMENT_INFO_LIST, * PIEM_ATTACHMENT_INFO_LIST;

//
// IEM_NODE_CONTEXT
// Context information stored in SNODE. Tracks per attachment infection
// information and reference to IEM_MAIL_INFO_BUFFER entry.
//

typedef struct _IEM_NODE_CONTEXT
{
    DWORD   dwMailInfoBufferIndex;              // Index to IEM_MAIL_INFO in shared memory buffer

    TCHAR   szWarningSubject[MAX_SUBJECT];      // Insert Warning Message: Formatted subject
    TCHAR   szWarningMessage[MAX_MESSAGE];      // Insert Warning Message: Formatted warning message

	bool    bAlertSender;                       // Alert the sender of an infection? true / false
    TCHAR   szSenderSubject[MAX_SUBJECT];       // Email Sender: Formatted subject
    TCHAR   szSenderMessage[MAX_MESSAGE];       // Email Sender: Formatted warning message

    TCHAR   szSelectedSubject[MAX_SUBJECT];     // Email Selected: Formatted subject
    TCHAR   szSelectedMessage[MAX_MESSAGE];     // Email Selected: Formatted warning message

    PIEM_ATTACHMENT_INFO_LIST   pAttachmentInfo;// Per attachment infection messages.
} IEM_NODE_CONTEXT, * PIEM_NODE_CONTEXT;

void AddAttachmentInfo( PIEM_NODE_CONTEXT pNodeContext, PIEM_ATTACHMENT_INFO_LIST pAttachmentInfo );
void DeleteAttachmentInfo( PIEM_NODE_CONTEXT pNodeContext );

//***************************************************************************
// CIMailStorage
//
// Class implementation of Internet Email RTVScan Storage Extension
// 

#define IMAIL_STORAGE_NAME          "InternetMail"
#define IMAIL_MAX_REALTIME_THREADS  3

#define IMAIL_STORAGE_INSTANCE      1       // Instance ID for first (and only) IDEF (logical storage volume).

class CIMailStorage  
{
public:
    CIMailStorage();
    virtual ~CIMailStorage();

    DWORD StorageInit( DWORD flags, PSTORAGEINFO * pInfo, HANDLE * phSEInstance, PSSFUNCTIONS pSSFuns );

private:
    //&? Bogus: static single instance of a CImailStorage
    // Don't get enough context data back from Storage Manager, so....

    static CIMailStorage *  s_pStorage;

    // Communication with email proxy filter

    CMailCommBuffer m_commBuffer;

    // Real-time options from registry

    MAIL_OPTIONS    m_options;

    TCHAR           m_szCurrentUser[ NAME_SIZE ];

    // Per-instance storage data

    STORAGEINFO     m_storageInfo;          // Top-level storage information returned in StorageInit().
    IDEF            m_instanceDefinition;   // One per logical volume. Only need one for IMail.
    STORAGESTATUS   m_storageStatus;        // Flags only.
    STORAGEDATA     m_storageData;          // Some high-level scan options.
    SFUNCTIONS      m_fnsStorage;           // Functions for walk scan, RT scan, lifecycle.
    NODEFUNCTIONS   m_fnsNode;              // SNODE functions.
    SFILETABLE      m_fnsNodeIo;            // SNODE IO table.

    PSSFUNCTIONS    m_pfnsStorageManager;   // Utility functions provided by Storage Manager.

    HANDLE          m_hStopRtsWatch;        // Signal for RTS Watch threads to exit.


    // Helper functions
    static DWORD formatMailString( LPTSTR pMsgBuf, size_t nNumMsgBufBytes, LPCTSTR pMsgFmt, PEVENTBLOCK eb, DWORD userParam );
    static DWORD doNotifications( PSNODE pSNode );
	static DWORD SetResponse( PSNODE pNode, IEM_RESPONSE eNew );

    // Storage Extension Functions (SFUNCTIONS implementations)

    static DWORD STORAGEAPI sf_FindFirstNode ( HANDLE hParent, DWORD dwInstance, const char *pszPath, PQNODE pQNode, PSNODE pSNode, HANDLE *phFind );
    static DWORD STORAGEAPI sf_FindNextNode ( HANDLE hFind, PQNODE pQNode, PSNODE pSNode );
    static DWORD STORAGEAPI sf_CreateSNode ( DWORD dwInstance, const char *pszPath, PSNODE pSNode );
    static DWORD STORAGEAPI sf_FindClose ( HANDLE hFind );
    static DWORD STORAGEAPI sf_DeInit ( HANDLE hStorage );
    static DWORD STORAGEAPI sf_BeginRTSWatch ( PROCESSRTSNODE ProcessRTSNode, PSNODE pSNode, void *pStorageManagerContext );
    static DWORD STORAGEAPI sf_StopRTSWatches ( );
    static DWORD STORAGEAPI sf_RefreshInstanceData ( );
    static DWORD STORAGEAPI sf_ReloadRTSConfig ( );
    static DWORD STORAGEAPI sf_ReInit ( DWORD flags );
    static DWORD STORAGEAPI sf_ProcessPacket ( WORD FunctionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address );
    static DWORD STORAGEAPI sf_FormatMessage ( char *out, size_t nNumOutBytes, const char *format, PEVENTBLOCK eb, DWORD userParam );
    static DWORD STORAGEAPI sf_ChangeUser ( const char *pszUser, HANDLE hAccessToken );

    // Storage Node IO Functions (SFILETABLE implementations)

    static DWORD STORAGEAPI io_open ( PSNODE pNode, DWORD dwAccess );
    static DWORD STORAGEAPI io_close ( DWORD hFile );
    static DWORD STORAGEAPI io_read ( DWORD hFile, void *pvData, DWORD dwLength );
    static DWORD STORAGEAPI io_write ( DWORD hFile, void *pvData, DWORD dwLength );
    static DWORD STORAGEAPI io_lseek ( DWORD hFile, long lOffset, DWORD dwOrigin );
    static DWORD STORAGEAPI io_access ( PSNODE pNode, int nAccess );
    static DWORD STORAGEAPI io_GetState ( PSNODE pNode, BYTE *pData );
    static DWORD STORAGEAPI io_SetState ( PSNODE pNode, BYTE *pData );
    static DWORD STORAGEAPI io_GetExtendedData ( PSNODE pNode, char *KeyName, BYTE *pData, DWORD dwLength );
    static DWORD STORAGEAPI io_SetExtendedData ( PSNODE pNode, char *KeyName, BYTE *pData, DWORD dwLength );
    static DWORD STORAGEAPI io_GetFullKey ( PSNODE pNode, char *Buffer, DWORD dwLength );

    // Storage Node Functions (NODEFUNCTIONS implementations)

    static DWORD STORAGEAPI nf_ReleaseSNode ( PSNODE pNode );
    static DWORD STORAGEAPI nf_NodeHasViruses ( PSNODE pNode, PEVENTBLOCK pEventBlock );
    static DWORD STORAGEAPI nf_RenameNode ( PSNODE pNode, char * newExt );
    static DWORD STORAGEAPI nf_RemoveNode ( PSNODE pNode );
};


#endif // !defined(AFX_IMAILSTORAGE_H__0402DDA6_2435_41B5_9D24_C56C63394C1A__INCLUDED_)
