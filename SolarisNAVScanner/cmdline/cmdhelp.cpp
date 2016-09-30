// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/CMDHELP.CPv   1.1   04 Jun 1996 18:14:48   DCHI  $
//
// Description:
//  Contains function to display options help screen.
//
// Contains:
//  DisplayOptionsHelp()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/CMDHELP.CPv  $
// 
//    Rev 1.1   04 Jun 1996 18:14:48   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.0   22 May 1996 11:05:50   DCHI
// Initial revision.
// 
//************************************************************************

#include "avtypes.h"
#include "cmdhelp.h"

#include <stdio.h>

void DisplayOptionsHelp(void)
{
    extern          LPSTR       HelpStrings[];
    int                         i;

    for (i=0;HelpStrings[i] != NULL;i++)
        printf("%s\n",HelpStrings[i]);
}

