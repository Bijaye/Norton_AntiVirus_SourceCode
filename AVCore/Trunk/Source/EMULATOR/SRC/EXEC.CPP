// Copyright 1995 Symantec, Peter Norton Product Group
//************************************************************************
//
// $Header:   S:/EMULATOR/VCS/exec.cpv   1.31   05 Jun 1996 12:31:44   CNACHEN  $
//
// Description:
//
//  This file deals with the top level emulation of programs.  It contains:
//
//  1. The main emulator interpretation subroutine: interpret()
//  2. A routine which determines which pages were modified during emulation
//     and scans those pages using the string scanning system (SEARCH.CPP),
//  3. Instruction fault code.
//
//  This file can also be build with the BUILD_SET define in order to create
//  a standalone program which can be used to obtain virus information instead
//  of just scanning for viruses.
//
// Contains:
//
// See Also:
//************************************************************************
// $Log:   S:/EMULATOR/VCS/exec.cpv  $
// 
//    Rev 1.31   05 Jun 1996 12:31:44   CNACHEN
// Fixed CSIP queue lookup
// 
//    Rev 1.30   20 May 1996 14:38:12   CNACHEN
// Added GETSIG change. NOTE: this is commented for normal scanner
// operation and will not affect emulation other than getsig.
// This fixes a prefetch-queue problem during getsig operation.
// Old behavior: getsig detects prefetch queue fumbling and also finds
// the BUILD.SIG signature.  When it returns, PAMAPI detects the prefetch
// queue modification and re-emulates with a smaller prefetch queue even
// though the signature was already found.  The new version does not do this.
// 
// 
//    Rev 1.29   09 Apr 1996 12:17:26   CNACHEN
// Fixed string searching so we are guaranteed to search the entire 1K buffer.
// This is done by adding an extra 24 bytes to the end of the buffer and zeroing
// this data.
// 
//    Rev 1.28   04 Mar 1996 16:01:24   CNACHEN
// Added #IFDEF'd cache support.
// 
//    Rev 1.27   27 Feb 1996 15:59:36   CNACHEN
// Commented this file and fixed a bug with STOP_16 and STOP_1K exclusion calls.
// 
//    Rev 1.26   26 Feb 1996 18:25:46   CNACHEN
// Commented the first 2 functions.  Lots more work to do...
// 
//    Rev 1.25   16 Feb 1996 13:23:24   CNACHEN
// Fixed handling of 16 and 1024 exclusions when the target program uses REPMOVS
// 
//    Rev 1.24   13 Feb 1996 12:00:32   CNACHEN
// Chnaged memset to MEMSET.
// 
//    Rev 1.23   09 Feb 1996 16:03:44   CNACHEN
// fixed no-fault bug...
// 
//    Rev 1.22   02 Feb 1996 11:44:20   CNACHEN
// Added new dwFlags and revamped all exclusion checking...
// 
// 
//    Rev 1.21   24 Jan 1996 13:34:18   DCHI
// Typecast fix.
// 
//    Rev 1.20   23 Jan 1996 14:43:32   DCHI
// Addition of CSIP queue, ARPL repair query, and modification of immunity
// counter to deal with nested immunity activations.
// 
//    Rev 1.19   22 Jan 1996 17:25:30   DCHI
// Added incrementing of instructionFetchCount.
// 
//    Rev 1.18   22 Jan 1996 13:40:10   CNACHEN
// Added new fault support.
// 
//    Rev 1.17   04 Jan 1996 10:15:48   CNACHEN
// Added two new PAM opcodes for PrefetchQ->VM and VM->PrefetchQ...
// 
//    Rev 1.16   21 Dec 1995 19:08:56   CNACHEN
// Added GETSIG stuff.
// 
//    Rev 1.15   19 Dec 1995 19:08:00   CNACHEN
// Added prefetch queue support!
// 
// 
//    Rev 1.14   14 Dec 1995 15:21:22   CNACHEN
// Added control relinquishing in between memory scans of 1K buffers.
// 
//    Rev 1.13   14 Dec 1995 13:02:44   CNACHEN
// Added support for control relinquishing under NLM...
// 
//    Rev 1.12   14 Dec 1995 10:49:32   CNACHEN
// No change.
// 
//    Rev 1.11   15 Nov 1995 17:45:34   CNACHEN
// Added additional handling for GETSIG.
// 
//    Rev 1.10   03 Nov 1995 11:33:24   CNACHEN
// Fixed iteration checkpoints.
// 
//    Rev 1.9   19 Oct 1995 18:23:42   CNACHEN
// Initial revision... with comment header :)
// 
//************************************************************************


#include "pamapi.h"

//#define BUILD_SET

#ifdef BUILD_SET
#include <conio.h>

extern DWORD dwGSIter;
#endif




//************************************************************************
// WORD search_buffers_for_string(hLocal)
//
// Purpose
//
//      This function searches all referenced pages (and their successor
//  pages) for virus signatures.  A page (and the page immediately following
//  it) is searched if the following criteria are met:
//
//  1. One or more bytes from an instrction were fetched from this page during
//     emulation.  I.E. CS:IP pointed to some byte within the page and that
//     byte was fetched for execution.
//
//  2. The contents of the page have been modified and at least
//     hLocal->hGPAM->config_info.wMinWritesForScan writes have been made
//     to any virtual memory within the virtual machine.  This threshold is
//     used so we don't scan pages that are modified by standard, non-encrypted
//     programs.
//
//      This function always scans 2 pages (1024 bytes) at a time.  It will
//  redundantly scan some pages.  For example, if pages 3 and 4 were modified,
//  3 and 4 would first be scanned together (to catch virus signatures that
//  were split over page 3 and 4).  Next, pages 4 and 5 would be scanned.
//
//  1024 bytes are scanned to guarantee that PAM looks for virus signatures
//  up to 512 bytes past the end of the modified/executed-through page.
//
//
//  Control is relinquished on the NLM platform in between each 2-page
//  scan.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      NOT_FOUND (0xFFFF)          If no virus signature was found
//      other WORD                  If a virus signature was found
//
//************************************************************************
// 2/26/96 Carey created.
//************************************************************************

