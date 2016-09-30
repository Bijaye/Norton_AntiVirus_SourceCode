// Copyright 1997 Symantec Corporation, Peter Norton Computing Group
// **********************************************************************
//
//  $Header:   S:/IAL/VCS/spawnusr.cpv   1.1   04 Mar 1998 12:45:46   MKEATIN  $
//
//  Name:           SPAWNUSR.CPP
//
//  Description:    To impersonate given user, and start the provided 
//                  Executable name in the impersonated user's context.
//
// **********************************************************************
//  $Log:   S:/IAL/VCS/spawnusr.cpv  $
// 
//    Rev 1.1   04 Mar 1998 12:45:46   MKEATIN
// 
//    Rev 1.0   25 Jun 1997 17:24:52   DSACKIN
// Initial revision.
// 
//    Rev 1.0   16 Apr 1997 16:23:08   pvenkat
// Initial revision.
// **********************************************************************

// System Includes.
#include <windows.h>
#include <stdio.h>
#include <tchar.h>


// Local defines for memory management.
#define  ALLOC(cbSize)              HeapAlloc (GetProcessHeap (),    \
                                                HEAP_ZERO_MEMORY,    \
                                                cbSize)

#define  FREE(lpMem)                HeapFree (GetProcessHeap (),     \
                                                0L,                  \
                                                lpMem)

#define  REALLOC(lpMem, cbSize)     HeapReAlloc (GetProcessHeap (),  \
                                                HEAP_ZERO_MEMORY,    \
                                                lpMem,               \
                                                cbSize)

// **********************************************************************
// Typedefs.
// **********************************************************************

// Verbose levels
typedef enum
{
    eMute,                              // Donot complain about any erros.
    eReportOnFailureOnly,               // Only if some call failed.
    eReportOnSuccessAndFailure,         // Display msg on success too.
}   VERBOSITY;

// Error processing types.
typedef enum
{
    eUseStdOut = 1,                         // Use printf (for console app)
    eUseMessageBox = 2,                     // Use MessageBox
    eUseLogFile = 4,                        // Append to Log file
    eUseOutputDebugString = 8,              // Write to Debug Monitor/app
    eExitProcessOnAPIFailure = 16           // Exit if API failed.
}   LOGTYPE;


// Options  to read from INI File.
typedef struct
{
    LOGTYPE     nLogType;                   //  How to log errors
    VERBOSITY   nVerboseLevel;              //  Verbositly level
    TCHAR       szLogFile[MAX_PATH + 1];    //  Log file name.
}       FLAGS, *PFLAGS;



typedef struct tagOPTIONS
{
    TCHAR    szNNSPath[MAX_PATH];
    int     nImpersonate;
    TCHAR    szImpersonationUserName[MAX_PATH];
    TCHAR    szImpersonationDomain[MAX_PATH];
    TCHAR    szImpersonationPassword[MAX_PATH];
    TCHAR    szSymantecSetupPath[MAX_PATH];
    TCHAR    szSymantecSetupParameters[MAX_PATH];
    short   nActivityLog;
    TCHAR    szLogLocation[MAX_PATH + 1];
    short   nAppendLog;
} OPTIONS, * LPOPTIONS;

PSID        gpSid;


// **********************************************************************
// * Functions
// **********************************************************************


