/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/PRM_HARD.C_v   1.4   10 Aug 1997 15:23:02   bsobel  $ *
 *                                                                      *
 * Description:                                                         *
 *      CMOS I/O, Bus Detection, and Disk Cache and Disk Monitor        *
 *      control functions.                                              *
 *                                                                      *
 * Contains:                                                            *
 *      EISACMOSRead                                                    *
 *      EISACMOSWrite                                                   *
 *      CMOSRead                                                        *
 *      CMOSWrite                                                       *
 *      HWIsPCI                                                         *
 *      HWIsEISA                                                        *
 *      HWIsMicroChannel                                                *
 *      DiskMonitor                                                     *
 *      DiskCache                                                       *
 *      DiskEnableCache                                                 *
 *      DiskDisableCache                                                *
 *	DiskSuspendCache	                                    					*
 *      DiskCacheInstalled                                              *
 *      DiskCacheDumpWrites                                             *
 *      DiskGetCacheSize                                                *
 *      DiskEraseProtect                                                *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/PRM_HARD.C_v  $ *
// 
//    Rev 1.4   10 Aug 1997 15:23:02   bsobel
// Changed to dynamically load thunk dll on win32 only when accessed for the first time.
// 
//    Rev 1.3   19 Mar 1997 21:39:04   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.2   02 Sep 1996 16:35:54   JALLEE
// HWHasCMOS() : Only restore DOS date/time if there is reason to believe we
// changed it. We were losing time, and even going backwards on some systems.
// 
//    Rev 1.1   18 Apr 1996 11:30:34   MBARNES
// Ensured that DS was correctly loaded in DiskIsSmartDriveInstalled()
// 
//    Rev 1.0   26 Jan 1996 20:22:16   JREARDON
// Initial revision.
// 
//    Rev 1.50   14 Dec 1995 20:48:08   RANSCHU
// Enabled and implemented PCI bus detection on NEC hardware.
// 
//    Rev 1.49   02 Aug 1995 00:52:40   AWELCH
// Merge changes from Quake 7.
// 
//    Rev 1.48   02 Jul 1995 16:04:30   BILL
// Copied branch version to trunk (trunk hadn't changed yet).
// 
//    Rev 1.47.1.1   02 Jul 1995 15:34:46   BILL
// DiskUnderSymEvent returns NULL under Windows 16 ver 3.5+ or under Windows 32
// Symevent uses a different hooking technique with the IFS under Chicago and
// this call doesn't need to load the symevent dll just so it can fail the
// call to symevent (per LC).
// 
//    Rev 1.47.1.0   27 Jun 1995 18:49:24   RJACKSO
// Branch base for version QAK8
// 
//    Rev 1.47   31 May 1995 13:07:38   REESE
// Made HWIsPCI more robust.
// 
//    Rev 1.46   30 May 1995 17:37:14   REESE
// Added HWIsPCI routine.
// 
//    Rev 1.45   07 Mar 1995 16:42:34   ARTW
// Add SetErrorMode around LoadLibrary of SYMEVNT (WIN16 only).
// 
//    Rev 1.44   24 Feb 1995 16:25:30   BILL
// Turned off optimizations around hwiseisa
// 
//    Rev 1.43   10 Feb 1995 18:54:04   BILL
// 
//    Rev 1.42.1.1   10 Feb 1995 18:53:00   BILL
// Rewrote EISA detection routine
// 
//    Rev 1.42.1.0   13 Jan 1995 20:16:14   MPYE
// Branch base for version QAK6
// 
//    Rev 1.42   04 Oct 1994 15:02:44   BRAD
// Incorporated NEC support
// 
//    Rev 1.41   23 Jun 1994 17:22:20   HENRI
// Moved 16 bit thunk layers to a separate DLL
// 
//    Rev 1.40   22 Jun 1994 12:25:38   HENRI
// Added thunking layers
// 
//    Rev 1.39   10 May 1994 16:07:20   MEIJEN
// Add EISACMOSRead, EISACMOSWrite for RESCUE future use
// 
//    Rev 1.38   05 May 1994 13:00:46   BRAD
// Fixed typo
// 
//    Rev 1.37   05 May 1994 12:47:52   BRAD
// DX support
// 
//    Rev 1.36   30 Mar 1994 16:10:12   BRAD
// Added Thunk stuff.
// 
//    Rev 1.35   26 Mar 1994 21:43:28   BRAD
// Use portable messages
// 
//    Rev 1.34   20 Mar 1994 18:29:18   BRAD
// Cleaned up some SYM_WIN32 issues
// 
//    Rev 1.33   15 Mar 1994 12:33:52   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
// 
//    Rev 1.32   25 Feb 1994 15:03:42   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
// 
//    Rev 1.30   23 Feb 1994 20:46:10   JOHN
// Changed DiskCacheSetWriteThru() to check for both SmartDrive and NCACHE.
// 
// NOTE:  The DiskIsSmartDriveInstalled() function returns FALSE when
// NCACHE is loaded, even though NCACHE emulates most of the SmartDrive
// API's.  This allows us to find out which cache is really loaded.
// Code that can work with either must test for both of them explicitly.
// 
//    Rev 1.29   17 Feb 1994 09:13:38   MARKK
// Changed DiskIsVFATInstalled to check if any VFAT volumes are mounted.
// 
//    Rev 1.28   14 Feb 1994 14:05:42   JOHN
// Added DiskGetUnderSymEvent().
// 
//    Rev 1.27   08 Feb 1994 19:30:28   MARKK
// Convert drive to upper case before sub. 'A'
// 
//    Rev 1.26   08 Feb 1994 16:34:44   MARKK
// Added DiskIsVFAT
// 
//    Rev 1.25   11 Jan 1994 16:40:10   JOHN
// Protected DiskGetCacheHits() within an "#ifdef SYM_WIN" block.
// I don't feel like writing a DOS version of this function.
// 
//    Rev 1.24   10 Jan 1994 19:19:14   JOHN
// Added NCACHE support to DiskGetCacheHits().
// 
//    Rev 1.23   07 Jan 1994 21:34:34   JOHN
// (1) Removed unreferenced local function SmartdrvInstalled().
// (2) Added new public function DiskGetCacheHits().  Right now it only
//     works for SmartDrive, but I expect to get the NCACHE code shortly.
// (3) Fixed DiskCacheSmartDrive(TSR_INSTALLED) so it actually works.
// 
//    Rev 1.22   27 Nov 1993 17:41:06   JOHN
// Changed from using Windows-specific AnsiUpperBuf to generic CharToUpper.
// 
//    Rev 1.21   25 Oct 1993 09:31:50   MARKK
// Added Cache write thru routines
// 
//    Rev 1.20   15 Oct 1993 16:57:28   BARRY
// Use ifdef SYM_WIN on DiskCheckForVFAT()
// 
//    Rev 1.19   15 Oct 1993 15:42:12   ED
// unreferenced varaible
// 
//    Rev 1.18   15 Oct 1993 15:36:34   ED
// Added DiskCheckForVFAT
// 
//    Rev 1.17   20 Sep 1993 14:48:50   HENRI
// Rename Disk32BitAccessedOn to DiskIsVFATInstalled
// 
//    Rev 1.16   15 Sep 1993 16:36:04   HENRI
// Added Disk32BitAccessOn
// 
//    Rev 1.15   25 Aug 1993 16:28:38   MARKK
// Use DPMISimulateInt in DiskGetUnderSmartDrive instead of inline
// assembly since for some reason under windows it was returning with
// ES = 0.
// 
//    Rev 1.14   02 Jul 1993 08:52:10   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the 
// C8 dos.h include file and platform.h wouldn't redefine it and cause 
// warnings.
// 
//    Rev 1.13   16 Mar 1993 15:50:28   JOHN
// Added DiskGetUnderSmartDrive().
// 
//    Rev 1.12   17 Feb 1993 22:34:16   BRAD
// Registers SI and DI weren't being preserved in many
// ASM routines.
// 
//    Rev 1.11   12 Feb 1993 04:26:22   ENRIQUE
// No change.
// 
//    Rev 1.10   08 Jan 1993 19:16:58   JOHN
// Only test once for a given disk caches being present.
// After that, just return the result of the orginal test.
// 
//    Rev 1.9   25 Dec 1992 14:32:56   HENRI
// Turned off global optimization around inline assembly.
// 
//    Rev 1.8   09 Dec 1992 19:33:40   HENRI
// Fixed another stack thrasher.
// 
//    Rev 1.7   09 Dec 1992 19:20:12   HENRI
// Fixed stack fault bug where SmartDrvSuspend would 
// trash the stack.
// 
//    Rev 1.6   07 Dec 1992 12:41:36   JOHN
// Fixed the SmartDrive suspend/resume functions so they actually work.
// They must have been broken in the Norton Library to Quake convesion.
// 
//    Rev 1.5   17 Nov 1992 13:21:28   LindaD
// Changed some jmp $+2 instructions to jmp short $+2 instructions
// to get rid of a crash occuring due to conversion from Medium to 
// Large model.
// 
//    Rev 1.4   21 Sep 1992 12:40:36   MARKK
// Fixed trashed bp in smartdrivesuspend
// 
//    Rev 1.3   16 Sep 1992 15:50:52   HENRI
// Removed a semicolon on an else if statement that
// cause an erroneous flow of control bug.
// 
//    Rev 1.2   16 Sep 1992 11:15:04   HENRI
// Added DiskCacheSuspend & DiskCacheResume
// 
//    Rev 1.1   10 Sep 1992 18:39:00   BRUCE
// Eliminated unnecesary includes
// 
//    Rev 1.0   26 Aug 1992 17:00:46   ED
// Initial revision.
 ************************************************************************/

