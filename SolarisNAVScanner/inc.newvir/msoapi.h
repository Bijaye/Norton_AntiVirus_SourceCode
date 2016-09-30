//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/msoapi.h_v   1.1   19 Nov 1998 15:54:30   DCHI  $
//
// Description:
//  Core Office 9 MSO access implementation header file.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/msoapi.h_v  $
// 
//    Rev 1.1   19 Nov 1998 15:54:30   DCHI
// Added MSOVBAIsSigned() prototype.
// 
//    Rev 1.0   17 Nov 1998 11:38:32   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _MSOAPI_H_

#define _MSOAPI_H_

#define MSO_TYPE_UNKNOWN                    -1
#define MSO_TYPE_CONTAINER                  0x00
#define MSO_TYPE_FILE_HEADER                0x01
#define MSO_TYPE_TOOLBAR_CUSTOMIZATIONS     0x02
#define MSO_TYPE_BINARY_TAG_OBJECTS         0x03
#define MSO_TYPE_RE_COLOR_INFO              0x04
#define MSO_TYPE_EMBED_FONT                 0x05
#define MSO_TYPE_VBA_PROJECT_XL             0x06
#define MSO_TYPE_VBA_PROJECT_WD             0x07
#define MSO_TYPE_VBA_PROJECT_PPT            0x08
#define MSO_TYPE_DRAWING_DATA               0x09
#define MSO_TYPE_OLE_OBJECTS                0x0A
#define MSO_TYPE_END_BLOCK                  0x0B
#define MSO_TYPE_ENVELOPE                   0x0C
#define MSO_TYPE_WORD_TCG                   0x0D
#define MSO_TYPE_ROUTING_SLIP               0x0E
#define MSO_TYPE_MAX                        0x0F

typedef struct tagMSO
{
    LPVOID          lpvRootCookie;
    LPVOID          lpvFile;
    DWORD           dwFileLen;

    // Current chunk

    int             nChunkType;
    WORD            wVerInst;
    DWORD           dwChunkOffset;
    DWORD           dwChunkSize;
} MSO_T, FAR *LPMSO;

BOOL MSOInit
(
    LPMSO               lpstMSO,
    LPVOID              lpvRootCookie,
    LPVOID              lpvFile
);

BOOL MSOGetFirstChunk
(
    LPMSO               lpstMSO
);

BOOL MSOGetNextChunk
(
    LPMSO               lpstMSO
);

void MSOGetChunkHdrOffset
(
    DWORD               dwChunkOffset,
    LPDWORD             lpdwHdrOffset
);

BOOL MSOExtractVBA
(
    LPMSO               lpstMSO,
    LPVOID              lpvDstFile
);

BOOL MSOVBAIsSigned
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    LPBOOL              lpbSigned
);

BOOL MSOExtractTDT
(
    LPMSO               lpstMSO,
    LPVOID              lpvDstFile
);

BOOL MSOIntegrateVBA
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    LPVOID              lpvVBAFile,
    LPVOID              lpvLZ77File,
    LPDWORD             lpdwOffsetToAdjust
);

BOOL MSOIntegrateTDT
(
    LPMSO               lpstMSO,
    DWORD               dwChunkHdrOffset,
    LPVOID              lpvTDTFile,
    LPDWORD             lpdwOffsetToAdjust
);

BOOL MSOTruncateIfEmpty
(
    LPMSO               lpstMSO
);

#endif // #ifndef _MSOAPI_H_

