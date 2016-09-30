/*
 *	InstHawk.h
 *
 *	Install Hawking modules (virus defs files)
 *
 *	Built as a custom DLL to be called from InstallShield 5.5
 *
 *	Ed Hopkins 4-8-99
 *
 */

// Define module symbols here
#define DllImport __declspec(dllimport)
#define DllExport __declspec(dllexport)


// List DLL function headers here
extern "C" DllExport BOOL SetupFilesToCopy(LPTSTR AppID, LPTSTR dir);
extern "C" DllExport BOOL CompleteDefsInstall(void);
