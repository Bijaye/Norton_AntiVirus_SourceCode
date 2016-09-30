/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_RUNR1.C                                               *
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


#include "cr_all.h"
#if defined(CR_DEBUG)
extern FILE *cr_dbgf;
extern int cr_hd;      /* Set to 1 to turn on coderunner output to file. */
extern int cr_itrace;  /* Set to 1 to turn on coderunner instruction trace. */
#endif

/*------------------------------------------------------------------------*/
/*                            Macros.                                     */
/*------------------------------------------------------------------------*/

/* Macro reads next opcode from rCS:TempIP.
 * Accepts as a parameter pointer to <int>.
 */
#define  GET_NEXT_OPCODE_TO(ptr2op)                                        \
   {                                                                       \
   *ptr2op = QueryNextOpcode (pMachState, pRetData, &TempIP);              \
   if (*ptr2op < 0)                                                        \
      {                                                                    \
      /* Code buffer not found! Nothing to do. Return... */                \
      /* <pRetData> already initialized.                 */                \
                                                                           \
      /* Set this variable. This variable is verified at the beginning of  \
       * CodeRunner so that <pfnBreakConditionCheck> and Trace INT won't   \
       * get executed before instruction, when CR execution resumed        \
       * after TRAP recovery or other condition.                           \
       */                                                                  \
      pMachState->ExecutionBroken = TRUE;                                  \
      pMachState->ulIgnoreNextOpcode = 0;                                  \
                                                                           \
      return;                                                              \
      }                                                                    \
   }


/*------------------------------------------------------------------------*/
/*                        Static Functions.                               */
/*------------------------------------------------------------------------*/

static int  OkToContinueLoop (
               MACHINE_STATE  *pMachState,   /* <-> Input/Output.          */
               RETURN_DATA    *pRetData      /* <-  Output.                */
               );


/*=========================================================================*/
/*                                                                         */
/* CodeRunner will try to execute code starting from rCS:rIP until:        */
/* -  Attempt to read from non-readable (or non supplied) memory.          */
/* -  Attempt to write to non-writeable ( - - - ) memory.                  */
/* -  Attempt to execute from non-executable ( - - - ) area.               */
/* -  Attempt to execute unsupported instruction.                          */
/* -  Attempt to execute <maxops>+1 instruction.                           */
/* -  Attempt to set TF.                                                   */
/* -  Attempt to divide by 0.                                              */
/* -  Result of division won't fit into the destination operand.           */
/* -  Attempt to execute port I/O instruction.                             */
/* -  Attempt to execute INT xx without supplied pointer to user-defined   */
/*    function or without supplied IVT or without enough stack space.      */
/* -  User supplied pointer to INT-function returned <USER_ABORT>.         */
/* -  User supplied <pfnBreak> function returned TRUE and related flag     */
/*    is not set as ignorable.                                             */
/*                                                                         */
/* If any of above condition will happen and related <Ignore> bit is not   */
/* set <CodeRunner> will return. Otherwise execution will continue after   */
/* related bit in <RetCode> will be set.                                   */
/* Note: EXECUTE_TRAP and USER_ABORT cannot be masked by <ulIgnore> bits.  */
/*                                                                         */
/* o  Executable code and data could reside into the few                   */
/*    (contigious or not) buffers.                                         */
/* o  Upon return rIP points to an instruction which did not get executed  */
/*    or caused a TRAP. At that point <.SegTrap> and <.OffTrap> are points */
/*    to the requested code/data area (real world segment/offset).         */
/*                                                                         */
/*                     Code/Data in MEMORY_AREA.                           */
/*                     -------------------------                           */
/*                                                                         */
/*                         <Seg>-+- /0000/                                 */
/*                               |                                         */
/*                               |                                         */
/*                               |                                         */
/*                     <Off>---->+-+<--<pBuff>                             */
/*                               |o|\                                      */
/*                     [IP/SP-->]|o| \                                     */
/*                               |o|  <BuffSize>                           */
/*                               |o| /                                     */
/*                               |o|/                                      */
/*                               +-+                                       */
/*                               |                                         */
/*                               |                                         */
/*                               |                                         */
/*                              -+- /FFFF/                                 */
/*                                                                         */
/* ----------------------------------------------------------------------- */
/*                                                                         */
/* Parameters:                                                             */
/*    pMachState - pointer to a structure with starting values of          */
/*                 all registers, linked list of memory areas, pointer     */
/*                 to caller-supplied function to emulate INT xx,          */
/*                  maximum allowed instructions to execute                */
/*                 and <Ignore> bits.                                      */
/*                 Upon exit all registers get updated and rCS:rIP will    */
/*                 point to the next instruction to execute.               */
/*                                                                         */
/*    pRetData --- Data to be initialized upon termination of routine:     */
/*                 total number of executed instructions,                  */
/*                 <Segment>:<Offset> of memory area, caused termination,  */
/*                 flag, whether code modified itself and misc bits,       */
/*                 explaining reason for termination.                      */
/*                 Note: <CodeRunner> will zero following vars in          */
/*                 <pRetData>, before starting:                            */
/*                 <SegTrap>   (will be changed upon exit)                 */
/*                 <OffTrap>   (will be changed upon exit)                 */
/*                 <ulRetCode> (will be changed upon exit)                 */
/*                 <ops>               will be updated upon exit, but      */
/*                 <CodeRunner> will not reset it before starting.         */
/*                                                                         */
/*=========================================================================*/

