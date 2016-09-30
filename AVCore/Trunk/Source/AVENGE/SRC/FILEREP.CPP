// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/FILEREP.CPv   1.5   14 Oct 1998 11:50:50   MKEATIN  $
//
// Description:
//  This source file contains all file repair routines.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/AVENGE/VCS/FILEREP.CPv  $
//
//    Rev 1.5   14 Oct 1998 11:50:50   MKEATIN
// Carey's ThreadSwitch additions for NLM.
//
//    Rev 1.4   08 Jul 1997 17:49:10   MKEATIN
// Changed DFFunctions() to EngFunctions().
//
//    Rev 1.3   08 Jul 1997 17:18:10   MKEATIN
// Passed NULL as the lpvProvidedFileInfo parameters in various functions.
// The old default NULL in the old prototype won't compile for the NLM
// C compiler.
//
//    Rev 1.2   15 May 1997 12:19:42   MKEATIN
// Made iterating though records faster by providing an open file info
// pointer (handle) to info routines.
//
//    Rev 1.1   13 May 1997 18:27:12   MKEATIN
// Ported Carey's boot caching code.
//
//    Rev 1.0   18 Apr 1997 13:37:28   MKEATIN
// Initial revision.
//
//    Rev 1.6   29 Aug 1996 17:27:32   CNACHEN
// Fixed the following bug:  During repair of viruses detected with CRC
// definitions, AVENGE would apply each repair (looking for proper bookmarks)
// and then if the bookmark didn't match, it would advance to the next matching
// CRC repair.  Unfortunately, if AVENGE was unable to repair, but did match
// the bookmark, it would also continue to apply subsequent repairs. CERTLIB
// would not attempt multiple repairs if one repair progressed further than the
// bookmark check.  This difference made AVENGE repair files that CERTLIB would
// not. AVENGE has been change to work like certlib.
//
//    Rev 1.5   13 Aug 1996 12:31:56   CNACHEN
// Took care of byte-alignment issues for UNIX.
//
//    Rev 1.4   29 May 1996 18:07:38   DCHI
// Changed lpvFileInfo to lpvInfo for file objects.
//
//    Rev 1.3   17 May 1996 15:37:56   CNACHEN
// Removed get/set date and time from repairaux.  this was moved outside.
//
//    Rev 1.2   17 May 1996 14:15:58   CNACHEN
// Changed READ_ONLY_FILE to FILE_OPEN_READ_ONLY and changed
//         READ_WRITE_FILE to FILE_OPEN_READ_WRITE
//
//    Rev 1.1   16 May 1996 14:12:44   CNACHEN
// Changed to use new AVENGE?.H headers.
//
//    Rev 1.0   13 May 1996 16:28:56   DCHI
// Initial revision.
//
//************************************************************************


#include "avengel.h"




