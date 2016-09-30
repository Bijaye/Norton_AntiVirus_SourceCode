/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_MISC.C                                                *
 *                                                                         *
 * Description:   INTeL 8086/88 interpreter.                               *
 *                                                                         *
 *                Miscellaneous helper functions for CodeRunner support.   *
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


/*=========================================================================*/

/* Function returns 0 if not enough memory, otherwise - pointer to a
 * newly allocated MEMORY_AREA structure, already added to linked list.
 */

MEMORY_AREA *AllocateNextMemoryArea (
               MEMORY_AREA    **ppMAList,    /* PPointer to start of linked list */
               WORD           Seg,           /* Real Segment wanted.       */
               WORD           Off,           /* Real Offset wanted.        */
               BYTE           *_pBuff,       /* Or ZERO if must be allocated.*/
               WORD           BuffSize,      /* Size to allocate.          */
               BOOL           freeable,      /* Record whether freeable    */
               BOOL           FromFile,      /* Record loaded from file    */
               ULONG          OffIntoFile,   /* Offset into file if FromFile*/
               ULONG          ulFlags)       /* Flags for memory area.     */
{
/*-------------------------------------------------------------------------*/
 /*int                  i;*/
   static MEMORY_AREA   *pAllocatedMA, *pNextMAInList;
   BYTE                 *pBuff   =  _pBuff;
/*-------------------------------------------------------------------------*/


   pAllocatedMA = malloc (sizeof(MEMORY_AREA));

   if (!pAllocatedMA)
      return (0);

   if (!pBuff)
      {
#ifdef   CR_DEBUG
      if (BuffSize > 32000)
         {
         printf ("\nMemory request ERROR! See CR_MISC.C, line 66\7\7\7\7\7");
         }
#endif
      if (BuffSize != 0)
         pBuff = malloc (BuffSize);
      else
         pBuff = malloc (1);
      if (!pBuff)
         {
         free (pAllocatedMA);
         return (0);
         }

      /* Zero allocated buffer.
       */
      memset(pBuff, 0, BuffSize);
      }

   /* Initialized just allocated structure.
    */
   pAllocatedMA->Seg       = (WORD)(LinearAddr(Seg,Off) >> 4);
   pAllocatedMA->Off       = (WORD)(LinearAddr(Seg,Off) & 0x0000000F);
   pAllocatedMA->LinAddr   = LinearAddr(Seg,Off);
   pAllocatedMA->pBuff     = pBuff;
   pAllocatedMA->BuffSize  = BuffSize;
   pAllocatedMA->freeable  = freeable;
   pAllocatedMA->FromFile  = FromFile;
   pAllocatedMA->OffIntoFile = OffIntoFile;
   pAllocatedMA->SizeExtended = !freeable;
   pAllocatedMA->ModifiedNotScanned = FromFile;
   pAllocatedMA->ulFlags   = ulFlags;
   pAllocatedMA->minOffWr  = 0xFFFF;
   pAllocatedMA->maxOffWr  = 0;
   pAllocatedMA->pNextArea = 0;


   if (*ppMAList == 0)
      *ppMAList = pAllocatedMA;
   else
      {
      /* Add newly allocated memory area to the end of linked list.
       * Search to the end and do it.
       */
      for (pNextMAInList=*ppMAList;
           pNextMAInList->pNextArea;
           pNextMAInList = pNextMAInList->pNextArea
          )
         ;
      pNextMAInList->pNextArea = pAllocatedMA;
      }


#if WATC
   TEST_FOR_TERMINATION;
#endif

   return (pAllocatedMA);
}
/*=========================================================================*/


/*=========================================================================*/

/* Free all buffers in memory areas and memory areas itselves.
 */

VOID  FreeAllMemoryAreas (MEMORY_AREA **ppMAList)
{
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pNextMAInList, *pPrevMAInList;
/*-------------------------------------------------------------------------*/

   /* Search for the last MA/free last MA loop.
    */

   while (*ppMAList)
      {
      for (pPrevMAInList = pNextMAInList = *ppMAList;
         pNextMAInList->pNextArea;
         pPrevMAInList = pNextMAInList,
         pNextMAInList = pNextMAInList->pNextArea)
         ;

      if (pNextMAInList->pBuff)           /* Could be zero!       */
         if (pNextMAInList->freeable)     /* Could be static!     */
            {
            BYTE        *pBuffToZero;
            WORD        BytesToZero;

            if (pNextMAInList->FromFile)
               {
               /*printf("Zeroing whole buffer (%u) in FreeAllMemoryAreas\n", pNextMAInList->BuffSize);*/
               memset(pNextMAInList->pBuff, 0, pNextMAInList->BuffSize);
               }
            else
               {
               if (pNextMAInList->pBuff && pNextMAInList->minOffWr != 0xFFFF)
                  {
                  BytesToZero = (WORD)(pNextMAInList->maxOffWr - pNextMAInList->minOffWr + 1);

                  if (BytesToZero > 8)
                     {
                     pBuffToZero = pNextMAInList->pBuff+(WORD)(pNextMAInList->minOffWr - pNextMAInList->Off);
                     /*printf("Zeroing partial buffer (%u) in FreeAllMemoryAreas\n", BytesToZero);*/
                     memset(pBuffToZero, 0, BytesToZero);
                     }
                  }
               }
            //if(_IVT == pNextMAInList->pBuff) _IVT = NULL;
            //if(_CR_PSP == pNextMAInList->pBuff) _CR_PSP = NULL;
            free (pNextMAInList->pBuff);  /* Free its buffer.     */
            }

      if (pPrevMAInList->pNextArea)       /* Zero pointer. Copy   */
         pPrevMAInList->pNextArea = 0;    /* of it yet exists.    */
      else
         *ppMAList = 0;                   /* Zero starting pointer to MA linked list. */

      free (pNextMAInList);               /* Free MA itself.      */
      }
   //if (_IVT)    { free(_IVT); _IVT = NULL; }
   //if (_CR_PSP) { free(_CR_PSP); _CR_PSP = NULL; }

   return;
}
/*=========================================================================*/


