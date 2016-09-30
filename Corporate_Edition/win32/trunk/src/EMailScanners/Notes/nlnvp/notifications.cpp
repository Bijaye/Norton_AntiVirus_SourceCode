// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#include "nlnvp.h"


/*---------------------------------------------------------------------------*/

void ReplaceChars(LPTSTR pszDest, LPTSTR pszCharSet, TCHAR cReplacement)
{
	LPTSTR pTmp = pszDest;
	while(pTmp = _tcspbrk(pTmp, pszCharSet))
	{
		*pTmp = cReplacement;
		pTmp++;
	}
}

/*---------------------------------------------------------------------------*/

// StripDirInfo strips off the directory information of a
// Lotus Notes email address

// maxLen is the maximum number of chars that can be copied into the dest.

void StripDirInfo(LPTSTR pszDest, LPTSTR pszSrc, int maxLen)
{
	LPTSTR pTmp;

	pTmp = _tcsstr(pszSrc, "CN=");
	if(pTmp)
		{
		// Copy from after the '='
		pTmp = _tcschr(pTmp, '=');
		pTmp = _tcsinc(pTmp);
		_tcsncpy(pszDest, pTmp, maxLen-1);
		pszDest[maxLen-1]=0;

		// End the string at the /O
		pTmp = _tcsstr(pszDest, "/O");
		if(pTmp)
			*pTmp = 0;
		}
	else
		{
		_tcsncpy(pszDest, pszSrc, maxLen-1);
		pszDest[maxLen-1]=0;
		}

	return;
}
				
/*---------------------------------------------------------------------------*/

// AppendFormattedText appends the string in pszFormat to a temporary
// Note item which is used for virus notifications.  A note item
// called TEMP_BODY_ITEM is retrieved and the text in pszFormat is appended
// to the end of the buffer.
//
//	Parameters:
//		hNote	[in] A handle to an open note.
//		pszFormat [in] A pointer to a string containing notification text.
//
//	Returns:
//		ERROR_SUCCESS

DWORD AppendFormattedText(NOTEHANDLE hNote, LPTSTR pText)
{
	WORD	textLen = 0;
	int		newTextLen;

	// the tempbody should be extra large to hold
	// both the message body and the infection info
	char	szTempBody[MAX_MESSAGE*2];

	// Get the TEMP_BODY_ITEM from the note
	textLen = NSFItemGetText(hNote, TEMP_BODY_ITEM,			// NOTESAPI
								szTempBody, MAX_MESSAGE*2);
	
	newTextLen = _tcslen(pText) + 1;
	
	// Strip all \r\n characters
	ReplaceChars(pText, _T("\r\n"), _T('\0'));

	newTextLen = min(newTextLen, MAX_MESSAGE*2 - 1 - textLen);  
	
	// Append the new infection string to the end of the temp body
	memcpy(szTempBody + textLen, pText, newTextLen);
	szTempBody[MAX_MESSAGE*2 - 1] = 0;

	// delete the old TEMP_BODY_ITEM
	NSFItemDelete(	hNote,									// NOTESAPI
					TEMP_BODY_ITEM,
					(WORD)_tcslen(TEMP_BODY_ITEM));

	// Append the new TEMP_BODY_ITEM to the note
	return NSFItemSetText(	hNote,							// NOTESAPI
							TEMP_BODY_ITEM,
							szTempBody,
							(WORD)(textLen + newTextLen));
}	

/*---------------------------------------------------------------------------*/

//	CreateSenderMessage creates a notes email message in a 
//	temporary database to notify the sender about a possible infection.
//	This message will be sent to the sender after all creation has occured.
//
//	Parameters:
//		hDB		[in] A handle to a mail database.
//		pFileInfo [in] A pointer to a file info structure.
//
//	Returns:
//		A handle to a newly created mail message or NULL if error.