// **********************************************************************
// * LogError                                                           *
// *                                                                    *
// *    To process error                                                *
// *                                                                    *
// * Parameters:                                                        *
// *    fErr        BOOL        success/failre                          *
// *    szAPI       LPSTR       which function succeeded/failed         *
// *    pFlags      PFLAGS      configurations                          *
// *                                                                    *
// * Returns:                                                           *
// *    fErr                                                            *
// *                                                                    *
// **********************************************************************
BOOL
LogError (BOOL fErr, LPSTR szAPI, PFLAGS pFlags)
{
    CHAR   szError [512];       // Error message from the system
    CHAR   szTemp [512];        // Output buffer 
    HANDLE  hLogFile;           // to open log file
    DWORD   dwNumWritten;       // # of bytes written to log file
    
    // Check for Verbosity level
    switch (pFlags->nVerboseLevel)
    {
        //  Mute: Ignore and donot display anything.
        case    eMute:  
            break;
        
        // Only if failed we need to display.
        case    eReportOnFailureOnly:
            if (fErr)
                break;
                
        // Even when succeeded we need to display an msg.
        case    eReportOnSuccessAndFailure:
            if (fErr)  // Success msg 
                sprintf (szTemp, "%s succeeded!\n", szAPI);
            else    // failed, get the message from system
            {
                FormatMessageA (FORMAT_MESSAGE_FROM_SYSTEM,
                                NULL,
                                GetLastError (),
                                MAKELANGID (LANG_NEUTRAL, SUBLANG_NEUTRAL),
                                szError,
                                sizeof (szError),
                                NULL);
                sprintf (szTemp, 
                        "%s failed.\nGetLastError returned [%u]: %s\n",
                        szAPI, 
                        GetLastError (), 
                        szError);
            }
            break;
    }   // switch
    
    //
    // We need to display msg only if there is an error and Verboselevel is set
    // to display both success and failure
    //
    
    if (fErr && pFlags->nVerboseLevel == eReportOnSuccessAndFailure)
    {
        //
        // Config may want to display errors in different ways
        // check one by one.
        //
            
        if ( (pFlags->nLogType & eUseStdOut) == eUseStdOut)
        {
            // Simple printf
            _tprintf (TEXT ("%s"), szTemp);
        }
        
        if ( (pFlags->nLogType & eUseMessageBox) == eUseMessageBox)
        {
            // MessageBox
            MessageBoxA (NULL, szTemp, szAPI, MB_OK | MB_SETFOREGROUND);
        }
        
        if ( (pFlags->nLogType & eUseLogFile) == eUseLogFile)
        {
            // Write to Logfile.
            dwNumWritten = 0;
            hLogFile = CreateFile (pFlags->szLogFile,
                                    GENERIC_READ | GENERIC_WRITE,
                                    0,
                                    NULL,
                                    OPEN_ALWAYS,
                                    FILE_ATTRIBUTE_NORMAL,
                                    NULL);
            SetFilePointer (hLogFile, 0, 0, FILE_END);
            WriteFile (hLogFile, 
                        szTemp, 
                        lstrlenA (szTemp), 
                        &dwNumWritten, 
                        NULL);
            FlushFileBuffers (hLogFile);
            CloseHandle (hLogFile);
        }
        
        if ( (pFlags->nLogType & eUseOutputDebugString) == eUseOutputDebugString)
        {
            // Write to Debug Monitor
            OutputDebugStringA (szTemp);
        }
    }
    
    // If verbosity is set to ExitonAPIFailure, then kill the process.
    if (pFlags->nLogType == eExitProcessOnAPIFailure && !fErr)
        ExitProcess (GetLastError ());
        
    return fErr;
    
}


BOOL
EnablePrivilege (LPTSTR lpszPrivilege, PFLAGS pFlags)
{
    HANDLE              hToken = NULL;
    TOKEN_PRIVILEGES    tokenPrivileges;
    BOOL                fReturn = FALSE;
    
    __try
    {
        if ( (fReturn = OpenProcessToken (GetCurrentProcess (), 
                                    TOKEN_ADJUST_PRIVILEGES, 
                                    &hToken)))
        {
            if (LookupPrivilegeValue (NULL, 
                                        lpszPrivilege, 
                                        &tokenPrivileges.Privileges[0].Luid))
            {
                tokenPrivileges.PrivilegeCount = 1;
                tokenPrivileges.Privileges[0].Attributes = 
                                            SE_PRIVILEGE_ENABLED;
                fReturn = AdjustTokenPrivileges (hToken, 
                                                FALSE,
                                                &tokenPrivileges,
                                                0,
                                                NULL,
                                                NULL);
                LogError (fReturn, "AdjustTokenPrivileges", pFlags);
            }
            else
                LogError (fReturn, "LookupPrivilegeValue", pFlags);
        }
        else
            LogError (fReturn, "OpenProcessToken", pFlags);
    }
    __finally 
    {
        CloseHandle (hToken);
    }
    return fReturn;
}

