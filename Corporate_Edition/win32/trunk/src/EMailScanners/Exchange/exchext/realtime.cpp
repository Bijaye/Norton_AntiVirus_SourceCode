// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright © 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
//	Virus Protect Storege Extension API routines for realtime mail scanning.

#include "ldvpmec.h"
#include "realtime.h"
#include "storage.h"
#include "comm.h"
#include "resource.h"
#include "vpstrutils.h"

#include <assert.h>
#ifdef _USE_CCW
#include "symcharconvert.h"
#endif

static ACTIONDATA 	s_actionData;
static HANDLE		s_hSERTShutdown;		// SE Shutdown event
static TCHAR	g_szUserName[256];			// Messaging User Name
static mSID	g_FalseSID;

extern SFILETABLE g_RealTimeIOTable;
extern NODEFUNCTIONS g_RealTimeNodeFunctions;

void CreateFalseSID(mSID *psid)
{
	// note that SID_SIZE is defined as 64, and that a SID is
	// defined as an array of DWORD [SID_SIZE], so the actual
	// number of TCHARs in the array is a multiple of SID_SIZE;
	// 4*SID_SIZE for chars, and 2*SID_SIZE for wchars.
	DWORD maxLen = SID_SIZE * (sizeof(DWORD) / sizeof(TCHAR));
	DWORD userLen;
	mSID  mySID = {0};
	TCHAR szSep[] = _T("\x01");
		
	mySID.stuff[0] = 0xFFFFFFFF;
	maxLen -= sizeof(DWORD) / sizeof(TCHAR);

	userLen = maxLen - (_tcslen(g_szComputerName)) - 2;
	_tcsncpy((LPTSTR)&mySID.stuff[1], g_szUserName, userLen);
	_tcscat((LPTSTR)&mySID.stuff[1], szSep);

	maxLen -= _tcslen((LPTSTR)&mySID.stuff[1]);
	_tcsncat((LPTSTR)&mySID.stuff[1], g_szComputerName, maxLen);
	
	*psid = mySID;


	/* previous implementation...  taken out so that new implementation
	// will properly handle TCHARs...
	TCHAR
		*pnt = (LPTSTR) &psid->stuff[1];

	int
		s,
		t,
		len = sizeof(mSID);

	// Note, it is theoretically possible for the user name to exceed the length allowed in the fake SID.
	// We will just truncate the name and ignore this (its very unlikely).  The user name would need to
	// exceed (252 - strlen(g_szComputerName) + 1) bytes to be truncated.  Thats a very long username.
	//

	psid->stuff[0] = 0xFFFFFFFF;
	len -= sizeof(psid->stuff[0]);

	t = len - (strlen(g_szComputerName) + 1);
	_tcstcpy(pnt, g_szUserName, t);

	s = strlen(pnt);
	len -= s;
	pnt += s;
	*pnt++ = '\x01';
	len--;
	_tcstcpy(pnt, g_szComputerName, len);

	*/ // end of previous implementation!
}

