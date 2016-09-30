/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1998 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/SpecialDefUtils.cpv   1.0   17 Jun 1998 21:51:28   mdunn  $
//
//  NavInst.CPP -   Module that holds mostly install functions for the
//                  InstallShield installation of NAVNT; this is the main
//                  point of entry for the custom DLL.
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/SpecialDefUtils.cpv  $
// 
//    Rev 1.0   17 Jun 1998 21:51:28   mdunn
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#include    "platform.h"
#include    "navstart.h"
#include    "navver.h"
#include    "DefUtils.h"
#include    "SpecialDefUtils.h"


//*******************************************************************
//
// Function: CopyDefs()
//
// Description: This function returns the location of the newest 
//              defs.
//
//*******************************************************************
BOOL CSpecialDefUtils::CopyDefs(LPCSTR pszSourceDir, LPCSTR pszDestDir)
{
    return (CDefUtils::CopyDefs(pszSourceDir, pszDestDir));
}


//*******************************************************************
//
// Function: DefsAreNewerOrSameAge()
//
// Description: This function returns the location of the newest 
//              defs.
//
//*******************************************************************
BOOL CSpecialDefUtils::DefsAreNewerOrSameAge(LPCSTR pszDir1, LPCSTR pszDir2)
{
    return (CDefUtils::DefsAreNewerOrSameAge(pszDir1, pszDir2));
}