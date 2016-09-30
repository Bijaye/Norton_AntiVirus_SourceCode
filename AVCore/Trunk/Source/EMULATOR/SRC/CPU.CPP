// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/cpu.cpv   1.43   13 Sep 1996 11:37:36   CNACHEN  $
//
// Description:
//
//  This source file contains the bulk of the CPU emulator's core routines.
//  It contains routines to abstracts the CPU data structures, to interface
//  with the paging system and to initialize the CPU emulator for a emulation.
//  It also contains the "large switch" statement which decodes each
//  instruction.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/cpu.cpv  $
//
//    Rev 1.43   13 Sep 1996 11:37:36   CNACHEN
// Fixed prefetch queue bug with COM files.
//
//    Rev 1.42   09 Aug 1996 18:03:20   CNACHEN
// Fixed a problem with computation of header size in EXE files.
//
//    Rev 1.41   04 Apr 1996 11:00:38   CNACHEN
// Changed functions for IN AL, CONST and IN AL, DX instructions.  Now
// pass in opcode as an argument so we can set the EAX/AX/AL register to
// zero.
//
//
//    Rev 1.40   22 Mar 1996 16:08:16   CNACHEN
// Removed exclusion for reading from SEG:FFFF.
//
//    Rev 1.39   15 Mar 1996 13:27:06   CNACHEN
// Added INVALID_INDEX exclusions...
//
//    Rev 1.38   04 Mar 1996 16:00:18   CNACHEN
// (1) Fixed (WORD)-1 check to (UINT)-1 check for FileRead.
// (2) Updated local_init_* to initialize FS and GS registers to 0 on startup.
// (3) Updated LOCK instruction so it doesn't reset all prefixes/overrides,
//     since it is a prefix itself...
//
//    Rev 1.37   01 Mar 1996 12:21:54   CNACHEN
// Added JUMP/CALL following for COM-file signature-based exclusions.
//
//    Rev 1.36   28 Feb 1996 13:56:12   CNACHEN
// Added error checking for all file seeks/reads in local_init*
//
//    Rev 1.35   21 Feb 1996 10:30:40   CNACHEN
// Fixed some errors in the comments.
//
//    Rev 1.34   21 Feb 1996 10:21:50   CNACHEN
// Finished adding comments...
//
//    Rev 1.33   20 Feb 1996 19:36:26   CNACHEN
// Partiall commented CPU.CPP.. More to come.  Also changed the init_cpu_sys
// function to check for exclusion signatures at the entry-point of SYS
// files...
//
//    Rev 1.32   02 Feb 1996 15:03:38   RWALDIN
// Changed Local_flags to local_flag
//
//    Rev 1.31   02 Feb 1996 14:59:06   CNACHEN
// No error on zero byte files now...
//
//    Rev 1.30   02 Feb 1996 12:36:36   CNACHEN
// fixed bug with EXE loadimage computation.
//
//    Rev 1.29   02 Feb 1996 11:43:56   CNACHEN
// Added new dwFlags and revamped all exclusion checking...
//
//
//    Rev 1.28   23 Jan 1996 14:40:48   DCHI
// Added code to initialize CSIP queue.
//
//    Rev 1.27   23 Jan 1996 11:08:00   CNACHEN
// Fixed bug with JUMP flag test...
//
//    Rev 1.26   22 Jan 1996 17:22:30   DCHI
// Added initialization of instructionFetchCount.
//
//    Rev 1.25   22 Jan 1996 13:40:14   CNACHEN
// Added new fault support and also added E9 checking for first
// 4 bytes of COM Files instead of just the first byte.
//
//    Rev 1.24   12 Jan 1996 12:05:42   CNACHEN
// Added floating point support (to skip over float instructions)
//
//    Rev 1.23   05 Jan 1996 18:08:18   CNACHEN
// keep on fixing that bug! :)
//
//
//    Rev 1.22   05 Jan 1996 18:02:00   CNACHEN
// Fixed SIB decoding when INDEX = 4 for non ESP.
//
//    Rev 1.21   27 Dec 1995 11:31:24   DCHI
// Corrected image size calculation for EXE.
//
//    Rev 1.20   20 Dec 1995 16:23:08   CNACHEN
// Fixed bug where image size (From header) is greater than actual image size
// on disk.
//
//    Rev 1.19   20 Dec 1995 12:19:30   CNACHEN
// Added 0F,AF, 69, 6B IMUL instructions...
//
//    Rev 1.18   19 Dec 1995 19:07:54   CNACHEN
// Added prefetch queue support!
//
//
//    Rev 1.17   14 Dec 1995 13:02:46   CNACHEN
// Added support for control relinquishing under NLM...
//
//    Rev 1.16   14 Dec 1995 10:49:30   CNACHEN
// Fixed repair stuff...
//
//    Rev 1.15   14 Dec 1995 10:03:52   CNACHEN
// Fixed flags for interrupts (into) and initialization
//
//    Rev 1.14   13 Dec 1995 11:58:04   CNACHEN
// All File and Memory functions now use #defined versions with PAM prefixes
//
//    Rev 1.13   13 Dec 1995 10:12:30   CNACHEN
// Fixed LONG->long
//
//    Rev 1.12   11 Dec 1995 14:28:44   CNACHEN
// Caused emulator to reset REP status before emulation!
//
//    Rev 1.11   28 Nov 1995 11:07:24   CNACHEN
// Zero buffer before reading in file in local_init_cpu.
// This guarantees that if the file is < 512 bytes, we don't
// have any residual garbage in the buffer which might be
// detected!
//
//    Rev 1.10   26 Oct 1995 14:09:32   CNACHEN
// Added memory swapping instead of disk swapping if NULL is passed in for
// the swap-file filename.
//
//    Rev 1.9   19 Oct 1995 18:23:34   CNACHEN
// Initial revision... with comment header :)
//
//************************************************************************


#include "pamapi.h"

/* code */

//************************************************************************
// void init_faults(hLocal)
//
// Purpose
//
//      This function is called every time a new file is scanned.  It is used
//  to unsuspend all faults.
//
//  Faults can be disabled during the emulation of a sample file.  A given
//  fault can be disabled for a certain number of "fault hits."  In other
//  words, if the fault is disabled for 10 hits
//  (hLocal->CPU.suspend_fault[fault_num] = 10),
//  then the fault must have its criteria satisfied 10 times before the actual
//  fault will take place.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


void init_faults(PAMLHANDLE hLocal)
{
    WORD i;

    for (i=0;i<NUM_FAULTS;i++)
        hLocal->CPU.suspend_fault[i] = 0;
}

// prefetch queue stuff first! :)


//************************************************************************
// void reset_prefetch_queue(hLocal)
//
// Purpose
//
//      This function resets the state of the prefetch queue.  It does this
//  by setting a flag bit which requests that the prefetch queue be reloaded
//  as soon as a byte is feteched for execution.  This function does not
//  effect the sizeof the prefetch queue (which should have already been set
//  during initialization).
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void reset_prefetch_queue(PAMLHANDLE hLocal)
{
    hLocal->CPU.prefetch.wFlags |= PREFETCH_FLAG_RELOAD;
}


//************************************************************************
// void suspend_prefetch_queue(hLocal)
//
// Purpose
//
//      This function suspends the prefetch queue and is used during faults.
//  During a fault, the state of the machine should not be modified in any
//  way.  This includes the prefetch queue.  Therefore, when a fault occurs,
//  this function is called to suspend all modifications to the queue until
//  the fault is over.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void suspend_prefetch_queue(PAMLHANDLE hLocal)
{
    hLocal->CPU.prefetch.wFlags |= PREFETCH_FLAG_FAULT;
}


//************************************************************************
// void enable_prefetch_queue(hLocal)
//
// Purpose
//
//      This function is used to re-enable the prefetch queue when a fault
//  issues a fret instruction (fault-return).
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void enable_prefetch_queue(PAMLHANDLE hLocal)
{
    hLocal->CPU.prefetch.wFlags &= ~(WORD)PREFETCH_FLAG_FAULT;
}


//************************************************************************
// void reload_prefetch_queue(hLocal)
//
// Purpose
//
//      This function can be called to reload the prefetch queue.
//  Depending on the hLocal->CPU.prefetch.wCurrentRequest value, it will set
//  the size of the prefetch queue and then fill the queue with the
//  appropriate bytes found at the CS:IP in the VM.  This is called whenever
//  the PREFETCH_FLAG_RELOAD bit is set. (e.g. on a reset_prefetch_queue()
//  call.)  It is called exclusively by the fetch_byte_from_prefetch_queue()
//  function.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


void reload_prefetch_queue(PAMLHANDLE hLocal)
{
    WORD            i, wSize;
    prefetch_type   *pQueue;

    pQueue = &hLocal->CPU.prefetch;

    switch (pQueue->wCurrentRequest)
    {
        case PREFETCH_0_BYTE:
            pQueue->wSize = 0;
            break;
        case PREFETCH_8_BYTE:
            pQueue->wSize = 8;
            break;
        case PREFETCH_16_BYTE:
            pQueue->wSize = 12;     // unlikely that more than 12 bytes
            break;                  // are in the queue at any one time
        case PREFETCH_32_BYTE:
            pQueue->wSize = 28;     // unlikely that more than 28 bytes
            break;                  // are in the queue at any one time
    }

    pQueue->wFlags &= (WORD)~PREFETCH_FLAG_RELOAD;
    wSize = pQueue->wSize;


    for (i=0;i<wSize;i++)
        pQueue->wQueue[i] = (WORD)get_byte(hLocal,
                                           hLocal->CPU.CS,
                                           (WORD)(hLocal->CPU.IP + i));
    pQueue->wTail = 0;
    pQueue->wHead = wSize;
}


//************************************************************************
// BYTE fetch_byte_from_prefetch_queue(hLocal)
//
// Purpose
//
//      This function retrieves a byte from the prefetch queue, if it is
//  currently active.  If the prefetch queue is disabled (or is 0 bytes in
//  size), then the CS:IP byte is fetched directly from virtual memory.  This
//  function will first reload the prefetch queue by calling
//  reload_prefetch_queue if the prefetch queue has been invalidated for
//  any reason (e.g. by executing a jump instruction).
//
//  The function will also check to see if the byte fetched from the prefetch
//  queue was modified during emulation.  If modification is detected, the
//  function will set hLocal->CPU.prefetch.wNextRequest variable to request
//  another emulation, if a virus is not detected during this emulation.
//
//  Finally, the function updates the IP and the prefetch queue head and tail
//  indices.  Every time a byte is fetched from the queue, the queue has an
//  additional byte added to its head.  This differs from actual hardware
//  prefetch queues which erratically load bytes whenever memory is free.
//
//  NOTE: The prefetch queue array is purposefully larger than the maximum
//        size of the prefetch queue by at least 2 entries.  Therefore, roll-
//        back of up to two fetched bytes from CS:IP can be performed.  This
//        roll-back is used when a fault-instruction is detected and a fault
//        is called.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      The BYTE fetched from the prefetch queue or from the VM (at CS:IP).
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

BYTE fetch_byte_from_prefetch_queue(PAMLHANDLE hLocal)
{
    prefetch_type   *pQueue;
    WORD            wNextByte;

    pQueue = &hLocal->CPU.prefetch;

    if (PREFETCH_0_BYTE == pQueue->wCurrentRequest ||
        (pQueue->wFlags & PREFETCH_FLAG_FAULT))
        return(get_byte(hLocal,hLocal->CPU.CS,hLocal->CPU.IP++));

    if (pQueue->wFlags & PREFETCH_FLAG_RELOAD)
        reload_prefetch_queue(hLocal);

    wNextByte = pQueue->wQueue[pQueue->wTail];

    if (wNextByte & PREFETCH_MODIFIED)
    {
        switch (pQueue->wCurrentRequest)
        {
            case PREFETCH_8_BYTE:
                pQueue->wNextRequest = PREFETCH_0_BYTE;
                break;
            case PREFETCH_16_BYTE:
                pQueue->wNextRequest = PREFETCH_8_BYTE;
                break;
            case PREFETCH_32_BYTE:
                pQueue->wNextRequest = PREFETCH_16_BYTE;
                break;
        }
    }

    pQueue->wTail++;
    if (pQueue->wTail == MAX_PREFETCH_QUEUE_SIZE)
		pQueue->wTail = 0;

    if (pQueue->wHead == MAX_PREFETCH_QUEUE_SIZE)
        pQueue->wHead = 0;

    pQueue->wQueue[pQueue->wHead] = (WORD)get_byte(hLocal,
                                                   hLocal->CPU.CS,
                                                   (WORD)(hLocal->CPU.IP +
                                                          pQueue->wSize));

    pQueue->wHead++;

    hLocal->CPU.IP++;                   // just fetched a byte from the
                                        // instruction stream...

    return((BYTE)(wNextByte & 0xFF));
}


//************************************************************************
// void roll_back_prefetch(hLocal)
//
// Purpose
//
//      This function rolls back the state of the prefetch queue, but does
//  not affect the IP.  It is currently being called only by the
//  instruction_fault function.  If a fault is detected and triggers, the
//  prefetch queue is rolled back (as is the IP, seperately) so that the
//  offending instruction is re-fetched again after the fault returns
//  (Assuming the fault does not skip over the instruction itself.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//  nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void roll_back_prefetch(PAMLHANDLE hLocal)
{
    if (hLocal->CPU.prefetch.wHead)
        hLocal->CPU.prefetch.wHead--;
    else
        hLocal->CPU.prefetch.wHead = MAX_PREFETCH_QUEUE_SIZE;

    if (hLocal->CPU.prefetch.wTail)
        hLocal->CPU.prefetch.wTail--;
    else
        hLocal->CPU.prefetch.wTail = MAX_PREFETCH_QUEUE_SIZE - 1;
}































