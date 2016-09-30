// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "notesext.h"
#include "fileinfo.h"
#include "initipc.h"
#include "noteswatch.h"
#include "SymSaferRegistry.h"
#include "vpstrutils.h"

#include "FIOOpen.c"

// scancontext.c: routines for the scan context for the notes extension 

BOOL g_bScanContextRegistered = FALSE;


// Local functions used in function tables

DWORD NSE_Open(PSNODE pSNode, DWORD access);
DWORD NSE_Close(DWORD handle);
DWORD NSE_Read(DWORD handle, void *data, DWORD length);
DWORD NSE_Write(DWORD handle, void *data, DWORD length);
DWORD NSE_Lseek(DWORD handle, long offset, DWORD fromwhere);
DWORD NSE_Access(PSNODE pSNode, int access);
DWORD NSE_GetState(SNODE * pContext, BYTE *pData);
DWORD NSE_SetState(SNODE *context,BYTE *Data);
DWORD NSE_GetExtendedData(struct _SNODE *node,char *KeyName,BYTE *Data,DWORD len);
DWORD NSE_SetExtendedData(struct _SNODE *node,char *KeyName,BYTE *Data,DWORD len);
DWORD NSE_GetFullKey(struct _SNODE *node, char *buffer, DWORD len);

DWORD NSE_ReleaseSNode(PSNODE pSNode);
DWORD NSE_RenameNode(PSNODE pSNode, TCHAR *newExt);
DWORD NSE_RemoveNode(PSNODE pSNode);
DWORD NSE_RealTimeNodeHasVirus(PSNODE pSNode, PEVENTBLOCK pEB);

DWORD  NSE_FindFirstNode(HANDLE hParent, DWORD instanceID, const char *path, PQNODE pQNode, PSNODE pSNode, HANDLE* handle);
DWORD  NSE_FindNextNode(HANDLE handle, PQNODE  pQNode, PSNODE pSNode);
DWORD  NSE_CreateSNode(DWORD instanceID, const char *path, PSNODE pSNode);
DWORD  NSE_FindClose(HANDLE handle);
DWORD  NSE_DeInit(HANDLE handle);
DWORD  NSE_BeginRTSWatch(PROCESSRTSNODE ProcessRTSNode, PSNODE pSNode, void * pUserContext);
DWORD  NSE_StopRTSWatches(void);
DWORD  NSE_RefreshInstanceData(void);
DWORD  NSE_ReloadRTSConfig(void);
DWORD  NSE_ReInit(DWORD flags);
DWORD  NSE_ProcessPacket(WORD FunctionCode,BYTE *sendBuffer, DWORD sendBufferSize,BYTE *replyBuffer, DWORD *replyBufferSize,CBA_Addr *address);
DWORD  NSE_FormatMessage(char *out, size_t nNumOutBytes, const char *format, PEVENTBLOCK eb, DWORD userParam);
DWORD  NSE_ChangeUser(const char *NewUserName,HANDLE hAccessToken);

DWORD  FormatMailString(LPTSTR pMsgBuf, size_t nNumMsgBufBytes, LPCTSTR pMsgFmt, PFILE_INFO pFileInfo);

/*---------------------------------------------------------------------------*/

PSSFUNCTIONS pSSFunctions;

// Functions used by storage manager to process files

NODEFUNCTIONS RealTimeNodeFunctions = 
{
	NSE_ReleaseSNode,
	NSE_RealTimeNodeHasVirus,
	NSE_RenameNode,
	NSE_RemoveNode
};

// Functions used by storage manager to access files

SFILETABLE fileTable = 
{
		NSE_Open,
		NSE_Close,
		NSE_Read,
		NSE_Write,
		NSE_Lseek,
		NSE_Access,
		NSE_GetState,
		NSE_SetState,
		NSE_GetExtendedData,
		NSE_SetExtendedData,
		NSE_GetFullKey
};

// Functions used by storage manager to process scans

SFUNCTIONS functions = 
{
	NSE_FindFirstNode,
	NSE_FindNextNode,
	NSE_CreateSNode,
	NSE_FindClose,
	NSE_DeInit,
	NSE_BeginRTSWatch,
	NSE_StopRTSWatches,
	NSE_RefreshInstanceData,
	NSE_ReloadRTSConfig,
	NSE_ReInit,
	NSE_ProcessPacket,
	NSE_FormatMessage,
	NSE_ChangeUser
};

IDEF instanceBlocks[] = 
{
	{
		NSE_SIGNATURE,			//	DWORD instanceID
		IT_MAIL | IT_CAN_RTS,	//	DWORD type
		"Lotus Notes",			// 	char DisplayName[32];
		"",						//	char VolumeName[32];
	},
	0 
};

STORAGEDATA storageData = {0,0,0,0,0,"","","",0,0};
	
