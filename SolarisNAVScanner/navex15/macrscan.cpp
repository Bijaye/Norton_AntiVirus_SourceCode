//************************************************************************
//
// $Header:   S:/NAVEX/VCS/macrscan.cpv   1.12   07 Apr 1997 18:46:08   DCHI  $
//
// Description:
//      Contains WordDocument macro scan function code.
//
//************************************************************************
// $Log:   S:/NAVEX/VCS/macrscan.cpv  $
// 
//    Rev 1.12   07 Apr 1997 18:46:08   DCHI
// Added code to speed up excel scanning by being more selective.
// 
//    Rev 1.11   14 Mar 1997 16:34:06   DCHI
// Added support for Office 97 repair.
// 
//    Rev 1.10   13 Feb 1997 13:30:46   DCHI
// Modifications to support VBA 5 scanning.
// 
//    Rev 1.9   23 Jan 1997 11:12:18   DCHI
// Fixed global variable usage problem.
// 
//    Rev 1.8   17 Jan 1997 11:01:48   DCHI
// Modifications supporting new macro engine.
// 
//    Rev 1.7   26 Dec 1996 15:23:02   AOONWAL
// No change.
// 
//    Rev 1.6   02 Dec 1996 14:01:14   AOONWAL
// No change.
// 
//    Rev 1.5   06 Nov 1996 17:17:42   DCHI
// Added functionality for wild macro scanning.
// 
//    Rev 1.4   29 Oct 1996 12:58:58   AOONWAL
// No change.
// 
//    Rev 1.3   28 Aug 1996 16:18:04   DCHI
// Added BAT cache support.
// 
//    Rev 1.2   21 May 1996 16:32:24   CNACHEN
// Happy birthday Darren! :)
// 
// 
//    Rev 1.1   03 May 1996 13:26:38   DCHI
// Endian-enabled the code for Mac compatibility.
// 
//    Rev 1.0   03 Jan 1996 17:14:54   DCHI
// Initial revision.
// 
//************************************************************************

#if defined(SYM_DOSX) || !defined(SYM_DOS)

#include "storobj.h"
#include "wdencdoc.h"
#include "worddoc.h"

#include "macrscan.h"

#include "endutils.h"

#include "wdscan.h"
#include "vba5scan.h"
#include "offcscan.h"

//********************************************************************
//
// Function:
//	EXTSTATUS EXTScanFileMacro()
//
// Description:
//	Scans a file for the presence of Word Macro viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileMacro
(
    LPCALLBACKREV1      lpstCallBack,       // File op callbacks
    HFILE               hFile,              // Handle to file to scan
    LPBYTE              lpbyWorkBuffer,     // Work buffer >= 2048 bytes
    LPWORD              lpwVID              // Virus ID storage on hit
)
{
    MSOFFICE_SCAN_T     stMSOfficeScan;
    LPOLE_FILE          lpstOLEFile;
    LPWD_VIRUS_SIG_INFO lpstVirusSigInfo;

    lpstOLEFile = (LPOLE_FILE)lpbyWorkBuffer;
    lpbyWorkBuffer += sizeof(OLE_FILE_T);

    lpstOLEFile->hFile = hFile;

    if (MicrosoftOfficeScan(lpstCallBack,
                            lpstOLEFile,
                            lpbyWorkBuffer,
                            &stMSOfficeScan,
                            &lpstVirusSigInfo) == EXTSTATUS_VIRUS_FOUND)
    {
        // Virus found

        if (stMSOfficeScan.wType == OLE_SCAN_XL5)
            *lpwVID = stMSOfficeScan.u.stXL5.wVID;
        else
            *lpwVID = lpstVirusSigInfo->wID;

        return(EXTSTATUS_VIRUS_FOUND);
    }

    // No virus

    return (EXTSTATUS_OK);
}

#else

// Plain DOS below

#include "storobj.h"
#include "worddoc.h"

#include "macrscan.h"
#include "mcrscndf.h"

#include "endutils.h"

//********************************************************************
//
// Function:
//	EXTSTATUS EXTScanFileMacro()
//
// Description:
//	Scans a file for the presence of Word Macro viruses.
//
// Returns:
//	EXTSTATUS_OK			If no virus was found
//	EXTSTATUS_VIRUS_FOUND   If a virus was found
//
//	If a virus was found, the function sets *lpwVID to the VID
//	of the virus.
//
//********************************************************************

