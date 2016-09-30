// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/repair.cpv   1.19   12 Aug 1996 22:13:54   JMILLAR  $
//
// Description:
//
//  This source file contains support routines for the PAM repair system.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/repair.cpv  $
// 
//    Rev 1.19   12 Aug 1996 22:13:54   JMILLAR
// include avendian.h to provide for an invoke of WENDIAN - this 
// breaks in the build - I don't know whether the avenge build
// include this indirectly?
// 
//    Rev 1.18   08 Aug 1996 13:07:44   CNACHEN
// Added a few endian changes and updated to use the latest ERS debugger code.
// Should all be ifdeffed out, so no worries.
// 
//    Rev 1.16   05 Mar 1996 16:40:54   CNACHEN
// Fixed bug in copy_bytes_to_vm.  If a read is done on a file (from ERS) where
// the fileptr is N bytes from the EOF, and the read tries to load N+1 or more
// bytes, an error was reported.  Now, the file read just reports that it read
// N bytes and does not cause an error.
// 
// 
//    Rev 1.15   02 Feb 1996 11:45:02   CNACHEN
// Added new dwFlag and exclusion checking...
// 
//    Rev 1.14   24 Jan 1996 13:35:44   DCHI
// Typecast fix.
// 
//    Rev 1.13   23 Jan 1996 14:49:12   DCHI
// Properly deal with fault suspension.
// 
//    Rev 1.12   22 Jan 1996 13:39:48   CNACHEN
// Added new fault support.
// 
//    Rev 1.11   19 Dec 1995 19:08:06   CNACHEN
// No change.
// 
//    Rev 1.10   15 Dec 1995 18:59:34   CNACHEN
// low memory can now be read in during global init so we don't need open file
// handles...
// 
//    Rev 1.9   14 Dec 1995 13:02:48   CNACHEN
// Added support for control relinquishing under NLM...
// 
//    Rev 1.8   14 Dec 1995 10:49:28   CNACHEN
// Fixed repair stuff...
// 
//    Rev 1.7   13 Dec 1995 11:58:10   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
// 
//    Rev 1.6   19 Oct 1995 18:23:30   CNACHEN
// Initial revision... with comment header :)
// 
//************************************************************************


#include "pamapi.h"
#include "ident.h"
#include "avendian.h"

///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)

#include <process.h>
#include <ctype.h>
#include <conio.h>
#include <stdlib.h>

#endif
///////////////////////////////////////////////////////////////////////////////

// protos from EXEC.CPP

WORD check_for_immunity(PAMLHANDLE hLocal);


WORD found_repair_sig(PAMLHANDLE        hLocal,
		      BYTE              byOpcode,
		      sig_list_type     *lpSigList,
		      WORD              *lpwSigNum)
{
	BYTE    byInstrStream[REPAIR_SIG_LEN];
	WORD    wSigByte;
	WORD    i, j, wNumSigs, wReadIn, wSigLen;

	wNumSigs = lpSigList->repair_bundle_info.wNumSigs;
	wReadIn = FALSE;                    // cache our reads from the instruction
										// stream
	for (i=0;i<wNumSigs;i++)
	{
		if (byOpcode == lpSigList->repair_sig[i].wSig[0])
		{
			if (FALSE == wReadIn)
			{
				for (j=1;j<REPAIR_SIG_LEN;j++)
					byInstrStream[j] = get_byte(hLocal,
												hLocal->CPU.CS,
						(WORD)(hLocal->CPU.IP + j - 1));

				wReadIn = TRUE;
			}

			wSigLen = lpSigList->repair_sig[i].wSigLen;

			for (j=1;j<wSigLen;j++)
			{
				wSigByte = lpSigList->repair_sig[i].wSig[j];

				if (byInstrStream[j] != wSigByte &&
					wSigByte != REPAIR_SIG_WILDCARD)
					break;
	    }

			if (j == wSigLen)           // got a signature hit!
			{
				*lpwSigNum = i;
				return(TRUE);
			}

		}
	}

	return(FALSE);
}


///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)

