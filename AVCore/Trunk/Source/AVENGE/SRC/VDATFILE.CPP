// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/vdatfile.cpv   1.2   09 Jun 1998 11:22:14   ksackin  $
//
// Description:
//
//  This source file contains support routines for acessing AVENGE's
//  indexed data files.
//
// Contains:
//  EngOpenDataFile()
//  EngLookUp()
//  EngCloseDataFile()
//
// See Also:
//  algload.c, booload.c, crcload.c, memload.c, reparaux.c, infoaux.c
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/vdatfile.cpv  $
// 
//    Rev 1.2   09 Jun 1998 11:22:14   ksackin
// Made a fix to EngOpenDataFile to zero out the lpvFileInfo pointer when a
// open failure occured.
//
//    Rev 1.1   08 Jul 1997 17:49:16   MKEATIN
// Changed DFFunctions() to EngFunctions().
//
//    Rev 1.0   18 Apr 1997 13:37:42   MKEATIN
// Initial revision.
//
//    Rev 1.1   16 May 1996 14:12:52   CNACHEN
// Changed to use new AVENGE?.H headers.
//
//    Rev 1.0   13 May 1996 16:29:10   DCHI
// Initial revision.
//
//************************************************************************

#include "avengel.h"



//********************************************************************
//
// Function:
//  EngOpenDataFile()
//
// Parameters:
//  lpDataFileCallBacks     Callbacks to access data files
//  wDataFileNum            ID number of the data file to access
//  wOpenMode               Mode (read, write, etc) to open the datafile with
//  lplpvFileInfo           Points to a pointer to file info.
//                          *lplpvFileInfo will be set to point to the
//                          appropriate file info on successful open.
//
// Description:
//  This function uses the data file callbacks to open the specified data
//  file (wDataFileNum) in the specified mode (wOpenMode).  It will set
//  *lplpvFileInfo to point to file info provided by the callback
//  GetDataFileInfo.  Currently, this function does no verification of the
//  data file's contents.
//
// Returns:
//  DFSTATUS_OK             On success
//  DFSTATUS_ERROR          On error
//
//********************************************************************

DFSTATUS EngOpenDataFile
(
    LPDATAFILECALLBACKS lpDataFileCallBacks,
    WORD                wDataFileNum,
    WORD                wOpenMode,
    LPLPVOID            lplpvFileInfo
)
{
    if (lpDataFileCallBacks->GetDataFileInfo(lplpvFileInfo, wDataFileNum) ==
        CBSTATUS_ERROR)
    {
        return(DFSTATUS_ERROR);
    }

    if (lpDataFileCallBacks->lpFileCallBacks->FileOpen(*lplpvFileInfo,
                                                       wOpenMode) ==
        CBSTATUS_ERROR)
    {
        // remember to undo our GetDataFileInfo if we have a problem opening
        // the file...

        lpDataFileCallBacks->CloseDataFileInfo(*lplpvFileInfo);

        // Set our lpvFileInfo to NULL so it will not get released again.
        *lplpvFileInfo = NULL;

        return(DFSTATUS_ERROR);
    }

    return(DFSTATUS_OK);
}

//********************************************************************
//
// Function:
//  EngLookUp()
//
// Parameters:
//  lpDataFileCallBacks     Callbacks to access data files
//  lpvFileInfo             Points to file info for the data file
//  dwIdent                 Section ID to search for
//  lpDataFileSection       Filled in with information about the specified
//                          data file section.
//  lpbSectionFound         *lpbSectionFound is set to TRUE if the specified
//                          section was found.
//
// Description:
//  This function attempts to locate the specified section in the specified
//  data file.  If it does locate the section, it fills in the structure
//  pointed to by lpDataFileSection with the appropriate information (start
//  offset of the section, date and time stamp, version number, etc.)  This
//  information is inserted in an endian-neutral fashion.  If the section is
//  found, *lpbSectionFound is set to TRUE, else it is set to FALSE.
//
// Returns:
//  DFSTATUS_OK             On success
//  DFSTATUS_ERROR          On error
//
//********************************************************************