//************************************************************************
// PAMSTATUS local_init_cpu(hLocal,hFile,scan_flags,entry_num,num_entry)
//
// Purpose
//
//      This function initializes the CPU and readies the emulator for a new
//  sample.  This initialization is performed once for each file scanned by
//  PAM.  Among others, it initializes the following items:
//
//      1. The CS:IP queue.  This is a queue of the last 64 CS:IP values.
//         It can be used for code analysis.
//      2. Iteration counts.
//      3. Segment, operand, and address override prefixes.
//      4. The number of writes that have been made during the current
//         emuluation to virtual memory.
//      5. The prefetch queue.
//      6. The paging sub-system.
//      7. The exclusion sub-system.
//      8. The "instruction run" count.  (used to detect data and text files)
//      9. All CPU flags.
//      10. The "invalid index" detection system.
//
//  The function sets the hLocal->CPU.stream variable equal to the passed-in
//  file handle.  This allows PAM to use the target file as a read-only swap
//  file.
//
//  The function then calls upon one of the 3 specific initialization functions
//  to set up the proper register values and perform file-specific exclusions.
//
//  If the file is 0 bytes long, it will be excluded from scanning at this
//  point.
//
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      HFILE       hFile           The file handle of the target file
//      WORD        scan_flags      Special information about the file
//                                  Values:  FLAG_SYS_FILE - indicates that
//                                           the file to be scanned is a SYS
//                                           format file, even if the header
//                                           does not start with FFFF
//      WORD        entry_num       Only meaningful for SYS files.  Specifies
//                                  which entrypoint should be emulated.
//                                  (strategy or interrupt)
//      WORD        *num_entry      This value is set to 2 when SYS files are
//                                  scanned.  This tells the calling function
//                                  how many times we need to emulate the file
//                                  to cover all entrypoints.
//
// Returns:
//
//      PAMSTATUS_OK                On success
//      PAMSTATUS_FILE_ERROR        On file error
//      PAMSTATUS_MEM_ERROR         On memory error
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

PAMSTATUS   local_init_cpu(PAMLHANDLE hLocal,
                           HFILE hFile,
                           WORD scan_flags,
                           WORD entry_num,
                           WORD *num_entry)
{
    WORD    *wFirstWord, wBytesRead;
	BYTE    buffer[SECTOR_SIZE];

    hLocal->CPU.stream = hFile;

    /* are there any bytes in the file? */

    if (PAMFileSeek(hLocal->CPU.stream,0,SEEK_SET) != 0)
        return(PAMSTATUS_FILE_ERROR);

    MEMSET(buffer,0,SECTOR_SIZE);       // make sure there's nothing there
                                        // after our file data

    wBytesRead = PAMFileRead(hLocal->CPU.stream,buffer,SECTOR_SIZE);

    if (wBytesRead == (WORD)-1)
        return(PAMSTATUS_FILE_ERROR);   // error check

    hLocal->dwFlags = 0;                // just reset all flags.
    hLocal->instr_run = 0;              // instruction run count = 0

    // Initialize CSIP queue

    hLocal->csip_queue.wIndex = 0;
    MEMSET(hLocal->csip_queue.dwCSIP,0xFF,sizeof(DWORD)*CSIP_QUEUE_SIZE);

    hLocal->CPU.iteration = 1;          // start with iteration #1
    hLocal->CPU.instructionFetchCount = 0; // no instructions fetched yet
    hLocal->CPU.num_writes = 0;         // no mem writes yet!
    hLocal->CPU.next_iter_stop = ITER_CHECK_POINT;
    hLocal->CPU.next_nlm_iter_stop = 0; // stop immediately

    hLocal->CPU.seg_over = DEFAULT_SEG; // remember to zero out our overrides!
    hLocal->CPU.op_size_over = FALSE;
    hLocal->CPU.addr_size_over = FALSE;
    hLocal->CPU.rep_prefix = FALSE;

    local_init_paging(hLocal);
    local_init_exclude(hLocal);

    hLocal->CPU.prefetch.wFlags = 0;    // make sure we start clean...
    hLocal->CPU.prefetch.wSize = 0;     // assume 0 byte prefetch queue until
                                        // we get_byte_at_csip and set it
                                        // properly


    // initialize the invalid index system

    hLocal->CPU.wIndexStatus = 0;               // all indices are invalid
    hLocal->CPU.dwBadIndexIteration = (DWORD)-1;// no last bad instruction

    reset_prefetch_queue(hLocal);       // prepare prefetch queue for emulation
    init_faults(hLocal);                // clear all fault info...

    wFirstWord = (WORD *)buffer;

    if (*wFirstWord == 0x4d5a || *wFirstWord == 0x5a4d)
        init_cpu_exe(hLocal,buffer);
    else if (scan_flags & FLAG_SYS_FILE || *wFirstWord == 0xFFFFU)
	{
		if (num_entry != NULL)
			*num_entry = 2;
        init_cpu_sys(hLocal,buffer, entry_num);
    }
    else
        init_cpu_com(hLocal,buffer);

    if (wBytesRead == 0)
        hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;

    return(PAMSTATUS_OK);
}




//************************************************************************
// PAMSTATUS locate_com_entry(hLocal,lpByBuffer)
//
// Purpose
//
//      This function is used to locate quiescent bytes in COM files.  It
//  will follow all E9, E8 and EB jumps/calls (similar to FSTART) up to
//  MAX_COM_JUMP_DEPTH (32) levels deep.  It will then provide the buffer
//  for init_cpu_com to examine for static exclusion signatures.  If this
//  function encounters file errors, it will set the appropriate flag in
//  hLocal->dwFlags.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      LPBYTE      lpbyBuffer      32-byte buffer to fill with host file
//                                  bytes (0 padded)
//                                  (SIG_EXCLUDE_LEN == 32)
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void locate_com_entry(PAMLHANDLE hLocal, LPBYTE lpbyBuffer)
{
    WORD wDepth = 0, wIP = COM_START_OFF;

    while (wDepth < MAX_COM_JUMP_DEPTH)
    {
        if (lpbyBuffer[0] == 0xE8 || lpbyBuffer[0] == 0xE9)
            wIP += *((LPWORD)(lpbyBuffer+1)) + 3;
        else if (lpbyBuffer[0] == 0xEB)
            wIP += (WORD)(signed short)
                    (signed char)*((LPBYTE)(lpbyBuffer+1)) + 2;
        else
            return;

        MEMSET(lpbyBuffer,0,SIG_EXCLUDE_LEN);

        if (wIP < COM_START_OFF)
            return;

        if (PAMFileSeek(hLocal->CPU.stream,wIP-COM_START_OFF,SEEK_SET) !=
            wIP-COM_START_OFF)
        {
            hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
            return;
        }

        if (PAMFileRead(hLocal->CPU.stream,
                                 lpbyBuffer,
                                 SIG_EXCLUDE_LEN) ==
            (UINT)-1)
        {
            hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
            return;
        }

        wDepth++;
    }
}


//************************************************************************
// void init_cpu_com(hLocal,buffer)
//
// Purpose
//
//      This function initializes the registers and flags of the CPU
//  specifically for a COM file.  It also performs various exclusions based
//  on the file type (COM) and the content of the entry-point instructions.
//  The function inserts the passed in sector (0 padded, not CD26) into the
//  paging system in page frame #0.
//
//  It checks whether or not the file is <= 64K-256 bytes.  If not, the file
//  is immediately excluded from scanning. (This is the max size for a COM
//  file).
//
//  It checks for the presence of an E9 byte (JUMP opcode) within the first
//  4 bytes of the entrypoint.  This is used for the "JUMP" exclusion.
//
//  It excludes files from scanning based on their file size.
//
//  It pushes a 0 on the stack as DOS would do before executing the program.
//
//  NOTE: All errors are set and propagated through the hLocal->dwFlags
//        variable.  It will have bits LOCAL_FLAG_MEM_ERROR or
//        LOCAL_FLAG_FILE_ERROR set on error.
//
//  NOTE: Even if a file is statically excluded during this function, the VM
//        will still be properly initialized.  This is required for the repair
//        system which does not care about exclusions.
//
//  It sets up the following variables:
//
//  hLocal->CPU.image_start_offset: Set to 0.  This indicates that the program
//                                  image in memory starts at offset 0 in the
//                                  actual COM file.
//  hLocal->CPU.offset_modifier:    Set to -8000h.  This value is added to all
//                                  linear addresses to determine whether or
//                                  not a memory reference falls before or
//                                  after the start of the load image of the
//                                  target file.  (NOTE: All files are loaded
//                                  in the PAM VM at linear offset 8000h.)
//
//
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        *byBuffer       Buffer containing the first sector of the
//                                  file.
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


void init_cpu_com(PAMLHANDLE hLocal, BYTE *buffer)
{
    BYTE byExcludeBuffer[SIG_EXCLUDE_LEN];

    hLocal->CPU.CS = hLocal->CPU.DS = hLocal->CPU.ES = hLocal->CPU.SS =
        COM_START_SEG;

    hLocal->CPU.FS = hLocal->CPU.GS = 0;        // starting values for FS, GS

    // initial register values

    hLocal->CPU.preg.D.EAX = hLocal->CPU.preg.D.EBX = 0;

    hLocal->CPU.preg.D.ECX = 0x000000FFL;

    // DX = PSP

    hLocal->CPU.preg.D.EDX = (DWORD)COM_START_SEG;

    hLocal->CPU.ireg.D.EBP = 0x0000091CL;

    hLocal->CPU.ireg.D.ESI = (DWORD)COM_START_OFF;

    hLocal->CPU.ireg.D.EDI = (DWORD)(STACK_START - sizeof(WORD));

                                            // CPU.ireg.X.DI = FFFE to start out
                                            // CPU.ireg.X.SP = 0, to start then
											// 0000 is pushed on stack to
											// make it FFFE

    hLocal->CPU.IP = COM_START_OFF;
    hLocal->CPU.ireg.D.ESP = (DWORD)STACK_START; // put stack pointer at EOS

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.D =
    hLocal->CPU.FLAGS.T = hLocal->CPU.FLAGS.S = hLocal->CPU.FLAGS.Z =
    hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.P = hLocal->CPU.FLAGS.C = FALSE;

    hLocal->CPU.FLAGS.I = TRUE;

    hLocal->CPU.image_start_offset = 0;     // top of file = start of image

    hLocal->CPU.offset_modifier  = -((long)COM_START_SEG * (long)PARAGRAPH +
                                     COM_START_OFF);

    insert_resident_page(hLocal,PHYS_LOAD_ADDR,buffer);

	// make sure we perform exclusions.  This function call will do nothing
	// if we are emulating in-house code.

    exclude_item(hLocal,BAD_COM);

	// if the COM program does NOT begin with a 3 byte JMP instruction, then
	// exclude the file from consideration for whatever viruses have this
	// requirement.

	hLocal->CPU.ulImageSize =
            PAMFileSeek(hLocal->CPU.stream,0,SEEK_END);

    if (hLocal->CPU.ulImageSize == (DWORD)-1)
        hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;   // error on seek...

    if (buffer[0] != 0xE9 && buffer[1] != 0xE9 &&
        buffer[2] != 0xE9 && buffer[3] != 0xE9)
	{
		exclude_item(hLocal,COM_JUMP_START);
    }

	exclude_image_size(hLocal,hLocal->CPU.ulImageSize);

	// exclude based on entry code in file...

    MEMCPY(byExcludeBuffer, buffer, SIG_EXCLUDE_LEN);

    locate_com_entry(hLocal, byExcludeBuffer);

    if (PExcludeTest(hLocal->hGPAM->sig_exclude_info, byExcludeBuffer) ==
		PAMSTATUS_NO_SCAN)
	{
		hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
	}

	if (hLocal->CPU.ulImageSize > MAX_COM_SIZE)
	{
		hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
	}

	/* store return address on stack which points to beginning of PSP for COM
	   files so RET will generate a int 20 */

	push_word(hLocal,0);
}



//************************************************************************
// void exclude_reloc_after_csip(hLocal, h)
//
// Purpose
//
//      This function checks whether the last relocation item of the EXE file
//  falls after the CS:IP entrypoint in the file.  This is used as an
//  exclusion to eliminate the file from consideration.  No current polymorphic
//  viruses have relocation items after their entrypoint.  However, many
//  normal programs do have reloc. items after their entry.  Therefore, PAM
//  can exclude many viruses from consideration if this condition is found.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      exeheader   *h              Buffer containing the EXE file header.
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void exclude_reloc_after_csip(PAMLHANDLE hLocal, exeheader *h)
{
	DWORD dwOffset, dwReloc, dwCSIP;

	if (0 == h->reloc_table_items)
		return;

	dwOffset = h->first_rel_offset +
			   (h->reloc_table_items - 1) * sizeof(DWORD);

    if ((DWORD)PAMFileSeek(hLocal->CPU.stream,dwOffset,SEEK_SET) != dwOffset)
	{
        hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
		return;
	}

	if (PAMFileRead(hLocal->CPU.stream,&dwReloc,sizeof(DWORD)) != sizeof(DWORD))
	{
		hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;
		return;
	}

	dwReloc = (dwReloc >> 12) + (dwReloc & 0xFFFFU);
	dwCSIP = ((DWORD)h->cs << 4) + h->ip;

	if (dwReloc > dwCSIP)
		exclude_item(hLocal,RELOC_AFTER_CSIP);
}



