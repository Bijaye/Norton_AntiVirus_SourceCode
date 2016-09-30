/*--------------------------------------------------------------------------*/
/* $Header:   S:/SYMKRNL/VCS/HSTRING.C_v   1.0   26 Jan 1996 20:22:44   JREARDON  $*/
/*                                                                          */
/* HSTRING.C							            */
/*                                                                          */
/*	Contains routines for manipulating huge strings.                    */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* $Log:   S:/SYMKRNL/VCS/HSTRING.C_v  $*/
// 
//    Rev 1.0   26 Jan 1996 20:22:44   JREARDON
// Initial revision.
// 
//    Rev 1.5   15 Mar 1994 12:34:40   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.4   25 Feb 1994 15:05:10   BRUCE
// Reinstated FP_SEG, FP_OFF, MAKELP
// 
//    Rev 1.3   25 Feb 1994 12:23:16   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
// 
//    Rev 1.2   22 Apr 1993 16:11:06   MFALLEN
// 
//    Rev 1.1   22 Apr 1993 15:13:42   MFALLEN
// Fixed bug with hstrlen()
// 
//    Rev 1.0   22 Apr 1993 14:15:16   MFALLEN
// Initial revision.
// 
//    Rev 1.0   20 Aug 1992 00:04:54   BRAD
// Initial revision.

#include	"platform.h"
#include <string.h>
#include	"undoc.h"
#include "hstring.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
int SYM_EXPORT WINAPI hstrcmp (HPSTR hpStr1, HPSTR hpStr2)
{
    for ( ; *hpStr1 == *hpStr2; hpStr1++, hpStr2++)
        if ( *hpStr1 == EOS )
            return (0);

    return (*hpStr1 - *hpStr2);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
HPSTR SYM_EXPORT WINAPI hstrcpy(HPSTR hpDest, HPSTR hpSource)
{
    auto        HPSTR   hpStart = hpDest;


    while ((*hpDest++ = *hpSource++) != EOS )
        ;

    return (hpStart);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
DWORD SYM_EXPORT WINAPI hstrlen(HPSTR hpStr)
{
    auto    DWORD     dwLen = 0;

    while ( *hpStr++ != EOS )
        dwLen++;

    return(dwLen);
}



/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
HPSTR SYM_EXPORT WINAPI hstrcat(HPSTR hpDest, HPSTR hpSource)
{
    auto        HPSTR   hpStart = hpDest;
    
                                        // Move to the end of 1st string
    while ( *hpDest++ != EOS )
        ;
    hpDest--;                           // 1 character too far

                                        // Copy 2nd string over
    while ((*hpDest++ = *hpSource++) != EOS )
        ;

    return (hpStart);
}


/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
WORD SYM_EXPORT WINAPI hGetWord(HPBYTE hpbyValue)
{
   auto         BYTE    byLo, byHi;

   byLo = *hpbyValue;
   byHi = *(hpbyValue + 1);
   return ( (((WORD) byHi) << 8) + byLo );
}

