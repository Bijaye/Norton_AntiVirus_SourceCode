/*
Copyright Notice
================
BOCHS is Copyright 1994,1995,1996 by Kevin P. Lawton.

BOCHS is commercial software.

For more information, read the file 'LICENSE' included in the bochs
distribution.  If you don't have access to this file, or have questions
regarding the licensing policy, the author may be contacted via:

	 US Mail:  Kevin Lawton
				  528 Lexington St.
				  Waltham, MA 02154

	 EMail:    bochs@world.std.com
*/




#define BX_DECODE_C 1


#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#ifndef WIN32
	#include <sys/time.h>
	#include <unistd.h>
#else
	#include <time.h>
#endif
#include <ctype.h>
#include <stdio.h>


#include "bochs.h"
#include "instr_proto.h"
#include "iodev/pic.h"
#include "extra.h"

#include "..\bochsw\watcher\watcher.h"
#include "..\bochsw\watcher\watch_ins_tbl.h"
//	#define A20ADDR(x) ( (x) & 0xffffff )

/* end prototypes */

extern volatile Boolean bx_fd_event;
extern bx_timer_t bx_timer[];
extern int max_fd;

Boolean bx_single_step_event = 0;

extern int registered_fds[BX_MAX_REGISTERED_FDS];
extern void (* registered_fd_handler[BX_MAX_REGISTERED_FDS])(void);
extern int num_registered_fds;

#ifndef WIN32
struct timeval timeout;
#endif

Boolean bx_rep_prefix;

extern Bit32u  bx_num_ticks;


Bit8u *bx_8bit_gen_reg[8] = {
	(Bit8u *) (((Bit8u *) &bx_cpu.eax) + BX_REG8L_OFFSET),
	(Bit8u *) (((Bit8u *) &bx_cpu.ecx) + BX_REG8L_OFFSET),
	(Bit8u *) (((Bit8u *) &bx_cpu.edx) + BX_REG8L_OFFSET),
	(Bit8u *) (((Bit8u *) &bx_cpu.ebx) + BX_REG8L_OFFSET),
	(Bit8u *) (((Bit8u *) &bx_cpu.eax) + BX_REG8H_OFFSET),
	(Bit8u *) (((Bit8u *) &bx_cpu.ecx) + BX_REG8H_OFFSET),
	(Bit8u *) (((Bit8u *) &bx_cpu.edx) + BX_REG8H_OFFSET),
	(Bit8u *) (((Bit8u *) &bx_cpu.ebx) + BX_REG8H_OFFSET)
};

Bit16u *bx_16bit_gen_reg[8] = {
	(Bit16u *) (((Bit8u *) &bx_cpu.eax) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ecx) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.edx) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ebx) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.esp) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ebp) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.esi) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.edi) + BX_REG16_OFFSET)
};

Bit32u *bx_32bit_gen_reg[8] = {
	&bx_cpu.eax,
	&bx_cpu.ecx,
	&bx_cpu.edx,
	&bx_cpu.ebx,
	&bx_cpu.esp,
	&bx_cpu.ebp,
	&bx_cpu.esi,
	&bx_cpu.edi
};

/* hack for the following defs.  Its easier to decode mod-rm bytes if
	you can assume there's always a base & index register used.  For
	modes which don't really use them, point to an empty (zeroed) register.
 */
static Bit32u empty_register = 0;

/* 16bit address mode base register, used for mod-rm decoding */
Bit16u *bx_16bit_base_reg[8] = {
	(Bit16u *) (((Bit8u *) &bx_cpu.ebx) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ebx) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ebp) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ebp) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &empty_register) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &empty_register) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ebp) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.ebx) + BX_REG16_OFFSET)
};

/* 16bit address mode index register, used for mod-rm decoding */
Bit16u *bx_16bit_index_reg[8] = {
	(Bit16u *) (((Bit8u *) &bx_cpu.esi) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.edi) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.esi) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.edi) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.esi) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &bx_cpu.edi) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &empty_register) + BX_REG16_OFFSET),
	(Bit16u *) (((Bit8u *) &empty_register) + BX_REG16_OFFSET),
};


/* This array defines a look-up table for the even parity-ness
	of an 8bit quantity, for optimal assignment of the parity bit
	in the EFLAGS register
  */
Boolean bx_parity_lookup[256] = {
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
	1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};





Bit8u lastone;


#if USE_INLINE
	#include "arith.c"
	#include "arith2.c"
	#include "bit.c"
	#include "ctrl_xfer.c"
	#include "data_xfer.c"
	#include "flag_ctrl.c"
	#include "io.c"
	#include "logical.c"
	#include "shift.c"
	#include "ndp.c"
	#include "proc_ctrl.c"
	#include "protect_ctrl.c"
	#include "segment_ctrl.c"
	#include "soft_int.c"
	#include "stack.c"
	#include "string.c"
	#include "bcd.c"
#endif


#include "tick.h"

// These are used for the timeout (in seconds)
int timeoutauxiliarycount=0;
int cpuloopstarttime=0;
extern int sectimeout;