/*=========================================================================*/
/*
 * Function processes relocation table items on a given code buffer.
 * Parameters:
 *
 * BYTE        *pCodeBuffer - ptr to buffer with code.
 * WORD        BuffSize     - size of code buffer.
 * ULONG       BuffOffset   - location(offset) of buffer from the beginning
 *                            of executable code of EXE file.
 * WORD        start_seg    - Segment fixup value to be added to
 *                            destination word into the code buffer.
 * WORD        *pItem       - ptr to an array of WORDs, terminated by -1, -1.
 *                            Each pair of WORDs represents one entry in
 *                            EXE relocation table in OFF,SEG order. End of
 *                            array is marked by a pair of -1,-1.
 */

VOID  ProcessExeRelocationTable  (
         BYTE        *pCodeBuffer,
         WORD        BuffSize,
         ULONG       BuffOffset, /* ...from the beginning of EXE Code area */
         WORD        start_seg,  /* Segment fixup value to be added.       */
         WORD        *pItem      /* Array of WORDs, terminated by -1, -1.  */
         )
{
/*-------------------------------------------------------------------------*/
   int            i, index;
/*-------------------------------------------------------------------------*/

   for (i=0; (pItem[i*2] != 0xFFFF) || (pItem[i*2+1] != 0xFFFF); i++)
      /* If relocation item is inside of code buffer...
       */
      if (
         LinearAddr (pItem[i*2+1], pItem[i*2]) >= BuffOffset            &&
         LinearAddr (pItem[i*2+1], pItem[i*2]) <= BuffOffset+BuffSize-2
         )
         {
         index = (int)(LinearAddr (pItem[i*2+1], pItem[i*2]) - BuffOffset);

         /* Perform relocation fixup.
          * LinearAddr (pItem[i*2+1], pItem[i*2]) - is an offset of word
          * of code area from the beginning of the code area of EXE file.
          * Code area begins right after the EXE file header. It must be:
          * BuffOffset ... BuffOffset+BuffSize-2.
          */
         *(WORD *)(pCodeBuffer+index) += start_seg;
         }


}
/*=========================================================================*/


/*=========================================================================*/
/*
 * Providing file size, type, trap addr. and other parameters function
 * calculates real seg:off of start of the buffer, buffer size and real
 * absolute offset of the beginning of buffer, where data must be read to.
 * Output parameters will be used by malloc(), subsequent file I/O
 * function and also by AllocateNextMA() function. Resulted parameters
 * giving enough information to recover from trap, cause by unsupplied
 * data. Function returns ERROR if trap_real_seg:off is outside of
 * valid executable area of given file. SUCCESS otherwise.
 */

int   calculate_file_region (
         BOOL  ExeFile,                      /* <-                         */
         WORD  hdr_size,                     /* <-                         */
         ULONG filesize,                     /* <-                         */
         WORD  starting_segment,             /* <- PSP_SEG+10h=START_SEG   */
         WORD  trap_real_seg,                /* <-                         */
         WORD  trap_real_off,                /* <-                         */
         WORD  trap_half_buff_size,          /* <-                         */
         WORD  *presulted_buff_real_seg,     /* ->                         */
         WORD  *presulted_buff_real_off,     /* ->                         */
         ULONG *pulabs_offset_of_buff_start, /* ->                         */
         WORD  *presulted_buff_size          /* ->                         */
         )
{
/*-------------------------------------------------------------------------*/
   ULONG          ul_abs_offset_of_trap_point,
                  resulted_buffer_start_real_la;   /* ...la == linear addr */
/*-------------------------------------------------------------------------*/


   /* We must validate trap point now. Trap point must be inside of
    * code part of file.
    */

   if (ExeFile)
      {
      /* Check for EXE files. */
      if (
         (LinearAddr (trap_real_seg, trap_real_off) <
          LinearAddr (starting_segment, 0))              ||
         (LinearAddr (trap_real_seg, trap_real_off) >=
         (LinearAddr (starting_segment, 0) + filesize - hdr_size))
         )
         return (ERROR);
      }
   else
      {
      /* Check for COM files. */
      if (
         (LinearAddr (trap_real_seg, trap_real_off) <
          LinearAddr (starting_segment, 0x100))
                                                         ||
         (LinearAddr (trap_real_seg, trap_real_off) >=
         (LinearAddr (starting_segment, 0x100) + filesize))
         )
         return (ERROR);
      }


   /* Calculate middle variable: absolute offset of trap point from the
    * very beginning of the file.
    */

   ul_abs_offset_of_trap_point =
      LinearAddr (trap_real_seg, trap_real_off) -
      LinearAddr (starting_segment, 0)          +
      ((LONG)(ExeFile ? hdr_size : (LONG)(-0x100)));


   /* Calculate absolute offset of the beginning of the future buffer from
    * the beginning of the file.
    */

   if ((ULONG)trap_half_buff_size >
       ul_abs_offset_of_trap_point-((ULONG)(ExeFile?hdr_size:0))
      )
      (*pulabs_offset_of_buff_start) = ((ULONG)(ExeFile?hdr_size:0));
   else
      (*pulabs_offset_of_buff_start) =
         ul_abs_offset_of_trap_point - trap_half_buff_size;


   /* Calculate size of future buffer.
    */

   if (filesize - (*pulabs_offset_of_buff_start) > trap_half_buff_size*2)
      (*presulted_buff_size) = (WORD)(trap_half_buff_size*2);
   else
      (*presulted_buff_size) = (WORD)(filesize - (*pulabs_offset_of_buff_start));


   resulted_buffer_start_real_la =
      LinearAddr (trap_real_seg, trap_real_off) -
      (ul_abs_offset_of_trap_point - (*pulabs_offset_of_buff_start));


   /* Calculate resulted buffer start real segment to be used for
    * AllocateNextMA() call.
    */

   (*presulted_buff_real_seg) =
      (WORD)((resulted_buffer_start_real_la >> 4) & 0xFFFF);


   /* Calculate resulted buffer start real offset to be used for
    * AllocateNextMA() call.
    */

   (*presulted_buff_real_off) =
      (WORD)(resulted_buffer_start_real_la & 0x0F);

   return (SUCCESS);
}
/*=========================================================================*/


