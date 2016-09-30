/* Copyright 1992-1994 Symantec Corporation                             */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/PRM_WIN.C_v   1.1   19 Mar 1997 21:38:26   BILL  $ *
 *                                                                      *
 * Description:                                                         *
 *      Mostly Windows-specific functions for critical sections and     *
 *      DPMI.  Also DOS version of Int().                               *
 *                                                                      *
 * Contains:                                                            *
 *      _GetSysVMLinearStart()                                          *
 *      CriticalSectionBegin                                            *
 *      CriticalSectionEnd                                              *
 *      DPMISegmentToDescriptor                                         *
 *      DPMISimulateInt                                                 *
 *      DPMISimulateFarCall                                             *
 *      IntWin                                                          *
 *      Int     - DOS only.  Windows and DOSX code macros to IntWin()   *
 *      RealModeFarCall							*
 *      SelectorAlloc                                                   *
 *      SelectorFree                                                    *
 *      SelectorSetDosAddress                                           *
 *      SelectorGetDosAddress                                           *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/PRM_WIN.C_v  $ *
// 
//    Rev 1.1   19 Mar 1997 21:38:26   BILL
// Modified files to build properly under Alpha platform
// 
//    Rev 1.0   26 Jan 1996 20:22:36   JREARDON
// Initial revision.
// 
//    Rev 1.46   18 Apr 1995 09:13:44   MARKK
// Correct problems in trunk merge
// 
//    Rev 1.45   11 Apr 1995 18:03:46   HENRI
// Merge DX platform changes form branch 6
//
//    Rev 1.44   02 Feb 1995 18:20:36   BRAD
// Changed GetProtModePtr() to use SYMKRNL VxD for WIN32
//
//    Rev 1.43   20 Jan 1995 09:45:36   MARKL
//
//    Rev 1.42   18 Jan 1995 15:02:54   RSTANEV
// _GetSysVMLinearStart() for VxD now works on Sys_VM, not the Cur_VM.
//
//    Rev 1.41   16 Jan 1995 12:04:22   RSTANEV
// Added VxD support to _GetSysVMLinearStart().
//
//    Rev 1.40   16 Jan 1995 08:24:44   MARKK
// Check device pointer to see if it is already a selector
//
//    Rev 1.39   10 Jan 1995 19:09:28   MBARNES
// Initialized uSelector to 0 in GetProtModePtr() before calling SelectorAlloc()
//
//    Rev 1.38   28 Nov 1994 13:29:08   PATRICKM
// Removed GetProtModePtr() stub function. This is now properly #define'd
// in XAPI.H.
//
//
//    Rev 1.37   18 Nov 1994 12:10:00   BRAD
// Added better error checking, so won't blow up if IOCTL.VXD doesn't exist
//
//    Rev 1.36   17 Nov 1994 15:47:30   DBUCHES
// Stub for GetProtModePointer is now properly #ifdef'd
//
//    Rev 1.35   16 Nov 1994 21:37:46   TORSTEN
// Added a version of GetProtModePtr() for good-old DOS.
//
//    Rev 1.34   16 Nov 1994 18:00:46   BRAD
// Added GetProtModePtr() for WIN32
//
//    Rev 1.33   04 Oct 1994 14:53:58   BRAD
// Incorporated NEC support
//
//    Rev 1.32   21 Jun 1994 11:27:04   MARKK
// Change DOSX to Rational
//
//    Rev 1.31   21 Jun 1994 09:23:32   MARKK
// Fixed pharlab bug
//
//    Rev 1.30   09 Jun 1994 15:31:44   MARKK
// Fixed crit error
//
//    Rev 1.29   09 Jun 1994 12:32:44   MARKK
// Moved error handler to C for DX
//
//    Rev 1.28   07 Jun 1994 03:25:04   GVOGEL
// Added comment blocks for Geologist project.
//
//    Rev 1.27   01 Jun 1994 09:18:08   MARKK
// Memset the regs before calling PharLap routine
//
//    Rev 1.26   31 May 1994 14:51:58   MARKK
// cld after DPMI int.
//
//    Rev 1.25   31 May 1994 14:07:46   MARKK
// Fix to GlobalDosAlloc
//
//    Rev 1.24   27 May 1994 11:41:44   MARKK
// PharLap gets confused by int 25/26 leaving the flags on the stack
//
//    Rev 1.23   27 May 1994 07:02:52   MARKK
// Change DX to PharLap
//
//    Rev 1.22   26 May 1994 12:40:26   MARKK
// Added param to DXHookInterrupt
//
//    Rev 1.21   26 May 1994 07:41:20   MARKK
// Change DX to PharLap
//
//    Rev 1.20   09 May 1994 09:21:42   MARKK
// DOSX work
//
//    Rev 1.19   05 May 1994 16:00:54   BRAD
// Added Int()
//
//    Rev 1.18   04 May 1994 11:00:36   MARKK
// DOSX work
//
//    Rev 1.17   04 May 1994 05:51:26   MARKK
// DOSX work
//
//    Rev 1.16   29 Apr 1994 17:15:16   MARKK
// DOSX work
//
//    Rev 1.15   14 Apr 1994 19:51:06   BRAD
// Move CriticalSectionBegin/End to IntWin, rather than all over the place
//
//    Rev 1.14   15 Mar 1994 13:51:20   BRUCE
// Changed EXPORT to SYM_EXPORT for real!
//
//    Rev 1.13   15 Mar 1994 12:32:44   BRUCE
// Changed SYM_EXPORT to SYM_EXPORT
//
//    Rev 1.12   25 Feb 1994 15:03:56   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.10   10 Jan 1994 19:18:04   JOHN
// Added RealModeFarCall() and DPMISimulateFarCall().
//
//    Rev 1.9   14 Oct 1993 08:58:54   ED
// Removed property diagnostic functions
//
//    Rev 1.8   06 Aug 1993 08:00:52   ED
// Fix to the last addition
//
//    Rev 1.7   05 Aug 1993 17:12:56   ED
// Added some window property dumping functions
//
//    Rev 1.6   02 Jul 1993 08:52:08   DOUGN
// Change for conversion to C8.  Moved "include <dos.h>" to be above the
// "include "platform.h"" line so that MK_FP would be defined by the
// C8 dos.h include file and platform.h wouldn't redefine it and cause
// warnings.
//
//    Rev 1.5   30 Mar 1993 10:57:28   HENRI
// Added function: GetProtModePtr(lpRealMode)
//
//    Rev 1.4   12 Feb 1993 04:24:38   ENRIQUE
// No change.
//
//    Rev 1.3   10 Sep 1992 18:39:06   BRUCE
// Eliminated unnecesary includes
//
//    Rev 1.2   03 Sep 1992 07:23:48   ED
// Fixed up
//
//    Rev 1.1   02 Sep 1992 11:25:58   ED
// Updated
//
//    Rev 1.0   26 Aug 1992 17:00:52   ED
// Initial revision.
 ************************************************************************/