DWORD LoadConfig()
{
	HKEY
		hkStorageRoot;

	DWORD
		rv;

	// Get mutex to memory file before filling in the values
    switch ( WaitForSingleObject(g_hMemMutex, WAIT_MemMutex) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
        // Okay
        break;
    default:
		return ERROR_GENERAL;		// Can't get the mutex, something is wrong.
    }

	// Load RTS settings
	g_pMemFile->bRTEnabled = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_RTSScanOnOff, 1);

	g_pMemFile->bMarkMessages = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_MarkMessages, 0);
	
	g_pMemFile->extInfo.fileType = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_FileTypes, 0);

	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_Extensions, g_pMemFile->extInfo.szExts, EXT_LIST_SIZE, "");

	// if we're scanning by extension and if there are no extensions, switch to scan-all-files
	if ((g_pMemFile->extInfo.fileType == 1) && (!_tcslen(g_pMemFile->extInfo.szExts)))
		g_pMemFile->extInfo.fileType = 0;

	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_SkipExts, g_pMemFile->extInfo.szSkipExts, EXT_LIST_SIZE, "");

	if (g_pMemFile->extInfo.bScanZipFiles = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_ScanZipFile, 1))
		g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_ZipExts, g_pMemFile->extInfo.szZIPExts, EXT_LIST_SIZE, DEFAULT_ARC_EXTS);

	// Infected Message notification settings
	s_actionData.bInsertWarning = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_InsertWarning, 1);
	s_actionData.bChangeSubject = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_ChangeSubject, 1);
	
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_WarningSubject, s_actionData.szWarningSubject, MAX_SUBJECT_STRING, "");
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_WarningMessage, s_actionData.szWarningMessage, MAX_MESSAGE_STRING, "");
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_WarningInfectionInfo, s_actionData.szWarningInfectionInformation, MAX_INFECT_INFO, "");

	// Notify Sender notification settings
	s_actionData.bNotifySender = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_EmailSender, 1);
	
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_SenderSubject, s_actionData.szSenderSubject, MAX_SUBJECT_STRING, "");
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_SenderMessage, s_actionData.szSenderMessage, MAX_MESSAGE_STRING, "");
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_SenderInfectionInfo, s_actionData.szSenderInfectionInformation, MAX_INFECT_INFO, "");

	// Administrator (selected) notification settings
	s_actionData.bNotifySelected = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_EmailSelected, 0);

	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_Recipients, s_actionData.szSelectedRecips, MAX_SELECTED_STRING, "");
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_SelectedSubject, s_actionData.szSelectedSubject, MAX_SUBJECT_STRING, "");
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_SelectedMessage, s_actionData.szSelectedMessage, MAX_MESSAGE_STRING, "");
	g_pSSFunctions->GetStr(g_storageInfo.hRTSConfigKey, szReg_Val_SelectedInfectionInfo, s_actionData.szSelectedInfectionInformation, MAX_INFECT_INFO, "");

	rv = g_pSSFunctions->Open((HKEY) HKEY_VP_STORAGE_ROOT, szReg_Key_Storage_Exchange, &hkStorageRoot);
	if (!rv)
		{
		g_debug = g_pMemFile->bDebug = g_pSSFunctions->GetVal(hkStorageRoot, _T("Debug"), 0);
		g_pSSFunctions->Close(hkStorageRoot);
		}

	ReleaseMutex(g_hMemMutex);

	// Update the STORAGEDATA structure

	g_storageInfo.RTSData->ZipDepth = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_ZipDepth, 3);
	g_storageInfo.RTSData->FileType = g_pMemFile->extInfo.fileType;
	// removed: g_storageInfo.RTSData->Types = g_pSSFunctions->GetVal(g_storageInfo.hRTSConfigKey, szReg_Val_ScanByType, 6);
	g_storageInfo.RTSData->ZipFile = g_pMemFile->extInfo.bScanZipFiles;

	_tcscpy(g_storageInfo.RTSData->Exts, g_pMemFile->extInfo.szExts);
	_tcscpy(g_storageInfo.RTSData->ZipExts, g_pMemFile->extInfo.szZIPExts);

	return ERROR_SUCCESS;

} // LoadConfig()


// StartSERT() Sets up the state and shutdown event for real time processing
// and loads the real time configuration data.
BOOL StartSERT(void)
{
	DebugOut(SM_GENERAL, "StartSERT");

	// Create a shutdown event (manual reset) to manage the RTSWatch threads
	s_hSERTShutdown = CreateEvent(0, TRUE, FALSE, 0);
	if (!s_hSERTShutdown)
		return FALSE;

	return TRUE;
} // StartSERT()


// EndSERT() Shutdown realtime and close realtime shutdown event
void EndSERT(void)
{
	DebugOut(SM_GENERAL, "EndSert");

	if (s_hSERTShutdown)
		{
		CloseHandle(s_hSERTShutdown);
		s_hSERTShutdown = 0;
		}

} // EndSERT()


