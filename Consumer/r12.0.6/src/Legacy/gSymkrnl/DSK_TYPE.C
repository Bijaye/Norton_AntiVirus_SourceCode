/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/DSK_TYPE.C_v   1.18   21 Aug 1998 16:34:06   MBROWN  $ *
 *                                                                      *
 * Description:                                                         *
 *      Disk Type API                                                   *
 *                                                                      *
 * Contains:                                                            *
 *      DiskExists                                                      *
 *      DiskIsPhantom                                                   *
 *      _DiskIsPhantom                                                  *
 *      DiskIsBlockDevice                                               *
 *      DiskIsAssigned                                                  *
 *      _DiskIsAssigned                                                 *
 *      DiskIsSubst                                                     *
 *      DiskIsEncrypted                                                 *
 *      DiskIsNetwork                                                   *
 *      DiskIsFixed                                                     *
 *      DiskIsRemovable                                                 *
 *      DiskIsRAM                                                       *
 *      DiskIsVFATClient                                                *
 *      DiskGetType                                                     *
 *      DiskGetFloppyType                                               *
 *      DiskGetPhysicalType                                             *
 *      DiskGetSuperStorSwapDrive                                       *
 *      DiskGetSuperStorHostDrive                                       *
 *      DiskIsCDRom                                                     *
 *      DiskIsStackerLoaded                                             *
 *      DiskIsStacker                                                   *
 *      DiskIsSuperStor                                                 *
 *      DiskIsDoubleSpace                                               *
 *      DiskIsCompressed                                                *
 *      DiskIsLogical (DOS only)                                        *
 *      DiskIsTrueDrive (DOS only)                                      *
 *      DiskParseFilename (DOS only)                                    *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/DSK_TYPE.C_v  $ *
// 
//    Rev 1.18   21 Aug 1998 16:34:06   MBROWN
// Ported in a couple SKURTZ bug fixes from L branch
// 
//    Rev 1.17   26 Mar 1998 13:24:12   MBROWN
// Reverted to previous revision to back out of change for ext int13 SCSI drives
// 
//    Rev 1.15   17 Mar 1998 19:12:00   MBROWN
// Updated DiskGetPhysicalType and DiskGetHDParams for extended BIOS support.
// 
//    Rev 1.14   11 Mar 1998 15:05:38   SKURTZ
// Numerous changes for Extended Int13 support
//
//    Rev 1.13   02 Sep 1997 20:12:58   gvogel
// Fixed my mistake for DiskIsSubst.
//
//    Rev 1.12   26 Aug 1997 07:53:26   gvogel
// Enabled DiskIsSubst thunk support and added a call to it in DiskGetType for W32. NT ignores this still.
//
//    Rev 1.11   10 Aug 1997 15:22:42   bsobel
// Changed to dynamically load thunk dll on win32 only when accessed for the first time.
//
//    Rev 1.10   14 Apr 1997 13:26:08   BILL
// Removed SYM_ASSERT from DiskIsCompressed under Alpha, just returns FALSE (not compressed) now.
//
//    Rev 1.9   19 Mar 1997 21:39:28   BILL
// Modified files to build properly under Alpha platform
//
//    Rev 1.8   18 Feb 1997 16:30:50   MZAREMB
// Added change per Cam Cotrill to fix LS120 drive detection problem in
// compaq machines.
//
//    Rev 1.7   09 Oct 1996 13:08:46   MARKK
// Synchronized with QAKE
//
//    Rev 1.6   02 Oct 1996 19:15:18   SPASNIK
// Attempt to fix breakage caused by
// people not mkupdating os2 side after working
// this module over for nec ports
//
//    Rev 1.5   24 Sep 1996 16:29:28   RCHINTA
// Merged NEC related changes from Quake C ( implementation of
// IsValidNECHDNumber(), IsValidNECFloppyNumber() and
// Chk144Drive()).
//
//
//    Rev 1.4   10 Sep 1996 16:32:30   MZAREMB
// Return FALSE in CheckFloppy() when CMOS says there are no floppies so that
// we do not check nor depend on the hardware equipment call.
//
//    Rev 1.3   29 Aug 1996 16:45:10   DHERTEL
// Merged changed from Quake E:
// Removed isFixed label to prevent unreferenced label warning which is fatal when we are doing build
// s.  This label became unused when this code was changed on 08-07-96
// Use _DiskGetPhysicalInfo as opposed to DiskGetPhysicalInfo to prevent problems with SCSI drives in
//  DOS apps, should not affect windows apps
 ************************************************************************/

#include <dos.h>
//#include <stdlib.h>
#include "platform.h"
#include "xapi.h"
#ifdef SYM_DOS
#include "stddos.h"
#endif

#if defined(SYM_OS2)
#   include     <stdlib.h>
#   include     <stdio.h>
#   include     "stdos2.h"
#   define      LDUNKNOWN   0xFE        //  for tracking unchanged values
typedef struct {                    //  Needs to be 31 bytes, not 19bytes
   USHORT   BytesSect;          //  Bytes per sector
   BYTE     SectClust;          //  Sectors per Cluster
   USHORT   SectResrv;          //  Sectors in reserved area
   BYTE     FatCopies;          //  Number of copies of FAT
   USHORT   RootEntry;          //  Number Entries in Root Dir
   USHORT   SectTotal;          //  Total number of sectors       10
   BYTE     MediaDes;           //  Media Descriptor Byte
   USHORT   SectFat;            //  Sectors per FAT
   USHORT   SectTrack;          //  Sectors per track
   USHORT   Heads;              //  Numbers of heads (sides)
   USHORT   SectHidden;         //  Number of hidden sectors
   USHORT   HugeSect;            //                               21
   BYTE     Resv1;
   BYTE     Resv2;
   ULONG    Resv3;
   ULONG    Resv4;              //                                31
}    BPBS;    // BIOS Parameter Block

typedef struct
{
   BPBS     BPB;
   USHORT   Cyl;
   BYTE     Type;
   USHORT   DevAttr;
}  Drives;
#endif

#include "symcfg.h"
#include "file.h"
#include "disk.h"
#include <stackerw.h>
#ifdef SYM_WIN32
#include <winioctl.h>
#endif
#include "quakever.h"

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

MODULE_NAME;

/************************************************************************
 * Local Prototypes                                                     *
 ************************************************************************/

#if !defined(SYM_OS2)
BOOL LOCAL PASCAL CheckSuperStorID(LPBYTE lpbyDriver);
BOOL LOCAL PASCAL VerifySuperStorDrive(BYTE byDriveLetter);
BOOL LOCAL PASCAL IsToshiba (VOID);
#ifndef _M_ALPHA // WES No (known) NEC Alpha machines
int LOCAL Chk144Drive(char drive);
#endif
#endif

BOOL LOCAL PASCAL Is720KDrive (VOID);
UINT LOCAL PASCAL GetTheType (BYTE dl, UINT FAR * lpwMajorType, UINT FAR * lpwMinorType);
int  LOCAL PASCAL NEC144Type (int floppyIndex);
BOOL LOCAL PASCAL CheckFloppy (BYTE drive);
UINT LOCAL PASCAL GetFileSystemType ( BYTE dl, UINT FAR * lpwMinorType );
UINT LOCAL PASCAL GetFSName( LPSTR pszDrive, LPSTR pszFileSystem, WORD wcFileSystem );


#if !defined(SYM_VXD)

/*----------------------------------------------------------------------*/
/* DiskExists()                                                         */
/*      Tests a drive to see if it exists.                              */
/*                                                                      */
/*      Returns TRUE if drive exists, else FALSE.                       */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskExists (BYTE byDl)
{
#if defined(SYM_WIN32)
    char szDisk[10];
    UINT wDriveType;

    STRCPY(szDisk, "x:\\");
    szDisk[0] = (char)byDl;
    wDriveType = GetDriveType(szDisk);
    return (BOOL)(wDriveType != 0 && wDriveType != 1);

#elif defined(SYM_OS2)

    WORD    ParmPack;
    UINT    ParmSize;
    APIRET  rc = 0;                     /* Return code */
    ULONG   ulAction;
    CHAR    CurDrive[50];               //  Data space for passed drive
    ULONG   DataLengthInOut;            /* Data area length (returned) */

    ParmSize = sizeof(ParmPack);
    ParmPack = (byDl - 'A') << 8;    // first byte: 0, second: drive letter #
    DataLengthInOut = 0;

    rc = DosDevIOCtl(-1,             // Handle to device
                   8,                // Category: Logical Disk
                   0x63,             // Function: Query Parms
                   (PVOID)&ParmPack, // ParmList pointer,
                   sizeof(ParmPack), // ParmLengthMax,
                   &ParmSize,        // &ParmLengthInOut,
                   (PVOID)CurDrive,  // DataArea,
                   sizeof(CurDrive), // DataLengthMax,
                   &DataLengthInOut);

    if (rc == ERROR_INVALID_DRIVE)
        return FALSE;
    return TRUE;

#elif defined(SYM_WIN)

    auto        UINT    wDriveType;

    byDl = (BYTE) CharToUpper(byDl);
    wDriveType = GetDriveType(byDl - 'A');
    return ((BOOL) (wDriveType != 0 && wDriveType != 1));

#else

    auto        UINT    uMajor, uMinor;

    DiskGetType(byDl, &uMajor, &uMinor);
    return ((BOOL)(uMajor != TYPE_UNKNOWN));

#endif
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
BYTE SYM_EXPORT WINAPI DiskIsPhantom(BYTE dl)
{
#if defined(SYM_WIN32) || defined(SYM_OS2)

    return(FALSE);                      // These systems don't support phantom
                                        // drives.
#else
    auto        BYTE    phantom;


    if (HWIsNEC())
        return(FALSE);

    dl = (BYTE) CharToUpper(dl);

    if (DOSGetVersion() < DOS_3_20)
        {
        phantom = 255;

        if (DiskGetEquipmentDrives() <= 1)
            {
            MemCopyPhysical(&phantom, MAKELP(0, 0x0504), sizeof(phantom), 0);

            phantom = (BYTE) ((phantom != (BYTE) 1) ? 'B' : 'A');
            return((BYTE) ((phantom == dl) ? dl : FALSE));
            }
        }

    return (_DiskIsPhantom(dl));
#endif		// NEC
}


#if (defined(SYM_WIN16) || defined(SYM_DOS)) && !defined(NEC)

/*----------------------------------------------------------------------*/
/* _DiskIsPhantom()                                                     */
/*      Returns drive letter of phantom drive, else 0.                  */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BYTE SYM_EXPORT WINAPI _DiskIsPhantom (BYTE driveLetter)
{
    auto        BYTE    returnValue = 0;


    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_3_10)
        {
        _asm
        {
        mov     ax,440Eh                ; I/O Control for remote/local
        mov     bl, driveLetter         ; Get drive letter
        sub     bl,'A' - 1              ; Adjust drive letter to number
        DOS
        jc      notPhantom              ; Do we have an error?
        or      al,al                   ; Is drive assigned to another?
        jz      notPhantom              ; No! It's NOT a phantom drive!

        add     al,'A' - 1              ; Convert to drive letter
        cmp     al, driveLetter         ; Is this the same drive?
        jne     locRet                  ; No! It's a PHANTOM...

notPhantom:
        xor     ax, ax                  ; Return FALSE (local)

locRet:
        mov     returnValue, al         ; Get return value
        }
        }

    return(returnValue);
}
#pragma optimize("", on)
#endif


/*----------------------------------------------------------------------*
 *                     (C) Copyright Peter Norton, 1986-88              *
 *                                                                      *
 * BOOL DiskIsBlockDevice (BYTE dl)                                     *
 *      Tests a drive to see if it is a block device.                   *
 *                                                                      *
 *      Returns TRUE if block device, else FALSE.                       *
 *                                                                      *
 *                      IMPLEMENTATION HISTORY                          *
 *                                                                      *
 * 01/12/90     Brad: Created                                           *
 *----------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI DiskIsBlockDevice (BYTE dl)
{
#if defined(SYM_OS2)

    return(FALSE);                      // These systems don't support phantom
                                        // drives.
#else
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
    static FARPROC	lpfnDiskIsBlockDevice;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnDiskIsBlockDevice == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnDiskIsBlockDevice = GetProcAddress(hInstThunkDll,"DiskIsBlockDevice32" );
    	    if (lpfnDiskIsBlockDevice == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnDiskIsBlockDevice( dl ) );
#endif // _M_ALPHA
#else
#ifdef  SYM_WIN
    static BYTE         byNoBlock[27];
    static BOOL         bIniRead = FALSE;
    static CFGTEXTREC   rText = { NULL, 0, "Defaults", "NonBlockDrives" };

    dl = (BYTE) CharToUpper(dl);

                                        // Handle .ini file entry to force
                                        // treatment of all drives as non
                                        // int 25/26 drives.
    if(!bIniRead)
        {
        // Since this is in SYM_WIN, use the Windows API.
        if (GetPrivateProfileString( rText.lpSection, rText.lpEntry, "", byNoBlock,
            sizeof(byNoBlock), "symcfg.ini" ))

            AnsiUpperBuff(byNoBlock,sizeof(byNoBlock)-1);
        else
            byNoBlock[0] = '\0';

        bIniRead = TRUE;
        }

    if(STRCHR(byNoBlock, dl))
        return(FALSE);

                    /* Must be an existing drive    */
    if (!DiskExists(dl))
        return (FALSE);

                                        /* Must be local drive          */
    if (DiskIsNetwork(dl) || DiskIsAssigned(dl) || DiskIsSubst(dl))
        return (FALSE);

    return (TRUE);

#else
    dl = (BYTE) CharToUpper(dl);

    if (!DiskIsLogical (dl))          /* Must be an existing drive      */
        return (FALSE);
                                        /* Must be local drive          */
    if (DiskIsNetwork(dl) || DiskIsAssigned(dl) || DiskIsSubst(dl))
        return (FALSE);

    return (TRUE);
#endif	// SYM_WIN
#endif	// SYM_WIN32
#endif  //  SYM_OS2
}

/*----------------------------------------------------------------------*/
/* Determines if a DOS logical drive is assigned or not.                */
/*                                                                      */
/* Returns TRUE if assigned, else FALSE                                 */
/*                                                                      */
/* NOTE: Since PC-MOS does not support ASSIGNed drives, then there is   */
/* no reason to test them. (Also, it will not work correctly).          */
/*                                                                      */
/* 12/29/88 - Brad: created                                             */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskIsAssigned (BYTE dl)
{
#if defined(SYM_WIN32) || defined(SYM_OS2)

    return(FALSE);                      // None of these platforms supports
                                        // ASSIGNED drives
#else

    static      BYTE    CON_STRING[] = "x:CON";
    auto        BYTE    path[256];
    auto        BOOL    returnValue;


    dl = (BYTE) CharToUpper(dl);
                                        /*------------------------------*/
                                        /* Lock DS                      */
                                        /*------------------------------*/
    if (DOSGetPCMOSVersion())
        returnValue = FALSE;            /* PC-MOS doesn't support ASSIGNed */
    else
        {
        CON_STRING[0] = dl;
        returnValue = _DiskIsAssigned ((LPSTR) CON_STRING, (LPSTR) path);
        }
                                        /*------------------------------*/
                                        /* Unlock DS                    */
                                        /*------------------------------*/
    return(returnValue);
#endif
}


/*----------------------------------------------------------------------*/
/* _DiskIsAssigned()                                                    */
/*      Tests the drive 'driveLetter' to determine if it is an ASSIGNed */
/*      drive. Uses an undocumented function to perform this service.   */
/*                                                                      */
/*      Returns TRUE if drive is ASSIGNed, else FALSE.                  */
/*----------------------------------------------------------------------*/

#if (defined(SYM_WIN16) || defined(SYM_DOS)) && !defined(NEC)