//************************************************************************
// void init_cpu_exe(hLocal,buffer)
//
// Purpose
//
//      This function initializes the registers and flags of the CPU
//  specifically for an EXE file.  It also performs various exclusions based
//  on the file type (EXE) and the content of the entry-point instructions.
//
//  It excludes files from scanning based on their file size.
//
//  It excludes files from scanning based on the distance from the entrypoint
//  to the EOF.  The EOF is determined from the header as opposed to the
//  actual file size.  This accounts for overlays.  If the actual image size
//  is less than the header-calculated image size, then the actual file size
//  is used instead to fix paging problems.
//
//  NOTE: All errors are set and propagated through the hLocal->dwFlags
//        variable.  It will have bits LOCAL_FLAG_MEM_ERROR or
//        LOCAL_FLAG_FILE_ERROR set on error.
//
//
//  NOTE: Even if a file is statically excluded during this function, the VM
//        will still be properly initialized.  This is required for the repair
//        system which does not care about exclusions.
//
//  It sets up the following variables:
//
//  hLocal->CPU.image_start_offset: This indicates that the program image in
//                                  memory starts at offset header_size * 16
//                                  in the actual EXE file.
//  hLocal->CPU.offset_modifier:    Set to -8000h.  This value is added to all
//                                  linear addresses to determine whether or
//                                  not a memory reference falls before or
//                                  after the start of the load image of the
//                                  target file.  (NOTE: All files are loaded
//                                  in the PAM VM at linear offset 8000h.)
//
//
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        *byBuffer       Buffer containing the first sector of the
//                                  file (i.e. the EXE header).
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void init_cpu_exe(PAMLHANDLE hLocal, BYTE *buffer)
{
	exeheader   *h;
	BYTE        entry_buffer[SIG_EXCLUDE_SIZE];
	WORD        i;
    DWORD       dwDiskImageSize, dwTemp;

	/* remember to check for invalid CS:IP entrypoint! */

    h = (exeheader *)buffer;


    hLocal->CPU.preg.D.EAX = hLocal->CPU.preg.D.EBX = 0;

    hLocal->CPU.preg.D.ECX = 0x000000FFL;

    // points to the PSP

    hLocal->CPU.preg.D.EDX = (DWORD)(EXE_START_SEG - 0x10);

    hLocal->CPU.ireg.D.EBP = 0x0000091CL;

    hLocal->CPU.ireg.D.ESI = (DWORD)h->ip;      // starting IP

    hLocal->CPU.ireg.D.EDI = (DWORD)h->sp;      // starting SP


    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.D =
    hLocal->CPU.FLAGS.T = hLocal->CPU.FLAGS.S = hLocal->CPU.FLAGS.Z =
    hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.P = hLocal->CPU.FLAGS.C = FALSE;

    hLocal->CPU.FLAGS.I = TRUE;

	// right after header/reloc. data = start of image

    hLocal->CPU.image_start_offset = ((DWORD)h->header_size * PARAGRAPH);

	/* this is the linear address of the start of the program in memory.  it
	   can be added to any address to determine the offset from the start of
	   the program image */

    hLocal->CPU.offset_modifier = -(long)EXE_START_SEG * (long)PARAGRAPH;

	/* set up CS:IP */

    hLocal->CPU.CS = h->cs + EXE_START_SEG;
    hLocal->CPU.IP = h->ip;

    hLocal->CPU.SS = h->ss + EXE_START_SEG;
    hLocal->CPU.ireg.D.ESP = (DWORD)h->sp;

    hLocal->CPU.DS = hLocal->CPU.ES = EXE_START_SEG - 0x10;

    hLocal->CPU.FS = hLocal->CPU.GS = 0;        // starting values for FS, GS

	// make sure we perform exclusions.

    exclude_item(hLocal,BAD_EXE);
	exclude_exe_entry_point(hLocal,h);

    hLocal->CPU.ulImageSize = (h->page_size * 512L) +
                       h->len_mod_512 - (long)h->header_size * PARAGRAPH;

	if (h->len_mod_512 != 0)
	{
		if (hLocal->CPU.ulImageSize >= 512L)
			hLocal->CPU.ulImageSize -= 512L;
		else
			hLocal->CPU.ulImageSize = 0;
	}

    // some EXE files may have smaller images than their size fields in the
    // exe header specify...

    dwTemp = PAMFileSeek(hLocal->CPU.stream,0,SEEK_END);

    if (dwTemp == (DWORD)-1)
        hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;   // error on seek...

    dwDiskImageSize = dwTemp - (DWORD)h->header_size * PARAGRAPH;

    if (hLocal->CPU.ulImageSize > dwDiskImageSize)
        hLocal->CPU.ulImageSize = dwDiskImageSize;

    exclude_image_size(hLocal, hLocal->CPU.ulImageSize);

    for (i=0;i<SIG_EXCLUDE_SIZE;i++)
		entry_buffer[i] = get_byte(hLocal,hLocal->CPU.CS,(WORD)(hLocal->CPU.IP+i));

	if (PExcludeTest(hLocal->hGPAM->sig_exclude_info, entry_buffer) ==
		PAMSTATUS_NO_SCAN)
	{
        hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
        return;
	}

    // now perform exclusion check for reloc item after CS:IP

    exclude_reloc_after_csip(hLocal, (exeheader *)buffer);

}


//************************************************************************
// void init_cpu_sys(hLocal,buffer,wEntryNum)
//
// Purpose
//
//      This function initializes the registers and flags of the CPU
//  specifically for a SYS file.  It also performs various exclusions based
//  on the file type (SYS). It also checks the content of the entry-point
//  instructions for common signatures.  If a common signature is found, the
//  SYS file is excluded from scanning.
//
//  It excludes files from scanning based on their file size.
//
//  It checks whether or not the file is <= 64K bytes.  If not, the file is
//  immediately excluded from scanning. (This is the max size for
//  a SYS file).
//
//  NOTE: All errors are set and propagated through the hLocal->dwFlags
//        variable.  It will have bits LOCAL_FLAG_MEM_ERROR or
//        LOCAL_FLAG_FILE_ERROR set on error.
//
//
//  NOTE: Even if a file is statically excluded during this function, the VM
//        will still be properly initialized.  This is required for the repair
//        system which does not care about exclusions.
//
//  It sets up the following variables:
//
//  hLocal->CPU.image_start_offset: This indicates that the program image in
//                                  memory starts at offset 0 in the SYS file.
//  hLocal->CPU.offset_modifier:    Set to -8000h.  This value is added to all
//                                  linear addresses to determine whether or
//                                  not a memory reference falls before or
//                                  after the start of the load image of the
//                                  target file.  (NOTE: All files are loaded
//                                  in the PAM VM at linear offset 8000h.)
//
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        *byBuffer       Buffer containing the first sector of the
//                                  file (i.e. the EXE header).
//      WORD        wEntryNum       Which entrypoint to start with (strategy
//                                  or interrupt)
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void init_cpu_sys(PAMLHANDLE hLocal,
				  BYTE *buffer,
                  WORD wEntryNum)
{
    sysheader               *h;
    BYTE                    entry_buffer[SIG_EXCLUDE_SIZE];
    WORD                    i;


    /* remember to check for invalid CS:IP entrypoint!  May want to
    set ES:BX to point to request header... As it turns out, the
    whole image of the SYS file is loaded into memory at CS:0 just like
    a COM file.  */

    hLocal->CPU.CS = hLocal->CPU.DS = hLocal->CPU.ES = hLocal->CPU.SS =
        SYS_START_SEG;

    hLocal->CPU.FS = hLocal->CPU.GS = 0;        // starting values for FS, GS

    hLocal->CPU.ireg.D.ESP = (DWORD)STACK_START; /* put stack pointer at EOS */

    hLocal->CPU.preg.D.EAX = hLocal->CPU.preg.D.EBX =
    hLocal->CPU.preg.D.ECX = hLocal->CPU.preg.D.EDX = 0;

    hLocal->CPU.ireg.D.ESI = hLocal->CPU.ireg.D.EDI = 0;
    hLocal->CPU.ireg.D.EBP = 0;

    hLocal->CPU.FLAGS.O = hLocal->CPU.FLAGS.D =
    hLocal->CPU.FLAGS.T = hLocal->CPU.FLAGS.S = hLocal->CPU.FLAGS.Z =
    hLocal->CPU.FLAGS.A = hLocal->CPU.FLAGS.P = hLocal->CPU.FLAGS.C = FALSE;

    hLocal->CPU.FLAGS.I = TRUE;

    h = (sysheader *)buffer;

    hLocal->CPU.image_start_offset = 0;

    hLocal->CPU.offset_modifier  = -(long)SYS_START_SEG * (long)PARAGRAPH;

    if (wEntryNum == 0)
        hLocal->CPU.IP = h->strategy_ip;
	else
        hLocal->CPU.IP = h->int_ip;

	// make sure we perform exclusions.

    exclude_item(hLocal,BAD_SYS);

    hLocal->CPU.ulImageSize = PAMFileSeek(hLocal->CPU.stream,0,SEEK_END);

    if (hLocal->CPU.ulImageSize == (DWORD)-1)
        hLocal->dwFlags |= LOCAL_FLAG_FILE_ERROR;   // error on seek...

    exclude_image_size(hLocal,hLocal->CPU.ulImageSize);

    // check for exclusion signatures...

    for (i=0;i<SIG_EXCLUDE_SIZE;i++)
		entry_buffer[i] = get_byte(hLocal,hLocal->CPU.CS,(WORD)(hLocal->CPU.IP+i));

	if (PExcludeTest(hLocal->hGPAM->sig_exclude_info, entry_buffer) ==
		PAMSTATUS_NO_SCAN)
	{
        hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
        return;
    }

    if (hLocal->CPU.ulImageSize > MAX_SYS_SIZE)
        hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
}



//************************************************************************
// void local_close_cpu(hLocal)
//
// Purpose
//
//      This function is called after a file has been scanned.  Basically, it
//  frees any memory that was dynamically allocated for paging during emulation
//  (repair or detection) of the host file.  This is the only shutdown item
//  that must be handled before another file can be scanned.  All other aspects
//  of the VM are reset during the call to local_init_cpu.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void local_close_cpu(PAMLHANDLE hLocal)
{
    local_close_paging(hLocal);
}

//************************************************************************
// BYTE get_byte(hLocal, seg, off)
//
// Purpose
//
//      This function is called to retrieve a byte from virtual memory.  This
// function always fetches bytes directly from the VM and not from the prefetch
// queue.  All addresses are masked to be below linear address 0xFFFFF.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg             Segment where to fetch byte
//      WORD        off             Offset where to fetch byte
//
// Returns:
//
//      BYTE fetched from VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


BYTE get_byte(PAMLHANDLE hLocal, WORD seg, WORD off)
{
	long 	global_offset, file_offset;
	BYTE	b;

	global_offset = (DWORD)seg * PARAGRAPH;
    global_offset += off;
    global_offset &= EFFECTIVE_ADDRESS_MASK;

    file_offset = global_offset + hLocal->CPU.offset_modifier;

    /* file case. if file_offset < 0 we're in low memory.  low memory is a
       memory image contained at ulLowStartOffset in the DATAFILE */

    if (file_offset < 0)
    {
        b = page_read_byte(hLocal,
                           global_offset,
                           hLocal->CPU.ulLowStartOffset,
                           global_offset, hLocal->CPU.low_stream);
        return(b);
	}

    b = page_read_byte(hLocal,
                       global_offset,
                       hLocal->CPU.image_start_offset,
					   file_offset,
                       hLocal->CPU.stream);
    return(b);
}


//************************************************************************
// WORD get_word(hLocal, seg, off)
//
// Purpose
//
//      This function is called to retrieve a word from virtual memory.  This
// function always fetches words directly from the VM and not from the prefetch
// queue.  All addresses are masked to be below linear address 0xFFFFF.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg             Segment where to fetch byte
//      WORD        off             Offset where to fetch byte
//
// Returns:
//
//      WORD fetched from VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


WORD get_word(PAMLHANDLE hLocal, WORD seg, WORD off)
{
	WORD w;

    w = get_byte(hLocal,seg,off);
    w += ((WORD)get_byte(hLocal, seg,(WORD)(off+1)) << 8);

    return(w);
}


//************************************************************************
// DWORD get_dword(hLocal, seg, off)
//
// Purpose
//
//      This function is called to retrieve a dword from virtual memory.  This
// function always fetches dwords directly from the VM and not from the
// prefetch queue.  All addresses are masked to be below linear address
// 0xFFFFF.  If a dword is fetched from offset 0xFFFF of any segment, the file
// is immediately excluded from further execution.  This type of fetch is
// invalid and will cause actual programs to crash (on an honest-to-goodness
// PC).
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg             Segment where to fetch byte
//      WORD        off             Offset where to fetch byte
//
// Returns:
//
//      DWORD fetched from VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD get_dword(PAMLHANDLE hLocal, WORD seg, WORD off)
{
	DWORD dw;

    dw = get_word(hLocal,seg,off);
	dw = ((DWORD)get_word(hLocal,seg,(WORD)(off+2)) << 16) + dw;

    return(dw);
}


//************************************************************************
// BYTE get_byte_at_csip(hLocal)
//
// Purpose
//
//      This function retrieves the next byte from the instruction stream. If
// a prefetch queue is active, the byte will be retrieved from the prefetch
// queue.  If not, the byte is retrieived directly from VM.  The
// hLocal->page_global.executable_fetch flag is set during all executable
// fetches.  This tells the paging system to mark the page containing the
// fetched byte as harboring an executable instruction, and will cause the
// page to be scanned for virus signatures.
//
// The IP is automatically updated.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      BYTE fetched from CS:IP from the prefetch queue or VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

BYTE get_byte_at_csip(PAMLHANDLE hLocal)
{
    BYTE b;

    hLocal->page_global.executable_fetch = TRUE;

    b = fetch_byte_from_prefetch_queue(hLocal);

    hLocal->page_global.executable_fetch = FALSE;

	return(b);
}

//************************************************************************
// WORD get_word_at_csip(hLocal)
//
// Purpose
//
//      This function retrieves the next word from the instruction stream. If
// a prefetch queue is active, the word will be retrieved from the prefetch
// queue.  If not, the word is retrieived directly from VM.  The
// hLocal->page_global.executable_fetch flag is set during all executable
// fetches.  This tells the paging system to mark the page containing the
// fetched word as harboring an executable instruction, and will cause the
// page to be scanned for virus signatures.
//
// The IP is automatically updated.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      WORD fetched from CS:IP from the prefetch queue or VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


WORD get_word_at_csip(PAMLHANDLE hLocal)
{
	WORD 			w;

    hLocal->page_global.executable_fetch = TRUE;

    w = (WORD)fetch_byte_from_prefetch_queue(hLocal);
    w |= (WORD)fetch_byte_from_prefetch_queue(hLocal) << 8;

    hLocal->page_global.executable_fetch = FALSE;

	return(w);
}

//************************************************************************
// DWORD get_dword_at_csip(hLocal)
//
// Purpose
//
//      This function retrieves the next dword from the instruction stream. If
// a prefetch queue is active, the dword will be retrieved from the prefetch
// queue.  If not, the dword is retrieived directly from VM.  The
// hLocal->page_global.executable_fetch flag is set during all executable
// fetches.  This tells the paging system to mark the page containing the
// fetched dword as harboring an executable instruction, and will cause the
// page to be scanned for virus signatures.
//
// The IP is automatically updated.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      DWORD fetched from CS:IP from the prefetch queue or VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


