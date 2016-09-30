/*
	watch_ins.c

  This file contains "watch versions" of instructions, that is, versions that report something to the expert system.
  They carry out the tasks of the normal instructions, and additionnally they submit audit records to Asax.

  It also contains the interrupt stack processing functions.

*/


#include "watcher.h"
#include "watch_ins_tbl.h"
#include "..\..\Bochs-96\instr_proto.h"

// Interrupt 3

void INT3_watch(void)
{
   /* INT 3 is not IOPL sensitive */
   report_tag = TAG_INT3;
   nadf_record[41].object = NULL;
   nadf_record[42].object = NULL;
   nadf_record[43].object = NULL;
   nadf_record[44].object = NULL;
   
   submit_record(nadf_record);
   if(cancel_instruction)
      return;
   else
      bx_interrupt(3, 1, 0);
}

void fillRequest(RequestDescription *r1)
{
	r1->reqAX = AX_;
	r1->reqBX = BX_;
	r1->reqCX = CX_;
	r1->reqDX = DX_;
	r1->reqSI = SI_;
	r1->reqDI = DI_;
	r1->reqBP = BP_;
	r1->reqDS = bx_cpu.ds.selector.value;
	r1->reqES = bx_cpu.es.selector.value;
}

int sameRequest(RequestDescription *r1, RequestDescription *r2)
{
	return r1->reqAX == r2->reqAX
		&& r1->reqBX == r2->reqBX
		&& r1->reqCX == r2->reqCX
		&& r1->reqDX == r2->reqDX
		&& r1->reqSI == r2->reqSI
		&& r1->reqDI == r2->reqDI
		&& r1->reqBP == r2->reqBP
		&& r1->reqDS == r2->reqDS
		&& r1->reqES == r2->reqES;
}

int lookupIntStack(Bit8u int_no, DWORD return_address)
{
	int i;

	for (i=int_audit_stack_top-1 ; i >=0 ; i--)
		if (int_audit_stack[i].int_no == int_no && int_audit_stack[i].return_address == return_address)
			return i;
	return -1;
}

int lookupIntStack_by_int_no(Bit8u int_no)
{
	int i;

	for (i=int_audit_stack_top-1 ; i >=0 ; i--)
		if (int_audit_stack[i].int_no == int_no)
			return i;
	return -1;
}

int lookupIntStack_by_return_address(DWORD return_address)
{
	int i;

	for (i=int_audit_stack_top-1 ; i >=0 ; i--)
		if (int_audit_stack[i].return_address == return_address)
			return i;
	return -1;
}

// The following procedure is used for interrupt auditing
// It may be called because of an INT instruction, or because of a breakpoint on an interrupt entry point
void pushOnIntStack(Bit8u int_no, WORD tag, DWORD return_address)
{
	static int resetcounter=0; //DEBUG
	int i;
	DWORD notify_type=0;
	BOOL must_report=0;	// This boolean is set if we have to report something to the expert system
						// There may be something to report before or after the interrupt, or both

#if 0
	for (i=0 ; i<w_e_top ; i++)
		if ( watched_event[i].int_no == int_no &&
			(watched_event[i].func   == -1 || watched_event[i].func == AH_) &&
			(watched_event[i].sub    == -1 || watched_event[i].sub  == AL_))
		{
			must_report = 1;
			notify_type |= watched_event[i].notify_type;
		}
#endif

	if (notify[int_no]&WATCHALLFUNCS) {
		must_report |= 1;
		notify_type |= notify[int_no]&(NOTIFY_BEFORE|NOTIFY_AFTER);
	}
	if (notify[int_no]&WATCHSOMEFUNCS){
		if (w_e_lookup(int_no, AH_, -1, &i)) {
			// This function is being watched
			must_report |= 1;
			notify_type |= watched_event[i].notify_type;
		}
	}

	if (must_report && (notify_type&NOTIFY_BEFORE)) {
		// We watch this event and we must report the interrupt before its actual execution
		report_tag = tag;
	    nadf_record[41].object = &int_no;
		nadf_record[41].len = 1;
	    nadf_record[42].object = NULL;
		nadf_record[43].object = NULL;
	    nadf_record[44].object = NULL;
	    submit_record(nadf_record);
		if(cancel_instruction) return;
	} // end if must NOTIFY_BEFORE

	if (must_report && (notify_type&NOTIFY_AFTER)) {
		// Check there is some place left on the stack
		if (int_audit_stack_top >= INT_AUDIT_STACK_SIZE) {
			// Stack is full, can't push any new record
			// In this case, we should discard the oldest interrupts, for now we just discard everything (empty the stack)
			int_audit_stack_top = 0;
			// DEBUG
			resetcounter++;
		}
		// We watch this event, record it on the stack

		// Fill the references array with appropriate values
		report_tag = tag;
	    nadf_record[41].object = &int_no;
		nadf_record[41].len = 1;
	    nadf_record[42].object = NULL;
		nadf_record[43].object = NULL;
	    nadf_record[44].object = NULL;
		// Start a record
		start_compile_record(nadf_record,
							&int_audit_stack[int_audit_stack_top].buffer,
							&int_audit_stack[int_audit_stack_top].buffer_count);
		// Event type kept on the stack too:
		int_audit_stack[int_audit_stack_top].int_no = int_no;
		int_audit_stack[int_audit_stack_top].func = AH_;
		int_audit_stack[int_audit_stack_top].sub = AL_;
		int_audit_stack[int_audit_stack_top].return_address = return_address;
		fillRequest(&int_audit_stack[int_audit_stack_top].req);
		int_audit_stack[int_audit_stack_top].tick = instruction_tick;
		int_audit_stack[int_audit_stack_top].tag = tag;
		int_audit_stack_top++;

	} // end if must NOTIFY_AFTER
}

