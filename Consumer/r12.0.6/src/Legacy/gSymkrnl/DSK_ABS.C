/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/dsk_abs.c_v   1.22   21 Aug 1998 16:37:20   MBROWN  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      DiskAbsBuffer* routines                                         *
 *      DiskAbsOperation                                                *
 *      DiskGetSectorStartOffset                                        *
 *      DiskGetSectorEndOffset                                          *
 *      DiskRecToFloppyAbsDiskRec                                       *
 *      DiskGetPhysicalInfo                                             *
 *      ConvertPhySectorToLong                                          *
 *      ConvertLongToPhySector                                          *
 *      DiskUnscrambleBIOSCoding                                        *
 *      DiskScrambleBIOSCoding                                          *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/dsk_abs.c_v  $ *
// 
//    Rev 1.22   21 Aug 1998 16:37:20   MBROWN
// Ported bug fixes from L branch
// 
//    Rev 1.21   18 Aug 1998 16:49:46   mdunn
// Ported Atomic changes from N branch
// 
//    Rev 1.20   19 May 1998 18:30:58   mbrown
// Fixed 2GB limit of NT block of DiskAbsOperation
// 
//    Rev 1.19   11 May 1998 18:26:28   SKURTZ
// Test to prevent writing to a null pointer
//
//    Rev 1.18   17 Apr 1998 16:52:18   MBROWN
// More large drive support fixes
//
//    Rev 1.17   03 Apr 1998 15:43:06   MBROWN
// Using more sophisticated routine to produce translated values
//
//    Rev 1.16   30 Mar 1998 13:52:18   MBROWN
// When using arbitrary translation values, set flag in ABSDISKREC
//
//    Rev 1.15   26 Mar 1998 13:36:06   MBROWN
// Changed 16-bit DiskGetPhysicalInfo so that we always use correct sec/track when we use our own translation scheme.
//
//    Rev 1.14   11 Mar 1998 15:05:30   SKURTZ
// Numerous changes for Extended Int13 support
//
//    Rev 1.13   17 Oct 1997 18:49:34   MBROWN
// Commented out int13 extionsion code in DiskGetPhsyicalInfo. Use regular int13
// for now.
//
//    Rev 1.12   13 Oct 1997 15:21:12   MBROWN
// Updated PMODE ver of _DiskGetPhysicalInfoEx to handle large drives supporting
// extended int13 services.
//
//    Rev 1.11   10 Aug 1997 15:16:34   bsobel
// Changed to dynamically load thunk dll on win32 only when accessed for the first time.
//
//    Rev 1.10   13 Jun 1997 13:08:00   MKEATIN
// Fixed comments.
//
//    Rev 1.9   13 Jun 1997 13:03:12   MKEATIN
// _DiskGetPhysicalInfoEx() no longer returns DISK_PHYSICAL_ERROR for
// floppy drives in DOS if it succeeds.
//
//    Rev 1.8   12 May 1997 13:08:48   RStanev
// #ifdef-ed out ior.h for SYM_VXD.  It is now included by platvxd.h
//
//    Rev 1.7   02 May 1997 10:32:48   MBROWN
// Folded in fix from QAKI where DiskGetPhysicalInfoEx now checks values returned
// from symkrnl.vxd to make sure call actually returned valid data. This addresses
// crash on NEC machines with MO drives.
//
//    Rev 1.6   19 Mar 1997 21:40:04   BILL
// Modified files to build properly under Alpha platform
//
//    Rev 1.5   18 Dec 1996 14:13:20   MLOPEZ
// Fix for defect #76260, bSuccess should be FALSE if DeviceIoControl returns an error in result
//
//    Rev 1.4   11 Dec 1996 18:01:50   SKURTZ
// Merging in NEC changes from Quake C
//
//    Rev 1.3   04 Dec 1996 05:54:04   FCHAPA
// Code cleanup for _DiskGetPhysicalInfoEx()"
//
//    Rev 1.2   21 May 1996 14:59:42   MRIVERA
// Gathered the number of physical drives in _DiskGetPhysicalInfoEx for NT.
//
//    Rev 1.1   08 Feb 1996 17:56:40   RSTANEV
// Added SYM_NTK support.
//
//    Rev 1.0   26 Jan 1996 20:22:26   JREARDON
// Initial revision.
//
//    Rev 1.64   18 Jan 1996 14:35:58   RANSCHU
// Temporarily disabled NEC specific code added in previous delta.
//
//    Rev 1.63   18 Jan 1996 14:30:22   RANSCHU
// WIN platform returned err (which was 0) instead of DISK_PHYSICAL_ERROR.
//
//    Rev 1.62   13 Dec 1995 14:06:48   DBUCHES
// 32 Bit FAT work in progress.
//
//    Rev 1.61   25 Sep 1995 17:12:24   DALLEE
// DiskAbsOperation() for NT - needed to convert return from
// ConvertPhySectorToLong() from sectors to bytes before seeking.
//
//    Rev 1.60   27 Jul 1995 14:09:20   BILL
// Merged branch changes to trunk
//
//    Rev 1.59.1.1   11 Jul 1995 20:19:20   RSTANEV
// Fixed _DiskGetPhysicalInfoEx() to return correct value for CMOS_Type.
//
//    Rev 1.59.1.0   27 Jun 1995 18:50:24   RJACKSO
// Branch base for version QAK8
//
//    Rev 1.59   25 May 1995 14:01:00   HENRI
// Added DiskGetPhysicalInfoEx (to provide translated vs. actual disk geometry)
//
//    Rev 1.58   25 Apr 1995 18:09:00   RSTANEV
// Fixed a VxD compilation error.
//
//    Rev 1.57   25 Apr 1995 17:11:36   BRAD
// Must pass a variable for the return size for NT
//
//    Rev 1.56   25 Apr 1995 12:41:02   BRAD
// Added NT support
//
//    Rev 1.55   13 Mar 1995 15:47:58   HENRI
// In fail-safe mode use thunks for INT 13h
//
//    Rev 1.54   08 Mar 1995 16:23:20   RSTANEV
// DiskGetPhysicalInfo() now supported for VxD platform.
//
//    Rev 1.53   06 Mar 1995 13:46:28   BRAD
// Need to call DiskGetPhysicalInfo() before DiskAbsOperation()
//
//    Rev 1.52   06 Mar 1995 13:36:42   BRAD
// Just addeded comment to call DiskGetPhysicalInfo() before calling
// DiskAbsOperation().
//
//    Rev 1.51   05 Mar 1995 12:26:50   HENRI
// DiskGetPhysicalInfo is now using SYMKRNL.VXD to obtain physical info (includes CMOS fix)
//
//    Rev 1.50   27 Feb 1995 15:29:54   DBUCHES
// Fixed DiskAbsOperation().  It will now function correctly on a
// Windows 95 system running MSDOS filesystem compatibility (no
// 32bit disk drivers).
//
//    Rev 1.49   02 Feb 1995 17:56:34   BRAD
// Added SYMKRNL VxD changes from QAK6
//
//    Rev 1.48   30 Jan 1995 18:38:20   RSTANEV
// Error conditions in DiskAbsOperation() for W32 now return 0xFF instead
// of FALSE.
//
//    Rev 1.47   06 Jan 1995 20:14:32   RSTANEV
// DiskGetPartitionTableEntry() now returns FALSE if IO buffer could not
// be allocated.
//
//    Rev 1.46   09 Dec 1994 20:52:30   RSTANEV
// Excluded some functions from VxD compilations.
//
//    Rev 1.45   09 Dec 1994 17:05:56   BRAD
// Added SYM_VXD support
//
//    Rev 1.44   09 Dec 1994 16:47:34   BILL
// Updated SystemIsNT
//
//    Rev 1.43   22 Nov 1994 16:27:48   BRAD
// Added WinNT abs. disk read/write
//
//    Rev 1.42   15 Nov 1994 15:40:30   BILL
// Drives expanded to 16
//
//    Rev 1.41   11 Nov 1994 18:15:00   BILL
// Changed valid drives to 80-83 from 80-81
//
//    Rev 1.40   14 Oct 1994 18:50:00   BRAD
// Buffers for absDisk operations are now virtual.  Correct for NEC
//
//    Rev 1.39   04 Oct 1994 17:24:54   BRAD
// We renamed NEC errors with the prefix 'NEC_'
//
//    Rev 1.38   04 Oct 1994 17:09:00   BRAD
// Added NEC support
//
//    Rev 1.37   17 Aug 1994 19:59:08   DBUCHES
// Fixed DiskAbsOperation().  Durring a write operation, the input
// buffer from the AbsDiskRec wasn't being copied to the buffer actually
// used for the Int13.
//
//    Rev 1.36   27 Jul 1994 16:17:24   DBUCHES
// Added thunk for DiskRecToFloppyAbsDiskRec
//
//    Rev 1.35   27 Jun 1994 10:17:00   HENRI
// Removed unused function (DiskAbsOperation16())
//
//    Rev 1.34   23 Jun 1994 17:21:40   HENRI
// Moved 16 bit thunk layers to a separate DLL
//
//    Rev 1.33   22 Jun 1994 12:26:10   HENRI
// Added thunking layers
//
//    Rev 1.32   06 Jun 1994 13:38:20   HENRI
// Changed the Convert BIOS encoded disk address routines
// to use WORD instead of UINTs.
//
//    Rev 1.31   26 May 1994 07:41:18   MARKK
// Change DX to PharLap
//
//    Rev 1.30   24 May 1994 14:15:54   SKURTZ
// DiskAbsOperation (under SYM_DOS) not preserving callers DS
//
//    Rev 1.29   05 May 1994 13:50:10   BRAD
// DX support
//
//    Rev 1.28   05 May 1994 10:22:34   MARKK
// DOSX work
//
//    Rev 1.27   04 May 1994 15:11:32   MFALLEN
// Incorrect switch() case:
//
//    Rev 1.26   04 May 1994 05:51:30   MARKK
// DOSX work
//
//    Rev 1.25   27 Apr 1994 16:46:46   BRAD
// Added DiskReadMBR() and DiskWriteMBR()
//
//    Rev 1.24   27 Apr 1994 15:24:58   BRAD
// added DiskGetPartitionTableEntry()
//
//    Rev 1.23   25 Apr 1994 14:50:56   SKURTZ
// (SYM_DOS)  _DiskGetPhysicalInfo() not preserving DS register for caller.
//
//
//    Rev 1.22   22 Apr 1994 18:52:48   BRAD
// Disable some functions for WIN32
//
//    Rev 1.21   22 Apr 1994 18:12:24   BRAD
// Added Win32s thunks for DiskAbsOperation() and DiskGetPhysicalInfo()
//
//    Rev 1.20   22 Apr 1994 13:27:16   BRAD
// Moved dwBufferSize, so computed if Registered absbuffer
//
//    Rev 1.19   22 Apr 1994 12:49:40   BRAD
// Added protected mode buffer to DiskAbsBuffer*
//
//    Rev 1.18   22 Apr 1994 12:04:24   BRAD
// Added DiskAbsBuffer* routines
//
//    Rev 1.17   18 Apr 1994 16:00:58   BRAD
// Buffer for DiskAbsOperation() is now virtual, rather than being a DOS buffer
//
//    Rev 1.16   14 Apr 1994 19:51:04   BRAD
// Move CriticalSectionBegin/End to IntWin, rather than all over the place
//
//    Rev 1.15   15 Mar 1994 12:32:38   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.14   04 Mar 1994 18:21:46   BRUCE
// [FIX] Fixed DiskGetHDParamTable() prototype
//
//    Rev 1.13   25 Feb 1994 15:03:30   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.11   02 Jul 1993 08:51:02   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//
//    Rev 1.10   30 Mar 1993 10:58:14   HENRI
// Fixed DiskGetHDParamTable for Windows.  Function was accessing
// a real mode pointer without first converting it to a protected
// mode ptr.
//
//    Rev 1.9   30 Mar 1993 01:31:10   BRUCE
// [FIX] Fixed prototype for DiskGetHDParamTable() to take explicitly FAR pointer
//
//    Rev 1.8   29 Mar 1993 14:49:00   DAVID
// Checkin in latest version from Quake 0100 branch.
// (Fixed DiskGetPhysicalInfo() for drives over 1024 cylinders.)
//
//    Rev 1.7.2.1   19 Mar 1993 22:30:12   JOHN
// Fixed DiskGetPhysicalInfo() for drives over 1024 cylinders.
//
//    Rev 1.7.2.0   01 Mar 1993 18:05:10   PATRICKM
// Branch base for version 0100
//
//    Rev 1.7   15 Feb 1993 21:03:50   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.6   15 Feb 1993 08:34:50   ED
// Changed the parameters of DiskLogSectorToPhySector to be far
// pointers, like they are in the prototype in DISK.H.
//
//
//    Rev 1.5   12 Feb 1993 04:27:32   ENRIQUE
// No change.
//
//    Rev 1.4   06 Nov 1992 12:45:52   HENRI
// Added DiskLogSectorToPhySector.  This is used instead
// of DiskMapLogSecToPhySec when it is not desirable to
// make an INT 13 call.
//
//    Rev 1.3   19 Oct 1992 17:02:04   BILL
// Added changes to handle const typedef
//
//    Rev 1.2   10 Sep 1992 19:05:36   BRUCE
// Deleted commented out includes
//
//    Rev 1.1   09 Sep 1992 18:18:44   BRUCE
// Eliminated dependency on nwinutil.h
//
//    Rev 1.0   27 Aug 1992 10:33:34   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>

