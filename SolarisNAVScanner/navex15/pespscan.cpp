// Copyright 1995 Symantec, Peter Norton Product Group
//***************************************************************************
//
// $Header:   S:/NAVEX/VCS/pespscan.cpv   1.23   14 Jan 1999 18:05:56   DKESSNE  $
//
// Description:
//   This file contains special pe detection code
//
//***************************************************************************
// $Log:   S:/NAVEX/VCS/pespscan.cpv  $
// 
//    Rev 1.23   14 Jan 1999 18:05:56   DKESSNE
// changes to GenericDecrypt and DecryptBufferWithKey
// 
//    Rev 1.22   17 Dec 1998 15:09:08   relniti
// REMOVING #ifdef DKZOO
// 
//    Rev 1.21   09 Dec 1998 13:10:06   MMAGEE
// removed #ifdef/endif for SARC_CUST_7982
// 
//    Rev 1.20   24 Nov 1998 12:52:54   DKESSNE
// removed some UL's to get rid of warnings in W32
// 
//    Rev 1.19   24 Nov 1998 12:47:22   DKESSNE
// added scan functions for W95.Parvo
// 
//    Rev 1.18   17 Nov 1998 09:59:40   AOONWAL
// Removed #ifdef SARC_CUST_6689
// 
//    Rev 1.17   12 Nov 1998 21:55:18   Relniti
// Bad assignment == shoulda been =
// 
//    Rev 1.16   12 Nov 1998 19:06:58   DKESSNE
// added special scan functions for W95.HPS
// 
//    Rev 1.15   23 Oct 1998 10:43:22   DKESSNE
// removed some unused local variables from ScanLibertine
// 
//    Rev 1.14   22 Oct 1998 17:53:38   DKESSNE
// added ScanLibertine for W95.Libertine
// 
//    Rev 1.13   14 Oct 1998 12:21:38   MMAGEE
// removed #ifdef/endif for SARC_CST_4577, vid 258b, W95.Inca
// 
//    Rev 1.12   14 Oct 1998 10:46:46   DKESSNE
// removed ifdefs for zoo defs
// 
//    Rev 1.11   13 Oct 1998 13:32:12   DKESSNE
// added Rotate and Negate decryption checking in GenericDecrypt
// 
//    Rev 1.10   10 Oct 1998 16:43:30   DKESSNE
// added ScanMemorial to detect W95.Memorial, and modified GenericDecrypt
// to allow for constant increments of decryption key.
// 
//    Rev 1.9   23 Sep 1998 16:22:54   MMAGEE
// reverting to rev 1.7 to restore SARC_CST_4577 
// 
//    Rev 1.7   17 Sep 1998 17:23:08   JWILBER
// Increased upper limit on segment size in W95.Inca detection, VID 258b.
// This was done for CI 4577.
//
//    Rev 1.6   27 Aug 1998 14:09:16   JWILBER
// Removed SARC_ZOO #ifdefs.
//
//    Rev 1.5   26 Aug 1998 16:56:24   DKESSNE
// added code to LocateMarburgStart to catch some variants of Marburg.A,
// and increased dwSearchDist to 500.
//
//    Rev 1.4   25 Aug 1998 12:27:04   CNACHEN
// added w95.inca detection, commented out with #ifdef SARC_ZOO_DARRENK
//
//    Rev 1.3   13 Aug 1998 18:47:38   PDEBATS
// Removed #ifdef/endif,ifdef: else/endif blocks, and block of ifndef/endif
// for SARC_CST_Margburgb vid 0x61a1
//
//    Rev 1.2   13 Aug 1998 13:36:40   jwilber
// Made modifications to LocateMarburgStart to also work with
// Margburg.B.  Mods are #ifdef'd with SARC_CST_MARBURGB.
//
//    Rev 1.1   27 Jul 1998 14:21:08   DKESSNE
// small change to avoid warning on w32 platform
//
//    Rev 1.0   27 Jul 1998 12:11:16   DKESSNE
// Initial revision.
//
//***************************************************************************

#ifdef SARCBOT
#include "config.h"
#endif

#include "endutils.h"

#include "nepescan.h"
#include "winsig.h"

#include "winconst.h"

#include "nepeshr.h"

#include "pespscan.h"

#include "navexvid.h"

#define MAXMARSIGLEN    32          // Maximum length of Marburg sig

#define MARASIGLEN      16          // Length of Marburg.A sig below

WORD        wMarbSig[MARASIGLEN] =  {   0xe8, 0x00, 0x00, 0x00,
                                        0x00, 0x5d, 0x8b, 0xdd,
                                        0x81, 0xed, 0x05, 0x00,
                                        0x42, 0x00, 0x81, 0xeb  };

MARBDATA    MarbTab[] = {   0x00001699, VID_MARBURG, MARASIGLEN, wMarbSig,
                            0x000016a1, VID_MARBURGB, MARASIGLEN, wMarbSig  };

WORD        wNumMarburgs = sizeof(MarbTab) / sizeof(MARBDATA);




//********************************************************************
//
// Function:
//  EXTSTATUS GenericDecrypt()
//
// Description:
//  Verifies that a buffer has been encrypted using add/xor
//
// Expects:
//  lpbyBuffer      --  Buffer to verify
//  lpwSignature    --  Signature to look for
//                      (byte array stored as a word array)
//  wLength         --  Number of bytes to verify
//  byFlags         --  type of decryption to attempt:
//                          DECRYPT_BYTE, DECRYPT_WORD, DECRYPT_DWORD
//                          DECRYPT_XOR, DECRYPT_ADD
//
//
// Returns:
//  EXTSTATUS_OK                If buffer matches signature
//  EXTSTATUS_ENTRY_NOT_FOUND   If it doesn't match
//
//  If the buffer matches, *lpbyDecryptFlags, *lpdwDecryptKey, and
//  *lpdwDecryptDelta are set to reflect the decryption used.
//
//********************************************************************