DWORD get_dword_at_csip(PAMLHANDLE hLocal)
{
	DWORD            dw;

    hLocal->page_global.executable_fetch = TRUE;

    dw =  (DWORD)fetch_byte_from_prefetch_queue(hLocal);
    dw |= (DWORD)fetch_byte_from_prefetch_queue(hLocal) << 8;
    dw |= (DWORD)fetch_byte_from_prefetch_queue(hLocal) << 16;
    dw |= (DWORD)fetch_byte_from_prefetch_queue(hLocal) << 24;

    hLocal->page_global.executable_fetch = FALSE;

    return(dw);
}


//************************************************************************
// BYTE put_byte(hLocal, seg, off, b)
//
// Purpose
//
//      This function writes the specified byte to the specified location in
// virtual memory.  It also checks whether or not this byte-write is within
// the prefetch queue region.  If so, it updates the prefetch queue. If at
// a later time, the modified byte is fetched from the prefetch queue, PAM
// can detect this and request further emulations with different prefetch
// queue sizes.  All addresses are masked to be below linear address
// 0xFFFFF.  All memory writes are counted (hLocal->CPU.num_writes), and
// if they exceed a threshold (hLocal->hGPAM->config_info.wMinWritesForScan),
// then PAM will scan all pages that were modified for virus signatures.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg             Segment of where to write
//      WORD        off             Offset of where to write
//      WORD        b               BYTE to write out
//
// Returns:
//
//      The BYTE written to VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

BYTE put_byte(PAMLHANDLE hLocal, WORD seg, WORD off, BYTE b)
{
    long    global_offset, file_offset;
    DWORD   dwCSAddr,
            dwTestAddr;
    WORD    wIndex,
            wTestOffset;

//	printf("Write %02X -> %04X:%04X\n",b,seg,off);

    if (hLocal->CPU.dwBadIndexIteration == hLocal->CPU.iteration)
        exclude_item(hLocal,INVALID_INDEX_WRITE);

    global_offset = (DWORD)seg * PARAGRAPH;
    global_offset += off;
    global_offset &= EFFECTIVE_ADDRESS_MASK;

    //  prefetch queue stuff: there are a number of cases.
    //  Rather than trying to separate all the cases,
    //      just check all the addresses that correspond to the queue.

    if (PREFETCH_0_BYTE != hLocal->CPU.prefetch.wCurrentRequest &&
        !(hLocal->CPU.prefetch.wFlags &
            (PREFETCH_FLAG_FAULT | PREFETCH_FLAG_RELOAD)))
    {
        dwCSAddr = (DWORD)hLocal->CPU.CS * PARAGRAPH;
        dwCSAddr &= EFFECTIVE_ADDRESS_MASK;
        wIndex = 0;
        wTestOffset = hLocal->CPU.IP;
        do
        {
            dwTestAddr = ( dwCSAddr + (DWORD) wTestOffset );
            dwTestAddr &= EFFECTIVE_ADDRESS_MASK;

            if( dwTestAddr == (DWORD) global_offset )
            {
                wIndex += hLocal->CPU.prefetch.wTail;
                if (wIndex >= MAX_PREFETCH_QUEUE_SIZE)
                    wIndex -= MAX_PREFETCH_QUEUE_SIZE;

                //  This test should always fail...
                //  I.e., this is equivalent to assert() error handling.
                if( wIndex >= MAX_PREFETCH_QUEUE_SIZE )
                {
                    hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
                    return(0);
                }

                hLocal->CPU.prefetch.wQueue[wIndex] |= PREFETCH_MODIFIED;
                break;
            }
            else
            {
                wIndex++;
                wTestOffset++;
            }
        } while( wIndex < hLocal->CPU.prefetch.wSize );
    }

    file_offset = global_offset + hLocal->CPU.offset_modifier;

    hLocal->CPU.num_writes++;           // inc total # of writes to memory

#ifdef SYM_NLM
    if ((hLocal->CPU.num_writes & 0x2F) == 0)
        PAMRelinquishControl();
#endif

    /* file case */

    if (file_offset < 0)
    {
        page_write_byte(hLocal, b, global_offset, 0, 0, hLocal->CPU.low_stream);
        return(b);
    }

    page_write_byte(hLocal,                 // send context info in
                    b,                      // b = byte to write
                    global_offset,          // linear offset in memory of byte
                    hLocal->CPU.image_start_offset, // offset in scanned file of
                                                    // the executable image (0 for COM
                                                    // and end of header in EXE)
                    file_offset,            // offset w/respect to the start
                                            // of the image
                    hLocal->CPU.stream);

    return(b);
}


//************************************************************************
// WORD put_word(hLocal, seg, off, w)
//
// Purpose
//
//      This function writes the specified word to the specified location in
// virtual memory.  It also checks whether or not this word-write is within
// the prefetch queue region.  If so, it updates the prefetch queue. If at
// a later time, the modified word is fetched from the prefetch queue, PAM
// can detect this and request further emulations with different prefetch
// queue sizes.  All addresses are masked to be below linear address
// 0xFFFFF.  All memory writes are counted (hLocal->CPU.num_writes), and
// if they exceed a threshold (hLocal->hGPAM->config_info.wMinWritesForScan),
// then PAM will scan all pages that were modified for virus signatures.  If
// the memory write is at the end of a segment (off=0xFFFF), all viruses are
// immediately excluded.  (This results in a crash on an actual PC.)
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg             Segment of where to write
//      WORD        off             Offset of where to write
//      WORD        w               WORD to write out
//
// Returns:
//
//      The WORD written to VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD put_word(PAMLHANDLE hLocal, WORD seg, WORD off, WORD w)
{
	if (off == 0xFFFF)
	{
        // if we fetch on a segment boundary, that's bad!

        if (!(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
        {
            hLocal->dwFlags |= LOCAL_FLAG_NO_CAND_LEFT;
            return(0);
        }
    }

    put_byte(hLocal,seg,(WORD)(off+1),(BYTE)(w >> 8));
    put_byte(hLocal,seg,off,(BYTE)(w & 0x00FFU));

    return(w);
}




//************************************************************************
// DWORD put_dword(hLocal, seg, off, dw)
//
// Purpose
//
//      This function writes the specified dword to the specified location in
// virtual memory.  It also checks whether or not this dword-write is within
// the prefetch queue region.  If so, it updates the prefetch queue. If at
// a later time, the modified dword is fetched from the prefetch queue, PAM
// can detect this and request further emulations with different prefetch
// queue sizes.  All addresses are masked to be below linear address
// 0xFFFFF.  All memory writes are counted (hLocal->CPU.num_writes), and
// if they exceed a threshold (hLocal->hGPAM->config_info.wMinWritesForScan),
// then PAM will scan all pages that were modified for virus signatures.  If
// the memory write is at the end of a segment (off=0xFFFF), all viruses are
// immediately excluded.  (This results in a crash on an actual PC.)
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg             Segment of where to write
//      WORD        off             Offset of where to write
//      DWORD       dw              DWORD to write out
//
// Returns:
//
//      The DWORD written to VM.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


DWORD put_dword(PAMLHANDLE hLocal, WORD seg, WORD off, DWORD dw)
{
    put_word(hLocal,seg,(WORD)(off+2),(WORD)(dw >> 16));
    put_word(hLocal,seg,off,(WORD)(dw & 0x0000FFFFL));

    return(dw);
}



//************************************************************************
// WORD push_word(hLocal, w)
//
// Purpose
//
//      This function pushes the specified word on the stack.  It updates
//  the SP accordingly.  This function resets the status of the
//  dwBadIndexIteration.  This is because a "PUSH [BX+SI]" type instruction
//  does a memory read from BX+SI, setting the invalid index iteration
//  (assuming BX or SI have not yet been initialized), then the memory write
//  takes place to the stack, which triggers the INVALID_INDEX exclusion...
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        w               WORD to push onto the stack.
//
// Returns:
//
//      The WORD pushed on the stack.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD push_word(PAMLHANDLE hLocal, WORD w)
{
    if (!(hLocal->dwFlags &
        (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
        hLocal->CPU.dwBadIndexIteration = (DWORD)-1;

    hLocal->CPU.ireg.X.SP -= sizeof(WORD);
    put_word(hLocal,hLocal->CPU.SS,hLocal->CPU.ireg.X.SP,w);

	return(w);
}



//************************************************************************
// WORD pop_word(hLocal)
//
// Purpose
//
//      This function ppops and returns the top word on the stack.  It updates
//  the SP accordingly.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      The WORD popped from the stack.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD pop_word(PAMLHANDLE hLocal)
{
    hLocal->CPU.ireg.X.SP += sizeof(WORD);
    return(get_word(hLocal,
                    hLocal->CPU.SS,
                    (WORD)(hLocal->CPU.ireg.X.SP - sizeof(WORD))));
}



//************************************************************************
// DWORD push_dword(hLocal, dw)
//
// Purpose
//
//      This function pushes the specified dword on the stack.  It updates
//  the SP accordingly.  This function resets the status of the
//  dwBadIndexIteration.  This is because a "PUSH [BX+SI]" type instruction
//  does a memory read from BX+SI, setting the invalid index iteration
//  (assuming BX or SI have not yet been initialized), then the memory write
//  takes place to the stack, which triggers the INVALID_INDEX exclusion...
//
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      DWORD       dw              DWORD to push onto the stack.
//
// Returns:
//
//      The DWORD pushed on the stack.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


DWORD push_dword(PAMLHANDLE hLocal, DWORD dw)
{
    if (!(hLocal->dwFlags &
        (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
        hLocal->CPU.dwBadIndexIteration = (DWORD)-1;

    hLocal->CPU.ireg.X.SP -= sizeof(DWORD);
    put_dword(hLocal,hLocal->CPU.SS,hLocal->CPU.ireg.X.SP,dw);

    return(dw);
}


//************************************************************************
// WORD pop_dword(hLocal)
//
// Purpose
//
//      This function ppops and returns the top dword on the stack.  It updates
//  the SP accordingly.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      The DWORD popped from the stack.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD pop_dword(PAMLHANDLE hLocal)
{
    hLocal->CPU.ireg.X.SP += sizeof(DWORD);
    return(get_dword(hLocal,
                     hLocal->CPU.SS,
                     (WORD)(hLocal->CPU.ireg.X.SP - sizeof(DWORD))));
}




//************************************************************************
// WORD sign_extend_byte(b)
//
// Purpose
//
//      This function sign extends the specified byte to a WORD.
//
// Parameters:
//
//      BYTE        b               BYTE to sign extend
//
// Returns:
//
//      The sign extended WORD value.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD sign_extend_byte(BYTE b)
{
	WORD w;

	if (b & 0x80)		/* check for high bit of byte */
		w = 0xff00;
	else
		w = 0;

	w += b;

	return(w);
}



//************************************************************************
// DWORD sign_extend_word(w)
//
// Purpose
//
//      This function sign extends the specified WORD to a DWORD.
//
// Parameters:
//
//      WORD        w               WORD to sign extend
//
// Returns:
//
//      The sign extended DWORD value.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD sign_extend_word(WORD w)
{
    DWORD dw;

    if (w & 0x8000U)       /* check for high bit of word */
        dw = 0xffff0000L;
	else
        dw = 0;

    dw += w;

    return(dw);
}


//************************************************************************
// DWORD dsign_extend_byte(b)
//
// Purpose
//
//      This function sign extends the specified BYTE to a DWORD.
//
// Parameters:
//
//      BYTE        b               BYTE to sign extend
//
// Returns:
//
//      The sign extended DWORD value.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD dsign_extend_byte(BYTE b)
{
    DWORD dw;

	if (b & 0x80)		/* check for high bit of byte */
        dw = 0xffffff00L;
	else
        dw = 0;

    dw += b;

    return(dw);
}



//************************************************************************
// void decode_amb_norm(hLocal, opcode, amb, mem_type, seg, off, reg)
//
// Purpose
//
//      This function uses the AMB byte from the instruction stream as well
//  and the actual opcode byte of the instruction to determine
//  the addressing mode, operand size, and addresses of the operands to the
//  instruction.  It may read in several additional immediate bytes to properly
//  decode each instruction.  It automatically updates the IP as required.
//  All fetching is done with the get_*_at_csip functions.
//
//  NOTE: This function uses 16-bit registers for its internal computation.
//        It is used only when a 67h opcode prefix is *NOT* specified.
//
//  NOTE: If memory is indexed with an index register (BX, SI, DI, BP) that
//        has not yet been initialized, the hLocal->CPU.dwBadIndexIteration
//        variable is set to the current iteration count.  If a subsequent
//        memory write is performed to this location, then appropriate viruses
//        will be excluded if they are not allowed to write using uninitialized
//        index registers.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        opcode          The first byte of the instruction
//      BYTE        amb             The amb byte of the instruction
//                                  AMB=Addressing Mode Byte
//      BYTE        *mem_type       This is filled with information about the
//                                  operands to the instructions (register or
//                                  memory, operand size, etc)
//                                  Bits:
//                                   WORD_SIZE: operands are WORDs
//                                   DWORD_SIZE: operands are DWORDs
//                                   (neither): operands are BYTEs
//                                   REGISTER: source operand is in a register
//
//      WORD        *seg            Segment of non-register operand
//      WORD        *off            Offset of non-register operand
//      WORD        *reg            Register number of destination
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


void decode_amb_norm(PAMLHANDLE hLocal,
                     BYTE       opcode,
                     BYTE       amb,
                     BYTE       *mem_type,
                     WORD       *seg,
                     WORD       *off,
                     WORD       *reg)
{
	WORD offset;

	*reg = REG_VALUE(amb);		/* dest */

    *mem_type = FALSE;          /* default - nothing being accessed */
	if (W_VALUE(opcode))
    {
        /* if it WAS a WORD operand without the operand prefix, then it
           becomes a DWORD operand with the 66h prefix */

        if (hLocal->CPU.op_size_over)
            *mem_type |= DWORD_SIZE;
        else
            *mem_type |= WORD_SIZE;
    }

	switch (MOD_VALUE(amb))		/* address mode byte */
	{
		case 0:		/* r/m != 110b, then this indicates that a register operand */
					/* is specified by the r/m field and must be BP, BX, SI or DI */
					/* or some combination of those */
			if (RM_VALUE(amb) == 6)		/* WORD offset follows */
                offset = get_word_at_csip(hLocal);
			else
				offset = 0;
			break;
		case 1:		/* 8-bit signed displacement follows */
            offset = sign_extend_byte(get_byte_at_csip(hLocal));
			break;
		case 2:		/* 16-bit signed displacement follows */
            offset = get_word_at_csip(hLocal);
			break;
		case 3:
			*off = RM_VALUE(amb);		/* source */
			*mem_type = REGISTER;
			if (W_VALUE(opcode))
            {
                /* if it WAS a WORD operand without the operand prefix, then it
                   becomes a DWORD operand with the 66h prefix */

                if (hLocal->CPU.op_size_over)
                    *mem_type |= DWORD_SIZE;
                else
                    *mem_type |= WORD_SIZE;
            }
			return;						/* all done! */

	}

	switch (RM_VALUE(amb))
	{
		case 0:			/* DS:[BX+SI] */
            *seg = hLocal->CPU.DS;
            *off = hLocal->CPU.preg.X.BX + hLocal->CPU.ireg.X.SI + offset;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_BX) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_SI) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

			break;
		case 1:			/* DS:[BX+DI] */
            *seg = hLocal->CPU.DS;
            *off = hLocal->CPU.preg.X.BX + hLocal->CPU.ireg.X.DI + offset;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_BX) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_DI) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

			break;
		case 2:			/* SS:[BP+SI] */
            *seg = hLocal->CPU.SS;
            *off = hLocal->CPU.ireg.X.BP + hLocal->CPU.ireg.X.SI + offset;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_BP) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_SI) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

			break;
		case 3:
            *seg = hLocal->CPU.SS;
            *off = hLocal->CPU.ireg.X.BP + hLocal->CPU.ireg.X.DI + offset;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_BP) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_DI) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

			break;
		case 4:
            *seg = hLocal->CPU.DS;
            *off = hLocal->CPU.ireg.X.SI + offset;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_SI) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

			break;
		case 5:
            *seg = hLocal->CPU.DS;
            *off = hLocal->CPU.ireg.X.DI + offset;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_DI) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

			break;
		case 6:
			if (MOD_VALUE(amb) == 0)		/* DS:[disp16] */
			{
                *seg = hLocal->CPU.DS;
				*off = offset;
			}
			else
			{								/* SS:[BP+dispX] */
                *seg = hLocal->CPU.SS;
                *off = hLocal->CPU.ireg.X.BP + offset;


                if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_BP) &&
                    !(hLocal->dwFlags &
                     (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                    hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;
			}
			break;
		case 7:
            *seg = hLocal->CPU.DS;
            *off = hLocal->CPU.preg.X.BX + offset;

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_BX) &&
                !(hLocal->dwFlags &
                (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                    hLocal->CPU.dwBadIndexIteration = hLocal->CPU.iteration;

			break;
	}
}




