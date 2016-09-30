/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_FSCAN.C                                               *
 *                                                                         *
 * Description:   INTeL 8086/88 interpreter.                               *
 *                                                                         *
 *                Caller for CodeRunner.                                   *
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
#else
   #ifdef CR_DO_SCANNER //riad//
   #include "scanhelp.pro"
   #endif

   #include "cr_all.h"
   #include "cr_fscan.pro"
#endif

#ifdef CR_DEBUG
#if WATC
#define INIT_CR_DBGF NULL
char              *cr_dbgf_name = "sys:\\system\\cr_debug.out";
#else
#define INIT_CR_DBGF stdout
char              *cr_dbgf_name = "cr_debug.out";
#endif
FILE              *cr_dbgf = INIT_CR_DBGF;
int               cr_hd = 0;      /* Set to 1 to turn on coderunner output to file. */
int               cr_itrace = 0;  /* Set to 1 to turn on coderunner instruction trace. */
extern ULONG      QNO_PrevTempIP; /* Set to -1 to initialize */
#endif

#ifndef CR_DO_SCANNER   //riad//
extern int        debug_option;
#endif

#ifdef CR_MOREFEATURES  //riad//
   extern MINMAX     *is_in_table(ULONG);
   extern MINMAX     *get_in_table(void);
   #ifdef MYDEBUG
      #include <stdio.h>
   #endif
#endif
#ifdef CR_MOREFEATURES  //riad//
   WORD           total_rw_traps;
   WORD           total_exec_traps;
#endif

/* Definitions.
 */



//riad//
// I grabbed this stuff down here from john evanson hack on coderunner 
// to do the same thing I am currently attempting to do in somwwhat 
// a very different and yet-not-known working method

/* jce */
/*
   The following is machine code to exercise a bunch of
   dos interrupts that viruses might hook.  This bit of
   code(&data) is patched into virtual memory and executed 
   when a TSR call is found in the code under examination.
   The source can be found in testtsr.asm, which can be
   assembled, the bytes extracted and then placed here.
   (ugly, but will do for now) 
*/
unsigned char tsr_test_code[] = {
0x0E, 0x1F, 0xB8, 0x00, 0x4B, 0xBA, 0x4A, 0x01,
0xCD, 0x21, 0xB8, 0x00, 0x4B, 0xBA, 0x53, 0x01, 
0xCD, 0x21, 0xB8, 0x02, 0x3D, 0xBA, 0x4A, 0x01,
0xCD, 0x21, 0xB8, 0x02, 0x3D, 0xBA, 0x53, 0x01,
0xCD, 0x21, 0xB4, 0x3E, 0xBB, 0x12, 0x00, 0xCD,
0x21, 0xB8, 0x00, 0x4E, 0xB9, 0x07, 0x03, 0xBA,
0x5C, 0x01, 0xCD, 0x21, 0xB4, 0x4F, 0xCD, 0x21,
0xB4, 0x11, 0xCD, 0x21, 0xB4, 0x12, 0xCD, 0x21,
0xB4, 0x0F, 0xCD, 0x21, 0xB4, 0x10, 0xCD, 0x21,
0xCD, 0x20, 0x74, 0x65, 0x73, 0x74, 0x2E, 0x63,
0x6F, 0x6D, 0x00, 0x74, 0x65, 0x73, 0x74, 0x2E,
0x65, 0x78, 0x65, 0x00, 0x2A, 0x2E, 0x2A, 0x00,
};
/* end jce */

int            tested_tsr = 0;      /* jce */
int            dont_test_tsr = 0;   /* riad */
int	timeout=0; //Fred
int starttime=0;