STORAGEINFO storageInfo = 
{
	sizeof(STORAGEINFO),	//	DWORD Size;
	0x1,					//	DWORD Version;
	1,						//	DWORD InstanceCount
	instanceBlocks,			//	PIDEF InstanceBlocks
	&functions,				//	PSFUNCTIONS Functions;
	NULL,					//  HKEY hRTSConfigKey
	{ 0 },
	"LotusNotes",			//	Name
	"nlnvp.inf",			//	InfFileName
	IT_MAIL | IT_CAN_RTS,	//	Type
	&storageData,
	MAX_REALTIME_THREADS
};		


BOOL IsWinNT( void )
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    return (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
}

/*===========================================================================*/
// SFILETABLE Implementation
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

//	NSE_Open is called by the storage manager to open the file specified
//	by context.  Open also extracts the file to disk to prepare for scanning.
//	
//	Parameters:
//		pSNode - [in] a pointer to a previously defined SNODE.
//		access - [in] a DWORD specifying the desired access.
//
//	Returns:
//			A valid file handle, otherwise INVALID_HANDLE_VALUE;

DWORD NSE_Open(PSNODE pSNode, DWORD access)
{
	PFILE_INFO	pFileInfo;
	DWORD		result = (DWORD)INVALID_HANDLE_VALUE;
	
	// Check the context index and see if it matches the 
	// bufferIndex stored in the file info.

	if(pFileInfo = GetFileInfoBlockFromContext(pSNode->Context))
	{
		ASSERT0(pFileInfo->bExtracted, "NSE_Open: File not previously extracted!")

		result = FIOOpen(pFileInfo->szFilePath, access);
	}
	
	return result;
}

/*---------------------------------------------------------------------------*/

//	NSE_Close is called to close a file previously opened by a call
//	to NSE_Open
//
//	Parameters:
//		handle - [in] a handle to an opened file.
//
//	Returns:
//		0 for success, otherwise -1 for error.		
//	

DWORD NSE_Close(DWORD handle)
{
	return (DWORD)_close(handle);
}

/*---------------------------------------------------------------------------*/

//	NSE_Read reads the number of bytes specified by length from the file
//	specified by handle into the buffer pointed to by data.  If fewer 
//	bytes than length are available, the remaining bytes are copied to the
//	buffer.
//	
//	Parameters:
//		handle	- [in] a valid file handle.
//		data	- [out] a pointer to a buffer where data will be written.
//		length	- [in] The number of bytes to be written to the buffer.
//
//	Returns:
//		The number of bytes read, 0 if the end of the file is reached,
//		or -1 if an error occurs.

DWORD NSE_Read(DWORD handle, void *data, DWORD length)
{
	return (DWORD)_read((int)handle, data, length);
}

/*---------------------------------------------------------------------------*/

//	NSE_Write writes the number of bytes specified by length
//	from the buffer pointed to by data into the file specified by
//	handle.
//	
//	Parameters:
//		handle	- [in] A valid file handle.
//		data	- [in] A pointer to a buffer from which data will be read.
//		length	- [in] The number of bytes to be written to the file.
//
//	Returns:
//		The number of bytes written, 0 if the end of the file is reached,
//		or -1 if an error occurs.

DWORD NSE_Write(DWORD handle, void *data, DWORD length)
{
	return (length == 0) ? 
		(DWORD)_chsize(handle, _lseek(handle, 0, SEEK_CUR)) 
		: (DWORD)_write( (int)handle, data, length);
}

/*---------------------------------------------------------------------------*/

//	NSE_Lseek moves the file cursor to the position specified
//	by fromwhere and offset.
//	
//	Parameters:
//		handle		- [in] A valid file handle.
//		offset		- [in] The number of bytes to move the cursor
//		fromwhere	- [in] The initial cursor position.
//
//	Returns:
//		The final position of the cursor from the beginning of the
//		file, or -1 for error.

DWORD NSE_Lseek(DWORD handle, long offset, DWORD fromwhere)
{
	return (DWORD)_lseek((int)handle, offset, (int)fromwhere);
}

/*---------------------------------------------------------------------------*/

//	NSE_Access verifies the file access rights specified in access for
//	the file specified by pSNode.
//	
//	Parameters:
//		pSNode	- [in] A file handle passed back to the server
//		access	- [in] A int specifying desired access permissions.
//
//	Returns:
//		0 for success, or -1 if the access rights are not allowed.

DWORD NSE_Access(PSNODE pSNode, int access)
{
	PFILE_INFO pFileInfo;
	
	if(pFileInfo = GetFileInfoBlockFromContext(pSNode->Context))
	{
		ASSERT0(pFileInfo->bExtracted, "NSE_Access: File not previously extracted!")

		_access(pFileInfo->szFilePath, access);
	}

	return 0;
}

