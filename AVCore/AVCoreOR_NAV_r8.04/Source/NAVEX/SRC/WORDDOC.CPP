//************************************************************************
//
// $Header:   S:/NAVEX/VCS/WORDDOC.CPv   1.24   08 May 1997 15:36:44   DCHI  $
//
// Description:
//      Contains WordDocument interface code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/WORDDOC.CPv  $
// 
//    Rev 1.24   08 May 1997 15:36:44   DCHI
// Added office 97 conversion of template to document and added criteria
// for absence of AutoText before converting to document for both Word versions.
// 
//    Rev 1.23   08 Apr 1997 12:41:00   DCHI
// Added support for FullSet(), FullSetRepair, Or()/Not(), MacroCount(), etc.
// 
//    Rev 1.22   14 Mar 1997 16:35:22   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.21   10 Mar 1997 15:52:28   DCHI
// Added magic numbers to DOS only section also.
// 
//    Rev 1.20   10 Mar 1997 15:44:56   DCHI
// Added 0xA698 to list of magic numbers.
// 
//    Rev 1.19   10 Mar 1997 11:50:16   DCHI
// Added 0xA697 magic number to known list.
// 
//    Rev 1.18   18 Feb 1997 11:44:42   DCHI
// Fixed problem in GetMacroInfoAtIndex where the cached offset of the
// next MCD structure was not used appropriately.
// 
//    Rev 1.17   13 Feb 1997 13:29:26   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.16   17 Jan 1997 11:02:02   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.15   26 Dec 1996 15:22:12   AOONWAL
// No change.
// 
//    Rev 1.14   02 Dec 1996 14:01:54   AOONWAL
// No change.
// 
//    Rev 1.13   13 Nov 1996 13:55:30   DCHI
// Commented out unused code.
// 
//    Rev 1.12   13 Nov 1996 13:04:32   DCHI
// Added switch statement to check additional valid word magic numbers.
// 
//    Rev 1.11   06 Nov 1996 17:17:38   DCHI
// Added functionality for wild macro scanning.
// 
//    Rev 1.10   29 Oct 1996 12:58:52   AOONWAL
// No change.
// 
//    Rev 1.9   17 Oct 1996 15:31:48   DCHI
// Entered in changes that allow parsing of Chinese generated documents.
// 
//    Rev 1.8   28 Aug 1996 16:43:48   DCHI
// Added BAT cache support.
// 
//    Rev 1.7   09 May 1996 15:54:22   DCHI
// One more endianization addition.
// 
//    Rev 1.6   09 May 1996 11:10:50   DCHI
// Endian corrections.
// 
//    Rev 1.5   06 May 1996 17:20:40   DCHI
// Added WordDocHasActiveMacros() and WordDocHasNonMacroTemplateInfo()
// functions and modified FindMacroTables() function to note record types seen.
// 
//    Rev 1.4   03 May 1996 13:40:10   DCHI
// Corrected parsing of record type #6 in template data table.
// 
//    Rev 1.3   03 May 1996 13:26:58   DCHI
// Endian-enabled the code for Mac compatibility.
// 
//    Rev 1.2   12 Jan 1996 13:12:36   DCHI
// Corrected parsing of record type 6 of template data table
// 
//    Rev 1.1   05 Jan 1996 11:32:44   DCHI
// Corrected parsing of record type 0x06.
// 
//    Rev 1.0   03 Jan 1996 17:14:50   DCHI
// Initial revision.
// 
//************************************************************************

#if defined(SYM_DOSX) || !defined(SYM_DOS)

#include "storobj.h"

#include "wdencdoc.h"
#include "worddoc.h"
#include "wdencrd.h"
#include "endutils.h"

//********************************************************************
//
// Function:
//	WORD FindMacroTables()
//
// Description:
//	Searches for the macro tables and fills in the MACRO_TABLE_INFO_T
//	structure.
//
// Returns:
//	1	on success
//	0	on error
//
//********************************************************************