/*=========================================================================*/
/*
 * This function will browse linked list of allocated memory areas and
 * check boundaries of requested buffer with already existing ones.
 * This check will find and eliminate any possible overlapping of
 * boundaries, by reducing the boundaries of requested buffer. As a
 * result of this call pNewxxxx variables could get changed.
 * This call must be done before any call to AllocateNextMA() call.
 * AllocateNextMA() is not responsible for any boundaries checkup.
 * Function will return ERROR if MustBeSeg:Off will be inside of already
 * allocated buffer. This could happen if already allocated buffer doesn't
 * have specific bit set, i.e. READABLE|WRITEABLE buffer is already exists,
 * but EXECUTABLE bit isn't set and buffer already covers a requested area.
 * This case must be fixed be caller, i.e. old buffer could be free()-ed
 * or so. Function will return SUCCESS in all other cases.
 *
 * Must-be point address required for the case, like this:
 *
 *    OOOOOOOOO
 *  NNNNNNNNNNNNN^NNNNNNNNNNNNNN
 *
 * Having must-be point we'll know that 'N'-new buffer must be reduced from
 * the left side.
 *
 */

int   ValidateBufferBoundaries (
         MEMORY_AREA    *maList,       /* <- &Ptr to start pointer of list */
         WORD           MustBeSeg,     /* <- Must-be-inside point real seg.*/
         WORD           MustBeOff,     /* <- Must-be-inside point real off.*/
         WORD          *pNewBuffSeg,   /* <-> Buffer real start addr.      */
         WORD          *pNewBuffOff,   /* <-> Buffer real start addr.      */
         WORD          *pNewBuffSize   /* <-> Buffer size.                 */
         )
{
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pMA;
   ULONG          ulMBPAddr;     /* Linear address of must-be point.       */
   ULONG          ulNewBuffLA;   /* Used to keep new linear address of buff*/
/*-------------------------------------------------------------------------*/

/* Definitions for: Old_Start, Old_End, New_Start, New_End.
 * Just for shorter source code.
 */
#define  O_S   (MALinearAddr(pMA))
#define  O_E   (O_S+pMA->BuffSize-1)
#define  N_S   (LinearAddr((*pNewBuffSeg),(*pNewBuffOff)))
#define  N_E   (N_S+(*pNewBuffSize)-1)


   ulMBPAddr = LinearAddr (MustBeSeg, MustBeOff);

   /* Just-in-case check.
    */
   if (!maList)
      return (SUCCESS);

   pMA = maList;

   do
      {
      /* Skip zero-size buffers.
       */
      if (pMA->BuffSize)
         {
         /* Check whether must-be point is inside of existing buffer.
          */
         if (ulMBPAddr>=O_S && ulMBPAddr<=O_E)
            /* Must-be point is inside of existing buffer. Aborting.
             * This could happen if buffer with different flags is
             * already allocated.
             */
            return (ERROR);

         /* Check if must-be point from the left side of "Old Buffer".
          * Case:
          *                 OOOOOOOOOOOOO
          *       NNNNNNN^NNNNNNN
          *                 \new buffer must be cut from here.
          */
         if (ulMBPAddr < O_S && N_E >= O_S)
            /* Adjust buffer size for this case.            */
            *pNewBuffSize = (WORD)(O_S - N_S);

         /* Check if must-be point from the right side of "Old Buffer".
          * Case:
          *       OOOOOOOOOOOOO
          *                NNNNNNN^NNNNNNN
          *                    \new buffer must be started from here.
          */
         if (ulMBPAddr > O_E && N_S <= O_E)
            {
            /* Adjust all buffer parameters in this case.   */
            ulNewBuffLA    = O_E+1;
            *pNewBuffSize -= ((WORD)(O_E - N_S + 1));
            *pNewBuffSeg   = (WORD)((ulNewBuffLA>>4) & 0xFFFF);
            *pNewBuffOff   = (WORD)(ulNewBuffLA & 0x0F);
            }
         }

      /* Switch to the next MA...
       */
      pMA = pMA->pNextArea;

      }
   while (pMA);


   return (SUCCESS);
}
/*=========================================================================*/