WORD search_buffers_for_string(PAMLHANDLE hLocal)
{
	WORD                    i, sig_num, scan_write_buffers;
    BYTE                    search_buffer[2*SECTOR_SIZE+SIG_LEN];
	long					global_offset, file_offset;

	global_offset = 0;

    if (hLocal->CPU.num_writes >= hLocal->hGPAM->config_info.wMinWritesForScan)
		scan_write_buffers = PAGE_DIRTY | PAGE_ON_DISK | PAGE_EXEC;
	else
		scan_write_buffers = PAGE_EXEC;

    // make sure we have at least SIG_LEN 0 bytes at the end of the buffer
    // so we can search for sigs all the way to the end of the 2 sectors.

    MEMSET(search_buffer+2*SECTOR_SIZE,0,SIG_LEN);

	for (i=0;i<MEMORY_SIZE_IN_PAGES;i++)
	{
        if (hLocal->page_global.page_index[i].flags & scan_write_buffers)
		{

#ifdef SYM_NLM
			// relinquish control to NLM just in case! :)

            PAMRelinquishControl();
#endif

			// at this point, we want to scan each buffer which has been
			// executed, as well as the following sector.

            file_offset = global_offset + hLocal->CPU.offset_modifier;

			if (file_offset < 0)
                page_read_sector(hLocal,
                                 global_offset,
                                 hLocal->CPU.ulLowStartOffset,
								 global_offset,
								 hLocal->CPU.low_stream,
								 search_buffer);
			else
				page_read_sector(hLocal,
								 global_offset,
								 hLocal->CPU.image_start_offset,
								 file_offset,
								 hLocal->CPU.stream,
								 search_buffer);

			file_offset += SECTOR_SIZE;

			if (i < MEMORY_SIZE_IN_PAGES - 1)
			{
				// for last page in system, don't read in last page + 1
				// or we may blow up!

				if (file_offset < 0)
					page_read_sector(hLocal,
									 global_offset + SECTOR_SIZE,
									 hLocal->CPU.ulLowStartOffset,
									 global_offset + SECTOR_SIZE,
									 hLocal->CPU.low_stream,
									 search_buffer + SECTOR_SIZE);
				else
					page_read_sector(hLocal,
									 global_offset + SECTOR_SIZE,
									 hLocal->CPU.image_start_offset,
									 file_offset,
									 hLocal->CPU.stream,
									 search_buffer + SECTOR_SIZE);
			}
			else
                MEMSET(search_buffer + SECTOR_SIZE, 0, SECTOR_SIZE);

            // search the entire 1K.

            sig_num = find_sig(hLocal->hGPAM,
                               search_buffer,
                               2*SECTOR_SIZE + SIG_LEN);

			if (sig_num != NOT_FOUND)
				return(sig_num);
		}

		global_offset += SECTOR_SIZE;
	}

	return(NOT_FOUND);
}


//************************************************************************
// void fault_iret(hLocal)
//
// Purpose
//
//      This function pops the appropriate information from the virtual stack
//  and returns the state of the VM to what it was before the fault occured.
//  It also takes the VM out of the immune state (~LOCAL_FLAG_IMMUNE).  This
//  involves restoring all flags (prefixes, reps, size overrides, and restoring
//  the SS:SP.)  The SS:SP is changed during a fault to push information in an
//  unused area of VM.  This is done because some viruses check the stack
//  to see if they are being debugged. Therefore, if state information for the
//  fault were pushed on the stack, the virus could notice the change and might
//  not decrypt itself properly.
//
//  Control is relinquished on the NLM platform before control of the VM is
//  returned to the host program.
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
// 2/26/96 Carey created.
//************************************************************************

void fault_iret(PAMLHANDLE hLocal)
{
	WORD wStatus, wOldSS, wOldSP;

	// Stack =
	// IP
	// CS
	// FLAGS
	// OLD SS:SP
	// STATUS BITS

    hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_IMMUNE;

	hLocal->CPU.IP = pop_word(hLocal);
	hLocal->CPU.CS = pop_word(hLocal);

	hLocal->CPU.op_size_over = FALSE;       // don't want 32 bit POPFD!

	oc10011101(hLocal);                     /* pop flags */

	wOldSP = pop_word(hLocal);
	wOldSS = pop_word(hLocal);

	wStatus = pop_word(hLocal);

	hLocal->CPU.seg_over = DEFAULT_SEG;

	if (wStatus & SAVE_CS_PREFIX)
		hLocal->CPU.seg_over = CS_OVER;

	if (wStatus & SAVE_DS_PREFIX)
		hLocal->CPU.seg_over = DS_OVER;

	if (wStatus & SAVE_ES_PREFIX)
		hLocal->CPU.seg_over = ES_OVER;

	if (wStatus & SAVE_SS_PREFIX)
		hLocal->CPU.seg_over = SS_OVER;

	if (wStatus & SAVE_FS_PREFIX)
		hLocal->CPU.seg_over = FS_OVER;

	if (wStatus & SAVE_GS_PREFIX)
		hLocal->CPU.seg_over = GS_OVER;

	hLocal->CPU.rep_prefix = 0;

	if (wStatus & SAVE_REPZ_PREFIX)
		hLocal->CPU.rep_prefix = PREFIX_REPZ;

	if (wStatus & SAVE_REPNZ_PREFIX)
		hLocal->CPU.rep_prefix = PREFIX_REPNZ;

	if (wStatus & SAVE_OPERAND_PREFIX)
		hLocal->CPU.op_size_over = TRUE;
	else
		hLocal->CPU.op_size_over = FALSE;

	if (wStatus & SAVE_ADDRESS_PREFIX)
		hLocal->CPU.addr_size_over = TRUE;
	else
		hLocal->CPU.addr_size_over = FALSE;

	hLocal->CPU.ireg.X.SP = wOldSP;
	hLocal->CPU.SS = wOldSS;

	enable_prefetch_queue(hLocal);

#ifdef SYM_NLM
    PAMRelinquishControl();
#endif
}



//************************************************************************
// void prefetch_queue_to_vm(hLocal)
//
// Purpose
//
//      This function stores the entire contents of the prefetch queue data
//  structure to virtual memory, so a fault handler can modify its state and
//  contents.  The prefetch queue structure (prefetch_type) is stored at ES:DI
//  in virtual memory, so these registers should be set up before this occurs.
//  This function is invoked if the appropriate ARPL-signature is encountered
//  during emulation.  The prefetch queue is stored byte-for-byte into the VM
//  just as it is stored in actual memory (byte packed).
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
// 2/27/96 Carey created.
//************************************************************************

void prefetch_queue_to_vm(PAMLHANDLE hLocal)
{
    int     i;
    BYTE    *byPtr;

    byPtr = (BYTE *)&hLocal->CPU.prefetch;

	for (i=0;i<sizeof(prefetch_type);i++)
		put_byte(hLocal,hLocal->CPU.ES,
				 (WORD)(hLocal->CPU.ireg.X.DI + i),
				 byPtr[i]);

}


