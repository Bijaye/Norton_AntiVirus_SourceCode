// Copyright 1995 Symantec Corporation
//***************************************************************************
//
// $Header:   S:/NAVXUTIL/VCS/navhalt.cpv   1.3   18 Feb 1998 17:25:02   mdunn  $
//
// Description:
//      This file contains the NavHaltSystem() routine used by the Norton
//      AntiVirus.
//
// Contains:
//      NavHaltSystem()
//
//***************************************************************************
// $Log:   S:/NAVXUTIL/VCS/navhalt.cpv  $
// 
//    Rev 1.3   18 Feb 1998 17:25:02   mdunn
// Ported NEC stuff from ROSNEC.
// 
//    Rev 1.2   02 Jun 1997 15:23:38   RStanev
// NavHaltSystem() for SYM_VXD should not do the shutdown.  Instead, it
// returns and allows its caller to take care of cleaning up and shutting
// down.
//
//    Rev 1.1   13 May 1997 11:38:40   RStanev
// Cleaned up SYM_VXD a little.
//
//    Rev 1.0   06 Feb 1997 21:05:00   RFULLER
// Initial revision
//
//    Rev 1.2   23 Sep 1996 17:03:28   GWESTER
// Fixed build problem.
//
//    Rev 1.1   23 Sep 1996 16:52:28   GWESTER
// Fixed build problem
//
//    Rev 1.0   30 Jan 1996 15:56:12   BARRY
// Initial revision.
//
//    Rev 1.0   30 Jan 1996 14:51:12   BARRY
// Initial revision.
//
//    Rev 1.7   10 Nov 1995 17:56:02   DALLEE
// NavHaltSystem(): Removed system type check from VXD platform -- this
// Quake function isn't available.
//
//    Rev 1.6   09 Nov 1995 15:05:08   GWESTER
// Modified NavHaltSystem() so that if NT is running, it does nothing.
// (NT Gargoyle does not support shutdown, and this function won't be called
// in the first place.)
//
//    Rev 1.5   18 Jul 1995 10:51:06   MARKL
// Fixed CopyFile() bug where it worked for VxD, but not Win32 platform.
//
//    Rev 1.4   16 Jul 1995 13:54:14   MARKL
// Fixed W32 platform include problem.
//
//    Rev 1.3   16 Jul 1995 01:50:06   RSTANEV
// Fixed VXD_WritePrivateProfileString(), VXD_GetWindowsDirectory() and
// VXD_ExitWindowsEx().
//
//    Rev 1.2   15 Jul 1995 21:28:34   MARKL
// Fixed defect #40024.  The VXD can now shut down the system.  However, we
// still need to fix the thunking of GetWindowsDirectory() and
// WritePrivateProfileString() for the changing of the bitmap to work
// correctly on shutdown in the VXD platform.  I also fixed the error checking
// on COPYFILE and the other routines.
//
//    Rev 1.1   10 Jul 1995 14:23:42   MARKL
// Fixed defect #39577.  VxD can now shutdown with the appropriate bitmaps.
//
//    Rev 1.1   10 Jul 1995 14:14:48   MARKL
// Made modifications such that VxD can provide the same functionality.
//
//    Rev 1.0   07 Jul 1995 11:17:32   MARKL
// Initial revision.
//***************************************************************************
#include "platform.h"
#include "file.h"
#include "xapi.h"
#include "navutil.h"



//***************************************************************************
// Local Defines
//***************************************************************************
#define SHUTDOWN_BANNER_FILENAME        "LOGOS.SYS"
#define SHUTDOWN_BANNER_SAVE_FILENAME   "~LOGOS.SYS"
#define SHUTDOWN_BANNER_NEW_FILENAME    "NLOGOS.SYS"
#define SHUTDOWN_WAIT_FILENAME          "LOGOW.SYS"
#define SHUTDOWN_WAIT_SAVE_FILENAME     "~LOGOW.SYS"
#define SHUTDOWN_WAIT_NEW_FILENAME      "NLOGOW.SYS"
#define WININIT_SECTION_NAME            "Rename"
#define WININIT_FILENAME                "wininit.ini"



