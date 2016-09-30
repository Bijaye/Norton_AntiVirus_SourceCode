//************************************************************************
//
// $Header:   S:/NAVEX/VCS/OFFCSC15.H_v   1.6   15 Dec 1998 12:13:18   DCHI  $
//
// Description:
//      Contains Microsoft Office macro scan function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/OFFCSC15.H_v  $
// 
//    Rev 1.6   15 Dec 1998 12:13:18   DCHI
// Removed PP_SCAN #ifdefs.
// 
//    Rev 1.5   08 Dec 1998 13:22:26   DCHI
// Changes for MSX.
// 
//    Rev 1.4   11 May 1998 18:04:46   DCHI
// Changes for Access 2.0 scanning and Access datafile conversion.
// 
//    Rev 1.3   15 Apr 1998 17:24:16   DCHI
// Modifications for new XL95 and XL5 engines.
// 
//    Rev 1.2   29 Jan 1998 19:04:48   DCHI
// Added support for Excel formula virus scan and repair.
// 
//    Rev 1.1   26 Nov 1997 18:35:14   DCHI
// Added #ifdef'd out PowerPoint scanning code.
// 
//    Rev 1.0   09 Jul 1997 16:17:40   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _OFFCSC15_H_

#define _OFFCSC15_H_

#define OLE_SCAN_WD7            1
#define OLE_SCAN_WD8            2
#define OLE_SCAN_XL97           3
#define OLE_SCAN_XL5            4

#define OLE_SCAN_PP             5

#define OLE_SCAN_XL4_95         6
#define OLE_SCAN_XL4_97         7

#define OLE_SCAN_A2             8
#define OLE_SCAN_A97            9

typedef struct tagMSOFFICE_SCAN
{
    LPSS_ROOT       lpstRoot;
    LPSS_STREAM     lpstStream;

    LPBYTE          lpbyHitMem;

    LPWD_SCAN       lpstScan;

    BOOL            bMVPApproved;

    DWORD           dwLongScanCount;

    // The type: OLE_SCAN_{WD7,WD8,XL97}

    WORD            wType;

    union
    {
        // Word 6.0/7.0

        struct
        {
            DWORD               dwWordDocumentEntry;
            LPWD7_MACRO_SCAN    lpstMacroScan;
        } stWD7;

        // Word 8.0, Excel 97

        O97_SCAN_T  stO97;

        // Excel 5.0/95

        struct
        {
            WORD                wVID;
            DWORD               dw_VBA_PROJECTEntry;
            DWORD               dw_VBA_PROJECTChildEntry;
            DWORD               dwDirEntry;
        } stXL5;

        // Excel 5.0/95/97 Excel 4.0 macro

        struct
        {
            WORD                wVID;
            DWORD               dwBoundSheetRecOffset;

            DWORD               dwBookEntry;
        } stXL4;

        // PowerPoint

        struct
        {
            DWORD               dwPowerPointDocumentEntry;
        } stPP;
    } u;
} MSOFFICE_SCAN_T, FAR *LPMSOFFICE_SCAN;

int MicrosoftOfficeScanCB
(
    LPSS_DIR_ENTRY      lpstEntry,  // Ptr to the entry
    DWORD               dwIndex,    // The entry's index in the directory
    LPVOID              lpvCookie
);

EXTSTATUS MicrosoftOfficeScan
(
    LPMSOFFICE_SCAN         lpstMSOfficeScan,   // Info about infection streams
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 1400 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
);

#endif // #ifndef _OFFCSC15_H_