#include <dos.h>  
#include "platform.h"
#include "undoc.h"
#include "disk.h"
#include "xapi.h"
#include "symevent.h"
#include "dialogs.h"
#include "thunk.h"
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

/**********************************************************************
** Local constants
** (no one said in-line assembly was perfect)
**********************************************************************/

#define SigCX	'C' * 256 + 'X'		// 'CX'
#define Sigcx	'c' * 256 + 'x'		// 'cx'

#define SigCF	'C' * 256 + 'F'		// 'CF'
#define Sigcf	'c' * 256 + 'f'		// 'cf'

#define SigCS	'C' * 256 + 'S'		// 'CS'
#define Sigcs	'c' * 256 + 's'		// 'cs'

#define SigNU	'N' * 256 + 'U'		// 'NU'
#define SigDM	'D' * 256 + 'M'		// 'DM'
#define Sigdm	'd' * 256 + 'm'		// 'dm'

#define SigFS   'F' * 256 + 'S'         // 'FS'
#define Sigfs   'f' * 256 + 's'         // 'fs'

#define MULTI_SMARTDRV                  0x4A10
#define SD_SIGNATURE		        0xBABE
#define SD_GET_STATUS		        0
#define SD_COMMIT_ALL		        1
#define SD_REINITIALIZE		        2
#define SD_CACHE_DRIVE		        3
#define CACHE_DRIVE_GET		        0
#define CACHE_DRIVE_READ_ENABLE	        1
#define CACHE_DRIVE_READ_DISABLE	2
#define CACHE_DRIVE_WRITE_ENABLE	3
#define CACHE_DRIVE_WRITE_DISABLE	4
#define SD_GET_INFO                     4
#define SD_GET_BUFFER_INFO	        5
#define SD_DONT_CACHE_DRIVE	        6
#define SD_GET_REAL_DRIVER_ADDR		7

#define TSR_NOT_PRESENT         	0
#define TSR_IS_DISABLED         	1
#define TSR_IS_ENABLED          	2


/**********************************************************************
** Local data
**********************************************************************/

static  BYTE    smartDriveInstalled = FALSE;
static  BYTE    smartDriveSuspended = 0;
static  BYTE    smartDriveStatus[26];	// SmartDrive per-drive cache status

static	WORD	wNCacheID;		// NCACHE TSR ID

/**********************************************************************
** Local function prototypes
**********************************************************************/

BYTE LOCAL PASCAL CacheGetCurrentConfig(WORD byDrive);
void LOCAL PASCAL SetWriteThru(WORD byDrive);
void LOCAL PASCAL ResetWriteThru(WORD byDrive);
void LOCAL PASCAL CacheCommit(BYTE byDrive);
UINT LOCAL PASCAL DiskCacheFlash (UINT wCommand);
UINT LOCAL PASCAL DiskCachePCKwik (UINT wCommand);
UINT LOCAL PASCAL DiskCacheNorton (UINT wCommand);
UINT LOCAL PASCAL DiskCacheSmartDrive (UINT wCommand);
BYTE LOCAL PASCAL DiskIsSmartDriveInstalled (void);
void LOCAL PASCAL DiskFlushSmartDrive (void);
void LOCAL PASCAL DiskSuspendSmartDrive (void);
void LOCAL PASCAL DiskResumeSmartDrive (void);

UINT LOCAL PASCAL GetTSRStatus (UINT wID, UINT wMatchID);
void LOCAL PASCAL EnableTSR (UINT wID);
void LOCAL PASCAL DisableTSR (UINT wID);
void LOCAL PASCAL ResetTSR (UINT wID);
void LOCAL PASCAL SuspendTSR (UINT wID);
void LOCAL PASCAL FlushTSR (UINT wID);

#pragma optimize("leg", off)    /* Can't global optimize inline assembly */



/*----------------------------------------------------------------------*/
/* EISACMOSRead                                  								*/
/*    BYTE    slot     : the slot number                                */
/*    BYTE    function : the function number                            */
/*    LPBYTE  address  : the address of buffer where EISA CMOS info is  */
/*                       stored, will read 320 bytes                    */
/*                                                                      */
/* This function will return 320 bytes of data about the specified EISA */
/* function in the specified slot.                                      */
/*                                                                      */
/* Return :                                                             */
/*    00h                    :     Successful completion                */
/*    EISA_ERR_SLOT_INVALID (80h): Invalid slot number                  */
/*    EISA_ERR_FUNC_INVALID (81h): Invalid func number                  */
/*    EISA_ERR_RAM_CORRUPT  (82h): Extended CMOS RAM corrupted          */
/*    EISA_ERR_SLOT_EMPTY   (83h): Empty slot specified                 */
/*    EISA_ERR_BIOS_INVALID (86h): Invalid BIOS routine call            */
/*    EISA_ERR_CFG_INVALID  (87h): Invalid system configuration         */
/*                                                                      */
/*  Ref :                                                               */
/*    System BIOS for IBM PCs, Compatibles, and EISA COmputers          */
/*    Second Edition by PHOENIX TECHNOLOGIES LTD.                       */
/*----------------------------------------------------------------------*/

BYTE SYM_EXPORT WINAPI EISACMOSRead(BYTE     slot,
                                    BYTE     funct,
                                    LPBYTE   address)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnEISACMOSRead;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnEISACMOSRead == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnEISACMOSRead = GetProcAddress(hInstThunkDll,"EISACMOSRead32" );
    	    if (lpfnEISACMOSRead == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnEISACMOSRead(slot, funct, address) );
#endif // _M_ALPHA
#else
    union       REGS            regs;
    struct      SREGS           sregs;
    auto        BYTE            returnValue;

                                        /*------------------------------*/
                                        /* fill up register for         */
                                        /* interrupt                    */
                                        /*------------------------------*/
    regs.x.ax = 0xD801;
    regs.h.ch = funct;
    regs.h.cl = slot;
    regs.x.si = FP_OFF(address);
    sregs.ds  = FP_SEG(address);

                                        /*------------------------------*/
                                        /* Call the interrupt           */
                                        /*------------------------------*/
    Int(0x15, &regs, &sregs);

                                        /*------------------------------*/
                                        /* store return value           */
                                        /*------------------------------*/
    returnValue = regs.h.ah;

    return(returnValue);
#endif
}


/*----------------------------------------------------------------------*/
/* EISACMOSWrite                                  								*/
/*    WORD    length   : length of data                                 */
/*    LPBYTE  address  : the address of buffer where EISA CMOS ram table*/
/*                       is stored                                      */
/*                                                                      */
/* This function will write configuration data from a table pointed by  */
/* DS:SI to CMOS RAM. This function must be called sequentially for     */
/* each slot, even for empty slot. To write configuration information   */
/* for an empty slot, the calling program must call this function       */
/* with a pointer to a data table filled with zero                      */
/*                                                                      */
/* Return :                                                             */
/*    00h                    :     Successful completion                */
/*    EISA_ERR_SLOT_INVALID (80h): Invalid slot number                  */
/*    EISA_ERR_WRITE_CMOS   (84h): Error writing to extended CMOS RAM   */
/*    EISA_ERR_CMOS_FULL    (85h): CMOS RAM is full
/*    EISA_ERR_BIOS_INVALID (86h): Invalid BIOS routine call            */
/*    EISA_ERR_CFG_INVALID  (87h): Invalid system configuration         */
/*                                                                      */
/*  Ref :                                                               */
/*    System BIOS for IBM PCs, Compatibles, and EISA COmputers          */
/*    Second Edition by PHOENIX TECHNOLOGIES LTD.                       */
/*----------------------------------------------------------------------*/