//***************************************************************************
// Local Prototypes
//***************************************************************************
#ifdef SYM_VXD
#undef  CopyFile
#define CopyFile(a,b,c)                                                    FileCopy(a,b,c)
#undef  GetWindowsDirectory
#define GetWindowsDirectory(lpBuffer,uSize)                                VXD_GetWindowsDirectory((LPSTR)lpBuffer,uSize)
#undef  WritePrivateProfileString
#define WritePrivateProfileString(lpAppName,lpKeyName,lpString,lpFileName) VXD_WritePrivateProfileString(lpAppName,lpKeyName,lpString,lpFileName)

UINT WINAPI VXD_GetWindowsDirectory(
    LPSTR       lpBuffer,
    UINT        uSize);

BOOL WINAPI VXD_WritePrivateProfileString(
    LPCSTR      lpAppName,
    LPCSTR      lpKeyName,
    LPCSTR      lpString,
    LPCSTR      lpFileName);



#endif


// ************************************************************************ //
// ****        (C) Copyright Symantec Corporation 1995                 **** //
// ************************************************************************ //
// ****                                                                **** //
// ****  Procedure:                                                    **** //
// ****                                                                **** //
// ****  Author:        Keith Mund              Date: May 10, 1995     **** //
// ****                                                                **** //
// ****  Functionality: Close down Windows without stopping. Use a     **** //
// ****                 special banner file to issue more text to      **** //
// ****                 the user regarding how to proceed after a      **** //
// ****                 virus alert.                                   **** //
// ****                                                                **** //
// ****  Input:         Zip                                            **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Output:        Zip                                            **** //
// ****                                                                **** //
// ****                                                                **** //
// ****  Update History:                                               **** //
// ****                                                                **** //
// ****                                                                **** //
// ************************************************************************ //
VOID SYM_EXPORT WINAPI NavHaltSystem ( VOID )
{
#if defined(SYM_WIN32) || defined (SYM_VXD)

                                        // If this is NT Gargoyle, and we
                                        // accidently get here (even though
                                        // it shouldn't happen), abort this
                                        // function. (NT Gargoyle doesn't
                                        // support shutdown.)
#ifndef SYM_VXD
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
        return;
        }
#endif


    char    szWinDir[SYM_MAX_PATH];
    char    szFileName1[SYM_MAX_PATH];
    char    szFileName2[SYM_MAX_PATH];
    char    szWinInitName[SYM_MAX_PATH];
    DWORD   dwLen;
                                        // Backup the Win95 shutdown BMP file.
    dwLen = GetWindowsDirectory ( szWinDir, SYM_MAX_PATH );
    CharToOem ( szWinDir, szWinDir );

#ifdef SYM_VXD
    if ( dwLen && !bHWIsNEC )
#else
    if ( dwLen && !HWIsNEC() )
#endif
        {
                                        // Backup the Win95 shutdown BMP file.
        STRCPY ( szFileName1, szWinDir );
        NameAppendFile ( szFileName1, SHUTDOWN_BANNER_FILENAME );
        STRCPY ( szFileName2, szWinDir );
        NameAppendFile ( szFileName2, SHUTDOWN_BANNER_SAVE_FILENAME );
        CopyFile ( szFileName1, szFileName2, FALSE );

                                        // Update the WININIT.INI to restore
                                        // the BMP from the saved BMP.
        STRCPY ( szWinInitName, szWinDir );
        NameAppendFile ( szWinInitName, WININIT_FILENAME );
        WritePrivateProfileString ( WININIT_SECTION_NAME,
                                    szFileName1,
                                    szFileName2,
                                    szWinInitName );

                                        // Update the WININIT.INI to delete
                                        // the saved BMP.
        WritePrivateProfileString ( WININIT_SECTION_NAME,
                                    szFileName2,
                                    "",
                                    szWinInitName );

                                        // Copy our BMP over the Win95 BMP.
        STRCPY ( szFileName2, szWinDir );
        NameAppendFile ( szFileName2, SHUTDOWN_BANNER_NEW_FILENAME );
        CopyFile ( szFileName2, szFileName1, FALSE );

                                        // Backup the Win95 wait for BMP file.
        STRCPY ( szFileName1, szWinDir );
        NameAppendFile ( szFileName1, SHUTDOWN_WAIT_FILENAME );
        STRCPY ( szFileName2, szWinDir );
        NameAppendFile ( szFileName2, SHUTDOWN_WAIT_SAVE_FILENAME );
        CopyFile ( szFileName1, szFileName2, FALSE );

                                        // Update the WININIT.INI to restore
                                        // the BMP from the saved BMP.
        STRCPY ( szWinInitName, szWinDir );
        NameAppendFile ( szWinInitName, WININIT_FILENAME );
        WritePrivateProfileString ( WININIT_SECTION_NAME,
                                    szFileName1,
                                    szFileName2,
                                    szWinInitName ) ;

                                        // Update the WININIT.INI to delete
                                        // the saved BMP.
        WritePrivateProfileString ( WININIT_SECTION_NAME,
                                    szFileName2,
                                    "",
                                    szWinInitName );

                                        // Copy our wait BMP over the Win95 BMP.
        STRCPY ( szFileName2, szWinDir );
        NameAppendFile ( szFileName2, SHUTDOWN_WAIT_NEW_FILENAME );
        CopyFile ( szFileName2, szFileName1, FALSE );
        }

