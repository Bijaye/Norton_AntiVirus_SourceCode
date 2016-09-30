// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright (c) 2005 Symantec Corporation. All rights reserved.
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
        // ERROR_MORE_DATA will be returned when a very long filename is passed in via lpszFileName.
        // The long filename exceeds the MAX_PATH length on the Windows system and GetFileSecurity can't 
        // process this.
        // 
        // See Defect Etrack 701157 for more details on how this can be cleverly constructed. By allowing this to
        // go through, we allow long filenames to be quarantined.
        //
        // Another side note. An error ---> ERROR_INSUFFICIENT_BUFFER = 122 will be returned by GetLastError() for the "successful" case
        // when we pass a NULL=0 buffer above to get the size. 
        //
        DWORD dwError = GetLastError();
        if ((dwError == ERROR_NOT_SUPPORTED) || (dwError == ERROR_MORE_DATA))
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

#ifdef _USE_CCW
////////////////////////////////////////////////////////////////////////////
// Function name    : GetFileSecurityDescW
//
// Description	    : Gets the security descriptor for a given file
//
// Return type		: BOOL - True if we can get a descriptor for the file.
//
// Argument         : LPWSTR lpszSourceFileName - file to get security attributes from
// Argument         : LPBYTE *pDesc - pointer to buffer pointer.  Note that this
//                    function allocates this buffer.  It must be freed by the
//                    calling routine.
//
////////////////////////////////////////////////////////////////////////////
BOOL GetFileSecurityDescW( LPCWSTR lpszFileName, LPBYTE *pDesc )
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

    if (!GetFileSecurityW( lpszFileName,
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
    if( FALSE == GetFileSecurityW( lpszFileName,
                     si,
                     (PSECURITY_DESCRIPTOR) *pDesc,
                     dwLengthNeeded,
                     &dwLengthNeeded ) )
        {
        return FALSE;
        }

    return TRUE;
}
#endif

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

        // Fixed issue when file security descriptors cannot be
        // fully set due to errors trying to "change" the ownership
        // portion. The error is ERROR_INVALID_OWNER and arises because
        // the current process doesn't have Change ownership rights.
        // See http://support.microsoft.com/default.aspx?scid=kb;en-us;245153 for 
        // more details about the distinction between having "ownership" and "change ownership"
        // rights.
        //
        // If the failure occurs because of an invalid ownership change, we clear out the 
        // OWNER_SECURITY_INFORMATION bits and call SetFileSecurity again with the other important bits set 
        // like the DACL security info.
        //
        // Note that this change is no worse off than we are now. We only redo on a failure
        // which is currently not being tracked or addressed. One bad effect of not addressing failure is that it leaves Quarantine
        // files with "Everyone" access and allows non-admin users to view Quarantine files
        // generated only on Admin accounts.
        //
        if (!SetFileSecurity( lpszFileName,
                         si,
                         pDesc ))
        {
            if (GetLastError() == ERROR_INVALID_OWNER) /* Error 1307 */
            {
                si &= ~(OWNER_SECURITY_INFORMATION);
                SetFileSecurity( lpszFileName,
                            si,
                            pDesc );
            }

        }
    }

    return TRUE;
}

#ifdef _USE_CCW
BOOL SetFileSecurityDescW( LPWSTR lpszFileName, LPBYTE pDesc )
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
		SetFileSecurityW( lpszFileName,
			si,
			pDesc );
	}

	return TRUE;
}
#endif

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
