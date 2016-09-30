/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/prm_dos.c_v   1.4   30 Jul 1998 20:38:02   dhertel  $ *
 *                                                                      *
 * Description:                                                         *
 *      DOS functions.                                                  *
 *                                                                      *
 * Contains:                                                            *
 *      DOSGetInternalVariables                                         *
 *      DOSGetEnvironment                                               *
 *      DOSGetEnvironmentCase                                           *
 *      DOSEnvSearchPathForFile                                         *
 *      DOSGetDate                                                      *
 *      DOSGetPCMOSVersion                                              *
 *      DOSGetTime                                                      *
 *      DOSSetTime                                                      *
 *      DOSSetDate                                                      *
 *      DOSGetDate                                                      *
 *      DOSGetVersion                                                   *
 *      DOSOEMNumber                                                    *
 *      DOSSetBreakStatus                                               *
 *      DOSGetBreakStatus                                               *
 *      DOSGetIntHandler                                                *
 *      DOSSetIntHandler                                                *
 *      DOSGetErrorInfo                                                 *
 *      DOSGetExtendedError                                             *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/prm_dos.c_v  $ *
// 
//    Rev 1.4   30 Jul 1998 20:38:02   dhertel
// 
//    Rev 1.3   19 Mar 1997 21:45:36   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.2   08 Feb 1996 15:25:50   RSTANEV
// Added SYM_NTK support.
//
//    Rev 1.1   07 Feb 1996 10:23:28   THOFF
// Fixed DOSGetVersion() so that it doesn't plaster the version number
// randomly into memory under DOS and X86.
//
//    Rev 1.0   26 Jan 1996 20:22:50   JREARDON
// Initial revision.
//
//    Rev 1.58   11 Jan 1996 13:19:54   DBUCHES
// Fixed DOSGetVersion() for WinNT platform!
//
//    Rev 1.56   10 Jan 1996 18:10:02   DBUCHES
// Changed DOSGetVersion for Win32.  The IOCtlVWin32() call was returning
// garbage, so we now use VXDIntBasedCall() to accomplish our task.
//
//    Rev 1.55   09 Jan 1996 17:11:32   MARKK
// Oops, slight error
//
//    Rev 1.54   09 Jan 1996 15:25:06   MARKK
// Fixed DOSGetVersion under W32
//
//    Rev 1.53   15 Dec 1995 12:50:50   DBUCHES
// DOSGetVersion() now caches the DOS version, so the Int21 call is only made once.
//
//    Rev 1.52   02 Aug 1995 00:44:58   AWELCH
// Merge changes from Quake 7.
//
//    Rev 1.50.1.6   17 Jul 1995 13:12:40   TONY
// Environmental Variables under OS/2 - now use pointers correctly.
//
//    Rev 1.50.1.5   11 May 1995 12:03:28   TORSTEN
// Made DOSGetVersion() a function, since the macro didn't work correctly.
//
//    Rev 1.50.1.4   11 May 1995 11:18:48   MBARNES
// Returned to version 1.50.1.0 because of build problems
//
//    Rev 1.50.1.0   24 Apr 1995 19:46:04   SCOTTP
// Branch base for version QAK7
//
//    Rev 1.50   11 Apr 1995 17:03:50   HENRI
// Merge from branch 6
//
//    Rev 1.49   29 Nov 1994 15:06:26   TONY
// Added time and date functions to OS/2
//
//    Rev 1.48   18 Nov 1994 14:08:12   BRAD
// Made DBCS stuff a little more efficient
//
//    Rev 1.47   17 Nov 1994 12:46:04   DBUCHES
// For Win32, the DOS time functions now report info on LOCAL time as opposed
// to system (GMT) time.
//
//    Rev 1.46   10 Nov 1994 13:31:56   BRIANF
// Added DBCS.H.
//
//    Rev 1.45   09 Nov 1994 16:51:16   BRAD
// DBCS enabled
//
//    Rev 1.44   28 Sep 1994 15:33:30   TONY
// DosGetEnvironment() temp buffer
//
//    Rev 1.43   23 Sep 1994 13:49:32   TONY
// Added DOSGetEnvironment() to OS/2
//
//    Rev 1.42   07 Jun 1994 02:27:36   GVOGEL
// Added comment blocks for Geologist project.
//
//    Rev 1.41   01 Jun 1994 11:49:52   MARKK
// Don't free the ENV selector!
//
//    Rev 1.40   01 Jun 1994 11:31:06   MARKK
// Get environment properly under DX
//
//    Rev 1.39   31 May 1994 13:27:46   BRAD
// DOSGetInternalVariables() now returns a real mode ptr.
//
//    Rev 1.38   26 May 1994 07:41:28   MARKK
// Change DX to PharLap
//
//    Rev 1.37   18 May 1994 16:37:38   BRAD
// Removed reference to Toolhelp.h.  Not used
//
//    Rev 1.36   09 May 1994 09:21:20   MARKK
// DOSX work
//
//    Rev 1.35   05 May 1994 12:59:32   BRAD
// DX stuff
//
//    Rev 1.34   04 May 1994 13:52:10   BRAD
// Undefined variable
//
//    Rev 1.33   03 May 1994 18:30:46   BRAD
// Use DOSGetPSP() to get PSP
//
//    Rev 1.32   03 May 1994 18:02:20   BRAD
// Use MAKELP, not MK_FP
//
//    Rev 1.31   29 Apr 1994 11:20:16   MARKK
// Fixes for SYM_DOSX
//
//    Rev 1.30   21 Mar 1994 15:47:54   BRUCE
// Uncommented undoc.h
//
//    Rev 1.29   21 Mar 1994 15:15:34   BRAD
// Move pragmas outside of functions
//
//    Rev 1.28   21 Mar 1994 00:20:18   BRAD
// Cleaned up for WIN32
//
//    Rev 1.27   15 Mar 1994 12:33:42   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.26   25 Feb 1994 15:03:36   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.24   18 Feb 1994 19:18:00   PGRAVES
// Win32.
//
//    Rev 1.23   17 Nov 1993 18:01:30   BARRY
// Merge changes from QAK2 branch
//
//    Rev 1.22.1.1   17 Nov 1993 17:57:36   BARRY
// Use NameConvertFwdSlashes() in DOSGetProgramName()
//
//    Rev 1.22.1.0   19 Oct 1993 09:28:58   SCOTTP
// Branch base for version QAK2
//
//    Rev 1.22   15 Oct 1993 07:36:48   ED
// Fixed compile problem
//
//    Rev 1.21   14 Oct 1993 13:26:32   DALLEE
// Removed unused local var warning in DOSGetDRDOSVersion().
//
//    Rev 1.20   11 Oct 1993 16:29:34   ED
// Fixed DR-DOS 7.0 ID
//
//    Rev 1.19   04 Oct 1993 13:24:44   ED
// Modified DOSGetDRDOSVersion to use DPMI under Windows to get the
// DR-DOS version ID.  The IOCTL call to do this is not automatically
// translated by Microsoft, which is no great surprise.
//
//    Rev 1.18   02 Jul 1993 08:52:14   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.17   04 Jun 1993 19:07:18   BASIL
// Added DOSGetDRDOSVersion()
//
//    Rev 1.16   28 Apr 1993 15:59:18   BRUCE
// [FIX] #undef MAX_PATH to include toolhelp.h
//
//    Rev 1.15   21 Feb 1993 18:31:24   BRAD
// Fixed up STRNCPY() problems, where wasn't terminating with EOS.
//
//    Rev 1.14   21 Feb 1993 18:00:12   BRAD
// Cleaned up some STRNCPY() routines.
//
//    Rev 1.13   15 Feb 1993 21:04:36   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.12   12 Feb 1993 04:26:16   ENRIQUE
// No change.
//
//    Rev 1.11   18 Dec 1992 13:59:00   ED
// Eliminated some strings that are no longer needed
//
//    Rev 1.10   15 Dec 1992 21:13:12   JOHN
// Removed check for carry being set on return from INT 21H AH=25H and 35H
// (Set and Get interrupt vector).  NOT ALL DOS FUNCTIONS AFFECT THE CARRY
// FLAG!  MAKE SURE THE FUNCTION IN QUESTION USES CARRY BEFORE YOU ADD CODE
// TO TEST FOR IT!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// (Can you tell that I wasted a whole day finding this one?)
//
//    Rev 1.9   15 Dec 1992 13:11:40   CRAIG
// Added DOSGetEnvironmentCase().
//
//    Rev 1.8   09 Nov 1992 16:19:58   MARKK
// Fixed bug in getinternalvars
//
//    Rev 1.7   28 Oct 1992 09:13:08   ED
// Optimized DOSGetProgramName
//
//    Rev 1.6   10 Sep 1992 18:39:04   BRUCE
// Eliminated unnecesary includes
//
//    Rev 1.5   08 Sep 1992 08:48:14   ED
// Final version of DOSGetProgramName.  Forever....
//
//    Rev 1.4   08 Sep 1992 08:04:20   ED
// Fixed
//
//    Rev 1.3   08 Sep 1992 07:52:30   ED
// Added a "legal" method of getting the instance handle for the task
// in DOSGetProgramName.  I think this is it.
//
//    Rev 1.2   03 Sep 1992 07:23:20   ED
// Added const
//
//    Rev 1.1   02 Sep 1992 11:25:38   ED
// Updated
//
//    Rev 1.10   01 Sep 1992 09:14:42   ED
// This is it, dammit!  No more DOSGetProgramName fixes.
//
//    Rev 1.9   01 Sep 1992 09:02:00   ED
// One more final fix to GetProgramName.
//
//    Rev 1.8   01 Sep 1992 08:41:32   ED
// Fixed up GetProgramName to work under Windows the way we expect it to.
// This required using some partially undocumented stuff, but that's Okay.
//
//
//    Rev 1.7   19 Aug 1992 08:20:54   ED
// The last fix to DOSGetProgramName.  It now works in protected mode!
//
//    Rev 1.6   19 Aug 1992 08:14:44   ED
// One more fix to DOSGetProgramName
//
//    Rev 1.5   19 Aug 1992 08:05:14   ED
// Fixed up DOSGetProgramName for protected mode
//
//    Rev 1.4   18 Aug 1992 09:16:46   ED
// Changed DOSGetProgName to DOSGetProgramName and fixed the SYM_WIN check
//
//    Rev 1.3   18 Aug 1992 09:15:20   ED
// Added DOSGetProgName
//
//    Rev 1.2   09 Aug 1992 15:43:48   MANSHAN
// Fixed the bug in DOSGetEnvironment() that the system hangs if one environment
// variable is a substring of another.
// Also fixed the bug that if the first letter of the variable is in a different
// case than specified, it will not be found.
//
//
//    Rev 1.1   29 Jul 1992 06:46:00   ED
// Changed a DOS3Call to INT 21h in the DOS version
//
//    Rev 1.0   20 Jul 1992 14:21:18   ED
// Initial revision.
 ************************************************************************/