WORD FindMacroTables
(
	LPCALLBACKREV1 		lpCallBack,				// File op callbacks
	LPOLE_FILE_T		pOLEFile,				// OLE file info
	LPBYTE 				lpbyWorkBuffer,			// Work buffer >= 512 bytes
    LPWDENCKEY          lpstKey,                // Decryption key
	DWORD 				dwTemplateTableOffset,	// Template table offset
	DWORD 				dwTemplateTableSize,	// Template table size
	LPMACRO_TABLE_INFO  pMacroTableInfo			// Macro table info
)
{
	BYTE byTemp, byRecType;
	WORD wTemp, wCount, dwRecSize;
	DWORD dwOffset;

    pMacroTableInfo->dwMacroTableContents = 0;
	pMacroTableInfo->wNumMacros = 0xFFFF;
	pMacroTableInfo->wNumMCDs = 0xFFFF;
	pMacroTableInfo->wULNameTableSize = 0xFFFF;
    
	// Verify that the first byte of the template data table is 0xFF

    if (WordDocEncryptedRead(lpCallBack,
                             pOLEFile,
                             lpstKey,
                             dwTemplateTableOffset,
                             &byTemp,
                             1) != 1 ||
		byTemp != 0xFF)
		return 0;

	// Now parse through the records in the table

	dwOffset = dwTemplateTableOffset + 1;
    while (dwOffset < (dwTemplateTableOffset+dwTemplateTableSize))
	{
		// Read the record type

        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset++,
                                 &byRecType,
                                 1) != 1)
			return 0;

		if (byRecType == 0x11)
		{
			WORD i;
			BYTE bCount;

            pMacroTableInfo->dwMacroTableContents |= MACRO_TABLE_TYPE_0X11;

			// Bingo, macro name table

			// Read number of macros

            if (WordDocEncryptedRead(lpCallBack,
                                     pOLEFile,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&(pMacroTableInfo->wNumMacros),
                                     2) != 2)
				return 0;

            pMacroTableInfo->wNumMacros =
                ConvertEndianShort(pMacroTableInfo->wNumMacros);

			if (pMacroTableInfo->wNumMacros == 0)
				return 0;

			dwOffset += 2;

			pMacroTableInfo->dwMacroNameTableOffset = dwOffset;

            for (i=pMacroTableInfo->wNumMacros;i!=0;i--)
			{
				dwOffset += 2;
                if (WordDocEncryptedRead(lpCallBack,
                                         pOLEFile,
                                         lpstKey,
                                         dwOffset,
                                         (LPBYTE)(&bCount),
                                         1) != 1)
					return 0;

				dwOffset += (DWORD)bCount + (DWORD)2;
			}
		}
		else
		if (byRecType >= 0x01 && byRecType <= 0x05)
		{
			switch (byRecType)
			{
				case 0x01:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X01;
                    dwRecSize = 24;
					break;

				case 0x02:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X02;
                    dwRecSize = 4;
					break;

				case 0x03:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X03;
                    dwRecSize = 14;
                    break;

                case 0x04:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X04;
                    dwRecSize = 14;
					break;

				default:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X05;
                    dwRecSize = 12;
					break;
			}

			// Read count

            if (WordDocEncryptedRead(lpCallBack,
                                     pOLEFile,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wCount,
                                     2) != 2)
				return 0;

            wCount = ConvertEndianShort(wCount);

            dwOffset += 2;

			if (byRecType == 0x01)
			{
				// MCD table

				pMacroTableInfo->dwMCDTableOffset = dwOffset;
				pMacroTableInfo->wNumMCDs = wCount;
			}

            // Now skip that many records

			dwOffset += wCount * dwRecSize;
		}
		else
		if (byRecType == 0x06)
		{
			WORD	cstr, cbTdsh, cbBts, ctb, cbut;

            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X06;

            cstr = 0;

			// Read parameters

            if (WordDocEncryptedRead(lpCallBack,
                                     pOLEFile,
                                     lpstKey,
                                     dwOffset,
                                     lpbyWorkBuffer,
                                     8) != 8)
				return 0;

			dwOffset += 8;

            cbTdsh = ConvertEndianShort(((LPWORD)lpbyWorkBuffer)[0]);
            cbBts = ConvertEndianShort(((LPWORD)lpbyWorkBuffer)[1]);
            ctb = ConvertEndianShort(((LPWORD)lpbyWorkBuffer)[3]);

            while (ctb-- != 0)
			{
                if (WordDocEncryptedRead(lpCallBack,
                                         pOLEFile,
                                         lpstKey,
                                         dwOffset,
                                         (LPBYTE)&wTemp,
                                         2) != 2)
					return 0;

                wTemp = ConvertEndianShort(wTemp);

				dwOffset += cbTdsh;

                if (WordDocEncryptedRead(lpCallBack,
                                         pOLEFile,
                                         lpstKey,
                                         dwOffset,
                                         (LPBYTE)&cbut,
                                         2) != 2)
					return 0;

                cbut = ConvertEndianShort(cbut);

                dwOffset += 2;

				while (cbut-- != 0)
				{
					if (wTemp != 0xFFFF)
						dwOffset += 6;

                    if (WordDocEncryptedRead(lpCallBack,
                                             pOLEFile,
                                             lpstKey,
                                             dwOffset,
                                             lpbyWorkBuffer,
                                             12) != 12)
						return 0;

                    dwOffset += cbBts;

                    if ((lpbyWorkBuffer[11] & 0xC0) == 0x40)
						++cstr;
				}
			}

			while (cstr-- != 0)
			{
                if (WordDocEncryptedRead(lpCallBack,
                                         pOLEFile,
                                         lpstKey,
                                         dwOffset,
                                         (LPBYTE)&byTemp,
                                         1) != 1)
					return 0;

				dwOffset += 1 + byTemp;
			}
		}
		else
		if (byRecType == 0x10)
		{
            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X10;

            if (WordDocEncryptedRead(lpCallBack,
                                     pOLEFile,
                                     lpstKey,
                                     dwOffset,
                                     (LPBYTE)&wCount,
                                     2) != 2)
				return 0;

            wCount = ConvertEndianShort(wCount);

            // For Chinese version of Word, found that
            //  0xFFFF means Unicode strings?

            if (wCount == 0xFFFF)
            {
                dwOffset += 2;

                if (WordDocEncryptedRead(lpCallBack,
                                         pOLEFile,
                                         lpstKey,
                                         dwOffset,
                                         (LPBYTE)&wCount,
                                         2) != 2)
                    return 0;

                wCount = ConvertEndianShort(wCount) - 2;
            }

			pMacroTableInfo->dwULNameTableOffset = dwOffset + 2;
			pMacroTableInfo->wULNameTableSize = wCount - 2;

			dwOffset += wCount;
		}
		else
		if (byRecType == 0x12)
		{
            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X12;

			dwOffset += 2;
		}
		else
		if (byRecType == 0x40)
		{
            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X40;

            dwOffset += 2;
			break;
		}
		else
		{
			// Invalid

			return 0;
		}
	}

	// No macros?

	if (pMacroTableInfo->wNumMacros == 0xFFFF ||
		pMacroTableInfo->wNumMCDs == 0xFFFF ||
		pMacroTableInfo->wULNameTableSize == 0xFFFF)
		return 0;

	// Good deal!

	return 1;
}


