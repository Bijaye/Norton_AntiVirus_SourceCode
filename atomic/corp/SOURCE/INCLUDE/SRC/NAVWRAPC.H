// Copyright (C) 1998 Symantec Corporation
//*****************************************************************************
// $Header:   S:/INCLUDE/VCS/NAVWRAPC.H_v   1.0   29 May 1998 14:35:44   DBuches  $
//
// Description: Contains headers for functions exported from NAVWRAPC.DLL.
//
//*****************************************************************************
// $Log:   S:/INCLUDE/VCS/NAVWRAPC.H_v  $
// 
//    Rev 1.0   29 May 1998 14:35:44   DBuches
// Initial revision.
// 
//    Rev 1.1   07 May 1998 07:59:50   TSmith
// Added hWnd and lpHelpFile parameters to DisplayNAVOptionsDialog declaration.
// 
//    Rev 1.0   30 Mar 1998 15:59:14   TSmith
// Initial revision.
//*****************************************************************************
#if !defined( __NavWrapC_h__ )
#define       __NavWrapC_h__



/*****************************************************************************
FUNCTION:
    DisplayNAVOptionsDialog
    
DESCRIPTION:
    Loads NAV options from the specifed source directory, displays the NAV
    property pages to allow modification of the options and writes the options
    out to the specified target directory. The source and target directories
    can, and most of the time will be, the same.
    
    Also registers the Symantec standard help system which is necessary for
    some of the help links in the options pages to work.

PARAMETERS:
    HWND  hWnd,       - Handle to parent window.
    LPSTR lpHelpFile  - Fully qualified name of NAV help file.
    LPSTR lpSourceDir - Where to find the NAV options files to load from.
    LPSTR lpTargetDir - Where to save the NAV options files to.
    BOOL  bIsNT       - Show Windows NT related pages. This actually determines
                        which NAV option DLL to load. If TRUE, XNTOPTS.DLL is
                        loaded else N32OPTS.DLL is used.

RETURN VALUE:
    TRUE if NAV options were successfully written, else FALSE.
******************************************************************************
3/26/1998 TSmith  - Ported from Savanna.
5/05/1998 TSmith  - Added hWnd and lpHelpFile parameters & call to standard
                    help initialization.
*****************************************************************************/
EXTERNC SYM_EXPORT BOOL WINAPI DisplayNAVOptionsDialog(
    HWND  hWnd,
    LPSTR lpHelpFile,
    LPSTR lpSourceDir,
    LPSTR lpTargetDir,
    BOOL  bIsNT
    );



#endif     // __NavWrapC_h__
