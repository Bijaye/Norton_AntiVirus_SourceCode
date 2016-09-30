// Copyright 1996 Symantec, Peter Norton Product Group
//***********************************************************************
// $Header:   S:/INCLUDE/VCS/symiup.h_v   1.1   17 Oct 1997 14:49:44   TCASHIN  $
//
// Description:
//       
//          Structure for Intelligent Updater Package Files
//
// See Also:
//
//***********************************************************************
//  $Log:   S:/INCLUDE/VCS/symiup.h_v  $ *
// 
//    Rev 1.1   17 Oct 1997 14:49:44   TCASHIN
// Ported changes from the 16-bit version to support other ini 
// files and configurable splash screens.
// 
//    Rev 1.0   11 Aug 1997 13:35:38   JBELDEN
// Initial revision.
// 
//    Rev 1.1   08 Jan 1997 11:48:50   JBELDEN
// added IUPOpenPackageRead for opening read only
// 
//    Rev 1.0   22 Oct 1996 10:49:48   TCASHIN
// Initial revision.
// 
//    Rev 1.1   03 Jun 1996 11:48:16   TCASHIN
// Changed packaging mechanism to not depend in exe header.
// 
//    Rev 1.0   20 May 1996 09:22:18   MZAREMB
// Initial revision.
// 
//***********************************************************************

#ifndef _symiup_h
#define _symiup_h


//****************************************************************************
// Includes
//****************************************************************************
#include "platform.h"


//****************************************************************************
// Defines
//****************************************************************************

// Header Signature
#define IU_SIGNATURE            0xDEADBEEF

// Header Flags
#define IU_DEFAULT_INI          0x0001
#define IU_COMPRESSED           0x0002
#define IU_BUFFER               0x0004
#define IU_UPDATER_EXE          0x8000

// Structure Types
#define IUP_FILE_HEADER         0x0001
#define IUP_CREATOR_HEADER      0x0002

// Creator Flags
#define IUP_SARC                0x0001
#define IUP_NEF                 0x0002

// Parameter Flags
#define IUP_GETFIRSTFILE        0x0001
#define IUP_GETFILENAME         0x0002

// Fatal Error Codes
#define IUP_OK                  0
#define IUP_FILE_OPEN_ERROR     1
#define IUP_FILE_CLOSE_ERROR    2
#define IUP_FILE_READ_ERROR     3
#define IUP_FILE_WRITE_ERROR    4
#define IUP_CANT_GET_MEM        5
#define IUP_PACKAGE_FMT_ERROR   6
#define IUP_MISSING_INSCAN      7
#define IUP_WRONG_HEADER_TYPE   8
#define IUP_BAD_POINTER         9
#define IUP_NO_SARC_RECORD      10
#define IUP_BOF                 11
#define IUP_CANT_FREE_MEM       12
#define IUP_NO_FILENAME         13
#define IUP_BUFFER_TOO_SMALL    14

// Nonfatal Error Codes
#define IUP_NONFATAL_ERROR      0X1000
#define IUP_HEADER_INVALID      0x1001
#define IUP_HEADER_NULL         0x1002

// Buffer Sizes
#define IUP_MAX_COPY_SIZE       (UINT)(31*1024)



//****************************************************************************
// Structures/TypeDefs
//****************************************************************************

// Package File Header Structure
typedef struct
    {
    DWORD   dwHdrSig;                   // Header Signature
    WORD    wHdrSize;                   // Header Size
    WORD    wStructType;                // Structure Type
    WORD    wHdrFlags;                  // Header Flags
    DWORD   dwDate;                     // Date
    DWORD   dwFileSize;                 // File Size
    DWORD   dwFileCRC;                  // File CRC
    DWORD   dwFileNameLength;           // File Name Length.
    DWORD   dwFileCommentLength;        // File Comment Length.
                                        // File name and comments follow
                                        // this structure.
    } IUHEADER, FAR *LPIUHEADER;


// Package File Header Chain
typedef struct
    {
    LPVOID      lpIUNext;               // Pointer to next file header in the chain.
    LPVOID      lpIUPrev;               // Pointer to previous file header in the chain.
    IUHEADER    IUHeader;               // File Header
    } IUHEADERLIST, FAR *LPIUHEADERLIST;

// Creator Record Structure
typedef struct
    {
    DWORD   dwCreSig;                   // Header Signature
    WORD    wCreSize;                   // Header Size
    WORD    wStructType;                // Structure Type
    WORD    wCreFlags;                  // Creator Flags
    DWORD   dwDate;                     // Date
    DWORD   dwCommentLength;            // Comment Length.
    } IUCREATOR, FAR *LPIUCREATOR;

// Creator Record Chain
typedef struct
    {
    LPVOID          lpNext;             // Pointer to the next IUCreator in chain
    IUCREATOR       IUCreator;          // IUCreator Structure
    } IUCREATELIST, FAR *LPIUCREATELIST;