//********************************************************************
//
// Function:
//	WORD WordDocContainsMacros()
//
// Returns:
//	1	If the opened stream contains macros
//	0	If error or if the stream has no macros
//
//********************************************************************

WORD WordDocContainsMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
	LPBYTE 				lpbyWorkBuffer,		// Work buffer >= 512 bytes
	LPMACRO_TABLE_INFO  pMacroTableInfo		// Macro table info
)
{
    WORD                wMagicNumber;

	// Read some initial info from the beginning for verification

    if (WordDocEncryptedRead(lpCallBack,
                             pOLEFile,
                             lpstKey,
                             0,
                             lpbyWorkBuffer,
                             288) != 288)
		return 0;

    // Make sure the magic number is one we recognize

    wMagicNumber = ConvertEndianShort(*((LPWORD)lpbyWorkBuffer));

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
            break;

        default:
            // Unrecognized
            return 0;
    }

//    // Word 6.0 and 7.0 files begin with 0xA5DC
//
//    if (ConvertEndianShort(*((LPWORD)lpbyWorkBuffer)) != (WORD)(0xA5DC) &&
//        ConvertEndianShort(*((LPWORD)lpbyWorkBuffer)) != (WORD)(0xA699))
//        return 0;

//	// Now check if this is a template file
//
//	if ((lpbyWorkBuffer[10] & 0x01) == 0)
//		return 0;

	// Check the size of the template data table
	//	If > 2 then possibility exists

    if (ConvertEndianLong(*((LPDWORD)(lpbyWorkBuffer+284))) > 2)
	{
        if (FindMacroTables(lpCallBack,
                            pOLEFile,
                            lpbyWorkBuffer,
                            lpstKey,
                            ConvertEndianLong(*((LPDWORD)(lpbyWorkBuffer+280))),
                            ConvertEndianLong(*((LPDWORD)(lpbyWorkBuffer+284))),
                            pMacroTableInfo) == 0)
			return 0;

		return 1;
	}

	// No way, ho say!

	return 0;
}


//********************************************************************
//
// Function:
//	WORD WordDocGetMacroInfo()
//
// Description:
//	Fills in the MACRO_INFO_T structure for the named macro.
//
// Returns:
//	WORDDOC_OK			on success
//	WORDDOC_ERR_???		on error
//
//********************************************************************

WORD WordDocGetMacroInfo
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
	LPMACRO_INFO_T		pMacroInfo,			// Macro info
	LPBYTE 				lpbyWorkBuffer,		// Work buffer >= 512 bytes
	LPSTR 				lpszMacroName		// Name of macro
)
{
	WORD i, j;
	WORD wID;
	BYTE byLen;
	DWORD dwOffset;

	// First find macro name in macro name table

	dwOffset = pMacroTableInfo->dwMacroNameTableOffset;
	for (i=0;i<pMacroTableInfo->wNumMacros;i++)
	{
        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&wID,
                                 2) != 2)
			return WORDDOC_ERR_READ_ID;

		dwOffset += 2;

        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset,
                                 (LPBYTE)&byLen,
                                 1) != 1)
			return WORDDOC_ERR_READ_LEN;

		dwOffset += 1;

        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset,
                                 lpbyWorkBuffer,
                                 byLen+1) !=
			byLen + 1)
			return WORDDOC_ERR_READ_NAME;

		// See if the name matches

		for (j=0;j<=(WORD)byLen;j++)
        {
			if (lpszMacroName[j] != lpbyWorkBuffer[j])
				break;
		}

		if (j > (WORD)byLen)
		{
			// Found a matching macro name

			break;
		}

		dwOffset += byLen + 1;
	}

	if (i == pMacroTableInfo->wNumMacros)
	{
		// No matching macro name found

		return WORDDOC_ERR_NO_SUCH_NAME;
	}

	// Note offset of name

	pMacroInfo->dwNameOffset = dwOffset;
	pMacroInfo->byNameLen = byLen;

	// Then look for MCD structure in MCD table

	dwOffset = pMacroTableInfo->dwMCDTableOffset;
	for (i=0;i<pMacroTableInfo->wNumMCDs;i++)
	{
        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset,
                                 lpbyWorkBuffer,
                                 sizeof(MCD_T)) != sizeof(MCD_T))
			return WORDDOC_ERR_READ_MCD;

		if (((MCD_T FAR *)(lpbyWorkBuffer))->wID == wID)
		{
			// Found a match

			// Verify dwN field
			// Questionable whether we should make this check

			if (((MCD_T FAR *)(lpbyWorkBuffer))->dwN == 0)
			{
				// This macro is invalid

				return WORDDOC_ERR_NO_MCD;
			}

			pMacroInfo->dwMCDOffset = dwOffset;
			pMacroInfo->dwOffset =
                ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwOffset);
            pMacroInfo->dwSize =
                ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwSize);
            pMacroInfo->byEncrypt =
				((MCD_T FAR *)(lpbyWorkBuffer))->byEncrypt;
			pMacroInfo->wULNameIdx =
                ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wULNameIdx);
			pMacroInfo->wMenuHelp =
                ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wMenuHelp);

			return WORDDOC_OK;
		}

		dwOffset += sizeof(MCD_T);
	}

	return WORDDOC_ERR_NO_MCD;
}


//********************************************************************
//
// Function:
//  WORD WordDocHasActiveMacros()
//
// Description:
//  Determines whether the document contains active macros
//
// Returns:
//  0       If the document doesn't have active macros
//  1       If the document does have active macros
//
//********************************************************************