#ifdef CR_DEBUG
extern FILE *cr_dbgf;
extern int cr_hd;      /* Set to 1 to turn on coderunner output to file. */
extern int cr_itrace;  /* Set to 1 to turn on coderunner instruction trace. */
void gnotm1(void)
{
   if (cr_hd && cr_itrace) fprintf(cr_dbgf, "\n");
}
void gnotm2(int op)
{
   if (cr_hd && cr_itrace) fprintf(cr_dbgf, "%02X ", op);
}
ULONG          QNO_PrevTempIP = -1l;
#define GNOTM1 gnotm1()
#define GNOTM2(op) \
        if (QNO_PrevTempIP != (ULONG) *TempIP) \
           { \
           gnotm2(op); \
           QNO_PrevTempIP = *TempIP; \
           }
#else
#define GNOTM1
#define GNOTM2(op)
#endif

#define MEMRECDBG 1

/*=========================================================================*/
void        UpdateModificationRecords (
               //WORD           Segment,    /* Real Segment written to.      */
               //WORD           Offset)     /* Real offset written to.       */
               ULONG          MMLA)
{
#ifdef CR_MOREFEATURES2 //riad//

   int begin = 1;
   //ULONG MMLA;
   int found = 0;
   extern MINMAX *get_new_active_section(ULONG);
   MINMAX *result = _MinMaxTable.ulPrevMemoryCurrent;

   // ActiveInstructionFlag = 1 -> New Active Insruction in a new piece of code
   // ActiveInstructionFlag = 2 -> New active being part of an existing code
   // ActiveInstructionFlag = 3 -> Redundant Active being part of an existing code
   _ActiveInstructionFlag = 0;

   //MMLA = LinearAddr(Segment, Offset);

   while(result != NULL)
   {
      long addr_diff1;
      long addr_diff2;

      // this way, we start with the current (for decryptors, this will speed up the search)
      
      // "break", because we don't expect to have holes in the
      // list (may be for later)
      // Now we expect to have holes, so "continue"
      if(result->present != -1)
      {
         addr_diff1 = (long)(MMLA - result->min);
         addr_diff2 = (long)(MMLA - result->max);

         if(addr_diff1 >= 0 && addr_diff2 <= 0)
         {
            // Well, let's give a chance to another decryptor to run
            // (decrypting the same body. If different body, well, I dunno)
            if(_Fulldecryption)
            {
               if((addr_diff1-addr_diff2) >= MIN_ACTIVE_SUSPICIOUS)
                  _ActiveInstructionFlag = 3;
               else
                  _ActiveInstructionFlag = 0;
            }
            else  _ActiveInstructionFlag = 0;

            _MinMaxTable.ulPrevMemoryCurrent = result;
            found = 1;
            break;
         }

         if(addr_diff1 >= -3L && addr_diff1 < 0L) 
         {
            _ActiveInstructionFlag = 2;
            result->min = MMLA;
            _MinMaxTable.ulPrevMemoryCurrent = result;
            found = 1;
            break;
         }

         if(addr_diff2 <= 3L && addr_diff2 > 0L) 
         {
            _ActiveInstructionFlag = 2;
            result->max = MMLA;
            _MinMaxTable.ulPrevMemoryCurrent = result;
            found = 1;
            break;
         }
      }
      result = result->next;
      if(begin) { result = _MinMaxTable.ulPrevMemoryMods; begin = 0; }
   }


   if (!found)    // create another section
   {
      _ActiveInstructionFlag = 1;

      result = get_new_active_section(MMLA);
      result->min = result->max = MMLA;
      result->present = 1;
      result->offsetfromfile =  0;
      _MinMaxTable.ulsections++;
      _MinMaxTable.ulPrevMemoryCurrent = result;
   }
   else 
   {
      // codedata stuff (i comented this, because i need to know also
      // the sections of code that are moved)
      if(com_options & OPT_DECRYPT)
         if(_InREP_MOVS) _ActiveInstructionFlag = 0;
   }

   return ;

   /*---------------------------------------------------------------------------------------*/

#else //riad//

   int i,j;
   ULONG MMLA;

   MMLA = LinearAddr(Segment, Offset);
   for (i = 0; i < NUM_PREV_MEMORY_MODS; ++i)
      {
      if (MMLA == _ulPrevMemoryMods[i])
         {
   #if defined(CR_DEBUG) && MEMRECDBG
         if (cr_hd && cr_itrace)
            fprintf(cr_dbgf, "modification %08lX = previous modification %08lX\n",
                    MMLA, _ulPrevMemoryMods[i]);
   #endif
         return;
         }
      }
   for (i = 0; i < NUM_PREV_MEMORY_MODS; ++i)
      {
      long addr_diff;
      addr_diff = (long)(MMLA - _ulPrevMemoryMods[i]);
      if (addr_diff == -2l || addr_diff == -1l ||
           addr_diff == 1l || addr_diff == 2l)
         {
         if (_InREP_MOVS == 0)
            {
   #if defined(CR_DEBUG) && MEMRECDBG
            if (cr_hd && cr_itrace)
               fprintf(cr_dbgf, "modification %08lX similar to previous modification %08lX\n",
                       MMLA, _ulPrevMemoryMods[i]);
   #endif
            for (j = 0; j < NUM_PREV_MEMORY_READS; ++j)
               {
               long read_addr_diff;
               read_addr_diff = (long)(MMLA - _ulPrevMemoryReads[j]);
               if (read_addr_diff == 0 ||
                   read_addr_diff == -1l ||
                   read_addr_diff == 1l)
                  {
   #if defined(CR_DEBUG) && MEMRECDBG
                  if (cr_hd && cr_itrace)
                     fprintf(cr_dbgf, "modification %08lX qualifies as active! %08lX\n",
                             MMLA, read_addr_diff);
   #endif
                  _ActiveInstructionFlag = 1;
                  break;
                  }
               }
            }
         else
            {
   #if defined(CR_DEBUG) && MEMRECDBG
            if (cr_hd && cr_itrace)
               fprintf(cr_dbgf, "MOVS modification %08lX similar to previous modification %08lX\n",
                       MMLA, _ulPrevMemoryMods[i]);
   #endif
            }
         break;
         }
      }
   if (_InREP_MOVS == 0)
      {
   #if defined(CR_DEBUG) && MEMRECDBG
      if (cr_hd && cr_itrace)
         fprintf(cr_dbgf, "modification %08lX NOT identical to previous modifications\n",
                 MMLA);
   #endif
      for (i = (NUM_PREV_MEMORY_MODS-1); i >= 1; --i)
         {
         _ulPrevMemoryMods[i] = _ulPrevMemoryMods[i-1];
         }
      _ulPrevMemoryMods[0] = MMLA;
      }
   return;

#endif //riad

}
/*=========================================================================*/

