//************************************************************************
//
// $Header:   S:/OLESSAPI/VCS/wd7api.cpv   1.21   21 Sep 1998 16:10:06   DCHI  $
//
// Description:
//  Core Word 6.0/7.0/95 access implementation.
//
//************************************************************************
// $Log:   S:/OLESSAPI/VCS/wd7api.cpv  $
// 
//    Rev 1.21   21 Sep 1998 16:10:06   DCHI
// Added WD7GetMCDInfo() implementation.
// 
//    Rev 1.20   10 Jun 1998 12:41:50   DCHI
// Added checks for valid name table values.
// 
//    Rev 1.19   14 May 1998 18:52:18   DCHI
// Modified apply DeleteDelta to return OK if there were no menu mods.
// 
//    Rev 1.18   29 Apr 1998 17:43:16   DCHI
// In DeleteDelta(), assume all other menu operation matches are okay
// regardless of specified name.
// 
//    Rev 1.17   03 Dec 1997 18:19:30   ECHIEN
// Fixed two bugs:
//   1. Byte ordering of paramater constants in WD7DeleteDelta
//   2. String comparision in WD7CheckMenuName (wasn't incrementing labyMenuName
// 
//    Rev 1.16   20 Nov 1997 16:03:16   ECHIEN
// Added WD7DeleteDelta() and supporting functions.
// 
//    Rev 1.15   20 Nov 1997 11:54:52   DCHI
// Removed unreferenced local variables.
// 
//    Rev 1.14   20 Nov 1997 11:50:08   DCHI
// Added WD7GetULNameAtIndex().  WD7GetULMacro{Name,Desc}AtIndex() now call
// WD7GetULNameAtIndex() to get the name/description.
// 
//    Rev 1.13   01 Oct 1997 15:00:20   DCHI
// Changed three return 0 statements to return(WD7_STATUS_ERROR).
// 
//    Rev 1.12   16 Sep 1997 14:58:18   DCHI
// Removed unreferenced local var from WD7SetTDTSize().
// 
//    Rev 1.11   16 Sep 1997 14:53:50   DCHI
// Added WD7SetTDTSize().
// 
//    Rev 1.10   15 Sep 1997 10:56:28   DCHI
// Really corrected problem with incorrect termination when searching for the
// next macro and the macro name is not found.
// 
//    Rev 1.9   15 Sep 1997 10:32:44   DCHI
// Corrected problem with incorrect termination when searching for the
// next macro and the macro name is not found.
// 
//    Rev 1.8   08 Jul 1997 17:18:20   DCHI
// Corrected problem in DeactivateMacro() in which stMCD structure needs
// to be reread because it is overwritten in the EncryptedWrite() call.
// 
//    Rev 1.7   13 Jun 1997 12:47:08   DCHI
// Added CRC function.
// 
//    Rev 1.6   09 Jun 1997 14:14:20   DCHI
// Removed unreferenced local from WD7SetULMacroNameAtIndex().
// 
//    Rev 1.5   02 Jun 1997 17:33:42   DCHI
// Added WD7GetULMacroDescAtIndex() and modified WD7GetMacroInfoAtULName()
// to accept NULL for info fields not desired.
// 
//    Rev 1.4   30 May 1997 11:36:34   DCHI
// Added WD7SetMacroNameAtIndex(), WD7GetMacroInfoAtULName(),
// WD7GetMacroInfoAtULName(), and WD7SetULMacroNameAtIndex().
// 
//    Rev 1.3   13 May 1997 17:26:48   DCHI
// Added WD7HasAutoText() function.
// 
//    Rev 1.2   13 May 1997 17:09:38   DCHI
// Corrected scan structure initialization loop for parsing through names.
// 
//    Rev 1.1   06 May 1997 12:26:08   DCHI
// Removed unreferenced local variables.
// 
//    Rev 1.0   05 May 1997 13:58:26   DCHI
// Initial revision.
// 
//************************************************************************

#include "wd7api.h"
#include "crc32.h"