//************************************************************************
// void vm_to_prefetch_queue(hLocal)
//
// Purpose
//
//      This function reads sizeof(prefetch_type) bytes from the VM from ES:DI
//  and fills up the VM's prefetch queue data structure with this information.
//  This allows a fault handler to modify a copy of the prefetch queue within
//  the VM and then have it adopted as the actual prefetch queue status.  This
//  function will boundary check all values before resetting the state of the
//  VM prefetch queue.  If any of the values are invalid, it will set the
//  prefetch reload bit in the queue data structure.  This will reload the
//  prefetch queue (which may affect VM emulation).
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
// 2/27/96 Carey created.
//************************************************************************

void vm_to_prefetch_queue(PAMLHANDLE hLocal)
{
	int     i;
	BYTE    *byPtr;
	WORD    wSize, wCurrentRequest, wNextRequest;

	wSize           = hLocal->CPU.prefetch.wSize;
	wCurrentRequest = hLocal->CPU.prefetch.wCurrentRequest;
	wNextRequest    = hLocal->CPU.prefetch.wNextRequest;

	byPtr = (BYTE *)&hLocal->CPU.prefetch;

	for (i=0;i<sizeof(prefetch_type);i++)
		*byPtr++ = get_byte(hLocal,
							hLocal->CPU.ES,
							(WORD)(hLocal->CPU.ireg.X.DI + i));

	// verify that the prefetch queue was not trashed...

	if (hLocal->CPU.prefetch.wHead > MAX_PREFETCH_QUEUE_SIZE)
	{
		hLocal->CPU.prefetch.wHead = 0;
		hLocal->CPU.prefetch.wFlags |= PREFETCH_FLAG_RELOAD;
	}

	if (hLocal->CPU.prefetch.wTail >= MAX_PREFETCH_QUEUE_SIZE)
	{
		hLocal->CPU.prefetch.wTail = 0;
		hLocal->CPU.prefetch.wFlags |= PREFETCH_FLAG_RELOAD;
	}

	if (wSize != hLocal->CPU.prefetch.wSize)
	{
		hLocal->CPU.prefetch.wSize = wSize;
		hLocal->CPU.prefetch.wFlags |= PREFETCH_FLAG_RELOAD;
	}

	if (wCurrentRequest != hLocal->CPU.prefetch.wCurrentRequest)
	{
		hLocal->CPU.prefetch.wCurrentRequest = wCurrentRequest;
		hLocal->CPU.prefetch.wFlags |= PREFETCH_FLAG_RELOAD;
	}

	if (wNextRequest != hLocal->CPU.prefetch.wNextRequest)
	{
		hLocal->CPU.prefetch.wNextRequest = wNextRequest;
		hLocal->CPU.prefetch.wFlags |= PREFETCH_FLAG_RELOAD;
	}

	// make sure flags are OK.

	if (hLocal->CPU.prefetch.wFlags &
		(WORD)~(PREFETCH_FLAG_RELOAD | PREFETCH_FLAG_FAULT))
		hLocal->CPU.prefetch.wFlags &=
			(PREFETCH_FLAG_RELOAD | PREFETCH_FLAG_FAULT);
}


//************************************************************************
// WORD check_for_immunity(hLocal)
//
// Purpose
//
//      This function is called whenever an ARPL instruction is encountered
//  in the instruction stream.  Actually, it is called whenenver a
//  hLocal->hGPAM->config_info.byImmuneSig[0] byte is encountered in the
//  instruction stream.  (Currently, this value is 63h 'c' ARPL)  This is
//  an invalid instruction on Intel processors under v86 and real mode.  If
//  this instruction is encounted, its because of one of two possibilities:
//
//  (1) The program we're scanning is corrupt or is a data file and we
//      randomly happened on an 'c' byte which was interpreted as an ARPL
//      instruction.
//  (2) The ARPL instruction was intentionally used by a low-memory handler
//      in the VM (such as a fault handler or interrupt handler).
//
//  NOTE: All config_info escape signatures must use the same start byte as
//  hLocal->hGPAM->config_info.byImmuneSig[0].
//
//  The two bytes following the ARPL indicate what function this escape code
//  is requesting.  Currently, there are 9 possible escape sequences.  All
//  other sequences beginning with ARPL are invalid.  Here are the current
//  sequences:
//
//  (1) byImmuneSig: If this signature is encountered, then the VM is placed
//      into Immune mode.  Instructions may be used without affecting the
//      exclusion state of PAM.  All emulated instructions (while this state
//      is active) count againt a max-execution count specific to Immune mode:
//      hLocal->hGPAM->config_info.ulMaxImmuneIter
//      Immune mode may not be re-entered.  When Immune mode is initially
//      entered, then a local count of immune iterations is reset.  This count
//      is increased for each emulated instruction.  If this count exceeds the
//      threshold mentioned above, then emulation is terminated.  During the
//      Immune mode, the prefetch queue and CSIP queue are unaltered.
//
//  (2) byNoFaultSig: If this signature is encountered, then a flag is set in
//      the VM:  hLocal->dwFlags |= LOCAL_FLAG_NO_FAULT;
//
//      This tells the VM that as soon as Immune mode is relinquished (via a
//      byFretSig), the VM should not fault on the first non-immune instruction.
//      This is used in the following situation:
//
//      Assume a fault is set on B8 (MOV AX, XXXX).  Upon encountering B8 in
//      the instruction stream, a fault is activated.  The MOV instruction does
//      not execute at this time.  The fault checks to see if it is dealing
//      with a particular virus.  It does not find what it is looking for.  It
//      issues an FRET instruction (byFretSig) which returns control to the B8
//      MOV instruction.  However, once again the VM detects the B8 MOV
//      and faults.  This would go on indefinitely.  However, if byNoFaultSig
//      is used, when the VM returns to the B8 MOV instruction, it will not
//      fault.  Rather, it will let the instruction execute and then reset the
//      hLocal->dwFlags by ANDing with ~LOCAL_FLAG_NO_FAULT.  The instruction
//      following the B8 would again be susceptible to faults.
//
//  (3) byFretSig: If this signature is encountered during execution, Immune
//      mode is exited via a call to fault_iret(hLocal).  Once Immune mode is
//      exited, all exclusions again apply.
//
//  (4) byLoadPreSig: This opcode sequence tells the prefetch queue module to
//      reload as soon as the virtual processor is out of Immune mode.  At
//      this time, the processor will reload the prefetch queue as if a JMP
//      were made to the instruction.
//
//  (5) byPreToVMSig: This opcode sequence stores the VM's prefetch queue into
//      virtual memory at ES:DI.  The BYTE-aligned bytes of the prefetch queue
//      are stored as-is into VM.  See function prefetch_queue_to_vm() for more
//      details. CPU.H contains the structure definition of the prefetch queue.
//
//  (6) byVMToPreSig: This opcode sequence loads the VM's prefetch queue from
//      virtual memory at ES:DI.  The BYTE-aligned bytes from virtual memory
//      are stored as-is into the prefetch queue.  If any values loaded into
//      the prefetch queue are invalid, the prefetch queue is reset (in the
//      same way as a byLoadPreSig reset).  See function vm_to_prefetch_queue()
//      for more details.  CPU.H contains the structure definition of the
//      prefetch queue.
//
//  (7) byFaultOff: This opcode sequence can be used to disable a given fault
//      for a specified number of hits.  Following the 3-BYTE opcode is a BYTE
//      specifying which of the 32 different faults should be disabled (0-31).
//      The next DWORD specifies how long the fault should be suspended.  A
//      DWORD value of 0 means "don't suspend this fault."
//
//  (8) byGetCSIPQ: This opcode sequence is used to retrieve the contents of
//      the CSIP queue during virus detection.  This feature is not available
//      during repair.  The CSIP queue is a queue of the last 64 CS:IP values
//      of executed instructions.  This can be used to locate the starting
//      opcode bytes for each of the last 64 instructions.  The AX register is
//      used to select between the 64 CS:IP values.  AX=1 selects the CS:IP
//      of the latest, non-Immune instruction executed.  AX=64 selects the
//      CS:IP of the oldest instruction executed.  ES:BX is set to the
//      desired CS:IP value.  The CS:IP queue is updated in the interpret()
//      function in this source file.  The CS:IP queue is initialized to FF's
//      during local_init_cpu.  If a target file has executed 5 instructions,
//      and this opcode is used with AX > 5, then ES:BX will be equal to
//      FFFF:FFFF.
//
//  (9) byInRepair: This opcode can be used to determine if the VM is in
//      detection mode or repair mode.  If the VM is in repair mode, the CF is
//      set.  Otherwise it is cleared.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      TRUE        if a valid fault opcode was found and interpreted
//                  the main interpretation loops use this result to
//                  determine code flow.  See interpret()
//
//      FALSE       otherwise
//
//************************************************************************
// 2/27/96 Carey created.
//************************************************************************

