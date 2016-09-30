////////////////////////
//
// PROPRIETARY / CONFIDENTIAL.
// Use of this product is subject to license terms.
// Copyright © 2006 Symantec Corporation.
// All rights reserved.
//
/////////////////////////////////////////////////////////////////////////////////////////////////

//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/o97mod.cpv   1.12   06 Jan 1999 14:23:06   DCHI  $
//
// Description:
//  Core Office 97 VBA module binary access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/o97mod.cpv  $
//
//    Rev 1.15   02 Jan 2001   ELAI
// Fixed O97TypeTableGetRoutineInfo() for Office 9 (97) change to Type Table
//
//    Rev 1.14   14 Feb 2001 16:23:38   ELAI
// Modified O97GetLineBinaryOffsets() to support another argument 
// used in the macrtools project
//
//    Rev 1.13   01 Feb 2001 16:23:38   ELAI
//  - calling of O97GetLineBinaryOffsets() moved to earlier to avoid
//    creating more stealth viruses (no text, but binaries exist)...
//	- banned the use of SSSetStreamLen() because of problems 
//		with the equivalent of "lost clusters" in truncated streams.  
//		SSTruncateStream() is used now.
//
//    Rev 1.12   06 Jan 1999 14:23:06   DCHI
// Fixed O97ModReplaceVBASrc() for case where both orig and new are < 4K.
// 
//    Rev 1.11   08 Dec 1998 12:39:00   DCHI
// Added implementation of O97ModReplaceVBASrc().
// 
//    Rev 1.10   09 Nov 1998 13:41:36   DCHI
// Added #pragma data_seg("FAR_DATA_0") for global FAR data.
// 
//    Rev 1.9   04 Aug 1998 13:46:30   DCHI
// Modified O97GetIdentifierTableOffset() to handle big-endian streams as well.
// 
//    Rev 1.8   23 Jul 1998 12:42:08   DCHI
// Corrected incorrect operand count incrementation for VAR_DEFN in CRCBuf().
// 
//    Rev 1.7   15 Jul 1998 12:00:38   DCHI
// Modified ModAccGetDataPtr() to check offset within range first.
// 
//    Rev 1.6   13 Jul 1998 11:08:40   DCHI
// Added support for Office 9 modules.
// 
//    Rev 1.5   08 Jul 1998 15:58:12   DCHI
// Added sanity checks in CRCBuf() and CRCFast().
// 
//    Rev 1.4   24 Jun 1998 11:00:26   DCHI
// - Added implementations for:
//     O97Get_VBA_PROJECTEndian()
//     O97GetOpSkip()
//     O97CRCBuf()
//     O97ModuleCRCFast()
//     O97ModAccInit()
//     O97ModAccGetLineInfo()
//     O97ModAccGetDataPtr()
// - Modified O97GetIdentifierTableOffset() to correctly endianize.
// - Added opcode operand table gabyO97OpSkip[].
// 
//    Rev 1.3   16 Oct 1997 14:53:14   DCHI
// Added more efficient O97GetLineBinaryOffsets() in sync with MS docs.
// 
//    Rev 1.2   28 Aug 1997 15:08:14   DCHI
// Added PowerPoint identifier table search support.
// 
//    Rev 1.1   11 Aug 1997 15:06:18   DCHI
// Removed unreferenced variables.
// 
//    Rev 1.0   11 Aug 1997 14:33:04   DCHI
// Initial revision.
// 
//************************************************************************

#include "stdafx.h"
#include "o97api.h"
#include "crc32.h"
#include "vba5opc.h"

//********************************************************************
//
// Function:
//  BOOL O97Get_VBA_PROJECTEndian()
//
// Parameters:
//  lpstStream      Ptr to open _VBA_PROJECT stream
//  lpnVersion      Ptr to int for version
//  lpbLitEnd       Ptr to BOOL for endian state
//
// Description:
//  The function reads the first six bytes of the module stream
//  to determine the version and endian state.
//
//  If the byte at offset 2 is 0x6A or less, it is assumed that it
//  is either Office 97 or Office 98 for the Mac.  Otherwise,
//  it is assumed that it is Office 98 for the PC.
//
//  If the byte at offset 4 is 0x01, it is assumed that the stream
//  is in little endian format, otherwise it is assumed to be
//  in big endian format.
//
//  Either of lpnVersion or lpbLitEnd may be NULL if the value
//  is not desired.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97Get_VBA_PROJECTEndian
(
    LPSS_STREAM         lpstStream,
    LPINT               lpnVersion,
    LPBOOL              lpbLitEnd
)
{
    BYTE                abyBuf[6];
    DWORD               dwBytesRead;

    // Read the first six bytes of the stream

    if (SSSeekRead(lpstStream,
                   0,
                   abyBuf,
                   6,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 6)
        return(FALSE);

    // Get the version

    if (lpnVersion != NULL)
    {
        if (abyBuf[2] <= 0x6A)
            *lpnVersion = O97_VERSION_O97;
        else
            *lpnVersion = O97_VERSION_O98;
    }

    if (lpbLitEnd != NULL)
    {
        if (abyBuf[5] == 0x01)
            *lpbLitEnd = TRUE;
        else
            *lpbLitEnd = FALSE;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97GetModuleVersionEndian()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpnVersion      Ptr to int for version
//  lpbLitEnd       Ptr to BOOL for endian state
//
// Description:
//  The function reads the first six bytes of the module stream
//  to determine the version and endian state.
//
//  If the byte at offset 5 is 0xB6, it is assumed that it
//  is either Office 97 or Office 98 for the Mac.  Otherwise,
//  it is assumed that it is Office 98 for the PC.
//
//  If the byte at offset 2 is 0x01, it is assumed that the stream
//  is in little endian format, otherwise it is assumed to be
//  in big endian format.
//
//  Either of lpnVersion or lpbLitEnd may be NULL if the value
//  is not desired.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleVersionEndian
(
    LPSS_STREAM         lpstStream,
    LPINT               lpnVersion,
    LPBOOL              lpbLitEnd
)
{
    BYTE                abyBuf[6];
    DWORD               dwBytesRead;

    // Read the first six bytes of the module stream

    if (SSSeekRead(lpstStream,
                   0,
                   abyBuf,
                   6,
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 6)
        return(FALSE);

    // Get the version

    if (lpnVersion != NULL)
    {
        if (abyBuf[5] == 0xB6)
            *lpnVersion = O97_VERSION_O97;
        else
            *lpnVersion = O97_VERSION_O98;
    }

    if (lpbLitEnd != NULL)
    {
        if (abyBuf[2] == 0x01)
            *lpbLitEnd = TRUE;
        else
            *lpbLitEnd = FALSE;
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetIdentifierTableOffset()
//
// Parameters:
//  lpstStream      Ptr to open _VBA_PROJECT stream
//  lpdwOffset      Ptr to DWORD variable for table offset
//  lpwNumIdents    Ptr to WORD for number of identifiers
//
// Description:
//  The function performs up to two passes to find the beginning
//  of the identifier table.  During the first pass, the function
//  scans starting from offset 0x700 up until the end of the stream.
//  The second passes occurs only if the first pass fails to find
//  the table.  It scans starting from the beginning of the stream
//  and ends at offset 0x700 + X (where X should be more than the 
//  string dependent on dwLimit, currently it should be 12 because
//  we check if (dwBytesToRead < 12) ).
//
//  Each pass searches for the byte sequence 0x80 0x00 0x00 0x00.
//  For each such found sequence, a search is done for either the byte
//  sequence 0x04 0x?? 'W' 'o' 'r' 'd' or
//           0x05 0x?? 'E' 'x' 'c' 'e' 'l' or
//           0x0A 0x?? 'P' 'o' 'w' 'e' 'r' 'P' 'o' 'i' 'n' 't'.
//  The function assumes that finding either of these second sequences
//  signals the beginning of the identifier table as starting at the
//  beginning of the found second sequence.
//
//  Let the 0x80 of the sequence 0x80 0x00 0x00 0x00 be at offset
//  zero for calculating the number of identifiers.  The number of
//  identifiers is the WORD at offset 6 minus the WORD at offset
//  10.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetIdentifierTableOffset
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwNumIdents
)
{
    int             i;
	DWORD           j;
	int             k, l, m;
    DWORD           dwOffset;
    DWORD           dwLimit, dwMaxLimit;
    DWORD           dwBytesToRead;
    DWORD           dwBytesRead;
    BYTE            abyWorkBuffer[512];
    int             nVersion;
    BOOL            bLittleEndian;
    WORD            wIDCount;
    WORD            wSavedIDCount = 0;
    WORD            wKeywordCount = 0;

    if (O97Get_VBA_PROJECTEndian(lpstStream,
                                 &nVersion,
                                 &bLittleEndian) == FALSE)
        return(FALSE);

    // first pass beginning offset to begin search is at 0x700

    dwOffset = 0x700;

    // dwLimit: the limit for searching for the 80 00 00 00 marker
    // dwMaxLimit: the limit for searching for the Word, Excel, and PowerPoint Streams

    dwLimit = dwMaxLimit = SSStreamLen(lpstStream) - 10;

    // two iterations, one for each pass

    for (i=0;i<2;i++)
    {

        // while we are with in the limit for searching for the 80 00 00 00 marker, 
        // we will keep searching

        while (dwOffset < dwLimit)
        {
            // read in the minimum of how much there is to read, and how much buffer space we have

            if (dwLimit - dwOffset < sizeof(abyWorkBuffer))
                dwBytesToRead = dwLimit - dwOffset;
            else
                dwBytesToRead = sizeof(abyWorkBuffer);

            // if we don't have enough bytes to read, we fail
            // 12 because sizeof (80 00 00 00) + sizeof (00 00) + sizeof(WORD) + sizeof(WORD) + sizeof(WORD)

            // GUIDMGR		m_cBuckets (meta-info)			2		80 00
            // GUIDMGR		m_hchunkTable (meta-info)		4		00 00 00 00
            // NAMMGR		number of identifiers in table		2		46 01
            // NAMMGR		number of identifiers to save		2		47 00
            // NAMMGR		number of keyword identifiers		2		ff 00


            if (dwBytesToRead < 12)
                break;

            // read it

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           abyWorkBuffer,
                           dwBytesToRead,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != dwBytesToRead)
                return(FALSE);

            // we will iteratively look for the 80 00 00 00, 
            // note the data structure and stream we are looking for is atleast 11 bytes long
            // sizeof (80 00 00 00) + sizeof (00 00) + sizeof(WORD) + sizeof(WORD) + sizeof(WORD) - 1
            //   - 1 since we do a < compare not a <= compare, is this right???

            dwBytesToRead -= 11;
            for (j=0;j<dwBytesToRead;j++)
            {
                // really ugly way to endianize a word field followed by a word field
                
                // please note that this 80 00 00 00 is a hacked field, it is actually part of 
                // this data structure

                // GUIDMGR		m_cBuckets (meta-info)			2		80 00
                // GUIDMGR		m_hchunkTable (meta-info)		4		00 00 00 00

                // dchi probably originally thought 80 00 00 00 was a dword, when infact it's the word
                // and the 00 00 00 00 is the dword according to microsoft's documentation

                if (((abyWorkBuffer[j] == 0x80 &&
                      abyWorkBuffer[j+1] == 0x00 &&
                      bLittleEndian != FALSE) ||
                     (abyWorkBuffer[j] == 0x00 &&
                      abyWorkBuffer[j+1] == 0x80 &&
                      bLittleEndian == FALSE)) &&
                    abyWorkBuffer[j+2] == 0x00 &&
                    abyWorkBuffer[j+3] == 0x00)
                {
                    // these are the three word's the follow the 80 00 00 00 00 00

                    // NAMMGR		number of identifiers in table		2		46 01
                    // NAMMGR		number of identifiers to save		2		47 00
                    // NAMMGR		number of keyword identifiers		2		ff 00
                    
                    // + 6 because we skipped the last two bytes of m_hchunkTable

                    wIDCount = SSEndianWORD(abyWorkBuffer+j+6,  
                                            bLittleEndian);

                    wSavedIDCount = SSEndianWORD(abyWorkBuffer+j+8,
                                                 bLittleEndian);

                    wKeywordCount = SSEndianWORD(abyWorkBuffer+j+10,
                                                 bLittleEndian);
                }
                else
                {
                    // we couldn't find 80 00 00 00

                    wIDCount = 0;
                }

                // wKeywordCount and wSavedIDCount are unintialized
                // but it doesn't matter since wIDCount will either be 0 
                // have the value that we just read in because we found 80 00 00 00

                // if we have a valid (read in) wIDCount, 
                // and valid looking wIDCount, wKeywordCount, and wSavedIDCount
                // then we will see if we can find our strings

                if (wIDCount > wKeywordCount &&
                    wIDCount - wKeywordCount <= wSavedIDCount)
                {
                    static BYTE abySeq0[] = // Word
                        { 0x57, 0x6F, 0x72, 0x64 };

                    static BYTE abySeq1[] = // Excel
                        { 0x45, 0x78, 0x63, 0x65, 0x6C };

                    static BYTE abySeq2[] = // PowerPoint
                        { 0x50, 0x6F, 0x77, 0x65, 0x72, 0x50, 0x6F, 0x69, 0x6E, 0x74 };

                    int         nLimit;
                    DWORD       dwCandOffset;
                    int         nCandSize;
                    LPBYTE      lpabyCand = NULL;
                    BOOL        bFullFlags;

                    // dchi: Potentially found it, note the number of IDs
                    // dchi:  WORD at offset 6 minus WORD at offset 10

                    // we will remember the number of identifiers we supposedly have 
                    // if this was a valid location for the 80 00 00 00

                    *lpdwNumIdents = (DWORD)(wIDCount - wKeywordCount);

                    // set another offset to point to the beginning of this big "hash table"
                    // offset + j points to the beinning of the 80 00 00 00
                    // + 15 is for:
                    //
                    // sizeof (80 00 00 00) + sizeof (00 00) + sizeof(WORD) + sizeof(WORD) + sizeof(WORD)
                    //   + sizeof (DWORD) - 1
                    //   - 1 since we do a < compare not a <= compare, is this right???

                    // GUIDMGR		m_cBuckets (meta-info)			2		80 00
                    // GUIDMGR		m_hchunkTable (meta-info)		4		00 00 00 00

                    // NAMMGR		number of identifiers in table		2		46 01
                    // NAMMGR		number of identifiers to save		2		47 00
                    // NAMMGR		number of keyword identifiers		2		ff 00

                    // NAMMGR		total size of the hash table		4		a0 2e 00 00

                    dwCandOffset = dwOffset + j + 15;

                    // so we'll keep iterating till we run out of space to look for the identifier.
                    // in actuality we probably ought to look tillt he end of 
                    // NAMMGR		total size of the hash table		4		a0 2e 00 00

                    while (dwCandOffset < dwMaxLimit)
                    {
                        // read in the minimum of what we want to read and our buffer size

                        if (dwMaxLimit - dwCandOffset < sizeof(abyWorkBuffer))
                            dwBytesToRead = dwMaxLimit - dwCandOffset;
                        else
                            dwBytesToRead = sizeof(abyWorkBuffer);

                        // 32  I guess this is the minimum size uh...
                        // this number doesn't really seem to have a significance, but 
                        // if it's too small, dereferencing sizeof(word) + "powerpoint" might have problems

                        if (dwBytesToRead < 32)
                            break;

                        if (SSSeekRead(lpstStream,
                                       dwCandOffset,
                                       abyWorkBuffer,
                                       dwBytesToRead,
                                       &dwBytesRead) != SS_STATUS_OK ||
                            dwBytesRead != dwBytesToRead)
                            return(FALSE);

                        // dchi: Make sure we also have the type byte

                        // - 2 because sizeof(WORD)
                        // lenVal = read(2); // Read the 'length/flags' value 

                        nLimit = (int)dwBytesToRead - 2;

                        nCandSize = 0;

                        bFullFlags = FALSE;

                        for (k=0;k<nLimit;k++)
                        {
                            // endianize the lengh/Flags value

                            // lenVal = read(2); // Read the 'length/flags' value 

                            // these are the flags
                          
                            // #define NAMMGR_IDF_UDT          0x02
                            // #define NAMMGR_IDF_CASE_FIXED   0x04
                            // #define NAMMGR_IDF_NONPARAM_DEF 0x08
                            // #define NAMMGR_IDF_AMBIGUOUS    0x10
                            // #define NAMMGR_IDF_GLOBAL       0x20
                            // #define NAMMGR_IDF_MULTIPLE     0x40
                            // #define NAMMGR_IDF_OVERFLOW     0x80

                            if (bLittleEndian)
                            {
                                // ignore k+1 byte because it's a bunch of flags

                                if (abyWorkBuffer[k] == sizeof(abySeq0) ||
                                    abyWorkBuffer[k] == sizeof(abySeq1) ||
                                    abyWorkBuffer[k] == sizeof(abySeq2))
                                {
                                    // store the size of the string if it's size is equal to any of
                                    // the strings we've found

                                    nCandSize = abyWorkBuffer[k];

                                    // bFullFlags means that the overflow flag was set

                                    if (abyWorkBuffer[k+1] & 0x80)
                                        bFullFlags = TRUE;
                                }
                            }
                            else
                            {
                                // ignore k byte because it's a bunch of flags

                                if (abyWorkBuffer[k+1] == sizeof(abySeq0) ||
                                    abyWorkBuffer[k+1] == sizeof(abySeq1) ||
                                    abyWorkBuffer[k+1] == sizeof(abySeq2))
                                {
                                    // store the size of the string if it's size is equal to any of
                                    // the strings we've found

                                    nCandSize = abyWorkBuffer[k+1];

                                    // bFullFlags means that the overflow flag was set

                                    if (abyWorkBuffer[k] & 0x80)
                                        bFullFlags = TRUE;
                                }
                            }

                            // if the string size is 0, then this is a keyword identifier

                            if (nCandSize != 0)
                            {
                                // this is a non-keyword identifier, 
                                // so let's check this string

                                if (bFullFlags)
                                {
                                    // + 6 is = + sizeof(WORD) + sizeof(WORD) + sizeof(WORD)
                                    //
                                    // read(2); // Read the full flags value
                                    // read(2); // Read idType
                                    // read(2); // Read idIlibHint

                                    // if the string cannot fit in the space alloted, then break out of
                                    // checking this identifier
                                    //
                                    // this means that we need to read in more of the identifier to make a 
                                    // determination

                                    if (k + 6 + nCandSize > nLimit)
                                        break;

                                    // m is 8 because 
                                    // 8 = + sizeof(WORD) + sizeof(WORD) + sizeof(WORD) + sizeof(WORD)
                                    //
                                    // lenVal = read(2); // Read the 'length/flags' value 
                                    // read(2); // Read the full flags value
                                    // read(2); // Read idType
                                    // read(2); // Read idIlibHint

                                    m = 8;
                                }
                                else
                                {
                                    // if the string cannot fit in the space alloted, then break out of
                                    // checking this identifier
                                    //
                                    // this means that we need to read in more of the identifier to make a 
                                    // determination

                                    if (k + nCandSize > nLimit)
                                        break;

                                    // m is 2 because sizeof(WORD)
                                    // lenVal = read(2); // Read the 'length/flags' value 

                                    m = 2;
                                }

                                // check the lenght to determine which string to check against.
                                // we're just lucky these microsoft products use different length strings :)

                                if (nCandSize == sizeof(abySeq0))
                                    lpabyCand = abySeq0;
                                else
                                if (nCandSize == sizeof(abySeq1))
                                    lpabyCand = abySeq1;
                                else
                                if (nCandSize == sizeof(abySeq2))
                                    lpabyCand = abySeq2;

                                // if none of the lengths matches lpabyCand is uninitialized, 
                                // but we know this won't happen because
                                // we earlier checked if abyWorkBuffer[k(+1)] == sizeof(abySeq0)

                                // so we add another case to reset nCandSize to 0 so we 
                                // don't try to do the comparison and potentially blue screen!
                                // setting nCandSize to 0 should be fine because do it later on 
                                // in "Reset Flags"

                                // it would be smart to add this case
                                // else
                                //     nCandSize = 0;


                                // check if the string matches

                                for (l=0;l<nCandSize;l++)
                                    if (abyWorkBuffer[k+l+m] != lpabyCand[l])
                                        break;  
                                        // note: this only breaks from this little 
                                        //       "check if the string mathes" for loop



                                // if our l is the same as the size we were looking for
                                // we matched the string

                                if (l == nCandSize)
                                {
                                    // Found it
                                    
                                    // setting the offset to this identifier offset

                                    *lpdwOffset = dwCandOffset + k;
                                    return(TRUE);
                                }

                                // Reset flags
                                // we're going to try to find the string again
                                // this time at a +1 offset

                                nCandSize = 0;
                                bFullFlags = FALSE;
                            }
                            
                            // instead of incrementing by 1 here, we ought parse correctly
                            // for the next possible location

                        } // for (k=0;k<nLimit;k++)

                        // instead of adding k, we ought to add k - 32 
                        // (with some special end condition), since 
                        // that is how much we are willing to read in the next time around. 

                        // otherwise if powerpoint, or whatever string we're looking for, will 
                        // be missed if it too falls on a boundary.

                        dwCandOffset += k;
                    }

                    // since we invalidated the dwBytesToRead in our attempt to find the word, excel, or powerpoint
                    // identifier, we need to force ourselves to find another 80 00 00 00 value
                    
                    // this way, when we iterate again, j == dwBytesToRead
                    // we'll get out of the for loop, and increment our offset by j bytes 
                    // thus we will begin the next search at the byte after the 80 byte

                    dwBytesToRead = j + 1;
                }
            }

            // begin the next search for 80 00 00 00 

            dwOffset += dwBytesToRead;
        }

        // second pass beginning offset to begin search is 0

        dwOffset = 0;

        // we want to limit the number of bytes search for a 80 00 00 00, otherwise we'll 
        // do the same work as the second pass.  However, if we use 0x700 only as our new
        // dwLimit, we'll miss any 80 00 00 00 which lies on the 0x700 boundary

        // + 12 because that's the minimum needed for searching for 80 00 00 00, 
        // + 4 is probably enough though because I think the failure only happens if 80 00 00 00 
        // comes at exactly the boundry

        if (dwLimit >= 0x700 + 12)
            dwLimit = 0x700 + 12;

        // (seemingly arbitrary) limit so we don't search forever for the identifier strings
        // the biggest ID table I saw was 0x910F bytes long

        if (dwMaxLimit >= 0x700 + 0x400)
            dwMaxLimit = 0x700 + 0x400;
    }

    return(FALSE);
}


//********************************************************************
//
// elai _VBA_PROJECT NOTES
// 
// useful parsing info from microsoft
//
// *************************************************************************************
// ** if you ever want to parse _VBA_PROJECT correctly in O97GetIdentifierTableOffset()
// ** here's the file format
// *************************************************************************************
//
// Project Data (_VBA_PROJECT) Stream Layout
//
// The _VBA_PROJECT stream contains project information that is platform and version specific. In this stream definition, the following string types are used:
//
// WriteString: Ansi string with two byte length followed without trailing NULL. Empty string is represented as 0xFFFF, arbitrarily. This function does not exist for Office9, and is fully replaced by WriteStringAsUnicode
//
// WriteSz: Unicode string with two byte length followed with Unicode string without trailing NULL. Cannot be empty.
// 
// WriteBstr: Ansi string with four byte length followed with Ansi string without trailing NULL. Empty string as represented as: 0x00000000 0x0001 (four bytes of zeros to mean zero length string, followed by a two byte Boolean TRUE)
// 
// WriteStringAsUnicode: Unicode string with two byte length followed with Unicode string without trailing NULL. Empty string is represented by 0xFFFF, arbitrarily.
// 
// 
// Object   Description     Size in bytes
// 
// GEN_PROJECT  identification bytes   2 (Intel Format) cc 61
// GEN_PROJECT  version number    2 (Intel Format) 6d 00
// GEN_PROJECT  wizard flag     1   00
// GEN_PROJECT  system type     2 (Intel Format) 00 01
// GEN_PROJECT  project visible flag   1   00
// GEN_PROJECT  local code id     4   ff 09 04 00
// GEN_PROJECT  local code id    4   
// GEN_PROJECT  project code page    2
// GEN_PROJECT  system type     2
// GEN_PROJECT  compatible exe flag   2
// GEN_PROJECT  show edit dialog flag   2
// GEN_PROJECT  exe name     WriteBstr
// GEN_PROJECT  lib’s ref’d lib count   2
// GEN_PROJECT  lib’s default ref’d lib count  2
// 
//GEN_PROJECT  lib’s reflib list  (below block * lib count)
//  
//  absolute libid of the reference   WriteSz
// 
//  //the following string will ONLY be present if
//  //the above string refers to a non-registered
//  //libid reference, e.g. the string begins with something
//  //other than “*\G”. So parsing code should check the
//  //absolute reference above and look for this
//  //relative reference IFF it is != “*\G”, 
//  //else skip to the next field.
//       relative libid of this reflib         WriteSz
// 
//  major id last saved     4
//  minor id last saved     2
//  new version number     4  
//  extended typelib marker—if any (1)   2
//   absolute libid of the reference  WriteSz
//  
//   //the following string will ONLY be present if
//   //the above string refers to a non-registered
//   //libid reference, e.g. the string begins with something
//   //other than “*\G”. So parsing code should check the
//   //absolute reference above and look for this
//   //relative reference IFF it is != “*\G”, 
//   //else skip to the next field.
// relative libid of this reflib   WriteSz
//   
// major id last saved    4
//   minor id last saved    2
//   new version number    4
//   cookie      4
//   GUID raw typelib     16
//  end of extended typelib marker (0)    2
// 
// GEN_PROJECT  count of OB_TYPE_ENTRYs   2
// GEN_PROJECT  ob type entry flag   2 * (count) 
// 
// GEN_PROJECT  count of compilation constants 2
// GEN_PROJECT  m_rghlnamCCConst[N]   2 * (count)
// GEN_PROJECT  m_rgCCConstValue [N]   2 * (count)
// 
// GEN_PROJECT  hlnam of the typelib   2
// #if Office9
// GEN_PROJECT  default TYPEINFO TYPEID    WriteStringAsUnicode
// #else
// GEN_PROJECT  default TYPEINFO TYPEID   WriteString
// #endif
// GEN_PROJECT  documentation string    WriteStringAsUnicode
// #if Office9
// GEN_PROJECT  helpfile name     WriteStringAsUnicode
// #else
// GEN_PROJECT  helpfile name    WriteString
// #endif
// GEN_PROJECT  help context    4  00 00 00 00
// GEN_PROJECT  invalid binding tables flag  2  FF FF
// GEN_PROJECT  lib flags     2  00 00
// GEN_PROJECT  project version number—major  4  26 20 aa 3c
// GEN_PROJECT  project version number—minor  2  40 00
// GEN_PROJECT  hash table     64  FF ...
// GEN_PROJECT  major version number   2  01 00
// GEN_PROJECT  minor version number   2  00 00
// GEN_PROJECT  GUID      16  00 ...
// GEN_PROJECT  project specific magic cookie  2  09 AB
//
// GEN_PROJECT  count of type entries   2  01 00
// 
// GEN_PROJECT  type entries:  (below block * count) 
// 
// type entry string         WriteStringAsUnicode ThisDocument
// #if Office9
// time stamp     WriteStringAsUnicode 013caa2026
// type info type id    WriteStringAsUnicode    FF FF
// #else 
// time stamp length    2
// time stamp     Above Length 
// type info type id    WriteString
// #endif
// usHlnamIsClass    2    13 02
// #if OFFICE9
// module name     WriteStringAsUnicode ThisDocument
// #endif
// doc string     WriteStringAsUnicode    FF FF
// magic cookie type    2    0B AB
// help context    4    00 00 00 00
// 
// count of REFMODS    2    00 00 
// REFMOD array    count * sizeof(REFMOD)  
// 
// m_hguidMod     4    00 02 00 00
// m_bFlags     1    00
// text offset in module’s IDE stream 4    0d 0e 00 00
// m_hteNext     2    FF FF 
// 
// GUIDMGR
// 
// BLKMGR32 list to head    4  FF FF FF FF
// BLKMGR32 coalescse freelist state  1  01
// BLKMGR32 roundup flag    1  01
// 
// BLKDESC32 block size     4  20 02 00 00
// BLKDESC32 contents of above block   (block size)
// 
// (see section: GUID hash table)
//   
// GUIDMGR  m_cBuckets (meta-info)   2  80 00
// GUIDMGR  m_hchunkTable (meta-info)  4  00 00 00 00
// 
// NAMMGR (name manager hash table)
// 
// NAMMGR  number of identifiers in table 2  46 01
// NAMMGR  number of identifiers to save  2  47 00
// NAMMGR  number of keyword identifiers  2  ff 00
// NAMMGR  total size of the hash table  4  a0 2e 00 00
// 
// NAMMGR  (… Hash Data …)
// 
// (see section: Pseudo code for reading the NAMMGR hash table)
// 
// GENPROJ_TYPEBIND compile state    1
// 
// GENPROJ_BINDNAME_TABLE
// 
// BLK_MGR list to head    2
// BLK_MGR coalescse freelist state  1
// BLK_MGR roundup flag    1
// 
// BLK_DESC block size     4
// BLK_DESC contents of above block   (block size)
// 
// (see section: Project level binding table)
// 
// GENPROJ_BINDNAME_TABLE m_indexFirstGlobal  2
// GENPROJ_BINDNAME_TABLE m_cBuckets    2
// GENPROJ_BINDNAME_TABLE m_hchunkBucketTbl   2
// 
// GEN_PROJECT  hasValidDiskImageNameCache  2
// BLK_DESC  block size     4
// BLK_DESC  contents of above block   (block size)
//
// Pseudo code for reading the NAMMGR hash table
//
//
//
// // Pseudo code to parse the name manager hash table.
// //
// // Where: read(x) indicates a stream read of x bytes,
// //
// // And the following flags are defined:
// //
// 
//  #define NAMMGR_MAX_LENGTH 0x01FF  // Maximum identifier name length
// 
//   #define NAMMGR_IDF_UDT          0x0200
//  #define NAMMGR_IDF_CASE_FIXED   0x0400
//   #define NAMMGR_IDF_NONPARAM_DEF 0x0800
//   #define NAMMGR_IDF_AMBIGUOUS    0x1000
//   #define NAMMGR_IDF_GLOBAL       0x2000
//   #define NAMMGR_IDF_MULTIPLE     0x4000
//   #define NAMMGR_IDF_OVERFLOW     0x8000
//   
// ReadHashData()
// {
//   read(2); // Count of identifiers in the hash table 
//   
//  identCount = read(2); // Count of identifiers that were actually saved 
// 
//   read(2); // Count of keyword identifiers 
//
//   read(4); // Size of the hash table in memory
// 
//   do
//  {
//     lenVal = read(2); // Read the 'length/flags' value 
// 
//     if (lenVal == 0) // Zero indicates a keyword entry 
//    {
//       read(2); // Read the name index 
// 
//       lenVal = read(2); // Read the 'length/flags' value again 
// 
//       isKwd = TRUE;
//    }
// 
//     idLen = lenVal & NAMMGR_MAX_LENGTH; // Get the identifier length
// 
//     if (lenVal & NAMMGR_IDF_OVERFLOW) // Check for flags overflow
//     {
//       read(2); // Read the full flags value
// 
//       read(2); // Read idType
//    
//       read(2); // Read idIlibHint
//     }
// 
//     readn(idLen); // Read the identifier string
//
//     if (isKwd == FALSE)
//       read(4); // Read the hash value of the identifier 
// 
//   } while (--identCount);
// 
// }    
//
//********************************************************************
   
//********************************************************************
//
// BOOL O97GetTypeTableOffset()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpdwOffset      Ptr to DWORD variable for table offset
//  lpdwSize        Ptr to DWORD variable for size of table
//
// Description:
//  For Office 97, here is how to parse to the type table offset:
//
//      abyUnknown0[0x0B]
//      dwSize1
//      abyUnknown1[dwSize1 + 0x40]
//      wSize2
//      abyUnknown2[wSize2 * 0x10]
//      dwSize3
//      abyUnknown3[dwSize3 + 6]
//      dwSize4
//      abyUnknown4[dwSize4 + 8]
//      dwPreTypeTableOffset
//
//  For Office 2000, the value dwPreTypeTableOffset is at an
//  absolute offset of 0x11 from the top of the stream.
//
//  At dwPreTypeTableOffset + 10 is the DWORD that specifies
//  the size of the type table.  The table data immediately
//  follows the size DWORD.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetTypeTableOffset
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize
)
{
    DWORD               dwOffset;
    DWORD               dwBytesRead;
    BYTE                abyWorkBuffer[512];
    int                 nVersion;
    BOOL                bLittleEndian;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLittleEndian) == FALSE)
        return(FALSE);

    if (nVersion == O97_VERSION_O97)
    {
        // Assume Office 97/Mac O98

        // abyUnknown0[0x0B]

        dwOffset = 0x0B;

        // dwSize1

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyWorkBuffer,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwOffset += sizeof(DWORD);

        // abyUnknown1[dwSize1 + 0x40]

        dwOffset += SSEndianDWORD(abyWorkBuffer,bLittleEndian) + 0x40;

        // wSize2

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyWorkBuffer,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        dwOffset += sizeof(WORD);

        // abyUnknown2[wSize2 * 0x10]

        dwOffset += SSEndianWORD(abyWorkBuffer,bLittleEndian) * 0x10;

        // dwSize3

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyWorkBuffer,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwOffset += sizeof(DWORD);

        // abyUnknown3[dwSize3 + 6]

        dwOffset += SSEndianDWORD(abyWorkBuffer,bLittleEndian) + 6;

        // dwSize4

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyWorkBuffer,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwOffset += sizeof(DWORD);

        // abyUnknown4[dwSize4 + 8]

        dwOffset += SSEndianDWORD(abyWorkBuffer,bLittleEndian) + 8;

        // dwPreTypeTableOffset

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyWorkBuffer,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwOffset = SSEndianDWORD(abyWorkBuffer,bLittleEndian);
    }
    else
    {
        // Assume Office 9

        // Get the offset of the type table

        if (SSSeekRead(lpstStream,
                       0x11,
                       (LPBYTE)&dwOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwOffset = DWENDIAN(dwOffset);
    }

    // The size is located at the DWORD + 10

    dwOffset += 10;

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   lpdwSize,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    *lpdwSize = SSEndianDWORD((LPBYTE)lpdwSize,bLittleEndian);
    *lpdwOffset = dwOffset + 4;
    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetLineBinaryOffsets()
//
// Parameters:
//  lpstStream          Ptr to open module stream
//  lpdwLineTableOffset Ptr to DWORD variable for line table offset
//  lpdwNumLines        Ptr to DWORD for number of lines
//  lpdwBinaryOffset    Ptr to DWORD variable for binary offset
//  lpbLittleEndian     Ptr to boolean for endian state
//
// Description:
//  The function assumes the file is in the following format:
//      abyUnknown0[11]
//      dwExtra0
//      abyExtra0[dwExtra0]
//      abyUnknown1[64]
//      wExtra1
//      abyExtra1[wExtra1 * 16]
//      dwExtra2
//      abyExtra2[dwExtra2]
//      abyUnknown2[6]
//      dwExtra3
//      abyExtra3[dwExtra3]
//      //abyUnknown3[77]
//      abyUnknown3[32]
//      dwFlags
//      dwUnknown4
//      if (dwFlags & 0x40)
//      {
//          wControlCount
//          {
//              wDescSize
//              abyDesc[wDescSize]
//          }[wControlCount]
//      }
//      abyUnknown4[35]
//      wCCConstMax
//      if (wCCConstMax != 0)
//          error
//      dwTypeSrcOffset
//
//  At dwTypeSrcOffset + 60:
//      FE CA ?? ??
//      wLineRecordCount
//      abyLineRecords[wLineRecordCount][12]
//
//  The binary offset is 10 bytes after the last line entry.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetLineBinaryOffsets
(
    LPSS_STREAM         lpstStream,
    LPDWORD             lpdwLineTableOffset,
    LPDWORD             lpdwNumLines,
    LPDWORD             lpdwBinaryOffset,
    LPBOOL              lpbPossiblyStealthVirus
)
{
    DWORD               dwOffset;
    DWORD               dwTemp;
    WORD                wTemp;
    BYTE                abyBuf[6];
    DWORD               dwBytesRead;
    WORD                wNumLines;
    int                 nVersion;
    BOOL                bLittleEndian;
    DWORD               dwPrevOffset = 0;

    // initialize return value

    if (lpbPossiblyStealthVirus != NULL)
        *lpbPossiblyStealthVirus = FALSE;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLittleEndian) == FALSE)
        return(FALSE);

    if (nVersion == O97_VERSION_O97)
    {
        // Assume Office 97/Mac O98

        dwOffset = 11;

        // dwExtra0

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Skip over dwExtra0, abyExtra0[dwExtra0], and abyUnknown1[64]

        dwOffset += sizeof(DWORD) + dwTemp + 64;

        // wExtra1

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        wTemp = SSEndianWORD((LPBYTE)&wTemp,bLittleEndian);

        // Skip over wExtra1 and abyExtra1[wExtra1 * 16]

        dwOffset += sizeof(WORD) + wTemp * (DWORD)16;

        // dwExtra2

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Skip over dwExtra2, abyExtra2[dwExtra2], and abyUnknown2[6]

        dwOffset += sizeof(DWORD) + dwTemp + 6;

        // dwExtra3

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Skip over dwExtra3, abyExtra3[dwExtra3], and abyUnknown3[32]

        dwOffset += sizeof(DWORD) + dwTemp + 32;

        // Get dwFlags

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Skip over dwFlags and dwUnknown4

        dwOffset += sizeof(DWORD) + sizeof(DWORD);

        if (dwTemp & 0x40)
        {
            WORD        wCount;

            // Get wControlCount

            if (SSSeekRead(lpstStream,
                           dwOffset,
                           &wCount,
                           sizeof(WORD),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(WORD))
                return(FALSE);

            dwOffset += sizeof(WORD);

            wCount = SSEndianWORD((LPBYTE)&wCount,bLittleEndian);
            while (wCount-- != 0)
            {
                // Get wDescSize

                if (SSSeekRead(lpstStream,
                               dwOffset,
                               &wTemp,
                               sizeof(WORD),
                               &dwBytesRead) != SS_STATUS_OK ||
                    dwBytesRead != sizeof(WORD))
                    return(FALSE);

                wTemp = SSEndianWORD((LPBYTE)&wTemp,bLittleEndian);

                // Skip over wDescSize and abyDesc[wDescSize]

                dwOffset += sizeof(WORD) + wTemp;
            }
        }

        // Skip abyUnknown4[35]

        dwOffset += 35;

        // Read wCCConstMax

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &wTemp,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(WORD))
            return(FALSE);

        dwOffset += sizeof(WORD);

        dwPrevOffset = dwOffset;

        wTemp = SSEndianWORD((LPBYTE)&wTemp,bLittleEndian);
        if (wTemp != 0)
        {
            // Don't know how to handle this

            return(FALSE);
        }

        // Get dwTypeSrcOffset
    }
    else
    {
        // Assume Office 9

        // Get the offset of the line table

        dwOffset = 0x19;
    }

    
    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &dwTemp,
                   sizeof(DWORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(DWORD))
        return(FALSE);

    dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

    // Read FE CA

    dwOffset = dwTemp + 60;

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   abyBuf,
                   2 * sizeof(BYTE),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != 2 * sizeof(BYTE))
    {

        if (lpbPossiblyStealthVirus != NULL)
            *lpbPossiblyStealthVirus = TRUE;

        // since there was an error...
        // this might be a word 2001 mac file.
        
        // because microsoft uses a different compiler for the macintosh
        // that drops two unnecessary bytes

        // 00 00 FF FF 00 00 00 00 XX XX    not word 2001 mac
        // FF FF 00 00 00 00 XX XX 00 00        word 2001 mac
        // so let's check if we're in the word 2001 mac situation.
        // where we accidentally read in XX XX 00 00 as our offset

        // go back to where we think the FF FF is
        // ambrose from microsoft says
        // where the FFFF is, is BASIC_TYPEROOT	m_usCCConst					2
        // where the 0000 is, is BASIC_TYPEROOT	m_usCCConstMax				2
        // though in general we probably can't count on 
        // either being definitely one of those values
        // because this is the file format for bytes following these two constants
        // however, right now we're assuming m_usCCConstMax == 0
        //
        // if (m_usCCConstMax > 0) {
        //       for (us = 0; us < (m_usCCConst + 1); us++) { 
        //	 BASIC_TYPEROOT	USHORT us				2
        //	 BASIC_TYPEROOT	VARIANT(See SkipVariant Code)	Variant
        //       } // for
        // }

        // go back and read m_usCCConst and m_usCCConstMax

        dwOffset = dwPrevOffset;

        dwOffset -= 3 * sizeof(WORD);

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyBuf,
                       2 * sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != 2 * sizeof(WORD))
            return(FALSE);

        // verify FFFF 0000

        wTemp = SSEndianWORD(&abyBuf[0],bLittleEndian);

        if (wTemp != 0xFFFF)
            return (FALSE);

        wTemp = SSEndianWORD(&abyBuf[2],bLittleEndian);

        if (wTemp != 0x0000)
            return (FALSE);

        // we've verified our "bookmarks" so now
        // we'll re-try finding the FE CA value
        // using the correct offset

        // skip over FFFF 0000

        dwOffset += 2 * sizeof(WORD);

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       &dwTemp,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != sizeof(DWORD))
            return(FALSE);

        dwTemp = SSEndianDWORD((LPBYTE)&dwTemp,bLittleEndian);

        // Read FE CA

        dwOffset = dwTemp + 60;

        if (SSSeekRead(lpstStream,
                       dwOffset,
                       abyBuf,
                       2 * sizeof(BYTE),
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != 2 * sizeof(BYTE))
            return (FALSE);
    }


    if (!((abyBuf[0] == 0xFE && abyBuf[1] == 0xCA) ||
        (abyBuf[0] == 0xCA && abyBuf[1] == 0xFE)))
        return(FALSE);

    // Skip over FE CA ?? ??

    dwOffset += 4;

    // Get the number of lines

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   &wNumLines,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(WORD))
        return(FALSE);

    // Skip over wLineRecordCount

    dwOffset += sizeof(WORD);

    *lpdwLineTableOffset = dwOffset;
    wNumLines = SSEndianWORD((LPBYTE)&wNumLines,bLittleEndian);
    *lpdwNumLines = wNumLines;
    *lpdwBinaryOffset = dwOffset + 10 +
        wNumLines * (DWORD)sizeof(O97_LINE_TABLE_LINE_T);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97CreateModuleAccessStruct()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lplpstModule    Ptr to var to store ptr to created module