/*---------------------------------------------------------------------------*/

//	NSE_GetState not implemented.
DWORD NSE_GetState(SNODE * pContext, BYTE *pData)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

// NSE_SetState not implemented
DWORD NSE_SetState(SNODE *context, BYTE *Data)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

// NSE_GetExtendedData not implemented
DWORD NSE_GetExtendedData(struct _SNODE *node, char *KeyName, BYTE *Data, DWORD len)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

// NSE_SetExtendedData not implemented
DWORD NSE_SetExtendedData(struct _SNODE *node,char *KeyName,BYTE *Data,DWORD len)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

//	NSE_GetFullKey should return the context for a node
//	scanned in realtime.  Since no context exists for
//	notes hooked in notes realtime we return an error.

DWORD NSE_GetFullKey(struct _SNODE *node, char *buffer, DWORD len)
{
	return ERROR_GENERAL;
}

/*===========================================================================*/
// NODEFUNCTIONS implementation
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

//	NSE_ReleaseSNode not implemented.

DWORD  NSE_ReleaseSNode(PSNODE pSNode)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

// NSE_RenameNode not implemented
DWORD  NSE_RenameNode(PSNODE pSNode, TCHAR *newExt)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

// NSE_RemoveNode not implemented
DWORD  NSE_RemoveNode(PSNODE pSNode)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

//	NSE_RealTimeNodeHasVirus is called by the Storage Manager when
//	a virus is detected in a scanned node.  The function saves
//	configuration information so that the mail context can do the
//	configured notification.
// 
//	Parameters:
//		pSNode	[in] A pointer to an SNODE structure
//		pEB		[in] A pointer to an EVENTBLOCK which contains virus info
//
//	Returns:
//		0 for success, otherwise ERROR_OUTOFMEMORY or WAIT_TIMEOUT.

DWORD  NSE_RealTimeNodeHasVirus(PSNODE pSNode, PEVENTBLOCK pEB)
{
	PFILE_INFO	pFileInfo;
	PREG_INFO	pRegInfo;
	TCHAR		textBuf[MAX_MESSAGE];
	DWORD		result = ERROR_SUCCESS;

	if (!(pRegInfo = (PREG_INFO)malloc(sizeof(REG_INFO))))
		return ERROR_OUTOFMEMORY;

	memset(pRegInfo, 0 , sizeof(REG_INFO));

	// Get the Buffer pool mutex and copy the new information
	if(WaitForSingleObject(g_hBufferPoolMutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0)
	{
		*pRegInfo = g_pFTBSBuf->regInfo; 
		ReleaseMutex(g_hBufferPoolMutex);
	}
	else
	{
		free(pRegInfo);
		return WAIT_TIMEOUT;
	}
	
	if (pFileInfo = GetFileInfoBlockFromContext(pSNode->Context))
	{
		if (pEB->pdfi->bIsCompressed)
		{
            pFileInfo->bIsContainer = TRUE;
			// if this isn't the top level, ignore the realaction.....
			if (pEB->pdfi->dwDepthOfFileToGet == 0)
			{
				if (pEB->RealAction == AC_MOVE || pEB->RealAction == AC_DEL || pEB->RealAction == AC_CLEAN)
				    pFileInfo->action = pEB->RealAction;
				else
				{
					// if we cleaned or deleted something, we need to replace the container file
					if (pEB->pdfi->dwCleaned > 0 ||  pEB->pdfi->dwDeleted > 0)
						pFileInfo->action = AC_DEL;
					else
						pFileInfo->action = pEB->RealAction;
				}
			}
		}
        else
		{
			// if this isn't a compressed file, use the realaction
			// but don't override any previous action......

			// this can happen if the file was first cleaned of an infection,
			// but then the ccEraser engine finds adware or spyware

			// if the file was cleaned, it can still be deleted or quarantined
			if (pFileInfo->action == AC_CLEAN)
			{
				if (pEB->RealAction == AC_MOVE || pEB->RealAction == AC_DEL)
					pFileInfo->action = pEB->RealAction;
			}
			// if the file has been deleted or quarantined, don't update the action
			else if (pFileInfo->action != AC_MOVE && pFileInfo->action != AC_DEL)
			{
			    pFileInfo->action = pEB->RealAction;
			}
		}

		// Check notifications to see what text fields need to be 
		// formatted and sent over.

		pFileInfo->bInsertWarning = pRegInfo->bInsertWarning;
		pFileInfo->bChangeMessageSubject = pRegInfo->bChangeMessageSubject;
		pFileInfo->bEmailSender = pRegInfo->bEmailSender;
		pFileInfo->bEmailSelected = pRegInfo->bEmailSelected;

		if (pRegInfo->bInsertWarning)
		{
			if(pRegInfo->bChangeMessageSubject)
			{
				pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->MessageSubject, pEB, 0);
                FormatMailString(pFileInfo->MessageSubject, sizeof (pFileInfo->MessageSubject), textBuf, pFileInfo);
			}
			
			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->MessageWarning, pEB, 0);
            FormatMailString(pFileInfo->MessageWarning, sizeof (pFileInfo->MessageWarning), textBuf, pFileInfo);
		    
            if (pFileInfo->bIsContainer)
            {
                if (strlen(pEB->VirusName) <= 0)
                {
                    static char szMultiVirus[255];
               		LoadString(g_hResource, IDS_MULTIPLE_VIRUS_FOUND, szMultiVirus, 255);
                    pEB->VirusName = szMultiVirus;
                }
            }

			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->MessageInfectionInfo, pEB, 0);
			FormatMailString(pFileInfo->MessageInfectionInfo, sizeof (pFileInfo->MessageInfectionInfo), textBuf, pFileInfo);

		}

		if(pRegInfo->bEmailSender)
		{
			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->SenderSubject, pEB, 0);
			FormatMailString(pFileInfo->SenderSubject, sizeof (pFileInfo->SenderSubject), textBuf, pFileInfo);
					
			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->SenderMessageBody, pEB, 0);
			FormatMailString(pFileInfo->SenderMessageBody, sizeof (pFileInfo->SenderMessageBody), textBuf, pFileInfo);
		
			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->SenderInfectionInfo, pEB, 0);
			FormatMailString(pFileInfo->SenderInfectionInfo, sizeof (pFileInfo->SenderInfectionInfo), textBuf, pFileInfo);
		}

		if(pRegInfo->bEmailSelected)
		{
			_tcscpy(pFileInfo->SelectedRecipients, pRegInfo->SelectedRecipients);

			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->SelectedSubject, pEB, 0);
			FormatMailString(pFileInfo->SelectedSubject, sizeof (pFileInfo->SelectedSubject), textBuf, pFileInfo);
					
			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->SelectedMessageBody, pEB, 0);
			FormatMailString(pFileInfo->SelectedMessageBody, sizeof (pFileInfo->SelectedMessageBody), textBuf, pFileInfo);
		
			pSSFunctions->FormatMessage(textBuf, sizeof (textBuf), pRegInfo->SelectedInfectionInfo, pEB, 0);
			FormatMailString(pFileInfo->SelectedInfectionInfo, sizeof (pFileInfo->SelectedInfectionInfo), textBuf, pFileInfo);
		}
	}
	
	free(pRegInfo);

	return result;
}