//************************************************************************
// void decode_sib(hLocal, amb, seg, off)
//
// Purpose
//
//      This function uses the AMB byte (passed in), along with the SIB
//  (Scaled Index Byte) of an instruction to determine the segment and offset
//  of addressed memory.
//
//  NOTE: This function uses 32-bit registers for its internal computation.
//        It is used only when a 67h opcode prefix is specified and the
//        SIB is present (indicated by the AMB byte).
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        amb             The AMB byte of the instruction
//                                  AMB=Addressing Mode Byte
//      WORD        *seg            Segment of instruction's operand
//      WORD        *off            Offset of instruction's operand
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void decode_sib(PAMLHANDLE hLocal,
                BYTE amb,
                WORD *seg,
                WORD *off)
{
    BYTE sib;
    WORD wShift;
    DWORD dwBase;

    sib = get_byte_at_csip(hLocal);

    wShift = SS_VALUE(sib);         // # of bits to shift index by

    if (BASE_VALUE(sib) == 5)
    {
        switch (MOD_VALUE(amb))
        {
            case 0:
                dwBase = get_dword_at_csip(hLocal);
                break;
            case 1:
                dwBase = dsign_extend_byte(get_byte_at_csip(hLocal)) +
                         hLocal->CPU.ireg.D.EBP;
                break;
            case 2:
                dwBase = get_dword_at_csip(hLocal) + hLocal->CPU.ireg.D.EBP;
                break;
            case 3:
                /* hmmmm */
                break;
        }
	}
	else
	{
        dwBase = dwreg_value(hLocal,(BYTE)(BASE_VALUE(sib)));

		switch (MOD_VALUE(amb))
		{
			case 1:
				dwBase += dsign_extend_byte(get_byte_at_csip(hLocal));
				break;
			case 2:
				dwBase += get_dword_at_csip(hLocal);
		}
	}

    if (INDEX_VALUE(sib) != 0x4)
        *off = (WORD)(dwreg_value(hLocal,
                                  (BYTE)(INDEX_VALUE(sib))) << wShift) +
                                    (WORD)dwBase;
    else
        *off = (WORD)dwBase;

	/* what segment should we use? */

	if (BASE_VALUE(sib) == 4 || (BASE_VALUE(sib) == 5 && MOD_VALUE(amb) != 0))
		*seg = hLocal->CPU.SS;
	else
		*seg = hLocal->CPU.DS;
}



//************************************************************************
// void decode_sib_off(hLocal, amb, off)
//
// Purpose
//
//      This function uses the AMB byte (passed in), along with the SIB
//  (Scaled Index Byte) of an instruction to determine the offset
//  of the addressed memory.  This offset is a 32-bit value and is only used
//  for the 32-bit version of the LEA instruction.
//
//  NOTE: This function uses 32-bit registers for its internal computation.
//        It is used only when a 67h opcode prefix is specified and the
//        SIB is present (indicated by the AMB byte) with the LEA instruction.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        amb             The AMB byte of the instruction
//                                  AMB=Addressing Mode Byte
//      DWORD       *off            Offset of instruction's operand
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void decode_sib_off(PAMLHANDLE hLocal, BYTE amb, DWORD *off)
{
    BYTE sib;
    WORD wShift;
    DWORD dwBase;

	sib = get_byte_at_csip(hLocal);

	wShift = SS_VALUE(sib);         // # of bits to shift index by

	if (BASE_VALUE(sib) == 5)
	{
		switch (MOD_VALUE(amb))
		{
			case 0:
				dwBase = get_dword_at_csip(hLocal);
				break;
			case 1:
				dwBase = dsign_extend_byte(get_byte_at_csip(hLocal)) +
						 hLocal->CPU.ireg.D.EBP;
				break;
			case 2:
				dwBase = get_dword_at_csip(hLocal) + hLocal->CPU.ireg.D.EBP;
				break;
			case 3:
				/* hmmmm */
				break;
		}
	}
	else
	{
        dwBase = dwreg_value(hLocal,(BYTE)(BASE_VALUE(sib)));

		switch (MOD_VALUE(amb))
		{
			case 1:
				dwBase += dsign_extend_byte(get_byte_at_csip(hLocal));
				break;
			case 2:
				dwBase += get_dword_at_csip(hLocal);
		}
	}

    if (INDEX_VALUE(sib) != 0x4)
        *off = (dwreg_value(hLocal, (BYTE)(INDEX_VALUE(sib))) << wShift) +
                dwBase;
    else
        *off = dwBase;
}


//************************************************************************
// void decode_amb_32(hLocal, opcode, amb, mem_type, seg, off, reg)
//
// Purpose
//
//      This function uses the AMB byte from the instruction stream as well
//  and the actual opcode byte of the instruction to determine
//  the addressing mode, operand size, and addresses of the operands to the
//  instruction.  It may read in several additional immediate bytes to properly
//  decode each instruction.  It automatically updates the IP as required.
//  All fetching is done with the get_*_at_csip functions.
//
//  NOTE: It is used only when a 67h opcode prefix is specified.
//        The output seg/off params are 16-bit because they are limited to 64K
//        in real-mode.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        opcode          The first byte of the instruction
//      BYTE        amb             The amb byte of the instruction
//                                  AMB=Addressing Mode Byte
//      BYTE        *mem_type       This is filled with information about the
//                                  operands to the instructions (register or
//                                  memory, operand size, etc)
//                                  Bits:
//                                   WORD_SIZE: operands are WORDs
//                                   DWORD_SIZE: operands are DWORDs
//                                   (neither): operands are BYTEs
//                                   REGISTER: source operand is in a register
//
//      WORD        *seg            Segment of non-register operand
//      WORD        *off            Offset of non-register operand
//      WORD        *reg            Register number of destination
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void decode_amb_32(PAMLHANDLE hLocal,
					 BYTE       opcode,
					 BYTE       amb,
					 BYTE       *mem_type,
					 WORD       *seg,
					 WORD       *off,
					 WORD       *reg)
{
    DWORD offset;

    *reg = REG_VALUE(amb);      /* dest */

    *mem_type = FALSE;          /* default - nothing being accessed */

	if (W_VALUE(opcode))
	{
		/* if it WAS a WORD operand without the operand prefix, then it
		   becomes a DWORD operand with the 66h prefix */

		if (hLocal->CPU.op_size_over)
			*mem_type |= DWORD_SIZE;
		else
			*mem_type |= WORD_SIZE;
	}

	switch (MOD_VALUE(amb))		/* address mode byte */
	{
		case 0:     /* r/m == 101b, then 32-bit displacement follows */
					/* r/m == 100b, then SIB byte follows */
					/* otherwise no offset follows */

			if (RM_VALUE(amb) == 5)     /* DWORD offset follows */
				offset = get_dword_at_csip(hLocal);
			else
				offset = 0;

			break;

		case 1:     /* 8-bit sign extended displacement */
					/* EAX, ECX, EDX, EBX, EBP, ESI or EDI used as index */

					/* if RM=4 we have a SIB byte before the immediate
					   8-bit displacement.  the 8-bit displacement will
					   be fetched when we process the SIB */

			if (RM_VALUE(amb) != 4)
				offset = dsign_extend_byte(get_byte_at_csip(hLocal));

			break;

		case 2:     /* 32-bit signed displacement follows, even if SIB
					   is used */
					/* EAX, ECX, EDX, EBX, EBP, ESI or EDI used as index */

					/* if RM=4 we have a SIB byte before the immediate
					   32-bit displacement.  the 32-bit displacement will
					   be fetched when we process the SIB */

			if (RM_VALUE(amb) != 4)
				offset = get_dword_at_csip(hLocal);

			break;

		case 3:

			*off = RM_VALUE(amb);		/* source */
			*mem_type = REGISTER;
			if (W_VALUE(opcode))
			{
                /* if it WAS a WORD operand without the operand prefix, then it
                   becomes a DWORD operand with the 66h prefix */

                if (hLocal->CPU.op_size_over)
                    *mem_type |= DWORD_SIZE;
				else
                    *mem_type |= WORD_SIZE;
            }

			return;						/* all done! */

	}

    /* all addressing must be within the range of 0-64K, so we can use the
       normal X registers and not worry about the high WORD of the E
       registers */

	switch (RM_VALUE(amb))
	{
        case 0:         /* DS:[EAX] */
            *seg = hLocal->CPU.DS;
            *off = (WORD)(hLocal->CPU.preg.X.AX + offset);
			break;
		case 1:         /* DS:[ECX] */
			*seg = hLocal->CPU.DS;
			*off = (WORD)(hLocal->CPU.preg.X.CX + offset);
			break;
		case 2:         /* DS:[EDX] */
			*seg = hLocal->CPU.DS;
			*off = (WORD)(hLocal->CPU.preg.X.DX + offset);
			break;
		case 3:         /* DS:[EBX] */
			*seg = hLocal->CPU.DS;
			*off = (WORD)(hLocal->CPU.preg.X.BX + offset);
			break;
		case 4:
			/* deal with SIB byte */

			decode_sib(hLocal, amb, seg, off);

			break;

        case 5:

            if (MOD_VALUE(amb) == 0)        /* DS:[disp32] */
			{
                *seg = hLocal->CPU.DS;
                *off = (WORD)offset;
			}
			else
            {                               /* SS:[BP+disp32/SIGN(disp8)] */
                *seg = hLocal->CPU.SS;
                *off = (WORD)(hLocal->CPU.ireg.X.BP + offset);
			}

			break;

        case 6:                             /* DS:[ESI + Disp32/Disp8/none] */

            *seg = hLocal->CPU.DS;
			*off = (WORD)(hLocal->CPU.ireg.X.SI + offset);

			break;

		case 7:                             /* DS:[EDI + Disp32/Disp8/none] */

			*seg = hLocal->CPU.DS;
			*off = (WORD)(hLocal->CPU.ireg.X.DI + offset);

			break;
	}
}



