/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_EXTS.H                                                *
 *                                                                         *
 * Description:   INTeL 80x86 interpreter.                                 *
 *                                                                         *
 *                Function prototypes for <CodeRunner>.                    *
 *                                                                         *
 *      Must be compiled with /dBIG_ENDIAN for big-endian machines.        *
 *      Must be compiled with /dAIX        for AIX.                        *
 *      Must be compiled with /dDOS        for DOS.                        *
 *      Must be compiled with /dOS2_16     for 16-bit OS2.                 *
 *      Must be compiled with /dOS2_32     for 32-bit OS2.                 *
 *                                                                         *
 ***************************************************************************/


#ifndef  _CR_EXTS_H_
#define  _CR_EXTS_H_


/*------------------- From CR_FSCAN.C   -----------------------------------*/

extern   int         filerun_and_scan (
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
         unsigned short int scalpel_mode,          /* Scan more buffers if */
         ULONG              saved_epoint           /* ept offset into file */
         );

/* riad */
extern   int         IntEmulator_Short (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         int            IntNumber
         );

extern   int         IntEmulator_Longer (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         int            IntNumber
         );

extern   int         BreakConditionCheck (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData
         );

extern   int         ExecutionControl (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData
         );

#ifdef CR_MOREFEATURES
extern   int         Init_IVT (MACHINE_STATE  *pMachState);
extern   int         Init_PSP (MACHINE_STATE  *pMachState);
extern   int         Init_BIOSDA (MACHINE_STATE *pMachState);
#else
extern   int         Init_IVT ( void );
extern   int         Init_PSP ( void );
extern   int         Init_BIOSDA ( void );
#endif

#ifdef CR_DO_SCANNER    //riad//
extern   int         ScanBuffers (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         unsigned short dump_buffers
         );
#endif
/*-------------------------------------------------------------------------*/
#ifdef NEW1

#define _ActiveInstructions      crvv._ActiveInstructions
#define _IdleInstructions        crvv._IdleInstructions
#define _IdleLoops               crvv._IdleLoops
#define _IdleLoopLinearAddr      crvv._IdleLoopLinearAddr
#define _MinMaxTable             crvv._MinMaxTable
#define _OpcodeSize              _MinMaxTable._OpcodeSize
#define _Fulldecryption          crvv._Fulldecryption
#define _ulPrevMemoryReads       crvv._ulPrevMemoryReads
#define _ReadInstructionFlag     crvv._ReadInstructionFlag
#define _ActiveSincePartialMatch crvv._ActiveSincePartialMatch
#define _ulLastMRLA              crvv._ulLastMRLA
#define _LastMRFromFile          crvv._LastMRFromFile
#define _InREP_MOVS              crvv._InREP_MOVS
#define _InREP_LODS              crvv._InREP_LODS
#define _ActiveInstructionFlag   crvv._ActiveInstructionFlag
#define _flagBufferDegarbled     crvv._flagBufferDegarbled
#define _CodeModifPrefetch       crvv._CodeModifPrefetch

extern   CR_VARIOUS_VARIABLES crvv;

#endif /* NEW1 */

extern   struct cr_ini  _Default_ini;

extern   BYTE           _IVT_FILL_PATTERN[];
//extern   BYTE           *_IVT;
extern   BYTE           _CR_PSP_FILL_PATTERN[];
//extern   BYTE           *_CR_PSP;
extern   BYTE           _CR_BIOSDA_FILL_PATTERN[];
//extern   BYTE           *_CR_BIOSDA;

#ifndef NEW1

extern   WORD           _ActiveInstructions;
extern   WORD           _IdleInstructions;
extern   WORD           _IdleLoops;
extern   ULONG          _IdleLoopLinearAddr;
extern   BYTE           _flagBufferDegarbled;
extern   int            _InREP_MOVS;
extern   int            _InREP_LODS;
extern   int            _CodeModifPrefetch;
extern   int            _ActiveInstructionFlag;
extern   ULONG          _ulLastMRLA;
extern   int            _LastMRFromFile;
extern   WORD           _ActiveSincePartialMatch;