void  CodeRunner   (
         MACHINE_STATE  *pMachState,         /* <-> Input/Output.          */
         RETURN_DATA    *pRetData            /* <-  Output.                */
         )
{
/*-------------------------------------------------------------------------*/
   WORD           TempIP;                    /* <TempIP> is a temporary copy
                                              * of <rIP>, until sucessful
                                              * execution of instruction.
                                              * Until this will happen - only
                                              * <CurrentIP> will be
                                              * incremented.
                                              */

                                             /* Temp holders of real address
                                              * of last successful opcode.
                                              */
   WORD           last_good_opcode_CS, last_good_opcode_IP;
   int            op1, op2, op3, op4, op5;
   WORD           temp_word;
   int            temp_flag;
   BYTE           rel_seg;                   /* Relative segment prefix.   */
   int            REP_bomb;                  /* REPZ/REPNZ not string instruction */
/*-------------------------------------------------------------------------*/


   /* Do some initialization/resetting of locals upon starting...
    */
   //riad//
   pMachState->pOp1     = (BYTE *)(&op1); /* Initialize pointers to opcodes*/
   pMachState->pOp2     = (BYTE *)(&op2);
   pMachState->pOp3     = (BYTE *)(&op3);
   pMachState->pOp4     = (BYTE *)(&op4);
   pMachState->pOp5     = (BYTE *)(&op5);
   pRetData->ops       -= ((ULONG)1);     /* Get updated before loop started. */
   pRetData->SegTrap    = 0;
   pRetData->OffTrap    = 0;
   pRetData->ulRetCode  = 0;
   rel_seg              = 0;
   TempIP               = rIP;


   /* Even if <EXECUTE_TRAP> or <USER_ABORT> bits are set, we must
    * reset them back. These conditions cannot be ignored.
    */
   pMachState->ulIgnore &= ~((ULONG)EXECUTE_TRAP);
   pMachState->ulIgnore &= ~((ULONG)USER_ABORT);


   /* Reset this variables.
    */
   last_good_opcode_CS = 0;
   last_good_opcode_IP = 0;


   /*----------------------------------------------------------------------*/
   /* Start main loop. Most instructions will update registers and flags   */
   /* in <pMachState> and also <pRetData> if TRAP occurs.                  */
   /*----------------------------------------------------------------------*/

   do
      {
      /* Flag to indicate that we failed a REPXX OPC because it
       * wasn't a string operation.
       */
      REP_bomb = 0;

      /* Update IP register after pass and <ops> - number of
       * executed instructions.
       */
      if (!rel_seg)
         /* Do it only if last opcode wasn't segment prefix. Segment prefix
          * is not an instruction. It only must initialize <rel_seg> and
          * increment <TempIP>.
          */
         {
         rIP = TempIP;
         pRetData->ops++;

         /* Re-initialize last successful instruction address.
          */
         pRetData->PrevCS     = last_good_opcode_CS;
         pRetData->PrevIP     = last_good_opcode_IP;

         last_good_opcode_CS  = rCS;
         last_good_opcode_IP  = rIP;

         /* If TF is set and at least one instruction was successfully
          * executed or ingored - execute INT 01h.
          */
#if 0
         if (fTF && pRetData->ops && (pMachState->ExecutionBroken != TRUE))
            {
            ExecuteUserInt  (pMachState, pRetData, &TempIP, 1);

            /* Check maybe return condition is already generated.
             */
            if (!OkToContinueLoop (pMachState, pRetData))
               break;   /* Get out from loop.                                 */
            }
#else
         if (pMachState->INT01Pending)
         {
            ExecuteUserInt  (pMachState, pRetData, &TempIP, 1);
            // riad
            // Very important for executed section update
            _MinMaxTable.dojump  = MODEJUMP;
            last_good_opcode_CS  = rCS;
            last_good_opcode_IP  = rIP;
         }

         pMachState->INT01Pending = fTF ? 1 : 0;
#endif

         /* If <pMachState->pfnBreak> - pointer to the user
          * Break-Condition-Check function is supplied - execute it after
          * each successfully executed or ignored instruction.
          * If function returned TRUE - set related bit in return data.
          */
         if (pMachState->pfnBreak && pRetData->ops
            && (pMachState->ExecutionBroken != TRUE))
            {
            if (pMachState->pfnBreak (pMachState, pRetData))
               {
               pRetData->ulRetCode |= BREAK_CONDITION;

               /* Check maybe return condition is already generated.
                */
               if (
                  (pRetData->ulRetCode & ~pMachState->ulIgnore) ||
                  (pRetData->ops >= (pMachState->maxops - 1l)) 
                  )
                  break;   /* Get out from loop.                              */
               }
            }
         }

      /* Flag to indicate that we're not emulating a REP MOVS instruction,
       * set to 1 if we *are* emulating a REP MOVS instruction.
       */
      _InREP_MOVS = 0;
      //riad//
      _InREP_LODS = 0;
      _ActiveInstructionFlag = 0;
#ifdef CR_DEBUG
      if (cr_hd && cr_itrace) 
         {
         if(!rel_seg)  /* jce - fixed CS:IP for instructions following
                          segment prefix */
         fprintf(cr_dbgf, "CS:IP = %04X:%04X, ops = ", rCS, rIP);
         else  /* if prefix in effect, IP has not been updated
                  yet, but in reality the following instruction is at
                  the next offset */
         fprintf(cr_dbgf, "CS:IP = %04X:%04X, ops = ", rCS, rIP+1);
         }
#endif

#ifdef CR_MOREFEATURES2
#else
      {
      ULONG InputLA;
      InputLA = LinearAddr(rCS, rIP);
      if (InputLA >= _ulPrevOpcodeLA)
         ++_ulIncreasingExecutionAddr;
      else
         _ulIncreasingExecutionAddr = 0l;
      _ulPrevOpcodeLA = InputLA;
      /*printf("_ulIncreasingExecutionAddr=%lu, %08lX\n", _ulIncreasingExecutionAddr, (unsigned long)InputLA);*/
      }
#endif

      // _OpcodeSize is initialized here, and updated in QueryNextOpcode()
      // Important to put it before reading any byte from the filename

      if(!rel_seg)   _OpcodeSize  = 0;   //riad//


      /* Query next opcode. If cannot be queried - it will execute
       * <return> after related init. of <pRetData>.
       */
      GET_NEXT_OPCODE_TO (&op1)

      /* Save first byte of instruction.
       */
      pMachState->PrevOpcode = (BYTE) op1;

      /* Here is the main switchboard.
       */

      switch (op1)
         {
         case  0x00: /* ADD r/m,r8                                         */
         case  0x01: /* ADD r/m,r16                                        */
         case  0x02: /* ADD r8,r/m                                         */
         case  0x03: /* ADD r16,r/m                                        */
            {
            ULONG mem_always = READABLE;

            if (op1 == 0x00 || op1 == 0x01)
               mem_always |= WRITEABLE;

            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, mem_always, FALSE, do_ADD);
            }
            break;

         case  0x04: /* ADD AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            SetArithmFlags (&rFlags, 1, rAL, op2);
            rAL += ((BYTE)op2); break;

         case  0x05: /* ADD AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            SetArithmFlags (&rFlags, 2, rAX, MK_WORD (op3, op2));
            rAX += MK_WORD (op3, op2); break;

         case  0x06: /* PUSH ES                                            */
            do_PUSH_POP (pMachState, pRetData, &rES, 1); break;

         case  0x07: /* POP  ES                                            */
            do_PUSH_POP (pMachState, pRetData, &rES, 0); break;

         case  0x08: /* OR  r/m,r8                                         */
         case  0x09: /* OR  r/m,r16                                        */
         case  0x0A: /* OR  r8,r/m                                         */
         case  0x0B: /* OR  r16,r/m                                        */
            {
            ULONG mem_always = READABLE;

            if (op1 == 0x08 || op1 == 0x09)
               mem_always |= WRITEABLE;

            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, mem_always, FALSE, do_OR);
            }
            break;

         case  0x0C: /* OR  AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            rAL |= ((BYTE)op2);
            SetAndOrXorFlags  (&rFlags, (char)rAL);
            break;

         case  0x0D: /* OR  AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            rAX |= MK_WORD (op3, op2);
            SetAndOrXorFlags  (&rFlags, (int)rAX);
            break;

         case  0x0E: /* PUSH CS                                            */
            do_PUSH_POP (pMachState, pRetData, &rCS, 1); break;

         case  0x10: /* ADC r/m,r8                                         */
         case  0x11: /* ADC r/m,r16                                        */
         case  0x12: /* ADC r8,r/m                                         */
         case  0x13: /* ADC r16,r/m                                        */
            {
            ULONG mem_always = READABLE;

            if (op1 == 0x10 || op1 == 0x11)
               mem_always |= WRITEABLE;

            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, mem_always, FALSE, do_ADC);
            }
            break;

         case  0x14: /* ADC AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            SetArithmFlags (&rFlags, 1, rAL, op2+fCF);
            rAL += ((BYTE)(op2+fCF)); break;

         case  0x15: /* ADC AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            SetArithmFlags (&rFlags, 2, rAX, MK_WORD (op3, op2)+fCF);
            rAX += (WORD)(MK_WORD (op3, op2) + fCF); break;

         case  0x16: /* PUSH SS                                            */
            do_PUSH_POP (pMachState, pRetData, &rSS, 1); break;

         case  0x17: /* POP  SS                                            */
            do_PUSH_POP (pMachState, pRetData, &rSS, 0); break;

         case  0x18: /* SBB r/m,r8                                         */
         case  0x19: /* SBB r/m,r16                                        */
         case  0x1A: /* SBB r8,r/m                                         */
         case  0x1B: /* SBB r16,r/m                                        */
            {
            ULONG mem_always = READABLE;

            if (op1 == 0x18 || op1 == 0x19)
               mem_always |= WRITEABLE;

            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, mem_always, FALSE, do_SBB);
            }
            break;

         case  0x1C: /* SBB AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            SetArithmFlags (&rFlags, 1, rAL, -(long)(op2+fCF));
            rAL -= ((BYTE)((BYTE)op2+fCF)); break;

         case  0x1D: /* SBB AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            SetArithmFlags (&rFlags, 2, rAX, -(long)(MK_WORD (op3, op2)+fCF));
            rAX -= ((WORD)(MK_WORD (op3, op2) + fCF)); break;

         case  0x1E: /* PUSH DS                                            */
            do_PUSH_POP (pMachState, pRetData, &rDS, 1); break;

         case  0x1F: /* POP  DS                                            */
            do_PUSH_POP (pMachState, pRetData, &rDS, 0); break;

         case  0x20: /* AND r/m,r8                                         */
         case  0x21: /* AND r/m,r16                                        */
         case  0x22: /* AND r8,r/m                                         */
         case  0x23: /* AND r16,r/m                                        */
            {
            ULONG mem_always = READABLE;

            if (op1 == 0x20 || op1 == 0x21)
               mem_always |= WRITEABLE;

            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, mem_always, FALSE, do_AND);
            }
            break;

         case  0x24: /* AND AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            rAL &= ((BYTE)op2);
            SetAndOrXorFlags  (&rFlags, (char)rAL);
            break;

         case  0x25: /* AND AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            rAX &= MK_WORD (op3, op2);
            SetAndOrXorFlags  (&rFlags, (int)rAX);
            break;

         case  0x26: /* ES: ... segment prefix.                            */
            rel_seg=(BYTE)op1;   break;

         case  0x27: /* DAA                                                */
            if (((rAL & 0x0F) > 9) || fAF) {rAL+=((BYTE)6); setAF;} else {clrAF;}
            if (rAL>0x9F || fCF)        {rAL+=((BYTE)0x60); setCF;} else {clrCF;}

            /* Set flags.
             */
            if (rAL)             clrZF; else setZF;
            if (rAL&0x80)        setSF; else clrSF;
            if (ParityFlag(rAL)) setPF; else clrPF;

            break;

         case  0x28: /* SUB r/m,r8                                         */
         case  0x29: /* SUB r/m,r16                                        */
         case  0x2A: /* SUB r8,r/m                                         */
         case  0x2B: /* SUB r16,r/m                                        */
            {
            ULONG mem_always = READABLE;

            if (op1 == 0x28 || op1 == 0x29)
               mem_always |= WRITEABLE;

            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, mem_always, FALSE, do_SUB);
            }
            break;

         case  0x2C: /* SUB AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            SetArithmFlags (&rFlags, 1, rAL, -(long)op2);
            rAL -= (BYTE)op2; break;

         case  0x2D: /* SUB AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            SetArithmFlags (&rFlags, 2, rAX, -(long)MK_WORD (op3, op2));
            rAX -= MK_WORD (op3, op2); break;

         case  0x2E: /* CS: ... segment prefix.                            */
            rel_seg=(BYTE)op1;   break;

         case  0x2F: /* DAS                                                */
            if (((rAL & 0x0F) > 9) || fAF) {rAL-=((BYTE)6); setAF;} else {clrAF;}
            if (rAL>0x9F || fCF)        {rAL-=((BYTE)0x60); setCF;} else {clrCF;}

            /* Set flags.
             */
            if (rAL)             clrZF; else setZF;
            if (rAL&0x80)        setSF; else clrSF;
            if (ParityFlag(rAL)) setPF; else clrPF;

            break;

         case  0x30: /* XOR r/m,r8                                         */
         case  0x31: /* XOR r/m,r16                                        */
         case  0x32: /* XOR r8,r/m                                         */
         case  0x33: /* XOR r16,r/m                                        */
            {
            ULONG mem_always = READABLE;

            if (op1 == 0x30 || op1 == 0x31)
               mem_always |= WRITEABLE;

            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, mem_always, FALSE, do_XOR);
            }
            break;

         case  0x34: /* XOR AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            rAL ^= ((BYTE)op2);
            SetAndOrXorFlags  (&rFlags, (char)rAL);
            break;

         case  0x35: /* XOR AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            rAX ^= MK_WORD (op3, op2);
            SetAndOrXorFlags  (&rFlags, (int)rAX);
            break;

         case  0x36: /* SS: ... segment prefix.                            */
            rel_seg=(BYTE)op1;   break;

         case  0x37: /* AAA                                                */
            if (((rAL & 0x0F) > 9) || fAF)
               { rAL = ((BYTE)(rAL+(BYTE)6) & (BYTE)0x0F); rAH+=((BYTE)1); setAF; setCF; }
            else
               { clrCF; clrAF; }
            break;

         case  0x38: /* CMP r/m,r8                                         */
         case  0x39: /* CMP r/m,r16                                        */
         case  0x3A: /* CMP r8,r/m                                         */
         case  0x3B: /* CMP r16,r/m                                        */
            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, READABLE, FALSE, do_CMP);
            break;

         case  0x3C: /* CMP AL,imm8                                        */
            GET_NEXT_OPCODE_TO (&op2)
            SetArithmFlags (&rFlags, 1, rAL, -(long)op2); break;

         case  0x3D: /* CMP AX,imm16                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            SetArithmFlags (&rFlags, 2, rAX, -(long)MK_WORD (op3, op2)); break;

         case  0x3E: /* DS: ... segment prefix.                            */
            rel_seg=(BYTE)op1;   break;

         case  0x3F: /* AAS                                                */
            if (((rAL & 0x0F) > 9) || fAF)
               {rAL -= ((BYTE)6); rAL&=0x0F; rAH -= ((BYTE)1); setAF; setCF;}
            else
               {clrCF; clrAF;}
            break;

         case  0x40: /* INC W/reg                                          */
         case  0x41: /* INC W/reg                                          */
         case  0x42: /* INC W/reg                                          */
         case  0x43: /* INC W/reg                                          */
         case  0x44: /* INC W/reg                                          */
         case  0x45: /* INC W/reg                                          */
         case  0x46: /* INC W/reg                                          */
         case  0x47: /* INC W/reg                                          */
         case  0x48: /* DEC W/reg                                          */
         case  0x49: /* DEC W/reg                                          */
         case  0x4A: /* DEC W/reg                                          */
         case  0x4B: /* DEC W/reg                                          */
         case  0x4C: /* DEC W/reg                                          */
         case  0x4D: /* DEC W/reg                                          */
         case  0x4E: /* DEC W/reg                                          */
         case  0x4F: /* DEC W/reg                                          */
            temp_flag = fCF;
            switch (op1&0x07)
               {
               case  0x00: SetArithmFlags (&rFlags, 2, rAX, op1<0x48?1:-1);
                           if (op1<0x48) rAX++; else rAX--;  break;
               case  0x01: SetArithmFlags (&rFlags, 2, rCX, op1<0x48?1:-1);
                           if (op1<0x48) rCX++; else rCX--;  break;
               case  0x02: SetArithmFlags (&rFlags, 2, rDX, op1<0x48?1:-1);
                           if (op1<0x48) rDX++; else rDX--;  break;
               case  0x03: SetArithmFlags (&rFlags, 2, rBX, op1<0x48?1:-1);
                           if (op1<0x48) rBX++; else rBX--;  break;
               case  0x04: SetArithmFlags (&rFlags, 2, rSP, op1<0x48?1:-1);
                           if (op1<0x48) rSP++; else rSP--;  break;
               case  0x05: SetArithmFlags (&rFlags, 2, rBP, op1<0x48?1:-1);
                           if (op1<0x48) rBP++; else rBP--;  break;
               case  0x06: SetArithmFlags (&rFlags, 2, rSI, op1<0x48?1:-1);
                           if (op1<0x48) rSI++; else rSI--;  break;
               case  0x07: SetArithmFlags (&rFlags, 2, rDI, op1<0x48?1:-1);
                           if (op1<0x48) rDI++; else rDI--;  break;
               }
            if (temp_flag) setCF; else clrCF;   /* CF not aff. by DEC/INC  */
            break;

         case  0x50: /* PUSH reg16                                         */
         case  0x51:
         case  0x52:
         case  0x53:
         case  0x54:
         case  0x55:
         case  0x56:
         case  0x57:
         case  0x58: /* POP  reg16                                         */
         case  0x59:
         case  0x5A:
         case  0x5B:
         case  0x5C:
         case  0x5D:
         case  0x5E:
         case  0x5F:
            switch (op1&0x07)
               {
               case  0x00: do_PUSH_POP (pMachState, pRetData, &rAX, (BOOL)((op1&8)?0:1)); break;
               case  0x01: do_PUSH_POP (pMachState, pRetData, &rCX, (BOOL)((op1&8)?0:1)); break;
               case  0x02: do_PUSH_POP (pMachState, pRetData, &rDX, (BOOL)((op1&8)?0:1)); break;
               case  0x03: do_PUSH_POP (pMachState, pRetData, &rBX, (BOOL)((op1&8)?0:1)); break;
               case  0x04: do_PUSH_POP (pMachState, pRetData, &rSP, (BOOL)((op1&8)?0:1)); break;
               case  0x05: do_PUSH_POP (pMachState, pRetData, &rBP, (BOOL)((op1&8)?0:1)); break;
               case  0x06: do_PUSH_POP (pMachState, pRetData, &rSI, (BOOL)((op1&8)?0:1)); break;
               case  0x07: do_PUSH_POP (pMachState, pRetData, &rDI, (BOOL)((op1&8)?0:1)); break;
               }
            break;

         case  0x60: /* PUSHA: Push: AX, CX, DX, BX, xxx, BP, SI, DI.      */
            { WORD xxx=0;
            if (do_PUSH_POP (pMachState, pRetData, &rAX, 1) == 0)
             if (do_PUSH_POP (pMachState, pRetData, &rCX, 1) == 0)
              if (do_PUSH_POP (pMachState, pRetData, &rDX, 1) == 0)
               if (do_PUSH_POP (pMachState, pRetData, &rBX, 1) == 0)
                if (do_PUSH_POP (pMachState, pRetData, &xxx, 1) == 0)
                 if (do_PUSH_POP (pMachState, pRetData, &rBP, 1) == 0)
                  if (do_PUSH_POP (pMachState, pRetData, &rSI, 1) == 0)
                       do_PUSH_POP (pMachState, pRetData, &rDI, 1);
            }
            break;

         case  0x61: /* POPA: Pop: DI, SI, BP, XXX, BX, DX, CX, AX.        */
            { WORD xxx;
            if (do_PUSH_POP (pMachState, pRetData, &rDI, 0) == 0)
             if (do_PUSH_POP (pMachState, pRetData, &rSI, 0) == 0)
              if (do_PUSH_POP (pMachState, pRetData, &rBP, 0) == 0)
               if (do_PUSH_POP (pMachState, pRetData, &xxx, 0) == 0)
                if (do_PUSH_POP (pMachState, pRetData, &rBX, 0) == 0)
                 if (do_PUSH_POP (pMachState, pRetData, &rDX, 0) == 0)
                  if (do_PUSH_POP (pMachState, pRetData, &rCX, 0) == 0)
                       do_PUSH_POP (pMachState, pRetData, &rAX, 0);
            }
            break;

         case  0x68: /* PUSH imm16                                         */
            {
            WORD   Value;

            GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
            Value = MK_WORD (op3, op2);
            do_PUSH_POP (pMachState, pRetData, &Value, 1);
            }
            break;

         case  0x6A: /* PUSH imm8 (with sign extension).                   */
            {
            WORD   Value;

            GET_NEXT_OPCODE_TO (&op2)
            Value = (WORD)((int)((char)op2));
            do_PUSH_POP (pMachState, pRetData, &Value, 1);
            }
            break;

         case  0x70: /* JO      xx                                         */
         case  0x71: /* JNO     xx                                         */
         case  0x72: /* JB/JNAE xx                                         */
         case  0x73: /* JNB/JAE xx                                         */
         case  0x74: /* JE/JZ   xx                                         */
         case  0x75: /* JNE/JNZ xx                                         */
         case  0x76: /* JBE/JNA xx                                         */
         case  0x77: /* JNBE/JA xx                                         */
         case  0x78: /* JS      xx                                         */
         case  0x79: /* JNS     xx                                         */
         case  0x7A: /* JP/JPE  xx                                         */
         case  0x7B: /* JNP/JPO xx                                         */
         case  0x7C: /* JL/JNG  xx                                         */
         case  0x7D: /* JNL/JGE xx                                         */
         case  0x7E: /* JLE/JNG xx                                         */
         case  0x7F: /* JNLE/JG xx                                         */
            GET_NEXT_OPCODE_TO (&op2)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {          
#ifdef CR_MOREFEATURES  //riad//
               // Well, for speed reason only, but this makes this code
               // dependent on something that shouldn't be
               if(_MinMaxTable.dojump == MODEJUMP) TempIP+=(char)op2;
#else
               switch (op1)
                  {
                  case  0x70: if (fOF)          TempIP+=(char)op2;    break;
                  case  0x71: if (!fOF)         TempIP+=(char)op2;    break;
                  case  0x72: if (fCF)          TempIP+=(char)op2;    break;
                  case  0x73: if (!fCF)         TempIP+=(char)op2;    break;
                  case  0x74: if (fZF)          TempIP+=(char)op2;    break;
                  case  0x75: if (!fZF)         TempIP+=(char)op2;    break;
                  case  0x76: if (fCF  || fZF)  TempIP+=(char)op2;    break;
                  case  0x77: if (!fCF && !fZF) TempIP+=(char)op2;    break;
                  case  0x78: if (fSF)          TempIP+=(char)op2;    break;
                  case  0x79: if (!fSF)         TempIP+=(char)op2;    break;
                  case  0x7A: if (fPF)          TempIP+=(char)op2;    break;
                  case  0x7B: if (!fPF)         TempIP+=(char)op2;    break;
                  case  0x7C: if (fSF != fOF)   TempIP+=(char)op2;    break;
                  case  0x7D: if (fSF == fOF)   TempIP+=(char)op2;    break;
                  case  0x7E: if (fZF  || (fSF != fOF)) TempIP+=(char)op2;break;
                  case  0x7F: if (!fZF && (fSF == fOF)) TempIP+=(char)op2;break;
                  }
#endif
               }
            }
            break;

         case  0x80: /* ArOp1 r/m8,imm8                                    */
         case  0x81: /* ArOp1 r/m16,imm16                                  */
         case  0x82: /* ArOp2 r/m8,imm8 (sign ext. doesn't works for B/dest*/
         case  0x83: /* ArOp2 r/m16,imm8 (with sign extension).            */
            GET_NEXT_OPCODE_TO (&op2)  /* Query next opcode.               */
            TempIP--;   /* AS if op2 wasn't queried.                       */
            _OpcodeSize--; //riad//
            switch ((op2 >> 3) & 7)
               {
               case  0x00: /* ADD ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE|WRITEABLE, do_ADD);
                  break;

               case  0x01: /* OR  ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE|WRITEABLE, do_OR);
                  break;

               case  0x02: /* ADC ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE|WRITEABLE, do_ADC);
                  break;

               case  0x03: /* SBB ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE|WRITEABLE, do_SBB);
                  break;

               case  0x04: /* AND ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE|WRITEABLE, do_AND);
                  break;

               case  0x05: /* SUB ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE|WRITEABLE, do_SUB);
                  break;

               case  0x06: /* XOR ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE|WRITEABLE, do_XOR);
                  break;

               case  0x07: /* CMP ...                                      */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                                    op1, rel_seg, READABLE,  do_CMP);
                  break;

               default:
                  pRetData->SegTrap    = rCS;
                  pRetData->OffTrap    = rIP;
                  pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
                  break;
               }
            break;

         case  0x84: /* TEST r/m, reg8                                     */
         case  0x85: /* TEST r/m, reg16                                    */
            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, READABLE, FALSE, do_TEST);
            break;

         case  0x86: /* XCHG r8/rm                                         */
         case  0x87: /* XCHG r16/rm                                        */
            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, READABLE|WRITEABLE, FALSE, do_XCHG);
            break;

         case  0x88: /* MOV r/m,r8                                         */
         case  0x89: /* MOV r/m,r16                                        */
         case  0x8A: /* MOV r8,r/m                                         */
         case  0x8B: /* MOV r16,r/m                                        */
            GenericArithm (pMachState, pRetData, &TempIP,
                           op1, rel_seg, 0, FALSE, do_MOV);
            break;

         case  0x8C: /* MOV r/m,segreg                                     */
            GenericArithm (pMachState, pRetData, &TempIP,
                           op1|1, rel_seg, 0, TRUE, do_MOV);
            break;

         case  0x8D: /* LEA r16,mem                                        */
            do_LEA (pMachState, pRetData, &TempIP);
            break;

         case  0x8E: /* MOV segreg,r/m                                     */
            GenericArithm (pMachState, pRetData, &TempIP,
                           op1|1, rel_seg, 0, TRUE, do_MOV);
            break;

         case  0x8F: /* POP r/m                                            */
            PUSH_POP_rm (pMachState, pRetData, &TempIP, 0, rel_seg);
            break;

         case  0x90: /* NOP (XCHG AX,AX)                                   */
            break;

         case  0x91: /* XCHG  AX,CX                                        */
            { temp_word=rAX; rAX=rCX; rCX=temp_word; } break;

         case  0x92: /* XCHG  AX,DX                                        */
            { temp_word=rAX; rAX=rDX; rDX=temp_word; } break;

         case  0x93: /* XCHG  AX,BX                                        */
            { temp_word=rAX; rAX=rBX; rBX=temp_word; } break;

         case  0x94: /* XCHG  AX,SP                                        */
            { temp_word=rAX; rAX=rSP; rSP=temp_word; } break;

         case  0x95: /* XCHG  AX,BP                                        */
            { temp_word=rAX; rAX=rBP; rBP=temp_word; } break;

         case  0x96: /* XCHG  AX,SI                                        */
            { temp_word=rAX; rAX=rSI; rSI=temp_word; } break;

         case  0x97: /* XCHG  AX,DI                                        */
            { temp_word=rAX; rAX=rDI; rDI=temp_word; } break;

         case  0x98: /* CBW                                                */
            rAX = (char) rAL;
            break;

         case  0x99: /* CWD                                                */
            {
            LONG     DX_AX;

            DX_AX = (int) rAX;
            rDX   = (WORD) (DX_AX>>16);
            rAX   = (WORD) (DX_AX & 0xFFFF);
            }
            break;

         case  0x9A: /* CALL Far Off Seg                                   */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            GET_NEXT_OPCODE_TO (&op4)
            GET_NEXT_OPCODE_TO (&op5)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl&&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               if (do_PUSH_POP (pMachState, pRetData, &rCS, 1)    == -1)
                  {
                  /* Canot PUSH return CS into stack. Write TRAP?
                  * Call cannot be executed. IP unchanged.
                  */
                  break;
                  }
               if (do_PUSH_POP (pMachState, pRetData, &TempIP, 1) == -1)
                  {
                  /* Canot PUSH return IP into stack. Write TRAP?
                  * Call cannot be executed. IP unchanged.
                  */
                  break;
                  }
               rCS    = MK_WORD (op5, op4);
               TempIP = MK_WORD (op3, op2);
               }
            }
            break;

         case  0x9B: /* WAIT                                               */
            break;

         case  0x9C: /* PUSHF                                              */
            do_PUSH_POP (pMachState, pRetData, &rFlags, 1);
            break;

         case  0x9D: /* POPF                                               */
            do_PUSH_POP (pMachState, pRetData, &rFlags, 0);
            if (fTF)
               pRetData->ulRetCode |= TRACE_FLAG_SET;
            else
               pRetData->ulRetCode &= ~TRACE_FLAG_SET;
            break;

         case  0x9E: /* SAHF                                               */
            rFlags = rAH;
            break;

         case  0x9F: /* LAHF                                               */
            rAH = (BYTE) rFlags;
            break;

         case  0xA0: /* MOV AL,mem8                                        */
         case  0xA1: /* MOV AX,mem16                                       */
         case  0xA2: /* MOV mem8,AL                                        */
         case  0xA3: /* MOV mem16,AX                                       */
            do_MOV_Ax_imm (pMachState, pRetData, &TempIP, op1, rel_seg);
            break;

         case  0xA4: /* MOVSB                                              */
         case  0xA5: /* MOVSW                                              */
            do_MOVS (pMachState, pRetData, op1&1, rel_seg, 0);
            break;

         case  0xA6: /* CMPSB                                              */
         case  0xA7: /* CMPSW                                              */
            do_CMPS (pMachState, pRetData, op1&1, rel_seg, 0);
            break;

         case  0xA8: /* TEST AL, imm8                                      */
            GET_NEXT_OPCODE_TO (&op2)
            SetAndOrXorFlags  (&rFlags, (char)(rAL & ((BYTE)op2)));
            break;

         case  0xA9: /* TEST AL, imm16                                     */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            SetAndOrXorFlags  (&rFlags, (int)(rAX & MK_WORD (op3, op2)));
            break;

         case  0xAA: /* STOSB                                              */
         case  0xAB: /* STOSW                                              */
            do_STOS (pMachState, pRetData, op1&1, 0);
            break;

         case  0xAC: /* LODSB                                              */
         case  0xAD: /* LODSW                                              */
            do_LODS (pMachState, pRetData, op1&1, rel_seg, 0);
            break;

         case  0xAE: /* SCASB                                              */
         case  0xAF: /* SCASW                                              */
            do_SCAS (pMachState, pRetData, op1&1, 0);
            break;

         case  0xB0: /* MOV bytereg,imm8                                   */
         case  0xB1:
         case  0xB2:
         case  0xB3:
         case  0xB4:
         case  0xB5:
         case  0xB6:
         case  0xB7:
            GET_NEXT_OPCODE_TO (&op2)
            switch (op1&7)
               {
               case  0: rAL=(BYTE)op2; break;
               case  1: rCL=(BYTE)op2; break;
               case  2: rDL=(BYTE)op2; break;
               case  3: rBL=(BYTE)op2; break;
               case  4: rAH=(BYTE)op2; break;
               case  5: rCH=(BYTE)op2; break;
               case  6: rDH=(BYTE)op2; break;
               case  7: rBH=(BYTE)op2; break;
               }
            break;

                     /* MOV wordreg,imm16                                  */
         case  0xB8: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rAX = MK_WORD (op3, op2);  break;
         case  0xB9: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rCX = MK_WORD (op3, op2);  break;
         case  0xBA: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rDX = MK_WORD (op3, op2);  break;
         case  0xBB: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rBX = MK_WORD (op3, op2);  break;
         case  0xBC: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rSP = MK_WORD (op3, op2);  break;
         case  0xBD: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rBP = MK_WORD (op3, op2);  break;
         case  0xBE: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rSI = MK_WORD (op3, op2);  break;
         case  0xBF: GET_NEXT_OPCODE_TO (&op2)  GET_NEXT_OPCODE_TO (&op3)
                     rDI = MK_WORD (op3, op2);  break;

         case  0xC0: /* SHR, ROL, RCR, RCL, RAR...                         */
         case  0xC1: /* SHR, ROL, RCR, RCL, RAR...                         */
            GET_NEXT_OPCODE_TO (&op2)
            TempIP--;   /* AS if op2 wasn't queried.                       */
            _OpcodeSize--; //riad//
            Generic_ShiftRot (pMachState, pRetData, &TempIP, op1, op2, rel_seg);
            break;

         case  0xC2: /* RET near imm16                                     */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               do_PUSH_POP (pMachState, pRetData, &TempIP, 0);
               rSP+=MK_WORD (op3, op2);
               }
            }
            break;

         case  0xC3: /* RET near                                           */
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               do_PUSH_POP (pMachState, pRetData, &TempIP, 0);
               }
            }
            break;

         case  0xC4: /* LES reg16, mem                                     */
         case  0xC5: /* LDS reg16, mem                                     */
            GenericLxS (pMachState, pRetData, &TempIP, rel_seg,
                        (op1==0xC4)?(&rES):(&rDS));
            break;

         case  0xC6: /* MOV r/m8,  imm8                                    */
         case  0xC7: /* MOV r/m16, imm16                                   */
            GenericArithm_8x (pMachState, pRetData, &TempIP,
               (op1==0xC6)?0x80:0x81, rel_seg, WRITEABLE, do_MOV);
            break;

         case  0xCA: /* RET far  imm16                                     */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               do_PUSH_POP (pMachState, pRetData, &TempIP, 0);
               do_PUSH_POP (pMachState, pRetData, &rCS, 0);
               rSP+=MK_WORD (op3, op2);
               }
            }
            break;

         case  0xCB: /* RET far                                            */
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               do_PUSH_POP (pMachState, pRetData, &TempIP, 0);
               do_PUSH_POP (pMachState, pRetData, &rCS, 0);
               }
            }
            break;

         case  0xCC: /* INT 3                                              */
            //riad//
            if (pMachState->pfnExecControl) pMachState->pfnExecControl(pMachState, pRetData);
            ExecuteUserInt  (pMachState, pRetData, &TempIP, 3);
            break;

         case  0xCD: /* INT imm8                                           */
            //riad//
            if (pMachState->pfnExecControl) pMachState->pfnExecControl(pMachState, pRetData);
            GET_NEXT_OPCODE_TO (&op2)
            ExecuteUserInt  (pMachState, pRetData, &TempIP, op2);
            break;

         case  0xCE: /* INTO (INT 04 if OF==1)                             */
            //riad//
            if (pMachState->pfnExecControl) pMachState->pfnExecControl(pMachState, pRetData);
            if (!fOF)
               break;
            else
               ExecuteUserInt  (pMachState, pRetData, &TempIP, 4);
            break;

         case  0xCF: /* IRET                                               */
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               do_PUSH_POP (pMachState, pRetData, &TempIP, 0);
               do_PUSH_POP (pMachState, pRetData, &rCS, 0);
               do_PUSH_POP (pMachState, pRetData, &rFlags, 0);
               if (fTF)
                  pRetData->ulRetCode |= TRACE_FLAG_SET;
               else
                  pRetData->ulRetCode &= ~TRACE_FLAG_SET;
               }
            }
            break;

         case  0xD0: /* SHR, ROL, RCR, RCL, RAR...                         */
         case  0xD1: /* SHR, ROL, RCR, RCL, RAR...                         */
         case  0xD2: /* SHR, ROL, RCR, RCL, RAR...                         */
         case  0xD3: /* SHR, ROL, RCR, RCL, RAR...                         */
            GET_NEXT_OPCODE_TO (&op2)
            TempIP--;   /* AS if op2 wasn't queried.                       */
            _OpcodeSize--; //riad//
            Generic_ShiftRot (pMachState, pRetData, &TempIP, op1, op2, rel_seg);
            break;

         case  0xD4: /* AAM                                                */
            /* Next opcode is suppose to be 0x0A. And almost all debuggers
             * are getting confused if there is something different, than
             * 0x0A. But actual CPU will use the second byte as an operand,
             * even though this feature is not documented. So CodeRunner
             * will use second byte as an operand, instead of using divide
             * or multiply by 10 (0x0A).
             */

            GET_NEXT_OPCODE_TO (&op2)

            /* Following is the right code. But it is commented, so that
             * CodeRunner would support ANY second byte.
             *
             * if (op2 == 0x0A)
             *    {
             *    rAH = rAL / 10;
             *    rAL = rAL - ((rAL / 10) * 10);
             *
             *    Set flags.
             *
             *    if (rAL)             clrZF; else setZF;
             *    if (rAL&0x80)        setSF; else clrSF;
             *    if (ParityFlag(rAL)) setPF; else clrPF;
             *    }
             * else
             *    {
             *    pRetData->SegTrap    = rCS;
             *    pRetData->OffTrap    = rIP;
             *    pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
             *    }
             */

            if (op2)
               {
               rAH = (BYTE)(rAL / ((BYTE)op2));
               rAL = (BYTE)(rAL - ((rAL / ((BYTE)op2)) * ((BYTE)op2)));

               /* Set flags.
               */
               if (rAL)             clrZF; else setZF;
               if (rAL&0x80)        setSF; else clrSF;
               if (ParityFlag(rAL)) setPF; else clrPF;
               }
            else
               pRetData->ulRetCode |= ZERODIVIDE;

            break;

         case  0xD5: /* AAD                                                */
            GET_NEXT_OPCODE_TO (&op2)

            /* Right-to-execute sequence is commented out. See above.
             *
             *
             * if (op2 == 0x0A)
             *    {
             *    rAL = rAH*10 + rAL;
             *    rAH = 0;
             *
             *    Set flags.
             *
             *    if (rAL)             clrZF; else setZF;
             *    if (rAL&0x80)        setSF; else clrSF;
             *    if (ParityFlag(rAL)) setPF; else clrPF;
             *    }
             * else
             *    {
             *    pRetData->SegTrap    = rCS;
             *    pRetData->OffTrap    = rIP;
             *    pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
             *    }
             */

            rAL = (BYTE)(rAH*((BYTE)op2) + rAL);
            rAH = 0;

            /* Set flags.
             */
            if (rAL)             clrZF; else setZF;
            if (rAL&0x80)        setSF; else clrSF;
            if (ParityFlag(rAL)) setPF; else clrPF;
            break;

         case  0xD7: /* XLAT  (MOV AL,DS:[BX+AL])                          */
            {
            MEMORY_AREA *pMA, *pMA2;

            if (MapAddressToMemoryArea (
                  pMachState, pRetData, 
                  rDS, (WORD)(rBX+rAL), READABLE, 0,
                  &pMA, &pMA2) != 0)
               break;
            else
               rAL = ReadByteFromMA (pMA, rDS, rBX+rAL);
            }

            break;

         case  0xD8: /* ESC xxx */
         case  0xD9:
         case  0xDA:
         case  0xDB:
         case  0xDC:
         case  0xDD:
         case  0xDE:
         case  0xDF:
            /* Do nothing if ESC (80x87) -related instruction met.
             * Just read off second byte.
             */
            GET_NEXT_OPCODE_TO (&op2)
            break;

         case  0xE0: /* LOOPNZ                                             */
            GET_NEXT_OPCODE_TO (&op2)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               rCX--;
               if (rCX && !fZF)
                  TempIP+=(char)op2;
               }
            }
            break;

         case  0xE1: /* LOOPZ                                              */
            GET_NEXT_OPCODE_TO (&op2)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               rCX--;
               if (rCX && fZF)
                  TempIP+=(char)op2;
               }
            }
            break;

         case  0xE2: /* LOOP                                               */
            GET_NEXT_OPCODE_TO (&op2)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               rCX--;
               if (rCX)
                  TempIP+=(char)op2;
               }
            }
            break;

         case  0xE3: /* JCXZ xx                                            */
            GET_NEXT_OPCODE_TO (&op2)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               if (!rCX)
                  TempIP+=(char)op2;
               }
            }
            break;

         case  0xE4: /* IN  AL,port8 - return something non-zero           */
            GET_NEXT_OPCODE_TO (&op2)
            pRetData->ulRetCode |= PORT_IO;
            rAL = 0xFF;
            break;
         case  0xE5: /* IN  AX,port8 - return something non-zero           */
            GET_NEXT_OPCODE_TO (&op2)
            pRetData->ulRetCode |= PORT_IO;
            rAX = 0xFFFF;
            break;

         case  0xE6: /* OUT AL,port8                                       */
         case  0xE7: /* OUT AX,port8                                       */
            GET_NEXT_OPCODE_TO (&op2)
            pRetData->ulRetCode |= PORT_IO;
            break;

         case  0xE8: /* CALL Near Off                                      */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               if (do_PUSH_POP (pMachState, pRetData, &TempIP, 1) == -1)
                  {
                  /* Canot PUSH return IP into stack. Write TRAP?
                  * Call cannot be executed. IP unchanged.
                  */
                  break;
                  }
               TempIP += (short)(MK_WORD (op3, op2));
               }
            }
            break;

         case  0xE9: /* JMP Near Off                                       */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               TempIP += (short)(MK_WORD (op3, op2));
               pMachState->PQLinearAddr = (unsigned long)-1; /* Invalidate prefetch cache.   */
               }
            }
            break;

         case  0xEA: /* JMP Far Off Seg                                    */
            GET_NEXT_OPCODE_TO (&op2)
            GET_NEXT_OPCODE_TO (&op3)
            GET_NEXT_OPCODE_TO (&op4)
            GET_NEXT_OPCODE_TO (&op5)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               rCS    = MK_WORD (op5, op4);
               TempIP = MK_WORD (op3, op2);
               pMachState->PQLinearAddr = (unsigned long)-1; /* Invalidate prefetch cache.   */
               }
            }
            break;

         case  0xEB: /* JMP  xx                                            */
            GET_NEXT_OPCODE_TO (&op2)
            {
            /*---------------------------------------------*/
            int   rc;
            /*---------------------------------------------*/
            if (pMachState->pfnExecControl &&
               ((rc=pMachState->pfnExecControl(pMachState, pRetData))
                  !=USER_EXEC_CONTINUE)
               )
               {
               switch (rc)
                  {
                  case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                     break;               /* Just do nothing... skipped!   */

                  case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                  default:
                     pRetData->ulRetCode |= USER_ABORT;  break;
                  }
               }
            else
               {
               TempIP+=(char)op2;
               pMachState->PQLinearAddr = (unsigned long)-1; /* Invalidate prefetch cache.*/
               }
            }
            break;

         case  0xEC: /* IN  AL,DX - return something non-zero              */
            pRetData->ulRetCode |= PORT_IO;
            rAL = 0xFF;
            break;
         case  0xED: /* IN  AX,DX - return something non-zero              */
            pRetData->ulRetCode |= PORT_IO;
            rAX = 0xFFFF;
            break;

         case  0xEE: /* OUT AL,DX                                          */
         case  0xEF: /* OUT AX,DX                                          */
            pRetData->ulRetCode |= PORT_IO;
            break;

         case  0xF0: /* LOCK                                               */
            break;

         case  0xF2: /* REPNZ (REPNE) prefix.                              */
         case  0xF3: /* REP   (REPZ ) prefix.                              */
            while(1)
               {
               GET_NEXT_OPCODE_TO(&op2)
               /* Check for: REP CS:MOVSB  and similar sequences.          */
               if (op2 == 0x26 || op2 == 0x2E || op2 == 0x36 || op2 == 0x3E)
                  rel_seg=(BYTE)op2;
               else
                  break;
               }

            switch (op2)
               {
               case  0xA4: /* MOVSB                                              */
               case  0xA5: /* MOVSW                                              */
                  /*do_MOVS (pMachState, pRetData, op2&1, rel_seg, (BYTE)op1);*/
                  {
                  int   rc;
                  WORD  wBytes = op2&1 ? (WORD)(rCX*2) : rCX;
                  if (
                     pMachState->pfnWipingMemory   &&
                     (rc =  pMachState->pfnWipingMemory (
                              pMachState, pRetData,
                              rES, fDF ? (WORD)(rDI-wBytes) : rDI,
                              wBytes
                              )) !=USER_EXEC_CONTINUE
                     )
                     {
                     switch (rc)
                        {
                        case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                           break;               /* Just do nothing... skipped!   */

                        case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                        default:
                           pRetData->ulRetCode |= USER_ABORT;
                           break;
                        }
                     }
                  else
                     {
                     _InREP_MOVS = 1;
                     do_MOVS (pMachState, pRetData, op2&1, rel_seg, (BYTE)op1);
                     }
                  }
                  break;

               case  0xA6: /* CMPSB                                              */
               case  0xA7: /* CMPSW                                              */
                  //riad//
                  _InREP_LODS = 1;
                  do_CMPS (pMachState, pRetData, op2&1, rel_seg, (BYTE)op1);
                  break;

               case  0xAA: /* STOSB                                              */
               case  0xAB: /* STOSW                                              */
                  /*!!! do_STOS (pMachState, pRetData, op2&1, (BYTE)op1);*/
                  {
                  int   rc;
                  WORD  wBytes = op2&1 ? (WORD)(rCX*2) : rCX;
                  if (
                     pMachState->pfnWipingMemory   &&
                     (rc =  pMachState->pfnWipingMemory (
                              pMachState, pRetData, rES,
                              fDF ? (WORD)(rDI-wBytes) : rDI,
                              wBytes
                              )) !=USER_EXEC_CONTINUE
                     )
                     {
                     switch (rc)
                        {
                        case  USER_EXEC_SKIP:   /* Skip this instruction.        */
                           break;               /* Just do nothing... skipped!   */

                        case  USER_EXEC_ABORT:  /* Abort everything and exit.    */
                        default:
                           pRetData->ulRetCode |= USER_ABORT;
                           break;
                        }
                     }
                  else
                     do_STOS (pMachState, pRetData, op2&1, (BYTE)op1);
                  }
                  break;

               case  0xAC: /* LODSB                                              */
               case  0xAD: /* LODSW                                              */
                  //riad//
                  _InREP_LODS = 1;
                  do_LODS (pMachState, pRetData, op2&1, rel_seg, (BYTE)op1);
                  break;

               case  0xAE: /* SCASB                                              */
               case  0xAF: /* SCASW                                              */
                  //riad//
                  _InREP_LODS = 1;
                  do_SCAS (pMachState, pRetData, op2&1, (BYTE)op1);
                  break;

               default:    /* REP INC AX and similars will be ignored.     */
                  TempIP--;   /* Recover after "GET_NEXT_OPCODE_TO (&op2)" */
                  REP_bomb = 1;
                  _OpcodeSize--; //riad//
                  break;
               }

            break;

         case  0xF4: /* HLT                                                */
            pRetData->ulRetCode |= HLT_EXECUTED;
            break;

         case  0xF5: /* CMC  (CF = ~CF)                                    */
            if (fCF) clrCF; else setCF;
            break;

         case  0xF6: /* III   r/m8,  imm8                                  */
         case  0xF7: /* III   r/m16, imm16                                 */
            GET_NEXT_OPCODE_TO (&op2)
            TempIP--;   /* AS if op2 wasn't queried.                       */
            _OpcodeSize--; //riad//
            switch ((op2 >> 3) & 7)
               {
               case  0x00: /* TEST ...                                     */
               case  0x01: /* TEST ... <- it is undocumented instruction
                            * but some CPU's duplicates it as TEST and it
                            * was seen in some viruses.
                            */
                  GenericArithm_8x (pMachState, pRetData, &TempIP,
                     (op1&1)|0x80, rel_seg, READABLE, do_TEST);
                  break;

               case  0x02: /* NOT r/m                                      */
               case  0x03: /* NEG r/m                                      */
                  GenericGroup_Fx_INCDECNOTNEG (
                     pMachState,             /* Machine state.             */
                     pRetData,               /* Return data.               */
                     &TempIP,                /* Ptr to temp IP.            */
                     op1,                    /* First  byte of instruction.*/
                     ((op2>>3)&7),           /* 0-INC, 1-DEC, 2-NOT, 3-NEG */
                     rel_seg                 /* Relative to segment.       */
                     );
                  break;

               case  0x04: /* MUL xxx                                      */
                  GenericMulDiv (pMachState, pRetData, &TempIP,
                                 op1, rel_seg, do_MUL);
                  break;

               case  0x05: /* IMUL xxx                                     */
                  GenericMulDiv (pMachState, pRetData, &TempIP,
                                 op1, rel_seg, do_IMUL);
                  break;

               case  0x06: /* DIV xxx                                      */
                  GenericMulDiv (pMachState, pRetData, &TempIP,
                                 op1, rel_seg, do_DIV);
                  break;

               case  0x07: /* IDIV xxx                                     */
                  GenericMulDiv (pMachState, pRetData, &TempIP,
                                 op1, rel_seg, do_IDIV);
                  break;

               default:
                  /* This case become unreachable, since we decided to
                   * include case 01 into switch.
                   */
                  pRetData->SegTrap    = rCS;
                  pRetData->OffTrap    = rIP;
                  pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
                  break;
               }
            break;

         case  0xF8: /* CLC                                                */
            clrCF;   break;

         case  0xF9: /* STC                                                */
            setCF;   break;

         case  0xFA: /* CLI                                                */
            clrIF;   break;

         case  0xFB: /* STI                                                */
            setIF;   break;

         case  0xFC: /* CLD                                                */
            clrDF;   break;

         case  0xFD: /* STD                                                */
            setDF;   break;

         case  0xFE: /* Grp2                                               */
            GET_NEXT_OPCODE_TO (&op2)  /* Query next opcode.               */
            TempIP--;   /* AS if op2 wasn't queried.                       */
            _OpcodeSize--; //riad//
            switch ((op2 >> 3) & 7)
               {
               case  0x00: /* INC ...                                      */
               case  0x01: /* DEC ...                                      */
                  GenericGroup_Fx_INCDECNOTNEG (
                     pMachState,             /* Machine state.             */
                     pRetData,               /* Return data.               */
                     &TempIP,                /* Ptr to temp IP.            */
                     op1,                    /* First  byte of instruction.*/
                     ((op2>>3)&7),           /* 0-INC, 1-DEC, 2-NOT, 3-NEG */
                     rel_seg                 /* Relative to segment.       */
                     );
                  break;

               default:
                  pRetData->SegTrap    = rCS;
                  pRetData->OffTrap    = rIP;
                  pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
                  break;
               }
            break;

         case  0xFF: /* Grp3                                               */
            GET_NEXT_OPCODE_TO (&op2)  /* Query next opcode.               */
            TempIP--;   /* AS if op2 wasn't queried.                       */
            _OpcodeSize--; //riad//
            switch ((op2 >> 3) & 7)
               {
               case  0x00: /* INC ...                                      */
               case  0x01: /* DEC ...                                      */
                  GenericGroup_Fx_INCDECNOTNEG (
                     pMachState,             /* Machine state.             */
                     pRetData,               /* Return data.               */
                     &TempIP,                /* Ptr to temp IP.            */
                     op1,                    /* First  byte of instruction.*/
                     ((op2>>3)&7),           /* 0-INC, 1-DEC, 2-NOT, 3-NEG */
                     rel_seg                 /* Relative to segment.       */
                     );
                  break;

               case  0x02:
                  //riad//
                  if (pMachState->pfnExecControl) pMachState->pfnExecControl(pMachState, pRetData);
                  JMP_CALL_rm (pMachState, pRetData, 
                     &TempIP, 0, 1, rel_seg);
                  break;

               case  0x03:
                  //riad//
                  if (pMachState->pfnExecControl) pMachState->pfnExecControl(pMachState, pRetData);
                  JMP_CALL_rm (pMachState, pRetData, 
                     &TempIP, 0, 0, rel_seg);
                  break;

               case  0x04:
                  //riad//
                  if (pMachState->pfnExecControl) pMachState->pfnExecControl(pMachState, pRetData);
                  JMP_CALL_rm (pMachState, pRetData, 
                     &TempIP, 1, 1, rel_seg);
                  break;

               case  0x05:
                  //riad//
                  if (pMachState->pfnExecControl) pMachState->pfnExecControl(pMachState, pRetData);
                  JMP_CALL_rm (pMachState, pRetData, 
                     &TempIP, 1, 0, rel_seg);
                  break;

               case  0x06: /* PUSH ...                                     */
                  PUSH_POP_rm (pMachState, pRetData, 
                     &TempIP, 1, rel_seg);
                  break;

               default:
                  pRetData->SegTrap    = rCS;
                  pRetData->OffTrap    = rIP;
                  pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
                  break;
               }
            break;


         default:
            pRetData->SegTrap    = rCS;
            pRetData->OffTrap    = rIP;
            pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
            break;
         }

      /* Reset this variable every time. It'll be set only in CR will be
       * terminated.
       */
      pMachState->ExecutionBroken = FALSE;

      /* Reset segment prefix after every instruction.
       */
      if ( !(op1 & 0x06) || (op1 != 0x26 && op1 != 0x36 && op1 != 0x2E && op1 != 0x3E)
           && !REP_bomb)
         {
         rel_seg=0;

         /* Reset bits, which must be ignored according to
          * <ms.ulIgnoreNextOpcode>. Segment prefix is not considered
          * as instruction, so this code is in 'else' part.
          */
         if (pMachState->ulIgnoreNextOpcode)
            {
            pRetData->ulRetCode &= (~pMachState->ulIgnoreNextOpcode);
            pMachState->ulIgnoreNextOpcode = 0;
            }
         }

