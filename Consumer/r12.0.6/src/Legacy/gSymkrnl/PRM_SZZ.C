// Copyright 1993 Symantec Corporation                                  
//***************************************************************************
// 
// $Header:   S:/SYMKRNL/VCS/PRM_SZZ.C_v   1.0   26 Jan 1996 20:23:04   JREARDON  $ 
// 
// Description:
//      These functions provide basic manipulation of szz strings.  It
//      only copes with true szz strings (i.e., even an empty list must
//      have two EOS's).  For any function returning an LPSTR, you must
//      watch for it returning a pointer to EOS -- this signals the end
//      of the list (i.e., while(*lpszzList){} ).
// 
// Contains:
//      SzzStripMatches()       
//      SzzStripDuplicates()    
//      SzzRemoveEntry()
//      SzzFindEnd()
//      SzzFindNext() (actually a macro in the header file)
// 
// See Also:                                                            
// 
//***************************************************************************
// $Log:   S:/SYMKRNL/VCS/PRM_SZZ.C_v  $ 
// 
//    Rev 1.0   26 Jan 1996 20:23:04   JREARDON
// Initial revision.
// 
//    Rev 1.4   15 Mar 1994 12:33:32   BRUCE
// Changed EXPORT to SYM_EXPORT
// 
//    Rev 1.3   25 Feb 1994 12:23:20   BRUCE
// Ran scripts to change to SYM_ constants and get ready for Win32
// 
//    Rev 1.2   14 Aug 1993 17:57:10   BRAD
// No change.
// 
//    Rev 1.1   06 Aug 1993 01:53:18   BARRY
// Fixed problems with LPSTR vs. LPCSTR
// 
//    Rev 1.0   06 Aug 1993 01:18:06   BARRY
// Initial revision.
//***************************************************************************

#include <dos.h>
#include "platform.h"
//#include "undoc.h"
#include "xapi.h"

//***************************************************************************
// SzzStripMatches
// 
// Description: Strip matching entries from Szz List.
//              All comparisons are caseless.
// 
// Parameters:
//      lpszzList -- A szz list of strings
//      lpszMatch -- String to match (can be part of lpszzList)
//      bSubStr   -- If TRUE, will match substrings (where lpszMatch is
//                   shorter than lpszzList string)
// 
// Return Value: 
//      Returns the number of matching strings removed
// 
// See Also: 
// 
//***************************************************************************
// 08/05/1993 BARRY Function Created.                                   
//***************************************************************************

UINT SYM_EXPORT WINAPI SzzStripMatches ( LPSTR lpszzList , LPCSTR lpszMatch , BOOL bSubStr )
{
    auto BOOL   bMatch ;
    auto UINT   uMatchLen ;
    auto UINT   uMatchCount ;
    auto char   szLocalMatch [ SYM_MAX_PATH + 1 ] ;

    // Make copy of match in case it's part of lpszzList 

    StringMaxCopy ( szLocalMatch , lpszMatch , sizeof ( szLocalMatch ) ) ;
    uMatchLen   = STRLEN ( szLocalMatch ) ;
    uMatchCount = 0 ;

    while ( * lpszzList )
        {
        bMatch = FALSE ;

        if ( ! bSubStr && STRICMP ( lpszzList , szLocalMatch ) == 0 )
            bMatch = TRUE ;
        else
        if ( bSubStr && STRNICMP ( lpszzList , szLocalMatch , uMatchLen ) == 0 )
            bMatch = TRUE ;

        if ( bMatch )
            {
            uMatchCount += 1 ;
            SzzRemoveEntry ( lpszzList ) ;
            }
        else
            lpszzList = (LPSTR) SzzFindNext ( (LPCSTR) lpszzList ) ;
        }

    return uMatchCount ;
}

//***************************************************************************
// SzzStripDuplicates
// 
// Description:
//      Removes duplicate strings in szz string.  Compares are caseless
// 
// Parameters:
//      lpszzList -- szz string to strip
// 
// Return Value: none
// 
// See Also: SzzStripMatches
// 
//***************************************************************************
// 08/05/1993 BARRY Function Created.                                   
//***************************************************************************

void SYM_EXPORT WINAPI SzzStripDuplicates ( LPSTR lpszzList )
{
    auto LPSTR  lpszzNext ;

    while ( * lpszzList )
        {
        lpszzNext = SzzFindNext ( lpszzList ) ;
        SzzStripMatches ( lpszzNext , lpszzList , FALSE ) ;
        lpszzList = lpszzNext ;
        }
}

//***************************************************************************
// SzzRemoveEntry
// 
// Description:
//      Removes the current string from an szz string.
//      Points to first EOS if removing last string so SzzFindNext() 
//      will work without checking this result first.
// 
// Parameters:   lpszzList
// 
// Return Value: void
// 
// See Also: 
// 
//***************************************************************************
// 08/05/1993 BARRY Function Created.                                   
//***************************************************************************

void SYM_EXPORT WINAPI SzzRemoveEntry ( LPSTR lpszzList )
{
    auto LPSTR  lpszzNext ;             // Points to start of next string  

    lpszzNext = SzzFindNext ( lpszzList ) ;

    if ( * lpszzNext  )                 // Watch for terminator
        {
        _fmemmove ( lpszzList , lpszzNext ,
                    (SzzFindEnd ( lpszzNext ) - lpszzNext) + 1 ) ;
        }
    else                                // Keep szz because caller may
        {                               // call SzzFindNext() on return.
        * lpszzList ++ = '\0' ;
        * lpszzList    = '\0' ;
        }
}

//***************************************************************************
// SzzFindNext ()
// 
// Description: Return next string in list or pointer to '\0' terminator
//***************************************************************************
// 08/06/1993 BARRY Function Created.                                   
//***************************************************************************

LPSTR SYM_EXPORT WINAPI SzzFindNext ( LPCSTR lpszzList )
{
    return (LPSTR) (lpszzList + STRLEN ( lpszzList ) + 1) ;
}

//***************************************************************************
// SzzFindEnd
// 
// Description:
//      Finds the EOS terminator for a szz string
//      NOTE:  Never pass this value to another Szz* routine because it
//             will go past the end.
// 
// Parameters:
//      lpszzList
// 
// Return Value: 
//      Pointer to last EOS in szz string
// 
// See Also: 
// 
//***************************************************************************
// 08/05/1993 BARRY Function Created.                                   
//***************************************************************************

LPSTR SYM_EXPORT WINAPI SzzFindEnd ( LPCSTR lpszzList )
{
    while ( * lpszzList )
        lpszzList = SzzFindNext ( lpszzList ) ;

    return (LPSTR) lpszzList ;
}