// All interrupts (this one is very important)
void INT_Ib_watch(void)
{
	Bit8u imm8; // interrupt number

	// Fisrt part of the normal processing of the interrupt by Bochs
	imm8 = bx_fetch_next_byte(); // imm8 contains the interrupt number
	BX_HANDLE_EXCEPTION()

	if (bx_v8086_mode() && IOPL<3)
	{
		bx_printf("int_ib: v8086: IOPL<3\n");
		bx_exception(BX_GP_EXCEPTION, 0, 0);
		return;
	}

	// Now, the audit
	// Test to see if we are watching this interrupt in any way
	if (notify [imm8])
		pushOnIntStack(imm8, TAG_INT, (bx_cpu.cs.selector.value << 4) + bx_cpu.eip);

	// 2nd part of the normal processing
	bx_interrupt(imm8, 1, 0);
}

void IRET_watch(void)
{

	// Look for an interrupt in the interrupt stack that would correspond to this return address
	// if found, complete audit record in stack, send it and pop

	DWORD return_address;
	int i;

	bx_IRET(); // Do the IRET
	// Now, we have the return address in the CPU fields
	return_address = bx_cpu.cs.selector.value*16+bx_cpu.eip;
	// Look for something in the interrupt auditing stack that corresponds to this return address

#if 0
	for (i=int_audit_stack_top-1 ; i>=0 ; i--) {
		// Look from the top (from most recent to oldest interrupt)
		if (int_audit_stack[i].return_address == return_address)
			// high probability this is the right interrupt
			break;
	} // end for i=stack_top to stack_bottom
#endif

	i = lookupIntStack_by_return_address(return_address);

	if (i>=0) {
		// We found the right i
		append_compile_record(nadf_record,
							&int_audit_stack[i].buffer,
							&int_audit_stack[i].buffer_count);
		complete_compile_record(
							&int_audit_stack[i].buffer,
							&int_audit_stack[i].buffer_count);
		if(asax((char *) int_audit_stack[i].buffer))
			done(); /* no more rules triggered, let's complete asax */
		// Pop all ints from top to i
		int_audit_stack_top = i;
	}
}

// Same as IRET
void RETfar_watch(void)
{
	DWORD return_address;
	int i;

	bx_RETfar();
	// Now, we have the return address in the CPU fields
	return_address = bx_cpu.cs.selector.value*16+bx_cpu.eip;
	// Look for something in the interrupt auditing stack that corresponds to this return address
	i = lookupIntStack_by_return_address(return_address);
	if (i>=0) {
		append_compile_record(nadf_record,
							&int_audit_stack[i].buffer,
							&int_audit_stack[i].buffer_count);
		complete_compile_record(
							&int_audit_stack[i].buffer,
							&int_audit_stack[i].buffer_count);
		if(asax((char *) int_audit_stack[i].buffer)) done();
		// Pop all ints from top to i
		int_audit_stack_top = i;
	}
}

