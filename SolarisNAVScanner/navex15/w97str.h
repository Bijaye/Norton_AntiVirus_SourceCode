//************************************************************************
//
// $Header:   S:/NAVEX/VCS/W97STR.H_v   1.1   12 Oct 1998 13:39:20   DCHI  $
//
// Description:
//  Header for Word 97 strings for known identifiers.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/W97STR.H_v  $
// 
//    Rev 1.1   12 Oct 1998 13:39:20   DCHI
// Added the following enums:
// 
//     eW97_STR_activevbproject
//     eW97_STR_addfromfile
//     eW97_STR_codemodule
//     eW97_STR_count
//     eW97_STR_countoflines
//     eW97_STR_export
//     eW97_STR_import
//     eW97_STR_item
//     eW97_STR_organizerrename
//     eW97_STR_selectionfilename
//     eW97_STR_vbcomponents
//     eW97_STR_vbe
//     eW97_STR_vbproject
// 
//    Rev 1.0   15 Oct 1997 13:20:32   DCHI
// Initial revision.
// 
//************************************************************************

typedef enum tagEW97_STR
{
    eW97_STR_activedocument                      = 0,
    eW97_STR_activevbproject                     = 1,
    eW97_STR_addfromfile                         = 2,
    eW97_STR_appinfoS                            = 3,
    eW97_STR_chr                                 = 4,
    eW97_STR_chrS                                = 5,
    eW97_STR_codemodule                          = 6,
    eW97_STR_copy                                = 7,
    eW97_STR_count                               = 8,
    eW97_STR_countmacros                         = 9,
    eW97_STR_countoflines                        = 10,
    eW97_STR_countwindows                        = 11,
    eW97_STR_day                                 = 12,
    eW97_STR_defaultdirS                         = 13,
    eW97_STR_destination                         = 14,
    eW97_STR_dialogrecord                        = 15,
    eW97_STR_directory                           = 16,
    eW97_STR_export                              = 17,
    eW97_STR_filename                            = 18,
    eW97_STR_filenameS                           = 19,
    eW97_STR_filenamefromwindowS                 = 20,
    eW97_STR_filenameinfoS                       = 21,
    eW97_STR_filenew                             = 22,
    eW97_STR_fileopen                            = 23,
    eW97_STR_filesaveas                          = 24,
    eW97_STR_filesummaryinfo                     = 25,
    eW97_STR_filetemplates                       = 26,
    eW97_STR_fullname                            = 27,
    eW97_STR_getdocumentvarS                     = 28,
    eW97_STR_getprofilestringS                   = 29,
    eW97_STR_import                              = 30,
    eW97_STR_instr                               = 31,
    eW97_STR_isexecuteonly                       = 32,
    eW97_STR_item                                = 33,
    eW97_STR_lcase                               = 34,
    eW97_STR_lcaseS                              = 35,
    eW97_STR_left                                = 36,
    eW97_STR_leftS                               = 37,
    eW97_STR_len                                 = 38,
    eW97_STR_ltrim                               = 39,
    eW97_STR_ltrimS                              = 40,
    eW97_STR_macrocontainer                      = 41,
    eW97_STR_macrocopy                           = 42,
    eW97_STR_macrodescS                          = 43,
    eW97_STR_macrofilenameS                      = 44,
    eW97_STR_macronameS                          = 45,
    eW97_STR_mid                                 = 46,
    eW97_STR_midS                                = 47,
    eW97_STR_newname                             = 48,
    eW97_STR_newtemplate                         = 49,
    eW97_STR_normaltemplate                      = 50,
    eW97_STR_organizer                           = 51,
    eW97_STR_organizercopy                       = 52,
    eW97_STR_organizerrename                     = 53,
    eW97_STR_rename                              = 54,
    eW97_STR_right                               = 55,
    eW97_STR_rightS                              = 56,
    eW97_STR_rtrim                               = 57,
    eW97_STR_rtrimS                              = 58,
    eW97_STR_selectionfilename                   = 59,
    eW97_STR_source                              = 60,
    eW97_STR_str                                 = 61,
    eW97_STR_strS                                = 62,
    eW97_STR_template                            = 63,
    eW97_STR_ucase                               = 64,
    eW97_STR_ucaseS                              = 65,
    eW97_STR_val                                 = 66,
    eW97_STR_vbcomponents                        = 67,
    eW97_STR_vbe                                 = 68,
    eW97_STR_vbproject                           = 69,
    eW97_STR_windownameS                         = 70,
    eW97_STR_wordbasic                           = 71,
} EW97_STR_T, FAR *LPEW97_STR;

extern LPBYTE FAR galpbyW97_STR[];

#define NUM_W97_STR             72