//********************************************************************
//
// Function:
//  WD7_STATUS WD7AllocMacroScanStruct()
//
// Parameters:
//  lpvRootCookie       Root cookie for callback function
//  lplpstMacroScan     Ptr to ptr for the stream structure
//  wMaxBufferedMCDs    Maximum number of MCDs to buffer
//  wNameBufferSize     Maximum number of name bytes to buffer
//
// Description:
//  Allocates memory for the macro scan structure as well as the
//  MCD and name buffers.
//
//  The function allocates at least enough buffer space for one
//  MCD and 260 bytes for the name buffer.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7AllocMacroScanStruct
(
    LPVOID              lpvRootCookie,
    LPLPWD7_MACRO_SCAN  lplpstMacroScan,
    WORD                wMaxBufferedMCDs,
    WORD                wNameBufferSize
)
{
    LPWD7_MACRO_SCAN    lpstMacroScan;

    if (wMaxBufferedMCDs == 0)
        wMaxBufferedMCDs = 1;

    if (wNameBufferSize < 260)
        wNameBufferSize = 260;

    // Allocate the memory

    if (SSMemoryAlloc(lpvRootCookie,
                      sizeof(WD7_MACRO_SCAN_T) +
                          wMaxBufferedMCDs * sizeof(WD7_MCD_T) +
                          wNameBufferSize * sizeof(BYTE),
                      (LPLPVOID)&lpstMacroScan) != SS_STATUS_OK)
    {
        return(WD7_STATUS_ERROR);
    }

    // Assign the memory

    lpstMacroScan->wMaxBufferedMCDs = wMaxBufferedMCDs;
    lpstMacroScan->lpstMCDs = (LPWD7_MCD)(lpstMacroScan + 1);

    lpstMacroScan->wMaxNameBufferSize = wNameBufferSize;
    lpstMacroScan->lpbyNames = (LPBYTE)
        (lpstMacroScan->lpstMCDs + wMaxBufferedMCDs);

    *lplpstMacroScan = lpstMacroScan;

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7FreeMacroScanStruct()
//
// Parameters:
//  lpvRootCookie       Root cookie for callback function
//  lpstMacroScan       Ptr to the stream structure to free
//
// Description:
//  Frees the macro scan structure memory.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7FreeMacroScanStruct
(
    LPVOID              lpvRootCookie,
    LPWD7_MACRO_SCAN    lpstMacroScan
)
{
    // Free the macro scan structure memory

    if (SSMemoryFree(lpvRootCookie,
                     lpstMacroScan) != SS_STATUS_OK)
    {
        return(WD7_STATUS_ERROR);
    }

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  BOOL WD7IsKnownMagicNumber()
//
// Parameters:
//  wMagicNumber        The magic number
//
// Description:
//  The function returns TRUE if the given magic number is in
//  the set of known magic numbers for Word 6.0/7.0/95.
//
// Returns:
//  TRUE        If the given magic number is in the known set
//  FALSE       If the given magic number is not in the known set
//
//********************************************************************

BOOL WD7IsKnownMagicNumber
(
    WORD                wMagicNumber
)
{
    switch (wMagicNumber)
    {
        case 0xA5DC:        // English version of Word 6.0 & 7.0
                            // Danish
                            // Thai
                            // Finnish
                            // Italian
                            // Swedish
                            // Brazilian
                            // Finnish
                            // French
                            // German
                            // Norwegian
                            // Polish
                            // Russian
                            // Spanish
                            // Turkish
        case 0xA697:        // Japanese
        case 0xA698:        // Korean Word 6.0 for Windows
        case 0xA699:        // Chinese
        case 0x8099:        // Chinese simplified
        case 0x8098:        // Korean
            return(TRUE);

        default:
            // Unrecognized
            return(FALSE);
    }
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7MacroScanInit()
//
// Parameters:
//  lpstStream          Ptr to WordDocument stream structure
//  lpstKey             Key to use for encrypted reads
//  lpstMacroScan       Ptr to acro scan structure
//  lpbHasMacros        Ptr to BOOL set to TRUE if macros present
//
// Description:
//  Parses through the template data table and buffers as much
//  of the MCDs and macro names as possible.
//
//  This function is called to initialize the scan structure
//  before calling WD7GetNextMacroForScan() to iterate through
//  the valid/active macros.
//
//  On success, *lpbHasMacros is set to TRUE if the document has
//  macros, FALSE otherwise.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error parsing the TDT
//
//********************************************************************

WD7_STATUS WD7MacroScanInit
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_MACRO_SCAN    lpstMacroScan,
    LPBOOL              lpbHasMacros
)
{
    BYTE                byTemp;
    BYTE                byRecType;
    WORD                wTemp;
    WORD                wCount;
    WORD                wIndex;
    DWORD               dwRecSize;
    DWORD               dwOffset;
    DWORD               dwTDTOffset;
    DWORD               dwTDTSize;
    DWORD               dwTDTLimit;
    BYTE                abyWorkBuffer[256];

    // Get the offset and size of the template data table

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         280,
                         (LPBYTE)&dwTDTOffset,
                         sizeof(DWORD)) != sizeof(DWORD) ||
        WD7EncryptedRead(lpstStream,
                         lpstKey,
                         284,
                         (LPBYTE)&dwTDTSize,
                         sizeof(DWORD)) != sizeof(DWORD))
    {
        // Failed to get offset and size of template data table

        return(WD7_STATUS_ERROR);
    }

    dwTDTOffset = DWENDIAN(dwTDTOffset);
    dwTDTSize = DWENDIAN(dwTDTSize);

    // If the size is <= 2, there are no macros

    if (dwTDTSize <= 2)
    {
        *lpbHasMacros = FALSE;
        return(WD7_STATUS_OK);
    }

    // Store stream and key

    lpstMacroScan->lpstStream = lpstStream;
    lpstMacroScan->lpstKey = lpstKey;

    // Initialize fields of scan structure

    lpstMacroScan->dwMCDTableOffset = 0xFFFFFFFF;
    lpstMacroScan->wNumMCDs = 0;
    lpstMacroScan->wNumBufferedMCDs = 0;
    lpstMacroScan->wNextMCDScanIndex = 0;

    lpstMacroScan->dwMacroNameTableOffset = 0xFFFFFFFF;
    lpstMacroScan->dwMacroNameTableLimit = 0xFFFFFFFF;
    lpstMacroScan->wNumMacroNames = 0;
    lpstMacroScan->wNumBufferedNameBytes = 0;
    lpstMacroScan->wNextNameIndex = 0;
    lpstMacroScan->wNextNameOffset = 0;

    // Verify that the first byte of the template data table is 0xFF

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwTDTOffset,
                         &byTemp,
                         sizeof(BYTE)) != sizeof(BYTE) ||
		byTemp != 0xFF)
        return(WD7_STATUS_ERROR);

	// Now parse through the records in the table

    dwTDTLimit = dwTDTOffset + dwTDTSize;
    dwOffset = dwTDTOffset + 1;
    while (dwOffset < dwTDTLimit)
	{
		// Read the record type

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwOffset++,
                             &byRecType,
                             sizeof(BYTE)) != sizeof(BYTE))
            return(WD7_STATUS_ERROR);

		if (byRecType == 0x11)
		{
            /////////////////////////////////////////////////////
            // This is the macro name table

			// Read number of macros

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&(lpstMacroScan->wNumMacroNames),
                                 sizeof(WORD)) != sizeof(WORD))
            {
                return(WD7_STATUS_ERROR);
            }

            // Store number of macro names

            lpstMacroScan->wNumMacroNames =
                WENDIAN(lpstMacroScan->wNumMacroNames);

            if (lpstMacroScan->wNumMacroNames == 0)
            {
                // No macro names

                return(WD7_STATUS_ERROR);
            }

            dwOffset += sizeof(WORD);

            // Store offset of first name

            lpstMacroScan->dwMacroNameTableOffset = dwOffset;

            // Read as much of the macro name table as possible

            if ((dwTDTLimit - dwOffset) >
                lpstMacroScan->wMaxNameBufferSize)
            {
                // Read up to the maximum buffer size

                wCount = lpstMacroScan->wMaxNameBufferSize;
            }
            else
            {
                // Read to the end

                wCount = (WORD)(dwTDTLimit - dwOffset);
            }

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 lpstMacroScan->lpbyNames,
                                 wCount) != wCount)
            {
                return(WD7_STATUS_ERROR);
            }

            lpstMacroScan->dwFirstBufferedNameOffset = dwOffset;
            lpstMacroScan->wFirstBufferedNameIndex = 0;
            lpstMacroScan->wNumBufferedNameBytes = wCount;

            // Now parse through buffered names, updating real
            //  offset as we do so

            wTemp = 0;
            wIndex = 0;
            while ((wTemp + 2) < wCount &&
                   wIndex < lpstMacroScan->wNumMacroNames)
            {
                // Increment past ID

                wTemp += 2;

                // Update offset

                dwOffset += 2 + 1 + 1 +
                    lpstMacroScan->lpbyNames[wTemp];

                wTemp += 1 + 1 + lpstMacroScan->lpbyNames[wTemp];

                // Increment index

                ++wIndex;
            }

            // Skip over unbuffered names

            while (wIndex < lpstMacroScan->wNumMacroNames)
			{
                // Skip over ID

				dwOffset += 2;

                // Get name length

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     &byTemp,
                                     sizeof(BYTE)) != sizeof(BYTE))
                    return(WD7_STATUS_ERROR);

                // Skip over length, name, and zero-terminator

                dwOffset += 1 + 1 + byTemp;

                // Increment index

                ++wIndex;
			}

            // Remember macro name table limit

            lpstMacroScan->dwMacroNameTableLimit = dwOffset;

            // Make sure the buffered byte count does not exceed the limit

            if ((lpstMacroScan->dwMacroNameTableLimit -
                 lpstMacroScan->dwMacroNameTableOffset) <
                 lpstMacroScan->wNumBufferedNameBytes)
                 lpstMacroScan->wNumBufferedNameBytes = (WORD)
                    (lpstMacroScan->dwMacroNameTableLimit -
                     lpstMacroScan->dwMacroNameTableOffset);
        }
		else
		if (byRecType >= 0x01 && byRecType <= 0x05)
		{
			switch (byRecType)
			{
				case 0x01:
                    dwRecSize = 24;
					break;

				case 0x02:
                    dwRecSize = 4;
					break;

				case 0x03:
                    dwRecSize = 14;
                    break;

                case 0x04:
                    dwRecSize = 14;
					break;

				default:
                    dwRecSize = 12;
					break;
			}

			// Read count

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wCount,
                                 sizeof(WORD)) != sizeof(WORD))
                return(WD7_STATUS_ERROR);

            wCount = WENDIAN(wCount);

            dwOffset += 2;

			if (byRecType == 0x01)
			{
				// MCD table

                lpstMacroScan->dwMCDTableOffset = dwOffset;
                lpstMacroScan->wNumMCDs = wCount;

                // Calculate how many MCDs to buffer

                if (wCount > lpstMacroScan->wMaxBufferedMCDs)
                    lpstMacroScan->wNumBufferedMCDs =
                        lpstMacroScan->wMaxBufferedMCDs;
                else
                    lpstMacroScan->wNumBufferedMCDs = wCount;

                // Buffer the MCDs

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)lpstMacroScan->lpstMCDs,
                                     lpstMacroScan->wNumBufferedMCDs *
                                         sizeof(WD7_MCD_T)) !=
                    lpstMacroScan->wNumBufferedMCDs * sizeof(WD7_MCD_T))
                {
                    return(WD7_STATUS_ERROR);
                }

                lpstMacroScan->wBufferedMCDStartIndex = 0;
			}

            // Now skip that many records

			dwOffset += wCount * dwRecSize;
		}
		else
		if (byRecType == 0x06)
		{
			WORD	cstr, cbTdsh, cbBts, ctb, cbut;

            cstr = 0;

			// Read parameters

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 abyWorkBuffer,
                                 8) != 8)
                return(WD7_STATUS_ERROR);

			dwOffset += 8;

            cbTdsh = WENDIAN(((LPWORD)abyWorkBuffer)[0]);
            cbBts = WENDIAN(((LPWORD)abyWorkBuffer)[1]);
            ctb = WENDIAN(((LPWORD)abyWorkBuffer)[3]);

            while (ctb-- != 0)
			{
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wTemp,
                                     2) != 2)
                    return(WD7_STATUS_ERROR);

                wTemp = WENDIAN(wTemp);

				dwOffset += cbTdsh;

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&cbut,
                                     2) != 2)
                    return(WD7_STATUS_ERROR);

                cbut = WENDIAN(cbut);

                dwOffset += 2;

				while (cbut-- != 0)
				{
					if (wTemp != 0xFFFF)
						dwOffset += 6;

                    if (WD7EncryptedRead(lpstStream,
                                         lpstKey,
                                         dwOffset,
                                         abyWorkBuffer,
                                         12) != 12)
                        return(WD7_STATUS_ERROR);

                    dwOffset += cbBts;

                    if ((abyWorkBuffer[11] & 0xC0) == 0x40)
						++cstr;
				}
			}

			while (cstr-- != 0)
			{
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&byTemp,
                                     1) != 1)
                    return(WD7_STATUS_ERROR);

				dwOffset += 1 + byTemp;
			}
		}
		else
		if (byRecType == 0x10)
		{
            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wCount,
                                 2) != 2)
                return(WD7_STATUS_ERROR);

            wCount = WENDIAN(wCount);

            // For Chinese version of Word, found that
            //  0xFFFF means Unicode strings?

            if (wCount == 0xFFFF)
            {
                dwOffset += 2;

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wCount,
                                     2) != 2)
                    return(WD7_STATUS_ERROR);

                wCount = WENDIAN(wCount) - 2;
            }

			dwOffset += wCount;
		}
		else
		if (byRecType == 0x12)
		{
			dwOffset += 2;
		}
		else
		if (byRecType == 0x40)
		{
            dwOffset += 2;
			break;
		}
		else
		{
            // Invalid, assume end of table

            break;
		}
	}

    // Check for missing tables and/or no macros

    if (lpstMacroScan->dwMCDTableOffset == 0xFFFFFFFF ||
        lpstMacroScan->wNumMCDs == 0)
    {
        // No macros

        *lpbHasMacros = FALSE;
        return(WD7_STATUS_OK);
    }

    // At this point we know that we have the offset of the MCDs
    //  and we know that there is at least one MCD

    *lpbHasMacros = TRUE;
    if (lpstMacroScan->dwMacroNameTableOffset == 0xFFFFFFFF ||
        lpstMacroScan->wNumMacroNames == 0)
    {
        // Missing macro name table

        lpstMacroScan->wNumMacroNames = 0;
    }

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7GetNextMacroForScan()
//
// Parameters:
//  lpstMacroScan       Ptr to macro scan structure
//  lpbyName            Ptr to buffer for name >= 256 bytes in size
//  lpdwOffset          Ptr to DWORD to store macro offset
//  lpdwSize            Ptr to DWORD to store macro size
//  lpbyEncryptByte     Ptr to BYTE to store encryption byte
//  lpbNoMore           Ptr to BOOL, set to TRUE if no more macros
//
// Description:
//  The function returns information for the next macro to scan.
//  WD7MacroScanInit() must have been called to initialize the
//  scan structure before the first call to this function.
//
//  The order in which the macros are scanned is the same as the
//  order of the MCDs in the TDT.  MCDs with a dwN field of zero
//  are assumed to be invalidated macros and thus information
//  for such macros is not returned.
//
//  Given the MCD, the wID field is used to search for the name
//  in the macro name table.  First the buffered names are searched
//  starting from where the last search left off.  If the name
//  is not found in the buffered names, more names are buffered
//  and searched starting from the end of the last set of buffered
//  names.  If it turns out that no names have the proper wID,
//  a zero length name is returned.
//
//  The name is always zero terminated and the name can never be
//  longer than 255 bytes (excluding the zero-terminator), so lpbyName
//  must point to a buffer at least 256 bytes in size.
//
//  The function sets *lpbNoMore to TRUE if there are no more
//  macros to scan.
//
//  If the function returns WD7_STATUS_ERROR, the caller should
//  not attempt to call the function again, as the error could be
//  such that the state could not be properly updated to iterate
//  to the next macro.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7GetNextMacroForScan
(
    LPWD7_MACRO_SCAN    lpstMacroScan,
    LPBYTE              lpbyName,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize,
    LPBYTE              lpbyEncryptByte,
    LPBOOL              lpbNoMore
)
{
    LPWD7_MCD           lpstMCD;
    BYTE                byID[2];
    WORD                wFirstSearchIndex;
    WORD                wNextOffset;
    LPBYTE              lpbyNames;
    WORD                w, wLen;
    BOOL                bCheckedAtLeastOne;

    //////////////////////////////////////////////////////////////////
    // Get the next active/valid MCD
    //////////////////////////////////////////////////////////////////

    *lpbNoMore = FALSE;
    while (1)
    {
        /////////////////////////////////////////////////////////
        // Are there more macros?

        if ((lpstMacroScan->wBufferedMCDStartIndex +
             lpstMacroScan->wNextMCDScanIndex) >= lpstMacroScan->wNumMCDs)
        {
            // No more macros to scan

            *lpbNoMore = TRUE;
            return(WD7_STATUS_OK);
        }

        /////////////////////////////////////////////////////////
        // Does a new set of MCDs need to be read?

        if (lpstMacroScan->wNextMCDScanIndex >=
            lpstMacroScan->wNumBufferedMCDs)
        {
            // Read a new set of MCDs

            lpstMacroScan->wBufferedMCDStartIndex +=
                lpstMacroScan->wMaxBufferedMCDs;

            // How many?

            if ((lpstMacroScan->wBufferedMCDStartIndex +
                 lpstMacroScan->wNextMCDScanIndex) >= lpstMacroScan->wNumMCDs)
            {
                lpstMacroScan->wNumBufferedMCDs =
                    lpstMacroScan->wNumMCDs -
                    lpstMacroScan->wBufferedMCDStartIndex;
            }
            else
            {
                lpstMacroScan->wNumBufferedMCDs =
                    lpstMacroScan->wMaxBufferedMCDs;
            }

            // Buffer the MCDs

            if (WD7EncryptedRead(lpstMacroScan->lpstStream,
                                 lpstMacroScan->lpstKey,
                                 lpstMacroScan->dwMCDTableOffset +
                                     lpstMacroScan->wBufferedMCDStartIndex *
                                     (DWORD)sizeof(WD7_MCD_T),
                                 (LPBYTE)lpstMacroScan->lpstMCDs,
                                 lpstMacroScan->wNumBufferedMCDs *
                                     sizeof(WD7_MCD_T)) !=
                lpstMacroScan->wNumBufferedMCDs * sizeof(WD7_MCD_T))
            {
                return(WD7_STATUS_ERROR);
            }

            lpstMacroScan->wNextMCDScanIndex = 0;
        }

        /////////////////////////////////////////////////////////
        // We must have a valid MCD at this point

        lpstMCD = lpstMacroScan->lpstMCDs +
            lpstMacroScan->wNextMCDScanIndex++;

        // Check to see whether it is an active/valid MCD

        if (lpstMCD->dwN != 0)
        {
            // Active/valid macro

            break;
        }
    }

    // Set the macro fields

    *lpdwOffset = DWENDIAN(lpstMCD->dwOffset);
    *lpdwSize = DWENDIAN(lpstMCD->dwSize);
    *lpbyEncryptByte = lpstMCD->byEncrypt;

    //////////////////////////////////////////////////////////////////
    // Get the macro's name
    //////////////////////////////////////////////////////////////////

    if (lpstMacroScan->wNumMacroNames == 0)
    {
        // Just return an empty name if the macro name table is missing

        *lpbyName = 0;
        return(WD7_STATUS_OK);
    }

    byID[0] = ((LPBYTE)&lpstMCD->wID)[0];
    byID[1] = ((LPBYTE)&lpstMCD->wID)[1];

    wFirstSearchIndex =
        lpstMacroScan->wFirstBufferedNameIndex +
        lpstMacroScan->wNextNameIndex;

    if (wFirstSearchIndex == lpstMacroScan->wNumMacroNames)
    {
        // Start from the beginning

        wFirstSearchIndex = 0;
        lpstMacroScan->wNumBufferedNameBytes = 0;
    }

    lpbyNames = lpstMacroScan->lpbyNames;
    wNextOffset = lpstMacroScan->wNextNameOffset;

    bCheckedAtLeastOne = FALSE;

    while (1)
    {
        // A refill needs to be done if the length byte and the zero-
        //  terminator do not fit or if the full name does not fit

        if ((wNextOffset + 4) >= lpstMacroScan->wNumBufferedNameBytes ||
            (wNextOffset + lpbyNames[wNextOffset+2] + 4) >=
            lpstMacroScan->wNumBufferedNameBytes)
        {
            // Refill

            lpstMacroScan->dwFirstBufferedNameOffset += wNextOffset;
            if (lpstMacroScan->dwFirstBufferedNameOffset ==
                lpstMacroScan->dwMacroNameTableLimit)
            {
                // Start over from the beginning

                lpstMacroScan->dwFirstBufferedNameOffset =
                    lpstMacroScan->dwMacroNameTableOffset;

                lpstMacroScan->wFirstBufferedNameIndex = 0;
            }
            else
            {
                // Continue from the last point

                lpstMacroScan->wFirstBufferedNameIndex +=
                    lpstMacroScan->wNextNameIndex;
            }

            // Find out how many bytes we can read

            if (lpstMacroScan->dwFirstBufferedNameOffset +
                lpstMacroScan->wMaxNameBufferSize >=
                lpstMacroScan->dwMacroNameTableLimit)
            {
                // Limit to the end

                lpstMacroScan->wNumBufferedNameBytes = (WORD)
                    (lpstMacroScan->dwMacroNameTableLimit -
                     lpstMacroScan->dwFirstBufferedNameOffset);
            }
            else
            {
                // Read the maximum number

                lpstMacroScan->wNumBufferedNameBytes =
                    lpstMacroScan->wMaxNameBufferSize;
            }

            if (WD7EncryptedRead(lpstMacroScan->lpstStream,
                                 lpstMacroScan->lpstKey,
                                 lpstMacroScan->dwFirstBufferedNameOffset,
                                 lpbyNames,
                                 lpstMacroScan->wNumBufferedNameBytes) !=
                lpstMacroScan->wNumBufferedNameBytes)
            {
                return(WD7_STATUS_ERROR);
            }

            lpstMacroScan->wNextNameIndex = 0;
            wNextOffset = 0;
        }

        // Update the index

        lpstMacroScan->wNextNameIndex++;

        // Get the length of the name

        wLen = lpbyNames[wNextOffset+2];

        // See if the current one is the one we want

        if (lpbyNames[wNextOffset] == byID[0] &&
            lpbyNames[wNextOffset+1] == byID[1])
        {
            // This is the one, copy the name

            lpbyNames += wNextOffset + 3;
            for (w=0;w<wLen;w++)
                *lpbyName++ = *lpbyNames++;

            // Zero terminate it

            *lpbyName = 0;

            // Update name search fields

            lpstMacroScan->wNextNameOffset = wNextOffset + 4 + wLen;

            return(WD7_STATUS_OK);
        }

        // Have we done a full round?

        if ((lpstMacroScan->wFirstBufferedNameIndex +
             lpstMacroScan->wNextNameIndex - 1) ==
            wFirstSearchIndex && bCheckedAtLeastOne == TRUE)
        {
            // Just return a zero length name

            *lpbyName = 0;

            // Update name search fields

            lpstMacroScan->wNextNameOffset = wNextOffset + 4 + wLen;

            return(WD7_STATUS_OK);
        }

        bCheckedAtLeastOne = TRUE;

        // Go to next name

        wNextOffset += 4 + wLen;
    }
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7InitTDTInfo()
//
// Parameters:
//  lpstStream          Ptr to structure for stream
//  lpstKey             Key to use for encrypted reads
//  lpstTDTInfo         Ptr to TDT structure to initialize
//
// Description:
//  The function parses through the TDT and stores offset and
//  size information in the TDT structure for the macro name
//  table, the MCD table, and the UL name table.
//
//  The function also stores the stream structure pointer,
//  the key structure pointer, and the offset and size of the
//  TDT.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7InitTDTInfo
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_TDT_INFO      lpstTDTInfo
)
{
    BYTE                byTemp;
    BYTE                byRecType;
    WORD                wTemp;
    WORD                wCount;
    DWORD               dwRecSize;
    DWORD               dwOffset;
    DWORD               dwTDTOffset;
    DWORD               dwTDTSize;
    DWORD               dwTDTLimit;
    BYTE                abyWorkBuffer[12];

    // Get the offset and size of the template data table

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         280,
                         (LPBYTE)&dwTDTOffset,
                         sizeof(DWORD)) != sizeof(DWORD) ||
        WD7EncryptedRead(lpstStream,
                         lpstKey,
                         284,
                         (LPBYTE)&dwTDTSize,
                         sizeof(DWORD)) != sizeof(DWORD))
    {
        // Failed to get offset and size of template data table

        return(WD7_STATUS_ERROR);
    }

    dwTDTOffset = DWENDIAN(dwTDTOffset);
    dwTDTSize = DWENDIAN(dwTDTSize);

    // Store stream and key

    lpstTDTInfo->lpstStream = lpstStream;
    lpstTDTInfo->lpstKey = lpstKey;

    // Initialize TDT info structure

    lpstTDTInfo->dwTDTOffset = dwTDTOffset;
    lpstTDTInfo->dwTDTSize = dwTDTSize;

    lpstTDTInfo->dwMacroTableContents = WD7_MACRO_TABLE_TYPE_NONE;
    lpstTDTInfo->dwMacroNameTableOffset = 0xFFFFFFFF;
    lpstTDTInfo->wNumMacroNames = 0;
    lpstTDTInfo->dwMCDTableOffset= 0xFFFFFFFF;
    lpstTDTInfo->wNumMCDs = 0;
    lpstTDTInfo->dwULNameTableOffset = 0xFFFFFFFF;
    lpstTDTInfo->wULNameTableSize = 0;
    lpstTDTInfo->dwMenuDeltaTableOffset = 0xFFFFFFFF;

	// Verify that the first byte of the template data table is 0xFF

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwTDTOffset,
                         &byTemp,
                         sizeof(BYTE)) != sizeof(BYTE) ||
		byTemp != 0xFF)
        return(WD7_STATUS_ERROR);

	// Now parse through the records in the table

    dwTDTLimit = dwTDTOffset + dwTDTSize;
    dwOffset = dwTDTOffset + 1;
    while (dwOffset < dwTDTLimit)
    {
		// Read the record type

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwOffset++,
                             &byRecType,
                             sizeof(BYTE)) != sizeof(BYTE))
            return(WD7_STATUS_ERROR);

		if (byRecType == 0x11)
		{
            WORD w;
            BYTE byLen;

            lpstTDTInfo->dwMacroTableContents |= WD7_MACRO_TABLE_TYPE_0X11;

			// Bingo, macro name table

			// Read number of macros

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&(lpstTDTInfo->wNumMacroNames),
                                 sizeof(WORD)) != sizeof(WORD))
                return(WD7_STATUS_ERROR);

            lpstTDTInfo->wNumMacroNames =
                WENDIAN(lpstTDTInfo->wNumMacroNames);

            dwOffset += 2;

            lpstTDTInfo->dwMacroNameTableOffset = dwOffset;

            for (w=0;w<lpstTDTInfo->wNumMacroNames;w++)
			{
				dwOffset += 2;
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)(&byLen),
                                     sizeof(BYTE)) != sizeof(BYTE))
                    return(WD7_STATUS_ERROR);

                // Skip over length byte, characters of name, and
                //  terminating zero character

                dwOffset += byLen + 2;
			}
		}
		else
		if (byRecType >= 0x01 && byRecType <= 0x05)
		{
			switch (byRecType)
			{
				case 0x01:
                    lpstTDTInfo->dwMacroTableContents |=
                        WD7_MACRO_TABLE_TYPE_0X01;
                    dwRecSize = 24;
					break;

				case 0x02:
                    lpstTDTInfo->dwMacroTableContents |=
                        WD7_MACRO_TABLE_TYPE_0X02;
                    dwRecSize = 4;
					break;

				case 0x03:
                    lpstTDTInfo->dwMacroTableContents |=
                        WD7_MACRO_TABLE_TYPE_0X03;
                    dwRecSize = 14;
                    break;

                case 0x04:
                    lpstTDTInfo->dwMacroTableContents |=
                        WD7_MACRO_TABLE_TYPE_0X04;
                    dwRecSize = 14;
					break;

				default:
                    lpstTDTInfo->dwMacroTableContents |=
                        WD7_MACRO_TABLE_TYPE_0X05;
                    lpstTDTInfo->dwMenuDeltaTableOffset = dwOffset;
                    dwRecSize = 12;
					break;
			}

			// Read count

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wCount,
                                 sizeof(WORD)) != sizeof(WORD))
                return(WD7_STATUS_ERROR);

            wCount = WENDIAN(wCount);

            dwOffset += 2;

			if (byRecType == 0x01)
			{
				// MCD table

                lpstTDTInfo->dwMCDTableOffset = dwOffset;
                lpstTDTInfo->wNumMCDs = wCount;
			}

            // Now skip that many records

			dwOffset += wCount * dwRecSize;
		}
		else
		if (byRecType == 0x06)
		{
			WORD	cstr, cbTdsh, cbBts, ctb, cbut;

            lpstTDTInfo->dwMacroTableContents |=
                WD7_MACRO_TABLE_TYPE_0X06;

            cstr = 0;

			// Read parameters

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 abyWorkBuffer,
                                 8) != 8)
				return(WD7_STATUS_ERROR);

			dwOffset += 8;

            cbTdsh = WENDIAN(((LPWORD)abyWorkBuffer)[0]);
            cbBts = WENDIAN(((LPWORD)abyWorkBuffer)[1]);
            ctb = WENDIAN(((LPWORD)abyWorkBuffer)[3]);

            while (ctb-- != 0)
			{
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wTemp,
                                     sizeof(WORD)) != sizeof(WORD))
                    return(WD7_STATUS_ERROR);

                wTemp = WENDIAN(wTemp);

				dwOffset += cbTdsh;

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&cbut,
                                     sizeof(WORD)) != sizeof(WORD))
                    return(WD7_STATUS_ERROR);

                cbut = WENDIAN(cbut);

                dwOffset += 2;

				while (cbut-- != 0)
				{
					if (wTemp != 0xFFFF)
						dwOffset += 6;

                    if (WD7EncryptedRead(lpstStream,
                                         lpstKey,
                                         dwOffset,
                                         abyWorkBuffer,
                                         12) != 12)
                        return(WD7_STATUS_ERROR);

                    dwOffset += cbBts;

                    if ((abyWorkBuffer[11] & 0xC0) == 0x40)
						++cstr;
				}
			}

			while (cstr-- != 0)
			{
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&byTemp,
                                     sizeof(BYTE)) != sizeof(BYTE))
					return(WD7_STATUS_ERROR);

				dwOffset += 1 + byTemp;
			}
		}
		else
		if (byRecType == 0x10)
		{
            lpstTDTInfo->dwMacroTableContents |=
                WD7_MACRO_TABLE_TYPE_0X10;

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wCount,
                                 sizeof(WORD)) != sizeof(WORD))
                return(WD7_STATUS_ERROR);

            wCount = WENDIAN(wCount);

            // For Chinese version of Word, found that
            //  0xFFFF means Unicode strings?

            if (wCount == (WORD)0xFFFF)
            {
                dwOffset += 2;

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wCount,
                                     sizeof(WORD)) != sizeof(WORD))
                    return(WD7_STATUS_ERROR);

                wCount = WENDIAN(wCount) - 2;
            }

            lpstTDTInfo->dwULNameTableOffset = dwOffset + 2;
            lpstTDTInfo->wULNameTableSize = wCount - 2;

			dwOffset += wCount;
		}
		else
		if (byRecType == 0x12)
		{
            lpstTDTInfo->dwMacroTableContents |=
                WD7_MACRO_TABLE_TYPE_0X12;

			dwOffset += 2;
		}
		else
		if (byRecType == 0x40)
		{
            lpstTDTInfo->dwMacroTableContents |=
                WD7_MACRO_TABLE_TYPE_0X40;

            dwOffset += 2;
			break;
		}
		else
		{
			// Invalid

            return(WD7_STATUS_ERROR);
		}
	}

	// Good deal!

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7CountActiveMacros()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  lpwNumActiveMacros  Ptr to WORD to store count of number
//                          of active macros
//
// Description:
//  The function iterates through the MCD table and counts
//  that MCD as one if it's dwN field is non-zero.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7CountActiveMacros
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    LPWORD              lpwNumActiveMacros
)
{
    WD7_MCD_T           stMCD;
    WORD                w;
    DWORD               dwOffset;

    // Iterate through MCD structures in MCD table

    *lpwNumActiveMacros = 0;
    dwOffset = lpstTDTInfo->dwMCDTableOffset;
    for (w=0;w<lpstTDTInfo->wNumMCDs;w++)
	{
        // Read the MCD structure

        if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                             lpstTDTInfo->lpstKey,
                             dwOffset,
                             (LPBYTE)&stMCD,
                             sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
            return(WD7_STATUS_ERROR);

        // Check the dwN field

        if (stMCD.dwN != 0 && *lpwNumActiveMacros < 0xFFFF)
		{
            // Found an active macro

            ++*lpwNumActiveMacros;
        }

        dwOffset += sizeof(WD7_MCD_T);
    }

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7GetMacroInfoAtIndex()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  wIndex              Index of macro info to look up
//  lpbyName            Buffer for name (>= 256 bytes)
//  lpdwOffset          Ptr to DWORD to store macro offset
//  lpdwSize            Ptr to DWORD to store macro size
//  lpbyEncryptByte     Ptr to BYTE to store encryption byte
//  lpbActive           Ptr to BOOL to store active status of macro
//
// Description:
//  The function looks up the MCD at the given index and then
//  searches for the name in the macro name table.  If there
//  is no MCD at the given index, the function returns error.
//  If there is no associated name, the name is returned as a
//  zero length string.
//
//  Any of lpbyName, lpdwOffset, lpdwSize, lpbyEncryptByte, and
//  lpbActive may be NULL in which case the corresponding values
//  will not be returned.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7GetMacroInfoAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize,
    LPBYTE              lpbyEncryptByte,
    LPBOOL              lpbActive
)
{
    DWORD               dwOffset;
    BYTE                abyIDandLen[3];
    WD7_MCD_T           stMCD;
    WORD                wID;
    WORD                w;

    // Make sure that the index is within range

    if (wIndex >= lpstTDTInfo->wNumMCDs)
        return(WD7_STATUS_ERROR);

    // Read the MCD

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         lpstTDTInfo->dwMCDTableOffset +
                             wIndex * sizeof(WD7_MCD_T),
                         (LPBYTE)&stMCD,
                         sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Copy and endianize desired fields

    wID = WENDIAN(stMCD.wID);
    if (lpbyEncryptByte != NULL)
        *lpbyEncryptByte = stMCD.byEncrypt;

    if (lpdwSize != NULL)
        *lpdwSize = DWENDIAN(stMCD.dwSize);

    if (lpdwOffset != NULL)
        *lpdwOffset = DWENDIAN(stMCD.dwOffset);

    if (lpbActive != NULL)
    {
        if (stMCD.dwN == 0)
            *lpbActive = FALSE;
        else
            *lpbActive = TRUE;
    }

    if (lpbyName == NULL)
        return(WD7_STATUS_OK);

    // Now search for the name

    dwOffset = lpstTDTInfo->dwMacroNameTableOffset;

    for (w=0;w<lpstTDTInfo->wNumMacroNames;w++)
    {
        // Read the ID and length

        if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                             lpstTDTInfo->lpstKey,
                             dwOffset,
                             abyIDandLen,
                             3) != 3)
            return(WD7_STATUS_ERROR);

        dwOffset += 3;

        // Is this the ID we're looking for?

        if (WENDIAN(*(LPWORD)abyIDandLen) == wID)
        {
            // Found it, read in the name

            if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                                 lpstTDTInfo->lpstKey,
                                 dwOffset,
                                 lpbyName,
                                 abyIDandLen[2]) != abyIDandLen[2])
                return(WD7_STATUS_ERROR);

            // Zero terminate the name

            lpbyName[abyIDandLen[2]] = 0;

            return(WD7_STATUS_OK);
        }

        // Characters of name, and
        //  terminating zero character

        dwOffset += abyIDandLen[2] + 1;
    }

    *lpbyName = 0;
    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7SetMacroNameAtIndex()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  wIndex              Index of macro name to set