#ifdef CR_MOREFEATURES2 //riad//
//extern   MINMAX         *_ulPrevMemoryMods;
//extern   MINMAX         *_ulPrevMemoryCurrent;
extern   int            _ReadInstructionFlag;
// codedata stuff
extern   ULONG          _ulPrevMemoryReads[NUM_PREV_MEMORY_READS];

#else
extern   ULONG          _ulPrevMemoryMods[NUM_PREV_MEMORY_MODS];
extern   ULONG          _ulPrevMemoryReads[NUM_PREV_MEMORY_READS];
extern   int            _FirstMemModFromFile;
extern   ULONG          _ulPrevOpcodeLA;
extern   ULONG          _ulIncreasingExecutionAddr;
#endif
#endif /* NEW1 */

#ifdef CR_MOREFEATURES  //riad//
#ifndef NEW1
extern   MIN_MAX_INDEX  _MinMaxTable;
extern   int            _Fulldecryption;
extern   int            _OpcodeSize;
#endif /* NEW1 */
#ifndef CR_DO_SCANNER   //riad//
extern   ULONG          _nupdate_modif; // some debuging informations
extern   ULONG          _nget_in_table; // some debuging informations
#endif
#endif

//riad//
extern   int            x86_prefetch_list[X86PREFETCHLIST];
extern   int            x86_prefetch_index;

//riad//
extern   int            com_options;
extern	int		timeout;
extern	int		starttime;
//extern int _Max_Deep;

/*-------------------------------------------------------------------------*/


/*------------------- From CR_MISC.C    -----------------------------------*/

extern   MEMORY_AREA *AllocateNextMemoryArea (
         MEMORY_AREA    **ppMAList,
         WORD           Seg,
         WORD           Off,
         BYTE           *pBuff,  /* Or ZERO if must be allocated. */
         WORD           BuffSize,
         BOOL           freeable,
         BOOL           FromFile,/* Record loaded from file    */
         ULONG          OffIntoFile,/* If FromFile, offset into file */
         ULONG          ulFlags
         );

/* jce */
int GetFreeMemoryBlock (MEMORY_AREA *pMA_,     /* start of list */
         WORD area_size,        /* size of area requested */
         WORD *Segment,         /* segment returned here */
         WORD *Offset           /* offset returned here */
         );
/* end jce */

extern   VOID        FreeAllMemoryAreas (
         MEMORY_AREA    **ppMAList
         );

extern   VOID        ProcessExeRelocationTable  (
         BYTE           *pCodeBuffer,
         WORD           BuffSize,
         ULONG          BuffOffset, /* ...from the beginning of EXE Code area */
         WORD           start_seg,  /* Segment fixup value to be added.       */
         WORD           *pItem      /* Array of WORDs, terminated by -1, -1.  */
         );

extern   int         calculate_file_region (
         BOOL           ExeFile,                      /* <- */
         WORD           hdr_size,                     /* <- */
         ULONG          filesize,                     /* <- */
         WORD           starting_segment,             /* <- */
         WORD           trap_real_seg,                /* <- */
         WORD           trap_real_off,                /* <- */
         WORD           trap_half_buff_size,          /* <- */
         WORD           *presulted_buff_real_seg,     /* -> */
         WORD           *presulted_buff_real_off,     /* -> */
         ULONG          *pulabs_offset_of_buff_start, /* -> */
         WORD           *presulted_buff_size          /* -> */
         );

extern   int         ValidateBufferBoundaries (
         MEMORY_AREA    *maList,       /* <- &Ptr to start pointer of list */
         WORD           MustBeSeg,     /* <- Must-be-inside point real seg.*/
         WORD           MustBeOff,     /* <- Must-be-inside point real off.*/
         WORD          *pNewBuffSeg,   /* <-> Buffer real start addr.      */
         WORD          *pNewBuffOff,   /* <-> Buffer real start addr.      */
         WORD          *pNewBuffSize   /* <-> Buffer size.                 */
         );
