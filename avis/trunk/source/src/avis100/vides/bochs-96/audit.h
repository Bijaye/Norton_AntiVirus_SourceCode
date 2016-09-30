/* audit.h -- defs for auditing vital functions */

#ifndef AUDIT_H
#define AUDIT_H

#ifdef AUDIT_ENABLED

/* Abstractions to access memory, registers, etc. -- WRS */

#define BOCHS 1  /* change as need be */

#ifdef BOCHS

typedef Bit8u BYTE;
typedef Bit16u WORD;
typedef Bit32u DWORD;

#define getAH() AH
#define getAL() AL
#define getAX() AX
#define getBX() BX
#define getCX() CX
#define getDX() DX
#define getSI() SI
#define getDI() DI
#define getCS() bx_cpu.cs.selector.value
#define getDS() bx_cpu.ds.selector.value
#define getES() bx_cpu.es.selector.value
#define getSS() bx_cpu.ss.selector.value
#define getIP() IP
#define getCF() CF

#define FALSE 0
#define TRUE !FALSE

#define SameAddress(seg1,ofs1,seg2,ofs2) (((seg1)<<4)+ofs1)==(((seg2)<<4)+ofs2)
/* this is useless, it doesn't take into consideration the segmention mecanism */
#define SameAddress32(seg1,ofs1,seg2,ofs2) ((seg1==seg2) && (ofs1=ofs2))
#endif

#define audit_dosint 0x1
#define audit_biosint 0x2
#define audit_mem 0x4
#define IntStackSize 131072
//131072 262144 524288 1048576

typedef struct IntStack_s { 
    struct internal_audit_rec_s * iar;
    unsigned int CSeg;
    unsigned int Ofs;
} IntStack_t;

struct OrigIntVec_s {
  unsigned int intr, seg, ofs;
};

struct MemAudit_s {
  unsigned long start,end;
  unsigned int flags;
};

#define MEM_AUDIT_READ 1
#define MEM_AUDIT_WRITE 2

#define MAXVEC 2

extern unsigned int audit_select;
extern  int audit_output;
extern long unsigned int audit_opcode_count;
extern struct OrigIntVec_s OrigIntVec[];
extern struct MemAudit_s *MemAuditList;
extern unsigned int MemAuditLen;

void init_audit(char *);

#define AUDIT_INTERRUPT(i) if(audit_select && (audit_dosint || audit_biosint))\
                               audit_interrupt(i);

void audit_interrupt(unsigned intnum);

#define AUDIT_GENRETF(seg, ofs) if(audit_select & (audit_dosint | audit_biosint))\
                               audit_genretf(seg, ofs);

#define AUDIT_MEMORY(addr, writing, len, data) if (audit_select & audit_mem) audit_memory((addr), (writing), (len), (data));

#define MIN_STRING 100

#define AUDIT_STRING(src,dest,len) if((audit_select&audit_mem) && (getCX()>=MIN_STRING)) audit_string((src),(dest),(len))

void audit_memory(unsigned long, int, int, void *);
void audit_genretf(unsigned return_seg, unsigned ofs);
void audit_string(unsigned long src, unsigned long dest, unsigned int len);

#endif /* AUDIT_ENABLED */
#endif /* AUDIT_H */

extern int fdAudit;
