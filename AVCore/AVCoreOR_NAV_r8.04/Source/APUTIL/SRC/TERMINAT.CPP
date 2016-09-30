// Copyright 1993 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/navexclu/VCS/terminat.cpv   1.0   06 Feb 1997 20:57:56   RFULLER  $
//
// See Also:
//************************************************************************

#include "platform.h"
#include "xapi.h"
#include "avexcl.h"
#include "file.h"

STATUS SYM_EXPORT WINAPI ExcludeTerminate (
    LPEXCLUDE lpExclude
    )
{
    SYM_ASSERT (lpExclude);

    if ( lpExclude->lpPrivate )
        {
        if ( lpExclude->lpPrivate->hLExclude )
            {
            ExcludeLClose ( lpExclude->lpPrivate->hLExclude );
            }

        if ( lpExclude->hExcludeMem )
            {
            MemRelease ( lpExclude->hExcludeMem );
            MemFree ( lpExclude->hExcludeMem );
            lpExclude->hExcludeMem = NULL;

            lpExclude->lpPrivate->lpExcludeMem = NULL;

            if ( lpExclude->lpPrivate )
                {
                MemFreePtr ( lpExclude->lpPrivate );
                lpExclude->lpPrivate = NULL;
                }

            lpExclude->hExcludeMem = NULL;
            }
        }

    return ( NOERR );
}