/*@Library: SYMKRNL*/

#include <dos.h>
#include "platform.h"
#define INCLUDE_VWIN32
#include "disk.h"
#include "xapi.h"
#include "file.h"
#include <stdlib.h>

#include "dbcs.h"

#ifdef SYM_WIN32
#include "symkvxd.h"
#include "vxdload.h"
#endif

UINT LOCAL PASCAL _DOSGetEnvironment(LPCSTR lpszVarName, LPSTR lpszValue, UINT wSize, BYTE bCase);


/*@API:**********************************************************************
@Declaration: BOOL  DOSGetInternalVariables(BYTE FAR * FAR * lpBuffer)

@Description:
This function returns the address of the DOS "list of lists," or configuration
variable table (CVT).  This table contains almost all information needed to
locate internal tables and other information used by DOS.  The contents and
layout of the table is version-specific.  For more information, see
"Undocumented DOS," by Schulman, et. al.

@Parameters:
$lpBuffer$ Pointer that receives the far address of the LoL.

@Returns:
The return value is non-zero if there was an error.

@Comments:
The address is the real mode address.

@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#if !defined(SYM_WIN32) && !defined(SYM_OS2) && !defined(SYM_NTK) && !defined(SYM_NLM)
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DOSGetInternalVariables(BYTE FAR * FAR *buffer)
{
    auto	union REGS	regs;
    auto	struct SREGS    sregs;


    regs.h.ah = 0x52;
    Int(0x21, &regs, &sregs);
    *buffer = MAKELP(sregs.es, regs.x.bx);
    return (regs.x.ax);

#if 0
    auto        BOOL    returnValue;

    _asm
        {
        push    ds                      ; Save registers
        push    si                      ;

        mov     ah, 52h                 ; Undocumented DOS function
        DOS
        lds     si, DWORD PTR (buffer)  ; Buffer offset
        mov     [si], bx                ; Store buffer offset
        mov     [si+2], es              ; Store buffer segment
        xor     ax, ax                  ; Return no error (FALSE)

        pop     si                      ; Restore registers
        pop     ds                      ;
        mov     returnValue, ax         ; Get the return value
        }

    return(returnValue);
#endif
}
#pragma optimize("", on)
#endif                                  // #if !defined(SYM_WIN32) && !defined(SYM_OS2) && !defined(SYM_NTK) && !defined(SYM_NLM)


#if !defined(SYM_WIN32) && !defined(SYM_NTK) && !defined(SYM_NLM)

/*@API:**********************************************************************
@Declaration: UINT  DOSGetEnvironment (LPCSTR lpszVarName, LPSTR lpszValue, UINT
uiSize)

@Description:
This function is used to return the string value of an environment variable.
In the Windows implementation, no OEM/ANSI conversions are done on variable
names or the value returned.  The values are always in the OEM character set.

@Parameters:
$lpString$ The name of the environment variable.

$lpEnvString$ Pointer to a buffer that contains the contents of the environment
variable.

$uiSize$ The maximum number of characters to return.

@Returns:
The value is NOERR if the environment variable was found. Otherwise, it is NOERR.

@See: DOSEnvSearchPathForFile DoEnvSubst

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
UINT SYM_EXPORT WINAPI DOSGetEnvironment(LPCSTR lpszVarName, LPSTR lpszValue, UINT wSize)
{
    return _DOSGetEnvironment(lpszVarName, lpszValue, wSize, FALSE);
}

/*------------------------------------------------------------------------*/
/* DOSGetEnvironment                                                      */
/* DOSGetEnvironmentCase                                                  */
/*    This function is used to return environment strings.                */
/*    DOSGetEnvironmentCase is case-sensitive, which is useful mainly     */
/*    in DOS apps when you want to see if you have a 'windir' variable    */
/*    with a lowercase name (i.e. you are running in a Windows DOS box).  */
/*    (Not supported in OS/2-native code)                                 */
/*                                                                        */
/*                                                                        */
/*    NOTE: This function purposely does no conversions (oem/ansi) on     */
/*          variable names or the value returned.  In the DOS environment */
/*          the names and values will be in OEM.                          */
/*                                                                        */
/*                                                                        */
/* INPUTS                                                                 */
/*    LPSTR     lpString      the string to match.                        */
/*    LPSTR     lpEnvString   on exit contains the environment string.    */
/*    UINT      wSize         size of the buffer.                         */
/*                                                                        */
/* RETURNS                                                                */
/*    NOERR     If environment variable was found                         */
/*    ERR       otherwise                                                 */
/*------------------------------------------------------------------------*/
UINT SYM_EXPORT WINAPI DOSGetEnvironmentCase(LPCSTR lpszVarName, LPSTR lpszValue, UINT wSize)
{
    return _DOSGetEnvironment(lpszVarName, lpszValue, wSize, TRUE);
}