BOOL
FindUserProfile (HANDLE hProfileList, LPTSTR lpszSidFromToken, PFLAGS pFlags)
{
    BOOL        fFoundSID;
    DWORD       nIndex;
    DWORD       dwReturn, dwNameLength;
    TCHAR       szSIDString [MAX_PATH + 1];
    FILETIME  rTime;
    
    // Cycle through all subkeys of HKLM\Software\Microsoft\Windows NT\
    // CurrentVersion\ProfileList and compare with string representation 
    // of the SID passed.
    fFoundSID = FALSE;
    nIndex = 0;
    dwReturn = ERROR_SUCCESS;

    // Check each entry.
    while (dwReturn == ERROR_SUCCESS && !fFoundSID)
    {
        dwNameLength = 1024;
        if ( (dwReturn = RegEnumKeyEx (hProfileList, 
                                    nIndex,
                                    szSIDString,
                                    &dwNameLength,
                                    NULL,
                                    NULL,
                                    NULL,
                                    &rTime)) == ERROR_SUCCESS)
        {
            if (lstrcmp (lpszSidFromToken, szSIDString) == 0)
                fFoundSID = TRUE;
            else
                nIndex++;
        }
    }

    return fFoundSID;
}

BOOL
LoadUserProfileFromSIDString (HANDLE hProfileList, LPTSTR lpszSIDString, PFLAGS pFlags)
{
    DWORD   dwReturn, dwBufferLength, dwType;
    TCHAR   szProfileLocation [ MAX_PATH + 1 ] ;
    TCHAR   szBuffer [256];
    HANDLE  hSidKey;
    
    if ( (dwReturn = RegOpenKeyEx (hProfileList, 
                                    lpszSIDString, 
                                    0, 
                                    KEY_READ, 
                                    &hSidKey) )
                                == ERROR_SUCCESS)
    {
        dwBufferLength = 256;
        if ( (dwReturn = RegQueryValueEx (hSidKey, 
                                TEXT ("ProfileImagePath"), 
                                NULL, 
                                &dwType, 
                                (unsigned char *) szBuffer, 
                                &dwBufferLength)) != ERROR_SUCCESS)
        {
            if ( (dwReturn = ExpandEnvironmentStrings (szBuffer, 
                                                        szProfileLocation, 
                                                        MAX_PATH)) != 0)
            {
                if (dwReturn < MAX_PATH)
                {
                    lstrcat (szProfileLocation, TEXT ("\\ntuser.dat"));
                    EnablePrivilege (SE_RESTORE_NAME, pFlags);
                    dwReturn = RegLoadKey (HKEY_USERS, 
                                            lpszSIDString, 
                                            szProfileLocation);
                    LogError ((dwReturn == ERROR_SUCCESS), 
                                "RegLoadKey", 
                                pFlags);
                }
            }
            else
                LogError (FALSE, "ExpandEnvironmentStrings", pFlags);
        }
        LogError (FALSE, "RegQueryValueEx", pFlags);
    }
    else
        LogError (FALSE, "RegOpenKeyEx", pFlags);
        
    return (dwReturn == ERROR_SUCCESS) ? TRUE : FALSE;
}


BOOL
GetStringRepresentationOfSID (SID *pSid, LPTSTR pszSIDString)
{
    PSID_IDENTIFIER_AUTHORITY   pSidIdentifierAuthority;
    DWORD                       dwNumberOfSubAuthorities;
    DWORD                       dwSIDStringLength;
    DWORD                       dwIndex;
    BOOL                        bSuccess = FALSE;
    
    if (!IsValidSid (pSid))
    {
        dwSIDStringLength = _stprintf (pszSIDString, 
                                        TEXT ("S-%u-"), 
                                        (DWORD) (pSid->Revision));
        pSidIdentifierAuthority = GetSidIdentifierAuthority (pSid);
        dwSIDStringLength += (* (short *) (pSidIdentifierAuthority->Value)) ?
                            _stprintf (pszSIDString + dwSIDStringLength,
                                        TEXT ("0x%.2X%.2X%.2X%.2X%.2X%.2X%."),
                                        pSidIdentifierAuthority->Value[0],
                                        pSidIdentifierAuthority->Value[1],
                                        pSidIdentifierAuthority->Value[2],
                                        pSidIdentifierAuthority->Value[3],
                                        pSidIdentifierAuthority->Value[4],
                                        pSidIdentifierAuthority->Value[5]) :
                            _stprintf (pszSIDString + dwSIDStringLength,
                                        TEXT ("%u"),
                                        ((DWORD)*(BYTE*)((DWORD)
                                        pSidIdentifierAuthority->Value + 2)) 
                                        << 24 | ((DWORD) 
                                            pSidIdentifierAuthority->Value
                                                     + 5));
        dwNumberOfSubAuthorities = (DWORD) * GetSidSubAuthorityCount (pSid);
        for (dwIndex = 0; dwIndex < dwNumberOfSubAuthorities; dwIndex++)
            dwSIDStringLength += _stprintf (pszSIDString + dwSIDStringLength,
                                        TEXT ("-%d"),
                                        *GetSidSubAuthority (pSid, dwIndex));
        bSuccess = TRUE;
    }
    else
        bSuccess = FALSE;
    return bSuccess;
}




