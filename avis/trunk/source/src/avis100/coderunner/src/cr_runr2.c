/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_RUNR2.C                                               *
 *                                                                         *
 * Description:   INTeL 8086/88 interpreter.                               *
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

/* Function decyphers <mod_rm> byte and initialize accordingly <pMRI>.
 * Function will query a few following bytes of code, if so required.
 * Returns (-1) in case of unable to query next opcode. In that case
 * <pRetData> will be initialized.
 */

  int ReadMOD_RM (
         MACHINE_STATE  *pMachState,         /* -> For <QueryNextOpcode>   */
         RETURN_DATA    *pRetData,           /*<-  For <QueryNextOpcode>   */
         WORD           *pTempIP,            /*<-> For <QueryNextOpcode>   */
         BOOL           word,                /* -> 1-use WORD Regs.        */
         BOOL           segreg,              /* -> 1-init Reg2 as seg.reg. */
         int            mod_rm,              /* -> Mod/RM byte itself.     */
         MOD_RM_INFO    *pMRI                /*<-  Initialized after.      */
         )
{
/*-------------------------------------------------------------------------*/
   WORD  ResSeg;
   WORD  ResOff;
   VOID  *pReg1=0;      /* If !NULL upon exit - <ppModRM_Reg1> must be
                         * initialized instead of ..SegVal., ..OffVal.
                         */
   VOID  *pReg2;
   int   op1, op2;
/*-------------------------------------------------------------------------*/


   switch (mod_rm >> 6)
      {
      case  0x00:
            switch (mod_rm & 0x07)
               {
               case  0: ResOff=(WORD)(rBX+rSI); ResSeg=rDS; break;
               case  1: ResOff=(WORD)(rBX+rDI); ResSeg=rDS; break;
               case  2: ResOff=(WORD)(rBP+rSI); ResSeg=rSS; break;
               case  3: ResOff=(WORD)(rBP+rDI); ResSeg=rSS; break;
               case  4: ResOff=rSI;     ResSeg=rDS; break;
               case  5: ResOff=rDI;     ResSeg=rDS; break;
               case  6:
                  op1 = QueryNextOpcode(pMachState,pRetData,pTempIP);
                  if (op1 < 0) return (-1);  /* Cannot query next opcode.  */
                  op2 = QueryNextOpcode(pMachState,pRetData,pTempIP);
                  if (op2 < 0) return (-1);  /* Cannot query next opcode.  */

                  ResOff=MK_WORD(op2, op1);
                  ResSeg=rDS;
                  break;

               case  7: ResOff=rBX;     ResSeg=rDS; break;
               default: break;
               }

            break;


      case  0x01:
            op1 = QueryNextOpcode (pMachState, pRetData, pTempIP);
            if (op1 < 0) return (-1);  /* Cannot query next opcode.        */

            switch (mod_rm & 0x07)
               {
               case  0: ResOff=(WORD)(rBX+rSI+(char)op1); ResSeg=rDS; break;
               case  1: ResOff=(WORD)(rBX+rDI+(char)op1); ResSeg=rDS; break;
               case  2: ResOff=(WORD)(rBP+rSI+(char)op1); ResSeg=rSS; break;
               case  3: ResOff=(WORD)(rBP+rDI+(char)op1); ResSeg=rSS; break;
               case  4: ResOff=(WORD)(rSI+(char)op1);     ResSeg=rDS; break;
               case  5: ResOff=(WORD)(rDI+(char)op1);     ResSeg=rDS; break;
               case  6: ResOff=(WORD)(rBP+(char)op1);     ResSeg=rSS; break;
               case  7: ResOff=(WORD)(rBX+(char)op1);     ResSeg=rDS; break;
               default: break;
               }

            break;


      case  0x02:
            op1 = QueryNextOpcode(pMachState,pRetData,pTempIP);
            if (op1 < 0) return (-1);  /* Cannot query next opcode.        */
            op2 = QueryNextOpcode(pMachState,pRetData,pTempIP);
            if (op2 < 0) return (-1);  /* Cannot query next opcode.        */
            switch (mod_rm & 0x07)
               {
               case  0: ResOff=(WORD)(rBX+rSI+(short)MK_WORD(op2, op1)); ResSeg=rDS; break;
               case  1: ResOff=(WORD)(rBX+rDI+(short)MK_WORD(op2, op1)); ResSeg=rDS; break;
               case  2: ResOff=(WORD)(rBP+rSI+(short)MK_WORD(op2, op1)); ResSeg=rSS; break;
               case  3: ResOff=(WORD)(rBP+rDI+(short)MK_WORD(op2, op1)); ResSeg=rSS; break;
               case  4: ResOff=(WORD)(rSI+(short)MK_WORD(op2, op1));     ResSeg=rDS; break;
               case  5: ResOff=(WORD)(rDI+(short)MK_WORD(op2, op1));     ResSeg=rDS; break;
               case  6: ResOff=(WORD)(rBP+(short)MK_WORD(op2, op1));     ResSeg=rSS; break;
               case  7: ResOff=(WORD)(rBX+(short)MK_WORD(op2, op1));     ResSeg=rDS; break;
               default: break;
               }

            break;


      case  0x03:
            ResSeg=0; ResOff=0;
            switch (mod_rm & 0x07)
               {
               case  0: pReg1 = word ? (PVOID)&rAX : (PVOID)&rAL; break;
               case  1: pReg1 = word ? (PVOID)&rCX : (PVOID)&rCL; break;
               case  2: pReg1 = word ? (PVOID)&rDX : (PVOID)&rDL; break;
               case  3: pReg1 = word ? (PVOID)&rBX : (PVOID)&rBL; break;
               case  4: pReg1 = word ? (PVOID)&rSP : (PVOID)&rAH; break;
               case  5: pReg1 = word ? (PVOID)&rBP : (PVOID)&rCH; break;
               case  6: pReg1 = word ? (PVOID)&rSI : (PVOID)&rDH; break;
               case  7: pReg1 = word ? (PVOID)&rDI : (PVOID)&rBH; break;
               default: break;
               }

            break;


      default: break;
      }


   if (!segreg)
      {
      switch ((mod_rm >> 3) & 0x07)
         {
         case  0: pReg2 = word ? (PVOID)&rAX : (PVOID)&rAL; break;
         case  1: pReg2 = word ? (PVOID)&rCX : (PVOID)&rCL; break;
         case  2: pReg2 = word ? (PVOID)&rDX : (PVOID)&rDL; break;
         case  3: pReg2 = word ? (PVOID)&rBX : (PVOID)&rBL; break;
         case  4: pReg2 = word ? (PVOID)&rSP : (PVOID)&rAH; break;
         case  5: pReg2 = word ? (PVOID)&rBP : (PVOID)&rCH; break;
         case  6: pReg2 = word ? (PVOID)&rSI : (PVOID)&rDH; break;
         case  7: pReg2 = word ? (PVOID)&rDI : (PVOID)&rBH; break;
         default: break;
         }
      }
   else
      {
      switch ((mod_rm >> 3) & 0x07)
         {
         case  0: pReg2 = (PVOID)&rES; break;
         case  1: pReg2 = (PVOID)&rCS; break;
         case  2: pReg2 = (PVOID)&rSS; break;
         case  3: pReg2 = (PVOID)&rDS; break;
         default: /* Default could mean invalid instruction also.          */
            pRetData->SegTrap   = rCS;
            pRetData->OffTrap   = rIP;
            pRetData->ulRetCode |= UNSUPPORTED_OPCODE;

            return (-1);
         }
      }


   /* Ok, return final results.
    */
   pMRI->Seg1 = ResSeg;
   pMRI->Off1 = ResOff;
   pMRI->pReg1 = pReg1;
   pMRI->pReg2 = pReg2;

   return (SUCCESS);
}
/*=========================================================================*/

