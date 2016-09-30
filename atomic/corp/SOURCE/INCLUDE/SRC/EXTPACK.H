// Copyright 1996 Symantec, Peter Norton Product Group
//***********************************************************************
// $Header:   S:/include/VCS/extpack.h_v   1.0   06 Feb 1997 20:51:34   RFULLER  $
//
// Description:
//       
//          Structure for headers framing external files packaged
//          with an executable file.
//
// See Also:
//
//***********************************************************************
//  $Log:   S:/include/VCS/extpack.h_v  $ *
// 
//    Rev 1.0   06 Feb 1997 20:51:34   RFULLER
// Initial revision
// 
//    Rev 1.0   08 Aug 1996 11:12:20   JBRENNA
// Initial revision.
// 
//    Rev 1.1   03 Jun 1996 11:48:16   TCASHIN
// Changed packaging mechanism to not depend in exe header.
// 
//    Rev 1.0   20 May 1996 09:22:18   MZAREMB
// Initial revision.
// 
//***********************************************************************

#ifndef _extpack_h
#define _extpack_h


//****************************************************************************
// Defines
//****************************************************************************

// Header Signature
#define EP_SIGNATURE    0xDEADBEEF

// Header Flags
#define EP_DEFAULT_INI  0x0001


//****************************************************************************
// Structures/TypeDefs
//****************************************************************************

typedef struct
    {
    DWORD   dwHdrSig;                   // Header Signature
    WORD    wHdrSize;                   // Header Size
    WORD    wHdrFlags;                  // Header Flags
    DWORD   dwFileSize;                 // File Size
    DWORD   dwFileCRC;                  // File CRC
    DWORD   dwFileNameLength;           // File Name Length. The file name
                                        // will immediately follow this
                                        // structure.
    } EPHEADER, FAR *LPEPHEADER;


#endif 


