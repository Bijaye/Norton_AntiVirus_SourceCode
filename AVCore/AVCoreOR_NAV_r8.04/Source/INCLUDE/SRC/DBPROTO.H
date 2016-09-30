// Copyright 1994 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:\include\vcs\dbproto.h_v   1.0   02 Sep 1994 17:24:38   CAREY  $
//
// Description: HASH DB prototypes
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:\include\vcs\dbproto.h_v  $
// 
//    Rev 1.0   02 Sep 1994 17:24:38   CAREY
// Initial revision.
//************************************************************************

DBSTATUS    WINAPI      DBNumEntries(LPDBC4_HEADER  lpHeader,
                                     LPDWORD        lpdwNumEntries);

DBSTATUS    WINAPI      DBGetEntry(LPDBC4_HEADER    lpHeader,
                                   DWORD            dwEntryNum,
                                   LPVOID           lpvPutHere);

DBSTATUS    WINAPI      DBGetEntryLow(LPDBC4_HEADER    lpHeader,
                                      DWORD            dwEntryNum,
                                      LPVOID           lpvPutHere);

DBSTATUS    WINAPI      DBInsert(LPDBC4_HEADER  lpHeader,
                                 LPVOID         lpvAddThis,
                                 LPVOID         lpWorkBuffer);

DBSTATUS    WINAPI      DBInsertLow(LPDBC4_HEADER  lpHeader,
                                    DWORD          dwIndexAddr,
                                    LPVOID         lpvAddThis,
                                    LPVOID         lpWorkBuffer);

DBSTATUS    WINAPI      DBInsertTopLevel(LPDBC4_HEADER  lpHeader,
                                         DWORD          dwIndexAddr,
                                         LPVOID         lpvAddThis);

DBSTATUS    WINAPI      DBUpdateOrInsert(LPDBC4_HEADER      lpHeader,
                                         DWORD              dwIndexAddr,
                                         DWORD              dwDataAddr,
                                         LPVOID             lpvAddThis,
                                         LPVOID             lpWorkBuffer);

DBSTATUS    WINAPI      DBDelete(LPDBC4_HEADER  lpHeader,
                                 LPVOID         lpvLookFor,
                                 LPVOID         lpvWork);

DBSTATUS    WINAPI      DBDeleteLow(LPDBC4_HEADER  lpHeader,
                                    DWORD          dwIndexAddr,
                                    LPVOID         lpvLookFor,
                                    LPVOID         lpvWork);

DBSTATUS    WINAPI      DBSearchQuick(LPDBC4_HEADER lpHeader,
                                      LPVOID        lpvLookFor,
                                      LPWORD        lpwBuffer);

DBSTATUS    WINAPI      DBClose(LPDBC4_HEADER lpHeader);

DBSTATUS    WINAPI      DBOpen(LPSTR         lpszFileName,
                               DWORD         dwIndexSize,
                               WORD          wElemSize,
                               BOOL          (WINAPI FAR *CompareFunc)(LPVOID,
                                                                       LPVOID),
                               WORD          (WINAPI FAR *HashFunc)(LPVOID),
                               WORD          (WINAPI FAR *QuickFunc)(LPVOID),
                               BYTE          (WINAPI FAR *BucketFunc)(LPVOID),
                               LPDBC4_HEADER lpHeader);

DBSTATUS    WINAPI      DBOpenCreate(LPSTR         lpszFileName,
                                     DWORD         dwIndexSize,
                                     WORD          wElemSize,
                                     BOOL          (WINAPI FAR *CompareFunc)(LPVOID,
                                                                      LPVOID),
                                     WORD          (WINAPI FAR *HashFunc)(LPVOID),
                                     WORD          (WINAPI FAR *QuickFunc)(LPVOID),
                                     BYTE          (WINAPI FAR *BucketFunc)(LPVOID),
                                     LPDBC4_HEADER lpHeader);

DBSTATUS    WINAPI      DBLoadQuick(LPDBC4_HEADER lpHeader,
                                    LPWORD        lpBuffer);

BOOL        WINAPI      DBSeekToIndexEntry(LPDBC4_HEADER    lpHeader,
                                           DWORD            dwIndexEntry);

BOOL        WINAPI      DBSeekToDataRecord(LPDBC4_HEADER    lpHeader,
                                           DWORD            dwDataAddr);

BOOL        WINAPI      DBSeekToDataChain(LPDBC4_HEADER     lpHeader,
                                          DWORD             dwLastRec);

BOOL        WINAPI      DBSeekToQuickEntry(LPDBC4_HEADER    lpHeader,
                                           DWORD            dwIndexAddr);

DWORD       WINAPI      DBFindFreeDataEntry(LPDBC4_HEADER   lpHeader);

BOOL        WINAPI      DBLockIndexEntry(LPDBC4_HEADER  lpHeader,
                                         DWORD          dwIndexAddr);

BOOL        WINAPI      DBUnlockIndexEntry(LPDBC4_HEADER  lpHeader,
                                           DWORD          dwIndexAddr);

BOOL        WINAPI      DBLockHeader(LPDBC4_HEADER  lpHeader);

BOOL        WINAPI      DBUnlockHeader(LPDBC4_HEADER  lpHeader);

DBSTATUS    WINAPI      DBSearch(LPDBC4_HEADER  lpHeader,
                                 LPVOID         lpvLookFor,
                                 LPVOID         lpvPutHere);

DBSTATUS    WINAPI      DBSearchLow(LPDBC4_HEADER  lpHeader,
                                    DWORD          dwIndexAddr,
                                    LPVOID         lpvLookFor,
                                    LPVOID         lpvPutHere);

DBSTATUS    WINAPI      DBCreate(LPSTR         lpszFileName,
                                 DWORD         dwIndexSize,
                                 WORD          wElemSize,
                                 BOOL          (WINAPI FAR *CompareFunc)(LPVOID,LPVOID),
                                 WORD          (WINAPI FAR *HashFunc)(LPVOID),
                                 WORD          (WINAPI FAR *QuickFunc)(LPVOID),
                                 BYTE          (WINAPI FAR *BucketFunc)(LPVOID),
                                 LPDBC4_HEADER lpHeader);

BOOL        WINAPI      DBSeekToBucketInfoEntry(LPDBC4_HEADER    lpHeader,
                                                DWORD            dwIndexAddr);

DBSTATUS    WINAPI      DBUpdateBucketEntry(LPDBC4_HEADER       lpHeader,
                                            DWORD               dwIndexEntry,
                                            LPBYTE              lpWorkBuffer);

DBSTATUS    WINAPI      DBLoadBucket(LPDBC4_HEADER lpHeader,
                                     LPWORD        lpBuffer);

BYTE        WINAPI      DBGetQuickBucket(LPDBC4_HEADER lpHeader,
                                         LPVOID        lpvLookFor,
                                         LPBYTE        lpwBuffer);