//  lpbyName            Buffer containing new name (zero-terminated)
//
// Description:
//  The function looks up the MCD at the given index and then
//  searches for the name in the macro name table.  If there
//  is no MCD at the given index, the function returns error.
//  If there is no associated name, the function returns okay.
//
//  The function will overwrite the existing name up to the
//  number of characters in the existing name.  If the new name
//  has fewer characters, the rest of the existing name is left
//  as is.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7SetMacroNameAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName
)
{
    DWORD               dwOffset;
    BYTE                abyIDandLen[3];
    WD7_MCD_T           stMCD;
    WORD                wID;
    WORD                w;

    // Make sure that the index is within range

    if (wIndex >= lpstTDTInfo->wNumMCDs)
        return(WD7_STATUS_ERROR);

    // Read the MCD

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         lpstTDTInfo->dwMCDTableOffset +
                             wIndex * sizeof(WD7_MCD_T),
                         (LPBYTE)&stMCD,
                         sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Copy and endianize desired fields

    wID = WENDIAN(stMCD.wID);

    // Now search for the name

    dwOffset = lpstTDTInfo->dwMacroNameTableOffset;

    for (w=0;w<lpstTDTInfo->wNumMacroNames;w++)
    {
        // Read the ID and length

        if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                             lpstTDTInfo->lpstKey,
                             dwOffset,
                             abyIDandLen,
                             3) != 3)
            return(WD7_STATUS_ERROR);

        dwOffset += 3;

        // Is this the ID we're looking for?

        if (WENDIAN(*(LPWORD)abyIDandLen) == wID)
        {
            // Found it, overwrite the name

            for (w=0;w<abyIDandLen[2];w++)
            {
                if (*lpbyName == 0)
                    return(WD7_STATUS_OK);

                if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                                      lpstTDTInfo->lpstKey,
                                      dwOffset++,
                                      lpbyName++,
                                      sizeof(BYTE)) != sizeof(BYTE))
                    return(WD7_STATUS_ERROR);
            }

            return(WD7_STATUS_OK);
        }

        // Characters of name, and
        //  terminating zero character

        dwOffset += abyIDandLen[2] + 1;
    }

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7GetMacroInfoAtULName()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  lpbyName            Name (>= 256 bytes)
//  lpwIndex            Ptr for index of macro info to look up
//  lpdwOffset          Ptr to DWORD to store macro offset
//  lpdwSize            Ptr to DWORD to store macro size
//  lpbyEncryptByte     Ptr to BYTE to store encryption byte
//  lpbActive           Ptr to BOOL to store active status of macro
//
// Description:
//  The function searches through the UL name table for the given
//  name which is in Pascal string format.  If the table is in
//  Unicode format, the function only checks the first byte
//  of each Unicode character.
//
//  If the function finds a matching name, it then performs
//  a linear search through the MCDs looking for the one with
//  its UL name index field set to the found one.  If none is
//  found (which could happen if the name matched a description),
//  the search through the UL name table continues for the next
//  occurrence, and so on.
//
//  Any of lpwIndex, lpdwOffset, lpdwSize, lpbyEncryptByte, and
//  lpbActive may be NULL in which case the corresponding values
//  will not be returned.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7GetMacroInfoAtULName
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    LPBYTE              lpbyName,
    LPWORD              lpwIndex,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwSize,
    LPBYTE              lpbyEncryptByte,
    LPBOOL              lpbActive
)
{
    DWORD               dwOffset;
    BYTE                byTemp, byLen;
    WD7_MCD_T           stMCD;
    WORD                wID;
    WORD                wCandID;
    WORD                w;
    WORD                wBytesRead;
    BOOL                bUnicoded;
    BYTE                bySrcByte;
    BYTE                byDstByte;
    BYTE                bySrcLen;
    BYTE                abyWorkBuffer[512];

    // Name must be non-empty

    bySrcLen = *lpbyName++;
    if (bySrcLen == 0)
        return(WD7_STATUS_ERROR);

    // Parse through the UL name table to find the index
    //  of the desired name

    dwOffset = lpstTDTInfo->dwULNameTableOffset;

    // First see if this is a Unicode table

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         dwOffset - 3,
                         &byTemp,
                         sizeof(BYTE)) != sizeof(BYTE))
        return(WD7_STATUS_ERROR);

    if (byTemp == 0x10)
        bUnicoded = FALSE;
    else
        bUnicoded = TRUE;

    wBytesRead = 0;
    wID = 0;
    while (1)
    {
        while (wBytesRead < lpstTDTInfo->wULNameTableSize)
        {
            // For Unicode, assume that the number of Unicode characters
            //  is no more than 255

            if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                                 lpstTDTInfo->lpstKey,
                                 dwOffset++,
                                 &byLen,
                                 sizeof(BYTE)) != sizeof(BYTE))
                return(WD7_STATUS_ERROR);

            if (bUnicoded == TRUE)
            {
                ++dwOffset;

                if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                                     lpstTDTInfo->lpstKey,
                                     dwOffset,
                                     abyWorkBuffer,
                                     byLen * sizeof(WORD)) !=
                    byLen * sizeof(WORD))
                    return(WD7_STATUS_ERROR);

                dwOffset += 2 * byLen;

                wBytesRead += 2 * byLen + 4;
            }
            else
            {
                if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                                     lpstTDTInfo->lpstKey,
                                     dwOffset,
                                     abyWorkBuffer,
                                     (WORD)(byLen)) != (WORD)(byLen))
                    return(WD7_STATUS_ERROR);

                dwOffset += byLen;
                wBytesRead += byLen + 3;
            }

            // Skip past the 0x01 0x00 after each UL name

            dwOffset += 2;

            // Increment index and check for completion

            ++wID;

            // Compare the names

            if (bySrcLen == byLen && byLen > 0)
            {
                for (w=0;w<byLen;w++)
                {
                    bySrcByte = lpbyName[w];
                    if (bUnicoded == FALSE)
                        byDstByte = abyWorkBuffer[w];
                    else
                        byDstByte = abyWorkBuffer[w * 2];

                    if (bySrcByte <= 'Z' && bySrcByte >= 'A')
                        bySrcByte = bySrcByte + ('a' - 'A');

                    if (byDstByte <= 'Z' && byDstByte >= 'A')
                        byDstByte = byDstByte + ('a' - 'A');

                    if (bySrcByte != byDstByte)
                        break;
                }

                if (w == byLen)
                {
                    // Found a possible match

                    break;
                }
            }
        }

        // Search through the MCDs looking for a matching one

        wCandID = WENDIAN(wID - 1);
        for (w=0;w<lpstTDTInfo->wNumMCDs;w++)
        {
            // Read the MCD

            if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                                 lpstTDTInfo->lpstKey,
                                 lpstTDTInfo->dwMCDTableOffset +
                                     w * sizeof(WD7_MCD_T),
                                 (LPBYTE)&stMCD,
                                 sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
                return(WD7_STATUS_ERROR);

            // See if this is the one

            if (stMCD.wULNameIdx == wCandID)
            {
                // This is the one

                if (lpwIndex != NULL)
                    *lpwIndex = w;

                if (lpbyEncryptByte != NULL)
                    *lpbyEncryptByte = stMCD.byEncrypt;

                if (lpdwSize != NULL)
                    *lpdwSize = DWENDIAN(stMCD.dwSize);

                if (lpdwOffset != NULL)
                    *lpdwOffset = DWENDIAN(stMCD.dwOffset);

                if (lpbActive != NULL)
                {
                    if (stMCD.dwN == 0)
                        *lpbActive = FALSE;
                    else
                        *lpbActive = TRUE;
                }

                return(WD7_STATUS_OK);
            }
        }

        // No more possibilities?

        if (wBytesRead >= lpstTDTInfo->wULNameTableSize)
        {
            // No such macro

            return(WD7_STATUS_ERROR);
        }
    }
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7GetULNameAtIndex()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  wIndex              Index in UL record of name to get
//  lpbyName            Buffer for name (>= 256 bytes)
//
// Description:
//  The function returns in lpbyName the string at the given
//  index in the UL name record (record 0x10).  If the index
//  exceeds the greatest index in the record, the function
//  returns a zero length string.
//
//  The string is returned as a Pascal string.  If the string
//  is a Unicode string, only the first byte of each of the first
//  255 characters are returned.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7GetULNameAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName
)
{
    DWORD               dwOffset;
    BYTE                byTemp;
    BYTE                byLen;
    BOOL                bUnicoded;
    WORD                wBytesRead;
    WORD                wID;

    // Now search for the name

    dwOffset = lpstTDTInfo->dwULNameTableOffset;

    // First see if this is a Unicode table

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         dwOffset - 3,
                         &byTemp,
                         sizeof(BYTE)) != sizeof(BYTE))
        return(WD7_STATUS_ERROR);

    if (byTemp == 0x10)
        bUnicoded = FALSE;
    else
        bUnicoded = TRUE;

    // Now parse through the UL name table

	wBytesRead = 0;
    wID = 0;
    while (wBytesRead < lpstTDTInfo->wULNameTableSize)
	{
        // For Unicode, assume that the number of Unicode characters
        //  is no more than 255

        if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                             lpstTDTInfo->lpstKey,
                             dwOffset++,
                             &byLen,
                             sizeof(BYTE)) != sizeof(BYTE))
            return(WD7_STATUS_ERROR);

        if (bUnicoded == TRUE)
        {
            if (wID == wIndex)
            {
                // Found it

                // Skip past the second byte of the length

                ++dwOffset;

                *lpbyName++ = byLen;

                while (byLen-- != 0)
                {
                    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                                         lpstTDTInfo->lpstKey,
                                         dwOffset,
                                         lpbyName++,
                                         sizeof(BYTE)) != sizeof(BYTE))
                        return(WD7_STATUS_ERROR);

                    dwOffset += sizeof(WORD);
                }

                return(WD7_STATUS_OK);
            }
            else
                dwOffset += 2 * byLen;

            wBytesRead += 2 * byLen + 4;
        }
        else
        {
            if (wID == wIndex)
            {
                // Found it

                if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                                     lpstTDTInfo->lpstKey,
                                     dwOffset,
                                     lpbyName + 1,
                                     (WORD)(byLen)) != (WORD)(byLen))
                    return(WD7_STATUS_ERROR);

                *lpbyName = byLen;
                return(WD7_STATUS_OK);
            }

            dwOffset += byLen;
            wBytesRead += byLen + 3;
        }

        // Skip past the 0x01 0x00 after each UL name

        dwOffset += 2;

        // Increment index and check for completion

        ++wID;
	}

    *lpbyName = 0;
    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7GetULMacroNameAtIndex()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  wIndex              Index of macro info to look up