/*------------------------------------------------------------------------*/
/* _DOSGetEnvironment                                                     */
/*                                                                        */
/* Does the actual work for both DOSGetEnvironment() and                  */
/* DOSGetEnvironmentCase().                                               */
/*------------------------------------------------------------------------*/

#pragma optimize("", off)
UINT LOCAL PASCAL _DOSGetEnvironment(LPCSTR lpszVarName, LPSTR lpszValue, UINT wSize, BYTE bCase)
{
#ifdef  SYM_OS2
    PSZ     pszValue;
    APIRET  rc;

    if (!(rc = DosScanEnv(lpszVarName, &pszValue)))
        STRNCPY(lpszValue, pszValue, wSize);
    return rc;
#else
    auto	LPSTR		lpszEnv;
    auto	LPSTR		lpszTemp;
    auto	int		nNameLen;


#ifdef  SYM_WIN
    lpszEnv = GetDOSEnvironment();      // Point to 1st "var=value" string
#else
    {
    auto    WORD        wPSP;

    wPSP = DOSGetPSP();
    lpszEnv = MAKELP(*((LPWORD) MAKELP(wPSP, 0x2C)), 0);
    }
#endif

    nNameLen = STRLEN(lpszVarName);
    while (*lpszEnv)                    // While not at end of string sequence
        {
        if (bCase? !STRNCMP(lpszEnv, lpszVarName, nNameLen): !STRNICMP(lpszEnv, lpszVarName, nNameLen))
            {
            lpszTemp = lpszEnv + nNameLen;
            while (*lpszTemp == ' ')            // Skip spaces before =
                lpszTemp++;
            if (*lpszTemp == '=')               // We found the variable
                {
                lpszTemp++;                     // Skip past =
                while (*lpszTemp == ' ')        // Skip spaces after =
                    lpszTemp++;
                STRNCPY(lpszValue, lpszTemp, wSize-1);  // Copy value after "="
                lpszValue[wSize-1] = EOS;
                lpszTemp = lpszValue + STRLEN(lpszValue);
                while (lpszTemp > lpszValue && lpszTemp[-1] == ' ')
                    lpszTemp--;
                *lpszTemp = '\0';               // Chop trailing spaces
                return (NOERR);
                }
            }

        //  8/9/92: Removed the "else" so that it can advance to the next
        //          string if lpszVarName is a substring of lpszEnv.

        lpszEnv += STRLEN(lpszEnv) + 1;
        }

    *lpszValue = '\0';                  // Did not find the variable
    return ((UINT)ERR);
#endif                                  //  #ifndef SYM_OS2
}
#pragma optimize("", on)
#endif                                 // #if !defined(SYM_WIN32) && !defined(SYM_NTK) && !defined(SYM_NLM)