BOOL
LoadUserProfile (HANDLE hToken, PFLAGS pFlags)
{
   DWORD          dwReturnLength;
   PTOKEN_USER    pTokenUser = NULL;
   HANDLE         hProfileList;
   TCHAR          szSidFromToken [MAX_PATH + 1];
   BOOL           fReturn;

    dwReturnLength = 0;
    if ( !GetTokenInformation (hToken, 
                                TokenUser, 
                                pTokenUser, 
                                dwReturnLength, 
                                &dwReturnLength))
    {
        if (dwReturnLength > 0)
        {
            pTokenUser = (PTOKEN_USER) ALLOC (dwReturnLength);
            if (GetTokenInformation (hToken, 
                                    TokenUser, 
                                    pTokenUser, 
                                    dwReturnLength, 
                                    &dwReturnLength))
            {
                GetStringRepresentationOfSID ((SID *)(pTokenUser->User.Sid), 
                                                szSidFromToken);
                if (RegOpenKeyEx (HKEY_LOCAL_MACHINE,
                                    TEXT ("Software\\Microsoft\\Windows NT\\CurrentVersion\\ProfileList"),
                                    0,
                                    KEY_ENUMERATE_SUB_KEYS,
                                    &hProfileList) == ERROR_SUCCESS)
                {
                    if (FindUserProfile (hProfileList, szSidFromToken, pFlags))
                    {
                        if (LoadUserProfileFromSIDString (hProfileList, 
                                                        szSidFromToken,
                                                        pFlags))
                            fReturn = TRUE;
                        else
                            fReturn = FALSE;
                    }
                    RegCloseKey (hProfileList);
                }
                else
                    LogError (FALSE, "RegOpenKeyEx", pFlags);
            }
            else
                LogError (FALSE, "GetTokenInformation", pFlags);
            FREE ((VOID*)pTokenUser);
        }
    }
    return fReturn;
}



