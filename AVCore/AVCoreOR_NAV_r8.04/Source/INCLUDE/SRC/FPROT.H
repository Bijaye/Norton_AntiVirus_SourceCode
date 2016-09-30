// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:\include\vcs\fprot.h_v   1.0   02 Sep 1994 17:24:38   CAREY  $
//
// Description: FileProtect structure definitions and defines
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:\include\vcs\fprot.h_v  $
// 
//    Rev 1.0   02 Sep 1994 17:24:38   CAREY
// Initial revision.
//************************************************************************


#define HEADER_SAVED_LEN            35

typedef struct
{

    // the following fields are backwards compatible with NAV 3.0 format
    // and the NAV 3.0 TSR.

    char    Filename[SYM_MAX_PATH+1];           //
    DWORD   Fileattr;                           //
    WORD    Filetime;                           //
    WORD    Filedate;                           //
    DWORD   Filesize;                           //
    BYTE    Header[HEADER_SAVED_LEN];           //
    BYTE    Version;                            //
    WORD    wReserved1;
    WORD    wReserved2;
    WORD    StartCRC;                           // for our level 1 protection
    WORD    CRCAllBuf;                          //
    WORD    CRCLastK;                           //

    // the following fields will be new to NAV 4.0

    DWORD   wExtendedCRC;       // executable segments CRC (XORed together)
    DWORD   dwFileCRC;          // whole file CRC value
    DWORD   dwUserCRC;          // user CRC value
    WORD    wHeaderCRC;         // the basic CRC of the WIN/OS2 exe header
    WORD    wFlags;             // flags about type of header...
    BYTE    byScanLevel;        // what type of information is stored here?
    BYTE    byRepairLevel;      //

} FILE_PROTECT;

typedef FILE_PROTECT FAR * LPFILE_PROTECT;

#define sFILE_PROTECT   sizeof(FILE_PROTECT)


#define     RECORD_VERSION          0x04

#define     FPROT_FLAG_LX_EXEC      0x01
#define     FPROT_FLAG_LE_EXEC      0x02
#define     FPROT_FLAG_PE_EXEC      0x04
#define     FPROT_FLAG_NE_EXEC      0x08


#define     FPROT_FLAG_EXTENDED     (FPROT_FLAG_LX_EXEC \
                                     | FPROT_FLAG_LE_EXEC \
                                     | FPROT_FLAG_PE_EXEC \
                                     | FPROT_FLAG_NE_EXEC)


#define     FPROT_TYPE_PREPEND      1
#define     FPROT_TYPE_GENERIC      2
#define     FPROT_ERROR             3

#define     EXE_SIG1                0x4D5AU
#define     EXE_SIG2                0x5A4DU