BYTE SYM_EXPORT WINAPI EISACMOSWrite(WORD     length,
                                     LPBYTE   address)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnEISACMOSWrite;
    extern HINSTANCE    hInstThunkDll;

    if (lpfnEISACMOSWrite == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnEISACMOSWrite = GetProcAddress(hInstThunkDll,"EISACMOSWrite32" );
    	    if (lpfnEISACMOSWrite == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnEISACMOSWrite(length, address) );
#endif _M_ALPHA
#else
    union       REGS            regs;
    struct      SREGS           sregs;
    auto        BYTE            returnValue;

                                        /*------------------------------*/
                                        /* fill up register for         */
                                        /* interrupt                    */
                                        /*------------------------------*/
    regs.x.ax = 0xD803;
    regs.x.cx = length;
    regs.x.si = FP_OFF(address);
    sregs.ds  = FP_SEG(address);

                                        /*------------------------------*/
                                        /* Call the interrupt           */
                                        /*------------------------------*/
    Int(0x15, &regs, &sregs);

                                        /*------------------------------*/
                                        /* store return value           */
                                        /*------------------------------*/
    returnValue = regs.h.ah;

    return(returnValue);
#endif
}


/*----------------------------------------------------------------------*/
/* CMOSRead								*/
/*	Read the CMOS value at address 'address'.			*/
/*----------------------------------------------------------------------*/

BYTE SYM_EXPORT WINAPI CMOSRead(BYTE address)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnCMOSRead;
    extern HINSTANCE    hInstThunkDll;


    if (HWIsNEC())
        return(FALSE);

    if (lpfnCMOSRead == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnCMOSRead = GetProcAddress(hInstThunkDll,"CMOSRead32" );
    	    if (lpfnCMOSRead == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnCMOSRead(address) );
#endif // _M_ALPHA
#else
    auto	BYTE	returnValue;

    if (HWIsNEC())
        return(FALSE);

    _asm
	{
	cli				; Disable interrupts
	mov	al, address		; Get address
	out	70h, al			; 
	jmp	short $+2       	; Delay...
	in	al, 71h			; Read CMOS
	sti				; Enable interrupts
	mov	returnValue, al		; Get the return value
	}

    return(returnValue);
#endif
}



/*----------------------------------------------------------------------*/
/* CMOSWrite								*/
/*	Writes 'data' to CMOS address 'address'.			*/
/*----------------------------------------------------------------------*/

VOID SYM_EXPORT WINAPI CMOSWrite(BYTE address, BYTE CMOSdata)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
#else	
    static FARPROC	lpfnCMOSWrite;
    extern HINSTANCE    hInstThunkDll;

    if (HWIsNEC())
        return;

    if (lpfnCMOSWrite == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnCMOSWrite = GetProcAddress(hInstThunkDll,"CMOSWrite32" );
    	    if (lpfnCMOSWrite == NULL)
				return;
		    }
		else
		    return;
	}

    lpfnCMOSWrite(address, CMOSdata);
#endif // _M_ALPHA
#else
    if (HWIsNEC())
        return;

    _asm
	{
	cli				; Disable interrupts
	mov	al, address		; Get address
	out	70h, al			; 
	jmp	short $+2 		; Delay...
	mov	al, CMOSdata		; Get data
	out	71h, al			; Set it
	sti				; Enable interrupts
	}
#endif
}

/*----------------------------------------------------------------------*/
/* HWIsPCI								*/
/*	Returns:							*/
/*	    TRUE	Computer has PCI.				*/
/*	    FALSE	PCI not installed.				*/
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI HWIsPCI(VOID)
{
#if defined(SYM_WIN32) /*[*/
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnHWIsPCI = NULL;
    extern HINSTANCE    hInstThunkDll;

    if(lpfnHWIsPCI == NULL)
	{ /* Get thunk information */
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
			lpfnHWIsPCI = GetProcAddress(hInstThunkDll, "HWIsPCI32" );
			
		if(lpfnHWIsPCI == NULL)
		    return(FALSE);
	}

    return(lpfnHWIsPCI());
#endif // _M_ALPHA    
#else /*][*/
    BOOL returnValue = FALSE;
    BYTE byNECFlag = HWIsNEC() ? TRUE : FALSE;

    _asm
	{
	xor dx,dx	; Successful call will set this to 0x4350
	mov al,0x01	; PCI BIOS installation Check
			; This function may require up to 1024 bytes of stack.
			; It will not enable interrupts if they were disabled
			; before making the call.
			; Copied from Ralf Brown's Interrupt List
	test byNECFlag,0
	stc		; Successful call will clear this
	jnz IsNEC

	mov ah,0xB1	; PCI BIOS function
	int 0x1A
	jmp EvaluatePCI

IsNEC:
	mov ah,0xCC	; PCI BIOS function
	int 0x1F

EvaluatePCI:
	mov bx,FALSE	; Indicate failure to detect PCI bus extensions

	jc notPCI	; A supported call should clear the carry flag

	cmp dx,0x4350	; First two letters of 'PCI ' which is in EDX on 386s
	jnz notPCI

	or ah,ah	; Will be zero if successful
	jnz notPCI

	; Passed all of the tests!
	mov bx,TRUE	; Indicate success

notPCI:
	mov	returnValue, bx
	}
	SYM_ASSERT(sizeof(returnValue) == 2);	/* Stored BX into here */

    return(returnValue);
#endif /*]*/
}
#pragma optimize("", on)

/*----------------------------------------------------------------------*/
/* HWIsEISA								*/
/*	Returns:							*/
/*	    TRUE	Computer has EISA.				*/
/*	    FALSE	EISA not installed.				*/
/*----------------------------------------------------------------------*/

#pragma optimize("", off)
BOOL SYM_EXPORT WINAPI HWIsEISA(VOID)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnHWIsEISA;
    extern HINSTANCE    hInstThunkDll;

    if (HWIsNEC())
        return(FALSE);

    if (lpfnHWIsEISA == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnHWIsEISA = GetProcAddress(hInstThunkDll, "HWIsEISA32" );
    	    if (lpfnHWIsEISA == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnHWIsEISA() );
#endif // _M_ALPHA
#else
    auto    BOOL    returnValue = FALSE;

    if (HWIsNEC())
        return(FALSE);

    // Modified WES to use D804 instead of D800 because of Win95
    // int 15 bug.

    _asm
	{
    mov ax,0D804h   ; EISA BIOS INT: read physical slot
    xor     cl,cl   ; CL = 0 (system board)
    int 15h         ; System services

    jc  notEISA     ; if carry set, then error and may not be EISA
    or  ah,ah       ; if (ah != 0)
    jnz locRet      ;     then not EISA
    jmp short isEISA

notEISA:
    cmp ah, 0x80    ; invalid slot number? (is EISA tho!)
    je isEISA       
    cmp ah, 0x83    ; empty slot (is EISA tho!)
    je isEISA
    xor ax,ax       ; not EISA for sure
    jmp short locRet

isEISA:
    mov ax, TRUE    ; Return TRUE
    
locRet:     
        mov	returnValue, ax
	}

    return(returnValue);
#endif
}
#pragma optimize("", on)

/*----------------------------------------------------------------------*/
/* HWIsMicroChannel							*/
/*	Returns:							*/
/*	    TRUE	Computer has MicroChannel.			*/
/*	    FALSE	MCA not installed.				*/
/*----------------------------------------------------------------------*/

#define		FEATURES_BYTE		5

