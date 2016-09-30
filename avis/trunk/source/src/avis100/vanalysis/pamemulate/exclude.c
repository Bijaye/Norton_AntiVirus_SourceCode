// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/exclude.cpv   1.23   18 Mar 1996 10:36:12   CNACHEN  $
//
// Description:
//
//  Contains PAM dynamic and static exclusion code.  This is different from
//  the signature-based exclusion code contained in SIGEX.CPP.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/exclude.cpv  $
// 
//    Rev 1.23   18 Mar 1996 10:36:12   CNACHEN
// Added CS:IP and iteration # to camel printount.
// 
//    Rev 1.22   15 Mar 1996 13:27:22   CNACHEN
// Added INVALID_INDEX exclusions...
// 
//    Rev 1.21   12 Mar 1996 10:32:46   CNACHEN
// Fixed commenting error.
// 
//    Rev 1.20   08 Mar 1996 10:50:56   CNACHEN
// Added NOT_IN_TSR support.
// 
//    Rev 1.19   28 Feb 1996 10:10:40   CNACHEN
// Properly return FILE_ERROR instead of MEM_ERROR...
// 
//    Rev 1.18   23 Feb 1996 14:44:24   CNACHEN
// Finished commenting this file...
// 
//    Rev 1.17   22 Feb 1996 13:17:20   CNACHEN
// Added comments.. not done yet tho...
// 
//    Rev 1.16   02 Feb 1996 12:22:48   CNACHEN
// Fixed bug with loading and setting of CALL128 flag.
// 
//    Rev 1.15   02 Feb 1996 11:44:18   CNACHEN
// Added new dwFlags and revamped all exclusion checking...
// 
// 
//    Rev 1.14   25 Jan 1996 16:00:22   CNACHEN
// Fixed EXE EOF bug...
// 
//    Rev 1.13   22 Jan 1996 17:24:12   DCHI
// exclude_item() returns immediately if instructionFetchCount is within
// config_info.ulMinNoExcludeCount.
// 
//    Rev 1.12   10 Jan 1996 13:23:28   CNACHEN
// Fixed bug checking for min file size...
// 
//    Rev 1.11   14 Dec 1995 10:49:34   CNACHEN
// No change.
// 
//    Rev 1.10   13 Dec 1995 11:58:06   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
// 
//    Rev 1.9   19 Oct 1995 18:23:44   CNACHEN
// Initial revision... with comment header :)
// 
//************************************************************************

#include <stdio.h>
#include "pamapi.h"

///////////////////////////////////////////////////////////////////////////////
//??#ifdef BORLAND

