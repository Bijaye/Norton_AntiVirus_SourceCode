// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.

#ifndef OSMISC_DEFS
#define OSMISC_DEFS

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NLS_H
#include "nls.h"						/* Need NLS_PINFO */
#endif

/*	String resource loading routine */

WORD LNPUBLIC		OSLoadString (HMODULE hModule, STATUS StringCode, char far *retBuffer, WORD BufferLength);
/*	Charsets used with OSTranslate */

#define OS_TRANSLATE_NATIVE_TO_LMBCS 	0	/* Translate platform-specific to LMBCS */
#define OS_TRANSLATE_LMBCS_TO_NATIVE 	1	/* Translate LMBCS to platform-specific */
#define	OS_TRANSLATE_LOWER_TO_UPPER		3	/* current int'l case table */
#define	OS_TRANSLATE_UPPER_TO_LOWER		4	/* current int'l case table */
#define OS_TRANSLATE_UNACCENT			5  	/* int'l unaccenting table */

#ifdef DOS									
#define OS_TRANSLATE_OSNATIVE_TO_LMBCS 	7  	/* Used in DOS (codepage) */
#define OS_TRANSLATE_LMBCS_TO_OSNATIVE 	8  	/* Used in DOS			*/
#elif defined (OS2)
#define OS_TRANSLATE_OSNATIVE_TO_LMBCS 	OS_TRANSLATE_NATIVE_TO_LMBCS
#define OS_TRANSLATE_LMBCS_TO_OSNATIVE 	OS_TRANSLATE_LMBCS_TO_NATIVE
#else
#define OS_TRANSLATE_OSNATIVE_TO_LMBCS 	OS_TRANSLATE_NATIVE_TO_LMBCS
#define OS_TRANSLATE_LMBCS_TO_OSNATIVE 	OS_TRANSLATE_LMBCS_TO_NATIVE
#endif

#if defined(DOS) || defined(OS2)
#define OS_TRANSLATE_LMBCS_TO_ASCII		13
#else
#define OS_TRANSLATE_LMBCS_TO_ASCII		11
#endif

/*	Character Set Translation Routines */

WORD LNPUBLIC		OSTranslate(WORD TranslateMode, char far *In, WORD InLength, char far *Out, WORD OutLength);

/*	Dynamic link library portable load routines */

STATUS	LNPUBLIC	OSLoadLibrary (char far *LibraryName, DWORD Flags, HMODULE far *rethModule, void far *retEntryPoint);
void	LNPUBLIC	OSFreeLibrary (HMODULE);
#define OS_LOADPROG_ICONIC			0x0001 	/* Don't show application window */
#define OS_LOADPROG_BACKGROUND		0x0002 	/* Don't bring to the foreground */
#define	OS_LOADPROG_DEBUG			0x0004 	/* Start with QNC */
#define	OS_LOADPROG_DETACHED		0x0008 	/* Don't wait around for this process to exit */
#define	OS_LOADPROG_ASSOCIATE		0x0010 	/* Look for a file type association */

STATUS	far PASCAL	OSLoadProgram (char far *filename, char far *WorkingDir, char far *Arguments, WORD Flags);

/*	Routine used in non-premptive platforms to simulate it. */

#ifdef PREEMPTIVE
#define	OSPreemptOccasionally()
#else
void	LNPUBLIC	OSPreemptOccasionally (void);
#endif
/*	Notes-specific NLS definitions */

NLS_PINFO LNPUBLIC		OSGetLMBCSCLS(void);
NLS_PINFO LNPUBLIC		OSGetNativeCLS(void);

#ifdef __cplusplus
}
#endif

#endif