BOOL SYM_EXPORT WINAPI _DiskIsAssigned (LPCSTR lpSource, LPSTR lpDest)
{
    auto        BOOL    returnValue = FALSE;

    if (DOSGetVersion() >= DOS_3_00)
        {
#ifdef  SYM_PROTMODE
        auto    DWORD           dwSourceDOS;    // DosAlloc values
        auto    DWORD           dwDestDOS;
        auto    LPSTR           lpSourcePtr;    // protected mode pointers
        auto    LPSTR           lpDestPtr;
        auto    union REGS      regs;
        auto    struct SREGS    sregs;


                                // alloc a real mode source buffer
        dwSourceDOS = GlobalDosAlloc ((DWORD) STRLEN (lpSource));
	if (dwSourceDOS == 0L)
	    return(FALSE);	// Assume not assigned if no memory available

                                // alloc a real mode destination buffer
        dwDestDOS = GlobalDosAlloc ((DWORD) SYM_MAX_PATH);
	if (dwDestDOS == 0L)
	    {
	    GlobalDosFree(LOWORD(dwSourceDOS));
	    return(FALSE);	// Assume not assigned if no memory available
	    }

                                // set up for access in protected mode
        FP_SEG (lpSourcePtr) = LOWORD (dwSourceDOS);
        FP_OFF (lpSourcePtr) = NULL;

                                // copy the source string
        STRCPY (lpSourcePtr, lpSource);

                                // set up for access in protected mode
        FP_SEG (lpDestPtr) = LOWORD (dwDestDOS);
        FP_OFF (lpDestPtr) = NULL;

                                // insert the basic pathname
        FileInitPath (lpDestPtr);
        *lpDestPtr = *lpSource;

                                // DS:SI points to the pathname
        sregs.ds  = HIWORD (dwSourceDOS);
        regs.x.si = NULL;

                                // ES:DI points to the pathname
        sregs.es  = HIWORD (dwDestDOS);
        regs.x.di = NULL;

        regs.h.ah = 0x60;               // undocumented DOS call
        IntWin (0x21, &regs, &sregs);   // convert path to canonical path

        STRCPY (lpDest, lpDestPtr);     // copy the result to our buffer

                                        // carry set on error, and
                                        // check if the drive letters
                                        // are different
        if (regs.x.cflag == TRUE || *lpDest == *lpSource)
            returnValue = FALSE;
        else
            returnValue = TRUE;
                                        // free up our DOS buffers
        GlobalDosFree (LOWORD (dwSourceDOS));
        GlobalDosFree (LOWORD (dwDestDOS));
#else
                                // insert the basic pathname
        FileInitPath (lpDest);
        *lpDest = *lpSource;

    _asm {

        push    ds
        push    si
        push    di

        lds     si, lpSource            // DS:SI points to source path
        les     di, lpDest              // ES:DI points to dest path

        mov     ah,60h                  // undocumented DOS call
        int     21h

        pop     di
        pop     si
        pop     ds

        jc      Bad_Call                // carry set on error
        }
                                        // check if the drive letters
                                        // are different
        if (*lpDest == *lpSource)
            {
Bad_Call:   returnValue = FALSE;
            }
        else
            returnValue = TRUE;
#endif
        }

    return (returnValue);
}

#endif


/*----------------------------------------------------------------------*/
/* DiskIsSubst()                                                        */
/*      Tests the drive 'driveLetter' to determine if it is a SUBSTed   */
/*      drive.                                                          */
/*                                                                      */
/*      Returns TRUE if drive is SUBSTed, else FALSE.                   */
/*----------------------------------------------------------------------*/
#if !defined(SYM_VXD)
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskIsSubst(BYTE driveLetter)
{
#if defined(SYM_OS2)

    return(FALSE);                      // These systems don't support subst
                                        // drives.
#elif defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_95)
    {
        static FARPROC	lpfnDiskIsSubst;
        extern HINSTANCE    hInstThunkDll;

        if (lpfnDiskIsSubst == NULL)
    	{
    		if (hInstThunkDll == NULL)
    		    {
    	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
    	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
    	        }
    	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
    		    {
    		    lpfnDiskIsSubst = GetProcAddress(hInstThunkDll,"DiskIsSubst32" );
        	    if (lpfnDiskIsSubst == NULL)
    				return FALSE;
    		    }
    		else
    		    return FALSE;
    	}
        return( lpfnDiskIsSubst(driveLetter) );
    }
    else
        return FALSE;                   // On NT, need to traverse Object Directory info.

#endif // _M_ALPHA
#else

    auto        BOOL    returnValue = FALSE;

    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_3_10)
        {
        _asm
        {
        mov     ax,4409h                ; I/O Control for remote/local
        mov     bl, driveLetter         ; Get drive letter
        sub     bl,'A' - 1              ; Adjust drive letter to number
        xor     dx, dx                  ; Set to nul state
        DOS
        test    dx, 8000h               ; Is it a SUBSTed drive?
        jnz     substDrive              ; Drive is substituted

        xor     ax, ax                  ; Return FALSE (local)
        jmp     short locRet            ; return to caller

substDrive:
        mov     ax, TRUE                ; Return TRUE (remote)

locRet:
        mov     returnValue, ax         ; Save the return value
        }
        }

    return(returnValue);
#endif
}
#pragma optimize("", on)
#endif

/*----------------------------------------------------------------------*/
/* DiskIsEncrypted                                                      */
/*      Tests the disk to see if it is a Norton Encrypted disk.         */
/*                                                                      */
/*      RETURNS:                                                        */
/*              0L              Not a PDISK                             */
/*              -1L             A CLOSED PDISK                          */
/*              n               Open PDISK size                         */
/*----------------------------------------------------------------------*/

/*----------------------------------------------------------------------*/
/* Constants needed just for this routine                               */
/*----------------------------------------------------------------------*/
#if !defined(SYM_OS2) && !defined(SYM_VXD)

#define CHECK_DRIVE     18              /* Check drive to see if NDISK  */

typedef struct
    {
    BYTE        command;
    BYTE        status;
    DWORD       dummy;
    BYTE        driveLetter;
    DWORD       returnValue;
    } IOCTL;

typedef IOCTL FAR * LPIOCTL;

#pragma optimize("", off)
DWORD SYM_EXPORT WINAPI DiskIsEncrypted(BYTE driveLetter)
{
#if defined(SYM_WIN32)

    return(0);

#else
    static  char        PRO_DEVICE60[] = "@DSKREET";
    static  BOOL        bDriverTested = FALSE;
    static  BOOL        bDiskreetPresent = FALSE;

    auto    DWORD       returnValue = 0L;
    auto    BOOL        bTempPresent;
    auto    HFILE       hFile;
    auto    UINT        wNU;
    auto    UINT        wDC;
    auto    UINT        wdc;

    wNU = ('N' << 8) + 'U';
    wDC = ('D' << 8) + 'C';
    wdc = ('d' << 8) + 'c';

                                        // Avoid multiple changes to real
                                        // mode by saving result of first
                                        // test in a static and not testing
                                        // for the driver again.
    if(!bDriverTested)
        {
        bTempPresent = FALSE;

        _asm
            {
            push ds                     // Make sure it is a Norton
            push bp                     // Device Driver
            push ss

            mov ah,0xFE
            mov al,0
            mov di,wNU
            mov si,wDC

            stc
            int 2Fh

            pop ss
            pop bp
            pop ds

            jc  notFound

            cmp si,wdc
            jne notFound
            mov [bTempPresent], TRUE
            }
notFound:
        bDriverTested = TRUE;
        bDiskreetPresent = bTempPresent;
        }

    if(bDiskreetPresent)
        {
#ifdef  SYM_PROTMODE
        auto    LPIOCTL     lpDiskreetSel;
        auto    union       REGS regs;
        auto    struct      SREGS segregs;
        auto    DWORD       dwSegSel;

#ifdef  SYM_WIN
        auto    UINT        wErrMode;
        wErrMode = SetErrorMode(TRUE);
#endif
#else
        auto        IOCTL   rDiskreet;
#endif

        driveLetter = (BYTE) CharToUpper(driveLetter);

                                        /*------------------------------*/
                    /* Open the PROCRYPT device     */
                    /* driver.                      */
                                        /*------------------------------*/
#ifdef  SYM_PROTMODE
#ifdef  SYM_WIN
        hFile = _lopen (PRO_DEVICE60, OF_READWRITE + OF_SHARE_COMPAT);

        SetErrorMode(wErrMode);
#else
        hFile = FileOpen (PRO_DEVICE60, OF_READWRITE + OF_SHARE_COMPAT);
#endif

        if (hFile != (HFILE)-1)
            {
            dwSegSel = GlobalDosAlloc(sizeof(IOCTL));
            lpDiskreetSel = MAKELP(LOWORD(dwSegSel),0);

            lpDiskreetSel->command     = CHECK_DRIVE;
            lpDiskreetSel->status      = 0xFF;
            lpDiskreetSel->dummy       = 0;
            lpDiskreetSel->driveLetter = driveLetter;
            lpDiskreetSel->returnValue = 0;

            regs.x.bx = (UINT)hFile;
            regs.x.ax = 0x4403;
            regs.x.cx = 7;
            segregs.ds = HIWORD(dwSegSel);
            regs.x.dx = 0;

            IntWin((BYTE)0x21,&regs,&segregs);

            returnValue = lpDiskreetSel->returnValue;

            GlobalDosFree(LOWORD(dwSegSel));

#else
        hFile = FileOpen (PRO_DEVICE60, OF_READWRITE + OF_SHARE_COMPAT);

        if (hFile != (HFILE)-1)
            {

            rDiskreet.command     = CHECK_DRIVE;
            rDiskreet.status      = 0xFF;
            rDiskreet.dummy       = 0;
            rDiskreet.driveLetter = driveLetter;
            rDiskreet.returnValue = 0;

            _asm {
                mov     bx,hFile
                mov     ax,4403h
                mov     cx,7
                lea     dx,word ptr rDiskreet

                int     21h
                }

            returnValue = rDiskreet.returnValue;

#endif
            FileClose(hFile);
            }
        }

    return(returnValue);
#endif
}
#pragma optimize("", on)

#endif

#endif // SYM_VXD

/*----------------------------------------------------------------------*/
/* DiskIsNetwork()                                                      */
/*      Tests the drive 'driveLetter' to determine if it is a network   */
/*      drive.                                                          */
/*                                                                      */
/*      Returns TRUE if drive is network, else FALSE.                   */
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskIsNetwork(BYTE driveLetter)
{
#if defined(SYM_WIN32)

    char szDisk[10];
    UINT wDriveType;

    STRCPY(szDisk, "x:\\");
    szDisk[0] = (char)driveLetter;
    wDriveType = GetDriveType(szDisk);
    return (BOOL)(wDriveType == DRIVE_REMOTE);

#elif defined(SYM_WIN)

    driveLetter = (BYTE) CharToUpper(driveLetter);
    driveLetter -= (BYTE) 'A';

    return (GetDriveType(driveLetter) == DRIVE_REMOTE);

#elif defined(SYM_OS2)

    //  We don't have a network-independent approach to this yet, so this
    //  is just a kludge - process of elimination.

    USHORT  ParmPack;
    UINT    ParmSize;
    Drives  CurDrive;
    APIRET  rc = 0;                     /* Return code */
    ULONG   ulAction;
    ULONG   DataLengthInOut;            /* Data area length (returned) */


    CurDrive.Cyl = 0;               // Set to known value each time because
    CurDrive.Type = LDUNKNOWN;      // network drives don't change these at
    ParmSize = 2;
    ParmPack = driveLetter - 'A' << 8;   // Load in the drive into 2nd byte
    DataLengthInOut = 0;

    rc = DosDevIOCtl(-1, 8, 0x63,   //  Logical disk info IOCtl
            (PVOID)&ParmPack, sizeof(ParmPack), &ParmSize,
            (PVOID)&CurDrive, sizeof(CurDrive), &DataLengthInOut);

    if (rc == ERROR_INVALID_DRIVE)
            return FALSE;
    if ((CurDrive.DevAttr & 0x01) ^ 0x01)
            return FALSE;

    if (CurDrive.Type == LDUNKNOWN )
            return TRUE;

    return FALSE;

#elif defined(SYM_VXD)

    extern BOOL SYM_EXPORT WINAPI __DiskIsNetwork(BYTE driveLetter);

    return(__DiskIsNetwork((BYTE) CharToUpper(driveLetter)));

#else

    auto        BOOL    returnValue = FALSE;

    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_3_10)
        {
        _asm
        {
        ; check for MSCDEX CD-ROM drives before checking for a remote
        ; drive, since MSCDEX uses the network redirector also
        mov     ax,150Bh                ; MSCDEX CD-ROM drive check
        xor     bx,bx                   ; clear BX
        mov     cl,driveLetter          ; get drive letter
        xor     ch,ch
        sub     cl,'A'                  ; adjust drive letter to number
        int     2fh                     ; multiplex interrupt
        cmp     ax,0000h
        je      testStatus
        cmp     bx,0ADADh
        jne     testStatus

        jmp     short notRemote         ; it's a CD-ROM, and therefore not
                                        ; a network drive

testStatus:
        mov     ax,4409h                ; I/O Control for remote/local
        mov     bl, driveLetter         ; Get drive letter
        sub     bl,'A' - 1              ; Adjust drive letter to number
        xor     dx,dx                   ; Set to nul state
        DOS
        test    dx, 1000h               ; Is it a networked drive?
        jnz     remoteDrive             ; Drive is remote

notRemote:
        xor     ax, ax                  ; Return FALSE (local)
        jmp     short locRet            ; return to caller

remoteDrive:
        mov     ax, TRUE                ; Return TRUE (remote)

locRet:
        mov     returnValue, ax         ; Save the return value
        }
        }

    return(returnValue);
#endif
}
#pragma optimize("", on)


/*----------------------------------------------------------------------*/
/* DiskIsRemovable()                                                    */
/*      Tests the drive 'driveLetter' to determine if it is REMOVABLE.  */
/*                                                                      */
/*      Returns TRUE if drive is REMOVABLE, else FALSE.                 */
/*----------------------------------------------------------------------*/
#if !defined(SYM_OS2)
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskIsRemovable(BYTE driveLetter)
{
#if defined(SYM_WIN32)

    char szDisk[10];
    UINT wDriveType;

    STRCPY(szDisk, "x:\\");
    szDisk[0] = (char)driveLetter;
    wDriveType = GetDriveType(szDisk);
    return (BOOL)(wDriveType == DRIVE_REMOVABLE);

#elif defined(SYM_VXD)

    extern BOOL SYM_EXPORT WINAPI __DiskIsRemovable(BYTE driveLetter);

    return(__DiskIsRemovable((BYTE) CharToUpper(driveLetter)));

#else

    auto        BOOL    returnValue = FALSE;


    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_3_00)
        {
        _asm
        {
        mov     bl, driveLetter         ; get drive letter
        sub     bl, 'A' - 1             ; Set drive letter to drive number
        mov     ax, 4408h               ; Is device removable function
        DOS
        jc      locRet

        or      ax, ax                  ; Is it removable?
        jnz     locRet                  ; No.

        mov     returnValue, TRUE       ; Yes.
locRet:
        }
        }
    return(returnValue);
#endif
}
#pragma optimize("", on)
#endif


#if !defined(SYM_VXD)