void print_camel(PAMLHANDLE hLocal, WORD wCamel)
{
    extern BOOL gbDumpMem;

    if (gbDumpMem == TRUE)
    {
        FILE *stream;
        WORD seg, off, line;

        stream = fopen("memdump.dat","wb");
        if (stream != NULL)
        {
            printf("Saving off memory...\n");

			for (off=seg=0;seg<2;seg++)
			{
				fprintf(stream,"%04X:%04X ",seg,off);
				for (line=off=0;off<=0xFFFFU;off++)
				{
					fprintf(stream,"%02X ",get_byte(hLocal,seg,off));
					line += 3;

					if (line > 47)
					{
						fprintf(stream,"\n");
						fprintf(stream,"%04X:%04X ",seg,off+1);
						line = 0;
					}
                }
                fprintf(stream,"\n");
            }

            fclose(stream);
        }

    }


    printf("Camel (CSIP=%04X:%04X, Iter=%ld): ",
            hLocal->CPU.CS,
            hLocal->CPU.IP,
            hLocal->CPU.iteration);

    if (wCamel < 256)
    {
        printf("Opcode 0x%02X\n",wCamel);
        return;
    }

    if (wCamel < 384)
    {
        printf("INT 21, AH=0x%02X\n",wCamel-256);
        return;
    }

    if (wCamel == 384)
    {
        printf("BAD_INT_05\n");
        return;
    }

    if (wCamel == 385)
    {
        printf("BAD_INT_10\n");
        return;
    }

    if (wCamel == 386)
    {
        printf("BAD_INT_12\n");
        return;
    }

    if (wCamel == 387)
    {
        printf("BAD_INT_13\n");
        return;
    }

    if (wCamel == 388)
    {
        printf("BAD_INT_16\n");
        return;
    }

    if (wCamel == 389)
    {
        printf("BAD_INT_17\n");
        return;
    }

    if (wCamel == 390)
    {
        printf("BAD_INT_1A\n");
        return;
    }

    if (wCamel == 391)
    {
        printf("BAD_INT_20\n");
        return;
    }

    if (wCamel == 392)
    {
        printf("BAD_INT_21\n");
        return;
    }

    if (wCamel == 393)
    {
        printf("BAD_INT_25\n");
        return;
    }

    if (wCamel == 394)
    {
        printf("BAD_INT_26\n");
        return;
    }

    if (wCamel == 395)
    {
        printf("BAD_INT_27\n");
        return;
    }

    if (wCamel == 396)
    {
        printf("BAD_INT_2F\n");
        return;
    }

    if (wCamel == 397)
    {
        printf("BAD_INT_33\n");
        return;
    }

    if (wCamel == 398)
    {
        printf("BAD_INT_67\n");
        return;
    }

    if (wCamel == 399)
    {
        printf("BAD_COM\n");
        return;
    }

    if (wCamel == 400)
    {
        printf("BAD_EXE\n");
        return;
    }

    if (wCamel == 401)
    {
        printf("BAD_SYS\n");
        return;
    }

    if (wCamel == 402)
    {
        printf("MAX_EXE_EOF_0P1K\n");
        return;
    }

    if (wCamel == 403)
    {
        printf("MAX_EXE_EOF_0P2K\n");
        return;
    }

    if (wCamel == 404)
    {
        printf("MAX_EXE_EOF_0P3K\n");
        return;
    }

    if (wCamel == 405)
    {
        printf("MAX_EXE_EOF_1P0K\n");
        return;
    }

    if (wCamel == 406)
    {
        printf("MAX_EXE_EOF_1P1K\n");
        return;
    }

    if (wCamel == 407)
    {
        printf("MAX_EXE_EOF_1P2K\n");
        return;
    }

    if (wCamel == 408)
    {
        printf("MAX_EXE_EOF_1P3K\n");
        return;
    }

    if (wCamel == 409)
    {
        printf("MAX_EXE_EOF_2P0K\n");
        return;
    }

    if (wCamel == 410)
    {
        printf("MAX_EXE_EOF_2P1K\n");
        return;
    }

    if (wCamel == 411)
    {
        printf("MAX_EXE_EOF_2P2K\n");
        return;
    }

    if (wCamel == 412)
    {
        printf("MAX_EXE_EOF_2P3K\n");
        return;
    }

    if (wCamel == 413)
    {
        printf("MAX_EXE_EOF_3P0K\n");
        return;
    }

    if (wCamel == 414)
    {
        printf("MAX_EXE_EOF_3P1K\n");
        return;
    }

    if (wCamel == 415)
    {
        printf("MAX_EXE_EOF_3P2K\n");
        return;
    }

    if (wCamel == 416)
    {
        printf("MAX_EXE_EOF_3P3K\n");
        return;
    }

    if (wCamel == 417)
    {
        printf("MAX_EXE_EOF_4P0K\n");
        return;
    }

    if (wCamel == 418)
    {
        printf("MAX_EXE_EOF_4P1K\n");
        return;
    }

    if (wCamel == 419)
    {
        printf("MAX_EXE_EOF_4P2K\n");
        return;
    }

    if (wCamel == 420)
    {
        printf("MAX_EXE_EOF_4P3K\n");
        return;
    }

    if (wCamel == 421)
    {
        printf("MAX_EXE_EOF_5P0K\n");
        return;
    }

    if (wCamel == 422)
    {
        printf("MAX_EXE_EOF_5P1K\n");
        return;
    }

    if (wCamel == 423)
    {
        printf("MAX_EXE_EOF_5P2K\n");
        return;
    }

    if (wCamel == 424)
    {
        printf("MAX_EXE_EOF_5P3K\n");
        return;
    }

    if (wCamel == 425)
    {
        printf("MAX_EXE_EOF_6P0K\n");
        return;
    }

    if (wCamel == 426)
    {
        printf("MAX_EXE_EOF_6P1K\n");
        return;
    }

    if (wCamel == 427)
    {
        printf("MAX_EXE_EOF_6P2K\n");
        return;
    }

    if (wCamel == 428)
    {
        printf("MAX_EXE_EOF_6P3K\n");
        return;
    }

    if (wCamel == 429)
    {
        printf("MAX_EXE_EOF_7P0K\n");
        return;
    }

    if (wCamel == 430)
    {
        printf("MAX_EXE_EOF_7P1K\n");
        return;
    }

    if (wCamel == 431)
    {
        printf("MAX_EXE_EOF_7P2K\n");
        return;
    }

    if (wCamel == 432)
    {
        printf("MAX_EXE_EOF_7P3K\n");
        return;
    }

    if (wCamel == 433)
    {
        printf("MIN_EXE_EOF_0P1K\n");
        return;
    }

    if (wCamel == 434)
    {
        printf("MIN_EXE_EOF_0P2K\n");
        return;
    }

    if (wCamel == 435)
    {
        printf("MIN_EXE_EOF_0P3K\n");
        return;
    }

    if (wCamel == 436)
    {
        printf("MIN_EXE_EOF_1P0K\n");
        return;
    }

    if (wCamel == 437)
    {
        printf("MIN_EXE_EOF_1P1K\n");
        return;
    }

    if (wCamel == 438)
    {
        printf("MIN_EXE_EOF_1P2K\n");
        return;
    }

    if (wCamel == 439)
    {
        printf("MIN_EXE_EOF_1P3K\n");
        return;
    }

    if (wCamel == 440)
    {
        printf("MIN_EXE_EOF_2P0K\n");
        return;
    }

    if (wCamel == 441)
    {
        printf("MIN_EXE_EOF_2P1K\n");
        return;
    }

    if (wCamel == 442)
    {
        printf("MIN_EXE_EOF_2P2K\n");
        return;
    }

    if (wCamel == 443)
    {
        printf("MIN_EXE_EOF_2P3K\n");
        return;
    }

    if (wCamel == 444)
    {
        printf("MIN_EXE_EOF_3P0K\n");
        return;
    }

    if (wCamel == 445)
    {
        printf("MIN_EXE_EOF_3P1K\n");
        return;
    }

    if (wCamel == 446)
    {
        printf("MIN_EXE_EOF_3P2K\n");
        return;
    }

    if (wCamel == 447)
    {
        printf("MIN_EXE_EOF_3P3K\n");
        return;
    }

    if (wCamel == 448)
    {
        printf("MIN_EXE_EOF_4P0K\n");
        return;
    }

    if (wCamel == 449)
    {
        printf("MIN_EXE_EOF_4P1K\n");
        return;
    }

    if (wCamel == 450)
    {
        printf("MIN_EXE_EOF_4P2K\n");
        return;
    }

    if (wCamel == 451)
    {
        printf("MIN_EXE_EOF_4P3K\n");
        return;
    }

    if (wCamel == 452)
    {
        printf("MIN_EXE_EOF_5P0K\n");
        return;
    }

    if (wCamel == 453)
    {
        printf("MIN_EXE_EOF_5P1K\n");
        return;
    }

    if (wCamel == 454)
    {
        printf("MIN_EXE_EOF_5P2K\n");
        return;
    }

    if (wCamel == 455)
    {
        printf("MIN_EXE_EOF_5P3K\n");
        return;
    }

    if (wCamel == 456)
    {
        printf("MIN_EXE_EOF_6P0K\n");
        return;
    }

    if (wCamel == 457)
    {
        printf("MIN_EXE_EOF_6P1K\n");
        return;
    }

    if (wCamel == 458)
    {
        printf("MIN_EXE_EOF_6P2K\n");
        return;
    }

    if (wCamel == 459)
    {
        printf("MIN_EXE_EOF_6P3K\n");
        return;
    }

    if (wCamel == 460)
    {
        printf("MIN_EXE_EOF_7P0K\n");
        return;
    }

    if (wCamel == 461)
    {
        printf("MIN_EXE_EOF_7P1K\n");
        return;
    }

    if (wCamel == 462)
    {
        printf("MIN_EXE_EOF_7P2K\n");
        return;
    }

    if (wCamel == 463)
    {
        printf("MIN_EXE_EOF_7P3K\n");
        return;
    }

    if (wCamel == 466)
    {
        printf("MIN_SIZE_1K\n");
        return;
    }

    if (wCamel == 467)
    {
        printf("MIN_SIZE_2K\n");
        return;
    }

    if (wCamel == 468)
    {
        printf("MIN_SIZE_4K\n");
        return;
    }

    if (wCamel == 469)
    {
        printf("MIN_SIZE_8K\n");
        return;
    }

    if (wCamel == 470)
    {
        printf("COM_JUMP_START\n");
        return;
    }

    if (wCamel == 471)
    {
        printf("REP_MOVS\n");
        return;
    }

    if (wCamel == 472)
    {
        printf("REP_STOS\n");
        return;
    }

    if (wCamel == 473)
    {
        printf("CALL128\n");
        return;
    }

    if (wCamel == 474)
    {
        printf("REP_SCAS\n");
        return;
    }

    if (wCamel == 475)
    {
        printf("RELOC_AFTER_CSIP\n");
        return;
    }

    if (wCamel == 476)
    {
        printf("STOP_AFTER_16\n");
        return;
    }

    if (wCamel == 477)
    {
        printf("STOP_AFTER_1K\n");
        return;
    }

    if (wCamel == 478)
    {
        printf("STOP_AFTER_16K\n");
        return;
    }

    if (wCamel == 479)
    {
        printf("STOP_AFTER_32K\n");
        return;
    }

    if (wCamel == 480)
    {
        printf("STOP_AFTER_48K\n");
        return;
    }

    if (wCamel == 481)
    {
        printf("STOP_AFTER_64K\n");
        return;
    }

    if (wCamel == 482)
    {
        printf("STOP_AFTER_80K\n");
        return;
    }

    if (wCamel == 483)
    {
        printf("STOP_AFTER_96K\n");
        return;
    }

    if (wCamel == 484)
    {
        printf("STOP_AFTER_128K\n");
        return;
    }

    if (wCamel == 485)
    {
        printf("RUN_32\n");
        return;
    }

    if (wCamel == 486)
    {
        printf("INVALID_INDEX_WRITE\n");
        return;
    }

    printf("The camel has landed!\n");

}

