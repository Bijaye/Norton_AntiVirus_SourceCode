#ifndef _BOOSCAN_H

#define _BOOSCAN_H

BOOL BloodhoundFindBootVirus
(
    LPCALLBACKREV1              lpstCallBack,
    LPBYTE                      lpbyBuffer,
    LPBOOL                      lpbExcluded
);

#endif // #ifndef _BOOSCAN_H