int   filerun_and_scan (
         unsigned int       scan_length,           /* Buffer size.         */
         unsigned int       ept_offset_into_buf,   /* Index of entry point into buffer. */
         char               *filename,             /* Not used by CR.      */
         unsigned short     is_exe_type,           /* Not used by CR.      */
         unsigned short     ExeFile,               /* TRUE => EXE type file*/
         WORD               StartSeg,              /* Starting CS. 0x3000? */
         WORD               FileHdrSize,           /* 0 for COM files.     */
         ULONG              FileSize,              /* Full file size.      */
         WORD              *pReloItems,            /* Exe reloc. pairs     */
         int              (*pfnReadFileRegion)(    /* Ptr to read-file fn. */
                              ULONG abs_offset,
                              BYTE  *buffer,
                              WORD  bytes_to_read
                              ),
         long               base,                  /* Not used by CR.      */
         int                object_type,           /* Not used by CR.      */
         byte               *buffer_to_be_scanned, /* Buffer itself.       */
         char               *vp_name,              /* Not used by CR.      */
         unsigned short     inside_compressed,     /* Not used by CR.      */
         unsigned short int initdses,              /* Init. val. of DS, ES */
         unsigned short int initcs,                /* Init. val. of CS reg */
         unsigned short int initip,                /* Init. val. of IP reg */
         unsigned short int initss,                /* Init. val. of SS reg */
         unsigned short int initsp,                /* Init. val. of SP reg */
         unsigned short     scalpel_mode,          /* Scan more buffers if */
         ULONG              saved_epoint           /* ept offset into file */
         )
{
/*-------------------------------------------------------------------------*/
   int            i;
   MEMORY_AREA    *maList = 0;            /* Start of all MA linked list.  */
   MEMORY_AREA    *RetMA;                 /* Used only as a return code.   */
   //riad//
   machine_stuff  *theMachine;            /* first element of the list     */
   MACHINE_STATE  *ms;                    /* Machine state structure.      */
   RETURN_DATA    *rd;                    /* Return data.                  */
   //int          PQBuff[PREFETCH_QUEUE_SIZE];
   int            *PQBuff;                /* Prefetch queue buffer.        */
#ifndef CR_MOREFEATURES
   WORD           total_rw_traps    = 0;
   WORD           total_exec_traps  = 0;
#endif
   int            RetCode = SUCCESS;
   int            TrapRecovered;

/*-------------------------------------------------------------------------*/


   /* Function consists of 2 parts:
    *
    * 1. Call <CodeRunner> for passed executable buffer in attempt to
    *    degarble it for polymorphic viruses.
    *
    * 2. Call <scanworkbuf>.
    *
    */

#ifdef   CR_DEBUG
#ifdef   HEX_DUMPER
   /* This part will allocate buffer to keep original contents of buffer,
    * passed to CodeRunner.
    */
/*-------------------------------------------------------------------------*/
   BYTE           *org_buf = 0;
/*-------------------------------------------------------------------------*/
#endif

#ifndef CR_DO_SCANNER   //riad//
#ifdef CR_DEBUG
   if(debug_option >= 1) cr_hd = 1;
   if(debug_option >= 2) cr_itrace = 1;
#endif 
#endif

#ifdef CR_DBGTF
    if (cr_hd)
    {
       cr_dbgf = fopen(cr_dbgf_name, "a");
       if (cr_dbgf == NULL)
          cr_dbgf = INIT_CR_DBGF;
       fprintf(cr_dbgf, "Starting interpretation of (%s)\n", filename);
    }
#endif
#if 0
#ifdef CR_DEBUG
    if (cr_hd) fprintf(cr_dbgf, "filerun_and_scan: scan_length=%u, ept_offset_into_buf=%u\n",
                       scan_length, ept_offset_into_buf);
    if (cr_hd) fprintf(cr_dbgf, "filename=%s, is_exe_type=%d, ExeFile=%d, StartSeg=%04X\n",
                       filename, is_exe_type, ExeFile, StartSeg);
    if (cr_hd) fprintf(cr_dbgf, "FileHdrSize=%u, FileSize=%lu, pReloItems=%08lX\n",
                       FileHdrSize, FileSize, (unsigned long)pReloItems);
    if (cr_hd) fprintf(cr_dbgf, "pfnReadFileRegion=%08lX, base=%lu, object_type=%d\n",
                       pfnReadFileRegion, base, object_type);
    if (cr_hd) fprintf(cr_dbgf, "buffer_to_be_scanned=%08lX, vp_name=%s, inside_compressed=%d\n",
                       (unsigned long) buffer_to_be_scanned, vp_name, inside_compressed);
    if (cr_hd) fprintf(cr_dbgf, "initdses=%04X, initcs=%04X, initip=%04X, initss=%04X, initsp=%04X\n",
                       initdses, initcs, initip, initss, initsp);
#endif
#endif
#ifdef CR_DEBUG
    if (cr_hd)
    {
    int i;
    if (cr_hd) fprintf(cr_dbgf, "Epoint: ");
    for (i=0; i<24; ++i)
       if (cr_hd) fprintf(cr_dbgf, "%02X ",
                          buffer_to_be_scanned[ept_offset_into_buf+i]);
    if (cr_hd) fprintf(cr_dbgf, "\n");
    }
#endif
#ifdef HEX_DUMPER
   org_buf = malloc (scan_length);

   if (org_buf)
      memcpy (org_buf, buffer_to_be_scanned, scan_length);
   else
      if (cr_hd) fprintf (cr_dbgf, "\n\x7\x7\x7ERROR: Cannot allocate <org_buf>!\n");
#endif
#endif

#if WATC
   {
   int rc;
   if ((rc = setjmp(_cr_return_mark)) != 0)
   {
      RetCode = ERROR_NON_TRAP_TERMINATION;
      goto done_filerun_and_scan;
   }
   }
#endif

/*--------------------------------------------------------------------------------*/

   //riad//
   // This is the first memory allocation we do to get a frech new
   // machine state with it's compagnion retdata
   if((theMachine=update_machine_state(NULL, NULL, 0, 0)) == NULL)   
   { 
      RetCode = ERROR_NOT_ENOUGH_MEMORY; 
      goto done_filerun_and_scan; 
   }
   else
   {
      ms = &(theMachine->ms);
      rd = &(theMachine->rd);
      PQBuff = theMachine->PQBuff;
   }

   /* 1-st part: prepare and run <CodeRunner>.
    */

   /* Reset all elements in the prefetch queue buffer.
    */
   for (i=0; i<PREFETCH_QUEUE_SIZE; i++)
      PQBuff[i] = -1;

   /* Initialize necessary structures and variables to call <CodeRunner>.
    */

   /* Reset these counters. They will be used to determine time to
    * stop <CodeRunner>. These variables will be checked/modified by
    * <BreakConditionCheck> routine.
    */

   _ActiveInstructionFlag     = 0;
   _ActiveInstructions        = 0;
   _IdleInstructions          = 0;
   _IdleLoops                 = 0;
   _IdleLoopLinearAddr        = 0;
   _flagBufferDegarbled       = FALSE;
#if defined (CR_DEBUG)
   QNO_PrevTempIP = -1l;
#endif

#ifdef CR_MOREFEATURES  //riad//
   _MinMaxTable.state               = MODEJUMP;             // for the first instruction
   _MinMaxTable.dojump              = MODEIDLE;
   _MinMaxTable.curindex            = &_MinMaxTable.minmax;  // the first element
   _MinMaxTable.minmax.min          = (ULONG)-1;
   _MinMaxTable.minmax.max          = (ULONG)-1;
   _MinMaxTable.minmax.next         = NULL;
   _MinMaxTable.minmax.present      = -1;
   _MinMaxTable.minmax.offsetfromfile = -1L;
   _MinMaxTable.nsections           = 1;
   _MinMaxTable.nmachines           = 1;
   _MinMaxTable.njumps              = 0;
   _MinMaxTable.NumOfLastDiffInst   = 0;
   _MinMaxTable.NumOfDiffInst       = 0;
   _MinMaxTable.NumOfDiffLoops      = 0;
   _MinMaxTable.NumOfRealActiveInst = 0;
   _MinMaxTable.NumOfTotalInst      = 0;
   _MinMaxTable.NumOfRealIdleInst   = 0;
   _MinMaxTable.NumOfDiffDecryptors = 0;  // test only
   _MinMaxTable.ChangedDecryptor    = 1;  // test only
   _MinMaxTable.jmpstuff            = NULL;
   _MinMaxTable.mstuff              = theMachine;
   _MinMaxTable.curmstuff           = theMachine;
   _MinMaxTable.curjmp              = NULL;
   _MinMaxTable.codata              = 0;
   _MinMaxTable.TotDiffInst         = 0;
#ifndef CR_DO_SCANNER   //riad//
   total_rw_traps    = 0;
   total_exec_traps  = 0;
#endif
   if(1)
   {
      MINMAX *cur = &_MinMaxTable.minmax;
      while(cur != NULL) { cur->present = -1; cur = cur->next; }
   }
   //for(i=0;i<MINMAXSIZE;i++) _MinMaxTable.minmax[i].present = -1;
#endif
#ifdef CR_MOREFEATURES2    //riad//
   //for(i=0;i<MINMAXACTIVE;++i) 
   //{
   //   _ulPrevMemoryMods[i].present = -1;
   //   _ulPrevMemoryMods[i].offsetfromfile = 0;
   //}
   _MinMaxTable.ulPrevMemoryMods    = NULL;
   _MinMaxTable.ulPrevMemoryCurrent = NULL;
   _MinMaxTable.ulsections          = 0;
   _ReadInstructionFlag             = 0;
#else
   for (i = 0; i< NUM_PREV_MEMORY_MODS; ++i)  _ulPrevMemoryMods[i] = (ULONG)(-1l);
   for (i = 0; i< NUM_PREV_MEMORY_READS; ++i) _ulPrevMemoryReads[i] = (ULONG)(-4l);
   _FirstMemModFromFile = 1;
   _ulPrevOpcodeLA = -1l;
   _ulIncreasingExecutionAddr = 0l;
#endif

#ifdef CR_MOREFEATURES  //riad//
   //riad//
   // Codedata stuff
   // Before running this beauty, i want to get the absolute offset of
   // the entry point in the filename
   if(1)
   {
      int retcode;
      WORD  _segentry = initcs, offentry = initip;



      WORD  HalfBuffSize = _Default_ini.RWTrapHalfBuffSize;
      WORD  NewBuffSeg, NewBuffOff;
      WORD  NewBuffSize;
      ULONG ulAbsOffsetOfBufferStart;

      retcode = 
              calculate_file_region (
                  ExeFile,             /* <- TRUE for EXE type file. */
                  FileHdrSize,         /* <- size of EXE file hdr.   */
                  FileSize,            /* <- full filesize.          */
                  StartSeg,            /* <- 0x3000?                 */
                  initcs,              /* <- Segment of trap point.  */
                  initip,              /* <- Offset of trap point.   */
                  HalfBuffSize,        /* <- Delta bytes from trap.  */
                  &NewBuffSeg,         /* ->                         */
                  &NewBuffOff,         /* ->                         */
                  &ulAbsOffsetOfBufferStart, /* ->                   */
                  &NewBuffSize         /* ->                         */
                  );

      if (retcode == SUCCESS) _MinMaxTable.AbsoluteOffset = (ULONG)(-1);

      // now NewSeg and NewOff are gonna be used in connection with
      // ulAbsOffsetOfBufferStart
      _MinMaxTable.AbsoluteOffset      = ulAbsOffsetOfBufferStart;
      _MinMaxTable.LinearOffsetOfEntry = LinearAddr(NewBuffSeg, NewBuffOff);
   }
#endif   //riad//

   _ulLastMRLA = (ULONG)(-1l);
   _LastMRFromFile = 0;
   _ActiveSincePartialMatch = 0;

   _InREP_MOVS = 0;
   _InREP_LODS = 0;
   _CodeModifPrefetch = 0;

   //riad//
   theMachine->previous  = NULL;
   theMachine->next      = NULL;

   //_ulPrevMemoryMods    = NULL;
   //_ulPrevMemoryCurrent = NULL;
   _ReadInstructionFlag = 0;
   
/*-------------------------------------------------------------------*/

   memset(ms, 0, sizeof(MACHINE_STATE));

#ifdef NEW1
   ms->Regs.EAX.DWORDREG.ERX   = 0;
   ms->Regs.EBX.DWORDREG.ERX   = 0;
   ms->Regs.ECX.DWORDREG.ERX   = 0;
   ms->Regs.EDX.DWORDREG.ERX   = 0;

   ms->Regs.DS                 = initdses;
   ms->Regs.ES                 = initdses;
   ms->Regs.CS                 = initcs;
   ms->Regs.SS                 = initss;

   ms->Regs.EIP.DWORDREG.ERX   = 0;
   ms->Regs.EIP.WORDREG.RX     = initip;

   ms->Regs.ESP.DWORDREG.ERX   = 0;
   ms->Regs.ESP.WORDREG.RX     = initsp;
   ms->Regs.ESI.DWORDREG.ERX   = 0;
   ms->Regs.ESI.WORDREG.RX     = 0x100;
   ms->Regs.EDI.DWORDREG.ERX   = 0;
   ms->Regs.EDI.WORDREG.RX     = 0x100;
   ms->Regs.EBP.DWORDREG.ERX   = 0;
   ms->Regs.EBP.WORDREG.RX     = 0;

   ms->Regs.EFLAGS.DWORDREG.ERX= 0;
   ms->Regs.EFLAGS.WORDREG.RX  = INT_FLAG_MASK;
#else
   ms->Regs.wordregs.RegAX     = 0;
   ms->Regs.wordregs.RegBX     = 0;
   ms->Regs.wordregs.RegCX     = 0;
   ms->Regs.wordregs.RegDX     = 0;
   ms->Regs.wordregs.RegDS     = initdses;
   ms->Regs.wordregs.RegES     = initdses;
   ms->Regs.wordregs.RegCS     = initcs;
   ms->Regs.wordregs.RegIP     = initip;
   ms->Regs.wordregs.RegSS     = initss;
   ms->Regs.wordregs.RegSP     = initsp;
   ms->Regs.wordregs.RegSI     = 0x100;
   ms->Regs.wordregs.RegDI     = 0x100;
   ms->Regs.wordregs.RegBP     = 0;
   ms->Regs.wordregs.RegFlags  = INT_FLAG_MASK;
#endif /* NEW1 */

   //riad//
   //ms->PQSize                  = PREFETCH_QUEUE_SIZE;
   ms->PQSize                  = x86_prefetch_list[x86_prefetch_index];
   ms->pPQBuff                 = PQBuff;
   ms->PQIndex                 = 0;
   ms->PQLinearAddr            = 0;

/* ms->pMemA                   = maList;            Start of linked list.
 * This element get initialized further after MA for executable buffer
 * will get allocated.
 */
   /* riad */
   if(com_options & OPT_DECRYPT)
   {
      // don't bother test the TSR stuff in this situation
      tested_tsr               = 1;
      dont_test_tsr            = 1;
      // short version (old) of IntEmulator
      ms->pfnINTxx             = IntEmulator_Short;
   }
   else
   {
      ms->pfnINTxx             = IntEmulator_Longer;
   }

   ms->pfnBreak                = BreakConditionCheck;
   ms->pfnExecControl          = ExecutionControl;

/* We don't need to waste a time to reset some vars. below. They will be
 * initialized by CodeRunner itself while CodeRunning...
 *
 * ms->PrevOpcode              = 0xFF;
 * ms->pOp1                    = 0;
 * ms->pOp2                    = 0;
 * ms->pOp3                    = 0;
 * ms->pOp4                    = 0;
 * ms->pOp5                    = 0;
 */
#ifdef CR_MOREFEATURES  //riad//
   ms->maxops                  = _Default_ini.MaxOpcodes;
#else
   ms->maxops                  = 0x00020000; /* Other condition will work. */
#endif
   ms->ExecutionBroken         = FALSE;       /* Must be reset to 0.        */
   ms->ulIgnore                = _Default_ini.ulFlagsIgnoreConditions; /* Ignore flags. */
   ms->ulIgnoreNextOpcode      = 0;           /* Must be resetted.          */
   ms->INT01Pending            = 0;
   ms->pPrevIFetchMA           = 0l;
   ms->pPrevDFetchMA           = 0l;

   /* Set only .ops and last memory modification for <rd> structure.
    */
   rd->ops                     = 0l;          /* <CodeRunner> only inc. it.*/
   rd->LMLinearAddr            = 0xFFFFFFFF;

   /* Allocate memory area for the main code buffer. Code Buffer itself
    * already been allocated so we're passing a ready pointer.
    *
    * Requested CS:
    *
    *
    *
    * +- <- CS (starting of buffer with binary code)
    * |
    * |  ((ept_offset_into_buf / 16) + 1) == how many paragraphs fits here.
    * |
    * |- <- IP (ept_offset_into_buf)
    * |
    * |
    * |
    * |   buffer
    * |
    * |
    * +-
    *
    * Requested CS = initcs - ((ept_offset_into_buf / 16) + 1)
    * Requested IP = initip + (16 - (ept_offset_into_buf % 16))
    */

   if (
      AllocateNextMemoryArea (
         &maList,                      /* &Ptr to starting pointer of list.*/
         (WORD)(initcs - ((ept_offset_into_buf / 16) + 1)),   /* MA segment.       */
         (WORD)(initip + (16 - (ept_offset_into_buf % 16))),  /* MA starting offset*/
         0,   /*buffer_to_be_scanned,*//* Ptr to buff or ZERO if not allocated*/
         (WORD)scan_length,            /* Size of buff (allocated or not). */
         FALSE,                        /* TRUE == allowed to free.         */
         TRUE,                         /* Loaded from file we're checking  */
         base,
         READABLE | WRITEABLE | EXECUTABLE   /* MA Flags.                  */
         ) != (MEMORY_AREA *)0
      )
      {
      // codedata stuff
      memcpy(maList->pBuff, buffer_to_be_scanned, (WORD)scan_length);
      ms->pMemA = maList;               /* Start of linked list.            */

      /* Run CodeRunner only if MEMORY_AREA was successfully allocated.
       */

      /* Now initialize variables used by scanworkbuf() so I don't have to
       * pass them down ten levels deep.
       */
      /*
      _scanworkbuf_vars.scalpel_mode = scalpel_mode;
      _scanworkbuf_vars.filename = filename;
      _scanworkbuf_vars.is_exe_type = is_exe_type;
      _scanworkbuf_vars.object_type = object_type;
      _scanworkbuf_vars.vp_name = vp_name;
      _scanworkbuf_vars.inside_compressed = inside_compressed;
      _scanworkbuf_vars.FileSize = FileSize;
      _scanworkbuf_vars.saved_epoint = saved_epoint;
      */

      /* Now allocate memory area for PSP emulation.
       * This area is not executable.
       * "If you want to run PSP you're not authorized to be executed at
       *  all!".
       */
      Init_PSP(ms);
      if (
         !AllocateNextMemoryArea (
            &maList,                      /* &Ptr to starting pointer of list.*/
            initdses,                     /* MA segment.                      */
            0,                            /* MA starting offset               */
            ms->_CR_PSP,                  /* Ptr to buff or ZERO if not allocated*/
            0x100,                        /* Size of buff (allocated or not). */
            TRUE,//FALSE,                 /* TRUE == allowed to free.         */
            FALSE,                        /* Not loaded from a file.          */
            0l,                           /* Not used.                        */
            // codedata stuff, I added EXECUTABLE (APR797DR.COM)              */
            READABLE | WRITEABLE | EXECUTABLE/* MA Flags.                     */
            )
         )
         { RetCode = ERROR_NOT_ENOUGH_MEMORY; goto done_filerun_and_scan; }

      /* Now allocate memory area for interrupt vector table emulation, after
       * initializing it to the standard fill pattern.
       */
      if (Init_IVT(ms) != 0)
         { RetCode = ERROR_NOT_ENOUGH_MEMORY; goto done_filerun_and_scan; }
      if (
         !AllocateNextMemoryArea (
            &maList,                      /* &Ptr to starting pointer of list.*/
            0,                            /* MA segment.                      */
            0,                            /* MA starting offset               */
            ms->_IVT,                     /* Ptr to buff or ZERO if not allocated*/
            VECTORS_IN_IVT*4,             /* Size of buff (allocated or not). */
            TRUE,//FALSE,                 /* TRUE == allowed to free.         */
            FALSE,                        /* Not loaded from file.            */
            0l,                           /* Not used.                        */
            READABLE | WRITEABLE | EXECUTABLE   /* MA Flags.                  */
            )
         )
         { RetCode = ERROR_NOT_ENOUGH_MEMORY; goto done_filerun_and_scan; }

      /* Now allocate memory area for BIOS Data Area emulation, after
       * initializing it to the standard fill pattern.
       */
      if (Init_BIOSDA(ms) != 0)
         { RetCode = ERROR_NOT_ENOUGH_MEMORY; goto done_filerun_and_scan; }
      if (
         !AllocateNextMemoryArea (
            &maList,                      /* &Ptr to starting pointer of list.*/
            0x40,                         /* MA segment.                      */
            0,                            /* MA starting offset               */
            ms->_CR_BIOSDA,                   /* Ptr to buff or ZERO if not allocated*/
            0x100,                        /* Size of buff (allocated or not). */
            FALSE,                        /* TRUE == allowed to free.         */
            FALSE,                        /* Not loaded from file.            */
            0l,                           /* Not used.                        */
            READABLE | WRITEABLE | EXECUTABLE /* MA Flags.                    */
            )
         )
         { RetCode = ERROR_NOT_ENOUGH_MEMORY; goto done_filerun_and_scan; }

      /* Start <CodeRunner> loop with automatic write trap recovery.
       * Write TRAP could be caused by PUSH instruction, when stack segment
       * is not supplied or so (as in $DUD1153.VXE or $PCFLU2.VXE).
       * When WRITE_TRAP occured we will supply 20h bytes buffer, so that
       * trap address will fall in the middle of this buffer.
       */


do_it_all_over_again_till_end:   /* riad */

      do
         {
         WORD  NewBuffSeg, NewBuffOff;

         //CodeRunner (&ms, &rd);
         //riad//
         // Paranoia re-check
         ms = &(_MinMaxTable.curmstuff->ms);
         rd = &(_MinMaxTable.curmstuff->rd);

         // molto bene, let's continue
         CodeRunner (ms, rd);

         /* Reset this variable. In case of successful trap recovery
          * it'll become other than 0.
          */
         TrapRecovered = 0;

#ifdef CR_MOREFEATURES
#ifdef NEW1
         /* I can check if divide 0 or unsurported opcode interrupt handler
          * has been hooked. If so, run it and don't abort.
          * We can also add the stuff for GENERAL_PROTECTION (int 0D).
          * We can also add the trace flag thingy (need to abort coderunner).
          */
         if (rd->ulRetCode & (ZERODIVIDE | DIVIDE_ERROR 
               | UNSUPPORTED_OPCODE | GENERAL_PROTECTION | BREAK_CONDITION))
         {
            int intnumber = 0;
            
            if(rd->ulRetCode & (ZERODIVIDE | DIVIDE_ERROR))  intnumber = 0x00;
            else if(rd->ulRetCode & UNSUPPORTED_OPCODE)      intnumber = 0x06;
            else if(rd->ulRetCode & GENERAL_PROTECTION)      intnumber = 0x0D;
            else if(rd->ulRetCode & BREAK_CONDITION)         intnumber = 0x08;
            
            if( (*(WORD *)(ms->_IVT + intnumber*4) != 0xCF09
               || *(WORD *)(ms->_IVT + intnumber*4 + 2) != 0xF310))
               {
               /* if the interrupt has changed from initial value, then
                * the program has hooked it itself--let the program code run
                * instead of handling it ourselves -- unless this is an int 21
                * call to actually hook the interrupt 
                */
               /* The return instruction in the address of the instruction 
                * itself that caused the exception.
                */
               /* be very very careful with this */
               if(intnumber == 0x08) { _IdleInstructions = _IdleLoops = 0;  }

               ExecuteUserInt  (ms, rd, &(ms->Regs.EIP.WORDREG.RX), intnumber);

         #if 0 /* This will do the same job */ 
               if (SetRegistersForInt (ms, rd, ms->Regs.EIP.WORDREG.RX, intnumber) == -1)
                  return (-1);
         #endif

               /* now that IP, CS, SP are adjusted, come back to Coderunner() */
               rd->ulRetCode &= ~(ZERODIVIDE | DIVIDE_ERROR | UNSUPPORTED_OPCODE
                                 | GENERAL_PROTECTION | BREAK_CONDITION);
               TrapRecovered = TRUE;

               continue;
               }
         }
#endif   /* NEW1 */
#endif
         /***********************************************************************/

         // ok, this TSR code is a portion of code that has plenty of calls to
         // various interrupts, so we can execute the part of code that the virus
         // put in memory and changed by the way the interrupt vector table

#ifdef CR_TSR
        /* jce */      
         if( (rd->ulRetCode & TEST_TSR) && !tested_tsr) 
         {
back_to_test_tsr:

            tested_tsr = 1;               /* don't do this more than once        */
            if(GetFreeMemoryBlock(maList,sizeof(tsr_test_code),&NewBuffSeg,
                   &NewBuffOff))
            {
            /* allocate a buffer for the code which will attempt to trigger
             * the code that has "gone resident" 
             */
            RetMA =
               AllocateNextMemoryArea(
                  &maList,                /* &Ptr to starting pointer of list.   */
                  NewBuffSeg,             /* MA segment.                         */
                  NewBuffOff,             /* MA starting offset.                 */
                  tsr_test_code,          /* Ptr to buff or ZERO if not allocated*/
                  sizeof(tsr_test_code),  /* Size of buff (allocated or not).    */
                  FALSE,                  /* TRUE == allowed to free.            */
                  FALSE,                  /* Trap point outside of file.         */
                  0l,                     /* Not used.                           */
                  READABLE | WRITEABLE  | EXECUTABLE /* MA Flags.                */
                  );
          
            if (!RetMA)
            { RetCode = ERROR_NOT_ENOUGH_MEMORY; goto done_filerun_and_scan; }
           
#if CR_DBGTF
            if(cr_itrace)
               fprintf(cr_dbgf,"Patched TSR testing code into CS:IP=%04x:%04x (size=0x%x)\n",
                  NewBuffSeg,NewBuffOff,sizeof(tsr_test_code));
#endif

            /* point CS:IP at code to run */
#ifdef NEW1
            ms->Regs.CS             = NewBuffSeg;
            ms->Regs.EIP.WORDREG.RX = NewBuffOff;
#else
            ms->Regs.wordregs.RegCS = NewBuffSeg;
            ms->Regs.wordregs.RegIP = NewBuffOff;
#endif   /* NEW1 */
            /* indicate that we recovered from the trap so execution
               will continue */
            TrapRecovered = TRUE;

            // codedata stuff, VERY IMPORTANT for executed code section update
            _MinMaxTable.state = MODEJUMP;

            continue;
            }
         }
         /* end jce */
#endif

         /***********************************************************************/

         /* Recover only from R/W/E traps. Any other termination condition
          * is considered as fatal.
          */
         if (!(rd->ulRetCode & ~(ms->ulIgnore | READ_TRAP | WRITE_TRAP | EXECUTE_TRAP)))
            {
            /*-------------------------------------------------------------*/
            WORD  HalfBuffSize;
            //WORD  NewBuffSeg, NewBuffOff;
            WORD  NewBuffSize;
            BYTE  *pNewBuff;
            ULONG ulAbsOffsetOfBufferStart;
            /*-------------------------------------------------------------*/

            /* Get all parameters for about-to-be-allocated buffer.
             * Check whether trap points is inside of executable part
             * of file.
             */

            if (rd->ulRetCode & EXECUTE_TRAP)
               HalfBuffSize = _Default_ini.ExeTrapHalfBuffSize;
            else
               HalfBuffSize = _Default_ini.RWTrapHalfBuffSize;

            RetCode =
               calculate_file_region (
                  ExeFile,             /* <- TRUE for EXE type file. */
                  FileHdrSize,         /* <- size of EXE file hdr.   */
                  FileSize,            /* <- full filesize.          */
                  StartSeg,            /* <- 0x3000?                 */
                  rd->SegTrap,          /* <- Segment of trap point.  */
                  rd->OffTrap,          /* <- Offset of trap point.   */
                  HalfBuffSize,        /* <- Delta bytes from trap.  */
                  &NewBuffSeg,         /* ->                         */
                  &NewBuffOff,         /* ->                         */
                  &ulAbsOffsetOfBufferStart, /* ->                   */
                  &NewBuffSize         /* ->                         */
                  );

            if (RetCode == SUCCESS)
               {
               /* Ok, R/W/E Trap point is inside of file code.
                */

               /* Now we must adjust NewBuffSeg, NewBuffOff and
                * NewBuffSize so that it
                * won't overlap with the existing addresses into the linked
                * list of memory areas.
                */

               /*----------------------------------------------------------*/
               ULONG    ulBuffOldLinearAddr;
               /*----------------------------------------------------------*/

               /* Check whether number of traps didn't exceed maximal allowed
                * number for each kind of trap.
                */
               if (rd->ulRetCode & EXECUTE_TRAP)
                  {  /* EXEC trap.  */
#ifdef   CR_DEBUG
                  if (cr_hd) fprintf (cr_dbgf, "EXECUTE_TRAP (unsupplied code?) at %04X:%04X...\n",
                                      ms->Regs.CS, ms->Regs.EIP.WORDREG.RX);
#endif
                  if (total_exec_traps < _Default_ini.MaxExecTrapsToRecover)
                     {
                     total_exec_traps++;
                     HalfBuffSize = _Default_ini.ExeTrapHalfBuffSize;
                     }
                  else
                     {
                     RetCode = ERROR_TOO_MANY_EXEC_TRAPS;
                     break;
                     }
                  }
               else  /* R/W trap.   */
                  {
#ifdef   CR_DEBUG
                  if (cr_hd) fprintf (cr_dbgf, "Instruction at %04X:%04X caused ",
                                      ms->Regs.CS,ms->Regs.EIP.WORDREG.RX);
                  if (rd->ulRetCode & READ_TRAP)
                     if (cr_hd) fprintf (cr_dbgf, "READ ");
                  if (rd->ulRetCode & WRITE_TRAP)
                     if (cr_hd) fprintf (cr_dbgf, "WRITE ");
                  if (cr_hd) fprintf (cr_dbgf, "trap access to %04X:%04X..\n", rd->SegTrap,rd->OffTrap);
#endif
                  if (total_rw_traps < _Default_ini.MaxRWTrapsToRecover)
                     {
                     if(!_InREP_LODS) total_rw_traps++;  //riad//
                     HalfBuffSize = _Default_ini.RWTrapHalfBuffSize;
                     }
                  else
                     {
                     if (rd->ulRetCode & WRITE_TRAP)
                        {
                        /* WRITE_TRAP bit is set - so either W or W/R trap
                         * happened.
                         */
                        RetCode = ERROR_TOO_MANY_RW_TRAPS;
                        break;
                        }
                     else
                        {
                        /* READ_TRAP (only) happened.
                         * Number of already successfully recovered R/W traps
                         * already too big.
                         * Now we'll start ignoring any READ_TRAP.
                         * Ignore this single instruction.
                         */
                        ms->ulIgnoreNextOpcode = READ_TRAP;
                        TrapRecovered = TRUE;
                        continue;
                        }
                     }
                  }

               /* We must save old real linear address of start of buffer,
                * because buffer Seg:Off could get changed during next call.
                * And we must be able to adjust <ulAbsOffsetOfBufferStart>
                * after that.
                */
               ulBuffOldLinearAddr = LinearAddr (NewBuffSeg, NewBuffOff);

               /* After this call <NewBuffSeg>, <NewBuffOff> and
                * <NewBuffSize> could get changed. This call will ensure us,
                * that new buffer boundaries aren't overlapping with any
                * of existing buffers in linked list of memory areas.
                */
               RetCode =
                  ValidateBufferBoundaries (
                     maList,           /* <- &Ptr to start pointer of list */
                     rd->SegTrap,      /* <- Must-be-inside point.         */
                     rd->OffTrap,      /* <- Must-be-inside point.         */
                     &NewBuffSeg,      /* <-> Buffer real start addr.      */
                     &NewBuffOff,      /* <-> Buffer real start addr.      */
                     &NewBuffSize      /* <-> Buffer size.                 */
                     );

               if (RetCode != SUCCESS)
                  /* Trap address falls into one of already allocated
                   * buffers in Memory_areas linked list.
                   */
                  {
                  RetCode =  ERROR_TRAP_ADDR_EXISTS;
                  break;
                  }

               /* Here after successful call to ValidateBufferBoundaries()
                * we have to re-calculate <ulAbsOffsetOfBufferStart>.
                */
               ulAbsOffsetOfBufferStart+=
                  LinearAddr(NewBuffSeg, NewBuffOff)-ulBuffOldLinearAddr;

               /* Allocate buffer...
                */
               pNewBuff = malloc (NewBuffSize);
               if (!pNewBuff)
                  {
                  RetCode = ERROR_NOT_ENOUGH_MEMORY;
                  goto done_filerun_and_scan;
                  /*break;*/
                  }

               /* Read file region into newly allocated buffer.
                */
               /*printf("ReadFileRegion, %lu, %u\n", ulAbsOffsetOfBufferStart, NewBuffSize);*/
               RetCode =
                  pfnReadFileRegion (
                     ulAbsOffsetOfBufferStart,  /* LSEEK to...          */
                     pNewBuff,                  /* Destination buffer.  */
                     NewBuffSize                /* Size of dest. buffer.*/
                     );

               if (RetCode != SUCCESS)
                  {
                  free (pNewBuff);
                  RetCode = ERROR_CANT_READ_FILE;
                  goto done_filerun_and_scan;
                  /*break;*/
                  }

               if (ExeFile)
                  {
                  /* Process relocation items on new buffer.
                   */
                  ProcessExeRelocationTable  (
                     pNewBuff,
                     NewBuffSize,
                     ulAbsOffsetOfBufferStart - (ExeFile?FileHdrSize:0),
                     StartSeg,
                     pReloItems
                     );
                  }

               /* And finally allocate memory area for newly created
                * buffer.
                */
               RetMA =
                  AllocateNextMemoryArea (
                     &maList,             /* &Ptr to starting pointer of list.*/
                     NewBuffSeg,          /* MA segment.                      */
                     NewBuffOff,          /* MA starting offset.              */
                     pNewBuff,            /* Ptr to buff or ZERO if not allocated*/
                     NewBuffSize,         /* Size of buff (allocated or not). */
                     TRUE,                /* TRUE == allowed to free.         */
                     TRUE,                /* Trap point loaded from a file.   */
                     ulAbsOffsetOfBufferStart,/* Offset into file             */
                     READABLE | WRITEABLE | EXECUTABLE /* MA Flags.           */
                     );

               if (!RetMA)
                  {
                  free (pNewBuff);
                  RetCode = ERROR_NOT_ENOUGH_MEMORY;
                  goto done_filerun_and_scan;
                  /*break;*/
                  }

               /* Now trap was successfully recovered.
                * Next loop will re-run CodeRunner.
                */
               TrapRecovered = TRUE;
               }
            else
               {
               /* TRAP address falls outside of file executable code
                * boundaries.
                */
               if (rd->ulRetCode & EXECUTE_TRAP)
                  {
                  /* Cannot recover from EXECUTE_TRAP in this case.
                   */
                  RetCode = ERROR_EXEC_TRAP_OUTSIDE;
                  break;
                  }
               else
                  {
                  /*-------------------------------------------------------*/
                  /* Linear address of file code start/end. */
                  ULONG ulLAFCodeStart, ulLAFCodeEnd;

                  /* Linear address of buffer start/end.    */
                  ULONG ulLABuffStart,  ulLABuffEnd;

                  /* Linear address of trap point.          */
                  ULONG ulLATrapPoint;
                  /*-------------------------------------------------------*/

#ifdef   CR_DEBUG
                  if (cr_hd) fprintf (cr_dbgf, "Instruction at %04X:%04X caused ",
                                      ms->Regs.CS, ms->Regs.EIP.WORDREG.RX);
                  if (rd->ulRetCode & READ_TRAP)
                     if (cr_hd) fprintf (cr_dbgf, "READ ");
                  if (rd->ulRetCode & WRITE_TRAP)
                     if (cr_hd) fprintf (cr_dbgf, "WRITE ");
                  if (cr_hd) fprintf (cr_dbgf, "trap access to %04X:%04X..\n", rd->SegTrap,rd->OffTrap);
#endif

                  if (rd->ulRetCode & READ_TRAP)
                     {
                     /* Ignore this single instruction, caused READ_TRAP
                      * from the "air" - unsupplied memory area, falls
                      * outside of executable part of file.
                      * We don't want to recover traps (| READ_TRAP)
                      * from the "air". Instructions, like:
                      * ADD [xxxx],BX would cause READ and WRITE trap at
                      * the same time. These type of instruction should
                      * be ignored also. Note: this doesn't include:
                      * MOV [xxxx],BX - type instruction, generating only
                      * WRITE_TRAP.
                      */
                     ms->ulIgnoreNextOpcode = READ_TRAP | WRITE_TRAP;
                     TrapRecovered = TRUE;
                     continue;
                     }

                  /* Now is the good time to check value of <total_rw_traps>.
                   * At this point it concerns only WRITE_TRAP.
                   */
                  if (total_rw_traps < _Default_ini.MaxRWTrapsToRecover)
                     {
                     if(!_InREP_LODS) total_rw_traps++;  //riad//
                     HalfBuffSize = _Default_ini.RWTrapHalfBuffSize;
                     }
                  else
                     {
                     RetCode = ERROR_TOO_MANY_RW_TRAPS;
                     break;
                     }

                  /* We still can recover from WRITE trap by allocating
                   * a virtual buffer.
                   */
                  ulLATrapPoint = LinearAddr(rd->SegTrap, rd->OffTrap);

                  ulLABuffStart = ulLATrapPoint - (_Default_ini.RWTrapHalfBuffSize);

                  /* Initialize NewBuffSeg,Off,Size.
                   * Next 2 if-s for cases, when trap address is very close to either
                   * 0:0 or FFFF:F. We don't want to allocate senseless buffer
                   * crossing boundaries of address space.
                   */
                  if ((ULONG)(_Default_ini.RWTrapHalfBuffSize) > ulLATrapPoint)
                     ulLABuffStart = 0;
                  if ((ULONG)(_Default_ini.RWTrapHalfBuffSize) > 0xFFFFFUL-ulLATrapPoint)
                     ulLABuffStart = (0xFFFFFUL-(_Default_ini.RWTrapHalfBuffSize)*2+1)&0xFFFFFUL;
                  NewBuffSeg  = (WORD)((ulLABuffStart>>4)&0xFFFF);
                  NewBuffOff  = (WORD)(ulLABuffStart&0x0F);
                  NewBuffSize = (WORD)((_Default_ini.RWTrapHalfBuffSize)*2);

                  /* Now we have to make sure none part of buffer is covers
                   * part of executable file addresses.
                   * If we're here - trap point is outside of file code.
                   * So we must be consistent - all buffer addresses must
                   * be outside of file code.
                   */
                  if (ExeFile)
                     {
                     ulLAFCodeStart = LinearAddr(StartSeg, 0);
                     ulLAFCodeEnd   = ulLAFCodeStart + FileSize - FileHdrSize - 1;
                     }
                  else
                     {
                     ulLAFCodeStart = LinearAddr(StartSeg, 0x100);
                     ulLAFCodeEnd   = ulLAFCodeStart + FileSize - 1;
                     }

                  ulLABuffEnd    = ulLABuffStart + NewBuffSize - 1;

                  /* F - executable address space of file.
                   * B - requested buffer.
                   * t - trap point (ulLATrapPoint).
                   *
                   * Linear address space:
                   * ---------------------------------
                   *            ulLAFCodeStart
                   *           /              ulLAFCodeEnd
                   *          /              /
                   *          FFFFFFFFFFFFFFF
                   *  BBBBBtBBBBB
                   *  \     \    \ulLABuffEnd
                   *   \     \ulLATrapPoint
                   *    \ulLABuffStart
                   *
                   *
                   *  We want it to be:
                   *
                   *          FFFFFFFFFFFFFFF
                   *  BBBBBtBB
                   *
                   */
                  if (ulLATrapPoint < ulLAFCodeStart  && /*trap before file*/
                      ulLABuffEnd   >= ulLAFCodeStart)
                     {
                     NewBuffSize = (WORD)(ulLAFCodeStart - ulLABuffStart);
                     ulLABuffEnd = ulLABuffStart + NewBuffSize - 1;
                     }

                  /* F - executable address space of file.
                   * B - requested buffer.
                   * t - trap point.
                   *
                   *          FFFFFFFFFFFFFFF
                   *                     BBBBBBtBBBBBB
                   *
                   *          We want it to be:
                   *
                   *          FFFFFFFFFFFFFFF
                   *                         BBBtBBBBBB
                   */
                  if (ulLATrapPoint >  ulLAFCodeEnd   && /*trap after file */
                      ulLABuffStart <= ulLAFCodeEnd)
                     {
                     NewBuffSize    -= ((WORD)((ulLAFCodeEnd - ulLABuffStart + 1)));
                     ulLABuffStart   =  ulLAFCodeEnd+1;

                     /* Recalculate buffer starting absolute segment/offset
                      * after <ulLABuffStart> was changed.
                      */
                     NewBuffSeg  = (WORD)((ulLABuffStart>>4)&0xFFFF);
                     NewBuffOff  = (WORD)(ulLABuffStart&0x0F);
                     }


                  /* Validate coordinates of about-to-be-allocated buffer.
                   * After this call <tmpSeg>, <tmpOff> and
                   * <tmpSize> could get changed. This call will ensure us,
                   * that new buffer boundaries aren't overlapping with any
                   * of existing buffers in linked list of memory areas.
                   */
                  RetCode =
                     ValidateBufferBoundaries (
                        maList,           /* <- &Ptr to start pointer of list */
                        rd->SegTrap,       /* <- Must-be-inside point.         */
                        rd->OffTrap,       /* <- Must-be-inside point.         */
                        &NewBuffSeg,      /* <-> Buffer real start addr.      */
                        &NewBuffOff,      /* <-> Buffer real start addr.      */
                        &NewBuffSize      /* <-> Buffer size.                 */
                        );

                  if (RetCode != SUCCESS)
                     {
                     /* Trap address falls into one of already allocated
                      * buffers in Memory_areas linked list.
                      */
                     RetCode =  ERROR_TRAP_ADDR_EXISTS;
                     break;
                     }

                  /* Now we can allocate new memory area for new buffer.
                   */
                  RetMA =
                     AllocateNextMemoryArea (
                        &maList,             /* &Ptr to starting pointer of list.   */
                        NewBuffSeg,          /* MA segment.                         */
                        NewBuffOff,          /* MA starting offset.                 */
                        0,                   /* Ptr to buff or ZERO if not allocated*/
                        NewBuffSize,         /* Size of buff (allocated or not).    */
                        TRUE,                /* TRUE == allowed to free.            */
                        FALSE,               /* Trap point outside of file.         */
                        0l,                  /* Not used.                           */
                        READABLE | WRITEABLE | EXECUTABLE /* MA Flags.              */
                        );

                  if (!RetMA)
                     {
                     RetCode = ERROR_NOT_ENOUGH_MEMORY;
                     goto done_filerun_and_scan;
                     /*break;*/
                     }

                  /* Now READ or WRITE trap was successfully recovered.
                   * Next loop will re-run CodeRunner.
                   */
                  TrapRecovered = TRUE;
                  }
               }

#ifdef   CR_DEBUG
               if (TrapRecovered)
                  if (cr_hd) fprintf (cr_dbgf, " Recovered\n");
               else
                  if (cr_hd) fprintf (cr_dbgf, "\nCannot recover from TRAP.\n");
#endif
            }  /* if() */
         else
            {
            /* Other reason than R/W/E trap caused termination.
             * This also includes normal termination of CodeRunner.
             */
            RetCode = ERROR_NON_TRAP_TERMINATION;
            break;
            }

         } /* do {} while() */

         /* While terminated only because of R/W/E TRAP and recovery was
          * successful and number of recovered traps did not exceed maximum.
          */

      while (
            !(rd->ulRetCode & ~(ms->ulIgnore | READ_TRAP | WRITE_TRAP | EXECUTE_TRAP
#ifdef CR_TSR  //riad//
            | TEST_TSR
#endif
            )) && TrapRecovered &&
			((timeout == 0) || (timeout>time(NULL)-starttime))
            );

      if(!tested_tsr && 
         ((*(WORD *)(ms->_IVT + 0x21*4) != 0xcf09       
         || *(WORD *)(ms->_IVT + 0x21*4 + 2) != 0xf310))) 
         goto back_to_test_tsr;



#ifdef CR_DEBUG
      {
      static         unsigned long op_count=0l;
      static         unsigned long file_count=0l;
      char           abort_explanation[128];

      abort_explanation[0] = '\0';
      if (rd->ulRetCode & READ_TRAP)
      {
        strcat(abort_explanation, "READ_TRAP ");
      }
      if (rd->ulRetCode & WRITE_TRAP)
      {
        strcat(abort_explanation, "WRITE_TRAP ");
      }
      if (rd->ulRetCode & EXECUTE_TRAP)
      {
        strcat(abort_explanation, "EXECUTE_TRAP ");
      }
      if (rd->ulRetCode & UNSUPPORTED_OPCODE)
      {
        strcat(abort_explanation, "UNSUPPORTED_OPCODE ");
      }
      if (rd->ulRetCode & MAXOPS_REACHED)
      {
        strcat(abort_explanation, "MAXOPS_REACHED ");
      }
      if (rd->ulRetCode & TRACE_FLAG_SET)
      {
        strcat(abort_explanation, "TRACE_FLAG_SET ");
      }
      if (rd->ulRetCode & ZERODIVIDE)
      {
        strcat(abort_explanation, "ZERODIVIDE ");
      }
      if (rd->ulRetCode & DIVIDE_ERROR)
      {
        strcat(abort_explanation, "DIVIDE_ERROR ");
      }
      if (rd->ulRetCode & PORT_IO)
      {
        strcat(abort_explanation, "PORT_IO ");
      }
      if (rd->ulRetCode & CANNOT_EXECUTE_INT)
      {
        strcat(abort_explanation, "CANNOT_EXECUTE_INT ");
      }
      if (rd->ulRetCode & USER_ABORT)
      {
        strcat(abort_explanation, "USER_ABORT ");
      }
      if (rd->ulRetCode & HLT_EXECUTED)
      {
        strcat(abort_explanation, "HLT_EXECUTED ");
      }
      if (rd->ulRetCode & CODE_SELF_MODIFIED)
      {
        strcat(abort_explanation, "CODE_SELF_MODIFIED ");
      }
      if (rd->ulRetCode & BREAK_CONDITION)
      {
        strcat(abort_explanation, "BREAK_CONDITION ");
      }
      if (rd->ulRetCode & INVALID_PARAMETER)
      {
        strcat(abort_explanation, "INVALID_PARAMETER ");
      }
      op_count += rd->ops;
      ++file_count;
      if (cr_hd) fprintf (cr_dbgf,
                          "\nCodeRunner terminated. Results:\n"
                          "rd.ops             = %04X (mean=%lu)\n"
                          "rd.SegTrap:OffTrap = %04X:%04X\n"
                          "rd.PrevCS :PrevIP  = %04X:%04X\n"
                          "rd.ulRetCode       = %04X%04X %s\n",
                          rd->ops, op_count/file_count,
                          rd->SegTrap,
                          rd->OffTrap,
                          rd->PrevCS,
                          rd->PrevIP,
                          (WORD)((rd->ulRetCode)>>16), (WORD)((rd->ulRetCode)&0xFFFF),
                          abort_explanation
                          );
      }
#endif
#ifdef HEX_DUMPER
#ifdef CR_DEBUG
      /* This part of code will write collected data to HEX and BIN file.  */
      if (cr_hd) fprintf (cr_dbgf, "\nWriting collected data...\n");
      {
      FILE  *hex_file=0;
      char  *hex_filename="mbuff.hex";
      WORD  start_mod_index, end_mod_index;
      int   j;
      static         unsigned long op_count=0l;
      static         unsigned long file_count=0l;

      hex_file = fopen (hex_filename, "a");
      if (!hex_file)
         {
         if (cr_hd) fprintf (cr_dbgf, "\x7\nError! Cannot open/create file! (%s)\n", hex_filename);
         goto done_info_dump;
         }

      start_mod_index = (WORD)
         (LinearAddr(ms->pMemA->Seg, ms->pMemA->minOffWr) -
          LinearAddr(ms->pMemA->Seg, ms->pMemA->Off));

      end_mod_index = (WORD)
         (LinearAddr(ms->pMemA->Seg, ms->pMemA->maxOffWr) -
          LinearAddr(ms->pMemA->Seg, ms->pMemA->Off));
      if (cr_hd) fprintf (hex_file, "\n\n***** %s", filename);

      op_count += rd->ops;
      ++file_count;
      if (cr_hd) fprintf (hex_file, "\t\tops=%08ld (mean=%lu)\n", rd->ops, op_count/file_count);
      if (cr_hd) fprintf (hex_file, "\t\_flagBufferDegarbled=%d\n", _flagBufferDegarbled);

      if (ms->pMemA->minOffWr == 0xFFFF)
         {
         if (cr_hd) fprintf (hex_file, "\t\t----- Buffer wasn't modified.");
         goto done_info_dump;
         }
      else
         {
         if (!org_buf)
            {
            if (cr_hd) fprintf (hex_file, "\t\t----- !!!!!!!!!!!!! Not enough memory to allocate org_buf !!!!!!!!!!!!.\n");
            }
         else
            {
            if ((end_mod_index - start_mod_index + 1) < 20)
               {
               if (cr_hd) fprintf (hex_file, "\t\t----- Modified buffer is TOO small (%d-%d+1=%d bytes).\n",
                                   end_mod_index, start_mod_index, end_mod_index - start_mod_index + 1);
               }
            else
               {
               /* Now we must search for:
                * First 20 bytes of presumably modified code, where first
                * byte and at least any other 9 bytes are really modified.
                * That will help to find a real offset (start_mod_index),
                * where data was actually modified.
                */
               /*---------------------*/
               int         i, num_of_srch;
               int         counter=0, k;
               /*---------------------*/
               if (cr_hd) fprintf (hex_file, "\n      start_mod_index, reported by CodeRunner = %d(%04Xh)",
                                   start_mod_index, start_mod_index);

               num_of_srch = (end_mod_index-start_mod_index+1)-20+1;

               for (i=0; i<num_of_srch; i++, start_mod_index++)
                  {
                  if (org_buf[start_mod_index] != buffer_to_be_scanned[start_mod_index])
                     {
                     for (k=0; k<10; k++)
                        if (org_buf[start_mod_index+k] != buffer_to_be_scanned[start_mod_index+k])
                           counter++;
                        if (counter >= 10)
                           break;
                     }
                  }

               if (counter < 10)
                  {
                  if (cr_hd) fprintf (hex_file, "      !!!!! Cannot find really modified data in buffer!\n");
                  }
               else
                  {
                  /* Write data to ASCII .HEX file.
                   */
                  if (cr_hd) fprintf (hex_file, "\n      Calculated start_mod_index = %d(%04Xh), end_mod_index = %d(%04Xh).\n",
                                      start_mod_index, start_mod_index,
                                      end_mod_index,   end_mod_index);
                  if (cr_hd) fprintf (hex_file, "    ");
                  for (i=(int)start_mod_index, j=1; i<=(int)end_mod_index; i++, j++)
                     {
                     if (cr_hd) fprintf (hex_file, "%02X ", buffer_to_be_scanned[i]);
                     if (j==20)
                        {
                        j=0;
                        if (cr_hd) fprintf (hex_file, "\n    ");
                        }
                     }
                  }
               }
            }
         }
done_info_dump:
         if (hex_file)     { fclose (hex_file); }
      }

#endif
#endif /* HEX_DUMPER */
      }
   else
      {
      RetCode = ERROR_NOT_ENOUGH_MEMORY;

#ifdef   CR_DEBUG
      if (cr_hd) fprintf (cr_dbgf, "\nERROR: Cannot allocate first MEMORY_AREA. CodeRunner skipped.\n");
#endif
      goto done_filerun_and_scan;
      }

#ifdef HEX_DUMPER
   /* Now we can deallocate buffer used to keep original data.
    */
   if (org_buf)
      free (org_buf);
#endif
   /* CodeRunner did it's job.
    * Start 2-nd part: run <scanworkbuf> for every modified buffer.
    */

#ifdef CR_DO_SCANNER //riad//
   RetCode = ScanBuffers ( &ms, &rd, 1 );
#endif

   /* Time to clean-up after working. Deallocate all memory areas.
    */
done_filerun_and_scan:

//just for debuging information
#ifndef CR_DO_SCANNER   //riad//
      _MinMaxTable.RetCode |= rd->ulRetCode;
#ifndef CR_MOREFEATURES //riad//
      {
      extern ULONG _NumOpcodes;
      _NumOpcodes = rd->ops;
      }
#endif
#endif
#ifdef CR_MOREFEATURES  //riad//

   // Free allocated sections for executed code and stuff
   // Also free the linked list stuff
   //free_almost_evrything();

   _MinMaxTable.NumOfTotalInst = rd->ops;
#endif

/*****************************************************************************
 *
 *     This part of code handle the switch to another machine state
 *
 *****************************************************************************/


   //riad//  
   // code is added here to handle the code data stuff
   // if return code (stored in _MinMaxTable.codata is
   // different from 0, then after a termination, we are going to
   // go to the beach and get hit by the sun to see how it looks like

   // free memory allocated for the current machine state that has just
   // terminated in a non-living-state. 
   // also, update the jump_stuff list (well, put a 2 somewhere)

   // we don't want to run free_machine_state(cur) when OPT_DECRYPT is on

   for(;;)
   {
      machine_stuff *cur = _MinMaxTable.curmstuff;

      // we don't want to run free_machine_state(cur) when OPT_DECRYPT is on
      if(com_options&OPT_DECRYPT) break;

      // to prevent some memory problems linked with TSR thing
      if(tested_tsr) dont_test_tsr = 1;

      //printf("---> Terminating machine [%u]%10s\r", cur->number, " ");

      //now update curmstuff
      _MinMaxTable.curmstuff = cur->previous;

      if(_MinMaxTable.curmstuff != NULL)
      {
         // some debuging informations....no getting away from it
         printdebug("Machine", cur->number);
      }

      free_machine_state(cur);

      // ok, now the current is updated, and also update cur->next
      // that should be the bottom of the list, which means NULL
      cur = _MinMaxTable.curmstuff;
      if(cur) cur->next = NULL;

	  //Fred: timeout test
	  //printf ("Timeout: %d\tTime-StartTime: %d\n",timeout,time(NULL)-starttime);
	  if (timeout && timeout<time(NULL)-starttime) break;

      if(cur == NULL)
      {
         // it's finnished, all jumps consumed !!!!! wow !
         break;
      }
      else
      {
         jmp_stuff *jumjum = _MinMaxTable.jmpstuff;

         // VERY IMPORTANT (or unimportant if you think the world
         // doesn't depend on it)
         // Initialize some values in _MinMaxTable that are important
         // for the status of the coming machine state
         // Also, don't forget some global variables
         _MinMaxTable.curmstuff->next  = NULL;
         _MinMaxTable.state            = MODEJUMP;
         _MinMaxTable.dojump           = MODEIDLE; /* or something */
         _ActiveInstructions           = cur->m_ActiveInstructions;
         _IdleInstructions             = cur->m_IdleInstructions;
         _IdleLoops                    = cur->m_IdleLoops;
         _IdleLoopLinearAddr           = cur->m_IdleLoopLinearAddr;
         _MinMaxTable.NumOfDiffInst       = cur->NumOfDiffInst;
         _MinMaxTable.NumOfRealActiveInst = cur->NumOfRealActiveInst;
         _MinMaxTable.NumOfRealIdleInst   = cur->NumOfRealIdleInst;
         _MinMaxTable.NumOfDiffLoops      = cur->NumOfDiffLoops;
         _MinMaxTable.NumOfLastDiffInst   = 0;
         _MinMaxTable.NumOfTotalInst      = 0;
         _MinMaxTable.RetCode             = 0L;

         // reset all "counterup/down" in jump_stuff list for the
         // next session
         while(jumjum != NULL)
         {
            jumjum->counterup = jumjum->counterdown = 0;
            jumjum = jumjum->next;
         }

         // now, update the current local variables, and the globals.
         ms       = &(_MinMaxTable.curmstuff->ms);
         rd       = &(_MinMaxTable.curmstuff->rd);
         maList   = ms->pMemA;
         //_IVT     = _MinMaxTable.curmstuff->_IVT;
         //_CR_PSP  = _MinMaxTable.curmstuff->_CR_PSP;

         // This is VERY IMPORTANT !!! (can also be done it in update_machine()
         ms->ExecutionBroken = TRUE;

         // We want to destroy immediately this state machine if somehow her
         // conditionnal jump has been encountered in the both ways.

         if((cur->curjmp)->jumpcond == 2) /*continue*/;
         else (cur->curjmp)->jumpcond = 2;

         printf("\r---> Starting machine [%3u]", cur->number, " ");

#ifdef __UNIX__
         fflush(stdout);
#endif
         goto do_it_all_over_again_till_end;
      }
      break;
   }

 /*****************************************************************************
 *
 *     This part of code handled the switch to another machine state
 *
 *****************************************************************************/
 
   // this is comented because it's done already above
   //FreeAllMemoryAreas (&maList);

#ifdef CR_DEBUG
   if (cr_dbgf != INIT_CR_DBGF)
      { fclose(cr_dbgf); cr_dbgf = INIT_CR_DBGF; }
#endif

   return (RetCode);
}