//??#endif
///////////////////////////////////////////////////////////////////////////////




//************************************************************************
// void clear_bit(exclude_info, set_num, virus_num)
//
// Purpose
//
//      During PAM global initialization, MAX_EXCLUDE_CAT buffers are allocated
//  for the PAM exclusion system.  Each of these buffers contains Ceil(N/16)
//  WORDs, where N is the number of virus profiles in the PAM system.
//  A bit value of 1 at index M of a given buffer indicates that the Mth
//  virus in the VIRUS.DEF file is allowed to use the associated item
//  without being excluded.  Here are some sample items:
//
//                                    Viruses
//                               0123456789ABCDEF
//
//  buffer0 (instruction 00):    0000000000000100
//  buffer1 (instruction 01):    0000000100000000
//  ...
//  bufferE9 (instruction E9):   0000101000000000
//
//  bufferInt21:                 0010000001000000
//  bufferInt26:                 0000000000000000
//
//  In the above example, we can support up to 16 viruses.  Each allocated
//  buffer is 1 WORD long.  During emulation, if a 00 opcode is encountered,
//  buffer0 is ANDed with an equal sized candidate bitmap buffer (that starts
//  out as all 1s - to indicate that all viruses are under consideration).  In
//  the above example, we see that as only virus number 0Dh can use the 00
//  instruction, so when the buffer0 bitmask is ANDed with the candidate
//  bitmask, all other viruses will be excluded (because of their 0 bit-value).
//
//  Look in EXCLUDE.H for the list of all exclusion buffers.
//
//  Now for the juice:  This function is used during global initialization to
//  clear a bit in one of the MAX_EXCLUDE_CAT buffers.  It basically allows
//  PAM to indicate that a given virus cannot use a given technique.
//
// Parameters:
//
//      EXCLUDE_TYPE    *exclude_info       PAM Global exclusion structure
//      WORD            set_num             Specifies one the MAX_EXCLUDE_CAT
//                                          buffers
//      WORD            virus_num           Specifies the bit number within the
//                                          buffer to clear.
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

void clear_bit(exclude_type *exclude_info, WORD set_num, WORD virus_num)
{
    WORD bit_num, word_num;

    bit_num  = virus_num & 0xF;
    word_num = virus_num >> 0x4;

    exclude_info->emask[set_num].bitmap[word_num] &=
            ~(WORD)(1 << bit_num);
}


//************************************************************************
// void clear_bits(exclude_info, set, virus_num, base, num_items)
//
// Purpose
//
//      This function can be applied during global initialization to clear
//  a whole slew of bits at a time.  For instance, each virus profile contains
//  a list of opcodes that the virus can use within its polymorphic code.  This
//  bit array can be specified (set param) and then used to clear or set bit
//  number virus_num in buffers numbered base to base+num_items-1.
//
//  Example:
//
//           0123456789ABCDEF
//  MTE set: 0111101000011100       (MTE can use opcodes 1, 2, 3,4, 6, etc)
//
//  Assume MTE is virus number 5 (virus_num)
//                    (*6*th profile in the VIRUS.DEF file)
//
//  base = 0 (starting with buffer 0)
//  num_items = 16 (clear or set bits in buffers 0 to F)
//
//  After calling this function, this would be the result:
//
//                   Viruses
//               0123456789ABCDE
//      buffer0: 111110111111111
//      buffer1: 111111111111111
//      buffer2: 111111111111111
//      buffer3: 111111111111111
//      buffer4: 111111111111111
//      buffer5: 111110111111111
//      buffer6: 111111111111111
//      buffer7: 111110111111111
//      buffer8: 111110111111111
//      buffer9: 111110111111111
//      bufferA: 111110111111111
//      bufferB: 111111111111111
//      bufferC: 111111111111111
//      bufferD: 111111111111111
//      bufferE: 111110111111111
//      bufferF: 111110111111111
//
// Parameters:
//
//      EXCLUDE_TYPE    *exclude_info       PAM Global exclusion structure
//      WORD            set                 Bits associated with the virus
//      WORD            virus_num           Specifies the bit number within the
//                                          buffer(s) to clear
//      WORD            base                Which buffer to start with
//      WORD            num_items           How many buffers should we apply
//                                          our bitmask to, starting from base.
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

void clear_bits(exclude_type *exclude_info,
                WORD set[INSTR_WORDS],
                WORD virus_num,
				WORD base,
				WORD num_items)
{
    WORD i, bit_num, word_num;

	for (i=0;i<num_items;i++)
    {
        bit_num  = i & 0xF;
        word_num = i >> 0x4;

        if ((set[word_num] & (1 << bit_num)) == 0)
        {
            bit_num = virus_num & 0xF;
			word_num = virus_num >> 0x4;

			/* placing a ZERO in means that the virus can be excluded if this
			   opcode is executed */

			exclude_info->emask[i+base].bitmap[word_num] &= ~(1 << bit_num);
		}
	}
}



