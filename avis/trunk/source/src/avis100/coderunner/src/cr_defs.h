/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_DEFS.H                                                *
 *                                                                         *
 * Description:   INTeL 8086/88 interpreter.                               *
 *                                                                         *
 *                All definitions for CodeRunner.                          *
 *                                                                         *
 *      Must be compiled with /dBIG_ENDIAN for big-endian machines.        *
 *      Must be compiled with /dAIX        for AIX.                        *
 *      Must be compiled with /dDOS        for DOS.                        *
 *      Must be compiled with /dOS2_16     for 16-bit OS2.                 *
 *      Must be compiled with /dOS2_32     for 32-bit OS2.                 *
 *                                                                         *
 ***************************************************************************/


#ifndef  _CR_DEFS_H_
#define  _CR_DEFS_H_


/* Number of interrupt vectors to be emulated into IVT.
 */
#define  VECTORS_IN_IVT 0x100


/*-------------------------------------------------------------------------*/
/* Datatypes.                                                              */
/*-------------------------------------------------------------------------*/

typedef  unsigned char  CR_BYTE;
typedef  unsigned short CR_WORD;
typedef  unsigned short USHORT;
typedef  char           CHAR;
typedef  unsigned char  UCHAR;
typedef  long           CR_LONG;
typedef  unsigned long  CR_ULONG;
typedef  unsigned long  *PULONG;
typedef  unsigned short BOOL;
typedef  void           VOID;
typedef  VOID           *PVOID;
#ifndef BYTE
#define BYTE CR_BYTE
#endif
#ifndef WORD
#define WORD CR_WORD
#endif
#ifndef LONG
#define LONG CR_LONG
#endif
#ifndef ULONG
#define ULONG CR_ULONG
#endif
#ifndef DWORD
#define DWORD CR_ULONG
#endif

typedef  BYTE           byte;

#define  MK_WORD(hi_byte,lo_byte) ((WORD)((((WORD)(hi_byte))<<8) + ((WORD)lo_byte)))
#define  SWAPB(a) (((a)<<8)+((a)>>8))

#ifndef  FALSE
   #define  FALSE 0
#endif
#ifndef  TRUE
   #define  TRUE  1
#endif


/* Definitions, compatible with decompression sources.
 */
#ifndef  SUCCESS
#define  SUCCESS                    (0)
#endif
#define  ERROR                      (-1)     /* General error.             */
#define  ERROR_CANT_OPEN_FILE       (-2)     /* Cannot open some file.     */
#define  ERROR_CANT_READ_FILE       (-3)
#define  ERROR_CANT_WRITE_FILE      (-4)
#define  ERROR_CANT_CREATE_FILE     (-5)
#define  ERROR_NOT_ENOUGH_MEMORY    (-8)

/* Specific to CR_FSCAN.C  */
#define  ERROR_TOO_MANY_EXEC_TRAPS  (-9)
#define  ERROR_TOO_MANY_RW_TRAPS    (-10)
#define  ERROR_EXEC_TRAP_OUTSIDE    (-11)
#define  ERROR_TRAP_ADDR_EXISTS     (-12)
#define  ERROR_NON_TRAP_TERMINATION (-13)



/*-------------------------------------------------------------------------*/
/* Macros.                                                                 */
/*-------------------------------------------------------------------------*/

/* Convert Real mode Segment:Offset to linear address.
 */
#define  LinearAddr(Seg,Off) (((((ULONG)(Seg))<<4)+(Off))&0xFFFFFUL)
//ULONG LinearAddr(WORD Seg, WORD Off);
#define  MALinearAddr(pMA) (pMA->LinAddr)

/* Macro returns ptr to allocated buffer in MA, corresponding to requested
 * segment and offset.
 */
#define  BufferInMA(pMA,RealSegment,RealOffset)   ((BYTE *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA))))

/* "Function" returning BYTE. Macro returns a byte from a specified
 * pointer to memory area. Has been replaced by a function so we can easily
 * keep records of memory reads.
 */
#if 0
#define  ReadByteFromMA(pMA,RealSegment,RealOffset) (*(BYTE *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA))))
#endif
/* Note: below 2 macros assuming, that all bytes are present in the single MA.
 */
#define  ReadDWordFromMA(pMA,RealSegment,RealOffset) (*(DWORD *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA))))

/* "Function" returning VOID, but doing something.
 *  This macro write a byte to a specified memory area.
 */
#define  WriteByteToMA(byte,pMA,RealSegment,RealOffset)  (*(BYTE *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA)))=((BYTE)(byte)))

