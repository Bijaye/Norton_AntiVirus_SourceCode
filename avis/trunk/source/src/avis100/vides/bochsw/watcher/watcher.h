#ifndef _WATCHER_H
#define _WATCHER_H


#include <io.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <process.h>
#include "..\..\Bochs-96\bochs.h"

#ifndef BYTE
typedef unsigned char BYTE;
#endif
#ifndef WORD
typedef unsigned short WORD;
#endif
#ifndef DWORD
typedef unsigned long DWORD;
#endif
#ifndef BOOL
typedef int BOOL;
#endif


/* macros to interface with bochs, and memory size parameters */

#define FILENAMESIZE 255

#define REAL_MODE bx_real_mode()
#define PANIC bx_panic
#define WATCH_ENABLED 1
#define MEMMARGIN 32 /* length of dummy memory at the end for bx_phys_mem */
#define MEMWATCH_SIZE (BX_PHY_MEM_SIZE + MEMMARGIN / 32) /* in QWORDS - 1/8th of the size of bx phy mem */
#define IOWATCH_SIZE (65536 / 32)
//#define BASE(segment) // need to do a descriptor lookup here


#define MEMWATCH_DESCR_SIZE 8192 /* in QWORDS */
//#define EXTRA_DESCR_ARRAY_SIZE 1024 /* in QWORDS as well */
#define WATCH_BUFFER_SIZE 4096 /* in BYTES, better be equal to PIPE_BUFFER_SIZE */
#define INT_WATCH_SIZE 1024 /* how many interrupt functions we are watching */

/* constant definitions */
#define I_FETCH_ACCESS 0
#define MEM_READ_ACCESS 1
#define MEM_WRITE_ACCESS 2
#define PUSH_ACCESS 3
#define POP_ACCESS 4

/* A buffer where the record is constructed */
extern BYTE watch_buffer[WATCH_BUFFER_SIZE];
extern DWORD watch_buffer_count;

/* a bit array to watch io port access */
extern DWORD iowatch[];

/* a huge datastructure: one bit for each byte of physical memory:
if the bit is set, watch that memory location is enabled */
extern DWORD memwatch[];
extern BOOL watching_mem;

/* macro to test if a particular memory location is audited */
#define MEM_IS_GUARDED(addr) (memwatch[addr >> 5] >> (addr & 0x1f)) & 1)
#define IO_IS_GUARDED(port) ((iowatch[port >> 5] >> (port & 0x1f)) & 1)
#define IO_IS_GUARDED2(port) (IO_IS_GUARDED(port) || IO_IS_GUARDED((port+1) & 0xffff))
#define IO_IS_GUARDED4(port) (IO_IS_GUARDED2(port) || IO_IS_GUARDED2((port+2) & 0xffff))

/* function to calculate the effective address in both real and protected mode */
extern int linear_addr(WORD seg_sel, DWORD offset, DWORD *addr);

/* list of descriptors for watched memory locations */
typedef struct Memwatch_descr_t {
   DWORD start; /* staring address */
   WORD size; /* size, max = 65536 */
   WORD marker; /* a marker corresponding to a tag to know what is at that memory location */
};

extern struct Memwatch_descr_t memwatch_descr[MEMWATCH_DESCR_SIZE];
extern unsigned int memwatch_descr_count;

/* tagging the register contents */

extern unsigned short reg_markers[0x10];
/*
	0: eax
	1: ecx
	2: edx
	3: ebx
	4: esp
	5: ebp
	6: esi
	7: edi
	8: es
	9: cs
	a: ss
	b: ds
	c: fs
	d: gs
	e: ?
	f: ?
*/

extern DWORD base_eip; /* eip at the beginning of an instruction */

#define TAG_INT 1
#define TAG_INT3 2
#define MEM_ACCESS 3
#define IN_AL 4
#define IN_AX 5
#define IN_EAX 6
#define OUT_AL 7
#define OUT_AX 8
#define OUT_EAX 9
#define REG32_ACCESS 10
#define IRQ 11
#define CMP 12
#define BRKPT 13
#define CALLFARIMM 14
#define TAG_MOVS 15
#define POST_END 16

// Describes the cause for an audit record
extern WORD report_tag;

// This is a counter increased at each instruction
extern int instruction_tick;

// Variables related to the audit breakpoint
extern int checkAuditBrkPt;		// On/Off switch

typedef struct {
	WORD brkPtCS;
	WORD brkPtIP;
	DWORD linAddr;
	int tag;
} BreakPoint;

#define BRKPTSTACKSIZE 4
extern BreakPoint brkPtStack[BRKPTSTACKSIZE];
extern brkPtStackTop;
extern _inline int brkPtLookup(WORD, DWORD);

// Constants defining the end of the first and the second (and last) part of the audit record
// The idea is that we may compile the beginning of an audit record at one time, and the second part later
// The goal is to build audit records at interrupt returns, that contain both argument and returned information
// See the structure used to build records in watcher.c
// See also the .adf file that describes the fields passed to Asax
#define MAX_FIRST_PART 45
#define MAX_REC_COUNT 60

