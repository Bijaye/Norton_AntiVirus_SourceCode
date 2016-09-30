// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "initipc.h"
#include "fileinfo.h"
#include "debug.h"
#include "SymSaferStrings.h"
#include "ccEveryoneDacl.h"

// Local data

static HANDLE	hFileMap = NULL;


/*===========================================================================*/
// Initialization functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

// DeInitIPC deinitializes the IPC between the notes hooks and the storage extension.
//	
//	Returns:
//		ERROR_SUCCESS for success

DWORD DeInitIPC()
{
	DWORD	i;

	if(g_phFileScannedEvents)
		{
		for(i=0; i<FILE_INFO_ENTRIES;i++)
			{
			if(g_phFileScannedEvents[i])
				CloseHandle(g_phFileScannedEvents[i]);
			}

		free(g_phFileScannedEvents);
		}

	if(g_hBufferPoolSema)
		{
		CloseHandle(g_hBufferPoolSema);
		g_hBufferPoolSema = NULL;
		}

	if(g_pFTBSBuf)
		{
		UnmapViewOfFile(g_pFTBSBuf);  
		g_pFTBSBuf = NULL;
		}

	if(hFileMap)
		{
		CloseHandle(hFileMap);
		hFileMap = NULL;
		}

	if(g_hBufferPoolMutex)
		{
		CloseHandle(g_hBufferPoolMutex);
		g_hBufferPoolMutex = NULL;
		}

	return ERROR_SUCCESS;
}

/*---------------------------------------------------------------------------*/

//	InitIPC initializes the IPC between the notes hook and the 
//	storage extension.
//	It creates (or retrieves handles to)
//	1.  The buffer pool mutex for all file info blocks.
//	2.	The memory mapped file to share between all modules.
//	3.  The storage extension file scanning semaphore.
//	4.	An event for each file info block to signal the notes 
//		hook of scan completion.
//		
//	Returns:
//		ERROR_SUCCESS or the error returned from GetLastError.

