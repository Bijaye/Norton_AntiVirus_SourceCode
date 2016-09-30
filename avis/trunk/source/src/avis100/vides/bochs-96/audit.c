/* audit.c -- audit routines */
#include <stdio.h>
/*#include <malloc.h>*/

#include "bochs.h"
#include "audit.h"
#include "dosfns.h"
#include "NADF_file.h"
#include <signal.h>

#define isodd(i) (((i)&1) == 1)

/* constants */

/* variables: */
IntStack_t IntStack[IntStackSize];
int IntStackTop = 0;
char pAuditBuf[512];    /* buffer to hold NADF output record */
char *tmpAuditBuf = pAuditBuf + 4;
struct MemAudit_s *MemAuditList;
unsigned int MemAuditLen;


unsigned int audit_select;
int fdAudit;
unsigned long int audit_opcode_count = 0l;
struct OrigIntVec_s OrigIntVec[] =
{
  {0x21, 0, 0},
  {0x13, 0, 0},
  {0x40, 0, 0} /* alternate int 13 */
};

/* internal prototypes */

char *xferFCB(char * fn);
char *xferXFCB(char * fn2);

void pushInt(struct internal_audit_rec_s * iar, WORD seg, WORD ofs);
struct internal_audit_rec_s *popInt(WORD *seg, WORD *ofs);
int peekInt(int how_far, struct internal_audit_rec_s * iar, WORD *seg, WORD *ofs);

void SetupIAR(struct internal_audit_rec_s * iar, int function);
void SetupUnDocIAR(struct internal_audit_rec_s *iar);
void FillArgs(struct ArgVals_s *fnarg);
static inline void WriteAuditRecord(struct internal_audit_rec_s *iar, WORD CSeg, WORD Ofs);
void WriteFields(struct ArgVals_s *fnarg, int set);
static inline void sanatize(char *str); /* WARNING: MODIFIES str !!!! */

void WriteNADFRecord(void);
void WriteByteField(int id, BYTE c);
void WriteWordField(int id, WORD w);
void WriteDWordField(int id, DWORD dw);
void WriteStringField(int id, char *s);

void exit_proc(int);

/* functions */

void dumpStack(void)
{
  int i;
  WORD tempCS, tempOfs;
  struct internal_audit_rec_s *iarTmp2;
  
  printf("Exiting and dumping all leftover ints from stack..\n");
  for (i=IntStackTop; IntStackTop>0; i++)
    {
      iarTmp2=popInt(&tempCS, &tempOfs);
      iarTmp2->end_time = audit_opcode_count;
      FillArgs(&iarTmp2->ret);
      WriteAuditRecord(iarTmp2, tempCS, tempOfs);
      if (iarTmp2) free(iarTmp2);
    }
  free (MemAuditList);
}

void init_audit(char *strNADF_file)
{
	unlink(strNADF_file);   /* in case the file exists, remove it first */
	fdAudit = creat_NADF(strNADF_file);     /* create NADF compatible file */
	audit_select = audit_dosint | audit_biosint | audit_mem;
	tmpAuditBuf = pAuditBuf + 4;    /* reset record pointer */

	MemAuditLen=6;
	MemAuditList=(struct MemAudit_s *)malloc(sizeof(struct MemAudit_s)*MemAuditLen);

	/* monitor changes to RAM available in BIOS data seg */
	MemAuditList[0].start=0x413;
	MemAuditList[0].end=0x413;
	MemAuditList[0].flags=MEM_AUDIT_READ|MEM_AUDIT_WRITE;

	/* monitor changes to vectors for int 21, 13, and 2F; 1C and 8. */
	MemAuditList[1].start=0x21*4;
	MemAuditList[1].end=0x21*4+3;
	MemAuditList[1].flags=MEM_AUDIT_WRITE;
	MemAuditList[2].start=0x13*4;
	MemAuditList[2].end=0x13*4+3;
	MemAuditList[2].flags=MEM_AUDIT_WRITE;
	MemAuditList[3].start=0x2F*4;
	MemAuditList[3].end=0x2F*4+3;
	MemAuditList[3].flags=MEM_AUDIT_WRITE;
	MemAuditList[4].start=0x1C*4;
	MemAuditList[4].end=0x1C*4+3;
	MemAuditList[4].flags=MEM_AUDIT_WRITE;
	MemAuditList[5].start=0x8*4;
	MemAuditList[5].end=0x8*4+3;
	MemAuditList[5].flags=MEM_AUDIT_WRITE;

	signal(SIGTERM, exit_proc);
 
	atexit (dumpStack);
}