BOOL SYM_EXPORT WINAPI HWIsMicroChannel(VOID)
{
#if defined(SYM_WIN32)
#ifdef _M_ALPHA
	SYM_ASSERT(FALSE);
	return FALSE;
#else	
    static FARPROC	lpfnHWIsMicroChannel;
    extern HINSTANCE    hInstThunkDll;

    if (HWIsNEC())
        return(FALSE);

    if (lpfnHWIsMicroChannel == NULL)
	{
		if ((hInstThunkDll == NULL) && (SystemGetWindowsType() == SYM_SYSTEM_WIN_95))
		    {
	        SYM_OUTPUT_DEBUG("Loading " QMODULE_SYMKRNLTHUNK ".DLL\n");
	        hInstThunkDll = LoadLibrary( QMODULE_SYMKRNLTHUNK ".DLL" );
	        }
	    if (hInstThunkDll > (HINSTANCE)HINSTANCE_ERROR)
		    {
		    lpfnHWIsMicroChannel = GetProcAddress(hInstThunkDll,"HWIsMicroChannel32" );
    	    if (lpfnHWIsMicroChannel == NULL)
				return FALSE;
		    }
		else
		    return FALSE;
	}

    return( lpfnHWIsMicroChannel() );
#endif // _M_ALPHA
#else
    auto	BOOL	returnValue;


    if (HWIsNEC())
        return(FALSE);

    _asm
	{
	push	ds			; Save registers
	push	es
	push	si
	push	di

	mov	ax,0C000h		; Return System configuration
	int	15h			; System services
	sti				; This call disables INTs upon returning
					;	on Compaq '386 computers
	jc	notMCA			; if (CF == 1) then not an MCA
        or	ah,ah			; 
	jnz	notMCA
	add	bx,FEATURES_BYTE
	mov	ax,es
	mov	ds,ax
	mov 	al, Byte Ptr [bx]
	test	al,02h	
	jz	notMCA
	shr	ax,1
        jmp     short locRet	

notMCA:
	xor	ax,ax			; Not Micro Channel, return FALSE

locRet:     
        mov	returnValue, ax		; Get the return value

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
	}

    return(returnValue);
#endif
}


#if !defined(SYM_WIN32)

static BYTE abyDriveCounts[26];
static BYTE abyDriveOriginal[26];

/************************************************************************
 * Will set smartdrive or ncache to write thru on the specified drive	*
 ************************************************************************/

VOID SYM_EXPORT WINAPI DiskCacheSetWriteThru(BYTE byDrive)
{
    if (!DiskCacheNorton(TSR_INSTALLED) && !DiskIsSmartDriveInstalled())
	return;

    byDrive = CharToUpper(byDrive) - 'A';

    if (abyDriveCounts[byDrive]++)
	return;
    
    abyDriveOriginal[byDrive] = CacheGetCurrentConfig((WORD)byDrive);
    if (abyDriveOriginal[byDrive] & 0x40)
	return;

    SetWriteThru((WORD)byDrive);
    CacheCommit(byDrive);
}

/************************************************************************
 * Will put the drive back into the mode it was in prior to the call	*
 * above.								*
 ************************************************************************/

VOID SYM_EXPORT WINAPI DiskCacheResetWriteThru(BYTE byDrive)
{
    byDrive = CharToUpper(byDrive) - 'A';
    
    if (abyDriveCounts[byDrive] == 0 || --abyDriveCounts[byDrive])
	return;
    
    if (! (abyDriveOriginal[byDrive] & 0x40))
	ResetWriteThru(byDrive);
}

#pragma optimize("leg", off)

BYTE LOCAL PASCAL CacheGetCurrentConfig(WORD byDrive)
{
    auto	BYTE		byRet;
    
    _asm
	{
	push	si
	push	di
	push	ds
	push	bp
	
	mov	ax, MULTI_SMARTDRV
	mov	bx, SD_CACHE_DRIVE
	xor	dl, dl
    	mov	bp, byDrive
	int	2fH
	
	pop	bp
	pop	ds
	pop	di
	pop	si
	
	mov	byRet, dl
	}
    
    return (byRet);
}
	
void LOCAL PASCAL SetWriteThru(WORD byDrive)
{
    
    _asm
	{
	push	si
	push	di
	push	ds
	push	bp
	
	mov	ax, MULTI_SMARTDRV
	mov	bx, SD_CACHE_DRIVE
	mov	dl, 1
    	mov	bp, byDrive
	int	2fH
	
	pop	bp
	push	bp
    
	mov	ax, MULTI_SMARTDRV
	mov	bx, SD_CACHE_DRIVE
	mov	dl, 4
    	mov	bp, byDrive
	int	2fH
	
	pop	bp
	pop	ds
	pop	di
	pop	si
	}
}

void LOCAL PASCAL ResetWriteThru(WORD byDrive)
{
    
    _asm
	{
	push	si
	push	di
	push	ds
	push	bp
	
	mov	ax, MULTI_SMARTDRV
	mov	bx, SD_CACHE_DRIVE
	mov	dl, 1
    	mov	bp, byDrive
	int	2fH
	
	pop	bp
	push	bp
    
	mov	ax, MULTI_SMARTDRV
	mov	bx, SD_CACHE_DRIVE
	mov	dl, 3
    	mov	bp, byDrive
	int	2fH
	
	pop	bp
	pop	ds
	pop	di
	pop	si
	}
}

void LOCAL PASCAL CacheCommit(BYTE byDrive)
{
  
    _asm
	{
	push	si
	push	di
	push	ds
	push	bp
	
	mov	ax, MULTI_SMARTDRV
	mov	bx, SD_COMMIT_ALL
	int	2fH
	
	pop	bp
	pop	ds
	pop	di
	pop	si
	}
}

/**********************************************************************
** DiskEnableCache()    	Enable disk cache software                                       
** DiskDisableCache()           Disable disk cache software
** DiskCacheInstalled()         Test for presence of any cache software                                   
**                                                      
** Returns:
**   1			if any cache is present
**   0			otherwise
**********************************************************************/

UINT SYM_EXPORT WINAPI DiskEnableCache(void)     { return DiskCache(TSR_ENABLE);  }
UINT SYM_EXPORT WINAPI DiskDisableCache(void)    { return DiskCache(TSR_DISABLE); }
UINT SYM_EXPORT WINAPI DiskCacheInstalled(void)  { return DiskCache(TSR_INSTALLED); }
UINT SYM_EXPORT WINAPI DiskCacheDumpWrites(void) { return DiskCache(TSR_FLUSH);    }
UINT SYM_EXPORT WINAPI DiskCacheSuspend(void)    { return DiskCache(TSR_SUSPEND);   }
UINT SYM_EXPORT WINAPI DiskCacheResume(void)     { return DiskCache(TSR_RESUME);   }

/**********************************************************************
** DiskCache()                                            
**                                                      
** Description:                                         
**   Test for, enable, or disable many types of disk cache software
**
** Input:
**   wCommand is one of the following:
**	TSR_DISABLE	to disable cache, if present
**	TSR_ENABLE	to enable cache, if present
**	TSR_INSTALLED	to test for cache presence only 
**	TSR_FLUSH       to dump cached writes without resetting the cache
**      TSR_SUSPEND     to suspend cache without dumping or resetting buffers
**      TSR_RESUME      to resume cache
**
** Returns:
**   1			if any cache is present
**   0			otherwise
**********************************************************************/

UINT SYM_EXPORT WINAPI DiskCache (UINT wCommand)
{
                                        // Do a DOS "Disk Reset" to flush 
                                        // caches we don't know about
                                        // (will also flush SmartDrive 4.0,
                                        // which we'll do again later)
    if (wCommand == TSR_DISABLE)
        {
        _asm    mov	ah,0DH
	_asm    int	21H
        }

    return (DiskCacheFlash(wCommand)   ||
	    DiskCachePCKwik(wCommand)  ||
	    DiskCacheNorton(wCommand)  || 
	    DiskCacheSmartDrive(wCommand));
}

/**********************************************************************
** DiskCacheFlash()                                            
**                                                      
** Description:                                         
**   Test for, enable, or disable FLASH cache
**
** Input:
**   wCommand is one of the following:
**	TSR_DISABLE	to disable cache, if present
**	TSR_ENABLE	to enable cache, if present
**	TSR_INSTALLED	to test for cache presence only 
**
** Returns:
**   1			if FLASH cache is present
**   0			otherwise
**********************************************************************/

UINT LOCAL PASCAL DiskCacheFlash (UINT wCommand)
{
    static	BOOL	bTested = FALSE;
    static	UINT	wPresent = 0;

    // Just return if we've already determined that Flash isn't installed
    if (bTested && !wPresent)
	return(wPresent);

    if (wCommand == TSR_FLUSH)         // sort of a bizarre solution!
        {
        DiskCacheFlash (TSR_DISABLE);
        return (DiskCacheFlash (TSR_ENABLE));
        }

    _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di
				   
	mov	ax,8369h		; Test for FLASH presence
	int	21h
	cmp	ax,6969h
	jne	EndCache

	mov	wPresent,1		; Present, so set return value

	mov	ax,wCommand		; Is wCommand == TSR_ENABLE?
	cmp	ax,TSR_ENABLE
	jnz	TestDisable
	
	mov	ax,8569h		; Enable FLASH cache
	int	21h
	jmp	EndCache

TestDisable:

	cmp	ax,TSR_DISABLE		; Is wCommand == TSR_DISABLE?
	jnz	EndCache

	mov	ax,8469h		; Disable FLASH cache
	int	21h			

EndCache:

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
	}

    bTested = TRUE;

    return (wPresent);
}

