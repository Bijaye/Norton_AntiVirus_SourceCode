// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CERTLIBN/VCS/onehalfm.cpv   1.5   12 May 1997 12:34:34   MKEATIN  $
//
// Description:
// Contains:
// See Also:
//************************************************************************
// $Log:   S:/CERTLIBN/VCS/onehalfm.cpv  $
// 
//    Rev 1.5   12 May 1997 12:34:34   MKEATIN
// Don't include avapi.h
// 
//    Rev 1.4   09 May 1997 12:03:38   MKEATIN
// IsOnehalfInMemory now always returns FALSE.
// 
//    Rev 1.3   31 May 1995 20:03:28   SZIADEH
// added the virus id, and encrypted the virus signature.
// 
//    Rev 1.2   31 May 1995 16:46:52   SZIADEH
// use local callback struct.
// 
//    Rev 1.1   31 May 1995 12:11:32   SZIADEH
// prototype
// 
//    Rev 1.0   31 May 1995 11:41:12   SZIADEH
// Initial revision.
// 
//************************************************************************
#include "platform.h"

//========================================================================
//========================================================================
BOOL WINAPI IsOneHalfInMemory ( VOID )
{
    return (FALSE);
}

// end of file