void exit_proc(int foo)
{
  exit(0);
}

void audit_interrupt(unsigned int int_num)
{
    int i;
    struct internal_audit_rec_s * iarTmp = NULL;
    struct dosfns_s *services; /* generic pointer to service table */
    enum iartype_e inttype;
    
    /* is it a dos interrupt */
    switch (int_num)
      {
      case 0x21:
	services=dosfns;
	inttype=DOSINT;
	break;
      case 0x13:
	services=int13fns;
	inttype=DISKINT;
	break;
      case 0x40:
	services=int13fns;
	inttype=DISKINT;
	break;
      case 0x1A:
	services=int1Afns;
	inttype=CMOSINT;
	break;
      default:
	return;   /* not an interrupt we're trapping so get lost */
      }

    for(i=0; services[i].num != -1; i++)
      {
	if(services[i].ah == getAH())
	  {
	    if(services[i].al == -1) /* function w/o subfunction */
	      {                 
		/* create audit record */
		iarTmp = malloc(sizeof(struct internal_audit_rec_s));
		if(iarTmp == NULL)
		  {
		    fprintf(stderr, "PANIC: malloc failed for iarTmp\n");
		    exit(1);
		  }
		/* fill it with data */
		iarTmp->type=inttype;
		SetupIAR(iarTmp, i);
		pushInt(iarTmp, getCS(), getIP());
		break;
	      }
	    else                /* function with subfunction */
	      {
		if( services[i].al == getAL())
		  {             /* we've found the subfunction */

		    /* create audit record */
		    iarTmp = malloc(sizeof(struct internal_audit_rec_s));
		    if(iarTmp == NULL)
		      {
			fprintf(stderr, "PANIC: malloc failed for iarTmp\n");
			exit(1);
		      }
		    
		    /* fill it with data */
		    iarTmp->type=inttype;
		    SetupIAR(iarTmp, i);
		    pushInt(iarTmp, getCS(), getIP());
		    break;
		  }
		else
		  continue;     /* try the next one */
	      }         /*else*/
	 }/*if*/
      } /*for*/
    if( iarTmp == NULL )
      {   /* Unknown function !!! */                        
	/* create audit record */
	iarTmp = malloc(sizeof(struct internal_audit_rec_s));
	if(iarTmp == NULL) {
	  fprintf(stderr, "PANIC: malloc failed for iarTmp\n");
	  exit(1);
	}
	/* fill it with data */
	iarTmp->type=inttype;
	SetupUnDocIAR(iarTmp);
	pushInt(iarTmp, getCS(), getIP());
      }
  }

void SetupIAR(struct internal_audit_rec_s *iar, int index)
{
  struct dosfns_s *services;

  switch(iar->type)
    {
    case DOSINT: services=dosfns; break;
    case DISKINT: services=int13fns; break;
    case CMOSINT: services=int1Afns; break;
    }
  
    iar->function = services[index].num;
    iar->start_time = audit_opcode_count;
    iar->end_time = -1;  /* meaning: not set */
    iar->args.type = services[index].argtype;
    iar->ret.type = services[index].rettype;

    FillArgs(&iar->args);
}

void SetupUnDocIAR(struct internal_audit_rec_s *iar)
{

    iar->function = 0;

    iar->start_time = audit_opcode_count;
    iar->end_time = -1;  /* meaning: not set */
    iar->args.type = fnargs_all;
    iar->ret.type = fnargs_all;

    FillArgs(&iar->args);
}