// ProcessRTSignal() is called (by BeginRTSWatch()) when a process block with state 'Scan' is found.
// ProcessRTSignal() sets up SNODE in preparation to call pfnProcessRTSNode().  Note that the context
// passed to the I/O and SFUNCTIONs is the copy of the process block.  This is so that the I/O functions
// do not need to obtain MemMutext before accessing process block data.  PROCESSBLOCK.ACTIONDATA will
// be copied back to the real process block by this function.
//
// Inputs
//	pRealProcessBlock	Pointer in MemFile of the process block (should be accessed only under MemMutex lockout)
//	processBlockIndex	Index of real process block
//	pCopy				Copy of process block (obtained under mutex by BeginRTSWatch() the values in the copy
//						can be safely accessed without obtaining MemMutex).  pCopy->instanceID should
//						be compared with pRealProcessBlock->instanceID to see if MEC has timed out.
//	pfnProcessRTSNode	Pointer to function in storage manager for processing of attachments
//	pSNode				Pointer to SNODE for this thread
//	pContext			Storage Manager context for this thread.
//
// Outputs
//	returns	ERROR_SUCCESS on successful signal processing
//			!0 when RTSWatch Thread should exit
//	*pRealProcessBlock	Updated with results of scan
//
static DWORD ProcessRTSignal(PPROCESSBLOCK pRealProcessBlock, int index, PPROCESSBLOCK pCopy, PROCESSRTSNODE pfnProcessRTSNode, PSNODE pSNode, void *pContext)
{
	LPTSTR
		ptExt;

	int
		bScanComplete;

	DebugOut(SM_GENERAL, "ProcessRTSignal: index %d, instance %d", index, pCopy->instanceID);

	g_pMemFile->cSEAttacmentsProcessed++;

	// initialize SNODE
    memset( pSNode, 0, sizeof( *pSNode ) );
	pSNode->IO = &g_RealTimeIOTable;
	pSNode->Context = pCopy;
	pSNode->Flags = N_MAILNODE | N_RTSNODE;
#ifdef _USE_CCW
	//TODO:KT Problem : assigning a stack variable to pSNode->InternalPath
	CharToUni	oIPTmpPath(pCopy->input.szTempPath);
	pSNode->InternalPath = (SYMUCHAR*)(const SYMUCHAR*)oIPTmpPath;
#else	//_USE_CCW
	pSNode->InternalPath = pCopy->input.szTempPath;
#endif	//_USE_CCW
	pSNode->Operations = FA_READ | FA_WRITE | FA_FILE_NEEDS_SCAN | FA_AFTER_OPEN;
	pSNode->InstanceID = MEC_SIGNATURE;	// our "drive" letter
	pSNode->UID = pCopy->instanceID;
#ifdef _USE_CCW
	ssStrnCpy(pSNode->Description, CharToUni(pCopy->input.szAttachmentName), sizeof(pSNode->Description));
#else //_USE_CCW
	strcpy(pSNode->Description, pCopy->input.szAttachmentName);
#endif	//_USE_CCW
	ptExt = pCopy->input.szExtension;
	if (*ptExt == _T('.'))	// if '.' found as first char in extension, skip it
		ptExt = _tcsinc(ptExt);
	strcpy(pSNode->Ext, ptExt);
	strcpy(pSNode->Name, pCopy->input.szAttachmentName);
	
	strcpy(g_szUserName, pCopy->input.szUserName);
	CreateFalseSID(&g_FalseSID);
	pSNode->Sid = g_FalseSID;

	pSNode->Functions = &g_RealTimeNodeFunctions;

	pfnProcessRTSNode(pSNode, pContext);	// scan the attachment!

	bScanComplete = false;
    switch ( WaitForSingleObject(g_hMemMutex, WAIT_MemMutex) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
		if (pRealProcessBlock->instanceID == pCopy->instanceID)	// check for MEC timeout
			{
			pRealProcessBlock->action = pCopy->action;			// lots copied here
			pRealProcessBlock->state = PROCESSBLOCK::Complete;
			bScanComplete = true;
			}
		else
			DebugOut(SM_WARNING, "ProcessRTSignal: index %d, instance %d, MEC timed out!", index, pCopy->instanceID);

		ReleaseMutex(g_hMemMutex);
        break;

    default:
		DebugOut(SM_ERROR, "ProcessRTSignal: index %d, instance %d, unable to get mutex!", index, pCopy->instanceID);
        break;
    }


	if (bScanComplete)
		SetEvent(g_hPBEvent[index]);			// Release MEC
	else
		DeleteFile(pCopy->input.szTempPath);	// if MEC timed out, SE must delete temporary file

	DebugOut(SM_GENERAL, "ProcessRTSignal: index %d, instance %d, exit!", index, pCopy->instanceID);

	return ERROR_SUCCESS;
}

