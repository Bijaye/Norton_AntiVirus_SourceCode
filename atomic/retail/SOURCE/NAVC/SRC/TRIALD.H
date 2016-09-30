// Copyright 1997 Symantec, Peter Norton Computing Group
// -------------------------------------------------------------------------
// TRYINT.H
// Declaration of CTrialWare object
//
// $Header:   S:/NAVC/VCS/triald.h_v   1.0   10 Jul 1997 13:40:40   MKEATIN  $
// -------------------------------------------------------------------------
// $Log:   S:/NAVC/VCS/triald.h_v  $
// 
//    Rev 1.0   10 Jul 1997 13:40:40   MKEATIN
// Initial revision.
// 
//    Rev 1.0   26 Feb 1997 16:36:12   MKEATIN
// Initial revision.
// 
//    Rev 1.0   23 Jan 1997 19:22:38   JBELDEN
// Initial revision.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __TRYINT_H__
#define __TRYINT_H__

#include "platform.h"

int Initialize( LPCTSTR, LPCTSTR, UINT, UINT);
int Setup( DWORD, DWORD );
int IsValid( BOOL, LPDWORD );


#endif // __TRYINT_H__