void Unassemble(PAMLHANDLE                      hLocal,
				WORD                            wSegment,
				WORD                            wOffset,
				LPWORD                          lpwNextOffset)
{
	WORD                            i;
	BYTE                            byData[128];
	char                            szLine[128];
	FILE                            *stream;
	int                                                             nLines;

	for (i=0;i<128;i++)
		byData[i] = get_byte(hLocal,wSegment,wOffset + i);

	stream = fopen("PAMDBG.DAT","wb");
	if (stream == NULL)
	{
		printf("ERS: Error creating PAMDBG.DAT file\n");
		return;
	}

	fseek(stream,wOffset,SEEK_SET);
	fwrite(byData,1,128,stream);
	fclose(stream);

	stream = fopen("PAMDBG.IN","wt");
	if (stream == NULL)
	{
		printf("ERS: Error creating PAMDBG.IN file\n");
		return;
	}

	fprintf(stream,"n PAMDBG.DAT\rl 0\ru %04X l 100\rq\r",wOffset);
	fclose(stream);

	system("debug < PAMDBG.IN > PAMDBG.OUT");

	stream = fopen("PAMDBG.OUT","rt");
	if (stream == NULL)
	{
		printf("ERS: Error creating PAMDBG.OUT file\n");
		return;
	}

	szLine[0] = 0;
	nLines = 16;

	printf("\n");

	fgets(szLine,127,stream);
	fgets(szLine,127,stream);
	fgets(szLine,127,stream);

	while (!feof(stream) && nLines--)
	{
		if (fgets(szLine,127,stream) && strlen(szLine) > 8)
		{
			char            szTemp[128];

			sprintf(szTemp,"%04X",wSegment);

			szLine[0] = szTemp[0];
			szLine[1] = szTemp[1];
			szLine[2] = szTemp[2];
			szLine[3] = szTemp[3];

			if (strlen(szLine) > 8)
				printf("%s",szLine);

		}
	}

	if (fgets(szLine,127,stream) && strlen(szLine) > 8)
		sscanf(strchr(szLine,':')+1,"%04X",lpwNextOffset);
	else
		*lpwNextOffset = 0;

	fclose(stream);
}


void DisplayDump(PAMLHANDLE hLocal,WORD wSeg,WORD wOff)
{
	BYTE                            byData[128];
	int                                                             i;

	for (i=0;i<16;i++)
		byData[i] = get_byte(hLocal,wSeg,wOff+i);

	printf("%04X:%04X ",wSeg,wOff);

	for (i=0;i<16;i++)
		printf("%02X ",byData[i]);

	printf(" ");

	for (i=0;i<16;i++)
		if (isprint(byData[i]))
			printf("%c",byData[i]);
		else
			printf(".");
	printf("\n");
}