//************************************************************************
// PAMSTATUS load_exclude(hStream, dwStartOff, exclude_info, wVirusCount)
//
// Purpose
//
//      This function loads all of the exclusion information from the
//  VIRSCAN2.DAT file.  This only loads virus profile information, not virus
//  signature data.  All memory is dynamically allocated for this purpose.
//  Memory is allocated in large chunks to reduce the number of handles used.
//  If we are dealing with wNumProfiles (read from VIRSCAN2.DAT), we require
//  wNumProfiles/16 WORDs per exclusion buffer.  This function allocates as
//  many near-64K chunks as required to store all exclusion buffers.  A max of
//  64K/(wNumProfiles/16*sizeof(WORD)) can be stored per buffer.  Currently,
//  there are 486 exclusion categories.  If we have 800 PAM profiles, we have
//  Ceil(800/16)=50 WORDs per exclusion buffer.  Therefore, we require
//  50 * sizeof(WORD) * 486 bytes to store all of our exclusion data.  This can
//  fit in one <64K buffer...
//
//  Data layout in RAM:
//
//      buffer1: [0--1--2--3--4--5--6--7--8--9--A--B--C--D--E--] (kilobytes)
//                buf0buf1buf2buf3buf4buf5buf6buf7buf8buf9bufA
//      buffer2:  bufBbufCbufD...
//      ...
//
//      exclude_info->emask[0].bitmap points to start of buffer1
//      exclude_info->emask[1].bitmap points to buffer1 +
//                                               (wNumProfiles/16*sizeof(WORD))
//      exclude_info->emask[2].bitmap points to buffer1 + 2 *
//                                               (wNumProfiles/16*sizeof(WORD))
//
//      wNumProfiles/16 is Ceiled in the above 2 lines.
//
//  If there are any errors allocating memory, all memory is freed and an error
//  is returned.
//
//  All buffers are initialized to contain FFh bytes before data is loaded
//  from the VIRSCAN2.DAT file.  As profiles are loaded, bits are *cleared*
//  to 0 value when a virus can't use a given instruction, interrupt, etc.
//
//
//  Data layout in VIRSCAN2.DAT (from the start of the section (dwStartOff)):
//
//  [WORD: N= Number Of Profiles][DecryptRec0][DecryptRec1]...[DecryptRecN-1]
//
//  DecryptRecs are of type profile_type, found in PROFILE.H
//
//
//  There is a NOT_IN_TSR flag that indicates whether or not a PROFILE should
//  be used in AutoProtect products.  If this flag is set (see below), then
//  all exclusion bits for the specified virus are set to 0.  This will cause
//  the profile to be immediately excluded as soon as any exclusions are
//  applied.
//
// Parameters:
//
//      HFILE           hStream         Handle into VIRSCAN2.DAT where
//                                      exclusion data is stored.
//      DWORD           dwStartOff      Location in VIRSCAN2.DAT of data, from
//                                      the TOF.
//      exclude_type    *exclude_info   Global exclusion structure to fill
//      WORD            *wVirusCount    # of *profiles* actually loaded
//
// Returns:
//
//      PAMSTATUS_OK            on success
//      PAMSTATUS_FILE_ERROR    on file error
//      PAMSTATUS_MEM_ERROR     on memory allocation error
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