/*===========================================================================*/
// SFUNCTIONS Implementation
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

//	NSE_BeginRTSWatch is called by the Storage manager to begin realtime scanning.
//	If the number of threads that can be supported has been reached, BeginRTSWatch 
//	returns.  Otherwise BeginRTSWatch waits for scannable files and passes them
//	to the storage manager for scanning through the ProcessRTSNode function.  When 
//	StopRTSWatches is called, an event is set which causes all instances of
//	BeginRTSWatch to return.
//	
//	Parameters:
//		ProcessRTSNode	[in] A pointer to a PROCESSRTSNODE function to be used to 
//						signal the storage manager that a file should be scanned.
//		pSNode		
//
//	Returns:
//		0 for success.

DWORD  NSE_BeginRTSWatch(PROCESSRTSNODE ProcessRTSNode, PSNODE pSNode, void * pUserContext)
{
	HANDLE		hEvents[2]; 
	DWORD		signal; 
	PFILE_INFO	pFileInfo;
	DWORD		result = ERROR_SUCCESS;
	TCHAR		pszFileName[MAX_PATH];

	hEvents[0] = g_hBufferPoolSema;
	hEvents[1] = g_hShutdown;

	if(g_pFTBSBuf->realTimeThreads >= MAX_REALTIME_THREADS)
		return ERROR_SUCCESS;

	// need to protect this variable when inc/dec it
	InterlockedIncrement(&(g_pFTBSBuf->realTimeThreads));

	while(1)
		{
		//	Wait to for files on the to be scanned queue.
		//	or for a signal to terminate.
		signal = WaitForMultipleObjects(2, hEvents, FALSE, INFINITE); 
		if(signal == WAIT_OBJECT_0)
			{
			pFileInfo = GetWaitingFileInfoBlock();
			if(pFileInfo)
				{
				TCHAR szFileExt[_MAX_EXT];

				//	Initialize the node structure 
                memset( pSNode, 0, sizeof( *pSNode ) );
				pSNode->IO = &fileTable;
				pSNode->Context = (void *)pFileInfo->bufferIndex;
				pSNode->Flags = N_RTSNODE | N_MAILNODE;
				pSNode->InternalPath = pFileInfo->szFilePath;			
				pSNode->UID = pFileInfo->bufferIndex;
				pSNode->Functions = &RealTimeNodeFunctions;
				pSNode->InstanceID = NSE_SIGNATURE;
				pSNode->Operations = FA_READ | FA_WRITE | FA_FILE_NEEDS_SCAN | FA_AFTER_OPEN;
                pSNode->dwPID = pFileInfo->dwPID;

                if ( FAILED( GetProcessOwner( pFileInfo->dwPID, &pSNode->Sid, sizeof(pSNode->Sid) ) ) )
                {
                    // On failure, just create fake SID based on user name Storage Manager last gave us,
                    // plus name of computer we're on.
    				GetFakeSID(&pSNode->Sid);
                }

				_tcscpy(pSNode->Description, pFileInfo->szAnsiFileName);

				GetExtension(pFileInfo->szAnsiFileName, szFileExt);
				_tcsncpy(pSNode->Ext, szFileExt, EXT_SIZE-1);
				pSNode->Ext[EXT_SIZE-1]=0;

				_tcscpy(pSNode->Name, pFileInfo->szAnsiFileName);

				// copy the filename in case the Notes hook times out
				_tcscpy(pszFileName, pFileInfo->szFilePath);

				ProcessRTSNode(pSNode, pUserContext);

				if(WaitForSingleObject(g_hBufferPoolMutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0)
					{
					//	Signal the scanned event
					if(pSNode->UID == pFileInfo->bufferIndex)
						{
						pFileInfo->state = BUFFER_SCANNED;
						ReleaseMutex(g_hBufferPoolMutex);
						SetEvent(g_phFileScannedEvents[pSNode->UID % FILE_INFO_ENTRIES]);
						}
					else
						{
						// A timeout has occured in the mail context.
						
						// Release the Mutex
						ReleaseMutex(g_hBufferPoolMutex);

						// no need to clean up the fileinfo block, but
						// we do need to delete the extracted file
						DeleteFile(pszFileName);
						}
					}
				else
					{
					// we timed out on the mutex, the hook has already timed out
					// no need to clean up the fileinfo block, but
					// we do need to delete the extracted file
					DeleteFile(pszFileName);
					}
				}
			}
		else	// we're shutting down!
			{
			break;
			}
		}
			
	// need to protect this variable when inc/dec it
	InterlockedDecrement(&(g_pFTBSBuf->realTimeThreads));
	
	return result;
}

/*---------------------------------------------------------------------------*/

//	NSE_StopRTSWatches is called by the storage manager to terminate all
//	real time threads (instances of NSE_BeginRTSWatch).  The function waits
//	for all threads to terminate and abandons the wait after 10 seconds.
//
//	Returns:
//		0 for success.

DWORD NSE_StopRTSWatches(void)
{
	SetEvent(g_hShutdown);
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

// NSE_RefreshInstanceData not implemented.

DWORD  NSE_RefreshInstanceData(void)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

//	NSE_ReloadRTSConfig is called by the storage manager to indicate a 
//	change in the storage extension configuration.  All configuration
//	information contained in the registry is loaded into the shared 
//	memory mapped file so that all Notes hooks and storage extensions 
//	can access it.
//	
//	Returns:
//		ERROR_SUCCESS, else ERROR_OUTOFMEMORY for allocation errors or
//		WAIT_TIMEOUT if the buffer pool mutex cannot be obtained.

DWORD NSE_ReloadRTSConfig(void)
{
	DWORD			result = ERROR_SUCCESS;
	PREG_INFO		pRegInfo;
	HKEY			hStorageKey;
	
	if (!(pRegInfo = (PREG_INFO)malloc(sizeof(REG_INFO))))
		return ERROR_OUTOFMEMORY;

	memset(pRegInfo, 0 , sizeof(REG_INFO));
	
	pRegInfo->bOnOff = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_RTSScanOnOff, 1);
	pRegInfo->bReads = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_ScanOnAccess, 1);
	pRegInfo->bWrites = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_ScanOnModify, 0);
	pRegInfo->bZipFile = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_ScanZipFile, 1);
	pRegInfo->ScanFilesType = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_FileTypes, 0);
	pRegInfo->bInsertWarning = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_InsertWarning, 1);
	pRegInfo->bChangeMessageSubject = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_ChangeSubject, 1);
	pRegInfo->bEmailSender = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_EmailSender, 1);
	pRegInfo->bEmailSelected = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_EmailSelected, 0);
	
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_Extensions, pRegInfo->Exts, EXT_LIST_SIZE, "");

	// if we're scanning by extension and if there are no extensions, switch to scan-all-files
	if ((pRegInfo->ScanFilesType == 1) && (!_tcslen(pRegInfo->Exts)))
		pRegInfo->ScanFilesType = 0;

	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_SkipExts, pRegInfo->SkipExts, EXT_LIST_SIZE, "");

	if (pRegInfo->bZipFile)
		pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_ZipExts, pRegInfo->ZipExts, EXT_LIST_SIZE, DEFAULT_ARC_EXTS);

	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_WarningSubject, pRegInfo->MessageSubject, MAX_SUBJECT, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_WarningMessage, pRegInfo->MessageWarning, MAX_MESSAGE, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_WarningInfectionInfo, pRegInfo->MessageInfectionInfo, MAX_INFECT_INFO, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_SenderSubject, pRegInfo->SenderSubject, MAX_SUBJECT, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_SenderMessage, pRegInfo->SenderMessageBody, MAX_MESSAGE, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_SenderInfectionInfo, pRegInfo->SenderInfectionInfo, MAX_INFECT_INFO, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_Recipients, pRegInfo->SelectedRecipients, MAX_SELECTED, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_SelectedSubject, pRegInfo->SelectedSubject, MAX_SUBJECT, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_SelectedMessage, pRegInfo->SelectedMessageBody, MAX_MESSAGE, "");
	pSSFunctions->GetStr(storageInfo.hRTSConfigKey, szReg_Val_SelectedInfectionInfo, pRegInfo->SelectedInfectionInfo, MAX_INFECT_INFO, "");
	
	pSSFunctions->Open(reinterpret_cast<HKEY>(HKEY_VP_STORAGE_ROOT), LOTUS_NOTES, &hStorageKey);
	if(hStorageKey)
	{
		g_debug = pRegInfo->bDebug = pSSFunctions->GetVal(hStorageKey, "Debug", 0);
		pSSFunctions->Close(hStorageKey);
	}

	// Get the Buffer pool mutex and copy the new information
	if(WaitForSingleObject(g_hBufferPoolMutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0)
	{
		g_pFTBSBuf->regInfo = *pRegInfo;				
		ReleaseMutex(g_hBufferPoolMutex);
	}
	else
		result = WAIT_TIMEOUT;
	
	// Update the STORAGEDATA structure

	storageInfo.RTSData->ZipDepth = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_ZipDepth, 3);
	storageInfo.RTSData->FileType = pRegInfo->ScanFilesType;
	// removed: storageInfo.RTSData->Types = pSSFunctions->GetVal(storageInfo.hRTSConfigKey, szReg_Val_ScanByType, 6);
	storageInfo.RTSData->ZipFile = pRegInfo->bZipFile;

	_tcscpy(storageInfo.RTSData->Exts, pRegInfo->Exts);
	_tcscpy(storageInfo.RTSData->ZipExts, pRegInfo->ZipExts);

	free(pRegInfo);

	return result;
}

