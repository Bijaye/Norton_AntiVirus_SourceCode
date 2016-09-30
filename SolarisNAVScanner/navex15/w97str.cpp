//************************************************************************
//
// $Header:   S:/NAVEX/VCS/W97STR.CPv   1.3   09 Dec 1998 17:45:20   DCHI  $
//
// Description:
//  Contains Word 97 strings for known identifiers.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/W97STR.CPv  $
// 
//    Rev 1.3   09 Dec 1998 17:45:20   DCHI
// Added #include "gdefines.h"
// 
//    Rev 1.2   09 Nov 1998 13:58:42   DCHI
// Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
// 
//    Rev 1.1   12 Oct 1998 13:44:06   DCHI
// Added the following strings:
// 
//     activevbproject
//     addfromfile
//     codemodule
//     count
//     countoflines
//     export
//     import
//     item
//     organizerrename
//     selectionfilename
//     vbcomponents
//     vbe
//     vbproject
// 
//    Rev 1.0   15 Oct 1997 13:20:34   DCHI
// Initial revision.
// 
//************************************************************************

#include "gdefines.h"

#ifdef MACROHEU

#include "platform.h"

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyW97_STR[] =
{
    14,'a','c','t','i','v','e','d','o','c','u','m','e','n','t',
    15,'a','c','t','i','v','e','v','b','p','r','o','j','e','c','t',
    11,'a','d','d','f','r','o','m','f','i','l','e',
    8,'a','p','p','i','n','f','o','$',
    3,'c','h','r',
    4,'c','h','r','$',
    10,'c','o','d','e','m','o','d','u','l','e',
    4,'c','o','p','y',
    5,'c','o','u','n','t',
    11,'c','o','u','n','t','m','a','c','r','o','s',
    12,'c','o','u','n','t','o','f','l','i','n','e','s',
    12,'c','o','u','n','t','w','i','n','d','o','w','s',
    3,'d','a','y',
    11,'d','e','f','a','u','l','t','d','i','r','$',
    11,'d','e','s','t','i','n','a','t','i','o','n',
    12,'d','i','a','l','o','g','r','e','c','o','r','d',
    9,'d','i','r','e','c','t','o','r','y',
    6,'e','x','p','o','r','t',
    8,'f','i','l','e','n','a','m','e',
    9,'f','i','l','e','n','a','m','e','$',
    19,'f','i','l','e','n','a','m','e','f','r','o','m','w','i','n','d','o','w','$',
    13,'f','i','l','e','n','a','m','e','i','n','f','o','$',
    7,'f','i','l','e','n','e','w',
    8,'f','i','l','e','o','p','e','n',
    10,'f','i','l','e','s','a','v','e','a','s',
    15,'f','i','l','e','s','u','m','m','a','r','y','i','n','f','o',
    13,'f','i','l','e','t','e','m','p','l','a','t','e','s',
    8,'f','u','l','l','n','a','m','e',
    15,'g','e','t','d','o','c','u','m','e','n','t','v','a','r','$',
    17,'g','e','t','p','r','o','f','i','l','e','s','t','r','i','n','g','$',
    6,'i','m','p','o','r','t',
    5,'i','n','s','t','r',
    13,'i','s','e','x','e','c','u','t','e','o','n','l','y',
    4,'i','t','e','m',
    5,'l','c','a','s','e',
    6,'l','c','a','s','e','$',
    4,'l','e','f','t',
    5,'l','e','f','t','$',
    3,'l','e','n',
    5,'l','t','r','i','m',
    6,'l','t','r','i','m','$',
    14,'m','a','c','r','o','c','o','n','t','a','i','n','e','r',
    9,'m','a','c','r','o','c','o','p','y',
    10,'m','a','c','r','o','d','e','s','c','$',
    14,'m','a','c','r','o','f','i','l','e','n','a','m','e','$',
    10,'m','a','c','r','o','n','a','m','e','$',
    3,'m','i','d',
    4,'m','i','d','$',
    7,'n','e','w','n','a','m','e',
    11,'n','e','w','t','e','m','p','l','a','t','e',
    14,'n','o','r','m','a','l','t','e','m','p','l','a','t','e',
    9,'o','r','g','a','n','i','z','e','r',
    13,'o','r','g','a','n','i','z','e','r','c','o','p','y',
    15,'o','r','g','a','n','i','z','e','r','r','e','n','a','m','e',
    6,'r','e','n','a','m','e',
    5,'r','i','g','h','t',
    6,'r','i','g','h','t','$',
    5,'r','t','r','i','m',
    6,'r','t','r','i','m','$',
    17,'s','e','l','e','c','t','i','o','n','f','i','l','e','n','a','m','e',
    6,'s','o','u','r','c','e',
    3,'s','t','r',
    4,'s','t','r','$',
    8,'t','e','m','p','l','a','t','e',
    5,'u','c','a','s','e',
    6,'u','c','a','s','e','$',
    3,'v','a','l',
    12,'v','b','c','o','m','p','o','n','e','n','t','s',
    3,'v','b','e',
    9,'v','b','p','r','o','j','e','c','t',
    11,'w','i','n','d','o','w','n','a','m','e','$',
    9,'w','o','r','d','b','a','s','i','c',
};