//                  access structure
//
// Description:
//  Allocates memory for a module access structure.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97CreateModuleAccessStruct
(
    LPVOID              lpvRootCookie,
    LPLPO97_MODULE      lplpstModule
)
{
    LPO97_MODULE        lpstModule;

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(O97_MODULE_T),
                      (LPLPVOID)&lpstModule) != SS_STATUS_OK)
        return(FALSE);

    lpstModule->stLineTable.dwNumBlocks = O97_LINE_TABLE_NUM_BLOCKS;
    lpstModule->stLineTable.dwBlockSize = O97_LINE_TABLE_BLOCK_SIZE;

    lpstModule->stBinary.dwNumBlocks = O97_MOD_BIN_NUM_BLOCKS;
    lpstModule->stBinary.dwBlockSize = O97_MOD_BIN_BLOCK_SIZE;

    *lplpstModule = lpstModule;

    return(TRUE);
}


//********************************************************************
//
// BOOL O97DestroyModuleAccessStruct()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpstModule      Ptr to created module access structure
//
// Description:
//  Deallocates memory allocated for a module access structure.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97DestroyModuleAccessStruct
(
    LPVOID              lpvRootCookie,
    LPO97_MODULE        lpstModule
)
{
    if (SSMemoryFree(lpvRootCookie,
                     lpstModule) != SS_STATUS_OK)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97InitModuleAccess()
//
// Parameters:
//  lpstStream      Ptr to open module stream
//  lpstModule      Ptr to created module access structure
//
// Description:
//  Initializes the fields of the structure for module access.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97InitModuleAccess
(
    LPSS_STREAM         lpstStream,
    LPO97_MODULE        lpstModule
)
{
    DWORD                 i;

    lpstModule->lpstStream = lpstStream;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &lpstModule->nVersion,
                                  &lpstModule->bLittleEndian) == FALSE)
        return(FALSE);

    // Initialize type table info

    if (O97GetTypeTableOffset(lpstStream,
                              &lpstModule->stTypeTable.dwOffset,
                              &lpstModule->stTypeTable.dwSize) == FALSE)
        return(FALSE);

    // Initialize line table cache

    for (i=0;i<lpstModule->stLineTable.dwNumBlocks;i++)
        lpstModule->stLineTable.adwBlockNum[i] = 0xFFFFFFFF;

    // Initialize binary cache

    for (i=0;i<lpstModule->stBinary.dwNumBlocks;i++)
        lpstModule->stBinary.adwBlockNum[i] = 0xFFFFFFFF;

    if (O97GetLineBinaryOffsets(lpstStream,
                                &lpstModule->stLineTable.dwOffset,
                                &lpstModule->stLineTable.dwNumLines,
                                &lpstModule->stBinary.dwOffset,
                                NULL) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetLineEntry()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwLine          Line entry to get
//  lpstEntry       Ptr to line entry structure to fill
//
// Description:
//  Fills a line entry structure with the information from the
//  given line.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetLineEntry
(
    LPO97_MODULE        lpstModule,
    DWORD               dwLine,
    LPO97_LINE_ENTRY    lpstEntry
)
{
    DWORD               dwBlockNum;
    DWORD               dwCacheLineNum;

    if (dwLine >= lpstModule->stLineTable.dwNumLines)
        return(FALSE);

    dwBlockNum = dwLine & O97_LINE_TABLE_BLOCK_NUM_MASK;
    dwCacheLineNum = (dwLine & O97_LINE_TABLE_CLN_MASK) >>
        O97_LINE_TABLE_CLN_SHIFT;

    // First check to see if the block containing the wanted line
    //  is in the cache.  If not, then the block must be loaded.

    if (lpstModule->stLineTable.adwBlockNum[dwCacheLineNum] !=
        dwBlockNum)
    {
        O97_LINE_TABLE_LINE_T   stLine;
        DWORD                   dwCurLine;
        DWORD                   dwLimitLine;
        DWORD                   dwOffset;
        DWORD                   dwBytesRead;
        LPO97_LINE_ENTRY        lpstCurEntry;

        // Determine the number of lines to read

        dwLimitLine = dwBlockNum + O97_LINE_TABLE_BLOCK_SIZE;
        if (dwLimitLine > lpstModule->stLineTable.dwNumLines)
            dwLimitLine = lpstModule->stLineTable.dwNumLines;

        // Load the block

        dwCurLine = dwBlockNum;
        dwOffset = lpstModule->stLineTable.dwOffset +
            dwCurLine * sizeof(O97_LINE_TABLE_LINE_T);
        lpstCurEntry = lpstModule->stLineTable.abyBlocks[dwCacheLineNum];

        while (dwCurLine < dwLimitLine)
        {
            // Read the line entry

            if (SSSeekRead(lpstModule->lpstStream,
                           dwOffset,
                           &stLine,
                           sizeof(O97_LINE_TABLE_LINE_T),
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != sizeof(O97_LINE_TABLE_LINE_T))
                return(FALSE);

            // Set the flags

            lpstCurEntry->wType = 0;
            if (lpstModule->bLittleEndian == FALSE)
            {
                BYTE    byTemp;

                // Swap the first and last bytes

                byTemp = stLine.abyFlags[0];
                stLine.abyFlags[0] = stLine.abyFlags[3];
                stLine.abyFlags[3] = byTemp;

                // Swap the second and third bytes

                byTemp = stLine.abyFlags[1];
                stLine.abyFlags[1] = stLine.abyFlags[2];
                stLine.abyFlags[2] = byTemp;
            }

            if ((stLine.abyFlags[0] & 0x42) == 0x42)
                lpstCurEntry->wType |= O97_LINE_FLAG_FUNCTION;

            if ((stLine.abyFlags[0] & 0x22) == 0x22)
                lpstCurEntry->wType |= O97_LINE_FLAG_SUB;

            if ((stLine.abyFlags[0] & 0x04) == 0x04)
                lpstCurEntry->wType |= O97_LINE_FLAG_END_FUNC_SUB;

            if ((stLine.abyFlags[1] & 0x40) == 0x40)
                lpstCurEntry->wType |= O97_LINE_FLAG_CONST;

            if ((stLine.abyFlags[1] & 0x81) == 0x81)
                lpstCurEntry->wType |= O97_LINE_FLAG_STATEMENT;
            else
            if ((stLine.abyFlags[1] & 0x81) == 0x80)
            {
                if ((stLine.abyFlags[2] & 0x09) == 0x09)
                    lpstCurEntry->wType |= O97_LINE_FLAG_COMMENT;
                else
                if ((stLine.abyFlags[2] & 0x08) == 0x08)
                    lpstCurEntry->wType |= O97_LINE_FLAG_DECLARE;
            }

            // Set the size and offset

            lpstCurEntry->wSize = SSEndianWORD((LPBYTE)&stLine.wSize,
                                               lpstModule->bLittleEndian);
            lpstCurEntry->dwOffset = SSEndianDWORD((LPBYTE)&stLine.dwOffset,
                                                   lpstModule->bLittleEndian);

            ++dwCurLine;
            dwOffset += sizeof(O97_LINE_TABLE_LINE_T);
            ++lpstCurEntry;
        }

        lpstModule->stLineTable.adwBlockNum[dwCacheLineNum] =
            dwBlockNum;
    }

    // Return the desired entry

    *lpstEntry = lpstModule->stLineTable.
        abyBlocks[dwCacheLineNum][dwLine & O97_LINE_TABLE_LIB_MASK];

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetModuleBinary()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in binary stream of bytes to get
//  lpbyBuf         Destination for bytes to get
//  dwNumBytes      Number of bytes to get
//
// Description:
//  Fills a buffer with the binary bytes of the module from the
//  given offset.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleBinary
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpbyBuf,
    DWORD               dwNumBytes
)
{
    DWORD               dwBlockNum;
    DWORD               dwCacheLineNum;
    DWORD               dwBlockOffset;
    DWORD               dwBytesToRead;
    DWORD               dwBytesRead;
    LPBYTE              lpbySrc;

    while (dwNumBytes != 0)
    {
        dwBlockNum = dwOffset & O97_MOD_BIN_BLOCK_NUM_MASK;
        dwCacheLineNum = (dwOffset & O97_MOD_BIN_CLN_MASK) >>
            O97_MOD_BIN_CLN_SHIFT;

        // Is the desired block in the cache?

        if (lpstModule->stBinary.adwBlockNum[dwCacheLineNum] !=
            dwBlockNum)
        {
            // Calculate the number of bytes to read and make
            //  sure the block read stays within the stream

            dwBlockOffset = lpstModule->stBinary.dwOffset +
                dwBlockNum;

            dwBytesToRead = lpstModule->stBinary.dwBlockSize;

            if (dwBlockOffset + dwBytesToRead >
                SSStreamLen(lpstModule->lpstStream))
            {
                if (dwBlockOffset >= SSStreamLen(lpstModule->lpstStream))
                    return(FALSE);

                dwBytesToRead = SSStreamLen(lpstModule->lpstStream) -
                    dwBlockOffset;
            }

            // Read the block

            if (SSSeekRead(lpstModule->lpstStream,
                           dwBlockOffset,
                           lpstModule->stBinary.abyBlocks[dwCacheLineNum],
                           dwBytesToRead,
                           &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != dwBytesToRead)
                return(FALSE);

            lpstModule->stBinary.adwBlockNum[dwCacheLineNum] =
                dwBlockNum;
        }

        // Calculate source and number of bytes in block to read

        lpbySrc = lpstModule->stBinary.abyBlocks[dwCacheLineNum] +
            (dwOffset & O97_MOD_BIN_BIB_MASK);

        dwBytesToRead = lpstModule->stBinary.dwBlockSize -
            (dwOffset & O97_MOD_BIN_BIB_MASK);

        if (dwBytesToRead > dwNumBytes)
            dwBytesToRead = dwNumBytes;

        // Copy bytes from block

        for (dwBytesRead=0;dwBytesRead<dwBytesToRead;dwBytesRead++)
            *lpbyBuf++ = *lpbySrc++;

        // Update count of number of bytes read and offset

        dwNumBytes -= dwBytesToRead;
        dwOffset += dwBytesToRead;
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetModuleWORD()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in binary stream of WORD to get
//  lpwValue        Destination of WORD
//
// Description:
//  Retrieves a WORD value from the module binary stream.  The value
//  is endianized appropriately.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleWORD
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwValue
)
{
    if (O97GetModuleBinary(lpstModule,
                           dwOffset,
                           (LPBYTE)lpwValue,
                           sizeof(WORD)) == FALSE)
        return(FALSE);

    // Endianize it

    *lpwValue = SSEndianWORD((LPBYTE)lpwValue,
                             lpstModule->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// BOOL O97GetModuleDWORD()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in binary stream of DWORD to get
//  lpdwValue       Destination of DWORD
//
// Description:
//  Retrieves a DWORD value from the module binary stream.  The value
//  is endianized appropriately.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97GetModuleDWORD
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPDWORD             lpdwValue
)
{
    if (O97GetModuleBinary(lpstModule,
                           dwOffset,
                           (LPBYTE)lpdwValue,
                           sizeof(DWORD)) == FALSE)
        return(FALSE);

    // Endianize it

    *lpdwValue = SSEndianDWORD((LPBYTE)lpdwValue,
                               lpstModule->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// BOOL O97TypeTableGetRecordIdent()
//
// Parameters:
//  lpstModule      Ptr to initialized module access structure
//  dwOffset        Offset in type table of record
//  lpwIdent        Ptr to WORD for identifier WORD
//
// Description:
//  Retrieves the ID of the identifier at the given record.  It is
//  assumed to be at offset two from the beginning of the record.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97TypeTableGetRecordIdent
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwIdent
)
{
    DWORD               dwBytesRead;

    if (SSSeekRead(lpstModule->lpstStream,
                   lpstModule->stTypeTable.dwOffset + dwOffset + 2,
                   lpwIdent,
                   sizeof(WORD),
                   &dwBytesRead) != SS_STATUS_OK)
        return(FALSE);

    *lpwIdent = SSEndianWORD((LPBYTE)lpwIdent,
                             lpstModule->bLittleEndian);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97TypeTableGetRoutineInfo()
//
// Parameters:
//  lpstModule           Ptr to initialized module access structure
//  dwOffset             Offset in type table of record
//  lpabyFlagBytes       First two bytes of record
//  lpwIdent             Ptr to WORD for identifier WORD
//  lpbyNumParams        Ptr to BYTE for number of parameters
//  lpdwFirstParamOffset Ptr to DWORD for next param record
//
// Description:
//  Retrieves the ID of the identifier and the first parameter offset
//  for the record at the given offset.  It is assumed that the ID
//  is at offset two from the beginning of the record and the first
//  parameter offset is at offset 0x24 from the beginning of the
//  record.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97TypeTableGetRoutineInfo
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPBYTE              lpabyFlagBytes,
    LPWORD              lpwIdent,
    LPBYTE              lpbyNumParams,
    LPDWORD             lpdwFirstParamOffset
)
{
    BYTE                abyTempNumParams[2];
    BOOL                bOffice9Shift4Bytes =  FALSE;
    DWORD               dwBytesRead;

    if (lpabyFlagBytes != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset,
                       lpabyFlagBytes,
                       2 * sizeof(BYTE),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);
    }

    if (lpwIdent != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 2,
                       lpwIdent,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpwIdent = SSEndianWORD((LPBYTE)lpwIdent,
                                 lpstModule->bLittleEndian);
    }

    if (lpbyNumParams != NULL)
    {
        // see if this is a office 9 table first
        // look in the new location first

        
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 0x3A,
                       abyTempNumParams,
                       sizeof(abyTempNumParams),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        // It has been observed that subs have a 0x94 or 0x84 preceeding the num params byte
        // and functions have a 0x8c or 0xbc preceeding the num params byte
        // I also noticed a 0x0c.  It's probably for a function, but I didn't check.

        // 0x9c is one that I assume is possible based on seeing 0x94, 0x84, and 0x8c

        // so since we know that a relatively constant set of flags preceeds the num params byte,
        // we will look for this byte to determine if this file appears to have the 4 byte shift

        if (abyTempNumParams[0] == 0x8C || 
            abyTempNumParams[0] == 0x94 || 
            abyTempNumParams[0] == 0x84 || 
            abyTempNumParams[0] == 0x9C ||  // leaving this constant so I don't cause regression
            abyTempNumParams[0] == 0xBC || 
            abyTempNumParams[0] == 0x0C )
        {
            // we need to look in the new location for the first param offset
            // so we'll set a flag so we check the new location now

            bOffice9Shift4Bytes = TRUE;

            // store the number of params which follows our "bookmark"

            *lpbyNumParams = abyTempNumParams[1];
        }
        else
        {
            // read from the old location

            if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 0x37,
                       lpbyNumParams,
                       sizeof(BYTE),
                       &dwBytesRead) != SS_STATUS_OK)
                return(FALSE);
        }
    }

    if (lpdwFirstParamOffset != NULL)
    {
        if (bOffice9Shift4Bytes == TRUE)
        {
            // already determined we need to shift so we read from the new location

            if (SSSeekRead(lpstModule->lpstStream,
                           lpstModule->stTypeTable.dwOffset + dwOffset + 0x28,
                           lpdwFirstParamOffset,
                           sizeof(DWORD),
                           &dwBytesRead) != SS_STATUS_OK)
                return(FALSE);
            
        }
        else
        {
            // read from the old location...

            if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 0x24,
                       lpdwFirstParamOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK)
                return(FALSE);
        }

        *lpdwFirstParamOffset = SSEndianDWORD((LPBYTE)lpdwFirstParamOffset,
                                              lpstModule->bLittleEndian);
    }

    return(TRUE);
}

/****************** elai TDT NOTES ******************************

    the bookmark used (8|9)(4|C) was chosen because 
    there be a pretty low chance of finding that randomly
    unlike the FF values...

    another option if this "hack" proves to be too problematic,
    would be to identify whether this was a 97 version of
    MS office some other way, somewhere else.

    // it appears that all subs have a 94 preceeding the num params
    //                 all functions a 8C             "
    // so we'll use this to figure out which format of the 
    // type table we're looking at.

    // exception: nalp, office 2000 
    //                     sub had   a 84
    // so we'll do a check for 9c as well for good measure
    //      see below this function for TDT notes

    Another exception is 491486A4.DOT, which had a function with BC
    And another unknown sample had 0C



    office 9 (2000), sub, two args, from NALP virus
    00000240: 0B 12 54 02 FF FF FF FF FF FF FF FF 00 00 00 00 ..T.............
    00000250: FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 ................
    00000260: FF FF FF FF 00 00 00 00 80 02 00 00 FF FF FF FF ................
    00000270: FF FF FF FF 05 00 00 00 00 00 84*02 00 00 00 00 ................

    wrench sub
    office 9 (2000)
    000001E0: 0C 11 36 02 C0 02 00 00 00 00 04 60 00 00 00 00 ..6........`....
    000001F0: FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 ................
    00000200: FF FF FF FF 00 00 00 00 20 02 00 00 FF FF FF FF ........ .......
    00000210: EC 03 FF FF 04 00 04 00 00 00 94*01 00 01 00 00 ................


    Office 8 (97)
    000002F0: 04 11 60 02 F8 03 00 00 FF FF FF FF 00 00 00 00 ..`.............
    00000300: FF FF FF FF FF FF FF FF 00 00 00 00 FF FF FF FF ................
    00000310: 00 00 00 00 30 03 00 00 FF FF FF FF FF FF FF FF ....0...........
    00000320: 37 00 00 00 00 00 94*01 00 00 00 00 00 00 00 00 7...............



    wrench
    function Office 9 (2000)
    Name: BENCH
    Type table offset: 0x00000196    Size: 0x00000048
    ----------------------------------------------------------------------
    00000000: 0B 12 1E 02 FF FF FF FF FF FF FF FF 00 00 00 00 ................
    00000010: FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 ................
    00000020: FF FF FF FF 00 00 00 00 FF FF FF FF 0C 00 FF FF ................
    00000030: FF FF FF FF 13 00 00 00 00 00 8C*00 00 02 00 00 ................
    00000040: FF FF FF FF 38 00 00 00                         ....8...

    function Office 8 (97)
    00000000: 0B 12 1E 02 40 00 00 00 FF FF FF FF 00 00 00 00 ....@...........
    00000010: FF FF FF FF FF FF FF FF 00 00 00 00 FF FF FF FF ................
    00000020: 00 00 00 00 FF FF FF FF 0C 00 FF FF FF FF FF FF ................
    00000030: 08 00 00 00 00 00 8C*00 00 00 00 00 48 00 00 00 ............H...

    
    491486A4.DOT, or Class97/2K.Sun maybe?
    ----------------------------------------------------------------------

    function Office 9 (2000)
    000001A0: FF FF FF FF 00 00 00 00 FF FF FF FF FF FF FF FF ................
    000001B0: 00 00 00 00 FF FF FF FF 00 00 00 00 D8 01 00 00 ................
    000001C0: 08 00 FF FF FF FF FF FF 11 00 11 00 00 00 BC*02 ................
    000001D0: 00 00 00 00 00 00 30 33 49 83 8C 02 FF FF FF FF ......03I.......
    000001E0: FF FF FF FF 0C 01 FF FF 00 00 00 00 F8 01 00 00 ................

    function office 8 (97)
    00000110: FF FF FF FF FF FF FF FF 00 00 00 00 00 00 00 00 ................
    00000120: FF FF FF FF 00 00 00 00 40 01 00 00 08 00 FF FF ........@.......
    00000130: FF FF FF FF 12 00 12 00 00 00 BC*02 00 00 00 00 ................
    00000140: 49 83 82 02 FF FF FF FF FF FF FF FF 0C 01 FF FF I...............
    00000150: 00 00 00 00 60 01 00 00 80 01 00 00 00 00 00 00 ....`...........


    // I think the constant is a set of bit flags

    0 0000
    B 1011
    8 1000
    9 1001


    4 0100  seems to indicate sub
    C 1100  seems to indicate function


********************end elai TDT NOTES **************************/


//********************************************************************
//
// BOOL O97TypeTableGetParamInfo()
//
// Parameters:
//  lpstModule          Ptr to initialized module access structure
//  dwOffset            Offset in type table of record
//  lpwIdent            Ptr to WORD for identifier WORD
//  lpdwNextParamOffset Ptr to DWORD for next param record
//
// Description:
//  Retrieves the ID of the identifier and the next parameter offset
//  for the record at the given offset.  It is assumed that the ID
//  is at offset two from the beginning of the record and the next
//  parameter offset is at offset 0x14 from the beginning of the
//  record.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97TypeTableGetParamInfo
(
    LPO97_MODULE        lpstModule,
    DWORD               dwOffset,
    LPWORD              lpwIdent,
    LPDWORD             lpdwNextParamOffset
)
{
    DWORD               dwBytesRead;

    if (lpwIdent != NULL)
    {
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 2,
                       lpwIdent,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpwIdent = SSEndianWORD((LPBYTE)lpwIdent,
                                 lpstModule->bLittleEndian);
    }

    if (lpdwNextParamOffset != NULL)
    {

        // not affected by the shift in the type table structure
        if (SSSeekRead(lpstModule->lpstStream,
                       lpstModule->stTypeTable.dwOffset + dwOffset + 0x14,
                       lpdwNextParamOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpdwNextParamOffset = SSEndianDWORD((LPBYTE)lpdwNextParamOffset,
                                             lpstModule->bLittleEndian);
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  int O97GetOpSkip()
//
// Parameters:
//  lpabyOp             Ptr to array of op bytes
//  nNumOpBytes         Number of bytes in array
//
// Description:
//  Calculates the number of operand bytes for the opcode at the
//  given pointer.  If the number of bytes needed to determine the
//  number of operand bytes for the opcode are not within nNumOpBytes,
//  the function returns -1.
//
// Returns:
//  >= 0            On success
//  < 0             On error
//
//********************************************************************

// The following table contains entries of the form:
//
//      A * 0x80 + B
//
// If A is 0, then B gives the constant number of operand bytes.
// If A is non-zero, then the opcode has a variable number of operands.
//  Except for opcode 0xD7 (quoted comment), the number of operand
//  bytes is given by the WORD value following the opcode WORD.
//  For opcode 0xD7, the number of additional operand bytes is given
//  in the second WORD following the opcode WORD.

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO97OpSkip[O97_OP_MAX+1] =
{
    0 * 0x80 + 0,   // 0x00 // Imp
    0 * 0x80 + 0,   // 0x01 // Eqv
    0 * 0x80 + 0,   // 0x02 // Xor
    0 * 0x80 + 0,   // 0x03 // Or
    0 * 0x80 + 0,   // 0x04 // And
    0 * 0x80 + 0,   // 0x05 // Eq
    0 * 0x80 + 0,   // 0x06 // Ne
    0 * 0x80 + 0,   // 0x07 // Le
    0 * 0x80 + 0,   // 0x08 // Ge
    0 * 0x80 + 0,   // 0x09 // Lt
    0 * 0x80 + 0,   // 0x0A // Gt
    0 * 0x80 + 0,   // 0x0B // Add
    0 * 0x80 + 0,   // 0x0C // Sub
    0 * 0x80 + 0,   // 0x0D // Mod
    0 * 0x80 + 0,   // 0x0E // IDv
    0 * 0x80 + 0,   // 0x0F // Mul
    0 * 0x80 + 0,   // 0x10 // Div
    0 * 0x80 + 0,   // 0x11 // Concat
    0 * 0x80 + 0,   // 0x12 // Like
    0 * 0x80 + 0,   // 0x13 // Pwr
    0 * 0x80 + 0,   // 0x14 // Is
    0 * 0x80 + 0,   // 0x15 // Not
    0 * 0x80 + 0,   // 0x16 // UMi
    0 * 0x80 + 0,   // 0x17 // FnAbs
    0 * 0x80 + 0,   // 0x18 // FnFix
    0 * 0x80 + 0,   // 0x19 // FnInt
    0 * 0x80 + 0,   // 0x1A // FnSgn
    0 * 0x80 + 0,   // 0x1B // FnLen
    0 * 0x80 + 0,   // 0x1C // FnLenB
    0 * 0x80 + 0,   // 0x1D // Paren
    0 * 0x80 + 0,   // 0x1E // Sharp
    0 * 0x80 + 2,   // 0x1F // LdLHS
    0 * 0x80 + 2,   // 0x20 // Ld
    0 * 0x80 + 2,   // 0x21 // MemLd
    0 * 0x80 + 2,   // 0x22 // DictLd
    0 * 0x80 + 2,   // 0x23 // IndexLd
    0 * 0x80 + 4,   // 0x24 // ArgsLd
    0 * 0x80 + 4,   // 0x25 // ArgsMemLd
    0 * 0x80 + 4,   // 0x26 // ArgsDictLd
    0 * 0x80 + 2,   // 0x27 // St
    0 * 0x80 + 2,   // 0x28 // MemSt
    0 * 0x80 + 2,   // 0x29 // DictSt
    0 * 0x80 + 2,   // 0x2A // IndexSt
    0 * 0x80 + 4,   // 0x2B // ArgsSt
    0 * 0x80 + 4,   // 0x2C // ArgsMemSt
    0 * 0x80 + 4,   // 0x2D // ArgsDictSt
    0 * 0x80 + 2,   // 0x2E // Set
    0 * 0x80 + 2,   // 0x2F // MemSet
    0 * 0x80 + 2,   // 0x30 // DictSet
    0 * 0x80 + 2,   // 0x31 // IndexSet
    0 * 0x80 + 4,   // 0x32 // ArgsSet
    0 * 0x80 + 4,   // 0x33 // ArgsMemSet
    0 * 0x80 + 4,   // 0x34 // ArgsDictSet
    0 * 0x80 + 2,   // 0x35 // MemLdWith
    0 * 0x80 + 2,   // 0x36 // DictLdWith
    0 * 0x80 + 4,   // 0x37 // ArgsMemLdWith
    0 * 0x80 + 4,   // 0x38 // ArgsDictLdWith
    0 * 0x80 + 2,   // 0x39 // MemStWith
    0 * 0x80 + 2,   // 0x3A // DictStWith
    0 * 0x80 + 4,   // 0x3B // ArgsMemStWith
    0 * 0x80 + 4,   // 0x3C // ArgsDictStWith
    0 * 0x80 + 2,   // 0x3D // MemSetWith
    0 * 0x80 + 2,   // 0x3E // DictSetWith
    0 * 0x80 + 4,   // 0x3F // ArgsMemSetWith
    0 * 0x80 + 4,   // 0x40 // ArgsDictSetWith
    0 * 0x80 + 4,   // 0x41 // ArgsCall
    0 * 0x80 + 4,   // 0x42 // ArgsMemCall
    0 * 0x80 + 4,   // 0x43 // ArgsMemCallWith
    0 * 0x80 + 4,   // 0x44 // ArgsArray
    0 * 0x80 + 2,   // 0x45 // Bos
    0 * 0x80 + 0,   // 0x46 // BosImplicit
    0 * 0x80 + 0,   // 0x47 // Bol
    0 * 0x80 + 0,   // 0x48 // Case
    0 * 0x80 + 0,   // 0x49 // CaseTo
    0 * 0x80 + 0,   // 0x4A // CaseGt
    0 * 0x80 + 0,   // 0x4B // CaseLt
    0 * 0x80 + 0,   // 0x4C // CaseGe
    0 * 0x80 + 0,   // 0x4D // CaseLe
    0 * 0x80 + 0,   // 0x4E // CaseNe
    0 * 0x80 + 0,   // 0x4F // CaseEq
    0 * 0x80 + 0,   // 0x50 // CaseElse
    0 * 0x80 + 0,   // 0x51 // CaseDone
    0 * 0x80 + 2,   // 0x52 // Circle
    0 * 0x80 + 2,   // 0x53 // Close
    0 * 0x80 + 0,   // 0x54 // CloseAll
    0 * 0x80 + 0,   // 0x55 // Coerce
    0 * 0x80 + 0,   // 0x56 // CoerceVar
    0 * 0x80 + 4,   // 0x57 // Context
    0 * 0x80 + 0,   // 0x58 // Debug
    0 * 0x80 + 4,   // 0x59 // Deftype
    0 * 0x80 + 0,   // 0x5A // Dim
    0 * 0x80 + 0,   // 0x5B // DimImplicit
    0 * 0x80 + 0,   // 0x5C // Do
    0 * 0x80 + 0,   // 0x5D // DoEvents
    0 * 0x80 + 0,   // 0x5E // DoUntil
    0 * 0x80 + 0,   // 0x5F // DoWhile
    0 * 0x80 + 0,   // 0x60 // Else
    0 * 0x80 + 0,   // 0x61 // ElseBlock
    0 * 0x80 + 0,   // 0x62 // ElseIfBlock
    0 * 0x80 + 2,   // 0x63 // ElseIfTypeBlock
    0 * 0x80 + 0,   // 0x64 // End
    0 * 0x80 + 0,   // 0x65 // EndContext
    0 * 0x80 + 0,   // 0x66 // EndFunc
    0 * 0x80 + 0,   // 0x67 // EndIf
    0 * 0x80 + 0,   // 0x68 // EndIfBlock
    0 * 0x80 + 0,   // 0x69 // EndImmediate
    0 * 0x80 + 0,   // 0x6A // EndProp
    0 * 0x80 + 0,   // 0x6B // EndSelect
    0 * 0x80 + 0,   // 0x6C // EndSub
    0 * 0x80 + 0,   // 0x6D // EndType
    0 * 0x80 + 0,   // 0x6E // EndWith
    0 * 0x80 + 2,   // 0x6F // Erase
    0 * 0x80 + 0,   // 0x70 // Error
    0 * 0x80 + 0,   // 0x71 // ExitDo
    0 * 0x80 + 0,   // 0x72 // ExitFor
    0 * 0x80 + 0,   // 0x73 // ExitFunc
    0 * 0x80 + 0,   // 0x74 // ExitProp
    0 * 0x80 + 0,   // 0x75 // ExitSub
    0 * 0x80 + 0,   // 0x76 // FnCurDir
    0 * 0x80 + 0,   // 0x77 // FnDir
    0 * 0x80 + 0,   // 0x78 // Empty0
    0 * 0x80 + 0,   // 0x79 // Empty1
    0 * 0x80 + 0,   // 0x7A // FnError
    0 * 0x80 + 0,   // 0x7B // FnFormat
    0 * 0x80 + 0,   // 0x7C // FnFreeFile
    0 * 0x80 + 0,   // 0x7D // FnInStr
    0 * 0x80 + 0,   // 0x7E // FnInStr3
    0 * 0x80 + 0,   // 0x7F // FnInStr4
    0 * 0x80 + 0,   // 0x80 // FnInStrB
    0 * 0x80 + 0,   // 0x81 // FnInStrB3
    0 * 0x80 + 0,   // 0x82 // FnInStrB4
    0 * 0x80 + 2,   // 0x83 // FnLBound
    0 * 0x80 + 0,   // 0x84 // FnMid
    0 * 0x80 + 0,   // 0x85 // FnMidB
    0 * 0x80 + 0,   // 0x86 // FnStrComp
    0 * 0x80 + 0,   // 0x87 // FnStrComp3
    0 * 0x80 + 0,   // 0x88 // FnStringVar
    0 * 0x80 + 0,   // 0x89 // FnStringStr
    0 * 0x80 + 2,   // 0x8A // FnUBound
    0 * 0x80 + 0,   // 0x8B // For
    0 * 0x80 + 0,   // 0x8C // ForEach
    0 * 0x80 + 2,   // 0x8D // ForEachAs
    0 * 0x80 + 0,   // 0x8E // ForStep
    0 * 0x80 + 4,   // 0x8F // FuncDefn
    0 * 0x80 + 4,   // 0x90 // FuncDefnSave
    0 * 0x80 + 0,   // 0x91 // GetRec
    0 * 0x80 + 2,   // 0x92 // Gosub
    0 * 0x80 + 2,   // 0x93 // Goto
    0 * 0x80 + 0,   // 0x94 // If
    0 * 0x80 + 0,   // 0x95 // IfBlock
    0 * 0x80 + 2,   // 0x96 // TypeOf
    0 * 0x80 + 2,   // 0x97 // IfTypeBlock
    0 * 0x80 + 0,   // 0x98 // Input
    0 * 0x80 + 0,   // 0x99 // InputDone
    0 * 0x80 + 0,   // 0x9A // InputItem
    0 * 0x80 + 2,   // 0x9B // Label
    0 * 0x80 + 0,   // 0x9C // Let
    0 * 0x80 + 2,   // 0x9D // Line
    1 * 0x80 + 0,   // 0x9E // LineCont
    0 * 0x80 + 0,   // 0x9F // LineInput
    0 * 0x80 + 2,   // 0xA0 // LineNum
    0 * 0x80 + 8,   // 0xA1 // LitCy
    0 * 0x80 + 8,   // 0xA2 // LitDate
    0 * 0x80 + 0,   // 0xA3 // LitDefault
    0 * 0x80 + 2,   // 0xA4 // LitDI2
    0 * 0x80 + 4,   // 0xA5 // LitDI4
    0 * 0x80 + 2,   // 0xA6 // LitHI2
    0 * 0x80 + 4,   // 0xA7 // LitHI4
    0 * 0x80 + 0,   // 0xA8 // LitNothing
    0 * 0x80 + 2,   // 0xA9 // LitOI2
    0 * 0x80 + 4,   // 0xAA // LitOI4
    0 * 0x80 + 4,   // 0xAB // LitR4
    0 * 0x80 + 8,   // 0xAC // LitR8
    0 * 0x80 + 0,   // 0xAD // LitSmallI2
    1 * 0x80 + 0,   // 0xAE // LitStr
    0 * 0x80 + 0,   // 0xAF // LitVarSpecial
    0 * 0x80 + 0,   // 0xB0 // Lock
    0 * 0x80 + 0,   // 0xB1 // Loop
    0 * 0x80 + 0,   // 0xB2 // LoopUntil
    0 * 0x80 + 0,   // 0xB3 // LoopWhile
    0 * 0x80 + 0,   // 0xB4 // Lset
    0 * 0x80 + 0,   // 0xB5 // Me
    0 * 0x80 + 0,   // 0xB6 // MeImplicit
    0 * 0x80 + 8,   // 0xB7 // MemRedim
    0 * 0x80 + 8,   // 0xB8 // MemRedimWith
    0 * 0x80 + 8,   // 0xB9 // MemRedimAs
    0 * 0x80 + 8,   // 0xBA // MemRedimAsWith
    0 * 0x80 + 0,   // 0xBB // Mid
    0 * 0x80 + 0,   // 0xBC // MidB
    0 * 0x80 + 0,   // 0xBD // Name
    0 * 0x80 + 2,   // 0xBE // New
    0 * 0x80 + 0,   // 0xBF // Next
    0 * 0x80 + 0,   // 0xC0 // NextVar
    0 * 0x80 + 2,   // 0xC1 // OnError
    1 * 0x80 + 0,   // 0xC2 // OnGosub
    1 * 0x80 + 0,   // 0xC3 // OnGoto
    0 * 0x80 + 2,   // 0xC4 // Open
    0 * 0x80 + 0,   // 0xC5 // Option
    0 * 0x80 + 0,   // 0xC6 // OptionBase
    0 * 0x80 + 0,   // 0xC7 // ParamByval
    0 * 0x80 + 0,   // 0xC8 // ParamOmitted
    0 * 0x80 + 2,   // 0xC9 // ParamNamed
    0 * 0x80 + 0,   // 0xCA // PrintChan
    0 * 0x80 + 0,   // 0xCB // PrintComma
    0 * 0x80 + 0,   // 0xCC // PrintEos
    0 * 0x80 + 0,   // 0xCD // PrintItemComma
    0 * 0x80 + 0,   // 0xCE // PrintItemNL
    0 * 0x80 + 0,   // 0xCF // PrintItemSemi
    0 * 0x80 + 0,   // 0xD0 // PrintNL
    0 * 0x80 + 0,   // 0xD1 // PrintObj
    0 * 0x80 + 0,   // 0xD2 // PrintSemi
    0 * 0x80 + 0,   // 0xD3 // PrintSpc
    0 * 0x80 + 0,   // 0xD4 // PrintTab
    0 * 0x80 + 0,   // 0xD5 // PrintTabComma
    0 * 0x80 + 2,   // 0xD6 // Pset
    0 * 0x80 + 0,   // 0xD7 // PutRec
    1 * 0x80 + 0,   // 0xD8 // QuoteRem
    0 * 0x80 + 8,   // 0xD9 // Redim
    0 * 0x80 + 8,   // 0xDA // RedimAs
    1 * 0x80 + 0,   // 0xDB // Reparse
    1 * 0x80 + 0,   // 0xDC // Rem
    0 * 0x80 + 2,   // 0xDD // Resume
    0 * 0x80 + 0,   // 0xDE // Return
    0 * 0x80 + 0,   // 0xDF // Rset
    0 * 0x80 + 2,   // 0xE0 // Scale
    0 * 0x80 + 0,   // 0xE1 // Seek
    0 * 0x80 + 0,   // 0xE2 // SelectCase
    0 * 0x80 + 2,   // 0xE3 // SelectIs
    0 * 0x80 + 0,   // 0xE4 // SelectType
    0 * 0x80 + 0,   // 0xE5 // SetStmt
    0 * 0x80 + 6,   // 0xE6 // Stack
    0 * 0x80 + 0,   // 0xE7 // Stop
    0 * 0x80 + 4,   // 0xE8 // Type
    0 * 0x80 + 0,   // 0xE9 // UnLock
    1 * 0x80 + 0,   // 0xEA // VarDefn
    0 * 0x80 + 0,   // 0xEB // Wend
    0 * 0x80 + 0,   // 0xEC // While
    0 * 0x80 + 0,   // 0xED // With
    0 * 0x80 + 0,   // 0xEE // WriteChan
    0 * 0x80 + 0,   // 0xEF // ConstFuncExpr
    0 * 0x80 + 2,   // 0xF0 // LbConst
    0 * 0x80 + 0,   // 0xF1 // LbIf
    0 * 0x80 + 0,   // 0xF2 // LbElse
    0 * 0x80 + 0,   // 0xF3 // LbElseif
    0 * 0x80 + 0,   // 0xF4 // LbEndif
    0 * 0x80 + 0,   // 0xF5 // LbMark
    0 * 0x80 + 0,   // 0xF6 // EndForVariable
    0 * 0x80 + 0,   // 0xF7 // StartForVariable
    0 * 0x80 + 0,   // 0xF8 // NewRedim
    0 * 0x80 + 0,   // 0xF9 // StartWithExpr
    0 * 0x80 + 2,   // 0xFA // SetOrSt
    0 * 0x80 + 0,   // 0xFB // EndEnum
    0 * 0x80 + 0,   // 0xFC // Illegal
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

int O97GetOpSkip
(
    BOOL        bLittleEndian,
    LPBYTE      lpabyOp,
    int         nNumOpBytes
)
{
    int         nOpcode;
    WORD        wTemp;

    if (nNumOpBytes < 2)
        return(-1);

    if (bLittleEndian)
        nOpcode = lpabyOp[0];
    else
        nOpcode = lpabyOp[1];

    if (nOpcode > O97_OP_MAX)
        nOpcode = O97_OP_MAX;

    if (gabyO97OpSkip[nOpcode] < 0x80)
        return(gabyO97OpSkip[nOpcode]);

    if (nOpcode == O97_OP_VAR_DEFN)
    {
        if ((bLittleEndian && (lpabyOp[1] & 0x40)) ||
            (bLittleEndian == FALSE && (lpabyOp[1] & 0x40) == 0))
            return(6);
        else
            return(4);
    }

    if (nOpcode == O97_OP_QUOTED_COMMENT)
    {
        if (nNumOpBytes < 6)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+4,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(4 + (int)wTemp);
    }
    else
    {
        if (nNumOpBytes < 4)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+2,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(2 + (int)wTemp);
    }
}


//********************************************************************
//
// Function:
//  int O98GetOpSkip()
//
// Parameters:
//  lpabyOp             Ptr to array of op bytes
//  nNumOpBytes         Number of bytes in array
//
// Description:
//  Calculates the number of operand bytes for the opcode at the
//  given pointer.  If the number of bytes needed to determine the
//  number of operand bytes for the opcode are not within nNumOpBytes,
//  the function returns -1.
//
// Returns:
//  >= 0            On success
//  < 0             On error
//
//********************************************************************

// The following table contains entries of the form:
//
//      A * 0x80 + B
//
// If A is 0, then B gives the constant number of operand bytes.
// If A is non-zero, then the opcode has a variable number of operands.
//  Except for opcode 0xD7 (quoted comment), the number of operand
//  bytes is given by the WORD value following the opcode WORD.
//  For opcode 0xD7, the number of additional operand bytes is given
//  in the second WORD following the opcode WORD.

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyO98OpSkip[O98_OP_MAX+1] =
{
    0 * 0x80 + 0,   // 0x00 // Imp
    0 * 0x80 + 0,   // 0x01 // Eqv
    0 * 0x80 + 0,   // 0x02 // Xor
    0 * 0x80 + 0,   // 0x03 // Or
    0 * 0x80 + 0,   // 0x04 // And
    0 * 0x80 + 0,   // 0x05 // Eq
    0 * 0x80 + 0,   // 0x06 // Ne
    0 * 0x80 + 0,   // 0x07 // Le
    0 * 0x80 + 0,   // 0x08 // Ge
    0 * 0x80 + 0,   // 0x09 // Lt
    0 * 0x80 + 0,   // 0x0A // Gt
    0 * 0x80 + 0,   // 0x0B // Add
    0 * 0x80 + 0,   // 0x0C // Sub
    0 * 0x80 + 0,   // 0x0D // Mod
    0 * 0x80 + 0,   // 0x0E // IDv
    0 * 0x80 + 0,   // 0x0F // Mul
    0 * 0x80 + 0,   // 0x10 // Div
    0 * 0x80 + 0,   // 0x11 // Concat
    0 * 0x80 + 0,   // 0x12 // Like
    0 * 0x80 + 0,   // 0x13 // Pwr
    0 * 0x80 + 0,   // 0x14 // Is
    0 * 0x80 + 0,   // 0x15 // Not
    0 * 0x80 + 0,   // 0x16 // UMi
    0 * 0x80 + 0,   // 0x17 // FnAbs
    0 * 0x80 + 0,   // 0x18 // FnFix
    0 * 0x80 + 0,   // 0x19 // FnInt
    0 * 0x80 + 0,   // 0x1A // FnSgn
    0 * 0x80 + 0,   // 0x1B // FnLen
    0 * 0x80 + 0,   // 0x1C // FnLenB
    0 * 0x80 + 0,   // 0x1D // Paren
    0 * 0x80 + 0,   // 0x1E // Sharp
    0 * 0x80 + 2,   // 0x1F // LdLHS
    0 * 0x80 + 2,   // 0x20 // Ld
    0 * 0x80 + 2,   // 0x21 // MemLd
    0 * 0x80 + 2,   // 0x22 // DictLd
    0 * 0x80 + 2,   // 0x23 // IndexLd
    0 * 0x80 + 4,   // 0x24 // ArgsLd
    0 * 0x80 + 4,   // 0x25 // ArgsMemLd
    0 * 0x80 + 4,   // 0x26 // ArgsDictLd
    0 * 0x80 + 2,   // 0x27 // St
    0 * 0x80 + 2,   // 0x28 // MemSt
    0 * 0x80 + 2,   // 0x29 // DictSt
    0 * 0x80 + 2,   // 0x2A // IndexSt
    0 * 0x80 + 4,   // 0x2B // ArgsSt
    0 * 0x80 + 4,   // 0x2C // ArgsMemSt
    0 * 0x80 + 4,   // 0x2D // ArgsDictSt
    0 * 0x80 + 2,   // 0x2E // Set
    0 * 0x80 + 2,   // 0x2F // MemSet
    0 * 0x80 + 2,   // 0x30 // DictSet
    0 * 0x80 + 2,   // 0x31 // IndexSet
    0 * 0x80 + 4,   // 0x32 // ArgsSet
    0 * 0x80 + 4,   // 0x33 // ArgsMemSet
    0 * 0x80 + 4,   // 0x34 // ArgsDictSet
    0 * 0x80 + 2,   // 0x35 // MemLdWith
    0 * 0x80 + 2,   // 0x36 // DictLdWith
    0 * 0x80 + 4,   // 0x37 // ArgsMemLdWith
    0 * 0x80 + 4,   // 0x38 // ArgsDictLdWith
    0 * 0x80 + 2,   // 0x39 // MemStWith
    0 * 0x80 + 2,   // 0x3A // DictStWith
    0 * 0x80 + 4,   // 0x3B // ArgsMemStWith
    0 * 0x80 + 4,   // 0x3C // ArgsDictStWith
    0 * 0x80 + 2,   // 0x3D // MemSetWith
    0 * 0x80 + 2,   // 0x3E // DictSetWith
    0 * 0x80 + 4,   // 0x3F // ArgsMemSetWith
    0 * 0x80 + 4,   // 0x40 // ArgsDictSetWith
    0 * 0x80 + 4,   // 0x41 // ArgsCall
    0 * 0x80 + 4,   // 0x42 // ArgsMemCall
    0 * 0x80 + 4,   // 0x43 // ArgsMemCallWith
    0 * 0x80 + 4,   // 0x44 // ArgsArray
    0 * 0x80 + 0,   // 0x45 // Assert
    0 * 0x80 + 2,   // 0x46 // Bos
    0 * 0x80 + 0,   // 0x47 // BosImplicit
    0 * 0x80 + 0,   // 0x48 // Bol
    0 * 0x80 + 2,   // 0x49 // LdAddressOf
    0 * 0x80 + 2,   // 0x4A // MemAddressOf
    0 * 0x80 + 0,   // 0x4B // Case
    0 * 0x80 + 0,   // 0x4C // CaseTo
    0 * 0x80 + 0,   // 0x4D // CaseGt
    0 * 0x80 + 0,   // 0x4E // CaseLt
    0 * 0x80 + 0,   // 0x4F // CaseGe
    0 * 0x80 + 0,   // 0x50 // CaseLe
    0 * 0x80 + 0,   // 0x51 // CaseNe
    0 * 0x80 + 0,   // 0x52 // CaseEq
    0 * 0x80 + 0,   // 0x53 // CaseElse
    0 * 0x80 + 0,   // 0x54 // CaseDone
    0 * 0x80 + 2,   // 0x55 // Circle
    0 * 0x80 + 2,   // 0x56 // Close
    0 * 0x80 + 0,   // 0x57 // CloseAll
    0 * 0x80 + 0,   // 0x58 // Coerce
    0 * 0x80 + 0,   // 0x59 // CoerceVar
    0 * 0x80 + 4,   // 0x5A // Context
    0 * 0x80 + 0,   // 0x5B // Debug
    0 * 0x80 + 4,   // 0x5C // Deftype
    0 * 0x80 + 0,   // 0x5D // Dim
    0 * 0x80 + 0,   // 0x5E // DimImplicit
    0 * 0x80 + 0,   // 0x5F // Do
    0 * 0x80 + 0,   // 0x60 // DoEvents
    0 * 0x80 + 0,   // 0x61 // DoUntil
    0 * 0x80 + 0,   // 0x62 // DoWhile
    0 * 0x80 + 0,   // 0x63 // Else
    0 * 0x80 + 0,   // 0x64 // ElseBlock
    0 * 0x80 + 0,   // 0x65 // ElseIfBlock
    0 * 0x80 + 2,   // 0x66 // ElseIfTypeBlock
    0 * 0x80 + 0,   // 0x67 // End
    0 * 0x80 + 0,   // 0x68 // EndContext
    0 * 0x80 + 0,   // 0x69 // EndFunc
    0 * 0x80 + 0,   // 0x6A // EndIf
    0 * 0x80 + 0,   // 0x6B // EndIfBlock
    0 * 0x80 + 0,   // 0x6C // EndImmediate
    0 * 0x80 + 0,   // 0x6D // EndProp
    0 * 0x80 + 0,   // 0x6E // EndSelect
    0 * 0x80 + 0,   // 0x6F // EndSub
    0 * 0x80 + 0,   // 0x70 // EndType
    0 * 0x80 + 0,   // 0x71 // EndWith
    0 * 0x80 + 2,   // 0x72 // Erase
    0 * 0x80 + 0,   // 0x73 // Error
    0 * 0x80 + 4,   // 0x74 // EventDecl
    0 * 0x80 + 4,   // 0x75 // RaiseEvent
    0 * 0x80 + 4,   // 0x76 // ArgsMemRaiseEvent
    0 * 0x80 + 4,   // 0x77 // ArgsMemRaiseEventWith
    0 * 0x80 + 0,   // 0x78 // ExitDo
    0 * 0x80 + 0,   // 0x79 // ExitFor
    0 * 0x80 + 0,   // 0x7A // ExitFunc
    0 * 0x80 + 0,   // 0x7B // ExitProp
    0 * 0x80 + 0,   // 0x7C // ExitSub
    0 * 0x80 + 0,   // 0x7D // FnCurDir
    0 * 0x80 + 0,   // 0x7E // FnDir
    0 * 0x80 + 0,   // 0x7F // Empty0
    0 * 0x80 + 0,   // 0x80 // Empty1
    0 * 0x80 + 0,   // 0x81 // FnError
    0 * 0x80 + 0,   // 0x82 // FnFormat
    0 * 0x80 + 0,   // 0x83 // FnFreeFile
    0 * 0x80 + 0,   // 0x84 // FnInStr
    0 * 0x80 + 0,   // 0x85 // FnInStr3
    0 * 0x80 + 0,   // 0x86 // FnInStr4
    0 * 0x80 + 0,   // 0x87 // FnInStrB
    0 * 0x80 + 0,   // 0x88 // FnInStrB3
    0 * 0x80 + 0,   // 0x89 // FnInStrB4
    0 * 0x80 + 2,   // 0x8A // FnLBound
    0 * 0x80 + 0,   // 0x8B // FnMid
    0 * 0x80 + 0,   // 0x8C // FnMidB
    0 * 0x80 + 0,   // 0x8D // FnStrComp
    0 * 0x80 + 0,   // 0x8E // FnStrComp3
    0 * 0x80 + 0,   // 0x8F // FnStringVar
    0 * 0x80 + 0,   // 0x90 // FnStringStr
    0 * 0x80 + 2,   // 0x91 // FnUBound
    0 * 0x80 + 0,   // 0x92 // For
    0 * 0x80 + 0,   // 0x93 // ForEach
    0 * 0x80 + 2,   // 0x94 // ForEachAs
    0 * 0x80 + 0,   // 0x95 // ForStep
    0 * 0x80 + 4,   // 0x96 // FuncDefn
    0 * 0x80 + 4,   // 0x97 // FuncDefnSave
    0 * 0x80 + 0,   // 0x98 // GetRec
    0 * 0x80 + 2,   // 0x99 // Gosub
    0 * 0x80 + 2,   // 0x9A // Goto
    0 * 0x80 + 0,   // 0x9B // If
    0 * 0x80 + 0,   // 0x9C // IfBlock
    0 * 0x80 + 2,   // 0x9D // TypeOf
    0 * 0x80 + 2,   // 0x9E // IfTypeBlock
    0 * 0x80 + 4,   // 0x9F // Implements
    0 * 0x80 + 0,   // 0xA0 // Input
    0 * 0x80 + 0,   // 0xA1 // InputDone
    0 * 0x80 + 0,   // 0xA2 // InputItem
    0 * 0x80 + 2,   // 0xA3 // Label
    0 * 0x80 + 0,   // 0xA4 // Let
    0 * 0x80 + 2,   // 0xA5 // Line
    1 * 0x80 + 0,   // 0xA6 // LineCont
    0 * 0x80 + 0,   // 0xA7 // LineInput
    0 * 0x80 + 2,   // 0xA8 // LineNum
    0 * 0x80 + 8,   // 0xA9 // LitCy
    0 * 0x80 + 8,   // 0xAA // LitDate
    0 * 0x80 + 0,   // 0xAB // LitDefault
    0 * 0x80 + 2,   // 0xAC // LitDI2
    0 * 0x80 + 4,   // 0xAD // LitDI4
    0 * 0x80 + 2,   // 0xAE // LitHI2
    0 * 0x80 + 4,   // 0xAF // LitHI4
    0 * 0x80 + 0,   // 0xB0 // LitNothing
    0 * 0x80 + 2,   // 0xB1 // LitOI2
    0 * 0x80 + 4,   // 0xB2 // LitOI4
    0 * 0x80 + 4,   // 0xB3 // LitR4
    0 * 0x80 + 8,   // 0xB4 // LitR8
    0 * 0x80 + 0,   // 0xB5 // LitSmallI2
    1 * 0x80 + 0,   // 0xB6 // LitStr
    0 * 0x80 + 0,   // 0xB7 // LitVarSpecial
    0 * 0x80 + 0,   // 0xB8 // Lock
    0 * 0x80 + 0,   // 0xB9 // Loop
    0 * 0x80 + 0,   // 0xBA // LoopUntil
    0 * 0x80 + 0,   // 0xBB // LoopWhile
    0 * 0x80 + 0,   // 0xBC // Lset
    0 * 0x80 + 0,   // 0xBD // Me
    0 * 0x80 + 0,   // 0xBE // MeImplicit
    0 * 0x80 + 8,   // 0xBF // MemRedim
    0 * 0x80 + 8,   // 0xC0 // MemRedimWith
    0 * 0x80 + 8,   // 0xC1 // MemRedimAs
    0 * 0x80 + 8,   // 0xC2 // MemRedimAsWith
    0 * 0x80 + 0,   // 0xC3 // Mid
    0 * 0x80 + 0,   // 0xC4 // MidB
    0 * 0x80 + 0,   // 0xC5 // Name
    0 * 0x80 + 2,   // 0xC6 // New
    0 * 0x80 + 0,   // 0xC7 // Next
    0 * 0x80 + 0,   // 0xC8 // NextVar
    0 * 0x80 + 2,   // 0xC9 // OnError
    1 * 0x80 + 0,   // 0xCA // OnGosub
    1 * 0x80 + 0,   // 0xCB // OnGoto
    0 * 0x80 + 2,   // 0xCC // Open
    0 * 0x80 + 0,   // 0xCD // Option
    0 * 0x80 + 0,   // 0xCE // OptionBase
    0 * 0x80 + 0,   // 0xCF // ParamByval
    0 * 0x80 + 0,   // 0xD0 // ParamOmitted
    0 * 0x80 + 2,   // 0xD1 // ParamNamed
    0 * 0x80 + 0,   // 0xD2 // PrintChan
    0 * 0x80 + 0,   // 0xD3 // PrintComma
    0 * 0x80 + 0,   // 0xD4 // PrintEos
    0 * 0x80 + 0,   // 0xD5 // PrintItemComma
    0 * 0x80 + 0,   // 0xD6 // PrintItemNL
    0 * 0x80 + 0,   // 0xD7 // PrintItemSemi
    0 * 0x80 + 0,   // 0xD8 // PrintNL
    0 * 0x80 + 0,   // 0xD9 // PrintObj
    0 * 0x80 + 0,   // 0xDA // PrintSemi
    0 * 0x80 + 0,   // 0xDB // PrintSpc
    0 * 0x80 + 0,   // 0xDC // PrintTab
    0 * 0x80 + 0,   // 0xDD // PrintTabComma
    0 * 0x80 + 2,   // 0xDE // Pset
    0 * 0x80 + 0,   // 0xDF // PutRec
    1 * 0x80 + 0,   // 0xE0 // QuoteRem
    0 * 0x80 + 8,   // 0xE1 // Redim
    0 * 0x80 + 8,   // 0xE2 // RedimAs
    1 * 0x80 + 0,   // 0xE3 // Reparse
    1 * 0x80 + 0,   // 0xE4 // Rem
    0 * 0x80 + 2,   // 0xE5 // Resume
    0 * 0x80 + 0,   // 0xE6 // Return
    0 * 0x80 + 0,   // 0xE7 // Rset
    0 * 0x80 + 2,   // 0xE8 // Scale
    0 * 0x80 + 0,   // 0xE9 // Seek
    0 * 0x80 + 0,   // 0xEA // SelectCase
    0 * 0x80 + 2,   // 0xEB // SelectIs
    0 * 0x80 + 0,   // 0xEC // SelectType
    0 * 0x80 + 0,   // 0xED // SetStmt
    0 * 0x80 + 6,   // 0xEE // Stack
    0 * 0x80 + 0,   // 0xEF // Stop
    0 * 0x80 + 4,   // 0xF0 // Type
    0 * 0x80 + 0,   // 0xF1 // UnLock
    1 * 0x80 + 0,   // 0xF2 // VarDefn
    0 * 0x80 + 0,   // 0xF3 // Wend
    0 * 0x80 + 0,   // 0xF4 // While
    0 * 0x80 + 0,   // 0xF5 // With
    0 * 0x80 + 0,   // 0xF6 // WriteChan
    0 * 0x80 + 0,   // 0xF7 // ConstFuncExpr
    0 * 0x80 + 2,   // 0xF8 // LbConst
    0 * 0x80 + 0,   // 0xF9 // LbIf
    0 * 0x80 + 0,   // 0xFA // LbElse
    0 * 0x80 + 0,   // 0xFB // LbElseif
    0 * 0x80 + 0,   // 0xFC // LbEndif
    0 * 0x80 + 0,   // 0xFD // LbMark
    0 * 0x80 + 0,   // 0xFE // EndForVariable
    0 * 0x80 + 0,   // 0xFF // StartForVariable
    0 * 0x80 + 0,   // 0x100 // NewRedim
    0 * 0x80 + 0,   // 0x101 // StartWithExpr
    0 * 0x80 + 2,   // 0x102 // SetOrSt
    0 * 0x80 + 0,   // 0x103 // EndEnum
    0 * 0x80 + 0    // 0x104 // Illegal
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

int O98GetOpSkip
(
    BOOL        bLittleEndian,
    LPBYTE      lpabyOp,
    int         nNumOpBytes
)
{
    int         nOpcode;
    WORD        wTemp;

    if (nNumOpBytes < 2)
        return(-1);

    if (bLittleEndian)
        nOpcode = lpabyOp[0] | ((lpabyOp[1] & 0x01) << 8);
    else
        nOpcode = lpabyOp[1] | ((lpabyOp[0] & 0x01) << 8);

    if (nOpcode > O98_OP_MAX)
        nOpcode = O98_OP_MAX;

    if (gabyO98OpSkip[nOpcode] < 0x80)
        return(gabyO98OpSkip[nOpcode]);

    if (nOpcode == O98_OP_VAR_DEFN)
    {
        if ((bLittleEndian && (lpabyOp[1] & 0x40)) ||
            (bLittleEndian == FALSE && (lpabyOp[1] & 0x40) == 0))
            return(6);
        else
            return(4);
    }

    if (nOpcode == O98_OP_QUOTED_COMMENT)
    {
        if (nNumOpBytes < 6)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+4,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(4 + (int)wTemp);
    }
    else
    {
        if (nNumOpBytes < 4)
            return(-1);

        wTemp = SSEndianWORD(lpabyOp+2,bLittleEndian);

        if (wTemp & 1)
            ++wTemp;

        return(2 + (int)wTemp);
    }
}


//********************************************************************
//
// Function:
//  BOOL O97ModuleCRCFast()
//
// Parameters:
//  lpstStream          Ptr to allocated stream to CRC
//  dwTableOffset       Offset of line table
//  dwTableSize         Number of lines
//  dwModuleOffset      Offset of module binary
//  bLittleEndian       TRUE if module is little endian
//  lpabyBuf            Buffer to use
//  nBufSize            Size of buffer
//  lpfnCB              Callback function
//  lpvCBCookie         Callback cookie
//  lpdwCRC             Ptr to DWORD to store CRC of module
//  lpdwCRCByteCount    Ptr to DWORD for CRC byte count
//
// Description:
//  Given a module stream, the function calculates a
//  32-bit CRC on the token values and literal string and numeric
//  token values.
//
//  The function attempts to optimize the calculation by grouping
//  contiguous lines and continuing the CRC on the batch of lines.
//
//  The provided buffer allows the function to optimize the
//  grouping operations.  Thus a larger buffer requires fewer
//  disk accesses.
//
//  The function assumes that the size of each line is a multiple
//  of two.  The function also assumes that each opcode has a value
//  less than 256.
//
//  Either or both of lpdwCRC or lpdwCRCByteCount may be NULL if
//  the corresponding value is not required.
//
//  If lpfnCB is not NULL then the function calls it each time
//  before continuing the CRC on the buffer.  If the callback
//  function returns FALSE, the function returns FALSE.
//
//  The function computes the CRC for both O97 and O98,
//  for Macintosh and Windows.
//
// Returns:
//  TRUE            On success
//  FALSE           On error
//
//********************************************************************

typedef struct tagO97B_CRC_STATE
{
    DWORD               dwCRC;
    DWORD               dwCRCByteCount;
    EO97B_CRC_STATE_T   eState;
    BYTE                abyOp[2];
    WORD                wOpLen;
    int                 nOpcode;
    DWORD               dwCRCN;
    DWORD               dwCRCSkipN;
    DWORD               dwN;
    BYTE                abyRevCRC[8];
} O97B_CRC_STATE_T, FAR *LPO97B_CRC_STATE;

//********************************************************************
//
// Function:
//  int O97CRCBuf()
//
// Parameters:
//  lpstCRC             Ptr to CRC state structure
//  bLittleEndian       TRUE if the module is in little endian format
//  lpabyBuf            Buffer containing bytes to CRC
//  nCount              Number of bytes in buffer
//
// Description:
//  The function continues the Office 97 module CRC on the
//  given buffer.
//
// Returns:
//  int             Number of bytes traversed
//
//********************************************************************

int O97CRCBuf
(
    LPO97B_CRC_STATE    lpstCRC,
    BOOL                bLittleEndian,
    LPBYTE              lpabyBuf,
    int                 nCount
)
{
    int                 i, j;
    O97B_CRC_STATE_T    stCRC;

    stCRC = *lpstCRC;

    i = 0;
    j = 0;
    while (i < nCount && j++ < nCount)
    {
        if (i < 0)
            return(nCount);

        switch (stCRC.eState)
        {
            case eO97B_CRC_STATE_OPCODE_LO:
                if (bLittleEndian)
                    stCRC.abyOp[0] = lpabyBuf[i++];
                else
                    stCRC.abyOp[1] = lpabyBuf[i++];

                stCRC.eState = eO97B_CRC_STATE_OPCODE_HI;
                break;

            case eO97B_CRC_STATE_OPCODE_HI:
                if (bLittleEndian)
                    stCRC.abyOp[1] = lpabyBuf[i++];
                else
                    stCRC.abyOp[0] = lpabyBuf[i++];

                stCRC.nOpcode = stCRC.abyOp[0];

                if (stCRC.nOpcode > O97_OP_MAX)
                    stCRC.nOpcode = O97_OP_MAX;

                if (gabyO97OpSkip[stCRC.nOpcode] < 0x80)
                {
                    // CRC the opcode

                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                    stCRC.dwCRCByteCount += 2;

                    switch (stCRC.nOpcode)
                    {
                        case O97_OP_CURRENCY:
                        case O97_OP_DATE:
                        case O97_OP_WORD_VALUE:
                        case O97_OP_DWORD_VALUE:
                        case O97_OP_HEX_WORD_VALUE:
                        case O97_OP_HEX_DWORD_VALUE:
                        case O97_OP_OCT_WORD_VALUE:
                        case O97_OP_OCT_DWORD_VALUE:
                        case O97_OP_SINGLE_FLOAT_VALUE:
                        case O97_OP_DOUBLE_FLOAT_VALUE:
                            stCRC.dwCRCN = gabyO97OpSkip[stCRC.nOpcode];
                            if (bLittleEndian)
                            {
                                stCRC.dwCRCSkipN = 0;
                                stCRC.eState = eO97B_CRC_STATE_CRC_N;
                            }
                            else
                            {
                                stCRC.dwN = stCRC.dwCRCN;
                                stCRC.eState = eO97B_CRC_STATE_REV_CRC_N;
                            }
                            break;

                        default:
                            // Skip over any operands

                            i += gabyO97OpSkip[stCRC.nOpcode];
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                            break;
                    }
                }
                else
                if (stCRC.nOpcode == O97_OP_VAR_DEFN)
                {
                    // Skip variable definitions

                    if (stCRC.abyOp[1] & 0x40)
                    {
                        // Add space for optional As column offset

                        i += 6;
                    }
                    else
                        i += 4;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                else
                {
                    // There are a variable number of arguments

                    if (stCRC.nOpcode == O97_OP_QUOTED_COMMENT)
                    {
                        // Skip the next two bytes

                        i += 2;
                    }
                    else
                    {
                        switch (stCRC.nOpcode)
                        {
                            case O97_OP_LINE_CONTINUATION:
                            case O97_OP_REM_COMMENT:
                                break;

                            default:
                                // CRC the opcode

                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                                stCRC.dwCRCByteCount += 2;
                                break;
                        }
                    }

                    // Then get the operand length

                    stCRC.eState = eO97B_CRC_STATE_OP_LEN_LO;
                }
                break;

            case eO97B_CRC_STATE_OP_LEN_LO:
                stCRC.wOpLen = lpabyBuf[i++];
                stCRC.eState = eO97B_CRC_STATE_OP_LEN_HI;
                break;

            case eO97B_CRC_STATE_OP_LEN_HI:
                if (bLittleEndian)
                    stCRC.wOpLen |= (WORD)lpabyBuf[i++] << 8;
                else
                    stCRC.wOpLen = (stCRC.wOpLen << 8) |
                        ((WORD)lpabyBuf[i++]);

                // Round up to next even number

                stCRC.dwCRCN = stCRC.wOpLen;
                if ((stCRC.wOpLen & 1) == 0)
                    stCRC.dwCRCSkipN = 0;
                else
                    stCRC.dwCRCSkipN = 1;

                switch (stCRC.nOpcode)
                {
                    case O97_OP_LIT_STR:
                        if (stCRC.dwCRCN == 0)
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        else
                            stCRC.eState = eO97B_CRC_STATE_CRC_N;
                        break;

                    default:
                        i += (int)(stCRC.dwCRCN + stCRC.dwCRCSkipN);
                        stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        break;
                }

                break;

            case eO97B_CRC_STATE_CRC_N:
                if (DWORD(nCount - i) >= stCRC.dwCRCN)
                    stCRC.dwN = stCRC.dwCRCN;
                else
                    stCRC.dwN = nCount - i;

                stCRC.dwCRC = CRC32Compute(stCRC.dwN,
                                           lpabyBuf+i,
                                           stCRC.dwCRC);
                stCRC.dwCRCByteCount += stCRC.dwN;
                i += (int)stCRC.dwN;
                stCRC.dwCRCN -= stCRC.dwN;

                if (stCRC.dwCRCN == 0)
                {
                    i += (int)stCRC.dwCRCSkipN;
                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }

                break;

            case eO97B_CRC_STATE_REV_CRC_N:
                while (i < nCount)
                {
                    stCRC.abyRevCRC[--stCRC.dwN] = lpabyBuf[i++];
                    if (stCRC.dwN == 0)
                        break;
                }

                if (stCRC.dwN == 0)
                {
                    stCRC.dwCRC = CRC32Compute(stCRC.dwCRCN,
                                               stCRC.abyRevCRC,
                                               stCRC.dwCRC);

                    stCRC.dwCRCByteCount += stCRC.dwCRCN;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                break;

            default:
                // This is an error condition
                return(nCount);
        }
    }

    *lpstCRC = stCRC;

    return(i);
}

//********************************************************************
//
// Function:
//  int O98CRCBuf()
//
// Parameters:
//  lpstCRC             Ptr to CRC state structure
//  bLittleEndian       TRUE if the module is in little endian format
//  lpabyBuf            Buffer containing bytes to CRC
//  nCount              Number of bytes in buffer
//
// Description:
//  The function continues the Office 98 module CRC on the
//  given buffer.
//
// Returns:
//  int             Number of bytes traversed
//
//********************************************************************

int O98CRCBuf
(
    LPO97B_CRC_STATE    lpstCRC,
    BOOL                bLittleEndian,
    LPBYTE              lpabyBuf,
    int                 nCount
)
{
    int                 i, j;
    O97B_CRC_STATE_T    stCRC;

    stCRC = *lpstCRC;

    i = 0;
    j = 0;
    while (i < nCount && j++ < nCount)
    {
        if (i < 0)
            return(nCount);

        switch (stCRC.eState)
        {
            case eO97B_CRC_STATE_OPCODE_LO:
                if (bLittleEndian)
                    stCRC.abyOp[0] = lpabyBuf[i++];
                else
                    stCRC.abyOp[1] = lpabyBuf[i++];

                stCRC.eState = eO97B_CRC_STATE_OPCODE_HI;
                break;

            case eO97B_CRC_STATE_OPCODE_HI:
                if (bLittleEndian)
                    stCRC.abyOp[1] = lpabyBuf[i++];
                else
                    stCRC.abyOp[0] = lpabyBuf[i++];

                stCRC.nOpcode = stCRC.abyOp[0] | ((stCRC.abyOp[1] & 0x01) << 8);

                if (stCRC.nOpcode > O98_OP_MAX)
                    stCRC.nOpcode = O98_OP_MAX;

                if (gabyO98OpSkip[stCRC.nOpcode] < 0x80)
                {
                    // CRC the opcode

                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                    CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                    stCRC.dwCRCByteCount += 2;

                    switch (stCRC.nOpcode)
                    {
                        case O98_OP_CURRENCY:
                        case O98_OP_DATE:
                        case O98_OP_WORD_VALUE:
                        case O98_OP_DWORD_VALUE:
                        case O98_OP_HEX_WORD_VALUE:
                        case O98_OP_HEX_DWORD_VALUE:
                        case O98_OP_OCT_WORD_VALUE:
                        case O98_OP_OCT_DWORD_VALUE:
                        case O98_OP_SINGLE_FLOAT_VALUE:
                        case O98_OP_DOUBLE_FLOAT_VALUE:
                            stCRC.dwCRCN = gabyO98OpSkip[stCRC.nOpcode];
                            if (bLittleEndian)
                            {
                                stCRC.dwCRCSkipN = 0;
                                stCRC.eState = eO97B_CRC_STATE_CRC_N;
                            }
                            else
                            {
                                stCRC.dwN = stCRC.dwCRCN;
                                stCRC.eState = eO97B_CRC_STATE_REV_CRC_N;
                            }
                            break;

                        default:
                            // Skip over any operands

                            i += gabyO98OpSkip[stCRC.nOpcode];
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                            break;
                    }
                }
                else
                if (stCRC.nOpcode == O98_OP_VAR_DEFN)
                {
                    // Skip variable definitions

                    if (stCRC.abyOp[1] & 0x40)
                    {
                        // Add space for optional As column offset

                        i += 6;
                    }
                    else
                        i += 4;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                else
                {
                    // There are a variable number of arguments

                    if (stCRC.nOpcode == O98_OP_QUOTED_COMMENT)
                    {
                        // Skip the next two bytes

                        i += 2;
                    }
                    else
                    {
                        switch (stCRC.nOpcode)
                        {
                            case O98_OP_LINE_CONTINUATION:
                            case O98_OP_REM_COMMENT:
                                break;

                            default:
                                // CRC the opcode

                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[0]);
                                CRC32Continue(stCRC.dwCRC,stCRC.abyOp[1]);
                                stCRC.dwCRCByteCount += 2;
                                break;
                        }
                    }

                    // Then get the operand length

                    stCRC.eState = eO97B_CRC_STATE_OP_LEN_LO;
                }
                break;

            case eO97B_CRC_STATE_OP_LEN_LO:
                stCRC.wOpLen = lpabyBuf[i++];
                stCRC.eState = eO97B_CRC_STATE_OP_LEN_HI;
                break;

            case eO97B_CRC_STATE_OP_LEN_HI:
                if (bLittleEndian)
                    stCRC.wOpLen |= (WORD)lpabyBuf[i++] << 8;
                else
                    stCRC.wOpLen = (stCRC.wOpLen << 8) |
                        ((WORD)lpabyBuf[i++]);

                // Round up to next even number

                stCRC.dwCRCN = stCRC.wOpLen;
                if ((stCRC.wOpLen & 1) == 0)
                    stCRC.dwCRCSkipN = 0;
                else
                    stCRC.dwCRCSkipN = 1;

                switch (stCRC.nOpcode)
                {
                    case O98_OP_LIT_STR:
                        if (stCRC.dwCRCN == 0)
                            stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        else
                            stCRC.eState = eO97B_CRC_STATE_CRC_N;
                        break;

                    default:
                        i += (int)(stCRC.dwCRCN + stCRC.dwCRCSkipN);
                        stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                        break;
                }

                break;

            case eO97B_CRC_STATE_CRC_N:
                if (DWORD(nCount - i) >= stCRC.dwCRCN)
                    stCRC.dwN = stCRC.dwCRCN;
                else
                    stCRC.dwN = nCount - i;

                stCRC.dwCRC = CRC32Compute(stCRC.dwN,
                                           lpabyBuf+i,
                                           stCRC.dwCRC);
                stCRC.dwCRCByteCount += stCRC.dwN;
                i += (int)stCRC.dwN;
                stCRC.dwCRCN -= stCRC.dwN;

                if (stCRC.dwCRCN == 0)
                {
                    i += (int)stCRC.dwCRCSkipN;
                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }

                break;

            case eO97B_CRC_STATE_REV_CRC_N:
                while (i < nCount)
                {
                    stCRC.abyRevCRC[--stCRC.dwN] = lpabyBuf[i++];
                    if (stCRC.dwN == 0)
                        break;
                }

                if (stCRC.dwN == 0)
                {
                    stCRC.dwCRC = CRC32Compute(stCRC.dwCRCN,
                                               stCRC.abyRevCRC,
                                               stCRC.dwCRC);

                    stCRC.dwCRCByteCount += stCRC.dwCRCN;

                    stCRC.eState = eO97B_CRC_STATE_OPCODE_LO;
                }
                break;

            default:
                // This is an error condition
                return(nCount);
        }
    }

    *lpstCRC = stCRC;

    return(i);
}

#define O97_CRC_MAX_GROUP_COUNT     128

BOOL O97ModuleCRCFast
(
    LPSS_STREAM             lpstStream,
    DWORD                   dwTableOffset,
    DWORD                   dwTableSize,
    DWORD                   dwModuleOffset,
    LPBYTE                  lpabyBuf,
    int                     nBufSize,
    LPFNO97_CRC_FAST_CB     lpfnCB,
    LPVOID                  lpvCBCookie,
    LPDWORD                 lpdwCRC,
    LPDWORD                 lpdwCRCByteCount
)
{
    LPO97_LINE_TABLE_LINE   lpstLine = NULL;
    int                     nMaxBufLines;
    WORD                    wSize;

    int                     i;
    int                     nNumGroups;
    DWORD                   adwOffsets[O97_CRC_MAX_GROUP_COUNT];
    DWORD                   adwSizes[O97_CRC_MAX_GROUP_COUNT];

    DWORD                   dwLine;
    DWORD                   dwCount;
    DWORD                   dwOffset;
    DWORD                   dwStartOffset = 0;
    DWORD                   dwEndOffset;
    int                     nStartI;

    DWORD                   dwGroupOffset;
    DWORD                   dwGroupSize;

    DWORD                   dwBytesRead;

    O97B_CRC_STATE_T        stCRC;
    EO97B_CRC_STATE_T       eState;

    int                     nVersion;
    BOOL                    bLittleEndian;

    // Get the version and endian state

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLittleEndian) == FALSE)
        return(FALSE);

    nMaxBufLines = nBufSize / sizeof(O97_LINE_TABLE_LINE_T);
    if (nMaxBufLines == 0 || lpabyBuf == NULL)
        return(FALSE);

    CRC32Init(stCRC.dwCRC);
    stCRC.dwCRCByteCount = 0;
    dwLine = 0;
    while (dwLine < dwTableSize)
    {
        /////////////////////////////////////////////////////////
        // Get up to 128 groups of contiguous lines

        nNumGroups = 0;
        dwEndOffset = 0;
        dwCount = 0;
        while (dwLine < dwTableSize)
        {
            // Need to rebuffer?

            if (dwCount == 0)
            {
                // Read as many line entries as possible

                if (dwLine + nMaxBufLines > dwTableSize)
                    dwCount = dwTableSize - dwLine;
                else
                    dwCount = nMaxBufLines;

                // Read the line entries

                if (SSSeekRead(lpstStream,
                               dwTableOffset +
                                   dwLine * sizeof(O97_LINE_TABLE_LINE_T),
                               lpabyBuf,
                               dwCount * sizeof(O97_LINE_TABLE_LINE_T),
                               &dwBytesRead) != SS_STATUS_OK ||
                    dwBytesRead != dwCount * sizeof(O97_LINE_TABLE_LINE_T))
                    return(FALSE);

                lpstLine = (LPO97_LINE_TABLE_LINE)lpabyBuf;
            }

            wSize = SSEndianWORD((LPBYTE)&lpstLine->wSize,
                                 bLittleEndian);

            // Ignore blank lines

            if (wSize != 0)
            {
                dwOffset = SSEndianDWORD((LPBYTE)&lpstLine->dwOffset,
                                         bLittleEndian);
                if (dwEndOffset == 0)
                {
                    // This is the first line in the group

                    dwStartOffset = dwOffset;
                    dwEndOffset = dwOffset + wSize;
                }
                else
                if (dwEndOffset == dwOffset)
                {
                    // The line is contiguous with the previous line

                    dwEndOffset += wSize;
                }
                else
                {
                    // This line is not contiguous with the previous line

                    // Store the offset and size of the group

                    adwOffsets[nNumGroups] = dwStartOffset;
                    adwSizes[nNumGroups] = dwEndOffset - dwStartOffset;

                    dwEndOffset = 0;

                    // Have the maximum number of groups been reached

                    if (++nNumGroups == O97_CRC_MAX_GROUP_COUNT)
                        break;

                    // Start a new group

                    dwStartOffset = dwOffset;
                    dwEndOffset = dwOffset + wSize;
                }
            }

            // Move to the next line

            ++dwLine;
            --dwCount;
            ++lpstLine;
        }

        // Store the hanging group

        if (dwEndOffset != 0)
        {
            adwOffsets[nNumGroups] = dwStartOffset;
            adwSizes[nNumGroups] = dwEndOffset - dwStartOffset;
            ++nNumGroups;
        }

        /////////////////////////////////////////////////////////
        // CRC the groups of contiguous lines

        i = 0;
        while (i < nNumGroups)
        {
            // Get a set of groups that fit within the buffer

            nStartI = i;
            dwOffset = adwOffsets[i];
            dwEndOffset = dwOffset + adwSizes[i];
            dwCount = adwSizes[i];
            ++i;
            while (i < nNumGroups)
            {
                dwGroupOffset = adwOffsets[i];
                dwGroupSize = adwSizes[i];
                if (dwGroupOffset < dwOffset)
                {
                    if (dwGroupOffset + dwGroupSize > dwEndOffset)
                    {
                        // This is weird

                        break;
                    }

                    // Will the buffer limit be exceeded?

                    if ((int)(dwEndOffset - dwGroupOffset) > nBufSize)
                        break;

                    // Set the start offset to this line

                    dwOffset = dwGroupOffset;
                }
                else
                if (dwGroupOffset + dwGroupSize > dwEndOffset)
                {
                    // Will adding this group exceed the buffer size?

                    if (dwGroupOffset + dwGroupSize - dwOffset > DWORD(nBufSize))
                        break;

                    dwEndOffset = dwGroupOffset + dwGroupSize;
                }

                ++i;
            }

            if (nStartI + 1 == i)
            {
                // There is only one group

                stCRC.eState = eState = eO97B_CRC_STATE_OPCODE_LO;
                dwOffset += dwModuleOffset;
                dwEndOffset += dwModuleOffset;
                dwCount = nBufSize;
                while (dwOffset < dwEndOffset)
                {
                    if (dwEndOffset - dwOffset < dwCount)
                        dwCount = dwEndOffset - dwOffset;

                    // Read a chunk

                    if (SSSeekRead(lpstStream,
                                   dwOffset,
                                   lpabyBuf,
                                   dwCount,
                                   &dwBytesRead) != SS_STATUS_OK ||
                        dwBytesRead != dwCount)
                        return(FALSE);

                    // Call the callback if necessary

                    if (lpfnCB != NULL)
                    {
                        if (lpfnCB(lpvCBCookie,
                                   &eState,
                                   nVersion,
                                   bLittleEndian,
                                   lpabyBuf,
                                   (int)dwCount) == FALSE)
                            return(FALSE);
                    }

                    // CRC the chunk and increment the offset

                    if (nVersion == O97_VERSION_O97)
                    {
                        dwOffset += O97CRCBuf(&stCRC,
                                              bLittleEndian,
                                              lpabyBuf,
                                              (int)dwCount);
                    }
                    else
                    {
                        dwOffset += O98CRCBuf(&stCRC,
                                              bLittleEndian,
                                              lpabyBuf,
                                              (int)dwCount);
                    }
                }
            }
            else
            {
                // There are multiple groups.  All groups
                //  fit within the buffer

                dwCount = dwEndOffset - dwOffset;

                // Make sure we are legal

                if (dwCount > DWORD(nBufSize))
                    return(FALSE);

                // Read a chunk

                if (SSSeekRead(lpstStream,
                               dwModuleOffset + dwOffset,
                               lpabyBuf,
                               dwCount,
                               &dwBytesRead) != SS_STATUS_OK ||
                    dwBytesRead != dwCount)
                    return(FALSE);

                // CRC each group

                while (nStartI < i)
                {
                    stCRC.eState = eState = eO97B_CRC_STATE_OPCODE_LO;

                    // Verify that the chunk is within the buffer

                    if (adwOffsets[nStartI] - dwOffset >= dwCount)
                        return(FALSE);

                    if ((dwCount - (adwOffsets[nStartI] - dwOffset)) <
                        adwSizes[nStartI])
                        return(FALSE);

                    // Call the callback if necessary

                    if (lpfnCB != NULL)
                    {
                        if (lpfnCB(lpvCBCookie,
                                   &eState,
                                   nVersion,
                                   bLittleEndian,
                                   lpabyBuf + adwOffsets[nStartI] - dwOffset,
                                   (int)adwSizes[nStartI]) == FALSE)
                            return(FALSE);
                    }

                    // CRC the group

                    if (nVersion == O97_VERSION_O97)
                    {
                        O97CRCBuf(&stCRC,
                                  bLittleEndian,
                                  lpabyBuf + adwOffsets[nStartI] - dwOffset,
                                  (int)adwSizes[nStartI]);
                    }
                    else
                    {
                        O98CRCBuf(&stCRC,
                                  bLittleEndian,
                                  lpabyBuf + adwOffsets[nStartI] - dwOffset,
                                  (int)adwSizes[nStartI]);
                    }

                    ++nStartI;
                }
            }
        }
    }

    if (lpdwCRC != NULL)
        *lpdwCRC = stCRC.dwCRC;

    if (lpdwCRCByteCount != NULL)
        *lpdwCRCByteCount = stCRC.dwCRCByteCount;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModAccInit()
//
// Parameters:
//  lpstMod                 Ptr to O97_MOD_ACC_T structure
//  lpstStream              Ptr to the module stream
//  lpabyBuf                Ptr to buffering buffer to use
//  dwBufSize               Size of buffering buffer
//
// Description:
//  The function initializes the O97_MOD_ACC_T structure
//  for buffered access to the line index and data.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL O97ModAccInit
(
    LPO97_MOD_ACC           lpstMod,
    LPSS_STREAM             lpstStream,
    LPBYTE                  lpabyBuf,
    DWORD                   dwBufSize
)
{
    lpstMod->lpstStream = lpstStream;

    if (O97GetModuleVersionEndian(lpstStream,
                                  &lpstMod->nVersion,
                                  &lpstMod->bLittleEndian) == FALSE)
        return(FALSE);

    if (O97GetLineBinaryOffsets(lpstStream,
                                &lpstMod->dwTableOffset,
                                &lpstMod->dwTableSize,
                                &lpstMod->dwDataOffset,
                                NULL) == FALSE)
        return(FALSE);

    lpstMod->dwDataSize = SSStreamLen(lpstStream) - lpstMod->dwDataOffset;

    // Assign line buffer

    lpstMod->dwMaxLineEntries = (dwBufSize / 2) /
        sizeof(O97_LINE_TABLE_LINE_T);
    lpstMod->lpastLines = (LPO97_LINE_TABLE_LINE)lpabyBuf;

    lpstMod->dwLineStart = 0;
    lpstMod->dwLineEnd = 0;
    lpstMod->dwLineCenter = 0;

    // Assign data buffer

    lpstMod->dwMaxDataSize = dwBufSize -
        lpstMod->dwMaxLineEntries * sizeof(O97_LINE_TABLE_LINE_T);
    lpstMod->lpabyData = (LPBYTE)
        (lpstMod->lpastLines + lpstMod->dwMaxLineEntries);

    lpstMod->dwDataStart = 0;
    lpstMod->dwDataEnd = 0;
    lpstMod->dwDataCenter = 0;

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModAccGetLineInfo()
//
// Parameters:
//  lpstMod                 Ptr to O97_MOD_ACC_T structure
//  dwLine                  Line to get
//  lpdwOffset              Ptr to DWORD for line offset
//  lpwSize                 Ptr to WORD for size offset
//
// Description:
//  The function returns in *lpdwOffset and *lpwSize the offset
//  and size of the given line.
//
//  If the line is outside of the buffered lines, then the buffer
//  is refilled starting with the given line.  If dwLineCenter
//  of lpstMod is non-zero, then dwLineCenter becomes the center
//  line, unless the desired line lies outside, in which case
//  the desired line becomes the starting line.  In this last
//  case, centering is left on for the next read.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//********************************************************************

BOOL O97ModAccGetLineInfo
(
    LPO97_MOD_ACC           lpstMod,
    DWORD                   dwLine,
    LPDWORD                 lpdwOffset,
    LPWORD                  lpwSize
)
{
    DWORD                   dwCount;
    DWORD                   dwBytesRead;
    LPO97_LINE_TABLE_LINE   lpstLine;

    if (dwLine < lpstMod->dwLineStart || lpstMod->dwLineEnd <= dwLine)
    {
        if (dwLine >= lpstMod->dwTableSize)
            return(FALSE);

        // Determine the starting and ending lines to read

        if (lpstMod->dwLineCenter == 0)
            lpstMod->dwLineStart = dwLine;
        else
        {
            if (lpstMod->dwLineCenter <= lpstMod->dwMaxLineEntries / 2)
                lpstMod->dwLineStart = 0;
            else
                lpstMod->dwLineStart = lpstMod->dwLineCenter -
                    lpstMod->dwMaxLineEntries / 2;

            if (dwLine < lpstMod->dwLineStart ||
                lpstMod->dwLineStart + lpstMod->dwMaxLineEntries <= dwLine)
            {
                // Just read from the starting line

                lpstMod->dwLineStart = dwLine;
            }
            else
            {
                // Turn off centering for the next read

                lpstMod->dwLineCenter = 0;
            }
        }

        lpstMod->dwLineEnd = lpstMod->dwLineStart + lpstMod->dwMaxLineEntries;
        if (lpstMod->dwLineEnd > lpstMod->dwTableSize)
            lpstMod->dwLineEnd = lpstMod->dwTableSize;

        // Read the lines

        dwCount = (lpstMod->dwLineEnd - lpstMod->dwLineStart) *
            sizeof(O97_LINE_TABLE_LINE_T);

        if (SSSeekRead(lpstMod->lpstStream,
                       lpstMod->dwTableOffset +
                           lpstMod->dwLineStart * sizeof(O97_LINE_TABLE_LINE_T),
                       lpstMod->lpastLines,
                       dwCount,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwCount)
            return(FALSE);
    }

    lpstLine = lpstMod->lpastLines + (dwLine - lpstMod->dwLineStart);
    *lpwSize = SSEndianWORD((LPBYTE)&lpstLine->wSize,
                            lpstMod->bLittleEndian);
    *lpdwOffset = SSEndianDWORD((LPBYTE)&lpstLine->dwOffset,
                                lpstMod->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  LPBYTE O97ModAccGetDataPtr()
//
// Parameters:
//  lpstMod                 Ptr to O97_MOD_ACC_T structure
//  wOffset                 Offset of data to get
//  wSize                   Size of data needed
//  lpwSize                 Ptr to WORD for size offset
//
// Description:
//  The function returns in *lpwSize the size of the data at
//  the returned ptr.
//
//  If the data is outside of the buffered data, then the buffer
//  is refilled starting with the given data.  If dwDataCenter
//  of lpstMod is non-zero, then dwDataCenter becomes the center
//  data, unless the desired data lies outside, in which case
//  the desired data becomes the starting data.  In this last
//  case, centering is left on for the next read.
//
// Returns:
//  LPBYTE          Ptr to buffer containing data
//  NULL            On error
//
//********************************************************************

LPBYTE O97ModAccGetDataPtr
(
    LPO97_MOD_ACC           lpstMod,
    DWORD                   dwOffset,
    WORD                    wSize,
    LPWORD                  lpwPtrSize
)
{
    DWORD                   dwCount;
    DWORD                   dwBytesRead;

    // Verify that the offset is within bounds

    if (dwOffset >= lpstMod->dwDataSize)
        return(NULL);

    if (dwOffset < lpstMod->dwDataStart ||
        lpstMod->dwDataEnd < ((DWORD)dwOffset + wSize))
    {
        // Determine the starting and ending offsets

        if (lpstMod->dwDataCenter == 0)
            lpstMod->dwDataStart = dwOffset;
        else
        {
            if (lpstMod->dwDataCenter <= lpstMod->dwMaxDataSize / 2)
                lpstMod->dwDataStart = 0;
            else
                lpstMod->dwDataStart = lpstMod->dwDataCenter -
                    lpstMod->dwMaxDataSize / 2;

            if (dwOffset < lpstMod->dwDataStart ||
                lpstMod->dwDataStart + lpstMod->dwMaxDataSize <
                ((DWORD)dwOffset + wSize))
            {
                // Just read from the starting offset

                lpstMod->dwDataStart = dwOffset;
            }
            else
            {
                // Turn off centering for the next read

                lpstMod->dwDataCenter = 0;
            }
        }

        lpstMod->dwDataEnd = lpstMod->dwDataStart + lpstMod->dwMaxDataSize;
        if (lpstMod->dwDataEnd > lpstMod->dwDataSize)
            lpstMod->dwDataEnd = lpstMod->dwDataSize;

        // Read the data

        dwCount = lpstMod->dwDataEnd - lpstMod->dwDataStart;

        if (SSSeekRead(lpstMod->lpstStream,
                       lpstMod->dwDataOffset + lpstMod->dwDataStart,
                       lpstMod->lpabyData,
                       dwCount,
                       &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != dwCount)
            return(NULL);
    }

    if (lpstMod->dwDataStart + wSize <= lpstMod->dwDataEnd)
        *lpwPtrSize = wSize;
    else
        *lpwPtrSize = (WORD)(lpstMod->dwDataEnd - lpstMod->dwDataStart);

    // Return the pointer to the data

    return(lpstMod->lpabyData + (dwOffset - lpstMod->dwDataStart));
}


//*************************************************************************
//
// BOOL O97ModAccGetOpcode()
//
// Parameters:
//  lpstMod             Ptr to module access structure
//  dwOffset            Offset of opcode
//  lpabyOpcode         Ptr to two-byte buffer for opcode
//
// Description:
//  The function reads the two bytes from the given offset of the
//  given module.
//
//  The opcode is properly endianized and converted to an O97 opcode.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//*************************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyModAccXOpO98toO97[O98_OP_MAX+1] =
{
    0x00,       // 0x00 // Imp
    0x01,       // 0x01 // Eqv
    0x02,       // 0x02 // Xor
    0x03,       // 0x03 // Or
    0x04,       // 0x04 // And
    0x05,       // 0x05 // Eq
    0x06,       // 0x06 // Ne
    0x07,       // 0x07 // Le
    0x08,       // 0x08 // Ge
    0x09,       // 0x09 // Lt
    0x0A,       // 0x0A // Gt
    0x0B,       // 0x0B // Add
    0x0C,       // 0x0C // Sub
    0x0D,       // 0x0D // Mod
    0x0E,       // 0x0E // IDv
    0x0F,       // 0x0F // Mul
    0x10,       // 0x10 // Div
    0x11,       // 0x11 // Concat
    0x12,       // 0x12 // Like
    0x13,       // 0x13 // Pwr
    0x14,       // 0x14 // Is
    0x15,       // 0x15 // Not
    0x16,       // 0x16 // UMi
    0x17,       // 0x17 // FnAbs
    0x18,       // 0x18 // FnFix
    0x19,       // 0x19 // FnInt
    0x1A,       // 0x1A // FnSgn
    0x1B,       // 0x1B // FnLen
    0x1C,       // 0x1C // FnLenB
    0x1D,       // 0x1D // Paren
    0x1E,       // 0x1E // Sharp
    0x1F,       // 0x1F // LdLHS
    0x20,       // 0x20 // Ld
    0x21,       // 0x21 // MemLd
    0x22,       // 0x22 // DictLd
    0x23,       // 0x23 // IndexLd
    0x24,       // 0x24 // ArgsLd
    0x25,       // 0x25 // ArgsMemLd
    0x26,       // 0x26 // ArgsDictLd
    0x27,       // 0x27 // St
    0x28,       // 0x28 // MemSt
    0x29,       // 0x29 // DictSt
    0x2A,       // 0x2A // IndexSt
    0x2B,       // 0x2B // ArgsSt
    0x2C,       // 0x2C // ArgsMemSt
    0x2D,       // 0x2D // ArgsDictSt
    0x2E,       // 0x2E // Set
    0x2F,       // 0x2F // MemSet
    0x30,       // 0x30 // DictSet
    0x31,       // 0x31 // IndexSet
    0x32,       // 0x32 // ArgsSet
    0x33,       // 0x33 // ArgsMemSet
    0x34,       // 0x34 // ArgsDictSet
    0x35,       // 0x35 // MemLdWith
    0x36,       // 0x36 // DictLdWith
    0x37,       // 0x37 // ArgsMemLdWith
    0x38,       // 0x38 // ArgsDictLdWith
    0x39,       // 0x39 // MemStWith
    0x3A,       // 0x3A // DictStWith
    0x3B,       // 0x3B // ArgsMemStWith
    0x3C,       // 0x3C // ArgsDictStWith
    0x3D,       // 0x3D // MemSetWith
    0x3E,       // 0x3E // DictSetWith
    0x3F,       // 0x3F // ArgsMemSetWith
    0x40,       // 0x40 // ArgsDictSetWith
    0x41,       // 0x41 // ArgsCall
    0x42,       // 0x42 // ArgsMemCall
    0x43,       // 0x43 // ArgsMemCallWith
    0x44,       // 0x44 // ArgsArray
    O97_OP_MAX, // 0x45 // *Assert
    0x45,       // 0x46 // Bos
    0x46,       // 0x47 // BosImplicit
    0x47,       // 0x48 // Bol
    0xA4,       // 0x49 // *LdAddressOf
    0xA4,       // 0x4A // *MemAddressOf
    0x48,       // 0x4B // Case
    0x49,       // 0x4C // CaseTo
    0x4A,       // 0x4D // CaseGt
    0x4B,       // 0x4E // CaseLt
    0x4C,       // 0x4F // CaseGe
    0x4D,       // 0x50 // CaseLe
    0x4E,       // 0x51 // CaseNe
    0x4F,       // 0x52 // CaseEq
    0x50,       // 0x53 // CaseElse
    0x51,       // 0x54 // CaseDone
    0x52,       // 0x55 // Circle
    0x53,       // 0x56 // Close
    0x54,       // 0x57 // CloseAll
    0x55,       // 0x58 // Coerce
    0x56,       // 0x59 // CoerceVar
    0x57,       // 0x5A // Context
    0x58,       // 0x5B // Debug
    0x59,       // 0x5C // Deftype
    0x5A,       // 0x5D // Dim
    0x5B,       // 0x5E // DimImplicit
    0x5C,       // 0x5F // Do
    0x5D,       // 0x60 // DoEvents
    0x5E,       // 0x61 // DoUntil
    0x5F,       // 0x62 // DoWhile
    0x60,       // 0x63 // Else
    0x61,       // 0x64 // ElseBlock
    0x62,       // 0x65 // ElseIfBlock
    0x63,       // 0x66 // ElseIfTypeBlock
    0x64,       // 0x67 // End
    0x65,       // 0x68 // EndContext
    0x66,       // 0x69 // EndFunc
    0x67,       // 0x6A // EndIf
    0x68,       // 0x6B // EndIfBlock
    0x69,       // 0x6C // EndImmediate
    0x6A,       // 0x6D // EndProp
    0x6B,       // 0x6E // EndSelect
    0x6C,       // 0x6F // EndSub
    0x6D,       // 0x70 // EndType
    0x6E,       // 0x71 // EndWith
    0x6F,       // 0x72 // Erase
    0x70,       // 0x73 // Error
    0xA5,       // 0x74 // *EventDecl
    0xA5,       // 0x75 // *RaiseEvent
    0xA5,       // 0x76 // *ArgsMemRaiseEvent
    0xA5,       // 0x77 // *ArgsMemRaiseEventWith
    0x71,       // 0x78 // ExitDo
    0x72,       // 0x79 // ExitFor
    0x73,       // 0x7A // ExitFunc
    0x74,       // 0x7B // ExitProp
    0x75,       // 0x7C // ExitSub
    0x76,       // 0x7D // FnCurDir
    0x77,       // 0x7E // FnDir
    0x78,       // 0x7F // Empty0
    0x79,       // 0x80 // Empty1
    0x7A,       // 0x81 // FnError
    0x7B,       // 0x82 // FnFormat
    0x7C,       // 0x83 // FnFreeFile
    0x7D,       // 0x84 // FnInStr
    0x7E,       // 0x85 // FnInStr3
    0x7F,       // 0x86 // FnInStr4
    0x80,       // 0x87 // FnInStrB
    0x81,       // 0x88 // FnInStrB3
    0x82,       // 0x89 // FnInStrB4
    0x83,       // 0x8A // FnLBound
    0x84,       // 0x8B // FnMid
    0x85,       // 0x8C // FnMidB
    0x86,       // 0x8D // FnStrComp
    0x87,       // 0x8E // FnStrComp3
    0x88,       // 0x8F // FnStringVar
    0x89,       // 0x90 // FnStringStr
    0x8A,       // 0x91 // FnUBound
    0x8B,       // 0x92 // For
    0x8C,       // 0x93 // ForEach
    0x8D,       // 0x94 // ForEachAs
    0x8E,       // 0x95 // ForStep
    0x8F,       // 0x96 // FuncDefn
    0x90,       // 0x97 // FuncDefnSave
    0x91,       // 0x98 // GetRec
    0x92,       // 0x99 // Gosub
    0x93,       // 0x9A // Goto
    0x94,       // 0x9B // If
    0x95,       // 0x9C // IfBlock
    0x96,       // 0x9D // TypeOf
    0x97,       // 0x9E // IfTypeBlock
    0xA4,       // 0x9F // *Implements
    0x98,       // 0xA0 // Input
    0x99,       // 0xA1 // InputDone
    0x9A,       // 0xA2 // InputItem
    0x9B,       // 0xA3 // Label
    0x9C,       // 0xA4 // Let
    0x9D,       // 0xA5 // Line
    0x9E,       // 0xA6 // LineCont
    0x9F,       // 0xA7 // LineInput
    0xA0,       // 0xA8 // LineNum
    0xA1,       // 0xA9 // LitCy
    0xA2,       // 0xAA // LitDate
    0xA3,       // 0xAB // LitDefault
    0xA4,       // 0xAC // LitDI2
    0xA5,       // 0xAD // LitDI4
    0xA6,       // 0xAE // LitHI2
    0xA7,       // 0xAF // LitHI4
    0xA8,       // 0xB0 // LitNothing
    0xA9,       // 0xB1 // LitOI2
    0xAA,       // 0xB2 // LitOI4
    0xAB,       // 0xB3 // LitR4
    0xAC,       // 0xB4 // LitR8
    0xAD,       // 0xB5 // LitSmallI2
    0xAE,       // 0xB6 // LitStr
    0xAF,       // 0xB7 // LitVarSpecial
    0xB0,       // 0xB8 // Lock
    0xB1,       // 0xB9 // Loop
    0xB2,       // 0xBA // LoopUntil
    0xB3,       // 0xBB // LoopWhile
    0xB4,       // 0xBC // Lset
    0xB5,       // 0xBD // Me
    0xB6,       // 0xBE // MeImplicit
    0xB7,       // 0xBF // MemRedim
    0xB8,       // 0xC0 // MemRedimWith
    0xB9,       // 0xC1 // MemRedimAs
    0xBA,       // 0xC2 // MemRedimAsWith
    0xBB,       // 0xC3 // Mid
    0xBC,       // 0xC4 // MidB
    0xBD,       // 0xC5 // Name
    0xBE,       // 0xC6 // New
    0xBF,       // 0xC7 // Next
    0xC0,       // 0xC8 // NextVar
    0xC1,       // 0xC9 // OnError
    0xC2,       // 0xCA // OnGosub
    0xC3,       // 0xCB // OnGoto
    0xC4,       // 0xCC // Open
    0xC5,       // 0xCD // Option
    0xC6,       // 0xCE // OptionBase
    0xC7,       // 0xCF // ParamByval
    0xC8,       // 0xD0 // ParamOmitted
    0xC9,       // 0xD1 // ParamNamed
    0xCA,       // 0xD2 // PrintChan
    0xCB,       // 0xD3 // PrintComma
    0xCC,       // 0xD4 // PrintEos
    0xCD,       // 0xD5 // PrintItemComma
    0xCE,       // 0xD6 // PrintItemNL
    0xCF,       // 0xD7 // PrintItemSemi
    0xD0,       // 0xD8 // PrintNL
    0xD1,       // 0xD9 // PrintObj
    0xD2,       // 0xDA // PrintSemi
    0xD3,       // 0xDB // PrintSpc
    0xD4,       // 0xDC // PrintTab
    0xD5,       // 0xDD // PrintTabComma
    0xD6,       // 0xDE // Pset
    0xD7,       // 0xDF // PutRec
    0xD8,       // 0xE0 // QuoteRem
    0xD9,       // 0xE1 // Redim
    0xDA,       // 0xE2 // RedimAs
    0xDB,       // 0xE3 // Reparse
    0xDC,       // 0xE4 // Rem
    0xDD,       // 0xE5 // Resume
    0xDE,       // 0xE6 // Return
    0xDF,       // 0xE7 // Rset
    0xE0,       // 0xE8 // Scale
    0xE1,       // 0xE9 // Seek
    0xE2,       // 0xEA // SelectCase
    0xE3,       // 0xEB // SelectIs
    0xE4,       // 0xEC // SelectType
    0xE5,       // 0xED // SetStmt
    0xE6,       // 0xEE // Stack
    0xE7,       // 0xEF // Stop
    0xE8,       // 0xF0 // Type
    0xE9,       // 0xF1 // UnLock
    0xEA,       // 0xF2 // VarDefn
    0xEB,       // 0xF3 // Wend
    0xEC,       // 0xF4 // While
    0xED,       // 0xF5 // With
    0xEE,       // 0xF6 // WriteChan
    0xEF,       // 0xF7 // ConstFuncExpr
    0xF0,       // 0xF8 // LbConst
    0xF1,       // 0xF9 // LbIf
    0xF2,       // 0xFA // LbElse
    0xF3,       // 0xFB // LbElseif
    0xF4,       // 0xFC // LbEndif
    0xF5,       // 0xFD // LbMark
    0xF6,       // 0xFE // EndForVariable
    0xF7,       // 0xFF // StartForVariable
    0xF8,       // 0x100 // NewRedim
    0xF9,       // 0x101 // StartWithExpr
    0xFA,       // 0x102 // SetOrSt
    0xFB,       // 0x103 // EndEnum
    0xFC        // 0x104 // Illegal
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL O97ModAccGetOpcode
(
    LPO97_MOD_ACC       lpstMod,
    DWORD               dwOffset,
    LPBYTE              lpabyOpcode
)
{
    LPBYTE              lpabyData;
    WORD                wSize;

    lpabyData = O97ModAccGetDataPtr(lpstMod,
                                    dwOffset,
                                    2,
                                    &wSize);

    if (lpabyData == NULL || wSize != 2)
        return(FALSE);

    lpabyOpcode[0] = lpabyData[0];
    lpabyOpcode[1] = lpabyData[1];

    // Endianize

    if (lpstMod->bLittleEndian == FALSE)
    {
        BYTE    byTemp;

        byTemp = lpabyOpcode[0];
        lpabyOpcode[0] = lpabyOpcode[1];
        lpabyOpcode[1] = byTemp;
    }

    // Convert to O97 opcode

    if (lpstMod->nVersion != O97_VERSION_O97)
    {
        int     nOpcode;

        // Translate O98 opcodes to O97

        nOpcode = lpabyOpcode[0] |
            ((lpabyOpcode[1] & 0x01) << 8);

        if (nOpcode > O98_OP_MAX)
            nOpcode = O98_OP_MAX;

        lpabyOpcode[0] = gabyModAccXOpO98toO97[nOpcode];
        lpabyOpcode[1] &= 0xFC;
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97ModAccGetWORD()
//
// Parameters:
//  lpstMod             Ptr to initialized module access structure
//  dwOffset            Offset in binary stream of WORD to get
//  lpwValue            Destination of WORD
//
// Description:
//  Retrieves a WORD value from the module binary stream.  The value
//  is endianized appropriately.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97ModAccGetWORD
(
    LPO97_MOD_ACC       lpstMod,
    DWORD               dwOffset,
    LPWORD              lpwValue
)
{
    LPBYTE              lpabyData;
    WORD                wSize;

    lpabyData = O97ModAccGetDataPtr(lpstMod,
                                    dwOffset,
                                    sizeof(WORD),
                                    &wSize);

    if (lpabyData == NULL || wSize != sizeof(WORD))
        return(FALSE);

    // Endianize it

    *lpwValue = SSEndianWORD(lpabyData,
                             lpstMod->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// BOOL O97ModAccGetDWORD()
//
// Parameters:
//  lpstMod             Ptr to initialized module access structure
//  dwOffset            Offset in binary stream of DWORD to get
//  lpdwValue           Destination of DWORD
//
// Description:
//  Retrieves a DWORD value from the module binary stream.  The value
//  is endianized appropriately.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97ModAccGetDWORD
(
    LPO97_MOD_ACC       lpstMod,
    DWORD               dwOffset,
    LPDWORD             lpdwValue
)
{
    LPBYTE              lpabyData;
    WORD                wSize;

    lpabyData = O97ModAccGetDataPtr(lpstMod,
                                    dwOffset,
                                    sizeof(DWORD),
                                    &wSize);

    if (lpabyData == NULL || wSize != sizeof(DWORD))
        return(FALSE);

    // Endianize it

    *lpdwValue = SSEndianDWORD(lpabyData,
                               lpstMod->bLittleEndian);
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModFillVBASrcTo4K()
//
// Parameters:
//  lpstStream          Ptr to stream to replace
//  dwOffset            Starting offset
//
// Description:
//  The function assumes that dwOffset is < 4096 and assumes
//  that the offset is the beginning of the next LZNT chunk
//  for a module stream.  The function fills it with a single
//  chunk containing 0x0D characters.  If the number of bytes
//  until 4096 is less than four, the function does nothing.
//
//  elai:  This is being done so that we don't have to deal with
//			the case where our stream goes from more than 4k
//          to less then 4k ... where to do this correctly, we'd
//			have to make this stream use the small FAT instead 
//			of the large FAT.
// 
// Returns:
//  TRUE                If successful
//  FALSE               On error
//
//********************************************************************

BOOL O97ModFillVBASrcTo4K
(
    LPSS_STREAM         lpstStream,
    DWORD               dwOffset
)
{
    WORD                wBytesLeft;
    WORD                wHdr;
    DWORD               dwCount;
    BYTE                abyBuf[9];
    WORD                w;
    DWORD               dwBytesToWrite;

    if (dwOffset >= 4096)
        return(TRUE);

    wBytesLeft = (WORD)(4096 - dwOffset);
    if (wBytesLeft < 4)
	{
		// we write Zeros in these bytes to avoid the possibility of a
		// 4093-4095 byte stream now and accidentally having legitimate 
		// signature bits in a compressed header.
		//
		// if office actually does read the slack space, (which it might)
		// The TruncateStream() should have Zeroed the remaining bytes in 
		// a potential LZ chunk header.

		if (SSWriteZeroes(lpstStream,
						  dwOffset,
						  wBytesLeft) != SS_STATUS_OK)
		{
			// Failed to zero out rest of stream

			return(FALSE);
		}

        return(TRUE);
	}

    // Write header

    wHdr = 0xB000 | (wBytesLeft - 3);
    wHdr = WENDIAN(wHdr);
    if (SSSeekWrite(lpstStream,
                    dwOffset,
                    &wHdr,
                    sizeof(WORD),
                    &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(WORD))
        return(FALSE);

    // Decrement number of bytes left by size of header

    wBytesLeft -= 2;
    dwOffset += 2;

    // Fill write buffer

    abyBuf[0] = 0;
    for (w=1;w<=8;w++)
        abyBuf[w] = 0x0D;

    // Write until 4096

    dwBytesToWrite = 9;
    while (wBytesLeft != 0)
    {
        if (wBytesLeft < 9)
            dwBytesToWrite = wBytesLeft;

        if (SSSeekWrite(lpstStream,
                        dwOffset,
                        abyBuf,
                        dwBytesToWrite,
                        &dwCount) != SS_STATUS_OK ||
            dwCount != dwBytesToWrite)
            return(FALSE);

        // Update bytes left and offset
		#pragma warning(push, 3)
		//wierd: BYTE += BYTE gives 'conversion from int to BYTE, possible loss of data'
		// BYTE = BYTE + BYTE does not
		// May be a global += operator somewhere...?
        wBytesLeft -= (WORD)dwBytesToWrite;
		#pragma warning(pop)
        dwOffset += dwBytesToWrite;
    }

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModReplaceVBASrc()
//
// Parameters:
//  lpstStream          Ptr to stream to replace
//  lpstLZNT            Ptr to LZNT structure
//  lpabySrc            Ptr to LZNT source
//  dwSrcSize           Size of source
//  lpbNoRoom           Ptr to BOOL for no room result
//
// Description:
//  Replaces the compressed source of the given stream with
//  the given source.  The remainder of the stream is zeroed out.
//  The stack-based binary is also zeroed out.
//
//  If the compressed source of the given stream is smaller than
//  the given source, the function returns TRUE and the value
//  TRUE in *lpbNoRoom.
//
// Returns:
//  TRUE                If replacement was successful
//  FALSE               On error
//
//********************************************************************

BOOL O97ModReplaceVBASrc
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabySrc,
    DWORD               dwSrcSize,
    LPBOOL              lpbNoRoom
)
{
    int                 nVersion;
    BOOL                bLitEnd;

    DWORD               dwLZNTOffset;
    DWORD               dwStreamLen;
    DWORD               dwZeroesToWrite;
    DWORD               dwBytesToWrite;
    DWORD               dwCount;

    DWORD               dwLineTableOffset;
    DWORD               dwNumLines;
    DWORD               dwBinaryOffset;

    // Determine module type

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLitEnd) == FALSE)
        return(FALSE);

    if (VBA5LZNTModuleInit(lpstStream,
                           lpstLZNT) == FALSE)
        return(FALSE);

 	// Get Line Binary Offsets now to avoid future problems
 	// like MAC 2001
 
     if (O97GetLineBinaryOffsets(lpstStream,
                                 &dwLineTableOffset,
                                 &dwNumLines,
                                 &dwBinaryOffset,
                                 NULL) == FALSE)
         return(FALSE);

    dwLZNTOffset = lpstLZNT->dwFirstChunkOffset - 1;

    // Validate the binary offset

    if (dwBinaryOffset > dwLZNTOffset || dwBinaryOffset < 10)
        return(FALSE);

    dwStreamLen = SSStreamLen(lpstStream);
    if (dwLZNTOffset >= dwStreamLen)
        return(FALSE);

    // Determine whether there is enough room for the new source

    dwBytesToWrite = dwStreamLen - dwLZNTOffset;
    if (dwBytesToWrite < dwSrcSize)
    {
        // Report too small

        *lpbNoRoom = TRUE;
        return(TRUE);
    }

    // Not too small

    *lpbNoRoom = FALSE;

    dwZeroesToWrite = dwBytesToWrite - dwSrcSize;

    /////////////////////////////////////////////////////////////
    // Replace the module source

    if (SSSeekWrite(lpstStream,
                    dwLZNTOffset,
                    lpabySrc,
                    dwSrcSize,
                    &dwCount) != SS_STATUS_OK ||
        dwCount != dwSrcSize)
    {
        // Wrote less than the desired number of bytes

        return(FALSE);
    }

    // Zero out the remainder of the stream

    if (SSWriteZeroes(lpstStream,
                      dwLZNTOffset + dwSrcSize,
                      dwZeroesToWrite) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Zero out the stack-based code


    // Get the size of the stack based code

    if (SSSeekRead(lpstStream,
                   dwBinaryOffset - 4,
                   &dwZeroesToWrite,
                   sizeof(DWORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(DWORD))
        return(FALSE);

    dwZeroesToWrite = SSEndianDWORD((LPBYTE)&dwZeroesToWrite,bLitEnd);

    // Validate the size

    if (dwZeroesToWrite >= dwStreamLen ||
        dwBinaryOffset + dwZeroesToWrite > dwLZNTOffset)
        return(FALSE);

    // Zero out the stack-based code

    if (SSWriteZeroes(lpstStream,
                      dwBinaryOffset,
                      dwZeroesToWrite) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    // Zero out everything before the compressed source

    if (SSWriteZeroes(lpstStream,
                      0,
                      dwLZNTOffset) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }


    // Calculate what the new size should be

    if (dwLZNTOffset + dwSrcSize < 4096)
    {
        if (dwStreamLen >= 4096)
        {
            O97ModFillVBASrcTo4K(lpstStream,
                                 dwLZNTOffset + dwSrcSize);

            dwStreamLen = 4096;
        }
        else
            dwStreamLen = dwLZNTOffset + dwSrcSize;
    }
    else
        dwStreamLen = dwLZNTOffset + dwSrcSize;

	// Truncate the stream

	if (SSTruncateStream( lpstStream, dwStreamLen ) != SS_STATUS_OK)
		return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModVBASrcHasVB_Base()
//
// Parameters:
//  lpstStream          Ptr to stream
//  lpstLZNT            Ptr to LZNT structure
//  lpabyBuf            Ptr to buffer to hold Attribute lines
//  nBufSize            Size of buffer
//  lpnAttributeSize    Ptr to integer for result size
//
// Description:
//  The function determines whether the given VBA source
//  begins with an "Attribute VB_Base" line in the first
//  nBufSize bytes.
//
//  If so, then *lpnAttributeSize returns with the index of the
//  end of the line and the buffer contains the attribute lines
//  up to and including the VB_Base line.
//
//  If a VB_Base line is not found, *lpnAttributeSize returns
//  with zero.
//
// Returns:
//  TRUE                If replacement was successful
//  FALSE               On error
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyAttributeVB_Base[] =
{
    0x41, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x20,
    0x56, 0x42, 0x5F, 0x42, 0x61, 0x73, 0x65
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL O97ModVBASrcHasVB_Base
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyBuf,
    int                 nBufSize,
    LPINT               lpnAttributeSize
)
{
    int                 i;
    int                 nBegI;

    // Initialize for access to the source

    if (VBA5LZNTModuleInit(lpstStream,
                           lpstLZNT) == FALSE)
        return(FALSE);

    // Limit the search size

    if (lpstLZNT->dwSize < DWORD(nBufSize))
        nBufSize = (int)lpstLZNT->dwSize;

    // Get the bytes

    if (LZNTGetBytes(lpstLZNT,
                     0,
                     nBufSize,
                     lpabyBuf) != nBufSize)
        return(FALSE);

    // Iterate through the buffer

    i = 0;
    for(;;)
    {
        // Skip white space

        while (i < nBufSize)
        {
            // if (lpabyBuf[i] != ' ' && lpabyBuf[i] != '\t')
            if (lpabyBuf[i] != 0x20 && lpabyBuf[i] != 0x09)
                break;

            ++i;
        }

        // Search for the end of the line

        nBegI = i;
        while (i < nBufSize)
        {
            if (lpabyBuf[i] == 0x0D || lpabyBuf[i] == 0x0A)
                break;

            ++i;
        }

        if (i >= nBufSize)
            break;

        if ((i - nBegI) > sizeof(gabyAttributeVB_Base))
        {
            // Search for "Attribute VB_Base" within this line

            if (O97MemICmp(gabyAttributeVB_Base,
                           lpabyBuf + nBegI,
                           sizeof(gabyAttributeVB_Base)) != FALSE)
            {
                // Found it

                // Skip past the CR/LF

                if (lpabyBuf[i] == 0x0D)
                    ++i;

                if (i < nBufSize && lpabyBuf[i] == 0x0A)
                    ++i;

                // Return it

                *lpnAttributeSize = i;
                return(TRUE);
            }
        }

        // Skip over the CR or LF

        ++i;
    }

    // Did not see an "Attribute VB_Base" line

    *lpnAttributeSize = 0;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModReplaceVB_BaseSrc()
//
// Parameters:
//  lpstStream          Ptr to stream to replace
//  lpstLZNT            Ptr to LZNT structure
//  lpabyWorkBuf        Ptr to 1024 byte buffer
//  lpbReplaced         Ptr to BOOL for no room result
//
// Description:
//  The function checks to see if the source begins with an
//  "Attribute VB_Base" line.  If so, the function replaces
//  removes all the source that follows it and sets
//  *lpbReplaced to TRUE.  If the source does not begin
//  with the "Attribute VB_Base" line in the first 1024
//  bytes, the function does nothing to the stream and returns
//  with *lpbReplaced set to FALSE.
//
// Returns:
//  TRUE                If successful
//  FALSE               On error
//
//********************************************************************

BOOL O97ModReplaceVB_BaseSrc
(
    LPSS_STREAM         lpstStream,
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyWorkBuf,
    LPBOOL              lpbReplaced
)
{
    int                 nVersion;
    BOOL                bLitEnd;

    int                 nAttributeSize;
    DWORD               dwLZNTOffset;
    DWORD               dwStreamLen;
    DWORD               dwEndOffset;

    DWORD               dwZeroesToWrite;
    DWORD               dwCount;

    DWORD               dwLineTableOffset;
    DWORD               dwNumLines;
    DWORD               dwBinaryOffset;

    // Determine module type

    if (O97GetModuleVersionEndian(lpstStream,
                                  &nVersion,
                                  &bLitEnd) == FALSE)
        return(FALSE);

    if (O97ModVBASrcHasVB_Base(lpstStream,
                               lpstLZNT,
                               lpabyWorkBuf,
                               1024,
                               &nAttributeSize) == FALSE)
        return(FALSE);

    if (nAttributeSize == 0)
    {
        // Couldn't find an "Attribute VB_Base" line

        *lpbReplaced = FALSE;
        return(TRUE);
    }

    dwLZNTOffset = lpstLZNT->dwFirstChunkOffset - 1;

    if (O97GetLineBinaryOffsets(lpstStream,
                                &dwLineTableOffset,
                                &dwNumLines,
                                &dwBinaryOffset,
                                NULL) == FALSE)
        return(FALSE);

    // Validate the binary offset

    if (dwBinaryOffset > dwLZNTOffset || dwBinaryOffset < 10)
        return(FALSE);

    dwStreamLen = SSStreamLen(lpstStream);
    if (dwLZNTOffset >= dwStreamLen)
        return(FALSE);

    // Write compressed

    if (LZNTCompressBufWrite(lpstLZNT,
                             dwLZNTOffset,
                             lpabyWorkBuf,
                             (DWORD)nAttributeSize,
                             &dwEndOffset) == FALSE)
        return(FALSE);

    if (dwEndOffset > dwStreamLen)
        return(FALSE);

    // Zero out the remainder of the stream

    if (SSWriteZeroes(lpstStream,
                      dwEndOffset,
                      dwStreamLen - dwEndOffset) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    /////////////////////////////////////////////////////////////
    // Zero out the stack-based code

    // Get the size of the stack based code

    if (SSSeekRead(lpstStream,
                   dwBinaryOffset - 4,
                   &dwZeroesToWrite,
                   sizeof(DWORD),
                   &dwCount) != SS_STATUS_OK ||
        dwCount != sizeof(DWORD))
        return(FALSE);

    dwZeroesToWrite = SSEndianDWORD((LPBYTE)&dwZeroesToWrite,bLitEnd);

    // Validate the size

    if (dwZeroesToWrite >= dwStreamLen ||
        dwBinaryOffset + dwZeroesToWrite > dwLZNTOffset)
        return(FALSE);

    // Zero out the stack-based code

    if (SSWriteZeroes(lpstStream,
                      dwBinaryOffset,
                      dwZeroesToWrite) != SS_STATUS_OK)
    {
        // Failed to zero out rest of stream

        return(FALSE);
    }

    // Calculate what the new size should be

    if (dwEndOffset < 4096)
    {
        if (dwStreamLen >= 4096)
        {
            O97ModFillVBASrcTo4K(lpstStream,
                                 dwEndOffset);

            dwStreamLen = 4096;
        }
        else
            dwStreamLen = dwEndOffset;
    }
    else
        dwStreamLen = dwEndOffset;

    // Truncate the stream

	if (SSTruncateStream( lpstStream, dwStreamLen ) != SS_STATUS_OK)
		return(FALSE);

    *lpbReplaced = TRUE;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97ModVBASrcGetVB_Name()
//
// Parameters:
//  lpstLZNT            Ptr to LZNT structure
//  lpabyBuf            Ptr to buffer to hold Attribute lines
//  nBufSize            Size of buffer
//  lpabyszName         Buffer for name
//  nMaxNameSize        Maximum size of name not including zero
//
// Description:
//  The function searches for an "Attribute VB_Base" line in the
//  first nBufSize bytes.
//
//  If present then, so, then the function copies the name
//  as a zero-terminated uppercased string to lpabyBuf.
//
//  If a VB_Name line is not found, the buffer returns as an
//  empty string.
//
//  The LZNT structure must have been initialized for access
//  before calling this function.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

#ifdef SYM_WIN16
#pragma data_seg("FAR_DATA_0")
#endif

BYTE FAR gabyAttributeVB_Name[] =
{
    0x41, 0x74, 0x74, 0x72, 0x69, 0x62, 0x75, 0x74, 0x65, 0x20,
    0x56, 0x42, 0x5F, 0x4E, 0x61, 0x6D, 0x65
};

#ifdef SYM_WIN16
#pragma data_seg()
#endif

BOOL O97ModVBASrcGetVB_Name
(
    LPLZNT              lpstLZNT,
    LPBYTE              lpabyBuf,
    int                 nBufSize,
    LPBYTE              lpabyszName,
    int                 nMaxNameSize
)
{
    int                 i;
    int                 nBegI;

    // Limit the search size

    if (lpstLZNT->dwSize < DWORD(nBufSize))
        nBufSize = (int)lpstLZNT->dwSize;

    // Get the bytes

    if (LZNTGetBytes(lpstLZNT,
                     0,
                     nBufSize,
                     lpabyBuf) != nBufSize)
        return(FALSE);

    // Iterate through the buffer

    i = 0;
    for(;;)
    {
        // Skip white space

        while (i < nBufSize)
        {
            // if (lpabyBuf[i] != ' ' && lpabyBuf[i] != '\t')
            if (lpabyBuf[i] != 0x20 && lpabyBuf[i] != 0x09)
                break;

            ++i;
        }

        // Search for the end of the line

        nBegI = i;
        while (i < nBufSize)
        {
            if (lpabyBuf[i] == 0x0D || lpabyBuf[i] == 0x0A)
                break;

            ++i;
        }

        if (i >= nBufSize)
            break;

        if ((i - nBegI) > sizeof(gabyAttributeVB_Name))
        {
            // Search for "Attribute VB_Name" within this line

            if (O97MemICmp(gabyAttributeVB_Name,
                           lpabyBuf + nBegI,
                           sizeof(gabyAttributeVB_Name)) != FALSE)
            {
                // Found it

                // Skip over "Attribute VB_Name"

                nBegI += sizeof(gabyAttributeVB_Name);

                // Search for the beginning double quote

                while (nBegI < i)
                {
                    if (lpabyBuf[nBegI++] == 0x22) // "
                        break;
                }

                // Couldn't find it?

                if (nBegI >= i)
                {
                    lpabyszName[0] = 0;
                    return(TRUE);
                }

                // Copy it to the beginning of the buffer

                i = 0;
                while (nBegI < nBufSize && i < nMaxNameSize)
                {
                    if (lpabyBuf[nBegI] == 0x22) // "
                        break;

                    lpabyszName[i++] = SSToUpper(lpabyBuf[nBegI++]);
                }

                lpabyszName[i] = 0;
                return(TRUE);
            }
        }

        // Skip over the CR or LF

        ++i;
    }

    // Did not see an "Attribute VB_Name" line

    lpabyszName[0] = 0;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  BOOL O97VBAStorageIsEmpty()
//
// Parameters:
//  lpstRoot            Ptr to root structure
//  lpstStream          Ptr to stream structure
//  dwVBAChildEntry     Child of VBA storage
//  lpstLZNT            Ptr to LZNT structure
//  lpstSibs            Ptr to sibling enumeration structure or NULL
//  lpbIsEmpty          Ptr to BOOL for empty result
//
// Description:
//  The function iterates through all the module streams of the
//  given VBA storage.  If all modules are empty, the function
//  sets *lpbIsEmpty to TRUE.  Otherwise, *lpbIsEmpty is set
//  to FALSE.
//
//  The function will allocate its own sibling enumeration structure
//  if lpstSibs is NULL.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL O97VBAStorageIsEmpty
(
    LPSS_ROOT           lpstRoot,
    LPSS_STREAM         lpstStream,
    DWORD               dwVBAChildEntry,
    LPLZNT              lpstLZNT,
    LPSS_ENUM_SIBS      lpstSibs,
    LPBOOL              lpbIsEmpty
)
{
    int                 i;
    LPSS_ENUM_SIBS      lpstPassedSibs;
    BOOL                bResult;

    lpstPassedSibs = lpstSibs;

    if (lpstSibs == NULL)
    {
        // Allocate a sibling enumeration structure

        if (SSAllocEnumSibsStruct(lpstRoot,
                                  &lpstSibs,
                                  SS_DEF_MAX_WAITING_SIBS) != SS_STATUS_OK)
            return(FALSE);
    }

    // Iterate through all the module streams

    SSInitEnumSibsStruct(lpstSibs,dwVBAChildEntry);

    *lpbIsEmpty = TRUE; // Assume empty
    bResult = TRUE;
    for (i=0;i<16384;i++)
    {
        if (SSEnumSiblingEntriesCB(lpstRoot,
                                   O97OpenVBA5ModuleCB,
                                   NULL,
                                   lpstSibs,
                                   lpstStream) != SS_STATUS_OK)
        {
            // No more streams to check?

            break;
        }

        // Determine whether the stream consists of only
        //  Attribute lines

        if (VBA5LZNTModuleInit(lpstStream,
                               lpstLZNT) == FALSE)
        {
            // Failed to initialize for reading source text string

            bResult = FALSE;
            break;
        }

        if (O97LZNTStreamAttributeOnly(lpstLZNT) == FALSE)
        {
            // The stream contained something other than
            //  Attribute lines

            *lpbIsEmpty = FALSE;
            break;
        }
    }

    if (lpstPassedSibs == NULL)
        SSFreeEnumSibsStruct(lpstRoot,lpstSibs);

    return(bResult);
}


//********************************************************************
//
// BOOL O97ModGetRoutineTypeInfo()
//
// Parameters:
//  lpstStream              Ptr to module stream
//  bLittleEndian           Little endian state of module
//  dwOffset                Offset of type info
//  lpabyFlagBytes          First two bytes of record
//  lpwIdent                Ptr to WORD for identifier WORD
//  lpbyNumParams           Ptr to BYTE for number of parameters
//  lpdwFirstParamOffset    Ptr to DWORD for next param record
//
// Description:
//  Retrieves the ID of the identifier and the first parameter offset
//  for the record at the given offset.  It is assumed that the ID
//  is at offset two from the beginning of the record and the first
//  parameter offset is at offset 0x24 from the beginning of the
//  record.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97ModGetRoutineTypeInfo
(
    LPSS_STREAM         lpstStream,
    BOOL                bLittleEndian,
    DWORD               dwOffset,
    LPBYTE              lpabyFlagBytes,
    LPWORD              lpwIdent,
    LPBYTE              lpbyNumParams,
    LPDWORD             lpdwFirstParamOffset
)
{
    DWORD               dwBytesRead;
    BYTE                abyRec[0x38];

    if (SSSeekRead(lpstStream,
                   dwOffset,
                   abyRec,
                   sizeof(abyRec),
                   &dwBytesRead) != SS_STATUS_OK ||
        dwBytesRead != sizeof(abyRec))
        return(FALSE);

    if (lpabyFlagBytes != NULL)
    {
        lpabyFlagBytes[0] = abyRec[0];
        lpabyFlagBytes[1] = abyRec[1];
    }

    if (lpwIdent != NULL)
    {
        *lpwIdent = SSEndianWORD(abyRec + 2,
                                 bLittleEndian);
    }

    if (lpbyNumParams != NULL)
    {
        *lpbyNumParams = abyRec[0x37];
    }

    if (lpdwFirstParamOffset != NULL)
    {
        *lpdwFirstParamOffset = SSEndianDWORD(abyRec + 0x24,
                                              bLittleEndian);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97ModGetParamInfo()
//
// Parameters:
//  lpstStream          Ptr to module stream
//  bLittleEndian       Little endian state of module
//  dwOffset            Offset in type table of record
//  lpwIdent            Ptr to WORD for identifier WORD
//  lpdwNextParamOffset Ptr to DWORD for next param record
//
// Description:
//  Retrieves the ID of the identifier and the next parameter offset
//  for the record at the given offset.  It is assumed that the ID
//  is at offset two from the beginning of the record and the next
//  parameter offset is at offset 0x14 from the beginning of the
//  record.
//
// Returns:
//  TRUE        On success
//  FALSE       On failure
//
//********************************************************************

BOOL O97ModGetParamInfo
(
    LPSS_STREAM         lpstStream,
    BOOL                bLittleEndian,
    DWORD               dwOffset,
    LPWORD              lpwIdent,
    LPDWORD             lpdwNextParamOffset
)
{
    DWORD               dwBytesRead;

    if (lpwIdent != NULL)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset + 2,
                       lpwIdent,
                       sizeof(WORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpwIdent = SSEndianWORD((LPBYTE)lpwIdent,
                                 bLittleEndian);
    }

    if (lpdwNextParamOffset != NULL)
    {
        if (SSSeekRead(lpstStream,
                       dwOffset + 0x14,
                       lpdwNextParamOffset,
                       sizeof(DWORD),
                       &dwBytesRead) != SS_STATUS_OK)
            return(FALSE);

        *lpdwNextParamOffset = SSEndianDWORD((LPBYTE)lpdwNextParamOffset,
                                             bLittleEndian);
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97ModGetSubsInit()
//
// Parameters:
//  lpstStream              Ptr to module stream
//  lpstSubs                Ptr to GETSUBS structure
//
// Description:
//  The function initializes for getting the subroutine IDs
//  of the given module stream.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97ModGetSubsInit
(
    LPSS_STREAM             lpstStream,
    LPO97MOD_GETSUBS        lpstSubs
)
{
    lpstSubs->dwNextLine = 0;

    // Get the type table offset and size

    if (O97GetTypeTableOffset(lpstStream,
                              &lpstSubs->dwTypeTableOffset,
                              &lpstSubs->dwTypeTableSize) == FALSE)
        return(FALSE);

    // Initialize for module access

    if (O97ModAccInit(&lpstSubs->stMod,
                      lpstStream,
                      lpstSubs->abyBuf,
                      O97MOD_GETSUBS_BUF_SIZE) == FALSE)
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// BOOL O97ModGetSubs()
//
// Parameters:
//  lpstSubs                Ptr to GETSUBS structure
//  dwFlags                 Flags (should be zero)
//
// Description:
//  The function continues at lpstSubs->dwNextLine to retrieve
//  the next set of subroutine IDs.  A subroutine ID is returned
//  only if it is not a function, is not private, and takes no
//  parameters.
//
//  The flags should be zero if only public subroutines that
//  take no parameters should be returned.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97ModGetSubs
(
    LPO97MOD_GETSUBS        lpstSubs,
    DWORD                   dwFlags
)
{
    DWORD                   dwMaxLine;
    DWORD                   dwLineOffset;
    WORD                    wLineSize;
    BYTE                    abyOpcode[2];
    DWORD                   dwTypeOffset;
    BYTE                    abyFlags[2];
    WORD                    wID;
    BYTE                    byNumParams;

    (void)dwFlags;

    // Initialize to none found

    lpstSubs->wCount = 0;

    dwMaxLine = lpstSubs->stMod.dwTableSize;
    while (lpstSubs->dwNextLine < dwMaxLine &&
           lpstSubs->wCount < O97MOD_GETSUBS_BUF_MAX_COUNT)
    {
        // Get the line info

        if (O97ModAccGetLineInfo(&lpstSubs->stMod,
                                 lpstSubs->dwNextLine++,
                                 &dwLineOffset,
                                 &wLineSize) == FALSE)
            return(FALSE);

        // If the line is too small for the opcode (2 bytes)
        //  and the type table offset (4 bytes), try the next line

        if (wLineSize < 6)
            continue;

        // Get the opcode

        if (O97ModAccGetOpcode(&lpstSubs->stMod,
                               dwLineOffset,
                               abyOpcode) == FALSE)
            return(FALSE);

        // Only do subroutines

        if (abyOpcode[0] != O97_OP_FUNC_DEFN ||
            (abyOpcode[1] & 0x04) == 0)
            continue;

        // Get the type table record offset

        if (O97ModAccGetDWORD(&lpstSubs->stMod,
                              dwLineOffset + 2,
                              &dwTypeOffset) == FALSE)
            return(FALSE);

        // Get information about the routine

        if (O97ModGetRoutineTypeInfo(lpstSubs->stMod.lpstStream,
                                     lpstSubs->stMod.bLittleEndian,
                                     lpstSubs->dwTypeTableOffset +
                                         dwTypeOffset,
                                     abyFlags,
                                     &wID,
                                     &byNumParams,
                                     NULL) == FALSE)
            return(FALSE);

        // If it is private don't add it

        if ((abyFlags[0] & 0x08) == 0)
            continue;

        // Store the ID

        lpstSubs->awSubIDs[lpstSubs->wCount++] = wID;
    }

    return(TRUE);
}


//********************************************************************
//
// BOOL O97IDLookupInit()
//
// Parameters:
//  lpstStream              Ptr to _VBA_PROJECT stream
//  lpstLookup              Lookup structure to initialize
//
// Description:
//  The function initializes for getting the subroutine IDs
//  of the given module stream.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97IDLookupInit
(
    LPSS_STREAM             lpstStream,
    LPO97_ID_LOOKUP         lpstLookup
)
{
    lpstLookup->lpstStream = lpstStream;

    // Add a cache to it

    SSStreamStructAddCache(lpstStream);

    // Determine the stream's endian state

    if (O97Get_VBA_PROJECTEndian(lpstStream,
                                 NULL,
                                 &lpstLookup->bLittleEndian) == FALSE)
    {
        // Couldn't get the endian state

        return(FALSE);
    }

    // Get identifier table location

    if (O97GetIdentifierTableOffset(lpstStream,
                                    &lpstLookup->dwTableOffset,
                                    &lpstLookup->dwNumIdents) == FALSE)
    {
        // Couldn't find identifier table

        return(FALSE);
    }

    // Success

    return(TRUE);
}


//********************************************************************
//
// BOOL O97IDLookupSet()
//
// Parameters:
//  lpstLookup              Ptr to lookup structure
//  lpawIDs                 Ptr to IDs to lookup
//  wCount                  Number of IDs to lookup
//  lpabyapszNames          Ptr to buffer for names
//  dwNameBufSize           Size of name buffer
//  lpalpapszNames          Ptr to buffer for name ptrs
//
// Description:
//  The function searches through the identifier table for the
//  names of the IDs in the given set.  IDs do not need to be
//  necessarily unique, however, each will point to the same
//  occurrence in the name buffer.  If an ID is not found,
//  its pointer will be NULL upon return.
//
//  Strings are stored as Pascal/C combination format.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97IDLookupSet
(
    LPO97_ID_LOOKUP         lpstLookup,
    LPWORD                  lpawIDs,
    WORD                    wCount,
    LPBYTE                  lpabyapszNames,
    DWORD                   dwNameBufSize,
    LPLPBYTE                lpalpapszNames,
    LPWORD                  lpwCountFilled
)
{
    WORD                    wID;
    WORD                    wNextID;
    WORD                    w;
    DWORD                   dwOffset;
    BYTE                    abyName[2];
    DWORD                   dwBytesRead;
    DWORD                   dwNameBufUsed;
    DWORD                   dw;

    // Initialize all to not found

    for (w=0;w<wCount;w++)
        lpalpapszNames[w] = NULL;

    *lpwCountFilled = 0;

    // Determine the next ID to lookup by looking for the lowest
    //  valued ID >= 200

    wNextID = 0xFFFF;
    for (w=0;w<wCount;w++)
    {
        if (0x200 <= lpawIDs[w] && lpawIDs[w] < wNextID)
            wNextID = lpawIDs[w] & 0xFFFE;
    }

    // Now iterate through the identifiers

    dwOffset = lpstLookup->dwTableOffset;
    wID = 0x200;
    dwNameBufUsed = 0;
    for (dw=0;dw<lpstLookup->dwNumIdents;dw++,wID+=2)
    {
#ifdef SYM_NLM
        if ((dw & 0xFF) == 0)
        {
            // Relinquish control every 256th identifier

            SSProgress(lpstLookup->lpstStream->lpstRoot->lpvRootCookie);
        }
#endif // #ifdef SYM_NLM

        // Get the string length and the flag byte

        if (SSCacheSeekRead(lpstLookup->lpstStream,
                            dwOffset,
                            abyName,
                            2,
                            &dwBytesRead) != SS_STATUS_OK ||
            dwBytesRead != 2)
            break;

        // Endianize

        if (lpstLookup->bLittleEndian == FALSE)
        {
            BYTE        byTemp;

            byTemp = abyName[0];
            abyName[0] = abyName[1];
            abyName[1] = byTemp;
        }

        // If the high bit on the flag byte is set, then skip
        //  the extra bytes

        if (abyName[1] & 0x80)
            dwOffset += 8;
        else
            dwOffset += 2;

        // Is it the identifier that we want?

        if (wID == wNextID)
        {
            // Is there room for the identifier?

            if (dwNameBufUsed + abyName[0] + 2 > dwNameBufSize)
                break;

            // Store the name length

            lpabyapszNames[dwNameBufUsed] = abyName[0];

            // Read the identifier

            if (SSCacheSeekRead(lpstLookup->lpstStream,
                                dwOffset,
                                lpabyapszNames + dwNameBufUsed + 1,
                                abyName[0],
                                &dwBytesRead) != SS_STATUS_OK ||
                dwBytesRead != abyName[0])
                break;

            // Zero-terminate it

            lpabyapszNames[dwNameBufUsed + abyName[0] + 1] = 0;

            // Set the pointers

            for (w=0;w<wCount;w++)
            {
                if (wID == (lpawIDs[w] & 0xFFFE))
                {
                    lpalpapszNames[w] = lpabyapszNames + dwNameBufUsed;
                    ++*lpwCountFilled;
                }
            }

            // Are we done?

            if (*lpwCountFilled == wCount)
                break;

            // Update the number of name buf bytes used

            dwNameBufUsed += abyName[0] + 2;

            // Find the next ID

            wNextID = 0xFFFF;
            for (w=0;w<wCount;w++)
            {
                if (wID + 2 <= lpawIDs[w] && lpawIDs[w] < wNextID)
                    wNextID = lpawIDs[w] & 0xFFFE;
            }

            if (wNextID == 0xFFFF)
            {
                // No more can be found

                break;
            }
        }

        // Move over the string and the end bytes

        dwOffset += abyName[0] + 4;
    }

    // Success

    return(TRUE);
}


//********************************************************************
//
// BOOL O97ModAccParseOpcode()
//
// Parameters:
//  lpstParse           Ptr to parse state structure
//
// Description:
//  Parses the next opcode.  Currently obtains the first two
//  WORD arguments if any.
//
// Returns:
//  TRUE                On success
//  FALSE               On failure
//
//********************************************************************

BOOL O97ModAccParseOpcode
(
    LPO97_MOD_ACC_PARSE lpstParse
)
{
    WORD                wSize;

    // Get the opcode

    if (O97ModAccGetOpcode(lpstParse->lpstMod,
                           lpstParse->dwLineOffset + lpstParse->dwOffset,
                           lpstParse->abyOpcode) == FALSE)
        return(FALSE);

    // Advance the offset past the opcode

    lpstParse->dwOffset += 2;

    switch (lpstParse->abyOpcode[0])
    {
        // Single WORD arguments

        case VBA5_OP_1F_LdLHS:              // 031: HLNAM
        case VBA5_OP_20_Ld:                 // 032: HLNAM
        case VBA5_OP_21_MemLd:              // 033: HLNAM
        case VBA5_OP_22_DictLd:             // 034: HLNAM
        case VBA5_OP_23_IndexLd:            // 035: USHORT
        case VBA5_OP_27_St:                 // 039: HLNAM
        case VBA5_OP_28_MemSt:              // 040: HLNAM
        case VBA5_OP_29_DictSt:             // 041: HLNAM
        case VBA5_OP_2A_IndexSt:            // 042: USHORT
        case VBA5_OP_2E_Set:                // 046: HLNAM
        case VBA5_OP_2F_MemSet:             // 047: HLNAM
        case VBA5_OP_30_DictSet:            // 048: HLNAM
        case VBA5_OP_31_IndexSet:           // 049: USHORT
        case VBA5_OP_35_MemLdWith:          // 053: HLNAM
        case VBA5_OP_36_DictLdWith:         // 054: HLNAM
        case VBA5_OP_39_MemStWith:          // 057: HLNAM
        case VBA5_OP_3A_DictStWith:         // 058: HLNAM
        case VBA5_OP_3D_MemSetWith:         // 061: HLNAM
        case VBA5_OP_3E_DictSetWith:        // 062: HLNAM
        case VBA5_OP_45_Bos:                // 069: USHORT
        case VBA5_OP_52_Circle:             // 082: USHORT
        case VBA5_OP_53_Close:              // 083: USHORT
        case VBA5_OP_63_ElseIfTypeBlock:    // 099: USHORT
        case VBA5_OP_6F_Erase:              // 111: USHORT
        case VBA5_OP_83_FnLBound:           // 131: USHORT
        case VBA5_OP_8A_FnUBound:           // 138: USHORT
        case VBA5_OP_92_Gosub:              // 146: HLNAM
        case VBA5_OP_93_Goto:               // 147: HLNAM
        case VBA5_OP_96_TypeOf:             // 150: HIMPTYPE
        case VBA5_OP_97_IfTypeBlock:        // 151: HLNAM
        case VBA5_OP_9B_Label:              // 155: HLNAM
        case VBA5_OP_9D_Line:               // 157: USHORT
        case VBA5_OP_A0_LineNum:            // 160: HLNAM
        case VBA5_OP_A4_LitDI2:             // 164: USHORT
        case VBA5_OP_A6_LitHI2:             // 166: USHORT
        case VBA5_OP_A9_LitOI2:             // 169: USHORT
        case VBA5_OP_BE_New:                // 190: HIMPTYPE
        case VBA5_OP_C1_OnError:            // 193: HLNAM
        case VBA5_OP_C4_Open:               // 196: USHORT
        case VBA5_OP_C9_ParamNamed:         // 201: HLNAM
        case VBA5_OP_D6_Pset:               // 214: USHORT
        case VBA5_OP_DD_Resume:             // 221: HLNAM
        case VBA5_OP_E0_Scale:              // 224: USHORT
        case VBA5_OP_E3_SelectIs:           // 227: USHORT
        case VBA5_OP_F0_LbConst:            // 240: HLNAM
        case VBA5_OP_FA_SetOrSt:            // 250: HLNAM
        {
            // Get the single WORD argument

            if (O97ModAccGetWORD(lpstParse->lpstMod,
                                 lpstParse->dwLineOffset + lpstParse->dwOffset,
                                 &lpstParse->wArg0) == FALSE)
                return(FALSE);

            // Advance past single WORD argument

            lpstParse->dwOffset += 2;
            break;
        }

        // Two WORD arguments

        case VBA5_OP_24_ArgsLd:             // 036: HLNAM USHORT
        case VBA5_OP_25_ArgsMemLd:          // 037: HLNAM USHORT
        case VBA5_OP_26_ArgsDictLd:         // 038: HLNAM USHORT
        case VBA5_OP_2B_ArgsSt:             // 043: HLNAM USHORT
        case VBA5_OP_2C_ArgsMemSt:          // 044: HLNAM USHORT
        case VBA5_OP_2D_ArgsDictSt:         // 045: HLNAM USHORT
        case VBA5_OP_32_ArgsSet:            // 050: HLNAM USHORT
        case VBA5_OP_33_ArgsMemSet:         // 051: HLNAM USHORT
        case VBA5_OP_34_ArgsDictSet:        // 052: HLNAM USHORT
        case VBA5_OP_37_ArgsMemLdWith:      // 055: HLNAM USHORT
        case VBA5_OP_38_ArgsDictLdWith:     // 056: HLNAM USHORT
        case VBA5_OP_3B_ArgsMemStWith:      // 059: HLNAM USHORT
        case VBA5_OP_3C_ArgsDictStWith:     // 060: HLNAM USHORT
        case VBA5_OP_3F_ArgsMemSetWith:     // 063: HLNAM USHORT
        case VBA5_OP_40_ArgsDictSetWith:    // 064: HLNAM USHORT
        case VBA5_OP_41_ArgsCall:           // 065: HLNAM USHORT
        case VBA5_OP_42_ArgsMemCall:        // 066: HLNAM USHORT
        case VBA5_OP_43_ArgsMemCallWith:    // 067: HLNAM USHORT
        case VBA5_OP_44_ArgsArray:          // 068: HLNAM USHORT
        {
            // Get the two WORD arguments

            if (O97ModAccGetWORD(lpstParse->lpstMod,
                                 lpstParse->dwLineOffset + lpstParse->dwOffset,
                                 &lpstParse->wArg0) == FALSE)
                return(FALSE);

            // Advance past the first WORD argument

            lpstParse->dwOffset += 2;

            if (O97ModAccGetWORD(lpstParse->lpstMod,
                                 lpstParse->dwLineOffset + lpstParse->dwOffset,
                                 &lpstParse->wArg1) == FALSE)
                return(FALSE);

            // Advance past second WORD argument

            lpstParse->dwOffset += 2;
            break;
        }

        // Single DWORD argument

        case VBA5_OP_57_Context:            // 087: HCONTEXT
        case VBA5_OP_59_Deftype:            // 089: USHORT USHORT
        case VBA5_OP_8F_FuncDefn:           // 143: HFUNCDEFN
        case VBA5_OP_90_FuncDefnSave:       // 144: HFUNCDEFN
        case VBA5_OP_A5_LitDI4:             // 165: USHORT USHORT
        case VBA5_OP_A7_LitHI4:             // 167: USHORT USHORT
        case VBA5_OP_AA_LitOI4:             // 170: USHORT USHORT
        case VBA5_OP_AB_LitR4:              // 171: USHORT USHORT
        case VBA5_OP_E8_Type:               // 232: HRTDEFN
            // Get the single DWORD argument

            if (O97ModAccGetDWORD(lpstParse->lpstMod,
                                  lpstParse->dwLineOffset + lpstParse->dwOffset,
                                  &lpstParse->dwArg) == FALSE)
                return(FALSE);

            lpstParse->dwOffset += 4;
            break;

        // Three WORD arguments

        case VBA5_OP_E6_Stack:              // 230: USHORT USHORT USHORT
            lpstParse->dwOffset += 6;
            break;

        // Two WORD arguments and one DWORD argument

        case VBA5_OP_B7_MemRedim:           // 183: USHORT USHORT HTYPEDEFN
        case VBA5_OP_B8_MemRedimWith:       // 184: USHORT USHORT HTYPEDEFN
        case VBA5_OP_B9_MemRedimAs:         // 185: USHORT USHORT HTYPEDEFN
        case VBA5_OP_BA_MemRedimAsWith:     // 186: USHORT USHORT HTYPEDEFN
        case VBA5_OP_D9_Redim:              // 217: USHORT USHORT HTYPEDEFN
        case VBA5_OP_DA_RedimAs:            // 218: USHORT USHORT HTYPEDEFN
            lpstParse->dwOffset += 8;
            break;

        // One QWORD argument

        case VBA5_OP_A1_LitCy:              // 161: USHORT USHORT USHORT USHORT
        case VBA5_OP_A2_LitDate:            // 162: USHORT USHORT USHORT USHORT
        case VBA5_OP_AC_LitR8:              // 172: USHORT USHORT USHORT USHORT
            lpstParse->dwOffset += 8;
            break;

        // Variable

        case VBA5_OP_9E_LineCont:           // 158: VARIABLE
        case VBA5_OP_AE_LitStr:             // 174: VARIABLE
        case VBA5_OP_C2_OnGosub:            // 194: VARIABLE
        case VBA5_OP_C3_OnGoto:             // 195: VARIABLE
        case VBA5_OP_DB_Reparse:            // 219: VARIABLE
        case VBA5_OP_DC_Rem:                // 220: VARIABLE
        {
            if (O97ModAccGetWORD(lpstParse->lpstMod,
                                 lpstParse->dwLineOffset + lpstParse->dwOffset,
                                 &wSize) == FALSE)
                return(FALSE);

            if (wSize & 1)
                ++wSize;

            lpstParse->dwOffset += 2 + wSize;
            break;
        }

        case VBA5_OP_D8_QuoteRem:           // 216: VARIABLE
        {
            lpstParse->dwOffset += 2;
            if (O97ModAccGetWORD(lpstParse->lpstMod,
                                 lpstParse->dwLineOffset + lpstParse->dwOffset,
                                 &wSize) == FALSE)
                return(FALSE);

            if (wSize & 1)
                ++wSize;

            lpstParse->dwOffset += 2 + wSize;
            break;
        }

        case VBA5_OP_EA_VarDefn:            // 234: VARIABLE
        {
            if (lpstParse->abyOpcode[1] & 0x40)
                lpstParse->dwOffset += 6;
            else
                lpstParse->dwOffset += 4;

            break;
        }

        // No arguments
#if 0
        case VBA5_OP_00_Imp:                // 000:
        case VBA5_OP_01_Eqv:                // 001:
        case VBA5_OP_02_Xor:                // 002:
        case VBA5_OP_03_And:                // 003:
        case VBA5_OP_04_Or:                 // 004:
        case VBA5_OP_05_Eq:                 // 005:
        case VBA5_OP_06_Ne:                 // 006:
        case VBA5_OP_07_Le:                 // 007:
        case VBA5_OP_08_Ge:                 // 008:
        case VBA5_OP_09_Lt:                 // 009:
        case VBA5_OP_0A_Gt:                 // 010:
        case VBA5_OP_0B_Add:                // 011:
        case VBA5_OP_0C_Sub:                // 012:
        case VBA5_OP_0D_Mod:                // 013:
        case VBA5_OP_0E_IDv:                // 014:
        case VBA5_OP_0F_Mul:                // 015:
        case VBA5_OP_10_Div:                // 016:
        case VBA5_OP_11_Concat:             // 017:
        case VBA5_OP_12_Like:               // 018:
        case VBA5_OP_13_Pwr:                // 019:
        case VBA5_OP_14_Is:                 // 020:
        case VBA5_OP_15_Not:                // 021:
        case VBA5_OP_16_UMi:                // 022:
        case VBA5_OP_17_FnAbs:              // 023:
        case VBA5_OP_18_FnFix:              // 024:
        case VBA5_OP_19_FnInt:              // 025:
        case VBA5_OP_1A_FnSgn:              // 026:
        case VBA5_OP_1B_FnLen:              // 027:
        case VBA5_OP_1C_FnLenB:             // 028:
        case VBA5_OP_1D_Paren:              // 029:
        case VBA5_OP_1E_Sharp:              // 030:
        case VBA5_OP_46_BosImplicit:        // 070:
        case VBA5_OP_47_Bol:                // 071:
        case VBA5_OP_48_Case:               // 072:
        case VBA5_OP_49_CaseTo:             // 073:
        case VBA5_OP_4A_CaseGt:             // 074:
        case VBA5_OP_4B_CaseLt:             // 075:
        case VBA5_OP_4C_CaseGe:             // 076:
        case VBA5_OP_4D_CaseLe:             // 077:
        case VBA5_OP_4E_CaseNe:             // 078:
        case VBA5_OP_4F_CaseEq:             // 079:
        case VBA5_OP_50_CaseElse:           // 080:
        case VBA5_OP_51_CaseDone:           // 081:
        case VBA5_OP_54_CloseAll:           // 084:
        case VBA5_OP_55_Coerce:             // 085:
        case VBA5_OP_56_CoerceVar:          // 086:
        case VBA5_OP_58_Debug:              // 088:
        case VBA5_OP_5A_Dim:                // 090:
        case VBA5_OP_5B_DimImplicit:        // 091:
        case VBA5_OP_5C_Do:                 // 092:
        case VBA5_OP_5D_DoEvents:           // 093:
        case VBA5_OP_5E_DoUntil:            // 094:
        case VBA5_OP_5F_DoWhile:            // 095:
        case VBA5_OP_60_Else:               // 096:
        case VBA5_OP_61_ElseBlock:          // 097:
        case VBA5_OP_62_ElseIfBlock:        // 098:
        case VBA5_OP_64_End:                // 100:
        case VBA5_OP_65_EndContext:         // 101:
        case VBA5_OP_66_EndFunc:            // 102:
        case VBA5_OP_67_EndIf:              // 103:
        case VBA5_OP_68_EndIfBlock:         // 104:
        case VBA5_OP_69_EndImmediate:       // 105:
        case VBA5_OP_6A_EndProp:            // 106:
        case VBA5_OP_6B_EndSelect:          // 107:
        case VBA5_OP_6C_EndSub:             // 108:
        case VBA5_OP_6D_EndType:            // 109:
        case VBA5_OP_6E_EndWith:            // 110:
        case VBA5_OP_70_Error:              // 112:
        case VBA5_OP_71_ExitDo:             // 113:
        case VBA5_OP_72_ExitFor:            // 114:
        case VBA5_OP_73_ExitFunc:           // 115:
        case VBA5_OP_74_ExitProp:           // 116:
        case VBA5_OP_75_ExitSub:            // 117:
        case VBA5_OP_76_FnCurDir:           // 118:
        case VBA5_OP_77_FnDir:              // 119:
        case VBA5_OP_78_Empty0:             // 120:
        case VBA5_OP_79_Empty1:             // 121:
        case VBA5_OP_7A_FnError:            // 122:
        case VBA5_OP_7B_FnFormat:           // 123:
        case VBA5_OP_7C_FnFreeFile:         // 124:
        case VBA5_OP_7D_FnInStr:            // 125:
        case VBA5_OP_7E_FnInStr3:           // 126:
        case VBA5_OP_7F_FnInStr4:           // 127:
        case VBA5_OP_80_FnInStrB:           // 128:
        case VBA5_OP_81_FnInStrB3:          // 129:
        case VBA5_OP_82_FnInStrB4:          // 130:
        case VBA5_OP_84_FnMid:              // 132:
        case VBA5_OP_85_FnMidB:             // 133:
        case VBA5_OP_86_FnStrComp:          // 134:
        case VBA5_OP_87_FnStrComp3:         // 135:
        case VBA5_OP_88_FnStringVar:        // 136:
        case VBA5_OP_89_FnStringStr:        // 137:
        case VBA5_OP_8B_For:                // 139:
        case VBA5_OP_8C_ForEach:            // 140:
        case VBA5_OP_8D_ForEachAs:          // 141:
        case VBA5_OP_8E_ForStep:            // 142:
        case VBA5_OP_91_GetRec:             // 145:
        case VBA5_OP_94_If:                 // 148:
        case VBA5_OP_95_IfBlock:            // 149:
        case VBA5_OP_98_Input:              // 152:
        case VBA5_OP_99_InputDone:          // 153:
        case VBA5_OP_9A_InputItem:          // 154:
        case VBA5_OP_9C_Let:                // 156:
        case VBA5_OP_9F_LineInput:          // 159:
        case VBA5_OP_A3_LitDefault:         // 163:
        case VBA5_OP_A8_LitNothing:         // 168:
        case VBA5_OP_AD_LitSmallI2:         // 173:
        case VBA5_OP_AF_LitVarSpecial:      // 175:
        case VBA5_OP_B0_Lock:               // 176:
        case VBA5_OP_B1_Loop:               // 177:
        case VBA5_OP_B2_LoopUntil:          // 178:
        case VBA5_OP_B3_LoopWhile:          // 179:
        case VBA5_OP_B4_Lset:               // 180:
        case VBA5_OP_B5_Me:                 // 181:
        case VBA5_OP_B6_MeImplicit:         // 182:
        case VBA5_OP_BB_Mid:                // 187:
        case VBA5_OP_BC_MidB:               // 188:
        case VBA5_OP_BD_Name:               // 189:
        case VBA5_OP_BF_Next:               // 191:
        case VBA5_OP_C0_NextVar:            // 192:
        case VBA5_OP_C5_Option:             // 197:
        case VBA5_OP_C6_OptionBase:         // 198:
        case VBA5_OP_C7_ParamByval:         // 199:
        case VBA5_OP_C8_ParamOmitted:       // 200:
        case VBA5_OP_CA_PrintChan:          // 202:
        case VBA5_OP_CB_PrintComma:         // 203:
        case VBA5_OP_CC_PrintEos:           // 204:
        case VBA5_OP_CD_PrintItemComma:     // 205:
        case VBA5_OP_CE_PrintItemNL:        // 206:
        case VBA5_OP_CF_PrintItemSemi:      // 207:
        case VBA5_OP_D0_PrintNL:            // 208:
        case VBA5_OP_D1_PrintObj:           // 209:
        case VBA5_OP_D2_PrintSemi:          // 210:
        case VBA5_OP_D3_PrintSpc:           // 211:
        case VBA5_OP_D4_PrintTab:           // 212:
        case VBA5_OP_D5_PrintTabComma:      // 213:
        case VBA5_OP_D7_PutRec:             // 215:
        case VBA5_OP_DE_Return:             // 222:
        case VBA5_OP_DF_Rset:               // 223:
        case VBA5_OP_E1_Seek:               // 225:
        case VBA5_OP_E2_SelectCase:         // 226:
        case VBA5_OP_E4_SelectType:         // 228:
        case VBA5_OP_E5_SetStmt:            // 229:
        case VBA5_OP_E7_Stop:               // 231:
        case VBA5_OP_E9_UnLock:             // 233:
        case VBA5_OP_EB_Wend:               // 235:
        case VBA5_OP_EC_While:              // 236:
        case VBA5_OP_ED_With:               // 237:
        case VBA5_OP_EE_WriteChan:          // 238:
        case VBA5_OP_EF_ConstFuncExpr:      // 239:
        case VBA5_OP_F1_LbIf:               // 241:
        case VBA5_OP_F2_LbElse:             // 242:
        case VBA5_OP_F3_LbElseif:           // 243:
        case VBA5_OP_F4_LbEndif:            // 244:
        case VBA5_OP_F5_LbMark:             // 245:
        case VBA5_OP_F6_EndForVariable:     // 246:
        case VBA5_OP_F7_StartForVariable:   // 247:
        case VBA5_OP_F8_NewRedim:           // 248:
        case VBA5_OP_F9_StartWithExpr:      // 249:
        case VBA5_OP_FB_EndEnum:            // 251:
        case VBA5_OP_FC_Illegal:            // 252:
#endif
        default:
            break;
    }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  BOOL VBAInitKnownIdent()
//
// Parameters:
//  lpstStream              Ptr to _VBA_PROJECT stream
//  lplpabyKnownIdentSet    Known identifier set
//  nNumKnownIdents         Number of known identifiers
//  lpawKnownIdentID        Array to hold known IDs
//  lpawIdentEnum           Array to hold enumerated IDs found
//
// Description:
//  This function searches enumerates through the identifier table
//  and notes the IDs of the known identifiers.
//
// Returns:
//  TRUE                    On success
//  FALSE                   On error
//
//*************************************************************************

BOOL VBAInitKnownIdent
(
    LPSS_STREAM     lpstStream,
    LPLPBYTE        lplpabyKnownIdentSet,
    int             nNumKnownIdents,
    LPWORD          lpawKnownIdentID,
    LPWORD          lpawIdentEnum
)
{
    BOOL            bLittleEndian;
    DWORD           dwIdentTableOffset;
    DWORD           dwNumIdents;
    WORD            wID;
    DWORD           dw;
    BYTE            abyName[256];
    DWORD           dwBytesRead;
    int             n;
    LPBYTE          lpbyCand;
    int             i;
    int             nGap, j;

    // Initialize arrays

    for (i=0;i<nNumKnownIdents;i++)
    {
        lpawKnownIdentID[i] = 0;
        lpawIdentEnum[i] = (WORD)i;
    }

    /////////////////////////////////////////////////////////////
    // Enumerate through identifier table
    /////////////////////////////////////////////////////////////

    // Determine the stream's endian state

    if (O97Get_VBA_PROJECTEndian(lpstStream,
                                 NULL,
                                 &bLittleEndian) == FALSE)
    {
        // Couldn't get the endian state

        return(FALSE);
    }

    // Get identifier table location

    if (O97GetIdentifierTableOffset(lpstStream,
                                    &dwIdentTableOffset,
                                    &dwNumIdents) == FALSE)
    {
        // Couldn't find identifier table

        return(FALSE);
    }

    // Enumerate through identifiers

    wID = 0x200;
    for (dw=0;dw<dwNumIdents;dw++,wID+=2)
    {
        /////////////////////////////////////////////////////////
        // Get the identifier

        // Get the string length and the flag byte

        if (SSSeekRead(lpstStream,
                       dwIdentTableOffset,
                       abyName,
                       2,
                       &dwBytesRead) != SS_STATUS_OK)
            break;

        // Endianize

        if (bLittleEndian == FALSE)
        {
            BYTE        byTemp;

            byTemp = abyName[0];
            abyName[0] = abyName[1];
            abyName[1] = byTemp;
        }

        // If the high bit on the flag byte is set, then skip
        //  the extra bytes

        if (abyName[1] & 0x80)
            dwIdentTableOffset += 8;
        else
            dwIdentTableOffset += 2;

        // Read the identifier

        if (SSSeekRead(lpstStream,
                       dwIdentTableOffset,
                       abyName + 1,
                       abyName[0],
                       &dwBytesRead) != SS_STATUS_OK)
            break;

        // Move over the string and the end bytes

        dwIdentTableOffset += abyName[0] + 4;

        // First lower case the name

        for (n=1;n<=abyName[0];n++)
        {
            if (abyName[n] <= 0x5A && // 'Z'
                abyName[n] >= 0x41)   // 'A'
                abyName[n] += (0x61 - 0x41); // ('a' - 'A')
        }

        /////////////////////////////////////////////////////////
        // See if it is a known identifier

        // Use a linear search

        for (n=0;n<nNumKnownIdents;n++)
        {
            lpbyCand = lplpabyKnownIdentSet[n];
            if (lpbyCand[0] == abyName[0])
            {
                for (i=1;i<=abyName[0];i++)
                    if (lpbyCand[i] != abyName[i])
                        break;

                if (i > abyName[0])
                {
                    // Found a match

                    lpawKnownIdentID[n] = wID;
                    break;
                }
            }
        }
    }

    // Do a shell sort on the identifiers

    for (nGap=nNumKnownIdents/2;nGap>0;nGap/=2)
        for (i=nGap;i<nNumKnownIdents;i++)
        {
            j = i - nGap;
            while (j >= 0 && lpawKnownIdentID[j] > lpawKnownIdentID[j+nGap])
            {
                // Swap

                wID = lpawKnownIdentID[j];
                lpawKnownIdentID[j] = lpawKnownIdentID[j+nGap];
                lpawKnownIdentID[j+nGap] = wID;

                wID = lpawIdentEnum[j];
                lpawIdentEnum[j] = lpawIdentEnum[j+nGap];
                lpawIdentEnum[j+nGap] = wID;

                j -= nGap;
            }
        }

    return(TRUE);
}


//*************************************************************************
//
// Function:
//  WORD VBAGetKnownIdentEnum()
//
// Parameters:
//  nNumKnownIdents         Number of known identifiers
//  lpawKnownIdentID        ID's of known identifiers
//  lpawIdentEnum           Enumerated values of known identifiers
//  wID                     ID to search for
//
// Description:
//  This function searches through the known identifier IDs and returns
//  the enumeration ID for it.  If the ID is not known the function
//  returns 0xFFFF.
//
// Returns:
//  WORD                    Enumeration number
//  0xFFFF                  If unknown
//
//*************************************************************************

WORD VBAGetKnownIdentEnum
(
    int             nNumKnownIdents,
    LPWORD          lpawKnownIdentID,
    LPWORD          lpawIdentEnum,
    WORD            wID
)
{
    long            lLow, lMid, lHigh;

    // Do a binary search through the known identifiers

    lLow = 0;
    lHigh = nNumKnownIdents - 1;
    while (lLow <= lHigh)
    {
        lMid = (lLow + lHigh) / 2;

        if (wID < lpawKnownIdentID[lMid])
        {
            // In lower half

            lHigh = lMid - 1;
        }
        else
        if (wID > lpawKnownIdentID[lMid])
        {
            // In upper half

            lLow = lMid + 1;
        }
        else
        {
            // Found a match

            return lpawIdentEnum[lMid];
        }
    }

    return(0xFFFF);
}