/*=========================================================================*/
void        UpdateMemReadRecords (
               ULONG MRLA)                 /* Linear address read from     */
{
#if 0 //CR_MOREFEATURES2   //riad//
   // the problem remains: when do i have to clear this flag ????
   // is it that useful anyway ? not many good files takes benefits of it
   // (some but not many)
   _ReadInstructionFlag = 1;
   return;
#else
   int i;

   // for codedata stuff
   //if (_InREP_MOVS == 1)
   //   {
   //#if defined(CR_DEBUG) && MEMRECDBG
   //   if (cr_hd && cr_itrace)
   //      fprintf(cr_dbgf, "Read with REP MOVS instruction; don't put it in read buffer.\n",
   //              MRLA);
   //#endif
   //      return;
   //   }
   for (i = 0; i < NUM_PREV_MEMORY_READS; ++i)
      {
      if (MRLA == _ulPrevMemoryReads[i])
         {
   #if defined(CR_DEBUG) && MEMRECDBG
         if (cr_hd && cr_itrace)
            fprintf(cr_dbgf, "Read %08lX = previous read %08lX\n",
                    MRLA, _ulPrevMemoryReads[i]);
   #endif
         return;
         }
      }
   #if defined(CR_DEBUG) && MEMRECDBG
   if (cr_hd && cr_itrace)
      fprintf(cr_dbgf, "Read %08lX NOT same as previous reads\n",
              MRLA);
   #endif
   for (i = (NUM_PREV_MEMORY_READS-1); i >= 1; --i)
      {
      _ulPrevMemoryReads[i] = _ulPrevMemoryReads[i-1];
      }
   _ulPrevMemoryReads[0] = MRLA;
   return;
#endif
}
/*=========================================================================*/

/*=========================================================================*/

/* Function updates MIN and MAX modified offsets in memory area(s).
 * Function usually called after certain BYTE or WORD was written into the
 * memory.
 * Function updates <minOffWr> and <maxOffWr> fields in <pMA> and <pMA2>
 * and also updates <pRetData->ulRetCode> flag, if buffer happened to be
 * executable.
 */