/*=========================================================================*/

/*
 * Generic Init for Register/Register-memory instruction.
 * Function called, when first byte of opcode shows, that second one will be
 * r/RM byte. So second byte will determine which registers and which memory
 * will be used for instruction. This function will query next byte of
 * instruction, analyze it and initialize <pMRI>, <pMA> and <pMA2>.
 */

int         GenInit_r_rm (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               int            dw,            /* Bit0-W(1)/B, bit1-Direction.*/
               BYTE           rel_seg,       /* Relative to segment.       */
               ULONG          mem_always,    /* Use it if set.             */
               BOOL           segreg,        /* -> 1-init Reg2 as seg.reg. */
               MOD_RM_INFO    *pMRI,         /* Mod/rm structure.          */
               MEMORY_AREA    **ppMA,        /* Memory area for memo instr.*/
               MEMORY_AREA    **ppMA2)       /* M.A. if word crossing border.*/
{
/*-------------------------------------------------------------------------*/
   int            op2;
   ULONG          ulMemoFlags;
/*-------------------------------------------------------------------------*/

   /*----------------------------------------------------------------------*/
   /* Query Mod/RM byte.                                                   */
   /*----------------------------------------------------------------------*/

   op2 = QueryNextOpcode (pMachState, pRetData, pTempIP);
   //riad//
   *pMachState->pOp2 = op2;
   if (op2 < 0)
      {
      /*-------------------------------------------------------------------*/
      /* Code buffer not found! Nothing to do. Return...                   */
      /* <pRetData> already initialized.                                   */
      /*-------------------------------------------------------------------*/
      return (-1);
      }


   /*----------------------------------------------------------------------*/
   /* Initialize <pMRI> structure from Mode/RM byte.                       */
   /*----------------------------------------------------------------------*/

   if (ReadMOD_RM (
         pMachState,    /*  -> For <QueryNextOpcode>                       */
         pRetData,      /* <-  For <QueryNextOpcode>                       */
         pTempIP,       /* <-> For <QueryNextOpcode>                       */
         (BOOL)(dw & 1), /*  -> 1-use WORD Regs.                           */
         segreg,        /*  -> 1-init Reg2 as seg.reg.                     */
         op2,           /*  -> Mod/RM byte itself.                         */
         pMRI           /* <-  Initialized after.                          */
         ) < 0)
      {
      /*-------------------------------------------------------------------*/
      /* Something went wrong. <pRetData> already initialized. Return.     */
      /*-------------------------------------------------------------------*/
      return (-1);
      }


   /* If only registers in operation - that's it. Everything's done.
    * <pMRI> initialized, set <ppMAx> all zero (as not used).
    */
   if (pMRI->pReg1)
      {
      *ppMA  = 0;
      *ppMA2 = 0;
      return (0);
      }

   /* Check whether segment prefix specified before instruction.
    */
   switch (rel_seg)
      {
      case  0x00:                   break;
      case  0x26: pMRI->Seg1 = rES; break;
      case  0x36: pMRI->Seg1 = rSS; break;
      case  0x2E: pMRI->Seg1 = rCS; break;
      case  0x3E: pMRI->Seg1 = rDS; break;
      default:    break;
      }


   /* Ok, here we know that memory operand will be on one side.
    * Initialize neccessary variables and do common things.
    */
   if (!mem_always)  /* If <mem_always> not set - use normal way.          */
      {
      if (dw & 0x02)
         ulMemoFlags = READABLE;    /* ADD reg,mem or CMP mem,reg          */
      else
         ulMemoFlags = WRITEABLE;   /* ADD mem,reg                         */
      }
   else
      ulMemoFlags = mem_always;


   /* Initialize <ppMA> and <ppMA2> - find proper memory area(s) for
    * Segment/Offset, found in Mode/RM byte.
    */
   if (MapAddressToMemoryArea (
               pMachState,                   /* Machine state.             */
               pRetData,                     /* Return data.               */
               pMRI->Seg1,                   /* Segment to map.            */
               pMRI->Off1,                   /* Offset to map.             */
               ulMemoFlags,                  /* Desired memory attributes. */
               dw&1,                         /* 1-word, 0-byte.            */
               ppMA,                         /* Memory area for memo instr.*/
               ppMA2) == -1)                 /* M.A. if word crossing border.*/
      {
      /* Write/Read TRAP occurs.
       */
      return (-1);
      }


   /* Ok, here we have pMA (and probably pMA2) initialized.
    */
   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Function called generally for the instructions starting from 0x80...0x83.
 * These instructions are in format: III r/m,imm (ADD, CMP, SUB, XOR...).
 * Second byte of instruction is r/rm.
 * Function accepts as a parameter - pointer to an actual arithmetic
 * operation. It is possible, because in general all arithmetic operations
 * uses 2 operands.
 */

int         GenericArithm_8x (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               int            op1,           /* First byte of instruction. */
               BYTE           rel_seg,       /* Relative to segment.       */
               ULONG          mem_always,    /* If !=0 - use for memory.   */
               int            (*ActualArithm) (
                                 VOID *_pDest,   /* Ptr to dest operand.    */
                                 VOID *_pDest2,  /*!=0if dest-word cross mem*/
                                 VOID *_pSrc,    /* Ptr to source operand.  */
                                 VOID *_pSrc2,   /*!=0if src-word cross mem */
                                 int  size,     /* 1(byte) or 2(word).     */
                                 WORD *pFlags)  /* Ptr to rFlags.          */
               )

{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2;
   int            int2;
   int            imm1, imm2;
   VOID           *Ptr, *Ptr2;
/*-------------------------------------------------------------------------*/


   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         op1,                                /* First byte of instruction. */
         rel_seg,                            /* Relative to segment.       */
         mem_always,                         /* Use it if set (CMP, 8x ..).*/
         0,                                  /* Set 1 to init pReg2 - segreg.*/
         &mri,                               /* Mod/rm structure.          */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) != 0)                        /* M.A. if word crossing border.*/
      {

      /* Something went wrong. Probably needed memory area wasn't found.
       * <pRetData> already initialized. Return.
       * Caller will judge regarding of <ulIgnore> bits and current
       * value of <pRetCode>.
       *
       * We must update *pTempIP, so that it will point to next
       * instruction! That needed for case of ignore Trap, when
       * execution must be continued from the next instruction.
       */

//riad//
      int op2 = *pMachState->pOp2;
      (*pTempIP)=(WORD)(rIP+1);  /* for immed argument that follows */
      switch (op2>>6)
         {
         case  0: if ((op2&7) == 6) (*pTempIP)+=((WORD)4); else (*pTempIP)+=((WORD)2); break;
         case  1: (*pTempIP)+=((WORD)3);  break;
         case  2: (*pTempIP)+=((WORD)4);  break;
         case  3: (*pTempIP)+=((WORD)2);  break;
         }

      if(op1==0x81)  (*pTempIP)+=((WORD)1);
      if(rel_seg)    (*pTempIP)+=((WORD)1);

      return (-1);
      }


   /* Ok - here we have <pMA>, <mri> and probably <pMA2> initialized.
    * We now have everything to continue with executing of instruction.
    * Note: we don't need mri.pReg2. For 8x iii instruction second operand
    * will be imm8 or imm16, so we'll just ignore mri.pReg2.
    */
   imm1 = QueryNextOpcode (pMachState, pRetData, pTempIP);
   if (imm1 < 0)
      {
      /*----------------------------------------------------------------*/
      /* Code buffer not found! Nothing to do. Return...                */
      /* <pRetData> already initialized.                                */
      /* We may not worry about adjusting TempIP to the next instruction*/
      /* because code query trap is not recoverable.                    */
      /*----------------------------------------------------------------*/
      return (-1);
      }

   if (mri.pReg1)
      {
      /* Here we have iii r,imm. No memory related instructions.
       */
      switch (op1)
         {
         case  0x80: /* iii B/ R1,imm8                                     */
         case  0x82: /* iii B/ R1,imm8 (with sign ext.,which doesn't works */
            #ifdef   BIG_ENDIAN
               {
               /*----------------------------------------------------------*/
               BYTE  tmp1;
               /*----------------------------------------------------------*/
               tmp1 = (BYTE)imm1;
               ActualArithm (mri.pReg1, 0, &tmp1, 0, 1, &rFlags);
               imm1 = tmp1;
               }
            #else
            ActualArithm (mri.pReg1, 0, &imm1, 0, 1, &rFlags);
            #endif
            break;

         case  0x81: /* iii W/ R1,imm16                                    */
         case  0x83: /* iii W/ R1,imm8 with sign extension.                */
            if (op1==0x81)
               {
               imm2 = QueryNextOpcode (pMachState, pRetData, pTempIP);
               if (imm2 < 0)
                  {
                  /* Code buffer not found! Nothing to do. Return...
                   * <pRetData> already initialized.
                   * We may not worry about adjusting TempIP to the next
                   * instruction because code query trap is not recoverable
                   */
                  return (-1);
                  }

               int2 = MK_WORD (imm2, imm1);
               }
            else
               int2 = (int)((char)(imm1));      /* Sign extent it.         */

            #ifdef   BIG_ENDIAN
               {
               /*----------------------------------------------------------*/
               WORD  tmp1, tmp2;
               /*----------------------------------------------------------*/
               tmp1 = SWAPB (*(WORD *)(mri.pReg1));   /* Low endian now.   */
               tmp2 = SWAPB ((WORD)int2);             /* Low endian now.   */
               ActualArithm (&tmp1, 0, &tmp2, 0, 2, &rFlags);
               *(WORD *)(mri.pReg1) = SWAPB (tmp1);
               int2                 = SWAPB (tmp2);
               }
            #else
            ActualArithm (mri.pReg1, 0, &int2, 0, 2, &rFlags);
            #endif

            break;

         }

      return (0);
      }

   /* Initialize destination pointers for both cases.
    */
   if (pMA)
      Ptr =
         pMA->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1)-MALinearAddr(pMA));
   else
      Ptr = 0;

   if (pMA2)
      Ptr2 =
         pMA2->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1+1)-MALinearAddr(pMA2));
   else
      Ptr2 = 0;


   /* Here we have iii mem,imm. No register related instructions.
    */
   switch (op1)
      {
      case  0x80: /* iii B/ mem,imm8                                       */
      case  0x82: /* iii B/ mem,imm8 (with sign ext.,which doesn't works). */

         {
         /*----------------------------------------------------------------*/
         int   aaRetCode;
         /*----------------------------------------------------------------*/

         #ifdef   BIG_ENDIAN
            {
            /*-------------------------------------------------------------*/
            BYTE  tmp1;
            /*-------------------------------------------------------------*/
            tmp1 = (BYTE)imm1;
            aaRetCode = ActualArithm (Ptr, 0, &tmp1, 0, 1, &rFlags);
            imm1 = tmp1;
            }
         #else
         aaRetCode = ActualArithm (Ptr, 0, &imm1, 0, 1, &rFlags);
         #endif

         if (aaRetCode)
            {
            /* Update address of 'modified' byte(s) in touched memory area.
             */
            UpdateModifiedMA (
               pMachState,
               pRetData,                     /* Upd. flag for EXECUTABLE MA.*/
               pMA,                          /* Memory area for memo instr. */
               0,                            /* M.A. if word crossing border*/
               1,                            /* Bytes was written: 1 or 2.  */
               mri.Seg1,                     /* Real segment written to.    */
               mri.Off1);                    /* Real offset written to.     */
            }
         }

         break;


      case  0x81: /* iii W/ mem,imm16                                      */
      case  0x83: /* iii W/ mem,imm8 (with sign extension).                */

         {
         /*----------------------------------------------------------------*/
         int   aaRetCode;
         /*----------------------------------------------------------------*/
         if (op1==0x81)
            {
            imm2 = QueryNextOpcode (pMachState, pRetData, pTempIP);
            if (imm2 < 0)
               {
               /* Code buffer not found! Nothing to do. Return...
                * <pRetData> already initialized.
                * We may not worry about adjusting TempIP to the next
                * instruction because code query trap is not recoverable.
                */
               return (-1);
               }

            int2 = MK_WORD (imm2, imm1);
            }
         else
            int2 = (int)((char)(imm1));

         #ifdef   BIG_ENDIAN
            {
            /*-------------------------------------------------------------*/
            WORD  tmp;
            /*-------------------------------------------------------------*/

            tmp  = SWAPB ((WORD)int2);
            aaRetCode = ActualArithm (Ptr, Ptr2, &tmp, 0, 2, &rFlags);
            int2 = SWAPB (tmp);
            }
         #else
         aaRetCode = ActualArithm (Ptr, Ptr2, &int2, 0, 2, &rFlags);
         #endif

         if (aaRetCode)
            {
            /* Update address of 'modified' byte(s) in touched memory area.
             */
            UpdateModifiedMA (
               pMachState,
               pRetData,                     /* Upd. flag for EXECUTABLE MA.*/
               pMA,                          /* Memory area for memo instr. */
               pMA2,                         /* M.A. if word crossing border*/
               2,                            /* Bytes was written: 1 or 2.  */
               mri.Seg1,                     /* Real segment written to.    */
               mri.Off1);                    /* Real offset written to.     */
            }
         }

         break;


      default: break;
      }


   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Function called generally for the Grp2 and Grp3 instructions starting
 * with 0xFE...0xFF.
 * These instructions are in format: III r/m (INC, DEC).
 * Second byte of instruction is r/rm.
 * Function accepts as a parameter - pointer to an actual arithmetic
 * operation. It is possible, because in general all arithmetic operations
 * uses 2 operands.
 */

