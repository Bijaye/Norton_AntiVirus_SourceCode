//************************************************************************
//
// $Header:   S:/NAVEX/VCS/offcscan.h_v   1.1   07 Apr 1997 18:46:16   DCHI  $
//
// Description:
//      Contains Microsoft Office macro scan function prototype.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/offcscan.h_v  $
// 
//    Rev 1.1   07 Apr 1997 18:46:16   DCHI
// Added code to speed up excel scanning by being more selective.
// 
//    Rev 1.0   14 Mar 1997 16:36:02   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _OFFCSCAN_H_

#define _OFFCSCAN_H_

#define OLE_SCAN_WD7            1
#define OLE_SCAN_WD8            2
#define OLE_SCAN_XL97           3
#define OLE_SCAN_XL5            4

typedef struct tagMSOFFICE_SCAN
{
    // The type: OLE_SCAN_{WD7,WD8,XL97}

    WORD            wType;

    union
    {
        // Word 6.0/7.0

        struct
        {
            WORD    wWordDocumentEntry;
        } stWD7;

        // Word 8.0, Excel 97

        VBA5_SCAN_T stVBA5;

        // Excel 5.0/95

        struct
        {
            WORD    wVID;
            WORD    wHexNameEntry;
        } stXL5;
    } u;
} MSOFFICE_SCAN_T, FAR *LPMSOFFICE_SCAN;

EXTSTATUS MicrosoftOfficeScan
(
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // OLE file structure
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 1400 bytes
    LPMSOFFICE_SCAN         lpstMSOfficeScan,   // Info about infection streams
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
);

#endif // #ifndef _OFFCSCAN_H_
