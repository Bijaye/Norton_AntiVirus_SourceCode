//************************************************************************
//
// $Header:   S:/NAVEX/VCS/macrodel.cpv   1.9   25 Mar 1997 18:19:26   DCHI  $
//
// Description:
//      Contains WordDocument macro delete function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrodel.cpv  $
// 
//    Rev 1.9   25 Mar 1997 18:19:26   DCHI
// Added else to correctly update offset when spacing out unicode names.
// 
//    Rev 1.8   14 Mar 1997 16:33:54   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.7   17 Jan 1997 11:01:36   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.6   26 Dec 1996 15:22:30   AOONWAL
// No change.
// 
//    Rev 1.5   22 Nov 1996 13:06:52   DCHI
// Corrected bug whereby presence of both menu name and name caused incorrect
// spacing of menu name and name.
// 
//    Rev 1.4   13 Nov 1996 13:03:44   DCHI
// Added modifications for correctly deleting Unicode UL names.
// 
//    Rev 1.3   29 Oct 1996 12:59:00   AOONWAL
// No change.
// 
//    Rev 1.2   06 May 1996 17:19:14   DCHI
// Added WordDocChangeToDocument() function.
// 
// 
//    Rev 1.1   30 Jan 1996 15:43:26   DCHI
// Added #ifndef SYM_NLM and #endif pairs to ifdef out functions unused
// on NLM platform.
// 
//    Rev 1.0   03 Jan 1996 17:14:54   DCHI
// Initial revision.
// 
//************************************************************************

#ifndef SYM_NLM

#if defined(SYM_DOSX) || !defined(SYM_DOS)

// Decryption enhanced functions below

#include "storobj.h"
#include "wdencdoc.h"
#include "macrodel.h"

#include "wdencrd.h"
#include "wdencwr.h"

//********************************************************************
//
// Function:
//	WORD WordDocDeleteMacro()
//
// Description:
//	Deletes the macro from the stream by performing the following:
//		1. Invalidate the macro by setting the dwN field to 0.
//		2. Spaces out the name in the name table.
//		3. Spaces out the name in the UL name table.
//		4. Spaces out the description in the UL name table.
//		5. Zeroes out the macro text
//
// Returns:
//	WORDDOC_OK			on success
//	WORDDOC_ERR_???		on error
//
//********************************************************************