//  lpbyName            Buffer for name (>= 256 bytes)
//
// Description:
//  The function looks up the MCD at the given index and then
//  searches for the name in the UL macro name table.  If there
//  is no MCD at the given index, the function returns error.
//  If there is no associated name, the name is returned as a
//  zero length string.
//
//  The string is returned as a Pascal string.  If the string
//  is a Unicode string, only the first byte of each of the first
//  255 characters are returned.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7GetULMacroNameAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName
)
{
    WD7_MCD_T           stMCD;

    // Make sure that the index is within range

    if (wIndex >= lpstTDTInfo->wNumMCDs)
        return(WD7_STATUS_ERROR);

    // Read the MCD

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         lpstTDTInfo->dwMCDTableOffset +
                             wIndex * sizeof(WD7_MCD_T),
                         (LPBYTE)&stMCD,
                         sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Endianize the index we are searching for

    stMCD.wULNameIdx = WENDIAN(stMCD.wULNameIdx);

    if (stMCD.wULNameIdx == 0xFFFF)
    {
        *lpbyName = 0;
        return(WD7_STATUS_OK);
    }

    // Get the name from the UL name table

    if (WD7GetULNameAtIndex(lpstTDTInfo,
                            stMCD.wULNameIdx,
                            lpbyName) != WD7_STATUS_OK)
        return(WD7_STATUS_ERROR);

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7GetULMacroDescAtIndex()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  wIndex              Index of macro info to look up
//  lpbyDesc            Buffer for description (>= 256 bytes)
//
// Description:
//  The function looks up the MCD at the given index and then
//  searches for the description in the UL macro name table.  If there
//  is no MCD at the given index, the function returns error.
//  If there is no associated description, the description is returned
//  as a zero length string.
//
//  The string is returned as a Pascal string.  If the string
//  is a Unicode string, only the first byte of each of the first
//  255 characters are returned.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7GetULMacroDescAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyDesc
)
{
    WD7_MCD_T           stMCD;

    // Make sure that the index is within range

    if (wIndex >= lpstTDTInfo->wNumMCDs)
        return(WD7_STATUS_ERROR);

    // Read the MCD

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         lpstTDTInfo->dwMCDTableOffset +
                             wIndex * sizeof(WD7_MCD_T),
                         (LPBYTE)&stMCD,
                         sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Endianize the index we are searching for

    stMCD.wMenuHelp = WENDIAN(stMCD.wMenuHelp);

    if (stMCD.wMenuHelp == 0xFFFF)
    {
        *lpbyDesc = 0;
        return(WD7_STATUS_OK);
    }

    // Get the name from the UL name table

    if (WD7GetULNameAtIndex(lpstTDTInfo,
                            stMCD.wMenuHelp,
                            lpbyDesc) != WD7_STATUS_OK)
        return(WD7_STATUS_ERROR);

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7SetULMacroNameAtIndex()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  wIndex              Index of macro info to look up
//  lpbyName            Buffer containing Pascal name
//
// Description:
//  The function looks up the MCD at the given index and then
//  searches for the name in the UL macro name table.  If there
//  is no MCD at the given index, the function returns error.
//
//  The name passed in is used to overwrite the existing name
//  up to the number of characters in the existing name.
//  If the name passed in has fewer characters, the rest of
//  the characters in the existing name are left as is.
//  If the existing name is Unicode, then only the first
//  character of each Unicode character is overwritten.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7SetULMacroNameAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyName
)
{
    DWORD               dwOffset;
    BYTE                byTemp, byLen;
    WD7_MCD_T           stMCD;
    WORD                w;
    WORD                wID;
    WORD                wBytesRead;
    BOOL                bUnicoded;

    // Make sure that the index is within range

    if (wIndex >= lpstTDTInfo->wNumMCDs)
        return(WD7_STATUS_ERROR);

    // Read the MCD

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         lpstTDTInfo->dwMCDTableOffset +
                             wIndex * sizeof(WD7_MCD_T),
                         (LPBYTE)&stMCD,
                         sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Now search for the name

    dwOffset = lpstTDTInfo->dwULNameTableOffset;

    // First see if this is a Unicode table

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         dwOffset - 3,
                         &byTemp,
                         sizeof(BYTE)) != sizeof(BYTE))
        return(WD7_STATUS_ERROR);

    if (byTemp == 0x10)
        bUnicoded = FALSE;
    else
        bUnicoded = TRUE;

    // Endianize the index we are searching for

    stMCD.wULNameIdx = WENDIAN(stMCD.wULNameIdx);

    // Now parse through the UL name table

	wBytesRead = 0;
    wID = 0;
    while (wBytesRead < lpstTDTInfo->wULNameTableSize)
	{
        // For Unicode, assume that the number of Unicode characters
        //  is no more than 255

        if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                             lpstTDTInfo->lpstKey,
                             dwOffset++,
                             &byLen,
                             sizeof(BYTE)) != sizeof(BYTE))
            return(WD7_STATUS_ERROR);

        if (bUnicoded == TRUE)
        {
            ++dwOffset;

            if (wID == stMCD.wULNameIdx)
            {
                // Found it

                if (byLen > *lpbyName)
                    byLen = *lpbyName;

                for (w=0;w<byLen;w++)
                {
                    if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                                          lpstTDTInfo->lpstKey,
                                          dwOffset,
                                          lpbyName + w + 1,
                                          sizeof(BYTE)) != sizeof(BYTE))
                        return(WD7_STATUS_ERROR);

                    dwOffset += 2;
                }

                return(WD7_STATUS_OK);
            }
            else
                dwOffset += 2 * byLen;

            wBytesRead += 2 * byLen + 4;
        }
        else
        {
            if (wID == stMCD.wULNameIdx)
            {
                if (byLen > *lpbyName)
                    byLen = *lpbyName;

                if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                                      lpstTDTInfo->lpstKey,
                                      dwOffset,
                                      lpbyName + 1,
                                      (WORD)(byLen)) != (WORD)(byLen))
                    return(WD7_STATUS_ERROR);

                return(WD7_STATUS_OK);
            }

            dwOffset += byLen;
            wBytesRead += byLen + 3;
        }

        // Skip past the 0x01 0x00 after each UL name

        dwOffset += 2;

        // Increment index and check for completion

        ++wID;
	}

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7DeactivateMacroAtIndex()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//  wIndex              Index of macro info to look up
//
// Description:
//  The macro is deactivated by doing the following:
//      1. Set the dwN field of the MCD to 0.
//      2. Space out the name in the macro name table.
//      3. Space out the name in the UL name table.
//      4. Space out the description in the UL name table.
//      5. Zero out the macro text
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7DeactivateMacroAtIndex
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex
)
{
    DWORD               dwOffset;
    BYTE                abyWorkBuffer[512];
    BYTE                abyIDandLen[3];
    BYTE                byLen;
    WD7_MCD_T           stMCD;
    WORD                wID;
    WORD                w;
    WORD                wBytesRead;
    BOOL                bUnicoded;

    // Make sure that the index is within range

    if (wIndex >= lpstTDTInfo->wNumMCDs)
        return(WD7_STATUS_ERROR);

    ////////////////////////////////////////////////////////
    // 1. Set the dwN field of the MCD to 0.
    ////////////////////////////////////////////////////////

    // Read the MCD

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         lpstTDTInfo->dwMCDTableOffset +
                             wIndex * sizeof(WD7_MCD_T),
                         (LPBYTE)&stMCD,
                         sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Set the dwN field to zero

    stMCD.dwN = 0;

    // Write the updated MCD

    if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                          lpstTDTInfo->lpstKey,
                          lpstTDTInfo->dwMCDTableOffset +
                              wIndex * sizeof(WD7_MCD_T),
                          (LPBYTE)&stMCD,
                          sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Re-read the MCD since the encrypted write messes it up

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         lpstTDTInfo->dwMCDTableOffset +
                             wIndex * sizeof(WD7_MCD_T),
                         (LPBYTE)&stMCD,
                         sizeof(WD7_MCD_T)) != sizeof(WD7_MCD_T))
        return(WD7_STATUS_ERROR);

    // Endianize the structure

    wID = WENDIAN(stMCD.wID);

    ////////////////////////////////////////////////////////
    // 2. Space out the name in the macro name table
    ////////////////////////////////////////////////////////

    // Now search for the name

    dwOffset = lpstTDTInfo->dwMacroNameTableOffset;

    for (w=0;w<lpstTDTInfo->wNumMacroNames;w++)
    {
        // Read the ID and length

        if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                             lpstTDTInfo->lpstKey,
                             dwOffset,
                             abyIDandLen,
                             3) != 3)
            return(WD7_STATUS_ERROR);

        dwOffset += 3;

        // Is this the ID we're looking for?

        if (WENDIAN(*(LPWORD)abyIDandLen) == wID)
        {
            // Found it, space out the name

            for (w=0;w<abyIDandLen[2];w++)
                abyWorkBuffer[w] = ' ';

            if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                                  lpstTDTInfo->lpstKey,
                                  dwOffset,
                                  abyWorkBuffer,
                                  abyIDandLen[2]) != abyIDandLen[2])
                return(WD7_STATUS_ERROR);

            break;
        }

        // Characters of name, and
        //  terminating zero character

        dwOffset += abyIDandLen[2] + 1;
    }

    ////////////////////////////////////////////////////////
    // 3. Space out the name in the UL name table.
    // 4. Space out the description in the UL name table.
    ////////////////////////////////////////////////////////

    dwOffset = lpstTDTInfo->dwULNameTableOffset;

    // First see if this is a Unicode table

    if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                         lpstTDTInfo->lpstKey,
                         dwOffset - 3,
                         abyWorkBuffer,
                         sizeof(BYTE)) != sizeof(BYTE))
        return(WD7_STATUS_ERROR);

    if (abyWorkBuffer[0] == 0x10)
        bUnicoded = FALSE;
    else
        bUnicoded = TRUE;

    // Endianize the indexes we are searching for

    stMCD.wULNameIdx = WENDIAN(stMCD.wULNameIdx);
    stMCD.wMenuHelp = WENDIAN(stMCD.wMenuHelp);

    // Now parse through the UL name table

	wBytesRead = 0;
    wID = 0;
    while (wBytesRead < lpstTDTInfo->wULNameTableSize)
	{
        // For Unicode, assume that the number of Unicode characters
        //  is no more than 255

        if (WD7EncryptedRead(lpstTDTInfo->lpstStream,
                             lpstTDTInfo->lpstKey,
                             dwOffset++,
                             &byLen,
                             sizeof(BYTE)) != sizeof(BYTE))
            return(WD7_STATUS_ERROR);

        if (bUnicoded == TRUE)
        {
            ++dwOffset;

            if (wID == stMCD.wULNameIdx ||
                wID == stMCD.wMenuHelp)
            {
                abyWorkBuffer[0] = ' ';
                abyWorkBuffer[1] = 0;
                for (w=0;w<byLen;w++,dwOffset+=2)
                    if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                                          lpstTDTInfo->lpstKey,
                                          dwOffset,
                                          abyWorkBuffer,
                                          sizeof(WORD)) != sizeof(WORD))
                        return(WD7_STATUS_ERROR);
            }
            else
                dwOffset += 2 * byLen;

            wBytesRead += 2 * byLen + 4;
        }
        else
        {
            if (wID == stMCD.wULNameIdx ||
                wID == stMCD.wMenuHelp)
            {
                for (w=0;w<(WORD)(byLen);w++)
                    abyWorkBuffer[w] = ' ';

                if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                                      lpstTDTInfo->lpstKey,
                                      dwOffset,
                                      abyWorkBuffer,
                                      (WORD)(byLen)) != (WORD)(byLen))
                    return(WD7_STATUS_ERROR);
            }

            dwOffset += byLen;
            wBytesRead += byLen + 3;
        }

        // Skip past the 0x01 0x00 after each UL name

        dwOffset += 2;

        // Increment index and check for completion

        ++wID;
        if ((wID > stMCD.wULNameIdx ||
            0xFFFF == stMCD.wULNameIdx) &&
            (wID > stMCD.wMenuHelp ||
            0xFFFF == stMCD.wMenuHelp))
			break;
	}

    ////////////////////////////////////////////////////////
    // 5. Zero out the macro text
    ////////////////////////////////////////////////////////

    // Zero out the work buffer

    for (w=0;w<512;w++)
        abyWorkBuffer[w] = 0;

    stMCD.dwSize = DWENDIAN(stMCD.dwSize);
    stMCD.dwOffset = DWENDIAN(stMCD.dwOffset);
    dwOffset = stMCD.dwOffset;

    wBytesRead = 512;
    while (dwOffset < stMCD.dwOffset + stMCD.dwSize)
    {
        if (dwOffset + 512 > stMCD.dwOffset + stMCD.dwSize)
            wBytesRead = (WORD)(stMCD.dwOffset + stMCD.dwSize - dwOffset);

        // Write a chunk

        if (WD7EncryptedWrite(lpstTDTInfo->lpstStream,
                              lpstTDTInfo->lpstKey,
                              dwOffset,
                              abyWorkBuffer,
                              wBytesRead) != wBytesRead)
            return(WD7_STATUS_ERROR);

        dwOffset += wBytesRead;
    }

    // Done

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  BOOL WD7TDTHasNonMacroInfo()
//
// Parameters:
//  lpstTDTInfo         Ptr to valid initialized TDT structure
//
// Description:
//  Determines whether the document contains non-macro template
//  information.
//
//  Note that this function is not entirely correct since the
//  meaning of all records is still unknown.
//
// Returns:
//  TRUE                If the TDT has non-macro records
//  FALSE               If the TDT has no non-macro records
//
//********************************************************************

