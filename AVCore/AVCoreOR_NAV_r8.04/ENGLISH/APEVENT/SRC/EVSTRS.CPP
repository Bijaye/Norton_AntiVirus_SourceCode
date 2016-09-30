// Copyright 1996 - 1998 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   N:/APEVENT/VCS/EVSTRS.CPv   1.2   17 Feb 1998 14:43:46   RStanev  $
//
// Description:
//      This file defines translatable strings.
//
// Contains:
//      szUnknownOwner[]
//
// See Also:
//      krnlstrs.cpp, uistrs.cpp, apstrs.cpp
//
// Notes:
//      Event Manager module.
//
//***************************************************************************
// $Log:   N:/APEVENT/VCS/EVSTRS.CPv  $
// 
//    Rev 1.2   17 Feb 1998 14:43:46   RStanev
// Updated copyright.
//
//    Rev 1.1   11 May 1997 23:52:10   RStanev
// Removed dependency on navap.h.
//
//    Rev 1.0   26 Feb 1997 04:40:40   RSTANEV
// Initial revision.
//
//***************************************************************************

#include "platform.h"
#include "evstrs.h"

//---------------------------------------------------------------------------
//
// szUnknownOwner[] is reported as an application generating an event, if
// SYMEvent was not able to determine the owner of the event.
//
// This string should be in OEM.
//
//---------------------------------------------------------------------------
TCHAR szUnknownOwner[] = _T("(no application name)");
