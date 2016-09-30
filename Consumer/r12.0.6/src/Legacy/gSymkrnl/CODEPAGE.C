// Copyright 1996 Symantec Corporation
//========================================================================
//
// $Header:   S:/SYMKRNL/VCS/CODEPAGE.C_v   1.0   08 Oct 1996 16:35:30   THOFF  $
//
// Description:
//
// See Also:
//
//------------------------------------------------------------------------
// $Log:   S:/SYMKRNL/VCS/CODEPAGE.C_v  $
// 
//    Rev 1.0   08 Oct 1996 16:35:30   THOFF
// Initial revision.
//------------------------------------------------------------------------

#include    "platform.h"
#include    "xapi.h"

WORD CDECL DOSGetCodePage(VOID)
    {
    COUNTRYCODE     Country    = {0};
    COUNTRYINFO     CtryBuffer = {0};
    ULONG           ulDataOut  = 0;

    DosQueryCtryInfo(sizeof(CtryBuffer), &Country, &CtryBuffer, &ulDataOut);

    return (WORD) CtryBuffer.codepage;
    }