#if defined(SYM_WIN32) || defined(SYM_VXD)
#define _DCB    Unused_DCB              // Hack to fix a typedef conflict
#define DCB     Unused_DCB
#endif

#include "platform.h"

#if defined(SYM_WIN32) || defined(SYM_VXD)
#undef  _DCB                            // Hack to fix a typedef conflict
#undef  DCB
#endif

#include "disk.h"
#include "xapi.h"
#include "thunk.h"
#if defined(SYM_WIN32) || defined(SYM_VXD)
#ifndef _M_ALPHA
#define DCB_SUPPORT
#include "dcb.h"
#endif // _M_ALPHA
#include "symkrnl.h"
#if !defined(_M_ALPHA) && !defined(SYM_VXD)
#include "ior.h"
#endif // !defined(_M_ALPHA) && !defined(SYM_VXD)
#include "symkvxd.h"
#include "vxdload.h"
#include <winioctl.h>
#include "quakever.h"
#endif

#if defined(SYM_WIN)                    // Only valid for WIN
#if defined(SYM_DEBUG)
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut) OutputDebugString(QMODULE_SYMKRNL": "szOut)
    #endif
#else
    #ifndef SYM_OUTPUT_DEBUG
        #define SYM_OUTPUT_DEBUG(szOut)
    #endif
#endif
#endif


#if defined(NCD)
BYTE SYM_EXPORT WINAPI _DiskGetPhyInfoNEC(LPABSDISKREC lpAbsDiskTranslated);
#if !defined(SYM_PROTMODE)
BYTE _DiskAbsOperationNEC(LPABSDISKREC absDiskRecordPtr, BYTE command_code);
#endif
#endif

#if defined(SYM_WIN16)
//----------------------------------------------------------------------------
// This stuff is used by the DiskAbsBuffer*() routines.  It always
// an app to register a buffer that they want to use for Abs Disk I/O.  The
// app should call Deregister upon completion, else it will leave an entry
// around as being marked as used.
//----------------------------------------------------------------------------

#define MAX_ABS_BUFFERS 64              // Can have upto x apps register
                                        // at once.
typedef struct
    {
    HINSTANCE   hInstance;
    LPVOID      lpRealBuffer;
    LPVOID      lpProtBuffer;
    } ABSBUFFER;

ABSBUFFER absBuffer[MAX_ABS_BUFFERS];   // Holds buffer addresses

//----------------------------------------------------------------------------
// DiskAbsBufferRegister (HINSTANCE hInstance, LPVOID lpBuffer)
//      Sets the buffer used by the INT 13h routines.
//
// Returns TRUE if set, else FALSE
//---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI DiskAbsBufferRegister (HINSTANCE hInstance, LPVOID lpRealBuffer, LPVOID lpProtBuffer)
{
    int i;


    if ( hInstance && hInstance != INVALID_HANDLE_VALUE && lpRealBuffer)
        {
                                        // First see if there is already a
                                        // buffer associated with this program
        for (i = 0; i < MAX_ABS_BUFFERS; i++)
            if ( hInstance == absBuffer[i].hInstance )
                {
                absBuffer[i].lpRealBuffer  = lpRealBuffer;
                absBuffer[i].lpProtBuffer  = lpProtBuffer;
                return(TRUE);
                }
                                        // Now look for a free entry
        for (i = 0; i < MAX_ABS_BUFFERS; i++)
            if ( !absBuffer[i].hInstance )  // Look for free entry
                {
                absBuffer[i].hInstance = hInstance;
                absBuffer[i].lpRealBuffer  = lpRealBuffer;
                absBuffer[i].lpProtBuffer  = lpProtBuffer;
                return(TRUE);
                }
        }
    return(FALSE);
}


//----------------------------------------------------------------------------
// DiskAbsBufferUnregister (HINSTANCE hInstance)
//      Disassociates a buffer to be used by a program.
//
// Returns TRUE if a buffer was previously set, else FALSE
//---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI DiskAbsBufferUnregister (HINSTANCE hInstance)
{
    int i;


    if ( hInstance && hInstance != INVALID_HANDLE_VALUE)
        {
        for (i = 0; i < MAX_ABS_BUFFERS; i++)
            if ( hInstance == absBuffer[i].hInstance )
                {
                absBuffer[i].hInstance = NULL;
                return(TRUE);
                }
        }
    return(FALSE);
}


//----------------------------------------------------------------------------
// DiskAbsBufferGet (HINSTANCE hInstance, LPLPVOID lplpBuffer)
//      Gets the buffer used by this module, if registered.
//
// Returns TRUE if buffer found, else FALSE
//---------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI DiskAbsBufferGet (LPVOID FAR *lplpRealBuffer, LPVOID FAR *lplpProtBuffer)
{
    int         i;
    HINSTANCE   hInstance;

#if defined(SYM_WIN32)
    hInstance = GetModuleHandle(NULL);
#else
                                        // This is undocumented stuff (pg. 194)
                                        // of "Undocumented Windows"
    hInstance = *((LPHINSTANCE) MAKELP(GetCurrentTask(), 0x1C));
#endif
    if ( hInstance && hInstance != INVALID_HANDLE_VALUE)
        {
        for (i = 0; i < MAX_ABS_BUFFERS; i++)
            if ( hInstance == absBuffer[i].hInstance )
                {
                *lplpRealBuffer = absBuffer[i].lpRealBuffer;
                *lplpProtBuffer = absBuffer[i].lpProtBuffer;
                return(TRUE);
                }
        }
    return(FALSE);
}

#elif defined(SYM_DOS)

LPVOID  _glpAbsBuffer;                  // Equivalent of ABSBUFFER above.
                                        // Since not a DLL, then only need
                                        // to save 1 value.  Macros in DISK.H
                                        // insure single code works for all
                                        // platforms.

#endif /* SYM_DOS */


#ifdef SYM_WIN32
static DWORD dwLastIOSResult;
DWORD SYM_EXPORT WINAPI DiskGetLastIOSAbsOpError()
{
    return dwLastIOSResult;
}

#ifndef _M_ALPHA
BYTE TranslateIOSErrorToDOSError( unsigned long iosResult )
{
    BYTE dosInt13Error = 0;
    dwLastIOSResult = iosResult;
    if (iosResult >= IORS_ERROR_DESIGNTR)
        {
        static BYTE iosErrorMap[] =
            {
               /* #define IORS_SUCCESS                    0x00  /* command completed successfully   */
               /* #define IORS_SUCCESS_WITH_RETRY         0x01  /* successful, but retries required */
               /* #define IORS_SUCCESS_WITH_ECC           0x02  /* ditto, but with ECC              */
             SYM_ERROR_BAD_SECTOR,        /* #define IORS_INVALID_SECTOR             0x10  /* old blockdev error code for         */
             SYM_ERROR_TIME_OUT,          /* #define IORS_CANCELED                   0x11  /* command was canceled                */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_CMD_IN_PROGRESS            0x12  /* command is in progress, and can't   */
             SYM_ERROR_INVALID_FUNCTION,  /* #define IORS_INVALID_CMD_PTR            0x13  /* cancel of an invalid command        */
             SYM_ERROR_INVALID_MEDIA,     /* #define IORS_MEDIA_ERROR                0x14  /* media failure                       */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_DEVICE_ERROR               0x15  /* error on recalib. drive, etc.       */
             SYM_ERROR_INVALID_FUNCTION,  /* #define IORS_INVALID_COMMAND            0x16  /* Command not supported or invalid    */
             SYM_ERROR_TIME_OUT,          /* #define IORS_NO_DEVICE                  0x17  /* physical or logical device          */
             SYM_ERROR_TIME_OUT,          /* #define IORS_NO_MEDIA                   0x18  /* media removed from device           */
             SYM_ERROR_TIME_OUT,          /* #define IORS_UNCERTAIN_MEDIA            0x19  /* media may have changed              */
             UNCORRECTABLE_ERRORS,        /* #define IORS_UNREC_ERROR                0x1A  /* un-recoverable error                */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_HW_FAILURE                 0x1B  /* general hardware failure            */
             SYM_ERROR_INVALID_MEDIA,     /* #define IORS_UNFORMATTED_MEDIA          0x1C  /* unformatted media                   */
             SYM_ERROR_DMA_OVERRUN,       /* #define IORS_MEMORY_ERROR               0x1D  /* error allocating DMA buffer space   */
             SYM_ERROR_TIME_OUT,          /* #define IORS_TIME_OUT                   0x1E  /* device timed out                    */
             SYM_ERROR_WRITE_PROTECTED,   /* #define IORS_WRITE_PROTECT              0x1F  /* write protect error                 */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_NOT_READY                  0x20  /* device not ready                    */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_BUSY                       0x21  /* device is busy                      */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_VOL_NOT_LOCKED             0x22  /* volume received unlock without lk   */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_VOL_LOCKED                 0x23  /* eject received with drive locked    */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_VOL_NOT_REMOVABLE          0x24  /* removable request to non-remov      */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_VOL_IN_USE                 0x25  /* volume is in use - used for eject   */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_LOCK_COUNT_EXCEEDED        0x26  /* volume lock count exceeded          */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_VALID_EJECT_FAILED         0x27  /* eject command was not accepted      */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_ILLEGAL_ACCESS_MODE        0x28  /* cdrom read on audio or attempt      */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_LOCK_VIOLATION             0x29  /* illegal access to locked device     */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_WRONG_MEDIA                0x2a  /* wrong media in drive                */
             SYM_ERROR_CONTROLLER_FAILED, /* #define IORS_OUT_OF_SPACE               0x2b  /* no space on media                   */
             SYM_ERROR_64K_DMA,           /* #define IORS_BIG_IO_BREAKUP_FAILED      0x2c  /* attempt to split big I/O barfed     */
             0
            };

        dosInt13Error = iosErrorMap[ iosResult - IORS_ERROR_DESIGNTR ];
        }
    return dosInt13Error;
}
#endif // _M_ALPHA
#endif /* SYM_WIN32 */

#if defined(SYM_WIN) || defined(SYM_PROTMODE)

//----------------------------------------------------------------------------
// DiskAbsOperation (command_code, absDiskRecordPtr)
//   performs the absolute sector operation specified by 'command_code'
//      using the information in the record ptr., 'absDiskRecordPtr'
//
//      command_code can be any of the following:
//              RESET_COMMAND
//              READ_COMMAND
//              WRITE_COMMAND
//              VERIFY_COMMAND
//              FORMAT_COMMAND
//
// DiskAbsOperation (Word commandCode, AbsDiskRec *lpAbsDisk)
//
// NOTE: The buffer is NOW virtual, rather than being a buffer returned
//       by GlobalDosAlloc().
//
// NOTE: You MUST call DiskGetPhysicalInfo() before calling this function
// to fill in all of the starting/ending fields, since these must be
// correct.  This change was made due to WIN32.
//
// returns the INT 13h error number if an error occurs, else 0
//---------------------------------------------------------------------------