//********************************************************************
//
// Function:
//  GetCRCRepairInfo()
//
// Parameters:
//  lpDataFileCallBacks     Callbacks to access data files
//  lpvFileInfo             File pointer to the VIRSCAN3.DAT file
//  lpVirusInfo             Pointer to a virus info record as found in
//                          VIRSCAN1.DAT
//  lpRepSec                Section information (starting offset, etc) on the
//                          section that contains CRC repair records
//  lpMemSec                Section information (starting offset, etc) on the
//                          section that contains CRC memory sigs for encrypted
//                          virus repair
//  lpwFirstRepairIndex     This points to a WORD which will be set to the
//                          index of the first CRC repair record in the CRC
//                          repair record section that has a matching CRC
//                          with the specified repair.
//  lpwTotalCRCRepairRecords
//                          This points to a WORD that will be set to the
//                          total number of CRC records in the CRC repair
//                          record section.  This is an upper bound on the
//                          number of CRC repairs we have to apply.
//
// Description:
//  This function locates the specified repair record in the CRC repair
//  section.  It does this by looking at the lpVirusInfo->wRepairIndex value.
//  This value is an index into the CRC repair section.  Once it locates the
//  specified record, it seaarches backwards until it locates the first repair
//  record with the same CRC value in the section.  This first record will be
//  the first repair applied to the target file.  Repairs will continue to
//  be applied until one works or all repair records with the same CRC value
//  are exhausted.  The CRC repair records *are* sorted by CRC value.  If a
//  given CRC definition does not have repair information associated with it
//  in VIRSCAN.DAT (yes, the old one), then it is set to point to the repair
//  information of another CRC definition with the same CRC, if such a
//  definition exists with a repair.  Otherwise, it has its bit set indicating
//  that there is no repair.  If no repairs are found, then
//  *lpwTotalCRCRepairRecords is set to 0 and the function returns.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS   GetCRCRepairInfo
(
    HGENG                   hGEng,
    LPDATAFILECALLBACKS     lpDataFileCallBacks,
    LPVOID                  lpvFileInfo,
    LPVIRUS_INFO            lpVirusInfo,
	LPDATA_FILE_TABLE       lpRepSec,
	LPDATA_FILE_TABLE		lpMemSec,
    LPWORD                  lpwFirstRepairIndex,
    LPWORD                  lpwTotalCRCRepairRecords
)
{
    WORD            wOrigCRC;
    DWORD           dwBytesRead, dwOff;
    long            lCurRecordNum;
    VIRUS_REPAIR_T  stRepair;
    BYTE            byDecryptData[DECRYPT_MEM_SIG_SIZE];

    if (lpVirusInfo->wFlags & AVENGE_TYPE_HAS_REPAIR)
    {
        // if we have a repair record then start looking there and move
        // backwards

        lCurRecordNum = (DWORD)lpVirusInfo->wRepairIndex;
    }
    else
    {
        *lpwFirstRepairIndex = 0;           // just to be safe
        *lpwTotalCRCRepairRecords = 0;      // no repairs to try

        return(ENGSTATUS_OK);
    }

    // determine how many CRC repair records there are total
    // seek to the start of the repair section

    if (lpDataFileCallBacks->
            lpFileCallBacks->FileSeek(lpvFileInfo,
                                     lpRepSec->dwStartOffset,
                                     SEEK_SET,
                                     &dwOff) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // read in the # of records in this section

	if (lpDataFileCallBacks->
        lpFileCallBacks->FileRead(lpvFileInfo,
                                  lpwTotalCRCRepairRecords,
                                  sizeof(WORD),
                                  &dwBytesRead) == CBSTATUS_ERROR ||
        dwBytesRead != sizeof(WORD))
    {
        return(ENGSTATUS_ERROR);
    }

    *lpwTotalCRCRepairRecords = WENDIAN(*lpwTotalCRCRepairRecords);

    // obtain the specified CRC repair record.

    if (GetRepairRecordAux(lpDataFileCallBacks,
                           lpvFileInfo,
			   lpRepSec,
			   lpMemSec,
                           (WORD)lCurRecordNum,
  			   &stRepair,
  			   byDecryptData) != ENGSTATUS_OK)
    {
        return(ENGSTATUS_ERROR);
    }

    // remember its CRC

    wOrigCRC = stRepair.wCRC;

    // search backwards for the first matching CRC in the set...

    while (--lCurRecordNum >= 0)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (GetRepairRecordAux(lpDataFileCallBacks,
                               lpvFileInfo,
                               lpRepSec,
                               lpMemSec,
                               (WORD)lCurRecordNum,
                               &stRepair,
                               byDecryptData) != ENGSTATUS_OK)
        {
            return(ENGSTATUS_ERROR);
        }

        // if we have a mismatch on the CRC, then we've gone to far up

        if (stRepair.wCRC != wOrigCRC)
        {
            // went back one too far, compensate and return our starting
            // CRC repair index.

            *lpwFirstRepairIndex = (WORD)(lCurRecordNum + 1);

            return(ENGSTATUS_OK);
        }
    }

    // went all the way to the top of the file!  We're the first CRC...

    *lpwFirstRepairIndex = 0;

    return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EngLocateSections()
//
// Parameters:
//  hLEng                   Local engine context
//  lpVirusInfo             Pointer to a virus info record as found in
//                          VIRSCAN1.DAT
//  lplpvFileInfo           Pointer to a file pointer.  The file pointer value
//                          is set up by this function to refer to the proper
//                          VIRSCAN?.DAT file that contains the repair data.
//  lpRepSec                Section information (starting offset, etc) on the
//                          section that contains CRC repair records.  This is
//                          filled in by the function.
//  lpMemSec                Section information (starting offset, etc) on the
//                          section that contains CRC memory sigs for encrypted
//                          virus repair.  This is filled in by the function.
//
// Description:
//  This function examines the virus information record (lpVirusInfo) and
//  determines what data file contains the repair information for this virus.
//  The repair information can either be located in VIRSCAN2.DAT or
//  VIRSCAN3.DAT.  If it's in VIRSCAN2.DAT, the wFlags field if lpVirusInfo
//  will have the AVENGE_TYPE_VIRSCAN2_REPAIR bit set.  All CRC repairs are
//  located in VIRSCAN3.DAT.  All definitions with PAM repairs (flag 2G in
//  the dwRepairFlags structure) are located in VIRSCAN2.DAT.  Some ALG/BOO
//  repairs will also be placed in the VIRSCAN2.DAT file to fill it up.
//  The remaining ALG/BOO signatures are placed in VIRSCAN3.DAT.  This function
//  sets the fields of the lpRepSec and lpMemSec structures to contain the
//  appropriate repair section and memory decrypt section information.  Both
//  records for a given repair are guaranteed to be in the same data file, to
//  eliminate disk swapping.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS   EngLocateSections
(
    HGENG                   hGEng,
    LPVIRUS_INFO            lpVirusInfo,
    LPLPVOID                lplpvFileInfo,
    LPDATA_FILE_TABLE       lpRepSec,
    LPDATA_FILE_TABLE       lpMemSec,
    LPVOID                  lpvProvidedFileInfo
)
{
    LPDATAFILECALLBACKS     lpDataFileCallBacks;
    WORD                    wDataFileNum;
    DWORD                   dwRepairSectionNum, dwMemSectionNum;
    LPVOID                  lpvFileInfo;
    BOOL                    bSectionFound;

    // set up our data file callbacks

    lpDataFileCallBacks = hGEng->lpDataFileCallBacks;


    if (lpvProvidedFileInfo == NULL)
    {
        // fetch our repair information and figure out what we're doing...

        if (lpVirusInfo->wFlags & AVENGE_TYPE_VIRSCAN2_REPAIR)
        {
            // repair is a PAM repair or is located in VIRSCAN2.DAT

            wDataFileNum = DATA_FILE_ID_VIRSCAN2DAT;
        }
        else
        {
            // repair is located in VIRSCAN3.DAT (most repairs)

            wDataFileNum = DATA_FILE_ID_VIRSCAN3DAT;
        }

        if (EngOpenDataFile(lpDataFileCallBacks,
                           wDataFileNum,
                           FILE_OPEN_READ_ONLY,
                           &lpvFileInfo) == DFSTATUS_ERROR)
        {
            return(ENGSTATUS_ERROR);
        }

        // make sure we pass our file information pointer back

        *lplpvFileInfo = lpvFileInfo;
    }
    else
    {
        lpvFileInfo = lpvProvidedFileInfo;
    }

	// determine which sections contain our repair data and memory data

    if (lpVirusInfo->wFlags & AVENGE_TYPE_CRC)
    {
        // CRC repair: all CRC records are sorted in the CRC section in
        // VIRSCAN3.DAT

        dwRepairSectionNum = SECTION_ID_CRC_REPAIR;
        dwMemSectionNum = SECTION_ID_CRC_MEMORY_REPAIR;
    }
    else if (lpVirusInfo->wFlags & AVENGE_TYPE_VIRSCAN2_REPAIR)
    {
        // PALG/PBOO repair: all PAM and some ALG/BOO repair records are sorted
        // in VIRSCAN2.DAT sections

        dwRepairSectionNum = SECTION_ID_PLG_REPAIR;
        dwMemSectionNum = SECTION_ID_PLG_MEM_REPAIR;
    }
    else
    {
        // ALG/BOO repair: remaining ALG/BOO repair records are sorted in
        // VIRSCAN3.DAT sections

        dwRepairSectionNum = SECTION_ID_ALG_BOO_REPAIR;
        dwMemSectionNum = SECTION_ID_ALG_BOO_MEM_REPAIR;
    }

    // locate the proper repair section... (stSectionInfo is endian neutral)

    if (EngLookUp(lpDataFileCallBacks,
                 lpvFileInfo,
                 dwRepairSectionNum,
                 lpRepSec,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // close and exit on error

        if (lpvProvidedFileInfo == NULL)
            EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

    // locate the proper memory signature section...

    if (EngLookUp(lpDataFileCallBacks,
                 lpvFileInfo,
                 dwMemSectionNum,
                 lpMemSec,
                 &bSectionFound) == DFSTATUS_ERROR ||
        bSectionFound == FALSE)
    {
        // close and exit on error

        if (lpvProvidedFileInfo == NULL)
            EngCloseDataFile(lpDataFileCallBacks, lpvFileInfo);

        return(ENGSTATUS_ERROR);
    }

    return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  DetermineXORBYTE()
//
// Parameters:
//  lpbyInfected            Pointer to bytes read from our target file
//  lpbyDecryptSig          Pointer to bytes from our signature to use in
//                          our plaintext attack.
//  lpbyBase                Points to the key value used in encryption (OUT)
//  lpbyDelta               Points to the delta value used in encryption (OUT)
//
// Description:
//
//  This function performs a plaintext attack on the data in the target file
//  in an attempt to determine if the XOR/DELTA encryption method was used.  If
//  it was, it returns TRUE and sets *lpbyBase and *lpbyDelta to the proper
//  key values.  Otherwise it returns FALSE.  This handles encryption like:
//
//      MOV AL, 12              12 is the BASE key
// TOP: XOR [SI], AL
//      ADD AL, 34              34 is the DELTA
//      INC SI
//      LOOP TOP
//
// Returns:
//  TRUE                    If the buffer uses this encryption scheme
//  FALSE                   Otherwise
//
//********************************************************************

BOOL DetermineXORBYTE
(
    LPBYTE  lpbyInfected,
    LPBYTE  lpbyDecryptSig,
    LPBYTE  lpbyBase,
    LPBYTE  lpbyDelta
)
{
    BYTE    byTemp;

	*lpbyBase = lpbyInfected[0] ^ (BYTE)(lpbyDecryptSig[0]-1);
    *lpbyDelta = ((lpbyInfected[1] ^ (BYTE)(lpbyDecryptSig[1]-1))) -
                 *lpbyBase;

    byTemp = *lpbyBase + 2 * *lpbyDelta;

    if ((lpbyInfected[2] ^ byTemp) != (BYTE)(lpbyDecryptSig[2]-1))
        return(FALSE);

    byTemp += *lpbyDelta;

    if ((lpbyInfected[3] ^ byTemp) != (BYTE)(lpbyDecryptSig[3]-1))
        return(FALSE);

    return(TRUE);
}


//********************************************************************
//
// Function:
//  DecryptXORBYTE()
//
// Parameters:
//  lpbyData                Pointer to a byte to be decrypted
//  byKey                   Value to XOR with the byte above
//
// Description:
//
//  This function decrypts the specified byte by XORing it with the key.
//
// Returns:
//  nothing
//
//********************************************************************

void DecryptXORBYTE
(
    LPBYTE  lpbyData,
    BYTE    byKey
)
{
    *lpbyData ^= byKey;
}

//********************************************************************
//
// Function:
//  DecryptXORWORD()
//
// Parameters:
//  lpbyData                Pointer to a WORD to be decrypted
//  byKey                   Value to XOR with the WORD above
//
// Description:
//
//  This function decrypts the specified WORD by XORing it with the key.
//
// Returns:
//  nothing
//
//********************************************************************

void DecryptXORWORD
(
    LPWORD  lpwData,
    WORD    wKey
)
{
    *lpwData ^= wKey;
}


//********************************************************************
//
// Function:
//  UpdateBYTEForward()
//
// Parameters:
//  lpbyKey                 Pointer to the current key value
//  byDelta                 Value to add to the current key value
//
// Description:
//
//  This function updates the key value by adding in the delta value.  It
//  is used when decrypting forward.
//
// Returns:
//  nothing
//
//********************************************************************

void UpdateBYTEForward
(
    LPBYTE  lpbyKey,
    BYTE    byDelta
)
{
    *lpbyKey += byDelta;
}


//********************************************************************
//
// Function:
//  UpdateBYTEBackward()
//
// Parameters:
//  lpbyKey                 Pointer to the current key value
//  byDelta                 Value to subtract to the current key value
//
// Description:
//
//  This function updates the key value by subtracting the delta value.  It
//  is used when decrypting backward.
//
// Returns:
//  nothing
//
//********************************************************************

void UpdateBYTEBackward
(
    LPBYTE  lpbyKey,
    BYTE    byDelta
)
{
    *lpbyKey -= byDelta;
}


//********************************************************************
//
// Function:
//  DecryptAddBYTE()
//
// Parameters:
//  lpbyData                Pointer to a byte to be decrypted
//  byKey                   Value to subtract from the byte above
//
// Description:
//
//  This function decrypts the specified (add-encrypted) byte by subtracting
//  out the key.
//
// Returns:
//  nothing
//
//********************************************************************

void DecryptAddBYTE
(
    LPBYTE  lpbyData,
    BYTE    byKey
)
{
    *lpbyData -= byKey;
}


//********************************************************************
//
// Function:
//  DecryptAddWORD()
//
// Parameters:
//  lpwData                Pointer to a word to be decrypted
//  wKey                   Value to subtract from the word above
//
// Description:
//
//  This function decrypts the specified (add-encrypted) word by subtracting
//  out the key.
//
// Returns:
//  nothing
//
//********************************************************************

void DecryptAddWORD
(
    LPWORD  lpwData,
    WORD    wKey
)
{
    *lpwData -= wKey;
}


//********************************************************************
//
// Function:
//  UpdateWORDForward()
//
// Parameters:
//  lpwKey                  Pointer to the current key value
//  wDelta                  Value to add to the current key value
//
// Description:
//
//  This function updates the key value by adding in the delta value.  It
//  is used when decrypting forward.
//
// Returns:
//  nothing
//
//********************************************************************

void UpdateWORDForward
(
    LPWORD  lpwKey,
    WORD    wDelta
)
{
    *lpwKey += wDelta;
}


//********************************************************************
//
// Function:
//  UpdateWORDBackward()
//
// Parameters:
//  lpwKey                  Pointer to the current key value
//  wDelta                  Value to add to the current key value
//
// Description:
//
//  This function updates the key value by subtracting the delta value.  It
//  is used when decrypting backward.
//
// Returns:
//  nothing
//
//********************************************************************

void UpdateWORDBackward
(
    LPWORD  lpwKey,
    WORD    wDelta
)
{
    *lpwKey -= wDelta;
}

//********************************************************************
//
// Function:
//  UpdateWORDOscil()
//
// Parameters:
//  lpwKey                  Pointer to the current key value
//  wDelta                  Value to add to the current key value
//
// Description:
//
//  This function updates the key value by XORing the delta value.  It
//  is used when decrypting forward or backward.
//
// Returns:
//  nothing
//
//********************************************************************

void UpdateWORDOscil
(
    LPWORD  lpwKey,
    WORD    wDelta
)
{
    *lpwKey ^= wDelta;
}


//********************************************************************
//
// Function:
//  DecryptBYTE()
//
// Parameters:
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//  byOrigKey               Key found during plaintext attack
//  byOrigDelta             Delta found during plaintext attack
//  lpUpdateBYTEKeyForward  Pointer to a function that is used to update the
//                          key when decrypting forward in the file
//  lpUpdateBYTEKeyBackward Pointer to a function that is used to update the
//                          key when decrypting backward in the file
//  lpDecryptBYTE           Pointer to a function that is used to decrypt a
//                          single byte of the encrypted virus, given the key
//                          and the delta.
//  lpbDecrypted            Set to TRUE if no problems are encountered
//
// Description:
//
//  This function applies the specified decryption routine comprised of
//  lpUpdateBYTEKeyForward, lpUpdateBYTEKeyBackward and lpDecryptBYTE to
//  the virus.  It decrypts the entire virus as specified by the virus size
//  or the first landing (depending on the flags used in the repair). This
//  routine physically modifies the contents of the file so the other repair
//  routines can be applied later as if the virus were not encrypted.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS DecryptBYTE
(
    LPFILEOBJECT        lpFileObject,
    LPVIRUS_REPAIR      lpVirusRepairInfo,
    BYTE                byOrigKey,
    BYTE                byOrigDelta,
    LPUPDATEBYTEKEY     lpUpdateBYTEKeyForward,
    LPUPDATEBYTEKEY     lpUpdateBYTEKeyBackward,
    LPDECRYPTBYTE       lpDecryptBYTE,
    LPBOOL              lpbDecrypted
)
{
    DWORD               dwStartOffset, dwOffset, dwLength, dwBytesRead,
                        dwTOV;
	BYTE                byFileData, byKey, byDelta;

    // assume the worst

    *lpbDecrypted = FALSE;

    // obtain the file length

    if (AVFileLength(lpFileObject->lpvInfo, &dwLength) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

	// determine where the top of the virus is in the file

    dwTOV = dwLength - lpVirusRepairInfo->wInfSize;

	// determine where we're going to start decrypting (at our decryption sig)

    dwStartOffset = dwTOV + lpVirusRepairInfo->wBookLocation;

    dwOffset = dwStartOffset;

    byKey = byOrigKey;
    byDelta = byOrigDelta;

    // decrypt from decryption signature forward til the EOF

    while (dwOffset < dwLength)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileRead(lpFileObject->lpvInfo,
                       &byFileData,
                       sizeof(BYTE),
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // unable to read byte

        if (dwBytesRead != sizeof(BYTE))
        {
            return(ENGSTATUS_OK);
        }

        // do decryption and key update

        lpDecryptBYTE(&byFileData,byKey);
        lpUpdateBYTEKeyForward(&byKey,byDelta);

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileWrite(lpFileObject->lpvInfo,
                        &byFileData,
                        sizeof(BYTE),
                        &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // unable to write byte

        if (dwBytesRead != sizeof(BYTE))
        {
            return(ENGSTATUS_OK);
        }

        dwOffset++;
    }

    dwOffset = dwStartOffset - 1;

    byKey = byOrigKey;
    byDelta = byOrigDelta;

    // decrypt from decryption signature backward til the TOV

	while (dwOffset >= dwTOV)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileRead(lpFileObject->lpvInfo,
                       &byFileData,
                       sizeof(BYTE),
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // unable to write byte

        if (dwBytesRead != sizeof(BYTE))
        {
            return(ENGSTATUS_OK);
        }

        // do key update and decryption

        lpUpdateBYTEKeyBackward(&byKey,byDelta);
        lpDecryptBYTE(&byFileData,byKey);

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileWrite(lpFileObject->lpvInfo,
                        &byFileData,
                        sizeof(BYTE),
                        &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // unable to write byte

        if (dwBytesRead != sizeof(BYTE))
        {
            return(ENGSTATUS_OK);
        }

        dwOffset--;
    }

    *lpbDecrypted = TRUE;

    return(ENGSTATUS_OK);
}

//********************************************************************
//
// Function:
//  DetermineAddBYTE()
//
// Parameters:
//  lpbyInfected            Pointer to bytes read from our target file
//  lpbyDecryptSig          Pointer to bytes from our signature to use in
//                          our plaintext attack.
//  lpbyBase                Points to the key value used in encryption (OUT)
//  lpbyDelta               Points to the delta value used in encryption (OUT)
//
// Description:
//
//  This function performs a plaintext attack on the data in the target file
//  in an attempt to determine if the Add/DELTA encryption method was used.  If
//  it was, it returns TRUE and sets *lpbyBase and *lpbyDelta to the proper
//  key values.  Otherwise it returns FALSE.  This handles encryption like:
//
//      MOV AL, 12              12 is the BASE key
// TOP: ADD [SI], AL            (ADD or SUB instructions OK)
//      ADD AL, 34              34 is the DELTA
//      INC SI
//      LOOP TOP
//
// Returns:
//  TRUE                    If the buffer uses this encryption scheme
//  FALSE                   Otherwise
//
//********************************************************************

BOOL DetermineAddBYTE
(
    LPBYTE  lpbyInfected,
    LPBYTE  lpbyDecryptSig,
    LPBYTE  lpbyBase,
    LPBYTE  lpbyDelta
)
{
    BYTE    byTemp;

	*lpbyBase = lpbyInfected[0] - (BYTE)(lpbyDecryptSig[0]-1);
    *lpbyDelta = (lpbyInfected[1] - (BYTE)(lpbyDecryptSig[1]-1)) -
                 *lpbyBase;

    byTemp = *lpbyBase + 2 * *lpbyDelta;

	if (lpbyInfected[2] != (BYTE)((BYTE)(lpbyDecryptSig[2]-1) + byTemp))
		return(FALSE);

	byTemp += *lpbyDelta;

	if (lpbyInfected[3] != (BYTE)((BYTE)(lpbyDecryptSig[3]-1) + byTemp))
        return(FALSE);

    return(TRUE);
}

//********************************************************************
//
// Function:
//  DetermineXORWORD()
//
// Parameters:
//  lpbyInfected            Pointer to bytes read from our target file
//  lpbyDecryptSig          Pointer to bytes from our signature to use in
//                          our plaintext attack.
//  lpwBase                 Points to the key value used in encryption (OUT)
//  lpwDelta                Points to the delta value used in encryption (OUT)
//
// Description:
//
//  This function performs a plaintext attack on the data in the target file
//  in an attempt to determine if the XOR/DELTA encryption method was used.  If
//  it was, it returns TRUE and sets *lpbyBase and *lpbyDelta to the proper
//  key values.  Otherwise it returns FALSE.  This handles encryption like:
//
//      MOV AX, 1234            1234 is the BASE key
// TOP: XOR [SI], AX
//      ADD AX, 5678            5678 is the DELTA
//      ADD SI, 2
//      LOOP TOP
//
// Returns:
//  TRUE                    If the buffer uses this encryption scheme
//  FALSE                   Otherwise
//
//********************************************************************

BOOL DetermineXORWORD
(
    LPBYTE  lpbyInfected,
    LPBYTE  lpbyDecryptSig,
    LPWORD  lpwBase,
    LPWORD  lpwDelta
)
{
    WORD    wTemp, wSig1, wSig2, wSig3, wSig4;
    LPWORD  lpwPtr;

	lpwPtr = (LPWORD)lpbyInfected;

	wSig1 = (BYTE)(lpbyDecryptSig[0]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[1]-1)) << 8);

	wSig2 = (BYTE)(lpbyDecryptSig[2]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[3]-1)) << 8);

    *lpwBase = AVDEREF_WORD(lpwPtr) ^ wSig1;
    *lpwDelta = (AVDEREF_WORD(lpwPtr+1) ^ wSig2) - *lpwBase;

	wTemp = *lpwBase + 2 * *lpwDelta;

	wSig3 = (BYTE)(lpbyDecryptSig[4]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[5]-1)) << 8);

    if ((AVDEREF_WORD(lpwPtr+2) ^ wTemp) != wSig3)
		return(FALSE);

	wTemp += *lpwDelta;

	wSig4 = (BYTE)(lpbyDecryptSig[6]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[7]-1)) << 8);

    if ((AVDEREF_WORD(lpwPtr+3) ^ wTemp) != wSig4)
		return(FALSE);

	return(TRUE);
}