/*@API:**********************************************************************
@Declaration: BYTE DOSGetDate (UINT FAR * lpMonth, UINT FAR * lpDay, UINT
FAR * lpYear)

@Description:
This function returns the date value maintained by DOS.  This function does not
compensate for the DOS "midnight flag" bug, where systems left running for an
extended period of time can lose one or more days.

@Parameters:
$lpMonth$ Points to the variable that is to receive the month.

$lpDay$ Points to the variable that is to receive the day.

$lpYear$ Points to the variable that is to receive the year.

@Returns:
The return value is the day of the week.

@See: DOSSetDate DateGetCurrentDateTime

@Include: xapi.h

@Compatibility: DOS, Win16, Win32, and NTK
*****************************************************************************/
#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI DOSGetDate (UINT FAR * monthPtr, UINT FAR * dayPtr, UINT FAR * yearPtr)
{
#if defined(SYM_NTK)

    auto LARGE_INTEGER CurrentTime;
    auto TIME_FIELDS   TimeFields;

    SYM_ASSERT ( monthPtr );
    SYM_ASSERT ( dayPtr );
    SYM_ASSERT ( yearPtr );

    KeQuerySystemTime ( &CurrentTime );
    RtlTimeToTimeFields ( &CurrentTime, &TimeFields );

    *monthPtr = TimeFields.Month;
    *dayPtr   = TimeFields.Day;
    *yearPtr  = TimeFields.Year;

    return ((BYTE)TimeFields.Weekday);

#elif defined(SYM_OS2)
    struct dosdate_t     ddate;

    _dos_getdate(&ddate);
    *monthPtr = (UINT) ddate.month;
    *dayPtr   = (UINT) ddate.day;
    *yearPtr  =        ddate.year;
    return((BYTE)ddate.dayofweek);
#elif defined(SYM_WIN32)
    SYSTEMTIME  systemTime;


    GetLocalTime(&systemTime);
    *monthPtr = systemTime.wMonth;
    *dayPtr   = systemTime.wDay;
    *yearPtr  = systemTime.wYear;
    return((BYTE)systemTime.wDayOfWeek);
#elif defined(SYM_NLM)

    time_t t;           // numeric time
    struct tm tm;       // componentized time

    time(&t);
    localtime(&t);

    *monthPtr = tm.tm_mon + 1;      // 1..12
    *dayPtr = tm.tm_mday;           // 1..31
    *yearPtr = tm.tm_year + 1900;   // 1900...
    return tm.tm_wday + 1;          // 1..7

#else

    auto        BYTE    returnValue;


    _asm
        {
        push    ds                      ; Save registers
        push    si                      ;

        mov     ah, 2Ah                 ; Request DOS date
        DOS
        lds     SI,yearPtr
        MOV     [SI],CX                         ;Store the year

        MOV     CX,DX
        MOV     CL,CH                           ;Put month in CL
        XOR     CH,CH                           ;Set upper nybble to 0
        lds     SI,monthPtr
        MOV     [SI],CX

        lds     SI,dayPtr
        XOR     DH,DH                   ;Leave just day in DX
        MOV     [SI],DX

        pop     si                      ; Restore registers
        pop     ds                      ;
        mov     returnValue, al         ; Return day/week
        }

    return(returnValue);

#endif                                  // #ifndef SYM_OS2
}
#pragma optimize("", on)

