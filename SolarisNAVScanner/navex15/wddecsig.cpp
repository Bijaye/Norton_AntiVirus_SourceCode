// Copyright 1998 Symantec, SABU
//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wddecsig.cpv   1.1   09 Nov 1998 13:58:44   DCHI  $
//
// Description:
//  Contains Word 6.0/95 Heuristics Level II deryption signatures.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wddecsig.cpv  $
// 
//    Rev 1.1   09 Nov 1998 13:58:44   DCHI
// Added #pragma data_seg()'s for global FAR data for SYM_WIN16.
// 
//    Rev 1.0   22 May 1998 11:46:34   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"

#include "wddecsig.h"

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

// Default Paragraph Font (Dutch)

BYTE FAR gabyRevKey0x37_0[] =
{
    11,
    0x1A+1, 0x53+1, 0x74+1, 0x61+1, 0x6E+1, 0x64+1, 0x61+1, 0x61+1, 
    0x72+1, 0x64+1, 0x61+1, 0x6C+1, 0x69+1, 0x6E+1, 0x65+1, 0x61+1, 
    0x2D+1, 0x6C+1, 0x65+1, 0x74+1, 0x74+1, 0x65+1, 0x72+1, 0x74+1, 
    0x79+1, 0x70+1, 0x65+1, 
};

// Times New Roman

BYTE FAR gabyRevKey0x58_1[] =
{
    16,
    0x54+1, 0x69+1, 0x6D+1, 0x65+1, 0x73+1, 0x20+1, 0x4E+1, 0x65+1, 
    0x77+1, 0x20+1, 0x52+1, 0x6F+1, 0x6D+1, 0x61+1, 0x6E+1, 0x00+1, 
    0x0C+1, 0x16+1, 0x90+1, 0x01+1, 0x02+1, 0x00+1, 0x53+1, 0x79+1, 
    0x6D+1, 0x62+1, 0x6F+1, 0x6C+1, 0x00+1, 0x0B+1, 0x26+1, 0x90+1, 
};

// Default Paragraph Font (Danish)

BYTE FAR gabyRevKey0x62_2[] =
{
    12,
    0x1B+1, 0x53+1, 0x74+1, 0x61+1, 0x6E+1, 0x64+1, 0x61+1, 0x72+1, 
    0x64+1, 0x73+1, 0x6B+1, 0x72+1, 0x69+1, 0x66+1, 0x74+1, 0x74+1, 
    0x79+1, 0x70+1, 0x65+1, 0x20+1, 0x69+1, 0x20+1, 0x61+1, 0x66+1, 
    0x73+1, 0x6E+1, 0x69+1, 0x74+1, 
};

// Default Paragraph Font (French)

BYTE FAR gabyRevKey0x64_3[] =
{
    2,
    0x11+1, 0x50+1, 0x6F+1, 0x6C+1, 0x69+1, 0x63+1, 0x65+1, 0x20+1, 
    0x70+1, 0x61+1, 0x72+1, 0x20+1, 0x64+1, 0xE9+1, 0x66+1, 0x61+1, 
    0x75+1, 0x74+1, 
};

// Default Paragraph Font (Finnish)

BYTE FAR gabyRevKey0x65_4[] =
{
    7,
    0x16+1, 0x4B+1, 0x61+1, 0x70+1, 0x70+1, 0x61+1, 0x6C+1, 0x65+1, 
    0x65+1, 0x6E+1, 0x20+1, 0x6F+1, 0x6C+1, 0x65+1, 0x74+1, 0x75+1, 
    0x73+1, 0x66+1, 0x6F+1, 0x6E+1, 0x74+1, 0x74+1, 0x69+1, 
};

// Default Paragraph Font (English, Hebrew, Intl Eng, Thai)

BYTE FAR gabyRevKey0x66_5[] =
{
    7,
    0x16+1, 0x44+1, 0x65+1, 0x66+1, 0x61+1, 0x75+1, 0x6C+1, 0x74+1, 
    0x20+1, 0x50+1, 0x61+1, 0x72+1, 0x61+1, 0x67+1, 0x72+1, 0x61+1, 
    0x70+1, 0x68+1, 0x20+1, 0x46+1, 0x6F+1, 0x6E+1, 0x74+1, 
};

// Default Paragraph Font (German)

BYTE FAR gabyRevKey0x6A_6[] =
{
    10,
    0x19+1, 0x41+1, 0x62+1, 0x73+1, 0x61+1, 0x74+1, 0x7A+1, 0x2D+1, 
    0x53+1, 0x74+1, 0x61+1, 0x6E+1, 0x64+1, 0x61+1, 0x72+1, 0x64+1, 
    0x73+1, 0x63+1, 0x68+1, 0x72+1, 0x69+1, 0x66+1, 0x74+1, 0x61+1, 
    0x72+1, 0x74+1, 
};

// Default Paragraph Font (Brazil)

BYTE FAR gabyRevKey0x77_7[] =
{
    4,
    0x13+1, 0x46+1, 0x6F+1, 0x6E+1, 0x74+1, 0x65+1, 0x20+1, 0x70+1, 
    0x61+1, 0x72+1, 0xE1+1, 0x67+1, 0x2E+1, 0x20+1, 0x70+1, 0x61+1, 
    0x64+1, 0x72+1, 0xE3+1, 0x6F+1, 
};

// Default Paragraph Font (Italian)

BYTE FAR gabyRevKey0x79_8[] =
{
    16,
    0x1F+1, 0x43+1, 0x61+1, 0x72+1, 0x61+1, 0x74+1, 0x74+1, 0x65+1, 
    0x72+1, 0x65+1, 0x20+1, 0x70+1, 0x72+1, 0x65+1, 0x64+1, 0x65+1, 
    0x66+1, 0x69+1, 0x6E+1, 0x69+1, 0x74+1, 0x6F+1, 0x20+1, 0x70+1, 
    0x61+1, 0x72+1, 0x61+1, 0x67+1, 0x72+1, 0x61+1, 0x66+1, 0x6F+1, 
};