BOOL WD7TDTHasNonMacroInfo
(
    LPWD7_TDT_INFO      lpstTDTInfo
)
{
    if ((lpstTDTInfo->dwMacroTableContents &
            (WD7_MACRO_TABLE_TYPE_0X02 |
             WD7_MACRO_TABLE_TYPE_0X03 |
             WD7_MACRO_TABLE_TYPE_0X04 |
             WD7_MACRO_TABLE_TYPE_0X05 |
             WD7_MACRO_TABLE_TYPE_0X06 |
             WD7_MACRO_TABLE_TYPE_0X12)) != 0)
    {
        // non-macro template information present

        return(TRUE);
    }

    // non-macro template information not present

    return(FALSE);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7HasAutoText()
//
// Parameters:
//  lpstStream          Ptr to structure for stream
//  lpstKey             Key to use for encrypted reads
//
// Description:
//  Checks the value at offset 0xA4.  This value stores the
//  size of the glossary string table.  If the value is non-zero,
//  the function sets *lpbHasAutoText to TRUE.
//
//  Otherwise, the function checks the second header.  The offset
//  of the second header is 0x200 multiplied by the WORD value at
//  offset 0x08.
//
//  If the value at offset 0xA4 relative to the second header is
//  non-zero, the function sets *lpbHasAutoText to TRUE.
//
//  Otherwise the function sets *lpbHasAutoText to FALSE.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7HasAutoText
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPBOOL              lpbHasAutoText
)
{
    DWORD               dwValue;
    WORD                w2ndHdrBlk;

    // Read DWORD at offset 0xA4

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         0xA4,
                         (LPBYTE)&dwValue,
                         sizeof(DWORD)) != sizeof(DWORD))
    {
        // Failed to read DWORD at offset 0xA4

        return(WD7_STATUS_ERROR);
    }

    dwValue = DWENDIAN(dwValue);
    if (dwValue != 0)
    {
        *lpbHasAutoText = TRUE;
        return(TRUE);
    }

    // Get the block number of the second header

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         0x08,
                         (LPBYTE)&w2ndHdrBlk,
                         sizeof(WORD)) != sizeof(WORD))
    {
        // Failed to read WORD at offset 0x08

        return(FALSE);
    }

    w2ndHdrBlk = WENDIAN(w2ndHdrBlk);

    // Read DWORD at offset 0xA4 relative to the second header

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         (DWORD)w2ndHdrBlk * (DWORD)0x200 + (DWORD)0xA4,
                         (LPBYTE)&dwValue,
                         sizeof(DWORD)) != sizeof(DWORD))
    {
        // Failed to read DWORD at offset 0xA4

        return(FALSE);
    }

    dwValue = DWENDIAN(dwValue);
    if (dwValue != 0)
    {
        *lpbHasAutoText = TRUE;
        return(TRUE);
    }

    *lpbHasAutoText = FALSE;
    return(TRUE);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7ChangeToDocument()
