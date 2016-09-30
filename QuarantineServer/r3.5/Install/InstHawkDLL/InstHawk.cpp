/*
 *	InstHawk.cpp
 *
 *	Install Hawking modules (virus defs files)
 *
 *	Built as a custom DLL to be called from InstallShield 5.5
 *
 *	Ed Hopkins 4-8-99
 *
 */

// Header file inclusions here
#include <windows.h>					// Standard windows types
#include <tchar.h>						// Double-byte strings

// Define Symantec-specific symbols here
#define SYM_WIN
#define SYM_WIN32

#include "Defutils.h"					// Virus defs utility class
#include "InstHawk.h"					// This module's header


// DLL-global pointer to def utils class
CDefUtils* defObjectPtr;


// Start of SetupFilesToCopy
extern "C" DllExport BOOL SetupFilesToCopy(LPTSTR AppID, LPTSTR dir)
{
	BOOL bError=FALSE;
	TCHAR szBuf[1024];
	UINT size;
	defObjectPtr=new CDefUtils;
	if(defObjectPtr==NULL)
	{
		bError=TRUE;
	}
	else
	{
		defObjectPtr->InitInstallApp(AppID, NULL, NULL);
		*szBuf=NULL;
		size=0;
		defObjectPtr->PreDefUpdate(szBuf, size, CDefUtils::Install);
		if(*szBuf==NULL)
		{
			bError=TRUE;
		}
		else
		{
			_tcscpy(dir, szBuf);
		}
	}
	return(bError);
}
// End of SetupFilesToCopy


// Start of CompleteDefsInstall
extern "C" DllExport BOOL CompleteDefsInstall(void)
{
	BOOL bError=FALSE;
	if(defObjectPtr==NULL)
	{
		bError=TRUE;
	}
    else
	{
		defObjectPtr->PostDefUpdate();
		defObjectPtr->UseNewestDefs();
		delete defObjectPtr;
	}
	return(bError);
}
// End of CompleteDefsInstall