WORD WordDocHasActiveMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    WORD    i;
    DWORD   dwOffset;

    // Look for MCD structures in MCD table

	dwOffset = pMacroTableInfo->dwMCDTableOffset;
	for (i=0;i<pMacroTableInfo->wNumMCDs;i++)
	{
        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset,
                                 lpbyWorkBuffer,
                                 sizeof(MCD_T)) != sizeof(MCD_T))
			return WORDDOC_ERR_READ_MCD;

        if (((MCD_T FAR *)(lpbyWorkBuffer))->dwN != 0)
		{
            // Found an active macro

            return 1;
        }

		dwOffset += sizeof(MCD_T);
	}

    // Document does not contain any active macros

    return 0;
}


//********************************************************************
//
// Function:
//  WORD WordDocCountActiveMacros()
//
// Description:
//  Counts the number of active macros in the document
//
// Returns:
//  0       If the document doesn't have active macros
//  1       If the document does have active macros
//
//********************************************************************

WORD WordDocCountActiveMacros
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,        // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  lpstMacroTableInfo, // Macro table info
    LPWORD              lpwNumMacros,       // Ptr to WORD for count
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 64 bytes
)
{
    WORD                i;
    DWORD               dwOffset;

    // Look for MCD structures in MCD table

    *lpwNumMacros = 0;
    dwOffset = lpstMacroTableInfo->dwMCDTableOffset;
    for (i=0;i<lpstMacroTableInfo->wNumMCDs;i++)
	{
        if (WordDocEncryptedRead(lpstCallBack,
                                 lpstOLEFile,
                                 lpstKey,
                                 dwOffset,
                                 lpbyWorkBuffer,
                                 sizeof(MCD_T)) != sizeof(MCD_T))
            return(WORDDOC_ERR_READ_MCD);

        if (((MCD_T FAR *)(lpbyWorkBuffer))->dwN != 0 &&
            *lpwNumMacros < 0xFFFF)
		{
            // Found an active macro

            ++*lpwNumMacros;
        }

		dwOffset += sizeof(MCD_T);
	}

    return(WORDDOC_OK);
}


//********************************************************************
//
// Function:
//  WORD WordDocGetMacroInfoAtIndex()
//
// Description:
//  Fills in the MACRO_INFO_T structure for the macro at the given
//  index.
//
// Returns:
//	WORDDOC_OK			on success
//	WORDDOC_ERR_???		on error
//
//********************************************************************

WORD WordDocGetMacroInfoAtIndex
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPWD_GET_MACRO_INFO lpstGetInfo,        // Get info cache structure
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPMACRO_INFO_T      pMacroInfo,         // Macro info
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPSTR               lpszMacroName       // Buffer to store name of macro
)
{
    WORD                i;
    BYTE                abyIDandLen[3];     // ID and length
    WORD                wID;
    BYTE                byLen;
    DWORD               dwOffset;
    BOOL                bFoundMCD;

    if (lpstGetInfo->wNextNameIndex >= pMacroTableInfo->wNumMacros)
        return(WORDDOC_ERR_NO_SUCH_NAME);

    /////////////////////////////////////////////////////////////
    // First find macro name in macro name table
    /////////////////////////////////////////////////////////////

    if (lpstGetInfo->bUseIndex == TRUE)
    {
        // Start from the beginning

        dwOffset = pMacroTableInfo->dwMacroNameTableOffset;
        i = 0;
    }
    else
    {
        // Continue from last point

        dwOffset = lpstGetInfo->dwNextNameOffset;
        i = lpstGetInfo->wNextNameIndex;
    }

    // Search for the name at the index i

    while (1)
    {
        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset,
                                 abyIDandLen,
                                 3) != 3)
            return WORDDOC_ERR_READ_ID;

        dwOffset += 3;

        if (i == lpstGetInfo->wNextNameIndex)
            break;

        dwOffset += abyIDandLen[2] + 1;

        ++i;
    }

    byLen = abyIDandLen[2];
    wID = *(LPWORD)abyIDandLen;

    // Get the name

    if (WordDocEncryptedRead(lpCallBack,
                             pOLEFile,
                             lpstKey,
                             dwOffset,
                             (LPBYTE)lpszMacroName,
                             byLen+1) != byLen + 1)
        return WORDDOC_ERR_READ_NAME;

    lpszMacroName[byLen] = '\0';

    // Note offset of name

    pMacroInfo->dwNameOffset = dwOffset;
    pMacroInfo->byNameLen = byLen;

    // Cache offset and next offset

    lpstGetInfo->wNextNameIndex = i + 1;
    lpstGetInfo->dwNextNameOffset = dwOffset + byLen + 1;

    /////////////////////////////////////////////////////////////
    // Get the MCD structure
    /////////////////////////////////////////////////////////////

    bFoundMCD = FALSE;
    if (lpstGetInfo->bUseIndex == FALSE)
    {
        // Search from the last point forward

        dwOffset = lpstGetInfo->dwNextMCDOffset;
        for (i=lpstGetInfo->wNextMCDIndex;i<pMacroTableInfo->wNumMCDs;i++)
        {
            if (WordDocEncryptedRead(lpCallBack,
                                     pOLEFile,
                                     lpstKey,
                                     dwOffset,
                                     lpbyWorkBuffer,
                                     sizeof(MCD_T)) != sizeof(MCD_T))
                return WORDDOC_ERR_READ_MCD;

            if (((MCD_T FAR *)(lpbyWorkBuffer))->wID == wID)
            {
                bFoundMCD = TRUE;
                break;
            }

            dwOffset += sizeof(MCD_T);
        }
    }

    // Then look for MCD structure in MCD table

    if (bFoundMCD == FALSE)
    {
        // Start from the beginning

        dwOffset = pMacroTableInfo->dwMCDTableOffset;
        for (i=0;i<pMacroTableInfo->wNumMCDs;i++)
        {
            if (WordDocEncryptedRead(lpCallBack,
                                     pOLEFile,
                                     lpstKey,
                                     dwOffset,
                                     lpbyWorkBuffer,
                                     sizeof(MCD_T)) != sizeof(MCD_T))
                return WORDDOC_ERR_READ_MCD;

            if (((MCD_T FAR *)(lpbyWorkBuffer))->wID == wID)
                break;

            dwOffset += sizeof(MCD_T);
        }
    }

    if (i < pMacroTableInfo->wNumMCDs)
    {
        // Found a match

        // Verify dwN field
        // Questionable whether we should make this check

        if (((MCD_T FAR *)(lpbyWorkBuffer))->dwN == 0)
        {
            // This macro is invalid

            return WORDDOC_ERR_NO_MCD;
        }

        pMacroInfo->dwMCDOffset = dwOffset;
        pMacroInfo->dwOffset =
            ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwOffset);
        pMacroInfo->dwSize =
            ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwSize);
        pMacroInfo->byEncrypt =
            ((MCD_T FAR *)(lpbyWorkBuffer))->byEncrypt;
        pMacroInfo->wULNameIdx =
            ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wULNameIdx);
        pMacroInfo->wMenuHelp =
            ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wMenuHelp);

        lpstGetInfo->wNextMCDIndex = i + 1;
        lpstGetInfo->dwNextMCDOffset = dwOffset + sizeof(MCD_T);

        return WORDDOC_OK;
    }

    return WORDDOC_ERR_NO_MCD;
}