typedef HFILE HPACKAGE;

// Package Info Structure
typedef struct
    {
    WORD            wInitFlags;         // Init Flags.
    HPACKAGE        hPackage;           // Einstein package handle.
    UINT            uError;             // Error Code
    LPIUHEADERLIST  lpHeaderList;       // List of IUHEADER list structures.
    LPIUCREATELIST  lpCreateList;       // List of IUCREATOR list structures.
    } IUINFO, FAR *LPIUINFO;

// Function Parameter Structure
typedef struct
    {
    DWORD    dwParamSize;               // Size of param structure.
    WORD     wParamFlags;               // Param Flags
    HPACKAGE hPackage;                  // Einstein package handle.
    LPVOID   lpBuffer;                  // Pointer to a misc. buffer.
    LPIUINFO lpIUInfo;                  // Pointer to  structure.
    LPSTR    lpFileName;                // File Name.
    LPSTR    lpComment;                 // Comments.
    BYTE     Reserved[16];              // Reserved.
    } IUPARAM, FAR *LPIUPARAM;


//****************************************************************************
// Function Prototypes
//****************************************************************************

#ifdef __cplusplus
extern "C" {
#endif

UINT SYM_EXPORT WINAPI IUPAddFileToPackage  ( LPIUPARAM       lpIUParam );
UINT SYM_EXPORT WINAPI IUPCheckPackage      ( LPIUPARAM       lpIUParam );
UINT SYM_EXPORT WINAPI IUPClosePackage      ( LPIUINFO        lpPackageInfo );
UINT SYM_EXPORT WINAPI IUPCreatePackage     ( LPCSTR          lpOldPackage,
                                              LPCSTR          lpNewPackage,
                                              LPIUPARAM       lpParam);
UINT SYM_EXPORT WINAPI IUPExtractBuffer     ( LPIUPARAM       lpIUParam, 
                                              LPSTR           lpBuffer,
                                              DWORD           dwBufferSize);
UINT SYM_EXPORT WINAPI IUPExtractFile       ( LPIUPARAM       lpIUParam, 
                                              LPCSTR          lpTempDir );
UINT SYM_EXPORT WINAPI IUPHistory           ( LPIUINFO        lpPackageInfo );
UINT SYM_EXPORT WINAPI IUPListPackage       ( LPIUINFO        lpPackageInfo );
LPIUINFO SYM_EXPORT WINAPI IUPOpenPackage   ( LPIUPARAM       lpIUParam);
LPIUINFO SYM_EXPORT WINAPI IUPOpenPackageRead   ( LPIUPARAM       lpIUParam);


UINT SYM_EXPORT WINAPI IUUAddCreator        ( LPIUINFO        lpInfo, 
                                              LPIUPARAM       lpParam );
UINT SYM_EXPORT WINAPI IUUBuildCreateList   ( LPIUINFO        lpPackageInfo );
UINT SYM_EXPORT WINAPI IUUBuildFileList     ( LPIUINFO        lpPackageInfo );
UINT SYM_EXPORT WINAPI IUUCopyCreateList    ( LPIUINFO        lpDestChain, 
                                              LPIUINFO        lpSrcChain );
UINT SYM_EXPORT WINAPI IUUDeleteCreateList  ( LPIUINFO        lpPackageInfo );
UINT SYM_EXPORT WINAPI IUUDeleteFileList    ( LPIUINFO        lpPackageInfo );
UINT SYM_EXPORT WINAPI IUUFindFirstFile     ( LPIUPARAM       lpIUParam );
UINT SYM_EXPORT WINAPI IUUFindNextFile      ( LPIUPARAM       lpIUParam );
UINT SYM_EXPORT WINAPI IUUGetFileByName     ( LPIUPARAM       lpIUParam );
UINT SYM_EXPORT WINAPI IUUGetFileHeader     ( LPIUPARAM       lpIUParam, 
                                              LPIUHEADER      lpIUHeader );
UINT SYM_EXPORT WINAPI IUUGetNameFromHdr    ( LPIUPARAM       lpIUParam, 
                                              LPIUHEADER      lpIUHeader );
UINT SYM_EXPORT WINAPI IUUFreeCreatorBlocks ( LPIUCREATELIST  lpIUCreateList );
LPIUINFO SYM_EXPORT WINAPI IUUNewPackage    ( LPIUPARAM       lpParam );
UINT SYM_EXPORT WINAPI IUUReadCreatorBlocks ( LPIUPARAM       lpIUParam, 
                                              LPIUCREATELIST *lpIUCreateList );
UINT SYM_EXPORT WINAPI IUUWriteCreatorBlocks( LPIUPARAM       lpIUParam, 
                                              LPIUCREATELIST *lpIUCreateList );

#ifdef __cplusplus
}
#endif

#endif 


