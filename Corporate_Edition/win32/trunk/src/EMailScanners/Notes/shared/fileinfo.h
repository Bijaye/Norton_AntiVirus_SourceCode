// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _NOTESFILEINFO_H_
#define _NOTESFILEINFO_H_

#pragma warning( disable: 4100 4115 4127 4201 4706 )	// Needed for /W4 and <windows.h>

//4100: unreferenced formal parameter
//4115:	named type definition in parentheses
//4127: conditional expression is constant
//4201: nameless unions
//4706	assignment within conditional expression

//4214: non 'int' bit fields
//4514: unreferenced inline functions

// OS header files

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <tchar.h>

// reset since windows.h resets some of these...
#pragma warning( disable: 4100 4115 4127 4201 4706 )	// Needed for /W4 and <windows.h>

// Notes header files

#include <global.h>
#include <pool.h>
#include <nsfdata.h>

// NavCorp header files

//#include "vpcommon.h"


/*---------------------------------------------------------------------------*/

#define MAX_SENDER				256
#define MAX_SUBJECT				256
#define MAX_SELECTED			512
#define MAX_INFECT_INFO			512
#define MAX_RECIPIENTS			512

// TODO: Defining this here is an ugly hack to work-around a build problem
//       with "vpcommon.h" and <xlocale>
#define EXT_LIST_SIZE 4096

// note that MAX_MESSAGE should be the longest of the
// consts above, and should NEVER be larger than MAX_MESSAGE_SIZE
#define MAX_MESSAGE				1024

#define MAX_NOTES_TEXT			((64*1024)-1)

#define MUTEX_TIMEOUT			2000

#define BUFFER_FREE				0
#define BUFFER_IN_USE			1
#define BUFFER_OWNED_BY_SCAN	2
#define	BUFFER_SCANNED			3

#define EXTENSION_MAP_FILE	_T("Global\\LNExtMapFile")
#define LNEXT_SEMA			_T("Global\\LANDeskVirusProtectLNEXTSema")
#define LNEXT_MUTEX			_T("Global\\LANDeskVirusProtectLNEXTMutex")
#define LNEXT_EVENT			_T("Global\\LANDeskVirusProtectLNEXTEvent%d")

#define FILE_INFO_ENTRIES		8

/*---------------------------------------------------------------------------*/

//	The FILE_INFO structure is used as the primary means to exchange file 
//	attachment information from the notes hook context to the storage context.
//	It also contains strings of formatted text which are passed from the storage
//	to the hook in the case of an infection.
        
typedef struct _FILE_INFO
{
	DWORD		bufferIndex;			// index of the buffer entry
	DWORD		state;					// state of the buffer entry
	BOOL		bExtracted;				// Flag to indicate if extraction has occured
    BOOL        bIsContainer;           // Flag to indicate if the attachment is a container file
	DWORD 		action;					// action taken by scan engine
	NOTEHANDLE	hNote;					// handle of the note being processed
	BLOCKID		blockID;				// attachment blockID
	WORD		compressionType;		// attachment compression type
	TCHAR		szLMBCSFilePath[MAX_PATH];	// LMBCS path (and filename) of the extracted file
	TCHAR		szFilePath[MAX_PATH];		// path (and filename) of the extracted file
	TCHAR		szOEMFileName[MAX_PATH];	// Notes file name
	TCHAR		szAnsiFileName[MAX_PATH];	// user displayable file name
	TCHAR		szSubject[MAX_SUBJECT];		// Original message subject
	TCHAR		szFrom[MAX_SENDER];			// Message sender
	TCHAR		szTo[MAX_RECIPIENTS];		// Recipient list
	DWORD		dwPID;					// ID of Notes process. Used for user info in storage extension.
	char		*szUserName;			//WARNING! Notes user name. Only available within Notes Hook.

	struct		_FILE_INFO *	pNext;  // used to create an infection list

	// Notification flags and buffers
	DWORD	bInsertWarning;			// Insert Warning message into infected message
	DWORD	bChangeMessageSubject;	// Change subject of infected message
	DWORD	bEmailSender;			// Send infection message to sender
	DWORD	bEmailSelected;			// Send infection message to selected list

	TCHAR	MessageSubject[MAX_SUBJECT];			// Body of formatted message subject
	TCHAR	MessageWarning[MAX_MESSAGE];			// Body of formatted message warning
	TCHAR	MessageInfectionInfo[MAX_INFECT_INFO];	// Body of formatted infection info
	
	TCHAR	SenderSubject[MAX_SUBJECT];				// Body of formatted message subject
	TCHAR	SenderMessageBody[MAX_MESSAGE];			// Body of formatted message warning
	TCHAR	SenderInfectionInfo[MAX_INFECT_INFO];	// Body of formatted infection info

	TCHAR	SelectedRecipients[MAX_SELECTED];		// List of selected email recipients
	TCHAR	SelectedSubject[MAX_SUBJECT];			// Body of formatted subject
	TCHAR	SelectedMessageBody[MAX_MESSAGE];		// Body of formatted message
	TCHAR	SelectedInfectionInfo[MAX_INFECT_INFO];	// Body of formatted infection info

	char	szLMBCS[MAX_NOTES_TEXT];				// used to translate to/from the Lotus char set

	TCHAR	ExtractDir[MAX_PATH];

} FILE_INFO, *PFILE_INFO;