int         GenericGroup_Fx_INCDECNOTNEG (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               int            op1,           /* First  byte of instruction.*/
               int            instruction,   /* 0-INC, 1-DEC, 2-NOT, 3-NEG */
               BYTE           rel_seg        /* Relative to segment.       */
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2;
   int            temp_flag;
   ULONG          int_dest;
/*-------------------------------------------------------------------------*/


   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         op1,                                /* Bit0-W(1)/B, bit1-Direction.*/
         rel_seg,                            /* Relative to segment.       */
         READABLE|WRITEABLE,                 /* Use it if set (CMP, 8x ..).*/
         0,                                  /* Set 1 to init pReg2 - segreg.*/
         &mri,                               /* Mod/rm structure.          */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) != 0)                        /* M.A. if word crossing border.*/
      {
      /* Something went wrong. Probably needed memory area wasn't found.
       * <pRetData> already initialized. Return.
       * Caller will judge regarding of <ulIgnore> bits and current
       * value of <pRetCode>.
       * TempIP is already updated to the next instruction. So Trap could
       * be ignored normally.
       */
      return (-1);
      }


   /* Ok - here we have <pMA>, <mri> and probably <pMA2> initialized.
    * We now have everything to continue with executing of instruction.
    * Note: we don't need mri.pReg2. There is no second operand for
    * INC/DEC/NOT/NEG.
    * These 3 bits were used to determine between INC/DEC/NOT/NEG.
    */
   if (mri.pReg1)
      {
      /* Here we have INC/DEC/NOT/NEG r. No memory related instructions.
       */
      switch (instruction)
         {
         case  0x00: /* INC ...  */
         case  0x01: /* DEC ...  */
            temp_flag = fCF;
            SetArithmFlags (&rFlags, op1&1?2:1,
               op1&1?(*(WORD *)(mri.pReg1)):(*(BYTE *)(mri.pReg1)),
               instruction==0x00?1:-1);

            /* Execute actual instruction.
            */
            if (op1&1)
               *(WORD *)(mri.pReg1) += (short)(instruction==0x00?1:-1);
            else
               *(BYTE *)(mri.pReg1) += (char)(instruction==0x00?1:-1);

            /* CF not affected by DEC/INC instruction.
            */
            if (temp_flag)
               setCF;
            else
               clrCF;

            break;

         case  0x02: /* NOT ...  */

            if (op1&1)
               /* Do actual operation.
               */
               *(WORD *)(mri.pReg1) = (WORD)(~(*(WORD *)(mri.pReg1)));
            else
               *(BYTE *)(mri.pReg1) = (BYTE)(~(*(BYTE *)(mri.pReg1)));

            break;

         case  0x03: /* NEG ...  */

            if (op1&1)
               int_dest = *(WORD *)(mri.pReg1);
            else
               int_dest = *(BYTE *)(mri.pReg1);

            /* Set flags.
            */
            SetArithmFlags (&rFlags, op1&1?2:1, 0, -(long)int_dest);

            if (int_dest)
               setCF;
            else
               clrCF;

            /* Yes, this is right. First check source operand, than set
             * flags. Then perform operation.
             */
            if (op1&1)
               *(WORD *)(mri.pReg1) = (WORD)(-int_dest);
            else
               *(BYTE *)(mri.pReg1) = (BYTE)(-int_dest);

            break;
         }

      return (0);
      }


   /* Here we have INC/DEC/NOT/NEG mem. No register related instructions.
    */

   /* Query operand first. It will be modified and written back.
    */
   if (op1&1)
      int_dest = ReadWordFromMA (pMA, pMA2, mri.Seg1, mri.Off1);
   else
      int_dest = ReadByteFromMA (pMA, mri.Seg1, mri.Off1);

   switch (instruction)
      {
      case  0x00: /* INC ...  */
      case  0x01: /* DEC ...  */

         temp_flag = fCF;

         /* Set flags.
         */
         SetArithmFlags (&rFlags, op1&1?2:1, int_dest, instruction==0x00?1:-1);

         /* CF not affected by DEC/INC instruction.
         */
         if (temp_flag)
            setCF;
         else
            clrCF;

         /* Do actual operation.
         */
         int_dest    += instruction==0x00?1:-1;

         break;

      case  0x02: /* NOT ...  */

         int_dest = ~int_dest;

         break;

      case  0x03: /* NEG ...  */

         /* Set flags.
         */
         SetArithmFlags (&rFlags, op1&1?2:1, 0, -(long)int_dest);

         if (int_dest)
            setCF;
         else
            clrCF;

         int_dest  = -int_dest;

         break;
      }


   /* After <int_dest> was modified - it's time now to write it back.
    */

   if (op1&1)
      WriteWordToMA ((WORD)int_dest, pMA, pMA2, mri.Seg1, mri.Off1);
   else
      WriteByteToMA (int_dest, pMA, mri.Seg1, mri.Off1);

   /* Update address of 'modified' byte(s) in touched memory area.
   */
   UpdateModifiedMA (
      pMachState,
      pRetData,                           /* Upd. flag for EXECUTABLE MA.*/
      pMA,                                /* Memory area for memo instr. */
      pMA2,                               /* M.A. if word crossing border*/
      op1&1?2:1,                          /* Bytes was written: 1 or 2.  */
      mri.Seg1,                           /* Real segment written to.    */
      mri.Off1);                          /* Real offset written to.     */

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Function called generally for the arithmetic instructions.
 * These instructions are in format: III r/m,r (ADD, AND, SUB, XOR...).
 * Second byte of instruction is r/rm.
 * Function accepts as a parameter - pointer to an actual arithmetic
 * operation. It is possible, because in general all arithmetic operations
 * uses 2 operands.
 */

