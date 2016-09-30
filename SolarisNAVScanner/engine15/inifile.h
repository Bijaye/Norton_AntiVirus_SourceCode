
#ifndef _INIFILE_H_

#define _INIFILE_H_

int GetProfileString
(
    HFILE               hFile,
    LPSTR               lpszSection,
    LPSTR               lpszKey,
    LPSTR               lpszDefault,
    LPSTR               lpszOut,
    int                 nOutBufferSize
);



int GetProfileInt
(
    HFILE               hFile,
    LPSTR               lpszSection,
    LPSTR               lpszKey,
    int                 nDefault,
    LPINT               lpnOut
);


#endif