void DisplayDebugInfo(PAMLHANDLE    hLocal)
{
	WORD                            i, wUCS, wUIP;
	BYTE                            byData[128];
	char                            szLine[128], szFlags[32], szCS[16],
									szCommand[128];
	FILE                            *stream;
	static long                     nTraceStepsLeft = 0;
	WORD                            wSeg, wOff;
	static WORD                     wLastSeg = 0xF000, wLastOff = 0xFFF0;
	static DWORD                                    dwGoOffset = (DWORD)-1;

	if (hLocal->dwFlags & LOCAL_FLAG_REPAIR_DECRYPT)
		return;

	if (nTraceStepsLeft > 0)
		nTraceStepsLeft--;

	if (dwGoOffset == hLocal->CPU.IP)
		nTraceStepsLeft = 0;

	if (nTraceStepsLeft == 0)
	{
		for (i=0;i<128;i++)
			byData[i] = get_byte(hLocal,hLocal->CPU.CS,hLocal->CPU.IP + i);

		stream = fopen("PAMDBG.DAT","wb");
		if (stream == NULL)
		{
			printf("ERS: Error creating PAMDBG.DAT file\n");
			return;
		}

		fseek(stream,hLocal->CPU.IP,SEEK_SET);
	fwrite(byData,1,32,stream);
	fclose(stream);

	stream = fopen("PAMDBG.IN","wt");
		if (stream == NULL)
	{
	    printf("ERS: Error creating PAMDBG.IN file\n");
	    return;
	}

		fprintf(stream,"n PAMDBG.DAT\rl 0\ru %04X l 1\rq\r",hLocal->CPU.IP);
		fclose(stream);

		system("debug PAMDBG.DAT < PAMDBG.IN > PAMDBG.OUT");

		stream = fopen("PAMDBG.OUT","rt");
	if (stream == NULL)
	{
	    printf("ERS: Error creating PAMDBG.OUT file\n");
			return;
	}

		szLine[0] = 0;
		fgets(szLine,127,stream);
		fgets(szLine,127,stream);
		fgets(szLine,127,stream);
		fgets(szLine,127,stream);

		fclose(stream);

		// display our stuff...

		strcpy(szFlags,"                    ");
		if (hLocal->CPU.FLAGS.C)
			szFlags[1] = 'C';

		if (hLocal->CPU.FLAGS.A)
			szFlags[3] = 'A';

		if (hLocal->CPU.FLAGS.O)
			szFlags[5] = 'O';

		if (hLocal->CPU.FLAGS.Z)
	    szFlags[7] = 'Z';

	if (hLocal->CPU.FLAGS.S)
			szFlags[9] = 'S';

	if (hLocal->CPU.FLAGS.D)
	    szFlags[11] = 'D';

		szFlags[18] = 0;

display_registers:

		printf("\n");

	printf("EAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX\n",
			  hLocal->CPU.preg.D.EAX,
			  hLocal->CPU.preg.D.EBX,
			  hLocal->CPU.preg.D.ECX,
			  hLocal->CPU.preg.D.EDX);
		printf("ESI=%08lX EDI=%08lX CS=%04X DS=%04X ES=%04X SS=%04X\n",
			  hLocal->CPU.ireg.D.ESI,
			  hLocal->CPU.ireg.D.EDI,
			  hLocal->CPU.CS,
			  hLocal->CPU.DS,
			  hLocal->CPU.ES,
			  hLocal->CPU.SS);

		printf("EBP=%08lX ESP=%08lX  Flags=[%s]\n",
			  hLocal->CPU.ireg.D.EBP,
			  hLocal->CPU.ireg.D.ESP,
			  szFlags);

		sprintf(szCS,"%04X",hLocal->CPU.CS);

		szLine[0] = szCS[0];
		szLine[1] = szCS[1];
		szLine[2] = szCS[2];
		szLine[3] = szCS[3];

		printf("%s",szLine);

		wUCS = hLocal->CPU.CS;
		wUIP = hLocal->CPU.IP;

		do
		{
			printf("\nERS Debugger (G XXXX, D XXXX:XXXX, R, T NNNN, U XXXX:XXXX): ");
			gets(szCommand);

			switch(toupper(szCommand[0]))
			{
				case 'U':

					if (strchr(szCommand,':'))
					{
						sscanf(strchr(szCommand,' ')+1,"%04X",&wSeg);
						sscanf(strchr(szCommand,':')+1,"%04X",&wOff);
					}
					else if (strchr(szCommand,' '))
					{
						wSeg = hLocal->CPU.CS;
						sscanf(strchr(szCommand,' ')+1,"%04X",&wOff);
					}
					else
					{
						wSeg = wUCS;
						wOff = wUIP;
					}

					wUCS = wSeg;

					Unassemble(hLocal,wSeg,wOff,&wUIP);
					break;
				case 'R':
					goto display_registers;
				case 'G':
					sscanf(strchr(szCommand,' ')+1,"%04lX",&dwGoOffset);
					nTraceStepsLeft = 1000000L;          // a large number
					break;
				case 'T':
					if (strlen(szCommand) > 1)
					{
						sscanf(strchr(szCommand,' ')+1,"%ld",&nTraceStepsLeft);
					}
					else
						nTraceStepsLeft = 0;
					break;
				case 'D':
					if (strchr(szCommand,':'))
					{
						sscanf(strchr(szCommand,' ')+1,"%04X",&wSeg);
						sscanf(strchr(szCommand,':')+1,"%04X",&wOff);
					}
					else if (strchr(szCommand,' '))
					{
						wSeg = hLocal->CPU.DS;
						sscanf(strchr(szCommand,' ')+1,"%04X",&wOff);
					}
					else
					{
						wSeg = wLastSeg;
						wOff = wLastOff + 8*16;
					}

					wLastSeg = wSeg;
					wLastOff = wOff;

					printf("\n");

					for (i=0;i<8;i++)
					{
						DisplayDump(hLocal,wSeg,wOff);
						wOff += 16;
					}
					break;
			}
		} while (toupper(szCommand[0]) == 'D' ||
				 toupper(szCommand[0]) == 'U');
	}
	else
	{
		if (kbhit())
		{
			printf("\n\nERS:Execution interrupted\n");
			nTraceStepsLeft = 0;                    // any key to stop GO...
		}
	}

	unlink("PAMDBG.DAT");
	unlink("PAMDBG.IN");
	unlink("PAMDBG.OUT");
}

#endif
///////////////////////////////////////////////////////////////////////////////




// returns TRUE if we hit one of our sigs