EXTSTATUS EXTScanFileMacro
(
	LPCALLBACKREV1 	lpCallBack,				// File op callbacks
    HFILE          	hFile,					// Handle to file to scan
    LPBYTE          lpbyWorkBuffer,         // Work buffer >= 1024 bytes
    LPWORD         	lpwVID					// Virus ID storage on hit
)
{
    WORD    wReturn = EXTSTATUS_OK;
	OLE_FILE_T OLEFile;
	MACRO_TABLE_INFO_T macroTableInfo;
	MACRO_INFO_T macroInfo;
	LPWORD_MACRO_SIG_T pwmsSig;
	BYTE LPF *lpbySig;
    WORD i, j, wSigOffset, wSigLen, wMagicNum, wMacroIdx;
    LPBYTE  lpbyStreamBATCache;

    lpbyStreamBATCache = lpbyWorkBuffer;
    lpbyWorkBuffer += 512;

	// Verify that it is a Word file and that it has macros

	wMagicNum = 0;
    while (WordDocOpen(lpCallBack,
                       hFile,
                       &OLEFile,
                       lpbyStreamBATCache,
                       &wMagicNum) != 0)
	{
		if (WordDocContainsMacros(lpCallBack,&OLEFile,lpbyWorkBuffer,
			&macroTableInfo) == 0)
			continue;

		for (i=0;i<gwNumMacroScanDefs;i++)
		{
			pwmsSig = gpWordVirusSigs[i].pwmsSig;
			while (pwmsSig->pszMacroName != NULL)
			{
				if (WordDocGetMacroInfo(lpCallBack,
					&OLEFile,
					&macroTableInfo,
					&macroInfo,
					lpbyWorkBuffer,
					(LPSTR)pwmsSig->pszMacroName) != WORDDOC_OK)
					break;

				// Macro by that name exists
    	        // 	Now verify all signatures

				lpbySig = pwmsSig->pbySig;
                wSigOffset = AVDEREF_WORD((LPBYTE)lpbySig);
				while (wSigOffset != (WORD)(0xFFFF))
				{
					// Skip pass signature offset

					lpbySig += 2;

					wSigLen = (WORD)(*lpbySig++);

					//	Seek to it and read

				    if (OLESeekRead(lpCallBack,&OLEFile,
						macroInfo.dwOffset + (DWORD)wSigOffset,
						lpbyWorkBuffer,wSigLen) != wSigLen)
					{
						// Couldn't read that many bytes, so fail

    	                break;
					}

					// Compare

                	for (j=0;j<wSigLen;j++)
					{
						if (lpbyWorkBuffer[j] !=
							(macroInfo.byEncrypt ^ (*lpbySig++)))
							break;
					}

					if (j != wSigLen)
					{
						// No match

						break;
					}

					// Next signature offset in this macro

                    wSigOffset = AVDEREF_WORD((LPBYTE)lpbySig);
				}

				if (wSigOffset != (WORD)(0xFFFF))
				{
					// Doesn't match this macro definition

					break;
				}

				++pwmsSig;
			}

			if (pwmsSig->pszMacroName == NULL)
			{
				// Found a match

				*lpwVID = gpWordVirusSigs[i].wVID;
				wReturn = EXTSTATUS_VIRUS_FOUND;
				break;
			}
		}

        // Search for wildcarded names

        for (wMacroIdx=0;wMacroIdx<macroTableInfo.wNumMacros;wMacroIdx++)
		{
            char        szMacroName[256];

            if (WordDocGetMacroInfoAtIndex(lpCallBack,
                                           wMacroIdx,
                                           &OLEFile,
                                           &macroTableInfo,
                                           &macroInfo,
                                           lpbyWorkBuffer,
                                           szMacroName) != WORDDOC_OK)
            {
                // Error getting this macro, go to the next one

                continue;
            }

            for (i=0;i<gwNumMacroScanDefs;i++)
            {
                pwmsSig = gpWordVirusSigs[i].pwmsSig;
                if (pwmsSig->pszMacroName != NULL &&
                    pwmsSig->pszMacroName[0] == '*')
                {
                    // Macro by that name exists
                    //  Now verify all signatures

                    lpbySig = pwmsSig->pbySig;
                    wSigOffset = AVDEREF_WORD((LPBYTE)lpbySig);
                    while (wSigOffset != (WORD)(0xFFFF))
                    {
                        // Skip pass signature offset

                        lpbySig += 2;

                        wSigLen = (WORD)(*lpbySig++);

                        //  Seek to it and read

                        if (OLESeekRead(lpCallBack,&OLEFile,
                            macroInfo.dwOffset + (DWORD)wSigOffset,
                            lpbyWorkBuffer,wSigLen) != wSigLen)
                        {
                            // Couldn't read that many bytes, so fail

                            break;
                        }

                        // Compare

                        for (j=0;j<wSigLen;j++)
                        {
                            if (lpbyWorkBuffer[j] !=
                                (macroInfo.byEncrypt ^ (*lpbySig++)))
                                break;
                        }

                        if (j != wSigLen)
                        {
                            // No match

                            break;
                        }

                        // Next signature offset in this macro

                        wSigOffset = AVDEREF_WORD((LPBYTE)lpbySig);
                    }

                    if (wSigOffset == (WORD)(0xFFFF))
                    {
                        // Found a match

                        *lpwVID = gpWordVirusSigs[i].wVID;
                        return(EXTSTATUS_VIRUS_FOUND);
                    }
                }
			}
		}
	}

	// No virus

	return (wReturn);
}

// Plain DOS above

#endif