/*---------------------------------------------------------------------------*/
// REG_INFO structure contains copies from the registry which
// are needed in the mail context for realtime scan.
	
typedef struct _REG_INFO
{
	DWORD	bOnOff;					//	Signifies whether realtime scanning is enabled
	DWORD	bReads;					//	Signifies whether reads are enabled
	DWORD	bWrites;				//	Signifies whether writes are enables
	DWORD	bZipFile;				//	Controls scan of pkzip and lha files
	DWORD	ScanFilesType;			//	0=Scan all, 1=by ext // removed: 2=by type
	
	TCHAR	Exts[EXT_LIST_SIZE];	// valid extension filenames to be scanned
	TCHAR	SkipExts[EXT_LIST_SIZE];// Extensions to be skipped
	TCHAR	ZipExts[EXT_LIST_SIZE];	// valid extension filenames to be scanned

	// Notification flags and buffers
	
	DWORD	bInsertWarning;			// Insert Warning message into infected message
	DWORD	bChangeMessageSubject;	// Change subject of infected message
	DWORD	bEmailSender;			// Send infection message to sender
	DWORD	bEmailSelected;			// Send infection message to selected list
	
	TCHAR	MessageSubject[MAX_SUBJECT];			// Body of unformatted message subject
	TCHAR	MessageWarning[MAX_MESSAGE];			// Body of unformatted message warning
	TCHAR	MessageInfectionInfo[MAX_INFECT_INFO];	// Body of unformatted infection info
	
	TCHAR	SenderSubject[MAX_SUBJECT];				// Body of unformatted message subject
	TCHAR	SenderMessageBody[MAX_MESSAGE];			// Body of unformatted message warning
	TCHAR	SenderInfectionInfo[MAX_INFECT_INFO];	// Body of unformatted infection info

	TCHAR	SelectedRecipients[MAX_SELECTED];		// List of selected email recipients
	TCHAR	SelectedSubject[MAX_SUBJECT];			// Body of unformatted subject
	TCHAR	SelectedMessageBody[MAX_MESSAGE];		// Body of unformatted message
	TCHAR	SelectedInfectionInfo[MAX_INFECT_INFO];	// Body of unformatted infection info

	DWORD	bDebug;

} REG_INFO, *PREG_INFO;


/*---------------------------------------------------------------------------*/

//	The FILE_BUFFER structure is the main structure for communication between
//	the hook context and the storage extension.  It composes the body of the
//	shared memory mapped file for IPC.

typedef struct _FILE_BUFFER
{
	unsigned int producerIndex;				// Hook index counter.
	unsigned int consumerIndex;				// Storage extension index counter.
	long		 realTimeThreads;			// Count of running realtime threads.
	FILE_INFO	 buf[FILE_INFO_ENTRIES];	// An array of FILE_INFO blocks for scanning.
	REG_INFO	 regInfo;					// A copy of registry configuration information.
} FILE_BUFFER;


/*---------------------------------------------------------------------------*/
//
// Global data
//

extern HANDLE		g_hBufferPoolSema;
extern HANDLE		g_hBufferPoolMutex;
extern HANDLE		*g_phFileScannedEvents;
extern FILE_BUFFER	*g_pFTBSBuf;


/*---------------------------------------------------------------------------*/
//
// Global functions
//

void ReleaseFileInfoBlock(PFILE_INFO pFileInfo);
PFILE_INFO GetWaitingFileInfoBlock();
PFILE_INFO GetAvailableFileInfoBlock();
PFILE_INFO GetFileInfoBlockFromContext(void * pContext);

#endif	// _NOTESFILEINFO_H_

/*--- end of header ---*/
