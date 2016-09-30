// Copyright 1993 Symantec Corporation                                  *
//***********************************************************************
//                                                                      *
// $Header:   S:/NPSCHECK/VCS/npscheck.cpv   1.0   17 Apr 1998 11:37:26   mdunn  $
//                                                                      *
// Description:                                                         *
//    This is the NPSCheck app that notifies the NPS service when a user
//    logs in.
//                                                                      *
//***********************************************************************
// $Log:   S:/NPSCHECK/VCS/npscheck.cpv  $
// 
//    Rev 1.0   17 Apr 1998 11:37:26   mdunn
// Initial revision.
//***********************************************************************

#include <platform.h>
#include "stdhelp.h"
#include "nps.h"

int WINAPI WinMain ( HINSTANCE hInst, HINSTANCE hPrevInst,
                     LPSTR lpCmdLine, int nCmdShow )
{
HANDLE hLoginEvent;

                                        // Open a handle to the login event
                                        // object.
    hLoginEvent = CreateEvent ( NULL, FALSE, FALSE,
                                NPS_LOGIN_EVENT_NAME );

    if ( NULL != hLoginEvent )
        {
        SetEvent ( hLoginEvent );       // Signal NPSSVC to check for old events
        CloseHandle ( hLoginEvent );
        }

    return 0;
}