void
bx_cpu_loop(void)
{
	//int byte_count;
	Bit8u next_byte;
	//Bit8u mod_rm_byte, mod, opcode, rm;
	BYTE int_no;
	int lastint_no;
	WORD return_CS;
	WORD return_IP;
	DWORD return_address;
	RequestDescription request; // This structure is used to compare two requests to a same interrupt (often 21h)
								// if all the regs in this struct fit, the requests are assumed to be equivalent
								// We use this at a breakpoint on an interrupt entry point to see if we had
								// already recorded a request on the interrupt stack or if the request appears to be
								// new, supposedly from a virus that has issued a far call to the int21 entry point
	int deltatick; //DEBUG
	int i;

	//extern unsigned long revert;
	//extern unsigned long max_count;

#ifdef WATCH_ENABLED
	extern bx_options_t bx_options;
#endif

	bx_cpu.errno = 0;

	cpuloopstarttime = time(NULL);

	for(;;)
	{

		// This is a timeout in seconds, we check the time only once every 1000 loops for speed
		if (sectimeout && ++timeoutauxiliarycount>256) {
			timeoutauxiliarycount = 0;
			if (time(NULL)-cpuloopstarttime>sectimeout) break;
		}

		instruction_tick++; // This counter is used for the audit

#if BX_CPU >= 2
		bx_cpu.EXT = 0;
		bx_cpu.prev_eip = bx_cpu.eip;	  /* save instruction pointer */
#endif
		bx_TICK();

		// I added the following breakpoint test for the audit
		// (useful to know when we pass by an interrupt entry point, for example)
		// unfortunately it may be very slow to test against a big number of breakpoints
		// thus, I only allowed 4 for the moment, and we'll see if we need more later, Fred
		if (checkAuditBrkPt) {
			if ((i = brkPtLookup(bx_cpu.cs.selector.value, bx_cpu.eip)) >= 0) {
				if (brkPtStack[i].tag < 0) {
					// The breakpoint is not in a particular place in memory
					report_tag = BRKPT;
					nadf_record[41].len = 4;
					nadf_record[41].object = &brkPtStack[i].tag;
					nadf_record[42].object = NULL;
					nadf_record[43].object = NULL;
					nadf_record[44].object = NULL;
					submit_record(nadf_record);
				}
				else {
					// A positive auditBrkPtTag indicates an int #. The breakpoint is on the entry point of this int
					int_no = brkPtStack[i].tag & 0xFF;
					if (notify [int_no]) {
						// The breakpoint is at the entry point of a watched interrupt

						// We compare the return address on the stack to the ones on the interrupt stack
						// If they fit, we do not push anything new on the stack since this request to interrupt int_no
						// has already been recorded
						bx_read_virtual_word(&bx_cpu.ss, SP, &return_IP);
						bx_read_virtual_word(&bx_cpu.ss, SP+2, &return_CS);
						return_address = (return_CS << 4) + return_IP;
						if (lookupIntStack(int_no, return_address) == -1) {
							// The return address was not found on the interrupt stack
							// We have to determine if the request has already been recorded on the interrupt stack

							// May be the virus has hooked 21h, and simply passes the requests through a far call
							// In this case, we search the last interrupt int_no on the stack, and compare the
							// current request (taken from the processor regs) to the one on the stack

							// If the requests are the same, then the original request issued by INT was just relayed
							// and we do not push anything on the stack

							// If the requests are different, then it may come from the virus as part of the infection
							// process, so we record the new request in the interrupt stack, just as if it had been issued
							// through an INT instruction
							if ((lastint_no = lookupIntStack_by_int_no(int_no)) == -1)
								// No interrupt of type int_no was found on the stack, so the request is new and we push it
								pushOnIntStack(int_no, BRKPT, return_address);
							else {
								// We have the last interrupt of type int_no on the stack
								// now let's see if the request recorded on the stack and the one being performed are the same
								fillRequest(&request);
								if (! sameRequest(&request, &int_audit_stack[lastint_no].req)) {
									// The requests are different, we push the new one on the stack
									pushOnIntStack(int_no, BRKPT, return_address);
									if (request.reqAX == 0x3D02 || request.reqAX == 0x4202 || request.reqAX&0xFF == 0x3F || request.reqAX&0xFF == 0x40)
										deltatick = instruction_tick - int_audit_stack[lastint_no].tick;
								}
								else {
									if (request.reqAX == 0x4B00 || request.reqAX&0xFF == 0x3D)
										deltatick = instruction_tick - int_audit_stack[lastint_no].tick;
								}
							}
						}
					}
				}
			}
		}

		if(bx_async_event)
		{
			/* INPUT PROCESSING ORDER:
			 * 1) instruction exception
			 * 2) single step
			 * 3) NMI
			 * 4) processor extension segment overrun
			 * 5) INTR
			 */
			/* ??? look at priorities for various cpu levels */

			/* single step recognized here */
			if( bx_single_step_event )
			{
				//bx_panic("decode: single step recognized\n"); // Needed for tunneling viruses
				/* should bx_cpu.EXT be set to 1 here ??? */
				bx_exception(1, 0, 0); /* single step interrupt */
				bx_single_step_event = 0;
				/* fall thru to other interrupt processing */
			}

			/* single step generated here */
			if( bx_cpu.eflags.tf )
			{
				bx_single_step_event = 1;
				//bx_panic("decode: single step generated\n"); // tunneling
			}

			/* if last instruction caused processing of interrupts to pend,
			 * such as STI or load of SS, then skip for one instruction */
			if(bx_cpu.inhibit_interrupts)
			{
				bx_cpu.inhibit_interrupts = 0;
			}

			else if( bx_cpu.INTR && bx_cpu.eflags.if_ )
			{
				Bit8u vector;

				vector = bx_IAC(); /* may set INTR with next interrupt */
				if(bx_dbg.interrupts) bx_printf("decode: interrupt %u\n",
														  (unsigned) vector);
            bx_cpu.errno = 0;
				bx_cpu.EXT   = 1;	/* external event */
				bx_interrupt(vector, 0, 0);
				if(bx_cpu.errno)
					bx_panic("decode: error processing interrupt\n");
			}

			if( !(bx_cpu.INTR || bx_fd_event || bx_single_step_event) )
				bx_async_event = 0;

		}

#if BX_CPU >= 3
		bx_cpu.is_32bit_opsize   = bx_cpu.cs.cache.u.segment.d_b;
		bx_cpu.is_32bit_addrsize = bx_cpu.cs.cache.u.segment.d_b;
#endif

		bx_cpu.segment_override = NULL;
		bx_rep_prefix = 0;

#if BX_CPU >= 2
		bx_cpu.errno = 0;
#endif


		/* Call debug to display current instruction.
			by conditional compile */
	#ifdef BX_DEBUGGER
		bx_debug(&bx_cpu.cs, bx_cpu.eip);
	#endif
		if(bx_dbg.debugger) bx_debug(&bx_cpu.cs, bx_cpu.eip);

#if 0
		if( (EAX & 0xFFFF0000) != 0  ||
			 (EBX & 0xFFFF0000) != 0  ||
			 (ECX & 0xFFFF0000) != 0  ||
			 (EDX & 0xFFFF0000) != 0  ||
			 (ESP & 0xFFFF0000) != 0  ||
			 (EBP & 0xFFFF0000) != 0  ||
			 (ESI & 0xFFFF0000) != 0  ||
			 (EDI & 0xFFFF0000) != 0  ||
			 (bx_cpu.eip & 0xFFFF0000) != 0
		  )
		{
			bx_printf("EAX: %08x\n", EAX);
			bx_printf("EBX: %08x\n", EBX);
			bx_printf("ECX: %08x\n", ECX);
			bx_printf("EDX: %08x\n", EDX);
			bx_printf("ESP: %08x\n", ESP);
			bx_printf("EBP: %08x\n", EBP);
			bx_printf("ESI: %08x\n", ESI);
			bx_printf("EDI: %08x\n", EDI);
			bx_printf("EIP: %08x\n", bx_cpu.eip);
			bx_panic("decode: Error: MSW set in 16bit register!\n");
		}
#endif

      base_eip = EIP;
      next_byte = bx_fetch_next_byte();
      instructions[next_byte]();
   }
}

// Here is the old switch, it is now table-driven, the table is in watch_ins_tbl.c


      /* On the 386 and above, instructions must be a maximum of 15 bytes long.
		 * this means redundant prefix codes can put the byte count over 15 and
		 * cause an illegal instruction.
		 */
		/* ??? byte_count */