//
// Parameters:
//  lpstStream          Ptr to structure for stream
//  lpstKey             Key to use for encrypted reads
//
// Description:
//  Changes a Word document to a document by resetting the
//  template bit to zero
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7ChangeToDocument
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey
)
{
    BYTE                byFlags;

    // Read Word Document flag byte

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         10,
                         &byFlags,
                         sizeof(BYTE)) != sizeof(BYTE))
        return(WD7_STATUS_ERROR);

    // Make it a document

    byFlags &= 0xFE;

    // Write Word Document flag byte

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          10,
                          &byFlags,
                         sizeof(BYTE)) != sizeof(BYTE))
        return(WD7_STATUS_ERROR);

    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7MacroBinaryCRC()
//
// Parameters:
//  lpstStream          Ptr to structure for stream
//  lpstKey             Key to use for encrypted reads
//  dwOffset            Offset of the macro binary
//  dwSize              Size of the macro binary to CRC
//  byEncrypt           The macro's encryption byte
//  lpdwCRC             Ptr to DWORD to store CRC
//
// Description:
//  CRCs the macro binary.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7MacroBinaryCRC
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwOffset,
    DWORD               dwSize,
    BYTE                byEncrypt,
    LPDWORD             lpdwCRC
)
{
    BYTE                abyWorkBuffer[512];
    DWORD               dwLimit;
    DWORD               dwBytesToRead;
    DWORD               dwCRC;
    DWORD               dw;

    // Initialize the CRC

    CRC32Init(dwCRC);

    dwBytesToRead = 512;
    dwLimit = dwOffset + dwSize;
    while (dwOffset < dwLimit)
    {
        if (dwLimit - dwOffset < dwBytesToRead)
            dwBytesToRead = dwLimit - dwOffset;

        // Get some bytes

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwOffset,
                             abyWorkBuffer,
                             dwBytesToRead) != dwBytesToRead)
            return(WD7_STATUS_ERROR);

        // CRC the bytes

        for (dw=0;dw<dwBytesToRead;dw++)
            CRC32Continue(dwCRC,abyWorkBuffer[dw] ^ byEncrypt);

        dwOffset += dwBytesToRead;
    }

    *lpdwCRC = dwCRC;
    return(WD7_STATUS_OK);
}