/*-------------------------------------------------------------------------*/
/* CPU flags masks.                                                        */
/*-------------------------------------------------------------------------*/
#define  CARRY_FLAG_MASK      ((WORD)0x001)
#define  PARITY_FLAG_MASK     ((WORD)0x004)
#define  AUX_FLAG_MASK        ((WORD)0x010)
#define  ZERO_FLAG_MASK       ((WORD)0x040)
#define  SIGN_FLAG_MASK       ((WORD)0x080)
#define  TRAP_FLAG_MASK       ((WORD)0x100)
#define  INT_FLAG_MASK        ((WORD)0x200)
#define  DIRECTION_FLAG_MASK  ((WORD)0x400)
#define  OVERFLOW_FLAG_MASK   ((WORD)0x800)

/*-------------------------------------------------------------------------*/
/* Types of supplied memory area.                                          */
/*-------------------------------------------------------------------------*/
#define  READABLE             0x01  /* Memory area is readable.            */
#define  WRITEABLE            0x02  /* Memory area is writeable.           */
#define  EXECUTABLE           0x04  /* Memory area is executable.          */

/*-------------------------------------------------------------------------*/
/* Possible causes of return from <CodeRunner>.                            */
/* Bits to be used in:                                                     */
/*    <RETURN_DATA.ulRetCode> and                                          */
/*    <MACHINE_STATE.ulIgnore>.                                            */
/*-------------------------------------------------------------------------*/
#define  READ_TRAP            0x01  /* Read from non-readable memory.      */
#define  WRITE_TRAP           0x02  /* Write to non-writeable memory.      */
#define  EXECUTE_TRAP         0x04  /*(!) Execute from non-executable area.*/
#define  UNSUPPORTED_OPCODE   0x08  /* Execute unsupported instruction.    */
#define  MAXOPS_REACHED       0x10  /* Execute <maxops>+1 instruction.     */
#define  TRACE_FLAG_SET       0x20  /* Attempt to set TF flag.             */
#define  ZERODIVIDE           0x40  /* Attempt to divide by 0.             */
#define  DIVIDE_ERROR         0x80  /* Division result doesn't fit in dest.*/
#define  PORT_IO              0x100 /* Attempt to execute port IN or OUT.  */
#define  CANNOT_EXECUTE_INT   0x200 /* Attempt to execute INT xx           */
                                    /* without user-supplied Fn pointer or */
                                    /* without supplied IVT or without     */
                                    /* enough stack space.                 */
#define  USER_ABORT           0x400 /*(!) Request to abort from user suppl.*/
                                    /* callback INT-function for any reason*/
#define  HLT_EXECUTED         0x800 /* HLT instruction executed.           */
#define  CODE_SELF_MODIFIED   0x20000000UL   /* Code modified itself       */
#define  BREAK_CONDITION      0x40000000UL   /* Break condition is TRUE.   */
                                    /* Break condition callback function   */
                                    /* Returned TRUE.                      */
#define  INVALID_PARAMETER    0x80000000UL   /* Invalid input data.        */

                                    /* Note: Flags marked as: (!) cannot   */
                                    /* be masked by bits in                */
                                    /* <MACHINE_STATE.ulIgnore>.           */
                                    /* <CodeRunner> will reset these ignore*/
                                    /* bits anyway.                        */
#ifdef CR_TSR
#define  TEST_TSR             0x1000/* jce - try stuff after going res.    */
#define  GENERAL_PROTECTION   0x4000/* 32 bit stuff, addressing violation  */
#define  USER_INT08           0x8000/* a need to execute int 0x08 ?        */
#endif
#ifdef CR_MOREFEATURES
#define  USER_CANCEL          0x2000/* riad - to avoid redenduncy          */
#endif

/*-------------------------------------------------------------------------*/
/* Possible return codes from user supplied callback function              */
/* MACHINE_STATE.pfnINTxx                                                  */
/*-------------------------------------------------------------------------*/
#define  USER_INT_CONTINUE    0     /* Execution successful. Continue.     */
                                    /* Or... same as ignore it. Continue.  */
#define  USER_INT_YOURSELF    1     /* Try to execute INT xx directly.     */
#define  USER_INT_ABORT      -1     /* Abort everything and exit.          */

#ifdef CR_TSR
/*jce*/
#define  USER_INT_TSR         2     /* went resident, try some other stuff */
#endif

/*-------------------------------------------------------------------------*/
/* Possible return codes from user supplied callback function              */
/* MACHINE_STATE.pfnExecControl and .pfnWipingMemory.                      */
/*-------------------------------------------------------------------------*/
#define  USER_EXEC_CONTINUE   0     /* Continue normal execution of this   */
                                    /* instruction.                        */