PSECURITY_DESCRIPTOR
GrantAccess (PSECURITY_DESCRIPTOR pSecDesc, PSID pSid, ACCESS_MASK access, PFLAGS pFlags)
{
    PSECURITY_DESCRIPTOR    pNewSD = NULL;
    BOOL                    bDaclPresent;
    BOOL                    bDaclDefaulted;
    ACL_SIZE_INFORMATION    aclInformation;
    DWORD                   dwNewSDSize;
    PACL                    pNewACL, pCurrentACL;
    DWORD                   dwNewACLSize;
    PVOID                   pAce, pAceList;
    DWORD                   dwIndex, nAceListLength;
    PACE_HEADER             pAceHeader;
    PACCESS_MASK            pMask;
    PSID                    pSidTarget;
    BOOL                    bRet = TRUE;
    
    if (GetSecurityDescriptorDacl (pSecDesc, 
                                    &bDaclPresent, 
                                    &pCurrentACL, 
                                    &bDaclDefaulted))
    {
        ZeroMemory (&aclInformation, sizeof (ACL_SIZE_INFORMATION));
        // If there is a DACL and it is not a NULL DACL
        if (bDaclPresent && pCurrentACL)
        {
            if (!GetAclInformation (pCurrentACL, 
                                    &aclInformation, 
                                    sizeof (ACL_SIZE_INFORMATION), 
                                    AclSizeInformation) )
                bRet = FALSE;
        }

        if (bRet)
        {
            dwNewACLSize = aclInformation.AclBytesInUse + 
                            aclInformation.AclBytesFree +
                            (2 * (sizeof (ACCESS_ALLOWED_ACE) - 
                            sizeof (DWORD) +
                            GetLengthSid (pSid) ) );
            dwNewSDSize = dwNewACLSize + 
                                GetSecurityDescriptorLength (pSecDesc);
            pNewSD = ALLOC (dwNewSDSize);
            pNewACL = (PACL) ALLOC (dwNewACLSize);
            
            if (InitializeSecurityDescriptor (pNewSD, 
                                        SECURITY_DESCRIPTOR_REVISION))
            {
                if (InitializeAcl (pNewACL, dwNewACLSize, ACL_REVISION))
                {
                    nAceListLength = (sizeof (ACCESS_ALLOWED_ACE) + 
                            GetLengthSid (pSid) - sizeof (DWORD));
                            
                    if ( ( (nAceListLength / sizeof (DWORD)) * 
                                        sizeof (DWORD) < sizeof (DWORD)) )
                    {
                        nAceListLength = ((nAceListLength / 
                                        sizeof (DWORD)) + 1) *
                                        sizeof (DWORD);
                        pAceList = ALLOC (nAceListLength);
                        pAceHeader = (PACE_HEADER) pAceList;
                        pAceHeader->AceType = (BYTE) ACCESS_ALLOWED_ACE_TYPE;
                        pAceHeader->AceFlags = (BYTE) 0;
                        pAceHeader->AceSize = (WORD) nAceListLength;
                        pMask = (PACCESS_MASK) (((DWORD)pAceHeader) + 
                                                    sizeof (DWORD));
                        *pMask = access;
                        pSidTarget = (PSID) (((DWORD) pMask) + 
                                                sizeof (DWORD));
                        bRet = CopySid (GetLengthSid (pSid), pSidTarget, pSid);
                        LogError (bRet, "CopySid", pFlags);
                    
                        for (dwIndex = 0; 
                                dwIndex < aclInformation.AceCount; 
                                dwIndex++)
                        {
                        
                            if (!GetAce (pCurrentACL, dwIndex, &pAce))
                                break;
                            
                            if (!AddAce (pNewACL, 
                                        ACL_REVISION, 
                                        MAXDWORD, 
                                        pAce, 
                                        ((PACE_HEADER)pAce)->AceSize))
                                break;
                        }
                        if (AddAce (pNewACL, 
                                ACL_REVISION, 
                                0, 
                                pAceList, 
                                nAceListLength))
                        {
                            bRet = SetSecurityDescriptorDacl (pNewSD, 
                                                        TRUE, 
                                                        (PACL) pNewACL, 
                                                        FALSE);
                            LogError (bRet, 
                                        "SetSecurityDescriptorDacl", 
                                        pFlags);
                        }
                    }
                }
                else
                    LogError (FALSE, "InitializeAcl", pFlags);
            }
            else
                LogError (FALSE, "InitializeSecurityDescriptor", pFlags);
        }
    }
    else
        LogError (FALSE, "GetSecurityDescriptorDacl", pFlags);
        
    if (!bRet)
        FREE (pSecDesc);
        
    return pNewSD;
}




BOOL
AccessToWinStation (HANDLE hToken, PSID pSid, HWINSTA *phOld, PFLAGS pFlags)
{
    HWINSTA                 hWindowStation, hOldWindowStation;
    BOOL                    bRet = FALSE;
    SECURITY_INFORMATION    secInfo;
    PSECURITY_DESCRIPTOR    pSecDesc = NULL;
    DWORD                   dwLen = 0;
    
    if ( (hOldWindowStation = GetProcessWindowStation ()) != NULL)
    {
        if ( (hWindowStation = OpenWindowStation (TEXT ("WinSta0"),
                                                    TRUE,
                                                    MAXIMUM_ALLOWED)) != NULL)
        {
            secInfo = DACL_SECURITY_INFORMATION;
            bRet = GetUserObjectSecurity (hWindowStation, 
                                    &secInfo, 
                                    pSecDesc, 
                                    0, 
                                    &dwLen);
            pSecDesc = (PSECURITY_DESCRIPTOR) ALLOC (dwLen);
            secInfo = DACL_SECURITY_INFORMATION;
            if (GetUserObjectSecurity (hWindowStation, 
                                        &secInfo,
                                        pSecDesc,
                                        dwLen,
                                        &dwLen))
            {
                pSecDesc = GrantAccess (pSecDesc, pSid, GENERIC_ALL, pFlags);
                secInfo = DACL_SECURITY_INFORMATION;
                if (SetUserObjectSecurity (hWindowStation,
                                            &secInfo,
                                            pSecDesc))
                {
                    bRet = SetProcessWindowStation (hWindowStation);
                }
                else
                    LogError (FALSE, "SetUserObjectSecurity", pFlags);
            }
            else
                LogError (FALSE, "GetUserObjectSecurity", pFlags);
        }
        else
            LogError (FALSE, "OpenWindowStation", pFlags);
    }
    else
        LogError (FALSE, "GetProcessWindowStation", pFlags);
        
    *phOld = hOldWindowStation;
    return bRet;
}


