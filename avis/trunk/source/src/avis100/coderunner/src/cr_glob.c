/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_GLOB.C                                                *
 *                                                                         *
 * Description:   INTeL 8086/88 interpreter.                               *
 *                                                                         *
 *                Global variables required by some functions.             *
 *                                                                         *
 *      Must be compiled with /dBIG_ENDIAN for big-endian machines.        *
 *      Must be compiled with /dAIX        for AIX.                        *
 *      Must be compiled with /dDOS        for DOS.                        *
 *      Must be compiled with /dOS2_16     for 16-bit OS2.                 *
 *      Must be compiled with /dOS2_32     for 32-bit OS2.                 *
 *                                                                         *
 ***************************************************************************/


#ifdef   GE_MACHINE
   #include "dirlevel.dir" /* To be used in GE machine.                    */

   #ifdef   DIR_LEVEL_0
   #include "cr_all.h"
   #endif

   #ifdef   DIR_LEVEL_1
   #include "..\cr_all.h"
   #endif
#else                      /* Case for IBM.                                */
   #include "cr_all.h"
#endif


/* Following is the global structure to be used by the caller of
 * <CodeRunner>.
 */

struct cr_ini _Default_ini  = {
/* Stop after this number of instructions, caused memory modification
 * were executed.
 */
   100000L, /*8192L,*/           /* MaxActiveInstructions                  */

/* Stop after this number of idle instructions were executed.
 * Idle = which are not modified executable memory.
 */
#ifdef CR_MOREFEATURES  //riad//
   7000,
#else
   1024,                         /* MaxIdleInstructions                    */
#endif
/* Stop after this number of idle LOOP instructions were executed.
 * Idle = during which executable memory wasn't modified.
 */
   64,   /*4,*/                  /* MaxIdleLoops                           */

/* Recover from no more than this number of write traps.
 */
#ifdef CR_MOREFEATURES  //riad//
   128, /*12,*/
#else
   16,                           /* MaxRWTrapsToRecover                    */
#endif
/* Recover from no more than this number of execute traps.
 */
   64, /*16,*/                   /* MaxExecTrapsToRecover                  */

/* This number of bytes will be allocated around execute trap address point.
 */
   2048,                         /* ExeTrapHalfBuffsize                    */
   
/* If one of following conditions will happen during CodeRunner
 * execution - ignore 'em and continue. Although non-zero trace flag
 * will cause INT callback to be executed.
 */
      PORT_IO              |     /* ulFlagsIgnoreConditions                */
      CODE_SELF_MODIFIED   |
      TRACE_FLAG_SET       |
      CANNOT_EXECUTE_INT   |
      HLT_EXECUTED,


/* Allocate +/- this number of bytes around write trap point in order
 * to recover.
 */
   512,                          /* RWTrapHalfBufSize                      */

/* Maximal value allowed for CX register while executing of LOOP
 * instruction. If CX will be above of that - it will be resetted to 1.
 * It will be done to skip "time" waste loops of some viruses and to
 * decrease time of interpreting of legitimate programs.
 */
   0xFFFF, /*0x7FFF,*/           /* MaxCXForLoops                          */

/* Maximal value allowed for number of instructions without a tranfer of
 * control to a linear address less than the address of the transfer of
 * control instruction.
 */
   0x0800,                       /* MaxIncreasingExecLA                    */

/* Maximal value allowed for number of active instructions since a
 * partial signature match was found. This is to terminate shortly after a
 * partial match is found, but not immediately.
 */
   0x0100                        /* MaxActiveSincePartialMatch             */

#ifdef CR_MOREFEAUTRES  //riad//
/* This is the max number of opcodes we allow the coderunner to execute    */
   ,0x00FFFFFF
/* MaxIdleBeforeSkip, which means the max idle before invalidating a jump  */
   ,30000
/* MaxDeepness, which means we won't rerun a jump in the other side more   */
   ,0
#endif
};

struct cr_ini _Default_ini_short  = {
   30000L,                       /* MaxActiveInstructions                  */
   1024,                         /* MaxIdleInstructions                    */
   4,                            /* MaxIdleLoops                           */
   16,                           /* MaxRWTrapsToRecover                    */
   16,                           /* MaxExecTrapsToRecover                  */
   2048,                         /* ExeTrapHalfBuffsize                    */
      PORT_IO              |     /* ulFlagsIgnoreConditions                */
      CODE_SELF_MODIFIED   |
      TRACE_FLAG_SET       |
      CANNOT_EXECUTE_INT   |
      HLT_EXECUTED,
   512,                          /* RWTrapHalfBufSize                      */
   0xFFFF, /*0x7FFF,*/           /* MaxCXForLoops                          */
   0x0800,                       /* MaxIncreasingExecLA                    */
   0x0100                        /* MaxActiveSincePartialMatch             */

/* This is the max number of opcodes we allow the coderunner to execute    */
   ,0x00FFFFFF
/* MaxIdleBeforeSkip, which means the max idle before invalidating a jump  */
   ,30000
/* MaxDeepness, which means we won't rerun a jump in the other side more   */
   ,0
};