#define  USER_EXEC_SKIP       2     /* Do not execute this instruction.    */
                                    /* Skip it and continue execution      */
                                    /* from the next instruction.          */
#define  USER_EXEC_ABORT     -1     /* Abort everything and exit.          */
                                    /* Will cause <USER_ABORT> flag to be  */
                                    /* set in return code.                 */
#ifdef CR_MOREFEATURES
#define  USER_EXEC_CANCEL    -2     /* to avoid redenduncy                 */
#endif
/*-------------------------------------------------------------------------*/
/* Structures (as datatypes).                                              */
/*-------------------------------------------------------------------------*/

struct   cr_ini
   {
   ULONG          MaxActiveInstructions;
   WORD           MaxIdleInstructions;
   WORD           MaxIdleLoops;
   WORD           MaxRWTrapsToRecover;
   WORD           MaxExecTrapsToRecover;
   WORD           ExeTrapHalfBuffSize;
   ULONG          ulFlagsIgnoreConditions;
   WORD           RWTrapHalfBuffSize;
   WORD           MaxCXForLoops;
   WORD           MaxIncreasingExecLA;
   WORD           MaxActiveSincePartialMatch;
#ifdef CR_MOREFEATURES  //riad//
   ULONG          MaxOpcodes;
   WORD           MaxIdleBeforeSkip;
   WORD           MaxDeepness;
#endif
   BYTE           xxx[16];
   };

struct   scanworkbuf_vars
   {
   unsigned short     scalpel_mode;          /* Scan more buffers if */
   char               *filename;             /* Not used by CR.      */
   unsigned short     is_exe_type;           /* Not used by CR.      */
   int                object_type;           /* Not used by CR.      */
   char               *vp_name;              /* Not used by CR.      */
   unsigned short     inside_compressed;     /* Not used by CR.      */
   ULONG              FileSize;              /* Full file size.      */
   ULONG              saved_epoint;          /* ept offset into file */
   };

typedef struct _MOD_RM_INFO
   {
   WORD           Seg1;
   WORD           Off1;
   VOID           *pReg1;     /* If==0 - use Seg1/Off1, else use *pReg1.   */
   VOID           *pReg2;     /* Register - Bits 3,4,5 of mod/rm byte.     */
   } MOD_RM_INFO;

#ifndef NEW1

#ifndef  BIG_ENDIAN
   typedef  struct   _BYTEREGS
      {
      BYTE  RegAL, RegAH;
      BYTE  RegBL, RegBH;
      BYTE  RegCL, RegCH;
      BYTE  RegDL, RegDH;
      } BYTEREGS;
#else
   typedef  struct   _BYTEREGS
      {
      BYTE  RegAH, RegAL;
      BYTE  RegBH, RegBL;
      BYTE  RegCH, RegCL;
      BYTE  RegDH, RegDL;
      } BYTEREGS;
#endif

typedef  struct   _WORDREGS
   {
   WORD  RegAX, RegBX, RegCX, RegDX;
   WORD  RegSI, RegDI, RegBP;
   WORD  RegDS;
   WORD  RegES;
   WORD  RegCS, RegIP;
   WORD  RegSS, RegSP;
   WORD  RegFlags;
   } WORDREGS;

typedef  union    _REGISTERS
   {
   BYTEREGS byteregs;
   WORDREGS wordregs;
   } REGISTERS;

#define  rAL      (pMachState->Regs.byteregs.RegAL)
#define  rAH      (pMachState->Regs.byteregs.RegAH)
#define  rAX      (pMachState->Regs.wordregs.RegAX)

#define  rBL      (pMachState->Regs.byteregs.RegBL)
#define  rBH      (pMachState->Regs.byteregs.RegBH)
#define  rBX      (pMachState->Regs.wordregs.RegBX)

#define  rCL      (pMachState->Regs.byteregs.RegCL)
#define  rCH      (pMachState->Regs.byteregs.RegCH)
#define  rCX      (pMachState->Regs.wordregs.RegCX)

#define  rDL      (pMachState->Regs.byteregs.RegDL)
#define  rDH      (pMachState->Regs.byteregs.RegDH)
#define  rDX      (pMachState->Regs.wordregs.RegDX)

#define  rSI      (pMachState->Regs.wordregs.RegSI)
#define  rDI      (pMachState->Regs.wordregs.RegDI)
#define  rBP      (pMachState->Regs.wordregs.RegBP)
#define  rDS      (pMachState->Regs.wordregs.RegDS)
#define  rES      (pMachState->Regs.wordregs.RegES)
#define  rCS      (pMachState->Regs.wordregs.RegCS)
#define  rIP      (pMachState->Regs.wordregs.RegIP)
#define  rSS      (pMachState->Regs.wordregs.RegSS)
#define  rSP      (pMachState->Regs.wordregs.RegSP)