WORD WordDocDeleteMacro
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
	LPMACRO_TABLE_INFO  pMacroTableInfo,	// Macro table info
	LPMACRO_INFO_T		pMacroInfo,			// Macro info
	LPBYTE 				lpbyWorkBuffer		// Work buffer >= 512 bytes
)
{
    BOOL    bUnicoded;
	DWORD 	dwOffset;
    WORD    wBytesRead, wIdx;
    BYTE    byLen;
	WORD 	i;

	// Get the MCD

    if (WordDocEncryptedRead(lpCallBack,
                             pOLEFile,
                             lpstKey,
                             pMacroInfo->dwMCDOffset,
                             lpbyWorkBuffer,
                             sizeof(MCD_T)) != sizeof(MCD_T))
		return WORDDOC_ERR_READ_MCD;

	// Invalidate the macro

	((MCD_T FAR *)(lpbyWorkBuffer))->dwN = (DWORD)(0);

    if (WordDocEncryptedWrite(lpCallBack,
                              pOLEFile,
                              lpstKey,
                              pMacroInfo->dwMCDOffset,
                              lpbyWorkBuffer,
                              sizeof(MCD_T)) != sizeof(MCD_T))
		return WORDDOC_ERR_WRITE_MCD;

	// Space out the name in the name table

	for (i=0;i<(WORD)(pMacroInfo->byNameLen);i++)
		lpbyWorkBuffer[i] = ' ';

    if (WordDocEncryptedWrite(lpCallBack,
                              pOLEFile,
                              lpstKey,
                              pMacroInfo->dwNameOffset,
                              lpbyWorkBuffer,
                              (WORD)(pMacroInfo->byNameLen)) !=
        (WORD)(pMacroInfo->byNameLen))
		return WORDDOC_ERR_WRITE;

	// Space out the name in the UL name table

	dwOffset = pMacroTableInfo->dwULNameTableOffset;

    // First see if this is a Unicode table

    if (WordDocEncryptedRead(lpCallBack,
                             pOLEFile,
                             lpstKey,
                             dwOffset-3,
                             lpbyWorkBuffer,
                             sizeof(BYTE)) != sizeof(BYTE))
        return WORDDOC_ERR_READ_MCD;

    if (lpbyWorkBuffer[0] == 0x10)
        bUnicoded = FALSE;
    else
        bUnicoded = TRUE;

	wBytesRead = 0;
	wIdx = 0;

	while (wBytesRead < pMacroTableInfo->wULNameTableSize)
	{
        // For Unicode, assume that the number of Unicode characters
        //  is no more than 255

        if (WordDocEncryptedRead(lpCallBack,
                                 pOLEFile,
                                 lpstKey,
                                 dwOffset++,
                                 &byLen,
                                 sizeof(BYTE)) != sizeof(BYTE))
            return WORDDOC_ERR_READ_MCD;

        if (bUnicoded == TRUE)
        {
            ++dwOffset;

            if (wIdx == pMacroInfo->wULNameIdx ||
                wIdx == pMacroInfo->wMenuHelp)
            {
                lpbyWorkBuffer[0] = ' ';
                lpbyWorkBuffer[1] = 0;
                for (i=0;i<byLen;i++,dwOffset+=2)
                    if (WordDocEncryptedWrite(lpCallBack,
                                              pOLEFile,
                                              lpstKey,
                                              dwOffset,
                                              lpbyWorkBuffer,
                                              sizeof(WORD)) != sizeof(WORD))
                        return WORDDOC_ERR_WRITE;
            }
            else
                dwOffset += 2 * byLen;

            wBytesRead += 2 * byLen + 4;
        }
        else
        {
            if (wIdx == pMacroInfo->wULNameIdx ||
                wIdx == pMacroInfo->wMenuHelp)
            {
                for (i=0;i<(WORD)(byLen);i++)
                    lpbyWorkBuffer[i] = ' ';

                if (WordDocEncryptedWrite(lpCallBack,
                                          pOLEFile,
                                          lpstKey,
                                          dwOffset,
                                          lpbyWorkBuffer,
                                          (WORD)(byLen)) != (WORD)(byLen))
                    return WORDDOC_ERR_WRITE;
            }

            dwOffset += byLen;
            wBytesRead += byLen + 3;
        }

        // Skip past the 0x01 0x00 after each UL name

        dwOffset += 2;

		++wIdx;
		if ((wIdx > pMacroInfo->wULNameIdx ||
			0xFFFF == pMacroInfo->wULNameIdx) &&
			(wIdx > pMacroInfo->wMenuHelp ||
			0xFFFF == pMacroInfo->wMenuHelp))
			break;
	}

	// Zero out the macro text in 512 byte chunks

	for (i=0;i<512;i++)
		lpbyWorkBuffer[i] = 0;

	dwOffset = pMacroInfo->dwOffset;

	while ((dwOffset + 512) < (pMacroInfo->dwOffset + pMacroInfo->dwSize))
	{
        if (WordDocEncryptedWrite(lpCallBack,
                                  pOLEFile,
                                  lpstKey,
                                  dwOffset,
                                  lpbyWorkBuffer,
                                  512) != 512)
			return WORDDOC_ERR_WRITE;

		dwOffset += 512;
	}

	// i = bytes left to write

	i = (WORD)((pMacroInfo->dwOffset + pMacroInfo->dwSize) - dwOffset);

    if (WordDocEncryptedWrite(lpCallBack,
                              pOLEFile,
                              lpstKey,
                              dwOffset,
                              lpbyWorkBuffer,
                              i) != i)
		return WORDDOC_ERR_WRITE;

	return WORDDOC_OK;
}