BYTE SYM_EXPORT WINAPI DiskAbsOperation (BYTE command, LPABSDISKREC lpAbsDisk)
{
#if defined(SYM_WIN32)
#ifndef _M_ALPHA
    static FARPROC	lpfnDiskAbsOperation;
    extern HINSTANCE    hInstThunkDll;

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
#endif
        char        szDrive[SYM_MAX_PATH];
        HANDLE      hFile;
        DWORD       dwOffset;
        DWORD       dwNumBytesLo;
        DWORD       dwNumBytesHi;
        BYTE        byRet;
        DWORD       dwProcessed;
        __int64     llOffset;

                                        // Open the drive as a file.
        if (lpAbsDisk->dn >= FIRST_HD)
            {
            wsprintf(szDrive, "\\\\.\\PHYSICALDRIVE%u", lpAbsDisk->dn - FIRST_HD);
            }
        else
            {
            // Floppies don't use the same create file name
            wsprintf(szDrive, "\\\\.\\%c:", 'A' + lpAbsDisk->dn);
            }

        hFile = CreateFile(szDrive, GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return(SYM_ERROR_INVALID_FUNCTION);

        if ((command == READ_COMMAND) || (command == WRITE_COMMAND))
            {
                                        // Convert to a linear offset
            dwOffset = ConvertPhySectorToLong(lpAbsDisk, lpAbsDisk->dwHead, lpAbsDisk->dwTrack,
                                          lpAbsDisk->dwSector);

			if( HWIsNEC()  && (lpAbsDisk->sectorLenCode == 3) )
				{
				// sectorLenCode = 3 for 1.2MB NEC floppy
				llOffset = (__int64)((__int64) dwOffset * PHYSICAL_SECTOR_SIZE98);
				}
			else
				{
				llOffset = (__int64)((__int64) dwOffset * PHYSICAL_SECTOR_SIZE);
				}

            dwNumBytesLo = (DWORD) (llOffset & 0x00000000ffffffff);
            dwNumBytesHi = (DWORD) (llOffset >> 32);

            SetFilePointer(hFile, dwNumBytesLo, &dwNumBytesHi, FILE_BEGIN);
            if(command == READ_COMMAND)
                {
				if( HWIsNEC()  && (lpAbsDisk->sectorLenCode == 3) )
					// sectorLenCode = 3 for 1.2MB NEC floppy
					{
					byRet = ReadFile(hFile, lpAbsDisk->buffer,
                             (DWORD) lpAbsDisk->numSectors * PHYSICAL_SECTOR_SIZE98, &dwProcessed, NULL) ?
                             SYM_ERROR_NO_ERROR : SYM_ERROR_SECTOR_NOT_FOUND;
					}
				else
					{
					byRet = ReadFile(hFile, lpAbsDisk->buffer,
                             (DWORD) lpAbsDisk->numSectors * PHYSICAL_SECTOR_SIZE, &dwProcessed, NULL) ?
                             SYM_ERROR_NO_ERROR : SYM_ERROR_SECTOR_NOT_FOUND;
					}
                }
            else
                {
                byRet = WriteFile(hFile, lpAbsDisk->buffer,
                             (DWORD) lpAbsDisk->numSectors * PHYSICAL_SECTOR_SIZE, &dwProcessed, NULL) ?
                             SYM_ERROR_NO_ERROR : SYM_ERROR_SECTOR_NOT_FOUND;
                }
            }
        else
            byRet = SYM_ERROR_INVALID_FUNCTION;
        CloseHandle(hFile);
        return(byRet);
#ifndef _M_ALPHA
        }
    else
        {
        extern HANDLE     hSymkrnlVxD;
        DWORD             dwBytesReturned;


        if (hSymkrnlVxD == NULL && !GetSystemMetrics(SM_CLEANBOOT) )
            VxDServicesInit();

        if (hSymkrnlVxD != NULL && (command == READ_COMMAND || command == WRITE_COMMAND) )
            {
            auto   IOSOp      ios;
            auto   BOOL       bSuccess;

                                       // Set the command type
            switch(command)
                {
                case READ_COMMAND:  ios.IOSCmd = IOR_READ; break;
                case WRITE_COMMAND: ios.IOSCmd = IOR_WRITE; break;
                default:            return (0xFF);
                }

            ios.Int13Drv = lpAbsDisk->dn;// 80h, 81h, 82h... or 00h, 01h,
            ios.Buffer   = lpAbsDisk->buffer;// address of buffer

                                        // starting sector number
            ios.Sector   = ConvertPhySectorToLong(lpAbsDisk,
                                                  lpAbsDisk->dwHead,
                                                  lpAbsDisk->dwTrack,
                                                  lpAbsDisk->dwSector);

                                        // number of sectors to read/write
            ios.SectorCnt = lpAbsDisk->numSectors;


                                              // Call VxD to perform operation
            bSuccess = DeviceIoControl(hSymkrnlVxD,// SysWatch VxD Handle
                            SYMKVxD_FN_IOSSRV,// Function 4: NUVXD_FN_IOSSRV
                            (LPVOID)&ios,     // input buffer
                            sizeof(ios),      // input buffer size
                            (LPVOID)&ios,     // output buffer
                            sizeof(ios),      // output buffer size
                            &dwBytesReturned, NULL);      // sync call

            if (!bSuccess)
                return 0xFF;
                                                    // First, check for internal error...
            if( ios.Result < IOSOP_INTERNALERROR )  // error codes from IOR.H: IORS_???
                return( TranslateIOSErrorToDOSError( ios.Result ) );
                                              // Else, fall thru and call thunked version.
            goto ThunkedCall;
            }
        else
ThunkedCall:
            {
            if (lpfnDiskAbsOperation == NULL)
	        {
			    if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
				    {
			        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
			        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
			        }
		        if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		            {
		            lpfnDiskAbsOperation = GetProcAddress(hInstThunkDll, "DiskAbsOperation32" );
    	            if (lpfnDiskAbsOperation == NULL)
				        return 0xFF;
		            }
		        else
		            return 0xFF;
	        }
            return( lpfnDiskAbsOperation(command, lpAbsDisk) );
            }
        }
#endif // ifndef _M_ALPHA
#else
    union       REGS            regs;
    struct      SREGS           sregs;
    auto        BYTE            err;
    auto        DWORD           dwBuffer = 0;
    auto        DWORD           dwBufferSize;
    auto        LPVOID          lpRealBuffer, lpProtBuffer;
    auto        BYTE            byTemp;
    auto        WORD            wLen;
    auto        BYTE            byCommandLoNibble;


                                        // No sectors processed
    lpAbsDisk->sectorsProcessed = 0;

                                        /*------------------------------*/
                                        /* Allocate DOS memory for      */
                                        /* buffer.
                                        /*------------------------------*/
    dwBufferSize = lpAbsDisk->numSectors * (DWORD) PHYSICAL_SECTOR_SIZE;
    if ( !DiskAbsBufferGet(&lpRealBuffer, &lpProtBuffer) )
        {
        dwBuffer = GlobalDosAlloc(dwBufferSize);
        if ( dwBuffer == 0 )
            return(0xFF);               // Fatal error.  Can't allocate mem

        lpRealBuffer = MAKELP(HIWORD(dwBuffer), 0);
        lpProtBuffer = MAKELP(LOWORD(dwBuffer), 0);
        }
                                        /*------------------------------*
                                         * If write operation,          *
                                         * Copy input buffer to transfer*
                                         * buffer.                      *
                                         *------------------------------*/
    if ( command == WRITE_COMMAND )
        MEMCPY( lpProtBuffer, lpAbsDisk->buffer, (UINT) dwBufferSize);

    if (HWIsNEC())
        {
        switch (command)
            {
            case RESET_COMMAND:
                command = RESET_COMMAND98;
                break;

            case READ_COMMAND:
                command = READ_COMMAND98;
                break;

            case WRITE_COMMAND:
                command = WRITE_COMMAND98;
                break;

            case VERIFY_COMMAND:
                command = VERIFY_COMMAND98;
                break;

            case FORMAT_COMMAND:
                command = FORMAT_COMMAND98;
                break;
            }
                                      /*------------------------------*/
                                      /* Move absDisk fields into     */
                                      /* register record.             */
                                      /*------------------------------*/
        byCommandLoNibble = command & 0x0f;

        regs.h.dh = LOBYTE(LOWORD(lpAbsDisk->dwHead));
        regs.x.bx = lpAbsDisk->numSectors;
        wLen =  128 << lpAbsDisk->sectorLenCode;
        regs.x.bx *= wLen;

        regs.h.al = lpAbsDisk->dn;

        if ( regs.h.al & 0x50 )
            {
            /* Floppy */
            if (byCommandLoNibble == intrFORMAT_COMMAND98 )
                {
                regs.h.bh = 0;
                regs.h.bl = lpAbsDisk->numSectors;
                }
            regs.h.dh = LOBYTE(LOWORD(lpAbsDisk->dwHead));
            regs.h.dl = LOBYTE(LOWORD(lpAbsDisk->dwSector));
            regs.h.ch = lpAbsDisk->sectorLenCode;
            regs.h.cl = LOBYTE(LOWORD(lpAbsDisk->dwTrack));
            }
        else
            {
            /* Hard Disk */
            regs.x.cx = LOWORD(lpAbsDisk->dwTrack);
            regs.h.dl = LOBYTE(LOWORD(lpAbsDisk->dwSector - 1));
            regs.h.dh = LOBYTE(LOWORD(lpAbsDisk->dwHead));

            if (( byCommandLoNibble != intrFORMAT_COMMAND98 ) &&
                ( byCommandLoNibble != intrSENSE_COMMAND98 ))
                {
                if ( byCommandLoNibble == intrRESET_COMMAND98 )
                    {
                    byCommandLoNibble = command = intrRECALIB_COMMAND98;
                    }

                _asm
                    {
                    mov     ah,command          // turn on retry bits
                    xor     ah,020h             //
                    mov     command,ah          //
                    }
                }
            }
                                                /* Instead of BP */
                                                /* NEC require BP, But */
                                                /* regs has not BP */
        sregs.es  = FP_SEG(lpRealBuffer);       // Pass it the segment
        regs.x.di = FP_OFF(lpRealBuffer);

        regs.h.ah = command;

        /*------------------------------*/
        /* Perform INT 1bh              */
        /*------------------------------*/
        IntWin(0x1b, &regs, &sregs);

        lpAbsDisk->sectorsProcessed = lpAbsDisk->numSectors;
        //
        // Set up Error/result code
        //
        regs.h.ah = (BYTE)( regs.h.ah & 0xf8 );
        if ( ! regs.x.cflag )
            {
            if (byCommandLoNibble != intrSENSE_COMMAND98 )
                regs.h.ah = 0;
            }
        err = regs.h.ah & 0xF0;
        if ( err == NEC_ERROR_WRITE_PROTECTED98 )
            err = SYM_ERROR_WRITE_PROTECTED;
        else if ( err == NEC_ERROR_BAD_CYLINDER98 )
            err = SYM_ERROR_BAD_CYLINDER;
        else if( err == NEC_UNCORRECTABLE_ERRORS98 )
            err = UNCORRECTABLE_ERRORS;
        else if( err == NEC_ERROR_NOT_READY )
            err = SYM_ERROR_TIME_OUT;
        }
    else // !NEC
        {
        // do we know if it is an extended Int13 drive, yet?
        // if not, find out now.
        if (lpAbsDisk->bIsExtInt13 == ISINT13X_UNKNOWN)
            lpAbsDisk->bIsExtInt13 = DiskIsExtendedInt13Drive(lpAbsDisk);

        if (lpAbsDisk->bIsExtInt13 == ISINT13X_YES)
            {
            DiskAddrPacket FAR * diskPacket;
            DWORD           dwSegSel;
            LPBYTE          lpRealPacket;


            dwSegSel = GlobalDosAlloc(sizeof(DiskAddrPacket));
            if (dwSegSel == NULL)
                return (0x01);              // return "invalid fn"


            lpRealPacket = MAKELP(HIWORD(dwSegSel),0);
            diskPacket = (DiskAddrPacket *) MAKELP(LOWORD(dwSegSel), 0);

            // next, do read/write
            MEMSET(diskPacket,0,sizeof(DiskAddrPacket));

            diskPacket->packet_size = sizeof(DiskAddrPacket);
            diskPacket->block_count = lpAbsDisk->numSectors;
            diskPacket->block_numLo = ConvertPhySectorToLong(lpAbsDisk,
                                                             lpAbsDisk->dwHead,
                                                             lpAbsDisk->dwTrack,
                                                             lpAbsDisk->dwSector);
            diskPacket->block_numHi = 0;    // We support up to 2 terrabytes
            diskPacket->buffer_Seg = FP_SEG(lpRealBuffer);
            diskPacket->buffer_Off = FP_OFF(lpRealBuffer);


            regs.h.dl = lpAbsDisk->dn;
            sregs.ds  = FP_SEG(lpRealPacket);       // Pass it the segment
            regs.x.si = FP_OFF(lpRealPacket);

            switch (command)
                {
                case READ_COMMAND:
                        regs.x.ax = 0x4200;
                        break;

                case WRITE_COMMAND:
                        regs.x.ax = 0x4300;
                        break;

                case VERIFY_COMMAND:
                        regs.x.ax = 0x4400;
                        break;
                default:
                        // if not one of these, then use the older Int13 call
                        GlobalDosFree(LOWORD(dwSegSel));
                        goto OldInt13;
                        break;

                }

            IntWin(0x13, &regs, &sregs);

            if (regs.x.cflag)
                {
                err = regs.h.ah;
                if (!err)
                    err = 1;                /* If no error, return "Invalid */
                }
            else
                {
                err = 0;                    /* No error                     */
                }

            GlobalDosFree(LOWORD(dwSegSel));
            }
        else
            {

OldInt13:

                                            /*------------------------------*/
                                            /* Clear diskette status reg.   */
                                            /* manually, since RESET command*/
                                            /* makes a terrible grinding    */
                                            /* noise.                       */
                                            /*------------------------------*/
            byTemp = 0;
            MemCopyPhysical(MAKELP(0x0040, 0x0041), &byTemp, 1, MCP_SRC_VIRTUAL | MCP_DEST_PHYSICAL);

                                            /*------------------------------*/
                                            /* Move absDisk fields into     */
                                            /* register record.             */
                                            /*------------------------------*/
            regs.h.dl = lpAbsDisk->dn;
            regs.h.dh = LOBYTE(LOWORD(lpAbsDisk->dwHead));
            regs.x.cx = (WORD)DiskScrambleBIOSCoding(lpAbsDisk->dwTrack, lpAbsDisk->dwSector);
            regs.h.al = lpAbsDisk->numSectors;
            regs.h.ah = command;
            sregs.es  = FP_SEG(lpRealBuffer);       // Pass it the segment
            regs.x.bx = FP_OFF(lpRealBuffer);
                                            /*------------------------------*/
                                            /* Is this a BIG cylinder?      */
                                            /*------------------------------*/
            if (lpAbsDisk->dwTrack >= 1024)
                {
                union   REGS    tempRegs;
                struct  SREGS   tempSregs;
                auto    UINT    err;

                                            /*------------------------------*/
                                            /* Save current values          */
                                            /*------------------------------*/
                tempRegs  = regs;
                tempSregs = sregs;

                regs.h.ah = 0xEE;               /* Disk Manager extended call   */
                IntWin(0x13, &regs, &sregs);
                err = regs.x.cflag;
                                            /*------------------------------*/
                                            /* Restore values               */
                                            /*------------------------------*/
                regs  = tempRegs;
                sregs = tempSregs;
                                            /*------------------------------*/
                                            /* If error, then try SpeedStor */
                                            /* scheme.                      */
                                            /*------------------------------*/
                                            /* MUST be SpeedStor (Since     */
                                            /* those are the only schemes I */
                                            /* know about; and we couldn't  */
                                            /* have gotten a track #        */
                                            /* > 1023, without using one of */
                                            /* these schemes.               */
                                            /* Set 2 high-order bits of DH  */
                                            /* with 2 high-order bits of the*/
                                            /* track #                      */
                                            /*------------------------------*/
                if (err)
                    regs.h.dh |= (HIBYTE(LOWORD(lpAbsDisk->dwTrack << 4)) & (BYTE) 0xC0);
                }
                                            /*------------------------------*/
                                            /* Perform INT 13h              */
                                            /*------------------------------*/
            IntWin(0x13, &regs, &sregs);
            err = regs.h.ah;
            lpAbsDisk->sectorsProcessed = regs.h.al;
            if (err && !(err & (BYTE) 0xE1))
                {
                                            /*------------------------------*/
                                            /* If fatal error, reset disk   */
                                            /* system                       */
                                            /*------------------------------*/

                regs.h.ah = 0;                  /* Do Reset                     */
                IntWin(0x13, &regs, &sregs);
                }
            }
        }



                                        // Copy to virtual memory
    if (lpAbsDisk->buffer)
        {
        MEMCPY(lpAbsDisk->buffer, lpProtBuffer, (UINT) dwBufferSize);
        }


    if ( dwBuffer )
        GlobalDosFree(LOWORD(dwBuffer));

    return(err);
#endif
}