#define  rFlags   (pMachState->Regs.wordregs.RegFlags)

#define  fCF      ((rFlags & CARRY_FLAG_MASK)?1:0)
#define  fPF      ((rFlags & PARITY_FLAG_MASK)?1:0)
#define  fAF      ((rFlags & AUX_FLAG_MASK)?1:0)
#define  fZF      ((rFlags & ZERO_FLAG_MASK)?1:0)
#define  fSF      ((rFlags & SIGN_FLAG_MASK)?1:0)
#define  fTF      ((rFlags & TRAP_FLAG_MASK)?1:0)
#define  fIF      ((rFlags & INT_FLAG_MASK)?1:0)
#define  fDF      ((rFlags & DIRECTION_FLAG_MASK)?1:0)
#define  fOF      ((rFlags & OVERFLOW_FLAG_MASK)?1:0)

#define  setCF    (rFlags |= CARRY_FLAG_MASK)
#define  setPF    (rFlags |= PARITY_FLAG_MASK)
#define  setAF    (rFlags |= AUX_FLAG_MASK)
#define  setZF    (rFlags |= ZERO_FLAG_MASK)
#define  setSF    (rFlags |= SIGN_FLAG_MASK)
#define  setTF    (rFlags |= TRAP_FLAG_MASK)
#define  setIF    (rFlags |= INT_FLAG_MASK)
#define  setDF    (rFlags |= DIRECTION_FLAG_MASK)
#define  setOF    (rFlags |= OVERFLOW_FLAG_MASK)

#define  clrCF    (rFlags &= (~CARRY_FLAG_MASK))
#define  clrPF    (rFlags &= (~PARITY_FLAG_MASK))
#define  clrAF    (rFlags &= (~AUX_FLAG_MASK))
#define  clrZF    (rFlags &= (~ZERO_FLAG_MASK))
#define  clrSF    (rFlags &= (~SIGN_FLAG_MASK))
#define  clrTF    (rFlags &= (~TRAP_FLAG_MASK))
#define  clrIF    (rFlags &= (~INT_FLAG_MASK))
#define  clrDF    (rFlags &= (~DIRECTION_FLAG_MASK))
#define  clrOF    (rFlags &= (~OVERFLOW_FLAG_MASK))

#else /* NEW1 */

typedef union
   {
   #ifndef BIG_ENDIAN
      struct { BYTE  RL, RH;  } BYTEREG;
      struct { WORD  RX, RX2; } WORDREG;
      struct { DWORD ERX;     } DWORDREG;
   #else
      struct { BYTE  RH, RL;  } BYTEREG;
      struct { WORD  RX2, RX; } WORDREG;
      struct { DWORD ERX;     } DWORDREG;
   #endif
   }  ALLREGS;

typedef struct
   {
   ALLREGS EAX, EBX, ECX, EDX;
   ALLREGS ESI, EDI, EBP, ESP;
   WORD    DS, ES, CS, SS, FS, GS;
   ALLREGS EIP;         /* or IP in 16 bits        */
   ALLREGS EFLAGS;      /* or FLAGS in 16 bits     */

   /* Eventually, some special segisters, like :   */
   /* DWORD CR0, CR1, CR2, CR3;                    */
   /* DWORD DR0, DR1, DR2, DR3, DR4, DR5, DR6, DR7;*/
   /* DWORD TR3, TR4, TR5, TR6, TR7;               */
   /* GDTR, IDTR, TR, LDTR                         */

   }  REGISTERS;

#define  rAL      (pMachState->Regs.EAX.BYTEREG.RL)
#define  rAH      (pMachState->Regs.EAX.BYTEREG.RH)
#define  rAX      (pMachState->Regs.EAX.WORDREG.RX)
#define  rEAX     (pMachState->Regs.EAX.DWORDREG.ERX)

#define  rBL      (pMachState->Regs.EBX.BYTEREG.RL)
#define  rBH      (pMachState->Regs.EBX.BYTEREG.RH)
#define  rBX      (pMachState->Regs.EBX.WORDREG.RX)
#define  rEBX     (pMachState->Regs.EBX.DWORDREG.ERX)

#define  rCL      (pMachState->Regs.ECX.BYTEREG.RL)
#define  rCH      (pMachState->Regs.ECX.BYTEREG.RH)
#define  rCX      (pMachState->Regs.ECX.WORDREG.RX)
#define  rECX     (pMachState->Regs.ECX.DWORDREG.ERX)