//********************************************************************
//
// Function:
//  WD7_STATUS WD7SetTDTSize()
//
// Parameters:
//  lpstStream          Ptr to structure for stream
//  lpstKey             Key to use for encrypted reads
//  dwTDTSize           New size for the TDT
//
// Description:
//  Changes a Word documents template data table size to the
//  given size.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7SetTDTSize
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwTDTSize
)
{
    // Endianize before writing

    dwTDTSize = DWENDIAN(dwTDTSize);

    // Get the offset and size of the template data table

    if (WD7EncryptedWrite(lpstStream,
                          lpstKey,
                          284,
                          (LPBYTE)&dwTDTSize,
                          sizeof(DWORD)) != sizeof(DWORD))
    {
        // Failed to set size of template data table

        return(WD7_STATUS_ERROR);
    }

    return(WD7_STATUS_OK);
}




//********************************************************************
//
// Function:
//  WD7_STATUS CopyEncryptedBytes
//
// Parameters:
//  lpstStream			Ptr to the stream
//	lpstKey				Key
//	dwSrcOffset			The source offset of data
//	dwDstOffset			The destination offset for the data
//	dwNumBytes			The number of bytes to copy
//
// Description:
//  This function will copy bytes in the stream
//
// Returns:
//  WD7_STATUS_OK       On success
//	WD7_STATUS_ERROR	On Error
//
//
//********************************************************************


WD7_STATUS WD7CopyEncryptedBytes
(
    LPSS_STREAM        lpstStream,
    LPWD7ENCKEY        lpstKey,
    DWORD              dwSrcOffset,
    DWORD              dwDstOffset,
    DWORD              dwNumBytes
)
{
	DWORD		dw=0;
	DWORD		dwBuffLen;
    BYTE        abyWorkBuffer[512];
		
	// while we aren't done copying the number of bytes
	while(dw < dwNumBytes)
	{
		// Figure out how much we are going to shift.  Shift the #define amount unless
		// there is less room to the end of the TDT
        if ((dwNumBytes - dw) < sizeof(abyWorkBuffer))
			dwBuffLen = dwNumBytes - dw;
		else
            dwBuffLen = sizeof(abyWorkBuffer);
				
		// Read in starting after the deletion point
		if (WD7EncryptedRead(lpstStream,
						     lpstKey,
							 dwSrcOffset + dw,
							 abyWorkBuffer,
							 dwBuffLen) != dwBuffLen)
			return(WD7_STATUS_ERROR);

		
		// Write out at the deletion point
		if (WD7EncryptedWrite(lpstStream,
						      lpstKey,
							  dwDstOffset + dw,
							  abyWorkBuffer,
							  dwBuffLen) != dwBuffLen)
			return(WD7_STATUS_ERROR);

		// increment based on our read/write buffer
		dw = dw + dwBuffLen;
	}

	return(WD7_STATUS_OK);
} // WD7CopyEncryptedBytes
	

//********************************************************************
//
// Function:
//  WD7_STATUS WD7TDTInfoRecomputeOffsets()
//
// Parameters:
//  lpstTDTInfo         Ptr to TDT Offset Structure
//  dwStartOffset       The offset of the modified record
//	dwLen				Change in length
//
// Description:
//  This function will recompute the offsets of the TDT after
//  deleting a portion of the modified record at dwStartOffset.
//
// Returns:
//  WD7_STATUS_OK       Always
//
//
//********************************************************************

WD7_STATUS WD7TDTInfoRecomputeOffsets
(
    LPWD7_TDT_INFO      lpstTDTInfo,
    DWORD               dwStartOffset,
    DWORD               dwLen
)
{
    if (lpstTDTInfo->dwMacroNameTableOffset > dwStartOffset)
        lpstTDTInfo->dwMacroNameTableOffset -= dwLen;

    if (lpstTDTInfo->dwMCDTableOffset > dwStartOffset)
        lpstTDTInfo->dwMCDTableOffset -= dwLen;

    if (lpstTDTInfo->dwULNameTableOffset > dwStartOffset)
        lpstTDTInfo->dwULNameTableOffset -= dwLen;

    if (lpstTDTInfo->dwMenuDeltaTableOffset > dwStartOffset)
        lpstTDTInfo->dwMenuDeltaTableOffset -= dwLen;

    return(WD7_STATUS_OK);
} // ComputeOffsets


//********************************************************************
//
// Function:
//  WD7_STATUS WD7CheckMenuName()
//
// Parameters:
//  lpstStream			Ptr to the Word Document Stream
//  lpstKey				Key to use for Encrypted Read
//  lpstTDTInfo         Ptr to TDT Offset Structure
//  wIndex				Record0x10 Index
//  lpbyMenuName        Ptr to Menu Name string to find (pascal string)
//
// Description:
//  This function matches the szMenuName at zero-base wIndex in Record0x10
//
// Returns:
//  bResult				1 = Found 0 = Not Found
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7CheckMenuName
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    LPWD7_TDT_INFO      lpstTDTInfo,
    WORD                wIndex,
    LPBYTE              lpbyMenuName,
    LPBOOL              lpbResult
)
{

	BYTE	bLen;
    BYTE    abyName[256];
	BYTE	b;

	// If they don't give a Menu name then it is wildcard and we match automatically
    if (lpbyMenuName == NULL)
	{
        *lpbResult = TRUE;
		return(WD7_STATUS_OK);
	}

    // Get the name from record 0x10

    if (WD7GetULNameAtIndex(lpstTDTInfo,
                            wIndex,
                            abyName) != WD7_STATUS_OK)
        return(WD7_STATUS_ERROR);
    
    bLen = lpbyMenuName[0];
    if (bLen == abyName[0])
    {
        // Compare the string byte for byte with our string we
        // want to match.  If they match set return OK
        // For unicode we only compare the first character
        b=1;
        while (b <= bLen)
        {
            if (abyName[b++] != *++lpbyMenuName)
            {
                // don't match so get out
                *lpbResult = FALSE;
                return(WD7_STATUS_OK);
            }
        } //while b<bLen

        // If we get through the whole thing, then they matched
        *lpbResult = TRUE;
        return(WD7_STATUS_OK);
    } // if strlen match

    // strlen didn't match so get out
    *lpbResult = FALSE;
    return(WD7_STATUS_OK);
} //CheckMenuName


//********************************************************************
//
// Function:
//  WD7_STATUS WD7DeleteTDTData()
//
// Parameters:
//  lpstStream          Ptr to structure for stream
//  lpstKey             Key to use for encrypted reads
//  dwDelOffset			Ptr to structure for TDT offsets
//  dwLen				Length in bytes to delete
//
// Description:
//	This function will delete a particular length at a set offset and
//  shift the remaining TDT up using WD7CopyEncryptedBytes and redo
//  the size of the TDT.  The dwDelOffset must lie within the TDT.
//  dwLen must be less than the length from the deletion offset to
//  the end of the TDT.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************

WD7_STATUS WD7DeleteTDTData
(
    LPSS_STREAM        lpstStream,
    LPWD7ENCKEY        lpstKey,
    DWORD              dwDelOffset,
    DWORD              dwLen,
    LPWD7_TDT_INFO     lpstTDTInfo
)
{
    DWORD   dwTDTOffset;
	DWORD	dwTDTSize;
	
    dwTDTOffset = lpstTDTInfo->dwTDTOffset;
    dwTDTSize = lpstTDTInfo->dwTDTSize;

    // Shift the TDT
    if (WD7CopyEncryptedBytes(lpstStream,
                              lpstKey,
                              dwDelOffset+dwLen,
                              dwDelOffset,
                              (dwTDTOffset+dwTDTSize) -
                                  (dwDelOffset+dwLen)) != WD7_STATUS_OK)
		return(WD7_STATUS_ERROR);

	// Recalculate the offsets because we have shifted the TDT
    if (WD7TDTInfoRecomputeOffsets(lpstTDTInfo,
                                   lpstTDTInfo->dwMenuDeltaTableOffset,
                                   dwLen) != WD7_STATUS_OK)
		return(WD7_STATUS_ERROR);

    // reset the size of the TDT when we are done
    dwTDTSize -= dwLen;
    lpstTDTInfo->dwTDTSize = dwTDTSize;

    dwTDTSize = DWENDIAN(dwTDTSize);
	
	if(WD7EncryptedWrite(lpstStream,
                         lpstKey,
                         284,
                         (LPBYTE)&dwTDTSize,
                         sizeof(DWORD)) != sizeof(DWORD))
		return(WD7_STATUS_ERROR);

	return(WD7_STATUS_OK);
}//DeleteTDTData


//********************************************************************
//
// Function:
//  WD7_STATUS WD7DeleteDelta()
//
// Parameters:
//  lpstStream          Ptr to structure for stream
//  lpstKey             Key to use for encrypted reads
//  lpstTDTInfo       Ptr to structure for TDT offsets
//
//  adwDeltaToDelete    Array of the DWORDs to match the delta to remove
//  szMenuNameA			Menu name one (may be NULL)
//  szMenuNameB			Menu name two (may be NULL)
//
// Description:
//	This function will match the Delta given with a Delta in Record0x05
//  and the associated name in Record0x10. If it matches, it will call
//  a function to Delete the data.
//
// Returns:
//  WD7_STATUS_OK       On success
//  WD7_STATUS_ERROR    On error
//
//********************************************************************