#elif defined(NCD)                      // Start of code for NCD (Japanese NEC DOS)
BYTE SYM_EXPORT WINAPI DiskAbsOperation(BYTE command, LPABSDISKREC lpAbsDisk)
{
	switch (command){
		case RESET_COMMAND:
      	command = RESET_COMMAND98;
         break;

      case READ_COMMAND:
         command = READ_COMMAND98;
         break;

      case WRITE_COMMAND:
         command = WRITE_COMMAND98;
         break;

      case VERIFY_COMMAND:
         command = VERIFY_COMMAND98;
         break;

      case FORMAT_COMMAND:
         command = FORMAT_COMMAND98;
         break;
	}

   return (_DiskAbsOperationNEC(lpAbsDisk,command));
}


#elif defined(SYM_NTK)

BYTE SYM_EXPORT WINAPI DiskAbsOperation (BYTE command, LPABSDISKREC lpAbsDisk)
    {
    return 0xFF;
    }

#elif !defined(SYM_VXD)

#pragma optimize("", off)

BYTE SYM_EXPORT WINAPI DiskAbsOperation (BYTE command, LPABSDISKREC lpAbsDisk)
{
    auto        BYTE            err = 0;

    if (lpAbsDisk->bIsExtInt13 == ISINT13X_UNKNOWN)
        lpAbsDisk->bIsExtInt13 = DiskIsExtendedInt13Drive(lpAbsDisk);

    if (lpAbsDisk->bIsExtInt13 == ISINT13X_YES)
        {
        DiskAddrPacket  diskPacket;
        DiskAddrPacket * lpDiskPacket = &diskPacket;
        BYTE  byDriveNum = lpAbsDisk->dn;
        WORD  wCommand;

        switch (command)
            {
            case READ_COMMAND:
                    wCommand = 0x4200;
                    break;

            case WRITE_COMMAND:
                    wCommand = 0x4300;
                    break;

            case VERIFY_COMMAND:
                    wCommand = 0x4400;
                    break;
            default:
                    // if not one of these, then use the older Int13 call
                    goto OldInt13;
                    break;
            }

        // next, do read/write
        MEMSET(&diskPacket,0,sizeof(DiskAddrPacket));

        diskPacket.packet_size = sizeof(DiskAddrPacket);
        diskPacket.block_count = lpAbsDisk->numSectors;
        diskPacket.block_numLo = ConvertPhySectorToLong(lpAbsDisk,
                                                         lpAbsDisk->dwHead,
                                                         lpAbsDisk->dwTrack,
                                                         lpAbsDisk->dwSector);
        diskPacket.block_numHi = 0;    // We support up to 2 terrabytes
        diskPacket.buffer_Seg  = FP_SEG(lpAbsDisk->buffer);
        diskPacket.buffer_Off  = FP_OFF(lpAbsDisk->buffer);


        _asm
            {
            ;--------------------------------------
            ; Set up drive number and command
            ;--------------------------------------
            mov     dl, byDriveNum
            mov     ax, wCommand

            ;--------------------------------------
            ; Address abs. disk info table
            ;--------------------------------------
            push    DS                      ;save caller's DS
            lds     si, lpDiskPacket         ; DS:SI points to packet
            int     13h                     ;       being used.
            pop     DS                      ;restore DS

            jnc     NoErr                   ;if no error, then just fine
            mov     err,ah                  ;an error, save its value
            or      ah,ah                   ;unless it is a 0
            jnz     NoErr

            mov     err,1                   ;inwhich, return a 1
NoErr:
            }
        }
    else
        {
OldInt13:

                                        /*------------------------------*/
                                        /* Clear diskette status reg.   */
                                        /* manually, since RESET command*/
                                        /* makes a terrible grinding    */
                                        /* noise.                       */
                                        /*------------------------------*/
        *((LPBYTE) 0x00400041L) = 0;


        _asm
            {
            push    DS                      ;save caller's DS
            ; Address abs. disk info table
            ;--------------------------------------
            lds     bx, lpAbsDisk           ; Get ptr. to abs disk record

            ;--------------------------------------
            ; Set up drive number and head number
            ;--------------------------------------
            mov     dl, [bx]LPABSDISKREC.dn         ; Get drive number
            mov     dh, byte ptr ([bx]LPABSDISKREC.dwHead)     ; Get head number

            ;--------------------------------------
            ; Set up track, sector
            ;--------------------------------------
            mov     si, word ptr ([bx]LPABSDISKREC.dwTrack)    ; Save track number
            mov     cx, si
            and     cx, 03FFh               ; Remove any EXTRA bits (>1023 cylinders)
            xchg    ch, cl                  ; Change order of low, high
            ror     cl, 1                   ; Get two high bits of track
            ror     cl, 1                   ;
            or      cl, byte ptr ([bx]LPABSDISKREC.dwSector)   ; Get sector number

            ;--------------------------------------
            ; Set up sector count
            ;--------------------------------------
            mov     al, [bx]LPABSDISKREC.numSectors ; Get count of sectors

            ;--------------------------------------
            ; Set up transfer buffer
            ;--------------------------------------
            push    dx                      ; Save this value for now

            mov     es, word ptr ([bx]LPABSDISKREC.buffer + 2)
            mov     dx, word ptr ([bx]LPABSDISKREC.buffer)

            mov     bx, dx                  ; Move to BX

            pop     dx                      ; Restore drive number, head number

            mov     ah, command             ; Get command

            ;--------------------------------------
            ; Is this a BIG cylinder?
            ;--------------------------------------
            cmp     si, 1024                ; Is it >= 1024 cylinder?
            jb      doIt                    ; No, do it

            ;--------------------------------------
            ; Big Cylinder.  See which scheme is
            ; being used.
            ;--------------------------------------
            push    ax                      ; Save all registers
            push    bx                      ;
            push    cx                      ;
            push    dx                      ;
            push    es                      ;
            mov     ah, 0EEh                ; See if Disk Manager scheme
            int     13h                     ;       being used.
            pop     es                      ; Restore all registers
            pop     dx                      ;
            pop     cx                      ;
            pop     bx                      ;
            pop     ax                      ;
            jnc     doIt                    ; Disk Manager installed. Use it

            ;--------------------------------------
            ; MUST be SpeedStor
            ;--------------------------------------

            push    ax                      ; Save AX
            push    cx                      ; Save CX

            mov     ax, si                  ; Get the track #
            mov     cl, 4                   ; Move 2 bits to high-order bits
            shl     ax, cl                  ; Move 2 bits to high-order position
            and     ah, 0C0h                ; Preserve only top 2 bits
            or      dh, ah                  ; Set top 2 bits of head

            pop     cx                      ; Restore CX
            pop     ax                      ; Restore AX

doIt:
            int     13h                     ; Abs. Sector service routine
            or      ah, ah                  ; Was there an error?
            jz      returnToCaller          ; No, return to caller

            ;--------------------------------------
            ; Error, reset disk system, if fatal error
            ;--------------------------------------
            test    ah, 0E1h                ; Fatal error mask
            jz      returnToCaller          ; If not fatal, skip

            push    ax                      ; Save error code
            xor     ah, ah                  ; Reset disk system
            int     13h                     ; Abs. Sector service routine
            pop     ax                      ; Restore error code

returnToCaller:
            lds     bx, lpAbsDisk   ; Get ptr. to abs disk record
            mov     [bx]LPABSDISKREC.sectorsProcessed, al ; Save number of sectors processed

            xchg    ah, al                  ; Put error code in low-byte
            xor     ah, ah                  ; Clear high byte
            mov     err,al
            pop     DS                      ;restore caller's DS
            }
        }

    return (err);
}
#pragma optimize("", on)

#endif

/*----------------------------------------------------------------------*
 *      Converts an absolute (physical sector) coordinate to an         *
 *      absolute sector number (zero-based).                            *
 *----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI ConvertPhySectorToLong(LPCABSDISKREC lpAbsDisk,
                         DWORD head, DWORD cylinder, DWORD sector)
{
    return ((cylinder * lpAbsDisk->dwSectorsPerTrack *
             lpAbsDisk->dwTotalHeads) +
             (head * lpAbsDisk->dwSectorsPerTrack) +
             (sector - 1L));
}


/*----------------------------------------------------------------------*
 *      Converts an absolute zero-based sector number to                *
 *      physical sector coordinates.                                    *
 *----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI ConvertLongToPhySector(LPCABSDISKREC lpAbsDisk, DWORD offset,
                         DWORD FAR *  headPtr, DWORD FAR * cylinderPtr, DWORD FAR *  sectorPtr)
{
    auto        DWORD           tracks;

                                        /* Convert to tracks first      */
    tracks = offset / lpAbsDisk->dwSectorsPerTrack;

    *sectorPtr   = (offset % lpAbsDisk->dwSectorsPerTrack) + 1L;
    *cylinderPtr = tracks / lpAbsDisk->dwTotalHeads;
    *headPtr     = tracks % lpAbsDisk->dwTotalHeads;
}



/*----------------------------------------------------------------------*/
/* DiskLogSectorToPhySector ()                                          */
/*      Converts a logical DOS sector to its physical 3-dimensional     */
/*      coordinates.                                                    */
/*                                                                      */
/*      void LogicalSectorToPhysicalSector (Ulong       sector,         */
/*                                          DiskRec    *diskPtr,        */
/*                                          AbsDiskRec *absDiskPtr);    */
/*                                                                      */
/* NOTE: 'sectorsBeforePartition' MUST be filled in first, before       */
/*      using.                                                          */
/*                                                                      */
/*----------------------------------------------------------------------*/

void SYM_EXPORT WINAPI DiskLogSectorToPhySector (DWORD sector,
                                    LPDISKREC diskPtr,
                                    LPABSDISKREC absDiskPtr)
{
    DWORD               sectorOffset;
    DWORD               result;
    register WORD       modResult;
    DWORD               totalHeads;


                                        /* Find total sector offset     */
    sectorOffset = diskPtr->sectorsBeforePartition +
                  (sector * (diskPtr->bytesPerSector / PHYSICAL_SECTOR_SIZE));

    result    = (sectorOffset / absDiskPtr->dwSectorsPerTrack);
    modResult = (WORD) (sectorOffset % absDiskPtr->dwSectorsPerTrack);
    totalHeads = absDiskPtr->dwEndingHead + 1;

                                        /* Compute sector offset        */
    absDiskPtr->dwSector = modResult + 1;
                                        /* Compute track offset         */
    absDiskPtr->dwTrack  = result / totalHeads;
                                        /* Compute head offset          */
    absDiskPtr->dwHead   = result % totalHeads;
}


