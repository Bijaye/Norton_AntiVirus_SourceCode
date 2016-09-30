//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDENCWR.H_v   1.0   17 Jan 1997 11:24:58   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDENCWR.H_v  $
// 
//    Rev 1.0   17 Jan 1997 11:24:58   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDENCWR_H_

#define _WDENCWR_H_

WORD WordDocEncryptedWrite
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPWDENCKEY          lpstKey,
    DWORD               dwOffset,
    LPBYTE              lpbyBuffer,
    WORD                wBytesToWrite
);

#endif // #ifndef _WDENCWR_H_