#if !defined(SYM_NTK) && !defined(SYM_NLM)
/*@API:**********************************************************************
@Declaration: BYTE  DOSSetDate (UINT wMonth, UINT wDay, UINT wYear)

@Description:
This function sets the date value maintained by DOS.  DOS automatically updates
CMOS clocks on systems that have CMOS.

@Parameters:
$wMonth$ The month value to set (1-12).

$wDay$ The day value to set (1-31).

$wYear$ The year value to set (1980-2099).

@Returns:
The return value is non-zero if there was an error. \n

@Comments:
This function also allows year values less than 1980.  A value less than 1980
but greater than 80 is added to 1900.  Values less than 80 is added to 2000.

@See: DOSGetDate DateGetCurrentDateTime

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
VOID SYM_EXPORT WINAPI DOSSetDate (UINT month, UINT day, UINT year)
{
#if defined(SYM_OS2)
    struct dosdate_t     ddate;

    ddate.month          = (char) month;
    ddate.day            = (char) day;
    ddate.year           = year;

    _dos_setdate(&ddate);
#else
#if defined(SYM_WIN32)
    SYSTEMTIME  systemTime;


    GetLocalTime(&systemTime);         // Get current settings
    systemTime.wMonth   = month;
    systemTime.wDay     = day;
    systemTime.wYear    = year;
    SetLocalTime(&systemTime);         // Set new settings

#else

    _asm
        {
        MOV     CX,year                         ;Get the year
        CMP     CX,1980
        JAE     YEAROK
        CMP     CX,80
        JAE     DO1900
        ADD     CX,2000
        JMP     SHORT YEAROK
DO1900:
        ADD     CX,1900
YEAROK:
        MOV     DH,byte ptr month               ;Get the month
        MOV     DL,byte ptr day                 ;Get the day
        MOV     AH,2BH                          ;Set the date
        DOS
        }
#endif                                  // #ifndef SYM_WIN32
#endif                                  // #ifndef SYM_OS2

}
#pragma optimize("", on)

/*@API:**********************************************************************
@Declaration: void  DOSSetTime (UINT wHour, UINT wMinutes, UINT wSeconds, UINT
wHundreths)

@Description:
This function sets the time value maintained by DOS.  DOS automatically updates
CMOS clocks on systems that have CMOS.

@Parameters:
$wHour$ The hour value to set (0-23).

$wMinutes$ The minutes value to set (0-59).

$wSeconds$ The seconds value to set (0-59).

$lpwHundreths$ The hundreths of seconds to be set (0-99).

@See: DOSSetTime DateGetCurrentDateTime
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DOSSetTime (UINT hour, UINT minutes, UINT seconds, UINT hundredths)
{
#if defined(SYM_OS2)
    struct dostime_t         stime;

    stime.hour          = (unsigned char) hour;
    stime.minute        = (unsigned char) minutes;
    stime.second        = (unsigned char) seconds;
    stime.hsecond       = (unsigned char) hundredths;

    _dos_settime(&stime);
#else
#if defined(SYM_WIN32)
    SYSTEMTIME  systemTime;


    GetLocalTime(&systemTime);         // Get current settings
    systemTime.wHour    = hour;
    systemTime.wMinute  = minutes;
    systemTime.wSecond  = seconds;
    systemTime.wMilliseconds = hundredths / 10;
    SetLocalTime(&systemTime);         // Set new settings

#else

    _asm
        {
        MOV     CH,byte ptr hour        ;Get the hour
        MOV     CL,byte ptr minutes     ;Get the mintutes
        MOV     DH,byte ptr seconds     ;Get the seconds
        MOV     DL,byte ptr hundredths  ;Get the hundredths
        MOV     AH,2DH                  ;set the time
        DOS
        }

#endif                                  // #ifndef SYM_WIN32
#endif                                  // #ifndef SYM_OS2
    return (0);
}
#pragma optimize("", on)
#endif                                  // #if !defined(SYM_NTK) && !defined(SYM_NLM)

/*@API:**********************************************************************
@Declaration: void  DOSGetTime (UINT FAR * lpwHour, UINT FAR * lpwMinutes, UINT
FAR * lpwSeconds, UINT FAR * lpwHundreths)

@Description:
This function returns the time value maintained by DOS.

@Parameters:
$lpwHour$ Points to the variable that is to receive the hour.

$lpwMinutes$ Points to the variable that is to receive the minutes.

$lpwSeconds$ Points to the variable that is to receive the seconds.

$lpwHundreths$ Points to the variable that is to receive the hundreths of
seconds.

@See: DOSSetTime DateGetCurrentDateTime
@Include: xapi.h
@Compatibility: DOS, Win16, Win32, and NTK
*****************************************************************************/
#pragma optimize("", off)
VOID SYM_EXPORT WINAPI DOSGetTime (UINT FAR *hourPtr, UINT FAR *minPtr,
                        UINT FAR *secPtr, UINT FAR *hundPtr)
{
#if defined(SYM_NTK)

    auto LARGE_INTEGER CurrentTime;
    auto TIME_FIELDS   TimeFields;

    SYM_ASSERT ( hourPtr );
    SYM_ASSERT ( minPtr );
    SYM_ASSERT ( secPtr );
    SYM_ASSERT ( hundPtr );

    KeQuerySystemTime ( &CurrentTime );
    RtlTimeToTimeFields ( &CurrentTime, &TimeFields );

    *hourPtr = TimeFields.Hour;
    *minPtr  = TimeFields.Minute;
    *secPtr  = TimeFields.Second;
    *hundPtr = TimeFields.Milliseconds;

#elif defined(SYM_OS2)
    struct dostime_t         stime;
    _dos_gettime(&stime);

    *hourPtr = (UINT) stime.hour;
    *minPtr  = (UINT) stime.minute;
    *secPtr  = (UINT) stime.second;
    *hundPtr = (UINT) stime.hsecond;

#elif defined(SYM_WIN32)

    SYSTEMTIME  systemTime;


    GetLocalTime(&systemTime);
    *hourPtr = systemTime.wHour;
    *minPtr  = systemTime.wMinute;
    *secPtr  = systemTime.wSecond;
    *hundPtr = systemTime.wMilliseconds * 10;

#elif defined(SYM_NLM)

    time_t t;           // numeric time
    struct tm tm;       // componentized time

    time(&t);
    localtime(&t);

    *hourPtr = tm.tm_hour;          // 0..23
    *minPtr = tm.tm_min;            // 0..59
    *secPtr = tm.tm_sec;            // 0..59
    *hundPtr = 0;                   // 0..99

#else

    _asm
        {
        MOV     AH,2CH                  ;Request the time
        INT     21h
        PUSH    DS
        MOV     BX,CX
        MOV     AX,DX
        XOR     DH,DH                           ;Always have upper nybble 0
        MOV     DL,BH                           ;Put hours in DX
        LDS     SI,hourPtr
        MOV     [SI],DX                         ;Save the hour
        MOV     DL,BL                           ;Put minutes in DX
        LDS     SI,minPtr
        MOV     [SI],DX                         ;Save the minutes
        MOV     DL,AH                           ;Put seconds into DX
        LDS     SI,secPtr
        MOV     [SI],DX                         ;Save seconds
        MOV     DL,AL                           ;Put 1/100 seconds into DX
        LDS     SI,hundPtr
        MOV     [SI],DX
        POP     DS
        }

#endif                                  // #ifndef SYM_OS2
}
#pragma optimize("", on)

#if !defined(SYM_OS2) && !defined(SYM_NTK) && !defined(SYM_NLM)
#ifndef SYM_WIN32