//************************************************************************
// void decode_amb_32_off(hLocal, opcode, amb, mem_type, seg, off, reg)
//
// Purpose
//
//      This function uses the AMB byte from the instruction stream as well
//  and the actual opcode byte of the instruction to determine
//  the addressing mode, operand size, and addresses of the operands to the
//  instruction.  It may read in several additional immediate bytes to properly
//  decode each instruction.  It automatically updates the IP as required.
//  All fetching is done with the get_*_at_csip functions.
//
//  NOTE: It is used only when a 67h opcode prefix is specified with an LEA
//        instruction.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        opcode          The first byte of the instruction
//      BYTE        amb             The amb byte of the instruction
//                                  AMB=Addressing Mode Byte
//      BYTE        *mem_type       This is filled with information about the
//                                  operands to the instructions (register or
//                                  memory, operand size, etc)
//                                  Bits:
//                                   WORD_SIZE: operands are WORDs
//                                   DWORD_SIZE: operands are DWORDs
//                                   (neither): operands are BYTEs
//                                   REGISTER: source operand is in a register
//
//      WORD        *seg            Segment of non-register operand
//      DWORD       *off            DWORD offset of non-register operand
//      WORD        *reg            Register number of destination
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void decode_amb_32_off(PAMLHANDLE hLocal,
                       BYTE       opcode,
                       BYTE       amb,
                       BYTE       *mem_type,
                       DWORD      *off,
                       WORD       *reg)
{
    DWORD offset;

    *mem_type = FALSE;          /* default - nothing being accessed */

    *reg = REG_VALUE(amb);      /* dest */

	if (W_VALUE(opcode))
    {
        /* if it WAS a WORD operand without the operand prefix, then it
           becomes a DWORD operand with the 66h prefix */

        if (hLocal->CPU.op_size_over)
            *mem_type |= DWORD_SIZE;
        else
            *mem_type |= WORD_SIZE;
    }

	switch (MOD_VALUE(amb))		/* address mode byte */
	{
        case 0:     /* r/m == 101b, then 32-bit displacement follows */
                    /* r/m == 100b, then SIB byte follows */
                    /* otherwise no offset follows */

            if (RM_VALUE(amb) == 5)     /* DWORD offset follows */
                offset = get_dword_at_csip(hLocal);
			else
				offset = 0;

			break;

        case 1:     /* 8-bit sign extended displacement */
                    /* EAX, ECX, EDX, EBX, EBP, ESI or EDI used as index */

                    /* if RM=4 we have a SIB byte before the immediate
                       8-bit displacement.  the 8-bit displacement will
                       be fetched when we process the SIB */

            if (RM_VALUE(amb) != 4)
                offset = dsign_extend_byte(get_byte_at_csip(hLocal));

			break;

        case 2:     /* 32-bit signed displacement follows, even if SIB
                       is used */
                    /* EAX, ECX, EDX, EBX, EBP, ESI or EDI used as index */

                    /* if RM=4 we have a SIB byte before the immediate
                       32-bit displacement.  the 32-bit displacement will
                       be fetched when we process the SIB */

            if (RM_VALUE(amb) != 4)
                offset = get_dword_at_csip(hLocal);

			break;

		case 3:

			*off = RM_VALUE(amb);		/* source */
			*mem_type = REGISTER;
			if (W_VALUE(opcode))
            {
                /* if it WAS a WORD operand without the operand prefix, then it
                   becomes a DWORD operand with the 66h prefix */

                if (hLocal->CPU.op_size_over)
                    *mem_type |= DWORD_SIZE;
                else
                    *mem_type |= WORD_SIZE;
            }

			return;						/* all done! */

	}

    /* all addressing must be within the range of 0-64K, so we can use the
       normal X registers and not worry about the high WORD of the E
       registers */

	switch (RM_VALUE(amb))
	{
        case 0:         /* DS:[EAX] */
            *off = hLocal->CPU.preg.D.EAX + offset;
			break;
        case 1:         /* DS:[ECX] */
            *off = hLocal->CPU.preg.D.ECX + offset;
			break;
        case 2:         /* DS:[EDX] */
            *off = hLocal->CPU.preg.D.EDX + offset;
			break;
		case 3:         /* DS:[EBX] */
            *off = hLocal->CPU.preg.D.EBX + offset;
			break;
        case 4:    
            /* deal with SIB byte */

			decode_sib_off(hLocal, amb, off);

			break;

        case 5:

            if (MOD_VALUE(amb) == 0)        /* DS:[disp32] */
            {
                *off = offset;
            }
			else
            {                               /* SS:[EBP+disp32/SIGN(disp8)] */
                *off = hLocal->CPU.ireg.D.EBP + offset;
			}

			break;

        case 6:                             /* DS:[ESI + Disp32/Disp8/none] */

            *off = hLocal->CPU.ireg.D.ESI + offset;

			break;

		case 7:                             /* DS:[EDI + Disp32/Disp8/none] */

            *off = hLocal->CPU.ireg.D.EDI + offset;

			break;
	}
}




//************************************************************************
// void decode_amb(hLocal, opcode, amb, mem_type, seg, off, reg)
//
// Purpose
//
//      This function calls upon the decode_amb_32 and decode_amb_norm to
//  determine the operands to instructions.  The seg and off params are
//  WORDs because in real mode, all addresses are restricted to 64-1 or
//  less.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        opcode          The first byte of the instruction
//      BYTE        amb             The amb byte of the instruction
//                                  AMB=Addressing Mode Byte
//      BYTE        *mem_type       This is filled with information about the
//                                  operands to the instructions (register or
//                                  memory, operand size, etc)
//                                  Bits:
//                                   WORD_SIZE: operands are WORDs
//                                   DWORD_SIZE: operands are DWORDs
//                                   (neither): operands are BYTEs
//                                   REGISTER: source operand is in a register
//
//      WORD        *seg            Segment of non-register operand
//      WORD       *off             WORD offset of non-register operand
//      WORD        *reg            Register number of destination
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void decode_amb(PAMLHANDLE hLocal,
                BYTE       opcode,
                BYTE       amb,
                BYTE       *mem_type,
                WORD       *seg,
                WORD       *off,
                WORD       *reg)
{
	if (hLocal->CPU.addr_size_over)
        decode_amb_32(hLocal, opcode, amb, mem_type, seg, off, reg);
    else
        decode_amb_norm(hLocal, opcode, amb, mem_type, seg, off, reg);
}



//************************************************************************
// WORD seg_to_use(hLocal, seg)
//
// Purpose
//
//      This function will determine what segment should be used by an
//  instruction.  The seg value passed in is the default segment value that
//  should be used assuming no segment-override has been used.  If a segment
//  override was specified, then the seg value will be ignored, and the value
//  of the specified override segment will be returned.
//  The typical value of seg is the value of the DS register.  Alternatively,
//  the value of the SS register may be passed in if the BP is used as a base
//  in the addressing.
//
//  NOTE: When a normal (non segment-prefix) instruction has completed, it must
//        always restore CPU.seg_over to DEFAULT_SEG for proper operation.
//        This prevents segment prefixes from persisting.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg             Default segment for instruction
//
// Returns:
//
//      WORD that contains the segment where the operation should take
//      place.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD seg_to_use(PAMLHANDLE hLocal, WORD seg)
{
    if (hLocal->CPU.seg_over == DEFAULT_SEG)
		return(seg);

    switch(hLocal->CPU.seg_over)
	{
		case CS_OVER:
            return(hLocal->CPU.CS);
		case DS_OVER:
            return(hLocal->CPU.DS);
		case ES_OVER:
            return(hLocal->CPU.ES);
		case SS_OVER:
            return(hLocal->CPU.SS);
        case FS_OVER:
            return(hLocal->CPU.FS);
        case GS_OVER:
            return(hLocal->CPU.GS);
    }

	return(FALSE);
}



//************************************************************************
// WORD wreg_value(hLocal, reg_num)
//
// Purpose
//
//      This function retrieves the value from the specified WORD register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number that contains our value
//
// Returns:
//
//      WORD that contains the value of the specified register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD wreg_value(PAMLHANDLE hLocal, WORD reg_num)
{
	switch (reg_num)
	{
		case REG_AX:
            return(hLocal->CPU.preg.X.AX);
		case REG_CX:
            return(hLocal->CPU.preg.X.CX);
		case REG_DX:
            return(hLocal->CPU.preg.X.DX);
		case REG_BX:
            return(hLocal->CPU.preg.X.BX);
		case REG_SP:
            return(hLocal->CPU.ireg.X.SP);
		case REG_BP:
            return(hLocal->CPU.ireg.X.BP);
		case REG_SI:
            return(hLocal->CPU.ireg.X.SI);
		case REG_DI:
            return(hLocal->CPU.ireg.X.DI);
	}

	return(FALSE);
}


//************************************************************************
// DWORD dwreg_value(hLocal, reg_num)
//
// Purpose
//
//      This function retrieves the value from the specified DWORD register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number that contains our value
//
// Returns:
//
//      DWORD that contains the value of the specified register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD dwreg_value(PAMLHANDLE hLocal, WORD reg_num)
{
	switch (reg_num)
	{
        case REG_EAX:
            return(hLocal->CPU.preg.D.EAX);
        case REG_ECX:
            return(hLocal->CPU.preg.D.ECX);
        case REG_EDX:
            return(hLocal->CPU.preg.D.EDX);
        case REG_EBX:
            return(hLocal->CPU.preg.D.EBX);
        case REG_ESP:
            return(hLocal->CPU.ireg.D.ESP);
        case REG_EBP:
            return(hLocal->CPU.ireg.D.EBP);
        case REG_ESI:
            return(hLocal->CPU.ireg.D.ESI);
        case REG_EDI:
            return(hLocal->CPU.ireg.D.EDI);
	}

	return(FALSE);
}


//************************************************************************
// BYTE breg_value(hLocal, reg_num)
//
// Purpose
//
//      This function retrieves the value from the specified BYTE register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number that contains our value
//
// Returns:
//
//      BYTE that contains the value of the specified register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

BYTE breg_value(PAMLHANDLE hLocal, WORD reg_num)
{
	switch (reg_num)
	{
		case REG_AL:
            return(hLocal->CPU.preg.H.AL);
		case REG_CL:
            return(hLocal->CPU.preg.H.CL);
		case REG_DL:
            return(hLocal->CPU.preg.H.DL);
		case REG_BL:
            return(hLocal->CPU.preg.H.BL);
		case REG_AH:
            return(hLocal->CPU.preg.H.AH);
		case REG_CH:
            return(hLocal->CPU.preg.H.CH);
		case REG_DH:
            return(hLocal->CPU.preg.H.DH);
		case REG_BH:
            return(hLocal->CPU.preg.H.BH);
	}

	return(FALSE);
}

/*
WORD wsign_status(WORD a)
{
	if (a & 0x8000)
		return(TRUE);
	return(FALSE);
}

WORD bsign_status(BYTE a)
{
	if (a & 0x80)
		return(TRUE);
	return(FALSE);
}

WORD wzero_status(WORD a)
{
	return (!!a);
}

WORD bzero_status(BYTE a)
{
	return(!!a);
}
*/


//************************************************************************
// DWORD sign_status(wMemType, dwData)
//
// Purpose
//
//      This function determines the sign of a BYTE, WORD or DWORD operand.
//
// Parameters:
//
//      WORD        wMemType       The size of the operand (bit-flags)
//                                 (wMemType & WORD_SIZE) -> WORD
//                                 (wMemType & DWORD_SIZE) -> DWORD
//                                 other -> BYTE
//      DWORD       dwData         Value that we want to check for sign status.
//
// Returns:
//
//      DWORD that contains 0 for positive numbers and 1 for negative numbers
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD sign_status(WORD wMemType, DWORD dwData)
{
    if (wMemType & WORD_SIZE)
        return(!!(dwData & 0x8000U));
    else if (wMemType & DWORD_SIZE)
        return(!!(dwData & 0x80000000L));

    return(!!(dwData & 0x80));
}



//************************************************************************
// WORD set_wreg(hLocal, reg_num, value)
//
// Purpose
//
//      This function sets the value of the specified WORD register.  It
//  remembers when index registers are modified and marks them as "valid"
//  so when these registers are used to index memory writes, they do not
//  exclude viruses.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number to contain our value
//      WORD        value           The value to store in the register
//
// Returns:
//
//      WORD that contains the value stored
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD set_wreg(PAMLHANDLE hLocal, WORD reg_num, WORD value)
{
	switch (reg_num)
	{
		case REG_AX:
            return(hLocal->CPU.preg.X.AX = value);
		case REG_CX:
            return(hLocal->CPU.preg.X.CX = value);
		case REG_DX:
            return(hLocal->CPU.preg.X.DX = value);
		case REG_BX:

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_IGNORE) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.wIndexStatus |= VALID_INDEX_BX;

            return(hLocal->CPU.preg.X.BX = value);
		case REG_SP:
            return(hLocal->CPU.ireg.X.SP = value);
		case REG_BP:

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_IGNORE) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.wIndexStatus |= VALID_INDEX_BP;

            return(hLocal->CPU.ireg.X.BP = value);
		case REG_SI:

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_IGNORE) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.wIndexStatus |= VALID_INDEX_SI;

            return(hLocal->CPU.ireg.X.SI = value);
		case REG_DI:

            if (!(hLocal->CPU.wIndexStatus & VALID_INDEX_IGNORE) &&
                !(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.wIndexStatus |= VALID_INDEX_DI;

            return(hLocal->CPU.ireg.X.DI = value);
	}

	return(FALSE);
}

//************************************************************************
// DWORD set_dwreg(hLocal, reg_num, value)
//
// Purpose
//
//      This function sets the value of the specified DWORD register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number to contain our value
//      DWORD       value           The value to store in the register
//
// Returns:
//
//      DWORD that contains the value stored
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD set_dwreg(PAMLHANDLE hLocal, WORD reg_num, DWORD value)
{
	switch (reg_num)
	{
        case REG_EAX:
            return(hLocal->CPU.preg.D.EAX = value);
        case REG_ECX:
            return(hLocal->CPU.preg.D.ECX = value);
        case REG_EDX:
            return(hLocal->CPU.preg.D.EDX = value);
        case REG_EBX:
            return(hLocal->CPU.preg.D.EBX = value);
        case REG_ESP:
            return(hLocal->CPU.ireg.D.ESP = value);
        case REG_EBP:
            return(hLocal->CPU.ireg.D.EBP = value);
        case REG_ESI:
            return(hLocal->CPU.ireg.D.ESI = value);
        case REG_EDI:
            return(hLocal->CPU.ireg.D.EDI = value);
	}

	return(FALSE);
}



//************************************************************************
// BYTE set_breg(hLocal, reg_num, value)
//
// Purpose
//
//      This function sets the value of the specified BYTE register. It
//  remembers when index registers (BH,BL) are modified and marks them as
//  "valid" so when these registers are used to index memory writes, they
//  do not exclude viruses.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number to contain our value
//      BYTE        value           The value to store in the register
//
// Returns:
//
//      BYTE that contains the value stored
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

