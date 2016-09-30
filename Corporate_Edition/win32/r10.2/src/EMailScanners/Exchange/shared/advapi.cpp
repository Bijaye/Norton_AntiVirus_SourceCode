// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/
#include <windows.h>
#include <tchar.h>
#include "advapi.h"

// Initialize Constants
LPCTSTR CAdvApiWrap::DllFilename									= _T("advapi32.dll");
LPCTSTR	CAdvApiWrap::ExportName_SetSecurityDescriptorDacl			= _T("SetSecurityDescriptorDacl");
LPCTSTR	CAdvApiWrap::ExportName_InitializeSecurityDescriptor		= _T("InitializeSecurityDescriptor");
LPCTSTR	CAdvApiWrap::ExportName_CreateProcessAsUser					= _T("CreateProcessAsUserA");
LPCTSTR	CAdvApiWrap::ExportName_DuplicateTokenEx					= _T("DuplicateTokenEx");

// ** MANAGEMENT **
HRESULT CAdvApiWrap::Initialize( void )
// Initializes the object for use.
// Returns S_OK on success, S_FALSE if not all functions retrieved from DLL, E_FAIL if already initialized, else the error code of the failure
{
	// Are we currently initialized?
	if (libraryHandle != NULL)
	{
		// Yes - bail
		return E_FAIL;
	}

	TCHAR szPath[MAX_PATH];
	ZeroMemory(szPath, sizeof(szPath));
	::GetSystemDirectory( szPath, MAX_PATH );
	_tcscat( szPath, _T("\\"));
	_tcscat( szPath, DllFilename);

	// Load the library and get all function pointers
	libraryHandle = LoadLibrary(szPath);
	if (libraryHandle != NULL)
	{
		setSecurityDescriptorDaclPtr		= (PSETSECURITYDESCRIPTORDACL)		GetProcAddress(libraryHandle, ExportName_SetSecurityDescriptorDacl);
		initializeSecurityDescriptorPtr		= (PINITIALIZESECURITYDESCRIPTOR)	GetProcAddress(libraryHandle, ExportName_InitializeSecurityDescriptor);
		createProcessAsUserPtr				= (PCREATEPROCESSASUSER)			GetProcAddress(libraryHandle, ExportName_CreateProcessAsUser);
		duplicateTokenExPtr					= (PDUPLICATETOKENEX)				GetProcAddress(libraryHandle, ExportName_DuplicateTokenEx);
	}

	// Did we successfully retrieve all functions?
	if ((setSecurityDescriptorDaclPtr != NULL) && (initializeSecurityDescriptorPtr != NULL) && (createProcessAsUserPtr != NULL) && (duplicateTokenExPtr != NULL))
	{
		// Yes - return init OK
		return S_OK;
	}
	else if ((setSecurityDescriptorDaclPtr == NULL) && (initializeSecurityDescriptorPtr == NULL) && (createProcessAsUserPtr == NULL) && (duplicateTokenExPtr == NULL))
	{
		// None retrieved - shutdown and return failure
		Shutdown();
		return E_FAIL;
	}
	else
	{
		// Some but not all retrieved - indicate partial Initialization success
		return S_FALSE;
	}
}

HRESULT CAdvApiWrap::IsInitialized( void )
// Determines whether the object was successfully initialized.
// Returns S_OK if successfully initialized, else S_FALSE.
{
	if (libraryHandle != NULL)
		return S_OK;
	else
		return S_FALSE;
}

HRESULT CAdvApiWrap::Shutdown( void )
// Shuts down the object.  Even if a shutdown error occurs, the object state is cleared (same as when newly created).
// Returns S_OK on success, E_FAIL if not currently initialized, else the error code of the failure during shutdown.
{
	HRESULT returnVal = S_OK;

	if (libraryHandle == NULL)
		return E_FAIL;

	// Release DLL
	if (!FreeLibrary(libraryHandle))
		returnVal = HRESULT_FROM_WIN32(GetLastError());

	// Clear all data members
	libraryHandle						= NULL;
	setSecurityDescriptorDaclPtr		= NULL;
	initializeSecurityDescriptorPtr		= NULL;
	createProcessAsUserPtr				= NULL;
	duplicateTokenExPtr					= NULL;

	// Return result
	return returnVal;
}

// ** ADVAPI FUNCTIONS **
// See the Win32 API documentation for parameter and return value information.
// All calls return FALSE if the object is not currently initialized
BOOL CAdvApiWrap::SetSecurityDescriptorDacl(
  PSECURITY_DESCRIPTOR pSecurityDescriptor,
  BOOL bDaclPresent,    
  PACL pDacl,           
  BOOL bDaclDefaulted)
{
	if (setSecurityDescriptorDaclPtr != NULL)
		return setSecurityDescriptorDaclPtr(pSecurityDescriptor, bDaclPresent, pDacl, bDaclDefaulted);
	else
		return FALSE;
}

BOOL CAdvApiWrap::InitializeSecurityDescriptor(
  PSECURITY_DESCRIPTOR pSecurityDescriptor,
  DWORD dwRevision)
{
	if (initializeSecurityDescriptorPtr != NULL)
		return initializeSecurityDescriptorPtr(pSecurityDescriptor, dwRevision);
	else
		return FALSE;
}

BOOL CAdvApiWrap::CreateProcessAsUser(
  HANDLE hToken,         
  LPCTSTR lpApplicationName,  
  LPTSTR lpCommandLine,       
  LPSECURITY_ATTRIBUTES lpProcessAttributes, 
  LPSECURITY_ATTRIBUTES lpThreadAttributes,  
  BOOL bInheritHandles,         
  DWORD dwCreationFlags,        
  LPVOID lpEnvironment,         
  LPCTSTR lpCurrentDirectory,   
  LPSTARTUPINFO lpStartupInfo,  
  LPPROCESS_INFORMATION lpProcessInformation  
)
{
	if (createProcessAsUserPtr != NULL)
		return createProcessAsUserPtr(hToken, lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles, dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
	else
		return FALSE;
}

BOOL CAdvApiWrap::DuplicateTokenEx(  HANDLE hExistingToken, 
  DWORD dwDesiredAccess, 
  LPSECURITY_ATTRIBUTES lpTokenAttributes,
  SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
  TOKEN_TYPE TokenType,  
  PHANDLE phNewToken)
{
	if (duplicateTokenExPtr != NULL)
		return duplicateTokenExPtr(hExistingToken, dwDesiredAccess, lpTokenAttributes, ImpersonationLevel, TokenType, phNewToken);
	else
		return FALSE;
}

// Constructor-Destructor
CAdvApiWrap::CAdvApiWrap() : libraryHandle(NULL), duplicateTokenExPtr(NULL), createProcessAsUserPtr(NULL),
	initializeSecurityDescriptorPtr(NULL), setSecurityDescriptorDaclPtr(NULL)
{
	// Nothing for now
}

CAdvApiWrap::~CAdvApiWrap()
{
	// Make sure shutdown is called in case the caller forgot
	if (libraryHandle != NULL)
		Shutdown();
}

/* end source file */