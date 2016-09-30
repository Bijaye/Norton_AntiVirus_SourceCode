// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/CMDLINE/VCS/ext.cpv   1.5   27 Aug 1996 15:15:16   DCHI  $
//
// Description:
//  Contains file name extension checking function.
//
// Contains:
//  ValidScanExtension()
//
// See Also:
//
//************************************************************************
// $Log:   S:/CMDLINE/VCS/ext.cpv  $
// 
//    Rev 1.5   27 Aug 1996 15:15:16   DCHI
// Addition of .XL? to list of extensions.
// 
//    Rev 1.4   12 Aug 1996 18:46:48   DCHI
// Modifications for UNIX.
// 
//    Rev 1.3   06 Jun 1996 14:02:56   RAY
// Increased szExtension by one byte.
// 
//    Rev 1.2   04 Jun 1996 18:14:56   DCHI
// Modifications to conform to new header file structure.
// 
//    Rev 1.1   03 Jun 1996 17:21:02   DCHI
// Changes to support building on BORLAND, VC20, SYM_W32, SYM_DOS, and SYM_DOSX.
// 
//    Rev 1.0   21 May 1996 12:25:50   DCHI
// Initial revision.
// 
//************************************************************************

#include <string.h>

#include "avtypes.h"

#ifdef SYM_DOS

extern "C" {
char * __cdecl _strupr(char *);
}

#define strupr      _strupr

#endif // SYM_DOS

#ifdef SYM_UNIX

#include <ctype.h>

char *strupr(char *s)
{
    char *psz = s;

    while (*psz)
    {
        *psz = toupper(*psz);
	++psz;
    }

    return(s);
}

#endif // SYM_UNIX

LPSTR gExecutableExtensionList[] = { ".386",
                                     ".BIN",
                                     ".CLA",
                                     ".COM",
                                     ".CPL",
                                     ".DLL",
                                     ".DOC",
                                     ".DOT",
                                     ".DRV",
                                     ".EXE",
                                     ".NCP",
                                     ".NED",
                                     ".NNL",
                                     ".OCX",
                                     ".OV?",
                                     ".SCR",
                                     ".SYS",
                                     ".VBX",
                                     ".VXD",
                                     ".XL?"
                                   };


#define NUM_EXTENSION_ITEMS     (sizeof(gExecutableExtensionList) / sizeof(LPSTR))
#define EXTENSION_LEN           4       // 1 byte for . and 3 for extension

//*************************************************************************
//
// Function:
//  ValidScanExtension()
//
// Parameters:
//  lpszFileName    File name for which to check the extension
//
// Description:
//  Looks at the extension of lpszFileName and returns TRUE if the
//  extension matches any of the extensions in gExecutableExtensionList.
//
// Returns:
//  FALSE           If the name does not have an extension in the list
//  TRUE            If the name does have an extension in the list
//
//*************************************************************************

BOOL ValidScanExtension
(
    LPSTR           lpszFileName
)
{
    int             i, j, nStrLen;
    LPSTR           lpszExt;
    char            szExtension[EXTENSION_LEN + 1];

    nStrLen = strlen(lpszFileName);

    // does this filename even have an extension?  If not, it can't be an
    // executable file.  Don't scan it!

    if (nStrLen < EXTENSION_LEN)
        return(FALSE);

    // point at the most probable location of the extension.

    lpszExt = lpszFileName + nStrLen - EXTENSION_LEN;

    strcpy(szExtension, lpszExt);
    strupr(szExtension);

    for (i=0;i<NUM_EXTENSION_ITEMS;i++)
    {
        for (j=0;j<EXTENSION_LEN;j++)
            if (szExtension[j] != gExecutableExtensionList[i][j] &&
                gExecutableExtensionList[i][j] != '?')
                break;

        if (j == EXTENSION_LEN)
            return(TRUE);
    }

    return(FALSE);
}