void        UpdateModifiedMA (
               //riad//
               MACHINE_STATE  *pMachState,/* Machine state                 */
               RETURN_DATA    *pRetData,  /* Upd. flag for EXECUTABLE MA.  */
               MEMORY_AREA    *pMA,       /* Memory area for memo instr.   */
               MEMORY_AREA    *pMA2,      /* M.A. if word crossing border. */
               int            bytes,      /* Bytes was written: 1 or 2.    */
               WORD           Segment,    /* Real Segment written to.      */
               WORD           Offset)     /* Real offset written to.       */
{
/*-------------------------------------------------------------------------*/
   WORD           OffsetWrittenTo;
   WORD           OffsetWrittenTo2;
	//riad//
	ULONG				MMLA = LinearAddr(Segment, Offset);
/*-------------------------------------------------------------------------*/

  /* commented for codedata stuff (i wanna know the moved code outside the */
   /* file */
   /*if (pMA->FromFile) */
  UpdateModificationRecords (MMLA);

   if(!pMA->FromFile & !(com_options&OPT_DECRYPT) )
   {
      // Ok, we are writing something outside the memory taken by the filename
      // This suggets that may be the virus is moving pieces of code outside
      // I have to remember when it's coming from, so I can't report the
      // code in the filename (I have to know the obsolute value)

      // The last read value is supposed to be _ulPrevMemoryReads[0], but well
      // may be it's something else...take care

      // Offset is in alsmot all case Lprecomputed - _ulPrevMemoryReads[0]

      LONG curOffset = _MinMaxTable.ulPrevMemoryCurrent->offsetfromfile;
      MINMAX *section = _MinMaxTable.ulPrevMemoryCurrent;

      if(_ActiveInstructionFlag == 2 /*&& ((section->max-section->min) > 9)*/)  
      // well, not a garbage copy/mov stuff
      {
         int i;
         LONG currentmin = (LONG)(MMLA - _ulPrevMemoryReads[0]);
         LONG localmin;

         if(currentmin != curOffset)
         {
            for(i=1;i<3;i++)
            {
               // reasonnable offset
               localmin = (LONG)(MMLA - _ulPrevMemoryReads[i]);

               // which is better ?
               if(labs(localmin - curOffset) < labs(currentmin - curOffset)) 
                  currentmin = localmin;
            }
            if((labs(currentmin - curOffset) > 2))
            {
               // it means we got another candidate
               section->offsetfromfile = (LONG)(MMLA - _ulPrevMemoryReads[0]);
            }
         }
      }
      _ActiveInstructionFlag = 0;
   }

#if defined(CR_DEBUG) && MEMRECDBG
   else
	{
      if (cr_hd && cr_itrace)
         fprintf(cr_dbgf, "Write to region not loaded from file: %08lX\n",
                 MMLA);
	}
#endif

#ifdef CR_MOREFEATURES2	//riad//

#else
   if (_FirstMemModFromFile)
      {
      if (pMA->FromFile)
          _ActiveInstructionFlag = 1;
      _FirstMemModFromFile = 0;
      }
#endif

   OffsetWrittenTo = (WORD)
      (pMA->Off + (WORD)(MMLA-MALinearAddr(pMA)));

   /* Update address of 'modified' byte(s) in touched memory area.
    */
   if (OffsetWrittenTo < pMA->minOffWr)
      pMA->minOffWr = OffsetWrittenTo;

   if (OffsetWrittenTo > pMA->maxOffWr)
      pMA->maxOffWr = OffsetWrittenTo;

   /* If <pMA2> was initialized <pMA2->maxOffWr> could be updated.
    */
   if (pMA2)
      {
      OffsetWrittenTo2 = (WORD)
         (pMA2->Off + (WORD)(MMLA+1-MALinearAddr(pMA2)));

      if (OffsetWrittenTo2 < pMA2->minOffWr)
         pMA2->minOffWr = OffsetWrittenTo2;

      if (OffsetWrittenTo2 > pMA2->maxOffWr)
         pMA2->maxOffWr = OffsetWrittenTo2;
      }
   else
      if (bytes==2 && (OffsetWrittenTo+1 > pMA->maxOffWr))
         pMA->maxOffWr = (WORD)(OffsetWrittenTo+1);

   /* Set Self-modify flag if modified buffer is executable.. */

	// If from file, and if Active not seen already before
	// This code has to be optimized in connection with
	// UpdateModoficationRecords and BreakConditionCheck

	if (
		(pMA->ulFlags & EXECUTABLE) ||
		(pMA2 && pMA2->ulFlags & EXECUTABLE)
		)
		{
		if (MMLA != pRetData->LMLinearAddr)
			{
			pRetData->LMLinearAddr = MMLA;
			pRetData->ulRetCode |= CODE_SELF_MODIFIED;
			}
		//riad//
		// check now if code modified within the borders of the
		// prefetch queue.
		// Problem: we had already some QueryNextOpcode(), so the 
		// prefetch changed...in fact, we are now referencing the
		// next opcode. I think it's architechture dependant.
		// it should be the MAX.

		//if(Lprecomputed < (pMachState->LprecomputedAddr+(ULONG)PREFETCH_QUEUE_SIZE) 
		//	&& Lprecomputed >= pMachState->LprecomputedAddr)
		if(MMLA < (LinearAddr(rCS,rIP)+(ULONG)PREFETCH_QUEUE_SIZE) 
			&& MMLA >= LinearAddr(rCS,rIP))
			{
         _CodeModifPrefetch = 1;

#ifdef MYDEBUG
			printf("\n\tMODIF = %X:%X %lX (PMA %lX -> %lX  PMA2 %lX)\n",
				Segment, Offset,
				Lprecomputed, pMA->LinAddr, 
				pMA->LinAddr+pMA->BuffSize, pMA2?pMA2->LinAddr:0);
#endif
			}

		}


   return;
}

/*=========================================================================*/

/*=========================================================================*/
/* Function returns opcode from MEMORY_AREA related to requested
 * <Segment>:<Offset> with at least <ulFlags>.
 * Function returns -1 if memory area cannot be found.
 * All addresses are normalized. 1230:0500 will be == 1200:0800
 */

int GetOpcodeFromAreaInList (
               //WORD           Segment,    /* Find match for this segment   */
               //WORD           Offset,     /* Find match for this offset.   */
               ULONG          InputLA,    /* Precomputed LinearAddr        */
               MACHINE_STATE  *pMachState /* Machine state                 */
               )
{
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pMA;                   /* Starting memory area to search*/
/*-------------------------------------------------------------------------*/

   //ULONG InputLA;
   ULONG TestLA;

   // 6/19/97 Fred: Bugfix: Added the test on the pointer
   if (pMachState == NULL)
		return -1;

   //InputLA = LinearAddr(Segment, Offset);
   if (
	  (pMA=pMachState->pPrevIFetchMA) != 0l &&
      (InputLA >= (TestLA=MALinearAddr(pMA))) &&
      (InputLA <= (TestLA + pMA->BuffSize - 1))
      )
      {
      return (int)(*(BYTE *)(pMA->pBuff+(WORD)(InputLA-TestLA)));
      }

   /* Search for the current Code buffer.
    */
   for (pMA=pMachState->pMemA; pMA; pMA=pMA->pNextArea)
      {
      if ((ULONG)EXECUTABLE == (ULONG)(pMA->ulFlags & (ULONG)EXECUTABLE))
         {
         /* Found buffer with matching flags. Check whether <Segment> and
          * <Offset> are points inside of this buffer.
          */
         if (
            (InputLA >= (TestLA=MALinearAddr(pMA))) &&
            (InputLA <= (TestLA + pMA->BuffSize - 1))
            )
            {
            pMachState->pPrevIFetchMA = pMA;
            return (int)(*(BYTE *)(pMA->pBuff+(WORD)(InputLA-TestLA)));
            }
         }
      }

   return (-1);
}


