// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
#ifdef __cplusplus
extern "C" {
#endif


#ifndef ENV_DEFS
#define ENV_DEFS

/*	Size of the buffer used to hold the environment variable values (i.e., it
	excludes the variable name) but including the trailing null terminator.

	NOTE:	The largest known example of an environment variable value is a
			max'ed out COMx=...  (the modem init strings can be large, and
			there's plenty of them).
*/
#define	MAXENVVALUE 256

/* Used to preface ini variables that are different between OSs which may */
/* share the same INI file. */

#if defined(PM)
#define OS_PREFIX			"PM"
#elif defined(W)
#define OS_PREFIX				"WIN"
#else
#define OS_PREFIX				""		/* Only necessary to distinguish */
									/* between entries used in a multi */
									/* OS environment with a single NOTES.INI */
#endif
/*	Environment variable package */

BOOL LNPUBLIC		OSGetEnvironmentString (char far *VariableName, char far *retValueBuffer, WORD BufferLength);
long LNPUBLIC		OSGetEnvironmentLong (char far *VariableName);
#define OSGetEnvironmentInt(s) ((int) OSGetEnvironmentLong(s))

void LNPUBLIC		OSSetEnvironmentVariable (char far *VariableName, char far *Value);
void LNPUBLIC		OSSetEnvironmentInt (char far *VariableName, int Value);

#endif

#ifdef __cplusplus
}
#endif


