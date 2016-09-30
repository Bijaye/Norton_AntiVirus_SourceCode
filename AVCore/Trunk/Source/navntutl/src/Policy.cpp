////////////////////////////////////////////////////////////////////////////
//
// Policy.cpp - Routines to check administrative policies on Windows 2000
//
////////////////////////////////////////////////////////////////////////////
#include "windows.h"
#include "navntutl.h"
#include "tchar.h"


////////////////////////////////////////////////////////////////////////////
// Local defines
#define POLICY_KEY_EXPLORER    "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"
#define POLICY_KEY_RESTRICTRUN "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer\\RestrictRun"
#define POLICY_KEY_NETWORK     "Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Network"

#define POLICY_VALUE_HIDE_SPECIFIED_DRIVE   "NoDrives"
#define POLICY_VALUE_RUN_ONLY_ALLOWED_APPS  "RestrictRun"
#define POLICY_VALUE_NO_ENTIRE_NETWORK      "NoEntireNetwork"

#define WINDOW_2000_VERSION     5

////////////////////////////////////////////////////////////////////////////
// Local functions

BOOL    IsWindows2000( void );


////////////////////////////////////////////////////////////////////////////
// Function name    : IsWindows2000
//
// Description      : Determines if the OS is Windows 2000
//
// Return type      : BOOL
//
////////////////////////////////////////////////////////////////////////////
// 2/1/2000 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
BOOL IsWindows2000( void )
{
    OSVERSIONINFO os;
    os.dwOSVersionInfoSize = sizeof( OSVERSIONINFO );
    GetVersionEx( &os );

    if ( os.dwPlatformId == VER_PLATFORM_WIN32_NT && os.dwMajorVersion >= WINDOW_2000_VERSION )
        return TRUE;
    else
        return FALSE;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : PolicyCheckHideDrives
//
// Description      : Determines the policy "Hide specified drives in My Computer"
//                    is enabled for this application.
//
// Return type		: DWORD - Standard error codes
//
// Argument         : LPDWORD lpdwDrives - Pointer to a DWORD to get hidden drives
//
////////////////////////////////////////////////////////////////////////////
// 2/1/2000 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
DWORD PolicyCheckHideDrives( LPDWORD lpdwDrives )
{
    HKEY    hKey = NULL;
    DWORD   dwDriveMask = 0;
    DWORD   dwRetValue = ERROR_SUCCESS;
    DWORD   dwSize = sizeof(DWORD);
    DWORD   dwType = REG_DWORD;
    long    lResult;

    //
    // Did the caller give us a valid pointer?
    //
    if ( !lpdwDrives )
    {
        //
        // Nope.
        //
        return ERROR_INVALID_PARAMETER;
    }

    if ( IsWindows2000() )
    {
        lResult = RegOpenKeyEx( HKEY_CURRENT_USER,
                                POLICY_KEY_EXPLORER,
                                0,
                                KEY_READ,
                                &hKey );

        if( ERROR_SUCCESS == lResult )
        {
            //
            // Read data.
            //
            lResult = RegQueryValueEx( hKey,
                                       POLICY_VALUE_HIDE_SPECIFIED_DRIVE,
                                       NULL,
                                       &dwType,
                                       (LPBYTE) &dwDriveMask,
                                       &dwSize );

            if( ERROR_SUCCESS == lResult )
            {
                //
                // We could read the value.                
                //
                *lpdwDrives = dwDriveMask;
            }
            else
            {
                //
                // See if there was no registry value. Then the policy 
                // is not configured and no drives are hidden
                // 
                if ( ERROR_FILE_NOT_FOUND == lResult )
                    *lpdwDrives = 0;
            }

            //
            // Cleanup.
            //
            RegCloseKey(hKey);
        }
        
        //
        // Always get the return value from the registry calls
        //
        dwRetValue = (DWORD) lResult;

    }
    else
    {
        //
        // We're not Windows 2000. Set the drive list 
        // to zero indicating no drives are hidden and
        // return success.
        //

        *lpdwDrives = 0;
    }

    return dwRetValue;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : PolicyCheckRunOnly
//
// Description      : Determines the policy "Run only allowed Windows Apps"
//                    is enabled for this application.
//
// Return type		: DWORD - Standard error codes
//
// Argument         : LPSTR lpszAppName - Application name string
// Argument         : LPDWORD lpbAllowed - Buffer to get TRUE if lpszAppName
//                    is allowed to run.
//
////////////////////////////////////////////////////////////////////////////
// 2/1/2000 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
DWORD PolicyCheckRunOnly(LPTSTR lpszAppName, LPDWORD lpbAllowed)
{
    HKEY    hKey = NULL;
    HKEY    hKeyRestricted = NULL;
    int     iNumRestricted = 0;
    DWORD   dwRetValue = ERROR_SUCCESS;
    DWORD   dwSize = sizeof(int);
    DWORD   dwRestrictedNameSize;
    TCHAR   szRestrictedName[MAX_PATH];
    char    szValueName[MAX_PATH];
    DWORD   dwValueNameSize;
    DWORD   dwType = REG_SZ;
    long    lResult;


    //
    // Did the caller give us a valid pointers?
    //
    if ( !lpszAppName || !lpbAllowed )
    {
        //
        // Nope.
        //
        return ERROR_INVALID_PARAMETER;
    }


    if ( IsWindows2000() )
    {
        //
        // Open the key.
        //
        lResult = RegOpenKeyEx( HKEY_CURRENT_USER,
                                POLICY_KEY_EXPLORER,
                                0,
                                KEY_READ,
                                &hKey );

        if( ERROR_SUCCESS == lResult )
        {
            //
            // Read the data.
            //
            lResult = RegQueryValueEx( hKey,
                                       POLICY_VALUE_RUN_ONLY_ALLOWED_APPS,
                                       NULL,
                                       &dwType,
                                       (LPBYTE) &iNumRestricted,
                                       &dwSize );

            if( ERROR_SUCCESS == lResult && iNumRestricted )
            {
                //
                // We could read the "RestrictedRun" value. So now
                // enumerate the RestrictedRun key and see if the
                // lpszAppName is in there somewhere. 
                //
             
                lResult = RegOpenKeyEx( HKEY_CURRENT_USER,
                                        POLICY_KEY_RESTRICTRUN,
                                        0,
                                        KEY_READ,
                                        &hKeyRestricted );

                if ( ERROR_SUCCESS == lResult )
                {
                    //
                    // Default to *NOT* all this one.
                    //
                    *lpbAllowed = FALSE;

                    //
                    // Loop through all the values under this key
                    //
                    for ( int iIndex=0; iIndex<iNumRestricted; iIndex++ )
                    {
                        //
                        // Init the buffers every time through
                        //
                        dwRestrictedNameSize = sizeof(szRestrictedName);
                        memset(szRestrictedName, 0, dwRestrictedNameSize);

                        dwValueNameSize = sizeof(szValueName);
                        memset(szValueName, 0, dwValueNameSize);

                        //
                        // Get the next value
                        //
                        lResult = RegEnumValue( hKeyRestricted,
                                                iIndex,
                                                szValueName,
                                                &dwValueNameSize,
                                                NULL,
                                                &dwType,
                                                (LPBYTE)&szRestrictedName,
                                                &dwRestrictedNameSize );

                        if ( ERROR_SUCCESS == lResult )
                        {
                            //
                            // Compare the restricted name with lpszAppName. 
                            // If they match then allow this app.
                            //
                    
                            if ( _tcsstr(_tcsupr(szRestrictedName), _tcsupr(lpszAppName)) )
                                *lpbAllowed = TRUE;
                        }
                    }
                }
                else
                {
                    //
                    // We failed to open the RestrictRun key or there were no 
                    // restricted apps ... allow it.
                    //
                    *lpbAllowed = TRUE;
                }

                //
                // Close the RestrictedRun key
                //
                RegCloseKey(hKeyRestricted);
            }
            else
            {
                //
                // RegQueryValueEx() failed, so the policy is not
                // configured. Indicate the app may run.
                //
                *lpbAllowed = TRUE;
            }

            //
            // Cleanup.
            //
            RegCloseKey(hKey);
        }
        else
        {
            //
            // We failed to open POLICY_KEY_EXPLORER so we can allow it.
            //
            *lpbAllowed = TRUE;            
        }

    }
    else
        //
        // Not Windows 2000 so allow it.
        //
        *lpbAllowed = TRUE;

    //
    // Always get the return value from the registry calls
    //

    dwRetValue = (DWORD) lResult;

    return dwRetValue;
}


////////////////////////////////////////////////////////////////////////////
// Function name    : PolicyCheckNoEntireNetwork
//
// Description      : Determines the policy "No Entire Network in my Network Places"
//                    is enabled.
//
// Return type		: DWORD - Standard error codes
//
// Argument         : LPDWORD lpbPolicyFlag - Buffer to get TRUE if this policy 
//                    is enabled.
//
////////////////////////////////////////////////////////////////////////////
// 5/15/98 TCASHIN - Function created / Header added.
////////////////////////////////////////////////////////////////////////////
DWORD PolicyCheckNoEntireNetwork( LPDWORD lpbPolicyFlag )
{
    HKEY    hKey = NULL;
    DWORD   dwFlag = 0;
    DWORD   dwRetValue = ERROR_SUCCESS;
    DWORD   dwSize = sizeof(DWORD);
    DWORD   dwType = REG_DWORD;
    long    lResult;


    //
    // Did the caller give us a valid pointer?
    //
    if ( !lpbPolicyFlag )
    {
        // Nope.
        return ERROR_INVALID_PARAMETER;
    }

    //
    // The default is the policy is not set
    //
    *lpbPolicyFlag = FALSE;

    if ( IsWindows2000() )
    {     
        //
        // Try to open the key
        //
        lResult = RegOpenKeyEx( HKEY_CURRENT_USER,
                                POLICY_KEY_EXPLORER,
                                0,
                                KEY_READ,
                                &hKey );

        if( ERROR_SUCCESS == lResult )
        {
            //
            // Read the data.
            //
            lResult = RegQueryValueEx( hKey,
                                       POLICY_VALUE_NO_ENTIRE_NETWORK,
                                       NULL,
                                       &dwType,
                                       (LPBYTE) &dwFlag,
                                       &dwSize );

            if( ERROR_SUCCESS == lResult )
            {
                //
                // We could read the value.                
                //
                *lpbPolicyFlag = dwFlag;
            }

            // 
            // Cleanup.
            //
            RegCloseKey(hKey);
        }
        
        //
        // Always get the return value from the registry calls
        //
        dwRetValue = (DWORD) lResult;
    }

    return dwRetValue;
}


