/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1998 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINST95/VCS/specialdefutils.h_v   1.0   18 Jun 1998 00:01:34   tcashin  $
//
//  SpecialDefUtils.CPP -   Module that holds ... 
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINST95/VCS/specialdefutils.h_v  $
// 
//    Rev 1.0   18 Jun 1998 00:01:34   tcashin
// Initial revision.
/////////////////////////////////////////////////////////////////////////////

#ifndef  _SPECICAL_DEFUTILS_H_
#define  _SPECICAL_DEFUTILS_H_

#include    "platform.h"
#include    "navstart.h"
#include    "navver.h"
#include    "DefUtils.h"

/////////////////////////////////////////////////////////////////////////////
//  Classes
/////////////////////////////////////////////////////////////////////////////

class CSpecialDefUtils : public CDefUtils
{
public:

    BOOL CopyDefs(LPCSTR pszSourceDir, LPCSTR pszDestDir);
    BOOL DefsAreNewerOrSameAge(LPCSTR pszDir1, LPCSTR pszDir2);

};


#endif