PAMSTATUS load_exclude(HFILE hStream,
					   DWORD dwStartOff,
					   exclude_type *exclude_info,
					   WORD *wVirusCount)
{
	profile_type    decrypt_rec;
    WORD            virus_num, i, j, wNumProfiles, wNumVirusWords,
                    wMaxClusterSize, wClusterSize;


	*wVirusCount = 0;                   // to detect errors

    /* this database uses the data structure found in decrypt.h.
       First we must seek to the proper start in the DATAFILE before we
       begin reading.  Then, fetch the WORD which specifies how many different
       mutation engine profiles we have and fetch them one at a time
    */

    if((DWORD)PAMFileSeek(hStream,dwStartOff,SEEK_SET) != dwStartOff)
        return(PAMSTATUS_FILE_ERROR);

    // read in the number of different profiles

    if (PAMFileRead(hStream,&wNumProfiles,sizeof(wNumProfiles)) !=
        sizeof(wNumProfiles))
        return(PAMSTATUS_FILE_ERROR);

    wNumProfiles = WENDIAN(wNumProfiles);

    // and allocate appropriate memory

	wNumVirusWords = wNumProfiles >> 0x4;
    if (wNumProfiles & 0xF)
        wNumVirusWords++;

    if (wNumVirusWords)
        wMaxClusterSize = (0xFFFF / (wNumVirusWords * sizeof(WORD)));
    else
        return(PAMSTATUS_MEM_ERROR);        // no profiles?!?!

    wClusterSize = 0;

    for (i=0;i<MAX_EXCLUDE_CAT;i++)
    {
        if (wClusterSize == 0)
        {
            wClusterSize = MAX_EXCLUDE_CAT - i;
            if (wClusterSize > wMaxClusterSize)
                wClusterSize = wMaxClusterSize;

            exclude_info->emask[i].bitmap =
                (WORD *)PAMMemoryAlloc(sizeof(WORD)*
                                    wNumVirusWords*
                                    wClusterSize);

            if (NULL == exclude_info->emask[i].bitmap)
            {
                for (j=0 ; j<i ; j += wMaxClusterSize)
                    PAMMemoryFree(exclude_info->emask[j].bitmap);

                return(PAMSTATUS_MEM_ERROR);
            }
        }
        else
            exclude_info->emask[i].bitmap =
                exclude_info->emask[i-1].bitmap + wNumVirusWords;

        wClusterSize--;

        /* fill our buffers so nothing is excluded unless we say so. */

        MEMSET(exclude_info->emask[i].bitmap,
               0xFF,                                    /* all bits on */
               wNumVirusWords * sizeof(WORD));
    }



    for (virus_num=0;virus_num<wNumProfiles;virus_num++)
    {
        if (PAMFileRead(hStream,&decrypt_rec,sizeof(decrypt_rec)) !=
            sizeof(decrypt_rec))
        {
            /* if we have any errors reading data, then free everything
               allocated so far and return. */

            for (j=0 ; j<i ; j += wMaxClusterSize)
                PAMMemoryFree(exclude_info->emask[j].bitmap);

            return(PAMSTATUS_FILE_ERROR);
        }

#ifdef BIG_ENDIAN

        ////////////////////////
        // endian stuff: convert decrypt_rec to big endian if necessary

        {
            WORD        wTemp;

            decrypt_rec.flags = DWENDIAN(decrypt_rec.flags);
            decrypt_rec.min_file_size = DWENDIAN(decrypt_rec.min_file_size);
            decrypt_rec.max_exe_eof_dist = DWENDIAN(decrypt_rec.max_exe_eof_dist);
            decrypt_rec.min_exe_eof_dist = DWENDIAN(decrypt_rec.min_exe_eof_dist);
            decrypt_rec.bad_int_set = WENDIAN(decrypt_rec.bad_int_set);

            for (wTemp = 0;wTemp < INSTR_WORDS;wTemp++)
                decrypt_rec.opcode_set[wTemp] =
                    WENDIAN(decrypt_rec.opcode_set[wTemp]);

            for (wTemp = 0;wTemp < DOS_FUNC_WORDS;wTemp++)
                decrypt_rec.good_dos_set[wTemp] =
                    WENDIAN(decrypt_rec.good_dos_set[wTemp]);

        }
        // endian stuff
        ////////////////////////

#endif

#if defined(IN_AUTOPROTECT)

        // Immediately exclude NOT_IN_TSR profiles from all AUTOPROTECT

        if ((decrypt_rec.flags & PROFILE_FLAG_NOT_IN_TSR))
        {
            WORD    wExcludeCat;

            for (wExcludeCat=0;wExcludeCat<MAX_EXCLUDE_CAT;wExcludeCat++)
                clear_bit(exclude_info, wExcludeCat, virus_num);

            continue;
        }
#endif

        clear_bits(exclude_info,
                   decrypt_rec.opcode_set,
                   virus_num,
				   OPCODE_BASE,
                   NUM_OPCODE_CAT);

		clear_bits(exclude_info,
				   decrypt_rec.good_dos_set,
				   virus_num,
				   INT_21_BASE,
                   NUM_INT_21_CAT);

        if ((decrypt_rec.infect_set & INFECTS_COM) == 0)
            clear_bit(exclude_info, BAD_COM, virus_num);

        if ((decrypt_rec.infect_set & INFECTS_EXE) == 0)
            clear_bit(exclude_info, BAD_EXE, virus_num);

        if ((decrypt_rec.infect_set & INFECTS_SYS) == 0)
            clear_bit(exclude_info, BAD_SYS, virus_num);

        if (decrypt_rec.flags & PROFILE_FLAG_COM_JUMP)
            clear_bit(exclude_info, COM_JUMP_START, virus_num);

        if (!(decrypt_rec.flags & PROFILE_FLAG_REP_MOVS))
			clear_bit(exclude_info, REP_MOVS, virus_num);

        if (!(decrypt_rec.flags & PROFILE_FLAG_REP_STOS))
			clear_bit(exclude_info, REP_STOS, virus_num);

        if (!(decrypt_rec.flags & PROFILE_FLAG_REP_SCAS))
            clear_bit(exclude_info, REP_SCAS, virus_num);

        if (!(decrypt_rec.flags & PROFILE_FLAG_RELOC_AFTER_CSIP))
            clear_bit(exclude_info, RELOC_AFTER_CSIP, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_16))
            clear_bit(exclude_info, STOP_AFTER_16, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_1024))
            clear_bit(exclude_info, STOP_AFTER_1K, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_16384))
            clear_bit(exclude_info, STOP_AFTER_16K, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_32768))
            clear_bit(exclude_info, STOP_AFTER_32K, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_49152))
            clear_bit(exclude_info, STOP_AFTER_48K, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_65536))
            clear_bit(exclude_info, STOP_AFTER_64K, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_81920))
            clear_bit(exclude_info, STOP_AFTER_80K, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_98304))
            clear_bit(exclude_info, STOP_AFTER_96K, virus_num);

        if ((decrypt_rec.flags & PROFILE_FLAG_STOP_AFTER_131072))
            clear_bit(exclude_info, STOP_AFTER_128K, virus_num);

        if (!(decrypt_rec.flags & PROFILE_FLAG_RUN_32))
            clear_bit(exclude_info, RUN_32, virus_num);

        if (!(decrypt_rec.flags & PROFILE_FLAG_CALL128))
            clear_bit(exclude_info, CALL128, virus_num);

        if (!(decrypt_rec.flags & PROFILE_FLAG_INVALID_INDEX_WRITE))
            clear_bit(exclude_info, INVALID_INDEX_WRITE, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_05)
            clear_bit(exclude_info, BAD_INT_05, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_10)
            clear_bit(exclude_info, BAD_INT_10, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_12)
            clear_bit(exclude_info, BAD_INT_12, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_13)
            clear_bit(exclude_info, BAD_INT_13, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_16)
            clear_bit(exclude_info, BAD_INT_16, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_17)
            clear_bit(exclude_info, BAD_INT_17, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_1A)
            clear_bit(exclude_info, BAD_INT_1A, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_20)
            clear_bit(exclude_info, BAD_INT_20, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_21)
            clear_bit(exclude_info, BAD_INT_21, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_25)
            clear_bit(exclude_info, BAD_INT_25, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_26)
            clear_bit(exclude_info, BAD_INT_26, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_27)
            clear_bit(exclude_info, BAD_INT_27, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_2F)
            clear_bit(exclude_info, BAD_INT_2F, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_33)
            clear_bit(exclude_info, BAD_INT_33, virus_num);

        if (decrypt_rec.bad_int_set & BIT_INT_67)
            clear_bit(exclude_info, BAD_INT_67, virus_num);

        // MAX dist of entry from from EOF in EXE files

        if (decrypt_rec.max_exe_eof_dist <= 256 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_0P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 512 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_0P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 768 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_0P3K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 1024 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_1P0K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 1280 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_1P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 1536 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_1P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 1892 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_1P3K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 2048 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_2P0K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 2304 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_2P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 2560 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_2P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 2816 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_2P3K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 3072 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_3P0K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 3328 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_3P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 3584 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_3P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 3840 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_3P3K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 4096 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_4P0K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 4352 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_4P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 4608 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_4P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 4864 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_4P3K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 5120 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_5P0K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 5376 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_5P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 5632 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_5P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 5888 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_5P3K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 6144 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_6P0K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 6400 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_6P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 6656 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_6P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 6912 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_6P3K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 7168 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_7P0K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 7424 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_7P1K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 7680 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_7P2K, virus_num);

        if (decrypt_rec.max_exe_eof_dist <= 7936 &&
            decrypt_rec.max_exe_eof_dist)
            clear_bit(exclude_info, MAX_EXE_EOF_7P3K, virus_num);

        // MIN dist of entry from from EOF in EXE files

        if (decrypt_rec.min_exe_eof_dist > 256 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_0P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 512 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_0P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 768 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_0P3K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 1024 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_1P0K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 1280 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_1P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 1536 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_1P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 1892 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_1P3K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 2048 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_2P0K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 2304 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_2P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 2560 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_2P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 2816 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_2P3K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 3072 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_3P0K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 3328 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_3P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 3584 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_3P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 3840 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_3P3K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 4096 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_4P0K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 4352 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_4P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 4608 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_4P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 4864 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_4P3K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 5120 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_5P0K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 5376 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_5P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 5632 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_5P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 5888 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_5P3K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 6144 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_6P0K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 6400 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_6P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 6656 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_6P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 6912 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_6P3K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 7168 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_7P0K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 7424 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_7P1K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 7680 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_7P2K, virus_num);

        if (decrypt_rec.min_exe_eof_dist > 7936 &&
            decrypt_rec.min_exe_eof_dist)
            clear_bit(exclude_info, MIN_EXE_EOF_7P3K, virus_num);





        if (decrypt_rec.min_file_size >= 1024 &&
            decrypt_rec.min_file_size != 0)
            clear_bit(exclude_info, MIN_SIZE_1K, virus_num);

        if (decrypt_rec.min_file_size >= 2048 &&
            decrypt_rec.min_file_size != 0)
            clear_bit(exclude_info, MIN_SIZE_2K, virus_num);

        if (decrypt_rec.min_file_size >= 4096 &&
            decrypt_rec.min_file_size != 0)
            clear_bit(exclude_info, MIN_SIZE_4K, virus_num);

        if (decrypt_rec.min_file_size >= 8192 &&
            decrypt_rec.min_file_size != 0)
            clear_bit(exclude_info, MIN_SIZE_8K, virus_num);
	}

    *wVirusCount = wNumProfiles;

    return(PAMSTATUS_OK);
}