/*-------------------------------------------------------------------------*/


/*------------------- From RUNNER1/2/3.C ----------------------------------*/

extern   void        CodeRunner (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData
         );

extern   MEMORY_AREA *FindAreaInList (
         WORD           Segment,    /* Find match for this segment   */
         WORD           Offset,     /* Find match for this offset.   */
         ULONG          ulFlags,    /* Buffer MUST have at least these*/
                     /* flags.                        */
         WORD           *pBytesAvl, /* Available bytes in buffer.    */
         MACHINE_STATE  *pMachState /* Machine state */
         );

extern   int         QueryNextOpcode (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         WORD           *TempIP
         );

extern   void        UpdateModificationRecords (
         //WORD           Segment,    /* Real Segment written to.      */
         //WORD           Offset);    /* Real offset written to.       */
         ULONG          MMLA);

extern   void        UpdateMemReadRecords (
         ULONG MRLA);               /* Linear address read from     */

extern   void        UpdateModifiedMA (
         //riad//
         MACHINE_STATE  *pMachState,   /* Machine state                                         */
         RETURN_DATA    *pRetData,     /* Upd. flag for EXECUTABLE MA.                          */
         MEMORY_AREA    *pMA,          /* Memory area for memo instr.                           */
         MEMORY_AREA    *pMA2,         /* M.A. if word crossing border.                         */
         int            bytes,         /* Bytes was written: 1 or 2.                            */
         WORD           Segment,       /* Real Segment written to.                              */
         WORD           Offset         /* Real offset written to.                               */
         );

extern   int         ReadMOD_RM (
         MACHINE_STATE  *pMachState,   /* -> For <QueryNextOpcode>                              */
         RETURN_DATA    *pRetData,     /*<-  For <QueryNextOpcode>                              */
         WORD           *pTempIP,      /*<-> For <QueryNextOpcode>                              */
         BOOL           word,          /* -> 1-use WORD Regs.                                   */
         BOOL           segreg,        /* -> 1-init Reg2 as seg.reg.                            */
         int            mod_rm,        /* -> Mod/RM byte itself.                                */
         MOD_RM_INFO    *pMRI          /*<-  Initialized after.                                 */
         );

extern   int         MapAddressToMemoryArea (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           Seg,           /* Segment to map.                                       */
         WORD           Off,           /* Offset to map.                                        */
         ULONG          ulMemoFlags,   /* Desired memory attributes.                            */
         int            word_size,     /* 1-word, 0-byte.                                       */
         MEMORY_AREA    **ppMA,        /* Memory area for memo instr.                           */
         MEMORY_AREA    **ppMA2        /* M.A. if word crossing border.                         */
         );

extern   int         GenInit_r_rm (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         int            dw,            /* Direct / W/B byte.                                    */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         ULONG          mem_always,    /* Use it if set.                                        */
         BOOL           segreg,        /* -> 1-init Reg2 as seg.reg.                            */
         MOD_RM_INFO    *pMRI,         /* Mod/rm structure.                                     */
         MEMORY_AREA    **ppMA,        /* Memory area for memo instr.                           */
         MEMORY_AREA    **ppMA2
         );

extern   int         GenericArithm_8x (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         int            op1,           /* First byte of instruction.                            */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         ULONG          mem_always,    /* If !=0 - use for memory.                              */
         int            (*ActualArithm) (
                 VOID *pDest,   /* Ptr to dest operand.                               */
                 VOID *pDest2,  /*!=0if dest-word cross mem                           */
                 VOID *pSrc,    /* Ptr to source operand.                             */
                 VOID *pSrc2,   /*!=0if src-word cross mem                            */
                 int  size,     /* 1(byte) or 2(word).                                */
                 WORD *pFlags)  /* Ptr to rFlags.                                     */
         );

