// Copyright 2004-2005 Symantec Corporation. All rights reserved.
// Author: Bertil Askelid <Bertil_Askelid@Symantec.com>
// Intent: Linux implementation of NDK File Directory Access functions

#ifndef SYM_DIRENT_H_INCLUDED
#define SYM_DIRENT_H_INCLUDED

#include <malloc.h>

#include "ndkDefinitions.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct symDirent {						//depot/Tools/NDK0403/libc/include/dirent.h
	DWORD d_attr;									// entry's attribute: FILE_ATTRIBUTE_DIRECTORY FILE_ATTRIBUTE_NORMAL
	WORD	d_time;									// entry's modification DOS time
	WORD	d_date;									// entry's modification DOS date
	long	d_size;									// entry's size - files only
	DWORD d_cdatetime;							// creation DOS date and time
	DWORD d_adatetime;							// last access DOS date and time - files only
	char	d_name[NAME_MAX + 1];							// entry's namespace name

	// For internal use only !!!

	void*	 __pDIR;									// directory stream
	void*	 __pDirent;								// directory entry
	char*	 __sDirName;							// directory path w/ trailing "/"
	size_t __nDirLength;							// number of characters in `__sDirName'
	char*	 __sMatch;							// directory path w/ trailing "/"
	size_t __nMatchLength;							// number of characters in `__sDirName'
} symDIR;

symDIR* symOpenDir( const char* sPathName);
int	  symCloseDir(symDIR*	  pSymDIR);
symDIR* symReadDir( symDIR*	  pSymDIR);

#ifdef __cplusplus
}
#endif

#endif // SYM_DIRENT_H_INCLUDED