/*----------------------------------------------------------------------*/
/*      Computes the linear offset of the starting or ending location   */
/*      in a partition table entry.                                     */
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI    DiskGetSectorStartOffset (
    LPCABSDISKREC          lpAbsDisk,
    LPCPARTENTRYREC        partPtr)
{
    auto        DWORD           cylinder;
    auto        DWORD           sector;
    auto        DWORD           offset;


                                        /* Unscramble 10-bit cylinder   */
    DiskUnscrambleBIOSCoding (*((WORD FAR *)&(partPtr->startSector)), &cylinder, &sector);

                                        /* Get offset of start sector   */
    offset = ConvertPhySectorToLong (lpAbsDisk, (DWORD)partPtr->startHead, cylinder, sector);

    return (offset);                    /* Return offset                */
}


/*----------------------------------------------------------------------*/
/*      Returns the linear offset of the ENDING LOCATION of a           */
/*      partition table entry.                                          */
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI    DiskGetSectorEndOffset (
    LPCABSDISKREC          lpAbsDisk,
    LPCPARTENTRYREC        partPtr)
{
    auto        DWORD            cylinder;
    auto        DWORD            sector;
    auto        DWORD            offset;


                                        /* Unscramble 10-bit cylinder   */
    DiskUnscrambleBIOSCoding (*((WORD FAR *)&(partPtr->endSector)), &cylinder, &sector);

                                        /* Get offset of start sector   */
    offset = ConvertPhySectorToLong (lpAbsDisk,(DWORD) partPtr->endHead, cylinder, sector);

    return (offset);                    /* Return offset                */
}


#if !defined(SYM_VXD) && !defined(SYM_NTK)
/*----------------------------------------------------------------------*/
/* DiskRecToFloppyAbsDiskRec()                                          */
/*      Converts a DiskRec for a floppy diskette to an AbsDiskRec.      */
/*                                                                      */
/*      Assumes that the drive number is filled in already              */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskRecToFloppyAbsDiskRec (LPCDISKREC diskPtr, LPABSDISKREC lpAbsDisk)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
    static FARPROC	lpfnDiskRecToFloppyAbs;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnDiskRecToFloppyAbs == NULL)
	{
	    if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnDiskRecToFloppyAbs = GetProcAddress(hInstThunkDll,"DiskRecToFloppyAbsDiskRec32" );
    	    if (lpfnDiskRecToFloppyAbs == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnDiskRecToFloppyAbs(diskPtr, lpAbsDisk) );
#endif // _M_ALPHA
#else

    extern BPBREC   FAR BPBTables[];
    DeviceParamRec      deviceParamTable; /* Generic IOCTL record       */
    BOOL                validGenericIO;
    LPBPBREC            BPBPtr;
    int                 i;

                                        /*------------------------------*/
                                        /* Ask DOS for the BPB.         */
                                        /*------------------------------*/
                                        /* Set to 1 so we get the       */
                                        /*      BUILD BPB               */
    MEMSET(&deviceParamTable, 0, sizeof(DeviceParamRec));
    deviceParamTable.specialFunctions = 1;
    if (validGenericIO = (BOOL) DiskGenericIOCTL (GET_DEVICE_PARAMS,
                                                  diskPtr->dl,
                                                  &deviceParamTable))
        {                               /* Use values from GENERIC IO   */

                                        /* Get total tracks             */
        lpAbsDisk->dwTotalTracks = deviceParamTable.numberOfTracks;

        lpAbsDisk->dwTotalHeads  = (BYTE)deviceParamTable.BPB.heads;
        lpAbsDisk->dwSectorsPerTrack = (BYTE)deviceParamTable.BPB.sectorsPerTrack;

                                        /* Make sure all values map onto*/
                                        /* this disk size.              */
        if ((lpAbsDisk->dwTotalTracks *
                        lpAbsDisk->dwTotalHeads *
                            lpAbsDisk->dwSectorsPerTrack) != diskPtr->totalSectors)
            validGenericIO = FALSE;
        }

                                        /*------------------------------*/
                                        /* Search through our BPB tables*/
                                        /* to see if it matches any of  */
                                        /* them.                        */
                                        /*------------------------------*/
    if (!validGenericIO)
        {
        for (i = 0; BPBTables[i].bytesPerSector; i++)
            {
            BPBPtr = &BPBTables[i];

            if ((BPBPtr->sectorsPerFat   == (WORD)diskPtr->dwSectorsPerFATEx) &&
                 (BPBPtr->oldTotalSectors == (UINT)diskPtr->totalSectors) &&
                 (BPBPtr->sectorsPerCluster == diskPtr->sectorsPerCluster) &&
                 (BPBPtr->maxRootEntries    == diskPtr->maxRootEntries))
                {
                lpAbsDisk->dwTotalTracks = deviceParamTable.numberOfTracks;

                lpAbsDisk->dwTotalHeads  = BPBPtr->heads;
                lpAbsDisk->dwSectorsPerTrack = BPBPtr->sectorsPerTrack;

                lpAbsDisk->dwTotalTracks = BPBPtr->oldTotalSectors /
                                (BPBPtr->heads * BPBPtr->sectorsPerTrack);
                validGenericIO = TRUE;
                break;
                }
            }
        }
                                        /*------------------------------*/
                                        /* Fills in the starting        */
                                        /* location and the ending      */
                                        /* location.                    */
                                        /*------------------------------*/
    lpAbsDisk->dwStartingHead = 0;
    lpAbsDisk->dwStartingTrack = 0;
    lpAbsDisk->dwStartingSector = 1;

    lpAbsDisk->dwEndingHead = (BYTE)(lpAbsDisk->dwTotalHeads - 1L);
    lpAbsDisk->dwEndingTrack = (WORD)(lpAbsDisk->dwTotalTracks - 1L);
    lpAbsDisk->dwEndingSector = (BYTE)lpAbsDisk->dwSectorsPerTrack;

    return (validGenericIO);

#endif
}
#endif // !defined(SYM_VXD) && !defined(SYM_NTK) for DiskRecToFloppyAbsDiskRec()


//-----------------------------------------------------------------------------
//  The following external declaration is required by DiskGetPhysicalInfo.
//---------------------------------------------------------------------------
#if defined(SYM_VXD)
EXTERNC BOOL WINAPI _IOSSrv_GetInfo (VxDDiskGetPhysicalInfo*);
#endif


#if defined(SYM_WIN32)

BYTE LOCAL _GetPhyInfoViaThunk(LPABSDISKREC lpAbsDiskActual)
{
    auto   BOOL         bSuccess = 0xFF;
#ifndef _M_ALPHA

    extern HINSTANCE    hInstThunkDll;
    static FARPROC      lpfnDiskGetPhysicalInfo;

    if (lpfnDiskGetPhysicalInfo == NULL)
        {
        if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
            {
            SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
            hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
            }

        if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
            {
            lpfnDiskGetPhysicalInfo = GetProcAddress(hInstThunkDll, "DiskGetPhysicalInfo32" );
            if (lpfnDiskGetPhysicalInfo == NULL)
                return 0xFF;

            }
        else
            return 0xFF;

        }
    bSuccess = lpfnDiskGetPhysicalInfo(lpAbsDiskActual);

#endif   // _M_ALPHA
    return bSuccess;

}

BYTE LOCAL _GetPhyInfoViaSymVxD (LPABSDISKREC lpAbsDiskTranslated, LPABSDISKREC lpAbsDiskActual)
{

    auto   BOOL         bSuccess = FALSE;
#ifndef _M_ALPHA
    extern HANDLE       hSymkrnlVxD;

   if (hSymkrnlVxD == NULL)
       VxDServicesInit();

   if (hSymkrnlVxD != NULL)
       {
       auto   DCB          dcb;    // Device Control Block
       auto   DCB_BLOCKDEV bdd;
       auto   VxDDiskGetPhysicalInfo getInfo;
       DWORD  dwBytesReturned;

       getInfo.Int13Drv = lpAbsDiskTranslated->dn; // 80h, 81h, 82h... or 00h, 01h,
       getInfo.pDCB     = &dcb;    // DCB as defined in DCB.H & DCB.INC
       getInfo.pBDD     = &bdd;    // DCB_BLOCKDEV as defined in DCB.H & DCB.INC

       bSuccess = DeviceIoControl(hSymkrnlVxD,// SysWatch VxD Handle
                   SYMKVxD_FN_GETPHYSICALINFO, // Function 5: Get physical disk info
                   (LPVOID)&getInfo, // input buffer
                   sizeof(getInfo),  // input buffer size
                   NULL,             // no output buffer
                   0,                // no output buffer size
                   &dwBytesReturned, NULL);      // sync call

                                   // We also need to ensure that
                                   // bdd.DCB_apparent_spt > 0 because on
                                   // NEC systems with MO drives, the call
                                   // appeared to succeed, but left the bdd
                                   // values zeroed out. These may be used
                                   // later in divisions causing crashes.
       if (bSuccess && (getInfo.Result == 0) && (bdd.DCB_apparent_spt > 0))
           {
           auto BYTE CMOS_Type;
           auto BYTE sectorLen;
           auto int  iBytesPerSector;

           lpAbsDiskTranslated->dwSectorsPerTrack =  bdd.DCB_apparent_spt;      /* Sectors on each track  */
           lpAbsDiskTranslated->dwTotalTracks     =  bdd.DCB_apparent_cyl_cnt;  /* Total tracks on disk   */
           lpAbsDiskTranslated->dwTotalHeads      =  bdd.DCB_apparent_head_cnt; /* Total heads on disk    */
           lpAbsDiskTranslated->dwMaxSectorsLo    =  bdd.DCB_apparent_sector_cnt[0];
           lpAbsDiskTranslated->dwMaxSectorsHi    =  bdd.DCB_apparent_sector_cnt[1];

           CMOS_Type   = 0;                                    /* CMOS drive type        */
           if (dcb.DCB_cmn.DCB_device_type == DCB_type_floppy)
               {
                    // Translate the DCB actual geometry into CMOS values:
                    // Drive Type   Tracks   Sectors per Track  CMOS type
                    //   360K         40        9                 1
                    //   720K         80        9                 2
                    //   1.2M         80        15                3
                    //   1.44M        80        18                4
                    //   2.88M        80        36                5
               if (dcb.DCB_actual_cyl_cnt == 40 && dcb.DCB_actual_spt == 9)
                   CMOS_Type = CMOS_360K;
               else if (dcb.DCB_actual_cyl_cnt == 80)
                   {
                                   // Use the sectors per track field
                                   // for identification
                   if (dcb.DCB_actual_spt == 9)
                       CMOS_Type = CMOS_720K;
                   else if (dcb.DCB_actual_spt == 15)
                       CMOS_Type = CMOS_12M;
                   else if (dcb.DCB_actual_spt == 18)
                       CMOS_Type = CMOS_144M;
                   else if (dcb.DCB_actual_spt == 36)
                       CMOS_Type = CMOS_288M;
                   }
               }
           lpAbsDiskTranslated->CMOS_Type   = CMOS_Type;                 /* CMOS drive type        */

           lpAbsDiskTranslated->numDrives       = (BYTE) getInfo.dwNumDrives;       /* Num. drives (floppy or hard) */

                                       /* Sector length code           */
                                   /* code  :   0    1    2    3   */
                                   /* length: 128  256  512 1024   */
           iBytesPerSector = bdd.DCB_apparent_blk_size >> 8;
           for ( sectorLen = 0; iBytesPerSector; sectorLen++ )
               iBytesPerSector = iBytesPerSector >> 1;
           lpAbsDiskTranslated->sectorLenCode = sectorLen;


           lpAbsDiskTranslated->bIsExtInt13 = ISINT13X_UNKNOWN;

                                   // Now lets setup the ACTUAL abs disk structure
           *lpAbsDiskActual = *lpAbsDiskTranslated;
           lpAbsDiskActual->dwSectorsPerTrack =  dcb.DCB_actual_spt;      /* Sectors on each track  */
           lpAbsDiskActual->dwTotalTracks     =  dcb.DCB_actual_cyl_cnt;  /* Total tracks on disk   */
           lpAbsDiskActual->dwTotalHeads      =  dcb.DCB_actual_head_cnt; /* Total heads on disk    */
           lpAbsDiskActual->dwMaxSectorsLo    =  dcb.DCB_actual_sector_cnt[0];
           lpAbsDiskActual->dwMaxSectorsHi    =  dcb.DCB_actual_sector_cnt[1];

           iBytesPerSector = dcb.DCB_actual_blk_size >> 8;
           for ( sectorLen = 0; iBytesPerSector; sectorLen++ )
               iBytesPerSector = iBytesPerSector >> 1;
           lpAbsDiskActual->sectorLenCode = sectorLen;




           }
       else
           {
// Added on 12-18-96
// We need to set bSuccess to FALSE if there was an error in the
// DeviceIoControl call as this will cause other problems to occur
// on NEC machines
                bSuccess = FALSE;
            }
        }

#endif   // _M_ALPHA

    if (bSuccess)
        return 0;                       // No Error!
    else
        return (0x01);
}
#endif  // _SYM_WIN32






//-----------------------------------------------------------------------------
//  _DiskGetPhysicalInfoEx ()
//      Gets the drive parameters returned by Int 13h, function 08h.
//              This function will clobber the absDisk field, 'buffer'
//              by having it point to the DDPT for a floppy disk.  Also
//              the CMOS_Type field is only valid for floppy disks.
//      If available (at the time of this authoring only Win95 supported this)
//              the actual absolute disk rec fields will be filled in.
//
// NOTE: Call this function before calling DiskAbsOperation().
//---------------------------------------------------------------------------