#else

// Plain DOS below

#include "storobj.h"
#include "worddoc.h"

#include "endutils.h"

//********************************************************************
//
// Function:
//	WORD FindMacroTables()
//
// Description:
//	Searches for the macro tables and fills in the MACRO_TABLE_INFO_T
//	structure.
//
// Returns:
//	1	on success
//	0	on error
//
//********************************************************************

WORD FindMacroTables
(
	LPCALLBACKREV1 		lpCallBack,				// File op callbacks
	LPOLE_FILE_T		pOLEFile,				// OLE file info
	LPBYTE 				lpbyWorkBuffer,			// Work buffer >= 512 bytes
	DWORD 				dwTemplateTableOffset,	// Template table offset
	DWORD 				dwTemplateTableSize,	// Template table size
	LPMACRO_TABLE_INFO  pMacroTableInfo			// Macro table info
)
{
	BYTE byTemp, byRecType;
	WORD wTemp, wCount, dwRecSize;
	DWORD dwOffset;

    pMacroTableInfo->dwMacroTableContents = 0;
	pMacroTableInfo->wNumMacros = 0xFFFF;
	pMacroTableInfo->wNumMCDs = 0xFFFF;
	pMacroTableInfo->wULNameTableSize = 0xFFFF;
    
	// Verify that the first byte of the template data table is 0xFF

    if (OLESeekRead(lpCallBack,
		pOLEFile,dwTemplateTableOffset,&byTemp,1) != 1 ||
		byTemp != 0xFF)
		return 0;

	// Now parse through the records in the table

	dwOffset = dwTemplateTableOffset + 1;
    while (dwOffset < (dwTemplateTableOffset+dwTemplateTableSize))
	{
		// Read the record type

	    if (OLESeekRead(lpCallBack,
			pOLEFile,dwOffset++,&byRecType,1) != 1)
			return 0;

		if (byRecType == 0x11)
		{
			WORD i;
			BYTE bCount;

            pMacroTableInfo->dwMacroTableContents |= MACRO_TABLE_TYPE_0X11;

			// Bingo, macro name table

			// Read number of macros

			if (OLESeekRead(lpCallBack,
				pOLEFile,dwOffset,
                (LPBYTE)&(pMacroTableInfo->wNumMacros),2) != 2)
				return 0;

            pMacroTableInfo->wNumMacros =
                ConvertEndianShort(pMacroTableInfo->wNumMacros);

			if (pMacroTableInfo->wNumMacros == 0)
				return 0;

			dwOffset += 2;

			pMacroTableInfo->dwMacroNameTableOffset = dwOffset;

            for (i=pMacroTableInfo->wNumMacros;i!=0;i--)
			{
				dwOffset += 2;
				if (OLESeekRead(lpCallBack,pOLEFile,
					dwOffset,(LPBYTE)(&bCount),1) != 1)
					return 0;

				dwOffset += (DWORD)bCount + (DWORD)2;
			}
		}
		else
		if (byRecType >= 0x01 && byRecType <= 0x05)
		{
			switch (byRecType)
			{
				case 0x01:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X01;
                    dwRecSize = 24;
					break;

				case 0x02:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X02;
                    dwRecSize = 4;
					break;

				case 0x03:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X03;
                    dwRecSize = 14;
                    break;

                case 0x04:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X04;
                    dwRecSize = 14;
					break;

				default:
                    pMacroTableInfo->dwMacroTableContents |=
                        MACRO_TABLE_TYPE_0X05;
                    dwRecSize = 12;
					break;
			}

			// Read count

		    if (OLESeekRead(lpCallBack,
				pOLEFile,dwOffset,(LPBYTE)&wCount,2) != 2)
				return 0;

            wCount = ConvertEndianShort(wCount);

            dwOffset += 2;

			if (byRecType == 0x01)
			{
				// MCD table

				pMacroTableInfo->dwMCDTableOffset = dwOffset;
				pMacroTableInfo->wNumMCDs = wCount;
			}

            // Now skip that many records

			dwOffset += wCount * dwRecSize;
		}
		else
		if (byRecType == 0x06)
		{
			WORD	cstr, cbTdsh, cbBts, ctb, cbut;

            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X06;

            cstr = 0;

			// Read parameters

		    if (OLESeekRead(lpCallBack,
				pOLEFile,dwOffset,lpbyWorkBuffer,8) != 8)
				return 0;

			dwOffset += 8;

            cbTdsh = ConvertEndianShort(((LPWORD)lpbyWorkBuffer)[0]);
            cbBts = ConvertEndianShort(((LPWORD)lpbyWorkBuffer)[1]);
            ctb = ConvertEndianShort(((LPWORD)lpbyWorkBuffer)[3]);

            while (ctb-- != 0)
			{
			    if (OLESeekRead(lpCallBack,
					pOLEFile,dwOffset,(LPBYTE)&wTemp,2) != 2)
					return 0;

                wTemp = ConvertEndianShort(wTemp);

				dwOffset += cbTdsh;

			    if (OLESeekRead(lpCallBack,
					pOLEFile,dwOffset,(LPBYTE)&cbut,2) != 2)
					return 0;

                cbut = ConvertEndianShort(cbut);

                dwOffset += 2;

				while (cbut-- != 0)
				{
					if (wTemp != 0xFFFF)
						dwOffset += 6;

				    if (OLESeekRead(lpCallBack,
                        pOLEFile,dwOffset,lpbyWorkBuffer,12)
                        != 12)
						return 0;

                    dwOffset += cbBts;

                    if ((lpbyWorkBuffer[11] & 0xC0) == 0x40)
						++cstr;
				}
			}

			while (cstr-- != 0)
			{
			    if (OLESeekRead(lpCallBack,
					pOLEFile,dwOffset,(LPBYTE)&byTemp,1) != 1)
					return 0;

				dwOffset += 1 + byTemp;
			}
		}
		else
		if (byRecType == 0x10)
		{
            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X10;

            if (OLESeekRead(lpCallBack,
				pOLEFile,dwOffset,(LPBYTE)&wCount,2) != 2)
				return 0;

            wCount = ConvertEndianShort(wCount);

            // For Chinese version of Word, found that
            //  0xFFFF means Unicode strings?

            if (wCount == 0xFFFF)
            {
                dwOffset += 2;

                if (OLESeekRead(lpCallBack,
                    pOLEFile,dwOffset,(LPBYTE)&wCount,2) != 2)
                    return 0;

                wCount = ConvertEndianShort(wCount) - 2;
            }

			pMacroTableInfo->dwULNameTableOffset = dwOffset + 2;
			pMacroTableInfo->wULNameTableSize = wCount - 2;

			dwOffset += wCount;
		}
		else
		if (byRecType == 0x12)
		{
            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X12;

			dwOffset += 2;
		}
		else
		if (byRecType == 0x40)
		{
            pMacroTableInfo->dwMacroTableContents |=
                MACRO_TABLE_TYPE_0X40;

            dwOffset += 2;
			break;
		}
		else
		{
			// Invalid

			return 0;
		}
	}

	// No macros?

	if (pMacroTableInfo->wNumMacros == 0xFFFF ||
		pMacroTableInfo->wNumMCDs == 0xFFFF ||
		pMacroTableInfo->wULNameTableSize == 0xFFFF)
		return 0;

	// Good deal!

	return 1;
}


