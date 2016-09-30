//***********************************************************************
//
// $Header:   S:/NAVEX/VCS/xl5scn15.h_v   1.0   09 Jul 1997 16:16:42   DCHI  $
//
// Description:
//      Contains prototype for Excel macro virus scanning.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/xl5scn15.h_v  $
// 
//    Rev 1.0   09 Jul 1997 16:16:42   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _XL5SCN15_H_

#define _XLSCN15_H_

EXTSTATUS XL5ScanBuffer
(
    LPBYTE              lpbyBuffer,
    DWORD               dwBufferSize,
    LPWORD              lpwVID
);

EXTSTATUS XL5Scan
(
    LPMSOFFICE_SCAN     lpstOffcScan
);

#endif // _XL5SCN15_H_