WORD  repair_interpret( PAMLHANDLE          hLocal,
						sig_list_type       *lpSigList,
						WORD                *lpwSigNum)
{
	BYTE            byOpcode;
	BYTE            immunity_opcode;
	PAMConfigType   *pConfig;
	WORD            wNum;

    hLocal->CPU.iteration = 0;

    // what is our max iterations?

	if (hLocal->dwFlags & LOCAL_FLAG_REPAIR_DECRYPT)
		hLocal->CPU.max_iteration = lpSigList->repair_bundle_info.dwMaxIter;
	else // REPAIR_ERS
		hLocal->CPU.max_iteration =
			lpSigList->repair_bundle_info.dwMaxRepairProgIter;


	hLocal->CPU.max_immune_iteration =
		hLocal->hGPAM->config_info.ulMaxImmuneIter;

	*lpwSigNum = NOT_FOUND;      // no entry signature has been located

	pConfig = &(hLocal->hGPAM->config_info);
	immunity_opcode = pConfig->byImmuneSig[0];

	for (;;)
	{
	if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
			return(FALSE);          // error accessing file


///////////////////////////////////////////////////////////////////////////////
#if defined(BORLAND) && defined(REPAIR_DEBUG)
    {
	extern  BOOL    gbDebugRepair;

	if (gbDebugRepair == TRUE &&
	    !(hLocal->dwFlags & LOCAL_FLAG_REPAIR_DECRYPT))
	{
	    DisplayDebugInfo(hLocal);
	}
    }
#endif
///////////////////////////////////////////////////////////////////////////////


	byOpcode = get_byte_at_csip(hLocal);

	////////////////////////////////////////////////////////////////
		// at this point, see if we need to instruction fault
		// don't fault if we're in no-exclude mode
		// also don't fault if we are in no-fault mode (set by an ARPL)
		// prefix code sequence
		////////////////////////////////////////////////////////////////

	if ((hLocal->dwFlags & LOCAL_FLAG_REPAIR_DECRYPT) &&
	    (wNum = pConfig->byInstrFaultTable[byOpcode]) != 0xFF &&
	    hLocal->CPU.iteration >= pConfig->dwFaultIterArray[wNum] &&
	    !(hLocal->dwFlags & (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_NO_FAULT)))
		{

			if (hLocal->CPU.suspend_fault[wNum] != 0)
			{
				hLocal->CPU.suspend_fault[wNum]--;
			}
			else
	    {
		instruction_fault(hLocal,(BYTE)wNum);
		continue;
	    }
	}


		if (byOpcode == immunity_opcode && check_for_immunity(hLocal))
	{
	    hLocal->CPU.immune_iteration++;

	    if (hLocal->CPU.immune_iteration >=
		hLocal->CPU.max_immune_iteration)
		return(FALSE);

	    continue;
	}

	if (hLocal->dwFlags & LOCAL_FLAG_REPAIR_DECRYPT)
	{
	    if (found_repair_sig(hLocal,byOpcode, lpSigList, lpwSigNum))
		return(TRUE);
	}
	else if (hLocal->dwFlags & LOCAL_FLAG_REPAIR_REPAIR)
	{
	    // non-zero value means you're done.  FFFF means error.

	    if (hLocal->repair_result != 0)
	    {
		*lpwSigNum = sign_extend_byte((BYTE)(hLocal->repair_result&0xFF));
		return(TRUE);
	    }
	}

	hLocal->CPU.reset_seg = TRUE;

	large_switch(hLocal,byOpcode);

		if (TRUE == hLocal->CPU.reset_seg)
	{
	    reset_seg_over(hLocal);
	    reset_rep_over(hLocal);
	    reset_32_bit_over(hLocal);
	}

		// Increase the number of instructions executed so far

	if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
	{
			hLocal->CPU.immune_iteration++;

	    if (hLocal->CPU.immune_iteration >=
		hLocal->CPU.max_immune_iteration)
	    {
		*lpwSigNum = REPAIR_FAILURE;
		return(FALSE);
	    }
	}
	else
	{
	    hLocal->CPU.iteration++;

	    // make sure we allow instruction faults now.

	    hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_NO_FAULT;

	    if (hLocal->CPU.iteration >= hLocal->CPU.max_iteration)
	    {
		*lpwSigNum = REPAIR_FAILURE;
		return(FALSE);
	    }

	}

#ifdef SYM_NLM
	    if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
	    {
		if (hLocal->CPU.immune_iteration >
			hLocal->CPU.next_nlm_immune_iter_stop)
		{
		    // relinquish control to other NLM's on NLM platforms

		    hLocal->CPU.next_nlm_immune_iter_stop +=
			NLM_ITER_CHECK_POINT;

		    PAMRelinquishControl();
		}
	    }
	    else
	    {
		if (hLocal->CPU.iteration > hLocal->CPU.next_nlm_iter_stop)
		{
		    // relinquish control to other NLM's on NLM platforms

		    hLocal->CPU.next_nlm_iter_stop += NLM_ITER_CHECK_POINT;

		    PAMRelinquishControl();
		}
	    }
#endif

	
	}
}


