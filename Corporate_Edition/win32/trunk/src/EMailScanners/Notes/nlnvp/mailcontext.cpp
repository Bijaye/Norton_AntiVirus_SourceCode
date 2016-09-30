// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
// mailcontext.c  Contains routines specific to the mail context.

#include <windows.h>
#include "SavrtModuleInterface.h"
#include "nlnvp.h"
#include "SymSaferStrings.h"

struct block_list
{
	BLOCKID	blockID;
	BLOCKID blockValue;
	struct block_list *next;
};

typedef struct block_list	BLOCK_ELEMENT;

/*---------------------------------------------------------------------------*/

//	_isExtWanted checks to see if the file name extension
//	matches an extension in the list.
//
//	Parameters:
//		list    	[in] A pointer to a buffer which contains a list of
//					comma separated file extensions (with no .'s)
//		ext     	[in] A pointer to a buffer which contains a file
//					name ext optionally including the '.' char
//
//	Returns:
//		TRUE if the extension is in the list, otherwise FALSE.

BOOL _isExtWanted(LPCTSTR list, LPCTSTR ext)
{
	int pLen = 0, rLen = 0;
	LPCTSTR p1, p2, q, r1, r2;
	TCHAR Ext[_MAX_EXT];

	if (ext[0] == _T('.'))
		ext = _tcsinc(ext);	// skip leading dot

    ssStrnCpy(Ext, ext, sizeof(Ext));
	_tcsupr(Ext);

#ifndef LINUX
    // For non-Linux platforms, making assumption that filesystem we're dealing
    // with is case-insensitive. Not quite true, but this is the historical
    // behavior for NLM and Windows flavors of PSCAN.
    char szExtList[EXT_LIST_SIZE];

    ssStrnCpy( szExtList, list, sizeof(szExtList) );
    _tcsupr( szExtList );
    list = szExtList;
#endif

	for (p1 = p2 = list; p1; p1 = (*q ? _tcsinc(q) : NULL))
    {
		q = _tcschr(p1, ',');
		if (!q)
			q = p1 + _tcslen(p1);

		for (r1 = r2 = Ext; ;
			p2 = _tcsinc(p1), pLen = p2 - p1, p1 = p2,
			r2 = _tcsinc(r1), rLen = r2 - r1, r1 = r2)
        {
			if (p1 >= q)
            {
				if (!(*r1))
					return TRUE;
				break;
            }

			if (!(*r1))
				break;		// End if Ext - no match here

			if (*p1 == '?')
				continue;	// i.e., '?' matches ANY char, even wchars

			if (pLen != rLen || *p1 != *r1)
				break;		// Dosen't match

			//	This assumes ONLY 1 or 2 byte characters!
			if (pLen > 1 && *(p1 - 1) != *(r1 - 1))
				break;		// PrevChar doesn't match
        }
    }

	return FALSE;
}

/*---------------------------------------------------------------------------*/

//	ExtensionRequiresScan checks to see if the file name extension
//	indicates that the file should be scanned.
//
//	Parameters:
//		pszFileExt	[in] A pointer to a buffer which contains a file
//					name ext including the '.' char
//		pExtList	[in] A pointer to a buffer which contains a list of
//					comma separated file extensions (with no .'s)
//
//	Returns:
//		TRUE if the extension requires scanning, otherwise FALSE.

