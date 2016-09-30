/*
 *	Unq.h
 *
 *	Uninstall quaratined files
 *
 *	Built as a custom DLL to be called from InstallShield 5.5
 *
 *	Ed Hopkins 4-11-99
 *
 */

// Define module symbols here
#define DllImport __declspec(dllimport)
#define DllExport __declspec(dllexport)


// List DLL function headers here
extern "C" DllExport int GetUserDecision(void);
extern "C" DllExport int SaveQFiles(void);
extern "C" DllExport int RemoveQFiles(void);