void FillArgs(struct ArgVals_s *fnarg)
{
    fnarg->ax =  getAX();
    fnarg->bx =  getBX();
    fnarg->cx =  getCX();
    fnarg->dx =  getDX();
    fnarg->si =  getSI();
    fnarg->di =  getDI();
    fnarg->ds =  getDS();
    fnarg->es =  getES();
    fnarg->cf =  getCF();

    /* get data for the special cases */
    switch (fnarg->type)
    {
	case fnargs_asciz:
	case fnargs_al_cl_asciz:
	case fnargs_al_asciz:
	case fnargs_cl_asciz:
#ifdef BOCHS
	    bx_virtual_block_read(&bx_cpu.ds, DX, ASCIZ_MAXLEN, fnarg->str1);
#endif
	    fnarg->str1[ASCIZ_MAXLEN-1] = 0;
	    sanatize(fnarg->str1);
	    break;
	case fnargs_cl_2_asciz:
#ifdef BOCHS
	    bx_virtual_block_read(&bx_cpu.ds, DX, ASCIZ_MAXLEN, fnarg->str1);
#endif
	    fnarg->str1[ASCIZ_MAXLEN-1] = 0;
	    sanatize(fnarg->str1);
#ifdef BOCHS
	    bx_virtual_block_read(&bx_cpu.es, DI, ASCIZ_MAXLEN, fnarg->str2);
#endif
	    fnarg->str2[ASCIZ_MAXLEN-1] = 0;
	    sanatize(fnarg->str2);
	    break;
	case fnargs_fcb:
	case fnargs_cx_fcb:
	    xferFCB(fnarg->str1);
	    break;
	case fnargs_dl_asciz_in_si:
	case fnargs_bx_cx_dx_asciz_in_si:
#ifdef BOCHS
	    bx_virtual_block_read(&bx_cpu.ds, SI, ASCIZ_MAXLEN, fnarg->str1);
#endif
	    fnarg->str1[ASCIZ_MAXLEN-1] = 0;
	    sanatize(fnarg->str1);
	    break;
	case fnargs_xfcb:
	    xferFCB(fnarg->str1);
	    xferXFCB(fnarg->str2);
	    break;
    }
}

void audit_memory(unsigned long addr, int writing, int len, void *data)
{
  WriteDWordField(1, getCS());
  WriteDWordField(2, getIP());
  WriteWordField(3, (WORD) MEM);
  WriteWordField(4, (WORD) writing);
  WriteDWordField(5, audit_opcode_count);
  WriteDWordField(6, audit_opcode_count);
  switch(writing)
    {
    case 0: /* memory read event */
    case 1: /* memory write event */
      WriteDWordField(37, addr);
      switch(len)
	{
	case 2: WriteDWordField(38, (Bit32u)*(WORD *)data);
	  break;
	case 1: WriteDWordField(38, (Bit32u)*(BYTE *)data);
	  break;
	}
      break;
    }
  WriteNADFRecord();
}

void audit_string(unsigned long src, unsigned long dest, unsigned int len)
{
  WriteDWordField(1, getCS());
  WriteDWordField(2, getIP());
  WriteWordField(3, (WORD) MEM);
  WriteWordField(4, (WORD) 2);
  WriteDWordField(5, audit_opcode_count);
  WriteDWordField(6, audit_opcode_count);
  WriteDWordField(37, src);
  WriteDWordField(38, len);
  WriteDWordField(39, dest);
  WriteNADFRecord();
}

void WriteNADFRecord(void)
{
	int *pi;
	int rc;

	pi = (int *)pAuditBuf;
	*pi = tmpAuditBuf - pAuditBuf;
	if ((rc = write_NADF(fdAudit, pAuditBuf, 1)) != 0) {
		fprintf(stderr, "cannot write: %d\n",  rc);
		exit(1);
	}
	tmpAuditBuf = pAuditBuf + 4;
}


void WriteByteField(int id, BYTE c)
{
	register char *p;
	WORD *pw;

	p = tmpAuditBuf;

	pw = (WORD *)p;
	*pw = id;
	p += sizeof(WORD);

	pw = (WORD *)p;
	*pw = sizeof(BYTE);
	p += sizeof(WORD);

	*p = c;
	p += sizeof(BYTE);

	tmpAuditBuf = p + 1;    /* padding byte added */
}

void WriteWordField(int id, WORD w)
{
	register char *p;
	WORD *pw;
	
	p = tmpAuditBuf;

	pw = (WORD *)p;
	*pw = id;
	p += sizeof(WORD);

	pw = (WORD *)p;
	*pw = sizeof(WORD);
	p += sizeof(WORD);

	pw = (WORD *)p;
	*pw = w;
	p += sizeof(WORD);

	tmpAuditBuf = p;                
}

void WriteDWordField(int id, DWORD dw)
{
	register char *p;
	//int *pi;
	WORD *pw;
	DWORD *pdw;
	
	p = tmpAuditBuf;

	pw = (WORD *)p;
	*pw = id;
	p += sizeof(WORD);

	pw = (WORD *)p;
	*pw = sizeof(DWORD);
	p += sizeof(WORD);

	pdw = (DWORD *)p;
	*pdw = dw;
	p += sizeof(DWORD);

	tmpAuditBuf = p;        
}

