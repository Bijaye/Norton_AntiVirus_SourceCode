#ifdef MCRENG15

  #include "wd7api.h"
  #include "o97api.h"
  #include "wdscan15.h"

#else

  #include "storobj.h"
  #include "wdencdoc.h"
  #include "worddoc.h"
  #include "wdscan.h"

#endif

#include "macrovid.h"

///////////////////////////////////////////////////////////////////////////
//
// Name signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyXL97NameSetOffsetSigs[] =
{
    //--------------------------------------------------
    // 0x0000 : NAME_ONLY_MODULE1
    // Only MODULE1

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x4E, 0x50, 0x45, 0x56, 0x4D, 0x46, 0x32, 

    //--------------------------------------------------
    // 0x0001 : NAME_ONLY_MODULE2
    // Only MODULE2

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x4E, 0x50, 0x45, 0x56, 0x4D, 0x46, 0x33, 

    //--------------------------------------------------
    // 0x0002 : NAME_ONLY_SHEET01
    // Only SHEET01

    0x03,
    0x00, 0x5E, 0x79, 
    0x07,
    0x54, 0x49, 0x46, 0x46, 0x55, 0x31, 0x32, 

    //--------------------------------------------------
    // 0x0003 : NAME_ONLY_LAROUX
    // Only LAROUX

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x4D, 0x42, 0x53, 0x50, 0x56, 0x59, 

    //--------------------------------------------------
    // 0x0004 : NAME_ONLY_11SPACES
    // Only "           "

    0x03,
    0x00, 0x5F, 0x79, 
    0x0C,
    0x0B, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 
    0x21, 0x21, 0x21, 0x21, 

    //--------------------------------------------------
    // 0x0005 : NAME_ONLY_LEGEND
    // Only LEGEND

    0x03,
    0x00, 0x5D, 0x79, 
    0x06,
    0x4D, 0x46, 0x48, 0x46, 0x4F, 0x45, 

    //--------------------------------------------------
    // 0x0006 : NAME_ONLY_COP
    // Only COP

    0x03,
    0x00, 0x5A, 0x79, 
    0x03,
    0x44, 0x50, 0x51, 

    //--------------------------------------------------
    // 0x0007 : NAME_ONLY_ROBO
    // Only ROBO

    0x03,
    0x00, 0x5B, 0x79, 
    0x04,
    0x53, 0x50, 0x43, 0x50, 

    0x00
};

BYTE FAR gabyXL97NameEndOffsetSigs[] =
{
    0x00
};

WD_SIG_INFO_T FAR gastXL97RawNameSigIndex[] =
{
    { 0, NULL }
};

BYTE FAR gabyXL97NameSigRawByteHit[256] =
{
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x00
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x08
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x10
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x18
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x20
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x28
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x30
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x38
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x40
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x48
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x50
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x58
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x60
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x68
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x70
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x78
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x80
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x88
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x90
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x98
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF8
};

///////////////////////////////////////////////////////////////////////////
//
// Named CRC signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyXL97NamedCRC0x00000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x00E293BE Sigs

    //--------------------------------------------------
    // 0x0000 : CRC_DUMBLAROUX_MODULE1
    // Named CRC for DUMB LAROUX

    0x01,
    0x45, 
    0x01,
    0x00, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0x2D000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x2D333F7B Sigs

    //--------------------------------------------------
    // 0x0001 : CRC_11SPACES_SOFA
    // Named CRC for

    0x01,
    0x45, 
    0x01,
    0x04, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0x38000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x387D4BED Sigs

    //--------------------------------------------------
    // 0x0002 : CRC_ROBO_ROBOCOP
    // Named CRC for ROBO

    0x01,
    0x45, 
    0x01,
    0x07, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0x81000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x81C63C3F Sigs

    //--------------------------------------------------
    // 0x0003 : CRC_SHEET01_DMV
    // Named CRC for SHEET01

    0x01,
    0x45, 
    0x01,
    0x02, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0x85000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8516823F Sigs

    //--------------------------------------------------
    // 0x0004 : CRC_LAROUX_LAROUXC
    // Named CRC for LAROUX.C

    0x01,
    0x45, 
    0x01,
    0x03, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0x8F000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0x8F14E37C Sigs

    //--------------------------------------------------
    // 0x0005 : CRC_COP_ROBOCOP
    // Named CRC for COP

    0x01,
    0x45, 
    0x01,
    0x06, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0xBE000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xBE70B006 Sigs

    //--------------------------------------------------
    // 0x0006 : CRC_LEGEND_LEGEND
    // Named CRC for LEGEND

    0x01,
    0x45, 
    0x01,
    0x05, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0xD5000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD5772916 Sigs

    //--------------------------------------------------
    // 0x0007 : CRC_MODULE1_DELTA
    // Named CRC for MODULE1

    0x01,
    0x45, 
    0x01,
    0x00, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0xD6000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xD6170E43 Sigs

    //--------------------------------------------------
    // 0x0008 : CRC_LAROUX_LAROUXB
    // Named CRC for LAROUX

    0x01,
    0x45, 
    0x01,
    0x03, 

    0x00
};