#define  rDL      (pMachState->Regs.EDX.BYTEREG.RL)
#define  rDH      (pMachState->Regs.EDX.BYTEREG.RH)
#define  rDX      (pMachState->Regs.EDX.WORDREG.RX)
#define  rEDX     (pMachState->Regs.EDX.DWORDREG.ERX)

#define  rSI      (pMachState->Regs.ESI.WORDREG.RX)
#define  rDI      (pMachState->Regs.EDI.WORDREG.RX)
#define  rBP      (pMachState->Regs.EBP.WORDREG.RX)
#define  rSP      (pMachState->Regs.ESP.WORDREG.RX)
#define  rESI     (pMachState->Regs.ESI.DWORDREG.ERX)
#define  rEDI     (pMachState->Regs.EDI.DWORDREG.ERX)
#define  rEBP     (pMachState->Regs.EBP.DWORDREG.ERX)
#define  rESP     (pMachState->Regs.ESP.DWORDREG.ERX)

#define  rDS      (pMachState->Regs.DS)
#define  rES      (pMachState->Regs.ES)
#define  rCS      (pMachState->Regs.CS)
#define  rSS      (pMachState->Regs.SS)
#define  rFS      (pMachState->Regs.FS)
#define  rGS      (pMachState->Regs.GS)

#define  rIP      (pMachState->Regs.EIP.WORDREG.RX)
#define  rEIP     (pMachState->Regs.EIP.DWORDREG.ERX)

#define  rFlags   (pMachState->Regs.EFLAGS.WORDREG.RX)
#define  rEFlags  (pMachState->Regs.EFLAGS.DWORDREG.ERX)

#define  fCF      ((rFlags & CARRY_FLAG_MASK)?1:0)
#define  fPF      ((rFlags & PARITY_FLAG_MASK)?1:0)
#define  fAF      ((rFlags & AUX_FLAG_MASK)?1:0)
#define  fZF      ((rFlags & ZERO_FLAG_MASK)?1:0)
#define  fSF      ((rFlags & SIGN_FLAG_MASK)?1:0)
#define  fTF      ((rFlags & TRAP_FLAG_MASK)?1:0)
#define  fIF      ((rFlags & INT_FLAG_MASK)?1:0)
#define  fDF      ((rFlags & DIRECTION_FLAG_MASK)?1:0)
#define  fOF      ((rFlags & OVERFLOW_FLAG_MASK)?1:0)

#define  setCF    (rFlags |= CARRY_FLAG_MASK)
#define  setPF    (rFlags |= PARITY_FLAG_MASK)
#define  setAF    (rFlags |= AUX_FLAG_MASK)
#define  setZF    (rFlags |= ZERO_FLAG_MASK)
#define  setSF    (rFlags |= SIGN_FLAG_MASK)
#define  setTF    (rFlags |= TRAP_FLAG_MASK)
#define  setIF    (rFlags |= INT_FLAG_MASK)
#define  setDF    (rFlags |= DIRECTION_FLAG_MASK)
#define  setOF    (rFlags |= OVERFLOW_FLAG_MASK)

#define  clrCF    (rFlags &= (~CARRY_FLAG_MASK))
#define  clrPF    (rFlags &= (~PARITY_FLAG_MASK))
#define  clrAF    (rFlags &= (~AUX_FLAG_MASK))
#define  clrZF    (rFlags &= (~ZERO_FLAG_MASK))
#define  clrSF    (rFlags &= (~SIGN_FLAG_MASK))
#define  clrTF    (rFlags &= (~TRAP_FLAG_MASK))
#define  clrIF    (rFlags &= (~INT_FLAG_MASK))
#define  clrDF    (rFlags &= (~DIRECTION_FLAG_MASK))
#define  clrOF    (rFlags &= (~OVERFLOW_FLAG_MASK))

#endif /* NEW1 */

struct            _MACHINE_STATE;
struct            _RETURN_DATA;
struct            _MEMORY_AREA;

typedef  struct   _MEMORY_AREA
   {
   WORD        Seg;        /* Real segment of memory area.                 */
   WORD        Off;        /* Real origin of memory area.                  */
   ULONG       LinAddr;    /* Precomputed linear address of memory area.   */
   BYTE        *pBuff;     /* Pointer to work buffer with copy from Seg:Off*/
   WORD        BuffSize;   /* Number of bytes in work buffer.              */
   BOOL        freeable;   /* !=0 if allowed to free this buffer(static?). */
   BOOL        FromFile;   /* !=0 if this is region loaded from file.      */
   ULONG       OffIntoFile;/* If FromFile, then offset into file of beg MA */
   BOOL        SizeExtended;/* Buffer already extended for searching past end */
   BOOL        ModifiedNotScanned; /* Buffer has been modified but not scanned */
   ULONG       ulFlags;    /* Memory area definition flags:                */
                           /* READABLE | WRITEABLE | EXECUTABLE.           */

   WORD        minOffWr;   /* Real offset of first modified byte in buffer.*/
   WORD        maxOffWr;   /* Real offset of last modified byte in buffer. */

   struct _MEMORY_AREA *pNextArea; /* Link to the next area (or 0 if last).        */
   } MEMORY_AREA;