WORD check_for_immunity(PAMLHANDLE hLocal)
{
    WORD            i;
    BYTE            buffer[IMM_SIZE];
    PAMConfigType   *pConfig;

	// are we executing an immunity code sequence?

    pConfig = &(hLocal->hGPAM->config_info);

	for (i=0;i<IMM_SIZE-1;i++)
		buffer[i] = get_byte(hLocal,hLocal->CPU.CS,(WORD)(hLocal->CPU.IP+i));

    if (!MEMCMP(pConfig->byImmuneSig+1,buffer,IMM_SIZE-1))
	{
		// executing immune code now. up the counter

        if (!(hLocal->dwFlags & LOCAL_FLAG_IMMUNE))
        {
            hLocal->CPU.immune_iteration = 0;
            hLocal->CPU.next_nlm_immune_iter_stop = NLM_ITER_CHECK_POINT;

            hLocal->dwFlags |= LOCAL_FLAG_IMMUNE;

#ifdef SYM_NLM
            PAMRelinquishControl();
#endif

        }


		// skip past our signature

		hLocal->CPU.IP += (IMM_SIZE - 1);

		// suspend the prefetch queue
        // this will be unsuspended when we IRET from the interrupt

        suspend_prefetch_queue(hLocal);

        // reset # of immune iterations before we timeout (for crashes)
    }
    else if (!MEMCMP(pConfig->byNoFaultSig+1,buffer,IMM_SIZE-1))
	{
        // after returning from our IRET, no-fault the next instruction
		// executed (after IRET no_exclude goes to 0, and no_fault is checked)

        hLocal->dwFlags |= LOCAL_FLAG_NO_FAULT;

        // skip past our signature

		hLocal->CPU.IP += (IMM_SIZE - 1);
    }
    else if (!MEMCMP(pConfig->byFretSig+1,buffer,IMM_SIZE-1))
	{
        // This special IRET resets all the segment stuff...

        fault_iret(hLocal);
    }
    else if (!MEMCMP(pConfig->byLoadPreSig+1,buffer,IMM_SIZE-1))
	{
        // This special opcode forces the pre-fetch queue to be reloaded upon
        // return from the fault

		reset_prefetch_queue(hLocal);

		hLocal->CPU.IP += (IMM_SIZE - 1);
    }
    else if (!MEMCMP(pConfig->byPreToVMSig+1,buffer,IMM_SIZE-1))
	{
        // This special opcode forces the pre-fetch queue to be reloaded upon
        // return from the fault

        prefetch_queue_to_vm(hLocal);

		hLocal->CPU.IP += (IMM_SIZE - 1);
    }
    else if (!MEMCMP(pConfig->byVMToPreSig+1,buffer,IMM_SIZE-1))
	{
        // This special opcode forces the pre-fetch queue to be reloaded upon
        // return from the fault

        vm_to_prefetch_queue(hLocal);

		hLocal->CPU.IP += (IMM_SIZE - 1);
    }
    else if (!MEMCMP(pConfig->byFaultOff+1,buffer,IMM_SIZE-1))
	{
        BYTE        byFaultNum;
        DWORD       dwHowLong;

        // turn a fault off for the specified number of hits
        // the byte indicating the fault number immediately follows the ARPL
        // sequence....
        // the number of hits follows the fault number

        hLocal->CPU.IP += (IMM_SIZE - 1);

        // skip over the next 5 bytes in the instruction stream...

        byFaultNum = get_byte_at_csip(hLocal);
        dwHowLong = get_dword_at_csip(hLocal);

        if (byFaultNum < NUM_FAULTS)
            hLocal->CPU.suspend_fault[byFaultNum] = dwHowLong;
    }
    else if (!MEMCMP(pConfig->byGetCSIPQ+1,buffer,IMM_SIZE-1))
	{
        // Index in AX, result in ES:BX

        if (hLocal->CPU.preg.X.AX <= CSIP_QUEUE_SIZE)
        {
            int nIndex;

            nIndex = (int)hLocal->csip_queue.wIndex -
                (int)hLocal->CPU.preg.X.AX;

            if (nIndex < 0)
                nIndex += CSIP_QUEUE_SIZE;

            hLocal->CPU.ES = (WORD)(hLocal->csip_queue.dwCSIP[nIndex] >> 16);
            hLocal->CPU.preg.X.BX = (WORD)hLocal->csip_queue.dwCSIP[nIndex];
		}

		hLocal->CPU.IP += (IMM_SIZE - 1);
	}
	else if (!MEMCMP(pConfig->byInRepair+1,buffer,IMM_SIZE-1))
	{
        if (hLocal->dwFlags &
                (LOCAL_FLAG_REPAIR_DECRYPT | LOCAL_FLAG_REPAIR_REPAIR))
			hLocal->CPU.FLAGS.C = TRUE;
		else
			hLocal->CPU.FLAGS.C = FALSE;

		hLocal->CPU.IP += (IMM_SIZE - 1);
    }
    else
        return(FALSE);

    return(TRUE);
}