/**********************************************************************
** DiskCachePCKwik()                                            
**                                                      
** Description:                                         
**   Test for, enable, or disable PC-Kwik cache (Version 3.1+)
**
** Input:
**   wCommand is one of the following:
**	TSR_DISABLE	to disable cache, if present
**	TSR_ENABLE	to enable cache, if present
**	TSR_INSTALLED	to test for cache presence only 
**
** Returns:
**   1			if PC-Kwik cache is present
**   0			otherwise
**********************************************************************/

UINT LOCAL PASCAL DiskCachePCKwik (UINT wCommand)
{
    static	BOOL	bTested = FALSE;
    static	UINT	wPresent = 0;


    if (HWIsNEC())
        return(FALSE);

    // Just return if we've already determined that PC-KWIK isn't installed
    if (bTested && !wPresent)
	return(wPresent);

    if (wCommand == TSR_FLUSH)         // sort of a bizarre solution!
        {
        DiskCachePCKwik(TSR_DISABLE);
        return (DiskCachePCKwik(TSR_ENABLE));
        }

    if (!wPresent)
	{
	bTested = TRUE;

	// Determine if PC-KWIK is loaded
	_asm
	{
	push	ds			; Save registers
	push	es
	push	si
	push	di

	mov	ah,2Ah			; Save current date on stack
	int	21h			
	push	ax		       
	push	cx			
	push	dx

						; Test for PC-Kwik presence:

	mov	ah, 2Bh			; DOS Set current date
	mov	cx, SigCX		; PC-Kwik signature
	xor	dx, dx			; Invalid month/day
	int	21h			; Is PC-Kwik installed?
	jc	NotPresent		; No, return

	test	al, al			; Test result register
	jnz	NotPresent		; No PC-Kwik

	cmp	cx, Sigcx		; Signature okay?
	jne	NotPresent		; No PC-Kwik

	cmp	dx, 3 * 256 + 10	; PC-Kwik Version 3.10+?
	jb	NotPresent		; No PC-Kwik

	mov	wPresent,1		; Present, so set return value

NotPresent:
	pop	dx			; Restore current date from stack
	pop	cx
	pop	ax
	mov	ah, 2Bh			; Set date
	int	21h  

	pop	di 			; Restore registers
	pop	si
	pop	es
	pop	ds
	}
	}

    if (wPresent)
	_asm
	{
	push	ds			; Save registers
	push	es
	push	si
	push	di

	mov	ax,wCommand		; Is wCommand == TSR_ENABLE?
	cmp	ax,TSR_ENABLE
	jnz	TestDisable
	
	mov	ah, 0A4h		; Enable PC-Kwik function number
	mov	si, SigCX		; PC-Kwik signature
	int	13h			; Enable caching
	jmp	EndCache

TestDisable:

	cmp	ax,TSR_DISABLE		; wCommand == TSR_DISABLE?
	jnz	EndCache

	mov	ah, 0A3h		; Disable PC-Kwik function number
	mov	si, SigCX		; PC-Kwik signature
	int	13h			; Disable caching

EndCache:

	pop	di 			; Restore registers
	pop	si
	pop	es
	pop	ds
	}

    return (wPresent);
}

#pragma optimize("", on)

/**********************************************************************
** DiskCacheNorton()                                            
**                                                      
** Description:                                         
**   Query and Control the Norton Cache TSR.
**
** Input:
**      TSR_INSTALLED   Check for Installation
**      TSR_ENABLE      Enable the cache
**      TSR_DISABLE     Disable the cache
**      TSR_FLUSH       Dump cached writes
**      TSR_SUSPEND     Suspend cache
**      TSR_RESUME      Resume cache
**
** Returns:
**      TSR_NOT_INSTALLED
**      TSR_IS_ENABLED
**      TSR_IS_DISABLED
**
**********************************************************************/

UINT LOCAL PASCAL DiskCacheNorton (UINT wCommand)
{
    static	BOOL	bTested = FALSE;
    auto	UINT	wStatus;

    // Just return if we've already determined that NCACHE isn't installed
    if (bTested && !wNCacheID)
	return(wNCacheID);

                                        // check for Norton cache installation
    if (wStatus = GetTSRStatus (SigCF, Sigcf))
        {
        wNCacheID = SigCF;
        }                               // check for small Norton cache
    else if (wStatus = GetTSRStatus (SigCS, Sigcs))
        {
        wNCacheID = SigCS;
        }

    bTested = TRUE;

    if (wStatus)
        {
        switch (wCommand)
            {
	    case TSR_RESUME:
            case TSR_ENABLE:
                EnableTSR (wNCacheID);
                break;

            case TSR_DISABLE:
                // First reset the cache, which forces all buffered 
                // writes to disk and clears all data from cache buffers.
                // Then disable the cache.  The assumption is that we are
                // about to do direct disk writes ourselves, which will 
                // invalidate cached data.

                ResetTSR (wNCacheID);
                DisableTSR (wNCacheID);
                break;

	    case TSR_SUSPEND:
	        SuspendTSR( wNCacheID );
	        break;

            case TSR_FLUSH:
                FlushTSR (wNCacheID);
                break;
            }
	}

    return (wNCacheID);
}

#pragma optimize("leg", off)    /* Can't global optimize inline assembly */

/**********************************************************************
** DiskCacheSmartDrive()                                            
**                                                      
** Description:                                         
**   Test for, enable, or disable SmartDrive 4.0 cache
**
** Input:
**   wCommand is one of the following:
**	TSR_DISABLE	to disable cache, if present
**	TSR_ENABLE	to enable cache, if present
**	TSR_INSTALLED	to test for cache presence only 
**      TSR_SUSPEND     Suspend cache
**      TSR_RESUME      Resume cache
**
** Returns:
**   1			if SmartDrive 4.0 cache is present
**   0			otherwise
**********************************************************************/

UINT LOCAL PASCAL DiskCacheSmartDrive (UINT wCommand)
{
    UINT	wPresent = 0;

    _asm
        {
        push    ds

        call    DiskIsSmartDriveInstalled
        or      ax,ax
        jz      EndCache

	mov	[wPresent],1

	mov	ax,wCommand		; Is wCommand == TSR_ENABLE?
	cmp	ax,TSR_ENABLE
	jnz	TestDisable
	
        call    DiskResumeSmartDrive
	jmp	EndCache

TestDisable:
	cmp	ax,TSR_DISABLE	; wCommand == TSR_DISABLE?
	jnz	TestSuspend
                                        ; Dump pending writes and empty 
        call    DiskFlushSmartDrive     ; the cache

        call    DiskSuspendSmartDrive   ; Disable cache on all drives
	jmp	EndCache

TestSuspend:
	cmp	ax,TSR_SUSPEND
	jnz	TestResume
        call    DiskSuspendSmartDrive   ; Disable cache on all drives
	jmp	EndCache

TestResume:
	cmp	ax,TSR_RESUME
	jnz 	TestDump
        call    DiskResumeSmartDrive
	jmp	EndCache

TestDump:
	cmp	ax,TSR_FLUSH           ; wCommand == TSR_FLUSH?
	jnz	EndCache

	push	bp
	mov	ax,MULTI_SMARTDRV
	mov	bx,SD_COMMIT_ALL	;Dump writes but don't reset cache
	int	2FH
	pop	bp


EndCache:
	pop	ds
	}

    return (wPresent);
}


BYTE LOCAL PASCAL DiskIsSmartDriveInstalled (void)
{
    static      BYTE    bTested = FALSE;

    if (!smartDriveInstalled && !bTested)
        {
        bTested = TRUE;

        _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di
				   
	mov	ax,352FH
	int	21H			;Get INT 2FH vector
	mov	ax,es			;Test if 0:0 (DOS 2.X doesn't
	or	ax,bx			; set up this vector!)
	jz	SDriveNotInstalled

	push	bp
	mov	ax,MULTI_SMARTDRV
	mov	bx,SD_GET_STATUS
	int	2FH
	pop	bp
	cmp	ax,SD_SIGNATURE
	jne	SDriveNotInstalled

//      Right now it looks like SMARTDRIVE is loaded, but we have to
//	see it is really just NCACHE trying to look like SMARTDRIVE.

	mov	ax,0FE00H		;NCACHE Get Status command
	mov	si,SigCF
	mov	di,SigNU
	stc
	int	2FH
	jc	SDriveIsInstalled
	cmp	si,Sigcf		;Did we get back right signature?
	je	SDriveNotInstalled	;Yes .. NCACHE must be loaded!

SDriveIsInstalled:
	mov	ax,1
	jmp	short locRet

SDriveNotInstalled:
	xor	ax,ax

locRet:
    mov di, seg smartDriveInstalled
    mov ds, di
    mov si, offset smartDriveInstalled
	mov	byte ptr [si], al

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        }
        }

    return (smartDriveInstalled);
}