/*@API:**********************************************************************
@Declaration: UINT  DOSGetPCMOSVersion ()

@Description:
This function returns the PCMOS version number. \n

This function has no parameters.

@Returns:
The return value is the PCMOS version number (for example, 300h for 3.0) if
PCMOS is running.  Otherwise the value is zero.

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DOSGetPCMOSVersion(VOID)
{
    auto        UINT    dosVersion;
    auto        UINT    returnValue = 0;

    dosVersion = DOSGetVersion();
                                        /*------------------------------*/
                                        /* Get PC-MOS Version #         */
                                        /*------------------------------*/
    _asm
        {
        mov     ax, 3000h               ; Get PC-MOS version number
        mov     bx, ax                  ;
        mov     cx, ax                  ;
        mov     dx, ax                  ;
        DOS
        xchg    ah, al                  ; Change order

        cmp     dosVersion, ax          ; Are they the same?
        je      locRet                  ; Yes, not PC-MOS

        mov     returnValue, ax         ; Save the version number
locRet:
        }

    return(returnValue);
}
#pragma optimize("", on)
#endif                                  // #ifndef SYM_WIN32


#ifndef SYM_WIN32
/*----------------------------------------------------------------------*/
/* DOSGetDRDOSVersion                                                   */
/*      Returns the DRDOS version number.                               */
/*                                                                      */
/* This routine was copied from CRT0.ASM of the norton library          */
/*                                                                      */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DOSGetDRDOSVersion(VOID)
{
    UINT                wVersion = 0;
                                        /*------------------------------*/
                                        /* Get DR-DOS Version #         */
                                        /*------------------------------*/
#ifndef SYM_PROTMODE
    _asm
        {
        mov     ax, 4452h               ;From Ralf Brown's PC Interrupts
	stc			        ;text file (not in the book!)
        int     21h
        jc      Not_DRDOS               ; Not DR-DOS
        mov     wVersion, ax
        }
#else
    union REGS          regs;
    struct SREGS        sregs;

    regs.x.cflag = TRUE;
    regs.x.ax = 0x4452;
    IntWin (0x21, &regs, &sregs);

    if (regs.x.cflag == TRUE)
        goto Not_DRDOS;

    wVersion = regs.x.ax;
#endif

    switch (wVersion)
        {
        case 0x1063: wVersion = 0x341; break;   // v 3.41
        case 0x1065: wVersion = 0x500; break;   // v 5.0
        case 0x1067: wVersion = 0x600; break;   // v 6.0
        case 0x1072: wVersion = 0x700; break;   // v 7.0
        default:     wVersion = 0x100; break;   // Unknown (we'll say v 1.0)
        }

Not_DRDOS:
    return (wVersion);
}
#pragma optimize("", on)
#endif                                  // #ifndef SYM_WIN32


/*@API:**********************************************************************
@Declaration: UINT  DOSGetVersion ()

@Description:
This function returns the DOS version number. \n

@Returns:
The return value is the DOS version number (i.e., 300h for 3.0) which is one of
the following constants:
   ~DOS_3_00~ DOS 3.0
   ~DOS_3_10~ DOS 3.1
   ~DOS_3_20~ DOS 3.2
   ~DOS_3_21~ DOS 3.21
   ~DOS_3_30~ DOS 3.3
   ~DOS_3_31~ DOS 3.31
   ~DOS_4_00~ DOS 4.0
   ~DOS_5_00~ DOS 5.0
   ~DOS_6_00~ DOS 6.0
   ~DOS_OS2_1_00~ OS/2 1.0
   ~DOS_OS2_2_00~ OS/2 2.0
   ~DOS_OS2_2_10~ OS/2 2.1

@See: DOSGetPCMOSVersion DOSGetOEMNumber
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
#if defined(SYM_WIN)
UINT SYM_EXPORT WINAPI DOSGetVersion(VOID)
{
    static      UINT    uDOSVer = 0;
                                        // Only make Int21 call once!
    if (uDOSVer)
        return(uDOSVer);

#if defined(SYM_WIN32)

#ifndef _M_ALPHA
    if(SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
#endif    
        uDOSVer = DOS_7_00;
#ifndef _M_ALPHA
    else
        {
        auto        SYM_REGS        regs;

        MEMSET(&regs, 0, sizeof(SYM_REGS));

        regs.EAX = 0x00003000;
        VxDIntBasedCall( 0x21, &regs );

        uDOSVer = (((regs.EAX & 0xFF00) >> 8) |
                   ((regs.EAX & 0x00FF) << 8));
        }
#endif        

#else

    _asm
        {
        mov     ah, 30h                 ; Get DOS Version
        DOS
        xchg    ah, al                  ; Swap order
        mov     uDOSVer, ax         ; Get return value
        }

#endif
    return(uDOSVer);
}
#endif
#if defined(SYM_DOS) || defined(SYM_X86)
UINT SYM_EXPORT WINAPI DOSGetVersion(VOID)
    {
    static      UINT    uDOSVer = 0;
    auto        short   uTemp;
                                        // Only make Int21 call once!
    if (uDOSVer)
        return (uDOSVer);

    _asm
        {
        push    si
        push    di
        push    es
        push    ds
        push    bp

        mov     ax, 03000h              ; Get DOS Version
        DOS
        xchg    ah, al                  ; Swap order
        mov     [uTemp], ax             ; Get return value

        pop     bp
        pop     ds
        pop     es
        pop     di
        pop     si
        }

    uDOSVer = uTemp;

    return (uDOSVer);
    }
#endif
#pragma optimize("", on)


#ifndef SYM_WIN32
/*----------------------------------------------------------------------*/
/* DOSOEMNumber                                                         */
/*      Returns the OEM DOS version.                                    */
/*----------------------------------------------------------------------*/

/*@API:**********************************************************************
@Declaration: BYTE DOSOEMNumber()

@Description:
This function returns the OEM DOS version.

@Returns:
The return value is the OEM DOS version.

@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI DOSOEMNumber(VOID)
{
    auto        BYTE    returnValue;


    _asm
        {
        mov     ax,3000h                ; Get DOS version number
        xor     bx,bx
        xor     cx,cx
        xor     dx,dx
        DOS
        mov     returnValue, bh         ; Get OEM number
        }

    return(returnValue);
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*/
/* DOSSetBreakStatus                                                    */
/*      Sets the DOS BREAK state.                                       */
/*                                                                      */
/* Flag should be:                                                      */
/*      0       Turn checking off                                       */
/*      1       Turn checking on                                        */
/*----------------------------------------------------------------------*/

