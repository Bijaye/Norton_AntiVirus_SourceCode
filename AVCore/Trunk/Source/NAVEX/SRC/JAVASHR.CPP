//************************************************************************
//
// $Header:   S:/NAVEX/VCS/JAVASHR.CPv   1.3   26 Dec 1996 15:22:02   AOONWAL  $
//
// Description:
//      Shared routines for Java virus scan/repair.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/JAVASHR.CPv  $
// 
//    Rev 1.3   26 Dec 1996 15:22:02   AOONWAL
// No change.
// 
//    Rev 1.2   02 Dec 1996 14:01:36   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:58:32   AOONWAL
// No change.
// 
//    Rev 1.0   04 Apr 1996 14:55:58   DCHI
// Initial revision.
// 
//************************************************************************

#if 0

#include "javashr.h"

WORD WordConvert(WORD wIn)
{
    return ((wIn >> 8) + (wIn << 8));
}

DWORD DWordConvert(DWORD dwIn)
{
	DWORD a,b,c,d;

	a = dwIn >> 24;
	b = (dwIn >> 8) & 0x0000FF00UL;
	c = (dwIn << 8) & 0x00FF0000UL;
	d = dwIn << 24;

    return a | b | c | d;
}

#endif // #if 0