#ifdef CR_DEBUG
      if (cr_hd && cr_itrace) fprintf(cr_dbgf, "\n");
#endif

/*****************************************************************************
 *
 *     This part of pitful code handle the switch to another machine state
 *
 *****************************************************************************/

      //riad//
      // Here code is added for code data segregation attempt, to segregate
      // bytes that don't have a clue what's that for anyway

      if((com_options & OPT_WOW)  && _Default_ini.MaxDeepness)
      {
         if(_MinMaxTable.codata == TEST_CODEDATA)
         {
            machine_stuff *temp;

            _MinMaxTable.codata = 0;   // reinitialize it to a magic number that is 
                                       // quite nothing in fact

            // Now, allocate another machine_state section, save the current
            // machinestate and retdata, updates the links in the linked list
            // and continue as nothing happened

            if((temp=update_machine_state(pMachState, pRetData, (char)op2, 
               (_MinMaxTable.dojump==MODEJUMP)?TempIP-(char)op2:TempIP)) == NULL)
            {
               fprintf(stderr, "\n\n\t!!! A frightfully fatalistic error occured in"
                               " update_machine_state()\n\texit(-1) or something\n\n");
               // gonna execute pMachState->ExecutionBroken = TRUE;
               break;
            }
            // we are going to use element returned by update_machine_state
            pMachState = &(temp->ms);
            pRetData   = &(temp->rd);
            //_IVT       = temp->_IVT;
            //_CR_PSP    = temp->_CR_PSP;
         }
      }

