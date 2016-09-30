#ifdef MCRENG15

  #include "wd7api.h"

#else

  #include "platform.h"
  #include "storobj.h"
  #include "wdencdoc.h"

#endif

#include "wddecsig.h"


// Default Paragraph Font

BYTE FAR gabyRevKey0x2C_0[] =
{
    6,
    0x44+1, 0x65+1, 0x66+1, 0x61+1, 0x75+1, 0x6C+1, 0x74+1, 0x20+1, 
    0x50+1, 0x61+1, 0x72+1, 0x61+1, 0x67+1, 0x72+1, 0x61+1, 0x70+1, 
    0x68+1, 0x20+1, 0x46+1, 0x6F+1, 0x6E+1, 0x74+1, 
};


LPWDREVKEY FAR galpstRevKeys[] =
{

    // Default Paragraph Font
    //  WD_ID_DEFAULT_PARAGRAPH_FONT,
    (LPWDREVKEY)gabyRevKey0x2C_0,
};


WDREVKEYRING_T gastRevKeyRings[] =
{
    // 1. 0x2C

    1,
    0,

};


WDREVKEYLOCKER_T gstRevKeyLocker =
{
    {
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x00
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x08
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x10
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x18
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x20
          0,   0,   0,   0,   1,   0,   0,   0,    // 0x28
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x30
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x38
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x40
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x48
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x50
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x58
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x60
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x68
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x70
          0,   0,   0,   0,   0,   0,   0,   0,    // 0x78
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