BOOL ExtensionRequiresScan(LPTSTR pszFileExt)
{
	BOOL result = TRUE;

	if(WaitForSingleObject(g_hBufferPoolMutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0)
		{
		// if we're scanning all files,
		// and the extension is empty, scan it!
		if ((g_pFTBSBuf->regInfo.ScanFilesType != 1) && (_tcslen(pszFileExt) == 0))
			result = TRUE;
		// check to see if we should skip the ext
		// note we process skip exts no matter which scan mode we are in!
		else if (_isExtWanted(g_pFTBSBuf->regInfo.SkipExts, pszFileExt))
			result = FALSE;
		else if (g_pFTBSBuf->regInfo.ScanFilesType == 1)
			{
			// we're scanning by ext, so check the ext list
			result = _isExtWanted(g_pFTBSBuf->regInfo.Exts, pszFileExt);

			// if not in the list, is it in the zip ext list?
			if(!result && g_pFTBSBuf->regInfo.bZipFile)
				result = _isExtWanted(g_pFTBSBuf->regInfo.ZipExts, pszFileExt);
			}

		ReleaseMutex(g_hBufferPoolMutex);
		}

	return result;
}


/*---------------------------------------------------------------------------*/

// DumpMailFieldsFromNote is used to display all fields in the note
// for debugging purposes.

/*
#ifdef DEBUG
STATUS DumpMailFieldsFromNote(DBHANDLE  hDB, HANDLE hNote)
{
	// note that the Notes API does not use unicode

	char		szItemBuffer[MAXPATH];
	char		szFileName[MAXPATH];
    STATUS      error = NOERROR;

    if(!NSFItemIsPresent(hNote,										// NOTESAPI
					ITEM_NAME_ATTACHMENT,
					(WORD)strlen(ITEM_NAME_ATTACHMENT)))
		return NOERROR;

	if(NSFItemIsPresent(hNote,										// NOTESAPI
						MAIL_RECIPIENTS_ITEM,
						(WORD)strlen(MAIL_RECIPIENTS_ITEM))
		&& NSFItemGetText(hNote,									// NOTESAPI
						MAIL_RECIPIENTS_ITEM,
						szItemBuffer,
						MAXPATH))
	{
		sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "Item '%s' exists with value %s.",MAIL_RECIPIENTS_ITEM, szItemBuffer);
		DebugOut(SM_GENERAL, g_szTextBuffer);
	}

	if(NSFItemIsPresent(hNote,										// NOTESAPI
					MAIL_SENDTO_ITEM,
					(WORD)strlen(MAIL_SENDTO_ITEM))
		&& NSFItemGetText(hNote,									// NOTESAPI
						MAIL_SENDTO_ITEM,
						szItemBuffer,
						MAXWORD))
	{
		sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "Item '%s' exists with value %s.",MAIL_SENDTO_ITEM, szItemBuffer);
    	DebugOut(SM_GENERAL, g_szTextBuffer);
	}

	if(NSFItemIsPresent(hNote,										// NOTESAPI
					MAIL_FROM_ITEM,
					(WORD)strlen(MAIL_FROM_ITEM))
		&& NSFItemGetText(hNote,									// NOTESAPI
					MAIL_FROM_ITEM,
					szItemBuffer,
					MAXPATH))
	{
		sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "Item '%s' exists with value %s.",MAIL_FROM_ITEM, szItemBuffer);
		DebugOut(SM_GENERAL, g_szTextBuffer);
	}

    if(NSFItemIsPresent(hNote, MAIL_SUBJECT_ITEM,					// NOTESAPI
					(WORD)strlen(MAIL_SUBJECT_ITEM))
		&& NSFItemGetText(hNote, MAIL_SUBJECT_ITEM,					// NOTESAPI
					szItemBuffer,MAXPATH))
	{
		sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "Item '%s' exists with value %s.",MAIL_SUBJECT_ITEM, szItemBuffer);
		DebugOut(SM_GENERAL, g_szTextBuffer);
	}
	
	if(NSFItemIsPresent(hNote, ITEM_NAME_ATTACHMENT,				// NOTESAPI
				(WORD)strlen(ITEM_NAME_ATTACHMENT)))
    {
		BLOCKID	blockID;
		WORD	dataType;
		BLOCKID	blockValue;
		DWORD	retValueLength;
		BYTE	* pData;
		FILEOBJECT	* pFile;

		sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "Item Attachment exists with value %s.", szItemBuffer);
		DebugOut(SM_GENERAL, g_szTextBuffer);

		NSFItemInfo(hNote,											// NOTESAPI
					ITEM_NAME_ATTACHMENT, 
					(WORD)strlen(ITEM_NAME_ATTACHMENT),
					&blockID,
					&dataType,
					&blockValue,
					&retValueLength);
	
		while (1)
		{
			pData = OSLockBlock(BYTE, blockValue);					// NOTESAPI

			if(!pData)
				return NOERROR;

			// step over the data type word to point to the file data
			pFile = (FILEOBJECT *)((BYTE *)pData + sizeof(WORD));

			// Print out the file name

			memcpy(szFileName, pFile + 1, pFile->FileNameLength);
			szFileName[pFile->FileNameLength] = 0;
			sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "Attachment file name %s", szFileName);
			DebugOut(SM_GENERAL, g_szTextBuffer);
			
			sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "\tHost Type %d", pFile->HostType);
			DebugOut(SM_GENERAL, g_szTextBuffer);
			
			sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "\tCompression Type %d", pFile->CompressionType);
			DebugOut(SM_GENERAL, g_szTextBuffer);

			sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "\tFile Attributes %d", pFile->FileAttributes);
			DebugOut(SM_GENERAL, g_szTextBuffer);

			sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "\tFlags %d", pFile->Flags);
			DebugOut(SM_GENERAL, g_szTextBuffer);

			sssnprintf(g_szTextBuffer, sizeof(g_szTextBuffer), "\tFile size: %lu", pFile->FileSize);
			DebugOut(SM_GENERAL, g_szTextBuffer);

			OSUnlockBlock(blockValue);								// NOTESAPI

			error = NSFItemInfoNext(								// NOTESAPI
						hNote, 
						blockID, 
						ITEM_NAME_ATTACHMENT, 
						(WORD)_tcslen(ITEM_NAME_ATTACHMENT),
						&blockID,
						&dataType,
						&blockValue,
						&retValueLength);

			if (ERR(error) == ERR_ITEM_NOT_FOUND)
				break;
		}
	}

	if(NSFItemIsPresent(hNote, MAIL_BODY_ITEM,						// NOTESAPI
					(WORD)_tcslen(MAIL_BODY_ITEM)))
	{
		DWORD fileSize;

		MailGetMessageBodyComposite(								// NOTESAPI
						hNote,
						NULL,
						"C:\\temp\\body.cpd",
						&fileSize);
	}

    return(NOERROR);
}
#endif // DEBUG
*/

/*---------------------------------------------------------------------------*/

//	ItemScanCallback is called to display more detailed note information for 
//	debugging purposes.

/*
#ifdef DEBUG
STATUS LNPUBLIC ItemScanCallback
			(WORD unused,
			WORD item_flags,
			LPTSTR name_ptr,
			WORD name_len,
			VOID far *item_value,
			DWORD item_value_len,
			VOID far *note_handle)
{
	TCHAR		szText[256];
	TCHAR		szItemName[256];
	TCHAR		szItem[256];

	if (name_len >= 256)
		name_len = 255;
    _tcsncpy(szItemName, name_ptr, name_len);
	szItemName[name_len] = 0;

	if(item_flags & ITEM_NAMES)
		{
		if (item_value_len >= 256)
			item_value_len = 255;
		_tcsncpy(szItem, item_value, item_value_len);
		szItem[item_value_len] = 0;
		sssnprintf(szText, sizeof(szText), "\tItem '%s' exists with value: %s.", szItemName, szItem);
		}
	else
		sssnprintf(szText, sizeof(szText), "\tItem '%s' exists with value: 0x%x.", szItemName, item_value);

	DebugOut(SM_GENERAL, szText);

	return ERROR_SUCCESS;
}
#endif // DEBUG
*/

/*---------------------------------------------------------------------------*/

//	AddFileInfoToList copies a FILE_INFO structure and adds it to the
//	FILE_INFO list pInfectionList.  This function is used when an infection
//	is detected for one or more file attachments.
//
//	Parameters:
//		pFileInfo	a pointer to a FILE_INFO block to be copied.
//		pInfectionList a pointer to FILE_INFO linked list of infection info.
//
//	Returns:
//		ERROR_SUCCESS for success
//		ERROR_OUTOFMEMORY for memory allocation errors
//		-1 if not added to infection list

DWORD AddFileInfoToList(PFILE_INFO pFileInfo, PFILE_INFO * pInfectionList)
{
	PFILE_INFO 		pNewFileInfo;
	DWORD			result = 0xFFFF;	// need to indicate if NOT added to list...
	
	//	If the file is infected, Store the FILE_INFO 
	//	in the local infected list.

	if(pFileInfo->action)
	{
		if (pNewFileInfo = (PFILE_INFO)malloc(sizeof(FILE_INFO)))
			{
			*pNewFileInfo = *pFileInfo;
			pNewFileInfo->pNext = *pInfectionList;
			*pInfectionList = pNewFileInfo;

			result = ERROR_SUCCESS;
			}
		else
			result = ERROR_OUTOFMEMORY;
	}
	
	return result;
}

/*---------------------------------------------------------------------------*/

//	ProcessHookedNote is called by the Notes Database hook when a note is hooked
//	for processing.  ProcessHooked note checks whether the note is signed or sealed
//	and signals the storage extension in another process space that a node is ready
//	to be scanned.
//
//	Parameters:
//		hDB			[in] A database handle to the note that has been hooked.
//		hNote		[in] A handle to a note that has been hooked.
//		szUserName	[in] Pointer to the logged-in user name
//
//	Returns:
//		TRUE if an update is required.

BOOL ProcessHookedNote(DBHANDLE hDB, NOTEHANDLE hNote, char *szUserName)
{	
	BLOCKID		blockID;
	BLOCKID		blockValue;
	WORD		dataType;
	DWORD		signal;
	DWORD		retValueLength;
	BYTE		* pData;
	FILEOBJECT	* pFile;
	STATUS		status;
	PFILE_INFO	pFileInfo;
	PFILE_INFO	pInfectionList = NULL;
	BOOL		bIsSigned;
	BOOL		bIsSealed;
	ENCRYPTION_KEY encryptKey = {0};
	BLOCK_ELEMENT  *head = NULL;
	BLOCK_ELEMENT  *tail = NULL;
	BLOCK_ELEMENT  *temp = NULL;
		
	if(!g_bMailContextRegistered || !g_pFTBSBuf->realTimeThreads || !g_pFTBSBuf->regInfo.bOnOff)
		{
		DebugOut(SM_WARNING, "NLNVP: ProcessHookedNote: note skipped, mail context [%d], threads [%d], OnOff [%d]", g_bMailContextRegistered, g_pFTBSBuf->realTimeThreads, g_pFTBSBuf->regInfo.bOnOff);
		return FALSE;
		}

//	DebugOut(SM_GENERAL, "NLNVP: ProcessHookedNote: processing note...");

#ifdef DEBUG
	//NSFItemScan(hNote, ItemScanCallback, &hNote);					// NOTESAPI
	//DumpMailFieldsFromNote(hDB, hNote);
#endif
	
	//	If the note has no attachments, return.
	if(!NSFItemIsPresent(hNote, ITEM_NAME_ATTACHMENT,				// NOTESAPI
			(WORD)_tcslen(ITEM_NAME_ATTACHMENT)))
		{
//		DebugOut(SM_GENERAL, "NLNVP: ProcessHookedNote: no attachments, note skipped");
		return FALSE;
		}

	// Get note encryption key if necessary.
	
	NSFNoteIsSignedOrSealed(hNote, &bIsSigned, &bIsSealed);			// NOTESAPI
	
	if(bIsSealed && NSFNoteDecrypt(hNote, 0, &encryptKey))			// NOTESAPI
		{
		DebugOut(SM_GENERAL, "NLNVP: note encrypted, note skipped");
		return FALSE;
		}
	
	// Call NSFItemInfo to get first attachment (the check for attachments
	//	was already made above).  We iterate through attachments with NSFItemInfoNext()

	if(NSFItemInfo(	hNote,											// NOTESAPI
					ITEM_NAME_ATTACHMENT, 
					(WORD)_tcslen(ITEM_NAME_ATTACHMENT),
					&blockID,
					&dataType,
					&blockValue,
					&retValueLength))
		{
		// If an error occurs, return ERROR_SUCCESS to allow 
		// a possible note update in the hook driver to complete.
		DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: unable to get attachment");
		return FALSE;
		}

 	// Start a list of all the attachments we need to scan
	head = (BLOCK_ELEMENT*)malloc(sizeof(BLOCK_ELEMENT));
	if (head == NULL)
	{
		DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: out of memory creating list");
		return FALSE;
	}

	head->blockID = blockID;
	head->blockValue = blockValue;
	head->next = NULL;
	tail = head;

	// Go through all the attachments in the note and add them to the list.
	// Don't scan them yet.
	while (1)
	{
		status = NSFItemInfoNext(									// NOTESAPI
					hNote, 
					blockID, 
					ITEM_NAME_ATTACHMENT, 
					(WORD)_tcslen(ITEM_NAME_ATTACHMENT),
					&blockID,
					&dataType,
					&blockValue,
					&retValueLength);

		if (ERR(status) == ERR_ITEM_NOT_FOUND)
		{
			DebugOut(SM_GENERAL, "NLNVP: no more attachments");
			break;
		}
		else if (status) // some other error occured...
		{
			DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: error getting next attachment");
			break;
		}
		else // got one, so add it to the list
		{
			temp = (BLOCK_ELEMENT*)malloc(sizeof(BLOCK_ELEMENT));
			if (temp == NULL)
			{
				DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: out of memory adding to list");
				break;
			}

			temp->blockID = blockID;
			temp->blockValue = blockValue;
			temp->next = NULL;

			tail->next = temp;
			tail = temp;
		}
	}
    // disable AP since we will be extracting files
	SAVRTUnProtectProcess();

	// Go through the list and extract/scan each attachment
	while (head != NULL)
		{
		//	Get a block from the Files To Be Scanned queue  
		if(pFileInfo = GetAvailableFileInfoBlock())
			{
			WORD name_len;
			TCHAR szFileExt[_MAX_EXT];
			TCHAR szLMBCSFileExt[_MAX_EXT];
			
			pData = OSLockBlock(BYTE, head->blockValue);					// NOTESAPI
			if(!pData)
				{
				DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: error locking block");
				goto ErrorLockingBlock;
				}

            // TCashin: Ported CRT fix for defect 1-DU1ND
            //
            // Notes had invalid attachment information on a mail and the following ASSERT0 crashed Notes.
            // In this case, skipping the invalid attachment and prceeding with next attachment

            // ASSERT0(((WORD) *pData) == TYPE_OBJECT, "ProcessHookedNote: Data type invalid!");

            if (((WORD) *pData) != TYPE_OBJECT)
            {
	            DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: Data type invalid!");
	            //so that OSUnlockBlock and ReleaseFileInfoBlock are called.
	            goto ErrorGettingTempDir;
            }

            // Step over the data type word to point to the file data 
            pFile = (FILEOBJECT *)((BYTE *)pData + sizeof(WORD));

            // Notes had invalid attachment information on a mail and the following ASSERT0 crashed 
            // Notes. In this case, skipping the invalid information and prceeding with next attachment

            // ASSERT0(pFile->Header.ObjectType == OBJECT_FILE, "ProcessHookedNote: Object type invalid!");

            if (pFile->Header.ObjectType != OBJECT_FILE)
            {
	            DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: Object type invalid!");
	            goto ErrorGettingTempDir;
            }

            // TCashin: End CRT fix for defect 1-DU1ND

            // Get current PID (for user info)
            pFileInfo->dwPID = GetCurrentProcessId();

			// Get the attachment name
			if ((name_len = pFile->FileNameLength) >= MAX_PATH)
				name_len = MAX_PATH - 1;

			_tcsncpy(pFileInfo->szOEMFileName, (LPTSTR)(pFile + 1), name_len);
			pFileInfo->szOEMFileName[name_len] = 0;

			OSTranslate(OS_TRANSLATE_LMBCS_TO_NATIVE, pFileInfo->szOEMFileName, name_len,	// NOTESAPI
				pFileInfo->szAnsiFileName, sizeof(pFileInfo->szAnsiFileName));

			// get the extention
			_tsplitpath(pFileInfo->szAnsiFileName, NULL, NULL, NULL, szFileExt);

			// get the LMBCS extention
			_tsplitpath(pFileInfo->szOEMFileName, NULL, NULL, NULL, szLMBCSFileExt);

			// Check the extension lists
			if(ExtensionRequiresScan(szFileExt))
				{
				WORD	i;
				WORD	nRecipients;
				TCHAR	UserName[MAX_SENDER];
				DWORD	UserNameLength;
				DWORD	totalLen = 0;
				WORD	length;
				DWORD	remainingSpace;

				// get the temp path
				if(!GetTempPath(MAX_PATH, pFileInfo->ExtractDir))
					{
					DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: error getting temp dir");
					goto ErrorGettingTempDir;
					}

				_tcscat(pFileInfo->ExtractDir, _T("LDVPScan"));

				if(!CreateDirectory(pFileInfo->ExtractDir, NULL))
					{
					int err = GetLastError();
					if (!(err == ERROR_ALREADY_EXISTS))
						{
						DebugOut(SM_ERROR, "ERROR: ProcessHookedNote: unable to create temp dir \"%s\" [%d]", pFileInfo->ExtractDir, err);
						goto ErrorGettingTempDir;
						}
					}
				
				MakeTempFileName(
					pFileInfo->ExtractDir,
					_T("LNA"),
					pFileInfo->bufferIndex,
					szFileExt,
					pFileInfo->szFilePath,
                    sizeof(pFileInfo->szFilePath));

				MakeTempFileName(
					pFileInfo->ExtractDir,
					_T("LNA"),
					pFileInfo->bufferIndex,
					szLMBCSFileExt,
					pFileInfo->szLMBCSFilePath,
                    sizeof(pFileInfo->szLMBCSFilePath));

				// delete any previous file that may be lingering...
				DeleteFile(pFileInfo->szFilePath);

				// get the username
				pFileInfo->szUserName = szUserName;

				// Copy the message subject
				length = NSFItemGetText(hNote, MAIL_SUBJECT_ITEM,	// NOTESAPI
					pFileInfo->szLMBCS, MAX_SUBJECT);

				OSTranslate(OS_TRANSLATE_LMBCS_TO_NATIVE,			// NOTESAPI
					pFileInfo->szLMBCS,
					length,
					pFileInfo->szSubject,
					sizeof(pFileInfo->szSubject));

				// Copy the message sender
				length = NSFItemGetText(hNote, MAIL_FROM_ITEM,		// NOTESAPI
					pFileInfo->szLMBCS, MAX_SENDER);

				OSTranslate(OS_TRANSLATE_LMBCS_TO_NATIVE,			// NOTESAPI
					pFileInfo->szLMBCS,
					length,
					UserName,
					sizeof(UserName));

				StripDirInfo(pFileInfo->szFrom, UserName, MAX_SENDER);

				// Get the SENDTO list
				pFileInfo->szLMBCS[0] = 0;
				nRecipients = NSFItemGetTextListEntries(hNote,		// NOTESAPI
										MAIL_SENDTO_ITEM);

				for (i = 0; i < nRecipients; i++)
					{
					UserNameLength = NSFItemGetTextListEntry(hNote,	// NOTESAPI
										MAIL_SENDTO_ITEM, 
										i, 
										UserName,
										MAX_SENDER-1);

					UserName[UserNameLength] = _T('\0');

					// the number of chars is the buffer size, minus
					// the current length, minus one just in case we
					// add a ';'
					remainingSpace = sizeof(pFileInfo->szLMBCS) - totalLen - 1;

					// this test isn't quite accurate, but close enough.
					// remember that we're potentially stripping info from
					// the UserName string, so it could be the case that
					// UserNameLength is greater than remainingSpace, but
					// the size of the stripped UserName is smaller than
					// the remaining space.  Oh well.
					if (remainingSpace > UserNameLength)
						{
						StripDirInfo(
							pFileInfo->szLMBCS + totalLen,
							UserName, 
							remainingSpace);
						
						if(i < nRecipients - 1)
							_tcscat(pFileInfo->szLMBCS, _T(";"));

						totalLen = _tcslen(pFileInfo->szLMBCS);
						}
					}

				OSTranslate(OS_TRANSLATE_LMBCS_TO_NATIVE,			// NOTESAPI
					pFileInfo->szLMBCS,
					(WORD)_tcslen(pFileInfo->szLMBCS),
					pFileInfo->szTo,
					sizeof(pFileInfo->szTo));

				// Extract file using NSFNoteExtractFile call
				if (status = NSFNoteExtractFile(					// NOTESAPI
								hNote,
								head->blockID,
								pFileInfo->szLMBCSFilePath,
								&encryptKey))
					{
					NOTESMSG(status);
					}
				else
					{
					pFileInfo->bExtracted = TRUE;

					//	Copy the important file info data and the 
					//	attachment path into the FTBS Queue entry.

					pFileInfo->hNote = hNote;
					pFileInfo->blockID = head->blockID;
					pFileInfo->compressionType = pFile->CompressionType;
			
					// We manually reset the event in case a previous 
					// scan context timeouted...
					ResetEvent(g_phFileScannedEvents[pFileInfo->bufferIndex % FILE_INFO_ENTRIES]);

					// signal the scan context that an item is ready.
					if (!ReleaseSemaphore(g_hBufferPoolSema, 1, NULL))
						{
						// error signaling the Notes storage ext scanner
						// this could happen if all the threads of the notes ext
						// have stopped processing requests
						DeleteFile(pFileInfo->szFilePath);
						// we don't really need to set the bufferIndex to 0,
						// but just in case...
						pFileInfo->bufferIndex = 0;
						DebugOut(SM_WARNING, "NLNVP: unable to signal RTVSCAN, skipping attachment \"%s\"", pFileInfo->szAnsiFileName);
						goto ErrorSignalingRTVScan;
						}

					DebugOut(SM_GENERAL, "NLNVP: scanning attachment \"%s\" as \"%s\"", pFileInfo->szAnsiFileName, pFileInfo->szFilePath);

					//	Wait for the scanned file event to be signaled. 
					//	If X seconds passes, allow notes to continue 
									
					signal = WaitForSingleObject(
						g_phFileScannedEvents[pFileInfo->bufferIndex % FILE_INFO_ENTRIES],
						SCAN_TIMEOUT);

					if(signal == WAIT_OBJECT_0)
						{
						// if we get signaled, check for infection
						// if infected, add to infection list, else
						// delete the file
						if (AddFileInfoToList(pFileInfo, &pInfectionList) != ERROR_SUCCESS)
							DeleteFile(pFileInfo->szFilePath);
						}
					else	// we timed out waiting for the scan to complete
						{
						// we wait for the mutex to avoid stealing the fileinfo
						// buffer from the scanning process...
						if(WaitForSingleObject(g_hBufferPoolMutex, MUTEX_TIMEOUT) == WAIT_OBJECT_0)
							{
							// check to see if the state changed during
							// the time we waited for the mutex
							if(pFileInfo->state == BUFFER_SCANNED)
								{
								ReleaseMutex(g_hBufferPoolMutex);

								// the scanning completed, so
								// check for infection
								// if infected, add to infection list, else
								// delete the file
								if(AddFileInfoToList(pFileInfo, &pInfectionList) != ERROR_SUCCESS)
									DeleteFile(pFileInfo->szFilePath);	
								}
							else
								{
								// we timed out again, so indicate we did
								// the SE must delete the file

								// there is a design issue in that if the
								// notes ext is not responding to us (i.e. it
								// is not scanning files), the temp file will
								// not be deleted, since we don't know at this
								// point if the notes ext is just slow (in
								// which case we don't want to delete the file
								// out from under it), or if the notes ext
								// has stopped responding (in which case we
								// would want to delete the temp file, if we
								// could recognize this condition).

								pFileInfo->bufferIndex = 0;
								ReleaseMutex(g_hBufferPoolMutex);
								}
							}
						else
							{
							// If we didn't get the mutex, we will still
							// check to see if the scan got completed, since
							// we can't delay Notes any longer...
							// If scanning did complete, process it.
							if(pFileInfo->state == BUFFER_SCANNED)
								{
								// the scanning completed, so
								// check for infection
								// if infected, add to infection list, else
								// delete the file
								if(AddFileInfoToList(pFileInfo, &pInfectionList) != ERROR_SUCCESS)
									DeleteFile(pFileInfo->szFilePath);
								}
							else
								{
								// we timed out, so indicate we did
								// the SE must delete the file

								// at this point there is a possible sync problem
								// between the two processes; it is possible for the
								// notes ext to have processed the file, and has
								// now marked the buffer as scanned, so it won't
								// delete the temp file, and neither will this
								// process, so the temp file will be left on the
								// hard drive.

								// also, there is a design issue in that if the
								// notes ext is not responding to us (i.e. it
								// is not scanning files), the temp file will
								// not be deleted, since we don't know at this
								// point if the notes ext is just slow (in
								// which case we don't want to delete the file
								// out from under it), or if the notes ext
								// has stopped responding (in which case we
								// would want to delete the temp file, if we
								// could recognize this condition).

								pFileInfo->bufferIndex = 0;
								}
							}
						}
					}
				}
			else
				{
				DebugOut(SM_GENERAL, "NLNVP: ext not in list, attachment skipped \"%s\"", pFileInfo->szAnsiFileName);
				}

ErrorSignalingRTVScan:
ErrorGettingTempDir:
			OSUnlockBlock(head->blockValue);								// NOTESAPI

ErrorLockingBlock:
			ReleaseFileInfoBlock(pFileInfo);
			}
		else
			{
			DebugOut(SM_ERROR, "ERROR: NLNVP: ProcessHookedNote: unable to get info block");
			}
		temp = head->next;
		free(head);
		head = temp;
		}

	// end of attachment scanning

    // re-enable AP
	SAVRTProtectProcess();

	if(pInfectionList)
		{
		DoNotifications(pInfectionList);

		while(pInfectionList)
			{
			pFileInfo = pInfectionList;
			DeleteFile(pFileInfo->szFilePath); 
			pInfectionList = pInfectionList->pNext;
			free(pFileInfo);
			}

        return TRUE;
		}

    return FALSE;
}

/*--- end of source ---*/