/*=========================================================================*/

/*=========================================================================*/
/* Function returns next opcode from rCS:TempIP and TempIP will get
 * incremented.
 * Function returns -1, if next opcode cannot be queried and <pRetData> is
 * initialized in that case.
 */

int   QueryNextOpcode (
         MACHINE_STATE  *pMachState,
         RETURN_DATA    *pRetData,
         WORD           *TempIP
         )
{
/*-------------------------------------------------------------------------*/
   int            NextOpcode;
   int            i;
   //int            PQSize;
   //riad//
   ULONG          MMLA = LinearAddr(rCS, *TempIP);
/*-------------------------------------------------------------------------*/

   if (0 != (/*PQSize =*/ pMachState->PQSize)) /* Does Prefetch queue must be emulated at all? */
      {
      int   *pPQBuff;

      pPQBuff = pMachState->pPQBuff;
      if (
         pMachState->PQLinearAddr != MMLA  ||      /* or wrong address   */
         pPQBuff[pMachState->PQIndex] == -1  /* If P.Q. is not initialized or... */
         )
         { 
           /* Prefetch queue is now empty or out of execution sequence.
            * Prefetch queue will be empty (pPQBuff[0] == -1)
            * in case of either very beginning of <CodeRunner> execution
            * or if no more EXECUTABLE data supplied for CodeRunner.
            * CodeRunner's caller must set: pPQBuff[0] = -1;
            * Prefetch queue will be out of execution sequence
            * (PQLinearAddr != rCS:(*TempIP)) after any instruction, which
            * changed IP (JMP, CALL, RET, ...).
            *
            * So, we must completely refill queue now.
            */

         /* Reset index and linear address. */
         pMachState->PQIndex      = 0;
         pMachState->PQLinearAddr = MMLA;

         /* Filling prefetch queue buffer completely.
          */

         for (i=0; i<pMachState->PQSize; i++)
            {
            pPQBuff[i] =
               GetOpcodeFromAreaInList (
                  MMLA+(ULONG)i,
                  pMachState);
            }
         }

      /* Get next opcode from Prefetch Queue.
       */
      NextOpcode = pPQBuff[pMachState->PQIndex];

      /* Initialize last element of prefetch queue after first element
       * was queried. pPQBuff[PQIndex] - is the last element. After that
       * index will get incremented.
       */
      pPQBuff[pMachState->PQIndex] =
         GetOpcodeFromAreaInList (
            MMLA+(ULONG)(pMachState->PQSize),
            pMachState);

      /* Finally check what we've got.
       */
      if (NextOpcode != -1)
         {
         /* If next opcode was queried successfully from queue - increment
          * linear addr, index and TempIP.
          */
         pMachState->PQLinearAddr++;
         pMachState->PQIndex++;
         if (pMachState->PQIndex>=pMachState->PQSize) pMachState->PQIndex=0;
         GNOTM2(NextOpcode);
         ++(*TempIP);
         
         _OpcodeSize++;    //riad//
         }
      else
         {
         pRetData->SegTrap    = rCS;
         pRetData->OffTrap    = *TempIP;
         pRetData->ulRetCode |= EXECUTE_TRAP;
         GNOTM1;
         }

      return (NextOpcode);
      }
   else
      {
      /* No prefetch queue required to be emulated.
       * Get <pCodeMA>.
       * Must re-do it every time, because rCS could get changed any time.
       */
     NextOpcode =
         GetOpcodeFromAreaInList (
            MMLA,
            pMachState);

      if (NextOpcode != -1)
         {
         GNOTM2(NextOpcode);
         ++(*TempIP);
         
         _OpcodeSize++;    //riad//

         return (NextOpcode);
         }
      else
         {
         pRetData->SegTrap    = rCS;
         pRetData->OffTrap    = *TempIP;
         pRetData->ulRetCode |= EXECUTE_TRAP;
         GNOTM1;
         return (-1);
         }
      }
}
/*=========================================================================*/

/*=========================================================================*/

/* Function returns pointer to found MEMORY_AREA related to requested
 * <Segment>:<Offset> with at least <ulFlags>.
 * Function returns 0 if memory area cannot be found.
 * Function returns a number of bytes available in buffer in <pBytesAvl>.
 * All addresses are normalized. 1230:0500 will be == 1200:0800
 */