BYTE LOCAL _DiskGetPhysicalInfoEx (LPABSDISKREC lpAbsDiskTranslated, LPABSDISKREC lpAbsDiskActual)
{
#if defined(SYM_WIN32)

    auto   BOOL         bSuccess = FALSE;

#ifndef _M_ALPHA
    extern HANDLE       hSymkrnlVxD;
    extern HINSTANCE    hInstThunkDll;
    static FARPROC      lpfnDiskGetPhysicalInfo;

    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
#endif
        char        szDrive[SYM_MAX_PATH];
        HANDLE      hFile;

                                        // Open the drive as a file.
        if (lpAbsDiskTranslated->dn >= FIRST_HD)
            {
            wsprintf(szDrive, "\\\\.\\PHYSICALDRIVE%u", lpAbsDiskTranslated->dn - FIRST_HD);
            }
        else
            {
            // Floppies don't use the same create file name
            wsprintf(szDrive, "\\\\.\\%c:", 'A' + lpAbsDiskTranslated->dn);
            }

        hFile = CreateFile(szDrive, GENERIC_READ | GENERIC_WRITE,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    NULL,
                                    OPEN_EXISTING,
                                    0,
                                    NULL);
        if (hFile != INVALID_HANDLE_VALUE)
            {
            DISK_GEOMETRY               dg;
            BYTE                        CMOS_Type;
            BYTE                        sectorLen;
            int                         iBytesPerSector;
            DWORD                       dwBytesReturned;
            int                         uDrive;
            HANDLE                      hDrive;

            if (DeviceIoControl(hFile, IOCTL_DISK_GET_DRIVE_GEOMETRY,
                            NULL, 0,
                            &dg,
                            sizeof(dg),
                            &dwBytesReturned, NULL))
                {
                lpAbsDiskTranslated->dwSectorsPerTrack =  dg.SectorsPerTrack;
                lpAbsDiskTranslated->dwTotalTracks     =  dg.Cylinders.LowPart;
                lpAbsDiskTranslated->dwTotalHeads      =  dg.TracksPerCylinder;
                switch (dg.MediaType)
                    {
                    case F5_1Pt2_512:
                        CMOS_Type = CMOS_12M;
                        break;

                    case F3_1Pt44_512:
                        CMOS_Type = CMOS_144M;
                        break;

                    case F3_2Pt88_512:
                        CMOS_Type = CMOS_288M;
                        break;

                    case F3_720_512:
                        CMOS_Type = CMOS_720K;
                        break;

                    case F5_360_512:
                    case F5_320_512:
                        CMOS_Type = CMOS_360K;
                        break;

                    default:
                        CMOS_Type = 0;
                        break;
                    }

                lpAbsDiskTranslated->CMOS_Type = CMOS_Type;

                                        /* Sector length code           */
					                    /* code  :   0	  1    2    3	*/
					                    /* length: 128  256  512 1024	*/
                iBytesPerSector = dg.BytesPerSector >> 8;
                for ( sectorLen = 0; iBytesPerSector; sectorLen++ )
                    iBytesPerSector = iBytesPerSector >> 1;
                lpAbsDiskTranslated->sectorLenCode = sectorLen;

                /* Count up the number of drives */
                //lpAbsDiskTranslated->numDrives = 1;   // We don't know the physical # of drives
                lpAbsDiskTranslated->numDrives = 0;
                for (uDrive = 0; uDrive < SYM_MAX_DOS_DRIVES; uDrive++)
                    {
                    wsprintf(szDrive, "\\\\.\\PhysicalDrive%d", uDrive);
                    hDrive = CreateFile(szDrive, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
                    if (hDrive != INVALID_HANDLE_VALUE)
                        {
                        lpAbsDiskTranslated->numDrives ++;
                        CloseHandle(hDrive);
                        }
                    }

                bSuccess = TRUE;
                }

            CloseHandle(hFile);

            if (!bSuccess)
                return (0x01);
            }
#ifndef _M_ALPHA
        }
    else
        {
        //  Non NT.
        ABSDISKREC absDisk;
        BYTE byVxDResult;
        BYTE byThunkResult;

        MEMSET(&absDisk,0,sizeof(ABSDISKREC));
        absDisk.dn = lpAbsDiskTranslated->dn;


        // First, try symkrnlvxd method

        byVxDResult = _GetPhyInfoViaSymVxD(lpAbsDiskTranslated,lpAbsDiskActual);

        bSuccess = (byVxDResult == 0)? TRUE : FALSE;

        // Next, try Thunk using a local absdisk

        byThunkResult = _GetPhyInfoViaThunk(&absDisk);

        if (bSuccess && byThunkResult == 0)
            {
            // Everyone gets to know what kind of drive this is.
            lpAbsDiskTranslated->bIsExtInt13 = lpAbsDiskActual->bIsExtInt13 = absDisk.bIsExtInt13;

            // If lo and behold, this is an extended Int13 drive, then we've got
            // some REAL numbers that we can use.  Move them over to the Actual
            // drive metrics
            if (absDisk.bIsExtInt13 == ISINT13X_YES)
                {
                lpAbsDiskActual->dwSectorsPerTrack = absDisk.dwSectorsPerTrack;
                lpAbsDiskActual->dwTotalTracks     = absDisk.dwTotalTracks;
                lpAbsDiskActual->dwTotalHeads      = absDisk.dwTotalHeads;
                lpAbsDiskActual->dwMaxSectorsLo    = absDisk.dwMaxSectorsLo;
                lpAbsDiskActual->dwMaxSectorsHi    = absDisk.dwMaxSectorsHi;
                lpAbsDiskTranslated->dwMaxSectorsLo= absDisk.dwMaxSectorsLo;
                lpAbsDiskTranslated->dwMaxSectorsHi= absDisk.dwMaxSectorsHi;
                }
            }
        }
#endif // _M_ALPHA

#if defined(SYM_WIN32)
    if(HWIsNEC() && GetSystemMetrics(SM_CLEANBOOT) && !bSuccess)
        return 0x01;
#endif


    if (bSuccess)
        return 0;                       // No Error!
    else
        return (0x01);

#elif defined (SYM_VXD)

    auto   BOOL         bSuccess = FALSE;

            {
            auto   DCB          dcb;    // Device Control Block
            auto   DCB_BLOCKDEV bdd;
            auto   VxDDiskGetPhysicalInfo getInfo;

            getInfo.Int13Drv = lpAbsDiskTranslated->dn; // 80h, 81h, 82h... or 00h, 01h,
            getInfo.pDCB     = &dcb;    // DCB as defined in DCB.H & DCB.INC
            getInfo.pBDD     = &bdd;    // DCB_BLOCKDEV as defined in DCB.H & DCB.INC

            bSuccess = _IOSSrv_GetInfo ( &getInfo );

            if (bSuccess && getInfo.Result == 0)
                {
                auto BYTE CMOS_Type;
                auto BYTE sectorLen;
                auto int  iBytesPerSector;

                lpAbsDiskTranslated->dwSectorsPerTrack = bdd.DCB_apparent_spt;      /* Sectors on each track  */
                lpAbsDiskTranslated->dwTotalTracks     = bdd.DCB_apparent_cyl_cnt;  /* Total tracks on disk   */
                lpAbsDiskTranslated->dwTotalHeads      = bdd.DCB_apparent_head_cnt; /* Total heads on disk    */
                CMOS_Type   = 0;                                    /* CMOS drive type        */
                if (dcb.DCB_cmn.DCB_device_type == DCB_type_floppy)
                    {
                         // Translate the DCB actual geometry into CMOS values:
                         // Drive Type   Tracks   Sectors per Track  CMOS type
                         //   360K         40        9                 1
                         //   720K         80        9                 2
                         //   1.2M         80        15                3
                         //   1.44M        80        18                4
                         //   2.88M        80        36                5
                    if (dcb.DCB_actual_cyl_cnt == 40 && dcb.DCB_actual_spt == 9)
                        CMOS_Type = CMOS_360K;
                    else if (dcb.DCB_actual_cyl_cnt == 80)
                        {
                                        // Use the sectors per track field
                                        // for identification
                        if (dcb.DCB_actual_spt == 9)
                            CMOS_Type = CMOS_720K;
                        else if (dcb.DCB_actual_spt == 15)
                            CMOS_Type = CMOS_12M;
                        else if (dcb.DCB_actual_spt == 18)
                            CMOS_Type = CMOS_144M;
                        else if (dcb.DCB_actual_spt == 36)
                            CMOS_Type = CMOS_288M;
                        }
                    }
                lpAbsDiskTranslated->CMOS_Type   = CMOS_Type;                 /* CMOS drive type        */

                lpAbsDiskTranslated->numDrives       = (BYTE) getInfo.dwNumDrives;       /* Num. drives (floppy or hard) */

                                            /* Sector length code           */
			    		/* code  :   0	  1    2    3	*/
			    		/* length: 128  256  512 1024	*/
                iBytesPerSector = bdd.DCB_apparent_blk_size >> 8;
                for ( sectorLen = 0; iBytesPerSector; sectorLen++ )
                    iBytesPerSector = iBytesPerSector >> 1;
                lpAbsDiskTranslated->sectorLenCode = sectorLen;

                                        // Now lets setup the ACTUAL abs disk structure
                *lpAbsDiskActual = *lpAbsDiskTranslated;
                lpAbsDiskActual->dwSectorsPerTrack = dcb.DCB_actual_spt;      /* Sectors on each track  */
                lpAbsDiskActual->dwTotalTracks     = dcb.DCB_actual_cyl_cnt;  /* Total tracks on disk   */
                lpAbsDiskActual->dwTotalHeads      = dcb.DCB_actual_head_cnt; /* Total heads on disk    */
                iBytesPerSector = dcb.DCB_actual_blk_size >> 8;
                for ( sectorLen = 0; iBytesPerSector; sectorLen++ )
                    iBytesPerSector = iBytesPerSector >> 1;
                lpAbsDiskActual->sectorLenCode = sectorLen;
                }
            else
                bSuccess = FALSE;
            }

    if (!bSuccess)
        {
        return 0xFF;
        }
    else
        return 0;                       // No Error!

#elif defined(SYM_WIN16) || defined(SYM_DOS)
//this used to be #elif defined(SYM_DOS)
    auto    BYTE        err;
    auto	HDParamTable	driveTable;

					/*------------------------------*/
					/* First call the good old-	*/
					/* fashioned method to get the	*/
					/* parameters.			*/
					/*------------------------------*/

    err = _DiskGetPhysicalInfo(lpAbsDiskTranslated);


					/*------------------------------*/
					/* If err, return		*/
					/*------------------------------*/
    if ( err )
        return (err);
					/*------------------------------*/
					/* No error.  If Hard Disk, 	*/
					/* look at the disk tables	*/
					/* and use their #s for total	*/
					/* cylinders.			*/
					/*------------------------------*/
    if(HWIsNEC())
	{
    	if(!IsValidNECHDNumber(lpAbsDiskTranslated->dn))
	    return(DISK_PHYSICAL_ERROR);
	}

					/*---------------------------------------*/
					/* if floppy, return NOERR at this point */
    else                                /*---------------------------------------*/
  	{
      if ( lpAbsDiskTranslated->dn < FIRST_HD  )
          return (NOERR);
      else if ( lpAbsDiskTranslated->dn >= (FIRST_HD + 16) )
          return (DISK_PHYSICAL_ERROR);
	}


    DiskGetHDParamTable(lpAbsDiskTranslated->dn, &driveTable);

					/*------------------------------*/
					/* Disregard, if bogus		*/
					/*------------------------------*/

    if ( (driveTable.maxHeads == 0) || (driveTable.maxCylinders == 0) )
        return (DISK_PHYSICAL_ERROR);

					/*------------------------------*/
					/* Must match exactly		*/
					/*------------------------------*/

    if (lpAbsDiskTranslated->bIsExtInt13 == ISINT13X_NO)
        {
        if ( (driveTable.maxHeads != lpAbsDiskTranslated->dwTotalHeads) ||
        (driveTable.sectorsPerTrack != lpAbsDiskTranslated->dwSectorsPerTrack) )
            return (DISK_PHYSICAL_ERROR);
        }


					/*------------------------------*/
                                        /* Use the value from the table */
					/* but don't include diagnostic	*/
					/* cylinder.			*/
					/*------------------------------*/


    if (lpAbsDiskTranslated->bIsExtInt13 == ISINT13X_NO)
        {
        if ((driveTable.maxCylinders - 1) > lpAbsDiskTranslated->dwTotalTracks )
            {
            lpAbsDiskTranslated->dwTotalTracks = driveTable.maxCylinders - 1;
            }
        }

    return (0);
#endif           // not Win32,not VxD,not Win16, not DOS
    return 0xFF;
}



#if defined(SYM_PROTMODE)

/////////////////////////////////////////////////////////////////////
// Makes the old Int13 function call to aquire the basic disk information
//
// Note that this function also fills out the lpAbsDisk->bIsExtInt13.
// With this, the caller can decide if it is worth it to call
// _DiskGetPhysicalInfo () to aquire both the old and new Int13 information.
//
/////////////////////////////////////////////////////////////////////

BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfoBasic(LPABSDISKREC lpAbsDisk)
{
    union   REGS	regs;
    struct  SREGS	sregs;
    auto        BYTE        err = 0;


    regs.x.ax = 0x0801;             /* Get Disk Parameters          */
    regs.h.bl = 0xFF;               /* Set for later evaluation     */
    regs.x.cx = 0;
    regs.h.dl = lpAbsDisk->dn;      /* Get drive                    */
    IntWin(0x13, &regs, &sregs);
    if (regs.x.cflag || !regs.h.ch || !regs.h.cl || !regs.h.dl)
        {
        err = regs.h.ah;            /* Return the error code        */
        if (!err)
            err = 0x01;             /* If no error, return "Invalid */
                                    /* Function"                    */
        }
    else
        {
        auto    DWORD    cylinder;
        auto    DWORD    sector;

                                     /* Get maximum heads            */
        lpAbsDisk->dwTotalHeads = regs.h.dh + 1L;
                                    /* Unscramble cylinder/sector   */
        DiskUnscrambleBIOSCoding(regs.x.cx, &cylinder, &sector);
                                    /* Get maximum tracks           */
        lpAbsDisk->dwTotalTracks = cylinder + 1;
                                    /* Get sectors per track        */
        lpAbsDisk->dwSectorsPerTrack = sector;

                                    /* Save # of drives             */
        lpAbsDisk->numDrives = regs.h.dl;
                                    /* Save DDPT address            */
        lpAbsDisk->buffer = MAKELP(sregs.es, regs.x.di);
                                    /* Save CMOS type               */
        lpAbsDisk->CMOS_Type = regs.h.bl;
        lpAbsDisk->dwMaxSectorsLo = ConvertPhySectorToLong(lpAbsDisk,
                                          lpAbsDisk->dwTotalHeads - 1,
                                          lpAbsDisk->dwTotalTracks - 1,
                                          lpAbsDisk->dwSectorsPerTrack) + 1;
        lpAbsDisk->dwMaxSectorsHi = 0;
        }


    // check to see if int 13 extensions are present
    lpAbsDisk->bIsExtInt13 = DiskIsExtendedInt13Drive(lpAbsDisk);

    return(err);
}

#elif defined(SYM_DOS)


BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfoBasic(LPABSDISKREC lpAbsDisk)
{

    auto        BYTE        err = 0;

    _asm
        {
        push    DS                      ; DS needs to be saved for caller.
        lds     bx, lpAbsDisk           ; Get ptr. to record
        mov     dl, [bx]LPABSDISKREC.dn ; Get physical drive number

        xor     cx, cx                  ; Clear a few registers
        mov     bl, 0FFh                ; Set for later evaluation

        mov     ax, 0801h               ; Read drive parameters
        int     13h                     ; Disk service handler
        jc      errorLabel              ; If carry, then error

        or      ch, ch                  ; Is it 0 ?
        jz      invalidFunction         ; Error

        or      cl, cl                  ; Is it 0 ?
        jz      invalidFunction         ; Error

        or      dl, dl                  ; Is number of drives installed == 0 ?
        jz      invalidFunction         ; Error

; Assume results are correct, since used multi-layered testing

        push    bx                      ; Save CMOS type

        lds     bx, lpAbsDisk           ; Get ptr. to abs disk record
        inc     dh                      ;
                                        ; Save maximum heads
        mov     byte ptr ([bx]LPABSDISKREC.dwTotalHeads), dh
        mov     ax, cx                  ; Get track/sector combo
        rol     al, 1                   ;
        rol     al, 1                   ;
        and     al, 3                   ; Remove 2 hi-bits
        xchg    al,ah                   ; Swap them so hi is hi!   HI!
        inc     ax
                                        ; Save maximum tracks
        mov     word ptr ([bx]LPABSDISKREC.dwTotalTracks), ax
        and     cl, 3Fh                 ; Remove track bits
                                        ; Save maximum sectors
        mov     byte ptr ([bx]LPABSDISKREC.dwSectorsPerTrack), cl
                                        ; Save number of drives
        mov     byte ptr [bx]LPABSDISKREC.numDrives, dl
                                        ; Save DDPT
        mov     word ptr ([bx]LPABSDISKREC.buffer), di
        mov     word ptr ([bx]LPABSDISKREC.buffer+2), es
        pop     ax                      ; Get saved CMOS drive type

                                        ; Save CMOS drive type
        mov     [bx]LPABSDISKREC.CMOS_Type, al

        xor     ax, ax                  ; Signal NO ERROR
        jmp     short returnLabel       ; Return to caller

errorLabel:
        xchg    ah, al                  ; Reverse error code
        xor     ah, ah                  ; Clear high-byte
        or      ax, ax                  ; Is it 0?
        jnz     returnLabel             ; No, return

invalidFunction:
        mov     ax, 1                   ; Return 'Invalid function'

returnLabel:
        mov     err,al
        pop     DS                      ;return caller's DS
        }
 // Now make the extended Int13 call, if applicable.

    lpAbsDisk->bIsExtInt13 = DiskIsExtendedInt13Drive(lpAbsDisk);
    return(err);
}


#endif




#if defined(SYM_PROTMODE)
BYTE SYM_EXPORT WINAPI _DiskGetPhysicalInfo (LPABSDISKREC lpAbsDisk)
{
    union   REGS	regs;
    struct  SREGS	sregs;
    auto    BYTE    err = 0;
    auto    BYTE	sectorLen;
    auto    BYTE	CMOS_Type;
    auto    WORD	numMask;
    auto    BYTE	wNum;
    auto    WORD	wEquipment;
	auto    DWORD           dwSegSel;
	auto    LPBYTE          lpBuf;
	auto	LPINT13EXTREC	lpExtInt13Info;
    auto    DWORD       dwTracks;
    auto    int         i;


    if (HWIsNEC())
        {
        regs.h.ah = 0x84;                     /* Extended Sense       */
        regs.h.al = lpAbsDisk->dn;           /* Get drive            */
        regs.x.cx = 0xffff;                   /* Set for later evaluation */
        IntWin(0x1b, &regs, &sregs);

        if ( lpAbsDisk->dn & 0x50 )
            {
            if ( regs.x.cflag && ( ( regs.h.ah & 0xf0 ) != 0x60 ) )
                {
                err = (BYTE)( regs.h.ah & 0xf0 ); /* Return the error code    */
                if ( !err )
                    err = 0x01;            /* If no error, return "Invalid    */
                                        /* Function"            */
                return ( err );
                }
            /* Floppy */
            CMOS_Type = 2;        /* type 1.2M */
            numMask = 0xf0ff;
            }
        else
            {
            /* HD */
            if ( regs.x.cflag )
                {
                err = (BYTE)( regs.h.ah & 0xf0 ); /* Return the error code    */
                if ( !err )
                    err = 0x01;             /* If no error, return "Invalid    */
                                        /* Function"            */
                return ( err );
                }
                                /* Get maximum heads */
            lpAbsDisk->dwTotalHeads = regs.h.dh;
                                /* Get maximum tracks */
            lpAbsDisk->dwTotalTracks = regs.x.cx;
                                /* Get sectors per track */
            lpAbsDisk->dwSectorsPerTrack = regs.h.dl;
                                /* Get sectorLencode */
            for ( sectorLen = 0; regs.h.bh; sectorLen++ )
                regs.h.bh = (BYTE)( regs.h.bh >> 1 );
            lpAbsDisk->sectorLenCode = sectorLen;

            CMOS_Type = 0;
            numMask = 0x0300;
            }

        wNum = 0;
        MemCopyPhysical(&wEquipment, MAKELP(0, 0x055C), sizeof(wEquipment), 0);
        wEquipment = wEquipment & numMask;
        for ( ; wEquipment; wEquipment = wEquipment >> 1 )
            {
            if ( wEquipment & 1 )
                wNum++;
            }
											/* Save # of drives              */
        lpAbsDisk->numDrives = wNum;
											/* Save CMOS type                */
        lpAbsDisk->CMOS_Type = CMOS_Type;
        err = 0;							/* No error                      */
        }

    else // !NEC
        {

        /*
         *  Even though this may be an extended Int13 drive,
         *  do the old int 13 call first because the new call
         *  does not supply all the information.
         */

        err = DiskGetPhysicalInfoBasic(lpAbsDisk);

        if(lpAbsDisk->bIsExtInt13 == ISINT13X_YES)
			{
            // Use the extended int 13 function to get parameters
			dwSegSel = GlobalDosAlloc(sizeof(INT13EXTREC));
			if (dwSegSel == NULL)
				return (0x01);				// return "invalid fn"

			lpBuf = (LPBYTE) MAKELP (dwSegSel, 0);
			lpExtInt13Info = (LPINT13EXTREC) lpBuf;

			MEMSET(lpExtInt13Info, 0, sizeof(INT13EXTREC));
			lpExtInt13Info->info_size = sizeof(INT13EXTREC);

            regs.x.ax = 0x4800;             // Get Drive Parameters
            regs.h.dl = lpAbsDisk->dn;      // Drive number
			regs.x.si = 0;
			sregs.ds = HIWORD(dwSegSel);
			IntWin(0x13, &regs, &sregs);
            if (regs.x.cflag)
				{
                err = regs.h.ah;            // Return the error code
				if (!err)
                    err = 0x01;             // If no error, return "Invalid
                                            // Function"
				}
            else
                {
                // Ensure the flag that tells us we can rely on the CHS values
                // is set. Also, ensure that CHS values are non-zero (don't just
                // trust the flag).
                if ((lpExtInt13Info->flags & 0x0002) &&
                    (lpExtInt13Info->cylinders != 0) &&
                    (lpExtInt13Info->heads != 0) &&
                    (lpExtInt13Info->sec_per_track != 0))

                    {
                                            // Get maximum tracks
                    lpAbsDisk->dwTotalTracks = lpExtInt13Info->cylinders;
                                            // Get maximum heads
                    lpAbsDisk->dwTotalHeads = lpExtInt13Info->heads;
                                            // Get sectors per track
                    lpAbsDisk->dwSectorsPerTrack = lpExtInt13Info->sec_per_track;
                    }
                else
                    {
                    /* NOTE:  This flag is clear to indicate that the CHS
                     * values are not valid.  In that case, the sectors per
                     * track already in lpAbsDisk should be correct since
                     * we got this from a plain int13. Use that to intelligently
                     * find a CHS combo that will give us a best fit and result
                     * in a scheme with at least 4 heads. The result we're
                     * after should give us a large number of cylinders relative
                     * to the number of heads, which is realistic.
                     *
                     * This will allow the ConvertPhySectorToLong() and other calls
                     * to continue to work.
                     */

                    if(lpExtInt13Info->sectorsLo != 0)
                        {

                        // Ensure we don't divide by zero
                        if(lpAbsDisk->dwSectorsPerTrack != 0L)
                            {
                            dwTracks  = lpExtInt13Info->sectorsLo/lpAbsDisk->dwSectorsPerTrack;
                            // Start with a reasonably realistic number as a base
                            lpAbsDisk->dwTotalHeads = 4;
                            for(i = 255; i > 4; i--)
                                {
                                // Is the number evenly divisible by i?
                                if((dwTracks % i) == 0)
                                    {
                                    lpAbsDisk->dwTotalHeads = (DWORD) i;
                                    break;
                                    }
                                }

                            lpAbsDisk->dwTotalTracks  = dwTracks / lpAbsDisk->dwTotalHeads;

                            // Indicate we're using our own translation scheme
                            lpAbsDisk->flags |= TRANSLATION_ARBITRARY;
                            }
                        else
                            {
                            // Indicate an error occurred
                            err = 0x01;             // Return "Invalid Function"
                            }
                        }
                    else
                        {
                        // Total sectors is zero, so we can't use it to do our
                        // own translation. Just leave the plain int 13 values
                        // in our absDisk and use them to calculate total sectors
                        lpExtInt13Info->sectorsLo = lpAbsDisk->dwTotalTracks *
                                                    lpAbsDisk->dwTotalHeads *
                                                    lpAbsDisk->dwSectorsPerTrack;

                        lpExtInt13Info->sectorsHi = 0;
                        }
                    }
                lpAbsDisk->dwMaxSectorsLo = lpExtInt13Info->sectorsLo;
                lpAbsDisk->dwMaxSectorsHi = lpExtInt13Info->sectorsHi;
                }
		    GlobalDosFree(LOWORD(dwSegSel));
			}
        }

    return(err);
}

#elif defined(SYM_DOS)