WD7_STATUS WD7DeleteDelta
(
    LPSS_STREAM        lpstStream,
    LPWD7ENCKEY        lpstKey,
    LPWD7_TDT_INFO     lpstTDTInfo,
    LPMENU_INFO        lpstMenuInfo
)
{
	DWORD				dwRecTenOffset, dwRecFiveOffset;
	DWORD				dwDelOffset;
	WORD                wCount;
	WORD                w,wTemp;
    BOOL                bResult=FALSE;
    DELTA_T             stDeltaBuffer;
   
	// First, let us set up our Offsets
    dwRecFiveOffset = lpstTDTInfo->dwMenuDeltaTableOffset;

    // If there were no menu modifications, just return

    if (dwRecFiveOffset == 0xFFFFFFFF)
        return(WD7_STATUS_OK);

	// Get the count of deltas in Record Five
	if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwRecFiveOffset,
                             (LPBYTE)&wCount,
                             sizeof(WORD)) != sizeof(WORD))
		return(WD7_STATUS_ERROR);

    wCount = WENDIAN(wCount);

    // For each delta, read in the whole delta into stDeltaBuffer
    for (w=0;w<wCount;w++)
    {
		// if on our last iteration we found a match and delete a delta
		// we need to reset our counts for the shift
		if (bResult == 1)
		{
			w--;
			wCount--;		
			bResult = 0;
		}

		if (WD7EncryptedRead(lpstStream,
							lpstKey,
                            dwRecFiveOffset + sizeof(WORD) +
                            w * (DWORD)sizeof(DELTA_T),
                            (LPBYTE)&stDeltaBuffer,
                            sizeof(DELTA_T)) != sizeof(DELTA_T))
			return(WD7_STATUS_ERROR);

        stDeltaBuffer.wInst = WENDIAN(stDeltaBuffer.wInst);
        stDeltaBuffer.wMenu = WENDIAN(stDeltaBuffer.wMenu);
        stDeltaBuffer.wMenuItem = WENDIAN(stDeltaBuffer.wMenuItem);
        stDeltaBuffer.wFunction = WENDIAN(stDeltaBuffer.wFunction);
        stDeltaBuffer.wMacroIndex = WENDIAN(stDeltaBuffer.wMacroIndex);
        stDeltaBuffer.wMenuItemPos = WENDIAN(stDeltaBuffer.wMenuItemPos);

		// Now, compare each DWORD in stDeltaBuffer to the delta we want to find
		// which is in lpstMenuInfo->stDelta
        if ((lpstMenuInfo->dwInst == stDeltaBuffer.wInst ||
            lpstMenuInfo->dwInst == WD7_DELTA_WILD) &&
            (lpstMenuInfo->dwMenu == stDeltaBuffer.wMenu ||
            lpstMenuInfo->dwMenu == WD7_DELTA_WILD) &&
            (lpstMenuInfo->dwMenuItem == stDeltaBuffer.wMenuItem ||
            lpstMenuInfo->dwMenuItem == WD7_DELTA_WILD) &&
            (lpstMenuInfo->dwFunction == stDeltaBuffer.wFunction ||
            lpstMenuInfo->dwFunction == WD7_DELTA_WILD) &&
            (lpstMenuInfo->dwMacroIndex == stDeltaBuffer.wMacroIndex ||
            lpstMenuInfo->dwMacroIndex == WD7_DELTA_WILD) &&
            (lpstMenuInfo->dwMenuItemPos == stDeltaBuffer.wMenuItemPos ||
            lpstMenuInfo->dwMenuItemPos == WD7_DELTA_WILD))
        {
            // Set up our Offset
            dwRecTenOffset = lpstTDTInfo->dwULNameTableOffset;

            // The first WORD is the instruction.  Each instruction means a
			// we will want to check a different thing.  So, switch on it.
            switch(stDeltaBuffer.wInst)
			{
				// This case is when there is a index for a name for a Menu name
				// in Record0x10.  This should match the name in szMenuNameA
			    // If szMenuNameB is defined, then this won't match
				case 0x0000:
                case 0x0001:
                case 0x0082:
                case 0x0092:
                    if (lpstMenuInfo->lpbyMenuNameB == NULL)
					{
                        if (WD7CheckMenuName(lpstStream,
                                             lpstKey,
                                             lpstTDTInfo,
                                             stDeltaBuffer.wMenu,
                                             lpstMenuInfo->lpbyMenuNameA,
                                             &bResult) != WD7_STATUS_OK)
							return(WD7_STATUS_ERROR);
					}
					else
                        bResult = FALSE;
					break;

			    // This is when we have to match a name to the index in MenuItem
				// in Record0x10  If a name is defined in MenuNameB then we don't match
                case 0x000A:
                case 0x000B:
                case 0x000C:
                case 0x000D:
                case 0x001A:
                case 0x001B:
                case 0x001C:
                case 0x001D:
                    if (lpstMenuInfo->lpbyMenuNameB == NULL)
					{
                        if (WD7CheckMenuName(lpstStream,
                                             lpstKey,
                                             lpstTDTInfo,
                                             stDeltaBuffer.wMenuItem,
                                             lpstMenuInfo->lpbyMenuNameA,
                                             &bResult) != WD7_STATUS_OK)
							return(WD7_STATUS_ERROR);
					}
					else
                        bResult = FALSE;
					break;
					
                // Here we need to compare a name indexed by both MenuItem
                // and Menu
                case 0x0002:
                case 0x0003:
                case 0x0004:
                    if (WD7CheckMenuName(lpstStream,
                                         lpstKey,
                                         lpstTDTInfo,
                                         stDeltaBuffer.wMenu,
                                         lpstMenuInfo->lpbyMenuNameA,
                                         &bResult) != WD7_STATUS_OK)
						return(WD7_STATUS_ERROR);

                    if (bResult == TRUE)
					{
                        if (WD7CheckMenuName(lpstStream,
                                             lpstKey,
                                             lpstTDTInfo,
                                             stDeltaBuffer.wMenuItem,
                                             lpstMenuInfo->lpbyMenuNameB,
                                             &bResult) != WD7_STATUS_OK)
							return(WD7_STATUS_ERROR);
					}
					break;

                // This case is when there is no Record0x10 entries associated
                // with the Delta.  Ignore any specified names.
                case 0x0009:
                case 0x008A:
                case 0x009A:
                default:
                    bResult = TRUE;
                    break;      
			} //switch dwInst
			
		} //if(stDeltaToFind==stDeltaBuffer)                  
		
		// If we found a match then setup the offset for the deletion
        if (bResult == TRUE)
        {
			// set up the offset to delete from
            dwDelOffset = dwRecFiveOffset + sizeof(WORD) +
                w * (DWORD)sizeof(DELTA_T);
	
			// delete it
            if (WD7DeleteTDTData(lpstStream,
                                 lpstKey,
                                 dwDelOffset,
                                 sizeof(DELTA_T),
                                 lpstTDTInfo) != WD7_STATUS_OK)
				return(WD7_STATUS_ERROR);
			
			// once delete we need to change the count

			wTemp = (wCount - 1);

			// if the count is zero now, we want to get rid of the count and the Record0x05 header
			// otherwise just subtract one from it
			if (wTemp != 0)
			{
                wTemp = WENDIAN(wTemp);
				if (WD7EncryptedWrite(lpstStream,
                                      lpstKey,
                                      dwRecFiveOffset,
                                      (LPBYTE)&wTemp,
                                      sizeof(WORD)) != sizeof(WORD))
					return(WD7_STATUS_ERROR);
			}
			else
			{
                if (WD7DeleteTDTData(lpstStream,
                                     lpstKey,
                                     dwRecFiveOffset-1,
                                     3,
                                     lpstTDTInfo) != WD7_STATUS_OK)
					return(WD7_STATUS_ERROR);
				// There is no more Record0x05 left so, just return
				return(WD7_STATUS_OK);
			}
			
        } // if bResult == TRUE
		
	} // for(w<wCount) each delta

	return(WD7_STATUS_OK);
} // DeleteDelta


//********************************************************************
//
// Function:
//  BOOL WD7GetMCDInfo()
//
// Parameters:
//  lpstStream          Ptr to WordDocument stream
//  lpstKey             Ptr to encryption key
//  dwTDTOffset         Offset of TDT
//  dwTDTSize           Size of TDT
//  lpdwMCDOffset       Ptr to DWORD for first MCD offset
//  lpwMCDCount         Ptr to WORD for MCD count
//
// Description:
//  The function verifies that the TDT offset and size are
//  within the stream and then parses through the TDT looking
//  for the MCD records which are of type 0x01.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL WD7GetMCDInfo
(
    LPSS_STREAM         lpstStream,
    LPWD7ENCKEY         lpstKey,
    DWORD               dwTDTOffset,
    DWORD               dwTDTSize,
    LPDWORD             lpdwMCDOffset,
    LPWORD              lpwMCDCount
)
{
    BYTE                byTemp;
    DWORD               dwLimit;
    DWORD               dwOffset;
    WORD                wCount;
    WORD                wTemp;
    BYTE                abyWorkBuffer[12];
    BYTE                byRecType;
    DWORD               dwRecSize;

    // Validate offset and size

    dwLimit = SSStreamLen(lpstStream);

    if (dwTDTOffset > dwLimit ||
        dwLimit - dwTDTOffset < dwTDTSize)
        return(FALSE);

    // Verify that the first byte of the template data table is 0xFF

    if (WD7EncryptedRead(lpstStream,
                         lpstKey,
                         dwTDTOffset,
                         &byTemp,
                         sizeof(BYTE)) != sizeof(BYTE) ||
        byTemp != 0xFF)
        return(FALSE);

    // Now parse through the records in the table

    dwLimit = dwTDTOffset + dwTDTSize;
    dwOffset = dwTDTOffset + 1;
    while (dwOffset < dwLimit)
    {
        // Read the record type

        if (WD7EncryptedRead(lpstStream,
                             lpstKey,
                             dwOffset++,
                             &byRecType,
                             sizeof(BYTE)) != sizeof(BYTE))
            return(FALSE);

        if (byRecType == 0x11)
        {
            // Read number of names

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wCount,
                                 sizeof(WORD)) != sizeof(WORD))
                return(FALSE);

            wCount = WENDIAN(wCount);

            dwOffset += 2;

            while (wCount-- != 0)
            {
                dwOffset += 2;
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     &byTemp,
                                     sizeof(BYTE)) != sizeof(BYTE))
                    return(FALSE);

                // Skip over length byte, characters of name, and
                //  terminating zero character

                dwOffset += byTemp + 2;
            }
        }
        else
        if (byRecType >= 0x01 && byRecType <= 0x05)
        {
            switch (byRecType)
            {
                case 0x01: dwRecSize = 24; break;
                case 0x02: dwRecSize = 4;  break;
                case 0x03: dwRecSize = 14; break;
                case 0x04: dwRecSize = 14; break;
                default:   dwRecSize = 12; break;
            }

            // Read count

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wCount,
                                 sizeof(WORD)) != sizeof(WORD))
                return(FALSE);

            wCount = WENDIAN(wCount);

            dwOffset += 2;

            if (byRecType == 0x01)
            {
                // MCD table

                *lpdwMCDOffset = dwOffset;
                *lpwMCDCount = wCount;
                return(TRUE);
            }

            // Now skip that many records

            dwOffset += wCount * dwRecSize;
        }
        else
        if (byRecType == 0x06)
        {
            WORD    cstr, cbTdsh, cbBts, ctb, cbut;

            cstr = 0;

            // Read parameters

            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 abyWorkBuffer,
                                 8) != 8)
                return(FALSE);

            dwOffset += 8;

            cbTdsh = WENDIAN(((LPWORD)abyWorkBuffer)[0]);
            cbBts = WENDIAN(((LPWORD)abyWorkBuffer)[1]);
            ctb = WENDIAN(((LPWORD)abyWorkBuffer)[3]);

            while (ctb-- != 0)
            {
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wTemp,
                                     sizeof(WORD)) != sizeof(WORD))
                    return(FALSE);

                wTemp = WENDIAN(wTemp);

                dwOffset += cbTdsh;

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&cbut,
                                     sizeof(WORD)) != sizeof(WORD))
                    return(FALSE);

                cbut = WENDIAN(cbut);

                dwOffset += 2;

                while (cbut-- != 0)
                {
                    if (wTemp != 0xFFFF)
                        dwOffset += 6;

                    if (WD7EncryptedRead(lpstStream,
                                         lpstKey,
                                         dwOffset,
                                         abyWorkBuffer,
                                         12) != 12)
                        return(FALSE);

                    dwOffset += cbBts;

                    if ((abyWorkBuffer[11] & 0xC0) == 0x40)
                        ++cstr;
                }
            }

            while (cstr-- != 0)
            {
                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&byTemp,
                                     sizeof(BYTE)) != sizeof(BYTE))
                    return(FALSE);

                dwOffset += 1 + byTemp;
            }
        }
        else
        if (byRecType == 0x10)
        {
            if (WD7EncryptedRead(lpstStream,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wCount,
                                 sizeof(WORD)) != sizeof(WORD))
                return(FALSE);

            wCount = WENDIAN(wCount);

            // For Chinese version of Word, found that
            //  0xFFFF means Unicode strings?

            if (wCount == (WORD)0xFFFF)
            {
                dwOffset += 2;

                if (WD7EncryptedRead(lpstStream,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wCount,
                                     sizeof(WORD)) != sizeof(WORD))
                    return(FALSE);

                wCount = WENDIAN(wCount) - 2;
            }

            dwOffset += wCount;
        }
        else
        if (byRecType == 0x12)
        {
            dwOffset += 2;
        }
        else
        if (byRecType == 0x40)
        {
            dwOffset += 2;
            break;
        }
        else
        {
            // Invalid

            return(FALSE);
        }
    }

    // Did not find any MCDs

    *lpdwMCDOffset = 0;
    *lpwMCDCount = 0;

    return(TRUE);
}