//********************************************************************
//
// Function:
//	WORD WordDocContainsMacros()
//
// Returns:
//	1	If the opened stream contains macros
//	0	If error or if the stream has no macros
//
//********************************************************************

WORD WordDocContainsMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPBYTE 				lpbyWorkBuffer,		// Work buffer >= 512 bytes
	LPMACRO_TABLE_INFO  pMacroTableInfo		// Macro table info
)
{
    WORD                wMagicNumber;

	// Read some initial info from the beginning for verification

    if (OLESeekRead(lpCallBack,pOLEFile,0,lpbyWorkBuffer,288) != 288)
		return 0;

    // Make sure the magic number is one we recognize

    wMagicNumber = ConvertEndianShort(*((LPWORD)lpbyWorkBuffer));

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
            break;

        default:
            // Unrecognized
            return 0;
    }

//    // Word 6.0 and 7.0 files begin with 0xA5DC
//
//    if (ConvertEndianShort(*((LPWORD)lpbyWorkBuffer)) != (WORD)(0xA5DC) &&
//        ConvertEndianShort(*((LPWORD)lpbyWorkBuffer)) != (WORD)(0xA699))
//        return 0;

//	// Now check if this is a template file
//
//	if ((lpbyWorkBuffer[10] & 0x01) == 0)
//		return 0;

	// Check the size of the template data table
	//	If > 2 then possibility exists

    if (ConvertEndianLong(*((LPDWORD)(lpbyWorkBuffer+284))) > 2)
	{
		if (FindMacroTables(lpCallBack,pOLEFile,lpbyWorkBuffer,
            ConvertEndianLong(*((LPDWORD)(lpbyWorkBuffer+280))),
            ConvertEndianLong(*((LPDWORD)(lpbyWorkBuffer+284))),
			pMacroTableInfo) == 0)
			return 0;

		return 1;
	}

	// No way, ho say!

	return 0;
}