PAMSTATUS load_repair_info(PAMLHANDLE hLocal,
			   DATAFILEHANDLE  hDataFile,
			   WORD wVirusID,
			   sig_list_type *sig_list)
{
    DFSTATUS            dfTemp;
    DWORD               dwRepairStart;
    WORD                wNumBundles, i, j;
    repair_bundle_type  stBundle;
	HFILE               hFile;

	(void)hLocal;

    hFile = DFGetHandle(hDataFile);

    dfTemp = DFLookUp(hDataFile,
					  ID_PAM_REPAIR_BUNDLES,
		      NULL,
		      &dwRepairStart,
		      NULL, NULL, NULL, NULL);


    if (DFSTATUS_OK != dfTemp)
	return(PAMSTATUS_FILE_ERROR);

    if ((DWORD)PAMFileSeek(hFile,dwRepairStart, SEEK_SET) != dwRepairStart)
	return(PAMSTATUS_FILE_ERROR);

    // load up the # of bundles so we can iterate

    if (PAMFileRead(hFile, &wNumBundles, sizeof(WORD)) != sizeof(WORD))
	return(PAMSTATUS_FILE_ERROR);

    wNumBundles = WENDIAN(wNumBundles);

    dwRepairStart += sizeof(WORD);      // skip over WORD which tells us how
					// many bundles we have.

    for (i=0;i<wNumBundles;i++)
    {
	if (PAMFileRead(hFile,&stBundle,sizeof(stBundle)) != sizeof(stBundle))
	    return(PAMSTATUS_FILE_ERROR);

#ifdef BIG_ENDIAN
	stBundle.wVirusID = WENDIAN(stBundle.wVirusID);
	stBundle.dwMaxIter = DWENDIAN(stBundle.dwMaxIter);
	stBundle.dwMaxRepairProgIter = DWENDIAN(stBundle.dwMaxRepairProgIter);
	stBundle.wNumSigs = WENDIAN(stBundle.wNumSigs);
	stBundle.wCodeSize = WENDIAN(stBundle.wCodeSize);
	stBundle.wFlags = WENDIAN(stBundle.wFlags);
#endif

	// did we find the proper bundle?

	if (wVirusID == stBundle.wVirusID)
	{
	    sig_list->dwBundleOffset = dwRepairStart;
	    sig_list->repair_bundle_info = stBundle;

	    for (j=0;j<stBundle.wNumSigs;j++)
	    {
		if (PAMFileRead(hFile,
			       &(sig_list->repair_sig[j]),
			       sizeof(repair_sig_type)) !=
		    sizeof(repair_sig_type))
		    return(PAMSTATUS_FILE_ERROR);

#ifdef BIG_ENDIAN
		{
		    WORD        wTemp;

		    sig_list->repair_sig[j].wSigLen =
			WENDIAN(sig_list->repair_sig[j].wSigLen);

		    for (wTemp=0;wTemp<REPAIR_SIG_LEN;wTemp++)
		    {
			sig_list->repair_sig[j].wSig[wTemp] =
			    WENDIAN(sig_list->repair_sig[j].wSig[wTemp]);
		    }
		}
#endif

	    }

	    return(PAMSTATUS_OK);
	}

	// update our repairstart info to point to the start of the next bundle

	dwRepairStart += sizeof(stBundle) +
			 stBundle.wNumSigs * sizeof(repair_sig_type) +
			 stBundle.wCodeSize;

	if ((DWORD)PAMFileSeek(hFile, dwRepairStart, SEEK_SET) !=
	    dwRepairStart)
	    return(PAMSTATUS_FILE_ERROR);
    }

	return(PAMSTATUS_NO_REPAIR);
}