EXTSTATUS GenericDecrypt
(
    LPBYTE  lpbyBuffer,
    LPWORD  lpwSignature,
    WORD    wLength,
    BYTE    byFlags,
    LPBYTE  lpbyDecryptFlags,
    LPDWORD lpdwDecryptKey,
    LPDWORD lpdwDecryptDelta
)
{

    //lpwSignature should be a byte array stored as a word array, to avoid
    //FP's with AV products

    WORD    i;
    BYTE    byKey, byInitKey, byDelta = 0;
    WORD    wKey, wInitKey, wDelta = 0, wTemp;
    DWORD   dwKey, dwInitKey, dwDelta = 0, dwTemp;


    if (wLength <= 0)
        return EXTSTATUS_ENTRY_NOT_FOUND;

    if ( (byFlags & DECRYPT_XOR) && (byFlags & DECRYPT_BYTE) )
    {
        byKey = lpbyBuffer[0] ^ (BYTE) lpwSignature[0];

        byInitKey = byKey;

        byDelta = (lpbyBuffer[1] ^ (BYTE) lpwSignature[1]) - byKey;
 
        for (i=0;i<wLength;i++)
        {
            if ( byKey != (BYTE)(lpbyBuffer[i] ^ (BYTE) lpwSignature[i]))
                break;

            if (byFlags & DECRYPT_DELTA)
                byKey += byDelta;
        }
 
        if (i >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_BYTE | DECRYPT_XOR;
            if (byFlags & DECRYPT_DELTA)
                *lpbyDecryptFlags |= DECRYPT_DELTA;
            *lpdwDecryptKey   = (DWORD) byInitKey;
            *lpdwDecryptDelta = (DWORD) byDelta;
            return EXTSTATUS_OK;
        }
    }//xor byte


    if ( (byFlags & DECRYPT_XOR) && (byFlags & DECRYPT_WORD) )
    {
        wKey = ( AVDEREF_WORD(lpbyBuffer) ^
                 ( lpwSignature[0] | (lpwSignature[1]<<8) ) );

        wInitKey = wKey;

        wDelta = ( AVDEREF_WORD(lpbyBuffer) ^
                    ( lpwSignature[2] | (lpwSignature[3]<<8) ) )
                 - wKey;


        for (i=0;i+1<wLength;i+=2)
        {
            if (wKey != (WORD)(( AVDEREF_WORD(lpbyBuffer+i) ^
                              ( lpwSignature[i] | (lpwSignature[i+1]<<8) ) ) ))
                break;

            if (byFlags & DECRYPT_DELTA)
                wKey += wDelta;
        }

        if (i+1 >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_WORD | DECRYPT_XOR;
            if (byFlags & DECRYPT_DELTA)
                *lpbyDecryptFlags |= DECRYPT_DELTA;
            *lpdwDecryptKey   = (DWORD) wInitKey;
            *lpdwDecryptDelta = (DWORD) wDelta;
            return EXTSTATUS_OK;
        }
    }//xor word


    if ( (byFlags & DECRYPT_XOR) && (byFlags & DECRYPT_DWORD) )
    {
        dwKey = ( AVDEREF_DWORD(lpbyBuffer) ^
                    ( ( (DWORD) lpwSignature[0] )      |
                      (( (DWORD) lpwSignature[1] )<<8)   |
                      (( (DWORD) lpwSignature[2] )<<16)  |
                      (( (DWORD) lpwSignature[3] )<<24) ) );

        dwInitKey = dwKey;

        dwDelta = ( AVDEREF_DWORD(lpbyBuffer) ^
                    ( (  (DWORD) lpwSignature[4]     )   |
                      (( (DWORD) lpwSignature[5] )<<8)   |
                      (( (DWORD) lpwSignature[6] )<<16)  |
                      (( (DWORD) lpwSignature[7] )<<24) ) )
                  - dwKey;

        for (i=0;i+3<wLength;i+=4)
        {
            if ( dwKey != (DWORD)(( AVDEREF_DWORD(lpbyBuffer+i) ^
                                  ( ( (DWORD) lpwSignature[i]   )     |
                                  (( (DWORD) lpwSignature[i+1] )<<8)  |
                                  (( (DWORD) lpwSignature[i+2] )<<16) |
                                  (( (DWORD) lpwSignature[i+3] )<<24 )) ) ) )
                break;

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        if (i+3 >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_DWORD | DECRYPT_XOR;
            if (byFlags & DECRYPT_DELTA)
                *lpbyDecryptFlags |= DECRYPT_DELTA;
            *lpdwDecryptKey   = (DWORD) dwInitKey;
            *lpdwDecryptDelta = (DWORD) dwDelta;
            return EXTSTATUS_OK;
        }
    }//xor dword


    if ( (byFlags & DECRYPT_ADD) && (byFlags & DECRYPT_BYTE) )
    {
        byKey = lpbyBuffer[0] - (BYTE) lpwSignature[0];

        byInitKey = byKey;

        byDelta = (lpbyBuffer[1] - (BYTE) lpwSignature[1]) - byKey;

        for (i=0;i<wLength;i++)
        {
            if ( byKey != (BYTE)(lpbyBuffer[i] - (BYTE) lpwSignature[i]))
                break;

            if (byFlags & DECRYPT_DELTA)
                byKey += byDelta;
        }
 
        if (i >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_BYTE | DECRYPT_ADD;
            if (byFlags & DECRYPT_DELTA)
                *lpbyDecryptFlags |= DECRYPT_DELTA;
            *lpdwDecryptKey   = (DWORD) byInitKey;
            *lpdwDecryptDelta = (DWORD) byDelta;
            return EXTSTATUS_OK;
        }
    }//add byte


    if ( (byFlags & DECRYPT_ADD) && (byFlags & DECRYPT_WORD) )
    {
        wKey = ( (AVDEREF_WORD(lpbyBuffer)) -
                    ( lpwSignature[0] | (lpwSignature[1]<<8) ) );

        wInitKey = wKey;

        wDelta = ( (AVDEREF_WORD(lpbyBuffer)) -
                    ( lpwSignature[2] | (lpwSignature[3]<<8) ) )
                 - wKey;

        for (i=0;i+1<wLength;i+=2)
        {
            if (wKey != (WORD)( AVDEREF_WORD(lpbyBuffer+i) -
                              ( lpwSignature[i] | (lpwSignature[i+1]<<8) ) ) )
                break;

            if (byFlags & DECRYPT_DELTA)
                wKey += wDelta;
        }

        if (i+1 >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_WORD | DECRYPT_ADD;
            if (byFlags & DECRYPT_DELTA)
                *lpbyDecryptFlags |= DECRYPT_DELTA;
            *lpdwDecryptKey   = (DWORD) wInitKey;
            *lpdwDecryptDelta = (DWORD) wDelta;
            return EXTSTATUS_OK;
        }
    }//add word


    if ( (byFlags & DECRYPT_ADD) && (byFlags & DECRYPT_DWORD) )
    {
        dwKey = ( AVDEREF_DWORD(lpbyBuffer) -
                    ( ( (DWORD) lpwSignature[0] )      |
                      ( (DWORD) lpwSignature[1] )<<8   |
                      ( (DWORD) lpwSignature[2] )<<16  |
                      ( (DWORD) lpwSignature[3] )<<24 ) );

        dwInitKey = dwKey;

        dwDelta = ( AVDEREF_DWORD(lpbyBuffer) -
                    ( ( (DWORD) lpwSignature[4] )      |
                      ( (DWORD) lpwSignature[5] )<<8   |
                      ( (DWORD) lpwSignature[6] )<<16  |
                      ( (DWORD) lpwSignature[7] )<<24 ) )
                  - dwKey;

        for (i=0;i+3<wLength;i+=4)
        {
            if ( dwKey != (DWORD) ( AVDEREF_DWORD(lpbyBuffer+i) -
                                  ( ( (DWORD) lpwSignature[i]   )     |
                                  (( (DWORD) lpwSignature[i+1] )<<8)  |
                                  (( (DWORD) lpwSignature[i+2] )<<16) |
                                  (( (DWORD) lpwSignature[i+3] )<<24)) ) )
                break;

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        if (i+3 >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_DWORD | DECRYPT_ADD;
            if (byFlags & DECRYPT_DELTA)
                *lpbyDecryptFlags |= DECRYPT_DELTA;
            *lpdwDecryptKey   = (DWORD) dwInitKey;
            *lpdwDecryptDelta = (DWORD) dwDelta;
            return EXTSTATUS_OK;
        }
    }//add dword


    if ( (byFlags & DECRYPT_ROT) && (byFlags & DECRYPT_BYTE) )
    {
        for (dwKey=0; dwKey<8; dwKey++)
        {
            for (i=0;i<wLength;i++)
            {
                if ( ((BYTE)lpwSignature[i]) != (BYTE)
                        ( (lpbyBuffer[i] >> dwKey) |
                          (lpbyBuffer[i] << (8-dwKey)) ) )
                    break;
            }

            if (i >= wLength)
            {
                *lpbyDecryptFlags = DECRYPT_BYTE | DECRYPT_ROT;
                *lpdwDecryptKey   = (DWORD) dwKey;
                return EXTSTATUS_OK;
            }

        }//for each possible rotation

    }//rotate byte


    if ( (byFlags & DECRYPT_ROT) && (byFlags & DECRYPT_WORD) )
    {
        for (dwKey=0; dwKey<16; dwKey++)
        {
            for (i=0;i+1<wLength;i+=2)
            {
                wTemp = (WORD)( lpwSignature[i] | (lpwSignature[i+1]<<8) );

                if ( AVDEREF_WORD(lpwSignature+i) !=
                       (WORD)( (wTemp >> dwKey) | (wTemp << (16-dwKey)) ) )
                    break;
            }

            if (i+1 >= wLength)
            {
                *lpbyDecryptFlags = DECRYPT_WORD | DECRYPT_ROT;
                *lpdwDecryptKey   = (DWORD) dwKey;
                return EXTSTATUS_OK;
            }

        }//for each possible rotation

    }//rotate word


    if ( (byFlags & DECRYPT_ROT) && (byFlags & DECRYPT_DWORD) )
    {
        for (dwKey=0; dwKey<32; dwKey++)
        {
            for (i=0;i+3<wLength;i+=4)
            {
                dwTemp = (DWORD) ( ((DWORD)lpwSignature[i])
                        | ( ((DWORD)lpwSignature[i+1]) <<8 )
                        | ( ((DWORD)lpwSignature[i+2]) <<16)
                        | ( ((DWORD)lpwSignature[i+3]) <<24) );

                if ( AVDEREF_DWORD(lpwSignature+i) !=
                       (DWORD)( (dwTemp >> dwKey) | (dwTemp << (32-dwKey)) ) )
                    break;
            }

            if (i+3 >= wLength)
            {
                *lpbyDecryptFlags = DECRYPT_DWORD | DECRYPT_ROT;
                *lpdwDecryptKey   = (DWORD) dwKey;
                return EXTSTATUS_OK;
            }

        }//for each possible rotation

    }//rotate dword


    if ( (byFlags & DECRYPT_NEG) && (byFlags & DECRYPT_BYTE) )
    {
        for (i=0;i<wLength;i++)
        {
            if ( (BYTE)lpwSignature[i] != (BYTE)(0-lpbyBuffer[i]) )
                break;
        }

        if (i >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_BYTE | DECRYPT_NEG;
            return EXTSTATUS_OK;
        }
    }//negate byte


    if ( (byFlags & DECRYPT_NEG) && (byFlags & DECRYPT_WORD) )
    {
        for (i=0;i+1<wLength;i+=2)
        {
            wTemp = (WORD)( lpwSignature[i] | (lpwSignature[i+1]<<8) );

            if ( AVDEREF_WORD(lpwSignature+i) != (WORD)(0-wTemp) )
                break;
        }

        if (i+1 >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_WORD | DECRYPT_NEG;
            return EXTSTATUS_OK;
        }
    }//negate word


    if ( (byFlags & DECRYPT_NEG) && (byFlags & DECRYPT_DWORD) )
    {
        for (i=0;i+1<wLength;i+=4)
        {
            dwTemp = (DWORD) ( ((DWORD)lpwSignature[i])
                    | ( ((DWORD)lpwSignature[i+1]) <<8 )
                    | ( ((DWORD)lpwSignature[i+2]) <<16)
                    | ( ((DWORD)lpwSignature[i+3]) <<24) );

            if ( AVDEREF_DWORD(lpwSignature+i) != (DWORD)(0-dwTemp) )
                break;
        }

        if (i+3 >= wLength)
        {
            *lpbyDecryptFlags = DECRYPT_DWORD | DECRYPT_NEG;
            return EXTSTATUS_OK;
        }
    }//negate dword

    return EXTSTATUS_ENTRY_NOT_FOUND;

}//GenericDecrypt


//********************************************************************
//
// Function:
//  EXTSTATUS DecryptBufferWithKey()
//
// Description:
//  Decrypts a buffer that has been encrypted using add/xor
//
// Expects:
//  lpbyBuffer      --  Buffer to decrypt
//  wLength         --  number of bytes to decrypt
//  byFlags         --  type of decryption to use:
//                         DECRYPT_BYTE, DECRYPT_WORD, DECRYPT_DWORD (one only)
//                         DECRYPT_XOR, DECRYPT_ADD (one only)
//  dwKey           --  key to use in decryption
//
// Returns:
//  EXTSTATUS_OK                If everything went ok
//  EXTSTATUS_ENTRY_NOT_FOUND   If something went wrong
//
//********************************************************************


EXTSTATUS DecryptBufferWithKey
(
    LPBYTE  lpbyBuffer,
    WORD    wLength,
    BYTE    byFlags,
    DWORD   dwKey,
    DWORD   dwDelta
)
{
    WORD i;

    if (wLength <= 0)
        return EXTSTATUS_ENTRY_NOT_FOUND;

    if ( (byFlags & DECRYPT_XOR) && (byFlags & DECRYPT_BYTE) )
    {
        for (i=0;i<wLength;i++)
        {
            lpbyBuffer[i] = (BYTE) (lpbyBuffer[i] ^ ((BYTE)dwKey));

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        return EXTSTATUS_OK;
    }//xor byte


    if ( (byFlags & DECRYPT_XOR) && (byFlags & DECRYPT_WORD) )
    {
        for (i=0;i<wLength;i+=2)
        {
            AVASSIGN_WORD(lpbyBuffer+i,
                   (WORD) ( AVDEREF_WORD(lpbyBuffer+i) ^ ((WORD)dwKey) ) );

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        return EXTSTATUS_OK;
    }//xor word


    if ( (byFlags & DECRYPT_XOR) && (byFlags & DECRYPT_DWORD) )
    {
        for (i=0;i<wLength;i+=4)
        {
            AVASSIGN_DWORD(lpbyBuffer+i,
                          AVDEREF_DWORD(lpbyBuffer+i) ^ ((DWORD)dwKey) );

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        return EXTSTATUS_OK;
    }//xor dword


    if ( (byFlags & DECRYPT_ADD) && (byFlags & DECRYPT_BYTE) )
    {
        for (i=0;i<wLength;i++)
        {
            lpbyBuffer[i] = (BYTE) (lpbyBuffer[i] - ((BYTE)dwKey));

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        return EXTSTATUS_OK;
    }//add byte


    if ( (byFlags & DECRYPT_ADD) && (byFlags & DECRYPT_WORD) )
    {
        for (i=0;i<wLength;i+=2)
        {
            AVASSIGN_WORD(lpbyBuffer+i,
                   (WORD) (AVDEREF_WORD(lpbyBuffer+i) - ((WORD)dwKey)) );

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        return EXTSTATUS_OK;
    }//add word


    if ( (byFlags & DECRYPT_ADD) && (byFlags & DECRYPT_DWORD) )
    {
        for (i=0;i<wLength;i+=4)
        {
            AVASSIGN_DWORD(lpbyBuffer+i,
                          AVDEREF_DWORD(lpbyBuffer+i) - ((DWORD)dwKey) );

            if (byFlags & DECRYPT_DELTA)
                dwKey += dwDelta;
        }

        return EXTSTATUS_OK;
    }//add dword


    return EXTSTATUS_ENTRY_NOT_FOUND;


}//DecryptBufferWithKey




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
)
{

    WORD    wResult;

    //unused storage
    DWORD   dwVirusOffset;
    DWORD   dwDecryptKey;
    BYTE    byDecryptFlags;
    WORD    wVirusBytesAtEP;
    DWORD   dwLastSectionHeaderOffset;

    wResult = LocateMarburgStart(lpCallBack,
                                   hFile,
                                   lpbyWorkBuffer,
                                   lpstPEInfo,
                                   dwPEHeaderOffset,
                                   dwEPOffset,
                                   &dwVirusOffset,
                                   &dwDecryptKey,
                                   &byDecryptFlags,
                                   &wVirusBytesAtEP,
                                   lpwVID,
                                   &dwLastSectionHeaderOffset);

    return(wResult);    // lpwVID is set in LocateMarburgStart
}//ScanMarburg



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
    LPWORD                      lpwVID,             // VID of Marburg Variant found
    LPDWORD                     lpdwLastSectionHeaderOffset //for repair
)
{
    BYTE    byBuffer[MAXMARSIGLEN];
    DWORD   dwVirusEPRVA, dwVirusEPOffset, dwVirusOffset, dwBytes;
    WORD    wEPSearchLength = 500;
    WORD    i;

    DWORD   dwVirusSecOffset, dwVirusSecRVA;

    WORD    j;

    BYTE    byFlags = DECRYPT_XOR | DECRYPT_ADD |
                      DECRYPT_BYTE | DECRYPT_WORD | DECRYPT_DWORD;

    DWORD   dwDecryptDelta;       //unused storage for GenericDecrypt


    //to read in the last section
    DWORD                       dwLastSectionOffset;
    NAVEX_IMAGE_SECTION_HEADER  stLastSection;

    //check that filesize is divisible by 101 -- if not, no virus
    if ( lpCallBack->FileSize(hFile) % 101 != 0 )
        return EXTSTATUS_OK;

    //read in last section from section table

    dwLastSectionOffset = dwPEHeaderOffset +
                          sizeof(NAVEX_IMAGE_NT_HEADERS) +
                          (WENDIAN(lpstPEInfo->FileHeader.NumberOfSections) - 1)
                            * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwLastSectionOffset !=
            lpCallBack->FileSeek(hFile, dwLastSectionOffset, SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;

    if (sizeof(NAVEX_IMAGE_SECTION_HEADER) !=
            lpCallBack->FileRead(hFile,
                                 &stLastSection,
                                 sizeof(NAVEX_IMAGE_SECTION_HEADER) ) )
        return EXTSTATUS_FILE_ERROR;


    //if last section is not writeable, no virus
    if ( !(DWENDIAN(stLastSection.Characteristics) & NAVEX_IMAGE_SCN_MEM_WRITE) )
        return EXTSTATUS_OK;


    //read in bytes from entry point

    if (dwEPOffset != lpCallBack->FileSeek(hFile, dwEPOffset, SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;

    if ((dwBytes = lpCallBack->FileRead(hFile, lpbyWorkBuffer, 512)) == UERROR)
        return EXTSTATUS_FILE_ERROR;


    wEPSearchLength =
        (WORD) ( (wEPSearchLength < dwBytes) ? wEPSearchLength : (WORD) dwBytes );

    //check to see which section EP is in (Marburg.A can have EP in last
    //section) if EP is in the last section
    if
    (
        ( DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint)
            >= DWENDIAN(stLastSection.VirtualAddress) ) &&
        ( DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint)
            < DWENDIAN(stLastSection.VirtualAddress) +
              DWENDIAN(stLastSection.SizeOfRawData) )
    )
    {

        for (i=0;i<wEPSearchLength;i++)
        {
            if (lpbyWorkBuffer[i] == 0xe9)
            {
                //check if the jump is backwards
                if (AVDEREF_WORD(lpbyWorkBuffer+i+3) == 0xffff)
                {
                    //calculate RVA of Virus EP (follow the jump)
                    dwVirusEPRVA =
                        DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint) +
                        i + 5 +
                        AVDEREF_DWORD(lpbyWorkBuffer+i+1);

                    dwVirusSecOffset = DWENDIAN(stLastSection.PointerToRawData);
                    dwVirusSecRVA    = DWENDIAN(stLastSection.VirtualAddress);

                    //this should be the beginning of the actual virus
                    dwVirusOffset = dwVirusSecOffset +
                                        (dwVirusEPRVA - dwVirusSecRVA);

                    // Read in buffer to decrypt, from beginning of virus
                    if (dwVirusOffset != lpCallBack->FileSeek(hFile,
                                                              dwVirusOffset,
                                                              SEEK_SET))
                        break;

                    if (MarbTab[1].wSigLen != lpCallBack->FileRead(hFile,
                                                            byBuffer,
                                                            MarbTab[1].wSigLen))
                        break;

                    //Generic decrypt

                    if (GenericDecrypt( byBuffer,
                                        MarbTab[1].lpwSig,
                                        MarbTab[1].wSigLen,
                                        byFlags,
                                        lpbyDecryptFlags,
                                        lpdwDecryptKey,
                                        &dwDecryptDelta) == EXTSTATUS_OK)
                    {
                        //return repair info
                        *lpdwVirusOffset = dwVirusOffset;
                        *lpwVirusBytesAtEP = i+5;
                        *lpdwLastSectionHeaderOffset = dwLastSectionOffset;
                        *lpwVID = MarbTab[1].wVID;

                        return(EXTSTATUS_VIRUS_FOUND);
                    }

                }//if jump is backwards

            }//if E9

        }//for i < wSearchLength

    }//if EP is in the last section

    else //EP is not in the last section
    {

        for (i=0;i<wEPSearchLength;i++)
        {
            if (lpbyWorkBuffer[i] == 0xe9)
            {

                //calculate RVA of Virus EP (follow the jump)
                dwVirusEPRVA =
                    DWENDIAN(lpstPEInfo->OptionalHeader.AddressOfEntryPoint) +
                    i + 5 +
                    AVDEREF_DWORD(lpbyWorkBuffer+i+1);

                //check if this RVA is in the last section
                if (dwVirusEPRVA >= DWENDIAN(stLastSection.VirtualAddress) &&
                    dwVirusEPRVA < DWENDIAN(stLastSection.VirtualAddress) +
                                    DWENDIAN(stLastSection.SizeOfRawData))
                {

                    dwVirusSecOffset = DWENDIAN(stLastSection.PointerToRawData);
                    dwVirusSecRVA    = DWENDIAN(stLastSection.VirtualAddress);


                    dwVirusEPOffset = dwVirusSecOffset +
                                        (dwVirusEPRVA - dwVirusSecRVA);

                    for (j = 0; j < wNumMarburgs; j++)
                    {
                        // Offset of encrypted virus
                        dwVirusOffset = dwVirusEPOffset - MarbTab[j].dwSize;

                        // Read in buffer to decrypt, from beginning of virus
                        if (dwVirusOffset != lpCallBack->FileSeek(hFile,
                                                                  dwVirusOffset,
                                                                  SEEK_SET))
                            break;      // Break so we can try next value of j

                        if (MarbTab[j].wSigLen != lpCallBack->FileRead(hFile,
                                                                byBuffer,
                                                                MarbTab[j].wSigLen))
                            break;

                        //Generic decrypt

                        if (GenericDecrypt( byBuffer,
                                            MarbTab[j].lpwSig,
                                            MarbTab[j].wSigLen,
                                            byFlags,
                                            lpbyDecryptFlags,
                                            lpdwDecryptKey,
                                            &dwDecryptDelta) == EXTSTATUS_OK)
                        {
                            //return repair info
                            *lpdwVirusOffset = dwVirusOffset;
                            *lpwVirusBytesAtEP = i+5;
                            *lpdwLastSectionHeaderOffset = dwLastSectionOffset;
                            *lpwVID = MarbTab[j].wVID;

                            return(EXTSTATUS_VIRUS_FOUND);
                        }
                    }//for j < wNumMarburgs

                }//if RVA in last section

            }//if E9

        }//for i < wSearchLength

    }//else (EP not in last section)

    //didn't find it
    return(EXTSTATUS_OK);

}//LocateMarburgStart



EXTSTATUS RVAToFileOffset
(
    LPCALLBACKREV1              lpCallBack,        // File op callbacks
    HFILE                       hFile,             // Handle to file to scan
    LPNAVEX_IMAGE_NT_HEADERS    lpstPEInfo,        // PE header
    LPBYTE                      lpbyWorkBuffer,    // Work buffer >= 512 bytes
    DWORD                       dwWinHeaderOffset, // where's the WIN header start?
    DWORD                       dwRVA,             // RVA to search for
    LPDWORD                     lpdwFileOffset     // returned
)
{
    DWORD   dwSecOffset, dwSecLen, dwSecFlags, dwSecRVA;

    if (EXTPELocateSectionWithRVA(lpCallBack,
								  hFile,
								  lpstPEInfo,
    							  lpbyWorkBuffer,
								  dwWinHeaderOffset,
                                  dwRVA,
                                  &dwSecOffset,
								  &dwSecLen,
                                  &dwSecFlags,
                                  &dwSecRVA) != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;

    *lpdwFileOffset = dwSecOffset + (dwRVA - dwSecRVA);

    return EXTSTATUS_OK;


}//RVAToFileOffset

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
)
{
    DWORD                       dwOffset, dwSecLen, dwFlags, dwRVABase;
    int                         i, j;

    // make sure our section is between 18 and 22K

    // 19456 is always used by the virus
    // One variant uses 20480 - changed upper limit from 19600 to 20992
    if (dwSegLength < 19300 || dwSegLength > 20992)
    {
        // not our inca

        return(EXTSTATUS_OK);
    }

    // determine if last segment is executable

    if (GetPESectionInfo(lpCallBack,
                         hFile,
                         lpbyWorkBuffer,
                         dwPEHeaderOffset,
                         WENDIAN(lpstPEInfo->FileHeader.NumberOfSections)-1,
                         &dwOffset,
                         &dwSecLen,
                         &dwFlags,
                         &dwRVABase) != EXTSTATUS_OK)
        	return(EXTSTATUS_FILE_ERROR);

    // last seg must be executable

    if (!(dwFlags & NAVEX_IMAGE_SCN_MEM_EXECUTE))
    {
        return(EXTSTATUS_OK);
    }

    if (dwEPOffset != dwOffset)
    {
        // not our inca: ep = start of segment

        return(EXTSTATUS_OK);
    }

    // find a call statement e8 00 01 00 00 after EP

    if (lpCallBack->FileSeek(hFile,dwEPOffset,SEEK_SET) != dwEPOffset)
        return(EXTSTATUS_FILE_ERROR);

    // must have at least 1024 bytes, guaranteed since the virus
    // is so big!

    if (lpCallBack->FileRead(hFile, lpbyWorkBuffer,1024) != 1024)
        return(EXTSTATUS_FILE_ERROR);

    for (i=0;i<(1024-256-5-1);i++)
    {
        if (lpbyWorkBuffer[i] == 0xE8 &&
            lpbyWorkBuffer[i+1] == 0x00 &&
            lpbyWorkBuffer[i+2] == 0x01 &&
            lpbyWorkBuffer[i+3] == 0x00 &&
            lpbyWorkBuffer[i+4] == 0x00)
        {
            BYTE byShouldBeZero = 0;

            // verify next 256 bytes are all unique.
            // xoring 256 bytes together, with each byte unique will
            // yield a 0.

            for (j=0;j<256;j++)
                byShouldBeZero ^= lpbyWorkBuffer[i+j+5];

            // report virus if they are and we find a pop afterwards

            if (byShouldBeZero == 0 &&
                (lpbyWorkBuffer[i+j+5] & 0xF0) == 0x50)
            {
                *lpwVID = VID_INCA;

                return(EXTSTATUS_VIRUS_FOUND);
            }
        }
    }

    return(EXTSTATUS_OK);

}//ScanInca




WORD    gawMemorialSig[] = { 0xba, 0x00, 0x00, 0xf7, 0xbf,
                             0x8b, 0xc2, 0x8b, 0xd8, 0x03,
                             0x40, 0x3c, 0x03, 0x58, 0x78,
                             0x8b, 0x43, 0x20, 0x03, 0xc2 };


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
)
{
    DWORD                       dwLastSectionOffset;
    NAVEX_IMAGE_SECTION_HEADER  stLastSection;

    //unused storage for GenericDecrypt
    BYTE                        byDecryptFlags;
    DWORD                       dwDecryptKey, dwDecryptDelta;


    //section length (size of raw data) must be 0x2400
    if (dwSegLength != 0x2400)
        return EXTSTATUS_OK;


    //read in last section from section table

    dwLastSectionOffset = dwPEHeaderOffset +
        sizeof(NAVEX_IMAGE_NT_HEADERS) +
        (WENDIAN(lpstPEHeader->FileHeader.NumberOfSections) - 1)
            * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwLastSectionOffset !=
            lpCallBack->FileSeek(hFile, dwLastSectionOffset, SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;

    if (sizeof(NAVEX_IMAGE_SECTION_HEADER) !=
            lpCallBack->FileRead(hFile,
                                 &stLastSection,
                                 sizeof(NAVEX_IMAGE_SECTION_HEADER) ) )
        return EXTSTATUS_FILE_ERROR;


    //EP must be at the beginning of the last section
    if (dwEPOffset != DWENDIAN(stLastSection.PointerToRawData) )
        return EXTSTATUS_OK;


    //Last section characteristics must be 0xE0000040
    if ( DWENDIAN(stLastSection.Characteristics) != 0xE0000040 )
        return EXTSTATUS_OK;


    //read in 66 bytes from entry point (46 for decryptor + 20 for signature)

    if (dwEPOffset != lpCallBack->FileSeek(hFile, dwEPOffset, SEEK_SET))
        return EXTSTATUS_FILE_ERROR;

    if (UERROR == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 66))
        return EXTSTATUS_FILE_ERROR;


    //verify gawMemorialSig at EP+46, encrypted using byte XOR w/ delta
    if ( GenericDecrypt(lpbyWorkBuffer + 46,
                        gawMemorialSig,
                        20,
                        DECRYPT_BYTE | DECRYPT_XOR | DECRYPT_DELTA,
                        &byDecryptFlags,
                        &dwDecryptKey,
                        &dwDecryptDelta) == EXTSTATUS_OK)
    {
        *lpwVID = VID_W95MEMORIAL;
        return EXTSTATUS_VIRUS_FOUND;
    }


    //didn't find it
    return EXTSTATUS_OK;


}//ScanMemorial


//
//  W95.Libertine is encrypted in 128 byte (32 DWORD) blocks.
//
//  The decryption loop begins at the entrypoint, which is at
//  lastsection+0x7000 (== EOF-0xbb8).
//
//  The decryption loop is mostly garbage, but contains 32
//  instructions (one for each DWORD in the block) to do the
//  decryption.  These decryptions are either ADD or XOR.
//  Some use a constant key; others use a register which is
//  modified throughout the loop.
//
//  A 'column' is an array of 8 DWORDS.  The first column is the array
//  consisting of the first DWORD from each of 8 128-byte
//  blocks;  the second column takes the second DWORD from
//  each of 8 blocks, etc.
//
//  The CheckSum of a column is the alternating sum of the elements in
//  the column.  The CheckXor is the total xor of the elements in the
//  column.  If the column has been encrypted using a constant key,
//  the corresponding CheckSum or CheckXor will match, because the
//  key will drop out of the calculation.
//
//  Verification is done using CheckSum/CheckXor on 8 columns, starting
//  at _Mylene_ + 0x600, with 2 matches required for a positive ID of
//  the virus.
//


#define LIBERTINE_NUM_COLUMNS 8

DWORD gadwLibertineCheckSum[] =
{
	0xddc52360,
	0x3435a44c,
	0xecf508f6,
	0x26ee366b,
	0x714ccc28,
	0x1e89b83c,
	0xc5b30377,
    0xafe3e5e9
};

DWORD gadwLibertineCheckXor[] =
{
	0x036ae370,
	0x85b5f8f8,
	0x020caa06,
	0x590ade33,
	0xf620e5e8,
	0x6807e754,
	0x4af11d79,
    0x531de4a9
};


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
)
{
    DWORD                       dwLastSectionOffset;
    NAVEX_IMAGE_SECTION_HEADER  stLastSection;

    DWORD                       dwVirusOffset;
    WORD                        wColumn, i, wMatchCount=0;

    DWORD                       adwCheckSum[LIBERTINE_NUM_COLUMNS];
    DWORD                       adwCheckXor[LIBERTINE_NUM_COLUMNS];


    //section length (size of raw data) must be 0x7bb8
    if (dwSegLength != 0x7bb8)
        return EXTSTATUS_OK;


    //EP must be 0x7000 from beginning of the section
    if (dwEPOffset-dwSectionOffset != 0x7000)
        return EXTSTATUS_OK;


    //read in last section from section table

    dwLastSectionOffset = dwPEHeaderOffset +
        sizeof(NAVEX_IMAGE_NT_HEADERS) +
        (WENDIAN(lpstPEHeader->FileHeader.NumberOfSections) - 1)
            * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwLastSectionOffset !=
            lpCallBack->FileSeek(hFile, dwLastSectionOffset, SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;

    if (sizeof(NAVEX_IMAGE_SECTION_HEADER) !=
            lpCallBack->FileRead(hFile,
                                 &stLastSection,
                                 sizeof(NAVEX_IMAGE_SECTION_HEADER) ) )
        return EXTSTATUS_FILE_ERROR;


    //EP must be at last section + 0x7000
    if (dwEPOffset != DWENDIAN(stLastSection.PointerToRawData) + 0x7000 )
        return EXTSTATUS_OK;


    //Last section characteristics must be 0xE0000040
    if ( DWENDIAN(stLastSection.Characteristics) != 0xE0000040 )
        return EXTSTATUS_OK;


    //Last section name must be "_Mylene_"
    if ( (stLastSection.Name[0] != '_') ||
         (stLastSection.Name[1] != 'M') ||
         (stLastSection.Name[2] != 'y') ||
         (stLastSection.Name[3] != 'l') ||
         (stLastSection.Name[4] != 'e') ||
         (stLastSection.Name[5] != 'n') ||
         (stLastSection.Name[6] != 'e') ||
         (stLastSection.Name[7] != '_') )
        return EXTSTATUS_OK;


    //now for the verification...


    //get 1k from the virus, starting at +0x600

    dwVirusOffset = DWENDIAN(stLastSection.PointerToRawData) + 0x600;

    if (dwVirusOffset != lpCallBack->FileSeek(hFile, dwVirusOffset, SEEK_SET))
        return EXTSTATUS_FILE_ERROR;

    if (UERROR == lpCallBack->FileRead(hFile, lpbyWorkBuffer, 1024))
        return EXTSTATUS_FILE_ERROR;


    //calculate dwCheckSum and dwCheckXor on this buffer
    for (wColumn=0;wColumn<LIBERTINE_NUM_COLUMNS;wColumn++)
	{
        adwCheckSum[wColumn] = adwCheckXor[wColumn] = 0;

		for (i=0;i<8;i++)
		{
            adwCheckXor[wColumn] ^=
                AVDEREF_DWORD(lpbyWorkBuffer + i*128 + wColumn*4);

			if (i%2)
                adwCheckSum[wColumn] -=
                    AVDEREF_DWORD(lpbyWorkBuffer + i*128 + wColumn*4);
            else
                adwCheckSum[wColumn] +=
                    AVDEREF_DWORD(lpbyWorkBuffer + i*128 + wColumn*4);
        }

        if ( (adwCheckSum[wColumn] == gadwLibertineCheckSum[wColumn]) ||
             (adwCheckXor[wColumn] == gadwLibertineCheckXor[wColumn]) )
             wMatchCount++;

	}//for each column


    //got it!
    if (wMatchCount >= 2)
    {
        *lpwVID = VID_LIBERTINE;
        return EXTSTATUS_VIRUS_FOUND;
    }


    //didn't find it
    return EXTSTATUS_OK;


}//ScanLibertine


WORD gawHPSSig[] = { 0xe8, 0x00, 0x00, 0x00, 0x00, 0x5d, 0x8b, 0xc5 };


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
)
{
    WORD    i, wBytesRead;
    DWORD   dwCurrentPosition = 0;
    DWORD   dwJumpDistance, dwVirusRVA, dwVirusOffset;
    DWORD   dwDecryptDelta; //unused storage for GenericDecrypt


    //read in bytes from entrypoint
    if (dwEPOffset != lpCallBack->FileSeek(hFile,dwEPOffset,SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;


    wBytesRead = lpCallBack->FileRead(hFile,lpbyWorkBuffer,512);
    if (wBytesRead == UERROR)
        return EXTSTATUS_FILE_ERROR;


    //HPS starts with 5 calls backward, each < 256 bytes
    //The first two set up registers, the other three are part of the
    //  decryption loop.

    for (i=0;i<5;i++)
    {
        if ( (lpbyWorkBuffer[i*5+0] != 0xe8) ||
             (lpbyWorkBuffer[i*5+2] != 0xff) ||
             (lpbyWorkBuffer[i*5+3] != 0xff) ||
             (lpbyWorkBuffer[i*5+4] != 0xff) )
             return EXTSTATUS_OK;
    }

    dwCurrentPosition = 25;   //for the 5 calls


    //look for the jump (to beginning of virus) at the end of decryption
    //  (should find it within 30 bytes)

    while ( (dwCurrentPosition < (DWORD)wBytesRead) &&
            (dwCurrentPosition < 55) )
    {
        dwJumpDistance = 0;

        //jz
        if ( (lpbyWorkBuffer[dwCurrentPosition]   == 0x0f) &&
             (lpbyWorkBuffer[dwCurrentPosition+1] == 0x84) )
        {
            dwJumpDistance = AVDEREF_DWORD(lpbyWorkBuffer+dwCurrentPosition+2);

            if ( (dwJumpDistance < (DWORD)-4000) &&
                 (dwJumpDistance > (DWORD)-6000) )
            {
                dwCurrentPosition += 6;
                break;
            }
        }

        //jmp
        else if ( lpbyWorkBuffer[dwCurrentPosition] == 0xe9 )
        {
            dwJumpDistance = AVDEREF_DWORD(lpbyWorkBuffer+dwCurrentPosition+1);

            if ( (dwJumpDistance < (DWORD)-4000) &&
                 (dwJumpDistance > (DWORD)-6000) )
            {
                dwCurrentPosition += 5;
                break;
            }
        }

        dwCurrentPosition ++;

    }//while (looking for the jump)


    //didn't find the jump!
    if (dwJumpDistance == 0)
        return EXTSTATUS_OK;


    //calculate virus offset
    dwVirusRVA =
        DWENDIAN(lpstPEHeader->OptionalHeader.AddressOfEntryPoint) +
        dwCurrentPosition +
        dwJumpDistance;

    if (RVAToFileOffset(lpCallBack,
                        hFile,
                        lpstPEHeader,
                        lpbyWorkBuffer,
                        dwPEHeaderOffset,
                        dwVirusRVA,
                        &dwVirusOffset)
                        != EXTSTATUS_OK)
        return EXTSTATUS_FILE_ERROR;


    //read in 8 bytes from beginning of the virus

    if (dwVirusOffset != lpCallBack->FileSeek(hFile,dwVirusOffset,SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;

    if (8 != lpCallBack->FileRead(hFile,lpbyWorkBuffer,8) )
        return EXTSTATUS_FILE_ERROR;


    //verify gawHPSSig, encrypted using BYTE or DWORD, XOR or ADD

    if ( GenericDecrypt(lpbyWorkBuffer,
                      gawHPSSig,
                      8,
                      DECRYPT_DWORD | DECRYPT_BYTE | DECRYPT_XOR | DECRYPT_ADD,
                      lpbyDecryptFlags,
                      lpdwDecryptKey,
                      &dwDecryptDelta) == EXTSTATUS_OK)
    {
        *lpdwVirusOffset = dwVirusOffset;
        return EXTSTATUS_VIRUS_FOUND;
    }


    //uh oh, didn't find it
    return EXTSTATUS_OK;


}//LocateHPSStart




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
)
{
    WORD    wResult;

    //storage
    DWORD   dwVirusOffset, dwDecryptKey;
    BYTE    byDecryptFlags;


    //check that filesize is divisible by 101 -- written by a 29A member
    if ( lpCallBack->FileSize(hFile) % 101 != 0 )
        return EXTSTATUS_OK;


    //last section must be big enough to hold the virus
    if ( dwSegLength < 5000 )
        return EXTSTATUS_OK;



    wResult = LocateHPSStart(lpCallBack,
                             hFile,
                             lpbyWorkBuffer,
                             lpstPEHeader,
                             dwPEHeaderOffset,
                             dwEPOffset,
                             &dwVirusOffset,
                             &dwDecryptKey,
                             &byDecryptFlags);


    if (wResult == EXTSTATUS_VIRUS_FOUND)
        *lpwVID = VID_HPS;

    return (wResult);

}//ScanHPS



WORD gawParvoSig[] =
{
    0xe8, 0x00, 0x00, 0x00,
    0x00, 0x5d, 0x81, 0xed,
    0x05, 0x30, 0x40, 0x00,
    0x8b, 0xd4, 0xb9, 0xfc,
    0x35, 0x00, 0x00, 0x8d
};

/*
e8 00 00 00 00
5d                          pop ebp
81 ed 05 30 40 00           sub ebp, 403005
8b d4                       mov edx, esp
b9 fc 35 00 00              mov ecx, 35fc
8d a5 25 30 40 00           lea esp, [ebp+403025]
decrypt:
58                          pop eax
c0 c0 04                    rol al, 04
50                          push eax
44                          inc esp
e2 f8                       loop decrypt
*/

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
)
{
    DWORD                       dwLastSectionHeaderOffset;
    NAVEX_IMAGE_SECTION_HEADER  stLastSection;

    WORD    i;
    WORD    wEPBytesRead, wEPSearchLength = 20;
    WORD    wDecryptorBytesRead, wDecryptorSearchLength = 100;
    LPBYTE  lpbyDecryptorBuffer, lpbyVirusBuffer;

    DWORD   dwVirusSecOffset, dwVirusSecRVA;
    DWORD   dwVirusEPOffset, dwVirusEPRVA;
    DWORD   dwVirusOffset, dwVirusRVA;

    DWORD   dwCurrentPosition = 0, dwJumpDistance = 0;
    WORD    bPossibleVirus = 0;

    DWORD   dwDecryptDelta; //unused storage for GenericDecrypt


    //read in last section from section table

    dwLastSectionHeaderOffset =
        dwPEHeaderOffset +
        sizeof(NAVEX_IMAGE_NT_HEADERS) +
        (WENDIAN(lpstPEHeader->FileHeader.NumberOfSections)-1)
            * sizeof(NAVEX_IMAGE_SECTION_HEADER);

    if (dwLastSectionHeaderOffset !=
            lpCallBack->FileSeek(hFile, dwLastSectionHeaderOffset, SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;

    if (sizeof(NAVEX_IMAGE_SECTION_HEADER) !=
            lpCallBack->FileRead(hFile,
                                 &stLastSection,
                                 sizeof(NAVEX_IMAGE_SECTION_HEADER) ) )
        return EXTSTATUS_FILE_ERROR;


    //make sure last section is writeable
    if ( !(DWENDIAN(stLastSection.Characteristics) & NAVEX_IMAGE_SCN_MEM_WRITE) )
        return EXTSTATUS_OK;


    //make sure last section is large enough to hold the virus
    if ( DWENDIAN(stLastSection.Misc.VirtualSize) < 14000 )
        return EXTSTATUS_OK;


    //set up buffers
    lpbyDecryptorBuffer = lpbyWorkBuffer + 256;
    lpbyVirusBuffer     = lpbyWorkBuffer + 256 + 512;


    //read in bytes from entrypoint
    if (dwEPOffset != lpCallBack->FileSeek(hFile,dwEPOffset,SEEK_SET) )
        return EXTSTATUS_FILE_ERROR;

    wEPBytesRead = lpCallBack->FileRead(hFile,lpbyWorkBuffer,256);
    if (wEPBytesRead == UERROR)
        return EXTSTATUS_FILE_ERROR;

    if (wEPSearchLength < wEPBytesRead)
        wEPSearchLength = wEPBytesRead;


    //Parvo patches the host code with a jump to the viral code, which it
    //  appends to the last section.  (like Marburg)



    //search for the jump to the last section
    for (i=0;i<wEPSearchLength;i++)
    {
        //we found a jump
        if (lpbyWorkBuffer[i] == 0xe9)
        {
            //calculate RVA of Virus EP (follow the jump)
            dwVirusEPRVA =
                DWENDIAN(lpstPEHeader->OptionalHeader.AddressOfEntryPoint) +
                i + 5 +
                AVDEREF_DWORD(lpbyWorkBuffer+i+1);

            //check if this RVA is in the last section
            if ( (dwVirusEPRVA >= DWENDIAN(stLastSection.VirtualAddress)) &&
                 (dwVirusEPRVA <  DWENDIAN(stLastSection.VirtualAddress) +
                    DWENDIAN(stLastSection.SizeOfRawData)) )
            {
                //calculate offset of virus EP (beginning of decryptor)

                dwVirusSecOffset = DWENDIAN(stLastSection.PointerToRawData);
                dwVirusSecRVA    = DWENDIAN(stLastSection.VirtualAddress);

                dwVirusEPOffset = dwVirusSecOffset +
                                    (dwVirusEPRVA - dwVirusSecRVA);


                //read in bytes from virus decryptor, into lpbyDecryptorBuffer

                if (dwVirusEPOffset !=
                        lpCallBack->FileSeek(hFile,dwVirusEPOffset,SEEK_SET) )
                    return EXTSTATUS_FILE_ERROR;

                wDecryptorBytesRead =
                    lpCallBack->FileRead(hFile,lpbyDecryptorBuffer,512);
                if (wDecryptorBytesRead == UERROR)
                    return EXTSTATUS_FILE_ERROR;

                if (wDecryptorSearchLength < wDecryptorBytesRead)
                    wDecryptorSearchLength = wDecryptorBytesRead;


                //look for the jump (to beginning of virus)
                //  at the end of decryption

                dwCurrentPosition = 0;
                bPossibleVirus = 0;

                while (dwCurrentPosition < (DWORD)wDecryptorSearchLength)
                {
                    dwJumpDistance = 0;
                    bPossibleVirus = 0;

                    //jz
                    if ( (lpbyDecryptorBuffer[dwCurrentPosition]   == 0x0f) &&
                         (lpbyDecryptorBuffer[dwCurrentPosition+1] == 0x84) )
                    {
                        dwJumpDistance =
                            AVDEREF_DWORD(lpbyDecryptorBuffer+
                                dwCurrentPosition+2);

                        if ( (dwJumpDistance < (DWORD)-0x3600) &&
                             (dwJumpDistance > (DWORD)-0x4000) )
                        {
                            dwCurrentPosition += 6;
                            bPossibleVirus = 1;
                        }
                    }

                    //jmp
                    else if ( lpbyDecryptorBuffer[dwCurrentPosition] == 0xe9 )
                    {
                        dwJumpDistance =
                            AVDEREF_DWORD(lpbyDecryptorBuffer+
                                dwCurrentPosition+1);

                        if ( (dwJumpDistance < (DWORD)-0x3600) &&
                             (dwJumpDistance > (DWORD)-0x4000) )
                        {
                            dwCurrentPosition += 5;
                            bPossibleVirus = 1;
                        }
                    }

                    if (bPossibleVirus)
                    {
                        //calculate virus offset
                        dwVirusRVA =
                            dwVirusEPRVA +
                            dwCurrentPosition +
                            dwJumpDistance;

                        dwVirusOffset = dwVirusSecOffset +
                                            (dwVirusRVA - dwVirusSecRVA);


                        //read in encrypted viral bytes
                        if (dwVirusOffset != lpCallBack->
                                FileSeek(hFile,dwVirusOffset,SEEK_SET) )
                            return EXTSTATUS_FILE_ERROR;

                        if (20 != lpCallBack->
                                    FileRead(hFile,lpbyVirusBuffer,20))
                            continue;


                        //verify gawParvoSig, encrypted using
                        //BYTE or DWORD, XOR or ADD

                        if ( GenericDecrypt(lpbyVirusBuffer,
                                          gawParvoSig,
                                          20,
                                          DECRYPT_DWORD | DECRYPT_BYTE |
                                            DECRYPT_XOR | DECRYPT_ADD,
                                          lpbyDecryptFlags,
                                          lpdwDecryptKey,
                                          &dwDecryptDelta) == EXTSTATUS_OK)
                        {
                            *lpdwVirusOffset = dwVirusOffset;
                            return EXTSTATUS_VIRUS_FOUND;
                        }


                    } //if (bPossibleVirus)

                    dwCurrentPosition ++;

                }//while (looking for the jump to the decrypted virus)

            }//if (the jump lands in the last section)

        }//if (we found a jump)

    }//for (i=0;i<wEPSearchLength;i++)

 
    //uh oh, didn't find it
    return EXTSTATUS_OK;


}//LocateParvoStart


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
)
{
    WORD    wResult;

    //storage for LocateParvoStart
    DWORD   dwVirusOffset, dwDecryptKey;
    BYTE    byDecryptFlags;


    //check that filesize is divisible by 101 -- written by a 29A member
    if ( lpCallBack->FileSize(hFile) % 101 != 0 )
        return EXTSTATUS_OK;


    //go get 'em
    wResult = LocateParvoStart(lpCallBack,
                               hFile,
                               lpbyWorkBuffer,
                               lpstPEHeader,
                               dwPEHeaderOffset,
                               dwEPOffset,
                               &dwVirusOffset,
                               &dwDecryptKey,
                               &byDecryptFlags);


    if (wResult == EXTSTATUS_VIRUS_FOUND)
        *lpwVID = VID_PARVO;

    return (wResult);

}//ScanParvo



