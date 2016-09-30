//************************************************************************
//
// $Header:   S:/MACROHEU/VCS/O97STR.CPv   1.1   09 Nov 1998 13:44:04   DCHI  $
//
// Description:
//  Source for O97 strings.
//  Generated using:
//      vbaemstr o97str.lst o97str O97
//
//************************************************************************
// $Log:   S:/MACROHEU/VCS/O97STR.CPv  $
// 
//    Rev 1.1   09 Nov 1998 13:44:04   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.0   15 Aug 1997 13:29:36   DCHI
// Initial revision.
// 
//************************************************************************

#include "platform.h"

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO97_STR[] =
{
    11,'a','p','p','l','i','c','a','t','i','o','n',
    3,'e','r','r',
};

LPBYTE FAR galpbyO97_STR[] =
{
    gabyO97_STR +    0,    //   0:application
    gabyO97_STR +   12,    //   1:err
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