#endif

#if !defined(SYM_VXD)
                                        // We stop for nothing so ignore
                                        // errors and shutdown unconditionally.
                                        // SYM_VXD: the caller is responsible
                                        // for shutting down the machine!
    ExitWindowsEx (EWX_FORCE | EWX_SHUTDOWN, 0);
#endif
}


#ifdef SYM_VXD

//***************************************************************************
// MapFlatAddress()
//
// Description:
//      Returns the linear address of a selector:offset pair.
//
// Parameters:
//      DWORD dwSelOff: HIWORD - selector, LOWORD - offset
//
// Note:
//      Call this function only during appy time.  This ensures that the
//      current VM is the sys vm and that it is in protected mode.
//
//***************************************************************************
// 07/10/1995 RSTANEV Function Created.
//***************************************************************************
PVOID VXDINLINE MapFlatAddress ( DWORD dwSelOff )
    {
    auto UINT uLinAddress;
    auto UINT uCurVMHandle;

    SYM_ASSERT ( Get_Cur_VM_Handle() == Get_Sys_VM_Handle() );

    uCurVMHandle = Get_Cur_VM_Handle();

    _asm    pushad
    _asm    push    0
    _asm    movzx   eax,word ptr dwSelOff + 2
    _asm    push    eax
    _asm    push    uCurVMHandle
    VMMCall(_SelectorMapFlat);
    _asm    add     esp,3*4
    _asm    mov     uLinAddress,eax
    _asm    popad

    return ( (PVOID) ( uLinAddress == 0xFFFFFFFF ?
                           NULL : uLinAddress + (WORD)dwSelOff )
           );
    }

//***************************************************************************
// VXD_GetWindowsDirectory()
//
// Description:
//      Calls GetWindowsDirectoryA()
//
// Parameters:
//      Same as for GetWindowsDirectory()
//
// Note:
//      VxD platform will return the result in ANSI!
//
//***************************************************************************
// 07/10/1995 MARKL Function Created.
//***************************************************************************
UINT WINAPI VXD_GetWindowsDirectory(
    LPSTR       lpBuffer,
    UINT        uSize)
{
    auto UINT uResult;
    auto PSTR pszRing0Buffer;

                                        // The following variable is actually
                                        // the stack frame for
                                        // GetWindowsDirectory().  The size
                                        // and position of the parameters is
                                        // very important.
    auto struct {
        WORD  wSize;
        DWORD lpBuffer;
        } rGetWindowsDirectoryAParams;

                                        // If current VM is not sys VM then
                                        // we are in trouble and we should
                                        // fail this function.

    if ( Get_Cur_VM_Handle() != Get_Sys_VM_Handle() )
        {
        SYM_ASSERT ( FALSE );

        return ( 0 );
        }

                                        // Allocate memory accessible to the
                                        // GetWindowsDirectory() API.

    rGetWindowsDirectoryAParams.lpBuffer =
        SHELL_LocalAllocEx ( LMEM_ZEROINIT, uSize, NULL );

    if ( rGetWindowsDirectoryAParams.lpBuffer == NULL )
        {
        SYM_ASSERT ( FALSE );

        return ( 0 );
        }

                                        // Call the GetWindowsDirectory() API
    SYM_ASSERT ( uSize == (WORD) uSize );

    rGetWindowsDirectoryAParams.wSize = uSize;

    uResult = (UINT)(WORD)SHELL_CallDll ( "KERNEL",
                                          "GETWINDOWSDIRECTORY",
                                          sizeof(rGetWindowsDirectoryAParams),
                                          &rGetWindowsDirectoryAParams );

                                        // Copy the string returned to us by
                                        // GetWindowsDirectory() to our
                                        // buffer.

    pszRing0Buffer =
        (PSTR) MapFlatAddress ( rGetWindowsDirectoryAParams.lpBuffer );

    if ( pszRing0Buffer != NULL &&
         uResult != 0 &&
         uResult < uSize &&
         STRLEN ( pszRing0Buffer ) == uResult )
        {
        STRCPY ( lpBuffer, pszRing0Buffer );
        }
    else
        {
        uResult = 0;
        }

                                        // Free ring 3 memory

    SHELL_LocalFree ( rGetWindowsDirectoryAParams.lpBuffer );

    return ( uResult );

} // VXD_GetWindowsDirectory()


