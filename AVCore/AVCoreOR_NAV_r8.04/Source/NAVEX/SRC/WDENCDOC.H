//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WDENCDOC.H_v   1.0   17 Jan 1997 11:24:54   DCHI  $
//
// Description:
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WDENCDOC.H_v  $
// 
//    Rev 1.0   17 Jan 1997 11:24:54   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef _WDENCDOC_H_

#define _WDENCDOC_H_

#define WD_FLAG_DOT                 0x0001
#define WD_FLAG_GLSY                0x0002
#define WD_FLAG_COMPLEX             0x0004
#define WD_FLAG_HAS_PIC             0x0008
#define WD_FLAG_COUNT_QUICK_SAVES   0x00F0
#define WD_FLAG_ENCRYPTED           0x0100
#define WD_FLAG_RESERVED            0x0200
#define WD_FLAG_READ_ONLY_REC       0x0400
#define WD_FLAG_WRITE_RESERVATION   0x0800
#define WD_FLAG_EXT_CHAR            0x1000
#define WD_FLAG_UNUSED              0xE000

typedef struct tagWD_FIB_T
{                                           // Offset
    short               shIdent;            //   0   0
    short               shFIBVersion;       //   2   2
    short               shProductVersion;   //   4   4
    short               shLanguageStamp;    //   6   6
    short               shNext;             //   8   8
    WORD                wFlags;             //  10   A
    WORD                shFIBBackVersion;   //  12   C
    long                lKey;               //  14   E
    BYTE                byEnvironment;      //  18  12
    BYTE                byReserved0;        //  19  13
} WD_FIB_T, FAR *LPWD_FIB;

#define WD_ENC_KEY_LEN      16

//////////////////////////////////////////////////////////////////////
// Reverse key structures
//////////////////////////////////////////////////////////////////////

typedef struct tagWDREVKEY_T
{
    BYTE            byHashLen;

    // This is used to find the reverse key if the hash fits
    //  Must be sixteen bytes

    BYTE            abyRevKey0[WD_ENC_KEY_LEN];

    // This is used to verify the key
    //  Must be byHashLen bytes

    BYTE            abyRevKey1[WD_ENC_KEY_LEN];
} WDREVKEY_T, FAR *LPWDREVKEY, FAR * FAR *LPLPWDREVKEY;

typedef struct tagWDREVKEYRING_T
{
    // Number of reverse keys in this ring

    WORD            wKeyCount;

    // Index of first reverse key pointer in reverse key array

    WORD            wFirstRevKeyIdx;
} WDREVKEYRING_T, FAR *LPWDREVKEYRING;

typedef struct tagWDREVKEYLOCKER_T
{
    // Array of indices into key ring array
    //  If zero, then no such key ring exists

    BYTE            abyRevKeyIdx[256];

    // Pointer to array of pointers to reverse keys

    LPLPWDREVKEY    lpalpstRevKey;

    // Array of reverse key rings.
    //  The key at index I contains reverse keys whose first byte is I

    LPWDREVKEYRING  lpastRing;
} WDREVKEYLOCKER_T, FAR *LPWDREVKEYLOCKER;

//////////////////////////////////////////////////////////////////////
// Key structure
//////////////////////////////////////////////////////////////////////

typedef struct tagWDENCKEY_T
{
    BOOL            bEncrypted;

    // If a key is found, the following two should be set

    WORD            wID;

    DWORD           dwReverseKeyOffset;

    // Aligned on 16-byte boundary

    BYTE            abyKey[WD_ENC_KEY_LEN];
} WDENCKEY_T, FAR *LPWDENCKEY;

//////////////////////////////////////////////////////////////////////
// Function prototypes
//////////////////////////////////////////////////////////////////////

WORD WordDocFindFirstKey
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPBYTE              lpbyWorkBuffer,
    LPWDENCKEY          lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
);

WORD WordDocFindNextKey
(
    LPCALLBACKREV1      lpstCallBack,
    LPOLE_FILE_T        lpstOLEFile,
    LPBYTE              lpbyWorkBuffer,
    LPWDENCKEY          lpstKey,
    LPWDREVKEYLOCKER    lpstLocker
);

#endif // #ifndef _WDENCDOC_H_


