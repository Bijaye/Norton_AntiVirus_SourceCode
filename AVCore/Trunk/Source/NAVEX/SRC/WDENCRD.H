//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDENCRD.H_v   1.0   17 Jan 1997 11:24:56   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDENCRD.H_v  $
// 
//    Rev 1.0   17 Jan 1997 11:24:56   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDENCRD_H_

#define _WDENCRD_H_

WORD WordDocEncryptedRead
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPWDENCKEY          lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    WORD                wBytesToRead
);

#endif // #ifndef _WDENCRD_H_