/*=========================================================================*/

/* Function called by IntEmulator to determine whether an interrupt is
 * in the list of interrupts that we don't continue execution on.
 * Returns
 * 0   : we should continue execution. (Interrupt is a potential nop.)
 * 1   : we should stop execution. (Interrupt is not a potential nop.)
 */
int   NotNOPInt_Longer (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         int            IntNumber
         )
{
   int rv = 0;

   switch(IntNumber)
   {
    /*case(0x02):*/
    /*case(0x04):*/
      //case(0x05):          /* Print screen */
    /*case(0x06):*/
    /*case(0x07):*/
    /*case(0x09):*/
    /*case(0x0A):*/
    /*case(0x0B):*/
    /*case(0x0C):*/
    /*case(0x0D):*/
    /*case(0x0E):*/
    /*case(0x0F):*/
         //rv = 1; //riad//
         //break;
      case(0x10):
         switch (rAH)
         {
            //case(0x00):    /* Set video mode. */
            //case(0x01):    /* Set text mode cursor shape */
            //case(0x02):    /* Set cursor position */
            //case(0x05):    /* Set display page */
            //case(0x06):    /* Scroll up */
            //case(0x07):    /* Scroll down */
            //case(0x09):    /* Write character + attribute */
            //case(0x0A):    /* Write character only */
            //case(0x0B):    /* Set border or background */
            //case(0x0C):    /* Write pixel */
            //case(0x0E):    /* Write character teletype mode */
            case(0x10):    /* Various video sets, and a few gets. */
            //case(0x13):    /* Write string */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x13):
         switch(rAH)
         {
            case(0x03):    /* Write sector(s) */
            case(0x05):    /* Format track */
            case(0x06):    /* Format track bad */
            case(0x07):    /* Format drive */
            case(0x0B):    /* Write long sector(s) */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x14):
         switch(rAH)
         {
            case(0x00):    /* Initialize serial port */
            case(0x01):    /* Write character to port */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x16):
         switch(rAH)
         {
            case(0x00):    /* Get keystroke */
            case(0x03):    /* Set typematic rate/delay */
            case(0x10):    /* Get enhanced keystroke */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x17):
         switch(rAH)
         {
            case(0x00):    /* Printer write character */
            case(0x01):    /* Printer initialize port */
            case(0x03):    /* Printer print string */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x19):          /* Reboot */
         rv = 1;
         break;
      case(0x1A):
         switch(rAH)
         {
            case(0x01):    /* Set system time */
            case(0x03):    /* Set real-time clock */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x1B):          /* Ctrl-Break handler */
         rv = 1;
         break;
      //case(0x20):          /* Terminate program */
      //   rv = 1;
      //   break;
      case(0x21):
         switch(rAH)
         {
            case(0x00):    /* Terminate */
            //case(0x02):    /* Write character to stdout */
            //case(0x04):    /* Write character to stdaux */
            //case(0x05):    /* Write character to printer */
            //case(0x06):    /* Direct console output */
            case(0x07):    /* Direct character input */
            case(0x08):    /* Direct character input without echo */
            //case(0x09):    /* Write string to standard output */
            //case(0x0E):    /* Select default drive */
            //case(0x0F):    /* FCB open file */
            //case(0x10):    /* FCB close file */
            //case(0x11):    /* FCB findfirst */
          /*case(0x12):*/  /* FCB findnext */
            //case(0x13):    /* FCB delete */
          /*case(0x14):*/  /* FCB read */
            //case(0x15):    /* FCB write */
            //case(0x16):    /* FCB create/truncate */
            //case(0x17):    /* FCB rename */
            //case(0x1A):     /* Set DTA */
          /*case(0x21):*/  /* FCB Random Record Read */
            //case(0x22):    /* FCB Random Record Write */
            //case(0x25):     /* Set interrupt vector */
            case(0x26):    /* Create new PSP */
          /*case(0x27):*/  /* FCB Random Block Read. */
            case(0x28):    /* FCB Random Block Write. */
            //case(0x2B):    /* Set system date. */
            //case(0x2D):    /* Set system time. */
            //case(0x31):    /* TSR. */
               rv = 1;
               break;
            //case(0x33):    /* Gets/Sets various. */
            //   switch(rAL)
            //   {
            //      case(0x01):
            //      case(0x02):
            //         rv = 1;
            //         break;
            //      default:
            //         break;
            //   }
            //   break;
            //case(0x39):    /* Create subdirectory */
            //case(0x3A):    /* Remove subdirectory */
            //case(0x3B):    /* Set current directory */
            //case(0x3C):    /* Create or truncate file */
            //case(0x3D):    /* Open file */
          /*case(0x3E):*/  /* Close file */
            // rv = 1;
            //   break;
            case(0x3F):    /* Read file */
               switch(rBX)
               {
                  case(0x0000):
                     rv = 1; /* User would notice read */
                             /* from standard input */
                     break;
                  default:
                     break;
               }
            //case(0x40):    /* Write file */
            //case(0x41):    /* Delete file */
          /*case(0x42):*/  /* Lseek */
          /*case(0x43):*/  /* Get/Set file attributes */
            //case(0x44):    /* IOCTL */
          /*case(0x45):*/  /* Duplicate file handle */
          /*case(0x46):*/  /* Force duplicate file handle */
          /*case(0x48):*/  /* Allocate memory */
          /*case(0x49):*/  /* Free memory */
            //case(0x4A):    /* Resize memory block */
            //case(0x4B):    /* Load and execute program */
            //case(0x4C):    /* Terminate */
            //case(0x4E):     /* Findfirst */
          /*case(0x4F):*/  /* Findnext */
            case(0x50):    /* Set PID */
            case(0x55):    /* Create child PSP */
            //case(0x56):    /* Rename file */
          /*case(0x57):*/  /* Get/set file date/time */
            case(0x60):    /* Make true name */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x23):          /* Ctrl-Break handler */
      case(0x24):          /* Critical error handler */
      case(0x26):          /* Absolute disk write */
      //case(0x27):          /* Obsolete TSR */ /* handled differently by jce TST stuff
      case(0x29):          /* Fast console output */
         rv = 1;
         break;
      case(0x2F):          /* Multiplex */
#if 0
         switch(rAH)
         {
            case(0x10):    /* SHARE, Nightfall viruses */
               break;
            default:
               if (rAL != 0)     /* Abort if not installation check. */
                  rv = 1;        /* This is may be too strong! */
               break;
         }
#endif
         break;
      case(0x33):
         switch(rAH)
         {
            case(0x00):
               rv = 1;     /* Reset driver and read status */
               break;
            default:
               break;
         }
         break;
      //case(0x3F):          /* Overlay: Should this be more specific? */
      case(0xB6):          /* ROM Basic. Crashes most machines anyway... */
         rv = 1;
         break;
      default:
         break;
   }
   return rv;
}
/*=========================================================================*/


/*=========================================================================*/

/* Callback function, being called by <CodeRunner> every time
 * INT xx, INTO, INT3 and StepTrace are about to be executed.
 */

int   IntEmulator_Longer (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         int            IntNumber
         )
{
/*-------------------------------------------------------------------------*/
   int   RetCode = USER_INT_ABORT;
/*-------------------------------------------------------------------------*/

   //riad//
   _MinMaxTable.interrupt1 = IntNumber;   // for debug info
   _MinMaxTable.interrupt2 = rAH;         // for debug info

#ifdef   CR_DEBUG
   if (cr_hd && cr_itrace) fprintf(cr_dbgf, "\n");
   if (cr_hd) fprintf (cr_dbgf, "INT %02Xh Called. "
                       "AX=%04Xh, BX=%04Xh, CX=%04Xh, DX=%04Xh, DS=%04Xh, ES=%04Xh.\n",
                       IntNumber, rAX, rBX, rCX, rDX, rDS, rES);
#endif

   //printf ("INT %02Xh Called. "
   //                    "AX=%04Xh, BX=%04Xh, CX=%04Xh, DX=%04Xh, DS=%04Xh, ES=%04Xh.\n",
   //                    IntNumber, rAX, rBX, rCX, rDX, rDS, rES);

   RetCode = USER_INT_YOURSELF;

#ifdef CR_TSR
   if(1)
   //if(!dont_test_tsr)
   {
      /* jce */
      /* if we come across a TSR or exit call, try to exercise the hooked
         interrupt if int 21 has been altered (otherwise, stop) */
      if( (IntNumber == 0x21 && rAH == 0x31) || (IntNumber == 27) 
         || (IntNumber == 0x21 && rAH == 0x4c)
         || IntNumber == 0x20)
         {
            if( (*(WORD *)(pMachState->_IVT + 0x21*4) != 0xcf09 
               || *(WORD *)(pMachState->_IVT + 0x21*4 + 2) != 0xf310))
            { 
               // if tested_tsr, it will be aborted anyway
               return USER_INT_TSR; 
            }
            else 
            {
               // no need to test the TSR thingy if the interrupt tables
               // didn't change
               return USER_INT_ABORT; 
            }
         }
      /* end jce */
   }
#endif

   if (NotNOPInt_Longer(pMachState, pRetData, IntNumber))
      RetCode = USER_INT_ABORT;
   switch (IntNumber)
      {
      case  0x21:
         switch (rAH)
            {
/*jce*/     case  0x06:  /* jce */  /* direct console I/O */
               if(rDL==0xFF)  /* FF in DL means get a char */
               RetCode = USER_INT_ABORT;
               else  /* other in DL means output */
               RetCode = USER_INT_CONTINUE;  
               break;
/*end jce*/

            case  0x09:
               break;

            case  0x0B: /* Get stdin status.                               */
               rAL = 0; /* No characters available.                        */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x19: /* Get current disk.                               */
               rAL = 0;
               RetCode = USER_INT_CONTINUE;
               break;

/*jce*/
            case 0x1D:   /* null function for cpm compat. */
            case 0x1E:
               rAL = 0;
               RetCode = USER_INT_CONTINUE;
               break;
#ifdef CR_TSR
            case  0x25: /* jce */ /* Set INT Vector   */
               //if(dont_test_tsr) break;
               if((rAL*4+2) >= (VECTORS_IN_IVT*4 + MAX_SIZE_SIGN))
               {
                  // to avoid addressing illegale memory areas */
                  RetCode = USER_INT_ABORT;
                  break;
               }
               *(WORD *)(pMachState->_IVT + rAL*4 + 2) = rDS;
               *(WORD *)(pMachState->_IVT + rAL*4) = rDX;
               if(rAL == 0x08)   RetCode = USER_INT_ABORT;
               else              RetCode = USER_INT_CONTINUE;
               break;
#endif
/* end jce */

            case  0x2A: /* Get DOS date.                                   */
               rAL = 0x03;
               rCX = 0x07CA;
               rDX = 0x081F;
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x2C: /* Get DOS time.                                   */
               rCX = 0x130D;
               rDX = 0x2361;
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x2E: /* Set verify flag. Used by "KHA" virus.           */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x30: /* Get DOS version.                                */
               rAX = 0x0205;  /* jce - uncommented to return real dos ver  */
               /*rAX = 0x0000;*//* Cause legit programs to abort immediately.*/
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x33: /* Get misc stuff.                                 */
               switch(rAL)
                  {
                  case(0x00):
                     rDL = 0x00; /* Get current extended break state */
                     RetCode = USER_INT_CONTINUE;
                     break;
                  case(0x05):    /* Get boot drive */
                     rDL = 0x02; /* C: */
                     RetCode = USER_INT_CONTINUE;
                     break;
                  default:
                     break;
                  }
               break;
#ifdef CR_TSR
            case  0x35: /* Get INT Vector.                                 */
               //if(dont_test_tsr) break;
               if((rAL*4+2) >= (VECTORS_IN_IVT*4 + MAX_SIZE_SIGN))
               {
                  // to avoid addressing illegale memory areas */
                  RetCode = USER_INT_ABORT;
                  break;
               }
               rES = *(WORD *)(pMachState->_IVT + rAL*4 + 2);
               rBX = *(WORD *)(pMachState->_IVT + rAL*4);
               RetCode = USER_INT_CONTINUE;
               break;
#endif

/* jce */
            case 0x39:  /* mkdir */
            case 0x3a:  /* rmdir */
            case 0x3b:  /* chdir */
            case 0x3c:  /* create/truncate file */
            case 0x3d:  /* open file */
            case 0x3e:  /* close file */
            case 0x41:  /* delete file */
               rAX = 0x100;   /* random file handle/"AX destroyed" */
               clrCF;   /* indicate success */
               RetCode = USER_INT_CONTINUE;
               break;

            case 0x3f:  /* read from file/device */
            case 0x40:
               RetCode = USER_INT_CONTINUE;
               rAX = rCX;   /* claim we read/wrote amount requested */
               clrCF;       /* indicate success */
               break;

            case 0x42:  /* lseek */
               if(rAL == 0 || rAL == 1) /* beginning || current */
               {
                  rAX = rDX;
                  rDX = rCX;
               }
               else if(rAL == 2)  /* end */
               { /* fake 0x12345 length file */
                  rAX = (WORD)((0x12345 - ((ULONG)rCX)<<16+(ULONG)(rDX))>>16);
                  rDX = (WORD)((0x12345 - ((ULONG)rCX)<<16+(ULONG)(rDX))&0x0000ffff);
               }
               clrCF;  /* indicate success */
               break;
/* end jce */

            case  0x48: /* Allocate memory.                                */
/* riad */
               //GetFreeMemoryBlock(pMachState->pMemA, rBX*16, &rAX, NULL);
               rAX = 0x0100;
               rBX = 0x00A1;
/* end riad */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x4B: /* Exec.                                           */
               if (rAL > 0x10)
                  {
                  setCF;
                  RetCode = USER_INT_CONTINUE;
                  }
               else
                  {
                  clrCF;
                  RetCode = USER_INT_CONTINUE;
                  }
               break;

            case  0x4D: /* Get RC of last-terminated process               */
               rAL = 0x0000;   /* Worked fine */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x54: /* Get verify flag                                 */
               rAX = 0x00;   /* Worked fine - verify off */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x58: /* Get memory allocation strategy                  */
               rAX = 0x0000;   /* Worked fine - low memory first fit */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x51: /* Get PSP segment                                 */
            case  0x62: /* Get PSP segment                                 */
               rBX = 0x3000-0x10;   /* Worked fine */
               RetCode = USER_INT_CONTINUE;
               break;
            //case  0x18:
            //case  0x1D:
            //case  0x1E:
            //case  0x20:
            //case  0x61:
            case  0xF9:
            case  0xFA:
            case  0xFB:
            case  0xFC:
            case  0xFD:
            case  0xFE:
            case  0xFF:
               rAL = 0x00;          /* Report success */
               RetCode = USER_INT_CONTINUE;
               break;
            default:
               if ((WORD)rAH >= 0x70)
               {
                  rAL = 0x00;          /* Report success */
                  RetCode = USER_INT_CONTINUE;
                  break;
               }
            }
         break;
/* jce */
      case 0x2F:
         if(rAX==0x1200)  /* DOS install check */
         rAL=0xff;      
         RetCode = USER_INT_CONTINUE;
         break;
/* end jce */

      default:
         break;
      }

#ifdef CR_TSR
   if(!dont_test_tsr)
   {
   /* jce */
      if( (*(WORD *)(pMachState->_IVT + IntNumber*4) != 0xcf09
         || *(WORD *)(pMachState->_IVT + IntNumber*4 + 2) != 0xf310))
         {
            if (RetCode == USER_INT_CONTINUE 
               && !(IntNumber==0x21 && rAH==0x25 && rAL==IntNumber))
            {
            /* if the interrupt has changed from initial value, then
               the program has hooked it itself--let the program code run
               instead of handling it ourselves -- unless this is an int 21
               call to actually hook the interrupt */
               RetCode = USER_INT_YOURSELF;  
            }
   #ifdef CR_DEBUG
            if(RetCode == USER_INT_YOURSELF && cr_itrace)
               fprintf(cr_dbgf,"Using hooked interrupt.\n");
   #endif
         }
   /* end jce */
   }
#endif


   return (RetCode);
}
/*=========================================================================*/

/*=========================================================================*/

/* Function called by IntEmulator to determine whether an interrupt is
 * in the list of interrupts that we don't continue execution on.
 * Returns
 * 0   : we should continue execution. (Interrupt is a potential nop.)
 * 1   : we should stop execution. (Interrupt is not a potential nop.)
 */
int   NotNOPInt_Short (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         int            IntNumber
         )
{
   int rv = 0;

   switch(IntNumber)
   {
    /*case(0x02):*/
    /*case(0x04):*/
      case(0x05):          /* Print screen */
    /*case(0x06):*/
    /*case(0x07):*/
    /*case(0x09):*/
    /*case(0x0A):*/
    /*case(0x0B):*/
    /*case(0x0C):*/
    /*case(0x0D):*/
    /*case(0x0E):*/
    /*case(0x0F):*/
         rv = 1;
         break;
      case(0x10):
         switch (rAH)
         {
            case(0x00):    /* Set video mode. */
            case(0x01):    /* Set text mode cursor shape */
            case(0x02):    /* Set cursor position */
            case(0x05):    /* Set display page */
            case(0x06):    /* Scroll up */
            case(0x07):    /* Scroll down */
            case(0x09):    /* Write character + attribute */
            case(0x0A):    /* Write character only */
            case(0x0B):    /* Set border or background */
            case(0x0C):    /* Write pixel */
            case(0x0E):    /* Write character teletype mode */
            case(0x10):    /* Various video sets, and a few gets. */
            case(0x13):    /* Write string */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x13):
         switch(rAH)
         {
            case(0x03):    /* Write sector(s) */
            case(0x05):    /* Format track */
            case(0x06):    /* Format track bad */
            case(0x07):    /* Format drive */
            case(0x0B):    /* Write long sector(s) */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x14):
         switch(rAH)
         {
            case(0x00):    /* Initialize serial port */
            case(0x01):    /* Write character to port */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x16):
         switch(rAH)
         {
            case(0x00):    /* Get keystroke */
            case(0x03):    /* Set typematic rate/delay */
            case(0x10):    /* Get enhanced keystroke */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x17):
         switch(rAH)
         {
            case(0x00):    /* Printer write character */
            case(0x01):    /* Printer initialize port */
            case(0x03):    /* Printer print string */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x19):          /* Reboot */
         rv = 1;
         break;
      case(0x1A):
         switch(rAH)
         {
            case(0x01):    /* Set system time */
            case(0x03):    /* Set real-time clock */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x1B):          /* Ctrl-Break handler */
         rv = 1;
         break;
      case(0x20):          /* Terminate program */
         rv = 1;
         break;
      case(0x21):
         switch(rAH)
         {
            case(0x00):    /* Terminate */
            case(0x02):    /* Write character to stdout */
            case(0x04):    /* Write character to stdaux */
            case(0x05):    /* Write character to printer */
            case(0x06):    /* Direct console output */
            case(0x07):    /* Direct character input */
            case(0x08):    /* Direct character input without echo */
            case(0x09):    /* Write string to standard output */
            case(0x0E):    /* Select default drive */
            case(0x0F):    /* FCB open file */
            case(0x10):    /* FCB close file */
            case(0x11):    /* FCB findfirst */
          /*case(0x12):*/  /* FCB findnext */
            case(0x13):    /* FCB delete */
          /*case(0x14):*/  /* FCB read */
            case(0x15):    /* FCB write */
            case(0x16):    /* FCB create/truncate */
            case(0x17):    /* FCB rename */
				case(0x1A):		/* Set DTA */
          /*case(0x21):*/  /* FCB Random Record Read */
            case(0x22):    /* FCB Random Record Write */
				case(0x25):		/* Set interrupt vector */
            case(0x26):    /* Create new PSP */
          /*case(0x27):*/  /* FCB Random Block Read. */
            case(0x28):    /* FCB Random Block Write. */
            case(0x2B):    /* Set system date. */
            case(0x2D):    /* Set system time. */
            case(0x31):    /* TSR. */
               rv = 1;
               break;
            case(0x33):    /* Gets/Sets various. */
               switch(rAL)
               {
                  case(0x01):
                  case(0x02):
                     rv = 1;
                     break;
                  default:
                     break;
               }
               break;
            case(0x39):    /* Create subdirectory */
            case(0x3A):    /* Remove subdirectory */
            case(0x3B):    /* Set current directory */
            case(0x3C):    /* Create or truncate file */
            case(0x3D):    /* Open file */
          /*case(0x3E):*/  /* Close file */
           		rv = 1;
               break;
            case(0x3F):    /* Read file */
               switch(rBX)
               {
                  case(0x0000):
                     rv = 1; /* User would notice read */
                             /* from standard input */
                     break;
                  default:
                     break;
               }
            case(0x40):    /* Write file */
            case(0x41):    /* Delete file */
          /*case(0x42):*/  /* Lseek */
          /*case(0x43):*/  /* Get/Set file attributes */
            case(0x44):    /* IOCTL */
          /*case(0x45):*/  /* Duplicate file handle */
          /*case(0x46):*/  /* Force duplicate file handle */
          /*case(0x48):*/  /* Allocate memory */
          /*case(0x49):*/  /* Free memory */
            case(0x4A):    /* Resize memory block */
            case(0x4B):    /* Load and execute program */
            case(0x4C):    /* Terminate */
				case(0x4E):		/* Findfirst */
          /*case(0x4F):*/  /* Findnext */
            case(0x50):    /* Set PID */
            case(0x55):    /* Create child PSP */
            case(0x56):    /* Rename file */
          /*case(0x57):*/  /* Get/set file date/time */
            case(0x60):    /* Make true name */
               rv = 1;
               break;
            default:
               break;
         }
         break;
      case(0x23):          /* Ctrl-Break handler */
      case(0x24):          /* Critical error handler */
      case(0x26):          /* Absolute disk write */
      case(0x27):          /* Obsolete TSR */
      case(0x29):          /* Fast console output */
         rv = 1;
         break;
      case(0x2F):          /* Multiplex */
#if 0
         switch(rAH)
         {
            case(0x10):    /* SHARE, Nightfall viruses */
               break;
            default:
               if (rAL != 0)     /* Abort if not installation check. */
                  rv = 1;        /* This is may be too strong! */
               break;
         }
#endif
         break;
      case(0x33):
         switch(rAH)
         {
            case(0x00):
               rv = 1;     /* Reset driver and read status */
               break;
            default:
               break;
         }
         break;
      /* Some Nutcracker uses 0x3F as a NOP */
      /*case(0x3F):*/          /* Overlay: Should this be more specific? */
      case(0xB6):          /* ROM Basic. Crashes most machines anyway... */
         rv = 1;
         break;
      default:
         break;
   }
   return rv;
}
/*=========================================================================*/


/*=========================================================================*/

/* Callback function, being called by <CodeRunner> every time
 * INT xx, INTO, INT3 and StepTrace are about to be executed.
 */

int   IntEmulator_Short (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         int            IntNumber
         )
{
/*-------------------------------------------------------------------------*/
   int   RetCode = USER_INT_ABORT;
/*-------------------------------------------------------------------------*/

   /* riad */
   _MinMaxTable.interrupt1 = IntNumber;   // for debug info
   _MinMaxTable.interrupt2 = rAH;         // for debug info

#ifdef   CR_DEBUG
   if (cr_hd && cr_itrace) fprintf(cr_dbgf, "\n");
   if (cr_hd) fprintf (cr_dbgf, "INT %02Xh Called. "
                       "AX=%04Xh, BX=%04Xh, CX=%04Xh, DX=%04Xh, DS=%04Xh, ES=%04Xh.\n",
                       IntNumber, rAX, rBX, rCX, rDX, rDS, rES);
#endif

#ifdef CR_MOREFEATURES /* riad */
   /* If an interrupt has been modified (hooked), use it rather than
    * the conventionnal ones.
    */
   /* jce */
   if( (*(WORD *)(pMachState->_IVT + IntNumber*4) != 0xcf09
      || *(WORD *)(pMachState->_IVT + IntNumber*4 + 2) != 0xf310))
      {
      if (!(IntNumber==0x21 && rAH==0x25 && rAL==IntNumber))
         {
         /* if the interrupt has changed from initial value, then
          * the program has hooked it itself--let the program code run
          * instead of handling it ourselves -- unless this is an int 21
          * call to actually hook the interrupt 
          */
         RetCode = USER_INT_YOURSELF;  
         }
   #ifdef CR_DEBUG
      if(RetCode == USER_INT_YOURSELF && cr_itrace)
         fprintf(cr_dbgf,"Using hooked interrupt.\n");
   #endif

      /* Check again in case some other conditions are added */
      if(RetCode == USER_INT_YOURSELF) return RetCode;
      }
   /* end jce */
#endif

   RetCode = USER_INT_YOURSELF;

   if (NotNOPInt_Short(pMachState, pRetData, IntNumber))
      RetCode = USER_INT_ABORT;
   switch (IntNumber)
      {
      case  0x21:
         switch (rAH)
            {
            case  0x0B: /* Get stdin status.                               */
               rAL = 0; /* No characters available.                        */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x19: /* Get current disk.                               */
               rAL = 0;
               RetCode = USER_INT_CONTINUE;
               break;

#ifdef CR_MOREFEATURES
            case  0x25: /* jce */ /* Set INT Vector   */
               //if(dont_test_tsr) break;
               if((rAL*4+2) >= (VECTORS_IN_IVT*4 + MAX_SIZE_SIGN))
               {
                  // to avoid addressing illegale memory areas */
                  RetCode = USER_INT_ABORT;
                  break;
               }
               *(WORD *)(pMachState->_IVT + rAL*4 + 2) = rDS;
               *(WORD *)(pMachState->_IVT + rAL*4) = rDX;
               RetCode = USER_INT_CONTINUE;
               break;
#endif
            case  0x2A: /* Get DOS date.                                   */
               rAL = 0x03;
               rCX = 0x07CA;
               rDX = 0x081F;
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x2C: /* Get DOS time.                                   */
               rCX = 0x130D;
               rDX = 0x2361;
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x2E: /* Set verify flag. Used by "KHA" virus.           */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x30: /* Get DOS version.                                */
               /*rAX = 0x0205;*/
               rAX = 0x0000; /* Cause legit programs to abort immediately. */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x33: /* Get misc stuff.                                 */
               switch(rAL)
                  {
                  case(0x00):
                     rDL = 0x00; /* Get current extended break state */
                     RetCode = USER_INT_CONTINUE;
                     break;
                  case(0x05):    /* Get boot drive */
                     rDL = 0x02; /* C: */
                     RetCode = USER_INT_CONTINUE;
                     break;
                  default:
                     break;
                  }
               break;

            case  0x35: /* Get INT Vector.                                 */
               rES = *(WORD *)(pMachState->_IVT + rAL*4 + 2);
               rBX = *(WORD *)(pMachState->_IVT + rAL*4);
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x48: /* Allocate memory.                                */
               rAX = 0x0008;
               rBX = 0x00A1;
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x4B: /* Exec.                                           */
               if (rAL > 0x10)
                  {
                  setCF;
                  RetCode = USER_INT_CONTINUE;
                  }
               break;

            case  0x4D: /* Get RC of last-terminated process               */
               rAL = 0x0000;   /* Worked fine */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x54: /* Get verify flag                                 */
               rAX = 0x00;   /* Worked fine - verify off */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x58: /* Get memory allocation strategy                  */
               rAX = 0x0000;   /* Worked fine - low memory first fit */
               RetCode = USER_INT_CONTINUE;
               break;

            case  0x51: /* Get PSP segment                                 */
            case  0x62: /* Get PSP segment                                 */
               rBX = 0x3000-0x10;   /* Worked fine */
               RetCode = USER_INT_CONTINUE;
               break;
            case  0x18:
            case  0x1D:
            case  0x1E:
            case  0x20:
            case  0x61:
            case  0xF9:
            case  0xFA:
            case  0xFB:
            case  0xFC:
            case  0xFD:
            case  0xFE:
            case  0xFF:
               rAL = 0x00;          /* Report success */
               RetCode = USER_INT_CONTINUE;
               break;
            default:
               if ((WORD)rAH >= 0x70)
               {
                  rAL = 0x00;          /* Report success */
                  RetCode = USER_INT_CONTINUE;
                  break;
               }
            }
         break;

      default:
         break;
      }

   return (RetCode);
}
/*=========================================================================*/


/*=========================================================================*/
/*
 * This callback function will be called by <CodeRunner> before each
 * instruction.
 * Returns TRUE if Break Condition is generated and is time to stop
 * interpreting of code, FALSE otherwise.
 */

int   BreakConditionCheck (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData)
{
/*-------------------------------------------------------------------------*/
   int            RetCode = FALSE;

#ifdef CR_MOREFEATURES  //riad//
   unsigned int   check       = 0;  // temporary value for stopcond check
   int            state       = _MinMaxTable.state;
   ULONG          current     = LinearAddr(pRetData->PrevCS, pRetData->PrevIP);
   MINMAX         *otherindex;
   MINMAX         *curindex   = _MinMaxTable.curindex;

#endif
/*-------------------------------------------------------------------------*/

#if WATC
   if (pRetData->ops % 10 == 0)
   {
      TEST_FOR_TERMINATION;
   }
#endif

#ifdef CR_MOREFEATURES  //riad//

   // This relatively big section isn't executed often when 
   // the emulator in emulating a decryptor, because we know
   // that we are executing an "already executed code" (this state
   // will be checked again after each jump/return/call/...etc).
   // So often, state == MODEIDLE

   if(state != MODEIDLE)            // mode PLUS or JUMP
   {
      // We don't test the 'present' flag when checking curindex
      // Save time (at the begining, min = max = -1
      // This stuff is added just for optimization
      if(current >= _MinMaxTable.curindex->min && current <= _MinMaxTable.curindex->max) 
      {
         _MinMaxTable.state = MODEIDLE;
         goto finish;
      }

      if((otherindex=is_in_exe_section(current)) != NULL)
      {
         _MinMaxTable.state = MODEIDLE;
         _MinMaxTable.curindex = otherindex;
         if(state == MODEJUMP) goto finish;
         if(curindex != otherindex)
         {
            // paranoia check (all this paranoia is because changing machine state
            // is kinda tricky, so for safety, report errors as soon as possible
            // and fix bugs afterward)
            if(((curindex->max+5)<otherindex->min) || (curindex->min>(otherindex->max+5)))
               fprintf(stderr, "\n\tDamned ! May be a very bad error in breackcheck()"
                               "\n\n");
// Fred: Just removed it for the demo, but the WARNING should be back into CR later

            otherindex->min = __min(curindex->min, otherindex->min);
            otherindex->max = __max(curindex->max, otherindex->max);
            curindex->present = -1;
            _MinMaxTable.nsections--;
         }
         goto finish;
      }

      if(state == MODEJUMP)
      {
         // not in Table -> create new, update
         if((otherindex=get_new_exe_section()) == NULL)
         {
            // this shouldn't happen. well, time to get off
#ifdef MYDEBUG
            fprintf(stderr, "(***) get_in_table() failed\n");
#endif
#ifndef CR_DO_SCANNER
            _nget_in_table++; // debuging information
#endif
            // we can merge some or remove some (the small ones)
            // sometimes, in SMEG, we need to merge close sections
            RetCode = TRUE;
         }
         else
         {
            otherindex->min = current;
            // codedata stuff
            otherindex->max = current + _OpcodeSize - 1;
            otherindex->present = 1;
            _MinMaxTable.curindex = otherindex;
            _MinMaxTable.state = MODEPLUS;
            _MinMaxTable.NumOfDiffInst++;
            // codedata stuff
            _MinMaxTable.TotDiffInst++;
            _MinMaxTable.nsections++;
            otherindex->offsetfromfile = -1L;
         }
         goto finish;
      }

      if(state == MODEPLUS )
      {
         _MinMaxTable.NumOfDiffInst++;
         // codedata stuff
         _MinMaxTable.TotDiffInst++;
         _MinMaxTable.state = MODEPLUS;
         // codedata stuff
         curindex->max = current + _OpcodeSize -1;
      }

   }

finish:

   if(_MinMaxTable.dojump != MODEIDLE) 
   {
      _MinMaxTable.state = _MinMaxTable.dojump;
      _MinMaxTable.dojump = MODEIDLE;
   }

   // 
   // The Code down here is very important. It determines whether we have
   // to stop emulating or no.
   //
   // If I notice that the number of DiffInstructions doesn't change, it means
   // that I am inside a loop. In this special case, I count NumOfRealActive
   // which represent the active instructions located in this "loop".
   // If this number becomes high enough (like 50), we can suspect a DECRYPTOR.
   // But it has to be higher so we can stop emulating (like 200).
   // If it's higher than 50 or 200 (just as an exemple), we reset the counter
   // NumOfRealActive only when we find another RealActiveInstruction 
   // happening OUTSIDE the loop (not in the known sections). Because,
   // sometimes, SMEG or others execute jumps+stuff and then come back to the
   // decryptor.
   //
   //   Remark 1:
   //   But if a decryptor happens to be smart enough to execute new RealActive
   //   outside the "current loop" but being part of a bigger loop, so we have
   //   to give the emulator another chance by not reseting NumOfActiveInst if
   //   we are already at 50 (but less than 200, where in this case, we stop)
   //
   //   Remark 2:
   //   Regarding the two-layer polymorphic viruses, there is a change to fix
   //   that if we see that we are decrypting the same region again (so we won't
   //   count it as RealActive, though, we continue to emulate).
   //   Also, if we see that we are continuing to decrypte the same region, 
   //   we won't stop. (NOT DONE YET)
   //

   // Ok, let's see if we have a Real Active Instruction
   // This section is almost always executed when code is in loop, to optimize

   if(_MinMaxTable.NumOfLastDiffInst == _MinMaxTable.NumOfDiffInst)
   {
      if((pRetData->ulRetCode & CODE_SELF_MODIFIED))
      {
         if(_ActiveInstructionFlag)
         {
            // I reset (or divide) total_rw_traps from time to time (each 50 
            // active instructions. For exemple). This allow some decyptors 
            // decrypting further even if the total number of RW has reached the maximum.
            // Also, it will execute the first time an Active occurs.
#ifdef CR_MYDEBUG
            if((_MinMaxTable.NumOfRealActiveInst % MIN_ACTIVE_SUSPICIOUS) == 0)
            {
               if(_MinMaxTable.NumOfRealActiveInst <= MIN_ACTIVE_SUSPICIOUS)
               {                  
                  if(_MinMaxTable.NumOfRealActiveInst == MIN_ACTIVE_SUSPICIOUS)
                  {
                     // i know this is weak, but i put it just to see, improve later
                     // coz in fact, i have to do that when all precedent were = 3
                     if(_MinMaxTable.ChangedDecryptor == 2) ++_MinMaxTable.NumOfDiffDecryptors;
                     _MinMaxTable.ChangedDecryptor = 0;
                  }
               }
               else  
                  if((_MinMaxTable.NumOfRealActiveInst % (2*MIN_ACTIVE_SUSPICIOUS)) == 0)
                     total_rw_traps >>= 1;   // divide by 2 (faster than divide)
            }
            if(_MinMaxTable.ChangedDecryptor == 1)
            {
               // Are we still in the same situation ? (a new decryptor, or we are
               // at the begining)
               if(_ActiveInstructionFlag == 3 || _MinMaxTable.NumOfDiffDecryptors == 0)
               {
                  _MinMaxTable.ChangedDecryptor = 2; // second mode
                  _MinMaxTable.NumOfRealActiveInst = 1;
               }
               else _MinMaxTable.ChangedDecryptor = 0;
            }
#else  /* CR_MYDEBUG */
            if((_MinMaxTable.NumOfRealActiveInst % (2*MIN_ACTIVE_SUSPICIOUS)) == 0)
            {
               if(_MinMaxTable.NumOfRealActiveInst) total_rw_traps >>= 1;
            }
#endif /* CR_MYDEBUG */

            // When Lastdiff = CurrentDiff, we won't accept something else than
            // ActiveFlag = 2 (expanding a decrypted zone), I added 3 too.
            // Anyway, we will miss some, but this won't stop the emulator
            if(_ActiveInstructionFlag /*== 2*/ != 1) _MinMaxTable.NumOfRealActiveInst++;

            // I found a virus that writes in 4 locations outside the loop
            // then, after starting the loop, it will have 4 idleloops
            // because it's SELF_MODIFE, but not ActiveFlag

            // codedata stuff
            if(_ActiveInstructionFlag != 3) _MinMaxTable.NumOfRealIdleInst = 0;

            _IdleInstructions = 0;
            _ActiveInstructions++;
            check = 1;
         }
         else 
            _IdleInstructions++;

         _IdleLoops = 0;
      }
      else _IdleInstructions++;
      
      // codedata stuff
      if(_ActiveInstructionFlag == 3 || _ActiveInstructionFlag == 0) 
         _MinMaxTable.NumOfRealIdleInst++;
   }
   else
   {
      // Sometimes, SMEG is still decrypting, but now executing a new 
      // instruction, and then back to the decryptor, take care of this
      // if(_MinMaxTable.NumOfRealActiveInst >= 50L) 
      // {
         // may be we can merge some close sections
         // and keep RealActive the same
         // so if really we are getting away from the 
         // decryptor, it would be because we are
         // executing a complete new section of code
         // and in this case, we can know that.
      // }
      _MinMaxTable.NumOfLastDiffInst = _MinMaxTable.NumOfDiffInst;

      // Here, NumOfRealIdle is related to RealActive, not to ActiveInst
      if((pRetData->ulRetCode & CODE_SELF_MODIFIED) && _ActiveInstructionFlag)
      {
         if(_MinMaxTable.NumOfRealActiveInst >= MIN_ACTIVE_SUSPICIOUS) 
         {
            // Something SUSPICIOUS
            _flagBufferDegarbled = TRUE;
            // OK, If i want to emulate far away after the first decryptor finished, 
            // I can continue, I won't stop until IdleInst or an Interupt tells me so.
            if(!_Fulldecryption)
            {
               if(_MinMaxTable.NumOfRealActiveInst >= MIN_ACTIVE_STOP) 
                  RetCode = TRUE;
            }
         }
         // if >= 20, we continue expecting more RealActive in the same
         // decryptor but after executing one or two new instruction.
         // I comented this out...
         // else if(RetCode != TRUE) _MinMaxTable.NumOfRealActiveInst = 0;
#ifdef CR_MYDEBUG         
         // Decrypting the same body we decrypted before...
         if(_ActiveInstructionFlag == 3)
         {
            // Yet again, another miserable flag, for test only, no stopcond on it
            _MinMaxTable.ChangedDecryptor = 1;
         }
#endif
         _ActiveInstructions++;
         // Hehe, this is a cool trick. While it's still ok with the good files 
         // (cica/simtel/hobbes), it's better for viruses (Satanbug for exemple)
         // Basicly, if we are collecting previous IdleInst, we would like to go
         // until 1400. But if we considere IdleInst alone, 700 is OK.
         // it has been commented for codedata stuff
         //_MinMaxTable.NumOfRealIdleInst += (_IdleInstructions >> 1);
         _IdleInstructions = 0;
         _IdleLoops = 0;
         check = 1;
      }
      else _IdleInstructions++;

      // codedata stuff
      _MinMaxTable.NumOfRealIdleInst = 0;
   }

   if(check)
   {
      /* enough active instructions ? */
      if (_ActiveInstructions >= _Default_ini.MaxActiveInstructions)
      {
#ifdef   CR_DEBUG
         if (cr_hd) fprintf (cr_dbgf, 
            "\nTerminating: _ActiveInstructions >= _Default_ini.MaxActiveInstructions\n");
#endif
         _flagBufferDegarbled = TRUE;
         RetCode = TRUE;
      }     
   }

   check = _IdleInstructions;

   if (check >= _Default_ini.MaxIdleInstructions)
   {
      // This trick down here is tricky and really working like hell
      // Let's check if the idle sections contain many loops
      // If so, -> likely to be a good file, STOP
      // If no, like a garbage of a dumb polymorphic virus, let's emulate further
      if((_MinMaxTable.NumOfDiffInst + 300) > check)
      {
         if(check < 4*_Default_ini.MaxIdleInstructions && /* or 2*() */
            pRetData->ops < (ULONG)(3*_Default_ini.MaxIdleInstructions))
         {
            goto skip_idle;
         }
      }
#ifdef   CR_DEBUG
      if (cr_hd) fprintf (cr_dbgf, 
         "\nTerminating: _IdleInstructions >= _Default_ini.MaxIdleInstructions\n");
#endif
      RetCode = TRUE;
   }

skip_idle:

#ifdef MYDEBUG

   if((pRetData->ulRetCode & CODE_SELF_MODIFIED))
   {
      if(_ActiveInstructionFlag)
         fprintf(mydebug2, "\n\nREAL (%04X:%04X)\t(%02X %02X)", pRetData->PrevCS, pRetData->PrevIP, 
            *(pMachState->pOp1), *(pMachState->pOp2));
      else
         fprintf(mydebug2, "\n\nIDLE (%04X:%04X)\t(%02X %02X)", pRetData->PrevCS, pRetData->PrevIP, 
            *(pMachState->pOp1), *(pMachState->pOp2));
   }
   
#endif

   pRetData->ulRetCode &= (~CODE_SELF_MODIFIED);

#else    /* CR_MOREFEATURES */

#endif   //riad//

   return (RetCode);

}
/*=========================================================================*/


/*=========================================================================*/
/*
 * This callback function will be called by <CodeRunner> before each
 * instruction, potentially capable of changing CS or IP.
 * Returns:
 * USER_EXEC_CONTINUE   0     Continue normal execution of this
 *                            instruction.
 * USER_EXEC_SKIP       2     Do not execute this instruction.
 *                            Skip it and continue execution
 *                            from the next instruction.
 * USER_EXEC_ABORT     -1     Abort everything and exit.
 *                            Will cause <USER_ABORT> flag to be
 *                            set in return code.
 */

int   ExecutionControl (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData)
{
/*-------------------------------------------------------------------------*/
   int            RetCode = USER_EXEC_CONTINUE;
   ULONG          LinearForJump = LinearAddr(rCS, rIP);

/*-------------------------------------------------------------------------*/

#ifdef CR_MOREFEATURES  //riad//

   int            state = MODEIDLE;
   jmp_stuff      *jumjum;

   // this stuff has to be optimized (because it's like done twice, here 
   // and in cr_runr1.c in the big switch
   switch (*(pMachState->pOp1))
      {
      case  0xE0: /* LOOPNZ.                                               */
         if( (rCX-1) && !fZF)       state = MODEJUMP; break;
      case  0xE1: /* LOOPZ.                                                */
         if( (rCX-1) && fZF)        state = MODEJUMP; break;
      case  0xE2: /* LOOP.                                                 */
         if((rCX-1))                state = MODEJUMP; break;
      case  0xE3: /* JCXZ xx                                               */
         if(!rCX)                   state = MODEJUMP; break;
      case  0x70: /* JO      xx                                            */
         if(fOF)                    state = MODEJUMP; break;
      case  0x71: /* JNO     xx                                            */
         if(!fOF)                   state = MODEJUMP; break;
      case  0x72: /* JB/JNAE xx                                            */
         if(fCF)                    state = MODEJUMP; break;
      case  0x73: /* JNB/JAE xx                                            */
         if(!fCF)                   state = MODEJUMP; break;
      case  0x74: /* JE/JZ   xx                                            */
         if(fZF)                    state = MODEJUMP; break;
      case  0x75: /* JNE/JNZ xx                                            */
         if(!fZF)                   state = MODEJUMP; break;
      case  0x76: /* JBE/JNA xx                                            */
         if(fCF || fZF)             state = MODEJUMP; break;
      case  0x77: /* JNBE/JA xx                                            */
         if(!fCF && !fZF)           state = MODEJUMP; break;
      case  0x78: /* JS      xx                                            */
         if(fSF)                    state = MODEJUMP; break;
      case  0x79: /* JNS     xx                                            */
         if(!fSF)                   state = MODEJUMP; break;
      case  0x7A: /* JP/JPE  xx                                            */
         if(fPF)                    state = MODEJUMP; break;
      case  0x7B: /* JNP/JPO xx                                            */
         if(!fPF)                   state = MODEJUMP; break;
      case  0x7C: /* JL/JNG  xx                                            */
         if(fSF != fOF)             state = MODEJUMP; break;
      case  0x7D: /* JNL/JGE xx                                            */
         if(fSF == fOF)             state = MODEJUMP; break;
      case  0x7E: /* JLE/JNG xx                                            */
         if(fZF  || (fSF != fOF))   state = MODEJUMP; break;
      case  0x7F: /* JNLE/JG xx                                            */
         if(!fZF && (fSF == fOF))   state = MODEJUMP; break;
      case  0xEB: /* JMP near8.                                            */
                                    state = MODEJUMP; break;
      default:    /* JMP LONG/FAR/CALL/RET/INT/IRET/etc                    */
                                    state = MODEJUMP; goto skipcheck;
      }

   /* those two lines below detect the jumps that act like NOP */
   if( (*(pMachState->pOp2) == 0x00))  state = MODEPLUS;
   if(state == MODEIDLE)               state = MODEPLUS;

   // for test information only
   if(*(pMachState->pOp1) != 0xEB) /* this is temporary */
   {
      if(is_in_exe_section(LinearForJump) == NULL && *(pMachState->pOp2) != 0x00) 
         _MinMaxTable.NumOfDiffLoops++;
   }

   /*******************************************************************
    *
    *          This part of the code deals with code data segregation
    *
    *******************************************************************/

   // Ok, here we have the JNZ and LOOP family, and JMP

   if(1)
   {
      #define MAX_TOLERATED_BEFORE_BAN 3

      // let's check the state of this particular unconditionnal jump
      // in the linked list, and decide whether we create another
      // "state machine" or no

      // first get the entry in the linked list of the jump

      jumjum = get_jmp_in_list(LinearForJump);

      if(jumjum != NULL)
      {
          // We have a new jump branch
         if(jumjum->jumpcond == -1)
         {
            jumjum->address = LinearForJump;

            // only when it's an unconditionnal jump
            if(*(pMachState->pOp1) >= 0x70 && *(pMachState->pOp1) <= 0x7F)
            {
               if(state == MODEJUMP)   jumjum->jumpcond = 1;   // jump branch ok
               else                    jumjum->jumpcond = 0;   // not done yet
            }
         }
         else if(jumjum->jumpcond != 2)
         {
            // to prevent slow decryptors to run several times, we cancel the
            // emulation of this particular state machine after we see that
            // NumOfRealIdleInst reached a maximum value.
            // It repesents the intructions where no new actives (in new memory
            // areas) are executed
            if((_MinMaxTable.NumOfRealIdleInst>_Default_ini.MaxIdleBeforeSkip) 
               && !(com_options&OPT_DECRYPT))
            {
               // i commented this because sometimes, a bugy virus (or bad 
               // emulation) runs a decryptor too much or indefinitly.
               // so even for the authentic first state machine, make a limit
               //if(_MinMaxTable.curmstuff->number > 2)
                  _MinMaxTable.RetCode |= USER_CANCEL;
                  return USER_EXEC_CANCEL;
            }

            if(state == MODEJUMP)
            {
               if(++(jumjum->counterup)>=_Default_ini.MaxDeepness)
               {
                  // is this jump banned, if yes, don't let it go
                  //if(jumjum->counterup>=4*_Max_Deep && jumjum->bannedup) 
                  //{
                  //   return USER_EXEC_ABORT;
                  //}
                  // check if we have to ban this particular jump
                  //else if(jumjum->counterup >= MAX_TOLERATED_BEFORE_BAN)
                  //{
                  //   jumjum->bannedup = 1;
                  //}

                  goto go_away;
               }
               // if previous state = 0, and we got a new state = 1
               // this means that both ways have been taken, so state = 2
               if(jumjum->jumpcond == 0)        jumjum->jumpcond = 2;
            }
            else
            {
               // the same situation as above, but in the other way
               if(++(jumjum->counterdown)>=_Default_ini.MaxDeepness)  
               {
                  goto go_away;
               }
               if(jumjum->jumpcond == 1)        jumjum->jumpcond = 2;
            }
         }

         if((jumjum->jumpcond == 0) || (jumjum->jumpcond == 1))
         {
            // ok, we may create another entry in the tree of the state
            // machine, continue emulating, and go to another state
            // machine when we are finnished with the current one
            _MinMaxTable.curjmp = jumjum;
            _MinMaxTable.codata = TEST_CODEDATA;
         }
         else if(jumjum->jumpcond == 2)
         {
            // somewhere, we have a jumpcond = 2, it means that we have 
            // to removes all entries where machine state is linked to
            // this jump (save memory and time and wealth)

            // No that the counterup/down are reset (they have to be, so
            // we know we are encountering this jump for the FIRST time
            // in THIS particular machine state (this jump is in state 2)
#if 1
            if(!(com_options&OPT_DECRYPT) && 
               !jumjum->counterup && 
               !jumjum->counterdown)
               if(jumjum->banned++ >= MAX_TOLERATED_BEFORE_BAN)
               {
                  _MinMaxTable.RetCode |= USER_CANCEL;
                  return USER_EXEC_CANCEL;
               }
#endif
         }
      }
      else
      {
         printf("\n\n\tDamned, not enough memory in get_jmp_in_list(), But let's continue\n\n");
      }
   }

go_away:
   
   /*******************************************************************/



   /* This if() checks whether instruction about to transfer control
    * "above".
    */
   if ((*(pMachState->pOp2)) & 0x80)
      {
      /* Check whether address of this instruction was already saved.
       */
      //riad//
      if(_IdleLoops == 0)
      {
         // reset to 0 all the _IdleLoops counter (which would have
         // happened after an Active Instruction
         jmp_stuff *cur;

         for(cur=_MinMaxTable.jmpstuff; cur != NULL; cur=cur->next)
            cur->j_IdleLoops = 0;
         //cur = _MinMaxTable.jmpstuff;
         //while(cur != NULL) { cur->_IdleLoops = 0; cur = cur->next; }
 
         _IdleLoops = 1;   // to avoid doing this over and over
      }
      //if (_IdleLoopLinearAddr == LinearForJump)
      if(jumjum != NULL)
         {
         /* This variable will be set in pfnBreak() to 0 whenever
          * memory will
          * be modified again.
          */
         //riad//
         jumjum->j_IdleLoops++;

         if (jumjum->j_IdleLoops >= _Default_ini.MaxIdleLoops)
            {
            RetCode    = USER_EXEC_SKIP;
            // debuging info
            //_IdleLoops = 0;

            /* For LOOP instruction - set CX==1 - so that
             * it will become 0 and loop will get terminated
             * right away.
             */
            if (
               (*(pMachState->pOp1))==0xE0 ||
               (*(pMachState->pOp1))==0xE1 ||
               (*(pMachState->pOp1))==0xE2
               )
               {
               RetCode = USER_EXEC_CONTINUE;
               rCX=1;
               }
            }
         }
      //else
      //   {
      //   /* Save address of new Jcond above_label instruction.
      //    */
      // //riad//
      //   _IdleLoopLinearAddr = LinearForJump;
      //   /* for just saved new address.            */
      //   _IdleLoops = 1;
      //   }
      }

skipcheck:

   _MinMaxTable.dojump = state;

#else    /* CR_MOREFEATURES */   //riad//

/*-----------------------------------------------------------------------------------------------*/

#endif

   return (RetCode);
}
/*=========================================================================*/


/*=========================================================================*/
/*
 * This function is called right before using the global variable _IVT.
 * It initializes the IVT to a sensible fill pattern.
 * Returns 0 == OK, 1 == out of memory.
 */

int   Init_IVT (MACHINE_STATE *pMachState)
{
/*-------------------------------------------------------------------------*/
   int            i;
/*-------------------------------------------------------------------------*/

   if (NULL == (pMachState->_IVT = malloc(VECTORS_IN_IVT*4 + MAX_SIZE_SIGN)))
      return 1;
   for (i=0; i<VECTORS_IN_IVT*4; i+=32)
      memcpy(&(pMachState->_IVT)[i], _IVT_FILL_PATTERN, 32);
   memset(&(pMachState->_IVT)[VECTORS_IN_IVT*4], 0, MAX_SIZE_SIGN);

   return (0);
}
/*=========================================================================*/

/*=========================================================================*/
/*
 * This function is called right before using the global variable _CR_PSP
 * It initializes the PSP to a sensible fill pattern.
 * Returns 0 == OK, 1 == out of memory.
 */

int   Init_PSP (MACHINE_STATE *pMachState)
{
   if (NULL == (pMachState->_CR_PSP = malloc(0x100 + MAX_SIZE_SIGN)))
      return 1;
   memcpy(pMachState->_CR_PSP, _CR_PSP_FILL_PATTERN, 0x80);
   memset(&(pMachState->_CR_PSP)[0x80], 0, 0x80);
   memset(&(pMachState->_CR_PSP)[0x100], 0, MAX_SIZE_SIGN);

   return (0);
}
/*=========================================================================*/
/*=========================================================================*/
/*
 * This function is called right before using the global variable _CR_PSP
 * It initializes the PSP to a sensible fill pattern.
 * Returns 0 == OK, 1 == out of memory.
 */

int   Init_BIOSDA (MACHINE_STATE *pMachState)
{
   if (NULL == (pMachState->_CR_BIOSDA = malloc(0x100 + MAX_SIZE_SIGN)))
      return 1;
   memcpy(pMachState->_CR_BIOSDA, _CR_BIOSDA_FILL_PATTERN, 0x100);
   memset(&(pMachState->_CR_BIOSDA)[0x100], 0, MAX_SIZE_SIGN);

   return (0);
}
/*==
/*=========================================================================*/
#ifdef CR_DO_SCANNER //riad//

/* This function is called to check buffers for viruses.
 * Returns:    SUCCESS
 *             ERROR_NOT_ENOUGH_MEMORY
 */
int   ScanBuffers (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         unsigned short dump_buffers
         )
{
   /*-------------------------------------------------------------------*/
   MEMORY_AREA  *pMA;
   BYTE         *pBuffToScan;
   BYTE         *pTmpBuff;
   WORD         BytesToScan;
   int          RetCode = SUCCESS;
   WORD         OffIntoBuf = 0;
   /*-------------------------------------------------------------------*/


   pMA = pMachState->pMemA; /* Start of linked list of memory areas. */

   do
      {
      if (((!_scanworkbuf_vars.scalpel_mode && (pMA->pBuff && pMA->minOffWr != 0xFFFF)) ||
          ( _scanworkbuf_vars.scalpel_mode &&  pMA->pBuff)
          )
         )
         {
         if (_scanworkbuf_vars.scalpel_mode)
            {
            if (pMA->freeable && pMA->FromFile) /* i.e. if we allocated, i.e. we haven't scanned it yet. */
               BytesToScan = (WORD)(pMA->BuffSize);
            else
               BytesToScan = (WORD)(pMA->maxOffWr - pMA->minOffWr + 1);
            }
         else
            {                  /* only scan changed regions. */
            BytesToScan = (WORD)(pMA->maxOffWr - pMA->minOffWr + 1);
            }

         if (BytesToScan > 8)
            {
            if (pMA->freeable)
               /* i.e. if we allocated, i.e. we haven't scanned it yet, and if we've
                  already extended it don't make it even bigger. */
               {
               if (!pMA->SizeExtended)
                  {
                  pTmpBuff = malloc(pMA->BuffSize+MAX_SIZE_SIGN-1);
                  if (pTmpBuff == NULL)
                     {
                     RetCode = ERROR_NOT_ENOUGH_MEMORY;
                     goto done_ScanBuffers;
                     /*break;*/
                     }
                  memcpy(pTmpBuff, pMA->pBuff, pMA->BuffSize);
                  memset(pMA->pBuff, 0, pMA->BuffSize);
                  free(pMA->pBuff); /* Zap out and free buffer */
                  pMA->pBuff = pTmpBuff;
                  pMA->SizeExtended = TRUE;
                  }
               if (_scanworkbuf_vars.scalpel_mode)
                  {
                  pBuffToScan = pMA->pBuff; /* Scan whole thing */
                  OffIntoBuf = 0;
                  }
               else
                  {
                  OffIntoBuf = pMA->minOffWr - pMA->Off;
                  pBuffToScan = pMA->pBuff+OffIntoBuf;
                  }
               }
            else
               {
               OffIntoBuf = pMA->minOffWr - pMA->Off;
               pBuffToScan = pMA->pBuff+OffIntoBuf;
               }
#ifdef CR_DEBUG
            if (cr_hd) fprintf(cr_dbgf, "Scanning buffer, linaddr=%08lX, length=%u (min=%u, max=%u)\n",
                               pMA->LinAddr, BytesToScan, pMA->minOffWr, pMA->maxOffWr);
            if (dump_buffers)
               {
               WORD i;
               for (i=0; i<BytesToScan; ++i)
                  {
                  if (i!=0 && (i%20==0)) if (cr_hd) fprintf(cr_dbgf, "\n");
                  if (cr_hd) fprintf(cr_dbgf, "%02X ", pBuffToScan[i]);
                  }
               if (BytesToScan%20 != 0) if (cr_hd) fprintf(cr_dbgf, "\n");
               }
#endif
            if (pMA->ModifiedNotScanned)
               {
               /* Now check pMA->pBuff to see if there are any lapping buffers. */
               /* If there are, then copy some extra past the top of the buffer. */
               if (pMA->freeable || pMA->FromFile)
                  {
                  MEMORY_AREA *pTMA;
                  for (pTMA=pMachState->pMemA; pTMA!=NULL; pTMA=pTMA->pNextArea)
                     {
                     if (pMA->LinAddr+(ULONG)pMA->BuffSize == pTMA->LinAddr)
                        {
                      /*printf("Lapping buffers, filling %u bytes (bs=%u).\n",
                               pTMA->BuffSize >= (MAX_SIZE_SIGN-1) ? (MAX_SIZE_SIGN-1) : pTMA->BuffSize,
                               pTMA->BuffSize);*/
                        memcpy(&pMA->pBuff[pMA->BuffSize],
                               pTMA->pBuff,
                               pTMA->BuffSize >= (MAX_SIZE_SIGN-1) ? (MAX_SIZE_SIGN-1) : pTMA->BuffSize);
                        }
                     }
                  }
#if WATC
               TEST_FOR_TERMINATION;
#endif
               /* This test is buried in here so that the debug code will display buffers even if */
               /* a virus has been found. */
               if (!found_precise_match)
                  {
                  scanworkbuf(BytesToScan,         /* scan_length,               */
                              _scanworkbuf_vars.filename,
                              _scanworkbuf_vars.is_exe_type,
                              pMA->OffIntoFile+OffIntoBuf,
                              _scanworkbuf_vars.object_type,
                              pBuffToScan,         /* buffer_to_be_scanned,      */
                              _scanworkbuf_vars.vp_name,
                              _scanworkbuf_vars.inside_compressed,
                              FALSE,
                              _scanworkbuf_vars.FileSize,
                              _scanworkbuf_vars.saved_epoint,
                              _scanworkbuf_vars.filename);
                  pMA->ModifiedNotScanned = 0;
                  }
               }
            }
         }
      pMA = pMA->pNextArea;
      }
   while (pMA);

done_ScanBuffers:
   return RetCode;
}
#endif   //riad//
/*=========================================================================*/