/*@API:**********************************************************************
@Declaration: void  DOSSetBreakStatus(BYTE bFlag)

@Description:
This function sets the status of the Ctrl-Break/Ctrl-C flag.  Break checking is
not normally done during most DOS functions except for a few I/O functions.
Setting the flag to 1 causes DOS to check for Ctrl-Break/Ctrl-C during all
DOS functions.

@Parameters:
$bFlag$ Determines the flag state, 1 is on, 0 is off. Don't use TRUE or FALSE
since this function does not refer to these defines (and it is
conceivably possible that these two defines may change).

@Comments:
The Break flag affects all of DOS, not just the current process.  The current
state of the flag should be saved before calling this function, and restored
before the process terminates.

@See: DOSGetBreakStatus
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
VOID SYM_EXPORT WINAPI DOSSetBreakStatus(BYTE flag)
{
    _asm
        {
        MOV     DL,flag                 ;Get the new flag
        MOV     AX,3301h                ;Set checking state
        DOS
        }
}
#pragma optimize("", on)


/*@API:**********************************************************************
@Declaration: BOOL  DOSGetBreakStatus()

@Description:
This function returns the status of the Ctrl-Break/Ctrl-C flag.

@Returns:
The return value is the current state of the break flag. 1 for on, 0 for off.

@See: DOSSetBreakStatus
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI DOSGetBreakStatus(VOID)
{
    auto        BYTE    returnValue;


    _asm
        {
        MOV     AX,3300h                ;Ask for current state
        DOS
        MOV     returnValue,DL          ; Get return value
        }

    return(returnValue);
}
#pragma optimize("", on)



/*@API:**********************************************************************
@Declaration: BOOL  DOSGetIntHandler(BYTE IntNum, BYTE FAR * FAR * lpHandler)

@Description:
This function returns the address of the handler for the specified interrupt.
In the Windows implementation, the address of the real mode interrupt handler
is returned, not the protected mode handler.

@Parameters:
$IntNum$ The interrupt to return.

$lpHandler$ Pointer to a far pointer that is to receive the address of the
interrupt handler.

@Returns:
The return value is non-zero if there was an error.  Otherwise it is zero.

@See: DOSSetIntHandler
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DOSGetIntHandler(BYTE intNum, BYTE FAR * FAR *intHandlerPtr)
{
    auto        BOOL    returnValue;
    auto        UINT    intSeg, intOff;

#ifdef  SYM_PROTMODE
                                        /*------------------------------*/
                                        /* If Protected mode, use DPMI  */
                                        /* to get real mode vector      */
                                        /*------------------------------*/
    _asm
        {
        mov     ax, 0200h               ; Get Real Mode Interrupt Vector
        mov     bl, intNum              ; Get interrupt
        int     31h                     ; DPMI function call
        jc      err                     ; If error, return ERROR

        mov     intSeg, cx              ; Save segment
        mov     intOff, dx              ; Save offset

        xor     ax, ax                  ; Return NO ERROR
        jmp     short locRet            ; Return to caller

err:
        mov     ax, 1                   ; Return ERROR

locRet:
        mov     returnValue, ax         ; Save the return value
        }

#else

    _asm
        {
        mov     ah, 35h                 ; Get Real Mode Interrupt Vector
        mov     al, intNum              ; Get interrupt
        int     21h                     ; DOS function call

        mov     intSeg, es              ; Save segment
        mov     intOff, bx              ; Save offset

        xor     ax, ax                  ; Return NO ERROR
        mov     returnValue, ax         ; Save the return value
        }

#endif

                                        /*------------------------------*/
                                        /* Save the return values       */
                                        /*------------------------------*/
    *intHandlerPtr = MAKELP(intSeg, intOff);
    return(returnValue);
}
#pragma optimize("", on)


/*@API:**********************************************************************
@Declaration: BOOL  DOSSetIntHandler(BYTE IntNum, LPBYTE lpHandler)

@Description:
This function sets the address of the handler for the specified interrupt.  In
the Windows implementation, the address of the real mode interrupt handler is
set, not the protected mode handler.

@Parameters:
$IntNum$ The interrupt to set.

$lpHandler$ Pointer to the address of the new interrupt handler.  Under
Windows, this must be a real mode accessible address.

@Returns:
The return value is non-zero if there was an error.  Otherwise it is zero.

@See: DOSGetIntHandler
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DOSSetIntHandler (BYTE intNum, const LPBYTE intHandler)
{
    auto        BOOL    returnValue;

#ifdef  SYM_PROTMODE
                                        /*------------------------------*/
                                        /* If Protected mode, use DPMI  */
                                        /* to set real mode vector      */
                                        /*------------------------------*/
    _asm
        {
        mov     cx, word ptr intHandler
        mov     dx, word ptr intHandler + 2
        mov     bl, intNum              ; Get interrupt
        mov     ax, 0201h               ; Set Real Mode Interrupt Vector
        int     31h                     ; DPMI function call
        jc      err                     ; If error, return ERROR
        xor     ax, ax                  ; Return NO ERROR
        jmp     short locRet            ; Return to caller
err:
        mov     ax, 1                   ; Return ERROR
locRet:
        mov     returnValue, ax         ; Save the return value
        }

#else

    _asm
        {
        push    ds                      ; We need to save DS
        lds     dx, dword ptr intHandler
        mov     ah, 25h                 ; Set Real Mode Interrupt Vector
        mov     al, intNum              ; Get interrupt
        int     21h                     ; DOS function call
        pop     ds                      ; Restore DS
        xor     ax, ax                  ; Return NO ERROR
        mov     returnValue, ax         ; Save the return value
        }

#endif

    return(returnValue);
}
#pragma optimize("", on)