//********************************************************************
//
// Function:
//  DecryptWORD()
//
// Parameters:
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//  byOrigKey               Key found during plaintext attack
//  byOrigDelta             Delta found during plaintext attack
//  lpUpdateWORDKeyForward  Pointer to a function that is used to update the
//                          key when decrypting forward in the file
//  lpUpdateWORDKeyBackward Pointer to a function that is used to update the
//                          key when decrypting backward in the file
//  lpDecryptWORD           Pointer to a function that is used to decrypt a
//                          single word of the encrypted virus, given the key
//                          and the delta.
//  lpbDecrypted            Was the file decrypted properly?
//
// Description:
//
//  This function applies the specified decryption routine comprised of
//  lpUpdateWORDKeyForward, lpUpdateWORDKeyBackward and lpDecryptBYTE to
//  the virus.  It decrypts the entire virus as specified by the virus size
//  or the first landing (depending on the flags used in the repair). This
//  routine physically modifies the contents of the file so the other repair
//  routines can be applied later as if the virus were not encrypted.  Even
//  though the routine decrypts WORD-encrypted viruses, it does so 1-byte at
//  a time, which is why lpDecryptBYTE is passed in.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS DecryptWORD
(
    LPFILEOBJECT        lpFileObject,
    LPVIRUS_REPAIR      lpVirusRepairInfo,
    WORD                wOrigKey,
    WORD                wOrigDelta,
    LPUPDATEWORDKEY     lpUpdateWORDKeyForward,
    LPUPDATEWORDKEY     lpUpdateWORDKeyBackward,
    LPDECRYPTWORD       lpDecryptWORD,
    LPBOOL              lpbDecrypted
)
{
    DWORD               dwStartOffset, dwOffset, dwLength, dwBytesRead,
                        dwTOV, dwBytesWritten,dwBytesToRead;
    WORD                wKey, wDelta, wFileData;

    // assume the worst

    *lpbDecrypted = FALSE;

    // obtain the file length

    if (AVFileLength(lpFileObject->lpvInfo, &dwLength) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // determine where the top of the virus is in the file

    dwTOV = dwLength - lpVirusRepairInfo->wInfSize;

    // determine where we're going to start decrypting (at our decryption sig)

    dwStartOffset = dwTOV + lpVirusRepairInfo->wBookLocation;

    dwOffset = dwStartOffset;

    wKey = wOrigKey;
    wDelta = wOrigDelta;

    // decrypt from decryption signature forward til the EOF

    while (dwOffset < dwLength)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (AVFileSeek(lpFileObject->lpvInfo,
                       dwOffset,
                       SEEK_SET,
                       &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwLength - dwOffset > sizeof(WORD))
            dwBytesToRead = sizeof(WORD);
        else
            dwBytesToRead = sizeof(BYTE);

        if (AVFileRead(lpFileObject->lpvInfo,
                       &wFileData,
                       dwBytesToRead,
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesRead != dwBytesToRead)
            return(ENGSTATUS_OK);

        wFileData = WENDIAN(wFileData); // endianize
        lpDecryptWORD(&wFileData,wKey); // decrypt
        wFileData = WENDIAN(wFileData); // unendianize for write-back

        // update our key

        lpUpdateWORDKeyForward(&wKey,wDelta);

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileWrite(lpFileObject->lpvInfo,
                        &wFileData,
                        dwBytesToRead,
                        &dwBytesWritten) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesWritten != dwBytesToRead)
            return(ENGSTATUS_OK);

        dwOffset += sizeof(WORD);
    }

    dwOffset = dwStartOffset - sizeof(WORD);

    wKey = wOrigKey;
    wDelta = wOrigDelta;

    // decrypt from decryption signature backward til the TOV

	while (dwOffset >= dwTOV)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileRead(lpFileObject->lpvInfo,
                       &wFileData,
                       sizeof(WORD),
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesRead != sizeof(WORD))
            return(ENGSTATUS_OK);

        lpUpdateWORDKeyBackward(&wKey,wDelta);
        wFileData = WENDIAN(wFileData); // endianize
        lpDecryptWORD(&wFileData,wKey);
        wFileData = WENDIAN(wFileData); // de-endianize

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   &dwOffset) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileWrite(lpFileObject->lpvInfo,
                        &wFileData,
                        sizeof(WORD),
                        &dwBytesWritten) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesWritten != sizeof(WORD))
            return(ENGSTATUS_OK);

        dwOffset -= sizeof(WORD);
	}

	*lpbDecrypted = TRUE;

    return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  DetermineAddWORD()
