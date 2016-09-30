// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
//	hook.c contains routines which implement a realtime hook for notes.

#include "nlnvp.h"
#include "initipc.h"

/*---------------------------------------------------------------------------*/

//	NoteOpenHook is called by Notes when a note is opened on the
//	client or the server.  If a note is cleaned, it must be updated
//	to save the changes in the database.  The open hook is typically
//	disabled on the server.  If it is enabled on the server it will CRAWL!
//
//	Parameters
//		pDBHooks	[in] A pointer to a function containing hook 
//					functions.
//		UserName	[in] A string containing the user name that generated
//					the request.		
//      GroupList	[in] A pointer to the notes group list
//		hDB			[in] The handle of the database in which the open originated.
//      NoteID		[in] The noteID of the opened note.
//      hNote		[in] The handle of the opened note.
//      OpenFlags	[in] The notes OPEN_??? flags passed to NSFNoteOpen
//
//	Returns:
//		A notes defined status code.

STATUS LNPUBLIC NoteOpenHook (
                    DBHOOKVEC  *pDBHooks,
                    char	   *UserName, 
                    LIST       *GroupList, 
                    DBHANDLE    hDB, 
                    NOTEID      NoteID, 
                    NOTEHANDLE  hNote,
                    WORD        OpenFlags)
{
	// Verify if check on reads is enabled
	if(!g_pFTBSBuf->regInfo.bReads)
		return NOERROR;

	// re-get debug flag in case it has changed recently...
	g_debug = g_pFTBSBuf->regInfo.bDebug;

//	DebugOut(SM_GENERAL, "NLNVP: NoteOpenHook: processing note...");

    // 2005.02.05 DALLEE - removed check here against NOTEID_ADD??? ID's.
    // These are meaningful in the NoteUpdateHook to determine whether an
    // existing or new note is being operated on.
    // All that happens here on the NoteOpen hook is we were failing to scan
    // depending on how the note is opened -- NOTEID_ADD ID is used when
    // replacing the note viewed in a current window.

	if( ProcessHookedNote(hDB, hNote, UserName) )
		NSFNoteUpdate(hNote, UPDATE_FORCE);					// NOTESAPI

    return NOERROR;
}

/*---------------------------------------------------------------------------*/

//	NoteUpdateHook is called when a note is written to a database.
//	We don't update the note if any changes occur because we are
//	already hooking a write (which we allow to proceed).		
//
//	Parameters
//		pDBHooks	[in] A pointer to a function containing hook functions.
//		UserName	[in] A string containing the user name that generated
//					the request.		
//      GroupList	[in] A pointer to the notes group list
//		hDB			[in] The handle of the database in which the open originated.
//      NoteID		[in] The noteID of the opened note.
//      hNote		[in] The handle of the opened note.
//      UpdateFlags	[in] The notes update flags for the note.
//
//	Returns:
//		A notes defined status code.

STATUS LNPUBLIC  NoteUpdateHook (
                    DBHOOKVEC  *pDBHooks,
                    char       *UserName, 
                    LIST       *GroupList, 
                    DBHANDLE    hDB, 
                    NOTEID      NoteID, 
                    NOTEHANDLE  hNote,
                    WORD       *pwUpdateFlags)
{
	// Verify if check on writes is enabled
	if(!g_pFTBSBuf->regInfo.bWrites)
		return NOERROR;

	// re-get debug flag in case it has changed recently...
	g_debug = g_pFTBSBuf->regInfo.bDebug;

//	DebugOut(SM_GENERAL, "NLNVP: NoteUpdateHook: processing note...");

    if (!(*pwUpdateFlags & UPDATE_DELETED))			// is this note being deleted?
		ProcessHookedNote(hDB, hNote, UserName);	// nope, so scan it...

    // note that the note will get updated by Notes...

    return NOERROR;
}

/*---------------------------------------------------------------------------*/

// Terminate hook is called when the notes hook is being unloaded.
//
//	Parameters
//		pDBHooks	[in] A pointer to a function containing hook 
//					functions.
//
//	Returns:
//		A notes defined status code.

STATUS LNPUBLIC  TerminateHook(DBHOOKVEC * pDBHooks)
{
	// re-get debug flag in case it has changed recently...
	g_debug = g_pFTBSBuf->regInfo.bDebug;

    DebugOut(SM_GENERAL, "NLNVP: mail context shutting down");

	DeInitIPC();
	g_bMailContextRegistered = FALSE;
    
    return NOERROR;
}

/*---------------------------------------------------------------------------*/

//	InitializeNotesHook is called by Notes to initialize the hook
//	This function must be exported at ordinal 1 in the module .def
//	file.  The hook is enumerated in the notes.ini with the line:
//	NSF_HOOKS=nLNVP
//
//	Parameters
//		pDBHooks	[out] A pointer to a function containing hook 
//					functions.
//
//	Returns:
//		A notes defined status code.

STATUS LNPUBLIC  InitializeNotesHook(DBHOOKVEC * pDBHooks)
{
	DWORD	result;
	TCHAR	szProcessName[MAX_PATH];
	TCHAR	*pToken;

	g_debug = TRUE;	// assume we want debug output until we map
					// to the shared buffer, at which time we will
					// get the real value of the debug flag

	if (g_bMailContextRegistered)
		return NOERROR;

	// get the name of the loaded .exe
	GetModuleFileName(NULL, szProcessName, MAX_PATH);
	// if we can find the actual filename...
	// if we can't, we allow it to load...
	if (pToken = _tcsrchr(szProcessName, '\\'))
		{
		// check if this is the actual nlnotes.exe...
		if (_tcsicmp(&pToken[1], "nlnotes.exe"))
			// nope, so don't load
			return ERROR_INVALID_FUNCTION;
		}

	// Initialize the DBHOOK vector.
	pDBHooks->Term         = TerminateHook;
    pDBHooks->NoteOpen     = NoteOpenHook;
    pDBHooks->NoteUpdate   = NoteUpdateHook;
    pDBHooks->DbStampNotes = NULL;

	// Initialize the IPC for the notes storage extension.
	if(result = InitIPC())
	{
		g_bMailContextRegistered = FALSE;
		DebugOut(SM_ERROR, "NLNVP: mail context initialization failed");
	}
	else
	{
		// set the debug flag...
		g_debug = g_pFTBSBuf->regInfo.bDebug;
		g_bMailContextRegistered = TRUE;
		DebugOut(SM_GENERAL, "NLNVP: mail context initialized");
	}

	// tmm: let's move the temp db deletion from shutdown to startup to
	// improve compatibility with third party caching tools (i.e. TurboGold)
	NSFDbDelete("tempnavc");										// NOTESAPI

	return (STATUS)result;
}

/*--- end of source ---*/