/*----------------------------------------------------------------------*/
/* DiskIsFixed()                                                        */
/*      Tests the drive 'driveLetter' to determine if it is FIXED.      */
/*                                                                      */
/*      Returns TRUE if drive is FIXED, else FALSE.                     */
/*----------------------------------------------------------------------*/
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskIsFixed(BYTE driveLetter)
{
#if defined(SYM_WIN32)

    char szDisk[10];
    UINT wDriveType;

    STRCPY(szDisk, "x:\\");
    szDisk[0] = (char)driveLetter;
    wDriveType = GetDriveType(szDisk);
    return (BOOL)(wDriveType == DRIVE_FIXED);

#elif defined(SYM_WIN)

	driveLetter = (BYTE) CharToUpper(driveLetter);
	driveLetter -= (BYTE) 'A';

	return (GetDriveType(driveLetter) == DRIVE_FIXED);

#elif defined(SYM_OS2)

    WORD    ParmPack;
    UINT    ParmSize;
    APIRET  rc = 0;                     /* Return code */
    ULONG   ulAction;
    Drives  CurDrive;
    ULONG   DataLengthInOut;            /* Data area length (returned) */

    ParmSize = sizeof(ParmPack);
    ParmPack = (driveLetter - 'A') << 8; // first byte: 0, second: drive letter #
    DataLengthInOut = 0;

    rc = DosDevIOCtl(-1, 8, 0x63,(PVOID)&ParmPack, sizeof(ParmPack),
            &ParmSize, (PVOID)&CurDrive, sizeof(CurDrive), &DataLengthInOut);

    if (rc == ERROR_INVALID_DRIVE)
        return FALSE;
    return (CurDrive.DevAttr & 0x01);

#else   //  not OS2

    auto        BOOL    returnValue = FALSE;


    driveLetter = (BYTE) CharToUpper(driveLetter);

    if (DOSGetVersion() >= DOS_3_00)
        {
        _asm
        {
        mov     bl, driveLetter         ; get drive letter
        sub     bl, 'A' - 1             ; Set drive letter to drive number
        mov     ax, 4408h               ; Is device removable function
        DOS
        jc      notFixed

        cmp     ax, 1                   ; Is it fixed?
        jne     notFixed                ; No, return FALSE

//isFixed:
        mov     ax, TRUE                ; Return TRUE (Fixed)
        jmp     short locRet            ; Return to user

        ;--------------------------------------------------
        ; Either not fixed, pre-DOS 3.0, or NET drive
        ;--------------------------------------------------
notFixed:
        xor     ax, ax                  ; Return FALSE (Not fixed)

locRet:
        mov     returnValue, ax         ; Get return value
        }
        }
    return(returnValue);
#endif
}
#pragma optimize("", on)



/*----------------------------------------------------------------------*
 *      Tests if a letter corresponds to a logical drive.               *
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskIsLogical (BYTE dl)
{
#if defined(SYM_OS2)
    return FALSE;                       //  Not ported to OS/2 yet.
#else
    BYTE                currentDrive;
    BYTE                oldDL;
    BYTE                validDisk;
                                        /*------------------------------*/
                                        /* Before working with any      */
                                        /* drives, allow access to them */
                                        /* DOS 4.x+ only.               */
                                        /*------------------------------*/
#if !defined(SYM_WIN32)
    if (DiskGetAccessFlag(dl) == DISALLOW_ACCESS)
        DiskSetAccessFlag(dl, ALLOW_ACCESS);
#endif

    validDisk = TRUE;

    oldDL = DiskGet();
    DiskSet (dl);
    currentDrive = DiskGet();
    if (currentDrive != dl)
        {
        validDisk = FALSE;
        }
    else
        {
#if defined(SYM_WIN16) || defined(SYM_DOS)
        {
        static BYTE         commandLineToParse [5] = "x:\r\n";
        BYTE            FCB [60];       /* Plenty of space, just in case */

        commandLineToParse [0] = dl;    /* Get drive to test            */

                                        /* If command parser doesn't    */
                                        /* recognize drive, then exit   */
        if (DiskParseFilename ((LPSTR) commandLineToParse, (LPSTR) FCB) == 255)
            validDisk = FALSE;
        }
#endif
        }

    DiskSet(oldDL);
    return (validDisk);
#endif                                  //  #if !defined(SYM_OS2)
}


#if defined(SYM_WIN16) || defined(SYM_DOS)

#pragma optimize("", off)
UINT SYM_EXPORT WINAPI DiskParseFilename (LPSTR commandLine, LPSTR FCBPtr)
{
    UINT        wReturn;

    _asm
        {
        push    ds
    	push	si
	    push	di
        lds     si,commandLine          ; get ptr. to command line string
        les     di,FCBPtr               ; get ptr. to unopened FCB
        mov     ax,2900h                ; parse filename function
        INT     21H                     ; DOS interrupt
        xor     ah, ah                  ; clear the high byte
        mov     wReturn, ax
    	pop	di
	    pop	si
        pop     ds
        }

    return (wReturn);
}
#pragma optimize("", on)
#endif


/*-----------------------------------------------------------------------
 *
 * Description:
 *      Try to determine if the drive of interest is in fact a RAM drive.
 *
 *      The only test done here is how many FAT's are on the disk.  One
 *      FAT is taken to indicate a RAM drive.  This is probably pretty
 *      accurate, since most other device-driven drives will still need
 *      the security of multiple FAT copies.
 *
 * Parameters:
 *      BYTE byDriveLetter      The drive to test.
 *
 * Return Value:
 *      TRUE if this is a RAM drive (1 FAT)
 *      FALSE otherwise
 *
 *-----------------------------------------------------------------------
 * 02/12/1992 PETERD Function Created.
 *----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskIsRAM(BYTE byDriveLetter)
{
#ifdef SYM_WIN32

    char szDisk[10];
    UINT wDriveType;

    STRCPY(szDisk, "x:\\");
    szDisk[0] = (char)byDriveLetter;
    wDriveType = GetDriveType(szDisk);
    return (BOOL)(wDriveType == DRIVE_RAMDISK);

#elif defined(SYM_OS2)

    return(FALSE);

#else

    auto        DISKREC		rDiskInfo;

	MEMSET(&rDiskInfo,0,sizeof(DISKREC));

    if (!DiskIsRemovable(byDriveLetter))
        {
        byDriveLetter = (BYTE) CharToUpper(byDriveLetter);

        if (DiskGetInfo(byDriveLetter, &rDiskInfo))
            {
            if (rDiskInfo.numberFats == 1)
                return(TRUE);
            }
        }

    return(FALSE);
#endif
}


#ifdef SYM_WIN16
/*-----------------------------------------------------------------------
 *
 * DiskIsVFATClient(BYTE byDriveLetter)
 *
 * Description:
 *      Determine if this logical volume is managed by VFAT.  We try to
 *      communicate with VFAT.386's PM entry point.  If we succeed at
 *      getting a PM entry point, we call function 3 which returns an
 *      array of drives that VFAT is managing.  We will maintain this
 *      list so that we only have to make the call once.
 *
 * Parameters:
 *      BYTE byDriveLetter      The drive to test.
 *
 * Return Value:
 *      TRUE if this is a VFAT client drive
 *      FALSE otherwise
 *
 *-----------------------------------------------------------------------
 *----------------------------------------------------------------------*/
typedef struct VFATTableStruct
    {
    DWORD	dwNumEntries;
    BYTE	byDrives[26];
    } VFATTableStruct;


BOOL SYM_EXPORT WINAPI DiskIsVFATClient (BYTE byDriveLetter)
{
    auto    DWORD           ii;
    static  VFATTableStruct rVFATTable;
    static  BOOL            bTested     = FALSE;

    if (!bTested)
	{
        auto    DWORD   lpCallBack;
        auto    WORD    i = sizeof(rVFATTable);

        _asm
        {
            push    ds
            push    si
            push    di
            xor     di, di
            mov     es, di
            mov     ax, 0x1684  // get PM API
            mov     bx, 0x0486  // VFAT VxD ID
            int     0x2F

            mov     word ptr [lpCallBack][2], es
            mov     word ptr [lpCallBack][0], di

            mov     ax, es
            or      ax, di      ;Q: Is VFAT installed on the system?
            jz      done        ; N: exit

            mov     ax, 3           ; get array of VFAT drives
            push    ds
            pop     es
            lea     bx, [rVFATTable]; array address
            mov     cx, [i]         ; array size
            call    [lpCallBack]    ; call VFAT PM API

          done:
            pop     di
            pop     si
            pop     ds
        }
        bTested = TRUE; // indicate we have already tried getting info from VFAT
    }

    if (rVFATTable.dwNumEntries)
    {
        byDriveLetter = (BYTE) (CharToUpper(byDriveLetter) - 'A');

        for (ii = 0; ii < rVFATTable.dwNumEntries ; ii++)
            if (byDriveLetter == rVFATTable.byDrives[ii])
                return (TRUE);
    }
    return (FALSE);
}

#else

BOOL SYM_EXPORT WINAPI DiskIsVFATClient (BYTE byDriveLetter)
{
    // Although this is not accurate.  Products on other platforms need other
    // ways to check for VFAT.  This stub was added so that at run-time
    // products don't have different code paths on a per platform basis.
    return (FALSE);
}

#endif




/*----------------------------------------------------------------------*
 * DiskIsSuperStor()                                                    *
 *                                                                      *
 * Test if a drive letter refers to a SuperStor compressed disk.        *
 *                                                                      *
 * Returns:     TRUE    It's a SuperStor disk.                          *
 *              FALSE   It isn't.                                       *
 *----------------------------------------------------------------------*/
#if defined(SYM_WIN16) || defined(SYM_DOS)

typedef struct
    {
    UINT        wSig;
    BYTE        byProduct;
    BYTE        byCommand;
    LPBYTE      lpUnitInfo;
    LPBYTE      lpDriverInfo;
    } SStorParamRec;

typedef struct
    {
    UINT        wSig;
    BYTE        byProduct;
    BYTE        byCommand;
    BYTE        bySwappedDriveNum;
    } SStorSwapRec;

typedef struct
    {
    DWORD       dwNext;
    BYTE        byFiller[14];
    LPBYTE      lpRealDriver;
    } QemmStub1;

typedef struct
    {
    DWORD       dwNext;
    BYTE        byFiller[8];
    LPBYTE      lpRealDriver;
    } QemmStub2;

typedef struct
    {
    DWORD       dwNext;
    BYTE        byFiller[64];
    LPBYTE      lpRealDriver;
    } Q386MaxStub;


BOOL SYM_EXPORT WINAPI DiskIsSuperStor(BYTE byDriveLetter)
{
    auto        UINT            wDriverPtrOffset;
    auto        LPBYTE          lpDPB;
    auto        LPBYTE          lpbyDriver;
#ifdef SYM_PROTMODE
    auto        UINT            wSelector;
#endif

    byDriveLetter = (BYTE) CharToUpper(byDriveLetter);

    /* Try to get driver address from SmartDrive */
    lpbyDriver = (LPBYTE) DiskGetUnderSmartDrive(byDriveLetter);

#ifdef SYM_WIN
    /* Try to get driver address from SymEvent */
    if (lpbyDriver == NULL)
	lpbyDriver = (LPBYTE) DiskGetUnderSymEvent(byDriveLetter);
#endif

    /* Walk DPB chain if SmartDrive or SymEvent didn't provide the address */
    if (lpbyDriver == NULL)
	{
	/* Get a pointer to the DPB for this drive */
	if (!DiskFindDPB(byDriveLetter, (LPDISKREC far *) &lpDPB))
	    return(FALSE);

#ifdef SYM_PROTMODE
	/* Convert real mode DPB segment to protected mode selector */
	if (SelectorAlloc(FP_SEG(lpDPB), 0xFFFF, &wSelector))
	    return(FALSE);
	FP_SEG(lpDPB) = wSelector;
#endif

	/* Get a pointer to the device driver from the DPB */
	wDriverPtrOffset = DOSGetVersion() >= 0x400 ? 0x0013 : 0x0012;
	lpbyDriver = *((BYTE FAR * FAR *) & ((LPBYTE)lpDPB)[wDriverPtrOffset]);

    if (!lpbyDriver)
        return (FALSE);

#ifdef SYM_PROTMODE
	/* Free the DPB selector */
	SelectorFree(FP_SEG(lpDPB));
#endif
	}

    /* Check if ID can be found where DPB points */
    if (CheckSuperStorID(lpbyDriver))
        return(VerifySuperStorDrive(byDriveLetter));

#ifdef SYM_PROTMODE
    /* Convert real mode segment to protected mode selector */
    if (SelectorAlloc(FP_SEG(lpbyDriver), 0xFFFF, &wSelector))
        return(FALSE);
    FP_SEG(lpbyDriver) = wSelector;
#endif

    /* Try to find ID via various LOADHI stubs */
    if (CheckSuperStorID(((QemmStub1 far *)lpbyDriver)->lpRealDriver) ||
        CheckSuperStorID(((QemmStub2 far *)lpbyDriver)->lpRealDriver) ||
        CheckSuperStorID(((Q386MaxStub far *)lpbyDriver)->lpRealDriver))
        {
#ifdef SYM_PROTMODE
        SelectorFree(wSelector);
#endif
        return(VerifySuperStorDrive(byDriveLetter));
        }

#ifdef SYM_PROTMODE
    SelectorFree(wSelector);
#endif

    /* Must not be a SuperStor drive */
    return(FALSE);
}


BOOL LOCAL PASCAL CheckSuperStorID(LPBYTE lpbyDriver)
{
    static      BYTE            szAddStorID[] = "ADDSTOR ";
    auto        BOOL            bResult;
#ifdef SYM_PROTMODE
    auto        UINT            wSelector;
#endif

#ifdef SYM_PROTMODE
    if (SelectorAlloc(FP_SEG(lpbyDriver), 0xFFFF, &wSelector))
        return(FALSE);
    FP_SEG(lpbyDriver) = wSelector;
#endif

    FP_OFF(lpbyDriver) = 0x0020;  // ID is at offset 0x0020 within driver
    bResult = (BOOL) (_fmemcmp(lpbyDriver, szAddStorID, 8) == 0);

#ifdef SYM_PROTMODE
    SelectorFree(wSelector);
#endif

    return(bResult);
}

/*----------------------------------------------------------------------*
 * VerifySuperStorDrive()						*
 *									*
 * Use the IOCTLGETCDPARMS function to verify that a drive we think	*
 * belongs to SuperStor actually does.  This is the "official" way to	*
 * test for a SuperStor drive according to AddStor.  We don't use this	*
 * as our primary test because doing IOCTL writes to an non-SuperStor	*
 * drive will have unpredictable results.  In the worst case, some	*
 * other type of driver could take our IOCTL as a command to format	*
 * the media!								*
 *									*
 * We only use this function after we've determined that SuperStor	*
 * owns the DPB for the drive.  This extra test is being added to deal	*
 * with the case of automounting removable drives.  SuperStor takes	*
 * over their DPB's even when an uncompressed disk is in the drive.	*
 * The only way to tell if a compressed or uncompressed disk is in the	*
 * drive is to use this function.					*
 *----------------------------------------------------------------------*/

BOOL LOCAL PASCAL VerifySuperStorDrive(BYTE byDriveLetter)
{
    auto	SStorParamRec	paramRec;
    auto	DeviceParamRec	deviceParams;

    // Force SuperStor to evaluate media in automounting drives
    deviceParams.specialFunctions = IOCTL_SF_GET_DEFAULT;
    DiskGenericIOCTL(GET_DEVICE_PARAMS, byDriveLetter, &deviceParams);

    // Ask SuperStor for compressed disk information
    paramRec.wSig      = 0xAA55;
    paramRec.byProduct = 1;
    paramRec.byCommand = 0x06;
    if (DiskIOCTL(IOCTL_BLOCK_WRITE, byDriveLetter, &paramRec, sizeof(paramRec)) != sizeof(paramRec))
        return(FALSE);

    if (paramRec.wSig == 0xAA55 || paramRec.byProduct != 1)
        return(FALSE);
    else
        return(TRUE);
}

/*----------------------------------------------------------------------*
 * DiskGetSuperStorSwapDrive()                                          *
 *                                                                      *
 * Returns:                                                             *
 *                                                                      *
 * <drive>      Original drive letter of swapped drive                  *
 * 0            Drive is invalid or not swapped                         *
 *----------------------------------------------------------------------*/

BYTE SYM_EXPORT WINAPI DiskGetSuperStorSwapDrive(BYTE bySuperStorDrive)
{
    auto        SStorSwapRec    swapRec;
    auto        BYTE            bySwapDrive;

    if (!DiskIsSuperStor(bySuperStorDrive))
        return(0);

    swapRec.wSig      = 0xAA55;
    swapRec.byProduct = 1;
    swapRec.byCommand = 0x08;
    if (DiskIOCTL(IOCTL_BLOCK_WRITE, bySuperStorDrive, &swapRec, sizeof(swapRec)) != sizeof(swapRec))
        return(0);
    if (swapRec.wSig == 0xAA55 || swapRec.byProduct != 1)
        return(0);

    bySwapDrive = (BYTE)(swapRec.bySwappedDriveNum + 'A');

    if (bySwapDrive == bySuperStorDrive)
        return(0);
    else
        return(bySwapDrive);
}