//********************************************************************
//
// Function:
//	WORD WordDocGetMacroInfo()
//
// Description:
//	Fills in the MACRO_INFO_T structure for the named macro.
//
// Returns:
//	WORDDOC_OK			on success
//	WORDDOC_ERR_???		on error
//
//********************************************************************

WORD WordDocGetMacroInfo
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPMACRO_TABLE_INFO  pMacroTableInfo,	// Macro table info
	LPMACRO_INFO_T		pMacroInfo,			// Macro info
	LPBYTE 				lpbyWorkBuffer,		// Work buffer >= 512 bytes
	LPSTR 				lpszMacroName		// Name of macro
)
{
	WORD i, j;
	WORD wID;
	BYTE byLen;
	DWORD dwOffset;

	// First find macro name in macro name table

	dwOffset = pMacroTableInfo->dwMacroNameTableOffset;
	for (i=0;i<pMacroTableInfo->wNumMacros;i++)
	{
	    if (OLESeekRead(lpCallBack,
			pOLEFile,dwOffset,(LPBYTE)&wID,2) != 2)
			return WORDDOC_ERR_READ_ID;

		dwOffset += 2;

	    if (OLESeekRead(lpCallBack,
			pOLEFile,dwOffset,(LPBYTE)&byLen,1) != 1)
			return WORDDOC_ERR_READ_LEN;

		dwOffset += 1;

	    if (OLESeekRead(lpCallBack,
			pOLEFile,dwOffset,lpbyWorkBuffer,byLen+1) !=
			byLen + 1)
			return WORDDOC_ERR_READ_NAME;

		// See if the name matches

		for (j=0;j<=(WORD)byLen;j++)
        {
			if (lpszMacroName[j] != lpbyWorkBuffer[j])
				break;
		}

		if (j > (WORD)byLen)
		{
			// Found a matching macro name

			break;
		}

		dwOffset += byLen + 1;
	}

	if (i == pMacroTableInfo->wNumMacros)
	{
		// No matching macro name found

		return WORDDOC_ERR_NO_SUCH_NAME;
	}

	// Note offset of name

	pMacroInfo->dwNameOffset = dwOffset;
	pMacroInfo->byNameLen = byLen;

	// Then look for MCD structure in MCD table

	dwOffset = pMacroTableInfo->dwMCDTableOffset;
	for (i=0;i<pMacroTableInfo->wNumMCDs;i++)
	{
	    if (OLESeekRead(lpCallBack,
			pOLEFile,dwOffset,lpbyWorkBuffer,
			sizeof(MCD_T)) != sizeof(MCD_T))
			return WORDDOC_ERR_READ_MCD;

		if (((MCD_T FAR *)(lpbyWorkBuffer))->wID == wID)
		{
			// Found a match

			// Verify dwN field
			// Questionable whether we should make this check

			if (((MCD_T FAR *)(lpbyWorkBuffer))->dwN == 0)
			{
				// This macro is invalid

				return WORDDOC_ERR_NO_MCD;
			}

			pMacroInfo->dwMCDOffset = dwOffset;
			pMacroInfo->dwOffset =
                ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwOffset);
            pMacroInfo->dwSize =
                ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwSize);
            pMacroInfo->byEncrypt =
				((MCD_T FAR *)(lpbyWorkBuffer))->byEncrypt;
			pMacroInfo->wULNameIdx =
                ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wULNameIdx);
			pMacroInfo->wMenuHelp =
                ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wMenuHelp);

			return WORDDOC_OK;
		}

		dwOffset += sizeof(MCD_T);
	}

	return WORDDOC_ERR_NO_MCD;
}


//********************************************************************
//
// Function:
//  WORD WordDocHasActiveMacros()
//
// Description:
//  Determines whether the document contains active macros
//
// Returns:
//  0       If the document doesn't have active macros
//  1       If the document does have active macros
//
//********************************************************************

WORD WordDocHasActiveMacros
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPMACRO_TABLE_INFO  pMacroTableInfo,	// Macro table info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    WORD    i;
    DWORD   dwOffset;

    // Look for MCD structures in MCD table

	dwOffset = pMacroTableInfo->dwMCDTableOffset;
	for (i=0;i<pMacroTableInfo->wNumMCDs;i++)
	{
	    if (OLESeekRead(lpCallBack,
			pOLEFile,dwOffset,lpbyWorkBuffer,
			sizeof(MCD_T)) != sizeof(MCD_T))
			return WORDDOC_ERR_READ_MCD;

        if (((MCD_T FAR *)(lpbyWorkBuffer))->dwN != 0)
		{
            // Found an active macro

            return 1;
        }

		dwOffset += sizeof(MCD_T);
	}

    // Document does not contain any active macros

    return 0;
}


//********************************************************************
//
// Function:
//  WORD WordDocGetMacroInfoAtIndex()
//
// Description:
//  Fills in the MACRO_INFO_T structure for the macro at the given
//  index.
//
// Returns:
//	WORDDOC_OK			on success
//	WORDDOC_ERR_???		on error
//
//********************************************************************

