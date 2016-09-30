//
// FILE: CMclGlobal.cpp
//
// Copyright (c) 1997 by Aaron Michael Cohen
//
/////////////////////////////////////////////////////////////////////////

#include "CMclGlobal.h"

void CMclInternalThrowError( DWORD dwStatus, LPCTSTR lpFilename, int line)
{
#if __CMCL_THROW_EXCEPTIONS__

	TCHAR	error[512];
    TCHAR	string[2*MAX_PATH+512];
	
	if (0 < FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwStatus, 0,
							error, 512, NULL))
	    wsprintf(string, "CMcl Library Win32 Error 0x%08x(%d)[%s] at %s line %d\n", 
								dwStatus, dwStatus, error, lpFilename, line); 
	else
	    wsprintf(string,
				"CMcl Library Win32 Error 0x%08x(%d)[unable to get descriptive string from system] at %s line %d\n", 
								dwStatus, dwStatus, lpFilename, line);

    OutputDebugString(string);


    // throw exception for fatal errors...
    throw MclException(string);
#endif
}

