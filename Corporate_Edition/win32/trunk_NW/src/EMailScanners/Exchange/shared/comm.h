// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef __COMM_H__
#define __COMM_H__

/*---------------------------------------------------------------------------*/

#define MAX_PROCESS_BLOCKS 16

enum MemFileObjectTimeouts
{
	WAIT_MemMutex     = 10000,
	WAIT_MemEvent     = 30000
};

/*---------------------------------------------------------------------------*/

#pragma pack(push, 4)

// ACTIONDATA structure contains all fields in a process block are written during the scan
// process in the SE that need to be copied back to the MEC.  Most of these relate to
// actions that the MEC needs to perform after a scan.


#define MAX_SENDER_STRING	256
#define MAX_SUBJECT_STRING	256
#define MAX_SELECTED_STRING 512
#define MAX_INFECT_INFO		512

// note that MAX_MESSAGE_STRING should be the longest of the
// consts above, and should NEVER be larger than MAX_MESSAGE_SIZE
#define MAX_MESSAGE_STRING  1024

// this doesn't need to be bigger than MAX_MESSAGE_STRING since
// the array that uses it is only used to output to a message!
#define MAX_RECIPIENTS_LEN	MAX_MESSAGE_STRING

typedef struct
{
	enum ActionCode { None = 0, Remove, Replace, LeaveAlone } code;

	DWORD
		bInsertWarning,
		bChangeSubject,
		bNotifySender,
		bNotifySelected;

	TCHAR
		szWarningSubject[MAX_SUBJECT_STRING],
		szWarningMessage[MAX_MESSAGE_STRING],
		szWarningInfectionInformation[MAX_INFECT_INFO],
		szSenderSubject[MAX_SUBJECT_STRING],
		szSenderMessage[MAX_MESSAGE_STRING],
		szSenderInfectionInformation[MAX_INFECT_INFO],
		szSelectedRecips[MAX_SELECTED_STRING],
		szSelectedSubject[MAX_SUBJECT_STRING],
		szSelectedMessage[MAX_MESSAGE_STRING],
		szSelectedInfectionInformation[MAX_INFECT_INFO];

	void Initialize(void)
	{
		code = None;
		bInsertWarning = bChangeSubject = bNotifySender = bNotifySelected = false;
		*szWarningSubject =
		*szWarningMessage =
		*szWarningInfectionInformation = 
		*szSenderSubject =
		*szSenderMessage =
		*szSenderInfectionInformation =
		*szSelectedRecips =
		*szSelectedSubject =
		*szSelectedMessage =
		*szSelectedInfectionInformation = 0;
	}
} ACTIONDATA, *PACTIONDATA;

/*---------------------------------------------------------------------------*/

// INPUTDATA structure contains the fields in the process block that relate
// to the attachment to be scanned.

typedef struct
{
	TCHAR
		szTempPath[IMAX_PATH],				// full path to temporary file containing attachment
		szAttachmentName[MAX_PATH],			// Attachment name and extension (no path)
		szExtension[EXT_SIZE],				// Attachment extension
		szSender[MAX_SENDER_STRING],		// Message sender
		szSubject[MAX_SUBJECT_STRING],		// Message subject
		szUserName[MAX_SENDER_STRING],		// Messaging User			
		szRecipients[MAX_RECIPIENTS_LEN];	// Recipients

	void Initialize(void)
	{
		*szTempPath =
		*szAttachmentName =
		*szExtension =
		*szSender =
		*szSubject =
		*szUserName =
		*szRecipients = 0;
	}

} INPUTDATA, *PINPUTDATA;

/*---------------------------------------------------------------------------*/

// MEC_MEMFILE and PROCESSBLOCK structures are the shared memory file layout
// used to communicated between the MEC storage extension and the MAPI Hook
// Provider personalities.