WORD WordDocGetMacroInfoAtIndex
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    WORD                wIndex,             // Index to get
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPMACRO_TABLE_INFO  pMacroTableInfo,    // Macro table info
    LPMACRO_INFO_T      pMacroInfo,         // Macro info
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 512 bytes
    LPSTR               lpszMacroName       // Buffer to store name of macro
)
{
    WORD i;
    WORD wID;
    BYTE byLen;
    DWORD dwOffset;

    if (wIndex >= pMacroTableInfo->wNumMacros)
        return(WORDDOC_ERR_NO_SUCH_NAME);

    // First find macro name in macro name table

    dwOffset = pMacroTableInfo->dwMacroNameTableOffset;
    for (i=0;i<pMacroTableInfo->wNumMacros;i++)
    {
        if (OLESeekRead(lpCallBack,
            pOLEFile,dwOffset,(LPBYTE)&wID,2) != 2)
            return WORDDOC_ERR_READ_ID;

        dwOffset += 2;

        if (OLESeekRead(lpCallBack,
            pOLEFile,dwOffset,(LPBYTE)&byLen,1) != 1)
            return WORDDOC_ERR_READ_LEN;

        dwOffset += 1;

        if (OLESeekRead(lpCallBack,
                        pOLEFile,
                        dwOffset,
                        (LPBYTE)lpszMacroName,
                        byLen+1) != byLen + 1)
            return WORDDOC_ERR_READ_NAME;

        lpszMacroName[byLen] = '\0';

        if (i == wIndex)
            break;

        dwOffset += byLen + 1;
    }

    // Note offset of name

    pMacroInfo->dwNameOffset = dwOffset;
    pMacroInfo->byNameLen = byLen;

    // Then look for MCD structure in MCD table

    dwOffset = pMacroTableInfo->dwMCDTableOffset;
    for (i=0;i<pMacroTableInfo->wNumMCDs;i++)
    {
        if (OLESeekRead(lpCallBack,
            pOLEFile,dwOffset,lpbyWorkBuffer,
            sizeof(MCD_T)) != sizeof(MCD_T))
            return WORDDOC_ERR_READ_MCD;

        if (((MCD_T FAR *)(lpbyWorkBuffer))->wID == wID)
        {
            // Found a match

            // Verify dwN field
            // Questionable whether we should make this check

            if (((MCD_T FAR *)(lpbyWorkBuffer))->dwN == 0)
            {
                // This macro is invalid

                return WORDDOC_ERR_NO_MCD;
            }

            pMacroInfo->dwMCDOffset = dwOffset;
            pMacroInfo->dwOffset =
                ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwOffset);
            pMacroInfo->dwSize =
                ConvertEndianLong(((MCD_T FAR *)(lpbyWorkBuffer))->dwSize);
            pMacroInfo->byEncrypt =
                ((MCD_T FAR *)(lpbyWorkBuffer))->byEncrypt;
            pMacroInfo->wULNameIdx =
                ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wULNameIdx);
            pMacroInfo->wMenuHelp =
                ConvertEndianShort(((MCD_T FAR *)(lpbyWorkBuffer))->wMenuHelp);

            return WORDDOC_OK;
        }

        dwOffset += sizeof(MCD_T);
    }

    return WORDDOC_ERR_NO_MCD;
}

// Plain DOS above

#endif

//********************************************************************
//
// Function:
//  WORD WordDocOpen()
//
// Description:
//  Opens the stream named WordDocument.
//
// Returns:
//  1   on success
//  0   on error
//
//********************************************************************

BYTE gpuszWordDocument[] =
{
    'W', 0, 'o', 0, 'r', 0, 'd', 0,
    'D', 0, 'o', 0, 'c', 0, 'u', 0, 'm', 0, 'e', 0, 'n', 0, 't', 0, 0, 0
};


WORD WordDocOpen
(
    LPCALLBACKREV1  lpCallBack,             // File op callbacks
    HFILE           hFile,                  // Handle to OLE file
    LPOLE_FILE_T    pOLEFile,               // OLE file info
    LPBYTE          lpbyStreamBATCache,     // Non NULL if cache, 512 bytes
    LPWORD          pwMagicNum              // Magic number
)
{
    if (OLEOpenStream(lpCallBack,
        hFile,pOLEFile,(LPWORD)gpuszWordDocument,
        lpbyStreamBATCache,pwMagicNum) != OLE_OK)
        return 0;

    return 1;
}

#ifndef SYM_NLM

//********************************************************************
//
// Function:
//  WORD WordDocHasNonMacroTemplateInfo()
//
// Description:
//  Determines whether the document contains non-macro template
//  information.
//
// Returns:
//  0       If the document does not have non-macro template
//              information
//  1       If the document does have non-macro template
//              information
//
//********************************************************************

WORD WordDocHasNonMacroTemplateInfo
(
    LPMACRO_TABLE_INFO  pMacroTableInfo     // Macro table info
)
{
    if ((pMacroTableInfo->dwMacroTableContents &
            (MACRO_TABLE_TYPE_0X02 |
             MACRO_TABLE_TYPE_0X03 |
             MACRO_TABLE_TYPE_0X04 |
             MACRO_TABLE_TYPE_0X05 |
             MACRO_TABLE_TYPE_0X06 |
             MACRO_TABLE_TYPE_0X12)) != 0)
    {
        // non-macro template information present

        return 1;
    }

    // non-macro template information not present

    return 0;
}

//********************************************************************
//
// Function:
//  WORD WordDocHasAutoText()
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
//  0       On error
//  1       On success
//
//********************************************************************

WORD WordDocHasAutoText
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    LPOLE_FILE_T        lpstOLEFile,        // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPBOOL              lpbHasAutoText      // Ptr to BOOL
)
{
    DWORD               dwValue;
    WORD                w2ndHdrBlk;

    // Read DWORD at offset 0xA4

    if (WordDocEncryptedRead(lpstCallBack,
                             lpstOLEFile,
                             lpstKey,
                             0xA4,
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

    // Get the block number of the second header

    if (WordDocEncryptedRead(lpstCallBack,
                             lpstOLEFile,
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

    if (WordDocEncryptedRead(lpstCallBack,
                             lpstOLEFile,
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

#endif // #ifndef SYM_NLM