BYTE set_breg(PAMLHANDLE hLocal, WORD reg_num, BYTE value)
{
	switch (reg_num)
	{
		case REG_AL:
            return(hLocal->CPU.preg.H.AL = value);
		case REG_CL:
            return(hLocal->CPU.preg.H.CL = value);
		case REG_DL:
            return(hLocal->CPU.preg.H.DL = value);
		case REG_BL:

            if (!(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.wIndexStatus |= VALID_INDEX_BX;

            return(hLocal->CPU.preg.H.BL = value);
		case REG_AH:
            return(hLocal->CPU.preg.H.AH = value);
		case REG_CH:
            return(hLocal->CPU.preg.H.CH = value);
		case REG_DH:
            return(hLocal->CPU.preg.H.DH = value);
		case REG_BH:

            if (!(hLocal->dwFlags &
                 (LOCAL_FLAG_IMMUNE | LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
                hLocal->CPU.wIndexStatus |= VALID_INDEX_BX;

            return(hLocal->CPU.preg.H.BH = value);
	}

	return(FALSE);
}


//************************************************************************
// WORD set_sreg(hLocal, reg_num, wval)
//
// Purpose
//
//      This function sets the value of the specified segment register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number to contain our value
//      WORD        value           The value to store in the register
//
// Returns:
//
//      WORD that contains the segment value stored
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


WORD set_sreg(PAMLHANDLE hLocal, WORD reg_num, WORD wval)
{
	switch (reg_num)
	{
		case REG_ES:
            return(hLocal->CPU.ES = wval);
		case REG_CS:
            return(hLocal->CPU.CS = wval);
		case REG_SS:
            return(hLocal->CPU.SS = wval);
        case REG_DS:
            return(hLocal->CPU.DS = wval);
        case REG_FS:
            return(hLocal->CPU.FS = wval);
        case REG_GS:
            return(hLocal->CPU.GS = wval);
	}

	return(FALSE);
}


//************************************************************************
// WORD get_sreg(hLocal, reg_num)
//
// Purpose
//
//      This function retrieves the value of the specified segment register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        reg_num         Register number to contain our value
//
// Returns:
//
//      WORD that contains the specified segment value
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


WORD get_sreg(PAMLHANDLE hLocal, WORD reg_num)
{
	switch (reg_num)
	{
		case REG_ES:
            return(hLocal->CPU.ES);
		case REG_CS:
            return(hLocal->CPU.CS);
		case REG_SS:
            return(hLocal->CPU.SS);
        case REG_DS:
            return(hLocal->CPU.DS);
        case REG_FS:
            return(hLocal->CPU.FS);
        case REG_GS:
            return(hLocal->CPU.GS);
	}

	return(FALSE);
}

//************************************************************************
// void set_seg_over(hLocal, seg_over)
//
// Purpose
//
//      This function establishes a segment override for the next non-override
//  instruction. (Override instructions include seg prefixes (e.g. CS:), as
//  well as operand and address overrides (67h, 66h) and REP prefixes.)
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        seg_over        Segment number to be used for override.
//                                  NOTE: This number specifies which segment
//                                        to use for the override.  It does not
//                                        specify the value of the segment.
//                                        Sample values: CS_OVER, DS_OVER
//                                        See CPU.H
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void set_seg_over(PAMLHANDLE hLocal, WORD seg_over)
{
    hLocal->CPU.seg_over = (BYTE)seg_over;
}



//************************************************************************
// void reset_seg_over(hLocal)
//
// Purpose
//
//      This function clears all segment overrides for the next instruction.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void reset_seg_over(PAMLHANDLE hLocal)
{
    hLocal->CPU.seg_over = DEFAULT_SEG;
}


//************************************************************************
// void reset_rep_over(hLocal)
//
// Purpose
//
//      This function clears all REPZ/REPNZ prefixes for the next instruction.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void reset_rep_over(PAMLHANDLE hLocal)
{
    hLocal->CPU.rep_prefix = 0;
}


//************************************************************************
// WORD wget_value(hLocal, mem_type, seg, off)
//
// Purpose
//
//      This function retrieves the specified value from either memory or
//  from a register (if (mem_type & REGISTER) is non-zero).  It's basically
//  a generic get-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        mem_type        Specifies register vs memory fetch
//      WORD        seg             Segment where to find our data.  This is
//                                  only valid if (mem_type & REGISTER) == 0
//      WORD        off             Offset where to find our data.  If
//                                  (mem_type & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//
// Returns:
//
//      WORD value from the specified memory/register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD wget_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off)
{
    if (mem_type & REGISTER)
        return(wreg_value(hLocal,off));

    return(get_word(hLocal,seg_to_use(hLocal,seg),off));
}


//************************************************************************
// DWORD dwget_value(hLocal, mem_type, seg, off)
//
// Purpose
//
//      This function retrieves the specified value from either memory or
//  from a register (if (mem_type & REGISTER) is non-zero).  It's basically
//  a generic get-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        mem_type        Specifies register vs memory fetch
//      WORD        seg             Segment where to find our data.  This is
//                                  only valid if (mem_type & REGISTER) == 0
//      WORD        off             Offset where to find our data.  If
//                                  (mem_type & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//
// Returns:
//
//      DWORD value from the specified memory/register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


DWORD dwget_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off)
{
    if (mem_type & REGISTER)
        return(dwreg_value(hLocal,off));

    return(get_dword(hLocal,seg_to_use(hLocal,seg),off));
}


//************************************************************************
// WORD wput_value(hLocal, mem_type, seg, off, value)
//
// Purpose
//
//      This function stores the specified value to either memory or
//  to a register (if (mem_type & REGISTER) is non-zero).  It's basically
//  a generic put-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        mem_type        Specifies register vs memory store
//      WORD        seg             Segment where to store our data.  This is
//                                  only valid if (mem_type & REGISTER) == 0
//      WORD        off             Offset where to store our data.  If
//                                  (mem_type & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//      WORD        value           The value to be stored.
//
// Returns:
//
//      WORD value that was stored to the specified memory/register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

WORD wput_value(PAMLHANDLE hLocal,
                WORD mem_type,
                WORD seg,
                WORD off,
                WORD value)
{
	if (mem_type & REGISTER)
        return(set_wreg(hLocal, off, value));

    return(put_word(hLocal, seg_to_use(hLocal, seg),off,value));
}


//************************************************************************
// DWORD dput_value(hLocal, mem_type, seg, off, value)
//
// Purpose
//
//      This function stores the specified value to either memory or
//  to a register (if (mem_type & REGISTER) is non-zero).  It's basically
//  a generic put-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        mem_type        Specifies register vs memory store
//      WORD        seg             Segment where to store our data.  This is
//                                  only valid if (mem_type & REGISTER) == 0
//      WORD        off             Offset where to store our data.  If
//                                  (mem_type & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//      DWORD       value           The value to be stored.
//
// Returns:
//
//      DWORD value that was stored to the specified memory/register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


DWORD dput_value(PAMLHANDLE hLocal,
                 WORD mem_type,
                 WORD seg,
                 WORD off,
                 DWORD value)
{
	if (mem_type & REGISTER)
		return(set_dwreg(hLocal, off, value));

	return(put_dword(hLocal, seg_to_use(hLocal, seg),off,value));
}


//************************************************************************
// BYTE bget_value(hLocal, mem_type, seg, off)
//
// Purpose
//
//      This function retrieves the specified value from either memory or
//  from a register (if (mem_type & REGISTER) is non-zero).  It's basically
//  a generic get-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        mem_type        Specifies register vs memory fetch
//      WORD        seg             Segment where to find our data.  This is
//                                  only valid if (mem_type & REGISTER) == 0
//      WORD        off             Offset where to find our data.  If
//                                  (mem_type & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//
// Returns:
//
//      BYTE value from the specified memory/register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************


BYTE bget_value(PAMLHANDLE hLocal, WORD mem_type, WORD seg, WORD off)
{
    if (mem_type & REGISTER)
        return(breg_value(hLocal,(BYTE)off));

    return(get_byte(hLocal,seg_to_use(hLocal,seg),off));
}


//************************************************************************
// BYTE bput_value(hLocal, mem_type, seg, off, value)
//
// Purpose
//
//      This function stores the specified value to either memory or
//  to a register (if (mem_type & REGISTER) is non-zero).  It's basically
//  a generic put-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        mem_type        Specifies register vs memory store
//      WORD        seg             Segment where to store our data.  This is
//                                  only valid if (mem_type & REGISTER) == 0
//      WORD        off             Offset where to store our data.  If
//                                  (mem_type & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//      BYTE        value           The value to be stored.
//
// Returns:
//
//      BYTE value that was stored to the specified memory/register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

BYTE bput_value(PAMLHANDLE hLocal,
                WORD mem_type,
                WORD seg,
                WORD off,
                BYTE value)
{
	if (mem_type & REGISTER)
        return(set_breg(hLocal,(BYTE)off, (BYTE)value));

	return(put_byte(hLocal,seg_to_use(hLocal,seg),off,value));
}

//************************************************************************
// void reset_32_bit_over(hLocal)
//
// Purpose
//
//      This function clears all 66h, 67h prefixes for the next instruction.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void reset_32_bit_over(PAMLHANDLE hLocal)
{
    // reset any overrides that may have been set

    hLocal->CPU.op_size_over = FALSE;
    hLocal->CPU.addr_size_over = FALSE;
}



//************************************************************************
// void large_switch(hLocal, opcode)
//
// Purpose
//
//      This function decodes the specified opcode and calls upon the
//  appropriate support functions in INSTR.CPP and INSTR2.CPP.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        opcode          Fetched opcode to emulate
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

void large_switch(PAMLHANDLE hLocal, BYTE opcode)
{
//    int op_len, i;
//	unsigned int ip;
//    char buffer[10];

    switch (opcode)
    {
        case oc00010000b:
        case oc00010001b:
        case oc00010010b:
        case oc00010011b:
            oc000100dw(hLocal, opcode);
            break;
        case oc10000000b:
        case oc10000001b:
        case oc10000010b:
        case oc10000011b:
            oc100000sw(hLocal, opcode);
            break;
        case oc00010100b:
        case oc00010101b:
            oc0001010w(hLocal, opcode);
            break;
        case oc00000000b:
        case oc00000001b:
        case oc00000010b:
        case oc00000011b:
            oc000000dw(hLocal, opcode);
            break;
        case oc00000100b:
        case oc00000101b:
            oc0000010w(hLocal, opcode);
            break;
        case oc11101000b:
            oc11101000(hLocal);
            break;
        case oc11111111b:
            oc11111111(hLocal, opcode);
            break;
        case oc10011010b:
            oc10011010(hLocal);
            break;
        case oc10011000b:
            oc10011000(hLocal);
            break;
        case oc11111000b:
            oc11111000(hLocal);
            break;
        case oc11111100b:
            oc11111100(hLocal);
            break;
        case oc11111010b:
            oc11111010(hLocal);
            break;
        case oc11110101b:
            oc11110101(hLocal);
            break;
        case oc00111000b:
        case oc00111001b:
        case oc00111010b:
        case oc00111011b:
            oc001110dw(hLocal, opcode);
            break;
        case oc00111100b:
        case oc00111101b:
            oc0011110w(hLocal, opcode);
            break;
        case oc10011001b:
            oc10011001(hLocal);
            break;
        case oc11111110b:
            oc11111110(hLocal, opcode);
            break;
        case oc01001000b:
        case oc01001001b:
        case oc01001010b:
        case oc01001011b:
        case oc01001100b:
        case oc01001101b:
        case oc01001110b:
        case oc01001111b:
            oc01001reg(hLocal, opcode);
            break;
        case oc11110110b:
        case oc11110111b:
            oc1111011w(hLocal, opcode);
            break;
        case oc01000000b:
        case oc01000001b:
        case oc01000010b:
        case oc01000011b:
        case oc01000100b:
        case oc01000101b:
        case oc01000110b:
        case oc01000111b:
            oc01000reg(hLocal, opcode);
            break;
        case oc11001111b:
            oc11001111(hLocal);
            break;
        case oc01110000b:
        case oc01110001b:
        case oc01110010b:
        case oc01110011b:
        case oc01110100b:
        case oc01110101b:
        case oc01110110b:
        case oc01110111b:
        case oc01111000b:
        case oc01111001b:
        case oc01111010b:
        case oc01111011b:
        case oc01111100b:
        case oc01111101b:
        case oc01111110b:
        case oc01111111b:
            oc0111cccc(hLocal, opcode);
            break;
        case oc11100011b:
            oc11100011(hLocal);
            break;
        case oc11101011b:
            oc11101011(hLocal);
            break;
        case oc11101010b:
            oc11101010(hLocal);
            break;
        case oc10011111b:           /* lahf */
            oc10011111(hLocal);
            break;
        case oc10011110b:           /* sahf */
            oc10011110(hLocal);
            break;
        case oc11000101b:
			oc11000101(hLocal);
			break;
		case oc10001101b:
			oc10001101(hLocal, opcode);
			break;
		case oc11000100b:
			oc11000100(hLocal);
            break;
        case oc10101100b:
        case oc10101101b:
            oc1010110w(hLocal, opcode);
            break;
        case oc11100010b:
            oc11100010(hLocal);
            break;
        case oc11100001b:
            oc11100001(hLocal);
            break;
        case oc11100000b:
            oc11100000(hLocal);
            break;
        case oc10001000b:
        case oc10001001b:
        case oc10001010b:
        case oc10001011b:
            oc100010dw(hLocal, opcode);
            break;
        case oc10110000b:
        case oc10110001b:
        case oc10110010b:
        case oc10110011b:
        case oc10110100b:
        case oc10110101b:
        case oc10110110b:
        case oc10110111b:
        case oc10111000b:
        case oc10111001b:
        case oc10111010b:
        case oc10111011b:
        case oc10111100b:
        case oc10111101b:
        case oc10111110b:
        case oc10111111b:
            oc1011wreg(hLocal, opcode);
            break;
        case oc11000110b:
        case oc11000111b:
            oc1100011w(hLocal, opcode);
            break;
        case oc10100000b:
        case oc10100001b:
            oc1010000w(hLocal, opcode);
            break;
        case oc10100010b:
        case oc10100011b:
            oc1010001w(hLocal, opcode);
            break;
        case oc10001110b:
            oc10001110(hLocal, opcode);
            break;
        case oc10001100b:
            oc10001100(hLocal, opcode);
            break;
        case oc10100100b:
        case oc10100101b:
            oc1010010w(hLocal, opcode);
            break;
        case oc00001000b:
        case oc00001001b:
        case oc00001010b:
        case oc00001011b:
            oc000010dw(hLocal, opcode);
            break;
        case oc00001100b:
        case oc00001101b:
            oc0000110w(hLocal, opcode);
            break;
        case oc01011000b:
        case oc01011001b:
        case oc01011010b:
        case oc01011011b:
        case oc01011100b:
        case oc01011101b:
        case oc01011110b:
        case oc01011111b:
            oc01011reg(hLocal, opcode);
            break;
        case oc10001111b:
            oc10001111(hLocal, opcode);
            break;
        case oc00000111b:
        case oc00010111b:
        case oc00011111b:
            oc00sreg111(hLocal, opcode);
            break;
        case oc00001111b:
            oc00001111(hLocal);
            break;
        case oc10011101b:
            oc10011101(hLocal);
            break;
        case oc01010000b:
        case oc01010001b:
        case oc01010010b:
        case oc01010011b:
        case oc01010100b:
        case oc01010101b:
        case oc01010110b:
        case oc01010111b:
            oc01010reg(hLocal, opcode);
            break;
        case oc00000110b:
        case oc00001110b:
        case oc00010110b:
        case oc00011110b:
            oc00sreg110(hLocal, opcode);
            break;
        case oc10011100b:
            oc10011100(hLocal);
            break;
        case oc11010000b:
        case oc11010001b:
            oc1101000w(hLocal, opcode);
            break;
        case oc11010010b:
        case oc11010011b:
            oc1101001w(hLocal, opcode);
            break;
        case oc11110010b:
            oc11110010(hLocal);
            break;
        case oc11110011b:
            oc11110011(hLocal);
            break;
        case oc11000011b:
            oc11000011(hLocal);
            break;
        case oc11000010b:
            oc11000010(hLocal);
            break;
        case oc11001011b:
            oc11001011(hLocal);
            break;
        case oc11001010b:
            oc11001010(hLocal);
            break;
        case oc00011000b:
        case oc00011001b:
        case oc00011010b:
        case oc00011011b:
            oc000110dw(hLocal, opcode);
            break;
        case oc00011100b:
        case oc00011101b:
            oc0001110w(hLocal, opcode);
            break;

        case oc00100110b:
        case oc00101110b:
        case oc00110110b:
        case oc00111110b:

            oc001seg110(hLocal, opcode);
            break;

        case oc01100100b:

            oc01100100(hLocal);                 /* FS: seg prefix */
            break;

        case oc01100101b:

            oc01100101(hLocal);                 /* GS: seg prefix */
            break;

        case oc11111001b:
            oc11111001(hLocal);
            break;

        case oc11111101b:
            oc11111101(hLocal);
            break;
        case oc11111011b:
            oc11111011(hLocal);
            break;

        case oc10101010b:
        case oc10101011b:
            oc1010101w(hLocal, opcode);
            break;

        case oc00101000b:
        case oc00101001b:
        case oc00101010b:
        case oc00101011b:
            oc001010dw(hLocal, opcode);
            break;

        case oc00101100b:
        case oc00101101b:
            oc0010110w(hLocal, opcode);
            break;

        case oc10000110b:
        case oc10000111b:
            oc1000011w(hLocal, opcode);
            break;

        case oc10010000b:
        case oc10010001b:
        case oc10010010b:
        case oc10010011b:
        case oc10010100b:
        case oc10010101b:
        case oc10010110b:
        case oc10010111b:
            oc10010reg(hLocal, opcode);
            break;

        case oc00110000b:
        case oc00110001b:
        case oc00110010b:
        case oc00110011b:
            oc001100dw(hLocal, opcode);
            break;

        case oc00110100b:
        case oc00110101b:
            oc0011010w(hLocal, opcode);
            break;

        case oc11101001b:
            oc11101001(hLocal);
            break;

        case oc00100000b:
        case oc00100001b:
        case oc00100010b:
        case oc00100011b:
            oc001000dw(hLocal, opcode);
            break;

        case oc00100100b:
        case oc00100101b:
            oc0010010w(hLocal, opcode);
            break;

        case oc10000100b:
        case oc10000101b:
            oc1000010w(hLocal, opcode);
            break;

        case oc10101000b:
        case oc10101001b:
            oc1010100w(hLocal, opcode);
            break;

        case oc11001101b:
            oc11001101(hLocal);
            break;

        case oc11001100b:
            oc11001100(hLocal);
            break;


        case oc10100110b:
        case oc10100111b:
            oc1010011w(hLocal, opcode);
            break;

        case oc10101110b:
        case oc10101111b:
            oc1010111w(hLocal, opcode);
            break;

        case oc01100110b:                   /* OPERAND 32-bit override */
            oc01100110(hLocal);
            break;

        case oc01100111b:                   /* ADDRESS 32-bit override */
            oc01100111(hLocal);
            break;

        case oc00110111b:                   /* AAA */
            oc00110111(hLocal);
            break;

        case oc11010101b:                   /* AAD */
            oc11010101(hLocal);
            break;

        case oc11010100b:                   /* AAM */
            oc11010100(hLocal);
            break;

        case oc00111111b:                   /* AAS */
            oc00111111(hLocal);
            break;

        case oc00100111b:                   /* DAA */
            oc00100111(hLocal);
            break;

        case oc00101111b:                   /* DAS */
            oc00101111(hLocal);
            break;

        case oc11001000b:                   /* ENTER */
            oc11001000(hLocal);
            break;

        case oc11100100b:
        case oc11100101b:
            oc1110010w(hLocal, opcode);     /* IN AL/AX/EAX, IMMED */
            break;

        case oc11101100b:
        case oc11101101b:
            oc1110110w(hLocal, opcode);     /* IN AL/AX/EAX, DX */
            break;

        case oc01101100b:                   /* INS */
        case oc01101101b:
            oc0110110w(hLocal,opcode);
            break;

        case oc11001110b:                   /* INTO */
            oc11001110(hLocal);
            break;

        case oc11001001b:                   /* LEAVE */
            oc11001001(hLocal);
            break;

        case oc11110000b:                   /* LOCK */

            // This is a prefix like REPZ or CS: and should be treated
            // as such...

            hLocal->CPU.reset_seg = FALSE;

            break;

        case OUT_8_AL:
        case OUT_8_AX:
        case OUT_DX_AL:
        case OUT_DX_AX:

            OUT_(hLocal,opcode);
            break;

        case OUTS_1:
        case OUTS_2:
            OUTS(hLocal,opcode);
            break;

        case POPA_OPCODE:
            POPA(hLocal);
            break;

        case PUSH_I8_OPCODE:
        case PUSH_I16_OPCODE:
            PUSH_IMMED(hLocal,opcode);
            break;

        case PUSHA_OPCODE:
            PUSH_ALL(hLocal);
            break;

		case oc11000000b:
		case oc11000001b:
            oc1100000w(hLocal,opcode);
            break;

        case WAIT_OPCODE:

            /* 1 byte opcode */

            break;

        case XLAT_OPCODE:
            XLAT(hLocal);
            break;

        case ARPL_OPCODE:
            ARPL(hLocal);
            break;

        case SETALC_OPCODE:
            SETALC(hLocal);
            break;

        case IMUL_69_OPCODE:
            IMUL_69(hLocal);
            break;

        case IMUL_6B_OPCODE:
            IMUL_6B(hLocal);
            break;

        case BOUND_OPCODE:
            BOUND(hLocal);
            break;

        case FLOAT_OPCODE1:
        case FLOAT_OPCODE2:
        case FLOAT_OPCODE3:
        case FLOAT_OPCODE4:
        case FLOAT_OPCODE5:
        case FLOAT_OPCODE6:
        case FLOAT_OPCODE7:
        case FLOAT_OPCODE8:
            FLOAT_INSTR(hLocal,opcode);
            break;

        case RESERVED_OPCODE1:
        case RESERVED_OPCODE2:

            // one byte skippers...

            break;


        default:

            // skip unknown instructions
#ifdef BORLAND
            printf("Unknown opcode encountered! %02X\n",opcode);
#endif

//            ip = hLocal->CPU.IP;
//
//            buffer[0] = opcode;
//
//            for (i=1;i< 10;i++)
//                buffer[i] = get_byte_at_csip(hLocal);
//
//            hLocal->CPU.IP = ip;
//
//            op_len = opcodelen(buffer);
//
//            while (--op_len > 0)
//                get_byte_at_csip(hLocal);
//
            break;
    }

}


//************************************************************************
// DWORD gen_get_value(hLocal, wMemType, wSeg, wOff)
//
// Purpose
//
//      This function retrieves the specified value from either memory or
//  from a register (if (mem_type & REGISTER) is non-zero).  It's basically
//  a generic get-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        wMemType        Specifies register vs memory fetch
//                                  Also specifies size of the operand.
//      WORD        wSeg            Segment where to find our data.  This is
//                                  only valid if (wMemType & REGISTER) == 0
//      WORD        wOff            Offset where to find our data.  If
//                                  (wMemType & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//
// Returns:
//
//      BYTE/WORD/DWORD value from the specified memory/register.  The size
//  of the returned value depends on the contents of the wMemType parameter.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD gen_get_value(PAMLHANDLE hLocal,
                    WORD wMemType,
                    WORD wSeg,
                    WORD wOff)
{
    if (wMemType & WORD_SIZE)
        return((DWORD)wget_value(hLocal,(BYTE)wMemType,wSeg,wOff));

    if (wMemType & DWORD_SIZE)
        return(dwget_value(hLocal,(BYTE)wMemType,wSeg,wOff));

    return((DWORD)bget_value(hLocal,(BYTE)wMemType,wSeg,wOff));
}


//************************************************************************
// DWORD gen_put_value(hLocal, wMemType, seg, off, dwValue)
//
// Purpose
//
//      This function stores the specified value to either memory or
//  to a register (if (wMemType & REGISTER) is non-zero).  It's basically
//  a generic put-operand function.  This function will automatically check
//  for a segment override and use the overridden segment if necessary.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        wMemType        Specifies register vs memory store
//                                  Also specifies the size of the stored data
//      WORD        seg             Segment where to store our data.  This is
//                                  only valid if (wMemType & REGISTER) == 0
//      WORD        off             Offset where to store our data.  If
//                                  (wMemType & REIGSTER) != 0, then this is
//                                  a register number instead of an offset.
//      DWORD       dwValue         The value to be stored.  Depending on the
//                                  bit settings of the wMemType register, this
//                                  function will store a BYTE, a WORD or a
//                                  DWORD.
//
// Returns:
//
//      BYTE/WORD/DWORD value that was stored to the specified memory/register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD gen_put_value(PAMLHANDLE hLocal,
                    WORD wMemType,
                    WORD wSeg,
                    WORD wOff,
                    DWORD dwValue)
{

	if (wMemType & WORD_SIZE)
		return(wput_value(hLocal,wMemType,wSeg,wOff,(WORD)(dwValue & 0x0000FFFFL)));

	if (wMemType & DWORD_SIZE)
        return(dput_value(hLocal,wMemType,wSeg,wOff,dwValue));

    return(bput_value(hLocal,wMemType,wSeg,wOff,(BYTE)(dwValue & 0x000000FFL)));
}


//************************************************************************
// DWORD reg_value(hLocal, wMemType, wRegNum)
//
// Purpose
//
//      This function returns the specified register value in a DWORD.  The
//  wMemType parameter specifies the size of the operand to be retrieved.  All
//  values that are BYTES or WORDS are zero-extended.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        wMemType        Specifies BYTE, WORD or DWORD parameters
//      DWORD       wRegNum         The register number that contains the
//                                  data to be retrieved.
//
// Returns:
//
//      BYTE/WORD/DWORD value that was retrieved from the specified register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD reg_value(PAMLHANDLE hLocal, WORD wMemType, WORD wRegNum)
{
	if (wMemType & WORD_SIZE)
		return(wreg_value(hLocal,wRegNum));

	if (wMemType & DWORD_SIZE)
        return(dwreg_value(hLocal,wRegNum));

    return(breg_value(hLocal,(BYTE)wRegNum));
}


//************************************************************************
// DWORD set_reg(hLocal, wMemType, wRegNum, dwValue)
//
// Purpose
//
//      This function stores the specified value into a register.  The
//  wMemType parameter specifies the size of the operand to be stored (and
//  therefore partially determines the register).
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        wMemType        Specifies BYTE, WORD or DWORD parameters
//      DWORD       wRegNum         The register number that contains the
//                                  data to be stored.
//      DWORD       dwValue         The value to be stored.
//
// Returns:
//
//      BYTE/WORD/DWORD value that was stored to the specified register.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD set_reg(PAMLHANDLE hLocal, WORD wMemType, WORD wRegNum, DWORD dwValue)
{
	if (wMemType & WORD_SIZE)
		return(set_wreg(hLocal,wRegNum, (WORD)dwValue));

	if (wMemType & DWORD_SIZE)
		return(set_dwreg(hLocal,wRegNum,dwValue));

    return(set_breg(hLocal,(BYTE)wRegNum,(BYTE)dwValue));
}

//************************************************************************
// DWORD get_value_at_csip(hLocal, wMemType)
//
// Purpose
//
//      This function retrieves a BYTE, WORD or DWORD from the instruction
//  stream.  The wMemType field specifies the size of the value to retrieve.
//  This function automatically updates the IP (as well as the prefetch queue).
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        wMemType        Specifies WORD or DWORD values
//
// Returns:
//
//      BYTE/WORD/DWORD value that was retrieved from CS:IP.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD get_value_at_csip(PAMLHANDLE hLocal, WORD wMemType)
{
	if (wMemType & WORD_SIZE)
		return(get_word_at_csip(hLocal));

	if (wMemType & DWORD_SIZE)
		return(get_dword_at_csip(hLocal));

    return(get_byte_at_csip(hLocal));
}


//************************************************************************
// DWORD push_value(hLocal, wMemType, dwValue)
//
// Purpose
//
//      This function pushes a WORD or DWORD onto the stack, depending on the
//  value of the wMemType parameter.  This function automatically updates the
//  SP register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        wMemType        Specifies BYTE, WORD or DWORD parameters
//      DWORD       dwValue         Value to push onto the stack (WORD or DWORD)
//
// Returns:
//
//      WORD/DWORD value that was pushed on the stack.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD push_value(PAMLHANDLE hLocal, WORD wMemType, DWORD dwValue)
{
    if (wMemType & WORD_SIZE)
        return(push_word(hLocal,(WORD)dwValue));

    return(push_dword(hLocal,dwValue));
}


//************************************************************************
// DWORD pop_value(hLocal, wMemType)
//
// Purpose
//
//      This function pops a WORD or DWORD from the stack, depending on the
//  value of the wMemType parameter.  This function automatically updates the
//  SP register.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        wMemType        Specifies WORD or DWORD values
//
// Returns:
//
//      WORD/DWORD value that was popped from the stack.
//
//************************************************************************
// 2/20/96 Carey created.
//************************************************************************

DWORD pop_value(PAMLHANDLE hLocal, WORD wMemType)
{
    if (wMemType & WORD_SIZE)
        return(pop_word(hLocal));

    return(pop_dword(hLocal));
}