BOOL
GrantAccessToDesktop (HANDLE hToken, PSID lpLogonSid, PFLAGS pFlags)
{
    HDESK                   hDesktop, hOldDesktop;
    BOOL                    bRet = FALSE;
    SECURITY_INFORMATION    secInfo;
    PSECURITY_DESCRIPTOR    pSecDesc = NULL;
    DWORD                   dwLen = 0;
    
    if ( (hOldDesktop = GetThreadDesktop (GetCurrentThreadId ())) != NULL)
    {
        if ( (hDesktop = OpenDesktop (TEXT ("Default"), 
                                        0, 
                                        TRUE, 
                                        MAXIMUM_ALLOWED)) != NULL)
        {
            secInfo = DACL_SECURITY_INFORMATION;
            GetUserObjectSecurity (hDesktop, &secInfo, pSecDesc, 0, &dwLen);
            pSecDesc = (PSECURITY_DESCRIPTOR) ALLOC (dwLen);
            secInfo = DACL_SECURITY_INFORMATION;
            if (GetUserObjectSecurity (hDesktop, 
                                        &secInfo, 
                                        pSecDesc, 
                                        dwLen, 
                                        &dwLen))
            {
                pSecDesc = GrantAccess (pSecDesc, 
                                        lpLogonSid, 
                                        GENERIC_ALL, 
                                        pFlags);
                secInfo = DACL_SECURITY_INFORMATION;
                if (SetUserObjectSecurity (hDesktop, &secInfo, pSecDesc))
//                    bRet = SetThreadDesktop (hDesktop);
                    ;
            }
            else
                LogError (FALSE, "GetUserObjectSecurity", pFlags);
        }
        else
            LogError (FALSE, "OpenDesktop", pFlags);
    }
    else
        LogError (FALSE, "GetThreadDesktop", pFlags);
        
    return bRet;
}


PSID
GetLogonSid (HANDLE hToken, PFLAGS pFlags)
{
    PSID            lpLogonSid;
    DWORD           dwSize = 4024, dwIndex;
    BOOL            bFound, bFailed = FALSE;
    PTOKEN_GROUPS   pGroups;
    BOOL            bRet;
    
    // Allocate default size
    lpLogonSid = ALLOC (4096);
    if ( (pGroups = (PTOKEN_GROUPS) GlobalAlloc (GPTR, 4024)) != NULL)
    {
        // Obtain Groups that the given token's user belong to
        bRet = GetTokenInformation (hToken,
                            TokenGroups,
                            pGroups,
                            dwSize,
                            &dwSize);
        LogError (bRet, "GetTokenInformation", pFlags);
        
        if (dwSize > 4024)
        {
            if ( (pGroups = (PTOKEN_GROUPS) GlobalReAlloc (pGroups, GPTR, dwSize)) != NULL)
            {
                bRet = GetTokenInformation (hToken,
                                    TokenGroups,
                                    pGroups,
                                    dwSize,
                                    &dwSize);
                LogError (bRet, "GetTokenInformation", pFlags);
            }
        }
        
        // Browse thru all Groups, and copy sid from Groups
        for (dwIndex = 0, bFound = FALSE; 
                                    !bFound && dwIndex < pGroups->GroupCount; 
                                    dwIndex++)
        {
            if ((pGroups->Groups[dwIndex].Attributes & 
                                    SE_GROUP_LOGON_ID) == SE_GROUP_LOGON_ID)
            {
                bRet = CopySid (GetLengthSid (pGroups->Groups[dwIndex].Sid),
                                    lpLogonSid,
                                    pGroups->Groups[dwIndex].Sid);
                LogError (bRet, "CopySid", pFlags);
                
                bFound = TRUE;
            }
        }
    }
    else
    {
        bFailed = TRUE;
    }
    // Cleanup.
    GlobalFree (pGroups);
    
    if (bFailed)
    {
        FREE (lpLogonSid);
        lpLogonSid = NULL;
    }
    
    return lpLogonSid;
}

