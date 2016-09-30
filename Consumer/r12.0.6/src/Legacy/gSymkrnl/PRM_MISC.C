/* Copyright 1992 Symantec Corporation                                  */
/************************************************************************
 *                                                                      *
 * $Header:   S:/SYMKRNL/VCS/prm_misc.c_v   1.1   05 Feb 1996 08:40:58   RSTANEV  $ *
 *                                                                      *
 * Description:                                                         *
 *                                                                      *
 * Contains:                                                            *
 *      QuickSort                                                       *
 *      MathPower                                                       *
 *      MathIsPower2                                                    *
 *      MathDiff                                                        *
 *      MathLongDiff                                                    *
 *      NDosSetError                                                    *
 *      NDosGetError                                                    *
 *      MemoryNormalizeAddress                                          *
 *                                                                      *
 * See Also:                                                            *
 *                                                                      *
 ************************************************************************
 * $Log:   S:/SYMKRNL/VCS/prm_misc.c_v  $ *
// 
//    Rev 1.1   05 Feb 1996 08:40:58   RSTANEV
// Now working on VxD and NTK platforms.
//
//    Rev 1.0   26 Jan 1996 20:21:56   JREARDON
// Initial revision.
//
//    Rev 1.16   23 Aug 1994 18:07:06   BRAD
// Added MathPower()
//
//    Rev 1.15   07 Jun 1994 03:38:52   GVOGEL
// Added comment blocks for Geologist project.
//
//    Rev 1.14   04 May 1994 14:24:18   BRAD
// Added WIN32 versions of NDosSetError() and NDosGetError()
//
//    Rev 1.13   04 May 1994 10:22:38   MARKK
// Activate MemoryNormalizeAddress properly under DX
//
//    Rev 1.12   22 Mar 1994 10:43:44   BRUCE
// Fixed #pragmas and #if/#endif matching
//
//    Rev 1.11   21 Mar 1994 00:20:24   BRAD
// Cleaned up for WIN32
//
//    Rev 1.10   15 Mar 1994 12:33:56   BRUCE
// Changed EXPORT to SYM_EXPORT
//
//    Rev 1.9   25 Feb 1994 15:03:12   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
//
//    Rev 1.7   07 Sep 1993 20:51:58   JOHN
// Changed the Windows version of MemoryNormalizeAddress() to use DS as
// the template for the new selector it allocates.
//
//    Rev 1.6   07 Sep 1993 12:42:28   MARKK
// Changed return out of windows version of MemoryNormalizeAddress
//
//    Rev 1.5   03 Sep 1993 14:59:56   JOHN
// Changed MemoryNormalizeAddress() so it works differently on DOS and
// Windows.  On DOS it does what it always did: normalize a pointer using
// segment arithmatic.  On Windows it allocates a new selector to form
// a normalized pointer.  FreeNormalizedAddress() (a macro in XAPI.H)
// must be called to free the selector.
//
//    Rev 1.4   03 Aug 1993 20:50:26   DAVID
// Added safety check to wSwapCost parameter of QuickSort()
//
//    Rev 1.3   15 Feb 1993 21:04:36   TORSTEN
// Fixup for -W3/-WX.
//
//    Rev 1.2   12 Feb 1993 04:26:30   ENRIQUE
// No change.
//
//    Rev 1.1   10 Sep 1992 18:39:06   BRUCE
// Eliminated unnecesary includes
//
//    Rev 1.0   26 Aug 1992 17:00:48   ED
// Initial revision.
 ************************************************************************/

/*@Library: SYMKRNL*/

#include <dos.h>
#include "platform.h"
#include "undoc.h"
#include "xapi.h"

                                        // NOTE: This global variable should
                                        // be used to store any errors.
                                        // The NDosGetError()
                                        // function should be used to see
                                        // what the error was.
                                        // NDosSetError() should be used to
                                        // set the value.
#if !defined(SYM_WIN32) && !defined(SYM_NTK) && !defined(SYM_VXD)
static UINT wLastError = 0;
#endif

/**********************************************************************
** Typedefs
**********************************************************************/

typedef struct                          // One subrange rec for stack
    {
    HPBYTE hpBase;
    HPBYTE hpLimit;
    }
    SORTRANGE;

typedef SORTRANGE FAR *LPSORTRANGE;


/**********************************************************************
**
**
** QuickSort (hpBase, wNumItems, wSize,
**            fnCompare, wParam, lParam, fnSwap, wSwapCost)
**
** Description:
**      Implements the quicker-sort algorithm.
**
** Input:
**      hpBase          Huge pointer to array of elements.
**      wNumItems       Number of elements to sort.
**      wSize           Size of an element in bytes.
**      fnCompare       Pointer to function receiving huge pointers to
**                       pair of elements to compare; returns value
**                       that is <0, 0, or >0
**      wParam          Value passed to compare calls.
**      lParam          Value passed to compare calls.
**      fnSwap          Pointer to function receiving huge pointers to
**                      pair of elements to swap.
**      wSwapCost       Cost of a swap relative to a compare; for
**                       example, 3 = cheap, 10 exteremely expensive.
**                       Blocks of this many items are insertion sorted.
**
**********************************************************************/