//
// Parameters:
//  lpbyInfected            Pointer to bytes read from our target file
//  lpbyDecryptSig          Pointer to bytes from our signature to use in
//                          our plaintext attack.
//  lpwBase                 Points to the key value used in encryption (OUT)
//  lpwDelta                Points to the delta value used in encryption (OUT)
//
// Description:
//
//  This function performs a plaintext attack on the data in the target file
//  in an attempt to determine if the Add/DELTA encryption method was used.  If
//  it was, it returns TRUE and sets *lpbyBase and *lpbyDelta to the proper
//  key values.  Otherwise it returns FALSE.  This handles encryption like:
//
//      MOV AX, 1234            1234 is the BASE key
// TOP: ADD [SI], AX            (ADD or SUB are OK)
//      ADD AX, 5678            5678 is the DELTA
//      ADD SI, 2
//      LOOP TOP
//
// Returns:
//  TRUE                    If the buffer uses this encryption scheme
//  FALSE                   Otherwise
//
//********************************************************************

BOOL DetermineAddWORD
(
    LPBYTE  lpbyInfectedBuffer,
    LPBYTE  lpbyDecryptSig,
    LPWORD  lpwBase,
    LPWORD  lpwDelta
)
{
    WORD    wTemp, wSig1, wSig2, wSig3, wSig4;
    LPWORD  lpwPtr;

    lpwPtr = (LPWORD)lpbyInfectedBuffer;

    wSig1 = (BYTE)(lpbyDecryptSig[0]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[1]-1)) << 8);

	wSig2 = (BYTE)(lpbyDecryptSig[2]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[3]-1)) << 8);

    *lpwBase = AVDEREF_WORD(lpwPtr) - wSig1;
    *lpwDelta = (AVDEREF_WORD(lpwPtr+1) - wSig2) - *lpwBase;

	wTemp = *lpwBase + 2 * *lpwDelta;

	wSig3 = (BYTE)(lpbyDecryptSig[4]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[5]-1)) << 8);

    if ((WORD)(AVDEREF_WORD(lpwPtr+2) - wTemp) != wSig3)
		return(FALSE);

	wTemp += *lpwDelta;

	wSig4 = (BYTE)(lpbyDecryptSig[6]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[7]-1)) << 8);

    if ((WORD)(AVDEREF_WORD(lpwPtr+3) - wTemp) != wSig4)
		return(FALSE);

	return(TRUE);
}


//********************************************************************
//
// Function:
//  DetermineXOROscilWORD()
//
// Parameters:
//  lpbyInfected            Pointer to bytes read from our target file
//  lpbyDecryptSig          Pointer to bytes from our signature to use in
//                          our plaintext attack.
//  lpwBase                 Points to the key value used in encryption (OUT)
//  lpwDelta                Points to the delta value used in encryption (OUT)
//
// Description:
//
//  This function performs a plaintext attack on the data in the target file
//  in an attempt to determine if the XOR/OSCIL encryption method was used.  If
//  it was, it returns TRUE and sets *lpbyBase and *lpbyDelta to the proper
//  key values.  Otherwise it returns FALSE.  This handles encryption like:
//
//      MOV AX, 1234            1234 is the BASE key
// TOP: XOR [SI], AX
//      XOR AX, 5678            5678 is the DELTA
//      ADD SI, 2
//      LOOP TOP
//
// Returns:
//  TRUE                    If the buffer uses this encryption scheme
//  FALSE                   Otherwise
//
//********************************************************************

BOOL DetermineXOROscilWORD
(
	LPBYTE  lpbyInfectedBuffer,
	LPBYTE  lpbyDecryptSig,
	LPWORD  lpwBase,
	LPWORD  lpwDelta
)
{
	WORD    wSig1, wSig2, wSig3, wSig4;
	LPWORD  lpwPtr;

	lpwPtr = (LPWORD)lpbyInfectedBuffer;

	wSig1 = (BYTE)(lpbyDecryptSig[0]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[1]-1)) << 8);

	wSig2 = (BYTE)(lpbyDecryptSig[2]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[3]-1)) << 8);

    *lpwBase = AVDEREF_WORD(lpwPtr) ^ wSig1;
    *lpwDelta = (AVDEREF_WORD(lpwPtr+1) ^ wSig2) ^ *lpwBase;

	wSig3 = (BYTE)(lpbyDecryptSig[4]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[5]-1)) << 8);

    if ((AVDEREF_WORD(lpwPtr+2) ^ *lpwBase) != wSig3)
		return(FALSE);

	wSig4 = (BYTE)(lpbyDecryptSig[6]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[7]-1)) << 8);

    if ((AVDEREF_WORD(lpwPtr+3) ^ *lpwBase ^ *lpwDelta) != wSig4)
		return(FALSE);

	return(TRUE);
}


//********************************************************************
//
// Function:
//  DetermineAddOscilWORD()
//
// Parameters:
//  lpbyInfected            Pointer to bytes read from our target file
//  lpbyDecryptSig          Pointer to bytes from our signature to use in
//                          our plaintext attack.
//  lpwBase                 Points to the key value used in encryption (OUT)
//  lpwDelta                Points to the delta value used in encryption (OUT)
//
// Description:
//
//  This function performs a plaintext attack on the data in the target file
//  in an attempt to determine if the ADD/OSCIL encryption method was used.  If
//  it was, it returns TRUE and sets *lpbyBase and *lpbyDelta to the proper
//  key values.  Otherwise it returns FALSE.  This handles encryption like:
//
//      MOV AX, 1234            1234 is the BASE key
// TOP: ADD [SI], AX            (ADD or SUB are OK)
//      XOR AX, 5678            5678 is the DELTA
//      ADD SI, 2
//      LOOP TOP
//
// Returns:
//  TRUE                    If the buffer uses this encryption scheme
//  FALSE                   Otherwise
//
//********************************************************************

BOOL DetermineAddOscilWORD
(
    LPBYTE  lpbyInfectedBuffer,
	LPBYTE  lpbyDecryptSig,
	LPWORD  lpwBase,
	LPWORD  lpwDelta
)
{
	WORD    wSig1, wSig2, wSig3, wSig4;
	LPWORD  lpwPtr;

	lpwPtr = (LPWORD)lpbyInfectedBuffer;

	wSig1 = (BYTE)(lpbyDecryptSig[0]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[1]-1)) << 8);

	wSig2 = (BYTE)(lpbyDecryptSig[2]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[3]-1)) << 8);

    *lpwBase = AVDEREF_WORD(lpwPtr) - wSig1;
    *lpwDelta = (AVDEREF_WORD(lpwPtr+1) - wSig2) ^ *lpwBase;

	wSig3 = (BYTE)(lpbyDecryptSig[4]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[5]-1)) << 8);

    if ((WORD)(AVDEREF_WORD(lpwPtr+2) - *lpwBase) != wSig3)
		return(FALSE);

	wSig4 = (BYTE)(lpbyDecryptSig[6]-1) |
			(((WORD)(BYTE)(lpbyDecryptSig[7]-1)) << 8);

    if ((WORD)(AVDEREF_WORD(lpwPtr+3) - (*lpwBase ^ *lpwDelta)) != wSig4)
		return(FALSE);

	return(TRUE);
}

//********************************************************************
//
// Function:
//  EngDecryptForRepair()
//
// Parameters:
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//  lpbyDecryptSig          This is the 8 (or 4) byte memory decrypt signature
//                          to be used in the plaintext attack
//  lpbDecrypted            Set TRUE if virus was decrypted, FALSE otherwise
//
// Description:
//
//  This function uses the lpbyDecryptSig to determine whether or not the
//  virus is encrypted using any of the known encryption schemes.  The
//  function assumes that the file pointer in the infected host is located
//  right at the point where the plaintext attach should take place.  If the
//  plaintext attack is successful, the resulting key/delta are used to
//  decrypt the entire virus (as specified by the virus size field or the
//  dynamic calculation of the virus size by using the first landing offset.)
//  This function will also allow defs to repair viruses that only encrypt
//  the repair bytes, but don't encrypt the rest of the virus.  In this case,
//  the wBookMark value is set to an offset in the virus where the decryption
//  key is located.  In addition, several extra control flags are specified to
//  indicate whether to use ADD, SUB or XOR to do the decryption of the host
//  bytes.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************