#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI _DiskGetPhysicalInfo (LPABSDISKREC lpAbsDisk)
{
    auto        BYTE        err = 0;
    auto        DWORD       dwTracks;
    auto        int         i;

        /*
         *  Even though this may be an extended Int13 drive,
         *  do the old int 13 call first for 2 reasons:
         *  1)  It will fill out parameters that the new call does not address
         *  2)  Should the drive be SCSI, the new call will not provide
         *      CHS values. In that case, they will have been filled out
         *      to their max values by the old int 13 call.
         */


    err = DiskGetPhysicalInfoBasic(lpAbsDisk);


    // Use the extended int 13 function to get parameters
    if(lpAbsDisk->bIsExtInt13 == ISINT13X_YES)
        {
        auto    BYTE        drive = lpAbsDisk->dn;
        auto    INT13EXTREC extRec;
        auto    INT13EXTREC * lpextRec = &extRec;


        MEMSET(&extRec,0,sizeof(INT13EXTREC));


        _asm
            {
            push    DS                      ;Save caller's ds
            mov     dl,drive
            lds     SI,lpextRec             ;point DS:SI to extRec
            mov     ax,4800h                ;make the info call.
            Int     13h
            pop     DS                      ;get back to reality
            jnc     ExtDone

            mov     err,1                   ;assume Invalid function call
            cmp     ah,0                    ;Was an error code retuned
            je      ExtDone                 ;if so, use it instead
            mov     err,ah
ExtDone:
            }

        if (err == 0)
            {
            // Ensure the flag that tells us we can rely on the CHS values
            // is set. Also, ensure that CHS values are non-zero (don't just
            // trust the flag).
            if ((extRec.flags & 0x0002) &&
                (extRec.cylinders != 0) &&
                (extRec.heads != 0) &&
                (extRec.sec_per_track != 0))
                {
                                            // Get maximum tracks
                lpAbsDisk->dwTotalTracks = extRec.cylinders;
                                            // Get maximum heads
                lpAbsDisk->dwTotalHeads = extRec.heads;
                                            // Get sectors per track
                lpAbsDisk->dwSectorsPerTrack = extRec.sec_per_track;
                }
            else
                {
                /* NOTE:  This flag is clear to indicate that the CHS
                 * values are not valid.  In that case, the sectors per
                 * track already in lpAbsDisk should be correct since
                 * we got this from a plain int13. Use that to intelligently
                 * find a CHS combo that will give us a best fit and result
                 * in a scheme with at least 4 heads. The result we're
                 * after should give us a large number of cylinders relative
                 * to the number of heads, which is realistic.
                 *
                 * This will allow the ConvertPhySectorToLong() and other calls
                 * to continue to work.
                 */
                if(extRec.sectorsLo != 0)
                    {
                    // Ensure we don't divide by zero
                    if(lpAbsDisk->dwSectorsPerTrack != 0L)
                        {
                        dwTracks  = extRec.sectorsLo/lpAbsDisk->dwSectorsPerTrack;
                        // Start with a reasonably realistic number as a base
                        lpAbsDisk->dwTotalHeads = 4;
                        for(i = 255; i > 4; i--)
                            {
                            // Is the number evenly divisible by i?
                            if((dwTracks % i) == 0)
                                {
                                lpAbsDisk->dwTotalHeads = (DWORD) i;
                                break;
                                }
                            }

                        lpAbsDisk->dwTotalTracks  = dwTracks / lpAbsDisk->dwTotalHeads;

                        // Indicate we're using our own translation scheme
                        lpAbsDisk->flags |= TRANSLATION_ARBITRARY;
                        }
                    else
                        {
                        // Indicate an error occurred
                        err = 0x01;             // Return "Invalid Function"
                        }
                    }
                else
                    {
                    // Total sectors is zero, so we can't use it to do our
                    // own translation. Just leave the plain int 13 values
                    // in our absDisk and use them to calculate total sectors
                    extRec.sectorsLo = lpAbsDisk->dwTotalTracks *
                                       lpAbsDisk->dwTotalHeads *
                                       lpAbsDisk->dwSectorsPerTrack;

                    extRec.sectorsHi = 0;
                    }
                }
            lpAbsDisk->dwMaxSectorsLo = extRec.sectorsLo;
            lpAbsDisk->dwMaxSectorsHi = extRec.sectorsHi;
            }
        }
    else
        {
        // if not Int13x drive, get this info anyway.
        lpAbsDisk->dwMaxSectorsLo = ConvertPhySectorToLong(lpAbsDisk,
                                              lpAbsDisk->dwTotalHeads - 1,
                                              lpAbsDisk->dwTotalTracks - 1,
                                              lpAbsDisk->dwSectorsPerTrack) + 1;
        lpAbsDisk->dwMaxSectorsHi = 0;
        }

    return (err);
}
#pragma optimize("", on)

#endif



#ifdef SYM_WIN32
BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfoEx (LPABSDISKREC lpAbsDiskTranslated, LPABSDISKREC lpAbsDiskActual)
{
    return(_DiskGetPhysicalInfoEx(lpAbsDiskTranslated, lpAbsDiskActual));
}
#endif


//-----------------------------------------------------------------------------
//  DiskGetPhysicalInfo ()
//      Gets the drive parameters returned by Int 13h, function 08h.
//              This function will clobber the absDisk field, 'buffer'
//              by having it point to the DDPT for a floppy disk.  Also
//              the CMOS_Type field is only valid for floppy disks.
//
// NOTE: Call this function before calling DiskAbsOperation().
//---------------------------------------------------------------------------
BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfo (LPABSDISKREC lpAbsDisk)
{


#if defined(NCD)
    return(_DiskGetPhyInfoNEC(lpAbsDisk));

#elif defined(SYM_WIN32)

    auto ABSDISKREC Actual;
    BYTE byResult;

    MEMSET(&Actual,0,sizeof(ABSDISKREC));

    byResult =_DiskGetPhysicalInfoEx(lpAbsDisk, &Actual);

    if (byResult == 0 && Actual.bIsExtInt13 == ISINT13X_YES)
       {
       lpAbsDisk->dwSectorsPerTrack = Actual.dwSectorsPerTrack;
       lpAbsDisk->dwTotalTracks     = Actual.dwTotalTracks;
       lpAbsDisk->dwTotalHeads      = Actual.dwTotalHeads;
       lpAbsDisk->dwMaxSectorsLo    = Actual.dwMaxSectorsLo;
       lpAbsDisk->dwMaxSectorsHi    = Actual.dwMaxSectorsHi;
       }
    return byResult;
#else
    auto ABSDISKREC NotUsed;

    return(_DiskGetPhysicalInfoEx(lpAbsDisk, &NotUsed));

#endif
}


#if !defined(SYM_WIN32) && !defined(SYM_VXD) && !defined(SYM_NTK)
/*----------------------------------------------------------------------*/
/* DiskGetHDParamTable(Byte dn, HDParamTable *driveParamTable);		*/
/*									*/
/*	Gets the current HD Param Table for the drive number in 'dn'.	*/
/*									*/
/*	Returns TRUE if no error, else FALSE				*/
/*----------------------------------------------------------------------*/

BYTE SYM_EXPORT WINAPI DiskGetHDParamTable(BYTE dn, HDParamTable FAR *driveParamTable)
{
    void far*   realModeOldTablePtr;
    void far*	oldTablePtr;

					/* Get old Int 41h or 46h table	*/
    DOSGetIntHandler((BYTE)((dn & 0x01) ? 0x46 : 0x41), (LPVOID) &realModeOldTablePtr);

    					/* Copy the table to our table	*/
    oldTablePtr = GetProtModePtr( realModeOldTablePtr );
    *driveParamTable = * (HDParamTable far*) oldTablePtr;
    SelectorFree( FP_SEG(oldTablePtr) );

    return (TRUE);
}
#endif

/*----------------------------------------------------------------------*/
/* UnscrambleBIOSCoding ()                                              */
/*      Takes an encoded BYTE-pair representing the cylinder, sector on */
/*      a disk, with the lower 8-bits of the cylinder, in the 2nd BYTE, */
/*      but the top 2-bits of the cylinder are in the first BYTE.       */
/*                                                                      */
/*      Returns the unencoded values for the cylinder and sector.       */
/*----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI DiskUnscrambleBIOSCoding (WORD encodedValue, DWORD FAR * cylinderPtr, DWORD FAR *  sectorPtr)
{
                                        /* Compute cylinder             */
    *cylinderPtr = HIBYTE(LOWORD(encodedValue)) + ((encodedValue << 2) & 0x0300);

                                        /* Remove 2 top bits            */
    *sectorPtr = (((BYTE) encodedValue) & (BYTE) 0x3F);
}



/*----------------------------------------------------------------------*/
/* ScrambleBIOSCoding ()                                                */
/*      Takes a cylinder and sector and encodes them so the BIOS has    */
/*      its way with the lower 8-bits of the cylinder, in the 2nd BYTE, */
/*      but the top 2-bits of the cylinder are in the first BYTE.       */
/*                                                                      */
/*      Returns the encoded values for the cylinder and sector.         */
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI DiskScrambleBIOSCoding (DWORD track, DWORD sector)
{
    auto        DWORD    encodedValue;


    encodedValue = (track << 8);

    encodedValue |= ((track >> 2) & 0x00C0);
    encodedValue += sector;

    return (encodedValue);
}


/*----------------------------------------------------------------------*/
/* DiskGetPartitionTableEntry()                                         */
/*      Gets an entry from the partition table.  Can get either an      */
/*      absDisk version of it, or a PARTENTRY.                          */
/*                                                                      */
/*      NOTE: uEntry is 0-based.                                        */
/*            lpAbsDisk and/or lpPartEntry can be NULL.  They won't be  */
/*            filled in.                                                */
/*                                                                      */
/*	Returns TRUE if no error, else FALSE				*/
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskGetPartitionTableEntry(BYTE dn,
                                UINT uEntry,
                                LPPARTENTRYREC lpPartEntry,
                                LPABSDISKREC lpAbsDisk)
{
    ABSDISKREC          absDisk;
    LPPARTENTRYREC      lpLocalPartEntry;
    BOOL                bRet = FALSE;
    DWORD               Track;
    DWORD               Sector;

                                        // Read the MBR
    absDisk.dn = dn;
    DiskGetPhysicalInfo(&absDisk);

    absDisk.dwHead = 0;
    absDisk.dwTrack = 0;
    absDisk.dwSector = 1;
    absDisk.numSectors = 1;
    absDisk.buffer = MemAllocPtr(GHND, PHYSICAL_SECTOR_SIZE);

    if ( !absDisk.buffer )
        {
        return (FALSE);
        }

    if ( !DiskAbsOperation(READ_COMMAND, &absDisk) )
        {
                                        // Grab the Partition entry we want
        lpLocalPartEntry = (LPPARTENTRYREC)(absDisk.buffer + PART_TABLE_OFFSET) + uEntry;

        if ( lpPartEntry )
            {
            *lpPartEntry = *lpLocalPartEntry;
            }
                                        // Convert the partition table entry
                                        // into an AbsDisk Rec
        if ( lpAbsDisk )
            {
            lpAbsDisk->dn = dn;
                                        /* Unscramble 10-bit cylinder   */
            DiskUnscrambleBIOSCoding (*((LPWORD) &(lpLocalPartEntry->startSector)), &Track, &Sector);
            lpAbsDisk->dwStartingHead   = lpLocalPartEntry->startHead;
            lpAbsDisk->dwStartingTrack  = Track;
            lpAbsDisk->dwStartingSector = Sector;

            DiskUnscrambleBIOSCoding (*((LPWORD) &(lpLocalPartEntry->endSector)), &Track, &Sector);
            lpAbsDisk->dwEndingHead   = lpLocalPartEntry->endHead;
            lpAbsDisk->dwEndingTrack  = Track;
            lpAbsDisk->dwEndingSector = Sector;
            }

        bRet = TRUE;
        }

    MemFreePtr(absDisk.buffer);
    return(bRet);
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

BYTE SYM_EXPORT WINAPI DiskReadMBR (LPABSDISKREC lpAbsDisk)
{
    DiskGetPhysicalInfo(lpAbsDisk);
    lpAbsDisk->dwHead   = 0;
    lpAbsDisk->dwTrack  = 0;
    lpAbsDisk->dwSector = 1;
    lpAbsDisk->numSectors = 1;
    return ( DiskAbsOperation(READ_COMMAND, lpAbsDisk) );
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

BYTE SYM_EXPORT WINAPI DiskWriteMBR (LPABSDISKREC lpAbsDisk)
{
    DiskGetPhysicalInfo(lpAbsDisk);
    lpAbsDisk->dwHead   = 0;
    lpAbsDisk->dwTrack  = 0;
    lpAbsDisk->dwSector = 1;
    lpAbsDisk->numSectors = 1;
    return ( DiskAbsOperation(WRITE_COMMAND, lpAbsDisk) );
}

/**************************************************************************/
/*IsExtendedInt13Drive
 *
 *
 *PARAMS :  Check if this is an extended FAT32 drive. This is done by
 *          calling the new documented CheckExtensionsPresent call.
 *
 *
 *RETURNS:  ISINT13X_NO or ISINT13X_YES.
 *
 *
 *************************************************************************
 * Created by: SKURTZ   12-11-97 04:56:01pm
 **************************************************************************/
BYTE SYM_EXPORT WINAPI DiskIsExtendedInt13Drive(LPCABSDISKREC lpAbsDisk)
{
// !!! The only function we have changed in our custom SymKrnl
    return ISINT13X_NO;
}   /*IsExtendedInt13Drive*/