//************************************************************************
// PAMSTATUS global_init_exclude(hStream, dwStartOff, exclude_info)
//
// Purpose
//
//      This function calls upon load_exclude to load all profile data from
//  VIRSCAN2.DAT.  This is called during the global initialization phase.
//
// Parameters:
//
//      HFILE           hStream         Handle into VIRSCAN2.DAT where
//                                      exclusion data is stored.
//      DWORD           dwStartOff      Location in VIRSCAN2.DAT of data, from
//                                      the TOF.
//      exclude_type    *exclude_info   Global exclusion structure to fill
//
// Returns:
//
//      PAMSTATUS_OK            on success
//      PAMSTATUS_FILE_ERROR    on file error
//      PAMSTATUS_MEM_ERROR     on memory allocation error
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

PAMSTATUS global_init_exclude(HFILE hStream,
                              DWORD dwStartOff,
                              exclude_type *exclude_info)
{
    WORD            virus_count;
    PAMSTATUS       pamResult;

    /* load all data for scanning */

    pamResult = load_exclude(hStream,
                             dwStartOff,
                             exclude_info,
                             &virus_count);

    if (pamResult != PAMSTATUS_OK)
        return(pamResult);

    /* determine how many bytes we need to store the virus bits */

    exclude_info->cand_words = virus_count >> 0x4;
    if (virus_count & 0xF)
        exclude_info->cand_words ++;

    /* determine how many bits of the last byte are needed (# of viruses
       modulo 16) */

    exclude_info->last_word_mask = (1 << (virus_count & 0xF)) - 1;

	return(PAMSTATUS_OK);
}



//************************************************************************
// void local_init_exclude(hLocal)
//
// Purpose
//
//      This function resets all exclusion information so PAM can scan a new
//  file for viruses.  Basically, all bits in the candidate bitmap are set to
//  1, indicating that we have not seen any behavior from the host file yet
//  that would exclude any viruses from consideration.
//
//  In addition, all exclusion categories have their "excluded" field set to
//  FALSE.  This indicates that no items have been excluded.  As opcodes are
//  encountered and viruses are excluded, the "excluded" fields will be set to
//  TRUE to indicate that they have already been dealt with.
//
// Parameters:
//
//      PAMLHANDLE      hLocal      A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

void local_init_exclude(PAMLHANDLE hLocal)
{
    WORD                i;
    exclude_type        *exclude_ptr;

    MEMSET(hLocal->candidates.bitmap,
           0xFF,
           (hLocal->hGPAM->exclude_info.cand_words << 1));

    exclude_ptr = &hLocal->hGPAM->exclude_info;

	if (exclude_ptr->last_word_mask)
		hLocal->candidates.bitmap[exclude_ptr->cand_words-1] =
            exclude_ptr->last_word_mask;

    /* initialize the table which says whether or not we've already checked
       this particular exclusion or not.  For instance, if we've already
       encountered an E9 opcode in the instruction stream, there's no need
       to re-exclude viruses if we encounter another E9 later on in the
       emulation */

    for (i=0;i<MAX_EXCLUDE_CAT;i++)
        hLocal->excluded[i] = FALSE;

    hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_NO_CAND_LEFT;
}


//************************************************************************
// void global_close_exclude(exclude_info)
//
// Purpose
//
//      This function frees all memory allocated during global initialization
//  of the exclusion sub-system.  It should *only* be called if
//  global_init_exclude successfully initializes and returns PAMSTATUS_OK.
//
// Parameters:
//
//  exclude_type        *exclude_info       Global exclusion structure
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

void global_close_exclude(exclude_type *exclude_info)
{
    WORD        i, wMaxClusterSize;

    wMaxClusterSize = (0xFFFF / (exclude_info->cand_words * sizeof(WORD)));

    for (i=0;i<MAX_EXCLUDE_CAT;i+=wMaxClusterSize)
    {
        PAMMemoryFree(exclude_info->emask[i].bitmap);
        exclude_info->emask[i].bitmap = NULL;
    }
}


//************************************************************************
// void exclude_item(hLocal, wExcludeNum)
//
// Purpose
//
//      This function is called every time a program exhibits some behavior
//  which can be used to exclude viruses from consideration.  This function is
//  called for both static and dynamic exclusions.  Static exclusions include
//  distance of the entrypoint from the EOF, the presence of a JMP instruction
//  at the TOF in COM programs, etc.  Dynamic exclusions include the use of a
//  certain opcode, generating a certain interrupt, using a CALL instruction
//  with a destination of >128 bytes away, etc.
//
//  During the emulation of a given file, a given exclusion (wExcludeNum) is
//  only applied once.  A flag is kept for each of the 486 exclusions to
//  indicate whether or not the exclusion has been applied yet during the scan
//  of the current file.  This status is kept in hLocal->excluded[wExcludeNum].
//  (During local initialization, the 486 excluded statuses are reset.)
//
//  The wExcludeNum specifies a bitmask that is ANDed with the candidate
//  bitmap for the current file being scanned.  So:
//
//  hLocal->hGPAM->exclude_info->emask[wExcludeNum].bitmap contains 1's and 0's
//  for every virus engine.  If the Nth engine should not be excluded for
//  performing exclusion wExcludeNum, then there's a 1 in the Nth bit position.
//  If the Nth engine should be excluded for exhibiting wExcludeNumth behavior,
//  then its bit value will be 0.
//
//  hLocal->candidates.bitmap contains all 1's upon startup, indicating that
//  all N viruses are currently under consideration.  As exclusions are applied,
//  these 1 bits are ANDed with 0 bits.  When all 1 bits in this buffer are
//  gone, then the file is no longer under consideration from any of the known
//  viruses, and emulation can cease (assuming we're emulating for detection).
//
//  Exclusions are not applied in the following cases:
//
//      1. If the exclusion has already been applied during this emulation
//         session.  Specifically, hLocal->excluded[wExcludeNum] != 0
//      2. If the VM is in an immunte state or if it has already been
//         established that all viruses have been eliminated from
//         consideration.
//
// Parameters:
//
//      PAMLHANDLE      hLocal          A local PAM handle
//      WORD            wExcludeNum     Which exclusion bitmask to use
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************


