#ifndef MACRODAT

#include "platform.h"

DWORD gdwWD7ExclusionSigCount = 68;

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_SIGS")
#endif

DWORD FAR gadwWD7ExclusionCRCs[] =
{
    0x00749C5A, // INSTALLATIONSAFEDOCFIRST macro of D:\TSMACROS\VESS\SETUP.DOC
    0x0080D088, // Template from Customer #990
    0x013EB0A2, // #5857: ADDCHARACTER has .edit
    0x08DFB099, // #7908: SETUP has FileSaveAs
    0x09382AA9, // Customer #2140: SSWAUTOEXEC macro has MacroCopy
    0x0A61352B, // #2445: AUTONEW macro has .EDIT
    0x0B7D8C97, // #4121: BloodHound FP on HDLINK macro
    0x0BCC748A, // #7994: GetGlobals installs macros to doc.
    0x0C2986CD, // #2597: GETSHELFSLOTS has FileSaveAs
    0x0FDE9D81, // #4543: CUSTOMIZE has ToolsMacro .Edit
    0x11FB9209, // #7994: AutoOpen installs macros to global.
    0x163A48C2, // 072298: PRIMEMACROMANAGER macro has .EDIT
    0x1D261DFD, // #2248: STARTWIZARD macro has TOOLSMACRO .EDIT
    0x1E739907, // SAFEDOCBEISPIEL macro of D:\TSMACROS\VESS\SETUP.DOC
    0x233D7C3C, // STESTI macro of D:\TSMACROS\VESS\STESTIN3.DOC
    0x2AD53582, // #2345: LIBRARY macro has FileSaveAs .format
    0x3C99C552, // #8140 COPYFPAS has .Copy
    0x440C3669, // #2345: SETUPDOCUMENT macro has FileSaveAs .format
    0x475940A2, // #8140 IMPORT has FileSaveAs
    0x49BA8CD8, // VASCAN macro of D:\TSMACROS\VESS\EVAL-VAM.DOC
    0x4C1CA2AA, // #2450: VASCAN macro has .EDIT
    0x51A6FB68, // #6055: Invircible Virus Protection's AutoOpen
    0x5A6777F9, // #8140 COPYADDRESS has .Copy
    0x653D31C1, // #2681: HDPCDOCS32 has FileSaveAs
    0x6759A610, // #5627: HDLINK has FileSaveAs
    0x6C2C1DE2, // DEFUSE macro of D:\TSMACROS\VESS\DEFUSE16.DOT
    0x71EE7C60, // #7696: FILEOPEN looks for common viruses.
    0x72AD222C, // #2400: BloodHound FP on IBM document
    0x750C31A2, // STESTI macro of D:\TSMACROS\VESS\STESTIN1.DOC
    0x768AAAAB, // #2600: SELECTSELECTION has GetDocumentVar$
    0x79809824, // DEFUSE macro of D:\TSMACROS\VESS\DEFUSE32.DOT
    0x80D56C6A, // Customer #2140: UPDATENOWNEW macro has Organizer .Copy
    0x83EFAB84, // #5973: German macro package installation.
    0x84B5B455, // #2681: HDAUTO has FileSaveAs
    0x89D2B21E, // AUTOOPEN macro of D:\TSMACROS\VESS\IVWDOG16.DOT
    0x8F835C3E, // #2681: HDASM32 has FileSaveAs
    0x90B1DBF7, // #4524: CUSTOMIZE has ToolsMacro .Edit
    0x91E3571B, // #7000: Install has .Copy
    0x92937BD0, // #5746: ADDCHARACTER has .edit
    0x9387AC0A, // ZLOCKSCANMACRO macro of D:\TSMACROS\VESS\Z-SCAN.DOC
    0x945DDBA6, // #4274: BloodHound FP on PVMERGE macro in PERFECTVIEW 7.0
    0x957A3368, // #2681: HDPCDOCS16 has FileSaveAs
    0x977498E2, // #8140 COPYBANKRUPT has .Copy
    0x9AB8FA6A, // Customer #2140: MAGICFORMUPDATE macro has Organizer .Copy
    0x9B84D2E3, // #4611: HDLINK macro
    0xA47CD890, // #6942: VAScan has .Copy and FileSaveAs
    0xA69D95AA, // #2260: CUSTOMIZE macro has TOOLSMACRO .EDIT
    0xABD4A327, // #8140 EDITADDR2 has FileSaveAs
    0xAC1042EC, // #2248: CONDITIONALVIEWANDDELETENAE macro has GetDocumentVar$
    0xAC8087F0, // #2681: HDLINK has FileSaveAs
    0xAC8D19C2, // #7611: INSTALL macro has .Copy and Filesaveas
    0xB157D685, // #6980: InstallWButn has .Copy and FileSaveAs
    0xB4F61C8F, // SAFEDOCSUBP macro of D:\TSMACROS\VESS\SAFEDOC.DOT
    0xB9980215, // Customer #2140: SSWMAIN macro has MacroCopy
    0xC8F37863, // #2481: STARTWIZARD macro has .EDIT
    0xCACF54ED, // #4704: HDLINK macro
    0xD48AC0CF, // #5546: AWTASETUP macro
    0xD493C738, // #8140 NEWMACROS has .Copy
    0xD7EACFB2, // STESTI macro of D:\TSMACROS\VESS\STESTIN2.DOC
    0xD9075F4B, // #2345: LIBRARY macro has FileSaveAs .format
    0xEA37DDFC, // #2345: SETUPDOCUMENT macro has FileSaveAs .format
    0xEAA25341, // Template from CI #1977  AutoExec from IBM's voice rec - jjw
    0xEACE07B7, // #8140 COPYMACROS has .Copy and FileSaveAs
    0xED530AF6, // Template from Customer #990
    0xED7598B1, // #5546: AWTAUPDATESYSTEM macro
    0xEFDE8FCB, // #7399: ADFERMERUNMODELE has .Copy
    0xF37845E4, // #6980: AutoOpen has .Copy and FileSaveAs
    0xFA37E42F, // AUTOOPEN macro of D:\TSMACROS\VESS\INSTALL1.DOC
};