// MEC_BeginRTSWatch() called by storage manager to wait for real time events.  This function will
// be called on many different threads.  BeginRTSWatch() waits for a real time event or for the shutdown
// event.  When an RTS Event occurs, BeginRTSWatch() searches for a process block with state of
// ScanAttachment.  It calls ProcessRTSignal() to do the actual scanning.
//
// Inputs:
//	pfnProcessRTSNode	Pointer to function in storage manager that processes real time scanning
//	pSNode				Pointer to SNODE structure used by this thread to process real time events
//	pContext			Storage Manager context for this thread.
//
// Return:
//	ERROR_SUCCESS		on completion (shutdown event or to many threads)
//	other				on some sort of non-recoverable error
//
// Outputs:
//	*pSNode				Modified appropriately for each real time event
//
// Globals/Statics used.
//	s_hSERTShutdown		Waits on the real time shutdown event
//	g_hRTSemaphore		Waits on the real time event semaphore
//	g_hMemMutex			Blocks MemFile as appropriate
//	g_pMemFile			Modifies data in MemFile as appropriate
//
DWORD MEC_BeginRTSWatch(PROCESSRTSNODE pfnProcessRTSNode, PSNODE pSNode, void *pContext)
{
	static TCHAR s_verboseName[] = _T("MEC_BeginRTSWatch");

	PPROCESSBLOCK	copy, pProcessBlock;
	HANDLE			hndls[2];
	DWORD			rv = ERROR_SUCCESS;
	int				index, exitNow, watchId;

	// Check number of threads.  Don't allow number to exceed number of process blocks.
    switch ( WaitForSingleObject(g_hMemMutex, WAIT_MemMutex) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
        // Okay
        break;
    default:
        return ERROR_GENERAL;       // Can't get the mutex, something is wrong.
    }

	if (g_pMemFile->cSERTWatch >= MAX_REALTIME_THREADS)
		{
		// If to many watches, simply return (successfully).  This thread will then die.
		ReleaseMutex(g_hMemMutex);
		return ERROR_SUCCESS;	
		}

	// Any exits after this point, must decrement thread count
	watchId = ++g_pMemFile->cSERTWatch;

	ReleaseMutex(g_hMemMutex);

	DebugOut(SM_GENERAL, _T("%s: [%d]"), s_verboseName, watchId);

	hndls[0] = g_hRTSemaphore;
	hndls[1] = s_hSERTShutdown;

	exitNow = false;

	copy = (PROCESSBLOCK*) malloc(sizeof(PROCESSBLOCK));
	if (!copy)
		{
		DebugOut(SM_ERROR, _T("ERROR: %s: [%d] unable to create PROCESSBLOCK"), s_verboseName, watchId);
		exitNow = true;
		}

	while (!exitNow)
		{
		DebugOut(SM_GENERAL, _T("%s: [%d] waiting for scan request..."), s_verboseName, watchId);
		rv = WaitForMultipleObjects(2, hndls, FALSE, INFINITE);
		if (rv == WAIT_FAILED) 
			{
			exitNow = true;
			continue;
			}
		else if ((rv - WAIT_OBJECT_0) == 1) 
			{
			// shutdown event has been signalled hence start shutdown
			DebugOut(SM_GENERAL, _T("%s: [%d] shutdown!"), s_verboseName, watchId);
			break;
			}

		// even if wait timed out, look for something to do
		DebugOut(SM_GENERAL, _T("%s: [%d] attempting to process a request..."), s_verboseName, watchId);

		do
			{		// repeat until no more process blocks to scan

			ZeroMemory(copy, sizeof(PROCESSBLOCK));
			pProcessBlock = ScanProcessBlock(&index, copy);
			if (pProcessBlock)
				{
				rv = ProcessRTSignal(pProcessBlock, index, copy, pfnProcessRTSNode, pSNode, pContext);
				if (rv != ERROR_SUCCESS)
					{
					exitNow = true;
					break;
					}
				}
			} while (pProcessBlock);
		}

	// out of main processing loop, so dec thread count
    switch ( WaitForSingleObject(g_hMemMutex, WAIT_MemMutex) )
    {
    case WAIT_OBJECT_0:
    case WAIT_ABANDONED:
		if (g_pMemFile)
			g_pMemFile->cSERTWatch--;
		ReleaseMutex(g_hMemMutex);
        break;

    default:
        break;
    }

	if (copy)
		free(copy);

	DebugOut(SM_GENERAL, _T("%s: [%d] exit"), s_verboseName, watchId);

	return rv;
} // MEC_BeginRTSWatch()