BYTE FAR gabyXL97NamedCRC0xFF000000Sigs[] =
{
    ////////////////////////////////////////////////////
    // NamedCRC 0xFFFFFFFF Sigs

    //--------------------------------------------------
    // 0x0009 : CRC_MODULE1_ROBOCOP
    // Named CRC for MODULE1

    0x01,
    0x45, 
    0x01,
    0x00, 

    //--------------------------------------------------
    // 0x000A : CRC_MODULE2_ROBOCOP
    // Named CRC for MODULE2

    0x01,
    0x45, 
    0x01,
    0x01, 

    0x00
};

BYTE FAR gabyXL97NamedCRCSigFullSet[] =
{
    0x01, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

WD_NAMED_CRC_SIG_INFO_T FAR gastXL97NamedCRCSigInfo[] =
{
    { 0x0000, 0x0000, gabyXL97NamedCRC0x00000000Sigs +    0 }, // 0x00E293BE
    { 0x0001, 0x0001, gabyXL97NamedCRC0x2D000000Sigs +    0 }, // 0x2D333F7B
    { 0x0002, 0x0002, gabyXL97NamedCRC0x38000000Sigs +    0 }, // 0x387D4BED
    { 0x0003, 0x0003, gabyXL97NamedCRC0x81000000Sigs +    0 }, // 0x81C63C3F
    { 0x0004, 0x0004, gabyXL97NamedCRC0x85000000Sigs +    0 }, // 0x8516823F
    { 0x0005, 0x0005, gabyXL97NamedCRC0x8F000000Sigs +    0 }, // 0x8F14E37C
    { 0x0006, 0x0006, gabyXL97NamedCRC0xBE000000Sigs +    0 }, // 0xBE70B006
    { 0x0007, 0x0007, gabyXL97NamedCRC0xD5000000Sigs +    0 }, // 0xD5772916
    { 0x0008, 0x0008, gabyXL97NamedCRC0xD6000000Sigs +    0 }, // 0xD6170E43
    { 0x0009, 0x0009, gabyXL97NamedCRC0xFF000000Sigs +    0 }, // 0xFFFFFFFF
};

///////////////////////////////////////////////////////////////////////////
//
// Macro signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyXL97MacroSetOffsetSigs[] =
{
    0x00
};

BYTE FAR gabyXL97MacroEndOffsetSigs[] =
{
    0x00
};

BYTE FAR gabyXL97MacroRaw0x53Sigs[] =
{
    //--------------------------------------------------
    // 0x0000 : MACRO_SIG_LAROUXA
    // Alg detection for Laroux

    0x01,
    0x7F, 
    0x16,
    0x15, 0x54, 0x69, 0x66, 0x66, 0x75, 0x74, 0x29, 
    0x23, 0x6D, 0x62, 0x73, 0x70, 0x76, 0x79, 0x23, 
    0x2A, 0x2F, 0x44, 0x70, 0x71, 0x7A, 

    0x00
};

BYTE FAR gabyXL97MacroSigFullSet[] =
{
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

WD_SIG_INFO_T FAR gastXL97RawMacroSigIndex[] =
{
    { 0x0000, gabyXL97MacroRaw0x53Sigs },
};

BYTE FAR gabyXL97MacroSigRawByteHit[256] =
{
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x00
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x08
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x10
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x18
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x20
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x28
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x30
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x38
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x40
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x48
      0,   0,   0,   1,   0,   0,   0,   0,     // 0x50
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x58
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x60
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x68
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x70
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x78
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x80
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x88
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x90
      0,   0,   0,   0,   0,   0,   0,   0,     // 0x98
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xA8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xB8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xC8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xD8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xE8
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF0
      0,   0,   0,   0,   0,   0,   0,   0,     // 0xF8
};

///////////////////////////////////////////////////////////////////////////
//
// CRC array
//
///////////////////////////////////////////////////////////////////////////

DWORD FAR gadwXL97CRCs[] =
{
    0x00E293BE,    // CRC_ID: 0x0000
    0x2D333F7B,    // CRC_ID: 0x0001
    0x387D4BED,    // CRC_ID: 0x0002
    0x81C63C3F,    // CRC_ID: 0x0003
    0x8516823F,    // CRC_ID: 0x0004
    0x8F14E37C,    // CRC_ID: 0x0005
    0xBE70B006,    // CRC_ID: 0x0006
    0xD5772916,    // CRC_ID: 0x0007
    0xD6170E43,    // CRC_ID: 0x0008
    0xFFFFFFFF,    // CRC_ID: 0x0009
};

///////////////////////////////////////////////////////////////////////////
//
// Known and unknown virus signatures
//
///////////////////////////////////////////////////////////////////////////

BYTE FAR gabyXL97VirusSigSet0000[] =
{
    //--------------------------------------------------
    // 0x0000 : VID_DELTA
    // XM.Delta

    0x01,
    0xF2, 
    0x07, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0001 : VID_XLDMV
    // XM.DMV

    0x01,
    0xF2, 
    0x03, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0002 : VID_LAROUX
    // XM.Laroux.A

    0x03,
    0x40, 0x28, 0xF9, 
    0x03, 0x00, 0x00, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0003 : VID_LAROUXB
    // XM.Laroux.B

    0x01,
    0xF2, 
    0x08, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0004 : VID_LAROUXC
    // XM.Laroux.C

    0x01,
    0xF2, 
    0x04, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0005 : VID_SOFA
    // XM.Sofa

    0x01,
    0xF2, 
    0x01, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0006 : VID_LEGEND
    // XM.Legend.A

    0x01,
    0xF2, 
    0x06, 

    0x01,
    0xFB, 

    0x00,

    //--------------------------------------------------
    // 0x0007 : VID_ROBOCOPA
    // Sig

    0x02,
    0x22, 0xF8, 
    0x05, 0x02, 

    0x01,
    0xFB, 

    0x01,
    0xF2, 
    0x09, 

    0x01,
    0xF2, 
    0x0A, 

    0x00,

    //--------------------------------------------------
    // 0x0008 : VID_FULL_SET
    // Macro Component Virus (works for Remnants, too)

    0x02,
    0x6E, 0xFF, 

    0x02,
    0x2E, 0xFF, 

    0x00,

};

WD_VIRUS_SIG_INFO_T FAR gastXL97VirusSigInfo[] =
{
    { VID_DELTA, gabyXL97VirusSigSet0000 + 0x0000 },
    { VID_XLDMV, gabyXL97VirusSigSet0000 + 0x0006 },
    { VID_LAROUX, gabyXL97VirusSigSet0000 + 0x000C },
    { VID_LAROUXB, gabyXL97VirusSigSet0000 + 0x0016 },
    { VID_LAROUXC, gabyXL97VirusSigSet0000 + 0x001C },
    { VID_SOFA, gabyXL97VirusSigSet0000 + 0x0022 },
    { VID_LEGEND, gabyXL97VirusSigSet0000 + 0x0028 },
    { VID_ROBOCOPA, gabyXL97VirusSigSet0000 + 0x002E },

    { VID_FULL_SET, gabyXL97VirusSigSet0000 + 0x003C },
};


///////////////////////////////////////////////////////////////////////////
//
// Custom scan and repair function pointer arrays
//
///////////////////////////////////////////////////////////////////////////

LPF_WD_CUSTOM_SCAN FAR gapfXL97CustomScan[] =
{
    NULL
};

LPF_WD_CUSTOM_REPAIR FAR gapfXL97CustomRepair[] =
{
    NULL
};


///////////////////////////////////////////////////////////////////////////
//
// Signature set structure
//
///////////////////////////////////////////////////////////////////////////

WD_SIG_SET_T gstXL97SigSet =
{
    // Name sig info

    8,
    gabyXL97NameSetOffsetSigs,
    0,
    gabyXL97NameEndOffsetSigs,
    gastXL97RawNameSigIndex,
    gabyXL97NameSigRawByteHit,
    8,

    // Named CRC sig info

    10,
    11,
    gastXL97NamedCRCSigInfo,

    // Macro sig info

    0,
    gabyXL97MacroSetOffsetSigs,
    0,
    gabyXL97MacroEndOffsetSigs,
    gastXL97RawMacroSigIndex,
    gabyXL97MacroSigRawByteHit,
    1,

    // CRC info

    10,
    gadwXL97CRCs,

    // Virus signatures

    9,
    gastXL97VirusSigInfo,

    // Custom scan and repair arrays

    gapfXL97CustomScan,
    gapfXL97CustomRepair,

    // Named CRC sig full set

    9,
    gabyXL97NamedCRCSigFullSet,

    // Macro sig full set

    9,
    gabyXL97MacroSigFullSet
};