MEMORY_AREA *FindAreaInList (
               WORD           Segment,    /* Find match for this segment   */
               WORD           Offset,     /* Find match for this offset.   */
               ULONG          ulFlags,    /* Buffer MUST have at least these*/
                                          /* flags.                        */
               WORD           *pBytesAvl, /* Available bytes in buffer.    */
               MACHINE_STATE  *pMachState /* Machine state                 */
               )
{
#ifndef CR_TEST   //riad//
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pMA;                   /* Starting memory area to search*/
/*-------------------------------------------------------------------------*/

   ULONG InputLA;
   ULONG TestLA;

   InputLA = LinearAddr(Segment,Offset);
   if (
      (pMA=pMachState->pPrevDFetchMA) != 0l &&
      (ulFlags == (pMA->ulFlags & ulFlags)) &&
      (InputLA >= (TestLA=MALinearAddr(pMA))) &&
      (InputLA <= (TestLA + pMA->BuffSize - 1))
      )
      {
      *pBytesAvl = (WORD)
         (pMA->BuffSize - (WORD)(InputLA - TestLA));
      if (ulFlags & READABLE)
         {
         _ulLastMRLA = InputLA;
         _LastMRFromFile = pMA->FromFile;
         }
      return (pMA);
      }
   /* Search for the current Code buffer.
    */
   for (pMA=pMachState->pMemA; pMA; pMA=pMA->pNextArea)
      {
      if (ulFlags == (pMA->ulFlags & ulFlags))
         {
         /* Found buffer with matching flags. Check whether <Segment> and
          * <Offset> are points inside of this buffer.
          */
         if (
            (InputLA >= (TestLA=MALinearAddr(pMA))) &&
            (InputLA <= (TestLA + pMA->BuffSize - 1))
            )
            {
            *pBytesAvl = (WORD)
               (pMA->BuffSize - (WORD)(InputLA - TestLA));
            if (ulFlags & READABLE)
               {
               _ulLastMRLA = InputLA;
               _LastMRFromFile = pMA->FromFile;
               }
            pMachState->pPrevDFetchMA = pMA;
            return (pMA);
            }
         }
      }

   return (0);

#else //riad//
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pMA = pMachState->pMemA;/* Starting memory area to search*/
/*-------------------------------------------------------------------------*/

   ULONG InputLA;
   ULONG TestLA;
   //int firstime = 1;

   InputLA = LinearAddr(Segment,Offset);
   //pMA = (pMachState->pMemCurrentRW)?pMachState->pMemCurrentRW:pMachState->pMemA;
 
   /* Search for the current Code buffer.
    */
   while(pMA)
   {
      if (ulFlags == (pMA->ulFlags & ulFlags))
      {
         /* Found buffer with matching flags. Check whether <Segment> and
          * <Offset> are points inside of this buffer.
          */
         if (
            (InputLA >= (TestLA=MALinearAddr(pMA))) &&
            (InputLA <= (TestLA + pMA->BuffSize - 1))
            )
         {
            *pBytesAvl = (WORD)
               (pMA->BuffSize - (WORD)(InputLA - TestLA));
            return (pMA);
         }
      }
      //if(firstime) { pMA = pMachState->pMemA; firstime = 0; }
      //else 
      pMA = pMA->pNextArea;
   }

   return (0);

#endif   //riad//
}
/*=========================================================================*/

/* Function returns initialized <ppMA> and <ppMA2>, which are corresponds
 * to the <Seg>:<Off> address. If only 1 byte address needed to be mapped -
 * only ppMA will be initialized. Upon success returns (0).
 * If memory areas cannot be found - function returns (-1) and <pRetData>
 * will be initialized.
 */

int         MapAddressToMemoryArea (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           Seg,           /* Segment to map.            */
               WORD           Off,           /* Offset to map.             */
               ULONG          ulMemoFlags,   /* Desired memory attributes. */
               int            word_size,     /* 1-word, 0-byte.            */
               MEMORY_AREA    **ppMA,        /* Memory area for memo instr.*/
               MEMORY_AREA    **ppMA2)       /* M.A. if word crossing border.*/
{
/*-------------------------------------------------------------------------*/
   WORD           BytesAvl;
/*-------------------------------------------------------------------------*/


   *ppMA2 = (MEMORY_AREA *) -1;     /* Set it to non-zero.                 */

   /* Find memory area.
    */
   *ppMA = FindAreaInList (
            Seg, Off, ulMemoFlags, &BytesAvl, pMachState);

   if ((*ppMA) && word_size && (BytesAvl==1))
      /* Ok, here we have WORD requested and only 1 byte available
       * on currently found memory area. We could try to find another
       * area for second byte to satisfy our needs.
       */
      *ppMA2 = FindAreaInList (
                  Seg, (WORD)(Off+1), ulMemoFlags, &BytesAvl, pMachState);

   if (!(*ppMA) || !(*ppMA2))
      {
      /* Write/Read TRAP occurs. Save information about it into the
       * <pRetData>.
       */
      if (ulMemoFlags & WRITEABLE)
         pRetData->ulRetCode |= WRITE_TRAP;

      if (ulMemoFlags & READABLE)
         pRetData->ulRetCode |= READ_TRAP;

      pRetData->SegTrap = Seg;
      pRetData->OffTrap = Off;
      if (!(*ppMA2))
         pRetData->OffTrap = (WORD)(Off+1);

      return (-1);
      }


   /* Ok, here we have pMA (and probably pMA2) initialized.
    * Reset <pMA2> if still not initialized.
    */
   if ((*ppMA2) == (MEMORY_AREA *) -1)
      *ppMA2 = 0;

   return (0);

}
/*=========================================================================*/