//------------------------------------------------------------------------
// void CDECL DiskFlushSmartDrive(void)
//
// Write pending sectors to disk and empty cache
//------------------------------------------------------------------------

void LOCAL PASCAL DiskFlushSmartDrive (void)
{
    if (DiskIsSmartDriveInstalled())
        {
        _asm    mov	ah,0DH
	_asm    int	21H
        }
}

//------------------------------------------------------------------------
// void CDECL DiskSuspendSmartDrive(void)
//
// Save the current SmartDrive cache state and disable further activity
//------------------------------------------------------------------------

void LOCAL PASCAL DiskSuspendSmartDrive (void)
{
    LPBYTE      lpStatus = (LPBYTE) smartDriveStatus;

    if (DiskIsSmartDriveInstalled())
        {
	// Keep track of how many times Suspend has been called and require
	// an equal number of Resume calls.  Data is only actually saved on
	// the first Suspend call and only restored on the last Resume call.

        if (++smartDriveSuspended == 1)
            {
	        // Get previous status and then disable cache on all drives
            _asm
                {
                push    ds
		push	bp
                push    si
		push	di

                lds     di,[lpStatus]
                mov	bp,0

        SDriveLoop:     
                push	di
        	push	bp
	        mov	ax,MULTI_SMARTDRV
        	mov	bx,SD_CACHE_DRIVE
        	mov	dl,CACHE_DRIVE_GET
        	int	2FH
        	pop	bp

        	test	dl,80H          ;Reads already disabled?
                                        ;Yes - Don't disable them again
        	jnz	Dont_Disable_Reads
        	push	dx
        	push	bp
        	mov	ax,MULTI_SMARTDRV
        	mov	bx,SD_CACHE_DRIVE
        	mov	dl,CACHE_DRIVE_READ_DISABLE
        	int	2FH
	        pop	bp
                pop	dx

        Dont_Disable_Reads:
                test	dl,40H		;Writes already disabled?
                                        ;Yes - Don't disable them again
        	jnz	Dont_Disable_Writes
        	push	dx
        	push	bp
        	mov	ax,MULTI_SMARTDRV
        	mov	bx,SD_CACHE_DRIVE
        	mov	dl,CACHE_DRIVE_WRITE_DISABLE
        	int	2FH
        	pop	bp
        	pop	dx

        Dont_Disable_Writes:
        	pop	di
        	mov	ds:[di],dl
        	inc	di
        	inc	bp
        	cmp	bp,26
        	jne	SDriveLoop

		pop	di
                pop     si
		pop	bp
                pop     ds
                }
            }
        }
}

//------------------------------------------------------------------------
// void CDECL DiskResumeSmartDrive(void)
//
// This function should only be called after DiskSuspendSmartDrive()!
//------------------------------------------------------------------------

void LOCAL PASCAL DiskResumeSmartDrive (void)
{
    LPBYTE      lpStatus = (LPBYTE) smartDriveStatus;

    if (DiskIsSmartDriveInstalled())
        {
                // Don't resume unless previously suspended
        if (--smartDriveSuspended == 0)
            {
	        // Get previous status and then disable cache on all drives
            _asm
                {
                push    ds
                push	bp
		push	si
                push    di

                ; Re-enable cache on drives where it was enabled before
                lds     si,[lpStatus]
 	        mov	bp,0

        SDriveLoop:
	        cld
	        lodsb				;Get prior drive status in AL
	        push	bp
	        push	si

	        test	al,80H			;Was read cache enabled?
                jnz     Dont_Enable_Reads       ;NZ = Off
	        push	ax
	        push	bp
	        mov	ax,MULTI_SMARTDRV
	        mov	bx,SD_CACHE_DRIVE
	        mov	dl,CACHE_DRIVE_READ_ENABLE
	        int	2FH
	        pop	bp
	        pop	ax

        Dont_Enable_Reads:
	        test	al,40H			;Was write cache enabled?
	        jnz	Dont_Enable_Writes      ;NZ = Off
	        mov	ax,MULTI_SMARTDRV
	        mov	bx,SD_CACHE_DRIVE
	        mov	dl,CACHE_DRIVE_WRITE_ENABLE
	        int	2FH

        Dont_Enable_Writes:
	        pop	si
	        pop	bp
	        inc	bp
	        cmp	bp,26
	        jne	SDriveLoop

                pop     di
		pop	si
	        pop	bp
                pop     ds
                }
            }
        }
}

//------------------------------------------------------------------------
// Ulong CDECL DiskGetCacheSize(Word *pwBlockSize, Word *pwCurrentBlocks,
//			       Word *pwDosBlocks, Word *pwWinBlocks);
//
// Returns the current cache size in bytes.
// Returns zero for all values if NCACHE or SmartDrive 4.0 not loaded.
//------------------------------------------------------------------------

DWORD SYM_EXPORT WINAPI DiskGetCacheSize(
    UINT FAR    *pwBlockSize, 
    UINT FAR    *pwCurrentBlocks,
    UINT FAR    *pwDosBlocks,
    UINT FAR    *pwWinBlocks)
{
    auto        DWORD   dwReturn;

    if (DiskCacheNorton(TSR_INSTALLED) || DiskIsSmartDriveInstalled())
        {
                                        // We can use SmartDrive API to 
                                        // query both NCACHE and SmartDrive 
                                        // itself
        _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di
				   
	push	bp
	mov	ax,MULTI_SMARTDRV
	mov	bx,SD_GET_INFO
	int	2FH
	pop	bp
                                        // Save cache size information
	les	si,pwBlockSize
	mov	es:[si],cx
	les	si,pwCurrentBlocks
	mov	es:[si],bx
	les	si,pwDosBlocks
	mov	es:[si],ax
	les	si,pwWinBlocks
	mov	es:[si],dx
                                        // Compute current cache size = 
                                        //      block size * number of blocks
	mov	ax,cx
	mul	bx

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        jmp     short Finish_Up
        }
        }
    else
        {
        *pwBlockSize = 0;
        *pwCurrentBlocks = 0;
        *pwDosBlocks = 0;
        *pwWinBlocks = 0;
        _asm    xor     ax,ax
        _asm    cwd
        }

Finish_Up:
        _asm
        {
        mov     word ptr dwReturn, ax
        mov     word ptr dwReturn + 2, dx
        }

    return (dwReturn);
}

//------------------------------------------------------------------------
// Get address of NCACHE ICOM handler
//------------------------------------------------------------------------

#ifdef SYM_PROTMODE

LPVOID LOCAL DiskGetNCacheICOMAddress(void)
{
    static	LPVOID		lpfnICOM;
    auto	struct DPMIRec	dpmi;

    if (lpfnICOM == NULL)
	{
	if (!DiskCacheNorton(TSR_INSTALLED))
	    return(NULL);

	MEMSET(&dpmi, 0, sizeof(dpmi));
	dpmi.eax = 0xFE00;
	dpmi.esi = wNCacheID;
	dpmi.edi = SigNU;
	dpmi.simulatedInt = 0x2F;
	DPMISimulateInt(&dpmi);
	lpfnICOM = MAKELP(dpmi.es, (WORD)dpmi.ebx);
	}

    return(lpfnICOM);
}

#endif // SYM_WIN

//------------------------------------------------------------------------
// BOOL DiskGetCacheHits(DWORD far *lpdwHits, DWORD far *lpdwMisses);
//
// Returns TRUE if the data was obtained, FALSE if not.
// This only works for SmartDrive 4.0 and NCACHE.
//------------------------------------------------------------------------

#ifdef SYM_PROTMODE

typedef struct
    {
    DWORD	dwTotalSectorsRead;
    DWORD	dwActualSectorsRead;
    DWORD	dwTotalSectorsWritten;
    DWORD	dwActualSectorsWritten;
    DWORD	dwTotalWriteReqs;
    DWORD	dwActualWriteReqs;
    } NCacheDriveStats;