typedef struct
{
	enum ProcessState { Free = 0, Aquired, Scan, Processing, Complete, Request } volatile state;

	DWORD volatile instanceID;	// Set when block is aquired.  If changed detected during processing, abort processing.

	ACTIONDATA action;			// SE -> MEC data

	INPUTDATA input;			// MEC -> SE data

	void Initialize(void)
	{
		state = Free;
		instanceID = 0;
		action.Initialize();
		input.Initialize();
	}

} PROCESSBLOCK, *PPROCESSBLOCK;

/*---------------------------------------------------------------------------*/

// EXTINFO contains information about which files to scan.  This information
// is contained in the header of the shared memory file and copied from there
// as needed.

typedef struct
{
	DWORD volatile fileType;					// Indicates whether all files or only those with selected .exts should be scanned
	TCHAR szExts[EXT_LIST_SIZE];				// .exts to be scanned: COM,DOC,DOT,EXT  (no dots)
	TCHAR szSkipExts[EXT_LIST_SIZE];			// .exts to be skipped
	DWORD volatile bScanZipFiles;
	TCHAR szZIPExts[EXT_LIST_SIZE];				// Archive extensions: ZIP,ARC (no dots)

	void Initialize(void)
	{
		fileType = 0;
		bScanZipFiles = true;
		*szExts = *szSkipExts = *szZIPExts = 0;
	}
} EXTINFO, *PEXTINFO;

/*---------------------------------------------------------------------------*/

typedef struct
{
	DWORD signature;							// MEC_Signature -- set when MemFile is created.  Used for sanity checks

	DWORD volatile cInboundMessages;			// Inbound messages received MEC
	DWORD volatile cInboundMessagesProcessed;	// Inbound messages processed by MEC
	DWORD volatile cInboundAttachments;			// Inbound attachments received MEC
	DWORD volatile cInboundAttachmentsProcessed;// Inbound attachments processed by MEC
	DWORD volatile cInboundAttachmentsTimeout;	// Inbound attachments skipped by MEC

	DWORD volatile cSEAttacmentsProcessed;		// Real time attachments processed by SE

	DWORD volatile cInstanceID;					// Instance counter for process blocks.  Initial value of 1, this value is
												// incremented by two in NextProcessBlockInstanceID() each time a process block
												// is aquired.  This value should never be zero.
	long volatile cSERTWatch;					// Count of SE real time watch threads
	long volatile cWalkerThreads;				// Count of realtime walker threads
	long volatile cStorages;					// Count of storages using the walker agent
	BOOL volatile bRTEnabled;					// Bool indicating whether realtime is enabled
	BOOL volatile bMarkMessages;				// Bool indicating whether we should mark a message as having been read
	BOOL volatile bDebug;						// Bool indicating whether debug is on

	EXTINFO extInfo;

	PROCESSBLOCK processBlock[MAX_PROCESS_BLOCKS];
} MEC_MEMFILE, *PMEC_MEMFILE;

#pragma pack(pop)

/*---------------------------------------------------------------------------*/

extern HANDLE
	g_hMemMutex,
	g_hRTSemaphore,
	g_hPBEvent[MAX_PROCESS_BLOCKS];

extern PMEC_MEMFILE
	g_pMemFile;

extern TCHAR
	NAME_MemMutex[],
	NAME_MemFile[],
	NAME_MemSemaphore[],
	NAME_MemEvent[];

/*---------------------------------------------------------------------------*/

void CloseCommunicationObjects(void);
DWORD CreateCommunicationObjects(void);
PPROCESSBLOCK FindProcessBlock(PROCESSBLOCK::ProcessState desiredState, int *pIndex = 0);
PPROCESSBLOCK AquireProcessBlock(int *pIndex);
PPROCESSBLOCK RequestProcessBlock(int *pIndex, PPROCESSBLOCK pCopy);
PPROCESSBLOCK ScanProcessBlock(int *pIndex, PPROCESSBLOCK pCopy);

#endif // __COMM_H__