void WriteStringField(int id, char *s)
{
	register char *p;
	//int *pi;
	WORD *pw;
	//DWORD *pdw;
	
	p = tmpAuditBuf;

	pw = (WORD *)p;
	*pw = id;
	p += sizeof(WORD);

	pw = (WORD *)p;
	*pw = strlen(s);
	p += sizeof(WORD);

	strcpy(p, s);
	p += strlen(s);
	
	tmpAuditBuf = p;

	if (isodd(pAuditBuf - tmpAuditBuf))
		*tmpAuditBuf++ = ' '; /* padding character */
}

static inline void sanatize(char *str) /* WARNING: MODIFIES str !!!! */
{
    int i;

    for(i=0; i < strlen(str); i++)
	if((str[i] < ' ') || (str[i] > '~'))
	    str[i] = '.';
}

char *xferFCB(char * fn)
{
    //unsigned i;
    unsigned int off = getDX();
#ifdef PANDORA
    BYTE *segp = &memory[(sregs[DS]) << 4];

    fn[0] = (char)((unsigned)GetMemB(segp, off) + (unsigned)('A' -1));
    fn[1] = ':';

    for(i=1; i < 0x0d; i++)
	fn[i+1] = GetMemB(segp, i+off);
#elif BOCHS
    {
    Bit8u datatemp;
    bx_read_virtual_byte(&bx_cpu.ds, off, &datatemp);
    fn[0] = (char) ('A'-1+datatemp);
    fn[1]=':';
    bx_virtual_block_read(&bx_cpu.ds, off+1, 12, fn+2);
    }
#endif
    fn[0x0e] = 0;
    return fn;
}

char *xferXFCB(char * fn2)
{
    //int i;
    WORD off = getDX();

#ifdef PANDORA
    BYTE *segp = &memory[getDS() << 4];

    for(i=0; i < 11; i++)
	fn2[i] = GetMemB(segp, i+off+11);
#else
#ifdef BOCHS
    bx_virtual_block_read(&bx_cpu.ds, off+11, 11, fn2);
#endif
#endif 

    fn2[11] = 0;
    return fn2;
}

void pushInt(struct internal_audit_rec_s * iarTmp, WORD seg, WORD ofs)
{

  int i;
  struct internal_audit_rec_s *iarTmp2;
  WORD tempCS, tempOfs;

    if(IntStackTop < IntStackSize)
    {
	IntStack[IntStackTop].iar = iarTmp;
	IntStack[IntStackTop].CSeg = seg;
	IntStack[IntStackTop].Ofs = ofs;
	IntStackTop++;
    }
    else
    {
	fprintf(stderr, "IntStack overflow! Dumping all Ints from stack\n");
	for (i=IntStackTop; i>=0; i++)
	  {
	    iarTmp2=popInt(&tempCS, &tempOfs);
	    iarTmp2->end_time = audit_opcode_count; 
	    FillArgs(&iarTmp2->ret);
	    WriteAuditRecord(iarTmp2, tempCS, tempOfs);
	    free(iarTmp2);
	  }
    }
}

struct internal_audit_rec_s *popInt(WORD *seg, WORD *ofs)
{
    struct internal_audit_rec_s * iarTmp;

    if(IntStackTop > 0)
    {
	IntStackTop--;
	iarTmp = IntStack[IntStackTop].iar;
	*seg = IntStack[IntStackTop].CSeg;
	*ofs = IntStack[IntStackTop].Ofs;
	return iarTmp;
    }
    else
    {
	fprintf(stderr, "IntStack underflow!!!!!\n");
	return NULL;
    }
}

int peekInt(int how_far, struct internal_audit_rec_s *iar, WORD *seg, WORD *ofs)
{
    register int peekStackPtr = IntStackTop - how_far -1;

    if(peekStackPtr >= 0)
    {
	iar = IntStack[peekStackPtr].iar;
	*seg = IntStack[peekStackPtr].CSeg;
	*ofs = IntStack[peekStackPtr].Ofs;
	return TRUE;
    }
    else
    {
	return FALSE;
    }
}

void flushInt(void)
{
    IntStackTop = 0;
}