BOOL SYM_EXPORT WINAPI DiskGetCacheHits(DWORD far *lpdwHits, DWORD far *lpdwMisses)
{
    auto	struct DPMIRec	dpmi;
    auto	LPVOID		lpfnICOM;
    auto	DWORD		dwStats;
    auto	NCacheDriveStats far *lpDriveStats;
    auto	DWORD		dwTotalRead;
    auto	DWORD		dwActualRead;
    auto	int		nDrive;

    *lpdwHits = *lpdwMisses = 0L;

    lpfnICOM = NULL;
    if (DiskCacheNorton(TSR_INSTALLED))
	{
	if ((lpfnICOM = DiskGetNCacheICOMAddress()) == NULL)
	    return(FALSE);

	// Allocate DOS memory to receive NCACHE stats structure
	if ((dwStats = GlobalDosAlloc(sizeof(NCacheDriveStats))) == NULL)
	    return(FALSE);
	lpDriveStats = MAKELP(LOWORD(dwStats), 0);

	// Get stats on each drive and keep totals
	dwTotalRead = dwActualRead = 0L;
	for (nDrive = 0; nDrive < 26; nDrive++)
	    {
	    MEMSET(&dpmi, 0, sizeof(dpmi));
	    dpmi.eax = 0x0500;			// AH = 5
	    dpmi.edx = nDrive;			// DL = Drive number
	    dpmi.es  = HIWORD(dwStats);	        // ES:DI = Buffer
	    // dpmi.edi = 0;  // Already done by MEMSET
	    dpmi.cs  = FP_SEG(lpfnICOM);	// Call ICOM handler
	    dpmi.ip  = FP_OFF(lpfnICOM);
	    DPMISimulateFarCall(&dpmi);

	    // NCACHE sets carry if drive number is invalid
	    if (!dpmi.cflag)
		{
		dwTotalRead  += lpDriveStats->dwTotalSectorsRead;
		dwActualRead += lpDriveStats->dwActualSectorsRead;
		}
	    }

	// Free the NCACHE stats buffer
	GlobalDosFree(LOWORD(dwStats));

	// Convert data into hit and miss counts
	*lpdwHits   = dwTotalRead - dwActualRead;
	*lpdwMisses = dwActualRead;

	return(TRUE);
	}

    if (DiskIsSmartDriveInstalled())
	{
	_asm
        {
	push	ds
	push	si
	push	di
				   
	push	bp
	mov	ax,MULTI_SMARTDRV
	mov	bx,SD_GET_STATUS
	int	2FH
	pop	bp

	mov	ax,di			; Move DI to AX so we can use DI
					; to index into memory
	les	di,[lpdwHits]
	mov	word ptr es:[di+0],bx
	mov	word ptr es:[di+2],dx

	les	di,[lpdwMisses]
	mov	word ptr es:[di+0],si
	mov	word ptr es:[di+2],ax	; Value was in DI

	pop	di
	pop	si
	pop	ds
        }

	return(TRUE);
	}

    return(FALSE);
}

#endif // SYM_WIN

/*----------------------------------------------------------------------*
 * DiskGetUnderSmartDrive()						*
 *									*
 * SmartDrive hooks the block drivers by changing the device driver	*
 * pointers in all DPB's so they point to SmartDrive.  NCACHE does	*
 * basically the same thing, but NCACHE only does it for drives it is	*
 * caching -- SmartDrive does it for all drives.  This was getting in	*
 * the way of detecting SuperStor, which works by looking for a string	*
 * within the driver.  Luckily, SmartDrive has an API to return	the	*
 * real address of a driver.  This function calls that API.		*
 *----------------------------------------------------------------------*/

LPVOID SYM_EXPORT WINAPI DiskGetUnderSmartDrive(BYTE byDriveLetter)
{
    auto	WORD		wUnitNumber;
    auto	LPVOID		lpDriver;
#ifdef SYM_PROTMODE
    auto	struct DPMIRec	dpmi;
#endif

    if (!DiskIsSmartDriveInstalled())
	return(NULL);

    // Unit numbers appear to be one based (A=1, B=2, ...)
    wUnitNumber = (CharToUpper(byDriveLetter) - 'A') + 1;

#ifdef SYM_PROTMODE
	MEMSET(&dpmi, 0, sizeof(dpmi));
	dpmi.ebp = wUnitNumber;
	dpmi.eax = MULTI_SMARTDRV;
	dpmi.ebx = SD_GET_REAL_DRIVER_ADDR;
	dpmi.simulatedInt = 0x2F;
	
	DPMISimulateInt(&dpmi);
	
	FP_SEG(lpDriver) = dpmi.es;
	FP_OFF(lpDriver) = (WORD) dpmi.edi;
#else
    _asm
	{
	push	ds
	push	es
	push	si
	push	di

	push	bp
	xor	di,di
	push	di
	pop	es		;Initialize ES:DI == NULL
	xor	ah,ah
	mov	bp,[wUnitNumber]
	mov	ax,MULTI_SMARTDRV
	mov	bx,SD_GET_REAL_DRIVER_ADDR
	int	2Fh
	pop	bp
	mov	word ptr [lpDriver+0],di
	mov	word ptr [lpDriver+2],es

	pop	di
	pop	si
	pop	es
	pop	ds
	}
#endif

    return(lpDriver);
}

#pragma optimize("", on)

/*----------------------------------------------------------------------*
 * DiskGetUnderSymEvent()						*
 *									*
 * SymEvent hooks the block drivers by changing the device driver	*
 * pointers in all DPB's so they point to SymEvent.  This function	*
 * returns the original driver address.  The primary purpose of this	*
 * code is to detect SuperStor drives, which is done by looking for	*
 * a string at a known offset within their driver.			*
 *----------------------------------------------------------------------*/

#ifdef SYM_WIN16

typedef LPVOID (WINAPI far *GETADDRPROC)(int);

static GETADDRPROC	lpfnGetDriverAddr;
extern HINSTANCE	hInst_SYMEVENT;

LPVOID SYM_EXPORT WINAPI DiskGetUnderSymEvent(BYTE byDriveLetter)
{
    UINT uError;

    // Drive will not be 'under' SymEvent under Chicago so skip under Win 3.5+
    BYTE wMajor = LOBYTE(LOWORD(GetVersion()));
    BYTE wMinor = HIBYTE(LOWORD(GetVersion()));
    if((wMajor > 3) || (wMajor == 3 && wMinor > 5))
        return NULL;

    // Load SYMEVENT.DLL if not already loaded
    uError = SetErrorMode (SEM_NOOPENFILEERRORBOX);

#if !defined (NO_SYMEVENT)
    if (hInst_SYMEVENT == NULL)
	hInst_SYMEVENT = LoadLibrary(QMODULE_SYMEVNT ".DLL");
#endif /* NO_SYMEVENT */

    SetErrorMode (uError);

    // Get exported function address
    if (hInst_SYMEVENT > (HINSTANCE)HINSTANCE_ERROR && lpfnGetDriverAddr == NULL)
	lpfnGetDriverAddr = (GETADDRPROC) GetProcAddress(hInst_SYMEVENT, "GetDrvHeaderAddr");

    // Return NULL if we couldn't load SYMEVENT or find exported function
    if (hInst_SYMEVENT <= (HINSTANCE)HINSTANCE_ERROR || lpfnGetDriverAddr == NULL)
	return(NULL);

    // Call function in SYMEVENT to get driver address
    return (*lpfnGetDriverAddr)(CharToUpper(byDriveLetter) - 'A');
}

#else

LPVOID SYM_EXPORT WINAPI DiskGetUnderSymEvent(BYTE byDriveLetter)
{
    return(NULL);
}

#endif

/*----------------------------------------------------------------------*
 * This routine detects whether a computer has CMOS.			*
 * It first calls INT 15, function 0xC0 to see it a real clock is	*
 * installed - if so CMOS is present.  Many computers however, do not	*
 * support this function.  They do however have CMOS.  In this case	*
 * we then read the time and date to see if the match the DOS time and  *
 * date.  If they do, CMOS is present, otherwise no CMOS is installed.	*
 * 									*
 * RETURNS:								*
 *	TRUE - CMOS present						*
 *	FALSE - no CMOS							*
 *									*
 * IMPLEMENTATION HISTORY						*
 * 03/16/90	Henri: created						*
 * 06/30/90	Brad: Modified to save/restore the date/time.  Also	*
 *		return FALSE if not System ID of 0xFC and lower.	*
  * 09/24/1999 CBrown: Code removed because it caused invalid date / *
 * 					  time changes on too many systems. *
*----------------------------------------------------------------------*/