BOOL
StartProcess (HANDLE hToken, LPTSTR szExePath, PFLAGS pFlags)
{
    STARTUPINFO         si;         // Startup details
    PROCESS_INFORMATION pi;         // New Process details
    BOOL                fRet;       // Return flag.
    
    // Clean up STARTUPINFO
    ZeroMemory (&si, sizeof (si));
    
    si.cb = sizeof (STARTUPINFO);                   // size of structure
    si.lpTitle = NULL;                              // No title
    si.lpDesktop = TEXT ("WinSta0\\Default");       // Desktop
    si.dwX = si.dwY = si.dwXSize = si.dwYSize = 0;  // Size of Window
    si.dwFlags = 0;                                 // Flags
    si.wShowWindow = SW_SHOW;                       // How to show the window
    si.lpReserved = NULL;                           
    si.lpReserved2 = NULL;
    si.cbReserved2 = 0L;

    //
    // NOTE:  we need specify CREATE_SEPARE_WOW_VDM  to start WIN16 application,
    // otherwise on NT3.51 it creats problems.
    //
    
    // Start the given process.
    fRet = CreateProcessAsUser (hToken,             // New user context
                                NULL,               // App name
                                szExePath,          // complete command line
                                NULL,               // Process Security Attr
                                NULL,               // Thread Security Attr
                                TRUE,               // Inherit this Process
                    CREATE_NEW_CONSOLE |            // On new console
                        CREATE_SEPARATE_WOW_VDM,    // Seperate VDM
                                NULL,               // Environment
                                NULL,               // No default dir
                                &si,                // Startup Info
                                &pi);               // Process Info.
    LogError (fRet, "CreateProcessAsUser", pFlags);                                
    return fRet;
}



VOID
ReadIni (LPTSTR lpszIniFile, PFLAGS pFlags)
{
    pFlags->nLogType = (LOGTYPE) GetPrivateProfileInt (TEXT ("Log"), 
                                            TEXT ("LogType"), 
                                            0, 
                                            lpszIniFile);
    pFlags->nVerboseLevel = (VERBOSITY) GetPrivateProfileInt (TEXT ("Log"),
                                            TEXT ("VerboseLevel"),
                                            0,
                                            lpszIniFile);
    GetPrivateProfileString (TEXT ("Log"), 
                                TEXT ("LogFileName"), 
                                TEXT ("C:\\LogFile.Txt"), 
                                pFlags->szLogFile, 
                                sizeof (pFlags->szLogFile), 
                                lpszIniFile);
}

HANDLE WINAPI
Spawn (LPOPTIONS lpOptions)
{
    HANDLE  hToken = NULL;
    PSID    lpLogonSid;
    HWINSTA hOld;
    FLAGS   rFlags;
    BOOL    fRet;
    
//    if (argc >= 5)
    {
        ZeroMemory (&rFlags, sizeof (FLAGS));
        
//        if (argc == 6)  // INI file specified
//            ReadIni (argv[5], &rFlags);

        rFlags.nLogType = eUseMessageBox;
        rFlags.nVerboseLevel = eMute;
        
            
        fRet = LogonUser ( lpOptions->szImpersonationUserName,
                        lpOptions->szImpersonationDomain,
                        lpOptions->szImpersonationPassword,
                        LOGON32_LOGON_INTERACTIVE,
                        LOGON32_PROVIDER_DEFAULT,
                        &hToken);
        if (fRet)
        {
            lpLogonSid = GetLogonSid (hToken, &rFlags);
            gpSid = lpLogonSid;
            AccessToWinStation (hToken, lpLogonSid, &hOld, &rFlags);
            GrantAccessToDesktop (hToken, lpLogonSid, &rFlags);
            SetProcessWindowStation (hOld);
            LoadUserProfile (hToken, &rFlags);
            
//            StartProcess (hToken, argv[4], &rFlags);
//            CloseHandle (hToken);
        }
        else
            _tprintf (TEXT ("LogonUser failed : %d\n"), GetLastError ());
    }
//    else
//        _tprintf (TEXT ("Usage: SU <username> <domain> <password> <Exe>\n"));
          
    return hToken;
}

