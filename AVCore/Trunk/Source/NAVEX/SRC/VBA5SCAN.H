//************************************************************************
//
// $Header:   S:/NAVEX/VCS/VBA5SCAN.H_v   1.2   07 Apr 1997 18:11:16   DCHI  $
//
// Description:
//      Header for VBA 5 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/VBA5SCAN.H_v  $
// 
//    Rev 1.2   07 Apr 1997 18:11:16   DCHI
// Added MVP support.
// 
//    Rev 1.1   14 Mar 1997 16:33:08   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.0   13 Feb 1997 13:24:22   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _VBA5SCAN_H_

#define _VBA5SCAN_H_

#define MAX_VBA5_SIB_DEPTH      16

#define VBA5_WD8                0
#define VBA5_XL97               1

typedef struct tagVBA5_SCAN
{
    WORD    wType;

    union
    {
        // Word 8.0

        struct
        {
            WORD    wParentStorageEntry;
            WORD    wWordDocumentEntry;
            WORD    w1TableEntry;
            WORD    w0TableEntry;
            WORD    wMacrosEntry;
            WORD    wMacrosChildEntry;
        } stWD8;

        // Excel 97

        struct
        {
            WORD    wParentStorageEntry;
            WORD    wWorkbookEntry;
            WORD    w_VBA_PROJECT_CUREntry;
            WORD    w_VBA_PROJECT_CURChildEntry;
        } stXL97;
    } u;

    // VBA 5

    WORD    wVBAEntry;
    WORD    wVBAChildEntry;
    WORD    wPROJECTEntry;
    WORD    wPROJECTwmEntry;
    WORD    w_VBA_PROJECTEntry;
    WORD    wdirEntry;
} VBA5_SCAN_T, FAR *LPVBA5_SCAN;

extern BYTE FAR gauszVBA[];
extern BYTE FAR gauszdir[];
extern BYTE FAR gausz_VBA_PROJECT[];
extern BYTE FAR gauszPROJECT[];
extern BYTE FAR gauszPROJECTwm[];
extern BYTE FAR gausz1Table[];
extern BYTE FAR gausz0Table[];
extern BYTE FAR gauszMacros[];
extern BYTE FAR gauszThisDocument[];
extern BYTE FAR gauszWorkbook[];
extern BYTE FAR gausz_VBA_PROJECT_CUR[];
extern BYTE FAR gauszThisWorkbook[];
extern BYTE gpuszWordDocument[];

#if defined(SYM_DOSX)
extern LZNT_T FAR stLZNT;
#endif

BOOL VBA5OpenCandidateModule
(
    LPCALLBACKREV1  lpstCallBack,           // File op callbacks
    LPOLE_FILE      lpstOLEFile,            // WordDocument stream
    LPBYTE          lpbyName,               // Storage for stream name
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPOLE_OPEN_SIB  lpstOLEOpenSib          // Sibling structure
);

BOOL VBA5MemICmp
(
    LPBYTE      lpbyMem0,
    LPBYTE      lpbyMem1,
    int         n
);

BOOL VBA5LZNTStreamAttributeOnly
(
    LPLZNT          lpstLZNT,
    LPBYTE          lpbyWorkBuffer      // >= 256 bytes
);

EXTSTATUS WD8Scan
(
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // OLE file structure
    LPBYTE                  lpbyStreamBATCache, // Non NULL if cache, 512 bytes
    LPVBA5_SCAN             lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 870 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
);

EXTSTATUS XL97Scan
(
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // OLE file structure
    LPBYTE                  lpbyStreamBATCache, // Non NULL if cache, 512 bytes
    LPVBA5_SCAN             lpstVBA5Scan,       // VBA5 project stream info
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 870 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
);

#endif // #ifndef _VBA5SCAN_H_