/* Notes:
 * PQSize       - Size of prefetch queue in bytes to be emulated.
 *                Must be set by caller.
 * pPQBuff      - caller's allocated prefetch queue buffer, which is
 *                <PQSize> bytes long. <pPQBuff> == NULL if buffer is not
 *                allocated.
 *                1. Must be either set to NULL or:
 *                2. Must be allocated and every element of queue must be
 *                   set to -1.
 * PQIndex      - pPQBuff[PQIndex] - is the next byte to query from P.Q.
 *                After each successful query this index get incremented.
 *                <PQIndex> could be 0...PQSize-1.
 *                Must be set to 0...PQSize-1 by caller.
 * PQLinearAddr - Linear address of next byte in prefetch queue buffer.
 *                Must be set to 0 by caller first time. But because
 *                all elements of PQ buffer are -1 anyway - that would be
 *                enough to force prefetch queue refill without specific
 *                value of <PQLinearAddr>.
 */

typedef  struct   _MACHINE_STATE
   {
   REGISTERS   Regs;       /* All CPU registers, defining current state.   */
   int         PQSize;     /* Size of CPU prefetch queue to emulate or 0.  */
   int         *pPQBuff;   /* Caller' allocated prefetch queue buffer.     */
   int         PQIndex;    /* pPQBuff[PQIndex] - is the next byte to query */
   ULONG       PQLinearAddr; /* Linear address of first byte in a queue.   */
   //riad//
   BYTE        *_IVT;
   BYTE        *_CR_PSP;
   BYTE        *_CR_BIOSDA;

   MEMORY_AREA *pMemA;     /* Linked list of all supplied memory buffers.  */

   int         (*pfnINTxx)( /* Ptr to function - replacement for INT xx    */
                  struct _MACHINE_STATE  *pMachState,
                  struct _RETURN_DATA    *pRetData,
                  int                    IntNumber);
   int         (*pfnBreak)( /* Ptr to function - BreakCondition check.     */
                  struct _MACHINE_STATE  *pMachState,
                  struct _RETURN_DATA    *pRetData);
   int         (*pfnExecControl)( /* Ptr to function - execution control.  */
                  struct _MACHINE_STATE  *pMachState,
                  struct _RETURN_DATA    *pRetData);
   int         (*pfnWipingMemory)( /* Ptr to function - memory wiping.  */
                  struct _MACHINE_STATE  *pMachState,
                  struct _RETURN_DATA    *pRetData,
                  WORD                    wSeg,    /* Seg of wiping memory.*/
                  WORD                    wOff,    /* Off of wiping memory.*/
                  WORD                    wBytes   /* Bytes to wipe.       */
                  );
   BYTE        PrevOpcode; /* First byte of last successful instruction.   */
   BYTE        *pOp1;      /* Ptr to first  byte of current opcode.        */
   BYTE        *pOp2;      /* Ptr to second byte of current opcode.        */
   BYTE        *pOp3;      /* Ptr to third  byte of current opcode.        */
   BYTE        *pOp4;      /* Ptr to fourth byte of current opcode.        */
   BYTE        *pOp5;      /* Ptr to fifth  byte of current opcode.        */

   ULONG       maxops;     /* Number of instructions allowed to execute.   */

                           /* <ExecutionBroken> Initially 0. Set to
                            * TRUE upon CodeRunner termination. This flag
                            * will help CodeRunner to detect, that it's
                            * execution was resumed after some
                            * termination conditions (TRAP ?) were
                            * fixed. In this case (ops!=0) it won't
                            * execute BreakCondCheck and StepTrace
                            * before instruction. This flag will get
                            * reset at the end of switch() loop.
                            */
   BOOL        ExecutionBroken;

   ULONG       ulIgnore;   /* Ignore illegal action flags:                 */
                           /* If bit is set, action ignored and execution  */
                           /* will continue from the next instruction.     */
                           /* Note: EXECUTE_TRAP and USER_ABORT will cause */
                           /* exit regardless of <ulIgnore> state.         */
                           /* Some other data could be added later.        */

                           /* These flags will be used only for the next
                            * single instruction. Usually this == 0.
                            */
   ULONG       ulIgnoreNextOpcode;

   BOOL        INT01Pending;

   MEMORY_AREA *pPrevIFetchMA;/* This is a cache that stores the memory    */
                            /* area where the last instruction was fetched */
                            /* from.                                       */
   MEMORY_AREA *pPrevDFetchMA;/* This is a cache that stores the memory    */
                            /* area resulting from the last memory are     */
                            /* lookup. Ifectchs are handled differently.   */
   } MACHINE_STATE;

