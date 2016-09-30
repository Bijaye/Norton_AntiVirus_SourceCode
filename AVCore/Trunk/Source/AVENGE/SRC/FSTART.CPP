// Copyright 1996 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/AVENGE/VCS/FSTART.CPv   1.1   14 Oct 1998 11:51:30   MKEATIN  $
//
// Description:
//  Functions for filling the FSTART buffer before scanning.
//
// Contains:
//  GetFStart()
//
// See Also:
//
//************************************************************************
// $Log:   S:/AVENGE/VCS/FSTART.CPv  $
//
//    Rev 1.1   14 Oct 1998 11:51:30   MKEATIN
// Carey's ThreadSwitch additions for NLM.
//
//    Rev 1.0   18 Apr 1997 13:37:32   MKEATIN
// Initial revision.
//
//    Rev 1.3   12 Aug 1996 17:48:24   DCHI
// Modifications to correctly access WORDs from byte buffers.
//
//    Rev 1.2   29 May 1996 18:07:50   DCHI
// Changed lpvFileInfo to lpvInfo for file objects.
//
//    Rev 1.1   16 May 1996 14:12:46   CNACHEN
// Changed to use new AVENGE?.H headers.
//
//    Rev 1.0   13 May 1996 16:29:00   DCHI
// Initial revision.
//
//************************************************************************

#include "avengel.h"

//////////////////////////////////////////////////////////////////////////////
// static global data
//////////////////////////////////////////////////////////////////////////////

BYTE byFStartSkipBytes[] = {  0xF5,0xF8,0xF9,0xFB,0xFC,0x46,0x4B,0x5E,
                              0x90,0x42,0x92,0xFA,0xFD,0x9F,0x9E,0x58,
                              0x59,0x5A,0x5D,0x4D };

//////////////////////////////////////////////////////////////////////////////
// FSTART code follows
//////////////////////////////////////////////////////////////////////////////


