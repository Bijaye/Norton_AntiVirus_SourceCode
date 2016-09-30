/////////////////////////////////////////////////////////////////////////////
//  Copyright (c)1998 - Symantec, Peter Norton Computing Group
/////////////////////////////////////////////////////////////////////////////
//
//  $Header:   S:/NAVINSTNT/VCS/SpecialDefUtils.h_v   1.0   17 Jun 1998 21:51:30   mdunn  $
//
//  SpecialDefUtils.CPP -   Module that holds ... 
//
/////////////////////////////////////////////////////////////////////////////
//  $Log:   S:/NAVINSTNT/VCS/SpecialDefUtils.h_v  $
// 
//    Rev 1.0   17 Jun 1998 21:51:30   mdunn
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