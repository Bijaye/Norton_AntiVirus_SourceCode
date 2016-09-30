/* Copyright 1994 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/THUNK.H_v   1.1   26 Aug 1997 07:52:26   gvogel  $ *
 *                                                                      *
 * Description:                                                         *
 *      Contains stuff relating to thunking down from 32-bit to 16-bit  *
 *      code.                                                           *
 *                                                                      *
 * Contains:                                                            *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/THUNK.H_v  $ *
// 
//    Rev 1.1   26 Aug 1997 07:52:26   gvogel
// Added DiskIsSubst to thunks.
// 
//    Rev 1.0   26 Jan 1996 20:21:14   JREARDON
// Initial revision.
// 
//    Rev 1.37   16 Jun 1995 16:56:20   DAVID
// Once again, do not #include toolhelp.h on the W32 platform. 
// 
//    Rev 1.36   30 May 1995 18:09:58   REESE
// Added HWIsPCI function.
// 
//    Rev 1.35   19 Mar 1995 19:16:14   TORSTEN
// Added a typedef for SYMSYSHEAPINFO for WIN16.
// 
//    Rev 1.34   18 Mar 1995 22:25:18   DAVID
// Do not #include toolhelp.h on the W32 platform.  It defines a MAX_PATH
// of 255 which conflicts with one defined by the Win95 SDK headers.
// 
//    Rev 1.33   10 Mar 1995 01:18:48   BRUCE
// 
// Changed the lpbSwaped parm to _DiskIsDoubleSpace() to be a pointer to a
// WORD instead of a ptr to a BOOL because BOOL changes size between 32 and 16
// sides (and the thunk compiler doesn't do well with that).
// 
//    Rev 1.32   04 Jan 1995 16:44:30   RGLENN
// Added StackerFlushDrive()
//
//    Rev 1.31   21 Dec 1994 16:08:24   BRAD
// Make long/short name functions consistent with Quake APIs.
//
//    Rev 1.30   13 Dec 1994 15:28:20   BRAD
// Only include include files if SYM_WIN
//
//    Rev 1.29   13 Dec 1994 13:31:14   BILL
//
//    Rev 1.28   09 Dec 1994 20:21:36   BRAD
// Removed platwin.h reference
//
//    Rev 1.27   09 Dec 1994 20:03:02   BILL
// Fixed toolhelp/platwin problem
//
//    Rev 1.26   09 Dec 1994 19:40:50   BILL
// Removed toolhelp.h from win32
//
//    Rev 1.25   09 Dec 1994 17:13:06   BILL
// Fixed header so it compiles
//
//    Rev 1.24   09 Dec 1994 15:52:26   BRAD
// Move TOOLHELP stuff into Win16-specific section
//
//    Rev 1.23   09 Dec 1994 15:19:52   HENRI
// Added toolhelp.h include
//
//    Rev 1.22   09 Dec 1994 15:12:50   HENRI
// Added SystemHeapInfo and GetSystemResources thunks
//
//    Rev 1.21   21 Nov 1994 19:06:20   BRUCE
// Added NameGetLong/ShortName() functions and thunks to get to them
//
//    Rev 1.20   16 Nov 1994 18:20:08   BRAD
// No longer thunk for MemCopyPhysical()
//
//    Rev 1.19   02 Nov 1994 20:11:38   RGLENN
// ore Stacker STuff
//
//    Rev 1.18   01 Nov 1994 11:58:24   DBUCHES
// Added StackerDiskBlockDevice thunk
//
//    Rev 1.17   27 Oct 1994 15:34:08   RGLENN
// Added thunks for StackerGet/PutUnitData
//
//    Rev 1.16   04 Oct 1994 16:13:08   DBUCHES
// Added thunk for DiskGetDriverTable
//
//    Rev 1.15   03 Oct 1994 17:00:30   DBUCHES
// Added thunk for DiskGetDASDType()
//
//    Rev 1.14   28 Sep 1994 18:27:00   DBUCHES
// Added thunk for DiskGetLockedTable
//
//    Rev 1.13   21 Sep 1994 16:06:26   DAVID
// More workarounds for thunk-induced exception violations under the
// debugger.  All pointers fields in structures are now declared as DWORDs
// to the thunk compiler, and an extra function argument is used to translate
// the field value separately.  The translated value is stuffed back into the
// structure before the 16-bit code is executed, and the original field value
// is restored before returning control back to the 32-bit caller.
//
//    Rev 1.12   27 Jul 1994 16:18:12   DBUCHES
// Added thunk for DiskRecToFloppyAbsDiskRec
//
//    Rev 1.11   25 Jul 1994 13:33:50   DBUCHES
// Added thunk for _DiskIsDoubleSpace()
//
//    Rev 1.10   14 Jul 1994 14:59:50   HENRI
// 16 bit thunk prototypes should always be available
//
//    Rev 1.9   12 Jul 1994 18:42:38   HENRI
// Moved thunk prototypes from XAPI.H and DISK.H into this file
//
//    Rev 1.8   10 May 1994 16:08:28   MEIJEN
// Add THUNK_EISACMOSREAD, THUNK_EISACMOSWRITE for RESCUE future use
//
//    Rev 1.7   29 Apr 1994 13:04:56   BRAD
// Added UTTRANSLATIONPROC
//
//    Rev 1.6   29 Apr 1994 12:40:18   BRAD
// Added NOT_SUPPORTED
//
//    Rev 1.5   27 Apr 1994 11:58:10   BRAD
// Added DiskReset() and DiskResetDOS() to Win32s thunks
//
//    Rev 1.4   26 Apr 1994 15:44:42   BRAD
// Added MemCopyPhysical() as Win32s thunk
//
//    Rev 1.3   26 Apr 1994 12:06:46   BRAD
// Added DiskMapLogToPhyParams() and DiskGetBootDrive() to Win32s thunks
//
//    Rev 1.2   25 Apr 1994 17:45:40   BRAD
// Added DiskBlockDevice() and DiskGetInfo() to WIN32s thunk
//
//    Rev 1.1   22 Apr 1994 18:14:08   BRAD
// Added Win32s thunks for DiskAbsOperation() and DiskGetPhysicalInfo()
//
//    Rev 1.0   30 Mar 1994 16:00:52   BRAD
// Initial revision.
//
//    Rev 1.0   30 Mar 1994 15:59:56   BRAD
// Initial revision.
 ************************************************************************/