typedef  struct   _RETURN_DATA
   {
   ULONG       ops;        /* Total number of executed instructions.       */
   WORD        SegTrap;    /* Access attempt to this address caused TRAP.  */
   WORD        OffTrap;    /* Access attempt to this address caused TRAP.  */
   WORD        PrevCS;     /* Real CS of last successful instruction.      */
   WORD        PrevIP;     /* Real IP of last successful instruction.      */
   ULONG       LMLinearAddr;/*Linear address of last memory modification   */
   ULONG       ulRetCode;  /* Bits of possible return causes:              */
                           /* - Attempt to read from non-readable memory.  */
                           /* - Attempt to write to non-writeable memory.  */
                           /* - Attempt to execute from non-executable area*/
                           /* - Attempt to execute unsupported instruction.*/
                           /* - Attempt to execute <maxops>+1 instruction. */
                           /* - Attempt to set TF.                         */
                           /* - Attempt to divide by 0.                    */
                           /* - Attempt to execute port I/O instruction.   */
                           /* - Attempt to execute INT xx instruction.     */
                           /* - IRET was executed.                         */
                           /* - Code modified itself.                      */
                           /* - Break condition is TRUE.                   */
                           /* - Invalid parameter supplied                 */
   } RETURN_DATA;


/*-------------------------------------------------------------------------*/
/* Definitions, useful for <CodeRunner>.                                   */
/*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*/
/* Definitions for external scan termination.                              */
/*-------------------------------------------------------------------------*/

#if WATC
extern void    IBMAVthreadSwitch(void);
extern int     external_scan_termination;
extern jmp_buf _cr_return_mark;
#define TEST_FOR_TERMINATION \
{ \
   IBMAVthreadSwitch(); \
   if(external_scan_termination) \
      longjmp(_cr_return_mark, ERROR_NON_TRAP_TERMINATION); \
}
#else
#define IBMAVthreadSwitch()
#define TEST_FOR_TERMINATION
#endif

#endif   /* _CR_DEFS_H_  */

/*-------------------------------------------------------------------------*/
/* Definitions for termination condition checking code and related record  */
/* keeping code.                                                           */
/*-------------------------------------------------------------------------*/
#define NUM_PREV_MEMORY_MODS  12
#define NUM_PREV_MEMORY_READS 16

//riad//
#define  PREFETCH_QUEUE_SIZE        8
#define  MAX_SIZE_SIGN              256
#define  X86PREFETCHLIST            6

/*------------------------------------------------------------------------*/


#ifdef CR_MOREFEATURES  //riad//

#define BACKOFFSET   2           // how much can we tolerate to separate sections
                                 // used also to merge sections (size of a word)
#define MINMAXSIZE   25          // max number of separate sections we can afford
                                 // if exeeded, then exit
#define MINMAXACTIVE 64          // max number of MODIFIED separate sections

/* those mode are used with the variable 'state' */

#define MODEIDLE     0x00
#define MODEPLUS     0x01
#define MODEJUMP     0x02

/* those numbers are used for the stop condition routine */

#define MIN_ACTIVE_SUSPICIOUS 16    // for optimization reasons, i took 2^(4)
#define MIN_ACTIVE_STOP       256

#define TEST_CODEDATA         0x01  // used with codata in _MinMaxTable

#ifndef __min
   #define __min  min
   #define __max  max
#endif
#ifndef min
   #define min(a,b) (a<=b)?a:b
   #define max(a,b) (a>=b)?a:b
#endif

typedef struct _minmax
{
   ULONG min;
   ULONG max;
   int   present;
   LONG  offsetfromfile;

   struct _minmax *next;

}  MINMAX;


// i put this include here because one needs the other

#include "codedata.h"