int         GenericArithm (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               int            op1,           /* First byte of instruction. */
               BYTE           rel_seg,       /* Relative to segment.       */
               ULONG          mem_always,    /* If !=0 - use for memory.   */
               BOOL           segreg,        /* -> 1-init Reg2 as segm. reg*/
               int            (*ActualArithm) (
                                 VOID *_pDest,  /* Ptr to dest operand.    */
                                 VOID *_pDest2, /* !=0 if dest-word cross mem*/
                                 VOID *pSrc,    /* Ptr to source operand.  */
                                 VOID *pSrc2,   /* !=0 if src-word cross mem */
                                 int  size,     /* 1(byte) or 2(word).     */
                                 WORD *pFlags)  /* Ptr to rFlags.          */
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2;
   VOID           *Ptr, *Ptr2;
/*-------------------------------------------------------------------------*/


   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         op1,                                /* First byte of instruction. */
         rel_seg,                            /* Relative to segment.       */
         mem_always,                         /* Use it if set (CMP, 8x ..).*/
         segreg,                             /* Set 1 to init pReg2 - segreg.*/
                                             /* Used for MOV (so far only).*/
         &mri,                               /* Mod/rm structure.          */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) != 0)                        /* M.A. if word crossing border.*/
      {
      /* Something went wrong. Probably needed memory area wasn't found.
       * <pRetData> already initialized. Return.
       * Caller will judge regarding of <ulIgnore> bits and current
       * value of <pRetCode>.
       */
      return (-1);
      }

   /* Ok - here we have <pMA>, <mri> and probably <pMA2> initialized.
    * We now have everything to continue with executing of instruction.
    */
   if (mri.pReg1)
      {
      /* Case of registers in use only.
       */
      switch (op1 & 0x03)
         {
         case  0x00: /* Byte instruction, iii B/ R1,R2                     */
            ActualArithm (mri.pReg1, 0, mri.pReg2, 0, 1, &rFlags); break;

         case  0x01: /* Word instruction, iii W/ R1,R2                     */
            #ifdef   BIG_ENDIAN
               {
               /*----------------------------------------------------------*/
               WORD  tmp1, tmp2;
               /*----------------------------------------------------------*/
               tmp1 = SWAPB (*(WORD *)mri.pReg1);
               tmp2 = SWAPB (*(WORD *)mri.pReg2);
               ActualArithm (&tmp1, 0, &tmp2, 0, 2, &rFlags);
               *(WORD *)mri.pReg1 = SWAPB (tmp1);
               *(WORD *)mri.pReg2 = SWAPB (tmp2);
               }
            #else
            ActualArithm (mri.pReg1, 0, mri.pReg2, 0, 2, &rFlags);
            #endif
            break;

         case  0x02: /* Byte instruction, iii B/ R2,R1                     */
            ActualArithm (mri.pReg2, 0, mri.pReg1, 0, 1, &rFlags); break;

         case  0x03: /* Word instruction, iii W/ R2,R1                     */
            #ifdef   BIG_ENDIAN
               {
               /*----------------------------------------------------------*/
               WORD  tmp1, tmp2;
               /*----------------------------------------------------------*/
               tmp1 = SWAPB (*(WORD *)mri.pReg1);
               tmp2 = SWAPB (*(WORD *)mri.pReg2);
               ActualArithm (&tmp2, 0, &tmp1, 0, 2, &rFlags);
               *(WORD *)mri.pReg1 = SWAPB (tmp1);
               *(WORD *)mri.pReg2 = SWAPB (tmp2);
               }
            #else
            ActualArithm (mri.pReg2, 0, mri.pReg1, 0, 2, &rFlags);
            #endif
            break;
         }

      return (0);
      }


   /* Initialize pointers for both cases.
    */
   if (pMA)
      Ptr =
         pMA->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1)-MALinearAddr(pMA));
   else
      Ptr = 0;

   if (pMA2)
      Ptr2 =
         pMA2->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1+1)-MALinearAddr(pMA2));
   else
      Ptr2 = 0;

   /* Here we have instruction with memory operand on one of sides.
    */
   switch (op1 & 0x03)
      {
      case  0x00: /* Byte instruction, iii M,R                             */

         if (ActualArithm (Ptr, 0, mri.pReg2, 0, 1, &rFlags))
            {
            /* Update address of 'modified' byte(s) in touched memory area.
             */
            UpdateModifiedMA (
               pMachState,
               pRetData,                    /* Upd. flag for EXECUTABLE MA.*/
               pMA,                         /* Memory area for memo instr. */
               0,                           /* M.A. if word crossing border*/
               1,                           /* Bytes was written: 1 or 2.  */
               mri.Seg1,                    /* Real segment written to.    */
               mri.Off1);                   /* Real offset written to.     */
            }

         break;


      case  0x01: /* Word instruction, iii M,R                             */
         {
         /*----------------------------------------------------------------*/
         int      aaRetCode;
         /*----------------------------------------------------------------*/

         #ifdef   BIG_ENDIAN
            {
            /*-------------------------------------------------------------*/
            WORD     tmp;
            /*-------------------------------------------------------------*/

            tmp = SWAPB (*(WORD *)mri.pReg2);
            aaRetCode = ActualArithm (Ptr, Ptr2, &tmp, 0, 2, &rFlags);
            *(WORD *)mri.pReg2 = SWAPB (tmp);
            }
         #else
         aaRetCode = ActualArithm (Ptr, Ptr2, mri.pReg2, 0, 2, &rFlags);
         #endif

         if (aaRetCode)
            {
            /* Update address of 'modified' byte(s) in touched memory area.
             */
            UpdateModifiedMA (
               pMachState,
               pRetData,                    /* Upd. flag for EXECUTABLE MA.*/
               pMA,                         /* Memory area for memo instr. */
               pMA2,                        /* M.A. if word crossing border*/
               2,                           /* Bytes was written: 1 or 2.  */
               mri.Seg1,                    /* Real segment written to.    */
               mri.Off1);                   /* Real offset written to.     */
            }

         }
         break;


      case  0x02: /* Byte instruction, iii R,M                             */

         ActualArithm (mri.pReg2, 0, Ptr, 0, 1, &rFlags);

         /* Update for XCHG instructions.
          */
         if (op1 == 0x86 || op1 == 0x87)
            {
            UpdateModifiedMA (
               pMachState,
               pRetData,                    /* Upd. flag for EXECUTABLE MA.*/
               pMA,                         /* Memory area for memo instr. */
               0,                           /* M.A. if word crossing border*/
               1,                           /* Bytes was written: 1 or 2.  */
               mri.Seg1,                    /* Real segment written to.    */
               mri.Off1);                   /* Real offset written to.     */
            }

         break;


      case  0x03: /* Word instruction, iii R,M                             */

         #ifdef   BIG_ENDIAN
            {
            /*-------------------------------------------------------------*/
            WORD     tmp;
            /*-------------------------------------------------------------*/

            tmp = SWAPB (*(WORD *)mri.pReg2);
            ActualArithm (&tmp, 0, Ptr, Ptr2, 2, &rFlags);
            *(WORD *)mri.pReg2 = SWAPB (tmp);
            }
         #else
         ActualArithm (mri.pReg2, 0, Ptr, Ptr2, 2, &rFlags);
         #endif

         /* Update for XCHG instructions.
          */
         if (op1 == 0x86 || op1 == 0x87)
            {
            UpdateModifiedMA (
               pMachState,
               pRetData,                    /* Upd. flag for EXECUTABLE MA.*/
               pMA,                         /* Memory area for memo instr. */
               pMA2,                        /* M.A. if word crossing border*/
               2,                           /* Bytes was written: 1 or 2.  */
               mri.Seg1,                    /* Real segment written to.    */
               mri.Off1);                   /* Real offset written to.     */
            }

         break;


      default:    break;
      }


   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Function called generally for the Shift/Rot instructions.
 * First byte of instruction is 0xD0...0xD3.
 * Second byte of instruction is r/rm and bits 3-5 defines what type of
 * instruction is currently executed.
 */