//********************************************************************
//
// Function:
//  WORD WordDocChangeToDocument()
//
// Description:
//  Changes a Word document to a document
//
// Returns:
//  WORDDOC_ERR_???     On error
//  WORDDOC_OK          On success
//
//********************************************************************

WORD WordDocChangeToDocument
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPWDENCKEY          lpstKey,            // Decryption key
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    // Read Word Document header

    if (WordDocEncryptedRead(lpCallBack,
                             pOLEFile,
                             lpstKey,
                             0,
                             lpbyWorkBuffer,
                             288) != 288)
        return WORDDOC_ERR_READ;

    // Make it a document

    lpbyWorkBuffer[10] &= 0xFE;

    // Write Word Document header

    if (WordDocEncryptedWrite(lpCallBack,
                              pOLEFile,
                              lpstKey,
                              0,
                              lpbyWorkBuffer,
                              288) != 288)
        return WORDDOC_ERR_WRITE;


    return WORDDOC_OK;
}

// Decryption enhanced functions above

#else

// Plain DOS below

#include "macrodel.h"
#include "olewrite.h"

//********************************************************************
//
// Function:
//	WORD WordDocDeleteMacro()
//
// Description:
//	Deletes the macro from the stream by performing the following:
//		1. Invalidate the macro by setting the dwN field to 0.
//		2. Spaces out the name in the name table.
//		3. Spaces out the name in the UL name table.
//		4. Spaces out the description in the UL name table.
//		5. Zeroes out the macro text
//
// Returns:
//	WORDDOC_OK			on success
//	WORDDOC_ERR_???		on error
//
//********************************************************************

