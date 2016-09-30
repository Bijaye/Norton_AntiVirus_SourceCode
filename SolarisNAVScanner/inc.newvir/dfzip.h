//************************************************************************
//
// $Header:   S:/INCLUDE/VCS/dfzip.h_v   1.1   18 Nov 1997 16:08:50   DCHI  $
//
// Description:
//  Header file for ZIP compression interface.
//
//************************************************************************
// $Log:   S:/INCLUDE/VCS/dfzip.h_v  $
// 
//    Rev 1.1   18 Nov 1997 16:08:50   DCHI
// Updated DFZip() prototype.
// 
//    Rev 1.0   11 Nov 1997 16:20:58   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _DFZIP_H_

#define _DFZIP_H_

BOOL DFZipFile
(
    LPDF                lpstDF,
    LPVOID              lpvFileIn,
    WORD                wNameLen,
    LPBYTE              lpabyName,
    DWORD               dwFileSize,
    WORD                wDate,
    WORD                wTime
);

BOOL DFZipCreateCentralDirectory
(
    LPDF                lpstDF
);

#endif // #ifndef _DFZIP_H_