/*@Library: SYMKRNL*/

#include <dos.h>
#include "platform.h"
#include "xapi.h"
#include "disk.h"
#if defined(SYM_WIN32)
#ifndef _M_ALPHA
#include <vmm.h>
#include "vsymkrnl.h"
#include "vxdload.h"
#endif // _M_ALPHA
#endif


//------------------------------------------------------------------------
// Used only within Windows 95 and VxD.  Returns the linear address of the
// start of the system VM.
//------------------------------------------------------------------------
LPBYTE LOCAL PASCAL _GetSysVMLinearStart(VOID)
{
#ifdef _M_ALPHA
	return NULL;
#else	
#if defined(SYM_WIN32)
    SYM_REGS        symRegs;

    if (VxDRegBasedCall(GetVxDServiceOrdinal(Get_Sys_VM_Handle), &symRegs))
        return((LPBYTE) (((struct cb_s *) symRegs.EBX)->CB_High_Linear));
    else
        return(NULL);
#elif defined (SYM_VXD)

#if !defined(__cplusplus)
    typedef struct cb_s cb_s;
#endif

    auto struct cb_s* prVMCB;

    prVMCB = (cb_s *)Get_Sys_VM_Handle();

    return ( (LPBYTE) (prVMCB->CB_High_Linear) );

#else
    return (NULL);
#endif // SYM_WIN32
#endif // _M_ALPHA
}

#if      defined(SYM_WIN) || defined(SYM_DOSX) || defined(SYM_VXD)

static BOOL bCritErrorOccurred;
static BYTE sLocalName[9];
static WORD wAX;
static WORD wDI;


/*@API:**********************************************************************
@Declaration: LPVOID GetProtModePtr( LPVOID lpRealMode )

@Description:
This function allocates a protected mode pointer to access an equivalent real
mode pointer.  This function allocates an entire 64K segment and should
therefore be used carefully.  It is most useful when using it in the same
function where it is also freed.

@See: SelectorAlloc
@Include: xapi.h
@Compatibility: Win16, Win32
*****************************************************************************/
LPVOID SYM_EXPORT PASCAL GetProtModePtr( LPVOID lpRealMode )
{
#if defined(SYM_WIN32) || defined(SYM_VXD)
    LPBYTE  lpByte;

    lpByte = _GetSysVMLinearStart();
    if (lpByte == NULL)
        return(NULL);
    return(lpByte + (FP_SEG(lpRealMode) * 16L + FP_OFF(lpRealMode)));
#else
    UINT uSelector;
    LPVOID lpProtMode;

    uSelector = 0;
    if (SelectorAlloc(FP_SEG(lpRealMode), 0xFFFF, &uSelector))
	    return(NULL);
    FP_SEG(lpProtMode) = uSelector;
    FP_OFF(lpProtMode) = FP_OFF(lpRealMode);
    return(lpProtMode);
#endif
}
#endif