/*****************************************************************************
 *
 *     This part of pitful code handled the switch to another machine state
 *
 *****************************************************************************/

      } /* do {... */

   while (   
   /* Do while ulRetCode is zero, or bits are set, which could be ignored
    * and number of instructions already executed are less than
    * <maxops-1>.
    */
      //riad//
      !(pRetData->ulRetCode & ~pMachState->ulIgnore) &&
       (pRetData->ops < (pMachState->maxops - 1l))   &&
	   ((timeout == 0) || (timeout>time(NULL)-starttime)));

   /* Set this variable. This variable is verified at the beginning of
    * CodeRunner so that <pfnBreakConditionCheck> and Trace INT won't get
    * executed before instruction, when CR execution resumed after TRAP
    * or other condition.
    */
   pMachState->ExecutionBroken = TRUE;

   /* Checked whether execution is terminated due to the <MAXOPS_REACHED>
    * condition.
    */
   if (
      !(pRetData->ulRetCode & ~pMachState->ulIgnore) &&
      (pRetData->ops >= (pMachState->maxops - 1l))
      )
      {
      pRetData->ops++;  /* Number of successfully executed instructions.   */
      pRetData->ulRetCode |= MAXOPS_REACHED;
      }

   return;
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns TRUE if its Ok to continue LOOP.
 */

static int  OkToContinueLoop (
               MACHINE_STATE  *pMachState,         /* <-> Input/Output.          */
               RETURN_DATA    *pRetData            /* <-  Output.                */
               )
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

   /* Do while ulRetCode is zero, or bits are set, which could be ignored
    * and number of instructions already executed are less than
    * <maxops-1>.
    */
   if (
      !(pRetData->ulRetCode & ~pMachState->ulIgnore) &&
       (pRetData->ops < (pMachState->maxops - 1l))
      )
      /* Ok to continue LOOP.
       */
      return TRUE;

   /* Time to exit LOOP otherwise!.
    */
   return FALSE;
}
/*=========================================================================*/