NOTEHANDLE CreateSenderMessage(DBHANDLE hDB, PFILE_INFO pFileInfo)
{
	NOTEHANDLE	hMsg;
	STATUS		status;
	char		szPriority[] = "N"; /* "Normal" */
	char		szSender[MAX_SENDER];
	TIMEDATE	tdDate;
	HANDLE		hRecipientsList;
	VOID		* pRecipientsList;
	WORD		senderLen;
	WORD		RecipientsListSize;

	// Create a new message in the database
	if (status = MailCreateMessage(hDB, &hMsg))					// NOTESAPI
	{
		NOTESMSG(status);
		return NULL;
	}

    // Add the Form item to the message 
    if (status = MailAddHeaderItem( hMsg,						// NOTESAPI
									MAIL_FORM_ITEM_NUM,
									MAIL_MEMO_FORM,
									(WORD)strlen(MAIL_MEMO_FORM)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

    // Add From, Delivery Priority, & etc. items to the message 
    if (status = MailAddHeaderItem( hMsg,						// NOTESAPI
									MAIL_FROM_ITEM_NUM,
									pFileInfo->szUserName, 
									(WORD)strlen(pFileInfo->szUserName)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

    if (status = MailAddHeaderItem( hMsg,						// NOTESAPI
									MAIL_DELIVERYPRIORITY_ITEM_NUM,
									szPriority, 
									(WORD)strlen(szPriority)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
	}

    // Set "PostedDate" to the current time/date  
    OSCurrentTIMEDATE(&tdDate);									// NOTESAPI
    if (status = MailAddHeaderItem( hMsg,						// NOTESAPI
									MAIL_POSTEDDATE_ITEM_NUM,
									(BYTE*)(&tdDate),
									(WORD)sizeof(TIMEDATE)))
    {
		NOTESMSG(status);
		goto CLOSEMSG;
    }

    //  Set "ComposedDate" to current time/date  
    if (status = MailAddHeaderItem( hMsg,						// NOTESAPI
									MAIL_COMPOSEDDATE_ITEM_NUM,
									(BYTE*)(&tdDate),
									(WORD)sizeof(TIMEDATE)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

	// Add the subject to the message
	// Ensure that there are no newline characters in the subject
	ReplaceChars(pFileInfo->SenderSubject, "\r\n", ' ');
	TranslateToLMBCS(pFileInfo->SenderSubject, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
	if (status = MailAddHeaderItem( hMsg,						// NOTESAPI
									MAIL_SUBJECT_ITEM_NUM,
									pFileInfo->szLMBCS, 
									(WORD)strlen(pFileInfo->szLMBCS)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

	// Get the sender from the message we are scanning.
	if(status = MailGetMessageItem(	pFileInfo->hNote,			// NOTESAPI
									MAIL_FROM_ITEM_NUM,
									szSender,
									MAX_SENDER,
									&senderLen))
	{
		NOTESMSG(status);
		goto CLOSEMSG;
	}

	// Append the sender to the message to be sent.
	if (status = MailAddHeaderItem( hMsg,						// NOTESAPI
									MAIL_SENDTO_ITEM_NUM,
									szSender, 
									senderLen))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

	// Append the mail recipient to the message
	if(status = ListAllocate(0, 0, TRUE, &hRecipientsList,		// NOTESAPI
					&pRecipientsList,
                    &RecipientsListSize))
	{
		NOTESMSG(status);
		goto CLOSEMSG;
	}

	OSUnlockObject(hRecipientsList);							// NOTESAPI

    if(status = ListAddEntry(hRecipientsList, TRUE,				// NOTESAPI
            &RecipientsListSize, 0, 
            szSender, (WORD)strlen(szSender)))
	{
		NOTESMSG(status);
		goto CLOSEMSG;
	}

	if(status = MailAddRecipientsItem(hMsg, hRecipientsList,	// NOTESAPI
							RecipientsListSize))
	{
		NOTESMSG(status);
		goto CLOSEMSG;
	}

	// Format the SenderMessageBody given information in pFileInfo
	// Append it to the message.

	TranslateToLMBCS(pFileInfo->SenderMessageBody, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
	if(AppendFormattedText(hMsg, pFileInfo->szLMBCS))
	{
		NOTESMSG(status);
		goto CLOSEMSG;
	}

	return hMsg;

CLOSEMSG:
	
	NSFNoteClose(hMsg);											// NOTESAPI

	return NULL;
}

/*---------------------------------------------------------------------------*/

//	CreateSelectedMessage creates a notes email message in a 
//	temporary database to notify a list of recipients about a 
//	possible infection.
//
//	Parameters:
//		hDB		[in] A database handle for the temporary mail database.
//		pFileInfo [in] A pointer to a file info structure.
//
//	Returns:
//		A handle to a newly created message or NULL if error.

NOTEHANDLE CreateSelectedMessage(DBHANDLE hDB, PFILE_INFO pFileInfo)
{
	NOTEHANDLE	hMsg;
	STATUS		status;
	char		szPriority[] = "N"; // "Normal" 
	TIMEDATE	tdDate;
	HANDLE		hRecipientsList = NULLHANDLE;
    HANDLE		hSendToList = NULLHANDLE;
    LIST		*plistRecipients;
	LIST		*plistSendTo;
    char		*szNextName;        // used when parsing szSendTo, etc. 
    WORD		wRecipientsSize;
	WORD		wSendToSize;
    WORD		wRecipientsCount = 0;
	WORD		wSendToCount = 0;

	// Create a new message in the database
	if (status = MailCreateMessage(hDB, &hMsg))						// NOTESAPI
		return NULL;

    // Add the Form item to the message 
    if (status = MailAddHeaderItem( hMsg,							// NOTESAPI
									MAIL_FORM_ITEM_NUM,
									MAIL_MEMO_FORM,
									(WORD)strlen(MAIL_MEMO_FORM)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

    // Add From, Delivery Priority, & etc. items to the message 
    if (status = MailAddHeaderItem( hMsg,							// NOTESAPI
									MAIL_FROM_ITEM_NUM,
									pFileInfo->szUserName, 
									(WORD)strlen(pFileInfo->szUserName)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

    if (status = MailAddHeaderItem( hMsg,							// NOTESAPI
									MAIL_DELIVERYPRIORITY_ITEM_NUM,
									szPriority, 
									(WORD)strlen(szPriority)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
	}

    // Set "PostedDate" to the current time/date right now 
    OSCurrentTIMEDATE(&tdDate);										// NOTESAPI
    if (status = MailAddHeaderItem( hMsg,							// NOTESAPI
									MAIL_POSTEDDATE_ITEM_NUM,
									(BYTE *)(&tdDate),
									(WORD)sizeof(TIMEDATE)))
    {
		NOTESMSG(status);
		goto CLOSEMSG;
    }

    //  Set "ComposedDate" to current time/date right now 
    if (status = MailAddHeaderItem( hMsg,							// NOTESAPI
									MAIL_COMPOSEDDATE_ITEM_NUM,
									(BYTE *)(&tdDate),
									(WORD)sizeof(TIMEDATE)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

	// Get the Subject from the registry. Append it to the note
	ReplaceChars(pFileInfo->SelectedSubject, "\r\n", ' ');
	TranslateToLMBCS(pFileInfo->SelectedSubject, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
	if (status = MailAddHeaderItem( hMsg,							// NOTESAPI
									MAIL_SUBJECT_ITEM_NUM,
									pFileInfo->szLMBCS, 
									(WORD)strlen(pFileInfo->szLMBCS)))
    {   
		NOTESMSG(status);
		goto CLOSEMSG;
    }

	// Format the SelectedMessageBody given information in pFileInfo
	// Append it to the message.

	TranslateToLMBCS(pFileInfo->SelectedMessageBody, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
	if(AppendFormattedText(hMsg, pFileInfo->szLMBCS))
	{
		DebugOut(SM_ERROR, "ERROR: NLNVP: Unable to append SelectedMessageBody to the message.");
		goto CLOSEMSG;
	}

	// create a new message in the database
    if(status = ListAllocate(0, 0, TRUE, &hRecipientsList,			// NOTESAPI
      &plistRecipients, &wRecipientsSize))
    {
	   // Unable to allocate list 
		NOTESMSG(status);
		goto CLOSEMSG;
	}
    OSUnlockObject(hRecipientsList);								// NOTESAPI

    if (status = ListAllocate(0, 0, TRUE, &hSendToList,				// NOTESAPI
      &plistSendTo, &wSendToSize))
    {
		NOTESMSG(status);
		goto CLOSEMSG;
    }
    OSUnlockObject(hSendToList);									// NOTESAPI

	// Check here to see if a name has been entered	in the Recipients field 
    if ((WORD)_tcslen(pFileInfo->SelectedRecipients) < 2)
   	{
		goto CLOSEMSG;
   	}

    // Parse SendTo string. Add names to SendTo and Recipients lists. 
	TranslateToLMBCS(pFileInfo->SelectedRecipients, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
    for (szNextName = strtok(pFileInfo->szLMBCS, ";"); szNextName != (char*)NULL; szNextName = strtok(NULL, ";"))
    {
	   while (isspace(*szNextName))    // Skip white space before name
		   szNextName++;

	   if (status = ListAddEntry(hSendToList, TRUE, &wSendToSize,	// NOTESAPI
				wSendToCount++, szNextName,
				(WORD)strlen(szNextName)))
	   {   // Unable to add name to SendTo list 
			NOTESMSG(status);
			goto CLOSEMSG;
	   }

	   if (status = ListAddEntry(hRecipientsList, TRUE,				// NOTESAPI
				&wRecipientsSize, wRecipientsCount++, szNextName,
				(WORD)strlen(szNextName)))
	   {   // Unable to add name to Recipients list 
			NOTESMSG(status);
			goto CLOSEMSG;
	   }
	}

    /* 
	Suggested enhancements: You might want to add code here to verify
	that the name & address book on the mail server contains person or
	group documents for each of the named recipients. See Notes API
	funciton NAMELookup(). Possibly query user to resolve unknown
	recipient names. You might also want to check the recipients list
	to ensure it contains no duplicate names.
    */

    // Add the Recipients item to the message. 
    if (wRecipientsCount == 0)  // Mail memo has no recipients. 
    {
		goto CLOSEMSG;
    }

    if (status = MailAddRecipientsItem( hMsg, hRecipientsList,		// NOTESAPI
               wRecipientsSize))
    {
	   // Unable to set Recipient item in memo 

		NOTESMSG(status);
		goto CLOSEMSG;
    }

	//	MailAddRecipientsItem and MailAddHeaderItemByHandle both attach
	//  the memory used by the list to the message. Set handle to NULL
	//  after these functions succeed so the code at CloseMsg: below does
	//  not attempt to free it.
	
    hRecipientsList = NULLHANDLE;

    // Add the SendTo item to the message.

    if (status = MailAddHeaderItemByHandle( hMsg,					// NOTESAPI
					MAIL_SENDTO_ITEM_NUM, hSendToList, wSendToSize, 0))
    {
		NOTESMSG(status);
		goto CLOSEMSG;
    }
    hSendToList = NULLHANDLE;

	return hMsg;

CLOSEMSG:
	
	NSFNoteClose(hMsg);												// NOTESAPI

	return NULL;
}

/*---------------------------------------------------------------------------*/

//	AddWarningMessage adds a warning message to a message containing infected 
//	attachments.
//
//	Parameters:
//		pFileInfo	[in] A pointer to a FILE_INFO structure.
//	
//	Returns:
//		A notes defined status.

DWORD AddWarningMessage(PFILE_INFO pFileInfo)
{
	if(pFileInfo->bChangeMessageSubject)
	{
		// Replace the newly formatted subject to the note
		// note that the string length does not include the NULL terminator

		TranslateToLMBCS(pFileInfo->MessageSubject, pFileInfo->szLMBCS, MAX_NOTES_TEXT);

		MailReplaceHeaderItem(pFileInfo->hNote,						// NOTESAPI
							MAIL_SUBJECT_ITEM_NUM,
							pFileInfo->szLMBCS, 
							(WORD)strlen(pFileInfo->szLMBCS));
	}
	
	// Append the MessageWarning formatted string from the FILE_INFO structure

	TranslateToLMBCS(pFileInfo->MessageWarning, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
	return AppendFormattedText(pFileInfo->hNote, pFileInfo->szLMBCS);
}

/*---------------------------------------------------------------------------*/

//	ConvertTempBody converts the temporary note item TEMP_BODY_ITEM
//	to a rich text BODY_ITEM.
//
//	Parameters:
//		hNote		[in] A handle to an open note.
//		pFileInfo	[in] A pointer to a FILE_INFO structure.
//	
//	Returns:
//		A notes defined status.

DWORD ConvertTempBody(NOTEHANDLE hNote, PFILE_INFO pFileInfo)
{
	BLOCKID     bidItem;
	STATUS		error = ERROR_SUCCESS;
	BLOCKID     bidValue;
	WORD        wDataType;
    DWORD       dwValueLen;
    DWORD       dwRichTextLen;
    BYTE	    * pRichTextBody;
	HANDLE      hRichTextBody;
	TCHAR		szBodyFile[MAX_PATH];
	DWORD		fileSize;
	BOOL		bOldBody = NULL;
	
	if(NSFItemIsPresent(hNote, MAIL_BODY_ITEM,						// NOTESAPI
						(WORD)_tcslen(MAIL_BODY_ITEM)))
	{
		bOldBody = TRUE;

		MakeTempFileName(
			pFileInfo->ExtractDir,
			_T("LNX"),
			pFileInfo->bufferIndex,
			_T(".tmp"),
			szBodyFile,
            sizeof(szBodyFile));

		// extract the current body into a file
		if(error = MailGetMessageBodyComposite(hNote,				// NOTESAPI
						MAIL_BODY_ITEM, szBodyFile, &fileSize))
		{
			NOTESMSG(error);
			goto CLEANUP;
		}

		// delete the original text from the note if present
		NSFItemDelete(hNote, MAIL_BODY_ITEM,						// NOTESAPI
							(WORD)_tcslen(MAIL_BODY_ITEM));
	}
	
	// Get the TEMP_BODY_ITEM from the message
    if (error = NSFItemInfo(hNote,									// NOTESAPI
                            TEMP_BODY_ITEM, 
                            (WORD)_tcslen(TEMP_BODY_ITEM),
                            &bidItem, &wDataType, &bidValue, &dwValueLen))
    {
		NOTESMSG(error);	
        goto CLEANUP;
    }

	// Convert the temp body item to a compound text object
    if (error = ConvertItemToComposite(bidValue,					// NOTESAPI
							dwValueLen, 
							DEFAULT_FONT_ID, 
							"", 
							PARADELIM_ANYLINE,
							&hRichTextBody, 
							&dwRichTextLen, 
							FALSE,
							NULL, 
							0, 
							FALSE))
    {
        NOTESMSG(error);
		return error;        
    }

    pRichTextBody = (BYTE*)OSLockObject(hRichTextBody);				// NOTESAPI
    pRichTextBody += sizeof(WORD);
    dwRichTextLen -= sizeof(WORD);

	// Append the new compound document file to the note
    if (error = NSFItemAppend(hNote, 0, MAIL_BODY_ITEM,				// NOTESAPI
                    (WORD)_tcslen(MAIL_BODY_ITEM), TYPE_COMPOSITE, pRichTextBody,
                    dwRichTextLen))
    {
        NOTESMSG(error);
		OSUnlockObject(hRichTextBody);								// NOTESAPI
        OSMemFree(hRichTextBody);									// NOTESAPI
		return error;
    }
        
    OSUnlockObject(hRichTextBody);									// NOTESAPI
    OSMemFree(hRichTextBody);										// NOTESAPI

	// Delete the TEMP_BODY_ITEM
    if (error = NSFItemDelete(hNote, TEMP_BODY_ITEM,				// NOTESAPI
                            (WORD)_tcslen(TEMP_BODY_ITEM)))
    {
		NOTESMSG(error);
    }

	if(bOldBody)
	{
		// add the original rich text body back to the note
		if(error = MailAddMessageBodyComposite(hNote,				// NOTESAPI
							MAIL_BODY_ITEM, szBodyFile))
		{
			NOTESMSG(error);
		}
	}

CLEANUP:
	
	DeleteFile(szBodyFile); 

	return error;
}

/*---------------------------------------------------------------------------*/

// SendMessageToMailBox sends the note specified by the note handle to
// the recipients specified in the recipient list.
//
//	Parameters:
//		hNote	[in] A handle to the note to be sent.
//		pFileInfo [in] A pointer to a file info structure.
//
//	Returns:
//		A notes defined status.

DWORD SendMessageToMailBox(NOTEHANDLE hNote, PFILE_INFO pFileInfo)
{
	DBHANDLE	hMailBox = NULL;
	DBHANDLE	hOrigDB;
	NOTEID		OrigNoteID;
	OID			OrigNoteOID;
	OID			NewNoteOID;
	TCHAR		szMailServerName[MAX_PATH];
	TCHAR		szMailBoxPath[MAX_PATH];
	STATUS		error;

	if(error = (WORD)ConvertTempBody(hNote, pFileInfo))
		goto CLEANUP;

	// Open the mailbox
    if (!OSGetEnvironmentString(MAIL_MAILSERVER_ITEM,				// NOTESAPI
             szMailServerName, MAX_PATH))
	    {    
		DebugOut(SM_ERROR, "ERROR: NLNVP: Unable to get mail server name."); 
		goto CLEANUP;
		}

    OSPathNetConstruct(NULL, szMailServerName,						// NOTESAPI
							MAILBOX_NAME, szMailBoxPath);

    if (error = NSFDbOpen(szMailBoxPath, &hMailBox))				// NOTESAPI
	    {
		DebugOut(SM_WARNING, "NLNVP: Unable to open server mail box."); 

		if (error = MailTransferMessageLocal(hNote))				// NOTESAPI
			{
			NOTESMSG(error);
			DebugOut(SM_ERROR, "ERROR: NLNVP: Unable to transfer message to local mail.box"); 
			goto CLEANUP;
			}
		}
	else
		{
		// Copy the message, which is a note in the temporary database,
		// to the mail box. Perform this copy by changing the origin to
		// the mail box then updating. Save the message's DBID, NOTEID,
		// and OID. Set the DBID to the MAIL.BOX handle, the NOTEID to zero,
		// and the OID to a newly generated OID associated with MAIL.BOX.
		// Then update the message. This stores it in the MAIL.BOX file.
		// Finally, restore the DBID, NOTEID, and OID.
     
		NSFNoteGetInfo(hNote, _NOTE_ID,  &OrigNoteID);					// NOTESAPI
		NSFNoteGetInfo(hNote, _NOTE_DB,  &hOrigDB);						// NOTESAPI
		NSFNoteGetInfo(hNote, _NOTE_OID, &OrigNoteOID);					// NOTESAPI

		// Set the message's OID database ID to match the mail box 
		if (error = NSFDbGenerateOID (hMailBox, &NewNoteOID))			// NOTESAPI
			{
			NOTESMSG(error);
			DebugOut(SM_ERROR, "ERROR: NLNVP: Unable to generate originator ID for mail box"); 
			goto CLEANUP;
			}
    
		NSFNoteSetInfo(hNote, _NOTE_DB,  &hMailBox);					// NOTESAPI
		NSFNoteSetInfo(hNote, _NOTE_ID,  0);							// NOTESAPI
		NSFNoteSetInfo(hNote, _NOTE_OID, &NewNoteOID);					// NOTESAPI
		}

	//	Update message into MAIL.BOX on mail server. 
    if (error = NSFNoteUpdate(hNote, UPDATE_NOCOMMIT))				// NOTESAPI
	    {
		NOTESMSG(error);
		DebugOut(SM_ERROR, "ERROR: NLNVP: Unable to update message to router mail box."); 
		}
    else	// message successfully copied into router's mail box 
	    {   //	restore msg to temporary mail file and Update to save it there.
	   
		//	NSFNoteSetInfo(hNote, _NOTE_DB,  &hOrigDB);				// NOTESAPI
		//	NSFNoteSetInfo(hNote, _NOTE_ID,  &OrigNoteID);			// NOTESAPI
		//	NSFNoteSetInfo(hNote, _NOTE_OID, &OrigNoteOID);			// NOTESAPI
        
		//	error = NSFNoteUpdate(hNote, UPDATE_NOCOMMIT);			// NOTESAPI
		}

CLEANUP:

	if(hMailBox)
		NSFDbClose(hMailBox);										// NOTESAPI

	if(hNote)
		NSFNoteClose(hNote);										// NOTESAPI
    
    if(!error)
		DebugOut(SM_GENERAL, "NLNVP: successfully deposited memo in mail box.");                                

	return error;
}

/*---------------------------------------------------------------------------*/

//	OpenTempMailDB opens the temporary notes database used to create notification
//	mail messages.  A temporary database is used to create messages to minimize the
//	impact on the system mailbox.
//
//	Returns:
//		A handle to the mail database or NULL on error.

DBHANDLE OpenTempMailDB()
{
	DBHANDLE hTempMail = NULL;

	NSFDbCreate("tempnavc",	DBCLASS_NOTEFILE, FALSE);				// NOTESAPI
	NSFDbOpen("tempnavc", &hTempMail);								// NOTESAPI

	return hTempMail;
}

/*---------------------------------------------------------------------------*/

//	DoNotifications is called when a message contains infected attachments.
//	Mail specific notifications are performed based on configuration information
//	specified in the registry (which is loaded into the FILE_INFO block by 
//	the storage extension).
//
//	Parameters:
//		pInfectionList	[in] A pointer to a linked list of FILE_INFO structures
//						which contain information about the infected attachments.
//
//	Returns:
//		ERROR_SUCCESS, WAIT_TIMEOUT or ERROR_OUTOFMEMORY

DWORD DoNotifications(PFILE_INFO pInfectionList)
{ 
	PFILE_INFO	pFileInfo = pInfectionList;
	PREG_INFO	pRegInfo;

	STATUS		status;
	NOTEHANDLE	hSenderMessage = NULL;
	NOTEHANDLE	hSelectedMessage = NULL;
	DBHANDLE	hTempMail = NULL;
	
	// Make a copy of the REG_INFO structure from the shared memory
	// mapped file.

	pRegInfo = (PREG_INFO)malloc(sizeof(REG_INFO));
	
	if(!pRegInfo)
		return ERROR_OUTOFMEMORY;

	memset(pRegInfo, 0, sizeof(REG_INFO));

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

	//	If the notify administrator option is set, notify the administrator 
	//	that the mail message that was sent contains infected files
	if(pRegInfo->bInsertWarning)
	{
		AddWarningMessage(pFileInfo);
	}
	
	if(pRegInfo->bEmailSender)
	{
		hTempMail = OpenTempMailDB();
		hSenderMessage = CreateSenderMessage(hTempMail, pFileInfo);
	}

	if(pRegInfo->bEmailSelected)
	{
		if(!hTempMail)
			hTempMail = OpenTempMailDB();
		hSelectedMessage = CreateSelectedMessage(hTempMail, pFileInfo);	
	}

	while(pFileInfo)
		{
		status = NOERROR;

		if((pFileInfo->action == AC_CLEAN) ||
           (pFileInfo->action == AC_DEL && pFileInfo->bIsContainer))
			{
			status = NSFNoteAttachFile(									// NOTESAPI
				pFileInfo->hNote,
				ITEM_NAME_ATTACHMENT,
				(WORD)_tcslen(ITEM_NAME_ATTACHMENT),
				pFileInfo->szLMBCSFilePath,		// path to file
				pFileInfo->szOEMFileName,		// user displayable name
				(WORD)(pFileInfo->compressionType | EFLAGS_INDOC));
			}

		if(status == NOERROR
			&& (pFileInfo->action == AC_DEL 
				|| pFileInfo->action == AC_MOVE
				|| pFileInfo->action == AC_CLEAN))
			{
			NSFNoteDetachFile(pFileInfo->hNote, pFileInfo->blockID);	// NOTESAPI
			}
	
		// Add the infection information as needed to the message, sender email 
		// or selected email.

		if(pRegInfo->bInsertWarning)
			{
			TranslateToLMBCS(pFileInfo->MessageInfectionInfo, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
			AppendFormattedText(pFileInfo->hNote, pFileInfo->szLMBCS);
			}

		if(hSenderMessage)
			{
			TranslateToLMBCS(pFileInfo->SenderInfectionInfo, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
			AppendFormattedText(hSenderMessage, pFileInfo->szLMBCS);
			}

		if(hSelectedMessage)
			{
			TranslateToLMBCS(pFileInfo->SelectedInfectionInfo, pFileInfo->szLMBCS, MAX_NOTES_TEXT);
			AppendFormattedText(hSelectedMessage, pFileInfo->szLMBCS);
			}

		pFileInfo = pFileInfo->pNext;
		}

	if(pRegInfo->bInsertWarning)
		ConvertTempBody(pInfectionList->hNote, pInfectionList);

	if(hSenderMessage)
		SendMessageToMailBox(hSenderMessage, pInfectionList);

	if(hSelectedMessage)
		SendMessageToMailBox(hSelectedMessage, pInfectionList);

	if(hTempMail)
		NSFDbClose(hTempMail);											// NOTESAPI

	if(pRegInfo)
		free(pRegInfo);

	return ERROR_SUCCESS;
}

/*--- end of source ---*/
