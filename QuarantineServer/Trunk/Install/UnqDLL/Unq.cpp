/*
 *	Unq.cpp
 *
 *	Check for and uninstall quarantined files if any
 *
 *	Built as a custom DLL to be called from InstallShield 5.5
 *
 *	Ed Hopkins 4-11-99
 *
 */

// Header file inclusions here
#include <windows.h>					// Standard windows types
#include <tchar.h>						// Double-byte strings

// Define Symantec-specific symbols here
#define SYM_WIN
#define SYM_WIN32


#include "qserver.h"					// Quarantine server COM object header
#include "Unq.h"						// This module's header


// DLL-global pointer to quarantine interface class
//CClass* defObjectPtr;
//defObjectPtr=new CDefUtils;
//delete defObjectPtr;


// Start of GetUserDecision
extern "C" DllExport int GetUserDecision(void)
{
	int err=0;
	return(err);
}
// End of GetUserDecision


// Start of SaveQFiles
extern "C" DllExport int SaveQFiles(void)
{
	int err=0;
	return(err);
}
// End of SaveQFiles


// Start of RemoveQFiles
extern "C" DllExport int RemoveQFiles(void)
{
	int err=0;
	return(err);
}
// End of RemoveQFiles