PAMSTATUS copy_bytes_to_vm(PAMLHANDLE   hLocal,
						   WORD         wNumBytes,
						   DWORD        dwStartOff,
			   HFILE        hFile,
						   WORD         wStartCS,
						   WORD         wStartIP,
						   WORD         *lpwBytesRead)
{
	BYTE    byBuffer[TRANSFER_BUFFER_SIZE];
	WORD    wBytesToRead, wBytesRead, i;

	*lpwBytesRead = 0;

	while (wNumBytes > 0)
	{
		if (wNumBytes > TRANSFER_BUFFER_SIZE)
			wBytesToRead = TRANSFER_BUFFER_SIZE;
		else
			wBytesToRead = wNumBytes;

	if ((DWORD)PAMFileSeek(hFile, dwStartOff, SEEK_SET) != dwStartOff)
			return(PAMSTATUS_FILE_ERROR);

	wBytesRead = PAMFileRead(hFile, byBuffer, wBytesToRead);

		if (wBytesRead == 0xFFFFU)
			return(PAMSTATUS_FILE_ERROR);

		*lpwBytesRead += wBytesRead;

		for (i=0;i<wBytesRead;i++)
			put_byte(hLocal,wStartCS,wStartIP++,byBuffer[i]);

		if (wBytesRead != wBytesToRead)
	    break;                      // less bytes read than anticipated...
					// *lpwBytesRead is correct.

		wNumBytes -= wBytesToRead;
		dwStartOff += wBytesToRead;
	}

	return(PAMSTATUS_OK);
}


PAMSTATUS copy_bytes_from_vm(PAMLHANDLE   hLocal,
			     WORD         wNumBytes,
			     DWORD        dwStartOff,
			     HFILE        hFile,
			     WORD         wStartSEG,
			     WORD         wStartOFF,
			     WORD         *lpwBytesWritten)
{
    BYTE    byBuffer[TRANSFER_BUFFER_SIZE];
    WORD    wBytesToWrite, i, wBytesWritten;

    *lpwBytesWritten = 0;

    if ((DWORD)PAMFileSeek(hFile, dwStartOff, SEEK_SET) != dwStartOff)
	return(PAMSTATUS_FILE_ERROR);

    while (wNumBytes > 0)
    {
	if (wNumBytes > TRANSFER_BUFFER_SIZE)
	    wBytesToWrite = TRANSFER_BUFFER_SIZE;
	else
	    wBytesToWrite = wNumBytes;

	// load the bytes from the VM into our buffer and send them out with
	// a block file write...

	for (i=0;i<wBytesToWrite;i++)
	    byBuffer[i] = get_byte(hLocal,wStartSEG,wStartOFF++);

	wBytesWritten = PAMFileWrite(hFile, byBuffer, wBytesToWrite);

	*lpwBytesWritten += wBytesWritten;

	if (wBytesWritten != wBytesToWrite)
	    return(PAMSTATUS_FILE_ERROR);

	wNumBytes -= wBytesToWrite;
    }

    return(PAMSTATUS_OK);
}


PAMSTATUS load_repair_code(PAMLHANDLE hLocal,
			   DATAFILEHANDLE  hDataFile,
			   sig_list_type *lpSigList)
{
	DWORD           dwSeekOff, dwLength;
	WORD                    wTemp = 0;
    PAMSTATUS       pamStatus;
	DFSTATUS        dfTemp;
    HFILE           hFile;

    hFile = DFGetHandle(hDataFile);

	dwSeekOff = lpSigList->dwBundleOffset +
				sizeof(repair_bundle_type) +
				lpSigList->repair_bundle_info.wNumSigs *
					sizeof(repair_sig_type);


	pamStatus = copy_bytes_to_vm(hLocal,
								 lpSigList->repair_bundle_info.wCodeSize,
								 dwSeekOff,
				 hFile,
								 REPAIR_SEG,
								 REPAIR_OVERLAY_OFF,
								 &wTemp);

	if (pamStatus != PAMSTATUS_OK)
		return(pamStatus);

    dfTemp = DFLookUp(hDataFile,
					  ID_PAM_REPAIR_FOUNDATION,
					  NULL,
					  &dwSeekOff,
					  &dwLength, NULL, NULL, NULL);

	if (dfTemp != DFSTATUS_OK)
		return(PAMSTATUS_FILE_ERROR);

	pamStatus = copy_bytes_to_vm(hLocal,
								 (WORD)dwLength,
								 dwSeekOff,
				 hFile,
								 REPAIR_SEG,
								 REPAIR_FOUNDATION_OFF,
								 &wTemp);

	return(pamStatus);
}