// StopRTSWatches() shuts down any threads blocked in BeginRTSWatch().  This does not
// imply that real time is shutting down, it just releases (unblocks) the threads.
// Its possible (though unlikely) that the storage manager will again call BeginRTSWatch()
// after StopRTSWatches() returns.
DWORD MEC_StopRTSWatches(void)
{
	static TCHAR s_verboseName[] = _T("MEC_StopRTSWatches");

	enum	{ SLEEP_MILLISECONDS = 100, SECONDS_TO_WAIT = 20 };
	int		count;

	DebugOut(SM_GENERAL, _T("%s: [%d]"), s_verboseName, g_pMemFile->cSERTWatch);

	// It's possibly that we're called an aborted startup--the event may not exist
	if (s_hSERTShutdown)				
		SetEvent(s_hSERTShutdown);

	count = (SECONDS_TO_WAIT * 1000) / SLEEP_MILLISECONDS;
	while (g_pMemFile->cSERTWatch > 0 && count-- > 0)
		Sleep(SLEEP_MILLISECONDS);

	// Set the event to non-signalled (in case we start up again)
	if (s_hSERTShutdown)				
		ResetEvent(s_hSERTShutdown);

	DebugOut(SM_GENERAL, _T("%s: [%d] exit"), s_verboseName, g_pMemFile->cSERTWatch);

	// ERROR_SUCCESS (== 0) if all threads shutdown, !0 if some threads haven't shutdown
	return !!g_pMemFile->cSERTWatch;	

} // MEC_StopRTSWatches()


// ReloadRTSConfig() copies all data from the configuration "registry" entries to the approprate
// state variables.
DWORD MEC_ReloadRTSConfig(void)
{
	static TCHAR s_verboseName[] = _T("MEC_ReloadRTSConfig");
	DWORD rc;

	DebugOut(SM_GENERAL, s_verboseName);

	rc = LoadConfig();

	return rc;
} // MEC_ReloadRTSConfig()


#include <io.h>
#include <fcntl.h>
#include <share.h>
#include <sys/stat.h>

// IO table functions specific to real time
// 
DWORD MEC_SERTopen(PSNODE node, DWORD access)
{
	static TCHAR s_verboseName[] = _T("MEC_SERTopen");

	PPROCESSBLOCK
		pProcessBlock = (PPROCESSBLOCK) node->Context;

	int
		oFlags;

//	DebugOut(SM_GENERAL, "%s: id = %d, file \"%s\", access = %u", s_verboseName, pProcessBlock->instanceID, pProcessBlock->input.szTempPath, access);

	if ((access & FTO_RDWR) == FTO_RDONLY)
		oFlags = _O_RDONLY;
	else
		oFlags = _O_RDWR;

	if (access & FTO_APPEND)
		oFlags |= _O_APPEND;

	if (access & FTO_TRUNC)
		oFlags |= _O_TRUNC;

	if (access & FTO_BINARY)
		oFlags |= _O_BINARY;

	return _tsopen(pProcessBlock->input.szTempPath, oFlags, _SH_DENYNO, _S_IREAD | _S_IWRITE);
}

DWORD MEC_SERTaccess(PSNODE node, int mode)
{
	static TCHAR s_verboseName[] = _T("MEC_SERTaccess");

	PPROCESSBLOCK
		pProcessBlock = (PPROCESSBLOCK) node->Context;

	DWORD yesno = _taccess(pProcessBlock->input.szTempPath, mode);

//	DebugOut(SM_GENERAL, "%s: id = %d, file \"%s\", mode = %d, yesno = %u", s_verboseName, pProcessBlock->instanceID, pProcessBlock->input.szTempPath, mode, yesno);

	return yesno;
}

DWORD MEC_SERTGetState(PSNODE /* node */, BYTE * /* Data */)
{
	return ERROR_GENERAL;
}