#if      defined(SYM_WIN16) || defined(SYM_DOSX)
					// don't compile this for DOS
#if      defined(SYM_WIN16)

//------------------------------------------------------------------------
//------------------------------------------------------------------------

/*@API:**********************************************************************
@Declaration: void  CriticalSectionBegin()

@Description:
This function prevents Windows from switching to another virtual machine or
performing a task switch.  A counter is maintained that prevents task switching
when non-zero.  Therefore, nested calls to this function are valid, but each
requires a corresponding call to CriticalSectionEnd(). \n

This function has no parameters.

@See: CriticalSectionEnd
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/
VOID SYM_EXPORT WINAPI CriticalSectionBegin(VOID)
{
    _asm        mov     ax, 1681h       // Begin Critical Section function
    _asm        int	2Fh
}

/*@API:**********************************************************************
@Declaration: void  CriticalSectionEnd()

@Description:
This function resumes normal task switching operations after being suspended by
a call to CriticalSectionBegin().  This function must be called once for every
call to CriticalSectionBegin(). \n

This function has no parameters.

@See: CriticalSectionBegin
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/
VOID SYM_EXPORT WINAPI CriticalSectionEnd(VOID)
{
    _asm        mov     ax, 1682h       // End Critical Section function
    _asm        int	2Fh
}

#endif

/*@API:**********************************************************************
@Declaration: UINT  IntWin(BYTE IntNum, union REGS FAR * regs, struct SREGS FAR
* segregs)

@Description:
This function executes the 8086-processor-family interrupt specified by IntNum,
using the DPMI simulate interrupt service.  This function should be used to
execute any interrupts not virtualized by Windows.  The input values for the
system call are specified by register values defined in regs, and the results
of the system call are returned in the same structure.  Segment registers are
passed and returned in the segregs structure. \n

This function is almost identical to the int86() function found in most C run-
time libraries.

@Parameters:
$IntNum$ The interrupt number to execute.

$regs$ The 8086 register set.

$segregs$ The 8086 segment register set.

@Returns:
The return value is the value of the AX register after the system call is
completed.

@See: DPMISimulateInt
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/

#ifdef	SYM_DOSX
typedef struct
    {
    WORD bCritErrorOccurred;
    WORD wAX;
    WORD wDI;
    } far *LPERROR;
#endif

UINT SYM_EXPORT WINAPI IntWin(BYTE intNum, union REGS FAR *regs, struct SREGS FAR *segregs)
{
    auto	DPMIRec		DPMI;
    auto	BOOL		err;


#ifdef	SYM_DOSX

    extern	WORD near	bErrorModeOff;
    extern	ERRORTRAPPROC	DISK_ERR_RTN;
    extern	WORD		inCriticalError;
    extern	WORD		wCritHandle;
    auto	WORD		wRet;
    auto	LPERROR		lpError = MAKELP(wCritHandle, 0);

 top:
#endif
					/*------------------------------*/
					/* Clear DPMI record		*/
					/*------------------------------*/
    MEMSET(&DPMI, 0, sizeof(DPMI));
					/*------------------------------*/
					/* Copy values from input to	*/
					/* DPMI record.			*/
					/*------------------------------*/
    DPMI.eax = regs->x.ax;
    DPMI.ebx = regs->x.bx;
    DPMI.ecx = regs->x.cx;
    DPMI.edx = regs->x.dx;
    DPMI.esi = regs->x.si;
    DPMI.edi = regs->x.di;
    DPMI.ebp = regs->x.di;	        // Needed for NEC
    DPMI.es  = segregs->es;
    DPMI.ds  = segregs->ds;
    DPMI.simulatedInt = intNum;
					/*------------------------------*/
					/* Do it!			*/
					/*------------------------------*/
#if      defined(SYM_WIN)
    CriticalSectionBegin();
#endif

#ifdef	SYM_DOSX

    if (lpError != NULL)
	lpError->bCritErrorOccurred = FALSE;
#endif

    err = DPMISimulateInt(&DPMI);