ENGSTATUS EngDecryptForRepair
(
    LPFILEOBJECT    lpFileObject,
    LPVIRUS_REPAIR  lpVirusRepairInfo,
    LPBYTE          lpbyDecryptSig,
    LPBOOL          lpbDecrypted
)
{
    DWORD           dwSeekOff, dwLength, dwBytesRead;
    WORD            wImmediateKey;
    BYTE            byInfectedBuffer[DECRYPT_MEM_SIG_SIZE];
    BYTE            byBase, byDelta;
    WORD            wBase, wDelta;

	// assume failure

	*lpbDecrypted = FALSE;

	// obtain the file length

    if (AVFileLength(lpFileObject->lpvInfo, &dwLength) == CBSTATUS_ERROR)
		return(ENGSTATUS_ERROR);

	// locate where our encrypted data is located

	dwSeekOff = dwLength - lpVirusRepairInfo->wInfSize +
				lpVirusRepairInfo->wBookLocation;

	// if we're out of synch with respect to encryption then advance to the
	// next byte.  Our book location changes too...

	if (lpVirusRepairInfo->dwRepairFlags & CTRL_SECONDBOOK)
	{
		lpVirusRepairInfo->wBookLocation++;
		dwSeekOff++;

		// note: when the VIRSCAN?.DAT data files are build, they automatically
		// have a shifted decryption memory signature in the 8 byte memory
		// repair field.  Therefore, there is no need to do the shift in here.
	}

	// seek & read our data from the infected file to determine the decryption
	// scheme.  we must have at least 8 bytes

    if (AVFileSeek(lpFileObject->lpvInfo,
                   dwSeekOff,
                   SEEK_SET,
                   &dwSeekOff) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (AVFileRead(lpFileObject->lpvInfo,
                   byInfectedBuffer,
                   DECRYPT_MEM_SIG_SIZE,
                   &dwBytesRead) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (dwBytesRead != DECRYPT_MEM_SIG_SIZE)
        return(ENGSTATUS_OK);           // *lpbDecrypted is false

    // check to see if the file is encrypted with a BYTE XOR encryption scheme

    if (DetermineXORBYTE(byInfectedBuffer,
                         lpbyDecryptSig,
						 &byBase,
						 &byDelta))
	{
		// we have a hit!  deal with the two possibilities:
		// 1. We actually have XOR encryption.
		// 2. We are performing a decryption where only the repair bytes are
		//    encrypted (using XOR or ADD, no deltas).  If the wBookMark value
		//    is non zero, then the code ASSUMES that the KEY and DELTA are 0
		//    and that the bulk of the file is not encrypted. Basically, we
		//    just performed a 4-byte bookmark and now we're going to locate
		//    a key at a fixed location in the executable.  We're going to
		//    encrypt the entire virus using this key, which will in turn
		//    decrypt the encrypted repair bytes.

		if (lpVirusRepairInfo->wBookMark != 0)      // case 2 from above
		{
			// set dwSeekOff to the location of the KEY in the virus

			dwSeekOff = dwLength - lpVirusRepairInfo->wInfSize +
						lpVirusRepairInfo->wBookMark;

            if (AVFileSeek(lpFileObject->lpvInfo,
						   dwSeekOff,
						   SEEK_SET,
						   &dwSeekOff) == CBSTATUS_ERROR)
				return(ENGSTATUS_ERROR);

			// read the key used to encrypt the repair bytes...

            if (AVFileRead(lpFileObject->lpvInfo,
						   &wImmediateKey,
						   sizeof(WORD),
                           &dwBytesRead) == CBSTATUS_ERROR)
				return(ENGSTATUS_ERROR);

            if (dwBytesRead != sizeof(WORD))
                return(ENGSTATUS_OK);           // *lpbDecrypted is false

			wImmediateKey = WENDIAN(wImmediateKey);

            // now see whether or not we have a WORD or BYTE decryption

            if ((lpVirusRepairInfo->dwRepairFlags & CTRL2_DECRYPT_WORD) == 0)
            {
                // byte decryption

                if (lpVirusRepairInfo->dwRepairFlags & CTRL2_DECRYPT_XOR)
                {
                    if (DecryptBYTE(lpFileObject,
                                    lpVirusRepairInfo,
                                    (BYTE)(wImmediateKey & 0xFF),
                                    0,
                                    UpdateBYTEForward,
                                    UpdateBYTEBackward,
                                    DecryptXORBYTE,
                                    lpbDecrypted) == ENGSTATUS_ERROR)
                        return(ENGSTATUS_ERROR);

                    return(ENGSTATUS_OK);
                }
                else if (lpVirusRepairInfo->dwRepairFlags & CTRL2_DECRYPT_ADD)
                {
                    if (DecryptBYTE(lpFileObject,
                                       lpVirusRepairInfo,
                                       (BYTE)(wImmediateKey & 0xFF),
									   0,
									   UpdateBYTEForward,
									   UpdateBYTEBackward,
                                       DecryptAddBYTE,
                                       lpbDecrypted) == ENGSTATUS_ERROR)
						return(ENGSTATUS_ERROR);

					return(ENGSTATUS_OK);
				}
				else
				{
					// fallthru indicates subtraction, so negate the key...

					if (DecryptBYTE(lpFileObject,
									lpVirusRepairInfo,
                                    (BYTE)-(signed char)(wImmediateKey & 0xFF),
									0,
									UpdateBYTEForward,
									UpdateBYTEBackward,
                                    DecryptAddBYTE,
                                    lpbDecrypted) == ENGSTATUS_ERROR)
						return(ENGSTATUS_ERROR);

					return(ENGSTATUS_OK);

				}
			}
			else
			{
				// word decryption

				if (lpVirusRepairInfo->dwRepairFlags & CTRL2_DECRYPT_XOR)
				{
					if (DecryptWORD(lpFileObject,
									lpVirusRepairInfo,
									wImmediateKey,
									0,
									UpdateWORDForward,
									UpdateWORDBackward,
                                    DecryptXORWORD,
                                    lpbDecrypted) == ENGSTATUS_ERROR)
						return(ENGSTATUS_ERROR);

					return(ENGSTATUS_OK);
				}
				else if (lpVirusRepairInfo->dwRepairFlags & CTRL2_DECRYPT_ADD)
				{
					if (DecryptWORD(lpFileObject,
									lpVirusRepairInfo,
									wImmediateKey,
									0,
									UpdateWORDForward,
									UpdateWORDBackward,
                                    DecryptAddWORD,
                                    lpbDecrypted) == ENGSTATUS_ERROR)
						return(ENGSTATUS_ERROR);

					return(ENGSTATUS_OK);
				}
				else
				{
					// fallthru indicates subtraction, so negate the key...

					if (DecryptWORD(lpFileObject,
									lpVirusRepairInfo,
                                    (WORD)-(short)wImmediateKey,
									0,
									UpdateWORDForward,
									UpdateWORDBackward,
                                    DecryptAddWORD,
                                    lpbDecrypted)  == ENGSTATUS_ERROR)
						return(ENGSTATUS_ERROR);

					return(ENGSTATUS_OK);
				}

			}

		}
		else
		{
			// our wBookMark is 0, which means we just want to do a normal
			// byte xor decrypt repair...

			if (DecryptBYTE(lpFileObject,
							lpVirusRepairInfo,
							byBase,
							byDelta,
							UpdateBYTEForward,
							UpdateBYTEBackward,
                            DecryptXORBYTE,
                            lpbDecrypted) == ENGSTATUS_ERROR)
						return(ENGSTATUS_ERROR);

			return(ENGSTATUS_OK);
		}
	}

	// now check for other simple encryption schemes

	if (DetermineAddBYTE(byInfectedBuffer,
						 lpbyDecryptSig,
						 &byBase,
						 &byDelta))
	{
		if (DecryptBYTE(lpFileObject,
						lpVirusRepairInfo,
						byBase,
						byDelta,
						UpdateBYTEForward,
						UpdateBYTEBackward,
                        DecryptAddBYTE,
                        lpbDecrypted) == ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

		return(ENGSTATUS_OK);
	}

	if (DetermineXORWORD(byInfectedBuffer,
						 lpbyDecryptSig,
						 &wBase,
						 &wDelta))
	{
		if (DecryptWORD(lpFileObject,
						lpVirusRepairInfo,
						wBase,
						wDelta,
						UpdateWORDForward,
						UpdateWORDBackward,
                        DecryptXORWORD,
                        lpbDecrypted) == ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

		return(ENGSTATUS_OK);
	}

	if (DetermineAddWORD(byInfectedBuffer,
						 lpbyDecryptSig,
						 &wBase,
						 &wDelta))
	{
		if (DecryptWORD(lpFileObject,
						lpVirusRepairInfo,
						wBase,
						wDelta,
						UpdateWORDForward,
						UpdateWORDBackward,
                        DecryptAddWORD,
                        lpbDecrypted) == ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

		return(ENGSTATUS_OK);
	}

	if (DetermineAddOscilWORD(byInfectedBuffer,
						 lpbyDecryptSig,
						 &wBase,
						 &wDelta))
	{
		if (DecryptWORD(lpFileObject,
						lpVirusRepairInfo,
						wBase,
						wDelta,
						UpdateWORDOscil,
						UpdateWORDOscil,
                        DecryptAddWORD,
                        lpbDecrypted) == ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

		return(ENGSTATUS_OK);
	}

	if (DetermineXOROscilWORD(byInfectedBuffer,
						 lpbyDecryptSig,
						 &wBase,
						 &wDelta))
	{
		if (DecryptWORD(lpFileObject,
						lpVirusRepairInfo,
						wBase,
						wDelta,
						UpdateWORDOscil,
						UpdateWORDOscil,
                        DecryptXORWORD,
                        lpbDecrypted) == ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

		return(ENGSTATUS_OK);
	}

	// not decrypted... (*lpbDecrypted is FALSE from above)

	return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  CopyBytesToTop()
//
// Parameters:
//  lpFileObject            File pointer to the virus-infected file
//  wHostBytesFromTOV       Offset from top of virus where to find host's bytes
//  dwCopyHere              Offset in host file where to copy bytes
//  wNumBytesToCopy         How many host bytes do we copy from virus to host
//  wVirusSize              The size of the (appended) virus
//  lpbRepaired             Was the file repaired or was there a problem?
//
// Description:
//
//  This function copies the specified number of bytes from the virus to
//  the specified location.  The virus is assumed to start wVirusSize bytes
//  from the end of the file.  The function sets *lpbRepaired to TRUE if
//  the byte copy was successful, to FALSE otherwise.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS CopyBytesToTop
(
    LPFILEOBJECT    lpFileObject,
    WORD            wHostBytesFromTOV,
    DWORD           dwCopyHere,
    WORD            wNumBytesToCopy,
    WORD            wVirusSize,
    LPBOOL          lpbRepaired
)
{
    DWORD           dwLength, dwOffset, dwBytesRead, dwSeekDest,
                    dwBytesWritten;
    WORD            wCopyCount;
    BYTE            byData;

    *lpbRepaired = FALSE;

    if (AVFileLength(lpFileObject->lpvInfo, &dwLength) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // determine where our repair bytes are in the file (in the virus)

    dwOffset = dwLength - wVirusSize + wHostBytesFromTOV;

    for (wCopyCount = 0; wCopyCount < wNumBytesToCopy; wCopyCount++)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (AVFileSeek(lpFileObject->lpvInfo,
                       dwOffset,
                       SEEK_SET,
                       &dwSeekDest) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileRead(lpFileObject->lpvInfo,
                       &byData,
                       sizeof(BYTE),
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesRead != sizeof(BYTE))
            return(ENGSTATUS_OK);

        if (AVFileSeek(lpFileObject->lpvInfo,
                       dwCopyHere,
                       SEEK_SET,
                       &dwSeekDest) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (AVFileWrite(lpFileObject->lpvInfo,
                        &byData,
                        sizeof(BYTE),
                        &dwBytesWritten) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesWritten != sizeof(BYTE))
            return(ENGSTATUS_OK);

        dwOffset ++;
        dwCopyHere++;
    }

    *lpbRepaired = TRUE;

    return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  RepairFileGeneric()
//
// Parameters:
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//  lpbRepaired             Was the file repaired?
//
// Description:
//
//  This function performs either a standard copy of contiguous bytes from
//  the virus body to the host, or performs a com-distinct repair (e.g. the
//  host bytes are distributed throughout the virus).  It is also used for
//  EXE repairs where we're copying the full header back to the TOF from the
//  virus.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS RepairFileGeneric
(
    LPFILEOBJECT    lpFileObject,
    LPVIRUS_REPAIR  lpVirusRepairInfo,
    LPBOOL          lpbRepaired
)
{
    *lpbRepaired = FALSE;               // assume the worst

    if ((lpVirusRepairInfo->dwRepairFlags & CTRL2_COM_DISTINCT) == 0)
    {
        // perform normal generic repair - just copy specified number of bytes
        // from virus to TOF

        if (CopyBytesToTop(lpFileObject,
                           lpVirusRepairInfo->wHostJmp,
                           0,                           // TOF
                           lpVirusRepairInfo->wJmpLen,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

		return(ENGSTATUS_OK);
	}

	// COM distinct repair (copy one byte at a time)

	if (lpVirusRepairInfo->wHostJmp != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
						   lpVirusRepairInfo->wHostJmp,
						   0,                           // TOF
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

	if (lpVirusRepairInfo->wJmpLen != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
						   lpVirusRepairInfo->wJmpLen,
						   1,                           // TOF+1
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

    if (lpVirusRepairInfo->wSS_reg != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
                           lpVirusRepairInfo->wSS_reg,
						   2,                           // TOF+2
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

    if (lpVirusRepairInfo->wSP_reg != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
                           lpVirusRepairInfo->wSP_reg,
						   3,                           // TOF+3
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

    if (lpVirusRepairInfo->wCS_reg != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
                           lpVirusRepairInfo->wCS_reg,
						   4,                           // TOF+4
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

    if (lpVirusRepairInfo->wIP_reg != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
                           lpVirusRepairInfo->wIP_reg,
						   5,                           // TOF+5
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

	if (lpVirusRepairInfo->wHeader != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
						   lpVirusRepairInfo->wHeader,
						   6,                           // TOF+6
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

	if (lpVirusRepairInfo->wHeadLen != 0xFFFFU)
	{
		if (CopyBytesToTop(lpFileObject,
						   lpVirusRepairInfo->wHeadLen,
						   7,                           // TOF+7
						   1,
                           lpVirusRepairInfo->wInfSize,
                           lpbRepaired) ==
			ENGSTATUS_ERROR)
			return(ENGSTATUS_ERROR);

        // if the repair goes bad, just return right away

        if (*lpbRepaired == FALSE)
            return(ENGSTATUS_OK);
	}

    *lpbRepaired = TRUE;

	return(ENGSTATUS_OK);
}



//********************************************************************
//
// Function:
//  RepairEXENorm()
//
// Parameters:
//  hLEng                   Handle to a local engine context
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//  lpbRepaired             Was the file repaired or was there an error?
//
// Description:
//
//  This function repairs EXE files.  It copies the various EXE header
//  fields (CS, IP, SS, SP) from the virus to the EXE header at the TOF.
//  It will also recompute the size of the EXE load image after subtracting
//  out the virus size and tweak CS/SS values appropriately.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS RepairEXENorm
(
    HLENG           hLEng,
    LPFILEOBJECT    lpFileObject,
    LPVIRUS_REPAIR  lpVirusRepairInfo,
    LPBOOL          lpbRepaired
)
{
    DWORD           dwLength, dwOffset, dwSeekDest,
                    dwBytesRead, dwBytesWritten;
    WORD            wPageSize, wLenMod512;

    *lpbRepaired = FALSE;               // assume the worst

    if (AVFileLength(lpFileObject->lpvInfo, &dwLength) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    // determine where our repair bytes are in the file (in the virus)

    dwOffset = dwLength - lpVirusRepairInfo->wInfSize;

    // read the original CS from the virus and place it in our FSTART buffer

    if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset + lpVirusRepairInfo->wCS_reg,
                   SEEK_SET,
                   &dwSeekDest) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

    if (AVFileRead(lpFileObject->lpvInfo,
                   &((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->cs,
                   sizeof(WORD),
                   &dwBytesRead) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (dwBytesRead != sizeof(WORD))
    {
        return(ENGSTATUS_OK);           // failure.  *lpbRepaired is false
    }

    // read the original IP from the virus and place it in our FSTART buffer

    if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset + lpVirusRepairInfo->wIP_reg,
                   SEEK_SET,
                   &dwSeekDest) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

    if (AVFileRead(lpFileObject->lpvInfo,
                   &((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->ip,
                   sizeof(WORD),
                   &dwBytesRead) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (dwBytesRead != sizeof(WORD))
    {
        return(ENGSTATUS_OK);           // failure.  *lpbRepaired is false
    }

    if (lpVirusRepairInfo->wSS_reg != 0xFFFF)
    {
        // read the original SS from the virus and place it in our FSTART buffer

        if (AVFileSeek(lpFileObject->lpvInfo,
                       dwOffset + lpVirusRepairInfo->wSS_reg,
                       SEEK_SET,
                       &dwSeekDest) == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

        if (AVFileRead(lpFileObject->lpvInfo,
                       &((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->ss,
                       sizeof(WORD),
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesRead != sizeof(WORD))
        {
            return(ENGSTATUS_OK);           // failure.  *lpbRepaired is false
        }
    }

    if (lpVirusRepairInfo->wSP_reg != 0xFFFF)
    {
        // read the original SP from the virus and place it in our FSTART buffer

        if (AVFileSeek(lpFileObject->lpvInfo,
                       dwOffset + lpVirusRepairInfo->wSP_reg,
                       SEEK_SET,
                       &dwSeekDest) == CBSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

        if (AVFileRead(lpFileObject->lpvInfo,
                       &((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->sp,
                       sizeof(WORD),
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwBytesRead != sizeof(WORD))
        {
            return(ENGSTATUS_OK);           // failure.  *lpbRepaired is false
        }
    }

    // now re-calculate the load image size and place it back in the EXE
    // header...  The new EOF is in DWOFFSET...
    // EXE files have their load image described in 512 byte pages...

    wPageSize = (WORD)(dwOffset / 512);
    wLenMod512 = (WORD)(dwOffset % 512);

    if (dwOffset % 512)
    {
        // if the file has extra data in its last page (less than 512 bytes)
        // then we bump up the page size count...

        wPageSize++;
    }

    // endianize after setting new size...

    ((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->page_size =
        WENDIAN(wPageSize);
    ((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->len_mod_512 =
        WENDIAN(wLenMod512);

    // now check to see if we need to weak our CS/SS values by subtracting
    // 0x10.  Many viruses add 0x10 to the CS, SS and store them in the
    // virus body.

	if (lpVirusRepairInfo->dwRepairFlags & CTRL2_TWEEK_CS_10)
	{
		WORD    wCS;

		wCS = WENDIAN(((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->cs);
		wCS -= 0x10;
		((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->cs = WENDIAN(wCS);
	}

    if (lpVirusRepairInfo->dwRepairFlags & CTRL2_TWEEK_SS_10)
    {
        WORD    wSS;

        wSS = WENDIAN(((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->ss);
        wSS -= 0x10;
        ((LPEXEHEADER)hLEng->stAlgScanData.stFStartData.byTOFBuffer)->ss = WENDIAN(wSS);
    }

    // finally, seek to the TOF and write out our new header.

    if (AVFileSeek(lpFileObject->lpvInfo,
                   0,
                   SEEK_SET,
                   &dwSeekDest) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (AVFileWrite(lpFileObject->lpvInfo,
                    hLEng->stAlgScanData.stFStartData.byTOFBuffer,
                    sizeof(EXEHEADER_T),
                    &dwBytesWritten) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // if we didn't write the right amount, just return (*lpbRepaired == FALSE)

    if (dwBytesWritten != sizeof(EXEHEADER_T))
        return(ENGSTATUS_OK);

    *lpbRepaired = TRUE;

    return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  RepairPrependedCOM()
//
// Parameters:
//  hLEng                   Handle to a local engine context
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//  lpbRepaired             Was the repair successful?
//
// Description:
//
//  This function repairs COM viruses of the prepended or improved overwriting
//  variety.  In the case of a prepending virus, V=virus size, H=host size,
//  it moves H bytes from offset V in the infected file to offset 0.  In the
//  case of an improved overwriting virus, it copies the last V bytes of the
//  file over the first V bytes of the file.  The file is truncated ourside of
//  this function.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS RepairPrependedCOM
(
    HLENG           hLEng,
    LPFILEOBJECT    lpFileObject,
    LPVIRUS_REPAIR  lpVirusRepairInfo,
    LPBOOL          lpbRepaired
)
{
    DWORD           dwBytesToCopy, dwReadOffset, dwWriteOffset, dwSeekDest,
					dwLength, dwPassBytes, dwBytesRead,
                    dwBytesWritten;

    // always assume the worst

    *lpbRepaired = FALSE;

    // always write data to the TOF

    dwWriteOffset = 0;

    // determine the length of the infected program...

    if (AVFileLength(lpFileObject->lpvInfo,
                     &dwLength) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    if (lpVirusRepairInfo->dwRepairFlags & CTRL_IMPROVED)
    {
        // improved overwriting infection
        // start reading from the last V bytes of the file where the virus
        // stored the original code

        dwReadOffset = dwLength - lpVirusRepairInfo->wInfSize;
        dwBytesToCopy = lpVirusRepairInfo->wInfSize;
    }
    else
    {
        // prepended infection
        // start reading from right after the virus

        dwReadOffset = lpVirusRepairInfo->wInfSize;
        dwBytesToCopy = dwLength - lpVirusRepairInfo->wInfSize;
    }

    if (dwBytesToCopy == 0 )
    {
        if (AVFileSeek(lpFileObject->lpvInfo,
                       0, SEEK_SET, &dwSeekDest) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        *lpbRepaired = TRUE;
        return(ENGSTATUS_OK);
    }


    while (dwBytesToCopy > 0)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        // how many bytes should we copy this pass?

        if (dwBytesToCopy > TEMP_BUFFER_SIZE)
            dwPassBytes = TEMP_BUFFER_SIZE;
        else
            dwPassBytes = dwBytesToCopy;

        // seek so we can read our data

        if (AVFileSeek(lpFileObject->lpvInfo,
                   dwReadOffset,
                   SEEK_SET,
                   &dwSeekDest) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // read the data into our temporary buffer

        if (AVFileRead(lpFileObject->lpvInfo,
                       hLEng->byTempBuffer,
                       dwPassBytes,
                       &dwBytesRead) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (dwPassBytes != dwBytesRead)
        {
            // *lpbRepaired is false, indicating error

            return(ENGSTATUS_OK);
        }

        // seek so we can write our data

        if (AVFileSeek(lpFileObject->lpvInfo,
                       dwWriteOffset,
                       SEEK_SET,
                       &dwSeekDest) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // write the data from our temporary buffer

        if (AVFileWrite(lpFileObject->lpvInfo,
                        hLEng->byTempBuffer,
                        dwPassBytes,
                        &dwBytesWritten) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

		if (dwPassBytes != dwBytesWritten)
        {
            // *lpbRepaired is false, indicating error

            return(ENGSTATUS_OK);
        }

        // calculate remaining bytes to copy and where to copy them...

        dwBytesToCopy -= dwPassBytes;

        dwReadOffset += dwPassBytes;
        dwWriteOffset += dwPassBytes;
    }

    *lpbRepaired = TRUE;

    return(ENGSTATUS_OK);
}



//********************************************************************
//
// Function:
//  RepairPrimedFile()
//
// Parameters:
//  hLEng                   Handle to a local engine context
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//	lpbRepaired				Was the file repaired correctly?
//
// Description:
//
//  This function assumes that the infected file has already been decrypted
//  (if necessary) and is ready for unencrypted bytes to be copied back to
//  the host file.  It determines the file type and then applies the proper
//  EXE or COM repair.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************

ENGSTATUS RepairPrimedFile
(
    HLENG           hLEng,
    LPFILEOBJECT    lpFileObject,
	LPVIRUS_REPAIR  lpVirusRepairInfo,
	LPBOOL          lpbRepaired
)
{
    *lpbRepaired = FALSE;               // assume the worst

	// are we dealing with an EXE file

    if (hLEng->stFStartInfo.wFileType == FSTART_EXE_FILE_TYPE)
	{
		if (lpVirusRepairInfo->dwRepairFlags & CTRL_GENERIC_EXE)
		{
			// set up the proper values and repair as if the file were a COM
			// this is the case where we're copying the entire header back to
			// the TOF from the virus.  Its just like copying the host bytes
			// in a COM file.

			lpVirusRepairInfo->wHostJmp = lpVirusRepairInfo->wHeader;
			lpVirusRepairInfo->wJmpLen = lpVirusRepairInfo->wHeadLen;

			// tag is for jerusalem-like viruses that both prepend and append

			lpVirusRepairInfo->wInfSize += lpVirusRepairInfo->wTagSize;
            if (RepairFileGeneric(lpFileObject,
                                  lpVirusRepairInfo,
                                  lpbRepaired)
				== ENGSTATUS_ERROR)
			{
				return(ENGSTATUS_ERROR);
			}

			return(ENGSTATUS_OK);
		}
		else
		{
			// perform a normal EXE repair...

            if (RepairEXENorm(hLEng,
                              lpFileObject,
                              lpVirusRepairInfo,
                              lpbRepaired)
				== ENGSTATUS_ERROR)
			{
				return(ENGSTATUS_ERROR);
			}

			return(ENGSTATUS_OK);
		}
	}
    else if (hLEng->stFStartInfo.wFileType != FSTART_SYS_FILE_TYPE)
	{
		// assume COM...

		if (lpVirusRepairInfo->dwRepairFlags & CTRL_GENERIC_COM)
		{
			// standard generic repair (3 byte jump at top of file type thing)

            if (RepairFileGeneric(lpFileObject,
                                  lpVirusRepairInfo,
                                  lpbRepaired)
				== ENGSTATUS_ERROR)
			{
				return(ENGSTATUS_ERROR);
			}

			return(ENGSTATUS_OK);
		}
		else
		{
			// prepending or improved overwriting virus

            if (RepairPrependedCOM(hLEng,
                                   lpFileObject,
                                   lpVirusRepairInfo,
                                   lpbRepaired)
				== ENGSTATUS_ERROR)
			{
				return(ENGSTATUS_ERROR);
			}

            return(ENGSTATUS_OK);
		}
	}

    // *lpbRepaired = FALSE and we were unable to repair.

    return(ENGSTATUS_OK);
}



//********************************************************************
//
// Function:
//  ZeroAndCut()
//
// Parameters:
//  lpFileObject            File pointer to the virus-infected file
//  dwTruncateOffset        Where to truncate the infected file (TOF-based)
//  dwFileLength            How long is the infected file?
//  lpbTruncated            Were we able to properly truncate?
//
// Description:
//
//  This function fills the region between dwTruncateOffset and dwFileLength
//  in the infected file with 0's and then truncates the file at offset
//  dwTruncateOffset.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************


ENGSTATUS ZeroAndCut
(
    LPFILEOBJECT    lpFileObject,
    DWORD           dwTruncateOffset,
    DWORD           dwFileLength,
    LPBOOL          lpbTruncated
)
{
    BYTE            byZero;
    DWORD           dwOff, dwBytesWritten;

    // assume the worst

    *lpbTruncated = FALSE;

    byZero = 0;

    // seek to the start of the virus

    if (AVFileSeek(lpFileObject->lpvInfo,
                   dwTruncateOffset,
                   SEEK_SET,
                   &dwOff) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // overwrite it with zeros

    for (dwOff = dwTruncateOffset; dwOff < dwFileLength; dwOff++)
    {
#ifdef SYM_NLM
        ThreadSwitch();
#endif

        if (AVFileWrite(lpFileObject->lpvInfo,
                        &byZero,
                        sizeof(BYTE),
                        &dwBytesWritten) == CBSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        // problem writing a byte!

        if (dwBytesWritten != sizeof(BYTE))
            return(ENGSTATUS_OK);
    }

    // seek to the start of the virus

    if (AVFileSeek(lpFileObject->lpvInfo,
                   dwTruncateOffset,
                   SEEK_SET,
                   &dwOff) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // truncate the file here

    if (AVFileSetEOF(lpFileObject->lpvInfo) == CBSTATUS_ERROR)
    {
        return(ENGSTATUS_ERROR);
    }

    *lpbTruncated = TRUE;

    return(ENGSTATUS_OK);
}




//********************************************************************
//
// Function:
//  AttemptRepair()
//
// Parameters:
//  hLEng                   Handle to a local engine context
//  lpFileObject            File pointer to the virus-infected file
//  lpVirusRepairInfo       Virus repair record
//  lpbyDecryptSig          Plaintext data used for decryption repairs
//  lpbRepairedFile         Set to TRUE on repair or FALSE on failure
//  lpbRepairTriedAndFailed Set to TRUE if a repair was attempted; false if
//                          the bookmark or decryption fails before a repair
//                          is applied.
//
// Description:
//
//  This function applies a given lpVirusRepairInfo record to the infected
//  file.  It first verifies the bookmark if the repair is not an encrypting
//  repair.  If the bookmark is found, it will complete the repair normally
//  and truncate the file.  If the repair requires decryption, it will attempt
//  to do a plaintext attack on the encryption to determine the keys.  It will
//  then decrypt the rest of the virus if it is successful in determing the
//  keys.  If the bookmark is not found or the plaintext attack fails, this
//  function will return and indicate that the repair failed.  The repair
//  engine will then attempt any other matching repairs (in the case of CRC
//  detection) or just report that the file cannot be repaired (in the case of
//  ALG detection).
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************


ENGSTATUS AttemptRepair
(
    HLENG           hLEng,
    LPFILEOBJECT    lpFileObject,
	LPVIRUS_REPAIR  lpVirusRepairInfo,
	LPBYTE          lpbyDecryptSig,
    LPBOOL          lpbRepairedFile,
    LPBOOL          lpbRepairTriedAndFailed
)
{
	DWORD           dwRepairFlags, dwLength, dwOff, dwBytesRead;
	WORD            wSlideDist, wIndex, wBookCheck;
	LPWORD          lpwBookValue;
	BOOL            bTryToRepair, bDecrypted, bRepaired;

	bTryToRepair = FALSE;
    *lpbRepairTriedAndFailed = *lpbRepairedFile = FALSE;

	dwRepairFlags = lpVirusRepairInfo->dwRepairFlags;

	// first get the file length

    if (AVFileLength(lpFileObject->lpvInfo,
		&dwLength) == CBSTATUS_ERROR)
	{
		return(ENGSTATUS_ERROR);
	}

	if ((dwRepairFlags & CTRL_NOCLEAN) == 0)
	{
		// well, there's not a no-clean flag so we might as well try!
		// see if we want to determine virus size from the entry-point

		if (dwRepairFlags & CTRL2_BOOK_FROM_BEGIN)
		{

            // calculate a new virus size equal to the distance between
            // the end of the file and the entry-point (as determined by
            // FSTART)

            lpVirusRepairInfo->wInfSize =
					(WORD)(dwLength - hLEng->stFStartInfo.dwFirstLandingOffset);
		}

		// verify the repair type...  If we have an EXE file type then
		// we must have EXE clean flags set.  Otherwise, we just assume
		// that the file is a COM file and must have COM clean flags set.

		if ((hLEng->stFStartInfo.wFileType == FSTART_EXE_FILE_TYPE &&
			 (dwRepairFlags & EXE_REPAIR_MASK)) ||
			(dwRepairFlags & COM_REPAIR_MASK))
		{
			// see if we're dealing with a virus at the TOF or at the EOF
			// Improved overwriters and Prepending viruses are at the TOF

			if ((dwRepairFlags & (CTRL_IMPROVED | CTRL_PREPENDING)) &&
				hLEng->stFStartInfo.wFileType != FSTART_EXE_FILE_TYPE)
			{
                // TOF

                dwOff = 0;
                wSlideDist = 1;
            }
            else
            {
                // EOF

                // if our virus size is > than our file size, we've got a bad
                // repair.  Return OK with *lpbRepairedFile = FALSE (default).

				if (dwLength < lpVirusRepairInfo->wInfSize)
                    return(ENGSTATUS_OK);

                // check to see if we want to slide for our bookmark...

                if (dwRepairFlags & CTRL_SLIDER)
                {
                    // if we add in the slide distance, check whether or not
                    // we go past the TOF.  If we do, then reduce the slide
                    // distance so we only slide from the TOF and not before
                    // the TOF.  This allows us to repair very small (<32 byte)
                    // COM files that would normally fail repair.

					if (dwLength <
                        (DWORD)(lpVirusRepairInfo->wInfSize + SLIDE_DISTANCE))
					{
                        wSlideDist = (WORD)(SLIDE_DISTANCE +
                                     dwLength - lpVirusRepairInfo->wInfSize);
                        lpVirusRepairInfo->wInfSize = (WORD)dwLength;
                    }
                    else
                    {
                        lpVirusRepairInfo->wInfSize += SLIDE_DISTANCE;
                        wSlideDist = SLIDE_DISTANCE * 2;
                    }
				}
				else
					wSlideDist = 1;		// don't slide

                // determine where we're going to read our 512 byte buffer
                // from for book-marking

				dwOff = dwLength - lpVirusRepairInfo->wInfSize;
            }

            // check if we're doing decryption or standard bookmarking

            if (dwRepairFlags & CTRL_DECRYPT)
            {
                // decrypting repair..  try to find memory signature and
                // decrypt proper stuff

                if (EngDecryptForRepair(lpFileObject,
                                        lpVirusRepairInfo,
										lpbyDecryptSig,
										&bDecrypted) == ENGSTATUS_ERROR)
                {
                    return(ENGSTATUS_ERROR);
                }

                // we found our encrypted string and decrypted the rest of the
                // virus.  Just perform a normal repair now...

				if (bDecrypted)
					bTryToRepair = TRUE;
            }
            else
            {
                // seek and read the proper information for standard bookmark
                // check.  The data is read into our temporary HLENG buffer

                if (AVFileSeek(lpFileObject->lpvInfo,
                       dwOff,
                       SEEK_SET,
                       &dwOff) == CBSTATUS_ERROR)
                    return(ENGSTATUS_ERROR);

                if (AVFileRead(lpFileObject->lpvInfo,
                       hLEng->byTempBuffer,
                       BOOK_MARK_BUFFER_SIZE,
                       &dwBytesRead) == CBSTATUS_ERROR)
                    return(ENGSTATUS_ERROR);

                // now look for the bookmark...  A bookmark offset of > 510
                // is definitely invalid (unless we're doing decryption).

                if (lpVirusRepairInfo->wBookLocation <
                    MAX_BOOK_OFFSET-sizeof(WORD))
                {
                    
                    // bookmark values are backwards!  Decode!

                    wBookCheck = (lpVirusRepairInfo->wBookMark >> 8) |
                                 (lpVirusRepairInfo->wBookMark << 8);


					for (wIndex = 0; wIndex < wSlideDist; wIndex++)
                    {
                        lpwBookValue = (LPWORD)(hLEng->byTempBuffer +
                                            lpVirusRepairInfo->wBookLocation +
											wIndex);
                        if (AVDEREF_WORD(lpwBookValue) == wBookCheck)
                        {
                            bTryToRepair = TRUE;
                            break;
                        }

                        // decrement the virus size by one and continue to
                        // slide for our bookmark

                        lpVirusRepairInfo->wInfSize--;
                    }
                }
                else if (lpVirusRepairInfo->wBookLocation &
                         BOOK_OFFSET_NO_CHECK_MASK)
                {
                    bTryToRepair = TRUE;
                }
            }

            if (bTryToRepair)
            {
                // whether or not we're successful, we're going to make
                // a concerted effort!  We've already decrypted by this time
				// so go ahead with the repair and cut

				bRepaired = FALSE;

				if (RepairPrimedFile(hLEng,
									 lpFileObject,
									 lpVirusRepairInfo,
									 &bRepaired) == ENGSTATUS_ERROR)
				{
					return(ENGSTATUS_ERROR);
				}

				// did we have a boo-boo during our repair attempt?
				// if so, return (*lpbRepairedFile == FALSE)

				if (!bRepaired)
                {
                    *lpbRepairTriedAndFailed = TRUE;

					return(ENGSTATUS_OK);
                }

				// now cut the virus out of the file...

				if (lpVirusRepairInfo->dwRepairFlags & CTRL2_CUT_BEGIN)
				{
					// cut from the first landing area

					dwOff = hLEng->stFStartInfo.dwFirstLandingOffset;
				}
				else
				{

					dwOff = dwLength -
                            lpVirusRepairInfo->wInfSize -
                            lpVirusRepairInfo->wTagSize;
                }

                // dwOff contains the offset where we want to truncate the file

                if (ZeroAndCut(lpFileObject,
                               dwOff,
                               dwLength,
                               lpbRepairedFile) ==
                    ENGSTATUS_ERROR)
                {
                    return(ENGSTATUS_ERROR);
                }

                return(ENGSTATUS_OK);
			}
        }
    }

    // whether we repaired or not, we didn't have any errors.

	return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  EngRepairFileAux()
//
// Parameters:
//  hLEng                   Handle to a local engine context
//  lpVirusInfo             Pointer to the virus information record
//  lpFileObject            A file object for the infected file
//  lpbFileRepaired         Set to TRUE on repair or FALSE on failure
//
// Description:
//
//  This function locates the repair record for the specified virus and
//  attempts to repair the infection.  If the virus was detected with an
//  ALG sig, then only one repair is attempted.  If the virus was detected
//  with a CRC sig, then all repairs with matching CRC values are applied
//  until the file is repaired or all repair attempts have failed. The calling
//  function must open the target file before calling this function.
//
// Returns:
//  ENGSTATUS_OK            On success
//  ENGSTATUS_ERROR         On error
//
//********************************************************************


ENGSTATUS EngRepairFileAux
(
	HLENG               hLEng,              // temporary buffers and callbacks
	LPVIRUS_INFO        lpVirusInfo,        // contains CRC/ALG info, etc...
	LPFILEOBJECT        lpFileObject,       // file object to repair
	LPBOOL              lpbFileRepaired     // OUT: was the file repaired?
)
{
	DATA_FILE_TABLE_T   stRepSec, stMemSec;
    WORD                wFirstRepairIndex, wCRC, wNumRepairCandidates;
	LPVOID              lpvFileInfo;
	VIRUS_REPAIR_T      stVirusRepairInfo;
	BYTE                byDecryptSig[DECRYPT_MEM_SIG_SIZE];
    BOOL                bRepairTriedAndFailed;

    // Assume we can't repair the file...

    bRepairTriedAndFailed = *lpbFileRepaired = FALSE;

	// determine which data file holds our repair data.  open it and get
	// the proper section information for our repair and memory decrypt
	// signatures

	if (EngLocateSections(hLEng->hGEng,
                              lpVirusInfo,
                              &lpvFileInfo,
                              &stRepSec,
                              &stMemSec,
                              NULL) != ENGSTATUS_OK)
    {
        // data file already closed inside of EngLocateSections

        return(ENGSTATUS_ERROR);
    }

    // at this point we know exactly where our sections are, how big they are,
    // etc.  Locate the first CRC repair we want to try...

    if (lpVirusInfo->wFlags & AVENGE_TYPE_CRC)
    {
        // If we're dealing with a CRC repair, locate and get the first repair

		if (GetCRCRepairInfo(hLEng->hGEng,
                             hLEng->hGEng->lpDataFileCallBacks,
                             lpvFileInfo,
                             lpVirusInfo,
							 &stRepSec,
							 &stMemSec,
                             &wFirstRepairIndex,
							 &wNumRepairCandidates) != ENGSTATUS_OK)
        {
            // close and exit on error

            EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

			return(ENGSTATUS_ERROR);
		}

        // are there any appropriate CRC repairs?  If not, return with
        // *lpbFileRepaired FALSE

        if (wNumRepairCandidates == 0)
        {
            EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

            return(ENGSTATUS_OK);
        }
	}
	else
	{
        if (!(lpVirusInfo->wFlags & AVENGE_TYPE_HAS_REPAIR))
        {
            EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

            // *lpbFileRepaired is FALSE at this point - no repair...

            return(ENGSTATUS_OK);
        }

		// If we're dealing with an ALG repair just get the indexed repair rec

		wFirstRepairIndex = lpVirusInfo->wRepairIndex;

        // only one repair attempt

        wNumRepairCandidates = 1;
	}

	// get the (first) matching repair record... (Both ALG and CRC)

	if (GetRepairRecordAux(hLEng->hGEng->lpDataFileCallBacks,
                               lpvFileInfo,
                               &stRepSec,
                               &stMemSec,
                               wFirstRepairIndex,
                               &stVirusRepairInfo,
                               byDecryptSig) != ENGSTATUS_OK)
	{
		// close and exit on error

		EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

		return(ENGSTATUS_ERROR);
	}

	// remember the CRC of the record we're using to repair

	wCRC = stVirusRepairInfo.wCRC;

	// Get the FSTART information for this file.  This helps us with
	// first landing (file cutting, etc)
	// always use the 0th entrypoint (only matters for SYS files, which we
	// don't repair anyway using this technique)

	if (GetFStart(lpFileObject,
				  0,
				  &hLEng->stAlgScanData.stFStartData,
				  &hLEng->stFStartInfo) == ENGSTATUS_ERROR)
	{
		EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

		return(ENGSTATUS_ERROR);
	}


	// store the file's date and time

    while (wNumRepairCandidates--)
	{
#ifdef SYM_NLM
        ThreadSwitch();
#endif

		// can we repair at all?

		if (AttemptRepair(hLEng,
						  lpFileObject,
						  &stVirusRepairInfo,
						  byDecryptSig,
                          lpbFileRepaired,
                          &bRepairTriedAndFailed) == ENGSTATUS_ERROR)
		{
			EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

			return(ENGSTATUS_ERROR);
		}

		// did we repair the file?

        if (*lpbFileRepaired == TRUE || bRepairTriedAndFailed)
			break;

		// if we were attempting to repair a virus that was detected by an
		// ALG sig, then we must return (don't look for other matching ALG
		// signatures)

        if (lpVirusInfo->wFlags & AVENGE_TYPE_ALG)
		{
                    break;
		}
		else
		{
			// CRC - get the next repair record

			if (GetRepairRecordAux( hLEng->hGEng->lpDataFileCallBacks,
						lpvFileInfo,
						&stRepSec,
						&stMemSec,
						++wFirstRepairIndex,
						&stVirusRepairInfo,
						byDecryptSig) != ENGSTATUS_OK)
			{
				EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

				return(ENGSTATUS_ERROR);
			}

			if (stVirusRepairInfo.wCRC != wCRC)
				break;
		}
	}

	EngCloseDataFile(hLEng->hGEng->lpDataFileCallBacks, lpvFileInfo);

	// restore the file's date and time

    return(ENGSTATUS_OK);
}