//************************************************************************
// void instruction_fault(hLocal, byFaultNum)
//
// Purpose
//
//  This function is called once the main interpretation loops have decided
//  to initiate a fault.  It rolls back the prefetch queue and CS:IP to point
//  to the first byte of the instruction that caused the fault.  It then
//  saves temporary copies of the SS:SP and establishes a new SS:SP to save
//  all VM state data on.  This new stack is established so the program's
//  stack is not modified in any way.  Some viruses use information on the
//  stack and will not function correctly if its contents are modified.
//  The CS:IP (of the faulted instruction), all flags, the original SS:SP, and
//  VM statuses (rep prefixes, overrides, etc) are pushed on the fault stack.
//  Each fault has a CS:IP specified in the configuration structure.  This
//  specifies the location of the fault handler in the VM.  All fault handlers
//  must be located within the first 32K of the VM (within the LOW.ASM data
//  area).  Control is relinqushed if running on the NLM.  Finally,  the VM
//  is placed in Immune mode.  This prevents the prefetch queue and the CSIP
//  queue from being used during emulation of the fault.  It also prevents
//  instructions and interrupts from excluding viruses.
//
//  NOTE: The faulted instruction does not have a chance to execute before
//  the fault.  It will execute after the fault handler executes, if the
//  fault handler returns to the original CS:IP with NoFault status set.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      BYTE        byFaultNum      Which fault should be launched
//
// Returns:
//
//      nothing
//
//************************************************************************
// 2/27/96 Carey created.
//************************************************************************

void instruction_fault(PAMLHANDLE hLocal,BYTE byFaultNum)
{
	WORD off, seg, wStatus;
	WORD wOldSS, wOldSP;

    // check to see if we're holding off on faults for the time being...


	roll_back_prefetch(hLocal);			// back up over fetched opcode
    suspend_prefetch_queue(hLocal);     // suspend during the fault

    wOldSS = hLocal->CPU.SS;
    wOldSP = hLocal->CPU.ireg.X.SP;

    hLocal->CPU.SS = hLocal->hGPAM->config_info.wFaultSS;
	hLocal->CPU.ireg.X.SP = hLocal->hGPAM->config_info.wFaultSP;

	off =
        (WORD)(hLocal->hGPAM->config_info.dwFaultSegOffArray[byFaultNum] & 0x0000FFFFU);
	seg =
        (WORD)(hLocal->hGPAM->config_info.dwFaultSegOffArray[byFaultNum] >> 16);

    // now push everything important.  Stack =
	// IP-1
    // CS
    // FLAGS
    // OLD SS:SP
    // STATUS BITS

    // 1. Status bits:

    wStatus = 0;

	switch(hLocal->CPU.seg_over)
	{
		case CS_OVER:
			wStatus |= SAVE_CS_PREFIX;
			break;
		case DS_OVER:
			wStatus |= SAVE_DS_PREFIX;
			break;
		case ES_OVER:
			wStatus |= SAVE_ES_PREFIX;
			break;
		case SS_OVER:
			wStatus |= SAVE_SS_PREFIX;
			break;
		case FS_OVER:
			wStatus |= SAVE_FS_PREFIX;
			break;
		case GS_OVER:
			wStatus |= SAVE_GS_PREFIX;
			break;
	}

	if (hLocal->CPU.rep_prefix & PREFIX_REPZ)
		wStatus |= SAVE_REPZ_PREFIX;

	if (hLocal->CPU.rep_prefix & PREFIX_REPNZ)
        wStatus |= SAVE_REPNZ_PREFIX;

    if (hLocal->CPU.op_size_over)
        wStatus |= SAVE_OPERAND_PREFIX;

    if (hLocal->CPU.addr_size_over)
        wStatus |= SAVE_ADDRESS_PREFIX;

    push_word(hLocal,wStatus);

    // 2. old SS:SP

    push_word(hLocal,wOldSS);
    push_word(hLocal,wOldSP);

    // 3. now the normal INT stuff is pushed

    // reset all prefixes, etc...

    hLocal->CPU.rep_prefix = 0;
    hLocal->CPU.seg_over = DEFAULT_SEG;
    hLocal->CPU.op_size_over = 0;
    hLocal->CPU.addr_size_over = 0;
    
    oc10011100(hLocal);                     /* pushf */

    // push IP-1, which will iret us back to the instruction we just faulted
    // on

	push_word(hLocal, hLocal->CPU.CS);
	push_word(hLocal, hLocal->CPU.IP-1);

    hLocal->CPU.CS = seg;
	hLocal->CPU.IP = off;

	// make sure we're in no_exclude mode!
	// and reset our # of iterations!


    if (!(hLocal->dwFlags & LOCAL_FLAG_IMMUNE))
    {
        hLocal->CPU.immune_iteration = 0;
        hLocal->CPU.next_nlm_immune_iter_stop = NLM_ITER_CHECK_POINT;

        hLocal->dwFlags |= LOCAL_FLAG_IMMUNE;

#ifdef SYM_NLM
        PAMRelinquishControl();
#endif
    }

}