int         Generic_ShiftRot (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               int            op1,           /* First  byte of instruction.*/
               int            op2,           /* Second byte of instruction.*/
               BYTE           rel_seg        /* Relative to segment.       */
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2;
   int            rot_bits;
   VOID           *Ptr, *Ptr2;
/*-------------------------------------------------------------------------*/


   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         op1,                                /* First byte of instruction. */
         rel_seg,                            /* Relative to segment.       */
         READABLE|WRITEABLE,                 /* Use it if set (CMP, 8x ..).*/
         0,                                  /* Set 1 to init pReg2 - segreg.*/
         &mri,                               /* Mod/rm structure.          */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) != 0)                        /* M.A. if word crossing border.*/
      {
      /* Something went wrong. Probably needed memory area wasn't found.
       * <pRetData> already initialized. Return.
       * Caller will judge regarding of <ulIgnore> bits and current
       * value of <pRetCode>.
       *
       * We must update *pTempIP, so that it will point to next
       * instruction! That needed for case of ignore Trap, when
       * execution must be continued from the next instruction.
       */
      (*pTempIP)=rIP;
      switch (op2>>6)
         {
         case  0: if ((op2&7) == 6) (*pTempIP)+=((WORD)4); else (*pTempIP)+=((WORD)2); break;
         case  1: (*pTempIP)+=((WORD)3);  break;
         case  2: (*pTempIP)+=((WORD)4);  break;
         case  3: (*pTempIP)+=((WORD)2);  break;
         }

      if ((op1 & 0xFE) == 0xC0)
         (*pTempIP)+=((WORD)1);   /* Immed argument follows. */
//riad//
      if(rel_seg) 
         (*pTempIP)+=((WORD)1);

      return (-1);
      }


   /* Ok - here we have <pMA>, <mri> and probably <pMA2> initialized.
    * We now have everything to continue with executing of instruction.
    * Note: we don't need mri.pReg2. For ShiftRot instruction second operand
    * will define type of instruction, so we'll just ignore mri.pReg2.
    */
   if ((op1 & 0xFE) == 0xC0)
      {
      rot_bits = QueryNextOpcode (pMachState, pRetData, pTempIP);
      if (rot_bits < 0)
         {
         /* Code buffer not found! Nothing to do. Return...
          * <pRetData> already initialized.
          * We may not worry about adjusting TempIP to the next instruction
          * because code query trap is not recoverable.
          */
         return (-1);
         }
      }
   else
      {
      if (op1 & 2)
         rot_bits = rCL;
      else
         rot_bits = 1;
      }


   /* Initialize pointers for both cases.
    */
   if (pMA)
      Ptr =
         pMA->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1)-MALinearAddr(pMA));
   else
      Ptr = 0;

   if (pMA2)
      Ptr2 =
         pMA2->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1+1)-MALinearAddr(pMA2));
   else
      Ptr2 = 0;

   if (mri.pReg1)
      {
      /* Here we have iii r,CL|1|imm. No memory related instructions.
       */
      #ifdef   BIG_ENDIAN
         {
         /*----------------------------------------------------------------*/
         BYTE  b_tmp;
         WORD  w_tmp;
         /*----------------------------------------------------------------*/
         if (op1&1)  /* 0-byte, 1-word.   */
            {
            w_tmp   = SWAPB(*(WORD *)mri.pReg1);
            do_ShiftRot ((op2>>3)&7, &w_tmp, 0, rot_bits, op1&1, &rFlags);
            *(WORD *)mri.pReg1 = SWAPB(w_tmp);
            }
         else
            {
            b_tmp   = *(BYTE *)mri.pReg1;
            do_ShiftRot ((op2>>3)&7, &b_tmp, 0, rot_bits, op1&1, &rFlags);
            *(BYTE *)mri.pReg1 = b_tmp;
            }
         }
      #else
      do_ShiftRot ((op2>>3)&7, mri.pReg1, 0, rot_bits, op1&1, &rFlags);
      #endif
      }
   else
      {
      /* Here we have iii mem,CL|1|imm. No register related instructions.
       */
      do_ShiftRot (
         (op2>>3)&7,                         /* 0-7. Instruction type.     */
         Ptr,
         Ptr2,
         rot_bits,                           /* Rorate on number of bits   */
         op1&1,                              /* 0-byte, 1-word instruction.*/
         &rFlags);                           /*                            */

      /* Update address of 'modified' byte(s) in touched memory area.
       */
      UpdateModifiedMA (
         pMachState,
         pRetData,                           /* Upd. flag for EXECUTABLE MA*/
         pMA,                                /* Memory area for memo instr.*/
         pMA2,                               /* M.A. if word crossing border*/
         op1&1?2:1,                          /* Bytes was written: 1 or 2. */
         mri.Seg1,                           /* Real segment written to.    */
         mri.Off1);                          /* Real offset written to.    */
      }

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Generic function call for LES - LDS instructions.
 */