/* Emulated IVT: first VECTORS_IN_IVT+1 vectors. Value of each vector
 * represents normalized pointer to INT 3 vector, which is
 * 0xCF == IRET. Must be array of BYTE-s (not ULONGS)
 * to be able to work on any hardware.
 * So every vector pointing on IRET. Changing any vector does not affect
 * fact, that all others will still be point to IRET. No extra 0xCF-bytes
 * required to be allocated.
 *
 * The fill pattern is so that we can quickly initialize (with memcpy()s) the
 * _IVT before working on a new file, without requiring an entire static copy
 * of the initial IVT.
 */
   BYTE        _IVT_FILL_PATTERN[32] = {
      0x09,0xCF,0x10,0xF3, 0x09,0xCF,0x10,0xF3, 0x09,0xCF,0x10,0xF3, 0x09,0xCF,0x10,0xF3,
      0x09,0xCF,0x10,0xF3, 0x09,0xCF,0x10,0xF3, 0x09,0xCF,0x10,0xF3, 0x09,0xCF,0x10,0xF3
      };
   //BYTE        *_IVT = NULL;


/* Emulated PSP.
 */
   BYTE        _CR_PSP_FILL_PATTERN[0x80] = {
/* OFFSET:  00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F */

/* 0000 */  0xCD, 0x20, 0xC0, 0x9F, 0x00, 0x9A, 0xEE, 0xFE, 0x1D, 0xF0, 0xED, 0x04, 0xBD, 0x1B, 0xED, 0x01,
/* 0010 */  0xEE, 0x0A, 0x0A, 0x07, 0xEE, 0x0A, 0xBD, 0x1B, 0xD1, 0xD1, 0xD1, 0xD0, 0xD2, 0x00, 0xFF, 0xFF,
/* 0020 */  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC1, 0x2B, 0xD0, 0xFF,
/* 0030 */  0xBD, 0x1B, 0x14, 0x00, 0x18, 0x00, 0xDA, 0x2B, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00,
/* 0040 */  0x14, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0050 */  0xCD, 0x21, 0xCB, 0x72, 0x6E, 0x6C, 0x3A, 0x20, 0x6E, 0x6F, 0x74, 0x20, 0x00, 0x20, 0x20, 0x20,
/* 0060 */  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x20, 0x20,
/* 0070 */  0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
      };
   //BYTE        *_CR_PSP = NULL;

/* BIOS Data Area. */
   BYTE        _CR_BIOSDA_FILL_PATTERN[256] = {
/* OFFSET:      00    01    02    03    04    05    06    07    08    09    0A    0B    0C    0D    0E    0F */
/* 0040:0000 */ 0xF8, 0x03, 0xF8, 0x02, 0x00, 0x00, 0x00, 0x00, 0x78, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:0010 */ 0x63, 0x44, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x1E, 0x00, 0x00, 0x00,
/* 0040:0020 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:0030 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x80,
/* 0040:0040 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x50, 0x00, 0x00, 0x10, 0x00, 0x00,
/* 0040:0050 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:0060 */ 0x0E, 0x0D, 0x00, 0xD4, 0x03, 0x29, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:0070 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x14, 0x14, 0x14, 0x00, 0x01, 0x01, 0x01, 0x01,
/* 0040:0080 */ 0x1E, 0x00, 0x3E, 0x00, 0x18, 0x10, 0x00, 0x60, 0x09, 0x11, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x77,
/* 0040:0090 */ 0x17, 0x02, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:00A0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:00B0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:00C0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:00D0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:00E0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* 0040:00F0 */ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
      };
   //BYTE        *_CR_BIOSDA = NULL;

#ifdef NEW1
/* General Various Variables structure holder
 */
   CR_VARIOUS_VARIABLES crvv;

#else
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

#ifdef CR_MOREFEATURES  //riad//

/* Index Table, keeping track of some events
 */
   MIN_MAX_INDEX _MinMaxTable;

/* Ok, this variable tells us if we wanna decryptor far away
 * in the darkside of the force or no.
 */
   int         _Fulldecryption = 0;
#endif

/* Linear address of previous 8 memory unique modifications. Array is
 * initialized to (ULONG)(-1l).
 */
#ifdef CR_MOREFEATURES2 //riad//
   // commented because it's now in _MinMaxTable structure
   MINMAX      *_ulPrevMemoryMods = NULL;
   MINMAX      *_ulPrevMemoryCurrent = NULL;
   int         _ReadInstructionFlag = 0;
   /* codedatat stuff */
   ULONG       _ulPrevMemoryReads[NUM_PREV_MEMORY_READS];
//#define _ulPrevMemoryMods     crvv._ulPrevMemoryMods
//#define _ulPrevMemoryCurrent  crvv._ulPrevMemoryCurrent

#else

#endif

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
   int         _InREP_MOVS = 0;
   int         _InREP_LODS = 0;     //riad//
   int         _CodeModifPrefetch;  //riad//

/* This is set if an instruction appears to be an active instruction
 * decrypting a block of memory.
 */
   int         _ActiveInstructionFlag = 0;

/* Overall flag, showing, that given buffer contains possibly decrypted
 * body of polymorphic virus.
 */
   BYTE        _flagBufferDegarbled;

/* Used by CodeRunner to cleanly exit on demand.
 */
   //jmp_buf     _cr_return_mark;

#endif
   
/* riad */

/* A list holding different prefetch modes we want to emulate
 */
   int   x86_prefetch_list[X86PREFETCHLIST] =
      {
      0,    /* Pentiums                            */
      //8,  /* 8486-8386 it's supposed to be 32/16 */
      //6,  /* 8086-8186-8286                      */
      -1    /* end                                 */
      };
/* Index used to locate the current prefetch mode
 */
   int   x86_prefetch_index;