void audit_genretf(unsigned return_seg, unsigned return_ofs)
{
    int i, r = FALSE;
    WORD uTmpCSeg, uTmpOfs;
	struct internal_audit_rec_s dummyIAR; // It is not of any utility (Fred)
    struct internal_audit_rec_s * iarTmp;


    /* first check whether the interrupt is on the stack */   
//    for(i = 0; peekInt(i, iarTmp, &uTmpCSeg, &uTmpOfs); i++)
    for(i = 0; peekInt(i, & dummyIAR, &uTmpCSeg, &uTmpOfs); i++)
    {
	r = ((uTmpCSeg == return_seg) && (return_ofs >= uTmpOfs));
	if(r) break;
    }

    if(r)
	/* we've found it on the stack, so lets pop it */
	if(i == 0)
	{
	    /* the audit record is right on the top (where it should be) */
	    iarTmp = popInt(&uTmpCSeg, &uTmpOfs);
	    iarTmp->end_time = audit_opcode_count; 
	    FillArgs(&iarTmp->ret);
	    WriteAuditRecord(iarTmp, uTmpCSeg, uTmpOfs);
	    free(iarTmp);
	}
	else
	{
	    /* dig for the interrupt, printing everything as we go */
	    for(; i >= 0; i--)
	    {
		iarTmp = popInt(&uTmpCSeg, &uTmpOfs);
		iarTmp->end_time = audit_opcode_count; 
		FillArgs(&iarTmp->ret);
		WriteAuditRecord(iarTmp, uTmpCSeg, uTmpOfs);
		free(iarTmp);
	    }
	}
}

static inline void WriteAuditRecord(struct internal_audit_rec_s *iar, WORD CSeg, WORD Ofs)
{
	WriteDWordField(1, CSeg);
	WriteDWordField(2, Ofs);
	WriteWordField(3, iar->type);
	WriteWordField(4, iar->function);
	WriteDWordField(5, iar->start_time);
	WriteDWordField(6, iar->end_time);
	WriteFields(&iar->args, 0);
	WriteFields(&iar->ret, 1);
	WriteNADFRecord();
}

void WriteFields(struct ArgVals_s *fnarg, int set)
{
    int i;
    WORD uFields = argNIL;
    /*char argbuf[sizeof(struct ArgVals_s) +2];
    int argidx= 0;*/

    for(i=0; fnregs[i].arg != -1; i++)
	if(fnregs[i].arg == fnarg->type) uFields = fnregs[i].regs;

    /* AL */
    if(uFields & argAL)
	    WriteWordField(7 + set * 15 + 0, (WORD)(BYTE)fnarg->ax);
    /* AX */
    if(uFields & argAX)
	    WriteDWordField(7 + set * 15 + 1, fnarg->ax);
    /* BL */
    if(uFields & argBL)
	    WriteWordField(7 + set * 15 + 2, (WORD)(BYTE)fnarg->bx);
    /* BX */
    if(uFields & argBX)
	    WriteDWordField(7 + set * 15 + 3, fnarg->bx);
    /* CL */
    if(uFields & argCL)
	    WriteWordField(7 + set * 15 + 4, (WORD)(BYTE)fnarg->cx);
    /* CX */
    if(uFields & argCX)
	    WriteDWordField(7 + set * 15 + 5, fnarg->cx);
    /* DL */
    if(uFields & argDL)
	    WriteWordField(7 + set * 15 + 6, (WORD)(BYTE)fnarg->dx);
    /* DX */
    if(uFields & argDX)
	    WriteDWordField(7 + set * 15 + 7, fnarg->dx);
    /* SI */
    if(uFields & argSI)
	    WriteDWordField(7 + set * 15 + 8, fnarg->si);
    /* DI */
    if(uFields & argDI)
	    WriteDWordField(7 + set * 15 + 9, fnarg->di);
    /* DS */
    if(uFields & argDS)
	    WriteDWordField(7 + set * 15 + 10, fnarg->ds);
    /* ES */
    if(uFields & argES)
	    WriteDWordField(7 + set * 15 + 11, fnarg->es);
    /* CF */
    if(uFields & argCF)
	    WriteWordField(7 + set * 15 + 12, (WORD)(BYTE)fnarg->cf);
    /* str1 */
    if(uFields & argSTR1)
	    WriteStringField(7 + set * 15 + 13, fnarg->str1);
    /* str2 */
    if(uFields & argSTR2)
	    WriteStringField(7 + set * 15 + 14, fnarg->str2);

}