// Same as IRET
void RETfar_Iw_watch(void)
{
	DWORD return_address;
	int i;

	bx_RETfar_Iw();
	// Now, we have the return address in the CPU fields
	return_address = bx_cpu.cs.selector.value*16+bx_cpu.eip;
	// Look for something in the interrupt auditing stack that corresponds to this return address
	i = lookupIntStack_by_return_address(return_address);
	if (i>=0) {
		append_compile_record(nadf_record,
							&int_audit_stack[i].buffer,
							&int_audit_stack[i].buffer_count);
		complete_compile_record(
							&int_audit_stack[i].buffer,
							&int_audit_stack[i].buffer_count);
		if(asax((char *) int_audit_stack[i].buffer)) done();
		// Pop all ints from top to i
		int_audit_stack_top = i;
	}
}

void CALL_Ep_watch(void)
{
	// Spied version of the CALL FAR [...]
	// (I thought I would need that to trace the calls to INT21 EP, but apparently a breakpoint is enough...)
	// Implementation should be the same as in CALL_Ap_watch

	bx_CALL_Ep();
}

void CALL_Ap_watch(void)
{
	// Spied version of the CALL FAR xxxx:xxxx

	report_tag = CALLFARIMM;
	nadf_record[41].object = NULL;
	nadf_record[41].len = 0;
	nadf_record[42].object = NULL;
	nadf_record[43].object = NULL;
	nadf_record[44].object = NULL;
	start_compile_record(nadf_record,
						&watch_buffer,
						&watch_buffer_count);

	bx_CALL_Ap(); // Actual call

	append_compile_record(nadf_record,
						&watch_buffer,
						&watch_buffer_count);
	complete_compile_record(
						&watch_buffer,
						&watch_buffer_count);
	if(asax((char *) watch_buffer)) done();
}


void movsb_watch(void)
{
	BYTE granularity = 1;

	if (bx_rep_prefix /*&& bx_cpu.ecx > 0x100*/) {
		report_tag = TAG_MOVS;
		nadf_record[41].object = &granularity;
		nadf_record[41].len = 1;
		nadf_record[42].object = NULL;
		nadf_record[43].object = NULL;
		nadf_record[44].object = NULL;
		submit_record(nadf_record);
		if(cancel_instruction) return;
	}
	movsb();
}

void movsw_watch(void)
{
	BYTE granularity = 2; // assuming real mode, else test bx_cpu.is32bit_opsize...

	if (bx_rep_prefix /*&& bx_cpu.ecx > 0x80*/) {
		report_tag = TAG_MOVS;
		nadf_record[41].object = &granularity;
		nadf_record[41].len = 1;
		nadf_record[42].object = NULL;
		nadf_record[43].object = NULL;
		nadf_record[44].object = NULL;
		submit_record(nadf_record);
		if(cancel_instruction) return;
	}
	movsw();
}


void inalib_watch(void)
{
   Bit8u imm8, al;
   imm8 = bx_fetch_next_byte();
   BX_HANDLE_EXCEPTION()

   if(IO_IS_GUARDED(imm8))
   {
      report_tag = IN_AL;
      nadf_record[41].object = &imm8;
      nadf_record[41].len = 1;
      nadf_record[42].object = NULL;
      nadf_record[43].object = NULL;
      nadf_record[44].object = NULL;
      submit_record(nadf_record);
      if(cancel_instruction)
        return;
   }
   al = bx_cpu_inp8(imm8);
   BX_HANDLE_EXCEPTION()
   AL = al;
}