DWORD MEC_SERTSetState(PSNODE /* node */, BYTE * /* Data */)
{
	return ERROR_GENERAL;
}

DWORD MEC_GetExtendedData(PSNODE /* node */, char * /* keyname */, BYTE * /* Data */, DWORD /* len */)
{
	return ERROR_GENERAL;
}

DWORD MEC_SetExtendedData(PSNODE /* node */, char * /* keyname */, BYTE * /* Data */, DWORD /* len */)
{
	return ERROR_GENERAL;
}

DWORD MEC_GetFullKey(PSNODE /* node */, char * /* Data */, DWORD /* len */)
{
	return ERROR_FUNCTION_NO_SUPPORTED;
}
	
// Generic SFileTable functions.

DWORD MEC_close(DWORD handle)
{
	return _close(handle);
}

DWORD MEC_read(DWORD handle, void *data, DWORD length)
{
	return _read(handle, data, length);
}

DWORD MEC_write(DWORD handle, void *data, DWORD length)
{
	return length == 0 ? _chsize(handle, _lseek(handle, 0, SEEK_CUR)) : _write(handle, data, length);
}

DWORD MEC_lseek(DWORD handle, long offset, DWORD fromwhere)
{
	return _lseek(handle, offset, fromwhere);
}

SFILETABLE g_RealTimeIOTable = {
	MEC_SERTopen,
	MEC_close,
	MEC_read,
	MEC_write,
	MEC_lseek,
	MEC_SERTaccess,
	MEC_SERTGetState,
	MEC_SERTSetState,
	MEC_GetExtendedData,
	MEC_SetExtendedData,
	MEC_GetFullKey
};


// Real time node functions
//
DWORD MEC_SERTReleaseFindNode(PSNODE /* node */)
{
	// static TCHAR s_verboseName[] = _T("MEC_SERTReleaseFindNode");

	// In real time, the context is on the stack (PROCESSBLOCK copy allocated in BeginRTSWatch).
	// Nothing needs to be done.

	return ERROR_SUCCESS;
}

DWORD MEC_SERTRenameNode(PSNODE /* node */, char * /* newExt */)
{
	// static TCHAR s_verboseName[] = _T("MEC_SERTRenameNode");

	// All Real time actions are done in MEC_SERTNodeHasVirus()

	return ERROR_SUCCESS;
}


DWORD MEC_SERTRemoveNode(PSNODE /* node */)
{
	// static TCHAR s_verboseName[] = _T("MEC_SERTRemoveNode");

	// All Real time actions are done in MEC_SERTNodeHasVirus()

	return ERROR_SUCCESS;
}

