// Copyright 1997 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/NAVC/VCS/cdefwrap.cpv   1.2   20 Jul 1998 14:34:32   TCASHIN  $ 
// 
// Description:                                                         
//     Need to hide the CPP stuff in its own file to avoid link 
//     problems in NAVC
// 
// Contains:                                                            
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/NAVC/VCS/cdefwrap.cpv  $ 
// 
//    Rev 1.2   20 Jul 1998 14:34:32   TCASHIN
// Changed the declaration of GetCDefUtilsDir() o fix a calling convention
// mismatch.
// 
//    Rev 1.1   11 Jun 1997 18:18:28   BGERHAR
// Add platform.h to fix defutils.h change
// 
//    Rev 1.0   01 Jun 1997 20:34:48   BGERHAR
// Initial revision.
//***************************************************************************

#include "platform.h"
#include "defutils.h"

extern "C" BOOL GetCDefUtilsDir(char *lpszDefsDir);

BOOL "C" GetCDefUtilsDir(char * lpszDefsDir)
{
    CDefUtils du;

    if (du.InitDOSApp(lpszDefsDir))
        {
        if (du.UseNewestDefs(lpszDefsDir, SYM_MAX_PATH))
            return TRUE;
        }

    return FALSE;
}