void exclude_item(PAMLHANDLE hLocal, WORD wExcludeNum)
{
    WORD                    i, w = 0, *cand, *mask, wCandWords;
    exclude_type            *exclude_ptr;

    /* immediately return if we are not to do dynamic exclusions yet */

	if (hLocal->CPU.instructionFetchCount <=
			hLocal->hGPAM->config_info.ulMinNoExcludeCount &&
		hLocal->CPU.instructionFetchCount)
        return;

	/* only exclude an item if has not yet been excluded */

	if (FALSE == hLocal->excluded[wExcludeNum] &&
        !(hLocal->dwFlags &
             (LOCAL_FLAG_IMMUNE |
              LOCAL_FLAG_IMMUNE_EXCLUSION_PERM |    // PERM set during all repairs
              LOCAL_FLAG_NO_CAND_LEFT)))
    {
		hLocal->excluded[wExcludeNum] = TRUE;

        exclude_ptr = &hLocal->hGPAM->exclude_info;

        cand = hLocal->candidates.bitmap;
        mask = exclude_ptr->emask[wExcludeNum].bitmap;

        /* go through each of the viruses and mask out the ones which are
           no longer suspicious */

        wCandWords = exclude_ptr->cand_words;

        for (i=0;i<wCandWords;i++)
        {
			w |= (*cand &= *mask);

			cand++;
			mask++;
		}

        if (!w)
        {
            hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;

//??#ifdef BORLAND
            print_camel(hLocal,wExcludeNum);
//??#endif

        }
    }
}


//************************************************************************
// void exclude_exe_entry_point(hLocal, h)
//
// Purpose
//
//      This function calculates the distance between the entrypoint in EXE
//  files to the end of the load image.  The EOLI value is calculated not
//  from the actual file size, but rather from the contents of the EXE header
//  (specifically with the page_size, len_mod_512 fields).  This is done to
//  account for overlays and Windows style executables.
//
//  NOTE: Each polymorphic profile specifies both a minimum and maximum
//  allowable distance from the CS:IP entrypoint to the end of the load image.
//  Therefore, if many viruses share the same profile (e.g. they use the same
//  mutation engine), the min and max fields must be inclusive.  E.G. They
//  should contain both the largest max distance and the smallest min distance.
//  These exclusions are applied with 256byte granularity.
//
// Parameters:
//
//      PAMLHANDLE      hLocal          A local PAM handle
//      exeheader       *h              Pointer to a structure containing the
//                                      EXE header of the target file.
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

void exclude_exe_entry_point(PAMLHANDLE hLocal, exeheader *h)
{
    DWORD entry, header_eof;
	WORD  seg_mult;

	// this seg_mult stuff is done because viruses such as NATAS have
    // CS entrypoints which are negative in some cases.  If a DWORD is used
	// when multiplying CS * PARAGRAPH, then the result will be > 1MB!

    seg_mult = WENDIAN(h->cs) + WENDIAN(h->header_size);

    entry = ((DWORD)seg_mult*PARAGRAPH + WENDIAN(h->ip))
                & EFFECTIVE_ADDRESS_MASK;
    header_eof = (WENDIAN(h->page_size) * 512L) + WENDIAN(h->len_mod_512);
    if (WENDIAN(h->len_mod_512))
        header_eof -= 512L;

	/* what is the distance of the entry code from the EOF, as defined in
	   the header */

	header_eof -= entry;

	if (header_eof > 256)
        exclude_item(hLocal,MAX_EXE_EOF_0P1K);
	if (header_eof > 512)
		exclude_item(hLocal,MAX_EXE_EOF_0P2K);
	if (header_eof > 768)
		exclude_item(hLocal,MAX_EXE_EOF_0P3K);
	if (header_eof > 1024)
		exclude_item(hLocal,MAX_EXE_EOF_1P0K);
	if (header_eof > 1280)
		exclude_item(hLocal,MAX_EXE_EOF_1P1K);
	if (header_eof > 1536)
		exclude_item(hLocal,MAX_EXE_EOF_1P2K);
	if (header_eof > 1892)
		exclude_item(hLocal,MAX_EXE_EOF_1P3K);
	if (header_eof > 2048)
		exclude_item(hLocal,MAX_EXE_EOF_2P0K);
	if (header_eof > 2304)
		exclude_item(hLocal,MAX_EXE_EOF_2P1K);
	if (header_eof > 2560)
		exclude_item(hLocal,MAX_EXE_EOF_2P2K);
	if (header_eof > 2816)
		exclude_item(hLocal,MAX_EXE_EOF_2P3K);
	if (header_eof > 3072)
		exclude_item(hLocal,MAX_EXE_EOF_3P0K);
	if (header_eof > 3328)
		exclude_item(hLocal,MAX_EXE_EOF_3P1K);
	if (header_eof > 3584)
		exclude_item(hLocal,MAX_EXE_EOF_3P2K);
	if (header_eof > 3840)
		exclude_item(hLocal,MAX_EXE_EOF_3P3K);
	if (header_eof > 4096)
		exclude_item(hLocal,MAX_EXE_EOF_4P0K);
	if (header_eof > 4352)
		exclude_item(hLocal,MAX_EXE_EOF_4P1K);
	if (header_eof > 4608)
		exclude_item(hLocal,MAX_EXE_EOF_4P2K);
	if (header_eof > 4864)
		exclude_item(hLocal,MAX_EXE_EOF_4P3K);
	if (header_eof > 5120)
		exclude_item(hLocal,MAX_EXE_EOF_5P0K);
	if (header_eof > 5376)
		exclude_item(hLocal,MAX_EXE_EOF_5P1K);
	if (header_eof > 5362)
		exclude_item(hLocal,MAX_EXE_EOF_5P2K);
	if (header_eof > 5888)
		exclude_item(hLocal,MAX_EXE_EOF_5P3K);
	if (header_eof > 6144)
		exclude_item(hLocal,MAX_EXE_EOF_6P0K);
	if (header_eof > 6400)
		exclude_item(hLocal,MAX_EXE_EOF_6P1K);
	if (header_eof > 6656)
		exclude_item(hLocal,MAX_EXE_EOF_6P2K);
	if (header_eof > 6912)
		exclude_item(hLocal,MAX_EXE_EOF_6P3K);
	if (header_eof > 7168)
		exclude_item(hLocal,MAX_EXE_EOF_7P0K);
	if (header_eof > 7424)
		exclude_item(hLocal,MAX_EXE_EOF_7P1K);
	if (header_eof > 7680)
		exclude_item(hLocal,MAX_EXE_EOF_7P2K);
	if (header_eof > 7936)
		exclude_item(hLocal,MAX_EXE_EOF_7P3K);


	if (header_eof < 7936)
		exclude_item(hLocal,MIN_EXE_EOF_7P3K);
	if (header_eof < 7680)
		exclude_item(hLocal,MIN_EXE_EOF_7P2K);
	if (header_eof < 7424)
		exclude_item(hLocal,MIN_EXE_EOF_7P1K);
	if (header_eof < 7168)
		exclude_item(hLocal,MIN_EXE_EOF_7P0K);
	if (header_eof < 6912)
		exclude_item(hLocal,MIN_EXE_EOF_6P3K);
	if (header_eof < 6656)
		exclude_item(hLocal,MIN_EXE_EOF_6P2K);
	if (header_eof < 6400)
		exclude_item(hLocal,MIN_EXE_EOF_6P1K);
	if (header_eof < 6144)
		exclude_item(hLocal,MIN_EXE_EOF_6P0K);
	if (header_eof < 5888)
		exclude_item(hLocal,MIN_EXE_EOF_5P3K);
	if (header_eof < 5362)
		exclude_item(hLocal,MIN_EXE_EOF_5P2K);
	if (header_eof < 5376)
		exclude_item(hLocal,MIN_EXE_EOF_5P1K);
	if (header_eof < 5120)
		exclude_item(hLocal,MIN_EXE_EOF_5P0K);
	if (header_eof < 4864)
		exclude_item(hLocal,MIN_EXE_EOF_4P3K);
	if (header_eof < 4608)
		exclude_item(hLocal,MIN_EXE_EOF_4P2K);
	if (header_eof < 4352)
		exclude_item(hLocal,MIN_EXE_EOF_4P1K);
	if (header_eof < 4096)
		exclude_item(hLocal,MIN_EXE_EOF_4P0K);
	if (header_eof < 3840)
		exclude_item(hLocal,MIN_EXE_EOF_3P3K);
	if (header_eof < 3584)
		exclude_item(hLocal,MIN_EXE_EOF_3P2K);
	if (header_eof < 3328)
		exclude_item(hLocal,MIN_EXE_EOF_3P1K);
	if (header_eof < 3072)
		exclude_item(hLocal,MIN_EXE_EOF_3P0K);
	if (header_eof < 2816)
		exclude_item(hLocal,MIN_EXE_EOF_2P3K);
	if (header_eof < 2560)
		exclude_item(hLocal,MIN_EXE_EOF_2P2K);
	if (header_eof < 2304)
		exclude_item(hLocal,MIN_EXE_EOF_2P1K);
	if (header_eof < 2048)
		exclude_item(hLocal,MIN_EXE_EOF_2P0K);
	if (header_eof < 1892)
		exclude_item(hLocal,MIN_EXE_EOF_1P3K);
	if (header_eof < 1536)
		exclude_item(hLocal,MIN_EXE_EOF_1P2K);
	if (header_eof < 1280)
		exclude_item(hLocal,MIN_EXE_EOF_1P1K);
	if (header_eof < 1024)
		exclude_item(hLocal,MIN_EXE_EOF_1P0K);
	if (header_eof < 768)
		exclude_item(hLocal,MIN_EXE_EOF_0P3K);
	if (header_eof < 512)
		exclude_item(hLocal,MIN_EXE_EOF_0P2K);
	if (header_eof < 256)
		exclude_item(hLocal,MIN_EXE_EOF_0P1K);
}