#ifdef	SYM_DOSX
    if (lpError != NULL && lpError->bCritErrorOccurred)
	{
	if (! (bErrorModeOff || (DWORD)DISK_ERR_RTN == 0xFFFFFFFF))
	    {
	    inCriticalError++;

	    wRet = DISK_ERR_RTN(lpError->wAX, lpError->wDI, sLocalName);

	    inCriticalError--;

	    if (LOBYTE(wRet) != 2)
		goto top;
	    }
	}
#endif
#if      defined(SYM_WIN)
    CriticalSectionEnd();
#endif
    if (err)
        DPMI.cflag = 0x01;		/* Set CY			*/

					/*------------------------------*/
					/* Copy results to output	*/
					/*------------------------------*/
    regs->x.ax = (UINT) DPMI.eax;
    regs->x.bx = (UINT) DPMI.ebx;
    regs->x.cx = (UINT) DPMI.ecx;
    regs->x.dx = (UINT) DPMI.edx;
    regs->x.si = (UINT) DPMI.esi;
    regs->x.di = (UINT) DPMI.edi;
    segregs->es= DPMI.es;
    segregs->ds= DPMI.ds;

    regs->x.cflag = DPMI.cflag;
					/*------------------------------*/
					/* Return AX			*/
					/*------------------------------*/
    return (regs->x.ax);
}

/*@API:**********************************************************************
@Declaration: UINT RealModeFarCall(LPVOID lpCallAddress, union REGS FAR *regs,
struct SREGS FAR *segregs)

@Description:
This function executes the 8086-processor-family interrupt specified by IntNum,
like the IntWin function, except that is uses a far call rather than an
interrupt.

@See: DPMISimulateInt IntWin
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/
UINT SYM_EXPORT WINAPI RealModeFarCall(LPVOID lpCallAddress, union REGS FAR *regs, struct SREGS FAR *segregs)
{
    auto	DPMIRec		DPMI;
    auto	BOOL		err;

					/*------------------------------*/
					/* Clear DPMI record		*/
					/*------------------------------*/
    MEMSET(&DPMI, 0, sizeof(DPMI));
					/*------------------------------*/
					/* Copy values from input to	*/
					/* DPMI record.			*/
					/*------------------------------*/
    DPMI.eax = regs->x.ax;
    DPMI.ebx = regs->x.bx;
    DPMI.ecx = regs->x.cx;
    DPMI.edx = regs->x.dx;
    DPMI.esi = regs->x.si;
    DPMI.edi = regs->x.di;
    DPMI.es  = segregs->es;
    DPMI.ds  = segregs->ds;
    DPMI.cs  = FP_SEG(lpCallAddress);
    DPMI.ip  = FP_OFF(lpCallAddress);
					/*------------------------------*/
					/* Do it!			*/
					/*------------------------------*/
    err = DPMISimulateFarCall(&DPMI);
    if (err)
        DPMI.cflag = 0x01;		/* Set CY			*/

					/*------------------------------*/
					/* Copy results to output	*/
					/*------------------------------*/
    regs->x.ax = (UINT) DPMI.eax;
    regs->x.bx = (UINT) DPMI.ebx;
    regs->x.cx = (UINT) DPMI.ecx;
    regs->x.dx = (UINT) DPMI.edx;
    regs->x.si = (UINT) DPMI.esi;
    regs->x.di = (UINT) DPMI.edi;
    segregs->es= DPMI.es;
    segregs->ds= DPMI.ds;

    regs->x.cflag = DPMI.cflag;
					/*------------------------------*/
					/* Return AX			*/
					/*------------------------------*/
    return (regs->x.ax);
}

/*@API:**********************************************************************
@Declaration: BOOL  DPMISimulateInt (DPMIRec FAR * DPMIRecPtr)

@Description:
This function simulates an interrupt call using DPMI.  This function allows
non-virtualized interrupts to be executed, including passing and returning
variables.  The DPMIRec should be initialized to zero before any values are
inserted.  The IntWin function is preferable to calling this function directly.

@Parameters:
$DPMIRecPtr$ The DPMI register structure, properly intialized (use memset to
initialize to zero before placing values into the structure).

@Returns:
The return value is 1 if the there was an error executing the DPMI call (not
the actual software interrupt); 0 otherwise.

@Comments:
The return values are the status of the DPMI call, not the actual software
interrupt. To test to see if the simulated software interrupt worked, you need
to check the flags field in the DPMI structure.

@See: IntWin
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/
BOOL SYM_EXPORT WINAPI DPMISimulateInt(DPMIRec FAR *DPMIRecPtr)
{
    auto	BYTE	simulatedInt;
    auto        WORD    wError;

    simulatedInt = DPMIRecPtr->simulatedInt;

_asm	{
	push	es			// Save registers
	push	di			//
	push	cx			//
	push	bx			//

	les	di, DPMIRecPtr		// Load ES:DI with pointer to table
	xor	cx, cx			// Don't copy any words from protected
					//	stack to real mode stack
	mov	bl, simulatedInt	// Get the interrupt to call
	xor	bh, bh			// Don't reset interrupt controller
	mov	ax, 0300h		// Simulate Real Mode Interrupt
	int	31h			// DPMI interrupt
	jc	error			// If error, return error code

	xor	ax, ax			// Return FALSE (no error)
	jmp	short locRet		// Return to caller

error:
	mov	ax, 1			// Return TRUE (error)
locRet:
        mov     wError, ax

	pop	bx			// Restore registers
	pop	cx			//
	pop	di			//
	pop	es			//
	}
					/*------------------------------*/
					/* Change to preserve just the	*/
					/* CY state.			*/
					/*------------------------------*/
    DPMIRecPtr->cflag &= 0x01;

    return (wError);
}

