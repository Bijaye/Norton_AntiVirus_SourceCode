// dllentry.h - Dll Entrypoint for the AvisServlet dll.

// I've separated this for three reasons:

// to avoid having <windows.h> where I don't need it
// to separate the "dll" architecture specifics entirelly
// to avoid having <windows.h> where I don't need it. :)

#if !defined (__DLLENTRY_H__)
#define __DLLENTRY_H__

#include <windows.h>


BOOL WINAPI DllMain (HINSTANCE hinstDll,DWORD fdwReason,LPVOID fImpLoad);

#endif // __DLLENTRY_H__



