/* 

  WATCHER.C - Hooman Vassef, 6/27/97
  last modified: 11/19/97

  11/06/97, added return values from interrupts in audit fields (+ int stack support in bochs), Fred
  11/11/97, use GNU regular expression in asax! Bug appears in match when using STDREGEX
  11/11/97, added register manip funcs in asax (rH,rL,rX...), Fred
  11/12/97, fixed linear_addr bug (high seg nibble was lost), Fred
  11/13/97, added an instruction tick. Time info was needed for the rules, Fred
  11/16/97, added a field for data in memory access audit record (ex:write new vector -> which value), Fred
  11/18/97, added audit breakpoint, Fred
  11/19/97, modified instruction processing/audit to spy group5 instr (table-driven -> do it everywhere), Fred

    Description: an alternative to audit.c,
				 communication between Bochs and Asax

*/


#include "watcher.h"
#include "watch_ins_tbl.h"

BYTE watch_buffer[WATCH_BUFFER_SIZE];
DWORD watch_buffer_count = 0;
DWORD iowatch[IOWATCH_SIZE];
DWORD memwatch[MEMWATCH_SIZE];
BOOL watching_mem = 0;
BOOL irq_watch[0x10] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

struct Memwatch_descr_t memwatch_descr[MEMWATCH_DESCR_SIZE];
unsigned int memwatch_descr_count = 0;
unsigned short reg_markers[0x10] = {
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
DWORD base_eip;

// The report_tag is a flag describing what condition triggered the transmission of the audit record
// Ex: TAG_INT indicating an interrupt, MEM_ACCESS a memory access...
// (This word is referenced by nadf_record)
WORD report_tag;

int instruction_tick=0;	// This is a counter increased at each instruction

// Audit breakpoint vars
int checkAuditBrkPt=0;
BreakPoint brkPtStack[BRKPTSTACKSIZE];
int brkPtStackTop=0;

// The record describing the state to pass to Asax, the actual audit record is derived from this array
Nadf_field nadf_record[MAX_REC_COUNT] = {
   { 0, NULL }, /* nothing, but asax records start at 1 */
   { 2, &bx_cpu.cs.selector.value }, /* 1: CS */
   { 4, &bx_cpu.eip }, /* 2: EIP */
   { 4, &bx_cpu.eax }, /* 3: EAX */
   { 4, &bx_cpu.ebx }, /* 4: EBX */
   { 4, &bx_cpu.ecx }, /* 5: ECX */
   { 4, &bx_cpu.edx }, /* 6: EDX */
   { 4, &bx_cpu.esi }, /* 7: ESI */
   { 4, &bx_cpu.edi }, /* 8: EDI */
   { 4, &bx_cpu.ebp }, /* 9: EBP */
   { 4, &bx_cpu.esp }, /* 10: ESP */
   { 2, &bx_cpu.ds.selector.value }, /* 11: DS */
   { 2, &bx_cpu.es.selector.value }, /* 12: ES */
   { 2, &bx_cpu.ss.selector.value }, /* 13: SS */
   { 2, &bx_cpu.fs.selector.value }, /* 14: FS */
   { 2, &bx_cpu.gs.selector.value }, /* 15: GS */
   { 2, &bx_cpu.eflags.cf }, /* 16: CF */

   // (Do we need all those flags?)
   
   { 2, &bx_cpu.eflags.pf_byte }, /* 17: PF */
   { 2, &bx_cpu.eflags.af }, /* 18: AF */
   { 2, &bx_cpu.eflags.zf }, /* 19: ZF */
   { 2, &bx_cpu.eflags.sf }, /* 20: SF */
   { 2, &bx_cpu.eflags.tf }, /* 21: TF */
   { 2, &bx_cpu.eflags.if_ }, /* 22: IF */
   { 2, &bx_cpu.eflags.df }, /* 23: DF */
   { 2, &bx_cpu.eflags.of }, /* 24: OF */
   { 2, &bx_cpu.eflags.iopl }, /* 25: IOPL */
   { 2, &bx_cpu.eflags.nt }, /* 26: NT */
   { 2, &bx_cpu.eflags.rf }, /* 27: RF */
   { 2, &bx_cpu.eflags.vm }, /* 28: VM */
   { 2, &bx_cpu.eflags.ac }, /* 29: AC */

      // need to add segment descriptor caches
      // maybe change eflags structure

   { 4, &base_eip }, /* 30: eip at the beginning of the instruction */

   { 4, &instruction_tick }, /* 31: the time when the first part of the audit record is compiled */
   { 4, NULL }, /* 32: data to be put in memory on a mem write (only if len <= 4) */

   { 0, NULL }, /* 33: dummy value */
   { 0, NULL }, /* 34: dummy value */
   { 0, NULL }, /* 35: dummy value */
   { 0, NULL }, /* 36: dummy value */
   { 0, NULL }, /* 37: dummy value */
   { 0, NULL }, /* 38: dummy value */
   { 0, NULL }, /* 39: first record: sample file name */

   
   /* variable fields */
   { 2, &report_tag }, /* 40: instruction */
   { 0, NULL }, /* 41: immediate val */
   { 4, NULL }, /* 42: address for a memory watch */
   { 4, NULL }, /* 43: size of the area accessed */
   { 1, NULL }, /* 44: W/r: true if write access */

   // Here starts the second part

   // Here is a new set of registers aimed at describing the processor state after an interrupt
   // (we often need return values from interrupt like handle numbers, flags indicating success/error...)
   { 2, &bx_cpu.cs.selector.value }, /* 45: return CS */
   { 4, &bx_cpu.eip }, /* 46: return EIP */
   { 4, &bx_cpu.eax }, /* 47: return EAX */
   { 4, &bx_cpu.ebx }, /* 48: return EBX */
   { 4, &bx_cpu.ecx }, /* 49: return ECX */
   { 4, &bx_cpu.edx }, /* 50: return EDX */
   { 4, &bx_cpu.esi }, /* 51: return ESI */
   { 4, &bx_cpu.edi }, /* 52: return EDI */
   { 4, &bx_cpu.ebp }, /* 53: return EBP */
   { 4, &bx_cpu.esp }, /* 54: return ESP */
   { 2, &bx_cpu.ds.selector.value }, /* 55: return DS */
   { 2, &bx_cpu.es.selector.value }, /* 56: return ES */
   { 2, &bx_cpu.ss.selector.value }, /* 57: return SS */
   { 2, &bx_cpu.eflags.cf }, /* 58: return CF */
   { 4, &instruction_tick }, /* 59: the time when the second part of the audit record is compiled */
};

Int_audit_stack_elem int_audit_stack[INT_AUDIT_STACK_SIZE];
int int_audit_stack_top=0;

// The stack of all events being watched (interrupts, functions, subfunctions)
Watched_event watched_event[INT_WATCH_SIZE];
int w_e_top = 0;

// An array describing which interrupts are being watched and in which way
DWORD notify[0x100];

int cancel_instruction;
int watcher_initializing;

// Input file for asax -> turn these into cmdline args
char asaxmodule[FILENAMESIZE] = "main";
char samplename[FILENAMESIZE] = "C:\\SAMPLE.EXE";
char asaxdescrfile[FILENAMESIZE] = "vides2.adf";
char virusreport[FILENAMESIZE] = "report.txt";

// Here are the functions that manipulates the array of watched events
// They keep the array sorted (high int # -> high index)


// This is a lookup on the sorted array using dichotomy
// returns 1 on successful lookup, 0 otherwise

// This is used in the CPU loop -> this IS time-critical

/*_inline*/ int w_e_lookup(int int_no, int func, int sub, int *retindex)
{
	int a,b,c;
	a=0;
	b=w_e_top;

	while (b > a) {
		c = (a+b) >> 1;
		if ((watched_event[c].int_no <  int_no) ||
			(watched_event[c].int_no == int_no && watched_event[c].func <  func) ||
			(watched_event[c].int_no == int_no && watched_event[c].func == func && watched_event[c].sub < sub))
			a=(c>a)?c:c+1;
		else
			b=(c<b)?c:c-1;
	}
	// At this point a=b
	*retindex = a;
	if (watched_event[a].int_no == int_no && watched_event[a].func == func && watched_event[a].sub == sub)
		return 1;
	else
		return 0;
}

// This inserts an element (a watched event) in the array, keeping it sorted

// Used only during setup by the expert system rules -> not time-critical

void w_e_insert(int int_no, int func, int sub, DWORD notify_type)
{
	int i;
	if (w_e_top >= INT_WATCH_SIZE)
		bx_panic("watcher.c - watch stack overflow!\n");
	if (w_e_lookup(int_no,func,sub,&i) == 0) {
		// Not found, we insert it
		// Move the upper part of the array one place up
		memmove(&watched_event[i+1], &watched_event[i], (w_e_top-i)*sizeof(Watched_event));
		w_e_top++;
		// Write new element in the hole we just created
		watched_event[i].int_no = int_no;
		watched_event[i].func = func;
		watched_event[i].sub = sub;
		watched_event[i].notify_type = notify_type;
	}
	else {
		// Found, change notify flag
		watched_event[i].notify_type |= notify_type;
	}
}

int watcher_init(void)
{
   int opcode;
   
   watcher_initializing = 1;

   // No interrupts, functions or subfunctions are watched at the beginning
   memset(notify, 0, 0x100*sizeof(DWORD));
   memset(watched_event, 0, INT_WATCH_SIZE*sizeof(Watched_event));

   /* initialize the memwatch and iowatch bit arrays */
   memset(memwatch, '\0', MEMWATCH_SIZE);
   memset(iowatch, '\0', IOWATCH_SIZE);

   /* initialize the instruction tables: all the no_watch's have to become what they are in the normal table */
   for(opcode = 0; opcode < 256; opcode++)
   {
      if(watching_instructions[opcode] == no_watch)
         watching_instructions[opcode] = instructions[opcode];
      if(watching_instructions_escape[opcode] == no_watch)
         watching_instructions_escape[opcode] = instructions_escape[opcode];
   }

   // Initialize the expert system
   if(asax_init(asaxdescrfile, asaxmodule, virusreport))
      return(1); /* Problem while initializing asax */
   else
   {
      nadf_record[39].object = samplename;
      nadf_record[39].len = strlen(samplename);
      submit_record(nadf_record); /* send in the initialization record */
      watcher_initializing = 0;
      return(0); /* initialization successful */
   }
}


int watcher_shutdown(void)
{
   return(asax_complete(watch_buffer));
}


void done(void)
{ /* is called if asax has no more rules triggered */
   /* processes the completion rules, then exits the program */

   if(watcher_initializing) /* Uh-oh */
      PANIC("No initializing rules triggered!\n");
   else
   {
      watcher_shutdown();
      PANIC("All rules processed, normal terminaison\n");
   }
}

void start_compile_record(Nadf_field *record, BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count)
{
   BYTE len;
   void *object;
   int rec_count;

   *buffer_count = 4;
   
   for(rec_count = 1; rec_count < MAX_FIRST_PART; rec_count++)
   {
      object = record[rec_count].object;
      if(!object)
         continue;
      else
      {
         len = record[rec_count].len;
         *(unsigned short *)&(*buffer)[*buffer_count] = rec_count;
         switch(len)
         {
         case 2:
            *(unsigned short *)&(*buffer)[*buffer_count + 4] =
               *(unsigned short *)object;
            break;
         case 4:
            *(unsigned long *)&(*buffer)[*buffer_count + 4] =
               *(unsigned long *)object;
            break;
         default:
            memcpy(&(*buffer)[*buffer_count + 4], object, len); break;
         }
         if(len % 2) /* len is odd - need word align for asax */
         {
            /* fill in with a char */
            (*buffer)[*buffer_count + 4 + len] = 0;
            len++;
         }
         *(unsigned short *)&(*buffer)[*buffer_count + 2] = len;
         *buffer_count += 4 + len;
      }
   }
}

void append_compile_record(Nadf_field *record, BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count)
{
   BYTE len;
   void *object;
   int rec_count;

   for(rec_count = MAX_FIRST_PART; rec_count < MAX_REC_COUNT; rec_count++)
   {
      object = record[rec_count].object;
      if(!object)
         continue;
      else
      {
         len = record[rec_count].len;
         *(unsigned short *)&(*buffer)[*buffer_count] = rec_count;
         switch(len)
         {
         case 2:
            *(unsigned short *)&(*buffer)[*buffer_count + 4] =
               *(unsigned short *)object;
            break;
         case 4:
            *(unsigned long *)&(*buffer)[*buffer_count + 4] =
               *(unsigned long *)object;
            break;
         default:
            memcpy(&(*buffer)[*buffer_count + 4], object, len); break;
         }
         if(len % 2) /* len is odd - need word align for asax */
         {
            /* fill in with a char */
            (*buffer)[*buffer_count + 4 + len] = 0;
            len++;
         }
         *(unsigned short *)&(*buffer)[*buffer_count + 2] = len;
         *buffer_count += 4 + len;
      }
   }
}

void complete_compile_record(BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count)
{
   *(DWORD *)&(*buffer)[0] = *buffer_count;
}

void compile_whole_record(Nadf_field *record, BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count)
{
	start_compile_record (record, &watch_buffer, &watch_buffer_count);
	append_compile_record (record, &watch_buffer, &watch_buffer_count);
	complete_compile_record (&watch_buffer, &watch_buffer_count);
}

int submit_record(Nadf_field *record) /* sends a record to the expert system using the info in nadf_record */
{
   /* using the nadf_record array, prepare a record in the buffer */
   cancel_instruction = 0;

   compile_whole_record(record, &watch_buffer, &watch_buffer_count);

   /* we call the function asax directly */
   if(asax((char *) watch_buffer))
      done(); /* no more rules triggered, let's complete and quit */
   else /* still going, indicate whether the current instruction should be cancelled */
      return(cancel_instruction);
}

_inline int brkPtLookup(WORD curcs, DWORD cureip)
{
	int i;
	// Here, use macro or inline function instead...
	DWORD linAddr = (((DWORD) curcs) << 4) + (cureip & 0xFFFF);

	for (i=0 ; i<brkPtStackTop ; i++)
		if (brkPtStack[i].linAddr == linAddr)
			return i;
	return -1;
}

int linear_addr(WORD seg_sel, DWORD offset, DWORD *addr) /* returns 0 if allright */
{
   DWORD descr;
   WORD index = seg_sel >> 3;
   if(REAL_MODE)
   {
      *addr = (DWORD) ((((DWORD) seg_sel) << 4) + (offset & 0xffff));
      return(0);
   }
   else
   {
      if (seg_sel & 4)
      {
         if(!bx_cpu.ldtr.cache.valid
            || (index*8 + 7) > bx_cpu.ldtr.cache.u.ldt.limit)
            return(1);
         else
            descr = bx_cpu.ldtr.cache.u.ldt.base + index*8;
      }
      else
      {
         if((index*8 + 7) > bx_cpu.gdtr.limit)
            return(1);
         else
            descr = bx_cpu.gdtr.base + index*8;
      }
      bx_access_linear(descr + 2, 3, 0, BX_READ, addr);
      bx_access_linear(descr + 7, 1, 0, BX_READ, addr + 1);
      return(0);
   }
}

int report_irq(int irqno) // the return value shows whether the irq should be cancelled
// However, I do not know how to do it yet, so for now it will never be cancelled
{
   report_tag = IRQ;
   nadf_record[41].object = &irqno; /* Report the irq # as an immediate value */
   nadf_record[41].len = 1;
   nadf_record[42].object = NULL;
   nadf_record[43].object = NULL;
   nadf_record[44].object = NULL;
   
   submit_record(nadf_record);
   return(cancel_instruction);
}