/*@API:**********************************************************************
@Declaration: BOOL DPMISimulateFarCall(DPMIRec FAR *DPMIRecPtr)

@Description:
This function performs a far call simulation using DPMI.

@Returns:
The return value is 1 if the there was an error executing the DPMI call (not
the actual software interrupt); 0 otherwise.

@Comments:
The return values are the status of the DPMI call, not the actual far call. To
see if the simulated software interrupt worked, you need to check the flags
field in the DPMI structure.

@See: IntWin
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/
/*	NOTE: Before putting your values into DPMIRec, first memset	*/
/*	it to 0.							*/
BOOL SYM_EXPORT WINAPI DPMISimulateFarCall(DPMIRec FAR *DPMIRecPtr)
{
    auto        WORD    wError;

_asm	{
	push	es			// Save registers
	push	di			//
	push	cx			//
	push	bx			//

	les	di, DPMIRecPtr		// Load ES:DI with pointer to table
	xor	cx, cx			// Don't copy any words from protected
					//	stack to real mode stack
	xor	bx, bx			// Don't reset interrupt controller
	mov	ax, 0301h		// Simulate Real Mode Far Call
	int	31h			// DPMI interrupt
	jc	error			// If error, return error code

	xor	ax, ax			// Return FALSE (no error)
	jmp	short locRet		// Return to caller

error:
	mov	ax, 1			// Return TRUE (error)
locRet:
        mov     wError, ax

	pop	bx			// Restore registers
	pop	cx			//
	pop	di			//
	pop	es			//
	}
					/*------------------------------*/
					/* Change to preserve just the	*/
					/* CY state.			*/
					/*------------------------------*/
    DPMIRecPtr->cflag &= 0x01;

    return (wError);
}

/*@API:**********************************************************************
@Declaration: UINT  DPMISegmentToDescriptor (UINT wSegment)

@Description:
This function creates a protected mode selector for a real mode segment value
using DPMI.  The selector returned cannot be freed once allocated, and
selectors are scarce resources.  Temporary or "freeable" selectors can be
allocated through the SelectorAlloc function.

@Parameters:
$wSegment$ The real mode segment value to convert.

@Returns:
The return value is a valid descriptor, or zero if there was an error.

@See: SelectorAlloc
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/
UINT SYM_EXPORT WINAPI DPMISegmentToDescriptor(UINT wSegment)
{
    auto        UINT    wDesc;

    _asm	{

   //---------------------------------------------------------------------
   // Save registers
   //---------------------------------------------------------------------
                push	es
                push    ds
                push    di
                push    si
                push    cx
                push    bx

   //---------------------------------------------------------------------
   // Load registers and call DPMI interrupt
   //---------------------------------------------------------------------
                mov     ax, 0002h      // Function - 2
                mov     bx, wSegment   // bx = Real Mode segment address

                int     31h            // DPMI Interrupt
                jnc     exit1          // if no carry occured, then result is in AX

                xor     ax, ax         // error occured, return NULL
   //---------------------------------------------------------------------
   // Restore registers
   //---------------------------------------------------------------------

exit1:          mov     wDesc, ax

                pop     bx
                pop     cx
                pop     si
                pop     di
                pop     ds
                pop     es
                }

    return (wDesc);
}


/*----------------------------------------------------------------------*/
/* SelectorAlloc							*/
/*      Allocates a Selector returned in 'lpwSelector'.			*/
/*      Sets the REAL MODE Address for that selector using 'wSegment'	*/
/*       	and Sets the limit to the value of 'wLimit'.		*/
/*									*/
/*      NOTE: If the calling function needs an offset of other than 0,	*/
/*	then the caller must use:					*/
/*		lpPtr = MAKELP(wSelector, offset)			*/
/*      Additionally it is the callers responsibility to free the	*/
/*      Selector using SelectorFree().					*/
/*									*/
/*	Returns:							*/
/*	   0				No error			*/
/*	   ALLOC_SELECTOR_FAILED	Can't allocate selector		*/
/*	   SET_SELECTOR_BASE_FAILED	Can't set BASE			*/
/*	   SET_SELECTOR_LIMIT_FAILED	Can't set LIMIT			*/
/*----------------------------------------------------------------------*/