/*---------------------------------------------------------------------------*/

// NSE_ReInit is called to indicate that initialization options may have changed.

DWORD  NSE_ReInit(DWORD flags)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/
// Manual Scan functions

/*---------------------------------------------------------------------------*/

//	NSE_FindFirstNode is called by the storage manager to open a find context.  This function 
//	uses the parent handle if present to try and open the find context requested by path.
//	If the specified context exists, information about the first node in that context is 
//	returned to the storage manager in the pQNode and PSNode structures.
//	
//	The key for the notes storage extension is formatted as follows:
//	$:\*d:[database]*f:[folderID]*n:[note ID]*a:[attachment]
//	$:\*d:dbname.nsf*f:0xFFFFFFFF*n:0xFFFFFFFF*a:0xFFFFFFFF:FFFF
//
//	Parameters:
//		hParent		[in] A handle to a previously opened find context.
//		instanceID	[in] The storage extension instanceID for this extension (always 0).	
//		pQNode		[out] A pointer to a QNODE structure which will contain display
//					and key info about the first node in the context.
//		pSNode		[out] A pointer to an SNODE structure which will contain file
//					information about the first node in the context.
//	Returns:
//		A pointer to a find context if it exists, otherwise INVALID_HANDLE_VALUE.

DWORD NSE_FindFirstNode(HANDLE hParent, DWORD instanceID, const char *path, PQNODE pQNode, PSNODE pSNode, HANDLE* handle)
{
	ASSERT0(FALSE, "NSE_FindFirstNode called!");
	if( handle != NULL )
		*handle = INVALID_HANDLE_VALUE;
	return ERROR_NO_MORE;
}

