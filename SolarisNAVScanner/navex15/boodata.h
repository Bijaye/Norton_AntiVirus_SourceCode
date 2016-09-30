#ifndef _BOO_DATA_H

#define _BOO_DATA_H

#define BOOT_FUNC_SCAN              0
#define BOOT_FUNC_MATCH             1
#define BOOT_FUNC_BINGO             2

typedef struct
{
    LPBYTE              lpbyData;
//    LPSTR               lpszDescriptor;     // to be removed...
} SIG_TYPE_T, *LPSIG_TYPE;


#endif // #ifndef _BOO_DATA_H
