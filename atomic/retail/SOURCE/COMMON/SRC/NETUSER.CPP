// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/common/VCS/netuser.cpv   1.0   06 Feb 1997 20:50:06   RFULLER  $
//
// Description:
//      Contains common code re: the Network User.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/common/VCS/netuser.cpv  $
// 
//    Rev 1.0   06 Feb 1997 20:50:06   RFULLER
// Initial revision
// 
//    Rev 1.0   01 Mar 1996 18:26:38   BARRY
// Initial revision.
// 
//    Rev 1.0   30 Jan 1996 09:55:30   BARRY
// Initial revision.
// 
//    Rev 1.3   13 Sep 1995 10:46:22   JWORDEN
// Add include for navutil.h
// 
//    Rev 1.2   09 May 1994 15:40:10   DALLEE
// include common.h so NetUserAlreadyExists() doesn't go through name-mangling.
// 
//    Rev 1.1   21 Apr 1994 11:08:44   MFALLEN
// Use WINAPI instead of PASCAL
// 
//    Rev 1.0   20 Apr 1994 15:16:18   DALLEE
// Initial revision.
//
// From NIRVANA netuser.c 1.0
//************************************************************************

#include "platform.h"
#include "navutil.h"
#include "options.h"
#include "common.h"

/**************************************************************************
 * NetUserAlreadyExists
 *
 * DESCRIPTION  : Make sure we don't have duplicate entries in the list.
 *
 * IN      :
 *
 * RETURN VALUES:
 *************************************************************************
 * SKURTZ : 05-14-93 at 05:40:06pm Function created
 ************************************************************************* */

BOOL WINAPI NetUserAlreadyExists(LPNETUSERITEM lpNetUserItem, LPNETUSER lpNetUser)
{
    auto        UINT            i;
    auto        LPNETUSERITEM   lpCurrentNetUserItem;

                                        // See if this entry already exists
    for (i = 0, lpCurrentNetUserItem = lpNetUser->lpNetUserItem; 
         i < lpNetUser->uEntries; 
         lpCurrentNetUserItem++, i++)
        {
        if ( MEMCMP(lpNetUserItem, lpCurrentNetUserItem, sizeof(NETUSERITEM)) == 0)
            return (TRUE);
        }
    return (FALSE);
}