//SwitchRec SW_NOCMOS = { "NOCMOS", EXACT };

UINT SYM_EXPORT WINAPI HWHasCMOS (void)
{
	//
	// Old code was cut out that casued invalid date / time changes on 
	// certain system.  It safe to assume that all systems have a CMOS 
	// installed these days...
	//
	// CBROWN - 09/24/1999
	//
	
	return( TRUE );	
}

/**********************************************************************
** DiskEraseProtect()                                            
**                                                      
** Description:                                         
**   Query and Control the Norton Erase Protect TSR (and the other clones).
**
** Input:
**      TSR_INSTALLED   Check for Installation
**      TSR_ENABLE      Enable Erase Protect
**      TSR_DISABLE     Disable Erase Protect
**
** Returns:
**      TSR_NOT_INSTALLED
**      TSR_IS_ENABLED
**      TSR_IS_DISABLED
**
**********************************************************************/

UINT SYM_EXPORT WINAPI DiskEraseProtect (UINT wCommand)
{
    static      UINT    wState = 0;
    auto        UINT    wStatus;

    wStatus = GetTSRStatus (SigFS, Sigfs);

    if (wStatus != TSR_NOT_PRESENT)
        {
        switch (wCommand)
            {
            case TSR_ENABLE:
                                        // is it already enabled, or
                                        // did we never turn it off?
                if (wStatus == TSR_IS_ENABLED || wState != 1)
                    break;

                wState = 0;                 // indicate we turned it on
                EnableTSR (SigFS);
                break;

            case TSR_DISABLE:
                wState = 1;                 // indicate we turned it off
                DisableTSR (SigFS);
                break;
            }
        }

    return (wStatus);
}

/**********************************************************************
** DiskMonitor()                                            
**                                                      
** Description:                                         
**   Query and Control the Norton Disk Monitor TSR.
**
** Input:
**      TSR_INSTALLED   Check for Installation
**      TSR_ENABLE      Enable Disk Monitor
**      TSR_DISABLE     Disable Disk Monitor
**
** Returns:
**      TSR_NOT_INSTALLED
**      TSR_IS_ENABLED
**      TSR_IS_DISABLED
**
**********************************************************************/

UINT SYM_EXPORT WINAPI DiskMonitor (UINT wCommand)
{
    BOOL	bOldState = FALSE;
    UINT        wStatus;

    wStatus = GetTSRStatus (SigDM, Sigdm);

    if (wStatus != TSR_NOT_PRESENT)
        {
        if (wStatus == TSR_IS_ENABLED)
            bOldState = TRUE;           // remember "ON" state

        switch (wCommand)
            {
            case TSR_ENABLE:
                EnableTSR (SigDM);
                break;

            case TSR_DISABLE:
                DisableTSR (SigDM);
                break;
            }
	}

    return (wStatus);
}

#pragma optimize("leg", off)    /* Can't global optimize inline assembly */

// **********************************************************************
// **********************************************************************
//                      NORTON TSR CONTROL FUNCTIONS
// **********************************************************************
// **********************************************************************

UINT LOCAL PASCAL GetTSRStatus (UINT wID, UINT wMatchID)
{
    UINT        wInstalled = 0;

    _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di
				   
	mov	ax,352FH
	int	21H			; Get INT 2FH vector
	mov	ax,es			; Test if 0:0 (DOS 2.X doesn't
	or	ax,bx			;  set up this vector!)
	jz	All_Done

	mov	ax,0FE00H		; Get Status command
	mov	si,wID
	mov	di,SigNU
	stc
	push	bp
	int	2FH
	pop	bp
	jc	All_Done
	cmp	si,wMatchID
	jne	All_Done                ; did the signatures match?

        inc     wInstalled              ; yes, signal installation (1)

	or	ah,ah			; Must be present, but is it enabled?
	jz	All_Done

        inc     wInstalled              ; yes, signal enabled (2)
All_Done:
	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        }

    return (wInstalled);
}

void LOCAL PASCAL EnableTSR (UINT wID)
{
    _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di

        mov	ax,0FE01H               ; Enable command
        mov	si,wID
        mov	di,SigNU
        int	2FH

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        }
}

void LOCAL PASCAL DisableTSR (UINT wID)
{
    _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di
				   
        mov	ax,0FE02H               ; Disable command
        mov	si,wID
        mov	di,SigNU
        int	2FH

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        }
}

void LOCAL PASCAL ResetTSR (UINT wID)
{
    _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di

        mov	ax,0FE03H               ; Reset command
        mov	si,wID
        mov	di,SigNU
        int	2FH

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        }

}

void LOCAL PASCAL SuspendTSR (UINT wID) // NCACHE only
{
    _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di
				   
        mov	ax,0FE04H               ; Reset command
        mov	si,wID
        mov	di,SigNU
        int	2FH

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        }

}

void LOCAL PASCAL FlushTSR (UINT wID)
{
    _asm
        {
	push	ds			; Save registers
	push	es
	push	si
	push	di

        mov	ax,0FE07H               ; Dump cached writes command
        mov	si,wID
        mov	di,SigNU
        int	2FH

	pop	di			; Restore registers
	pop	si
	pop	es
	pop	ds
        }
}


typedef struct VFATTableStruct
    {
    DWORD	dwNumEntries;
    BYTE	byDrives[26];
    } VFATTableStruct;

static	VFATTableStruct	rVFATTable;
static	BOOL		bTested = FALSE;

/*----------------------------------------------------------------------*
 * This function tests for the 32 bit FILE access capabilities of 	*
 * Windows for Work Groups 3.11.  This is also known as VFAT.		*
 *----------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI DiskIsVFATInstalled(void)
{
    auto	WORD		i = sizeof(rVFATTable);
    static	DWORD		lpCallBack = 0;
    
    if (! bTested)
	{
	_asm
	    {
	    push	ds
	    push	si
	    push	di
	    xor		di, di
	    mov		es, di
	    mov		ax, 0x1684
	    mov		bx, 0x0486
	    int		0x2F
	
	    mov		WORD PTR lpCallBack + 2, es
	    mov		WORD PTR lpCallBack, di
	    
	    mov		ax, es
	    or		ax, di
	    jz		done
	    
	    mov		ax, 3
	    push	ds
	    pop		es
	    lea		bx, rVFATTable
	    mov		cx, i
	    call	lpCallBack
	    
	done:
	    pop		di
	    pop		si
	    pop		ds
	    }
	
	/****************************************************************
	 * If no VFAT volumes are mounted then we don't consider VFAT	*
	 * to be running.						*
	 ****************************************************************/

	if (lpCallBack != 0 && rVFATTable.dwNumEntries == 0)
	    lpCallBack = 0;

	bTested = TRUE;
	}
    
    return (lpCallBack != 0);
}

/*----------------------------------------------------------------------*
 * This function tests for the 32 bit FILE access capabilities of 	*
 * Windows for Work Groups 3.11.  This is also known as VFAT.		*
 *----------------------------------------------------------------------*/
BOOL SYM_EXPORT WINAPI DiskIsVFAT(BYTE dl)
{
    auto	WORD		i;
   
    dl = CharToUpper(dl) - 'A';
    
    for (i = 0; i < rVFATTable.dwNumEntries; i++)
	if (rVFATTable.byDrives[i] == dl)
	    return (TRUE);
	
    return (FALSE);
}


#pragma optimize("", on)

#ifdef SYM_WIN
BOOL LOADDS CALLBACK NoVFATDlgProc (HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam);

BOOL SYM_EXPORT WINAPI DiskCheckForVFAT (void)
{
    extern HINSTANCE hInst_SYMKRNL;
    BOOL bResult;

    bResult = DiskIsVFATInstalled ();

    if (bResult)
        {  
        DialogBox (hInst_SYMKRNL, MAKEINTRESOURCE (IDD_NO_VFAT_ALLOWED),
                        GetActiveWindow(), (DLGPROC) NoVFATDlgProc);
        }

    return (bResult);
}


BOOL LOADDS CALLBACK NoVFATDlgProc (HWND hDlg, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    switch (wMessage)
        {
        case WM_INITDIALOG:
            return (TRUE);

        case WM_COMMAND:
            switch (GET_WM_COMMAND_ID(wParam, lParam))
                {
                case IDCANCEL:
                case IDOK:
                    EndDialog(hDlg, (WORD) (GET_WM_COMMAND_ID(wParam, lParam) == IDOK) );
                    break;
                }
            break;
        }

    return (FALSE);
}
#endif // SYM_WIN32

#endif