WORD WordDocDeleteMacro
(
	LPCALLBACKREV1 		lpCallBack,			// File op callbacks
	LPOLE_FILE_T		pOLEFile,			// OLE file info
	LPMACRO_TABLE_INFO  pMacroTableInfo,	// Macro table info
	LPMACRO_INFO_T		pMacroInfo,			// Macro info
	LPBYTE 				lpbyWorkBuffer		// Work buffer >= 512 bytes
)
{
    BOOL    bUnicoded;
	DWORD 	dwOffset;
    WORD    wBytesRead, wIdx;
    BYTE    byLen;
	WORD 	i;

	// Get the MCD

    if (OLESeekRead(lpCallBack,
		pOLEFile,pMacroInfo->dwMCDOffset,lpbyWorkBuffer,
		sizeof(MCD_T)) != sizeof(MCD_T))
		return WORDDOC_ERR_READ_MCD;

	// Invalidate the macro

	((MCD_T FAR *)(lpbyWorkBuffer))->dwN = (DWORD)(0);

    if (OLESeekWrite(lpCallBack,
		pOLEFile,pMacroInfo->dwMCDOffset,lpbyWorkBuffer,
		sizeof(MCD_T)) != sizeof(MCD_T))
		return WORDDOC_ERR_WRITE_MCD;

	// Space out the name in the name table

	for (i=0;i<(WORD)(pMacroInfo->byNameLen);i++)
		lpbyWorkBuffer[i] = ' ';

    if (OLESeekWrite(lpCallBack,
		pOLEFile,pMacroInfo->dwNameOffset,lpbyWorkBuffer,
		(WORD)(pMacroInfo->byNameLen)) != (WORD)(pMacroInfo->byNameLen))
		return WORDDOC_ERR_WRITE;

	// Space out the name in the UL name table

	dwOffset = pMacroTableInfo->dwULNameTableOffset;

    // First see if this is a Unicode table

    if (OLESeekRead(lpCallBack,
                    pOLEFile,
                    dwOffset-3,
                    lpbyWorkBuffer,
                    sizeof(BYTE)) != sizeof(BYTE))
        return WORDDOC_ERR_READ_MCD;

    if (lpbyWorkBuffer[0] == 0x10)
        bUnicoded = FALSE;
    else
        bUnicoded = TRUE;

	wBytesRead = 0;
	wIdx = 0;

	while (wBytesRead < pMacroTableInfo->wULNameTableSize)
	{
        // For Unicode, assume that the number of Unicode characters
        //  is no more than 255

        if (OLESeekRead(lpCallBack,
                        pOLEFile,
                        dwOffset++,
                        &byLen,
                        sizeof(BYTE)) != sizeof(BYTE))
            return WORDDOC_ERR_READ_MCD;

        if (bUnicoded == TRUE)
        {
            ++dwOffset;

            if (wIdx == pMacroInfo->wULNameIdx ||
                wIdx == pMacroInfo->wMenuHelp)
            {
                lpbyWorkBuffer[0] = ' ';
                lpbyWorkBuffer[1] = 0;
                for (i=0;i<byLen;i++,dwOffset+=2)
                    if (OLESeekWrite(lpCallBack,
                                     pOLEFile,
                                     dwOffset,
                                     lpbyWorkBuffer,
                                     sizeof(WORD)) != sizeof(WORD))
                        return WORDDOC_ERR_WRITE;
            }

            dwOffset += 2 * byLen;
            wBytesRead += 2 * byLen + 4;
        }
        else
        {
            if (wIdx == pMacroInfo->wULNameIdx ||
                wIdx == pMacroInfo->wMenuHelp)
            {
                for (i=0;i<(WORD)(byLen);i++)
                    lpbyWorkBuffer[i] = ' ';

                if (OLESeekWrite(lpCallBack,
                                 pOLEFile,
                                 dwOffset,
                                 lpbyWorkBuffer,
                                 (WORD)(byLen)) != (WORD)(byLen))
                    return WORDDOC_ERR_WRITE;
            }

            dwOffset += byLen;
            wBytesRead += byLen + 3;
        }

        // Skip past the 0x01 0x00 after each UL name

        dwOffset += 2;

		++wIdx;
		if ((wIdx > pMacroInfo->wULNameIdx ||
			0xFFFF == pMacroInfo->wULNameIdx) &&
			(wIdx > pMacroInfo->wMenuHelp ||
			0xFFFF == pMacroInfo->wMenuHelp))
			break;
	}

	// Zero out the macro text in 512 byte chunks

	for (i=0;i<512;i++)
		lpbyWorkBuffer[i] = 0;

	dwOffset = pMacroInfo->dwOffset;

	while ((dwOffset + 512) < (pMacroInfo->dwOffset + pMacroInfo->dwSize))
	{
	    if (OLESeekWrite(lpCallBack,
			pOLEFile,dwOffset,lpbyWorkBuffer,
			512) != 512)
			return WORDDOC_ERR_WRITE;

		dwOffset += 512;
	}

	// i = bytes left to write

	i = (WORD)((pMacroInfo->dwOffset + pMacroInfo->dwSize) - dwOffset);

    if (OLESeekWrite(lpCallBack,
		pOLEFile,dwOffset,lpbyWorkBuffer,i) != i)
		return WORDDOC_ERR_WRITE;

	return WORDDOC_OK;
}


//********************************************************************
//
// Function:
//  WORD WordDocChangeToDocument()
//
// Description:
//  Changes a Word document to a document
//
// Returns:
//  WORDDOC_ERR_???     On error
//  WORDDOC_OK          On success
//
//********************************************************************

WORD WordDocChangeToDocument
(
    LPCALLBACKREV1      lpCallBack,         // File op callbacks
    LPOLE_FILE_T        pOLEFile,           // OLE file info
    LPBYTE              lpbyWorkBuffer      // Work buffer >= 512 bytes
)
{
    // Read Word Document header

    if (OLESeekRead(lpCallBack,pOLEFile,0,lpbyWorkBuffer,288) != 288)
        return WORDDOC_ERR_READ;

    // Make it a document

    lpbyWorkBuffer[10] &= 0xFE;

    // Write Word Document header

    if (OLESeekWrite(lpCallBack,pOLEFile,0,lpbyWorkBuffer,288) != 288)
        return WORDDOC_ERR_WRITE;


    return WORDDOC_OK;
}

// Plain DOS above

#endif

#endif  // #ifndef SYM_NLM

