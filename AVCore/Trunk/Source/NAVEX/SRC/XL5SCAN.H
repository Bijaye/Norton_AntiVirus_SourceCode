//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/xl5scan.h_v   1.0   07 Apr 1997 18:41:14   DCHI  $
//
// Description:
//      Contains prototype for Excel macro virus scanning.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl5scan.h_v  $
// 
//    Rev 1.0   07 Apr 1997 18:41:14   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL5SCAN_H_

#define _XL5SCAN_H_

EXTSTATUS XL5Scan
(
    LPCALLBACKREV1  lpstCallBack,       // File op callbacks
    LPOLE_FILE      lpstOLEFile,        // OLE file structure of hex stream
    LPBYTE          lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPWORD          lpwVID              // Virus ID storage on hit
);

#endif // _XL5SCAN_H_

