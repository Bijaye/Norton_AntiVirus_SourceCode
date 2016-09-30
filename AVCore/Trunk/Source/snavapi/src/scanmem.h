#ifndef __SCANMEM_H
#define __SCANMEM_H

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>

#include "avshared.h"
#include "shared.h"
#include "log.h"

// WIN 32 declarations
#if defined (SYM_WIN32)
	#include <windows.h>

// UNIX declarations
#elif defined (SYM_UNIX)


// NLM declarations
#elif defined (SYM_NLM)


#endif


///////////////////////////////////////////////////////////////////////////////
// Scan boot options structure
///////////////////////////////////////////////////////////////////////////////
typedef struct {
	char	szDefLoc[_MAX_PATH];
	char	szNavexInfFile[_MAX_PATH];
	WORD	wHeuristicLevel;
	char	szLogLoc[_MAX_PATH];
	DWORD	dwOptions;
} SCANMEMOPTS, *LPSCANMEMOPTS;


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanMemory()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void ScanMemory
(
	int argc, 
	char* argv[]
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		BOOL GetScanMemoryOptions()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//
///////////////////////////////////////////////////////////////////////////////
BOOL GetScanMemoryOptions 
(
	LPSCANMEMOPTS	lpstScanMemOpts,
	int				argc,
	char*			argv[]
);


///////////////////////////////////////////////////////////////////////////////
//
//	Function:
//		void ScanMemoryHelp()
//
//	Description:
//
//	Parameters:
//
//	Returns:
//		NONE
//
///////////////////////////////////////////////////////////////////////////////
void ScanMemoryHelp(void);



#endif // __SCANMEM_H
