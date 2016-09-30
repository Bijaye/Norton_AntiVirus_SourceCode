//************************************************************************
//
// $Header:   S:/NAVEX/VCS/olewrite.cpv   1.7   26 Dec 1996 15:23:00   AOONWAL  $
//
// Description:
//      Contains OLE 2 Storage Object write function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/olewrite.cpv  $
// 
//    Rev 1.7   26 Dec 1996 15:23:00   AOONWAL
// No change.
// 
//    Rev 1.6   02 Dec 1996 14:00:54   AOONWAL
// No change.
// 
//    Rev 1.5   29 Oct 1996 12:59:36   AOONWAL
// No change.
// 
//    Rev 1.4   02 Aug 1996 11:31:00   DCHI
// Readded accidentally deleted write.
// 
//    Rev 1.3   02 Aug 1996 10:58:52   DCHI
// Corrected boundary read condition on read of an even multiple of blocks
// above the block size.
// 
//    Rev 1.2   03 May 1996 16:45:10   DCHI
// Added code to make sure # of bytes to write > 0.
// 
//    Rev 1.1   30 Jan 1996 15:43:38   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
// 
//    Rev 1.0   03 Jan 1996 17:14:52   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#include "olewrite.h"

#include "navexshr.h"

//********************************************************************
//
// Function:
//	WORD OLESeekWrite512()
//
// Description:
//	Seeks to dwOffset within the given stream and writes
//	wBytesToWrite bytes from the write buffer.
//
// Returns:
//	wBytesToWrite	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekWrite512
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
)
{
	DWORD 	dwBegBlk, dwEndBlk, dwCurBlk;
	WORD	wBytesWrite;
	DWORD	dwBlkFileOffset, dwSeekOffset;

	dwBegBlk = dwOffset / BLK_SIZE;
	dwEndBlk = (dwOffset + wBytesToWrite - 1) / BLK_SIZE;

	wBytesWrite = 0;

    // Write bytes in first block

	if (OLESeekStreamBlk512(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwBegBlk,&dwBlkFileOffset) != dwBegBlk)
		return OLE_ERR_SEEK;

	wBytesWrite = (WORD)(BLK_SIZE - (dwOffset % BLK_SIZE));

	if (wBytesWrite > wBytesToWrite)
		wBytesWrite = wBytesToWrite;

	dwSeekOffset = dwBlkFileOffset + dwOffset % BLK_SIZE;
	if (lpCallBack->FileSeek(pOLEFile->hFile,
		dwSeekOffset,SEEK_SET) != dwSeekOffset)
	{
		// Seek failed

		return OLE_ERR_SEEK;
	}

    if (wBytesWrite > 0 &&
        lpCallBack->FileWrite(pOLEFile->hFile,
		lpbBuffer,wBytesWrite) != wBytesWrite)
	{
		// Write failed

		return OLE_ERR_WRITE;
	}

    // Return immediately if there are no more bytes to write

	if (dwBegBlk == dwEndBlk)
		return wBytesToWrite;

	lpbBuffer += wBytesWrite;

    // Now write middle blocks

	for (dwCurBlk=dwBegBlk+1;dwCurBlk<dwEndBlk;dwCurBlk++)
	{
		if (OLESeekStreamBlk512(lpCallBack,
			pOLEFile,psiStreamInfo,
			dwCurBlk,&dwBlkFileOffset) != dwCurBlk)
			return OLE_ERR_SEEK;

		if (lpCallBack->FileSeek(pOLEFile->hFile,
			dwBlkFileOffset,SEEK_SET) != dwBlkFileOffset)
		{
			// Seek failed

			return OLE_ERR_SEEK;
		}

		if (lpCallBack->FileWrite(pOLEFile->hFile,
			lpbBuffer,BLK_SIZE) != BLK_SIZE)
		{
			// Write failed

			return OLE_ERR_WRITE;
		}

		lpbBuffer += BLK_SIZE;
	}

	// Write last block

	if (OLESeekStreamBlk512(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwEndBlk,&dwBlkFileOffset) != dwEndBlk)
		return OLE_ERR_SEEK;

	if (lpCallBack->FileSeek(pOLEFile->hFile,
		dwBlkFileOffset,SEEK_SET) != dwBlkFileOffset)
	{
		// Seek failed

		return OLE_ERR_SEEK;
	}

	wBytesWrite = (WORD)((dwOffset + wBytesToWrite) % BLK_SIZE);

    // Take care of boundary condition

    if (wBytesWrite == 0)
        wBytesWrite = BLK_SIZE;

    if (lpCallBack->FileWrite(pOLEFile->hFile,lpbBuffer,wBytesWrite) !=
        wBytesWrite)
    {
        // Write failed

        return OLE_ERR_WRITE;
    }

	return (wBytesToWrite);
}


