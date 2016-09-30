//************************************************************************
//
// $Header:   S:/NAVEX/VCS/wd7scan.h_v   1.1   14 Mar 1997 16:33:26   DCHI  $
//
// Description:
//      Header for Word 6.0/7.0 macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/wd7scan.h_v  $
// 
//    Rev 1.1   14 Mar 1997 16:33:26   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.0   13 Feb 1997 13:37:12   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WD7SCAN_H_

#define _WD7SCAN_H_

EXTSTATUS WD7Scan
(
    LPCALLBACKREV1          lpstCallBack,       // File op callbacks
    LPOLE_FILE              lpstOLEFile,        // WordDocument stream
    LPBYTE                  lpbyWorkBuffer,     // Work buffer >= 870 bytes
    LPLPWD_VIRUS_SIG_INFO   lplpstVirusSigInfo  // Virus info storage on hit
);

#endif // #ifndef _WD7SCAN_H_