//************************************************************************
// void exclude_int(hLocal, int_num)
//
// Purpose
//
//      This function is called whenever an interrupt is generated by an
//  emulated program.  It calls upon the exclude_item function if one of the
//  specified interrupts are used.
//
// Parameters:
//
//      PAMLHANDLE      hLocal          A local PAM handle
//      BYTE            int_num         Which interrupt was generated (13, 21,
//                                      etc.)
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************


void exclude_int(PAMLHANDLE hLocal, BYTE int_num)
{
	switch (int_num)
	{
		case 0x05:
			exclude_item(hLocal, BAD_INT_05);
			break;
		case 0x10:
			exclude_item(hLocal, BAD_INT_10);
			break;
		case 0x12:
			exclude_item(hLocal, BAD_INT_12);
			break;
		case 0x13:
			exclude_item(hLocal, BAD_INT_13);
			break;
		case 0x16:
			exclude_item(hLocal, BAD_INT_16);
			break;
		case 0x17:
			exclude_item(hLocal, BAD_INT_17);
			break;
        case 0x1A:
            exclude_item(hLocal, BAD_INT_1A);
            break;
        case 0x20:
            exclude_item(hLocal, BAD_INT_20);
            break;
        case 0x21:
            exclude_item(hLocal, BAD_INT_21);
            break;
        case 0x25:
            exclude_item(hLocal, BAD_INT_25);
            break;
        case 0x26:
            exclude_item(hLocal, BAD_INT_26);
            break;
        case 0x27:
            exclude_item(hLocal, BAD_INT_27);
            break;
        case 0x2F:
            exclude_item(hLocal, BAD_INT_2F);
            break;
        case 0x33:
            exclude_item(hLocal, BAD_INT_33);
            break;
        case 0x67:
            exclude_item(hLocal, BAD_INT_67);
            break;
    }
}



//************************************************************************
// void exclude_image_size(hLocal, file_size)
//
// Purpose
//
//      This static exclusion excludes virus profiles based on the size of the
//  target file.  File size exclusion occurs at several boundaries: 1K, 2K, 4K,
//  and 8K.
//
// Parameters:
//
//      PAMLHANDLE      hLocal          A local PAM handle
//      DWORD           file_size       Size of the target file
//
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

void exclude_image_size(PAMLHANDLE hLocal, DWORD file_size)
{
    if (file_size < 8192)
        exclude_item(hLocal,MIN_SIZE_8K);
    if (file_size < 4096)
        exclude_item(hLocal,MIN_SIZE_4K);
    if (file_size < 2048)
        exclude_item(hLocal,MIN_SIZE_2K);
    if (file_size < 1024)
		exclude_item(hLocal,MIN_SIZE_1K);
}


//************************************************************************
// PAMSTATUS context_init_exclude(hLocal)
//
// Purpose
//
//      This function is called before each file is scanned.  It allocates
//  memory for the file's candidate exclusion bitmap.  context_close_exclude
//  should only be called (after emulation) if this function returns
//  PAMSTATUS_OK.
//
// Parameters:
//
//      PAMLHANDLE      hLocal          A local PAM handle
//
// Returns:
//
//      PAMSTATUS_OK            If the memory was allocated
//      PAMSTATUS_MEM_ERROR     If the memory could not be allocated
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

PAMSTATUS context_init_exclude(PAMLHANDLE hLocal)
{
    hLocal->candidates.bitmap =
        (WORD *)PAMMemoryAlloc(hLocal->hGPAM->exclude_info.cand_words * sizeof(WORD));

    if (hLocal->candidates.bitmap == NULL)
        return(PAMSTATUS_MEM_ERROR);

    return(PAMSTATUS_OK);
}


//************************************************************************
// PAMSTATUS context_close_exclude(hLocal)
//
// Purpose
//
//      This function is called during local close.  It frees
//  memory used by the file's candidate exclusion bitmap.
//
// Parameters:
//
//      PAMLHANDLE      hLocal          A local PAM handle
//
// Returns:
//
//      PAMSTATUS_OK            If the memory was allocated
//
//************************************************************************
// 2/22/96 Carey created.
//************************************************************************

PAMSTATUS context_close_exclude(PAMLHANDLE hLocal)
{
    if (hLocal->candidates.bitmap != NULL)
        PAMMemoryFree(hLocal->candidates.bitmap);

    return(PAMSTATUS_OK);
}