/*
      for(byte_count=0; byte_count<15; byte_count++)
		{
			next_byte = bx_fetch_next_byte();

			/* ??? * /
			if(bx_cpu.errno) bx_panic("decode: fetch(): errno\n");

#if 0
	#if BX_CPU >= 2
			if(bx_cpu.errno) break;	/* exception occurred * /
	#endif
#endif

#ifdef WIN32DEBUG2
			bx_printf("opcode:::%04X - %08lX [%02X]\n",
						 bx_cpu.cs.cache.u.segment.base, bx_cpu.eip, next_byte);
#endif


         /*
         switch(next_byte)
			{

				case 0x00: bx_ADD_EbGb(); break;
				case 0x01: bx_ADD_EvGv(); break;
				case 0x02: bx_ADD_GbEb(); break;
				case 0x03: bx_ADD_GvEv(); break;
				case 0x04: bx_ADD_ALIb(); break;
				case 0x05: bx_ADD_eAXIv(); break;
				case 0x06: bx_PUSH_ES(); break;
				case 0x07: bx_POP_ES(); break;
				case 0x08: bx_OR_EbGb(); break;
				case 0x09: bx_OR_EvGv(); break;
				case 0x0A: bx_OR_GbEb(); break;
				case 0x0B: bx_OR_GvEv(); break;
				case 0x0C: bx_OR_ALIb(); break;
				case 0x0D: bx_OR_eAXIv(); break;
				case 0x0E: bx_PUSH_CS(); break;
				case 0x0F: // 2-byte escape *
					next_byte = bx_fetch_next_byte();
#if BX_CPU >= 2
					if(bx_cpu.errno) break;	// exception occurred *
#endif
					if( ++byte_count >= 15 )
					{
						bx_panic("decode: : byte_count >= 15\n");
						break;
					}
					switch(next_byte)
					{
						case 0x00: // Group 6 *
							mod_rm_byte = bx_peek_next_byte();
							BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
							switch(opcode)
							{
								case 0x00: bx_SLDT_Ew(); break;
								case 0x01: bx_STR_Ew(); break;
								case 0x02: bx_LLDT_Ew(); break;
								case 0x03: bx_LTR_Ew(); break;
								case 0x04: bx_VERR_Ew(); break;
								case 0x05: bx_VERW_Ew(); break;
								case 0x06: bx_panic("decode: : Grp6:\n"); break;
								case 0x07: bx_panic("decode: : Grp6:\n"); break;
								default: bx_panic("decode: error in bx_decode()!\n"); break;
							}
							break;
						case 0x01: // Group 7 *
							mod_rm_byte = bx_peek_next_byte();
							BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
							switch(opcode)
							{
								case 0x00: bx_SGDT_Ms(); break;
								case 0x01: bx_SIDT_Ms(); break;
								case 0x02: bx_LGDT_Ms(); break;
								case 0x03: bx_LIDT_Ms(); break;
								case 0x04: bx_SMSW_Ew(); break;
								case 0x05: bx_panic("decode: : Grp7:\n"); break;
								case 0x06: bx_LMSW_Ew(); break;
								case 0x07: bx_panic("decode: : Grp7:\n"); break;
								default: bx_panic("decode: error in bx_decode()!\n"); break;
							}
							break;

						case 0x02: bx_LAR_GvEw(); break;
						case 0x03: bx_LSL_GvEw(); break;
						case 0x04: bx_panic("decode: : 0F 04:\n"); break;
						case 0x05: bx_LOADALL(); break;
						case 0x06: bx_CLTS(); break;
						case 0x07: bx_panic("decode: : 0F 07:\n"); break;
						case 0x08: bx_INVD(); break;
						case 0x09: bx_WBINVD(); break;
						case 0x0A:
						case 0x0B:
						case 0x0C:
						case 0x0D:
						case 0x0E:
						case 0x0F: bx_panic("decode: : 0F XX:\n"); break;

						case 0x10:
						case 0x11:
						case 0x12:
						case 0x13:
						case 0x14:
						case 0x15:
						case 0x16:
						case 0x17:
						case 0x18:
						case 0x19:
						case 0x1A:
						case 0x1B:
						case 0x1C:
						case 0x1D:
						case 0x1E: bx_panic("decode: : 0F XX:\n"); break;
						case 0x1F: bx_PSEUDO_INT_Ib(); break;

						case 0x20: bx_MOV_RdCd(); break;
						case 0x21: bx_MOV_RdDd(); break;
						case 0x22: bx_MOV_CdRd(); break;
						case 0x23: bx_MOV_DdRd(); break;
						case 0x24: bx_MOV_RdTd(); break;
						case 0x25: bx_panic("decode: : 0F XX:\n"); break;
						case 0x26: bx_MOV_TdRd(); break;
						case 0x27:
						case 0x28:
						case 0x29:
						case 0x2A:
						case 0x2B:
						case 0x2C:
						case 0x2D:
						case 0x2E:
						case 0x2F: bx_panic("decode: : 0F XX:\n"); break;

						case 0x30:
						case 0x31:
						case 0x32:
						case 0x33:
						case 0x34:
						case 0x35:
						case 0x36:
						case 0x37:
						case 0x38:
						case 0x39:
						case 0x3A:
						case 0x3B:
						case 0x3C:
						case 0x3D:
						case 0x3E:
						case 0x3F: bx_panic("decode: : 0F XX:\n"); break;

						case 0x40:
						case 0x41:
						case 0x42:
						case 0x43:
						case 0x44:
						case 0x45:
						case 0x46:
						case 0x47:
						case 0x48:
						case 0x49:
						case 0x4A:
						case 0x4B:
						case 0x4C:
						case 0x4D:
						case 0x4E:
						case 0x4F: bx_panic("decode: : 0F XX:\n"); break;

						case 0x50:
						case 0x51:
						case 0x52:
						case 0x53:
						case 0x54:
						case 0x55:
						case 0x56:
						case 0x57:
						case 0x58:
						case 0x59:
						case 0x5A:
						case 0x5B:
						case 0x5C:
						case 0x5D:
						case 0x5E:
						case 0x5F: bx_panic("decode: : 0F XX:\n"); break;

						case 0x60:
						case 0x61:
						case 0x62:
						case 0x63:
						case 0x64:
						case 0x65:
						case 0x66:
						case 0x67:
						case 0x68: bx_panic("decode: : 0F XX:\n"); break;
							//case 0x69:*
#ifdef AUDIT_ENABLED
						case 0x69: bx_EXTRA_SERVICES(); break;
#else
						case 0x69:
#endif

						case 0x6A:
						case 0x6B:
						case 0x6C:
						case 0x6D:
						case 0x6E:
						case 0x6F: bx_panic("decode: : 0F XX:\n"); break;

						case 0x70:
						case 0x71:
						case 0x72:
						case 0x73:
						case 0x74:
						case 0x75:
						case 0x76:
						case 0x77:
						case 0x78:
						case 0x79:
						case 0x7A:
						case 0x7B:
						case 0x7C:
						case 0x7D:
						case 0x7E:
						case 0x7F: bx_panic("decode: : 0F XX:\n"); break;

						case 0x80: bx_JO_Jv(); break;
						case 0x81: bx_JNO_Jv(); break;
						case 0x82: bx_JB_Jv(); break;
						case 0x83: bx_JNB_Jv(); break;
						case 0x84: bx_JZ_Jv(); break;
						case 0x85: bx_JNZ_Jv(); break;
						case 0x86: bx_JBE_Jv(); break;
						case 0x87: bx_JNBE_Jv(); break;
						case 0x88: bx_JS_Jv(); break;
						case 0x89: bx_JNS_Jv(); break;
						case 0x8A: bx_JP_Jv(); break;
						case 0x8B: bx_JNP_Jv(); break;
						case 0x8C: bx_JL_Jv(); break;
						case 0x8D: bx_JNL_Jv(); break;
						case 0x8E: bx_JLE_Jv(); break;
						case 0x8F: bx_JNLE_Jv(); break;

						case 0x90: bx_SETO_Eb(); break;
						case 0x91: bx_SETNO_Eb(); break;
						case 0x92: bx_SETB_Eb(); break;
						case 0x93: bx_SETNB_Eb(); break;
						case 0x94: bx_SETZ_Eb(); break;
						case 0x95: bx_SETNZ_Eb(); break;
						case 0x96: bx_SETBE_Eb(); break;
						case 0x97: bx_SETNBE_Eb(); break;
						case 0x98: bx_SETS_Eb(); break;
						case 0x99: bx_SETNS_Eb(); break;
						case 0x9A: bx_SETP_Eb(); break;
						case 0x9B: bx_SETNP_Eb(); break;
						case 0x9C: bx_SETL_Eb(); break;
						case 0x9D: bx_SETNL_Eb(); break;
						case 0x9E: bx_SETLE_Eb(); break;
						case 0x9F: bx_SETNLE_Eb(); break;

						case 0xA0: bx_PUSH_FS(); break;
						case 0xA1: bx_POP_FS(); break;
						case 0xA2: bx_CPUID(); break;
						case 0xA3: bx_BT_EvGv(); break;
						case 0xA4: bx_SHLD_EvGvIb(); break;
						case 0xA5: bx_SHLD_EvGvCL(); break;
						case 0xA6: bx_CMPXCHG_XBTS(); break;
						case 0xA7: bx_CMPXCHG_IBTS(); break;
						case 0xA8: bx_PUSH_GS(); break;
						case 0xA9: bx_POP_GS(); break;
						case 0xAA: bx_panic("decode: : 0F AA:\n"); break;
						case 0xAB: bx_BTS_EvGv(); break;
						case 0xAC: bx_SHRD_EvGvIb(); break;
						case 0xAD: bx_SHRD_EvGvCL(); break;
						case 0xAE: bx_panic("decode: 0F AE:\n"); break;
						case 0xAF: bx_IMUL_GvEv(); break;

						case 0xB0: bx_CMPXCHG_EbGb(); break;
						case 0xB1: bx_CMPXCHG_EvGv(); break;
						case 0xB2: bx_LSS_GvMp(); break;
						case 0xB3: bx_BTR_EvGv(); break;
						case 0xB4: bx_LFS_GvMp(); break;
						case 0xB5: bx_LGS_GvMp(); break;
						case 0xB6: bx_MOVZX_GvEb(); break;
						case 0xB7: bx_MOVZX_GvEw(); break;
						case 0xB8: bx_panic("decode: 0F B8:\n"); break;
						case 0xB9: bx_panic("decode: 0F B9:\n"); break;
						case 0xBA: // Group 8 Ev,Ib *
							mod_rm_byte = bx_peek_next_byte();
							BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
							switch(opcode)
							{
								case 0x00:
								case 0x01:
								case 0x02:
								case 0x03: bx_panic("decode: Grp8:\n"); break;
								case 0x04: bx_BT_EvIb(); break;
								case 0x05: bx_BTS_EvIb(); break;
								case 0x06: bx_BTR_EvIb(); break;
								case 0x07: bx_BTC_EvIb(); break;
								default: bx_panic("decode: error in bx_decode()!\n"); break;
							}
							break;

						case 0xBB: bx_BTC_EvGv(); break;
						case 0xBC: bx_BSF_GvEv(); break;
						case 0xBD: bx_BSR_GvEv(); break;
						case 0xBE: bx_MOVSX_GvEb(); break;
						case 0xBF: bx_MOVSX_GvEw(); break;

						case 0xC0: bx_XADD_EbGb(); break;
						case 0xC1: bx_XADD_EvGv(); break;
						case 0xC2:
						case 0xC3:
						case 0xC4:
						case 0xC5:
						case 0xC6:
						case 0xC7: bx_panic("decode: 0F C7:\n"); break;
						case 0xC8: bx_BSWAP_EAX(); break;
						case 0xC9: bx_BSWAP_ECX(); break;
						case 0xCA: bx_BSWAP_EDX(); break;
						case 0xCB: bx_BSWAP_EBX(); break;
						case 0xCC: bx_BSWAP_ESP(); break;
						case 0xCD: bx_BSWAP_EBP(); break;
						case 0xCE: bx_BSWAP_ESI(); break;
						case 0xCF: bx_BSWAP_EDI(); break;

						case 0xD0:
						case 0xD1:
						case 0xD2:
						case 0xD3:
						case 0xD4:
						case 0xD5:
						case 0xD6:
						case 0xD7:
						case 0xD8:
						case 0xD9:
						case 0xDA:
						case 0xDB:
						case 0xDC:
						case 0xDD:
						case 0xDE:
						case 0xDF: bx_panic("decode: 0F DF:\n"); break;

						case 0xE0:
						case 0xE1:
						case 0xE2:
						case 0xE3:
						case 0xE4:
						case 0xE5:
						case 0xE6:
						case 0xE7:
						case 0xE8:
						case 0xE9:
						case 0xEA:
						case 0xEB:
						case 0xEC:
						case 0xED:
						case 0xEE:
						case 0xEF: bx_panic("decode: 0F EF:\n"); break;

						case 0xF0:
						case 0xF1:
						case 0xF2:
						case 0xF3:
						case 0xF4:
						case 0xF5:
						case 0xF6:
						case 0xF7:
						case 0xF8:
						case 0xF9:
						case 0xFA:
						case 0xFB:
						case 0xFC:
						case 0xFD:
						case 0xFE:
						case 0xFF:
							bx_printf("decode: instruction 0F Fx causes exception 6\n");
							bx_exception(6, 0, 0);
							break;

						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0x10: bx_ADC_EbGb(); break;
				case 0x11: bx_ADC_EvGv(); break;
				case 0x12: bx_ADC_GbEb(); break;
				case 0x13: bx_ADC_GvEv(); break;
				case 0x14: bx_ADC_ALIb(); break;
				case 0x15: bx_ADC_eAXIv(); break;
				case 0x16: bx_PUSH_SS(); break;
				case 0x17: bx_POP_SS(); break;
				case 0x18: bx_SBB_EbGb(); break;
				case 0x19: bx_SBB_EvGv(); break;
				case 0x1A: bx_SBB_GbEb(); break;
				case 0x1B: bx_SBB_GvEv(); break;
				case 0x1C: bx_SBB_ALIb(); break;
				case 0x1D: bx_SBB_eAXIv(); break;
				case 0x1E: bx_PUSH_DS(); break;
				case 0x1F: bx_POP_DS(); break;


				case 0x20: bx_AND_EbGb(); break;
				case 0x21: bx_AND_EvGv(); break;
				case 0x22: bx_AND_GbEb(); break;
				case 0x23: bx_AND_GvEv(); break;
				case 0x24: bx_AND_ALIb(); break;
				case 0x25: bx_AND_eAXIv(); break;
				case 0x26:
					bx_cpu.segment_override = &bx_cpu.es;
					continue;
					break;
				case 0x27: bx_DAA(); break;
				case 0x28: bx_SUB_EbGb(); break;
				case 0x29: bx_SUB_EvGv(); break;
				case 0x2A: bx_SUB_GbEb(); break;
				case 0x2B: bx_SUB_GvEv(); break;
				case 0x2C: bx_SUB_ALIb(); break;
				case 0x2D: bx_SUB_eAXIv(); break;
				case 0x2E:
#if BX_CPU <= 2
					bx_panic("decode: CS prefix used\n");
#else
					bx_cpu.segment_override = &bx_cpu.cs;
#endif
					continue;
					break;
				case 0x2F: bx_DAS(); break;

				case 0x30: bx_XOR_EbGb(); break;
				case 0x31: bx_XOR_EvGv(); break;
				case 0x32: bx_XOR_GbEb(); break;
				case 0x33: bx_XOR_GvEv(); break;
				case 0x34: bx_XOR_ALIb(); break;
				case 0x35: bx_XOR_eAXIv(); break;
				case 0x36:
					bx_cpu.segment_override = &bx_cpu.ss;
					continue;
					break;
				case 0x37: bx_AAA(); break;
				case 0x38: bx_CMP_EbGb(); break;
				case 0x39: bx_CMP_EvGv(); break;
				case 0x3A: bx_CMP_GbEb(); break;
				case 0x3B: bx_CMP_GvEv(); break;
				case 0x3C: bx_CMP_ALIb(); break;
				case 0x3D: bx_CMP_eAXIv(); break;
				case 0x3E:
					bx_cpu.segment_override = &bx_cpu.ds;
					continue;
					break;
				case 0x3F: bx_AAS(); break;

				case 0x40: bx_INC_eAX(); break;
				case 0x41: bx_INC_eCX(); break;
				case 0x42: bx_INC_eDX(); break;
				case 0x43: bx_INC_eBX(); break;
				case 0x44: bx_INC_eSP(); break;
				case 0x45: bx_INC_eBP(); break;
				case 0x46: bx_INC_eSI(); break;
				case 0x47: bx_INC_eDI(); break;
				case 0x48: bx_DEC_eAX(); break;
				case 0x49: bx_DEC_eCX(); break;
				case 0x4A: bx_DEC_eDX(); break;
				case 0x4B: bx_DEC_eBX(); break;
				case 0x4C: bx_DEC_eSP(); break;
				case 0x4D: bx_DEC_eBP(); break;
				case 0x4E: bx_DEC_eSI(); break;
				case 0x4F: bx_DEC_eDI(); break;


				case 0x50: bx_PUSH_eAX(); break;
				case 0x51: bx_PUSH_eCX(); break;
				case 0x52: bx_PUSH_eDX(); break;
				case 0x53: bx_PUSH_eBX(); break;
				case 0x54: bx_PUSH_eSP(); break;
				case 0x55: bx_PUSH_eBP(); break;
				case 0x56: bx_PUSH_eSI(); break;
				case 0x57: bx_PUSH_eDI(); break;
				case 0x58: bx_POP_eAX(); break;
				case 0x59: bx_POP_eCX(); break;
				case 0x5A: bx_POP_eDX(); break;
				case 0x5B: bx_POP_eBX(); break;
				case 0x5C: bx_POP_eSP(); break;
				case 0x5D: bx_POP_eBP(); break;
				case 0x5E: bx_POP_eSI(); break;
				case 0x5F: bx_POP_eDI(); break;


				case 0x60: bx_PUSHAD(); break;
				case 0x61: bx_POPAD(); break;
				case 0x62: bx_BOUND_GvMa(); break;
				case 0x63: bx_ARPL_EwGw(); break;
				case 0x64:
#if BX_CPU <= 2
					bx_panic("decode: FS segment override used\n");
#else
					bx_cpu.segment_override = &bx_cpu.fs;
#endif
					continue;
					break;
				case 0x65:
#if BX_CPU <= 2
					bx_panic("decode: GS segment override used\n");
#else
					bx_cpu.segment_override = &bx_cpu.gs;
#endif
					continue;
					break;
				case 0x66:
#if BX_CPU < 3
					bx_panic("decode: 32bit_opsize prefix not supported on < 386\n");
					bx_exception(6, 0, 0);
#else // 386+ *
					bx_cpu.is_32bit_opsize   = !bx_cpu.cs.cache.u.segment.d_b;
#endif
					continue;
					break;
				case 0x67:
#if BX_CPU < 3
					bx_panic("decode: 32bit_addrsize prefix not supported < 386!\n");
					bx_exception(6, 0, 0);
#else // 386+ *
					bx_cpu.is_32bit_addrsize = !bx_cpu.cs.cache.u.segment.d_b;
#endif
					continue;
					break;
				case 0x68: bx_PUSH_Iv(); break;
				case 0x69: bx_IMUL_GvEvIv(); break;
				case 0x6A: bx_PUSH_Ib(); break;
				case 0x6B: bx_IMUL_GvEvIb(); break;
				case 0x6C:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_INSB_YbDX();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_INSB_YbDX();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_INSB_YbDX();
					break;


				case 0x6D:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_INSW_YvDX();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_INSW_YvDX();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_INSW_YvDX();
					break;

				case 0x6E:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_OUTSB_DXXb();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_OUTSB_DXXb();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_OUTSB_DXXb();
					break;


				case 0x6F:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_OUTSW_DXXv();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_OUTSW_DXXv();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_OUTSW_DXXv();
					break;


				case 0x70: bx_JO_Jb(); break;
				case 0x71: bx_JNO_Jb(); break;
				case 0x72: bx_JB_Jb(); break;
				case 0x73: bx_JNB_Jb(); break;
				case 0x74: bx_JZ_Jb(); break;
				case 0x75: bx_JNZ_Jb(); break;
				case 0x76: bx_JBE_Jb(); break;
				case 0x77: bx_JNBE_Jb(); break;
				case 0x78: bx_JS_Jb(); break;
				case 0x79: bx_JNS_Jb(); break;
				case 0x7A: bx_JP_Jb(); break;
				case 0x7B: bx_JNP_Jb(); break;
				case 0x7C: bx_JL_Jb(); break;
				case 0x7D: bx_JNL_Jb(); break;
				case 0x7E: bx_JLE_Jb(); break;
				case 0x7F: bx_JNLE_Jb(); break;

				case 0x80: // Immdediate Group 1 EbIb *
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ADD_EbIb(); break;
						case 0x01: bx_OR_EbIb();  break;
						case 0x02: bx_ADC_EbIb(); break;
						case 0x03: bx_SBB_EbIb(); break;
						case 0x04: bx_AND_EbIb(); break;
						case 0x05: bx_SUB_EbIb(); break;
						case 0x06: bx_XOR_EbIb(); break;
						case 0x07: bx_CMP_EbIb(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0x81: // Immdediate Group 1 EvIv *
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ADD_EvIv(); break;
						case 0x01: bx_OR_EvIv(); break;
						case 0x02: bx_ADC_EvIv(); break;
						case 0x03: bx_SBB_EvIv(); break;
						case 0x04: bx_AND_EvIv(); break;
						case 0x05: bx_SUB_EvIv(); break;
						case 0x06: bx_XOR_EvIv(); break;
						case 0x07: bx_CMP_EvIv(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0x82: bx_panic("decode: 82:\n"); break;

				case 0x83: // Immdediate Group 1 EvIb *
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ADD_EvIb(); break;
						case 0x01: bx_OR_EvIb(); break;
						case 0x02: bx_ADC_EvIb(); break;
						case 0x03: bx_SBB_EvIb(); break;
						case 0x04: bx_AND_EvIb(); break;
						case 0x05: bx_SUB_EvIb(); break;
						case 0x06: bx_XOR_EvIb(); break;
						case 0x07: bx_CMP_EvIb(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0x84: bx_TEST_EbGb(); break;
				case 0x85: bx_TEST_EvGv(); break;
				case 0x86: bx_XCHG_EbGb(); break;
				case 0x87: bx_XCHG_EvGv(); break;
				case 0x88: bx_MOV_EbGb(); break;
				case 0x89: bx_MOV_EvGv(); break;
				case 0x8A: bx_MOV_GbEb(); break;
				case 0x8B: bx_MOV_GvEv(); break;
				case 0x8C: bx_MOV_EwSw(); break;
				case 0x8D: bx_LEA_GvM(); break;
				case 0x8E: bx_MOV_SwEw(); break;
				case 0x8F: bx_POP_Ev(); break;


				case 0x90: // NOP * break;
				case 0x91: bx_XCHG_eCXeAX(); break;
				case 0x92: bx_XCHG_eDXeAX(); break;
				case 0x93: bx_XCHG_eBXeAX(); break;
				case 0x94: bx_XCHG_eSPeAX(); break;
				case 0x95: bx_XCHG_eBPeAX(); break;
				case 0x96: bx_XCHG_eSIeAX(); break;
				case 0x97: bx_XCHG_eDIeAX(); break;
				case 0x98: bx_CBW(); break;
				case 0x99: bx_CWD(); break;
				case 0x9A: bx_CALL_Ap(); break;
				case 0x9B: bx_WAIT(); break;
				case 0x9C: bx_PUSHF_Fv(); break;
				case 0x9D: bx_POPF_Fv(); break;
				case 0x9E: bx_SAHF(); break;
				case 0x9F: bx_LAHF(); break;


				case 0xA0: bx_MOV_ALOb(); break;
				case 0xA1: bx_MOV_eAXOv(); break;
				case 0xA2: bx_MOV_ObAL(); break;
				case 0xA3: bx_MOV_OveAX(); break;
				case 0xA4:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_MOVSB_XbYb();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif

#ifdef AUDIT_ENABLED
							AUDIT_STRING(A20ADDR(bx_cpu.ds.cache.u.segment.base+SI),
											 A20ADDR(bx_cpu.es.cache.u.segment.base+DI),
											 CX);
#endif

						while(CX != 0)
						{
							bx_MOVSB_XbYb();
#if BX_CPU >= 2
							if(bx_cpu.errno) break;
#endif
							CX = CX - 1;
						}

						break;
					}

					// no repeat prefix used *
					bx_MOVSB_XbYb();
					break;

				case 0xA5:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_MOVSW_XvYv();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_MOVSW_XvYv();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
               bx_MOVSW_XvYv();
					break;

				case 0xA6:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_CMPSB_XbYb();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}
						else
#endif
							while(CX != 0)
							{
								bx_CMPSB_XbYb();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}

						break;
					}

					// no repeat prefix used *
					bx_CMPSB_XbYb();
					break;

				case 0xA7:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_CMPSW_XvYv();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}
						else
#endif
							while(CX != 0)
							{
								bx_CMPSW_XvYv();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}

						break;
					}

					// no repeat prefix used *
					bx_CMPSW_XvYv();
					break;

				case 0xA8: bx_TEST_ALIb(); break;
				case 0xA9: bx_TEST_eAXIv(); break;
				case 0xAA:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_STOSB_YbAL();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_STOSB_YbAL();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_STOSB_YbAL();
					break;

				case 0xAB:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_STOSW_YveAX();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_STOSW_YveAX();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_STOSW_YveAX();
					break;

				case 0xAC:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_LODSB_ALXb();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_LODSB_ALXb();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_LODSB_ALXb();
					break;

				case 0xAD:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_LODSW_eAXXv();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
							}
						else
#endif
							while(CX != 0)
							{
								bx_LODSW_eAXXv();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
							}

						break;
					}

					// no repeat prefix used *
					bx_LODSW_eAXXv();
					break;

				case 0xAE:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_SCASB_ALXb();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}
						else
#endif
							while(CX != 0)
							{
								bx_SCASB_ALXb();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}

						break;
					}

					// no repeat prefix used *
					bx_SCASB_ALXb();
					break;

				case 0xAF:
					if(bx_rep_prefix)
					{
#if BX_CPU > 2
						if(bx_cpu.is_32bit_addrsize)
							while(ECX != 0)
							{
								bx_SCASW_eAXXv();
								if(bx_cpu.errno) break;
								ECX = ECX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}
						else
#endif
							while(CX != 0)
							{
								bx_SCASW_eAXXv();
#if BX_CPU >= 2
								if(bx_cpu.errno) break;
#endif
								CX = CX - 1;
								if(bx_rep_prefix==BX_REPE_PREFIX && bx_get_ZF()==0) break;
								if(bx_rep_prefix==BX_REPNE_PREFIX && bx_get_ZF()!=0) break;
							}

						break;
					}

					// no repeat prefix used *
					bx_SCASW_eAXXv();
					break;


				case 0xB0: bx_MOV_ALIb(); break;
				case 0xB1: bx_MOV_CLIb(); break;
				case 0xB2: bx_MOV_DLIb(); break;
				case 0xB3: bx_MOV_BLIb(); break;
				case 0xB4: bx_MOV_AHIb(); break;
				case 0xB5: bx_MOV_CHIb(); break;
				case 0xB6: bx_MOV_DHIb(); break;
				case 0xB7: bx_MOV_BHIb(); break;
				case 0xB8: bx_MOV_eAXIv(); break;
				case 0xB9: bx_MOV_eCXIv(); break;
				case 0xBA: bx_MOV_eDXIv(); break;
				case 0xBB: bx_MOV_eBXIv(); break;
				case 0xBC: bx_MOV_eSPIv(); break;
				case 0xBD: bx_MOV_eBPIv(); break;
				case 0xBE: bx_MOV_eSIIv(); break;
				case 0xBF: bx_MOV_eDIIv(); break;

				case 0xC0: // Group 2 Eb,Ib *
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ROL_EbIb(); break;
						case 0x01: bx_ROR_EbIb(); break;
						case 0x02: bx_RCL_EbIb(); break;
						case 0x03: bx_RCR_EbIb(); break;
						case 0x04: bx_SHL_EbIb(); break;
						case 0x05: bx_SHR_EbIb(); break;
						case 0x06: bx_panic("decode: Grp2: (SHL_EbIb duplicate)\n"); break;
						case 0x07: bx_SAR_EbIb(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0xC1: // Group 2 Ev,Ib * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ROL_EvIb(); break;
						case 0x01: bx_ROR_EvIb(); break;
						case 0x02: bx_RCL_EvIb(); break;
						case 0x03: bx_RCR_EvIb(); break;
						case 0x04: bx_SHL_EvIb(); break;
						case 0x05: bx_SHR_EvIb(); break;
						case 0x06: bx_panic("decode: Grp2: (SHL_EvIb duplicate)\n"); break;
						case 0x07: bx_SAR_EvIb(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0xC2: bx_RETnear_Iw(); break;
				case 0xC3: bx_RETnear(); break;
				case 0xC4: bx_LES_GvMp(); break;
				case 0xC5: bx_LDS_GvMp(); break;
				case 0xC6: bx_MOV_EbIb(); break;
				case 0xC7: bx_MOV_EvIv(); break;
				case 0xC8: bx_ENTER_IwIb(); break;
				case 0xC9: bx_LEAVE(); break;
				case 0xCA: bx_RETfar_Iw(); break;
				case 0xCB: bx_RETfar(); break;
				case 0xCC: bx_INT3(); break;
				case 0xCD: bx_INT_Ib(); break;
				case 0xCE: bx_INTO(); break;
				case 0xCF: bx_IRET(); break;


				case 0xD0: // Group 2 Eb,1 * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ROL_Eb1(); break;
						case 0x01: bx_ROR_Eb1(); break;
						case 0x02: bx_RCL_Eb1(); break;
						case 0x03: bx_RCR_Eb1(); break;
						case 0x04: bx_SHL_Eb1(); break;
						case 0x05: bx_SHR_Eb1(); break;
						case 0x06: bx_panic("decode: Grp2:(SHL_Eb1 duplicate)\n"); break;
						case 0x07: bx_SAR_Eb1(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0xD1: // group2 Ev,1 * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ROL_Ev1(); break;
						case 0x01: bx_ROR_Ev1(); break;
						case 0x02: bx_RCL_Ev1(); break;
						case 0x03: bx_RCR_Ev1(); break;
						case 0x04: bx_SHL_Ev1(); break;
						case 0x05: bx_SHR_Ev1(); break;
						case 0x06: bx_panic("decode: Grp2:(SHL_Ev1 duplicate)\n"); break;
						case 0x07: bx_SAR_Ev1(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0xD2: // group2 Eb,CL * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ROL_EbCL(); break;
						case 0x01: bx_ROR_EbCL(); break;
						case 0x02: bx_RCL_EbCL(); break;
						case 0x03: bx_RCR_EbCL(); break;
						case 0x04: bx_SHL_EbCL(); break;
						case 0x05: bx_SHR_EbCL(); break;
						case 0x06: bx_panic("decode: Grp2:\n"); break;
						case 0x07: bx_SAR_EbCL(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0xD3: // group2 Ev,CL * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_ROL_EvCL(); break;
						case 0x01: bx_ROR_EvCL(); break;
						case 0x02: bx_RCL_EvCL(); break;
						case 0x03: bx_RCR_EvCL(); break;
						case 0x04: bx_SHL_EvCL(); break;
						case 0x05: bx_SHR_EvCL(); break;
						case 0x06: bx_panic("decode: Grp2:\n"); break;
						case 0x07: bx_SAR_EvCL(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0xD4: bx_AAM(); break;
				case 0xD5: bx_AAD(); break;
				case 0xD6: bx_panic("decode: D6:(SETALC) Intel reserved opcode\n");
					break;
				case 0xD7: bx_XLAT(); break;

				case 0xD8: // ESC0 * /
					bx_panic("ESC0:\n");
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FADD_Es(); break;
							case 0x01: bx_FMUL_Es(); break;
							case 0x02: bx_FCOM_Es(); break;
							case 0x03: bx_FCOMP_Es(); break;
							case 0x04: bx_FSUB_Es(); break;
							case 0x05: bx_FSUBR_Es(); break;
							case 0x06: bx_FDIV_Es(); break;
							case 0x07: bx_FDIVR_Es(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						// used bx_peek_next_byte().  do I need to read forward another byte? * /
						switch(opcode)
						{
							case 0x00: bx_FADD_ST_STi(); break;
							case 0x01: bx_FMUL_ST_STi(); break;
							case 0x02: bx_FCOM_ST_STi(); break;
							case 0x03: bx_FCOMP_ST_STi(); break;
							case 0x04: bx_FSUB_ST_STi(); break;
							case 0x05: bx_FSUBR_ST_STi(); break;
							case 0x06: bx_FDIV_ST_STi(); break;
							case 0x07: bx_FDIVR_ST_STi(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					break;

				case 0xD9: // ESC1 * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					bx_printf("decode: ESC1 sequence found, mod = %d, opcode = %d\n",
								 (int) mod, (int) opcode);
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FLD_Es(); break;
							case 0x01: bx_panic("decode: ESC1:\n"); break;
							case 0x02: bx_FST_Es(); break;
							case 0x03: bx_FSTP_Es(); break;
							case 0x04: bx_FLDENV_Ea(); break;
							case 0x05: bx_FLDCW_Ew(); break;
							case 0x06: bx_FSTENV_Ea(); break;
							case 0x07: bx_FSTCW_Ew(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						// used bx_peek_next_byte().  do I need to read forward another byte? * /
						switch(opcode)
						{
							case 0x00:
								bx_FLD_STi(); break;
							case 0x01:
								bx_FXCH_STi(); break;
							case 0x02:
								if(rm == 0)
								{
									bx_FNOP(); break;
								}
								else
								{
									bx_panic("decode: ESC9:\n"); break;
								}
							case 0x03:
								bx_FSTP_STi(); break;

							case 0x04:
								switch(rm)
								{
									case 0x00: bx_FCHS(); break;
									case 0x01: bx_FABS(); break;
									case 0x02:
									case 0x03: bx_panic("decode: ESC9:\n"); break;
									case 0x04: bx_FTST(); break;
									case 0x05: bx_FXAM(); break;
									case 0x06:
									case 0x07: bx_panic("decode: ESC9:\n"); break;
								}
								break;
							case 0x05:
								switch(rm)
								{
									case 0x00: bx_FLD1(); break;
									case 0x01: bx_FLDL2T(); break;
									case 0x02: bx_FLDL2E(); break;
									case 0x03: bx_FLDPI(); break;
									case 0x04: bx_FLDLG2(); break;
									case 0x05: bx_FLDLN2(); break;
									case 0x06: bx_FLDZ(); break;
									case 0x07: bx_panic("decode: ESC9:\n"); break;
								}
								break;
							case 0x06:
								switch(rm)
								{
									case 0x00: bx_F2XM1(); break;
									case 0x01: bx_FYL2X(); break;
									case 0x02: bx_FPTAN(); break;
									case 0x03: bx_FPATAN(); break;
									case 0x04: bx_FXTRACT(); break;
									case 0x05: bx_FPREM1(); break;
									case 0x06: bx_FDECSTP(); break;
									case 0x07: bx_FINCSTP(); break;
								}
								break;
							case 0x07:
								switch(rm)
								{
									case 0x00: bx_FPREM(); break;
									case 0x01: bx_FYL2XP1(); break;
									case 0x02: bx_FSQRT(); break;
									case 0x03: bx_FSINCOS(); break;
									case 0x04: bx_FRNDINT(); break;
									case 0x05: bx_FSCALE(); break;
									case 0x06: bx_FSIN(); break;
									case 0x07: bx_FCOS(); break;
								}
								break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					break;

				case 0xDA: // ESC2 * /
					bx_panic("ESC2:\n");
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FIADD_Ed(); break;
							case 0x01: bx_FIMUL_Ed(); break;
							case 0x02: bx_FICOM_Ed(); break;
							case 0x03: bx_FICOMP_Ed(); break;
							case 0x04: bx_FISUB_Ed(); break;
							case 0x05: bx_FISUBR_Ed(); break;
							case 0x06: bx_FIDIV_Ed(); break;
							case 0x07: bx_FIDIVR_Ed(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						// used bx_peek_next_byte().  do I need to read forward another byte? * /
						switch(opcode)
						{
							case 0x05:
								if(rm == 1)
								{
									bx_FUCOMPP(); break;
								}
								else
								{
									bx_panic("decode: ESC2:\n"); break;
								}
							default: bx_panic("decode: ESC2:\n"); break;
						}
						break;
					}
					break;

				case 0xDB: // ESC3 * /
					bx_printf("ESC3:\n");
					if(bx_cpu.cr0.em)
					{
						bx_exception(BX_NM_EXCEPTION, 0, 0);
						break;
					}
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
#if 0
					bx_panic("decode: ESC3 sequence found, mod = %d, opcode = %d\n",
								(int) mod, (int) opcode);
#endif
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FILD_Ed(); break;
							case 0x01: bx_panic("decode: ESC3:\n"); break;
							case 0x02: bx_FIST_Ed(); break;
							case 0x03: bx_FISTP_Ed(); break;
							case 0x04: bx_panic("decode: ESC3:\n"); break;
							case 0x05: bx_FLD_Et(); break;
							case 0x06: bx_panic("decode: ESC3:\n"); break;
							case 0x07: bx_FSTP_Et(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						(void) bx_fetch_next_byte();
#if BX_CPU >= 2
						if(bx_cpu.errno) break;	// exception occurred * /
#endif
						switch(mod_rm_byte)
						{
							case 0xe0: bx_FENI(); break;
							case 0xe1: bx_FDISI(); break;
							case 0xe2: bx_FCLEX(); break;
							case 0xe3: bx_FINIT(); break;
							case 0xe4: bx_FSETPM(); break;
							default:
								bx_panic("decode: ESC3: reserved opcode\n"); break;
						}
						break;
					}
					break;

				case 0xDC: // ESC4 * /
					bx_panic("ESC4:\n");
					mod_rm_byte = bx_peek_next_byte();
					// used bx_peek_next_byte().  do I need to read forward another byte? * /
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FADD_El(); break;
							case 0x01: bx_FMUL_El(); break;
							case 0x02: bx_FCOM_El(); break;
							case 0x03: bx_FCOMP_El(); break;
							case 0x04: bx_FSUB_El(); break;
							case 0x05: bx_FSUBR_El(); break;
							case 0x06: bx_FDIV_El(); break;
							case 0x07: bx_FDIVR_El(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						// used bx_peek_next_byte().  do I need to read forward another byte? * /
						switch(opcode)
						{
							case 0x00: bx_FADD_STi_ST(); break;
							case 0x01: bx_FMUL_STi_ST(); break;
							case 0x02: bx_FCOM_STi_ST(); break;
							case 0x03: bx_FCOMP_STi_ST(); break;
							case 0x04: bx_FSUBR_STi_ST(); break;
							case 0x05: bx_FSUB_STi_ST(); break;
							case 0x06: bx_FDIVR_STi_ST(); break;
							case 0x07: bx_FDIV_STi_ST(); break;
							default: bx_panic("decode: ESC4:\n"); break;
						}
						break;
					}
					break;


				case 0xDD: // ESC5 * /
					bx_printf("ESC5:\n");
					if(bx_cpu.cr0.em)
					{
						bx_exception(BX_NM_EXCEPTION, 0, 0);
						break;
					}
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FLD_El(); break;
							case 0x01: bx_panic("decode: ESC5:\n"); break;
							case 0x02: bx_FST_El(); break;
							case 0x03: bx_FSTP_El(); break;
							case 0x04: bx_FRSTOR_Ea(); break;
							case 0x05: bx_panic("decode: ESC5:\n"); break;
							case 0x06: bx_FSAVE_Ea(); break;
							case 0x07: bx_FSTSW_Ew(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						bx_panic("ESC5: recode\n");
						// used bx_peek_next_byte().  do I need to read forward another byte? * /
						switch(opcode)
						{
							case 0x00: bx_FFREE_STi(); break;
							case 0x01: bx_FXCH_STi(); break;
							case 0x02: bx_FST_STi(); break;
							case 0x03: bx_FSTP_STi(); break;
							case 0x04: bx_FUCOM_STi_ST(); break;
							case 0x05: bx_FUCOMP_STi(); break;
							case 0x06: bx_panic("decode: ESC5:\n"); break;
							case 0x07: bx_panic("decode: ESC5:\n"); break;
							default: bx_panic("decode: ESC5:\n"); break;
						}
						break;
					}
					break;

				case 0xDE: // ESC6 * /
					bx_panic("ESC6:\n");
					mod_rm_byte = bx_peek_next_byte();
					// used bx_peek_next_byte().  do I need to read forward another byte? * /
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FIADD_Ew(); break;
							case 0x01: bx_FIMUL_Ew(); break;
							case 0x02: bx_FICOM_Ew(); break;
							case 0x03: bx_FICOMP_Ew(); break;
							case 0x04: bx_FISUB_Ew(); break;
							case 0x05: bx_FISUBR_Ew(); break;
							case 0x06: bx_FIDIV_Ew(); break;
							case 0x07: bx_FIDIVR_Ew(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						// used bx_peek_next_byte().  do I need to read forward another byte? * /
						switch(opcode)
						{
							case 0x00: bx_FADDP_STi_ST(); break;
							case 0x01: bx_FMULP_STi_ST(); break;
							case 0x02: bx_FCOMP_STi(); break;
							case 0x03:
								switch(rm)
								{
									case 0x01: bx_FCOMPP(); break;
									default: bx_panic("decode: ESC6:\n"); break;
								}
								break;
							case 0x04: bx_FSUBRP_STi_ST(); break;
							case 0x05: bx_FSUBP_STi_ST(); break;
							case 0x06: bx_FDIVRP_STi_ST(); break;
							case 0x07: bx_FDIVP_STi_ST(); break;
							default: bx_panic("decode: ESC6:\n"); break;
						}
						break;
					}
					break;

				case 0xDF: // ESC7 * /
					bx_panic("ESC7:\n");
					mod_rm_byte = bx_peek_next_byte();
					// used bx_peek_next_byte().  do I need to read forward another byte? * /
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					if(mod != 3)
					{
						switch(opcode)
						{
							case 0x00: bx_FILD_Ew(); break;
							case 0x01: bx_panic("decode: ESC7:\n"); break;
							case 0x02: bx_FIST_Ew(); break;
							case 0x03: bx_FISTP_Ew(); break;
							case 0x04: bx_FBLD_Eb(); break;
							case 0x05: bx_FILD_Eq(); break;
							case 0x06: bx_FBSTP_Eb(); break;
							case 0x07: bx_FISTP_Eq(); break;
							default: bx_panic("decode: error in bx_decode()!\n"); break;
						}
						break;
					}
					else
					{ // mod == 3 * /
						// used bx_peek_next_byte().  do I need to read forward another byte? * /
						switch(opcode)
						{
							case 0x00: bx_FFREE_STi(); break;
							case 0x01: bx_FXCH_STi(); break;
							case 0x02: bx_FST_STi(); break;
							case 0x03: bx_FSTP_STi(); break;
							case 0x04:
								switch(rm)
								{
									case 0x01: bx_FSTSW_AX(); break;
									default: bx_panic("decode: ESC7:\n"); break;
								}
								break;
							default: bx_panic("decode: ESC7:\n"); break;
						}
						break;
					}
					break;


				case 0xE0: bx_LOOPNE_Jb(); break;
				case 0xE1: bx_LOOPE_Jb(); break;
				case 0xE2: bx_LOOP_Jb(); break;
				case 0xE3: bx_JCXZ_Jb(); break;
				case 0xE4: bx_IN_ALIb(); break;
				case 0xE5: bx_IN_eAXIb(); break;
				case 0xE6: bx_OUT_IbAL(); break;
				case 0xE7: bx_OUT_IbeAX(); break;
				case 0xE8: bx_CALL_Av(); break;
				case 0xE9: bx_JMP_Jv(); break;
				case 0xEA: bx_JMP_Ap(); break;
				case 0xEB: bx_JMP_Jb(); break;
				case 0xEC: bx_IN_ALDX(); break;
				case 0xED: bx_IN_eAXDX(); break;
				case 0xEE: bx_OUT_DXAL(); break;
				case 0xEF: bx_OUT_DXeAX(); break;

				case 0xF0: // LOCK * /
#if 1 // riad * /
					bx_printf("decode: LOCK prefix encountered\n");
#endif
					bx_panic("decode: LOCK prefix encountered\n");
					if(bx_protected_mode() && CPL>IOPL)
					{
						bx_panic("decode: LOCK prefix found: CPL>IOPL\n");
						// #GP(0) * /
						break;
					}
					continue;
					break;
				case 0xF1: bx_panic("decode: F1: Intel reserved opcode\n");
					break;
				case 0xF2: // REPNE/REPNZ * /
					bx_rep_prefix = BX_REPNE_PREFIX;
					continue;
					break;
				case 0xF3: // REP/REPE/REPZ * /
					bx_rep_prefix = BX_REPE_PREFIX;
					continue;
					break;
				case 0xF4: bx_HLT(); break;
				case 0xF5: bx_CMC(); break;
				case 0xF6: // Group 3 Eb * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_TEST_EbIb(); break;
						case 0x01: bx_panic("decode: F6:TEST_EbIb duplicate\n"); break;
						case 0x02: bx_NOT_Eb(); break;
						case 0x03: bx_NEG_Eb(); break;
						case 0x04: bx_MUL_ALEb(); break;
						case 0x05: bx_IMUL_ALEb(); break;
						case 0x06: bx_DIV_ALEb(); break;
						case 0x07: bx_IDIV_ALEb(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;

				case 0xF7: // GROUP3 Ev * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_TEST_EvIv(); break;
						case 0x01: bx_panic("decode: F7 01:TEST_EvIv duplicate\n"); break;
						case 0x02: bx_NOT_Ev(); break;
						case 0x03: bx_NEG_Ev(); break;
						case 0x04: bx_MUL_eAXEv(); break;
						case 0x05: bx_IMUL_eAXEv(); break;
						case 0x06: bx_DIV_eAXEv(); break;
						case 0x07: bx_IDIV_eAXEv(); break;
						default: bx_panic("decode: error in bx_decode()!\n"); break;
					}
					break;
				case 0xF8: bx_CLC(); break;
				case 0xF9: bx_STC(); break;
				case 0xFA: bx_CLI(); break;
				case 0xFB: bx_STI(); break;
				case 0xFC: bx_CLD(); break;
				case 0xFD: bx_STD(); break;
				case 0xFE: // GROUP4 * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_INC_Eb(); break;
						case 0x01: bx_DEC_Eb(); break;
						default: bx_panic("decode: FE default:\n"); break;
					}
					break;

				case 0xFF: // GROUP 5 * /
					mod_rm_byte = bx_peek_next_byte();
					BX_DECODE_MODRM(mod_rm_byte, mod, opcode, rm);
					switch(opcode)
					{
						case 0x00: bx_INC_Ev(); break;
						case 0x01: bx_DEC_Ev(); break;
						case 0x02: bx_CALL_Ev(); break;
						case 0x03: bx_CALL_Ep(); break;
						case 0x04: bx_JMP_Ev(); break;
						case 0x05: bx_JMP_Ep(); break;
						case 0x06: bx_PUSH_Ev(); break;
						default: bx_panic("decode: FF default:\n"); break;
					}
					break;

				default:	// only invalid instructions left * /
					bx_panic("decode: error in bx_decode()!\n");
					break;
			} // switch (next_byte) * /
			break;
		} // for (byte_count... */