int         GenericLxS (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               BYTE           rel_seg,       /* Relative to segment.       */
               WORD           *pSegReg       /* &rDS or &rES.              */
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2;
/*-------------------------------------------------------------------------*/

   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         0x01,                               /* Give it as WORD instruction*/
         rel_seg,                            /* Relative to segment.       */
         READABLE,                           /* Use it if set (CMP, 8x ..).*/
         0,                                  /* Set 1 to init pReg2 - segreg.*/
                                             /* Used for MOV (so far only).*/
         &mri,                               /* Mod/rm structure.          */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) != 0)                        /* M.A. if word crossing border.*/
      {
      /* Something went wrong. Probably needed memory area wasn't found.
       * <pRetData> already initialized. Return.
       * Caller will judge regarding of <ulIgnore> bits and current
       * value of <pRetCode>.
       */
      return (-1);
      }

   if (mri.pReg1)
      {
      /* Cannot be. If happened - invalid instruction in place.
       * Must be memory reference instead of direct register.
       */
      pRetData->SegTrap    = rCS;
      pRetData->OffTrap    = rIP;
      pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
      return (-1);
      }

   /* Initialize non-segment register of instruction.
    */
   *(WORD *)(mri.pReg2) = ReadWordFromMA (pMA, pMA2, mri.Seg1, mri.Off1);


   /* Get ready to initialize Segment part of LxS instruction.
    */
   if (MapAddressToMemoryArea (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         mri.Seg1,                           /* Segment to map.            */
         (WORD)(mri.Off1+2),                 /* Offset to map.             */
         READABLE,                           /* Desired memory attributes. */
         2,                                  /* 1-word, 0-byte.            */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) == -1)                       /* M.A. if word crossing border*/
      {
      /* READ/WRITE trap occurs. <pRetData> already initialized.
       * Exiting...
       */
      return (-1);
      }

   /* Initialize Segment register of instruction.
    */
   *pSegReg = ReadWordFromMA (pMA, pMA2, mri.Seg1, (WORD)(mri.Off1+2));


   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Function to execute MUL, IMUL, DIV, IDIV.
 * Function also accepts pointer to an actual MUL/DIV function to execute.
 */