//************************************************************************
// DWORD interpret(hLocal, found_string)
//
// Purpose
//
//      This function contains the main instruction interpretation loop.
//  It has the following pseudo-code:
//
//  Set up pointer variables for quick access to data structures.
//
//  LABEL TOP:
//
//      IF an error has occured THEN
//          Return with value of 0, indicating that the file was statically
//          excluded.  This tells the calling function not to scan any pages
//          for viruses.
//      ENDIF
//
//      Fetch the first BYTE of the next instruction.
//
//      Check for faults:
//      IF the opcode BYTE has a fault waiting on it AND
//         the number of emulated instructions is greater than the minimum
//         required instruction count for the specified fault AND
//         the VM is not in immune mode AND
//         the VM is not in no-fault mode THEN
//
//          IF the fault is being suspended THEN
//              Decrement the suspend count (when it reaches 0, the fault is
//              no longer suspended)
//          ELSE
//              Call the instruction_fault function. (go into Immune mode, etc)
//              GOTO TOP
//              (NOTE: This GOTO prevents the faulted instruction from
//               executing.  The next instruction to be fetched will be the
//               first instruction of the fault handler, in Immune mode.  Also
//               note that the CS:IP queue does not contain the faulted
//               instruction.)
//          ENDIF
//      ENDIF
//
//      Check for instruction runs:
//      IF we have seen the same exact opcode 32 times in a row THEN
//          Invoke the exclusion manager with the RUN_32 exclusion.
//      ENDIF
//
//      Check if we need to relinquish control on the NLM:
//      IF our number of iterations is sufficient THEN
//          Relinquish control to other NLM threads.
//      ENDIF
//
//      Check to see if we have excecuted 16 instructions:
//      IF we are not in Immune mode AND
//         we have executed more than 16 instructions AND
//         we just crossed this threshold THEN
//          Invoke the exclusion manager with the STOP_AFTER_16 exclusion.
//      ENDIF
//
//      Check to see if we have excecuted 1024 instructions:
//      IF we are not in Immune mode AND
//         we have executed more than 1024 instructions AND
//         we just crossed this threshold THEN
//          Invoke the exclusion manager with the STOP_AFTER_1K exclusion.
//      ENDIF
//
//      Check to see if the fetched opcode is an ARPL (special) opcode:
//      IF the fetched opcode is NOT an ARPL opcode THEN
//          IF the VM is not in Immune mode THEN
//              Increase the number of instructions feteched. (NOTE: This
//              is used to determine how many instructions have been fetched
//              as opposed to how many instructions have been emulated.  The
//              two quantities differ when REPZ MOVS/SCAS type instructions
//              are used.  In these cases, two instructions are fetched but
//              many thousands of iterations may be executed.  This
//              instructionFetchCount variable is used in the exclusion
//              manager.  If hLocal->hGPAM->config_info.ulMinNoExcludeCount
//              is non-zero, then no exclusions are applied until
//              instructionFetchCount reaches this value.
//              The value of
//              hLocal->hGPAM->config_info.ulMinNoExcludeCount must be
//              strictly less than 16.)
//
//              Update the CS:IP contents of the queue.
//
//              IF the current opcode matches the last fetched opcode THEN
//                  Increment the instruction run count.
//              ELSE
//                  Zero the instruction run count.
//              ENDIF
//
//              Establish a new last instruction (to detect instruction runs).
//          ENDIF
//
//          Exclude all viruses that cannot use the opcode. (NOTE: If the VM
//          is in immune mode, then this call has no effect.)
//      ELSE (the fetched opcode was an ARPL (special) opcode)
//          Check to see if we've encountered a special ARPL opcode.
//              (NOTE: during this check, actions are taken to change the
//               state of the VM.  If TRUE is returned from the check, then
//               an action was taken.  If FALSE is returned, then the ARPL
//               instruction was not recognized.)
//          IF we did encounter a valid ARPL instruction (TRUE return) THEN
//              Increase the number of immune iterations.
//              IF we exceeded the maximum number of immune iterations THEN
//                  Return to the calling function.
//              ENDIF
//          ELSE (invalid ARPL instruction)
//              Invoke the exclusion manager with the ARPL exclusion. Currently
//              this is just a standard exclude_item(hLocal, 'c'), where 'c'
//              is the first byte of all ARPL opcodes.
//          ENDIF
//      ENDIF (the fetched opcode was an ARPL)
//
//      Check if all viruses have been excluded:
//      IF all viruses have been excluded AND
//         the VM is not in Immune mode THEN
//          Return to the calling function.
//      ENDIF
//
//      Update number of emulated instructions:
//      IF VM is in Immune mode THEN
//          Increment the number of immune iterations.
//          IF the number of immune iterations exceeds the maximum immune
//             iterations THEN
//              Return to the calling function.
//          ENDIF
//      ELSE (not in Immune mode)
//          Increment the number of normal iterations.
//          IF the number of iterations equals the cache check
//              iteration count and the cache holds a state the
//              same as the current state THEN
//               Set the next prefetch queue request to NO_REQUEST
//               Return to the calling function.
//          ENDIF
//          Update the no-fault status.  Now that we have passed all fault
//          checks (done above), we can reset the no-fault status.  The next
//          instruction fetched will be capable of faulting.
//      ENDIF
//
//      Set the reset_seg flag.  (NOTE: When an instruction is emulated, it
//      can ZERO this reset_seg variable.  All prefix and override instructions
//      ZERO its value to indicate that they want all overrides to be
//      persistent for at least one more instruction.  All other instructions
//      do not modify its value.)
//
//      Emulate the instruction. (call to large_switch)
//
//      Check to see if we need to reset all prefixes and overrides:
//      IF the instruction just emulated was not a prefix/override THEN
//          Reset segment overrides.
//          Reset REP prefixes.
//          Reset 32-bit overrides (addressing and operand size)
//      ENDIF
//
//      Check to see if we need to invoke the exclusion system because of
//      iteration counts:
//      IF the VM is not in Immune mode AND
//         the number of iterations is larger than the next specified
//         iteration stop point THEN
//          IF the VM has executed more than 16K instructions THEN
//              Invoke the exclusion manager with the STOP_AFTER_16K exclusion.
//          ENDIF
//
//          ...
//
//          IF the VM has executed more than 128K instructions THEN
//              Invoke the exclusion manager with the STOP_AFTER_128K
//              exclusion.
//          ENDIF
//
//          Update the next iteration check-point.
//
//          Check to see if our iteration count has exceeded the maximum
//          number of iterations.
//          IF the iteration count is too high THEN
//              Return to the calling function.
//          ENDIF
//
//          Search for virus signatures in all modified buffers.
//          IF a signature was found THEN
//              Return to the calling function.
//          ENDIF
//      ENDIF
//
//  GOTO TOP (END OF FUNCTION)
//
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//      WORD        *found_string   Set to a virus ID # if a virus is found
//
// Returns:
//
//      !0 = The number of instructions emulated.
//      0  = means none were emulated, and all viruses were statically excluded
//           before emulation began (i.e. there's no reason to scan buffers for
//           viruses).  It could also mean that an error occured.
//
//************************************************************************
// 2/27/96 Carey created.
//************************************************************************

