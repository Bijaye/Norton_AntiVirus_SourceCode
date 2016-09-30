// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2003, 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
//
//	This source module contains the Virus Protect Storege Extension API routines.  It is the "main"
//	for the storage extension personality of this .DLL.  The main entry point is MEC_StorageInit().
//
//	Most of the API functions are prefixed by MEC_*().  They are all done this way for consistancy.
//	If the prefix weren't present, a few of the function names (like FindFirstNode()) would conflict with
//	WIN32 API functions of the name.  The names don't really matter as they are all accessed through
//	function pointers.

#include "ldvpmec.h"
#include "storage.h"
#include "realtime.h"
#include "comm.h"
#include "advapi.h"
#include "SymSaferRegistry.h"
#include "vpstrutils.h"

#include <io.h>

static IDEF	g_instanceDef;
static STORAGEDATA	g_RTSData;

STORAGEINFO g_storageInfo;
PSSFUNCTIONS g_pSSFunctions;

BOOL IsWinNT( void )
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    return (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
}

// SFunctions functions

static DWORD MEC_RefreshInstanceData(void)
{
	return ERROR_SUCCESS;
}

static DWORD MEC_FindFirstNode(HANDLE parent, DWORD InstanceID, const char *path, PQNODE query, PSNODE node, HANDLE* handle)
{
	if( handle != NULL )
		*handle = INVALID_HANDLE_VALUE;
	return ERROR_NO_MORE;
}

static DWORD MEC_FindNextNode(HANDLE handle, PQNODE query, PSNODE node)
{
	return ERROR_NO_MORE;
}

#ifdef _USE_CCW
static DWORD MEC_CreateSNode(DWORD InstanceID, const wchar_t *path, PSNODE node)
#else	//_USE_CCW
static DWORD MEC_CreateSNode(DWORD InstanceID, const char *path, PSNODE node)
#endif	//_USE_CCW
{
	// don't let anyone think we've actually created an SNODE...
	return ERROR_GENERAL;
}

static DWORD MEC_FindClose(HANDLE handle)
{
	return ERROR_SUCCESS;
}


static DWORD MEC_ProcessPacket(WORD FunctionCode, BYTE *sendBuffer, DWORD sendBufferSize, BYTE *replyBuffer, DWORD *replyBufferSize, CBA_Addr *address)
{
	return ERROR_SUCCESS;
}


//*************************************************************************
// FormatEmailMessage()
//
// This function formats message text which may contain mail specific
// format specifiers. These include EmailMessageSubject, InfectedAttachmentName,
// EmailSender.
//
// DWORD FormatEmailMessage (char * pMsgBuf, int size, char * pMsgFmt, PPROCESSBLOCK pBlock)
//
// Parameters:
//		pMsgBuf         [out] A buffer which will contain formatted message text.
//		nNumMsgBufBytes [in]  Size of pMsgBuf in bytes including space for EOS.
//		pMsgFmt         [in]  A pointer to a format string.
//		pBlock          [in]  A pointer to email message information
//*************************************************************************
// 2003.09.12 DALLEE - comments and changed size parameter to be size of
//      buffer including space for EOS (based on usage).
//*************************************************************************

DWORD FormatEmailMessage (char *pMsgBuf, size_t nNumMsgBufBytes, const char *pMsgFmt, PPROCESSBLOCK pBlock)
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
				pText = pBlock->input.szSubject;
				break;
			
			case 'o':	// OriginalAttachmentName
			case 'O':
				pText = pBlock->input.szAttachmentName;
				break;

			case 'd':	// EmailSender					
			case 'D':
				pText = pBlock->input.szSender;
				break;

			case 'i':	// Recipients					
			case 'I':
				pText = pBlock->input.szRecipients;
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

static DWORD MEC_FormatMessage (char * pMsgBuf, size_t nNumMsgBufBytes, const char * pMsgFmt, PEVENTBLOCK eb, DWORD userParam)
{
	PPROCESSBLOCK pBlock = (PPROCESSBLOCK)eb->so->Node->Context;

	if(!pBlock)
		return ERROR_GENERAL;

	// tmm: I checked that all calls (expect for one that I fixed)
	// to this callback have an "out" parameter of MAX_MESSAGE_SIZE in size
	return FormatEmailMessage(pMsgBuf, nNumMsgBufBytes, pMsgFmt, pBlock);
}

static DWORD  MEC_ChangeUser(const char *NewUserName, HANDLE hAccessToken)
{
	return ERROR_SUCCESS;
}

// DeInit() called by storage manager before the .DLL is unloaded.  This function should insure
// that all resources allocated by the storage are released before returning.
static DWORD MEC_DeInit(HANDLE handle)
{
	DebugOut(SM_GENERAL, "MEC_DeInit");

	if (handle != (HANDLE) MEC_SIGNATURE)
		return ERROR_GENERAL;
	
	--g_pMemFile->cStorages;
	
	// Shutdown real time threads, release real time resources and system objects
	EndSERT();	

	// Release all communication objects
	CloseCommunicationObjects();	

	// Close all open handles
	if (g_storageInfo.hRTSConfigKey) {
		g_pSSFunctions->Close(g_storageInfo.hRTSConfigKey);
		g_storageInfo.hRTSConfigKey = 0;
	}

	return ERROR_SUCCESS;
} // MEC_DeInit()