//********************************************************************
//
// Function:
//	WORD OLESeekWrite64()
//
// Description:
//	Seeks to dwOffset within the given stream and writes
//	wBytesToWrites bytes from the write buffer.  This function is
//	used for streams stored within the Stream64 stream.
//
// Returns:
//	wBytesToRead	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekWrite64
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	LPSTREAM_INFO_T	psiStreamInfo,			// Stream info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
)
{
	DWORD 	dwBegBlk, dwEndBlk, dwCurBlk;
	WORD	wBytesWrite;
	DWORD	dwBlkStreamOffset;

	dwBegBlk = dwOffset / BLK64_SIZE;
	dwEndBlk = (dwOffset + wBytesToWrite - 1) / BLK64_SIZE;

	wBytesWrite = 0;

	// Read bytes in first subblock

	if (OLESeekStreamBlk64(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwBegBlk,&dwBlkStreamOffset) != dwBegBlk)
		return OLE_ERR_SEEK;

	wBytesWrite = (WORD)(BLK64_SIZE - (dwOffset % BLK64_SIZE));

	if (wBytesWrite > wBytesToWrite)
		wBytesWrite = wBytesToWrite;

	if (OLESeekWrite512(lpCallBack,
		pOLEFile,&(pOLEFile->siBlk64Stream),
		dwBlkStreamOffset + dwOffset % BLK64_SIZE,
		lpbBuffer,wBytesWrite) != wBytesWrite)
		return OLE_ERR_WRITE;

	// Return immediately if there are no more bytes to write

	if (dwBegBlk == dwEndBlk)
		return wBytesToWrite;

	lpbBuffer += wBytesWrite;

	// Now write middle blocks

	for (dwCurBlk=dwBegBlk+1;dwCurBlk<dwEndBlk;dwCurBlk++)
	{
		if (OLESeekStreamBlk64(lpCallBack,
			pOLEFile,psiStreamInfo,
			dwCurBlk,&dwBlkStreamOffset) != dwCurBlk)
			return OLE_ERR_SEEK;

		if (OLESeekWrite512(lpCallBack,
			pOLEFile,&(pOLEFile->siBlk64Stream),
			dwBlkStreamOffset,lpbBuffer,BLK64_SIZE) != BLK64_SIZE)
			return OLE_ERR_WRITE;

		lpbBuffer += BLK64_SIZE;
	}

	// Write last block

	if (OLESeekStreamBlk64(lpCallBack,
		pOLEFile,psiStreamInfo,
		dwEndBlk,&dwBlkStreamOffset) != dwEndBlk)
		return OLE_ERR_SEEK;

	wBytesWrite = (WORD)((dwOffset + wBytesToWrite) % BLK64_SIZE);

    // Take care of boundary condition

    if (wBytesWrite == 0)
        wBytesWrite = BLK64_SIZE;

	if (OLESeekWrite512(lpCallBack,
		pOLEFile,&(pOLEFile->siBlk64Stream),
		dwBlkStreamOffset,lpbBuffer,wBytesWrite) != wBytesWrite)
		return OLE_ERR_WRITE;

	return (wBytesToWrite);
}


//********************************************************************
//
// Function:
//	WORD OLESeekWrite()
//
// Description:
//	Seeks to dwOffset within the given stream and writes
//	wBytesToWrites bytes from the write buffer.
//
// Returns:
//	wBytesToRead	on success
//	OLE_ERR_???		on error
//
//********************************************************************

WORD OLESeekWrite
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
	LPOLE_FILE_T	pOLEFile,				// OLE file info
	DWORD 			dwOffset,				// Write offset in stream
	LPBYTE 			lpbBuffer,				// Write buffer
	WORD 			wBytesToWrite			// Number of bytes to write
)
{
	if (pOLEFile->dwBATType == BAT512)
	{
		return OLESeekWrite512(lpCallBack,
			pOLEFile,&(pOLEFile->siStreamInfo),
			dwOffset,lpbBuffer,wBytesToWrite);
	}
	else
	if (pOLEFile->dwBATType == BAT64)
	{
		return OLESeekWrite64(lpCallBack,
			pOLEFile,&(pOLEFile->siStreamInfo),
			dwOffset,lpbBuffer,wBytesToWrite);
	}

	return OLE_ERR_SEEK;
}

#endif  // #ifndef SYM_NLM