DWORD MEC_SERTNodeHasVirus(PSNODE node, PEVENTBLOCK eb)
{
	static TCHAR s_verboseName[] = _T("MEC_SERTNodeHasVirus");

	PPROCESSBLOCK
		pProcessBlock = (PPROCESSBLOCK) node->Context;

	int
		bProcessActionStrings = false;

	TCHAR
		work[MAX_MESSAGE_STRING];
 
	DebugOut(SM_GENERAL, "%s: id = %d, realAction = %d", s_verboseName, pProcessBlock->instanceID, eb->RealAction);

	// check to see if the file is compressed, if not use the EVENTBLOCK's
	// status to determine what to do.  this function can be called multiple times
	// for the same file when the attachment is compressed, and each time this 
	// fcn is called the RealAction may vary which is why we need to ignore 
	// the RealAction for compressed files. -15AUG99 AMS
	if (!eb->pdfi->bIsCompressed)
	{
		if (eb->RealAction == AC_CLEAN)
		{
			pProcessBlock->action.code = ACTIONDATA::Replace;
			bProcessActionStrings = true;
		}
		else if (eb->RealAction == AC_DEL)
		{
			pProcessBlock->action.code = ACTIONDATA::Remove;
			bProcessActionStrings = true;
		}
		else if (eb->RealAction == AC_MOVE)
		{
			pProcessBlock->action.code = ACTIONDATA::Remove;
			bProcessActionStrings = true;
		}
		if (eb->RealAction == AC_NOTHING)
		{
			pProcessBlock->action.code = ACTIONDATA::LeaveAlone;
			bProcessActionStrings = true;
		}
	}
	else // file is compressed
	{
        // File is compressed. We report the summary information for the entire
        // container, not entries for each individual contained file.
        if (eb->pdfi->dwDepthOfFileToGet == 0)
        {
		    if (eb->RealAction == AC_MOVE)
		    {
			    pProcessBlock->action.code = ACTIONDATA::Remove;
			    bProcessActionStrings = true;
		    }
		    else if (eb->RealAction == AC_DEL)
		    {
			    pProcessBlock->action.code = ACTIONDATA::Remove;
			    bProcessActionStrings = true;
		    }
		    else if (eb->pdfi->dwCleaned > 0 ||  eb->pdfi->dwDeleted > 0) //  some cleaned or deleted then we need to replace the container file
		    {
			    pProcessBlock->action.code = ACTIONDATA::Replace;
			    bProcessActionStrings = true;
		    }
		    else
		    {
		        pProcessBlock->action.code = ACTIONDATA::LeaveAlone;
		        bProcessActionStrings = true;
		    }

		    // if the VirusName field is blank, insert a message saying this contains multiple viruses
            if (strlen(eb->VirusName) <= 0)
            {
                static char szMultiVirus[255];
                LoadString(g_hResInstance, IDS_MULTIPLE_VIRUS_FOUND, szMultiVirus, 255);
                eb->VirusName = szMultiVirus;
    	    }
	    }
	}


	if (bProcessActionStrings)
	{
		pProcessBlock->action.bInsertWarning = s_actionData.bInsertWarning;
		pProcessBlock->action.bChangeSubject = s_actionData.bChangeSubject;
		pProcessBlock->action.bNotifySender = s_actionData.bNotifySender;
		pProcessBlock->action.bNotifySelected = s_actionData.bNotifySelected;

		if (pProcessBlock->action.bInsertWarning)
		{
			if (pProcessBlock->action.bChangeSubject)
			{
				g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szWarningSubject, eb, 0);
				FormatEmailMessage(pProcessBlock->action.szWarningSubject, sizeof (pProcessBlock->action.szWarningSubject), work, pProcessBlock);
			}

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szWarningMessage, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szWarningMessage, sizeof (pProcessBlock->action.szWarningMessage), work, pProcessBlock);

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szWarningInfectionInformation, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szWarningInfectionInformation, sizeof (pProcessBlock->action.szWarningInfectionInformation), work, pProcessBlock);
		}

		if (pProcessBlock->action.bNotifySender)
		{
			vpstrncpy(pProcessBlock->action.szSelectedRecips, s_actionData.szSelectedRecips, sizeof (pProcessBlock->action.szSelectedRecips));

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szSenderSubject, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szSenderSubject, sizeof (pProcessBlock->action.szSenderSubject), work, pProcessBlock);

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szSenderMessage, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szSenderMessage, sizeof (pProcessBlock->action.szSenderMessage), work, pProcessBlock);

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szSenderInfectionInformation, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szSenderInfectionInformation, sizeof (pProcessBlock->action.szSenderInfectionInformation), work, pProcessBlock);
		}

		if (pProcessBlock->action.bNotifySelected)
		{
			vpstrncpy(pProcessBlock->action.szSelectedRecips, s_actionData.szSelectedRecips, sizeof (pProcessBlock->action.szSelectedRecips));

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szSelectedSubject, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szSelectedSubject, sizeof (pProcessBlock->action.szSelectedSubject), work, pProcessBlock);

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szSelectedMessage, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szSelectedMessage, sizeof(pProcessBlock->action.szSelectedMessage), work, pProcessBlock);

			g_pSSFunctions->FormatMessage(work, sizeof (work), s_actionData.szSelectedInfectionInformation, eb, 0);
			FormatEmailMessage(pProcessBlock->action.szSelectedInfectionInformation, sizeof(pProcessBlock->action.szSelectedInfectionInformation), work, pProcessBlock);
		}
	}

	return ERROR_SUCCESS;
}

NODEFUNCTIONS g_RealTimeNodeFunctions = {
	MEC_SERTReleaseFindNode,
	MEC_SERTNodeHasVirus,
	MEC_SERTRenameNode,
	MEC_SERTRemoveNode,
};

/* end source file */