DWORD interpret(PAMLHANDLE hLocal, WORD *found_string)
{
	BYTE            opcode, immunity_opcode;
	WORD            wLastOpcode, wNum;
	DWORD           dwLastHandledIter = 0;

#ifdef USE_CACHE

    DWORD           dwCacheCheckIter =
                        hLocal->hGPAM->config_info.dwCacheCheckIter;

#endif // USE_CACHE

	PAMConfigType   *pConfig;
	csip_queue_type *pCSIPQ;
//    char            flags[26];

	// if we have an immunity opcode then we want to test for possible
	// immunity signature


//////////////////////////////////////////////////////////////////////////////
#ifdef BUILD_SET

	BYTE            stop_sig[8];
	FILE            *stream;
	BYTE			hits[256] = {0};
	int             i;

	switch (hLocal->CPU.prefetch.wCurrentRequest)
	{
		case PREFETCH_32_BYTE:
			printf("32 byte prefetch used.\n");
			break;
		case PREFETCH_16_BYTE:
			printf("16 byte prefetch used.\n");
			break;
		case PREFETCH_8_BYTE:
			printf("8 byte prefetch used.\n");
			break;
		case PREFETCH_0_BYTE:
			printf("(pentium) 0 byte prefetch used.\n");
			break;
	}

	stream = fopen("build.sig","rt");
	if (!stream)
	{
		printf("Unable to open BUILD.SIG file.\n");
		exit(0);
	}

	fscanf(stream,"%x %x %x %x %x %x %x %x",
			&stop_sig[0],
			&stop_sig[1],
			&stop_sig[2],
			&stop_sig[3],
			&stop_sig[4],
			&stop_sig[5],
			&stop_sig[6],
			&stop_sig[7]);

	printf("Stop sig: ");
	for (i=0;i<8;i++)
		printf("%02X ",stop_sig[i]);
	printf("\n");

	fclose(stream);



#endif
//////////////////////////////////////////////////////////////////////////////

    pConfig = &(hLocal->hGPAM->config_info);

    pCSIPQ = &(hLocal->csip_queue);

	immunity_opcode = pConfig->byImmuneSig[0];

	wLastOpcode = 0xFFFFU;			// no last opcode to start with!

	*found_string = NOT_FOUND;      // no virus found is default...


//////////////////////////////////////////////////////////////////////////////
#ifndef BUILD_SET

    // only stop executing if we are not immune.

    if ((hLocal->dwFlags & LOCAL_FLAG_NO_CAND_LEFT) &&
        !(hLocal->dwFlags & (LOCAL_FLAG_IMMUNE |
                             LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
		return(0);                      // no iterations executed

#endif
//////////////////////////////////////////////////////////////////////////////

	// By this time CPU.iteration has already been set to 1

	for (;;)
	{
        if (hLocal->dwFlags & LOCAL_FLAG_ERROR)
            return(0);                          // error for whatever reason

		opcode = get_byte_at_csip(hLocal);

		///////////////////////
/*
		strcpy(flags,"                    ");
		if (hLocal->CPU.FLAGS.C)
			flags[1] = 'C';

		if (hLocal->CPU.FLAGS.A)
			flags[3] = 'A';

		if (hLocal->CPU.FLAGS.O)
			flags[5] = 'O';

		if (hLocal->CPU.FLAGS.Z)
			flags[7] = 'Z';

		if (hLocal->CPU.FLAGS.S)
			flags[9] = 'S';

		flags[18] = 0;

		printf("IP>%04X:%04X OP>%02X AX>%08lX BX>%08lX CX>%08lX DX>%08lX\n",
			  hLocal->CPU.CS,
			  hLocal->CPU.IP-1,
			  opcode,
			  hLocal->CPU.preg.D.EAX,
			  hLocal->CPU.preg.D.EBX,
			  hLocal->CPU.preg.D.ECX,
			  hLocal->CPU.preg.D.EDX);
		printf("%18s SI>%08lX DI>%08lX DS>%04X ES>%04X SS>%04X\n",
			  flags,
			  hLocal->CPU.ireg.D.ESI,
			  hLocal->CPU.ireg.D.EDI,
			  hLocal->CPU.DS,
			  hLocal->CPU.ES,
			  hLocal->CPU.SS);

		printf("                   BP>%08lX SP>%08lX\n",
			  hLocal->CPU.ireg.D.EBP,
			  hLocal->CPU.ireg.D.ESP);



		getchar();

*/
		//////////////////////////


		////////////////////////////////////////////////////////////////
		// at this point, see if we need to instruction fault
		// don't fault if we're in no-exclude mode
		// also don't fault if we are in no-fault mode (set by an ARPL)
		// prefix code sequence
		////////////////////////////////////////////////////////////////

        if ((wNum = pConfig->byInstrFaultTable[opcode]) != 0xFF &&
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

		if (hLocal->instr_run > 32)
        {
            // if we have a run of > 32, then exclude this as a data file...

			exclude_item(hLocal,RUN_32);
		}

		// now take care of max instructions executed...

#ifdef SYM_NLM
        if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
        {
            if (hLocal->CPU.immune_iteration >
                hLocal->CPU.next_nlm_immune_iter_stop)
            {
                // relinquish control to other NLM's on NLM platforms

                hLocal->CPU.next_nlm_immune_iter_stop += NLM_ITER_CHECK_POINT;
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

        if ((!(hLocal->dwFlags & LOCAL_FLAG_IMMUNE)) &&
            dwLastHandledIter < 16UL &&
            hLocal->CPU.iteration > 16UL)
		{
            dwLastHandledIter = 16UL;
			exclude_item(hLocal,STOP_AFTER_16);
        }

        if ((!(hLocal->dwFlags & LOCAL_FLAG_IMMUNE)) &&
            dwLastHandledIter < 1024UL &&
            hLocal->CPU.iteration > 1024UL)
        {
            dwLastHandledIter = 1024UL;
            exclude_item(hLocal,STOP_AFTER_1K);
        }

		/////////////////////////////////////////////////////////////////////
		// if we get a special "immunity" opcode then we are probably executing
		// ISR code or have a crash.  If we are executing ISR code, then there
        // is the possibility that we will have code which we want to execute
        // without exclusion.  Check this out in the ARPL instruction code.
        /////////////////////////////////////////////////////////////////////

		if (opcode != immunity_opcode)
		{
            // Only increment instruction fetch count and
            //  only queue cs:ips
            //  on real (non-exclude) fetches

            if (!(hLocal->dwFlags & LOCAL_FLAG_IMMUNE))
            {
                // Increment the instruction fetch count

                hLocal->CPU.instructionFetchCount++;

                // Queue the cs:ip

                pCSIPQ->dwCSIP[pCSIPQ->wIndex] =
                    (((DWORD)hLocal->CPU.CS) << 16) |
                    (hLocal->CPU.IP - 1);
                if (++pCSIPQ->wIndex >= CSIP_QUEUE_SIZE)
                    pCSIPQ->wIndex = 0;

                if (opcode == wLastOpcode)
                    hLocal->instr_run++;
                else
                    hLocal->instr_run = 0;

                wLastOpcode = (WORD)opcode;
            }

            exclude_item(hLocal,opcode);
        }
		else
		{
			if (check_for_immunity(hLocal))
            {
                // increase # of immune instructions just in case we
                // encounter many immune instructions in a row...

                hLocal->CPU.immune_iteration++;

                if (hLocal->CPU.immune_iteration >=
                    hLocal->CPU.max_immune_iteration)
                    return(hLocal->CPU.iteration);

				continue;
            }
			else
				exclude_item(hLocal,opcode);
		}


/////////////////////////////////////////////////////////////////////////////
#ifdef BUILD_SET

		if (hLocal->CPU.iteration % 1000 == 0)
			fprintf(stderr,"Iteration: %8ld\r",hLocal->CPU.iteration);

		hLocal->CPU.iteration++;

        if (kbhit())
        {
            char temp[5];

			getch();
            printf("\n\nAbort emulation? ");
            gets(temp);
            if (temp[0] == 'y' || temp[0] == 'Y')
                return(FALSE);
        }

		if (opcode == stop_sig[0] || hLocal->CPU.iteration >= dwGSIter)
		{
			int i, len;

			len = 8;

			if (hLocal->CPU.iteration >= dwGSIter)
				i = len;
			else
				for (i=1;i<len;i++)
					if (get_byte(hLocal,
								 hLocal->CPU.CS,
								 hLocal->CPU.IP + i - 1) != stop_sig[i])
						break;

			if (i == len)
			{
				stream = fopen("build.set","rb+");
				if (!stream)
				{
					printf("Unable to open BUILD.SET file.\n");
					exit(0);
				}

				for (i=0;i<256;i++)
					if (hits[i])
					{
						fseek(stream,i,SEEK_SET);
						fputc('1',stream);
					}

				printf("\n\nHit signature!\n");
				fclose(stream);

                *found_string = TRUE;

				return (FALSE);
			}
		}

		hits[opcode] = TRUE;



#else
/////////////////////////////////////////////////////////////////////////////

		// check if all viruses have been excluded.  If so, exit out and
		// scan all buffers

        if ((hLocal->dwFlags & LOCAL_FLAG_NO_CAND_LEFT) &&
            !(hLocal->dwFlags & (LOCAL_FLAG_IMMUNE |
                                 LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
			return(hLocal->CPU.iteration);


        // Increase the number of instructions executed so far
        //
        // the following if statements must be placed before the large_switch
        // because when the iret instruction is interpreted, it decrements
        // the no_exclude count and will foul up the no_fault functionality

        if (hLocal->dwFlags & LOCAL_FLAG_IMMUNE)
        {
            hLocal->CPU.immune_iteration++;

            if (hLocal->CPU.immune_iteration >=
                hLocal->CPU.max_immune_iteration)
                return(hLocal->CPU.iteration);
        }
        else
        {
            hLocal->CPU.iteration++;


#ifdef USE_CACHE

            // Are we at the # iterations to check cache?

            if (hLocal->CPU.iteration == dwCacheCheckIter &&
                CacheCheckPresence(hLocal) == TRUE)
            {
                hLocal->CPU.prefetch.wNextRequest = PREFETCH_NO_REQUEST;

                return (hLocal->CPU.iteration);
            }

#endif // USE_CACHE


            // make sure we allow instruction faults now.

            hLocal->dwFlags &= ~(DWORD)LOCAL_FLAG_NO_FAULT;
        }

#endif

        hLocal->CPU.reset_seg = TRUE;   /* reset SEG/operand/address OVERRIDE */
                                        /* unless this is explicitly changed */
                                        /* by CS: ES:, SS:, DS:, FS: GS: or */
                                        /* 66h=operand->32-bit, 67h=addr->32-bit */

		large_switch(hLocal,opcode);

        if (TRUE == hLocal->CPU.reset_seg)
		{
            reset_seg_over(hLocal);     /* in the case of segment prefixes */
										/* this is not reset. */

            reset_rep_over(hLocal);     /* reset REPNZ/REPZ prefix overrides */

            reset_32_bit_over(hLocal);  /* forget about 32 bit stuff */
        }
        
        // every ITER_CHECK_POINT iterations check to see if we have
		// decrypted enough of the sample to catch a virus.  Scan for
		// search strings even though we haven't finished executing yet,
		// and perhaps we'll find one and can exit out...

		if (!(hLocal->dwFlags & LOCAL_FLAG_IMMUNE) &&
            hLocal->CPU.iteration >= hLocal->CPU.next_iter_stop)
		{

            if (hLocal->CPU.iteration >= 16384UL)
            {
                exclude_item(hLocal,STOP_AFTER_16K);
            }


            if (hLocal->CPU.iteration >= 32768UL)
            {
                exclude_item(hLocal,STOP_AFTER_32K);
            }

            if (hLocal->CPU.iteration >= 49152UL)
            {
                exclude_item(hLocal,STOP_AFTER_48K);
            }

            if (hLocal->CPU.iteration >= 65536UL)
            {
                exclude_item(hLocal,STOP_AFTER_64K);
            }

            if (hLocal->CPU.iteration >= 81920UL)
            {
                exclude_item(hLocal,STOP_AFTER_80K);
            }

            if (hLocal->CPU.iteration >= 98304UL)
            {
                exclude_item(hLocal,STOP_AFTER_96K);
            }

            if (hLocal->CPU.iteration >= 131072UL)
            {
                exclude_item(hLocal,STOP_AFTER_128K);
            }

            hLocal->CPU.next_iter_stop += ITER_CHECK_POINT;

            if (hLocal->CPU.iteration >= hLocal->CPU.max_iteration)
            {
                // hit the max # of iterations

				return(hLocal->CPU.iteration);
            }

#ifndef BUILD_SET

            *found_string = search_buffers_for_string(hLocal);

			if (*found_string != NOT_FOUND)
				return(hLocal->CPU.iteration);
#endif

		}
	}
}

//************************************************************************
// BOOL CacheCheckPresence(hLocal)
//
// Purpose
//
//      This function contains checks to see if the CPU cache contains
//  an entry with the same state as the current state of the emulator.
//
// Parameters:
//
//      PAMLHANDLE  hLocal          A local PAM handle
//
// Returns:
//
//      FALSE = If the caller should not stop emulating
//      TRUE  = If the cache contains such an entry and the caller
//              should stop emulating
//
//************************************************************************
// 3/1/96 Carey created.
//************************************************************************

#ifdef USE_CACHE

BOOL CacheCheckPresence(PAMLHANDLE hLocal)
{
    // Don't want to stop emulating if in immune mode

    if (!(hLocal->dwFlags & (LOCAL_FLAG_IMMUNE |
                             LOCAL_FLAG_IMMUNE_EXCLUSION_PERM)))
    {
        // Only fill the temp cache entry if this is the
        // first emulation iteration.

        if (hLocal->stTempEntry.dwLRUIter == 0)
        {
            // Get cache entry info

            StoreCPUStateIntoTemp(hLocal,&hLocal->stTempEntry.stCPUState);

            // Has such an entry been cached?

            if (CacheLookupItem(&hLocal->hGPAM->stCache,
                                &hLocal->stTempEntry.stCPUState) ==
                CACHESTATUS_ENTRY_FOUND)
            {
                // If such an entry

                return (TRUE);
            }
        }
    }

    return (FALSE);
}

#endif  // USE_CACHE