extern   int         GenericGroup_Fx_INCDECNOTNEG (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         int            op1,           /* First  byte of instruction.                           */
         int            instruction,   /* 0-INC, 1-DEC, 2-NOT, 3-NEG */
         BYTE           rel_seg        /* Relative to segment.                                  */
         );

extern   int         GenericArithm    (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         int            op1,           /* First byte of instruction.                            */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         ULONG          mem_always,    /* If !=0 - use for memory.                              */
         BOOL           segreg,        /* -> 1-init Reg2 as segm. reg.                          */
         int            (*ActualArithm) (
                 VOID *pDest,   /* Ptr to dest operand.                               */
                 VOID *pDest2,  /*!=0if dest-word cross mem                           */
                 VOID *pSrc,    /* Ptr to source operand.                             */
                 VOID *pSrc2,   /*!=0if src-word cross mem                            */
                 int  size,     /* 1(byte) or 2(word).                                */
                 WORD *pFlags)  /* Ptr to rFlags.                                     */
         );

extern   int         Generic_ShiftRot (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         int            op1,           /* First  byte of instruction.                           */
         int            op2,           /* Second byte of instruction.                           */
         BYTE           rel_seg        /* Relative to segment.                                  */
         );

extern   int         GenericLxS (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         WORD           *pSegReg       /* &rDS or &rES.                                         */
         );

extern   int         GenericMulDiv (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         int            op1,           /* First byte of instruction.                            */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         int            (*ActualMulDiv) (
                 MACHINE_STATE  *pMachState,
                 RETURN_DATA    *pRetData,
                 int            word0_byte1,
                 VOID           *pInput,
                 VOID           *pInput2)
         );

extern   int         do_MUL (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 0-byte, 1-word instruction.                           */
         VOID           *pMult,        /* Pointer to destination data.                          */
         VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries.  */
         );

extern   int         do_IMUL (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 0-byte, 1-word instruction.                           */
         VOID           *pMult,        /* Pointer to destination data.                          */
         VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries.  */
         );

extern   int         do_DIV (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 0-byte, 1-word instruction.                           */
         VOID           *pMult,        /* Pointer to destination data.                          */
         VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries.  */
         );

extern   int         do_IDIV (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 0-byte, 1-word instruction.                           */
         VOID           *pMult,        /* Pointer to destination data.                          */
         VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries.  */
         );

extern   void        do_ShiftRot (
         int            Operation,     /* 0...7 - instruction type.                             */
         VOID           *pDest,        /* Pointer to destination data.                          */
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         int            rot_bits,      /* Rorate on number of bits.                             */
         int            size,          /* 0-byte, 1-word instruction.                           */
         WORD           *pFlags        /* Get updated. Also source of CF.                       */
         );