void ineaxib_watch(void)
{
   Bit8u imm8;
   Bit32u eax;
   Bit16u ax;

   imm8 = bx_fetch_next_byte();
   BX_HANDLE_EXCEPTION()

   if (bx_cpu.is_32bit_opsize)
   {  if(IO_IS_GUARDED4(imm8))
      {
         report_tag = IN_EAX;
         nadf_record[41].object = &imm8;
         nadf_record[41].len = 1;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      eax = bx_cpu_inp32(imm8);
      BX_HANDLE_EXCEPTION()
      EAX = eax;
   }
   else
   {
      if(IO_IS_GUARDED4(imm8))
      {
         report_tag = IN_AX;
         nadf_record[41].object = &imm8;
         nadf_record[41].len = 1;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      ax = bx_cpu_inp16(imm8);
      BX_HANDLE_EXCEPTION()
      AX = ax;
   }
}

void inaldx_watch(void)
{
   unsigned short dx;
   Bit8u al;
   dx = DX;

   if(IO_IS_GUARDED(dx))
   {
      report_tag = IN_AL;
      nadf_record[41].object = &dx;
      nadf_record[41].len = 2;
      nadf_record[42].object = NULL;
      nadf_record[43].object = NULL;
      nadf_record[44].object = NULL;
      submit_record(nadf_record);
      if(cancel_instruction)
        return;
   }
   al = bx_cpu_inp8(dx);
   BX_HANDLE_EXCEPTION()
   AL = al;
}

void ineaxdx_watch(void)
{
   Bit16u ax;
   Bit32u eax;
   unsigned short dx;
   dx = DX;

   if (bx_cpu.is_32bit_opsize)
   {
      if(IO_IS_GUARDED4(dx))
      {
         report_tag = IN_EAX;
         nadf_record[41].object = &dx;
         nadf_record[41].len = 2;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      eax = bx_cpu_inp32(dx);
      BX_HANDLE_EXCEPTION()
      EAX = eax;
   }
   else
   {
      if(IO_IS_GUARDED4(dx))
      {
         report_tag = IN_AX;
         nadf_record[41].object = &dx;
         nadf_record[41].len = 2;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      ax = bx_cpu_inp16(dx);
      BX_HANDLE_EXCEPTION()
      AX = ax;
   }
}

void outibal_watch(void)
{
   Bit8u imm8;
   imm8 = bx_fetch_next_byte();
   BX_HANDLE_EXCEPTION()

   if(IO_IS_GUARDED(imm8))
   {
      report_tag = OUT_AL;
      nadf_record[41].object = &imm8;
      nadf_record[41].len = 1;
      nadf_record[42].object = NULL;
      nadf_record[43].object = NULL;
      nadf_record[44].object = NULL;
      submit_record(nadf_record);
      if(cancel_instruction)
        return;
   }
   bx_cpu_outp8(imm8, AL);
   BX_HANDLE_EXCEPTION()
}

void outibeax_watch(void)
{
   Bit8u imm8;
   imm8 = bx_fetch_next_byte();
   BX_HANDLE_EXCEPTION()

   if (bx_cpu.is_32bit_opsize)
   {
      if(IO_IS_GUARDED4(imm8))
      {
         report_tag = OUT_EAX;
         nadf_record[41].object = &imm8;
         nadf_record[41].len = 1;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      bx_cpu_outp32(imm8, EAX);
      BX_HANDLE_EXCEPTION()
   }
   else
   {
      if(IO_IS_GUARDED4(imm8))
      {
         report_tag = OUT_AX;
         nadf_record[41].object = &imm8;
         nadf_record[41].len = 1;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      bx_cpu_outp16(imm8, AX);
      BX_HANDLE_EXCEPTION()
   }
}

void outdxal_watch(void)
{
   unsigned short dx;
   dx = DX;

   if(IO_IS_GUARDED(dx))
   {
      report_tag = OUT_AL;
      nadf_record[41].object = &dx;
      nadf_record[41].len = 2;
      nadf_record[42].object = NULL;
      nadf_record[43].object = NULL;
      nadf_record[44].object = NULL;
      submit_record(nadf_record);
      if(cancel_instruction)
        return;
   }
   bx_cpu_outp8(dx, AL);
   BX_HANDLE_EXCEPTION()
}

void outdxeax_watch(void)
{
   unsigned short dx;
   dx = DX;

   if (bx_cpu.is_32bit_opsize)
   {
      if(IO_IS_GUARDED4(dx))
      {
         report_tag = OUT_EAX;
         nadf_record[41].object = &dx;
         nadf_record[41].len = 2;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      bx_cpu_outp32(dx, EAX);
      BX_HANDLE_EXCEPTION()
   }
   else
   {
      if(IO_IS_GUARDED4(dx))
      {
         report_tag = OUT_AX;
         nadf_record[41].object = &dx;
         nadf_record[41].len = 2;
         nadf_record[42].object = NULL;
         nadf_record[43].object = NULL;
         nadf_record[44].object = NULL;
         submit_record(nadf_record);
         if(cancel_instruction)
            return;
      }
      bx_cpu_outp16(dx, AX);
      BX_HANDLE_EXCEPTION()
   }
}
   
void insb_watch(void)
{
   // TO DO   

}

void outsb_watch(void)
{
   // TO DO

}

void insw_watch(void)
{
   // TO DO   

}

void outsw_watch(void)
{
   // TO DO

}

void cmp_watch(void)
{

}