//*************************************************************************
//
// Function:
//  SeekAndRead()
//
// Parameters:
//  lpFileObject        File object to read from
//  dwSegmentBase       Offset in file of base of current segment
//  wCurIP              Current IP
//  wIPSubConst         Used to adjust calculated offset
//  wBytesToRead        Number of bytes to read
//  lpbyBuffer          Buffer to read data
//  lpdwBytesRead       Pointer to DWORD to store number of bytes read
//  lpdwStartOffset     Pointer to DWORD to store offset in file
//                      of base of read data
//
// Description:
//  Used by FSTART during code tracing to read successive bytes
//  of code stream.  The function performs wraparound when
//  wCurIP + wBytesToRead crosses a segment boundary, assuming a
//  64K segment beginning at dwSegmentBase in the file.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS SeekAndRead
(
    LPFILEOBJECT    lpFileObject,
    DWORD           dwSegmentBase,
    WORD            wCurIP,
    WORD            wIPSubConst,
    WORD            wBytesToRead,
    LPBYTE          lpbyBuffer,
    LPDWORD         lpdwBytesRead,
    LPDWORD         lpdwStartOffset
)
{
    WORD            wAdjustedIP;
    DWORD           dwOffset;

#ifdef SYM_NLM
    ThreadSwitch();
#endif

    // first check to see if we're reading and need to wrap inside of the
    // segment (reading past IP=FFFF)

    if ((DWORD)wCurIP > (DWORD)(0x10000UL - wBytesToRead))
    {
        DWORD   dwBytesToRead, dwBytesRead1, dwBytesRead2, dwStartOffset1,
                dwStartOffset2;

        dwBytesToRead = 0x10000UL - wCurIP;

        if (SeekAndRead(lpFileObject,
                        dwSegmentBase,
                        wCurIP,
                        wIPSubConst,
                        (WORD)dwBytesToRead,
                        lpbyBuffer,
                        &dwBytesRead1,
                        &dwStartOffset1) != ENGSTATUS_OK)
            return(ENGSTATUS_ERROR);

        if (dwBytesRead1 != dwBytesToRead)
        {
            *lpdwBytesRead = dwBytesRead1;
			*lpdwStartOffset = dwStartOffset1;

            return(ENGSTATUS_OK);
        }

        dwBytesToRead = wBytesToRead - (WORD)(0x10000UL - wCurIP);

        if (SeekAndRead(lpFileObject,
                        dwSegmentBase,
                        0,              // wrap to start of segment
                        wIPSubConst,
                        (WORD)dwBytesToRead,
                        lpbyBuffer + (WORD)(0x10000UL - wCurIP),
                        &dwBytesRead2,
                        &dwStartOffset2) != ENGSTATUS_OK)
            return(ENGSTATUS_ERROR);

        *lpdwBytesRead = dwBytesRead1 + dwBytesRead2;
        *lpdwStartOffset = dwStartOffset1;

        return(ENGSTATUS_OK);
    }

    // zero out our buffer before reading data...

    MEMSET(lpbyBuffer,0,FSTART_BUFFER_SIZE);

    // check if we are trying to seek before the TOF in a COM file?

    if (wCurIP < wIPSubConst)
    {
        *lpdwBytesRead = 0;
        *lpdwStartOffset = 0;

        return(ENGSTATUS_OK);
    }

    // compute where we're going to read our data from.  make sure it falls
    // under 1MB (0xFFFFF).

    wAdjustedIP = wCurIP - wIPSubConst;
    dwOffset = (DWORD)(dwSegmentBase + wAdjustedIP) & FSTART_ADDRESS_MASK;

    if (AVFileSeek(lpFileObject->lpvInfo,
                   dwOffset,
                   SEEK_SET,
                   lpdwStartOffset) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);


    if (AVFileRead(lpFileObject->lpvInfo,
                   lpbyBuffer,
                   wBytesToRead,
                   lpdwBytesRead) == CBSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  FStartSetupEXE()
//
// Parameters:
//  lpFileObject        File object to read from
//  dwTOFBytesRead      Number of bytes in TOF buffer
//  lpdwBytesRead       Ptr to DWORD to store # bytes read into FStart buf
//  lpdwSegmentBase     Ptr to DWORD to store file offset of entry segment
//  lpwCurIP            Ptr to WORD to store beginning IP of EXE
//  lpwIPSub            Ptr to WORD to store IP subtraction constant
//  lpbSetFirst         Ptr to BOOL to store first landing status
//  lpFStartData        Ptr to FStartData structure
//  lpFStartInfo        Ptr to FStartInfo structure
//
// Description:
//  Initializes FStart data assuming an EXE file.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS FStartSetupEXE
(
    LPFILEOBJECT    lpFileObject,
    DWORD           dwTOFBytesRead,
    LPDWORD         lpdwBytesRead,
    LPDWORD         lpdwSegmentBase,
    LPWORD          lpwCurIP,
    LPWORD          lpwIPSub,
    LPBOOL          lpbSetFirst,
    LPFSTARTDATA    lpFStartData,
    LPFSTARTINFO    lpFStartInfo
)
{
    LPEXEHEADER     lpEXEHeaderPtr;
    DWORD           dwTemp;

    if (dwTOFBytesRead < EXE_HEADER_SIZE)
    {
        // the file is too short and doesn't even contain the full EXE header

        lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

        return(ENGSTATUS_OK);
    }

	lpEXEHeaderPtr = (LPEXEHEADER)lpFStartData->byTOFBuffer;

    // we have a complete header, so compute our segment base and entry-
    // point

    *lpdwSegmentBase = (DWORD)((WORD)(WENDIAN(lpEXEHeaderPtr->cs) +
                                      WENDIAN(lpEXEHeaderPtr->header_size))) << 4;

    // set the IP

    *lpwCurIP = WENDIAN(lpEXEHeaderPtr->ip);

    // no machinations with EXE files (no wIPSub)

    *lpwIPSub = 0;

    // read in the entry-point code

    if (SeekAndRead(lpFileObject,
                    *lpdwSegmentBase,
                    *lpwCurIP,
                    *lpwIPSub,
                    FSTART_BUFFER_SIZE,
                    lpFStartData->byFStartBuffer,
                    lpdwBytesRead,
                    &dwTemp) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    // did we get anything?

    if (*lpdwBytesRead == 0)
    {
        // no!

        lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

        return(ENGSTATUS_OK);
    }

    // set our file type to EXE

    lpFStartInfo->wFileType = FSTART_EXE_FILE_TYPE;

    // in EXE files, our first landing offset is right at the entrypoint
    // also set up our buffer offset

    lpFStartInfo->dwFirstLandingOffset = dwTemp;
	lpFStartInfo->dwFStartBufferOffset = dwTemp;

    // we have set the first landing for the EXE, so there's no need to
    // do it again later. (we'll only do it for COM files later)

    *lpbSetFirst = TRUE;

    // remember how many bytes are in our fstart buffer

    lpFStartInfo->wFStartBufferLen = (WORD)(*lpdwBytesRead);

    return(ENGSTATUS_OK);
}


//*************************************************************************
//
// Function:
//  FStartSetupSYS()
//
// Parameters:
//  lpFileObject        File object to read from
//  wEntryPointNum      Entry point to setup
//  dwTOFBytesRead      Number of bytes in TOF buffer
//  lpdwBytesRead       Ptr to DWORD to store # bytes read into FStart buf
//  lpdwSegmentBase     Ptr to DWORD to store file offset of entry segment
//  lpwCurIP            Ptr to WORD to store beginning IP of EXE
//  lpwIPSub            Ptr to WORD to store IP subtraction constant
//  lpbSetFirst         Ptr to BOOL to store first landing status
//  lpFStartData        Ptr to FStartData structure
//  lpFStartInfo        Ptr to FStartInfo structure
//
// Description:
//  Initializes FStart data assuming a SYS file.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS FStartSetupSYS
(
    LPFILEOBJECT    lpFileObject,
    WORD            wEntryPointNum,
    DWORD           dwTOFBytesRead,
    LPDWORD         lpdwBytesRead,
    LPDWORD         lpdwSegmentBase,
    LPWORD          lpwCurIP,
    LPWORD          lpwIPSub,
    LPBOOL          lpbSetFirst,
    LPFSTARTDATA    lpFStartData,
    LPFSTARTINFO    lpFStartInfo
)
{
    LPSYSHEADER     lpSYSHeaderPtr;
    DWORD           dwTemp;


    // we have a SYS file.  NOTE: some SYS files do not start with two
    // FFFF WORDS at the TOF.  These files won't be scanned for SYS
    // infections.

    if (dwTOFBytesRead < SYS_HEADER_SIZE)
    {
        // the file is too short and doesn't even contain the full SYS header

        lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

        return(ENGSTATUS_OK);
    }

	lpSYSHeaderPtr = (LPSYSHEADER)lpFStartData->byTOFBuffer;

	*lpdwSegmentBase = 0;              // only one segment in SYS files

	if (wEntryPointNum == STRATEGY_ENTRY)
	{
		*lpwCurIP = WENDIAN(lpSYSHeaderPtr->strategy_ip);
		lpFStartInfo->wEntryPointNum = STRATEGY_ENTRY;
	}
	else
	{
		*lpwCurIP = WENDIAN(lpSYSHeaderPtr->int_ip);
		lpFStartInfo->wEntryPointNum = INTERRUPT_ENTRY;
	}

    *lpwIPSub = 0;                     // no machinations with EXE files

    // read in the entry-point code

    if (SeekAndRead(lpFileObject,
                    *lpdwSegmentBase,
                    *lpwCurIP,
                    *lpwIPSub,
                    FSTART_BUFFER_SIZE,
                    lpFStartData->byFStartBuffer,
                    lpdwBytesRead,
                    &dwTemp) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    lpFStartInfo->wFileType = FSTART_SYS_FILE_TYPE;

    // in SYS files, our first landing offset is right at the entrypoint
    // also set up our buffer offset

    lpFStartInfo->dwFirstLandingOffset = dwTemp;
    lpFStartInfo->dwFStartBufferOffset = dwTemp;

    // we have set the first landing for the SYS, so there's no need to
    // do it again later. (we'll only do it for COM files later)

    *lpbSetFirst = TRUE;

    // remember how many bytes are in our fstart buffer

    lpFStartInfo->wFStartBufferLen = (WORD)(*lpdwBytesRead);

    return(ENGSTATUS_OK);

}


//*************************************************************************
//
// Function:
//  FStartSetupCOM()
//
// Parameters:
//  dwBytesRead         DWORD containing # bytes read into FStart buf
//  lpdwSegmentBase     Ptr to DWORD to store file offset of entry segment
//  lpwCurIP            Ptr to WORD to store beginning IP of EXE
//  lpwIPSub            Ptr to WORD to store IP subtraction constant
//  lpbSetFirst         Ptr to BOOL to store first landing status
//  lpFStartInfo        Ptr to FStartInfo structure
//
// Description:
//  Initializes FStart data assuming a COM file.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//*************************************************************************

ENGSTATUS FStartSetupCOM
(
    DWORD           dwBytesRead,
    LPDWORD         lpdwSegmentBase,
    LPWORD          lpwCurIP,
    LPWORD          lpwIPSub,
    LPBOOL          lpbSetFirst,
    LPFSTARTINFO    lpFStartInfo
)
{

	// dealing with a COM file...

	*lpdwSegmentBase = 0;              // only one segment in COM files

	*lpwCurIP = COM_INITIAL_IP;        // execution starts at CS:100
	*lpwIPSub = COM_IP_SUB_CONST;      // CS:100 corresponds to TOF+0

	// set our type to COM.  note that we don't need to re-read anything
    // yet, since we've already got the entry-code in our buffer.
    // our buffer offset is 0

    // we still need to establish the first landing...

    *lpbSetFirst = FALSE;

    // remember how many bytes are in our fstart buffer (dwBytesRead is
    // set by the first SeekAndRead call)

    lpFStartInfo->wFStartBufferLen = (WORD)dwBytesRead;

    // set up our buffer and first landing offsets.

    lpFStartInfo->dwFirstLandingOffset = 0;
    lpFStartInfo->dwFStartBufferOffset = 0;

	// bulgarian and verified COM files have been killed.

    lpFStartInfo->wFileType = FSTART_COM_FILE_TYPE;

    return(ENGSTATUS_OK);
}


//********************************************************************
//
// Function:
//  GetFStart()
//
// Parameters:
//  lpFileObject        File object of which to get the FSTART
//  wEntryPointNum      Entry point number to get
//  lpFStartData        Pointer to FStartData structure
//  lpFStartInfo        Pointer to FStartInfo structure
//
// Description:
//  Fstart.
//
// Returns:
//  ENGSTATUS_ERROR On error
//  ENGSTATUS_OK    On success
//
//********************************************************************

// Notes:
//
//  1. Whale detection will have to be done via NAVEX.
//  2. V2P6Z detection will have to be done via NAVEX/PAM.
//

ENGSTATUS GetFStart
(
    LPFILEOBJECT    lpFileObject,
    WORD            wEntryPointNum,
    LPFSTARTDATA    lpFStartData,
    LPFSTARTINFO    lpFStartInfo
)
{
    WORD            wDepth, wByteInBuffer, wCurIP, wIPSub, wMaxPad, i;
    BOOL            bSetFirst, bFollowJump;
    DWORD           dwTemp, dwBytesRead, dwSegmentBase;
    LPBYTE          lpbyFStartPtr;
    LPSYSHEADER     lpSYSHeaderPtr;
    LPEXEHEADER     lpEXEHeaderPtr;

    wDepth = 0;                         // our JMP following depth = 0

    // zero out all of our fstart data so we have no false ID's...

	MEMSET(lpFStartData,0,sizeof(FSTARTDATA_T));

    // seek to the TOF so we can start reading.

	if (SeekAndRead(lpFileObject,
                    0,                  // segment start = 0
                    0,                  // ip = 0
                    0,                  // ip sub const = 0
                    FSTART_BUFFER_SIZE,
                    lpFStartData->byFStartBuffer,
                    &dwBytesRead,
                    &dwTemp) == ENGSTATUS_ERROR)
        return(ENGSTATUS_ERROR);

    if (dwBytesRead == 0)
    {
        // the file is 0 bytes long.  it could not contain a virus.
        // lpFStartInfo->wFileType is set to indicate that no virus infection
        // is possible.

        lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

        return(ENGSTATUS_OK);
    }

    // store the first 256 bytes of the file into our byTOFBuffer.
    // this is used by the ScanHeader function during ALG scanning

    MEMCPY(lpFStartData->byTOFBuffer,
		   lpFStartData->byFStartBuffer,
           FSTART_BUFFER_SIZE);

	// check if we have an EXE file (no worries about BIG/LITTLE engine stuff)

	lpEXEHeaderPtr = (LPEXEHEADER) lpFStartData->byTOFBuffer;
	lpSYSHeaderPtr = (LPSYSHEADER) lpFStartData->byTOFBuffer;


    if (lpEXEHeaderPtr->exesig == EXE_SIG1 ||
        lpEXEHeaderPtr->exesig == EXE_SIG2)
    {
        if (FStartSetupEXE(lpFileObject,
                           dwBytesRead,     // initial number of TOF bytes read
                           &dwBytesRead,    // bytes read from entry-point
                           &dwSegmentBase,
                           &wCurIP,
                           &wIPSub,
                           &bSetFirst,
                           lpFStartData,
                           lpFStartInfo) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

        if (lpFStartInfo->wFileType == FSTART_INVALID_FILE_TYPE)
            return(ENGSTATUS_OK);
    }
    else if (lpSYSHeaderPtr->offset == SYS_SIG &&
             lpSYSHeaderPtr->segment == SYS_SIG)
    {
		if (FStartSetupSYS(lpFileObject,
						   wEntryPointNum,
                           dwBytesRead,     // initial number of TOF bytes read
                           &dwBytesRead,    // bytes read from entry-point
                           &dwSegmentBase,
                           &wCurIP,
                           &wIPSub,
                           &bSetFirst,
                           lpFStartData,
                           lpFStartInfo) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);

		if (lpFStartInfo->wFileType == FSTART_INVALID_FILE_TYPE)
            return(ENGSTATUS_OK);
    }
    else
    {
		if (FStartSetupCOM(dwBytesRead,
						   &dwSegmentBase,
						   &wCurIP,
						   &wIPSub,
						   &bSetFirst,
                           lpFStartInfo) == ENGSTATUS_ERROR)
            return(ENGSTATUS_ERROR);
    }

    // now, our lpFStartData->byFStartBuffer contains the appropriate entry
    // code and we can start jumping around.  in addition, all of our
    // offsets are set up too (dwSegmentBase, wCurIP and wIPSub)
    // bSetFirst is set to FALSE for COM files and TRUE for EXE/SYS files
	// since we've located the entry-point already for EXE/SYS files...

	lpbyFStartPtr = lpFStartData->byFStartBuffer;

	while (TRUE)
	{
#ifdef SYM_NLM
        ThreadSwitch();
#endif

		// skip over "skip bytes".  This may be optimized later, but it is
		// probably not necessary.  Most "normal" programs don't use these
		// opcodes at their entry-point, so we're probably only going to
		// do the inner "j" loop just once.

		// at this point, we will always be pointing to the start of the
		// fstart buffer...

        if (dwBytesRead > FSTART_MAX_PAD_BYTES)
            wMaxPad = FSTART_MAX_PAD_BYTES;
        else
            wMaxPad = (WORD)dwBytesRead;

		// note: wByteInBuffer is always set to 0 at this point.

		for (wByteInBuffer=0;wByteInBuffer<wMaxPad;wByteInBuffer++)
        {
            for (i=0;i<sizeof(byFStartSkipBytes);i++)
				if (lpbyFStartPtr[wByteInBuffer] == byFStartSkipBytes[i])
                    break;
            if (i == sizeof(byFStartSkipBytes))
                break;

            wCurIP++;
        }

        // Now we're past our skip bytes:
        //
        // Case 1: We skipped over < 64 bytes and found a non skip-byte.
        // Case 2: We skipped over exactly 64 skip-byte bytes.
        // Case 3: We skipped over < 64 bytes and then hit the EOF.
        //
        // Let's look for JMPs and stuff...

		if (wByteInBuffer == dwBytesRead)
        {
            // we hit the EOF, this is an invalid file!

            lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

            return(ENGSTATUS_OK);
		}

        // assume we're not following a jump

        bFollowJump = FALSE;

		if (lpbyFStartPtr[wByteInBuffer] == JUMP_TWO_BYTE ||
			lpbyFStartPtr[wByteInBuffer] == CALL_TWO_BYTE)
        {
            LPWORD lpwPtr;

            // make sure the destination of our JMP/CALL is within our
            // fstart buffer.  Otherwise we will grab random data which
            // is bad.

            if ((DWORD)(wByteInBuffer + JUMP_TWO_BYTE_SIZE) > dwBytesRead)
            {
                lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

                return(ENGSTATUS_OK);
            }

			lpwPtr = (LPWORD)(lpbyFStartPtr+wByteInBuffer+1);

            // update our wCurIP so we can re-read a new fstart buffer...

            wCurIP += AVDEREF_WORD(lpwPtr) + JUMP_TWO_BYTE_SIZE;

            // remember that we're following a JUMP...

            bFollowJump = TRUE;
        }
		else if (lpbyFStartPtr[wByteInBuffer] == JUMP_ONE_BYTE)
        {
			// make sure the destination of our JMP is within our  fstart
            // buffer.  Otherwise we will grab random data which
            // is bad.

            if ((DWORD)(wByteInBuffer + JUMP_ONE_BYTE_SIZE) > dwBytesRead)
            {
                lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

                return(ENGSTATUS_OK);
            }

            // update our wCurIP so we can re-read a new fstart buffer...

			wCurIP += SignExtendByte(lpbyFStartPtr[wByteInBuffer+1]) +
                      JUMP_ONE_BYTE_SIZE;

            // remember that we're following a JUMP...

            bFollowJump = TRUE;
        }
        else if (lpFStartInfo->wFileType == FSTART_COM_FILE_TYPE)
        {
            // try to find the following machine code:
            //
            // B? ZZ ZZ     MOV ?X, ZZZZ
            // 5?           PUSH ?X
            // C3           RET
            //
            // or:
            //
            // B? ZZ ZZ     MOV ?X, ZZZZ
            // 5?           PUSH ?X
            // FA           CLI
            // C3           RET

            if (((DWORD)(wByteInBuffer + JUMP_PUSH_RET_SIZE1) <= dwBytesRead &&
				 lpbyFStartPtr[wByteInBuffer + 4] == RET_INSTRUCTION) ||
                ((DWORD)(wByteInBuffer + JUMP_PUSH_RET_SIZE2) <= dwBytesRead &&
				 lpbyFStartPtr[wByteInBuffer + 4] == CLI_INSTRUCTION &&
				 lpbyFStartPtr[wByteInBuffer + 5] == RET_INSTRUCTION))
            {
                // it looks like we've found one of the above code sequences...
                // verify that our MOV and our PUSH use the same register...
                // the lower 3 bits of each opcode specify the register.
                //
                //      MOV REG, IMMED = 1011 1???
                //      PUSH REG       = 0101 0???
                //      --------------   ---------
                //      XORED result     1110 1000      (==0xE8)
                //
                // the lower 3 bits will be 0 if they specify the same register
                // in both instructions.

				if ((BYTE)(lpbyFStartPtr[wByteInBuffer] ^
						   lpbyFStartPtr[wByteInBuffer+3]) == 0xE8)
                {
                    // now verify that we have a PUSH instruction at offset
                    // IP+3 (right after the MOV)

					if ((BYTE)(lpbyFStartPtr[wByteInBuffer+3] & 0xF8) == 0x50)
                    {
                        LPWORD lpwPtr;

                        // its a PUSH instruction.  Locate the destination of
                        // the push-ret and jump there!  The destination of the
                        // push-ret is at offset IP+1, right after the MOV
                        // opcode...

						lpwPtr = (LPWORD)(lpbyFStartPtr +
										  wByteInBuffer+1);

                        // update our wCurIP so we can re-read a new fstart
                        // buffer...  This is a

                        wCurIP = AVDEREF_WORD(lpwPtr);

                        // remember that we're following a JUMP...

                        bFollowJump = TRUE;
                    }
                }
            }
        }

        if (bFollowJump == TRUE)
        {
            // jump one more level down and see if we've been going too long...

            wDepth++;

            if (wDepth == FSTART_MAX_DEPTH)
            {
                lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

                return(ENGSTATUS_OK);
            }

            // now read in our new buffer!

            if (SeekAndRead(lpFileObject,
                            dwSegmentBase,
                            wCurIP,
							wIPSub,
                            FSTART_BUFFER_SIZE,
                            lpFStartData->byFStartBuffer,
                            &dwBytesRead,
                            &lpFStartInfo->dwFStartBufferOffset) == ENGSTATUS_ERROR)
                return(ENGSTATUS_ERROR);

            // if we got 0 bytes back, we've got a problem...

            if (dwBytesRead == 0)
            {
                lpFStartInfo->wFileType = FSTART_INVALID_FILE_TYPE;

                return(ENGSTATUS_OK);
            }

            // see if we've set our first landing yet... if not, do so!

            if (bSetFirst == FALSE)
            {
                bSetFirst = TRUE;

				lpFStartInfo->dwFirstLandingOffset =
					lpFStartInfo->dwFStartBufferOffset;
            }
        }
        else        // bFollowJump == FALSE
        {
            // check if we're right at the top of our buffer.  if we are, our
            // data is all ready to send back. do so!  dwBytesRead contains
            // the proper value so we'll just place this into our
            // wFStartBufferLen variable

            if (wByteInBuffer == 0)
            {
				lpFStartInfo->wFStartBufferLen = (WORD)dwBytesRead;
				lpFStartInfo->dwSegmentBase = dwSegmentBase;

                return(ENGSTATUS_OK);
            }
            else
            {
                // we're not at the start of the fstart buffer, so lets read
                // 256 bytes from our current IP (refilling our fstart
                // buffer) and then return.

                if (SeekAndRead(lpFileObject,
                            dwSegmentBase,
                            wCurIP,
							wIPSub,
                            FSTART_BUFFER_SIZE,
                            lpFStartData->byFStartBuffer,
							&dwBytesRead,
                            &lpFStartInfo->dwFStartBufferOffset) == ENGSTATUS_ERROR)
                    return(ENGSTATUS_ERROR);

                lpFStartInfo->wFStartBufferLen = (WORD)dwBytesRead;
                lpFStartInfo->dwSegmentBase = dwSegmentBase;

                return(ENGSTATUS_OK);
            }
        }
    }
}

