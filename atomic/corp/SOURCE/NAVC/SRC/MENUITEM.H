// Copyright 1992-1996 Symantec, Peter Norton Product Group
//***********************************************************************
//
// $Header:   S:/navc/VCS/menuitem.h_v   1.0   06 Feb 1997 20:56:34   RFULLER  $
//
// Description:
//  Menu items for DOS NAV main program
//
// See Also:
//***********************************************************************
// $Log:   S:/navc/VCS/menuitem.h_v  $
// 
//    Rev 1.0   06 Feb 1997 20:56:34   RFULLER
// Initial revision
// 
//    Rev 1.0   31 Dec 1996 15:25:32   MKEATIN
// Initial revision.
// 
//    Rev 1.0   02 Oct 1996 12:35:04   JBELDEN
// Initial revision.
//***********************************************************************


//-----------------------------------------------------------------------
// Pulldown menu items
//-----------------------------------------------------------------------

#define EXIT_ITEM                       100
#define SCAN_DRIVES_ITEM                101
#define SCAN_FOLDERS_ITEM               102
#define SCAN_FILES_ITEM                 103

#define FILE_NEW_ITEM                   101
#define FILE_OPEN_ITEM                  102
#define FILE_SAVE_ITEM                  103
#define FILE_SAVEAS_ITEM                104

#define TOOLS_INOCULATION_ITEM          201
#ifdef USE_VIRUS_LIST
#define TOOLS_VIRUS_LIST_ITEM           202
#endif
#define TOOLS_REPORTS_ITEM              203
#define TOOLS_OPTIONS_ITEM              204

#define HELP_CONTENTS_ITEM              401
#define HELP_COMMANDS_ITEM              402
#define HELP_PROCEDURES_ITEM            403
#define HELP_QUICKHELP_ITEM             404
#define HELP_USING_HELP_ITEM            405
#define HELP_ABOUT_ITEM                 406