typedef struct _MIN_MAX_INDEX 
{
   MINMAX         minmax;     // this has to be initialized at the first place
   MINMAX         *curindex;  // for speed reasons, pointer to the current MINMAX

   jmp_stuff      *jmpstuff;  // a linked list of jump branchs and their state 
   jmp_stuff      *curjmp;    // current jump, used by update_machine()

   machine_stuff  *mstuff;    // double linked list for machine state structures
   machine_stuff  *curmstuff; // current machine state

   short          codata;           // tells us to save another state machine or no
   short          state;            // the current state of emulation (idle, plus, jump)
   short          dojump;           // used with the previous variable
   short          nsections;        // test only
   short          nmachines;        // number of vapeur machines
   short          njumps;           // number of different unconditionnal jumps

   ULONG          TotDiffInst;      // well, wehen -wow switch is on, this is used
                                    // It's the total amount of diff inst for all state
                                    // machines

   int            _OpcodeSize;      // this is the opcode size counter
                                    // used to precisely determine the
                                    // min and max of executed sections
                                    // in _MinMaxTable.minmax

/* Total number of instructions executed                                   */
   ULONG       NumOfTotalInst;
/* Counter of instructions, without redundancy                             */
   WORD        NumOfDiffInst, NumOfLastDiffInst;
/* And by the way, the number of loops without redundancy                  */
   WORD        NumOfDiffLoops;
/* The actual number of active instructions in the main loop               */
   WORD        NumOfRealActiveInst;
/* The number of idle instructions not in main loop or something           */
   WORD        NumOfRealIdleInst;
/* The number of actual different decryptors bodies we found, and a flag   */
   WORD        NumOfDiffDecryptors, ChangedDecryptor;

/* Well, debug informations, to know what's the INT that caused the ABORT  */
   BYTE        interrupt1, interrupt2;

/* this is the approximative Linear address of the entry point             */
   ULONG       LinearOffsetOfEntry;
/* the absolute offset of this approximative entry point in the file       */
   ULONG       AbsoluteOffset;
/* used for the external executed sections (external of the file)          */
   LONG        OffsetFromFile;

/* Modified sections linked list, the current list, and the section number */
   MINMAX      *ulPrevMemoryMods;
   MINMAX      *ulPrevMemoryCurrent;
   short       ulsections;

   ULONG       RetCode;

}  MIN_MAX_INDEX;


/*-------------------------------------------------------------------------*/

#ifdef NEW1

typedef struct _CR_VARIOUS_VARIABLES
{
   MIN_MAX_INDEX _MinMaxTable;

   /* Linear address of previous 8 memory unique modifications. 
    * Array is initialized to (ULONG)(-1l).
    */
   //MINMAX    * _ulPrevMemoryMods;
   //MINMAX    * _ulPrevMemoryCurrent;
	int         _ReadInstructionFlag;
   ULONG       _ulPrevMemoryReads[NUM_PREV_MEMORY_READS];

   /* Ok, this variable tells us if we wanna decryptor far away
    * in the darkside of the force or no.
    */
   int         _Fulldecryption;


   /* Counter of instructions, modified executable memory area.
    */
   WORD        _ActiveInstructions;

   /* Counter of instructions, which aren't modified executable memory area.
    */
   WORD        _IdleInstructions;

   /* Counter of LOOP instructions being executed without any modifications
    * of executable memory.
    */
   WORD        _IdleLoops;

   /* Linear address of LOOP (or Jcond) instruction.
    */
   ULONG       _IdleLoopLinearAddr;

   /* Incremented if opcode fetched from linear address >= linear address of
    * previous fetch, otherwise reset. If it gets too big, we stop. FWIW this
    * happens with ASCII text.
    */
   WORD        _ActiveSincePartialMatch;

   /* Set whenever a readable memory region is succesfully looked up.
    * Initialized to -1l;
    */
   ULONG       _ulLastMRLA;

   /* Set whenever a readable memory region is succesfully looked up.
    * Initialized to -1l;
    */
   int         _LastMRFromFile;

   /* Flag set if we're processing a REP MOVS instruction. Memory
    * modifications involving only REP MOVS don't set the
    * active_instruction flag.
    */
   int         _InREP_MOVS;

   /* This is to fix the problems when this causes too many Read Traps
    */
   int         _InREP_LODS;

   /* Support for multiple prefetch queues. Flag indicating if something
    * has been modified within the prefetch queue.
    */
	int			_CodeModifPrefetch;

   /* This is set if an instruction appears to be an active instruction
    * decrypting a block of memory.
    */
   int         _ActiveInstructionFlag;

   /* Overall flag, showing, that given buffer contains possibly decrypted
    * body of polymorphic virus.
    */
   BYTE        _flagBufferDegarbled;

   /* 32 address and operande size override flag 
    */
   BYTE        rPOverrd;
 
   /* Used to know how many bytes are available in the first memory area
    * when we need to read a word or a double word 
    */
   int         _ByteLeft;


}  CR_VARIOUS_VARIABLES;

#endif /* NEW1 */

/*-------------------------------------------------------------------------*/

#endif   //riad//   

