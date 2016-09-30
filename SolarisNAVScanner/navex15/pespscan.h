// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVEX/VCS/pespscan.h_v   1.13   14 Jan 1999 18:06:12   DKESSNE  $
//
// Description:
//   This file contains special pe detection prototypes/consts
//
//***************************************************************************
// $Log:   S:/NAVEX/VCS/pespscan.h_v  $
// 
//    Rev 1.13   14 Jan 1999 18:06:12   DKESSNE
// changed prototype for DecryptBufferWithKey
// 
//    Rev 1.12   14 Jan 1999 17:20:50   DKESSNE
// reverted back
// 
//    Rev 1.10   24 Nov 1998 11:52:40   DKESSNE
// added prototype for LocateParvoStart
// 
//    Rev 1.9   24 Nov 1998 09:13:28   DKESSNE
// added prototype for ScanParvo
// 
//    Rev 1.8   12 Nov 1998 18:30:50   DKESSNE
// added two prototypes for W95.HPS detection
// 
//    Rev 1.7   21 Oct 1998 18:02:22   DKESSNE
// added prototype for ScanLibertine
// 
//    Rev 1.6   12 Oct 1998 18:14:06   DKESSNE
// added DECRYPT_NEG flag
// 
//    Rev 1.5   10 Oct 1998 16:42:48   DKESSNE
// modified GenericDecrypt prototype, added flag DECRYPT_DELTA, and added
// prototype for ScanMemorial
// 
//    Rev 1.4   10 Oct 1998 14:57:28   DKESSNE
// added prototype for ScanMemorial
// 
//    Rev 1.3   25 Aug 1998 12:27:06   CNACHEN
// added w95.inca detection, commented out with #ifdef SARC_ZOO_DARRENK
// 
//    Rev 1.2   13 Aug 1998 18:34:10   PDEBATS
// Removed SARC_CST_MARBURGB #ifdef/#endif statements
// 
//    Rev 1.1   13 Aug 1998 13:33:52   jwilber
// Made detections to detect Marburg.B, VID 61a1.  These mods were
// #ifdef'd with SARC_CST_MARBURGB.
//
//    Rev 1.0   27 Jul 1998 12:11:48   DKESSNE
// Initial revision.
//
//***************************************************************************

#ifndef _PESPSCAN_H_

#define _PESPSCAN_H_

typedef struct MarbScan
{
    DWORD   dwSize;                     // Size of Marburg variant
    WORD    wVID, wSigLen;              // VID, length of signature
    LPWORD  lpwSig;                     // Pointer to sig for Marburg variant
} MARBDATA;

EXTSTATUS GenericDecrypt
(
    LPBYTE  lpbyBuffer,
    LPWORD  lpwSignature,
    WORD    wLength,
    BYTE    byFlags,
    LPBYTE  lpbyDecryptFlags,
    LPDWORD lpdwDecryptKey,
    LPDWORD lpdwDecryptDelta
);

EXTSTATUS DecryptBufferWithKey
(
    LPBYTE  lpbyBuffer,
    WORD    wLength,
    BYTE    byFlags,
    DWORD   dwKey,
    DWORD   dwDelta
);

EXTSTATUS RVAToFileOffset
(
    LPCALLBACKREV1              lpCallBack,        // File op callbacks
    HFILE                       hFile,             // Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,        // PE header
    LPBYTE                      lpbyWorkBuffer,    // Work buffer >= 512 bytes
    DWORD                       dwWinHeaderOffset, // where's the WIN header start?
    DWORD                       dwRVA,             // RVA to search for
    LPDWORD                     lpdwFileOffset     // returned
);


EXTSTATUS   ScanMarburg
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,         // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwSectionOffset,    // Start of section
    DWORD                       dwSegLength,        // Length of section
    DWORD                       dwEPOffset,         // Entry Point offset
    LPWORD                      lpwVID              // Virus ID storage on hit
);

EXTSTATUS   ScanInca
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,         // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwSectionOffset,    // Start of section
    DWORD                       dwSegLength,        // Length of section
    DWORD                       dwEPOffset,         // Entry Point offset
    LPWORD                      lpwVID              // Virus ID storage on hit
);


EXTSTATUS   LocateMarburgStart
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,         // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwEPOffset,         // Entry Point offset
    LPDWORD                     lpdwVirusOffset,    // virus file offset
    LPDWORD                     lpdwDecryptKey,     // key for repair
    LPBYTE                      lpbyDecryptFlags,   // flags for repair
    LPWORD                      lpwVirusBytesAtEP,  // # of bytes to repair
    LPWORD                      lpwVID,             // VID of Marburg found
    LPDWORD                     lpdwLastSectionHeaderOffset //for repair
);

extern WORD     wNumMarburgs;   // Number of Marburg sigs in pespscan.cpp
extern MARBDATA MarbTab[];      // Marburg sig data in pespscan.cpp

#define DECRYPT_BYTE    0x01
#define DECRYPT_WORD    0x02
#define DECRYPT_DWORD   0x04
#define DECRYPT_XOR     0x08
#define DECRYPT_ADD     0x10
#define DECRYPT_ROT     0x20
#define DECRYPT_DELTA   0x40
#define DECRYPT_NEG     0x80


EXTSTATUS   ScanMemorial
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEHeader,       // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwSectionOffset,    // Start of section
    DWORD                       dwSegLength,        // Length of section
    DWORD                       dwEPOffset,         // Entry Point offset
    LPWORD                      lpwVID              // Virus ID storage on hit
);

EXTSTATUS   ScanLibertine
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEHeader,       // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwSectionOffset,    // Start of section
    DWORD                       dwSegLength,        // Length of section
    DWORD                       dwEPOffset,         // Entry Point offset
    LPWORD                      lpwVID              // Virus ID storage on hit
);


EXTSTATUS   LocateHPSStart
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEHeader,       // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwEPOffset,         // Entry Point offset
    LPDWORD                     lpdwVirusOffset,    // storage: virus file offset
    LPDWORD                     lpdwDecryptKey,     // storage: key for repair
    LPBYTE                      lpbyDecryptFlags    // storage: flags for repair
);

EXTSTATUS   ScanHPS
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEHeader,       // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwSectionOffset,    // Start of section
    DWORD                       dwSegLength,        // Length of section
    DWORD                       dwEPOffset,         // Entry Point offset
    LPWORD                      lpwVID              // Virus ID storage on hit
);


EXTSTATUS   LocateParvoStart
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEHeader,       // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwEPOffset,         // Entry Point offset
    LPDWORD                     lpdwVirusOffset,    // storage: virus file offset
    LPDWORD                     lpdwDecryptKey,     // storage: key for repair
    LPBYTE                      lpbyDecryptFlags    // storage: flags for repair
);

EXTSTATUS   ScanParvo
(
    LPCALLBACKREV1              lpCallBack,         // File op callbacks
    HFILE                       hFile,              // Handle to file to scan
    LPBYTE                      lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEHeader,       // PE header
    DWORD                       dwPEHeaderOffset,   // Offset of PE header
    DWORD                       dwSectionOffset,    // Start of section
    DWORD                       dwSegLength,        // Length of section
    DWORD                       dwEPOffset,         // Entry Point offset
    LPWORD                      lpwVID              // Virus ID storage on hit
);



#endif // #ifndef _PESPSCAN_H_