#ifndef INCL_THUNK
#define INCL_THUNK

#if defined(SYM_WIN)                    // Only relevant for WIN16 and WIN32

#include "disk.h"
#include "cdisk.h"

#if defined(SYM_WIN32)
#   define W32SUT_32
#else
#   define W32SUT_16
#   include <toolhelp.h>
#endif


#if defined(SYM_WIN16)
    typedef SYSHEAPINFO SYMSYSHEAPINFO;
#endif

#include <w32sut.h>                     // WIN32s UT header




#if defined(SYM_WIN32)

typedef BOOL (WINAPI *PUTREGISTER) ( HANDLE     hModule,
                                      LPCSTR     lpsz16BitDLL,
                                      LPCSTR     lpszInitName,
                                      LPCSTR     lpszProcName,
                                      UT32PROC * ppfn32Thunk,
                                      FARPROC    pfnUT32Callback,
                                      LPVOID     lpBuff
                                    );
typedef VOID (WINAPI *PUTUNREGISTER) (HANDLE hModule);

DWORD SYM_EXPORT WINAPI ThunkProc(
        LPVOID  lpData,
        DWORD   dwFunction,
        LPVOID  *lpTranslationList);

/*----------------------------------------------------------------------*
 * Thunker helpers function prototypes					*
 *									*
 * These functions are used internally by SYMKRNL, a user should not 	*
 * call these directly from a program.  Call the original function	*
 * (without the '32' or '16' suffix).					*
 *----------------------------------------------------------------------*/
BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfo32 (LPABSDISKREC lpAbsDisk);
BYTE SYM_EXPORT WINAPI DiskAbsOperation32 (BYTE command, LPABSDISKREC lpAbsDisk);
UINT SYM_EXPORT WINAPI DiskBlockDevice32 (UINT uCommand, LPDISKREC lpDisk);
VOID SYM_EXPORT WINAPI DiskMapLogToPhyParams32 (DWORD dwSector, BYTE dl,
                                   LPINT13REC lpInt13Package);
BOOL SYM_EXPORT WINAPI DiskGetInfo32 (BYTE dl, LPDISKREC lpDisk);
BYTE SYM_EXPORT WINAPI DiskGetBootDrive32(VOID);
VOID SYM_EXPORT WINAPI DiskResetDOS32 (BYTE dl);
VOID SYM_EXPORT WINAPI DiskReset32(VOID);
BOOL SYM_EXPORT WINAPI DiskGetHDParams32 (LPDISKREC diskPtr, LPABSDISKREC absDiskPtr,
				        LPBPBREC BPBPtr);
