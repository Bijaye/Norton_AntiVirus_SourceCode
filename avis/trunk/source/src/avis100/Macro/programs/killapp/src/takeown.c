#include <stdio.h>
#include <windows.h>
#include <winnt.h>
#include <winbase.h>
#include <aclapi.h>

#include "privileges.h"

BOOL TakeProcessOwnership(HANDLE hProcess) 
{ 
HANDLE hToken; 
BYTE sidBuffer[100];
PSID pSID = (PSID)&sidBuffer;
SID_IDENTIFIER_AUTHORITY SIDAuth = SECURITY_NT_AUTHORITY;
DWORD dwRes;

// Try to remove the object's DACL.

dwRes = SetSecurityInfo (
    hProcess,                 // name of the object
    SE_KERNEL_OBJECT,              // type of object
    DACL_SECURITY_INFORMATION,   // change only the object's DACL
    NULL, NULL,     // don't change owner or group
    NULL,           // no DACL specified allows everyone access
    NULL);          // don't change SACL

if (dwRes == ERROR_SUCCESS)
    return TRUE; 
else if (dwRes != ERROR_ACCESS_DENIED) {
    printf("First SetSecurityInfo call failed: %u\n", dwRes ); 
    return FALSE;
}

// If the preceding call failed because access was denied, 
// enable the SE_TAKE_OWNERSHIP_NAME privilege, create a SID for 
// the Administrators group, take ownership of the object, and 
// disable the privilege. Then try again to set the object's DACL.

// Open a handle to the access token for the calling process.

if (!OpenProcessToken( GetCurrentProcess(), 
                       TOKEN_ADJUST_PRIVILEGES, &hToken)) {
      printf("OpenProcessToken failed: %u\n", GetLastError() ); 
      return FALSE; 
} 

// Enable the SE_TAKE_OWNERSHIP_NAME privilege.
 
if (! SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, TRUE) ) {
    printf("You must be logged on as Administrator\n");
    return FALSE; 
}

// Create a SID for the BUILTIN\Administrators group.

if(! AllocateAndInitializeSid( &SIDAuth, 2,
                 SECURITY_BUILTIN_DOMAIN_RID,
                 DOMAIN_ALIAS_RID_ADMINS,
                 0, 0, 0, 0, 0, 0,
                 &pSID) ) {
    printf( "AllocateAndInitializeSid Error %u\n", GetLastError() );
    return FALSE;
}

// Set the owner in the object's security descriptor.

dwRes = SetSecurityInfo (
    hProcess,                 // name of the object
    SE_KERNEL_OBJECT,              // type of object
    OWNER_SECURITY_INFORMATION,  // change only the object's pwner
    pSID,                        // SID of Administrator group
    NULL, NULL, NULL); 
if (pSID)
    FreeSid(pSID); 
if (dwRes != ERROR_SUCCESS) {
    printf("Could not set owner. Error: %u\n", dwRes ); 
    return FALSE;
}

// Disable the SE_TAKE_OWNERSHIP_NAME privilege.
 
if (! SetPrivilege(hToken, SE_TAKE_OWNERSHIP_NAME, FALSE) ) {
    printf("You must be logged on as Administrator\n");
    return FALSE; 
}

// Try again to remove the object's DACL, now that we are the owner.

dwRes = SetSecurityInfo (
    hProcess,                 // name of the object
    SE_KERNEL_OBJECT,              // type of object
    DACL_SECURITY_INFORMATION,   // change only the object's DACL
    NULL, NULL,     // don't change owner or group
    NULL,           // no DACL specified allows everyone access
    NULL);          // don't change SACL 

if (dwRes == ERROR_SUCCESS)
    return TRUE; 

printf("Second SetSecurityInfo call failed: %u\n", dwRes ); 
return FALSE;
}
