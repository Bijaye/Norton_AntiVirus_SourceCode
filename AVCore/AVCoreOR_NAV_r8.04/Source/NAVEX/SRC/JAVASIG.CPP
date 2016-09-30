//************************************************************************
//
// $Header:   S:/NAVEX/VCS/JAVASIG.CPv   1.3   26 Dec 1996 15:21:36   AOONWAL  $
//
// Description:
//      Contains Java virus definitions.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/JAVASIG.CPv  $
// 
//    Rev 1.3   26 Dec 1996 15:21:36   AOONWAL
// No change.
// 
//    Rev 1.2   02 Dec 1996 14:00:58   AOONWAL
// No change.
// 
//    Rev 1.1   29 Oct 1996 12:59:38   AOONWAL
// No change.
// 
//    Rev 1.0   04 Apr 1996 14:56:00   DCHI
// Initial revision.
// 
//************************************************************************

#if 0

#include "javasig.h"

#include "callback.h"
#include "navexshr.h"

WORD gpwJavaTestSig[] =
{
    0x02, 0xB8, 0x00, 0x26, 0xB3, 0x00, 0x2E, 0xB2,
    0x00, 0x2E, 0x03, 0xB6

//    0x10, 0x0A, 0x3E, 0xA7, 0x00, 0x69, 0x10, 0x0D,
//    0x3E, 0xA7
};

WORD gpwJavaTestSig2[] =
{
    0x04, 0xA7, 0x00, 0x10, 0x2A, 0x2B, 0x1C, 0x15,
    0x04, 0x60, 0x33, 0xB6
};

BYTE gpbyJavaMethodWrite[] = "write";

// Remember to add at least 8 to the distance to account
// for fields before code field in "Code" attribute

JAVASIG_T gJavaSigs[] =
{
    VID_JAVATEST2,
    JAVA_SCAN_NAMED_METHOD,     // flags
    sizeof(gpbyJavaMethodWrite)-1,// Method name length
    gpbyJavaMethodWrite,        // Method name
    0x0C,                       // signature length
    gpwJavaTestSig2,            // signature
    0xA704,                     // first word of signature
    0x20,                       // Distance to search

    VID_JAVATEST,               // VID
    JAVA_SCAN_NAMED_METHOD,     // flags
    sizeof("<clinit>")-1,       // Method name length
    (PBYTE)"<clinit>",          // Method name
    0x0C,                       // signature length
    gpwJavaTestSig,             // signature
    0xB802,                     // first word of signature
    0x32,                       // Distance to search

};

// Warning: We do not want to exceed MAX_JAVA_SIGS, defined in "javasig.h"

WORD gwNumJavaSigs = (sizeof(gJavaSigs) / sizeof(JAVASIG_T));

#endif // #if 0

