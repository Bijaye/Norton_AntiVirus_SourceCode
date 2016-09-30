#include <stdio.h>
#include <windows.h>
#include <winnt.h>

BOOL SetPrivilege(
    HANDLE hToken,          // access token handle
    LPCTSTR lpszPrivilege,  // name of privilege to enable/disable
    BOOL bEnablePrivilege   // to enable or disable privilege
    ) 
{
TOKEN_PRIVILEGES tp;
LUID luid;

	if ( !LookupPrivilegeValue( NULL,            // lookup privilege on local system
								lpszPrivilege,   // privilege to lookup 
								&luid ) ) 
	{      // receives LUID of privilege
	    printf("LookupPrivilegeValue error: %u\n", GetLastError() ); 
	    return FALSE; 
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;

	if (bEnablePrivilege)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
	    tp.Privileges[0].Attributes = 0;

	// Enable the privilege or disable all privileges.

	AdjustTokenPrivileges(	hToken, 
							FALSE, 
							&tp, 
							sizeof(TOKEN_PRIVILEGES), 
							(PTOKEN_PRIVILEGES) NULL, 
							(PDWORD) NULL); 
 
	// Call GetLastError to determine whether the function succeeded.

	if (GetLastError() != ERROR_SUCCESS) 
	{ 
	    printf("AdjustTokenPrivileges failed: %u\n", GetLastError() ); 
		return FALSE; 
	} 

return TRUE;

}

