/* $Header:   V:/INCLUDE/VCS/NLOADER.H_V   1.2   14 Dec 1992 14:06:56   SKURTZ  $ */

/*----------------------------------------------------------------------*
 * NLOADER.H								*
 *									*
 * Constants, Data Structures, and Prototypes for the "New Loader"	*
 *									*
 * NOTE: This file must be kept in sync with NLOADER.INC!!!		*
 *									*
 * Copyright 1992 by Symantec Corporation				*
 *----------------------------------------------------------------------*/

/*
 * $Log:   V:/INCLUDE/VCS/NLOADER.H_V  $
 * 
 *    Rev 1.2   14 Dec 1992 14:06:56   SKURTZ
 * Added prototype for NLoaderExecuteCore()
 *
 *    Rev 1.1   14 Oct 1992 22:52:04   JOHN
 * Increased command lines to 129 bytes
 *
 *    Rev 1.0   14 Oct 1992 15:42:40   JOHN
 * Initial revision.
 */

#ifndef INCL_NLOADER
#define INCL_NLOADER

#ifndef INCL_TYPES
#include <types.h>
#endif

#ifndef INCL_FILE
#include <file.h>
#endif

/*----------------------------------------------------------------------*
 * NLoaderRec								*
 *----------------------------------------------------------------------*/

typedef struct
    {
    Byte	szLoaderSignature[8];		// Identify NLoaderRec
    Word	wAppSignature;			// Identify application
    Word	wState;				// Loader state
    Word	wTSRLoaded;			// TRUE if TSR on top of loader
    Word	wPauseBeforeReload;		// See values below
    Word	wExitCode;			// Exit from child process
    Word	wDataSize;			// Size of data area
    void	far *lpData;			// Pointer to data area
    Byte	szRunFile[MAX_PATH_NAME];	// Child process to load
    Byte	szRunCmdline[129];		// Child command line
    Byte	szReloadFile[MAX_PATH_NAME];	// Parent process to reload
    Byte	szReloadCmdline[129];		// Parent command line
    } NLoaderRec;

// NLoaderRec.szLoaderSignature
#define NLDR_SIGNATURE	"NLoader"

// NLoaderRec.wState
#define NLDR_EXIT	0x0000
#define NLDR_FIRST_TIME	0x0001
#define NLDR_EXEC	0x0002

// NLoaderRec.wPauseBeforeReload
#define	NLDR_NO_PAUSE	 0x0000
#define	NLDR_KBD_PAUSE	 0x0001
#define	NLDR_MOUSE_PAUSE 0x0002

/*----------------------------------------------------------------------*
 * Prototypes								*
 *----------------------------------------------------------------------*/

void		 PASCAL NLoaderInit(Byte *pszSignature, Byte *pszCmdline);
NLoaderRec far * PASCAL NLoaderGetRecPtr(void);
void far *	 PASCAL NLoaderGetResidentDataAddr(void);
void		 PASCAL NLoaderExecute(Byte *pszFile, Byte *pszCmdline);
void  		 PASCAL NLoaderExecuteCore(Byte *pszFile, Byte *pszCmdline,Byte *pszPauseMsg, Byte * pszExecErrMsg);

// Internal function, not intended for direct application use
void PASCAL far _NLoaderExecute(NLoaderRec far *lpLoaderRec,
				Byte far *lpszPauseMsg,
				Byte far *lpszExecErrMsg);

#endif /* INCL_NLOADER */



