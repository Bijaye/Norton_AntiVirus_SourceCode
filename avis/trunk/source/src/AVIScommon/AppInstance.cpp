// AppInstance.cpp

#include "stdafx.h"
#include "AppInstance.h"

// create a mutex kernel object; if a handle is returned and GetLastError() returns
// ERROR_ALREADY_EXISTS, then we have openend the mutex, indicating that and instance
// of the mutex already exists (implying that another instance of the calling process
// already exists).
BOOL AreWeFirstInstance(LPCSTR pszMutexName, DWORD& dwErr)
{
	// attempt to create/open a new mutex (if the mutex already existed, then we are
	// opening it)
	HANDLE hMutex = ::CreateMutex(NULL, TRUE, pszMutexName);
	dwErr = ::GetLastError();

	// failed to create/open the mutex
	if (hMutex == NULL)
	{
		return FALSE;
	}

	if (dwErr == ERROR_ALREADY_EXISTS)
	{
		::CloseHandle(hMutex);
		return FALSE;
	}

	// don't bother closing the handle to the mutex; there needs to be a non-zero
	// reference count on the mutex (hopefully, 1) in order to cause all future calls
	// to CreateMutex() to open a handle and return ERROR_ALREADY_EXISTS.  This is OK,
	// because, when the calling process ends, the reference count for this object
	// is decremented to zero, destroying the kernel mutex object.
	return TRUE;
}