DWORD FAR *gpadwWD7ExclusionCRCs = gadwWD7ExclusionCRCs;

DWORD FAR gadwWD7ExclusionCRCByteCounts[] =
{
    0x0000FB84, // INSTALLATIONSAFEDOCFIRST macro of D:\TSMACROS\VESS\SETUP.DOC
    0x0000063D, // Template from Customer #990
    0x00001DF4, // #5857: ADDCHARACTER has .edit
    0x00003446, // #7908: SETUP has FileSaveAs
    0x0000141B, // Customer #2140: SSWAUTOEXEC macro has MacroCopy
    0x00002622, // #2445: AUTONEW macro has .EDIT
    0x00001772, // #4121: BloodHound FP on HDLINK macro
    0x000005FF, // #7994: GetGlobals installs macros to doc.
    0x0000201E, // #2597: GETSHELFSLOTS has FileSaveAs
    0x00002E8E, // #4543: CUSTOMIZE has ToolsMacro .Edit
    0x00000493, // #7994: AutoOpen installs macros to global.
    0x0000663D, // 072298: PRIMEMACROMANAGER macro has .EDIT
    0x000037A2, // #2248: STARTWIZARD macro has TOOLSMACRO .EDIT
    0x00009053, // SAFEDOCBEISPIEL macro of D:\TSMACROS\VESS\SETUP.DOC
    0x000341B4, // STESTI macro of D:\TSMACROS\VESS\STESTIN3.DOC
    0x00003A13, // #2345: LIBRARY macro has FileSaveAs .format
    0x0000186C, // #8140 COPYFPAS has .Copy
    0x0000AF63, // #2345: SETUPDOCUMENT macro has FileSaveAs .format
    0x000002E8, // #8140 IMPORT has FileSaveAs
    0x00027332, // VASCAN macro of D:\TSMACROS\VESS\EVAL-VAM.DOC
    0x0003259C, // #2450: VASCAN macro has .EDIT
    0x00000C70, // #6055: Invircible Virus Protection's AutoOpen
    0x00000150, // #8140 COPYADDRESS has .Copy
    0x00000362, // #2681: HDPCDOCS32 has FileSaveAs
    0x000017A0, // #5627: HDLINK has FileSaveAs
    0x00009744, // DEFUSE macro of D:\TSMACROS\VESS\DEFUSE16.DOT
    0x00002042, // #7696: FILEOPEN looks for common viruses.
    0x000035F1, // #2400: BloodHound FP on IBM document
    0x00032FA2, // STESTI macro of D:\TSMACROS\VESS\STESTIN1.DOC
    0x0000335F, // #2600: SELECTSELECTION has GetDocumentVar$
    0x00009744, // DEFUSE macro of D:\TSMACROS\VESS\DEFUSE32.DOT
    0x0000046F, // Customer #2140: UPDATENOWNEW macro has Organizer .Copy
    0x0000CDC6, // #5973: German macro package installation.
    0x0000062B, // #2681: HDAUTO has FileSaveAs
    0x00000BF1, // AUTOOPEN macro of D:\TSMACROS\VESS\IVWDOG16.DOT
    0x00002B3B, // #2681: HDASM32 has FileSaveAs
    0x00002E57, // #4524: CUSTOMIZE has ToolsMacro .Edit
    0x00008D05, // #7000: Install has .Copy
    0x00001DDA, // #5746: ADDCHARACTER has .edit
    0x00006D5A, // ZLOCKSCANMACRO macro of D:\TSMACROS\VESS\Z-SCAN.DOC
    0x00007CA3, // #4274: BloodHound FP on PVMERGE macro in PERFECTVIEW 7.0
    0x00000360, // #2681: HDPCDOCS16 has FileSaveAs
    0x0000012D, // #8140 COPYBANKRUPT has .Copy
    0x00000372, // Customer #2140: MAGICFORMUPDATE macro has Organizer .Copy
    0x00001A44, // #4611: HDLINK macro
    0x0003223C, // #6942: VAScan has .Copy and FileSaveAs
    0x00002E48, // #2260: CUSTOMIZE macro has TOOLSMACRO .EDIT
    0x00000086, // #8140 EDITADDR2 has FileSaveAs
    0x00005736, // #2248: CONDITIONALVIEWANDDELETENAE macro has GetDocumentVar$
    0x00001A44, // #2681: HDLINK has FileSaveAs
    0x00009881, // #7611: INSTALL macro has .Copy and Filesaveas
    0x0000B649, // #6980: InstallWButn has .Copy and FileSaveAs
    0x000180A0, // SAFEDOCSUBP macro of D:\TSMACROS\VESS\SAFEDOC.DOT
    0x000107EB, // Customer #2140: SSWMAIN macro has MacroCopy
    0x000037D2, // #2481: STARTWIZARD macro has .EDIT
    0x000016EC, // #4704: HDLINK macro
    0x00008598, // #5546: AWTASETUP macro
    0x000000AC, // #8140 NEWMACROS has .Copy
    0x00034121, // STESTI macro of D:\TSMACROS\VESS\STESTIN2.DOC
    0x00003712, // #2345: LIBRARY macro has FileSaveAs .format
    0x0000A4E4, // #2345: SETUPDOCUMENT macro has FileSaveAs .format
    0x00005B5B, // Template from CI #1977  AutoExec from IBM's voice rec - jjw
    0x00003164, // #8140 COPYMACROS has .Copy and FileSaveAs
    0x0000063E, // Template from Customer #990
    0x00001540, // #5546: AWTAUPDATESYSTEM macro
    0x000033A5, // #7399: ADFERMERUNMODELE has .Copy
    0x000040C6, // #6980: AutoOpen has .Copy and FileSaveAs
    0x00001965, // AUTOOPEN macro of D:\TSMACROS\VESS\INSTALL1.DOC
};

DWORD FAR *gpadwWD7ExclusionCRCByteCounts = gadwWD7ExclusionCRCByteCounts;

#ifdef SYM_WIN16
#pragma data_seg()
#endif

#endif // #ifndef MACRODAT