DWORD InitIPC()
{
	int		i;
	HANDLE	hEvent;
	TCHAR	szEventName[48];
	DWORD	result;

	PSECURITY_DESCRIPTOR    pSD;
	SECURITY_ATTRIBUTES     sa;

	// Get an empty security desriptor
	pSD = (PSECURITY_DESCRIPTOR) LocalAlloc(LPTR, sizeof(SECURITY_DESCRIPTOR));

	if (pSD == NULL)
		return GetLastError();

	if (!InitializeSecurityDescriptor(pSD, SECURITY_DESCRIPTOR_REVISION)) 
		{
		result = GetLastError();
		LocalFree((HLOCAL)pSD);
		return result;
		}

		SAV::CEveryoneDACL nullDacl;
		PACL pDacl= NULL;

	    OSVERSIONINFO VersionInfo;
		ZeroMemory(&VersionInfo, sizeof(VersionInfo));
		VersionInfo.dwOSVersionInfoSize = sizeof(VersionInfo);
		GetVersionEx(&VersionInfo);
		if (VersionInfo.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			if (nullDacl.Create() == TRUE)
				pDacl= nullDacl;
			else
			{
				result = GetLastError();
				LocalFree((HLOCAL)pSD);
				return result;
			}
		}

	// Add a NULL DACL to the security descriptor.
	if (!SetSecurityDescriptorDacl(pSD, TRUE, pDacl, FALSE))
		{
		result = GetLastError();
		LocalFree((HLOCAL)pSD);
		return result;
		}

	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE; 
	
	// Create the Buffer pool mutex
	g_hBufferPoolMutex = CreateMutex(&sa, FALSE, LNEXT_MUTEX); 
	if(!g_hBufferPoolMutex)
		{
		// now that we're using a non-null security descriptor, CreateMutex will fail
		// if the mutex was already created by another process, so we need to try open also
		g_hBufferPoolMutex = OpenMutex( SYNCHRONIZE|MUTEX_MODIFY_STATE, true, LNEXT_MUTEX);

		if (!g_hBufferPoolMutex)
			{
			result = GetLastError();
			DebugOut(SM_ERROR, "ERROR: NLNVP: Attempt to create mail buffer Mutex failed.");
			LocalFree((HLOCAL)pSD);
			DeInitIPC();
			return result;
			}
		}

	// Create the file mapping
	hFileMap = CreateFileMapping(
						(HANDLE)0xffffffff, 
						&sa, 
						PAGE_READWRITE,
						0, 
						sizeof(FILE_BUFFER),
						EXTENSION_MAP_FILE);

	if(!hFileMap)
		{
		// now that we're using a non-null security descriptor, CreateFileMapping will fail
		// if the object was already created by another process, so we need to try open also
		hFileMap = OpenFileMapping(FILE_MAP_READ|FILE_MAP_WRITE, true, EXTENSION_MAP_FILE);

		if(!hFileMap)
			{
			result = GetLastError();
			DebugOut(SM_ERROR, "ERROR: NLNVP: Attempt to create memory mapped file failed.");
			LocalFree((HLOCAL)pSD);
			DeInitIPC();
			return result;
			}
		}
		
	//	Attempt to create a view of the memory mapped file
	g_pFTBSBuf = (FILE_BUFFER*)MapViewOfFile(hFileMap, FILE_MAP_WRITE, 0, 0, 0);
	if(!g_pFTBSBuf)
		{
		result = GetLastError();
		DebugOut(SM_ERROR, "ERROR: NLNVP: Attempt to create memory mapped file view failed.");
		LocalFree((HLOCAL)pSD);
		DeInitIPC();
		return result;
		}

	// Create the storage extension notification semaphore.
	g_hBufferPoolSema = CreateSemaphore(&sa, 0, FILE_INFO_ENTRIES, LNEXT_SEMA); 
	if (!g_hBufferPoolSema)
		{
		// now that we're using a non-null security descriptor, CreateSemaphore will fail
		// if the object was already created by another process, so we need to try open also
		g_hBufferPoolSema = OpenSemaphore( SYNCHRONIZE|SEMAPHORE_MODIFY_STATE, true, LNEXT_SEMA);

		if (!g_hBufferPoolSema)
			{
			result = GetLastError();
			DebugOut(SM_ERROR, "ERROR: NLNVP: Attempt to create mail buffer Semaphore failed.");
			LocalFree((HLOCAL)pSD);
			DeInitIPC();
			return result;
			}
		}
	
	// Create the scanned file notification events

	g_phFileScannedEvents = (HANDLE*)calloc(FILE_INFO_ENTRIES, sizeof(HANDLE));
	if(!g_phFileScannedEvents)
		{
		DebugOut(SM_ERROR, "ERROR: NLNVP: Attempt to allocate event handle buffer failed.");
		LocalFree((HLOCAL)pSD);
		DeInitIPC();
		return ERROR_OUTOFMEMORY;
		}

	for(i=0; i<FILE_INFO_ENTRIES; i++)
	{
		sssnprintf(szEventName, sizeof(szEventName), LNEXT_EVENT, i);
	
		hEvent = CreateEvent(&sa, FALSE, FALSE, szEventName); 
		if(!hEvent)
			{
			// now that we're using a non-null security descriptor, CreateEvent will fail
			// if the object was already created by another process, so we need to try open also
			hEvent = OpenEvent( EVENT_MODIFY_STATE | SYNCHRONIZE, true, szEventName);

			if(!hEvent)
				{
				result = GetLastError();
				DebugOut(SM_ERROR, "ERROR: NLNVP: Attempt to create scanned file event failed.");
				LocalFree((HLOCAL)pSD);
				DeInitIPC();
				return result;
				}
			}
		
		g_phFileScannedEvents[i] = hEvent;
	}

	LocalFree((HLOCAL)pSD);
	return ERROR_SUCCESS;
}

/*--- end of source ---*/