/*---------------------------------------------------------------------------*/

//	FindNextNode is called by the storage manager to enumerate the next node
//	in the given find context.  A unique function is called to perform the 
//	enumeration based on the type of find context.
//	
//	Parameters:
//		handle		[in] A find context defined by a call to FindFirstNode.
//		pQNode		[out] A pointer to a QNODE which will contain display
//					information about the node.
//		pSNode		[out] Unused in this function.
//
//	Returns:
//			ERROR_NO_MORE since scanning for this context is complete.

DWORD  NSE_FindNextNode(HANDLE handle, PQNODE  pQNode, PSNODE pSNode)
{
	ASSERT0(FALSE, "NSE_FindNextNode called!");
	return ERROR_NO_MORE;
}

/*---------------------------------------------------------------------------*/

//	NSE_CreateSNode	is called by the storage manager to prepare
//	a previously queried node for scanning.  CreateSNode opens
//	a database, note and extracts the file attachment to disk to 
//	be scanned.
//	
//	Parameters:
//		instanceID	[in] The instanceID (always 0)
//		path		[in] A string indicating the requested node.
//					This string is of the following format:
//					*d:dbname.nsf*f:000fab34*m:000afb23*a:ffffffff:2afe
//		pSNode		[out] A pointer to an SNODE structure that will be
//					contain information about a node to be scanned.
//
//	Returns:
//		ERROR_SUCCESS