// Default Paragraph Font (Spanish)

BYTE FAR gabyRevKey0x7C_9[] =
{
    11,
    0x1A+1, 0x46+1, 0x75+1, 0x65+1, 0x6E+1, 0x74+1, 0x65+1, 0x20+1, 
    0x64+1, 0x65+1, 0x20+1, 0x70+1, 0xE1+1, 0x72+1, 0x72+1, 0x61+1, 
    0x66+1, 0x6F+1, 0x20+1, 0x70+1, 0x72+1, 0x65+1, 0x64+1, 0x65+1, 
    0x74+1, 0x65+1, 0x72+1, 
};

// Default Paragraph Font (Swedish)

BYTE FAR gabyRevKey0x7C_10[] =
{
    10,
    0x19+1, 0x53+1, 0x74+1, 0x61+1, 0x6E+1, 0x64+1, 0x61+1, 0x72+1, 
    0x64+1, 0x73+1, 0x74+1, 0x79+1, 0x63+1, 0x6B+1, 0x65+1, 0x74+1, 
    0x65+1, 0x63+1, 0x6B+1, 0x65+1, 0x6E+1, 0x73+1, 0x6E+1, 0x69+1, 
    0x74+1, 0x74+1, 
};

// Default Paragraph Font (Norwegian)

BYTE FAR gabyRevKey0x7C_11[] =
{
    11,
    0x1A+1, 0x53+1, 0x74+1, 0x61+1, 0x6E+1, 0x64+1, 0x61+1, 0x72+1, 
    0x64+1, 0x73+1, 0x6B+1, 0x72+1, 0x69+1, 0x66+1, 0x74+1, 0x20+1, 
    0x66+1, 0x6F+1, 0x72+1, 0x20+1, 0x61+1, 0x76+1, 0x73+1, 0x6E+1, 
    0x69+1, 0x74+1, 0x74+1, 
};


LPWDREVKEY FAR galpstRevKeys[] =
{

    // Default Paragraph Font (Dutch)
    //  WD_ID_DPF_DUTCH,
    (LPWDREVKEY)gabyRevKey0x37_0,

    // Times New Roman
    //  WD_ID_TIMES_NEW_ROMAN_SYMBOL,
    (LPWDREVKEY)gabyRevKey0x58_1,

    // Default Paragraph Font (Danish)
    //  WD_ID_DPF_DANISH,
    (LPWDREVKEY)gabyRevKey0x62_2,

    // Default Paragraph Font (French)
    //  WD_ID_DPF_FRENCH,
    (LPWDREVKEY)gabyRevKey0x64_3,

    // Default Paragraph Font (Finnish)
    //  WD_ID_DPF_FINNISH,
    (LPWDREVKEY)gabyRevKey0x65_4,

    // Default Paragraph Font (English, Hebrew, Intl Eng, Thai)
    //  WD_ID_DEFAULT_PARAGRAPH_FONT,
    (LPWDREVKEY)gabyRevKey0x66_5,

    // Default Paragraph Font (German)
    //  WD_ID_DPF_GERMAN,
    (LPWDREVKEY)gabyRevKey0x6A_6,

    // Default Paragraph Font (Brazil)
    //  WD_ID_PDF_BRAZIL,
    (LPWDREVKEY)gabyRevKey0x77_7,

    // Default Paragraph Font (Italian)
    //  WD_ID_DPF_ITALIAN,
    (LPWDREVKEY)gabyRevKey0x79_8,

    // Default Paragraph Font (Spanish)
    //  WD_ID_DPF_SPANISH,
    (LPWDREVKEY)gabyRevKey0x7C_9,

    // Default Paragraph Font (Swedish)
    //  WD_ID_DPF_SWEDISH,
    (LPWDREVKEY)gabyRevKey0x7C_10,

    // Default Paragraph Font (Norwegian)
    //  WD_ID_DPF_NORWEGIAN,
    (LPWDREVKEY)gabyRevKey0x7C_11,
};


WDREVKEYRING_T FAR gastRevKeyRings[] =
{
    // 1. 0x37

    1,
    0,

    // 2. 0x58

    1,
    1,

    // 3. 0x62

    1,
    2,

    // 4. 0x64

    1,
    3,

    // 5. 0x65

    1,
    4,

    // 6. 0x66

    1,
    5,

    // 7. 0x6A

    1,
    6,

    // 8. 0x77

    1,
    7,

    // 9. 0x79

    1,
    8,

    // 10. 0x7C

    3,
    9,

};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

WDREVKEYLOCKER_T gstRevKeyLocker =
{
    {
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x00
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x08
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x10
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x18
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x20
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x28
          0,   0,   0,   0,   0,   0,   0,   1,    // 0x30
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x38
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x40
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x48
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x50
          2,   0,   0,   0,   0,   0,   0,   0,    // 0x58
          0,   0,   3,   0,   4,   5,   6,   0,    // 0x60
          0,   0,   7,   0,   0,   0,   0,   0,    // 0x68
          0,   0,   0,   0,   0,   0,   0,   8,    // 0x70
          0,   9,   0,   0,  10,   0,   0,   0,    // 0x78
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x80
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x88
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x90
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x98
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xA0
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xA8
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xB0
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xB8
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xC0
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xC8
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xD0
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xD8
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xE0
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xE8
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xF0
          0,   0,   0,   0,   0,   0,   0,   0,    // 0xF8
    },
    galpstRevKeys,
    gastRevKeyRings
};