//***************************************************************************
// VXD_WritePrivateProfileString()
//
// Description:
//      Calls WritePrivateProfileStringA()
//
// Parameters:
//      Same as for WritePrivateProfileStringA()
//
// Note:
//      VxD platform expects lpFileName in ANSI!
//
//***************************************************************************
// 07/10/1995 MARKL Function Created.
//***************************************************************************
BOOL WINAPI VXD_WritePrivateProfileString(
    LPCSTR      lpAppName,
    LPCSTR      lpKeyName,
    LPCSTR      lpString,
    LPCSTR      lpFileName)
{
    auto BOOL bResult;
                                        // The following variable is actually
                                        // the stack frame for
                                        // WritePrivateProfileString().  The
                                        // size and position of the parameters
                                        // is very important.
    auto struct {
        DWORD lpFileName;
        DWORD lpString;
        DWORD lpKeyName;
        DWORD lpAppName;
        } rWritePrivateProfileStringAParams;

                                        // Call the WritePrivateProfileString
                                        // API.
    rWritePrivateProfileStringAParams.lpAppName  =
        SHELL_LocalAllocEx ( LMEM_STRING, 0, (PVOID)lpAppName );
    rWritePrivateProfileStringAParams.lpKeyName  =
        SHELL_LocalAllocEx ( LMEM_STRING, 0, (PVOID)lpKeyName );
    rWritePrivateProfileStringAParams.lpString   =
        SHELL_LocalAllocEx ( LMEM_STRING, 0, (PVOID)lpString );
    rWritePrivateProfileStringAParams.lpFileName =
        SHELL_LocalAllocEx ( LMEM_STRING, 0, (PVOID)lpFileName );

    if ( rWritePrivateProfileStringAParams.lpAppName  != NULL &&
         rWritePrivateProfileStringAParams.lpKeyName  != NULL &&
         rWritePrivateProfileStringAParams.lpString   != NULL &&
         rWritePrivateProfileStringAParams.lpFileName != NULL )
        {
        bResult = ( (WORD) SHELL_CallDll (
                                "KERNEL",
                                "WRITEPRIVATEPROFILESTRING",
                                sizeof(rWritePrivateProfileStringAParams),
                                &rWritePrivateProfileStringAParams ) ) == 0 ?
                  FALSE : TRUE;
        }
    else
        {
        bResult = FALSE;
        }

    if ( rWritePrivateProfileStringAParams.lpAppName != NULL )
        {
        SHELL_LocalFree ( rWritePrivateProfileStringAParams.lpAppName );
        }

    if ( rWritePrivateProfileStringAParams.lpKeyName != NULL )
        {
        SHELL_LocalFree ( rWritePrivateProfileStringAParams.lpKeyName );
        }

    if ( rWritePrivateProfileStringAParams.lpString != NULL )
        {
        SHELL_LocalFree ( rWritePrivateProfileStringAParams.lpString );
        }

    if ( rWritePrivateProfileStringAParams.lpFileName != NULL )
        {
        SHELL_LocalFree ( rWritePrivateProfileStringAParams.lpFileName );
        }

    return ( bResult );

} // VXD_WritePrivateProfileString()

#endif