DWORD  NSE_CreateSNode(DWORD instanceID, const char *path, PSNODE pSNode)
{
	ASSERT0(FALSE, "NSE_CreateSNode called!");
	// don't let anyone think we've actually created an SNODE...
	return ERROR_GENERAL;
}


/*---------------------------------------------------------------------------*/

//	No messages are defined yet for ProcessPacket
DWORD  NSE_ProcessPacket(WORD FunctionCode,BYTE *sendBuffer,
						 DWORD sendBufferSize,BYTE *replyBuffer,
						 DWORD *replyBufferSize,CBA_Addr *address)
{
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

//	NSE_FormatMessage is called to format a text message which
//	may contain mail specific format specifiers.  This function
//	is called by the storage manager to format a message for 
//	a message box that is displayed to a user when an infection
//	in detected in a message attachment.
//	
//	Parameters:
//		out	[out] A pointer to a buffer which will contain formatted text.
//		format [in] A pointer to a buffer which contains a format string.
//		eb	[in] A pointer to an event block which contains virus information.
//
//	Returns:
//		ERROR_SUCCESS for success.
//		ERROR_GENERAL for error.

static DWORD NSE_FormatMessage(char *out, size_t nNumOutBytes, const char *format, PEVENTBLOCK eb, DWORD userParam)
{
	PFILE_INFO pFileInfo = GetFileInfoBlockFromContext(eb->so->Node->Context);
	
	if(!pFileInfo)
		return ERROR_GENERAL;

	// tmm: I checked that all calls (expect for one that I fixed)
	// to this callback have an "out" parameter of MAX_MESSAGE_SIZE in size
	return FormatMailString(out, nNumOutBytes, format, pFileInfo);
}

/*---------------------------------------------------------------------------*/

//	NSE_ChangeUser is called to notify the extension that the 
//	current logged on user status has changed.  The extension may
//	do anything necessary here to make sure that authentication is complete.
//	
//	Parameters:
//		NewUserName	[in] A pointer to a string containing the new user name.
//		HANDLE [in] A handle to the WIN32 access token of the new user.
//		
//	Returns:
//		ERROR_SUCCESS for success.


DWORD  NSE_ChangeUser(const char *NewUserName,HANDLE hAccessToken)
{
    (void)NewUserName;
    InterlockedExchangePointer(reinterpret_cast<PVOID*>(&g_hUserAccessToken), static_cast<PVOID>(hAccessToken));
    if (hAccessToken != NULL)
    {
		pSSFunctions->dPrintf("User change notification received, hooking notes if needed...\n");
        HookNotesIfNeeded(hAccessToken);
    }
	return ERROR_SUCCESS;
}


/*---------------------------------------------------------------------------*/

//	NSE_FindClose is called to close the specified find context.
//	
//	Parameters:
//		handle	[in] A pointer to a previously opened find context.
//
//	Returns:
//		ERROR_SUCCESS

DWORD NSE_FindClose(HANDLE handle)
{
	ASSERT0(FALSE, "NSE_FindClose called!");
	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

//	NSE_DeInit is called to Deinitialize the storage extension.
//	
//	Parameters:
//		handle	[in]  A handle returned from the STORAGEINIT function.
//	Returns:
//		ERROR_SUCCESS

DWORD  NSE_DeInit(HANDLE handle)
{
	pSSFunctions->Close(storageInfo.hRTSConfigKey);

	g_bScanContextRegistered = FALSE;

	DebugOut(SM_GENERAL, "NLNVP: scan context deinitialized");

	DeInitIPC();

	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/
//	FormatMailString formats message text which may contain mail specific
//	format specifiers.  These include EmailMessageSubject, InfectedAttachmentName,
//	EmailSender.
//
//	Parameters:
//		pMsgBuf         [out] A buffer which will contain formatted message text.
//		nNumMsgBufBytes [in]  Size of pMsgBuf in bytes including space for EOS.
//		pMsgFmt         [in]  A pointer to a format string.
//		pFileInfo       [in]  A pointer to a FILE_INFO structure.
//
//	Returns:
//		ERROR_SUCCESS

DWORD FormatMailString(LPTSTR pMsgBuf, size_t nNumMsgBufBytes, LPCTSTR pMsgFmt, PFILE_INFO pFileInfo)
{
    // Check for space for at least one char for EOS

    if ( nNumMsgBufBytes < sizeof(pMsgBuf[0]) )
    {
        return ( ERROR_GENERAL );
    }

    // Initialize output buffer

    pMsgBuf[0] = _T('\0');

    char szMyMsgFmt[MAX_MESSAGE_SIZE];
    vpstrncpy (szMyMsgFmt, pMsgFmt, sizeof (szMyMsgFmt));

	LPSTR pLast  = szMyMsgFmt;
	LPSTR pBegin = szMyMsgFmt;

	while ((pBegin = _tcschr(pBegin, '~')))
	{
		LPSTR pTmp  = _tcsinc(pBegin);
		LPSTR pText = NULL;

		switch(*pTmp)
		{
			case 'u':	// EmailMessageSubject
			case 'U':		
				pText = pFileInfo->szSubject;
				break;
			
			case 'o':	// OriginalAttachmentName
			case 'O':
				pText = pFileInfo->szAnsiFileName;
				break;

			case 'd':	// EmailSender					
			case 'D':
				pText = pFileInfo->szFrom;
				break;

			case 'i':	// Recipients					
			case 'I':
				pText = pFileInfo->szTo;
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
	
	return ERROR_SUCCESS;
}
	
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

//	NSE_StorageInit is called by the storage manager to initialize the storage extension.
//	All necessary structures are initialized and the primary registry key for the extension
//	is opened and stored in the STORAGEINFO stucture.
//	
//	Parameters:
//		flags	[in] A DWORD indicating initialization options.
//		pInfo	[out] A pointer to a storage info structure which is initialized by the 
//				storage extension.
//		handle	[out] A pointer to a module handle which is initialized here.
//		SSFunctions [in] A pointer to functions provided by the storage manager.
//
//	Returns:
//		ERROR_SUCCESS, ERROR_NO_KEY, ERROR_FUNCTION_NOT_SUPPORTED, or an error
//		returned from InitIPC.

DWORD NSE_StorageInit(DWORD flags, PSTORAGEINFO * pInfo, HANDLE * handle, PSSFUNCTIONS pSSFuns)
{
	static HANDLE moduleHandle = (HANDLE)1;
	DWORD result = ERROR_SUCCESS;
	HKEY hKey = NULL;
	
	// WARNING: don't call DebugOut(...) until after the next check
	// (to see if we're are RTSing or not).  If we are not RTSing, then
	// we may have been started from CliScan, in which case the FindWindow
	// call in the DebugOut(...) function will probably hang, since
	// the apps that use CliScan don't respond to window messages while
	// the storage manager startup is processing (on Win9x machines).
	// check to see if we should init at all...  we only init if we're RTSing...

	if(!(flags & S_WANT_RTS))
		return ERROR_FUNCTION_NOT_SUPPORTED;

	g_debug = TRUE;	// assume we want debug output until we
					// get the real value of the debug flag
					// from the registry

	*pInfo = &storageInfo;
	
	*handle = moduleHandle;

	pSSFunctions = pSSFuns;
	pSSFunctions->Open(reinterpret_cast<HKEY>(HKEY_VP_STORAGE_ROOT), LOTUS_NOTES, &hKey);
	if(!hKey)
		return ERROR_NO_KEY;
	
	pSSFunctions->Open(hKey, "RealTimeScan", &storageInfo.hRTSConfigKey);
	if(!storageInfo.hRTSConfigKey)
	{
		pSSFunctions->Close(hKey);
		hKey = NULL;
		return ERROR_NO_KEY;
	}

	if (result = InitIPC())
	{
		DebugOut(SM_ERROR, "ERROR: NLNVP: scan context NOT successfully initialized");
		pSSFunctions->Close(hKey);
		hKey = NULL;
		return result;
	}

	g_pFTBSBuf->realTimeThreads = 0;
	g_pFTBSBuf->consumerIndex = 1;
	g_pFTBSBuf->producerIndex = 1;
	
	g_bScanContextRegistered = TRUE;

	// Initialize the registry settings
	NSE_ReloadRTSConfig();

	// start the NotesWatch thread
	startNotesWatch();

	DebugOut(SM_GENERAL, "NLNVP: scan context successfully initialized");

	pSSFunctions->Close(hKey);
	hKey = NULL;
	return ERROR_SUCCESS;
}

/*--- end of source ---*/
