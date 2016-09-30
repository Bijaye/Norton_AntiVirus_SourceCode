// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "fileinfo.h"
#include "debug.h"

// Global data

HANDLE		g_hBufferPoolSema = NULL;
HANDLE		g_hBufferPoolMutex = NULL;
HANDLE		*g_phFileScannedEvents;
FILE_BUFFER	*g_pFTBSBuf;


/*===========================================================================*/
// FileInfo buffer functions
/*===========================================================================*/

/*---------------------------------------------------------------------------*/

//	ReleaseFileInfoBlock marks the file info block free.

void ReleaseFileInfoBlock(PFILE_INFO pFileInfo)
{
	pFileInfo->state = BUFFER_FREE;
}

/*---------------------------------------------------------------------------*/

//	GetAvailableFileInfoBlock finds the first available file info block
//	in the file info buffer pool.  If no blocks are available we return NULL.
//
//	Returns:
//		A pointer to a FILE_INFO block or NULL if none are available.

PFILE_INFO GetAvailableFileInfoBlock()
{
    DWORD		first;
	PFILE_INFO  pFileInfo = NULL;
	PFILE_INFO  pTempFileInfo;

    if(WaitForSingleObject(g_hBufferPoolMutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0)
	{
		first = g_pFTBSBuf->producerIndex % FILE_INFO_ENTRIES;
    
		do 
		{
			pTempFileInfo = &g_pFTBSBuf->buf[g_pFTBSBuf->producerIndex % FILE_INFO_ENTRIES];
        
			if (pTempFileInfo->state == BUFFER_FREE) 
			{
                // Reset memory block
                memset(pTempFileInfo, 0, sizeof(FILE_INFO));

                // Set the state of the buffer to be in use.
				pTempFileInfo->state = BUFFER_IN_USE;

				// and set the error code to success
				//pTempFileInfo->error = 0;	// not needed since we zero out the memory above
				
				// This assignment allows us to uniquely identify a scan
				// request in the case of a timeout abandonment.
				pTempFileInfo->bufferIndex = g_pFTBSBuf->producerIndex;

				pFileInfo = pTempFileInfo;

	            // Increment to the next block
				g_pFTBSBuf->producerIndex++;
		
				// We don't allow the producerIndex, thus the bufferIndex,
				// to equal 0 since 0 is the value used to indicate a timeout
				if(g_pFTBSBuf->producerIndex == 0)
					g_pFTBSBuf->producerIndex = 1;

				break;
			}
		
            // Increment to the next block
			g_pFTBSBuf->producerIndex++;

			// We don't allow the producerIndex, thus the bufferIndex,
			// to equal 0 since 0 is the value used to indicate a timeout
			if(g_pFTBSBuf->producerIndex == 0)
				g_pFTBSBuf->producerIndex = 1;

		} while (g_pFTBSBuf->producerIndex % FILE_INFO_ENTRIES != first);

		ReleaseMutex(g_hBufferPoolMutex);
	}

    return pFileInfo;
}

/*---------------------------------------------------------------------------*/

//	GetWaitingFileInfoBlock searches the buffer pool for a FILE_INFO block
//	which describes a file that needs to be scanned by the storage extension.
//	This function is called by the storage extension inside of the function
//	BeginRTSWatch when the file scanning semaphore is signaled.
//
//	Returns:
//		A pointer to the first available FILE_INFO buffer requiring scanning,
//		or NULL if none are present.

PFILE_INFO GetWaitingFileInfoBlock()
{
    DWORD		first;
	PFILE_INFO  pFileInfo = NULL;
	PFILE_INFO  pTempFileInfo;

    if(WaitForSingleObject(g_hBufferPoolMutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0)
	{
		first = g_pFTBSBuf->consumerIndex % FILE_INFO_ENTRIES;
    
		do 
		{
			pTempFileInfo = &g_pFTBSBuf->buf[g_pFTBSBuf->consumerIndex % FILE_INFO_ENTRIES];
        
			if (pTempFileInfo->state == BUFFER_IN_USE) 
			{
				pTempFileInfo->state = BUFFER_OWNED_BY_SCAN;
				
				pFileInfo = pTempFileInfo;
				g_pFTBSBuf->consumerIndex++;
				break;
			}
		
			g_pFTBSBuf->consumerIndex++;
		
		} while (g_pFTBSBuf->consumerIndex % FILE_INFO_ENTRIES != first);

		ReleaseMutex(g_hBufferPoolMutex);
	}

    return pFileInfo;
}

/*---------------------------------------------------------------------------*/

//	GetFileInfoBlockFromContext locates the FILE_INFO block which 
//	matches the bufferIndex passed in pContext.

//	The pContext parameter is actually a bufferIndex which
//	can be used to find the FILE_INFO block with a mod operation.
//	If the bufferIndex stored in the correponding FILE_INFO structure
//	doesn't match, the buffer is being reused and the hook has 
//	probably abandoned the wait.
//
//	Parameters:
//		pContext	A DWORD bufferIndex
//	
//	Returns:
//		A pointer to the FILE_INFO buffer corresponding to pContext
//		if it is valid, otherwise NULL.

PFILE_INFO GetFileInfoBlockFromContext(void * pContext)
{
	DWORD bufferIndex = (DWORD)pContext;
	PFILE_INFO pFileInfo = &g_pFTBSBuf->buf[bufferIndex % FILE_INFO_ENTRIES];

	// Check the context index to see if it matches 
	// the bufferIndex in the file info and that  
	// the buffer is still owned by the scan context
	// If the buffer is not owned by the scan context, 
	// the mail context may have timed out.

	if(bufferIndex != pFileInfo->bufferIndex 
		|| pFileInfo->state != BUFFER_OWNED_BY_SCAN)
		pFileInfo = NULL;

	return pFileInfo;
}

/*--- end of source ---*/