/*@API:**********************************************************************
@Declaration: UINT SelectorAlloc(UINT wSegment, UINT wLimit, UINT FAR *
lpwSelector)

@Description:
This function allocates and initializes a protected mode selector for a real
mode segment address.  The limit and the base address of the selector are set
to the specified values.  The selector must be freed using the SelectorFree()
function, not with the Windows FreeSelector function.  The selector's access
rights are copied from the current data segment.

@Parameters:
$wSegmemt$ The real mode segment address that a selector is created for.

$wLimit$ The limit for the selector.  This value cannot be greater than 0xFFFF
when running in standard mode.

$lpwSelector$ Pointer to the variable that is to receive the new selector.

@Returns:
The return value is zero if there was no error.  Otherwise, it is one of the
following constants:
    ~ALLOC_SELECTOR_FAILED~ Can't allocate a selector.
    ~SET_SELECTOR_BASE_FAILED~ Can't set the base address.
    ~SET_SELECTOR_LIMIT_FAILED~ Can't set limit.

@Comments:
If the calling function needs an offset of other than 0, then the caller must
use lpPtr = MAKELP(wSelector, offset). \n

Additionally it is the callers responsibility to free the selector using
SelectorFree().

@See: SelectorFree SelectorSetDosAddress
@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
UINT SYM_EXPORT WINAPI SelectorAlloc(UINT wSegment, UINT wLimit, UINT FAR * lpwSelector)
{
    auto	DWORD 		dwBaseAddress;
    auto	UINT 		wTemp;
    auto	UINT		wSelector;

   	      				/*------------------------------*/
					/* Just set the current value	*/
					/* to current DS, so we 	*/
					/* inherit the access rights.	*/
					/* This is why we then pass this*/
					/* value to AllocSelector().	*/
   	      				/*------------------------------*/
    _asm
        {
        les     bx,[lpwSelector]
        mov     ax, ds        		//  use DS to inherit access rights
        mov     es:[bx],ax
        }
   	      				/*------------------------------*/
					/* Allocate Selector		*/
   	      				/*------------------------------*/
    wSelector = AllocSelector(*lpwSelector);
    if (wSelector == NULL)
        return (ALLOC_SELECTOR_FAILED);

   	      				/*------------------------------*/
					/* Set the Base Address		*/
   	      				/*------------------------------*/
    dwBaseAddress = (DWORD) wSegment * 16L;
    wTemp = SetSelectorBase(wSelector, dwBaseAddress);
    if (wTemp != wSelector)
        return (SET_SELECTOR_BASE_FAILED);

   	      				/*------------------------------*/
					/* Set the Limit		*/
   	      				/*------------------------------*/
    wTemp = SetSelectorLimit(wSelector, wLimit);
    if (wTemp != 0)
        return (SET_SELECTOR_LIMIT_FAILED);

   	      				/*------------------------------*/
					/* Return the Selector		*/
   	      				/*------------------------------*/
    *lpwSelector = wSelector;

    return (0);
}


/*@API:**********************************************************************
@Declaration: UINT  SelectorFree (UINT wSelector)

@Description:
This function frees a selector that was allocated with SelectorAlloc.

@Parameters:
$wSelector$ Specifies the selector to free.

@Returns:
The return value is zero if there was no error.  Otherwise, it is set to
FREE_SELECTOR_FAILED.

@See: SelectorAlloc SelectorSetDosAddress

@Include: xapi.h

@Compatibility: Win16, Win32 (doesn't do a thing)
*****************************************************************************/
UINT SYM_EXPORT WINAPI SelectorFree(UINT wSelector)
{
    auto	UINT  	wResult;

    wResult = FreeSelector(wSelector);
    if (wResult != NULL)
        return (FREE_SELECTOR_FAILED);
    return (0);
}


/*@API:**********************************************************************
@Declaration: UINT  SelectorSetDosAddress(UINT FAR * lpwSelector, LPSTR
lpRealAddress)

@Description:
This function sets the base address of a selector to the specified address.

@Parameters:
$lpwSelector$ Pointer to the selector to be modified.

$lpRealAddress$ A real mode address.

@Returns:
The return value is zero if there was no error.

@See: SelectorGetDosAddress

@Include: xapi.h

@Compatibility: Win16
*****************************************************************************/
UINT SYM_EXPORT WINAPI SelectorSetDosAddress(UINT FAR * lpwSelector, LPSTR lpRealAddress)
{
    auto	DWORD		dwBaseAddress;

    dwBaseAddress = FP_SEG(lpRealAddress) * 16L + FP_OFF(lpRealAddress);

    SetSelectorBase(*lpwSelector, dwBaseAddress);

    return (0);
}