int SYM_EXPORT WINAPI DiskGetPhysicalType32(LPDISKREC diskPtr, LPABSDISKREC absDiskPtr);
BOOL SYM_EXPORT WINAPI   DiskIsCompressed32(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsDoubleSpace32(BYTE drive);
BOOL SYM_EXPORT WINAPI DiskIsBlockDevice32 (BYTE dl);
BOOL SYM_EXPORT WINAPI DiskIsSubst32(BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskIsDriveSpace32(BYTE drive);
BOOL SYM_EXPORT WINAPI   DiskIsStacker32(BYTE byDriveLetter);
BYTE SYM_EXPORT WINAPI EISACMOSRead32(BYTE slot, BYTE funct, LPBYTE address);
BYTE SYM_EXPORT WINAPI EISACMOSWrite32(WORD length, LPBYTE address);
BYTE SYM_EXPORT WINAPI CMOSRead32(BYTE address);
VOID SYM_EXPORT WINAPI CMOSWrite32(BYTE address, BYTE CMOSdata);
BOOL SYM_EXPORT WINAPI HWIsPCI32(VOID);
BOOL SYM_EXPORT WINAPI HWIsEISA32(VOID);
BOOL SYM_EXPORT WINAPI HWIsMicroChannel32(VOID);
BOOL SYM_EXPORT WINAPI _DiskIsDoubleSpace32(BYTE drive, LPBYTE lpbyHostDriveLetter,
                                   LPWORD lpbSwapped, LPBYTE lpbySequence);
BOOL SYM_EXPORT WINAPI DiskRecToFloppyAbsDiskRec32 (LPCDISKREC diskPtr, LPABSDISKREC lpAbsDisk);
VOID SYM_EXPORT WINAPI DiskGetLockedTable32(LPBYTE lockedTablePtr);
BOOL SYM_EXPORT WINAPI DiskGetDASDType32(BYTE drive, LPBYTE DASDTypePtr);
BOOL SYM_EXPORT WINAPI DiskGetDriverTable32 (BYTE dl, LPDISKREC lpDiskPtr);
BOOL SYM_EXPORT WINAPI StackerGetUnitData32(BYTE drive, LPPERUNIT lpPerUnit);
BOOL SYM_EXPORT WINAPI StackerPutUnitData32(BYTE drive, LPPERUNIT lpPerUnit);
UINT SYM_EXPORT WINAPI   STK20DiskBlockDeviceThunk32(UINT command, LPDISKREC diskPtr);
BOOL SYM_EXPORT WINAPI StackerFlushDrive32(BYTE drive);
STATUS SYM_EXPORT WINAPI NameReturnLongName32(LPCSTR lpszSpec, LPSTR lpszLongSpec);

#endif

BOOL SYM_EXPORT WINAPI SystemHeapInfo16(SYMSYSHEAPINFO FAR * lpSysHeap);
typedef LPVOID (WINAPI *UTTRANSLATIONPROC) (LPBYTE lpByte);
BYTE SYM_EXPORT WINAPI DiskGetPhysicalInfo16 (LPABSDISKREC lpAbsDisk);
BYTE SYM_EXPORT WINAPI DiskAbsOperation16 (BYTE command, LPABSDISKREC lpAbsDisk, LPVOID buffer);
UINT SYM_EXPORT WINAPI DiskBlockDevice16 (UINT uCommand, LPDISKREC lpDisk, LPVOID buffer);
VOID SYM_EXPORT WINAPI DiskMapLogToPhyParams16 (DWORD dwSector, BYTE dl,
                                   LPINT13REC lpInt13Package);
BOOL SYM_EXPORT WINAPI DiskGetInfo16 (BYTE dl, LPDISKREC lpDisk);
BYTE SYM_EXPORT WINAPI DiskGetBootDrive16(VOID);
VOID SYM_EXPORT WINAPI DiskResetDOS16 (BYTE dl);
VOID SYM_EXPORT WINAPI DiskReset16(VOID);
BOOL SYM_EXPORT WINAPI DiskGetHDParams16 (LPDISKREC diskPtr, LPABSDISKREC absDiskPtr,
				        LPBPBREC BPBPtr);
int SYM_EXPORT WINAPI DiskGetPhysicalType16(LPDISKREC diskPtr, LPABSDISKREC absDiskPtr);
BOOL SYM_EXPORT WINAPI   DiskIsCompressed16(BYTE byDriveLetter);
BOOL SYM_EXPORT WINAPI   DiskIsDoubleSpace16(BYTE drive);
BOOL SYM_EXPORT WINAPI   DiskIsBlockDevice16 (BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskIsSubst16(BYTE dl);
BOOL SYM_EXPORT WINAPI   DiskIsDriveSpace16(BYTE drive);
BOOL SYM_EXPORT WINAPI   DiskIsStacker16(BYTE byDriveLetter);

BYTE SYM_EXPORT WINAPI EISACMOSRead16(BYTE slot, BYTE funct, LPBYTE address);
BYTE SYM_EXPORT WINAPI EISACMOSWrite16(WORD length, LPBYTE address);
BYTE SYM_EXPORT WINAPI CMOSRead16(BYTE address);
VOID SYM_EXPORT WINAPI CMOSWrite16(BYTE address, BYTE CMOSdata);
BOOL SYM_EXPORT WINAPI HWIsPCI16(VOID);
BOOL SYM_EXPORT WINAPI HWIsEISA16(VOID);
BOOL SYM_EXPORT WINAPI HWIsMicroChannel16(VOID);
BOOL SYM_EXPORT WINAPI _DiskIsDoubleSpace16(BYTE drive, LPBYTE lpbyHostDriveLetter,
                                   LPWORD lpbSwapped, LPBYTE lpbySequence);
BOOL SYM_EXPORT WINAPI DiskRecToFloppyAbsDiskRec16 (LPCDISKREC diskPtr, LPABSDISKREC lpAbsDisk);
VOID SYM_EXPORT WINAPI DiskGetLockedTable16(LPBYTE lockedTablePtr);
BOOL SYM_EXPORT WINAPI DiskGetDASDType16(BYTE drive, LPBYTE DASDTypePtr);
BOOL SYM_EXPORT WINAPI DiskGetDriverTable16 (BYTE dl, LPDISKREC lpDiskPtr);
BOOL SYM_EXPORT WINAPI StackerGetUnitData16(BYTE drive, LPPERUNIT lpPerUnit);
BOOL SYM_EXPORT WINAPI StackerPutUnitData16(BYTE drive, LPPERUNIT lpPerUnit);
UINT SYM_EXPORT WINAPI   STK20DiskBlockDeviceThunk16(UINT command, LPDISKREC diskPtr, LPVOID buffer);
BOOL SYM_EXPORT WINAPI StackerFlushDrive16(BYTE drive);
STATUS SYM_EXPORT WINAPI NameReturnLongName16(LPCSTR lpszSpec, LPSTR lpszLongSpec);
WORD SYM_EXPORT WINAPI GetFreeSystemResources16(WORD fuSysResource);


#define NOT_SUPPORTED   0xFFFFFFFF      // Function not supported

//************************************************************************
// TP (Thunk Packages)
//************************************************************************
                                        // DiskAbsOperation()
typedef struct
    {
    BYTE                command;
    LPABSDISKREC        lpAbsDisk;
    } TP_DISKABSOPERATION, FAR *LPTP_DISKABSOPERATION;

                                        // DiskBlockDevice()
typedef struct
    {
    WORD                wCommand;
    LPDISKREC           lpDisk;
    } TP_DISKBLOCKDEVICE, FAR *LPTP_DISKBLOCKDEVICE;

                                        // DiskGetInfo()
typedef struct
    {
    BYTE                dl;
    LPDISKREC           lpDisk;
    } TP_DISKGETINFO, FAR *LPTP_DISKGETINFO;

                                        // DiskMapLogToPhyParams()
typedef struct
    {
    DWORD               dwSector;
    BYTE                dl;
    LPINT13REC          lpInt13Package;
    } TP_DISKMAPLOGTOPHYPARAMS, FAR *LPTP_DISKMAPLOGTOPHYPARAMS;

typedef struct
    {
    BYTE                slot;
    BYTE                funct;
    LPBYTE              address;
    } TP_EISACMOSREAD, FAR *LPTP_EISACMOSREAD;

typedef struct
    {
    WORD                length;
    LPBYTE              address;
    } TP_EISACMOSWRITE, FAR *LPTP_EISACMOSWRITE;

//************************************************************************
// FUNCTION DEFINES.
// Make them the same as the function name itself, with the word "THUNK"
// before it.
//************************************************************************
#define THUNK_CMOSREAD                  0
#define THUNK_CMOSWRITE                 1
#define THUNK_HWISEISA                  2
#define THUNK_HWISMICROCHANNEL          3
#define THUNK_DISKABSOPERATION          4
#define THUNK_DISKGETPHYSICALINFO       5
#define THUNK_DISKBLOCKDEVICE           6
#define THUNK_DISKGETINFO               7
#define THUNK_DISKMAPLOGTOPHYPARAMS     8
#define THUNK_DISKGETBOOTDRIVE          9
#define THUNK_DISKRESETDOS              11
#define THUNK_DISKRESET                 12
#define THUNK_EISACMOSREAD              13
#define THUNK_EISACMOSWRITE             14
#define THUNK_HWISPCI                   15

#endif  // if defined(SYM_WIN)

#endif