void SYM_EXPORT WINAPI QuickSort (HPBYTE hpBase, UINT wNumItems, UINT wSize,
                           COMPPROC fnCompare, UINT wParam, DWORD lParam,
                           SWAPPROC fnSwap, UINT wSwapCost)
{
    SORTRANGE   stack [25];             /* Stack of 25 is very generous */
    LPSORTRANGE lpStackBase;
    LPSORTRANGE lpStackTop;
    long        lThreshold;
    HPBYTE      hpItemA;
    HPBYTE      hpItemB;
    HPBYTE      hpLimit;
    HPBYTE      hpPivot;


    lpStackBase = stack;
    lpStackTop = stack;
    if (wSwapCost < 3)
        wSwapCost = 3;
    lThreshold = (long) wSwapCost * wSize;
    hpLimit = hpBase + (DWORD) wNumItems * wSize;

    while (TRUE)
        {
        /* Repeat the QuickSort algorithm until the subrange
         * is small enough to use an alternate algorithm.  */

        while ((long)(hpLimit - hpBase) > lThreshold)
            {
            /* hpBase points to the first item in the subrange.
             * hpLimit points to the first item past the subrange.
             * Find the pivot item in the middle of the subrange. */

            hpPivot = hpBase + (((hpLimit - hpBase) >> 1) / wSize) * wSize;

            (*fnSwap)(hpPivot, hpBase);

            hpItemA = hpBase + wSize;   /* 'A' scans from left to right */
            hpItemB = hpLimit - wSize;  /* 'B' scans from right to left */

            /* Sort three elements so that *hpBase
             * is the pivot element and 'A' <= *hpBase <= 'B'  */

            if ((*fnCompare)(hpItemA, hpItemB, wParam, lParam) > 0)
                (*fnSwap)(hpItemA, hpItemB);
            if ((*fnCompare)(hpBase, hpItemB, wParam, lParam) > 0)
                (*fnSwap)(hpBase, hpItemB);
            if ((*fnCompare)(hpItemA, hpBase, wParam, lParam) > 0)
                (*fnSwap)(hpItemA, hpBase);

            while (TRUE)
                {
                /* While 'A' < pivot item, move 'A' to the right */

                do  {
                    hpItemA += wSize;
                    } while ((*fnCompare)(hpItemA, hpBase, wParam, lParam) < 0);

                /* While 'B' > pivot item, move 'B' to the left */

                do  {
                    hpItemB -= wSize;
                    } while ((*fnCompare)(hpItemB, hpBase, wParam, lParam) > 0);

                /* If 'A' and 'B' went past each other, exit loop.
                 * Otherwise exchange the 'A' and 'B' items */

                if (hpItemA > hpItemB)
                    break;
                (*fnSwap)(hpItemA, hpItemB);
                }

            (*fnSwap)(hpBase, hpItemB); /* put pivot where it belongs   */

            /* Stack the larger of the two subranges and
             * proceed to sort the smaller subrange */

            if (hpItemB - hpBase > hpLimit - hpItemA)
                {

                lpStackTop->hpBase  = hpBase;
                lpStackTop->hpLimit = hpItemB;
                hpBase = hpItemA;
                }
            else                            /* right subrange is larger */
                {
                lpStackTop->hpBase  = hpItemA;
                lpStackTop->hpLimit = hpLimit;
                hpLimit = hpItemB;
                }
            lpStackTop++;
        }


        /* At this point the subrange has wSwapCost or fewer items.
         * Now do an insertion sort on these items. */

        hpItemA = hpBase + wSize;
        while (hpItemA < hpLimit)
            {
            hpItemB = hpItemA;
            while (hpItemB > hpBase &&
                   (*fnCompare)(hpItemB - wSize, hpItemB, wParam, lParam) > 0)
                {
                (*fnSwap)(hpItemB - wSize, hpItemB);
                hpItemB -= wSize;
                }
            hpItemA += wSize;
            }

        /* If there are any subranges left on the stack,
         * pop off the next one and sort it. */

        if (lpStackTop == lpStackBase)
            break;

        lpStackTop--;
        hpBase  = lpStackTop->hpBase;
        hpLimit = lpStackTop->hpLimit;
    }
}


/*----------------------------------------------------------------------*/
/*      Returns the value x to the power y.                             */
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI MathPower (UINT uX, UINT uY)
{
    UINT        i;
    DWORD       dwRetValue;


    if (uY == 0)
        dwRetValue = 0;
    else
        {
        dwRetValue = uX;
        for (i = 0; i < uY - 1; i++)
            dwRetValue *= uX;
        }
    return(dwRetValue);
}