/*@API:**********************************************************************
@Declaration: UINT  SelectorGetDosAddress(UINT FAR * wSelector, LPSTR
lpRealAddress)

@Description:
This function returns the base address of a selector.

@Parameters:
$lpwSelector$ Pointer to the selector to be modified.

$lpRealAddress$ A real mode address.

@Returns:
The return value is zero if there was no error.

@See: SelectorSetDosAddress
@Include: xapi.h
@Compatibility: Win16
*****************************************************************************/
UINT SYM_EXPORT WINAPI SelectorGetDosAddress(UINT wSelector, LPSTR FAR *lpRealAddress)
{
    auto	DWORD		dwSelectorBase;

    dwSelectorBase = GetSelectorBase(wSelector);

    *lpRealAddress = MAKELP(dwSelectorBase / 16L, dwSelectorBase % 16L);

    return (0);
}

#if      defined(SYM_DOSX)

DWORD WINAPI GlobalDosAlloc(DWORD dwMem)
{
    auto	short		wOldStrat = d16SetMemStrat(MForceLow);
    auto	LPVOID		lpPMData;
    auto	LPVOID		lpRMData;

    lpPMData = d16AllocMem((WORD)dwMem);

    d16SetMemStrat(wOldStrat);

    if (lpPMData == NULL)
	return 0;

    lpRMData = d16GetRealAddress(lpPMData);

    if (lpRMData == NULL)
	{
	d16FreeMem(lpPMData);
	return 0;
	}

    FP_OFF(lpRMData) = FP_SEG(lpPMData);

    return (DWORD) lpRMData;
}

UINT WINAPI GlobalDosFree(UINT uSel)
{

    d16FreeMem(MAKELP(uSel, 0));

    return 0;
}

UINT    WINAPI AllocSelector(UINT uSource)
{
    auto	UINT		uSel;

    uSel = d16AllocDesc(1);

    if (uSel == 0 || uSource == 0)
	return (uSel);

    d16SetSegBase(uSel, d16GetSegBase(uSource));
    d16SetSegLimit(uSel, d16GetSegLimit(uSource));

    return (uSel);
}

UINT    WINAPI FreeSelector(UINT uSel)
{

    return (d16FreeDesc(uSel) != 0);
}

DWORD   WINAPI GetSelectorBase(UINT uSel)
{

    return (d16GetSegBase(uSel));
}

DWORD   WINAPI GetSelectorLimit(UINT uSel)
{

    return (d16GetSegLimit(uSel));
}

UINT    WINAPI SetSelectorBase(UINT uSel, DWORD dwBase)
{

    if (d16SetSegBase(uSel, dwBase) == 0)
	return (0);

    return (uSel);
}

UINT    WINAPI SetSelectorLimit(UINT uSel, DWORD dwLimit)
{
    return (d16SetSegLimit(uSel, dwLimit) == 0);
}

#define	MAXHOOKS	16

typedef void (_interrupt _far *PIHANDLER)(REGS16 regs);
typedef unsigned long REALPTR;

typedef struct INTMAP
    {
    WORD	wInt;
    REALPTR	lpRM;
    } INTMAP;

static INTMAP rHooks[MAXHOOKS];

void PASCAL _DXHookInterrupt(WORD wInt, PIHANDLER lpHandler, REALPTR FAR *lpOld, BOOL bHook)
{
    auto	UINT		i;

    _asm {

	mov	ax, WORD PTR lpOld
	or 	ax, WORD PTR lpOld+2
	jz 	setit
	mov	ax, wInt
	mov	ah, 35H
	int	21h
	mov	dx, es
	mov	ax, bx
	les	bx, lpOld
	mov	WORD PTR es:[bx], ax
	mov	WORD PTR es:[bx+2], dx
 setit:
	push	ds
	lds	dx, lpHandler
	mov	ax, wInt
	mov	ah, 25H
	int	21h
	pop	ds
    }

    if (bHook)
	{
	for (i = 0; i < MAXHOOKS; i++)
	    if (rHooks[i].wInt == 0)
		{
		rHooks[i].wInt = wInt;
		rHooks[i].lpRM = *lpOld;
		return;
		}
	}
    else
	{
	for (i = 0; i < MAXHOOKS; i++)
	    if (rHooks[i].wInt == wInt)
		{
		MEMSET(&rHooks[i], 0, sizeof(INTMAP));
		return;
		}
	}

    wInt = * ((LPBYTE) 0);
}

