////////////////////////////////////////////////////////////////////////////
// $Header:$
////////////////////////////////////////////////////////////////////////////
//
// SecDesc.cpp - Routines to get/set security descriptors in Win NT.
//               aken from global.cpp in Atomic/Quar32
//
////////////////////////////////////////////////////////////////////////////
// $Log:  $
//
////////////////////////////////////////////////////////////////////////////

#include "secdesc.h"


////////////////////////////////////////////////////////////////////////////
// Local functions

BOOL    IsWinNT( void );


////////////////////////////////////////////////////////////////////////////
// Function name    : IsWinNT
//
// Description      : Determines if the OS is WindowsNT
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 5/15/98 JTAYLOR - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL IsWinNT( void )
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );
    return (BOOL)( os.dwPlatformId == VER_PLATFORM_WIN32_NT );
}

////////////////////////////////////////////////////////////////////////////
// Function name    : GetFileSecurityDesc
//
// Description	    : Gets the security descriptor for a given file
//
// Return type		: BOOL - True if we can get a descriptor for the file.
//
// Argument         : LPSTR lpszSourceFileName - file to get security attributes from
// Argument         : LPBYTE *pDesc - pointer to buffer pointer.  Note that this
//                    function allocates this buffer.  It must be freed by the
//                    calling routine.
//
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL GetFileSecurityDesc( LPSTR lpszFileName, LPBYTE *pDesc )
{
    // Do nothing if not on NT.
    if( FALSE == IsWinNT() )
        {
        return TRUE;
        }

    // Get security attributes of original file.
    SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
                              GROUP_SECURITY_INFORMATION |
                              DACL_SECURITY_INFORMATION;

    // This call should fail, returning the length needed for the
    // security descriptor.
    DWORD dwLengthNeeded = 0;

    if (!GetFileSecurity( lpszFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     0,
                     &dwLengthNeeded ))
    {
        // ERROR_NOT_SUPPORTED will be returned when asking for
        // the security descriptor from a file on a non-NT system.
        // Lie about it and tell the caller ewverything is ok.
        if (GetLastError() == ERROR_NOT_SUPPORTED)
            return TRUE;
    }

    // Allocate space needed for call.
    *pDesc = new BYTE[dwLengthNeeded];
    if( *pDesc == NULL )
        {
        return FALSE;
        }

    // This should get the actual security descriptor.
    if( FALSE == GetFileSecurity( lpszFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     dwLengthNeeded,
                     &dwLengthNeeded ) )
        {
        return FALSE;
        }

    return TRUE;
}




////////////////////////////////////////////////////////////////////////////
// Function name    : SetFileSecurityDesc
//
// Description	    : Set file security for a filesystem object
//
// Return type		: BOOL
//
// Argument         : LPSTR szFileName - File to modify
// Argument         : LPBYTE pDesc     - descriptor to set
//
////////////////////////////////////////////////////////////////////////////
// 5/12/98 DBUCHES - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL SetFileSecurityDesc( LPSTR lpszFileName, LPBYTE pDesc )
{
    // Do nothing if not on NT.
    if( FALSE == IsWinNT() )
        {
        return TRUE;
        }

    if( pDesc != NULL )
        {
        SECURITY_INFORMATION si = OWNER_SECURITY_INFORMATION |
                                  GROUP_SECURITY_INFORMATION |
                                  DACL_SECURITY_INFORMATION;

        // Ignore return result.
        SetFileSecurity( lpszFileName,
                         si,
                         pDesc );
        }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : FreeFileSecurityDesc
//
// Description	    : Free the memory allocated for file security descriptor
//                    of a filesystem object
//
// Return type		: BOOL
//
// Argument         : LPBYTE pDesc     - pointer to file security descriptor.
//
////////////////////////////////////////////////////////////////////////////
// 3/24/99 RCHINTA - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL FreeFileSecurityDesc( LPBYTE pDesc )
{

    // Free security descriptor.
	if( NULL != pDesc )
	{
		delete [] pDesc;
		pDesc = NULL;
        return TRUE;
	}
    else
        return FALSE;
}