/*----------------------------------------------------------------------*/
/*      Determines if a UINT is a power of 2.                           */
/*----------------------------------------------------------------------*/

BOOL SYM_EXPORT WINAPI MathIsPower2 (register UINT num)
{
    register UINT count = 0;

    while (num)
        {
        if (num & 0x0001)
            count++;

        num >>= 1;
        }
    return ((BOOL) (count == 1));
}


/*----------------------------------------------------------------------*/
/* Computes the difference between 2 Word numbers                       */
/*      Note that everything is Word (UINT)                             */
/*----------------------------------------------------------------------*/

UINT SYM_EXPORT WINAPI MathDiff (UINT number1, UINT number2)
{
    if (number1 > number2)
        return (number1 - number2);
    else
        return (number2 - number1);
}



/*----------------------------------------------------------------------*/
/* Computes the absolute difference between 2 Ulong numbers             */
/*      Note that everything is Ulong                                   */
/*----------------------------------------------------------------------*/

DWORD SYM_EXPORT WINAPI MathLongDiff (DWORD number1, DWORD number2)
{
    if (number1 > number2)
        return (number1 - number2);
    else
        return (number2 - number1);
}

/*@API:**********************************************************************
@Declaration: void  NDosSetError (UINT wError)

@Description:
This routine is used to retain the error which occurred on the last failed DOS
call.  The DOSGetErrorInfo or DOSGetExtendedError function should be called
immediately after an error, but BEFORE this function is called.  The error
value is maintained until the next call to this function.  The value can be
retrieved with the NDosGetError function.

@Parameters:
$wError$ The DOS error value to store.

@See: NDosGetError DOSGetErrorInfo DOSGetExtendedError
@Include: xapi.h
@Compatibility: DOS, Win16, Win32, NTK and VxD
*****************************************************************************/
/* The int 21 function 59 should be used                */
/*    IMMEDIATELY after getting the error and BEFORE calling this function.  */
void SYM_EXPORT WINAPI NDosSetError(UINT wError)
{
#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
    SetLastError(wError);
#else
    wLastError = wError;
#endif
}

/*@API:**********************************************************************
@Declaration: UINT  NDosGetError()

@Description:
This function returns the error code stored with the NDosSetError function.
Functions which set the error code on failure are: FileDelete, FileRename,
FileFindFirst, FileFindNext, FileGetAttr, FileSetAttr, FileGetDateTime,
FileSetDateTime, FileFlush, FileCommit, FileDup. \n

This function has no parameters.

@Returns:
The return value is the value of the last error code saved by the NDosSetError
function.

@See: NDosSetError
@Include: xapi.h
@Compatibility: DOS, Win16, Win32, NTK and VxD
*****************************************************************************/
UINT SYM_EXPORT WINAPI NDosGetError(void)
{
#if defined(SYM_WIN32) || defined(SYM_VXD) || defined(SYM_NTK)
    return ((UINT) GetLastError());
#else
    return(wLastError);
#endif
}

/*@API:**********************************************************************
@Declaration: LPVOID MemoryNormalizeAddress(LPVOID address)

@Description:
[[What does this function do?]]

@Include: xapi.h
@Compatibility: DOS, Win16 and Win32
*****************************************************************************/
#if !defined(SYM_32BIT)                 // macrod in XAPI.H
#pragma optimize("leg", off)
LPVOID SYM_EXPORT WINAPI MemoryNormalizeAddress(LPVOID address)
{
#if defined(SYM_PROTMODE)

// Windows version needs to allocate a new selector because
// we can't do arithmatic on huge pointers.
    auto	UINT		wSelector;
    auto	DWORD		dwBase;

    // Inherit new selector access rights from data segment
    _asm mov [wSelector],ds;

    // Allocate new selector for normalized pointer
    wSelector = AllocSelector(wSelector);
    if (wSelector == NULL)
	return(NULL);

    // Get linear address of input pointer
    dwBase = GetSelectorBase(FP_SEG(address));
    dwBase += FP_OFF(address);

    // Set base and limit of new normalized pointer
    SetSelectorBase(wSelector, dwBase);
    SetSelectorLimit(wSelector, 0xFFFF);

    // Return normalized pointer
    FP_SEG(address) = wSelector;
    FP_OFF(address) = 0;
    return(address);

#else

// Real Mode version just plays with the segment and offset
    _asm
        {
        mov     ax,word ptr address
        mov     dx,word ptr address+2
        mov     bx,ax
        shr     bx,1
        shr     bx,1
        shr     bx,1
        shr     bx,1
        add     dx,bx
        and     ax,0Fh
        mov     word ptr address,ax
        mov     word ptr address+2,dx
        }

    return (address);
#endif // SYM_WIN
}
#pragma optimize("", on)

#endif