/*----------------------------------------------------------------------*
 * DiskGetSuperStorHostDrive()                                          *
 *                                                                      *
 * Returns:                                                             *
 *                                                                      *
 * <drive>      Host drive letter of compressed drive                   *
 * 0            Compressed drive is invalid or not SuperStor            *
 *----------------------------------------------------------------------*/

BYTE SYM_EXPORT WINAPI DiskGetSuperStorHostDrive(BYTE bySuperStorDrive)
{
    auto        SStorParamRec   paramRec;
#ifdef SYM_PROTMODE
    auto        LPBYTE          lpUnitInfo;
    auto        LPBYTE          lpDPB;
    auto        UINT            wDriverSelector;
    auto        UINT            wDPBSelector;
    auto        BYTE            byHostDrive;
#endif

    if (!DiskIsSuperStor(bySuperStorDrive))
        return(0);

    paramRec.wSig      = 0xAA55;
    paramRec.byProduct = 1;
    paramRec.byCommand = 0x06;
    if (DiskIOCTL(IOCTL_BLOCK_WRITE, bySuperStorDrive, &paramRec, sizeof(paramRec)) != sizeof(paramRec))
        return(0);
    if (paramRec.wSig == 0xAA55 || paramRec.byProduct != 1)
        return(0);

    // A pointer to the host drive DPB can be found at offset 95 in the
    // "per-unit" data structure.  AddStor says this pointer to unlikely
    // to move in later versions of the driver.  The first byte of a DPB
    // contains the drive number (A=0), so once we have the host DPB we
    // can easily get the host drive letter.

#ifndef SYM_PROTMODE

    // As always, the DOS version is easy...
    return((**((BYTE far * far *)(paramRec.lpUnitInfo + 95))) + 'A');

#else

    // ...and the Windows version is a pain in the ass...

    lpUnitInfo = paramRec.lpUnitInfo;
    if (!SelectorAlloc(FP_SEG(lpUnitInfo), 0xFFFF, &wDriverSelector))
        return(0);
    FP_SEG(lpUnitInfo) = wDriverSelector;

    lpDPB = *((BYTE far * far *)(lpUnitInfo + 95));
    if (!SelectorAlloc(FP_SEG(lpDPB), 0xFFFF, &wDPBSelector))
        {
        SelectorFree(wDriverSelector);
        return(0);
        }
    FP_SEG(lpDPB) = wDPBSelector;

    byHostDrive = lpDPB[0] + 'A';

    SelectorFree(wDriverSelector);
    SelectorFree(wDPBSelector);

    return(byHostDrive);

#endif
}

#endif                                  // !(SYM_WIN32 || SYM_OS2)


#ifndef SYM_OS2

/*----------------------------------------------------------------------*
 * DiskGetType()                                                        *
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI DiskGetType(BYTE dl, LPUINT lpwMajorType, LPUINT lpwMinorType)
{
#ifdef SYM_WIN32
    auto    char    szDisk[10];
    auto    UINT    wType;
    auto    UINT    uWinType;
                                        // -----------------------------------
                                        // Do not perform compressed drive
                                        // checks if running under Win NT
                                        // The OS will complain about direct
                                        // disk access.
                                        // -----------------------------------
    uWinType = SystemGetWindowsType();

    *lpwMajorType = TYPE_UNKNOWN;
    *lpwMinorType = TYPE_UNKNOWN;

    STRCPY(szDisk, "x:\\");
    szDisk[0] = (char)dl;

                                        // Check for subst'ed drives before falling-
                                        // through to Win32 API.
    if (DiskIsSubst(dl))
	{
        *lpwMajorType = TYPE_SUBST;
		return NOERR;
	}

    wType = GetDriveType(szDisk);

    switch (wType)
        {
        case 0:                         // Drive type cannot be determined.
            break;

        case 1:
            *lpwMajorType = TYPE_DOESNT_EXIST;
            break;

        case DRIVE_REMOTE:
            *lpwMajorType = TYPE_REMOTE;
            break;

        case DRIVE_REMOVABLE:
            *lpwMajorType = TYPE_REMOVEABLE;

            if ( uWinType != SYM_SYSTEM_WIN_NT )
                {
                if (DiskIsDoubleSpace(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_DBLSPACE;
                    }
                else if (DiskIsStacker(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_STACKER;
                    }
                else
                    {
                    DiskGetFloppyType(dl, lpwMinorType);
                    }
                }
            else
                {
                DiskGetFloppyType(dl, lpwMinorType);
                }

            break;

        case DRIVE_CDROM:
            *lpwMajorType = TYPE_CDROM;
            break;

        case DRIVE_RAMDISK:
            *lpwMajorType = TYPE_RAMDISK;
            break;

        case DRIVE_FIXED:

            if ( uWinType != SYM_SYSTEM_WIN_NT )
                {
                if (DiskIsDoubleSpace(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_DBLSPACE;
                    }
                else if (DiskIsStacker(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_STACKER;
                    }
                else
                    *lpwMajorType = TYPE_FIXED;
                }
            else
                {
                *lpwMajorType = TYPE_FIXED;
                }

            break;

        default:
            *lpwMajorType = TYPE_FIXED;
            break;

        }

    return NOERR;
#else
    /*------------------------------------------------------------------*/
    /*  10/31/91: Better convert this first.  Happy Halloween...        */
    /*------------------------------------------------------------------*/

    dl = (BYTE) CharToUpper(dl);

    *lpwMajorType = TYPE_UNKNOWN;
    *lpwMinorType = TYPE_UNKNOWN;

    return (GetTheType (dl, lpwMajorType, lpwMinorType));
#endif
}

#ifndef SYM_WIN32

#ifdef  SYM_WIN                           // Windows Version

