// PROPRIETARY/CONFIDENTIAL. Use of this product is subject to license terms.
// Copyright 2004, 2005 Symantec Corporation. All rights reserved.
//***************************************************************************
//
//***************************************************************************
#ifndef _QS_H
#define _QS_H


//***************************************************************************
// Global Constants
//***************************************************************************
#define BUILD_NUM               "Build 1"
#define VERSION_NUM             "Version 3.5"
#define VERSION_CONFIG_FILE     1


//========================================================================
// define all the optional values if they haven't been set...
//========================================================================
#define NSC_VERSION_MAJOR       3
#define NSC_VERSION_MINOR       05

#ifndef VR_FILEVERSION
    #define VR_FILEVERSION      3,05,05,57
#endif

#ifndef VR_PRODUCTVERSION
    #define VR_PRODUCTVERSION   3,05,05,57
#endif

#ifndef VR_FILEFLAGSMASK
    #define VR_FILEFLAGSMASK    0
#endif

#ifndef VR_FILEFLAGS
    #ifdef _DEBUG
        #define VR_FILEFLAGS    0x1L
    #else
        #define VR_FILEFLAGS    0x0L
    #endif
#endif

#ifndef VR_FILEOS
    #define VR_FILEOS           0x4L    // VOS_WINDOWS51
#endif

#ifndef VR_FILETYPE
    #define VR_FILETYPE         0x2L    // VFT_DLL
#endif

#ifndef VR_FILESUBTYPE
    #define VR_FILESUBTYPE      0x0L    // VFT2_UNKNOWN
#endif

#ifndef VR_FILEVERSIONSTRING
    #define VR_FILEVERSIONSTRING "3.05.05.57\0"
#endif

#ifndef VR_COPYRIGHT
    #define VR_COPYRIGHT        "Copyright \251 2005 - 2006 Symantec Corporation\0"
#endif

#ifndef VR_PUBLISHERNAME
    #define VR_PUBLISHERNAME    "Symantec Corporation\0"
#endif

#ifndef VR_PRODUCTNAME
    #define VR_PRODUCTNAME      "Symantec Central Quarantine\0"
#endif

#ifndef VR_PRODUCTVERSIONSTRING
    #define VR_PRODUCTVERSIONSTRING "3.05.05.57\0"
#endif

#ifndef VR_LICENSEVERSIONSTRING
    #define VR_LICENSEVERSIONSTRING "3.05.05.57\0"
#endif

#ifndef VR_FILEDESCRIPTION
    #error You must define VR_FILEDESCRIPTION in the local resource script.
#endif

#ifndef VR_INTERNALNAME
    #error You must define VR_INTERNALNAME in the local resource script.
#endif

#ifndef VR_ORIGINALFILENAME
    #error You must define VR_ORIGINALFILENAME in the local resource script.
#endif


#endif