DFSTATUS EngLookUp
(
    LPDATAFILECALLBACKS lpDataFileCallBacks,
    LPVOID              lpvFileInfo,
    DWORD               dwIdent,
    LPDATA_FILE_TABLE   lpDataFileSection,
    LPBOOL              lpbSectionFound
)
{
    DATA_FILE_HEADER_T  stDataFileHeader;
    DWORD               dwOff, dwBytesRead;
    DATA_FILE_TABLE_T   stTableEntry;
    WORD                wCurSection;

    // assume we're not going to find our section...

    *lpbSectionFound = FALSE;

    // seek to and read the header in so we can locate the section table...

	if (lpDataFileCallBacks->lpFileCallBacks->FileSeek(lpvFileInfo,
													  0,
													  SEEK_SET,
													  &dwOff) ==
		CBSTATUS_ERROR)
	{
		return(DFSTATUS_ERROR);
	}

	if (lpDataFileCallBacks->lpFileCallBacks->FileRead(lpvFileInfo,
													  &stDataFileHeader,
													  sDATAFILEHEADER,
													  &dwBytesRead) ==
		CBSTATUS_ERROR || dwBytesRead != sDATAFILEHEADER)
	{
		return(DFSTATUS_ERROR);
	}

	// endian enable that structure...

	stDataFileHeader.dwSignature =
		DWENDIAN(stDataFileHeader.dwSignature);
	stDataFileHeader.wNumSections =
		WENDIAN(stDataFileHeader.wNumSections);
	stDataFileHeader.dwTableOffset =
		DWENDIAN(stDataFileHeader.dwTableOffset);

	// check the file signature...

	if (stDataFileHeader.dwSignature != DATA_FILE_SIGNATURE)
		return(DFSTATUS_ERROR);

	// seek to the data file section table

	if (lpDataFileCallBacks->
			lpFileCallBacks->FileSeek(lpvFileInfo,
									 stDataFileHeader.dwTableOffset,
									 SEEK_SET,
									 &dwOff) ==
		CBSTATUS_ERROR)
	{
		return(DFSTATUS_ERROR);
	}

	// search in the table for the proper ID so we can find where the
	// section is in our data file

	for (wCurSection=0;
		 wCurSection<stDataFileHeader.wNumSections;
		 wCurSection++)
	{
		if (lpDataFileCallBacks->
				lpFileCallBacks->FileRead(lpvFileInfo,
										 &stTableEntry,
										 sDATA_FILE_TABLE,
										 &dwBytesRead) == CBSTATUS_ERROR)
		{
			return(DFSTATUS_ERROR);
		}

		if (sDATA_FILE_TABLE != dwBytesRead)
			return(DFSTATUS_ERROR);

		/* found the specified table entry.  report to the user */

		if (DWENDIAN(stTableEntry.dwIdent) == dwIdent)
		{
			lpDataFileSection->dwVersionNumber =
				DWENDIAN(stTableEntry.dwVersionNumber);
			lpDataFileSection->dwStartOffset =
				DWENDIAN(stTableEntry.dwStartOffset);
			lpDataFileSection->dwLength =
				DWENDIAN(stTableEntry.dwLength);
			lpDataFileSection->byMon = stTableEntry.byMon;
			lpDataFileSection->byDay = stTableEntry.byDay;
			lpDataFileSection->byYear = stTableEntry.byYear;

			*lpbSectionFound = TRUE;

			return(DFSTATUS_OK);
		}
	}

	// None of the sections matched with dwIdent.
	return(DFSTATUS_ERROR);
}

//********************************************************************
//
// Function:
//  EngCloseDataFile()
//
// Parameters:
//  lpDataFileCallBacks     Callbacks to access data files
//  lpvFileInfo             Points to file info for the data file
//
// Description:
//  This function relinquishes control over the specified data file.
//
// Returns:
//  DFSTATUS_OK             On success
//  DFSTATUS_ERROR          On error
//
//********************************************************************


DFSTATUS EngCloseDataFile
(
    LPDATAFILECALLBACKS    lpDataFileCallBacks,
    LPVOID                 lpvFileInfo
)
{
	// close the file first

	if (lpDataFileCallBacks->lpFileCallBacks->FileClose(lpvFileInfo) ==
		CBSTATUS_ERROR)
	{
		return(DFSTATUS_ERROR);
	}

	if (lpDataFileCallBacks->CloseDataFileInfo(lpvFileInfo) == CBSTATUS_ERROR)
	{
		return(DFSTATUS_ERROR);
	}

	return(DFSTATUS_OK);
}