int         GenericMulDiv (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               int            op1,           /* First byte of instruction. */
               BYTE           rel_seg,       /* Relative to segment.       */
               int            (*ActualMulDiv) (
                                 MACHINE_STATE  *pMachState,
                                 RETURN_DATA    *pRetData,
                                 int            word,
                                 VOID           *pMult,
                                 VOID           *pMult2)
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2;
   VOID           *Ptr, *Ptr2;
/*-------------------------------------------------------------------------*/


   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         op1,                                /* First byte of instruction. */
         rel_seg,                            /* Relative to segment.       */
         READABLE,                           /* Use it if set (CMP, 8x ..).*/
         0,                                  /* Set 1 to init pReg2 - segreg*/
                                             /* Used for MOV (so far only).*/
         &mri,                               /* Mod/rm structure.          */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) != 0)                        /* M.A. if word crossing border*/
      {
      /* Something went wrong. Probably needed memory area wasn't found.
       * <pRetData> already initialized. Return.
       * Caller will judge regarding of <ulIgnore> bits and current
       * value of <pRetCode>.
       */
      return (-1);
      }

   /* Ok - here we have <pMA>, <mri> and probably <pMA2> initialized.
    * We now have everything to continue with executing of instruction.
    * Note: we don't need <mri.pReg2>, because it always be AL/X or DX:AX.
    * These 3 bits are part of an instruction.
    */

   /* Initialize pointers for both cases.
    */
   if (pMA)
      Ptr =
         pMA->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1)-MALinearAddr(pMA));
   else
      Ptr = 0;

   if (pMA2)
      Ptr2 =
         pMA2->pBuff+(WORD)(LinearAddr(mri.Seg1,mri.Off1+1)-MALinearAddr(pMA2));
   else
      Ptr2 = 0;

   if (mri.pReg1)
      /* Case of registers in use only.
       */
#ifdef   BIG_ENDIAN
      {
      /*-------------------------------------------------------------------*/
      int   rc;
      WORD  w_tmp;
      /*-------------------------------------------------------------------*/

      if (op1&1)  /* 0-byte, 1-word.   */
         {
         w_tmp   = SWAPB(*(WORD *)mri.pReg1);
         rc = ActualMulDiv  (pMachState, pRetData, op1&1, &w_tmp, 0);
         *(WORD *)mri.pReg1 = SWAPB(w_tmp);
         }
      else
         rc = ActualMulDiv  (pMachState, pRetData, op1&1, mri.pReg1, 0);

      return (rc);
      }
#else
      return (ActualMulDiv  (pMachState, pRetData, op1&1, mri.pReg1, 0));
#endif
   else
      return (ActualMulDiv  (
                  pMachState, pRetData, 
                  op1&1, Ptr, (op1&1 && pMA2)?Ptr2:0));

}
/*=========================================================================*/