#endif  // defined(SYM_DOSX)

#elif defined(SYM_DOS)

/*----------------------------------------------------------------------*/
/* Offsets into stack when doing INT86x()				*/
/*----------------------------------------------------------------------*/
#define		OFFAX		0
#define		OFFBX		2
#define		OFFCX		4
#define		OFFDX		6
#define		OFFSI		8
#define		OFFDI		10
#define		OFFC		12

#define		OFFES		0
#define		OFFDS		6


/*----------------------------------------------------------------------*
 * Int()								*
 *									*
 * Like the C runtime int86x() but has parameters like IntWin().        *
 * That way, we have a single API, that will work under both DOS and    *
 * Windows.  Int() is macroed to IntWin() for DOSX and Windows.         *
 *                                                                      *
 * This code is from the 'C' library, with slight modifications.        *
 *----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI Int(BYTE intNum, union REGS FAR *regs, struct SREGS FAR *segregs)
{

	_asm
	{
	push	ds			; Save these registers
	push	di			;
	push	si			;

;----------------------------------------------------------------------
; the stack is made to look like this:
;
;	-------------------
;	|     outregs	  |  (arg3)
;	-------------------
;	|     inregs	  |  (arg2)
;	-------------------
;	|     intNum	  |  (arg1)
;	-------------------
;	|    ret addr	  |
;	-------------------
;  bp-> |     old bp	  |
;	-------------------
;	|     old di	  |
;	-------------------
;	|     old si	  |
;	-------------------
;	|     old ds	  |
;	-------------------
;	| 'retf'|	  |
;	-------------------
;	| 'inc sp inc sp' |  <- only in if int 25 or int 26
;	-------------------
;	| 'int'  | intNum |  <- we do a long call to 'int' opcode
;	-------------------
;	|	ss	  |
;	-------------------
; (bp)->| offset of 'int' |
;	-------------------
;----------------------------------------------------------------------

	sub	sp,10			; room for 5 words
	mov	byte ptr [bp-12],0cdh 	; an 'int' opcode
	mov	al,intNum
	mov	[bp-11],al		; the interrupt number
	cmp	al,25h
	je	abnorm
	cmp	al,26h
	je	abnorm
	mov	byte ptr [bp-10],0cbh 	; a 'retf' opcode
	jmp	short continue

abnorm:
	mov	byte ptr [bp-8],0cbh 	; a 'retf' opcode
	mov	byte ptr [bp-9],044h 	; a 'inc sp' opcode for flags on stack
	mov	byte ptr [bp-10],044h 	; a 'inc sp' opcode for flags on stack

continue:
	mov	[bp-14],ss		; seg part of far call addr
	lea	ax,[bp-12]		; offset part of far call addr
	mov	[bp-16],ax		; we're going to jump onto the stack

	;----------------------------------
	; Get Registers
	;----------------------------------
	lds	di,regs			; get union REGS addr

	mov	ax,[di+OFFAX]
	mov	bx,[di+OFFBX]
	mov	cx,[di+OFFCX]
	mov	dx,[di+OFFDX]
	mov	si,[di+OFFSI]
	push	[di+OFFDI]		; save di, need it for indexing

	;----------------------------------
	; Get Segments
	;----------------------------------
	lds	di,segregs		; get struct SREGS addr

	mov	es,[di+OFFES]
	mov	ds,[di+OFFDS]
	pop	di			; restore di
	push	bp

	;----------------------------------
	; Call the interrupt
	;----------------------------------
	call	dword ptr [bp-10h] 	; execute interrupt
	pop	bp			; bp points to old bp
	cld				; ensure that direction is "up"
	push	di			; save di
	push	ds

	;----------------------------------
	; Save Segment values
	;----------------------------------
	lds	di,segregs		; get struct SREGS addr

	mov	[di+OFFES],es
	pop	[di+OFFDS]

	;----------------------------------
	; Save Register values
	;----------------------------------
	lds	di,regs			; get union REGS addr

	mov	[di+OFFAX],ax
	mov	[di+OFFBX],bx
	mov	[di+OFFCX],cx
	mov	[di+OFFDX],dx
	mov	[di+OFFSI],si
	pop	[di+OFFDI]		; restore di
	jc	carry

	xor	si,si
	jmp	short toend

	;----------------------------------
	; Error.  Set the error number
	;----------------------------------
carry:
	mov	si,1

toend:
	mov	[di+OFFC],si		; set carry/no carry flag
	add	sp,10			; to get back to old sp

	pop	si			; Restore these registers
	pop	di			;
	pop	ds			;
	}
					/*------------------------------*/
					/* Return AX			*/
					/*------------------------------*/
    return (regs->x.ax);

}

#endif

