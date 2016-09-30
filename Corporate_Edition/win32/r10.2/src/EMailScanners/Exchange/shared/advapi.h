// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
/*===========================================================================*/

#ifndef _ADAVPI_H_
#define _ADAVPI_H_

// Wraps access to the ADVAPI functions, eliminating the need for global variables
class CAdvApiWrap
{
public:
	// Management
	// Initializes the object for use.
	// Returns S_OK on success, else the error code of the failure
	HRESULT Initialize( void );
	// Determines whether the object was successfully initialized.
	// Returns S_OK if successfully initialized, else S_FALSE.
	HRESULT IsInitialized( void );
	// Shuts down the object.  Even if a shutdown error occurs, the object state is cleared (same as when newly created).
	// Returns S_OK on success, E_FAIL if not currently initialized, else the error code of the failure.
	HRESULT Shutdown( void );

	// AdvApi Functions
	// See the Win32 API documentation for parameter and return value information.
	// All calls return FALSE if the object is not currently initialized
	BOOL SetSecurityDescriptorDacl(
	  PSECURITY_DESCRIPTOR pSecurityDescriptor,
	  BOOL bDaclPresent,    
	  PACL pDacl,           
	  BOOL bDaclDefaulted);
	BOOL InitializeSecurityDescriptor(
	  PSECURITY_DESCRIPTOR pSecurityDescriptor,
	  DWORD dwRevision);
	BOOL CreateProcessAsUser(
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
	);
	BOOL DuplicateTokenEx(  HANDLE hExistingToken, 
	  DWORD dwDesiredAccess, 
	  LPSECURITY_ATTRIBUTES lpTokenAttributes,
	  SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
	  TOKEN_TYPE TokenType,  
	  PHANDLE phNewToken);     

	// Constructor-Destructor
	CAdvApiWrap();
	~CAdvApiWrap();
private:
	// DATA TYPES
	typedef BOOL (WINAPI * PSETSECURITYDESCRIPTORDACL)
	(
	  PSECURITY_DESCRIPTOR pSecurityDescriptor,
	  BOOL bDaclPresent,    
	  PACL pDacl,           
	  BOOL bDaclDefaulted   
	);
	typedef BOOL (WINAPI * PINITIALIZESECURITYDESCRIPTOR)
	(
	  PSECURITY_DESCRIPTOR pSecurityDescriptor,
	  DWORD dwRevision  
	);
	typedef BOOL (WINAPI * PCREATEPROCESSASUSER)
	(
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
	);
	typedef BOOL (WINAPI * PDUPLICATETOKENEX)(  HANDLE hExistingToken, 
	  DWORD dwDesiredAccess, 
	  LPSECURITY_ATTRIBUTES lpTokenAttributes,
	  SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
	  TOKEN_TYPE TokenType,  
	  PHANDLE phNewToken);

	// CONSTANTS
	static LPCTSTR					DllFilename;
	static LPCTSTR					ExportName_SetSecurityDescriptorDacl;
	static LPCTSTR					ExportName_InitializeSecurityDescriptor;
	static LPCTSTR					ExportName_CreateProcessAsUser;
	static LPCTSTR					ExportName_DuplicateTokenEx;

	// DATA MEMBERS
	HMODULE							libraryHandle;
	PDUPLICATETOKENEX				duplicateTokenExPtr;
	PCREATEPROCESSASUSER			createProcessAsUserPtr;
	PINITIALIZESECURITYDESCRIPTOR	initializeSecurityDescriptorPtr;
	PSETSECURITYDESCRIPTORDACL		setSecurityDescriptorDaclPtr;
};

#endif  // _ADAVPI_H_