/*@API:**********************************************************************
@Declaration: void  DOSGetErrorInfo(UINT FAR * lpwExtendedError, LPBYTE
lpbyErrorClass, LPBYTE lpbySuggestedAction, LPBYTE lpbyLocus)

@Description:
This function returns extended error information that details the failure of a
DOS function.  The values returned by this function are updated after most DOS
function calls, so this function should be called immediately an error.  This
ensures that the values correspond to the current error condition. \n

For detailed information on the return values, consult a DOS reference guide
for INT 21h, function 59h.

@Parameters:
$lpwExtendedError$ Pointer to the variable that is to receive the error code
(AX register).

$lpbyErrorClass$ Pointer to the variable that is to hold the error class (BH
register).

$lpbySuggestedAction$ Pointer to the variable that is to hold the recommended
action (BL register).

$lpbyLocus$ Pointer to the variable that is to hold the error locus (CH
register).

@See: DOSGetExtendedError NDosSetError NDosGetError
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
VOID SYM_EXPORT WINAPI DOSGetErrorInfo(UINT FAR * wExtendedError, LPBYTE byErrorClass,
                                    LPBYTE bySuggestedAction, LPBYTE byLocus)
{
    _asm
        {
        push    ds                      ; Save registers
        push    si                      ;

        mov     ah, 59h                 ; Get EXTENDED ERROR function
        xor     bx, bx                  ; Version #
        DOS
        lds     si, wExtendedError      ; Get the Extended Error
        mov     [si], ax                ;

        lds     si, byErrorClass        ; Get the Error Class
        mov     [si], bh                ;

        lds     si, bySuggestedAction   ; Get the Suggested Action
        mov     [si], bl                ;

        lds     si, byLocus             ; Get the Locus
        mov     [si], ch                ;

        pop     si                      ; Restore registers
        pop     ds                      ;
        }
}
#pragma optimize("", on)

#endif                                  // #ifndef SYM_WIN32

/*@API:**********************************************************************
@Declaration: UINT  DOSGetExtendedError ()

@Description:
This function returns the extended error code that corresponds to the failure
of a DOS function.  The value returned by this function is updated after most
DOS function calls, so this function should be called immediately an error.
This ensures that the value corresponds to the current error condition. \n

For detailed information on error codes, consult a DOS reference guide for INT
21h, function 59h.  Also, see DOSGetErrorInfo. \n

This function has no parameters.

@Returns:
The return value is the current error code (AX register).

@See: DOSGetErrorInfo NDosSetError NDosGetError

@Include: xapi.h

@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
UINT SYM_EXPORT WINAPI DOSGetExtendedError(VOID)
{
#if defined(SYM_WIN32)
    return ( (UINT) GetLastError() );
#else
    auto        UINT            wExtendedError;
    auto        BYTE            byErrorClass;
    auto        BYTE            bySuggestedAction;
    auto        BYTE            byLocus;


    DOSGetErrorInfo(&wExtendedError, &byErrorClass, &bySuggestedAction, &byLocus);

    return(wExtendedError);
#endif
}


//--------------------------------------------------------------------
//
//
//
//--------------------------------------------------------------------

/*@API:**********************************************************************
@Declaration: void DOSGetProgramName (LPSTR lpString)

@Description:
This function gets the current program name from the environment
space.  It returns a file name that is, at most, MAX_PATH_NAME+1
(including the terminating null).

@See: DOSGetErrorInfo NDosSetError NDosGetError
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#pragma optimize("", off)
void SYM_EXPORT WINAPI DOSGetProgramName (LPSTR lpString)
{
#if defined(SYM_WIN32)

    LPSTR pSrc, pDest;

    pSrc = GetCommandLine();
    pDest = lpString;
    while (*pSrc != '\0' && *pSrc != ' ' && *pSrc != '\t')
        {
        if (DBCSIsLeadByte(*pSrc))
            *pDest++ = *pSrc++;
        *pDest++ = *pSrc++;
        }
    *pDest = '\0';

#elif defined(SYM_WIN)

    HINSTANCE           hInst;

    *lpString = '\0';                   // clear the string

        // Convert the stack segment into an instance handle.  we use the
        // stack because it belongs to the app that called us, while DS
        // belongs to our DLL.

    _asm        push    ss
    _asm        call    GlobalHandle
    _asm        mov     hInst, ax

    if (hInst)                          // get the module filename
        GetModuleFileName (hInst, lpString, SYM_MAX_PATH);

#else
    LPSTR       lpPSP;
    LPSTR       lpCommand;

    // In DOS 3.0+, the program name is located at the end of the environment.
    // This doesn't work for Windows (at least in a DLL), because DLLs share
    // the PSP of KERNEL.

    lpPSP = MAKELP(DOSGetPSP(), 0);
                                        // get the address of the environment
    lpCommand = MAKELP (* (LPWORD) &lpPSP [0x2C], 0);

    *lpString = '\0';                   // blank the string

    if (DOSGetVersion() >= DOS_3_00)
        {
        _asm
        {
        les     di,lpCommand            // point to the environment
        mov     cx,8000h                // Look through at most 32K
        xor     di,di                   // Set to start of environment
find_end_loop:
        xor     al,al                   // Look for 0 byte
        repne   scasb                   // We found zero byte
        jcxz    Not_Tonight_Dear        // Passed end of environment, return ""
        cmp     byte ptr es:[di],0      // Do we have a second 0 (end of env.)?
        jne     find_end_loop           // No, then look for another 0

        inc     di                      // Move to the word count
        add     di,2                    // Skip over the word count
        mov     word ptr lpCommand, di  // store the result
        }

        STRCPY (lpString, lpCommand);
Not_Tonight_Dear:;
        }
#endif
                                        // NetWare (and others) allow you
                                        // to load using forward slashes.
                                        // Convert to what DOS likes to
                                        // avoid problems later.
    NameConvertFwdSlashes (lpString);
}

#pragma optimize("", on)
#endif // #if !defined(SYM_OS2) && !defined(SYM_NTK) && !defined(SYM_NLM)