static DWORD MEC_Reinit(DWORD flags)
{
	return ERROR_SUCCESS;
} // MEC_Reinit()


SFUNCTIONS g_SFunctions = {
	MEC_FindFirstNode,
	MEC_FindNextNode,
	MEC_CreateSNode,
	MEC_FindClose,
	MEC_DeInit,
	MEC_BeginRTSWatch,
	MEC_StopRTSWatches,
	MEC_RefreshInstanceData,
	MEC_ReloadRTSConfig,
	MEC_Reinit,
	MEC_ProcessPacket,
	MEC_FormatMessage,
	MEC_ChangeUser
};

// Initialization function
//
// Called by storage manager (CLISCAN or RTVSCAN) when this storage is initialized.
// This is the "main" function for the SE_Personality.
//
// Inputs:
//	flags
//	functions	pointer to SSFUNCTIONS structure.  This structure contains pointers to functions
//				in the storage manager to access the registry (or simulation of the registry),
//				format messages, display debug messages, etc.
//
// Outputs
//	return		success or failure code
//	*info		This routine must fill out the storage information pointed at by info.
//	*handle		Must return a handle to this instance of the storage.
//
// This function must be extern "C".  Its name is stored in the registry (by the install process).
// The storage manager calls GetProcAddress() on the name.  We don't want to contend with name decoration.
//
DllExport DWORD MEC_StorageInit(DWORD flags, PSTORAGEINFO *info, HANDLE *handle, PSSFUNCTIONS functions)
{
	static TCHAR s_verboseName[] = _T("MEC_StorageInit");

	DWORD
		rv;

	HKEY
		hkStorageRoot;

	g_personality = SE_Personality;

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

//	DebugOut(SM_GENERAL, "MEC_StorageInit");

	// save the SSFunctions
	g_pSSFunctions = functions;
	
	*handle = (HANDLE) MEC_SIGNATURE;
	
	*info = &g_storageInfo;

	memset(&g_storageInfo, 0, sizeof(g_storageInfo));
	g_storageInfo.Size = sizeof(g_storageInfo);
	g_storageInfo.Version = MEC_VERSION;
	g_storageInfo.InstanceCount = 1;
	g_storageInfo.InstanceBlocks = &g_instanceDef;
	g_storageInfo.Functions = &g_SFunctions;
	g_storageInfo.Type = IT_MAIL | IT_CAN_RTS;
	_tcscpy(g_storageInfo.InfFilename, _T("VPMECINS.INF"));
	_tcscpy(g_storageInfo.Name, szReg_Key_Storage_Exchange);
	
	memset(&g_instanceDef, 0, sizeof(g_instanceDef));
	g_instanceDef.InstanceID = MEC_SIGNATURE;
	g_instanceDef.Type = IT_MAIL | IT_CAN_RTS;
	_tcscpy(g_instanceDef.DisplayName, _T("Microsoft Exchange"));
	_tcscpy(g_instanceDef.VolumeName, _T(""));
	
	memset(&g_RTSData, 0, sizeof(g_RTSData));
	g_storageInfo.RTSData = &g_RTSData;

	g_storageInfo.MaxThreads = MAX_REALTIME_THREADS;

	rv = g_pSSFunctions->Open((HKEY) HKEY_VP_STORAGE_ROOT, szReg_Key_Storage_Exchange, &hkStorageRoot);
	if (rv != ERROR_SUCCESS)
		return rv;
	g_debug = g_pSSFunctions->GetVal(hkStorageRoot, _T("Debug"), 0);

	rv = g_pSSFunctions->Open(hkStorageRoot, szReg_Key_Storage_RealTime, &g_storageInfo.hRTSConfigKey);
	g_pSSFunctions->Close(hkStorageRoot);
	if (rv != ERROR_SUCCESS)
		return rv;

	// Create all communication objects
	rv = CreateCommunicationObjects();
	if (rv != ERROR_SUCCESS)
		{
		g_pSSFunctions->Close(g_storageInfo.hRTSConfigKey);
		g_storageInfo.hRTSConfigKey = 0;
		return rv;
		}

	// Load all realtime configuration data
	rv = MEC_ReloadRTSConfig();
	if(rv != ERROR_SUCCESS)
		{
		CloseCommunicationObjects();
		g_pSSFunctions->Close(g_storageInfo.hRTSConfigKey);
		g_storageInfo.hRTSConfigKey = 0;
		return rv;
		}
	
	if (!StartSERT())
		{
		CloseCommunicationObjects();
		g_pSSFunctions->Close(g_storageInfo.hRTSConfigKey);
		g_storageInfo.hRTSConfigKey = 0;
		return ERROR_GENERAL;
		}

	g_pMemFile->cStorages++;

	if(g_pMemFile->cSERTWatch)
		{
		// check to see if the RTS thread count is nonzero.
		// If it is, this is probably an error condition which
		// we need to recover from. Reset the thread count to zero.
		g_pMemFile->cSERTWatch = 0;
		}

	DebugOut(SM_GENERAL, "MS Exchange storage ext initialized");

	return ERROR_SUCCESS;
} // MEC_StorageInit()

/* end source file */