UINT LOCAL PASCAL GetTheType (BYTE dl, UINT FAR * lpwMajorType, UINT FAR * lpwMinorType)
{
    auto    LONG    lEncrypted;
    auto    UINT    wType;
    auto    UINT    uWinType;


                                        // -----------------------------------
                                        // Do not perform compressed drive
                                        // checks if running under Win NT
                                        // The OS will complain about direct
                                        // disk access.
                                        // -----------------------------------

    uWinType = SystemGetWindowsType();

    /*------------------------------------------------------------------*/
    /*  Check phantom FIRST, as DiskExists() will return FALSE          */
    /*  for a phanton drive                                             */
    /*------------------------------------------------------------------*/
    if (!HWIsNEC() && dl < 'C' && DiskIsPhantom(dl))
        *lpwMajorType = TYPE_PHANTOM;
    else
        {
        /*------------------------------*/
        /*  Get type through Windows    */
        /*------------------------------*/
        wType = GetDriveType(dl - 'A');

        switch (wType)
            {
            case 0:
                *lpwMajorType = TYPE_UNKNOWN;
                break;

            case 1:
                *lpwMajorType = TYPE_DOESNT_EXIST;
                break;

            case DRIVE_REMOTE:
                                        // MSCDEX is a network redirector
                                        // as such, cd rom drives are reported
                                        // as network drives.  We check
                                        // here to see if it really is a
                                        // network drive or a cd rom
                if(DiskIsCDRom(dl))
                    *lpwMajorType = TYPE_CDROM;
                else
                    *lpwMajorType = TYPE_REMOTE;
                break;

            case DRIVE_REMOVABLE:
                *lpwMajorType = TYPE_REMOVEABLE;

                if (DiskIsInterLink(dl))
                    {
                    *lpwMajorType = TYPE_REMOTE;
                    }
                else if ( uWinType != SYM_SYSTEM_WIN_NT &&
                          DiskIsSuperStor(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_SUPERSTOR;
                    }
                else if ( uWinType != SYM_SYSTEM_WIN_NT &&
                          DiskIsDoubleSpace(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_DBLSPACE;
                    }
                else if ( uWinType != SYM_SYSTEM_WIN_NT &&
                          DiskIsStacker(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_STACKER;
                    }
                else
                    {
                    DiskGetFloppyType(dl, lpwMinorType);
                    }

                break;

            case DRIVE_FIXED:
            default:
                if ((lEncrypted = DiskIsEncrypted(dl)) != 0)
                    {
                    /*----------------------------------*/
                    /*  6/27/91: Added the minor types  */
                    /*----------------------------------*/
                    *lpwMajorType = TYPE_ENCRYPTED;
                    *lpwMinorType = (lEncrypted > 0 ? TYPE_OPEN : TYPE_CLOSE);
                    }
                else if ( uWinType != SYM_SYSTEM_WIN_NT &&
                          DiskIsSuperStor(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_SUPERSTOR;
                    }
                else if ( uWinType != SYM_SYSTEM_WIN_NT &&
                          DiskIsDoubleSpace(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_DBLSPACE;
                    }
                else if ( uWinType != SYM_SYSTEM_WIN_NT &&
                          DiskIsStacker(dl))
                    {
                    *lpwMajorType = TYPE_LOGICAL;
                    *lpwMinorType = TYPE_STACKER;
                    }
                else if (DiskIsRAM(dl))
                    {
                    *lpwMajorType = TYPE_RAMDISK;
                    }
                else if (DiskIsSubst(dl))
                    {
                    *lpwMajorType = TYPE_SUBSTED;
                    }
                else if (DiskIsAssigned(dl))
                    {
                    *lpwMajorType = TYPE_ASSIGNED;
                    }
                else
                    {
                    *lpwMajorType = TYPE_FIXED;
                    }
                break;
            }
        } /* else */

    return(NOERR);
}

#else                           // DOS version

UINT LOCAL PASCAL GetTheType (BYTE dl, UINT FAR * lpwMajorType, UINT FAR * lpwMinorType)
{
    auto        LONG            lEncrypted;
    auto        DISKREC         rDisk;
    auto        ABSDISKREC      rAbsDisk;


    if (DiskGetAccessFlag(dl) == DISALLOW_ACCESS)
        DiskSetAccessFlag(dl, ALLOW_ACCESS);

    if (!HWIsNEC() && dl < 'C' && DiskIsPhantom(dl))
        {
        *lpwMajorType = TYPE_PHANTOM;
        }
    else if (!DiskIsLogical(dl))
        {
        *lpwMajorType = TYPE_UNKNOWN;
        }
    else if (DiskIsCDRom(dl))
        {
        *lpwMajorType = TYPE_CDROM;
        }
    else if (DiskIsNetwork(dl))
        {
        *lpwMajorType = TYPE_REMOTE;
        }
    else if (DiskIsInterLink(dl))
        {
        *lpwMajorType = TYPE_REMOTE;
        }
    else if (DiskIsSubst(dl))
        {
        *lpwMajorType = TYPE_SUBST;
        }
    else if (DiskIsAssigned(dl))
        {
        *lpwMajorType = TYPE_ASSIGNED;
        }
    else if ((lEncrypted = DiskIsEncrypted(dl)) != 0)
        {
        *lpwMajorType = TYPE_ENCRYPTED;
        *lpwMinorType = (lEncrypted > 0 ? TYPE_OPEN : TYPE_CLOSE);
        }
    else
        {
        rDisk.dl = dl;

        *lpwMajorType = DiskGetPhysicalType (&rDisk, &rAbsDisk);

                                    /*------------------------------*/
                                    /* DiskGetPhysicalType return   */
                                    /* a combination of major and   */
                                    /* minor types.  For this       */
                                    /* reason, we need to seperate  */
                                    /* them out.                    */
                                    /*------------------------------*/
        if (IsTypeFloppy(*lpwMajorType) )
            {
            *lpwMinorType = *lpwMajorType;
            *lpwMajorType = TYPE_REMOVEABLE;
            }

        if (*lpwMajorType == TYPE_UNKNOWN)
            {
            if (DiskIsTrueDrive(dl, FILTER_ALL_DRIVES, NULL))
                *lpwMajorType = TYPE_LOGICAL;
            }
        }


                    // Make some distinctions about device-driven types...
    if (*lpwMajorType == TYPE_LOGICAL)
        {


        if (DiskIsSuperStor(dl))
            {
            *lpwMinorType = TYPE_SUPERSTOR;
            }
        else if (DiskIsDoubleSpace(dl))
            {
            *lpwMinorType = TYPE_DBLSPACE;
            }
        else if (DiskIsStacker(dl))
            {
            *lpwMinorType = TYPE_STACKER;
            }
        else if (DiskIsRAM(dl))
            {
            *lpwMajorType = TYPE_RAM;
            }
        else if (DiskIsFixed(dl))
            {
            *lpwMajorType = TYPE_FIXED;
            }
        else
            {
            *lpwMajorType = TYPE_REMOVEABLE;
            DiskGetFloppyType (dl, lpwMinorType);
            }
        }

                        // -----------------------------------------------
                        // if we say this is a floppy, make sure
                        // -----------------------------------------------
    if (*lpwMajorType != TYPE_UNKNOWN && CheckFloppy ((BYTE)(dl - 'A')) == FALSE)
        *lpwMajorType = TYPE_UNKNOWN;

    return(NOERR);
}

#pragma optimize("", off)
BOOL LOCAL PASCAL CheckFloppy (BYTE drive)
{
    auto        BYTE    cmosType;
    auto        BYTE    type1, type2;

    /*
     *
     * SKURTZ :09-11-95 04:40:06pm
     * On Nec machines, force a TRUE return.
     */

    if (!HWIsNEC())
        {

		if (drive > 1)              // this isn't drive A: or drive B:
			return (TRUE);

				                    // ----------------------------------------
					                // If we have a CMOS, verify that there is
						            // a value set for the diskette
							        // ----------------------------------------

		if (HWHasCMOS())            // get the CMOS drive types
			{
			cmosType = CMOSRead (0x10);

			type1 = (BYTE) (cmosType >> 4);
			type2 = (BYTE) (cmosType & 0x0F);

			if (drive == 0 && type1 != 0)
				return (TRUE);
			else if (type2 != 0)
				return (TRUE);
			else                    // If CMOS says there are no floppies
				return (FALSE);     //  then let's believe it.
			}

									// ----------------------------------------
									// Check the equipment list
									// ----------------------------------------
		_asm
			{
			int     11h             // get the equipment list

			mov     bx,ax
			and     bx,1
			mov     type1, bl       // save the diskette boot flag

			mov     cl,6
			shr     ax,cl
			and     ax,3
			mov     type2, al       // save the number of floppies
			}

		if (type1 == 0)             // if no boot diskette, no A: or B: drive
			return (FALSE);

		if (type2 < drive)          // can the drive number exist?
			return (FALSE);         // no
		}
    return (TRUE);					// this is a tough call--assume TRUE
}
#pragma optimize("", on)


BOOL SYM_EXPORT WINAPI DiskIsTrueDrive (BYTE dl, BYTE byFilter, DRIVEFILTERPROC lpfnFilter)
{
    if (!DiskIsLogical(dl))
        return (FALSE);

    if (byFilter == FILTER_ALL_DRIVES)  /* If all DOS drives, done      */
        return (TRUE);

                                        /* If assigned or network, then */
                                        /* return FALSE no matter what  */
    if (byFilter != FILTER_CUSTOM &&
         (DiskIsNetwork(dl) || DiskIsAssigned(dl) || DiskIsSubst(dl)))
        return (FALSE);

                                        /* We now have all logical      */
                                        /* drives. Return TRUE          */
    if (byFilter == FILTER_LOGICAL_ONLY)
        return (TRUE);

    if (byFilter == FILTER_SPECIAL || byFilter == FILTER_CUSTOM)
        {
        if (lpfnFilter != NULL)
            return (lpfnFilter (dl));
        }

    return (FALSE);
}
#endif                                  // not SYM_WIN (DOS case)
#endif                                  // #ifndef SYM_WIN32

/*----------------------------------------------------------------------*/
/*                                                                      */
/*      Returns the floppy drive type, using IOCTL.                     */
/*----------------------------------------------------------------------*/


// An NEC can have lots of floppies starting at any drive letter. This array
// stores the drive letter of each floppy. E.g. 'C','D','E','F',0,0,0,....
static BYTE floppysIndex [SYM_MAX_DOS_DRIVES] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

UINT SYM_EXPORT WINAPI DiskGetFloppyType (BYTE dl, UINT FAR * lpwType)
{
    auto        DeviceParamRec          deviceParams;
    auto        BYTE                    byDeviceType;
    auto        UINT                    wType = TYPE_UNKNOWN;
    auto        int                     index ;
    auto        BOOL                    bHWIsNEC;



    dl = (BYTE) CharToUpper(dl);

#ifdef SYM_WIN32
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        {
        char              szDrive[SYM_MAX_PATH];
        HANDLE            hFile;
        DISK_GEOMETRY     dg[20];
        DWORD             dwBytesReturned;
        UINT              wNumberOfTypes;


        wsprintf(szDrive, "\\\\.\\%c:", dl);
        hFile = CreateFile(         szDrive,
                                    0,
                                    FILE_SHARE_READ,
                                    NULL,
                                    OPEN_ALWAYS,
                                    0,
                                    NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return(SYM_ERROR_INVALID_FUNCTION);

        if (DeviceIoControl(hFile, IOCTL_DISK_GET_MEDIA_TYPES,
                NULL, 0, dg, sizeof(dg), &dwBytesReturned, NULL))
            {
                wNumberOfTypes = dwBytesReturned / sizeof(DISK_GEOMETRY);
                if (wNumberOfTypes < 1)
                    return NOERR;

                switch (dg[wNumberOfTypes-1].MediaType)
                    {
                    case F5_1Pt2_512:
                       wType = TYPE_12M;
                       break;
                    case F3_1Pt44_512:
                       wType = TYPE_144M;
                       break;
                    case F3_2Pt88_512:
                       wType = TYPE_288M;
                       break;
                    case F3_720_512:
                       wType = TYPE_720K;
                       break;
                    case F5_360_512:
                    case F5_320_512:
                       wType = TYPE_360K;
                       break;
                    case RemovableMedia:
                       wType = TYPE_REMOVEABLE;
                       break;
                    case FixedMedia:
                       wType = TYPE_FIXED;
                       break;
                    }
            }
        CloseHandle(hFile);
        *lpwType = wType;
        return NOERR;
        }

#endif

    bHWIsNEC = HWIsNEC();
    if (bHWIsNEC)
        {
        for ( index = 0; index < SYM_MAX_DOS_DRIVES; index ++ )
            {
            if ( floppysIndex [index] == dl ) // Has this letter been used before?
                break ;                       // Yes. We've found it.

            if ( floppysIndex [index] == 0 )  // Is this the end of the list?
                {
                floppysIndex [index] = dl ;   // Yes. Add this letter to the list
                break;
                }
            }
        }
                                        /*------------------------------*/
                                        /* Set to single drive          */
                                        /*------------------------------*/
#ifndef SYM_WIN32
    DiskSetToSingleDisk (dl);
#endif
                                        /*------------------------------*/
                                        /* Get the default parameters   */
                                        /*------------------------------*/
    deviceParams.specialFunctions = IOCTL_SF_GET_DEFAULT;

    if (DiskGenericIOCTL (GET_DEVICE_PARAMS, dl, (LPVOID) &deviceParams))
        {
        byDeviceType = deviceParams.deviceType;

                                        /* See if media type is valid   */
        if (!(byDeviceType == IOCTL_8_INCH_SINGLE ||
               (!bHWIsNEC && byDeviceType == IOCTL_8_INCH_DOUBLE) ||
	       (bHWIsNEC && byDeviceType == IOCTL_360K) ||
               byDeviceType == IOCTL_TAPE_DRIVE))
            {
                                        /*------------------------------*/
                                        /* Figure out the type          */
                                        /*------------------------------*/
            switch (byDeviceType)
                {
                case IOCTL_HD:
                    wType = TYPE_FIXED;
                    break;

                case IOCTL_360K:
                    wType = TYPE_360K;
                    break;

	        case IOCTL_8_INCH_DOUBLE:
                    // On the NEC this has a special meaning
                    // Here we determine if the NEC supports 1.44 MB disks
                    wType = NEC144Type (index);
                    break;

                case IOCTL_12M:
                    wType = TYPE_12M;
                    break;

                    // Added case to fix LS120 detection problem
                case IOCTL_144M:
                    wType = TYPE_144M;
                    break;
                    // end added case

                case IOCTL_720K:
                    wType = TYPE_720K;
                    break;

                default:
                    if (deviceParams.BPB.oldTotalSectors == 1440)
                        wType = TYPE_720K;
                    else if (deviceParams.BPB.oldTotalSectors == 2880)
                        wType = TYPE_144M;
                    else if (deviceParams.BPB.oldTotalSectors == 5760)
                        wType = TYPE_288M;
                }
            }
        }

    *lpwType = wType;

    return(NOERR);
}



/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/
int LOCAL PASCAL NEC144Type (int floppyIndex)
{
#ifdef _M_ALPHA
    // This should never be called, there are no (known) Alpha NEC machines
    SYM_ASSERT(FALSE);
	return TYPE_144M;
#else
     BYTE       ioBits;


     if ( floppyIndex > 3 )      // Can't have more than 4 1.44's
         return TYPE_12M;

     // We take the index of the disk shifted left by 5 and output
     // it to the port 0x4BE. Then we get the input from the same
     // port, and if is is 0xFE, then the NEC supports 1.44M disks
     ioBits = floppyIndex << 5 ;
     _asm
        {
        mov		dx, 0x04BE
        mov		al, ioBits
        out		dx, al
        in		al, dx
        cmp		al, 0xFE
        jz		is_144
        mov		al, ioBits
        or		al, 0x10
        out		dx, al
        in		al, dx
        xchg	        al, ah
        mov		al, ioBits
        out		dx, al
        in		al, dx
        cmp		ah, 0xFE
        jnz		is_not_144
is_144:
        mov		ioBits, al
        jmp	        short is_144_done
is_not_144:
        xor		al, al
        mov	        ioBits, al
is_144_done:
        }

     return ( ioBits ) ? TYPE_144M : TYPE_12M ;
#endif
}


/**********************************************************************
** DiskGetPhysicalType
**
** Description:
**      Determines the physical type of a drive.
**
** Input:
**      diskPtr points to a disk record whose drive letter is initialized.
**      absDiskPtr points to an empty absolute disk record
**      buffer points to a big DMA safe buffer, typically 40 * 512 bytes
**
** Returns:
**      Drive type constant, e.g. TYPE_FIXED, TYPE_360
**********************************************************************/

int SYM_EXPORT WINAPI DiskGetPhysicalType (DISKREC FAR *diskPtr,
                                           ABSDISKREC FAR *absDiskPtr)
{
    auto	    BYTE		    paramsError;
    auto        BYTE            error;
    auto        BYTE            DASDType;
    auto        BYTE            driveType;
    auto        BYTE            deviceType;
    auto        BYTE            CMOS_Type;
    auto        INT13REC        int13Package;
    auto        DeviceParamRec  deviceParamTable; /* Generic IOCTL record */
    auto        BPBREC          BPB;
    auto        DWORD           head,cylinder, endingTrack,sector;
    auto        BOOL            validGenericIOCTL;
    auto        ABSDISKREC      tempAbsDisk;
    auto        BYTE            byMajorVersion;
    auto        BOOL            bHWIsNEC;

    bHWIsNEC = HWIsNEC();

#if defined(SYM_WIN) && !defined(SYM_WIN32)
                                        /*------------------------------*/
                                        /* The Int 13h hook routine     */
                                        /* doesn't work in Standard     */
                                        /* mode yet.  If Standard mode, */
                                        /* return floppy disk type or   */
                                        /* type UNKNOWN (device driven) */
                                        /*------------------------------*/
    if (bHWIsNEC || (GetWinFlags() & WF_STANDARD))
        {
        UINT wMajorType, wMinorType;

        DiskGetType(diskPtr->dl, &wMajorType, &wMinorType);
        if (bHWIsNEC)
            {
            if (wMajorType == TYPE_REMOVEABLE)
	        {
	        DiskMapLogToPhyParams (0L, diskPtr->dl, &int13Package);
	        absDiskPtr->dn     = int13Package.dn;
            absDiskPtr->dwHead   = absDiskPtr->dwStartingHead   = int13Package.dwHead;
            absDiskPtr->dwTrack  = absDiskPtr->dwStartingTrack  = int13Package.dwTrack;
            absDiskPtr->dwSector = absDiskPtr->dwStartingSector = int13Package.dwSector;
	        absDiskPtr->sectorLenCode = int13Package.sectorLenCode;

					/* Get physical disk parameters	*/
					/*    from ROM.			*/
					/* (NOTE: Ext. Sence BIOS call	*/
					/*        in case of NEC)	*/
	        paramsError = DiskGetPhysicalInfo (absDiskPtr);
                return (wMinorType);
	        }
            }
        else
            {
            if (wMajorType == TYPE_REMOVEABLE)
                return (wMinorType);
            }

        return (TYPE_UNKNOWN);
        }
#endif

#ifndef SYM_WIN32
    if (DiskGetAccessFlag(diskPtr->dl) == DISALLOW_ACCESS)
        DiskSetAccessFlag(diskPtr->dl, ALLOW_ACCESS);
#endif

    driveType = TYPE_UNKNOWN;           /* Mark as unknown              */

                                        /* Convert to drive number      */
    diskPtr->dn = (BYTE)(diskPtr->dl - 'A');
    absDiskPtr->dn = 0xFF;              /* Initially set to invalid     */

                                        /* If this drive doesn't exist, */
                                        /* or isn't a block device,     */
                                        /* return UNKNOWN type          */
    if (DiskIsEncrypted (diskPtr->dl))
        return (driveType);

    if (!DiskIsBlockDevice(diskPtr->dl))
        return(driveType);

#ifndef SYM_WIN32
    DiskSetToSingleDisk (diskPtr->dl);  /* Set to single drive          */
#endif
                                        /*------------------------------*/
                                        /* Get starting physical        */
                                        /*      locations               */
                                        // Don't do this for OS/2 2.0+
                                        /*------------------------------*/
    byMajorVersion = HIBYTE(DOSGetVersion());
    int13Package.generated = FALSE;
    if ( byMajorVersion < 20 )
        {
        DiskMapLogToPhyParams (0L, diskPtr->dl, &int13Package);

                                        /*------------------------------*/
                                        /* If no INT 13 generated, then */
                                        /*   we can't format this disk  */
                                        /* DEVICE DRIVEN                */
                                        /*------------------------------*/
        if (!int13Package.generated)
            return (driveType);

                                        /*------------------------------*/
                                        /* Move to physical disk info   */
                                        /*------------------------------*/
        absDiskPtr->dn     = int13Package.dn;
        absDiskPtr->dwHead   = absDiskPtr->dwStartingHead   = int13Package.dwHead;
        absDiskPtr->dwTrack  = absDiskPtr->dwStartingTrack  = int13Package.dwTrack;
        absDiskPtr->dwSector = absDiskPtr->dwStartingSector = int13Package.dwSector;
        if (bHWIsNEC)
            {
	    absDiskPtr->sectorLenCode = int13Package.sectorLenCode;
#if defined (SYM_WIN)
        paramsError = DiskGetPhysicalInfo (absDiskPtr);
#else
        paramsError = _DiskGetPhysicalInfo (absDiskPtr);
#endif
            }
        }

                                        /*------------------------------*/
                                        /* Get Logical and Physical     */
                                        /*   parameters                 */
                                        /* Set to 0, so we get the      */
                                        /*   Default BPB, not the       */
                                        /*   actual BPB.                */
                                        /*------------------------------*/
    MEMSET(&deviceParamTable, 0, sizeof(DeviceParamRecord));

    if (validGenericIOCTL = DiskGenericIOCTL (GET_DEVICE_PARAMS,
                                        diskPtr->dl,
                                        (LPVOID) &deviceParamTable))
        {
        deviceType = deviceParamTable.deviceType;

                                        /* See if media type is valid   */
        if (deviceType == IOCTL_8_INCH_SINGLE ||
	    (bHWIsNEC && deviceType == IOCTL_360K) ||
            (!bHWIsNEC && deviceType == IOCTL_8_INCH_DOUBLE) ||
            deviceType == IOCTL_TAPE_DRIVE)
            {
            return (driveType);         /* Not valid media types        */
            }

                                        /* Figure out the type          */
        switch (deviceType)
            {
            case IOCTL_HD:
                driveType = TYPE_FIXED;
                break;

            case IOCTL_360K:
                driveType = TYPE_360K;
                break;

	    // NEC's DOS 3.30 returns IOCTL_12M, but DOS 3.30A returns
	    // IOCTL_8_INCH_DOUBLE. Also these 2 version of DOS	have same DOS
	    // version number!
	    case IOCTL_12M:
                if (!bHWIsNEC)
                    {
                    driveType = TYPE_12M;
                    break;
                    }
                // Fall through, if NEC computer

	    case IOCTL_8_INCH_DOUBLE:
		if (!(absDiskPtr->dn & 0x60))
		    driveType = TYPE_12M;
		else
		    driveType = TYPE_720K;
		break;

            case IOCTL_720K:
                driveType = TYPE_720K;
                break;

            default:
                                   /*------------------------------*/
                                   /* Compute diskette type from   */
                                   /* parameters found             */
                                   /*------------------------------*/
               if ((deviceParamTable.numberOfTracks == 40) &&
                    (deviceParamTable.BPB.sectorsPerTrack == 9))
                   {
                   driveType = TYPE_360K;
                   }
               else if (deviceParamTable.numberOfTracks == 80)
                   {
                   switch (deviceParamTable.BPB.sectorsPerTrack)
                       {
                       case 9:
                           driveType = TYPE_720K;
                           break;

                       case 15:
                           driveType = TYPE_12M;
                           break;

                       case 18:
                           driveType = TYPE_144M;
                           break;

                       case 36:
                           driveType = TYPE_288M;
                           break;
                       }
                   }
            }
        }
    else if (!bHWIsNEC)
        {
#if defined (SYM_WIN)
        if (!DiskGetPhysicalInfo(absDiskPtr))
#else
        if (!_DiskGetPhysicalInfo(absDiskPtr))
#endif
            {
            if (absDiskPtr->dn & 0x80)
                {
                driveType = TYPE_FIXED;
                }
            else
                {
                CMOS_Type = absDiskPtr->CMOS_Type;

                switch (CMOS_Type)
                    {
                    case CMOS_360K:
                        driveType = TYPE_360K;
                        break;

                    case CMOS_12M:
                        driveType = TYPE_12M;
                        break;

                    case CMOS_720K:
                        driveType = TYPE_720K;
                        break;

                    case CMOS_144M:
                        driveType = TYPE_144M;
                        break;

                    case CMOS_288M:
                        driveType = TYPE_288M;
                        break;

                    default:
                        goto tryMore;
                    }
                }
            }
        else
            {
tryMore:
                                        /* Read DASD Type               */
            error = (BYTE) DiskGetDASDType (absDiskPtr->dn, &DASDType);

            if (!error)                     /* If no error, check type      */
                {
                                        /* Return UNKNOWN type          */
                if ((DASDType == 0) || DASDType > 2)
                    return (driveType);

                                        /*------------------------------*/
                                        /* Change-Line supported;       */
                                        /* MOST of the time it is 1.2M, */
                                        /* except for Toshibas, where   */
                                        /* it could be 720K             */
                                        /* Even DOS FORMAT 3.3 will     */
                                        /* format drive wrong on        */
                                        /* Toshiba 1100+                */
                                        /*------------------------------*/
                if (DASDType == 2)
                    {
#ifndef SYM_WIN32
                    if (IsToshiba())
                        {
                        driveType = (BYTE) (((absDiskPtr->dwSectorsPerTrack == 0) ||
                                          (absDiskPtr->dwSectorsPerTrack != 15)) ?
                                                TYPE_720K : TYPE_12M);
                        }
                    else
#endif
                        {
                        driveType = TYPE_12M;
                        }
                    }
                else
                    {
                    goto changeLineUnsupported;
                    }
                }
            else
                {
changeLineUnsupported:
                                        /* Doesn't support DASD types   */

                                        /* Search ROM for 720K machines */
#ifdef SYM_WIN32
                driveType = TYPE_720K;
#else
                driveType = (BYTE) (((Is720KDrive ()) ? TYPE_720K : TYPE_360K));
#endif
                }
            }
        }
    else if (!paramsError)      // HWIsNEC
        {
        if ( ! ( absDiskPtr->dn & 0x50 ) )
           driveType = TYPE_HD;
        else if ( ! ( absDiskPtr->dn & 0x60 ) )
#ifndef SYM_OS2
           {
           driveType = TYPE_12M;
#ifndef _M_ALPHA
           if (HWIsNEC() &&
               Chk144Drive(diskPtr->dl))
               {
               driveType = TYPE_144M;
               absDiskPtr->dn &= 0x0f;
               absDiskPtr->dn |= 0x30;
               }
#endif
           }
#else
           driveType = TYPE_12M;
#endif
        else
           driveType = TYPE_720K;
        }
    else                        /* Always NEC return DASDType = 4 */
        driveType = TYPE_UNKNOWN;

                                        /*------------------------------*/
                                        /* If HD, see if it truly is    */
                                        /* physical or logical.  We will*/
                                        /* want to do this for types    */
                                        /* of "Hard Disk" scenarios:    */
                                        /* 1) DEVICE DRIVEN LOGICAL     */
                                        /*      PARTITION, like SQUISH  */
                                        /* 2) Partions which are NOT    */
                                        /*      sequentially allocated  */
                                        /*      (Like squish).          */
                                        /* 3) Partitions which span     */
                                        /*      drives.                 */
                                        /* 4) Partitions which cross    */
                                        /*      the 1024 cylinder       */
                                        /*      boundary.               */
                                        /*------------------------------*/
                                        /* To detect these scenarios,   */
                                        /* we will make sure the ending */
                                        /* cylinder is what we THINK    */
                                        /* it should be, by counting the*/
                                        /* number of DOS sectors in the */
                                        /* partition.                   */
                                        /*------------------------------*/
                                        // Don't do this for OS/2 2.0+
    if ( (driveType == TYPE_FIXED) && (byMajorVersion < 20) )
        {
                                        /*------------------------------*/
                                        /* If HD type, then make sure   */
                                        /* its drive # is within valid  */
                                        /* range (80h-8xh).             */
                                        /*------------------------------*/
        if (HWIsNEC())
            {
            if (!IsValidNECHDNumber(absDiskPtr->dn))
                return(TYPE_UNKNOWN);
            }
        else
            {
            if (absDiskPtr->dn < FIRST_HD)
                return(TYPE_UNKNOWN);
            }


        if (!HWIsNEC())
            {
            tempAbsDisk.dn = FIRST_HD;
#if defined (SYM_WIN)
            if (DiskGetPhysicalInfo(&tempAbsDisk))
#else
            if (_DiskGetPhysicalInfo(&tempAbsDisk))
#endif
                return(TYPE_UNKNOWN);
            else
                {
                if (((BYTE) (absDiskPtr->dn - FIRST_HD) >= tempAbsDisk.numDrives) ||
                    (absDiskPtr->dn < FIRST_HD) || (absDiskPtr->dn >= (FIRST_HD + 16)))
                    return(TYPE_UNKNOWN);
                }
            }
                                        /*------------------------------*/
                                        /* ALSO, the starting sector #  */
                                        /* for a partition MUST be 1 or */
                                        /* 2 to be valid.               */
                                        /*------------------------------*/
        if (!HWIsNEC())
           {
           if (!absDiskPtr->dwStartingSector || (absDiskPtr->dwStartingSector > 2))
                return (TYPE_UNKNOWN);
           }
                                        /*------------------------------*/
                                        /* Get complete disk attributes */
                                        /*------------------------------*/

        if (!DiskGetHDParams (diskPtr, absDiskPtr, &BPB))
            return (TYPE_UNKNOWN);

                                        /*------------------------------*/
                                        /* If no INT 13 generated, then */
                                        /*   we can't format this disk  */
                                        /* DEVICE DRIVEN                */
                                        /*------------------------------*/
        if (!int13Package.generated || (absDiskPtr->dn != int13Package.dn))
            return (TYPE_UNKNOWN);

        if (absDiskPtr->dwEndingTrack < absDiskPtr->dwStartingTrack)
            return (TYPE_UNKNOWN);
                                        /*------------------------------*/
                                        /* Now check these values to    */
                                        /* insure our system is in Synch*/
                                        /* The ONLY place we could get  */
                                        /* screwed is if the HEADs value*/
                                        /* is not correct for the drive.*/
                                        /* (too large or too small).    */
                                        /*------------------------------*/
        if (HWIsNEC())
            {
            auto    WORD    wSecLen;
            auto    DWORD   dwEndingTrack;

            wSecLen = 128 << absDiskPtr->sectorLenCode;

            ConvertLongToPhySector (absDiskPtr,
                                    diskPtr->totalSectors *
                                    (diskPtr->bytesPerSector / wSecLen),
                                    &head, &cylinder, &sector);

                                        /*------------------------------*/
                                        /* If beyond 1024 cylinder, then*/
                                        /* device-driven                */
                                        /*------------------------------*/
            dwEndingTrack = absDiskPtr->dwStartingTrack + cylinder;

                                        // NEC Int 1B can support 64k tracks!
            if( dwEndingTrack > 0xFFFF )
                return(TYPE_UNKNOWN);

            endingTrack = dwEndingTrack;
            }
        else
            {
			ConvertLongToPhySector (absDiskPtr,
                                diskPtr->totalSectors *
                                (diskPtr->bytesPerSector / PHYSICAL_SECTOR_SIZE),
                                &head, &cylinder, &sector);

                                        /*------------------------------*/
                                        /* If beyond 1024 cylinder, then*/
                                        /* device-driven                */
                                        /*------------------------------*/
            endingTrack = absDiskPtr->dwStartingTrack + cylinder;
            // Extended int13 drives can have ending tracks higher than 1023
            if((absDiskPtr->bIsExtInt13 != ISINT13X_YES) &&
			   (endingTrack > 1023))
                return(TYPE_UNKNOWN);
			}
                                        /*------------------------------*/
                                        /* Now see if the ending        */
                                        /* is what we think it should be*/
                                        /*------------------------------*/
        if (MathLongDiff(endingTrack, absDiskPtr->dwEndingTrack) > 2)
            return(TYPE_UNKNOWN);
        }

    return (driveType);                 /* Return the found drive type  */

}


#ifndef SYM_WIN32

/**********************************************************************
** IsToshiba
**
** Description:
**      Helper routine for DiskGetPhysicalType()
**********************************************************************/

BOOL LOCAL PASCAL IsToshiba (VOID)
{
    auto        BOOL    returnValue = FALSE;
    auto        LPBYTE  lpMem;
    static      char    TOSHIBA_STR [] = "TOSHIBA";
#ifdef  SYM_PROTMODE
    auto        UINT    wSelector;
#endif

    if (HWIsNEC())
        return(FALSE);

#ifdef  SYM_PROTMODE
    if (!SelectorAlloc(0xF800, 0xFFFF, &wSelector))
        {
        lpMem = MAKELP(wSelector, 0x0000);
#else
        lpMem = (LPBYTE) 0xF8000000L;
#endif

        if (MemorySearch(lpMem, 0x8000, TOSHIBA_STR, STRLEN(TOSHIBA_STR)))
            returnValue = TRUE;

#ifdef  SYM_PROTMODE
        SelectorFree(wSelector);
        }
#endif

    return (returnValue);
}


/**********************************************************************
** Is720KDrive
**
** Description:
**      Helper routine for DiskGetPhysicalType()
**
** Returns:
**      TRUE if the computer is one that has 720K drives, not 360K.
**      Many of the early lap-tops were of this type.
**********************************************************************/

BOOL LOCAL PASCAL Is720KDrive (VOID)
{
    auto        BYTE    machineId;
    auto        BOOL    returnValue = FALSE;
    auto        LPBYTE  lpMem;
    auto        int     i;
    static      BYTE    TANDY_STR [] = "Tandy";
    static      LPSTR   MACHINES_720K [] = { "KAYPRO 2000",
                                              "NEC M",
                                              "DATA GEN",
                                              "SPARK",
                                              NULL
                                            };


    if (HWIsNEC())
        return(FALSE);

#ifdef  SYM_PROTMODE
    {
    auto        UINT    wSelector;

    if (!SelectorAlloc(0xF800, 0xFFFF, &wSelector))
        {
        lpMem = MAKELP(wSelector, 0x0000);
#else
        lpMem = (LPBYTE) 0xF8000000L;
#endif
                                        /* Search for 720K machines     */
        for (i = 0; MACHINES_720K [i]; i++)
            if (MemorySearch(lpMem, 0x8000,
                             MACHINES_720K[i], STRLEN(MACHINES_720K[i])))
                returnValue = TRUE;

        if (!returnValue)
            {
                                        /* Special case TANDY           */
            if (MemorySearch(lpMem, 0x8000, TANDY_STR, STRLEN((LPSTR)TANDY_STR)))
                {
                                        /* Look at machine type         */
                                        /* If early machine type, then  */
                                        /* return as 360K drive         */
                MemCopyPhysical(&machineId, MAKELP(0x0F000, 0x0FFFE), 1, 0);

                                        /* If not PC or old XT          */
                if (!((machineId == 0xFF) || (machineId == 0xFE)))
                    returnValue = TRUE;
                }
            }

#ifdef  SYM_PROTMODE
        SelectorFree(wSelector);
        }
    }
#endif

    return (returnValue);
}
#endif                                  // #ifndef SYM_WIN32


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This function converts the drive types to a mask needed for     */
/*      displaying the drive bitmaps.                                   */
/*                                                                      */
/* Parameters:                                                          */
/*      UINT wMajorType     -- Major type of the disk                   */
/*      UINT wMinorType     -- Minor type of the disk                   */
/*                                                                      */
/* Return values:                                                       */
/*      The type mask                                                   */
/*                                                                      */
/* See Also:                                                            */
/*      DiskGetType()                                                   */
/*                                                                      */
/************************************************************************/
/* 10/16/91 Replaced DiskGetAllTypes()                                  */
/************************************************************************/
UINT SYM_EXPORT WINAPI DiskGetMask(UINT wMajorType, UINT wMinorType)
{
    switch (wMajorType)
        {
        case  TYPE_REMOVEABLE:
            if (wMinorType == TYPE_FIXED)
                return(NDRV_HARD);

            return(wMinorType == TYPE_360K  ||  wMinorType == TYPE_12M ?
                                    NDRV_525 : NDRV_350);

        case TYPE_FIXED:
            return(NDRV_HARD);

        case TYPE_REMOTE:
            return(NDRV_NETWORK);

        case TYPE_RAMDISK:
            return(NDRV_RAM);

        case TYPE_CDROM:
            return(NDRV_CDROM);

        case TYPE_SUBSTED:
            return(NDRV_SUBSTED);

        case TYPE_ASSIGNED:
            return(NDRV_ASSIGNED);

        case TYPE_ENCRYPTED:
            return(wMinorType == TYPE_OPEN ? NDRV_UNLOCKED : NDRV_LOCKED);

        case TYPE_LOGICAL:
            if (wMinorType == TYPE_STACKER)
                return (NDRV_STACKER);
            return(NDRV_COMPRESSED);

        case TYPE_PHANTOM:
            return(NDRV_PHANTOM);

        case TYPE_DOESNT_EXIST:
        case TYPE_UNKNOWN:              // Added 2/14/92
        default:
            return(NDRV_AVAILABLE);
        } /* switch */
} /* DiskGetMask */


#if !defined(SYM_WIN32) && !defined(SYM_OS2)
/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This function checks if a network is installed.                 */
/*                                                                      */
/*      (May be just for MS type nets i.e. LanMan, LanTastic, etc.?     */
/*       Probably just checks for NetBios?)                             */
/*                                                                      */
/* THIS FUNCTION IS WORTHLESS!!!  I HOPE NOBODY IS CALLING IT           */
/* AND EXPECTING TO GET BACK USEFUL INFORMATION.                        */
/*                                                                      */
/* Parameters:                                                          */
/*      none                                                            */
/*                                                                      */
/* Return values:                                                       */
/*      FALSE no network installed                                      */
/*      nonzero - network installed.  Bits have following meaning:      */
/*        bit 6 - server                                                */
/*        bit 2 - messenger                                             */
/*        bit 7 - receiver                                              */
/*        bit 3 - redirector                                            */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
#pragma optimize("", off)
UINT SYM_EXPORT WINAPI NetworkInstalled(void)
{
    BYTE byAl;
    BYTE byBl;

    _asm                                // Check for a network
        {
        mov     ax,   0B800h            ; Function B8h, subfunction 00h
        int     2fh
        mov     byAl, al
        mov     byBl, bl
        }
                                        // byAl is zero for no network
                                        // nonzero for network.  If network
                                        // then byBl contains bitcodes for
                                        // server/messenger, etc.
    return((UINT)(byAl ? byBl : FALSE));
}
#pragma optimize("", on)
#endif                                  // #ifndef SYM_WIN32


#if !defined(SYM_WIN32) && !defined(SYM_OS2)
/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This function checks if the given drive letter is safe to       */
/*      perform direct disk access (int 13h, int 25h, int 26h) on,      */
/*      at least as far as the network is concerned - use               */
/*      DiskIsBlockDevice() for the complete set of tests (that function*/
/*      calls this one).                                                */
/*                                                                      */
/*      Direct disk access is also not safe if the drive is redirected. */
/*                                                                      */
/*      The interupt call that this makes will only work if MS or       */
/*      LanTastic networks are installed.  NetworkInstalled() is called,*/
/*      but that may be generic for any network?                        */
/*                                                                      */
/* Parameters:                                                          */
/*                                                                      */
/*      dl - Drive to be tested                                         */
/*                                                                      */
/* Return values:                                                       */
/*      TRUE if block I/O allowed (or no network is installed)          */
/*      FALSE if not.                                                   */
/*                                                                      */
/* See Also:                                                            */
/*      DiskIsBlockDevice()                                             */
/*                                                                      */
/************************************************************************/

BOOL SYM_EXPORT WINAPI NetDirectIOOK(BYTE dl)
{
    BOOL bRet = TRUE;
    BYTE szDriveSpec[3];                // Drive letter, colon, EOS
    LPBYTE lpszDriveSpec = szDriveSpec;

    szDriveSpec[0] = dl;
    szDriveSpec[1] = ':';
    szDriveSpec[2] = EOS;

                                        // If the SERVER bit (bit 6) is
                                        // set, then direct IO is NOT OK.
                                        // ... return false.
    if (NetworkInstalled() & 0x40)
       bRet = FALSE;

//
// - doesn't work with LANtastic v4.x and N10Net
//
//    if (NetworkInstalled())
//        {
//
//        _asm
//            {
//            push    ds                  ; Save DS
//            mov     ax,   0300h         ; Function 03h, Subfunction 00h
//            lds     si,   lpszDriveSpec ; Drive letter colon (could be
//                                        ; full path)
//
//            int     2ah                 ; Check direct I/O
//            xor     ax, ax              ; Direct I/O not OK
//            jc      NOT_ALLOWED
//            inc     ax                  ; Direct I/O OK
//        NOT_ALLOWED:
//            pop     ds                  ; Restore DS
//            mov     bRet, ax            ; Return result
//            }
//        }
//


    return(bRet);
}
#endif


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      IMPORTANT:  JUST A STUB RIGHT NOW                               */
/*      This function checks if the given drive letter is using an      */
/*      HPFS file system.  This is significant for at least the         */
/*      following functions:  DiskHasBootRecord(), VolumeLabelGet(),    */
/*      and VolumeLabelSet().                                           */
/*      IMPORTANT:  The above named functions are currently kludged to  */
/*      work around this lack of knowledge.  When this function is      */
/*      implemented for real, please check and update these functions.  */
/*                                                                      */
/* Return values:                                                       */
/*      TRUE if HPFS file system is being used                          */
/*      FALSE if not.                                                   */
/*                                                                      */
/************************************************************************/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskIsHPFS (BYTE dl)
{
    //&? See note above!
    return (FALSE);
}
#endif //   #ifndef SYM_OS2 from many routines ago


/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This function checks if the given drive letter maps to a MS     */
/*      CD ROM drive.                                                   */
/*                                                                      */
/* Parameters:                                                          */
/*                                                                      */
/* Return values:                                                       */
/*      TRUE if CD ROM                                                  */
/*      FALSE if not.                                                   */
/*                                                                      */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
#if !defined(SYM_OS2) && !defined(SYM_VXD)
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskIsCDRom (BYTE dl)
{
#ifdef SYM_WIN32
    char szDisk[10];
    UINT wDriveType;

    STRCPY(szDisk, "x:\\");
    szDisk[0] = (char)dl;
    wDriveType = GetDriveType(szDisk);
    return (BOOL)(wDriveType == DRIVE_CDROM);
#else
    BOOL bCDRom = FALSE;
    UINT DriveNum;
    UINT wDriver;

    dl = (BYTE) CharToUpper(dl);

    DriveNum = (UINT) dl - (UINT) 'A';

    _asm                                // Check if actually a CD drive
        {
        mov     ah, 15h
        mov     al, 0Bh
        xor     bx, bx
        mov     cx, DriveNum
        int     2fh
        mov     bCDRom, ax
        mov     wDriver, bx
        }

    return (wDriver != 0xADAD ? FALSE : bCDRom);
#endif
}
#pragma optimize("", on)
#endif //   #ifndef SYM_OS2



#if !defined(SYM_WIN32) && !defined(SYM_OS2)

/************************************************************************
 *                                                                      *
 * Description:                                                         *
 *      LPVOID SYM_EXPORT WINAPI DiskIsStackerLoaded()                      *
 *                                                                      *
 * Parameters:                                                          *
 *      None                                                            *
 *                                                                      *
 * Return Value:                                                        *
 *      Pointer to Stacker information if installed, else NULL.         *
 *	Pointer is always REAL MODE, even on Windows platform.		*
 *                                                                      *
 *      Stacker is detected by making an INT 25H call with invalid      *
 *      parameters.                                                     *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * 07/14/1992 ED Created.                                               *
 ************************************************************************/

#if defined(SYM_PROTMODE)

LPVOID SYM_EXPORT WINAPI DiskIsStackerLoaded(void)
{
    static LPVOID lpStackerCA = NULL;
    struct int25rec
        {
        unsigned signature;     // must be STACKER_INT25_SIG on return
        unsigned cnt;           // must be same as value passed in CX to INT 25
        void far *captr;
        char pad[1024];         // temp buffer (just in case INT 25 tries to read!)
        } far *lpIdRecREAL, far *lpIdRecPROT;
    DWORD dwDosBuffer;
    DPMIRec DPMI;
    BOOL bDpmiError;
    WORD wSelector;
    LPWORD lpwCommAreaPROT;

    // Only do this test once per session
    if (lpStackerCA != NULL)
        return(lpStackerCA);

    // If this is Windows NT, bail out right now ...
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        return(NULL);

    if ((dwDosBuffer = GlobalDosAlloc(sizeof(struct int25rec))) == NULL)
        return(NULL);

    lpIdRecREAL = MAKELP(HIWORD(dwDosBuffer), 0);
    lpIdRecPROT = MAKELP(LOWORD(dwDosBuffer), 0);

    lpIdRecPROT->signature = 0; // clear fields before call to see if it works
    lpIdRecPROT->cnt       = 0xFFFF;
    lpIdRecPROT->captr     = NULL;

    MEMSET(&DPMI, 0, sizeof(DPMI));   // Must start with zero'd structure
    DPMI.eax = 0xCDCD;                  // Special drive # that Stacker uses
    DPMI.ecx = 1;                       // Read one sector
    DPMI.edx = 0;                       // Sector 0
    DPMI.ds =  FP_SEG(lpIdRecREAL);     // Buffer segment
    DPMI.ebx = FP_OFF(lpIdRecREAL);     // Buffer offset

    // Work around bug in MSDOS 3.X where the word at ES:[BP+1EH] gets
    // 0xFFFF written to it during an INT 25H.  We point ES:BP at the
    // int25rec, so the only thing trashed are some "pad" bytes that
    // we don't care about anyway.
    DPMI.es = DPMI.ds;
    DPMI.ebp = DPMI.ebx;

    DPMI.simulatedInt = 0x25;
    bDpmiError = DPMISimulateInt(&DPMI);

    if (!bDpmiError &&
        lpIdRecPROT->signature == 0xCDCD &&
        lpIdRecPROT->cnt == 1)
        {
        if (!SelectorAlloc(FP_SEG(lpIdRecPROT->captr), 0xFFFF, &wSelector))
	    {
	    lpwCommAreaPROT = MAKELP(wSelector, FP_OFF(lpIdRecPROT->captr));
	    if (*lpwCommAreaPROT == 0x0A55A)
		{
		// Found the communications area!
		lpStackerCA = lpIdRecPROT->captr;
		}

	    SelectorFree(wSelector);
	    }
        }

    GlobalDosFree(LOWORD(dwDosBuffer));

    return(lpStackerCA);
}

#elif defined(SYM_DOS)

#pragma optimize("", off)

LPVOID SYM_EXPORT WINAPI DiskIsStackerLoaded(void)
{
    BYTE        szTemp[1024];
    LPWORD      lpStacker = NULL;
    UINT        wID;

    // If this is Windows NT, bail out right now ...
    if (SystemGetWindowsType() == SYM_SYSTEM_WIN_NT)
        return(NULL);

    _asm
        {
        push    ds                      ;save original DS
        mov     ax,ss                   ;set DS == SS
        mov     ds,ax
        mov     ax,0CDCDh               ;invalid drive #
        lea     bx,word ptr szTemp      ;DS:BX --> buffer for INT 25H
        mov     cx,1
        xor     dx,dx                   ;read boot sector of invalid drive
        mov     word ptr ss:[bx],dx     ;clear the "return" value
        push    bp                      ;save our BP
        push    ds                      ;(set ES:BP to fix DOS 3 INT 25H bug)
        pop     es
        mov     bp,bx
        int     25H
#ifndef	SYM_DOSX
        pop     cx                      ;get rid of flags
#endif
        pop     bp                      ;restore our BP
        pop     ds                      ;restore original DS
        }

                                        // Stacker INT 25 fills in some fields
    if (*(LPWORD) &szTemp[0] == 0xCDCD && *(LPWORD) &szTemp[2] == 1)
        {
                                        // get a pointer into Stacker
        lpStacker = (LPWORD) (* (LPDWORD) &szTemp[4]);

        wID = *lpStacker;               // get the Stacker signature
        if (wID != 0xA55A)       // check Stacker signature
            lpStacker = NULL;
        }

    return ((LPVOID) lpStacker);
}

#pragma optimize("", on)

#endif // SYM_PROTMODE


/*----------------------------------------------------------------------*
 * DiskIsStacker2()							*
 *									*
 * This is similar to the standard DiskIsStacker() function, but it	*
 * performs the test by reading the boot sector on the subject drive	*
 * and checking for Stac's signature.  This works on WFWG 3.11 VFAT	*
 * systems, whereas the IOCTL method used by DiskIsStacker() does not.	*
 * The problem is that this function will cause a disk I/O when it	*
 * reads the boot sector, so it isn't appropriate for all situations.	*
 *									*
 * It should be noted that Stac's API is based on IOCTL's, so we aren't	*
 * able to do anything more than detect their drive -- having this	*
 * function is not a complete solution to the problems of Stacker and	*
 * VFAT.  The main reason for this function is so NDDW and SDW can	*
 * detect when Stacker and VFAT are together, and put up an dialog	*
 * telling the user that they're out of luck.				*
 *----------------------------------------------------------------------*/

#if defined(SYM_DOS) || defined(SYM_WIN16)

static BYTE byStacSignature[8] =
    {
    0xCD, 'S'-'@', 0xCD, 'T'-'@', 0xCD, 'A'-'@', 0xCD, 'C'-'@'
    };

typedef struct
    {
    BYTE	bySignature[8];	/* signature				*/
    LPVOID	lpCommArea;	/* pointer to comm. area		*/
    BYTE	byLastUnit;	/* last unit number, set on each call	*/
    BYTE	byPassUnit;	/* actual lastUnit for passthru units	*/
    WORD	wInt21;		/* pointer to INT 21 code		*/
    } StacBootSig;		/* Should be 16 bytes long		*/

typedef struct
    {
    BYTE	filler[PHYSICAL_SECTOR_SIZE - sizeof(StacBootSig)];
    StacBootSig	ID;
    } StacBootRec;

BOOL SYM_EXPORT WINAPI DiskIsStacker2(BYTE drive)
{
    auto	LPVOID		lpStacker;
    auto	StacBootRec	far *lpBootRec;
    auto	DiskRec		disk;
    auto	BOOL		bIsStacker;

    // See if Stacker driver is loaded
    if ((lpStacker = DiskIsStackerLoaded()) == NULL)
	return(FALSE);

    // Try normal Stacker detection first
    if (DiskIsStacker(drive))
	return(TRUE);

    // Fill in a DiskRec so we can do a DiskBlockDevice() on the drive
    if (!DiskGetInfo(drive, &disk))
	return(FALSE);

    // Allocate memory for a boot sector buffer
    if (disk.bytesPerSector != sizeof(StacBootRec))
	return(FALSE);
    if ((lpBootRec = MemAllocFixed(sizeof(StacBootRec))) == NULL)
	return(FALSE);

    // Read boot sector and check for Stacker signature
    bIsStacker = FALSE;
    disk.sector = 0L;
    disk.buffer = (HPBYTE) lpBootRec;
    disk.sectorCount = 1;
    if (!DiskBlockDevice(BLOCK_READ, &disk))
	{
	if ((MEMCMP(lpBootRec->ID.bySignature, byStacSignature, sizeof(byStacSignature)) == 0) &&
	    (lpBootRec->ID.lpCommArea == lpStacker))
	    {
	    bIsStacker = TRUE;
	    }
	}

    MemFreeFixed(lpBootRec);

    return(bIsStacker);
}

#endif //defined(SYM_DOS) || defined(SYM_WIN16)

#endif // #if !defined (SYM_WIN32) && !defined(SYM_OS2)

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#ifndef SYM_OS2
#pragma optimize("", off)

BOOL SYM_EXPORT WINAPI _DiskIsDoubleSpace(BYTE drive, LPBYTE lpbyHostDriveLetter,
                                   LPWORD lpbSwapped, LPBYTE lpbySequence)
{

#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfn_DiskIsDoubleSpace;
    extern HINSTANCE    hInstThunkDll;

    if (lpfn_DiskIsDoubleSpace == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfn_DiskIsDoubleSpace = GetProcAddress(hInstThunkDll,"_DiskIsDoubleSpace32" );
    	    if (lpfn_DiskIsDoubleSpace == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfn_DiskIsDoubleSpace( drive, lpbyHostDriveLetter, lpbSwapped, lpbySequence) );
#endif // _M_ALPHA
#else
    auto BOOL bIsDblSpace   = 0;
    auto BOOL bSwapped      = 0;
    auto BYTE byHost        = 0;
    auto BYTE bySequence    = 0;
    auto BYTE byDriveNumber = (BYTE) (drive - 'A');

    _asm
    {
        mov     ax, 4A11h       ; function call to detect double space drive
        mov     bx, 1
        mov     dl, byDriveNumber
        int     2Fh

        or      ax,ax           ; if AX != 0000h
        jnz     locRet          ;    then function is not successful

        test    bl,80h          ; is high bit of BL set?
        jz      HostDrive       ;     if yes, then it's a double space drive?


        ; We have a DoubleSpace Drive, need to figure out host drive.
        ;
        ; This is tricky because of the manner in which DBLSPACE.BIN
        ; keeps track of drives.
        ;
        ; For a swapped CVF, the current drive number of the host
        ; drive is returned by the first GetDriveMap call.  But for
        ; an unswapped CVF, we must make a second GetDriveMap call
        ; on the "host" drive returned by the first call.  But, to
        ; distinguish between swapped and unswapped CVFs, we must
        ; make both of these calls.  So, we make them, and then check
        ; the results.

        mov     bIsDblSpace, TRUE
        mov     bySequence,bh   ; Save sequence number

        and     bl,7Fh          ; bl = "host" driver Number
        mov     byHost,bl               ; Save 1st host drive
        mov     dl,bl           ; Set up for query of "host" drive

        mov     ax, 4A11h       ; function call to detect double space drive
        mov     bx, 1           ; bx = GetDriveMap function
        int     2Fh             ; (bl AND 7fh) == 2nd host drive

        and     bl, 7Fh         ; bl = 2nd host drive
        cmp     bl, byDriveNumber       ; Is host of host of drive itself?
        mov     bSwapped, TRUE   ; Assume compressed volume is swapped
        je      locRet          ; Yes, compressed volume is swapped!

        mov     bSwapped, FALSE ; No, compressed volume is not swapped
        mov     byHost,bl               ; The true host drive is 2nd host drive
        jmp     short locRet

    HostDrive:
        and     bl,7Fh          ; Strip high bit
        cmp     bl,dl           ; Is drive a swapped drive?
        je      locRet          ;    NO
        mov     bSwapped,TRUE   ;    YES
        mov     byHost,bl               ; set Boot drive number

    locRet:
    }

    *lpbSwapped          = bSwapped;
    *lpbyHostDriveLetter = (BYTE) (byHost + 'A');
    *lpbySequence        = bySequence;

    return( bIsDblSpace );
#endif // SYM_WIN32
}

#pragma optimize("", on)

/*=======================================================================
==
==  DiskIsDriveSpace3()
==
  =======================================================================
**
** DESCRIPTION:
**
**
** RETURNS:
**      TRUE: This is a DriveSpace 3 volume.
**
 *=====================================================================*/
BOOL SYM_EXPORT WINAPI DiskIsDriveSpace3(BYTE dl)
{
    auto    WORD     iVersion;

    if(DiskGetDriveSpaceVersion(dl, &iVersion) == -1)
        return(FALSE);

    if(iVersion != 3)
        return(FALSE);

    return(TRUE);

}

/*=======================================================================
==
==  DiskGetDriveSpaceVersion()
==
  =======================================================================
**
** DESCRIPTION:
**      This function returns the DriveSpace version for a given
**      volume.  This is done by reading the 1st sector of the actual
**      compressed volume, and checking the byte value at offset 52(dec).
**
**
** ARGUMENTS:
**          BYTE dl     - Drive letter of volume.
**          int * lpVer - Address to store version number.
**                        returns:
**                          3 for DriveSpace 3
**                          2 for DriveSpace 2
**                          0 non DriveSpace volume.
**
** Return values: same as above for lpVer.
**                -1 Error opening or reading CVF
**
 *=====================================================================*/

int SYM_EXPORT WINAPI DiskGetDriveSpaceVersion(BYTE dl, LPWORD lpVer)
{
    auto    BYTE    byHost;
    auto    BYTE    bySequence;
    auto    WORD    bySwapped;
    auto    BYTE    szCVF[16];
    auto    BYTE    buffer[512];
    auto    HFILE   hCVFHandle;

    *lpVer = 0;

    /*----------------05-13-95 12:52pm------------------
    ** First, is this a DriveSpace drive?
    ** ------------------------------------------------*/
    if(!DiskIsDriveSpace(dl))
        return(*lpVer);

    /*----------------05-13-95 12:53pm------------------
    ** Now we need to get the sequence byte to build
    ** the CVF file name.
    ** ------------------------------------------------*/
    if(!_DiskIsDoubleSpace(dl, &byHost, &bySwapped, &bySequence))
        return(*lpVer);

    /*----------------05-13-95 12:54pm------------------
    ** Now, construct DriveSpace filename.
    ** ------------------------------------------------*/
#ifdef SYM_WIN
    wsprintf(szCVF,"%c:\\DRVSPACE.%03d", byHost, bySequence);
#else
    StringPrint(szCVF,"%c1:\\DRVSPACE.%03d2", byHost, bySequence);
#endif

    /*----------------05-13-95 12:57pm------------------
    ** Now, open CVF.
    ** ------------------------------------------------*/
    if((hCVFHandle = FileOpen(szCVF, OF_READ)) == HFILE_ERROR)
        {
        /*----------------05-13-95 02:21pm------------------
        ** Try old DblSpace filename format.
        ** ------------------------------------------------*/
#ifdef SYM_WIN
        wsprintf(szCVF,"%c:\\DBLSPACE.%03d", byHost, bySequence);
#else
        StringPrint(szCVF,"%c1:\\DBLSPACE.%03d2", byHost, bySequence);
#endif
        if((hCVFHandle = FileOpen(szCVF, OF_READ)) == HFILE_ERROR)
            return(-1);
        }

    if(FileRead(hCVFHandle, buffer, 512) == HFILE_ERROR)
        {
        FileClose(hCVFHandle);
        return(-1);
        }

    /*----------------05-13-95 01:05pm------------------
    ** Look at BYTE at offset 52 of buffer.  This is the
    ** hi-byte of the version stamp.  If it is 0x01, then
    ** we have a DriveSpace3 volume, else we have a
    ** DriveSpace 2 volume.
    ** ------------------------------------------------*/
    if( buffer[52] == 0x01 )
        *lpVer = 3;
    else
        *lpVer = 2;

    /*----------------05-13-95 01:09pm------------------
    ** Close the CVF.
    ** ------------------------------------------------*/
    FileClose(hCVFHandle);

    return(*lpVer);
}


#endif // !SYM_OS2

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#ifndef SYM_OS2

#define UNIT_OFFS       0x3E            // offset within Stacker

#if defined(SYM_PROTMODE) || defined(SYM_WIN)

//;-----------------------------------------------------------------------------
//;
//; BOOL PASCAL DiskIsStacker(BYTE dl)
//;
//; Input: dl: 'A', 'B', etc.
//;
//; Output:AX=0 --> Is a Stacker volume
//;      AX=1 --> Is not a Stacker volume
//;
//;-----------------------------------------------------------------------------

BOOL SYM_EXPORT WINAPI DiskIsStacker(BYTE drive)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnDiskIsStacker;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnDiskIsStacker == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnDiskIsStacker = GetProcAddress(hInstThunkDll,"DiskIsStacker32" );
    	    if (lpfnDiskIsStacker == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}
    return( lpfnDiskIsStacker( drive ) );
#endif // _M_ALPHA
#else
    static      LPBYTE  lpStacker = NULL;
    static      BOOL    bChecked = FALSE;
    auto        BOOL    bResult = FALSE;
    auto	DPMIRec	DPMI;
    auto	BOOL	bDpmiError;

    if (!bChecked)                      // do installation check
        {
        lpStacker = (LPBYTE) DiskIsStackerLoaded();
        bChecked = TRUE;
        }

    if (lpStacker)
        {
        LPBYTE  lpTemp = lpStacker;
        UINT    wSelector;

        if (SelectorAlloc(FP_SEG(lpStacker), 0xFFFF, &wSelector))
            return(FALSE);
        FP_SEG(lpTemp) = wSelector;

	lpTemp[UNIT_OFFS] = 0xFF;

	MEMSET(&DPMI, 0, sizeof(DPMI)); // Must start with zero'd structure
	DPMI.eax = 0x4408;		// Do IOCTL call
	DPMI.ebx = drive - '@';		// BL = Drive (A=1, B=2, etc.)
	DPMI.simulatedInt = 0x21;
	bDpmiError = DPMISimulateInt(&DPMI);

	if (!bDpmiError && !DPMI.cflag && lpTemp[UNIT_OFFS] != 0xFF)
	    bResult = TRUE;

        SelectorFree(wSelector);
        }

    return (bResult);
#endif // SYM_WIN32
}

#elif defined(SYM_DOS)

#pragma optimize("", off)

BOOL SYM_EXPORT WINAPI DiskIsStacker(BYTE drive)
{
    static      LPBYTE  lpStacker = NULL;
    static      BOOL    bChecked = FALSE;
    auto        BOOL    bResult = FALSE;

    if (!bChecked)                      // do installation check
        {
        lpStacker = (LPBYTE) DiskIsStackerLoaded();
        bChecked = TRUE;
        }

    if (lpStacker)
        {
        _asm
        {
        les     di,lpStacker
        mov     byte ptr es:UNIT_OFFS[di],0FFH  ;set unit #

        mov     ax,4408H                ;do an ioctl call (removeable?)
        mov     bl,drive                ;BL=Drive Letter (A='A', B='B', etc.)
        sub     bl,'@'                  ;BL=Drive # (A=1, B=2, etc.)
        xor     bh,bh
        int     21h                     ;if Stacker drive, will return
        mov     ax,0
        jc      notstk2                 ;no ioctl support --> not Stacker

        les     di,lpStacker            ;see if unit # changed
        cmp     byte ptr es:UNIT_OFFS[di],0FFH
        jz      notstk2
        inc     bResult                 ;if so, we have a Stacker volume
notstk2:
        }
        }

    return (bResult);
}

#pragma optimize("", on)

#endif

typedef struct{
		DWORD 	mi_wVendor;
		WORD	mi_wVendorVersion;
		WORD	mi_wMRCIVersion;
		DWORD	mi_pfnOperate;
		WORD	mi_flCapability;
		WORD	mi_flHWAssist;
		WORD	mi_cbMax;
	} MRCIStruct;
#define sigOLD_CX   (0x4D52)    /* 'MR'                                 */
#define sigOLD_DX   (0x4349)    /* 'CI'                                 */

#define sigNEW_CX   (0x4943)    /* 'IC'                                 */
#define sigNEW_DX   (0x524D)    /* 'RM'                                 */


#pragma optimize ("", off)

BOOL SYM_EXPORT WINAPI DiskIsDriveSpace( BYTE drive )
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnDiskIsDriveSpace;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnDiskIsDriveSpace == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnDiskIsDriveSpace = GetProcAddress(hInstThunkDll,"DiskIsDriveSpace32" );
    	    if (lpfnDiskIsDriveSpace == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnDiskIsDriveSpace( drive ) );
#endif // _M_ALPHA
#else
	MRCIStruct	FAR *lpMRCIStruct;
#ifdef SYM_PROTMODE
	MRCIStruct	FAR *lpReal;
	auto	struct DPMIRec	dpmi;
	auto	UINT	wSelector;
#endif

	if ( !DiskIsDoubleSpace(drive) )
		return(FALSE);	// Can't be Drivespace.

#ifdef SYM_PROTMODE

	MEMSET(&dpmi, 0, sizeof(dpmi));
	dpmi.eax = 0x4a12;		// MRCI Query Function
	dpmi.ecx = sigOLD_CX;
	dpmi.edx = sigOLD_DX;
	dpmi.simulatedInt = 0x2F;

	CriticalSectionBegin();		// Must be in Critical Section

	DPMISimulateInt(&dpmi);

	CriticalSectionEnd();

	lpReal= MAKELP(dpmi.es, dpmi.edi);

	lpMRCIStruct=GetProtModePtr(lpReal);

	wSelector=FP_SEG(lpMRCIStruct);// Save off Selector

#else
	_asm
       	{
       	       /*-----------------05-04-94 01:24pm-----------------
       	         We will use the MRCIQuery function to determine
       	        the compression capibility of the MRCI server.  If
       	        it supports MRCI2, it's DRVSPACE.
       	       --------------------------------------------------*/
	       	push	ax
	       	push	bx
	       	push	cx
	       	push	dx
	       	push	si
	       	push	bp
	       	push	ds		;Save Regs

       	       	mov	ax, 4a12h	;MRCI Query function
       	       	mov	cx, sigOLD_CX	;'MR'
       	       	mov 	dx, sigOLD_DX	;'CI'
       	       	int	2fh

		cmp     cx, sigNEW_CX   ; proper signature on return?
        	jne     mq_no_MRCI
        	cmp     dx, sigNEW_DX
        	je      mq_got_MRCI

	mq_no_MRCI:
	        xor     di, di          ; NULL return
        	mov     es, di

	mq_got_MRCI:
        	mov     word ptr [lpMRCIStruct], di         ; save MRCINFO ptr for return
        	mov     word ptr [lpMRCIStruct+2], es

	       	pop	ds
	       	pop	bp
	       	pop	si
	       	pop	dx
	       	pop	cx
	       	pop	bx
	       	pop	ax		;Restore Regs
       	}
#endif

	if ( lpMRCIStruct != NULL )
		if( lpMRCIStruct->mi_flCapability & 0x00C0 )
       		{       // If true, we have MRCI 2 (DriveSpace).
#ifdef SYM_PROTMODE
			SelectorFree(wSelector);
#endif
			return (TRUE);
		}

#ifdef SYM_PROTMODE
	SelectorFree(wSelector);
#endif
	return (FALSE);

#endif  //SYM_WIN32
}

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/



/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskIsDoubleSpace( BYTE drive )
{

#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnDiskIsDoubleSpace;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnDiskIsDoubleSpace == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnDiskIsDoubleSpace = GetProcAddress(hInstThunkDll,"DiskIsDoubleSpace32" );
    	    if (lpfnDiskIsDoubleSpace == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnDiskIsDoubleSpace( drive ) );
#endif // _M_ALPHA
#else
   WORD bSwapped;
   BYTE byHost;
   BYTE bySequence;

   return _DiskIsDoubleSpace(drive, &byHost, &bSwapped, &bySequence);
#endif
}

/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI DiskIsCompressed( BYTE dl )
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	return FALSE;
#else	
    static FARPROC	lpfnDiskIsCompressed;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnDiskIsCompressed == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnDiskIsCompressed = GetProcAddress(hInstThunkDll,"DiskIsCompressed32" );
    	    if (lpfnDiskIsCompressed == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnDiskIsCompressed( dl ) );
#endif // _M_ALPHA
#else
    return (BOOL)( DiskIsStacker(dl) ||
                   DiskIsSuperStor(dl) ||
                   DiskIsDoubleSpace(dl) );
#endif
}

#endif // !SYM_OS2

/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#ifndef SYM_OS2
BOOL SYM_EXPORT WINAPI DiskHasBootRecord( BYTE dl )
{
    auto        UINT    wMajor, wMinor;

                                        // OS/2 won't let you access the
                                        // boot record for HPFS volumes
    if ( DiskIsHPFS(dl) )
        return (FALSE);

    DiskGetType(dl, &wMajor, &wMinor);

    return ((BOOL)!(wMajor == TYPE_UNKNOWN ||
                    wMajor == TYPE_DOESNT_EXIST ||
                    wMajor == TYPE_PHANTOM ||
                    wMajor == TYPE_REMOTE ||
                    wMajor == TYPE_CDROM));
}
#endif //   #ifndef SYM_OS2


/*----------------------------------------------------------------------*/
/*----------------------------------------------------------------------*/

#ifndef SYM_WIN32
#ifndef SYM_OS2
#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI DiskIsInterLink( BYTE drive )
{
    BOOL bIsInterLink = FALSE;

    drive -= 'A';
    _asm
    {
        mov     ax, 05601h
        mov     bh, drive
        mov     bl, 0
        mov     dx, 0FFFFh
        int     2fh
        cmp     AL, 0FFh
        jne     done
        mov     bIsInterLink,TRUE
done:
    }

    return( bIsInterLink );
}
#pragma optimize("", on)
#endif //   #ifndef SYM_OS2
#endif                                  // #ifndef SYM_WIN32



#ifndef SYM_OS2
/************************************************************************/
/*                                                                      */
/* Description:                                                         */
/*      This function checks if the given drive letter maps to a MS     */
/*      DMF (Distribution Media Format) drive.                          */
/*                                                                      */
/* Parameters:                                                          */
/*                                                                      */
/* Return values:                                                       */
/*      HIWORD = DMF Major Version                                      */
/*      LOWORD = DMF Minor Version                                      */
/*      0x00000000 if drive is not DMF                                  */
/*      0xFFFFFFFF if error                                             */
/* See Also:                                                            */
/*                                                                      */
/************************************************************************/
DWORD SYM_EXPORT WINAPI DiskIsDMF (BYTE dl)
{
	DWORD dwRet = 0xffffffff;
	DiskRec disk;
	LPVOID lpBuffer;

	if(!DiskHasBootRecord( dl ))
		return dwRet;

					// Fill in DiskRec so we can do a DiskBlockDevice()
        if(!DiskGetInfo(dl,&disk))
            return dwRet;

                                            // Doesn't need to be fixed, DiskBlock uses its own buffers...
        if((lpBuffer = MemAllocPtr(GHND,disk.bytesPerSector)) == NULL)
            return dwRet;

        disk.buffer = (HPBYTE)lpBuffer;
        disk.sector = 0;
        disk.sectorCount = 1;

                                        // Read boot record
        if(!DiskBlockDevice(BLOCK_READ,&disk))
	    {
            BPBBootRecord _far * br = (BPBBootRecord _far *)lpBuffer;

                                    // Does this look like a DMF disk?
            dwRet = 0;
            if(STRNCMP(&br->oemName[0],"MSDMF",5) == 0)
                {
                                            // Yes, let's extract the oem info
                LPSTR lpMinor,lpMajor = &br->oemName[5];
                if((*lpMajor != EOS) && (lpMinor = STRCHR(lpMajor,'.')) != NULL)
                    {
                    UINT wMajor,wMinor;
                    *lpMinor = EOS;
                    lpMinor++;
                                            // Conver DMF version to DWORD
                    ConvertStringToWord(&wMajor,lpMajor);
                    ConvertStringToWord(&wMinor,lpMinor);
                    dwRet = (DWORD)MAKELP((WORD)wMajor,(WORD)wMinor);
                    }
                }
            }

	MemFreePtr(lpBuffer);

	return dwRet;
}
#endif //   #ifndef SYM_OS2

#endif // SYM_VXD




/**************************************************************************/
/*IsValidNECHDNumber
 *PARAMS :  byDn is hard disk drive number.
 *RETURNS: True if this is a valid drive number on a NEC machine.
 **************************************************************************/
BOOL SYM_EXPORT WINAPI IsValidNECHDNumber(BYTE byDn )
{
    return ( (byDn >= 0   && byDn  <= 0x03) ||
             (byDn >= 0x20 && byDn <= 0x27) ||
             (byDn >= 0x80 && byDn <= 0x83) ||
             (byDn >= 0xA0 && byDn <= 0xA7) ||
             (byDn >= 0xC0 && byDn <= 0xC7));

}   /*IsValidNECHDNumber*/


/**************************************************************************/
/*IsValidNECFloppyNumber
 *PARAMS : byDn is Floppy disk drive number.
 *RETURNS: True if this is a valid drive number on a NEC machine.
 **************************************************************************/
BOOL SYM_EXPORT WINAPI IsValidNECFloppyNumber(BYTE byDn)
{
    return ( (byDn >= 0x10 && byDn <= 0x13) ||
             (byDn >= 0x30 && byDn <= 0x33) ||
             (byDn >= 0x90 && byDn <= 0x93) ||
             (byDn >= 0xB0 && byDn <= 0xB3) ||
             (byDn >= 0xF0 && byDn <= 0xF3));

}   /*IsValidNECFloppyNumber*/


#ifndef SYM_OS2
#ifndef _M_ALPHA
int LOCAL Chk144Drive(char drive)
{
    static BYTE floppysIndex[26] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    char    drv;
    WORD            flag;
    WORD            index;


    for (index = 0; index < 26; index ++)
        {
        if ( floppysIndex [index] == drive ) // Has this letter been used before?
            break ;                       // Yes. We've found it.

        if ( floppysIndex [index] == 0 )
            {  // Is this the end of the list?
            floppysIndex [index] = drive ;   // Yes. Add this letter to the list
            break ;
            }
	}
    if (index > 3)
        return 0;

    drv = (char )index;
    flag = 1;
    _asm
        {
        mov     al, drv
        or      al, 30h                         ;; for 1.44
        mov     ah, 0c4h
        int     1bh
        mov     al, ah
        and     al, 0f0h
        cmp     al, 40h
        je      no_144
        test    ah, 04h
        jnz     chk_done                        ;; is 1.44
no_144:
        mov     ax, 0
        mov     flag, ax
chk_done:
	}
	return(flag);
}
#endif // _M_ALPHA
#endif

/* END OF FILE */
/***************/

