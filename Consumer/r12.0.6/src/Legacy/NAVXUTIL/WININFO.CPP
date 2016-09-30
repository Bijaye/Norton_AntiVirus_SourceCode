// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/wininfo.cpv   1.1   09 Apr 1997 15:24:12   TIVANOV  $
//
// Description:
//
// Contents:
//    This will be where we should put all information gathering functions
//    that tell about the Windows OS as far as versions or capabilities.
//
//
// See Also:
//************************************************************************
// $Log:   S:/NAVXUTIL/VCS/wininfo.cpv  $
// 
//    Rev 1.1   09 Apr 1997 15:24:12   TIVANOV
// the define supports WINW as well
//
//    Rev 1.0   06 Feb 1997 21:05:34   RFULLER
// Initial revision
//
//    Rev 1.3   23 Aug 1996 17:22:42   DALLEE
// Added missing break statements.
//
//    Rev 1.2   23 Aug 1996 17:01:42   DALLEE
// Include NAVUTIL.H so function name isn't cpp mangled.
//
//    Rev 1.1   02 May 1996 17:47:20   DSACKIN
// Removed two of the functions because they only duplicated functionality
// we already had in Quake.
//
//    Rev 1.0   02 May 1996 16:58:22   DSACKIN
// Initial revision.
//
//************************************************************************


#include "platform.h"
#include "navutil.h"


MODULE_NAME;


/////////////////////////////////////////////////////////////////////
// Local DEFINES

                                        // Makes it easier to determine
                                        // appropriate code paths:
#if defined (WIN32) || defined (WINW)
   #define IS_WIN32 TRUE
#else
   #define IS_WIN32 FALSE
#endif


/////////////////////////////////////////////////////////////////////
// WinInfoIsNewShell
//
// Purpose:
//    This function will tell us if the UI has the new capabilities
//    of the shell that was introduced with Win95, and later added
//    into Windows NT v4.0
//
// Parameters:
//    None
//
// Returns:
//    TRUE  - Has Win95/NT40 shell
//    FALSE - Win31/NT351 shell
//
/////////////////////////////////////////////////////////////////////
// 05/02/1996 DSACKIN - Function Created
/////////////////////////////////////////////////////////////////////

#if defined (SYM_WIN32)

BOOL SYM_EXPORT WINAPI WinInfoIsNewShell ( VOID )
{
    auto    OSVERSIONINFO   rVersion;
    auto    BOOL            bReturn;

    if (IS_WIN32)
        {
        rVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

        if ( !GetVersionEx (&rVersion) )
            {
            bReturn = FALSE;            // No version?  assume no new shell.
            }
        else switch(rVersion.dwPlatformId)
            {
            case VER_PLATFORM_WIN32_NT:
                if (rVersion.dwMajorVersion < 4)
                    bReturn = FALSE;    // NT < 4.0
                else
                    bReturn = TRUE;     // NT >= 4.0
                break;

                                        // Is it Win95?
            case VER_PLATFORM_WIN32_WINDOWS:
                bReturn = TRUE;
                break;

                                        // Win32s or don't know, no newshell.
            case VER_PLATFORM_WIN32s:
            default:
                bReturn = FALSE;
                break;
            }
        }
    else
        bReturn = FALSE;

   return bReturn;
} // WinInfoIsNewShell()

#endif // defined (SYM_WIN32)