// Pass variables to the expert system by reference
typedef struct {
   BYTE len;
   void *object;
} Nadf_field;

// Size of the stack used for interrupt auditing
#define INT_AUDIT_STACK_SIZE 1024

typedef struct {
	WORD reqAX;
	WORD reqBX;
	WORD reqCX;
	WORD reqDX;
	WORD reqSI;
	WORD reqDI;
	WORD reqBP;
	WORD reqDS;
	WORD reqES;
} RequestDescription;

typedef struct {
	BYTE int_no;
	short func;
	short sub;
	DWORD return_address;
	BYTE buffer[WATCH_BUFFER_SIZE];
	int buffer_count;
	int tick;
	WORD tag;
	RequestDescription req;
} Int_audit_stack_elem;

extern Int_audit_stack_elem int_audit_stack[INT_AUDIT_STACK_SIZE];
extern int int_audit_stack_top;

// The structure used to build audit records
extern Nadf_field nadf_record[];


// WARNING: the masks below must not clash with those used for notify types (NOTIFY_BEFORE and NOTIFY_AFTER)
#define WATCHSOMEFUNCS 0x100
#define WATCHALLFUNCS  0x200

// Array describing which interrupts are being watched and in which way
extern DWORD notify[0x100];


#define NOTIFY_BEFORE	1
#define NOTIFY_AFTER	2

// Watched_event:
// Structured type describing something being watched: what it is (interrupt, function, subfunction) and when to report it
// (before or after it happens, or both)
typedef struct {
	int int_no;
	int func;
	int sub;
	DWORD notify_type;
} Watched_event;

// The array of all watched events
extern Watched_event watched_event[INT_WATCH_SIZE];
extern int w_e_top;

extern int cancel_instruction;
extern int watcher_initializing;

extern char asaxmodule[];
extern char samplename[];
extern char asaxdescrfile[];
extern char virusreport[];


int irq_watch[];


/* function prototypes for watcher.c */
extern int watcher_init(void);
extern int watcher_shutdown(void);
extern void done(void);
extern int submit_record(Nadf_field *record);
extern int watch_feedback(void);
extern int report_irq(int irqno);
void start_compile_record(Nadf_field *record, BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count);
void append_compile_record(Nadf_field *record, BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count);
void complete_compile_record(BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count);
void compile_whole_record(Nadf_field *record, BYTE (*buffer)[WATCH_BUFFER_SIZE], DWORD *buffer_count);

// prototypes from watch_ins.c
void pushOnIntStack(Bit8u int_no, WORD tag, DWORD return_address);
int lookupIntStack(Bit8u int_no, DWORD return_address);
int lookupIntStack_by_int_no(Bit8u int_no);
int lookupIntStack_by_return_address(DWORD return_address);
int sameRequest(RequestDescription *r1, RequestDescription *r2);
void fillRequest(RequestDescription *r1);

// Manipulation of watched events array
/*_inline*/ int w_e_lookup(int int_no, int func, int sub, int *retindex);
void w_e_insert(int int_no, int func, int sub, DWORD notify_type);

/* function prototypes for watch_memory.c */
extern __inline void set_guard(DWORD addr, WORD size, WORD marker);
extern __inline void dismiss_guard(DWORD addr);
extern __inline int memarea_is_guarded(unsigned int len, unsigned int shift, unsigned int tbl_addr);
extern __inline int memwatch_scan(DWORD index, DWORD tbl_len);
extern __inline int note_memaccess(unsigned int addr, unsigned int len, unsigned int rw, void *data);

/* prototypes for asax functions called by bochs */
int asax_init(char *descrfile, char *module_name, char *reportfile);
int asax(char *record_buffer);
int asax_complete(char *record_buffer);

/* input/output functions */
extern Bit8u   bx_cpu_inp8(Bit16u addr);
extern void    bx_cpu_outp8(Bit16u addr, Bit8u value);
extern Bit16u  bx_cpu_inp16(Bit16u addr);
extern void    bx_cpu_outp16(Bit16u addr, Bit16u value);
extern Bit32u  bx_cpu_inp32(Bit16u addr);
extern void    bx_cpu_outp32(Bit16u addr, Bit32u value);

/* register control functions */
extern __inline int checkreg8(int index);
extern __inline int checkreg16(int index);
extern __inline int checkreg32(int index);
extern __inline int checkreadreg8(int index);
extern __inline int checkreadreg16(int index);
extern __inline int checkreadreg32(int index);
extern __inline int checkwritereg8(int index);
extern __inline int checkwritereg16(int index);
extern __inline int checkwritereg32(int index);
extern __inline int checkbasereg16(int index);
extern __inline int checkindexreg16(int index);


#endif /* _WATCHER_H */