extern   int         do_OR  (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_AND (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_XOR  (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_TEST  (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_AND_OR_XOR_TEST (
         int            instruction,   /* 0-AND, 1-OR, 2-XOR.                                   */
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_ADD (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_ADC (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_SUB (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_SBB (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_MOV_Ax_imm (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         int            op1,           /* First byte of instruction.                            */
         BYTE           rel_seg        /* Relative to segment.                                  */
         );

extern   int         do_MOV (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_XCHG (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_CMP (
         VOID           *pDest,
         VOID           *pDest2,       /* Non-zero if dest - word memory, crossing boundaries.  */
         VOID           *pSrc,
         VOID           *pSrc2,        /* Non-zero if src  - word memory, crossing boundaries.  */
         int            size,          /* 1 or 2 bytes instruction.                             */
         WORD           *pFlags        /* Get updated. Also source of CF for ADC..., SBB... .   */
         );

extern   int         do_MOVS (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 1-WORD, 0-BYTE instruction.                           */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         BYTE           rep_prefix     /* Curr value of <rep_prefix>.                           */
         );

extern   int         do_CMPS (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 1-WORD, 0-BYTE instruction.                           */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         BYTE           rep_prefix     /* Curr value of <rep_prefix>.                           */
         );

extern   int         do_STOS (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 1-WORD, 0-BYTE instruction.                           */
         BYTE           rep_prefix     /* Curr value of <rep_prefix>.                           */
         );

extern   int         do_LODS (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 1-WORD, 0-BYTE instruction.                           */
         BYTE           rel_seg,       /* Relative to segment.                                  */
         BYTE           rep_prefix     /* Curr value of <rep_prefix>.                           */
         );

extern   int         do_SCAS (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         int            word,          /* 1-WORD, 0-BYTE instruction.                           */
         BYTE           rep_prefix     /* Curr value of <rep_prefix>.                           */
         );

extern   int         PUSH_POP_rm (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         BOOL           push,          /* 1 - PUSH, 0 - POP.                                    */
         BYTE           rel_seg        /* Relative to segment.                                  */
         );

extern   int         JMP_CALL_rm (
         MACHINE_STATE  *pMachState,   /* Machine state.                                        */
         RETURN_DATA    *pRetData,     /* Return data.                                          */
         WORD           *pTempIP,      /* Ptr to temp IP.                                       */
         BOOL           Jmp,           /* 1-JMP, 0-CALL.                                        */
         BOOL           Near,          /* 1-NEAR, 0-FAR.                                        */
         BYTE           rel_seg        /* Relative to segment.                                  */
         );

extern   int         do_PUSH_POP (
         MACHINE_STATE  *pMachState,   /* <-> Input/Output.                                     */
         RETURN_DATA    *pRetData,     /* <-  Output.                                           */
         WORD           *pValue,       /* Ptr to value.                                         */
         BOOL           push           /* TRUE - PUSH, FALSE - POP                              */
         );

extern   int         do_LEA (
         MACHINE_STATE  *pMachState,   /* <-> Input/Output.          */
         RETURN_DATA    *pRetData,     /* <-  Output.                */
         WORD           *pTempIP       /* Ptr to temp IP.            */
         );

extern   int         SetRegistersForInt   (
         MACHINE_STATE  *pMachState,   /* <-> Input/Output.                                     */
         RETURN_DATA    *pRetData,     /* <-  Output.                                           */
         WORD           RetIP,         /* Return after - IP.                                    */
         int            IntNo
         );

extern   int         ExecuteUserInt  (
         MACHINE_STATE  *pMachState,   /* <-> Input/Output.                                     */
         RETURN_DATA    *pRetData,     /* <-  Output.                                           */
         WORD           *pTempIP,
         int            IntNo
         );

extern   BYTE        ReadByteFromPTR (
         VOID    *ptr
         );

extern   WORD        ReadWordFromPTR (
         VOID           *ptr,
         VOID           *ptr2
         );

extern   VOID        WriteWordToPTR (
         WORD           word,
         VOID           *ptr,
         VOID           *ptr2
         );

extern   BYTE        ReadByteFromMA (
         MEMORY_AREA    *pMA,
         WORD           RealSegment,
         WORD           RealOffset
         );

extern   WORD        ReadWordFromMA (
         MEMORY_AREA    *pMA,
         MEMORY_AREA    *pMA2,
         WORD           RealSegment,
         WORD           RealOffset
         );

extern   VOID        WriteWordToMA (
         WORD           word,
         MEMORY_AREA    *pMA,
         MEMORY_AREA    *pMA2,
         WORD           RealSegment,
         WORD           RealOffset
         );

extern   void        SetAndOrXorFlags  (WORD *pFlags, int int_dest);

extern   void        SetArithmFlags    (WORD *pFlags, int size, long int1, long int2);

extern   int         ParityFlag        (int input);
/*-------------------------------------------------------------------------*/


#endif /*_CR_EXTS_H_ */