LPBYTE FAR galpbyW97_STR[] =
{
    gabyW97_STR +    0,    //   0:activedocument
    gabyW97_STR +   15,    //   1:activevbproject
    gabyW97_STR +   31,    //   2:addfromfile
    gabyW97_STR +   43,    //   3:appinfo$
    gabyW97_STR +   52,    //   4:chr
    gabyW97_STR +   56,    //   5:chr$
    gabyW97_STR +   61,    //   6:codemodule
    gabyW97_STR +   72,    //   7:copy
    gabyW97_STR +   77,    //   8:count
    gabyW97_STR +   83,    //   9:countmacros
    gabyW97_STR +   95,    //  10:countoflines
    gabyW97_STR +  108,    //  11:countwindows
    gabyW97_STR +  121,    //  12:day
    gabyW97_STR +  125,    //  13:defaultdir$
    gabyW97_STR +  137,    //  14:destination
    gabyW97_STR +  149,    //  15:dialogrecord
    gabyW97_STR +  162,    //  16:directory
    gabyW97_STR +  172,    //  17:export
    gabyW97_STR +  179,    //  18:filename
    gabyW97_STR +  188,    //  19:filename$
    gabyW97_STR +  198,    //  20:filenamefromwindow$
    gabyW97_STR +  218,    //  21:filenameinfo$
    gabyW97_STR +  232,    //  22:filenew
    gabyW97_STR +  240,    //  23:fileopen
    gabyW97_STR +  249,    //  24:filesaveas
    gabyW97_STR +  260,    //  25:filesummaryinfo
    gabyW97_STR +  276,    //  26:filetemplates
    gabyW97_STR +  290,    //  27:fullname
    gabyW97_STR +  299,    //  28:getdocumentvar$
    gabyW97_STR +  315,    //  29:getprofilestring$
    gabyW97_STR +  333,    //  30:import
    gabyW97_STR +  340,    //  31:instr
    gabyW97_STR +  346,    //  32:isexecuteonly
    gabyW97_STR +  360,    //  33:item
    gabyW97_STR +  365,    //  34:lcase
    gabyW97_STR +  371,    //  35:lcase$
    gabyW97_STR +  378,    //  36:left
    gabyW97_STR +  383,    //  37:left$
    gabyW97_STR +  389,    //  38:len
    gabyW97_STR +  393,    //  39:ltrim
    gabyW97_STR +  399,    //  40:ltrim$
    gabyW97_STR +  406,    //  41:macrocontainer
    gabyW97_STR +  421,    //  42:macrocopy
    gabyW97_STR +  431,    //  43:macrodesc$
    gabyW97_STR +  442,    //  44:macrofilename$
    gabyW97_STR +  457,    //  45:macroname$
    gabyW97_STR +  468,    //  46:mid
    gabyW97_STR +  472,    //  47:mid$
    gabyW97_STR +  477,    //  48:newname
    gabyW97_STR +  485,    //  49:newtemplate
    gabyW97_STR +  497,    //  50:normaltemplate
    gabyW97_STR +  512,    //  51:organizer
    gabyW97_STR +  522,    //  52:organizercopy
    gabyW97_STR +  536,    //  53:organizerrename
    gabyW97_STR +  552,    //  54:rename
    gabyW97_STR +  559,    //  55:right
    gabyW97_STR +  565,    //  56:right$
    gabyW97_STR +  572,    //  57:rtrim
    gabyW97_STR +  578,    //  58:rtrim$
    gabyW97_STR +  585,    //  59:selectionfilename
    gabyW97_STR +  603,    //  60:source
    gabyW97_STR +  610,    //  61:str
    gabyW97_STR +  614,    //  62:str$
    gabyW97_STR +  619,    //  63:template
    gabyW97_STR +  628,    //  64:ucase
    gabyW97_STR +  634,    //  65:ucase$
    gabyW97_STR +  641,    //  66:val
    gabyW97_STR +  645,    //  67:vbcomponents
    gabyW97_STR +  658,    //  68:vbe
    gabyW97_STR +  662,    //  69:vbproject
    gabyW97_STR +  672,    //  70:windowname$
    gabyW97_STR +  684,    //  71:wordbasic
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

#endif // #ifdef MACROHEU

