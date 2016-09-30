#include "platform.h"
#include "callback.h"
#include "avdatfil.h"
#include "avendian.h"

//********************************************************************
//
// Function:
//  BOOL VDFLookUp()
//
// Parameters:
//  lpstCallBack        Callbacks to access files
//  hFile               Handle to open data file
//  dwIdent             Identifier of section to look up
//  lpdwOffset          Ptr to DWORD for offset of section
//  lpdwLength          Ptr to DWORD for length of section
//
// Description:
//  This function attempts to locate the specified section in the
//  specified data file.  If it does locate the section, it fills
//  in the DWORDs pointed to by lpdwOffset and lpdwLength the start
//  offset and length of the section, respectively.  If the section
//  is found the function returns TRUE.  Otherwise, the function
//  returns error if the section is not found, there was an error,
//  or the header signature does not match.
//
// Returns:
//  TRUE                On success
//  FALSE               On error
//
//********************************************************************

BOOL VDFLookUp
(
    LPCALLBACKREV1      lpstCallBack,
    HFILE               hFile,
    DWORD               dwIdent,
    LPDWORD             lpdwOffset,
    LPDWORD             lpdwLength
)
{
    DATA_FILE_HEADER_T        stHeader;
    DATA_FILE_TABLE_T         stTableEntry;
    WORD                      wCurSection;

    // seek to and read the header in so we can locate the section table...

    if (lpstCallBack->FileSeek(hFile,0,SEEK_SET) != 0)
        return(FALSE);

    if (lpstCallBack->FileRead(hFile,&stHeader,sizeof(stHeader)) !=
        sizeof(stHeader))
        return(FALSE);

    // endian enable that structure...

    stHeader.dwSignature = DWENDIAN(stHeader.dwSignature);
    stHeader.wNumSections = WENDIAN(stHeader.wNumSections);
    stHeader.dwTableOffset = DWENDIAN(stHeader.dwTableOffset);

    // check the file signature...

    if (stHeader.dwSignature != DATA_FILE_SIGNATURE)
        return(FALSE);

    // seek to the data file section table

    if (lpstCallBack->FileSeek(hFile,
                               stHeader.dwTableOffset,
                               SEEK_SET) != stHeader.dwTableOffset)
        return(FALSE);

    // search in the table for the proper ID so we can find where the
    // section is in our data file

    for (wCurSection=0;wCurSection<stHeader.wNumSections;wCurSection++)
    {
        if (lpstCallBack->FileRead(hFile,
                                   &stTableEntry,
                                   sizeof(stTableEntry)) != sizeof(stTableEntry))
            return(FALSE);

        // found the specified table entry.  report to the user

        if (DWENDIAN(stTableEntry.dwIdent) == dwIdent)
        {
            *lpdwOffset = DWENDIAN(stTableEntry.dwStartOffset);
            *lpdwLength = DWENDIAN(stTableEntry.dwLength);

            return(TRUE);
        }
    }

    return(FALSE);
}

