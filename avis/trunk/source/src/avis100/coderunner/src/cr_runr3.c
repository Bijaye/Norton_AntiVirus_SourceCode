/***************************************************************************
 *                                                                         *
 *                              (C) IBM Corp.                              *
 *                                                                         *
 * File:          CR_RUNR3.C                                               *
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


#ifdef CR_DEBUG
extern FILE *cr_dbgf;
extern int cr_hd;      /* Set to 1 to turn on coderunner output to file. */
extern int cr_itrace;  /* Set to 1 to turn on coderunner instruction trace. */
#endif

#define MEMRECDBG 1
/*=========================================================================*/

int         do_MUL (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               int            word,          /* 0-byte, 1-word instruction.*/
               VOID           *pMult,        /* Pointer to destination data*/
               VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries.*/
               )
{
/*-------------------------------------------------------------------------*/
   WORD           multiplier;
   ULONG          ulResult;
/*-------------------------------------------------------------------------*/

   if (!word)
      multiplier = ReadByteFromPTR (pMult);
   else
      multiplier = ReadWordFromPTR (pMult, pMult2);

   if (word)
      {
      ulResult = (ULONG)rAX * multiplier;
      rDX = (WORD)(ulResult >> 16);
      rAX = (WORD)(ulResult);
      }
   else
      rAX = (WORD)(rAL * multiplier);

   if (word?rDX:rAH)
      { setCF; setOF; }
   else
      { clrCF; clrOF; }

   /* PF */
   if (ParityFlag(rAL))
      setPF;
   else
      clrPF;

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

int         do_IMUL (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               int            word,          /* 0-byte, 1-word instruction.*/
               VOID           *pMult,        /* Pointer to destination data*/
               VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries. */
               )
{
/*-------------------------------------------------------------------------*/
   WORD           multiplier;
   LONG           lResult, temp;
/*-------------------------------------------------------------------------*/

   if (!word)
      multiplier = (WORD)((char)(ReadByteFromPTR(pMult)));
   else
      multiplier = (WORD)((int)(ReadWordFromPTR (pMult, pMult2)));

   if (word)
      {
      lResult = (LONG)((int)rAX) * (int)multiplier;
      rDX = (WORD)(lResult >> 16);
      rAX = (WORD)(lResult);
      }
   else
      {
      lResult = (int)((char)rAL) * (int)multiplier;
      rAX = (WORD)lResult;
      }

   /* Conditions of clearing OF and CF. (As I understood stupid manual).
    * Module of result (|result|) must fit into 8-1/16-1 bit =>
    * must not use sign bit or 8/16 bit.
    */
   temp = lResult>0?lResult:-lResult;
   if (word)
      if ((temp&0x7FFF) == temp)
         { clrCF; clrOF; }
      else
         { setCF; setOF; }
   else
      if ((temp&0x7F) == temp)
         { clrCF; clrOF; }
      else
         { setCF; setOF; }

   /* PF */
   if (ParityFlag(rAL))
      setPF;
   else
      clrPF;

   return (0);
}
/*=========================================================================*/


/*=========================================================================*/

int        do_DIV (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               int            word,          /* 0-byte, 1-word instruction.*/
               VOID           *pMult,        /* Pointer to destination data*/
               VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries.*/
               )
{
/*-------------------------------------------------------------------------*/
   WORD           divisor, newAH, newAL;
   ULONG          DX_AX, newDX, newAX;
/*-------------------------------------------------------------------------*/

   if (!word)
      divisor = ReadByteFromPTR (pMult);
   else
      divisor = ReadWordFromPTR (pMult, pMult2);

   if (!divisor)
      {
      pRetData->SegTrap    = rCS;
      pRetData->OffTrap    = rIP;
      pRetData->ulRetCode |= ZERODIVIDE;
      return (-1);
      }

   DX_AX = ((((ULONG)rDX)<<16) + rAX);

   if (word)
      {
      newDX = DX_AX - (DX_AX / divisor) * divisor;
      newAX = DX_AX / divisor;

      if (newAX > 0xFFFF)
         {
         pRetData->SegTrap    = rCS;
         pRetData->OffTrap    = rIP;
         pRetData->ulRetCode |= DIVIDE_ERROR;
         return (-1);
         }
      rAX = (WORD) newAX;
      rDX = (WORD) newDX;
      }
   else
      {
      newAH = (WORD)(rAX - (rAX / divisor) * divisor);
      newAL = (WORD)(rAX / divisor);

      if (newAL > 0xFF)
         {
         pRetData->SegTrap    = rCS;
         pRetData->OffTrap    = rIP;
         pRetData->ulRetCode |= DIVIDE_ERROR;
         return (-1);
         }
      rAH = (BYTE) newAH;
      rAL = (BYTE) newAL;
      }

   /* PF */
   if (ParityFlag(rAL))
      setPF;
   else
      clrPF;

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

int        do_IDIV (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               int            word,          /* 0-byte, 1-word instruction.*/
               VOID           *pMult,        /* Pointer to destination data*/
               VOID           *pMult2        /* Non-zero if dest - word memory, crossing boundaries.*/
               )
{
/*-------------------------------------------------------------------------*/
   int            divisor, newAH, newAL;
   WORD           abs_divisor;
   LONG           DX_AX, abs_DX_AX, newDX, newAX, absAX;
/*-------------------------------------------------------------------------*/

   if (!word)
      divisor = (int)((char)(ReadByteFromPTR(pMult)));
   else
      divisor = (int)(ReadWordFromPTR (pMult, pMult2));

   if (!divisor)
      {
      pRetData->SegTrap    = rCS;
      pRetData->OffTrap    = rIP;
      pRetData->ulRetCode |= ZERODIVIDE;
      return (-1);
      }

   abs_divisor = (WORD)(divisor>0?divisor:-divisor);

   if (word)
      {
      DX_AX       = (LONG)((((ULONG)rDX)<<16) + rAX);
      abs_DX_AX   = DX_AX>0?DX_AX:-DX_AX;
      newDX       = abs_DX_AX - (abs_DX_AX / abs_divisor) * abs_divisor;

      /* Remainder must have the same sign as dividend.
       */
      newDX = DX_AX>0?newDX:-newDX;
      newAX = DX_AX / divisor;

      if ((ULONG)(newAX>0?newAX:-newAX) > (ULONG)0x7FFF)
         {
         /* Result cannot fit in destination. Sign bit cannot be used for
          * value in case of signed division.
          */
         pRetData->SegTrap    = rCS;
         pRetData->OffTrap    = rIP;
         pRetData->ulRetCode |= DIVIDE_ERROR;
         return (-1);
         }
      rAX = (WORD) newAX;
      rDX = (WORD) newDX;
      }
   else
      {
      absAX = (int)rAX;
      if (absAX < 0) absAX = -absAX;
      newAH = (int)(absAX - (absAX / abs_divisor) * abs_divisor);

      /* Remainder must have the same sign as dividend.
       */
      newAH = ((int)rAX)>0?newAH:-newAH;
      newAL = (int)rAX / divisor;

      if ((WORD)newAL > (WORD)0x7F)
         {
         pRetData->SegTrap    = rCS;
         pRetData->OffTrap    = rIP;
         pRetData->ulRetCode |= DIVIDE_ERROR;
         return (-1);
         }
      rAH = (BYTE) newAH;
      rAL = (BYTE) newAL;
      }

   /* PF */
   if (ParityFlag(rAL))
      setPF;
   else
      clrPF;

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

#define  F_CF     (((*pFlags) & CARRY_FLAG_MASK)?1:0)
#define  SET_CF   ((*pFlags) |= CARRY_FLAG_MASK)
#define  CLR_CF   ((*pFlags) &= (~CARRY_FLAG_MASK))

#define  F_OF     (((*pFlags) & OVERFLOW_FLAG_MASK)?1:0)
#define  SET_OF   ((*pFlags) |= OVERFLOW_FLAG_MASK)
#define  CLR_OF   ((*pFlags) &= (~OVERFLOW_FLAG_MASK))

void         do_ShiftRot (
               int   Operation,            /* 0...7 - instruction type.    */
               VOID  *pDest,               /* Pointer to destination data. */
               VOID  *pDest2,              /* Non-zero if dest - word memory, crossing boundaries.*/
               int   rot_bits,             /* Rotate on number of bits.    */
               int   size,                 /* 0-byte, 1-word instruction.  */
               WORD  *pFlags               /* Get updated. Also source of CF.*/
               )
{
/*-------------------------------------------------------------------------*/
   WORD           int_dest;
   int            i, bit, save_cf, save_of;
/*-------------------------------------------------------------------------*/

   rot_bits = rot_bits&0x1F;
   if (!size)
      {
      /* Byte operands.
       */
      int_dest = (WORD)(ReadByteFromPTR(pDest));

      /* Do actual operation.
       */
      switch (Operation)
         {
         case  0: /* ROL      */
            for (i=0; i<rot_bits; i++)
               {
               int_dest = (WORD)((int_dest<<1) | ((int_dest&0x80)?1:0));
               if (int_dest & 1) SET_CF; else CLR_CF;
               }
            if (rot_bits==1)
               {
               if (F_CF ^ (int_dest>>7))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  1: /* ROR      */
            for (i=0; i<rot_bits; i++)
               {
               int_dest = (WORD)((int_dest>>1) | ((int_dest&1)?0x80:0));
               if (int_dest & 0x80) SET_CF; else CLR_CF;
               }
            if (rot_bits==1)
               {
               if (((int_dest>>6)&1) ^ (int_dest>>7))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  2: /* RCL      */
            for (i=0; i<rot_bits; i++)
               {
               bit = F_CF;
               if (int_dest&0x80) SET_CF; else CLR_CF;
               int_dest = (WORD)((int_dest<<1) | bit);
               }
            if (rot_bits==1)
               {
               if (F_CF ^ (int_dest>>7))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  3: /* RCR      */
            for (i=0; i<rot_bits; i++)
               {
               bit = F_CF;
               if (int_dest&1) SET_CF; else CLR_CF;
               int_dest = (WORD)((int_dest>>1) | (bit?0x80:0));
               }
            if (rot_bits==1)
               {
               if (((int_dest>>6)&1) ^ (int_dest>>7))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  4: /* SHL      */
         case  6: /* SHL-undocumented, but executed by 486 CPU in this way */
            for (i=0; i<rot_bits; i++)
               {
               if (int_dest&0x80) SET_CF; else CLR_CF;
               int_dest = (WORD)(int_dest<<1);
               }
            if (rot_bits==1)
               {
               if (F_CF ^ (int_dest>>7))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  5: /* SHR      */
            for (i=0; i<rot_bits; i++)
               {
               if (int_dest&1) SET_CF; else CLR_CF;
               int_dest = (WORD)(int_dest>>1);
               }
            /* For SHR - OF == high order bit of original oerand.
             */
            if (rot_bits==1)
               {
               if (int_dest&0x40)
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         /* Following case is undocumented but being executed by 486 CPU
            in the same way as case 4.
         case  6:
            pRetData->SegTrap    = rCS;
            pRetData->OffTrap    = rIP;
            pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
            return;
          */

         case  7: /* SAR      */
            for (i=0; i<rot_bits; i++)
               {
               if (int_dest&1) SET_CF; else CLR_CF;
               int_dest = (WORD)((char)((BYTE)int_dest)/2);
               }
            if (rot_bits==1)
               CLR_OF;
            break;
         }

      *(BYTE *)(pDest) = (BYTE)int_dest;
      }
   else
      {
      /* Word operands in effect.
       */
      int_dest = ReadWordFromPTR (pDest, pDest2);

      /* Do actual operation.
       */
      switch (Operation)
         {
         case  0: /* ROL      */
            for (i=0; i<rot_bits; i++)
               {
               int_dest = (WORD)((int_dest<<1) | ((int_dest&0x8000)?1:0));
               if (int_dest & 1) SET_CF; else CLR_CF;
               }
            if (rot_bits==1)
               {
               if (F_CF ^ (int_dest>>15))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  1: /* ROR      */
            for (i=0; i<rot_bits; i++)
               {
               int_dest = (WORD)((int_dest>>1) | ((int_dest&1)?0x8000:0));
               if (int_dest & 0x8000) SET_CF; else CLR_CF;
               }
            if (rot_bits==1)
               {
               if (((int_dest>>14)&1) ^ (int_dest>>15))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  2: /* RCL      */
            for (i=0; i<rot_bits; i++)
               {
               bit = F_CF;
               if (int_dest&0x8000) SET_CF; else CLR_CF;
               int_dest = (WORD)((int_dest<<1) | bit);
               }
            if (rot_bits==1)
               {
               if (F_CF ^ (int_dest>>15))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  3: /* RCR      */
            for (i=0; i<rot_bits; i++)
               {
               bit = F_CF;
               if (int_dest&1) SET_CF; else CLR_CF;
               int_dest = (WORD)((int_dest>>1) | (bit?0x8000:0));
               }
            if (rot_bits==1)
               {
               if (((int_dest>>14)&1) ^ (int_dest>>15))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  4: /* SHL      */
         case  6: /* SHL-undocumented, but executed by 486 CPU in this way */
            for (i=0; i<rot_bits; i++)
               {
               if (int_dest&0x8000) SET_CF; else CLR_CF;
               int_dest = (WORD)(int_dest<<1);
               }
            if (rot_bits==1)
               {
               if (F_CF ^ (int_dest>>15))
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         case  5: /* SHR      */
            for (i=0; i<rot_bits; i++)
               {
               if (int_dest&1) SET_CF; else CLR_CF;
               int_dest = (WORD)(int_dest>>1);
               }
            /* For SHR - OF == high order bit of original oerand.
             */
            if (rot_bits==1)
               {
               if (int_dest&0x4000)
                  SET_OF;
               else
                  CLR_OF;
               }
            break;

         /* Following case is undocumented but being executed by 486 CPU
            in the same way as case 4.
         case  6:
            pRetData->SegTrap    = rCS;
            pRetData->OffTrap    = rIP;
            pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
            return;
          */

         case  7: /* SAR      */
            for (i=0; i<rot_bits; i++)
               {
               if (int_dest&1) SET_CF; else CLR_CF;
               int_dest = (WORD)(((int)int_dest)/2);
               }
            if (rot_bits==1)
               CLR_OF;
            break;
         }

      WriteWordToPTR (int_dest, pDest, pDest2);
      }


   switch (Operation)
      {
      case  0: /* ROL      */
      case  1: /* ROR      */
      case  2: /* RCL      */
      case  3: /* RCR      */
         break;

      case  4: /* SHL      */
      case  5: /* SHR      */
      case  6: /* ???      */
      case  7: /* SAR      */
         save_cf = F_CF;
         save_of = F_OF;
         SetArithmFlags (pFlags, size+1, int_dest, 0);
         if (save_cf) SET_CF; else CLR_CF;
         if (save_of) SET_OF; else CLR_OF;
         break;
      }


   return;
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_OR (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


   return   (do_AND_OR_XOR_TEST
               (
               1,       /* 0-AND, 1-OR, 2-XOR.                             */
               pDest,
               pDest2,  /* non-zero if dest - word memory, crossing boundaries.*/
               pSrc,
               pSrc2,   /* non-zero if src  - word memory, crossing boundaries.*/
               size,    /* 1 or 2 bytes instruction.                           */
               pFlags   /* Get updated. Also source of CF for ADC..., SBB... . */
               )
            );
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_AND (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


   return   (do_AND_OR_XOR_TEST
               (
               0,       /* 0-AND, 1-OR, 2-XOR.                             */
               pDest,
               pDest2,  /* non-zero if dest - word memory, crossing boundaries.*/
               pSrc,
               pSrc2,   /* non-zero if src  - word memory, crossing boundaries.*/
               size,    /* 1 or 2 bytes instruction.                           */
               pFlags   /* Get updated. Also source of CF for ADC..., SBB... . */
               )
            );
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_XOR (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


   return   (do_AND_OR_XOR_TEST
               (
               2,       /* 0-AND, 1-OR, 2-XOR.                             */
               pDest,
               pDest2,  /* non-zero if dest - word memory, crossing boundaries.*/
               pSrc,
               pSrc2,   /* non-zero if src  - word memory, crossing boundaries.*/
               size,    /* 1 or 2 bytes instruction.                           */
               pFlags   /* Get updated. Also source of CF for ADC..., SBB... . */
               )
            );
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_TEST (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


   return   (do_AND_OR_XOR_TEST
               (
               3,       /* 0-AND, 1-OR, 2-XOR, 3-TEST                      */
               pDest,
               pDest2,  /* non-zero if dest - word memory, crossing boundaries.*/
               pSrc,
               pSrc2,   /* non-zero if src  - word memory, crossing boundaries.*/
               size,    /* 1 or 2 bytes instruction.                           */
               pFlags   /* Get updated. Also source of CF for ADC..., SBB... . */
               )
            );
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_AND_OR_XOR_TEST (
      int   instruction,   /* 0-AND, 1-OR, 2-XOR, 3-TEST.                  */
      VOID  *pDest,
      VOID  *pDest2,       /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,        /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,          /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags        /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   int            int_dest, int_src;
/*-------------------------------------------------------------------------*/

   if (size==1)
      {
      BYTE byte_dest;
      /* With byte operands everything is easy.
       * Do actual operation.
       */
      byte_dest = (char)ReadByteFromPTR (pDest);
      switch (instruction)
         {
         case  0: /* AND ...  */
            byte_dest &= ReadByteFromPTR (pSrc);   break;

         case  1: /* OR  ...  */
            byte_dest |= ReadByteFromPTR (pSrc);   break;

         case  2: /* XOR ...  */
            byte_dest ^= ReadByteFromPTR (pSrc);   break;

         case  3: /* TEST ... */
            byte_dest  = ReadByteFromPTR (pDest);
            byte_dest &= ReadByteFromPTR (pSrc);           break;
         }

      int_dest = (int)byte_dest;
      if (instruction != 3)
         *((BYTE *)pDest) = byte_dest;
      }
   else
      {
      /* Word operands in effect.
       */
      int_dest = ReadWordFromPTR (pDest, pDest2);
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);

      /* Do actual operation.
       */
      switch (instruction)
         {
         case  0: /* AND ...  */
         case  3: /* TEST ... */
            int_dest    &= int_src;    break;

         case  1: /* OR  ...  */
            int_dest    |= int_src;    break;

         case  2: /* XOR ...  */
            int_dest    ^= int_src;    break;
         }

      if (instruction != 3)
         WriteWordToPTR ((WORD)int_dest, pDest, pDest2);
      }


   /* Set flags.
    */
   SetAndOrXorFlags (pFlags, int_dest);

//riad//
   /* Oups, there is a problem here, if it's TEST, return 0 */
   if(instruction == 3) return 0;
   return (1);
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_ADD (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   ULONG          int_dest, int_src;
/*-------------------------------------------------------------------------*/

   if (size==1)
      {
      /* With byte operands everything is easy.
       * Set flags.
       */
      SetArithmFlags (pFlags, 1, *(BYTE *)(pDest), *(BYTE *)(pSrc));

      /* Do actual operation.
       */
      *(BYTE *)(pDest) += ReadByteFromPTR (pSrc);
      }
   else
      {
      /* Word operands in effect.
       */
      int_dest = ReadWordFromPTR (pDest, pDest2);
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);

      /* Set flags.
       */
      SetArithmFlags (pFlags, 2, int_dest, int_src);

      /* Do actual operation.
       */
      int_dest    += int_src;

      WriteWordToPTR ((WORD)int_dest, pDest, pDest2);
      }


   return (1);
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_ADC (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   ULONG          int_dest, int_src, cf;
/*-------------------------------------------------------------------------*/

   /* Extract CF.
    */
   cf = ((*pFlags) & CARRY_FLAG_MASK)?1:0;

   if (size==1)
      {
      /* With byte operands everything is easy.
       * Set flags.
       */
      SetArithmFlags (pFlags, 1, *(BYTE *)(pDest), *(BYTE *)(pSrc) + cf);

      /* Do actual operation.
       */
      *(BYTE *)(pDest) += ((BYTE)(ReadByteFromPTR(pSrc) + cf));
      }
   else
      {
      /* Word operands in effect.
       */
      int_dest = ReadWordFromPTR (pDest, pDest2);
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);

      /* Set flags.
       */
      SetArithmFlags (pFlags, 2, int_dest, int_src + cf);
      int_dest    += int_src + cf;

      /* Do actual operation.
       */
      WriteWordToPTR ((WORD)int_dest, pDest, pDest2);
      }


   return (1);
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_SUB (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   ULONG          int_dest, int_src;
/*-------------------------------------------------------------------------*/

   if (size==1)
      {
      /* With byte operands everything is easy.
       * Set flags.
       */
      SetArithmFlags (pFlags, 1, *(BYTE *)(pDest), -(long)(*(BYTE *)(pSrc)));

      /* Do actual operation.
       */
      *(BYTE *)(pDest) -= ReadByteFromPTR (pSrc);
      }
   else
      {
      /* Word operands in effect.
       */
      int_dest = ReadWordFromPTR (pDest, pDest2);
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);

      /* Set flags.
       */
      SetArithmFlags (pFlags, 2, int_dest, -(long)int_src);
      int_dest    -= int_src;

      /* Do actual operation.
       */
      WriteWordToPTR ((WORD)int_dest, pDest, pDest2);
      }


   return (1);
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_SBB (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   ULONG   int_dest, int_src, cf;
/*-------------------------------------------------------------------------*/

   /* Extract CF.
    */
   cf = ((*pFlags) & CARRY_FLAG_MASK)?1:0;

   if (size==1)
      {
      /* With byte operands everything is easy.
       * Set flags.
       */
      SetArithmFlags (pFlags, 1, *(BYTE *)(pDest), -(long)(*(BYTE *)(pSrc) + cf));

      /* Do actual operation.
       */
      *(BYTE *)(pDest) -= ((BYTE)(ReadByteFromPTR(pSrc) + cf));
      }
   else
      {
      /* Word operands in effect.
       */
      int_dest = ReadWordFromPTR (pDest, pDest2);
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);

      /* Set flags.
       */
      SetArithmFlags (pFlags, 2, int_dest, -(long)(int_src + cf));
      int_dest    -= int_src + cf;

      /* Do actual operation.
       */
      WriteWordToPTR ((WORD)int_dest, pDest, pDest2);
      }


   return (1);
}
/*=========================================================================*/


/*=========================================================================*/

/* MOV AL/AX, imm (opcode 0xA0, 0xA1, 0xA2, 0xA3).
 */

int         do_MOV_Ax_imm (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               int            op1,           /* First byte of instruction. */
               BYTE           rel_seg        /* Relative to segment.       */
               )
{
/*-------------------------------------------------------------------------*/
   int            op2, op3;
   WORD           Seg;
   ULONG          ulMemoFlags;
   MEMORY_AREA    *pMA, *pMA2;
   VOID           *pDest, *pDest2, *pSrc, *pSrc2;
/*-------------------------------------------------------------------------*/

   op2 = QueryNextOpcode (pMachState, pRetData, pTempIP);
   //riad//
   *pMachState->pOp2 = op2;
   if (op2 < 0)

      /* Code buffer not found! Nothing to do. Return...
       * <pRetData> already initialized.
       * We may not worry about adjusting TempIP to the next instruction
       * because code query trap is not recoverable.
       */
      return (-1);

   op3 = QueryNextOpcode (pMachState, pRetData, pTempIP);
   if (op3 < 0)
      return (-1);

   /* Init <Seg>.
    */
   switch (rel_seg)
      {
      case  0x26: Seg = rES;  break;
      case  0x2E: Seg = rCS;  break;
      case  0x36: Seg = rSS;  break;

      case  0x00:
      case  0x3E:
      default:    Seg = rDS;  break;
      }

   /* Init <ulMemoFlags>.
    */
   if (op1==0xA0 || op1==0xA1)
      ulMemoFlags = READABLE;
   else
      ulMemoFlags = WRITEABLE;

   if (MapAddressToMemoryArea (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         Seg,                                /* Segment to map.            */
         MK_WORD (op3, op2),                 /* Offset to map.             */
         ulMemoFlags,                        /* Desired memory attributes. */
         op1&1,                              /* 1-word, 0-byte.            */
         &pMA,                               /* Memory area for memo instr.*/
         &pMA2) == -1)                       /* M.A. if word crossing border*/
      {
      /* READ/WRITE trap occurs. <pRetData> already initialized.
       * Exiting...
       */
      return (-1);
      }

   switch (op1)
      {
      case  0xA0:
      case  0xA1: pDest = (op1==0xA0||op1==0xA2)?(VOID *)(&rAL):(VOID *)(&rAX);
                  pDest2= 0;
                  pSrc = pMA->pBuff+(WORD)(LinearAddr(Seg,MK_WORD(op3,op2))-MALinearAddr(pMA));
                  if (pMA2)
                     pSrc2 = pMA2->pBuff+(WORD)(LinearAddr(Seg,MK_WORD(op3,op2)+1)-MALinearAddr(pMA2));
                  else
                     pSrc2 = 0;
                  break;

      case  0xA2:
      case  0xA3: pSrc  = (op1==0xA0||op1==0xA2)?(VOID *)(&rAL):(VOID *)(&rAX);
                  pSrc2 = 0;
                  pDest = pMA->pBuff+(WORD)(LinearAddr(Seg,MK_WORD(op3,op2))-MALinearAddr(pMA));
                  if (pMA2)
                     pDest2= pMA2->pBuff+(WORD)(LinearAddr(Seg,MK_WORD(op3,op2)+1)-MALinearAddr(pMA2));
                  else
                     pDest2= 0;
                  break;

      }


   /* Do actual MOV
    */
   do_MOV (pDest, pDest2, pSrc, pSrc2, op1&1?2:1, &rFlags);

   if (op1==0xA2 || op1==0xA3)
      {
      /* Update address of 'modified' byte(s) in touched memory area.
       */
      UpdateModifiedMA (
         pMachState,
         pRetData,                     /* Upd. flag for EXECUTABLE MA.     */
         pMA,                          /* Memory area for memo instr.      */
         pMA2,                         /* M.A. if word crossing border.    */
         op1&1?2:1,                    /* Bytes was written: 1 or 2.       */
         Seg,                          /* Real segment written to.         */
         MK_WORD(op3,op2));            /* Real offset written to.          */
      }

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_MOV (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   ULONG          int_src;
/*-------------------------------------------------------------------------*/

   if (size==1)
      {
      /* With byte operands everything is easy.
       * Do actual operation.
       */
      *(BYTE *)(pDest) = ReadByteFromPTR (pSrc);
      }
   else
      {
      /* Word operands in effect.
       */
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);

      /* Do actual operation.
       */
      WriteWordToPTR ((WORD)int_src, pDest, pDest2);
      }

   return (1);
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_XCHG (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   WORD           int_src;
   WORD           int_dest;
/*-------------------------------------------------------------------------*/

   if (size==1)
      {
      /* With byte operands everything is easy.
       * Do actual operation.
       */
      int_dest = ReadByteFromPTR (pDest);
      *(BYTE *)(pDest) = ReadByteFromPTR (pSrc);
      *(BYTE *)(pSrc) = (BYTE)int_dest;
      }
   else
      {
      /* Word operands in effect.
       */
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);
      int_dest = ReadWordFromPTR (pDest, pDest2);

      /* Do actual operation.
       */
      WriteWordToPTR (int_src, pDest, pDest2);
      WriteWordToPTR (int_dest, pSrc, pSrc2);
      }

   return (1);
}
/*=========================================================================*/


/*=========================================================================*/

/* Returns 1-destination operand updated, 0-not.
 */

int         do_CMP (
      VOID  *pDest,
      VOID  *pDest2, /* non-zero if dest - word memory, crossing boundaries.*/
      VOID  *pSrc,
      VOID  *pSrc2,  /* non-zero if src  - word memory, crossing boundaries.*/
      int   size,    /* 1 or 2 bytes instruction.                           */
      WORD  *pFlags  /* Get updated. Also source of CF for ADC..., SBB... . */
      )
{
/*-------------------------------------------------------------------------*/
   ULONG          int_dest, int_src;
/*-------------------------------------------------------------------------*/

   if (size==1)
      {
      /* With byte operands everything is easy.
       * Set flags.
       */
      SetArithmFlags (pFlags, 1, *(BYTE *)(pDest), -(long)(*(BYTE *)(pSrc)));
      }
   else
      {
      /* Word operands in effect.
       */
      int_dest = ReadWordFromPTR (pDest, pDest2);
      int_src  = ReadWordFromPTR (pSrc,  pSrc2);

      /* Set flags.
       */
      SetArithmFlags (pFlags, 2, int_dest, -(long)int_src);
      }


   return (0);
}
/*=========================================================================*/


/*=========================================================================*/


int                do_MOVS (
                        MACHINE_STATE  *pMachState,   /* Machine state.    */
                        RETURN_DATA    *pRetData,     /* Return data.      */
                        int            word,          /* 1-WORD, 0-BYTE instruction. */
                        BYTE           rel_seg,       /* Relative to segment.        */
                        BYTE           rep_prefix     /* Curr value of <rep_prefix>. */
                        )
{
/*-------------------------------------------------------------------------*/
   WORD           SrcSeg;
   MEMORY_AREA    *pSrcMA, *pDestMA, *unused;
   int            i, needed, increment;
   BYTE           ByteToMove;
/*-------------------------------------------------------------------------*/

   _InREP_MOVS = 1;
   /* Init <SrcSeg>.
    */
   switch (rel_seg)
      {
      case  0x26: SrcSeg = rES;  break;
      case  0x2E: SrcSeg = rCS;  break;
      case  0x36: SrcSeg = rSS;  break;

      case  0x00:
      case  0x3E:
      default:    SrcSeg = rDS;  break;
      }

   increment = (fDF)?-1:1;

   for (needed=rep_prefix?rCX:1; needed; needed=rep_prefix?(--rCX):0)
      {
      /* Trying to find source memory areas.
       */
      for (i=0; i<=word; i++)
         {
         if (MapAddressToMemoryArea (
               pMachState,                   /* Machine state.             */
               pRetData,                     /* Return data.               */
               SrcSeg,                       /* Segment to map.            */
               rSI,                          /* Offset to map.             */
               READABLE,                     /* Desired memory attributes. */
               0,                            /* 1-word, 0-byte.            */
               &pSrcMA,                      /* Memory area for memo instr.*/
               &unused) == -1)               /* M.A. if word crossing border.*/
            {
            /* READ/WRITE trap occurs. <pRetData> already initialized.
             * Exiting... But increment DI and SI in case somebody is
             * using this instruction only for the side effects.
             */
            if (pMachState->ulIgnoreNextOpcode)
               { rDI+=((short)increment); rSI+=((short)increment); }
            return (-1);
            }


         /* Trying to find destination memory areas.
          */
         if (MapAddressToMemoryArea (
               pMachState,                   /* Machine state.             */
               pRetData,                     /* Return data.               */
               rES,                          /* Segment to map.            */
               rDI,                          /* Offset to map.             */
               WRITEABLE,                    /* Desired memory attributes. */
               0,                            /* 1-word, 0-byte.            */
               &pDestMA,                     /* Memory area for memo instr.*/
               &unused) == -1)               /* M.A. if word crossing border.*/
            {
            /* READ/WRITE trap occurs. <pRetData> already initialized.
             * Exiting... But increment DI and SI in case somebody is
             * using this instruction only for the side effects.
             */
            if (pMachState->ulIgnoreNextOpcode)
               { rDI+=((short)increment); rSI+=((short)increment); }
            return (-1);
            }

         /* Move byte from source to destination.
          */
         ByteToMove = ReadByteFromMA (pSrcMA, SrcSeg, rSI);
         WriteByteToMA (ByteToMove, pDestMA, rES, rDI);


         UpdateModifiedMA (
            pMachState,
            pRetData,                        /* Upd. flag for EXECUTABLE MA.*/
            pDestMA,                         /* Memory area for memo instr. */
            0,                               /* M.A. if word crossing border*/
            1,                               /* Bytes was written: 1 or 2.  */
            rES,                             /* Real segment written to.    */
            rDI                              /* Real offset written to.     */
            );

         rDI+=((short)increment); rSI+=((short)increment);
         }
      }

   return (0);
}
/*=========================================================================*/


/*=========================================================================*/


int                do_CMPS (
                        MACHINE_STATE  *pMachState,   /* Machine state.    */
                        RETURN_DATA    *pRetData,     /* Return data.      */
                        int            word,          /* 1-WORD, 0-BYTE instruction.*/
                        BYTE           rel_seg,       /* Relative to segment.*/
                        BYTE           rep_prefix     /* Curr value of <rep_prefix>.*/
                        )
{
/*-------------------------------------------------------------------------*/
   WORD           int_src, int_dest;
   WORD           SrcSeg;
   MEMORY_AREA    *pSrcMA, *pSrcMA2, *pDestMA, *pDestMA2;
   int            needed, increment;
/*-------------------------------------------------------------------------*/

   /* Init <SrcSeg>.
    */
   switch (rel_seg)
      {
      case  0x26: SrcSeg = rES;  break;
      case  0x2E: SrcSeg = rCS;  break;
      case  0x36: SrcSeg = rSS;  break;

      case  0x00:
      case  0x3E:
      default:    SrcSeg = rDS;  break;
      }

   increment = (fDF)?-(word+1):(word+1);

   for (needed=rep_prefix?rCX:1; needed;)
      {
      /* Trying to find source memory areas.
       */
      if (MapAddressToMemoryArea (
            pMachState,                      /* Machine state.             */
            pRetData,                        /* Return data.               */
            SrcSeg,                          /* Segment to map.            */
            rSI,                             /* Offset to map.             */
            READABLE,                        /* Desired memory attributes. */
            word,                            /* 1-word, 0-byte.            */
            &pSrcMA,                         /* Memory area for memo instr.*/
            &pSrcMA2) == -1)                 /* M.A. if word crossing border*/
         {
         /* READ/WRITE trap occurs. <pRetData> already initialized.
          * Exiting... But increment SI and DI in case somebody is
          * using this instruction for side effects...
          */
         if (pMachState->ulIgnoreNextOpcode)
            { rDI+=((short)increment); rSI+=((short)increment); }
         return (-1);
         }

      /* Trying to find destination memory areas.
       */
      if (MapAddressToMemoryArea (
            pMachState,                      /* Machine state.             */
            pRetData,                        /* Return data.               */
            rES,                             /* Segment to map.            */
            rDI,                             /* Offset to map.             */
            READABLE,                        /* Desired memory attributes. */
            word,                            /* 1-word, 0-byte.            */
            &pDestMA,                        /* Memory area for memo instr.*/
            &pDestMA2) == -1)                /* M.A. if word crossing border.*/
         {
         /* READ/WRITE trap occurs. <pRetData> already initialized.
          * Exiting... But increment SI and DI in case somebody is
          * using this instruction for side effects...
          */
         if (pMachState->ulIgnoreNextOpcode)
            { rDI+=((short)increment); rSI+=((short)increment); }
         return (-1);
         }


      /* Initialize <int_src> and <int_dest>.
       */
      if (!word)
         {
         /* Byte instruction.
          */
         int_src   = ReadByteFromMA (pSrcMA,  SrcSeg, rSI);
         int_dest  = ReadByteFromMA (pDestMA, rES,    rDI);
         }
      else
         {
         /* Word instruction.
          */
         int_src  = ReadWordFromMA (pSrcMA, pSrcMA2, SrcSeg, rSI);
         int_dest = ReadWordFromMA (pDestMA, pDestMA2, rES,  rDI);
         }

      /* Do actual comparison.
       * CMPS set flags according to: xx:[SI] - ES:[DI].
       */
      SetArithmFlags (&rFlags, word+1, int_src, -(long)int_dest);

      rDI+=((short)increment); rSI+=((short)increment);


      /* Check whether instruction still need to be repeated.
       */
      needed = rep_prefix?--rCX:0;
      if (needed)
         if ((rep_prefix==0xF2 && fZF) || (rep_prefix==0xF3 && !fZF))
            needed = 0;
      }

   return (0);
}
/*=========================================================================*/


/*=========================================================================*/


int                do_STOS (
                        MACHINE_STATE  *pMachState,   /* Machine state.    */
                        RETURN_DATA    *pRetData,     /* Return data.      */
                        int            word,          /* 1-WORD, 0-BYTE instruction.*/
                        BYTE           rep_prefix     /* Curr value of <rep_prefix>.*/
                        )
{
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pDestMA, *pDestMA2;
   int            needed, increment;
/*-------------------------------------------------------------------------*/

   increment = (fDF)?-(word+1):(word+1);

   for (needed=rep_prefix?rCX:1; needed; needed=rep_prefix?(--rCX):0)
      {
      /* Trying to find source memory areas.
       * Trying to find destination memory areas.
       */
      if (MapAddressToMemoryArea (
            pMachState,                      /* Machine state.             */
            pRetData,                        /* Return data.               */
            rES,                             /* Segment to map.            */
            rDI,                             /* Offset to map.             */
            WRITEABLE,                       /* Desired memory attributes. */
            word,                            /* 1-word, 0-byte.            */
            &pDestMA,                        /* Memory area for memo instr.*/
            &pDestMA2) == -1)                /* M.A. if word crossing border.*/
         {
         /* READ/WRITE trap occurs. <pRetData> already initialized.
          * Exiting...
          */
         return (-1);
         }


      /* Store byte/word from AX/AL to destination.
       */
      if (!word)
         WriteByteToMA (rAL, pDestMA, rES, rDI);
      else
         WriteWordToMA (rAX, pDestMA, pDestMA2, rES, rDI);

      UpdateModifiedMA (
         pMachState,
         pRetData,                        /* Upd. flag for EXECUTABLE MA.  */
         pDestMA,                         /* Memory area for memo instr.   */
         pDestMA2,                        /* M.A. if word crossing border. */
         word,                            /* Bytes was written: 1 or 2.    */
         rES,                             /* Real segment written to.    */
         rDI                              /* Real offset written to.       */
         );

      rDI+=((short)increment);

      }

   return (0);
}
/*=========================================================================*/


/*=========================================================================*/


int                do_LODS (
                        MACHINE_STATE  *pMachState,   /* Machine state.    */
                        RETURN_DATA    *pRetData,     /* Return data.      */
                        int            word,          /* 1-WORD, 0-BYTE instruction.*/
                        BYTE           rel_seg,       /* Relative to segment.       */
                        BYTE           rep_prefix     /* Curr value of <rep_prefix>.*/
                        )
{
/*-------------------------------------------------------------------------*/
   WORD           SrcSeg;
   MEMORY_AREA    *pSrcMA, *pSrcMA2;
   int            needed, increment;
/*-------------------------------------------------------------------------*/

   /* Init <SrcSeg>.
    */
   switch (rel_seg)
      {
      case  0x26: SrcSeg = rES;  break;
      case  0x2E: SrcSeg = rCS;  break;
      case  0x36: SrcSeg = rSS;  break;

      case  0x00:
      case  0x3E:
      default:    SrcSeg = rDS;  break;
      }

   increment = (fDF)?-(word+1):(word+1);

   for (needed=rep_prefix?rCX:1; needed; needed=rep_prefix?(--rCX):0)
      {
      /* Trying to find source memory areas.
       * Trying to find destination memory areas.
       */
      if (MapAddressToMemoryArea (
            pMachState,                      /* Machine state.             */
            pRetData,                        /* Return data.               */
            SrcSeg,                          /* Segment to map.            */
            rSI,                             /* Offset to map.             */
            READABLE,                        /* Desired memory attributes. */
            word,                            /* 1-word, 0-byte.            */
            &pSrcMA,                         /* Memory area for memo instr.*/
            &pSrcMA2) == -1)                 /* M.A. if word crossing border.*/
         {
         /* READ/WRITE trap occurs. <pRetData> already initialized.
          * Exiting... But update SI in case somebody is using
          * only the side effect of this instruction.
          */
         if (pMachState->ulIgnoreNextOpcode)
            rSI+=((short)increment);
         return (-1);
         }


      /* Read byte/word from AX/AL from source.
       */
      if (!word)
         rAL = ReadByteFromMA (pSrcMA, SrcSeg, rSI);
      else
         rAX = ReadWordFromMA (pSrcMA, pSrcMA2, SrcSeg, rSI);

      rSI+=((short)increment);
      }

   return (0);
}
/*=========================================================================*/


/*=========================================================================*/


int                do_SCAS (
                        MACHINE_STATE  *pMachState,   /* Machine state.    */
                        RETURN_DATA    *pRetData,     /* Return data.      */
                        int            word,          /* 1-WORD, 0-BYTE instruction.*/
                        BYTE           rep_prefix     /* Curr value of <rep_prefix>.*/
                        )
{
/*-------------------------------------------------------------------------*/
   WORD           int_dest;
   MEMORY_AREA    *pDestMA, *pDestMA2;
   int            needed, increment;
/*-------------------------------------------------------------------------*/


   increment = (fDF)?-(word+1):(word+1);

   for (needed=rep_prefix?rCX:1; needed;)
      {
      /* Trying to find destination memory areas.
       */
      if (MapAddressToMemoryArea (
            pMachState,                   /* Machine state.                */
            pRetData,                     /* Return data.                  */
            rES,                          /* Segment to map.               */
            rDI,                          /* Offset to map.                */
            READABLE,                     /* Desired memory attributes.    */
            word,                         /* 1-word, 0-byte.               */
            &pDestMA,                     /* Memory area for memo instr.   */
            &pDestMA2) == -1)             /* M.A. if word crossing border. */
         {
         /* READ/WRITE trap occurs. <pRetData> already initialized.
          * Exiting... But increment DI in case something is using
          * only the side effect of this instruction...
          */
         if (pMachState->ulIgnoreNextOpcode)
            rDI+=((short)increment);
         return (-1);
         }


      /* Initialize <int_dest>.
       */
      if (!word)
         {
         /* Byte instruction.
          */
         int_dest = ReadByteFromMA (pDestMA, rES, rDI);
         }
      else
         /* Word instruction.
          */
         int_dest = ReadWordFromMA (pDestMA, pDestMA2, rES, rDI);


      /* Do actual comparison.
       * SCAS set flags according to: AX|AL - ES:[DI].
       */
      SetArithmFlags (&rFlags, word+1, word?rAX:rAL, -(long)int_dest);

      rDI+=((short)increment);

      /* Check whether instruction still need to be repeated.
       */
      needed = rep_prefix?--rCX:0;
      if (needed)
         if ((rep_prefix==0xF2 && fZF) || (rep_prefix==0xF3 && !fZF))
            needed = 0;
      }

   return (0);
}
/*=========================================================================*/


/*=========================================================================*/

/* Function performs PUSH/POP r/mem instruction. Opcode 0x8F(POP), Grp3(PUSH)
 */

int         PUSH_POP_rm (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               BOOL           push,          /* 1 - PUSH, 0 - POP.         */
               BYTE           rel_seg        /* Relative to segment.       */
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2;
   WORD           Value;
   int            RetVal;
/*-------------------------------------------------------------------------*/


   /* For area, which is suppose to be PUSHed into stack or POPed from.
    */
   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         1,                                  /* Bit0-W(1)/B, bit1-Direction.*/
         rel_seg,                            /* Relative to segment.       */
         push?READABLE:WRITEABLE,            /* Use it if set (CMP, 8x ..).*/
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
       * If we are ignoring a trap, we should still update SP, so
       * update SP if we are ignoring a trap.
       */
      if (pMachState->ulIgnoreNextOpcode & (READ_TRAP | WRITE_TRAP))
         rSP=(WORD)(push?(rSP-2):(rSP+2));
      return (-1);
      }


   /* Ok - here we have <pMA>, <mri> and probably <pMA2> initialized.
    * We now have everything to continue with executing of instruction.
    * Note: we don't need mri.pReg2. There is no second operand for
    * PUSH/POP.
    */
   if (mri.pReg1)
      {
      /* Here we have PUSH/POP r. No memory related instructions.
       * Execute actual instruction.
       */
      return (do_PUSH_POP (
               pMachState,                   /* <-> Input/Output.          */
               pRetData,                     /* <-  Output.                */
               mri.pReg1,                    /* Ptr to value.              */
               push));                       /* 1 - PUSH, 0 - POP          */
      }


   /* Here we have PUSH/POP mem. No register related instructions.
    * WORD operand of PUSH/POP.
    * For PUSH we must read what to PUSH first.
    */
   if (push)
      Value = ReadWordFromMA (pMA, pMA2, mri.Seg1, mri.Off1);

   RetVal = do_PUSH_POP (
               pMachState,                   /* <-> Input/Output.          */
               pRetData,                     /* <-  Output.                */
               &Value,                       /* Ptr to value.              */
               push);                        /* TRUE - PUSH, FALSE - POP   */

   if (RetVal == -1)
      return (RetVal);


   /* For POP we now have to write POP-ed value to memory.
    */
   if (!push)
      {
      WriteWordToMA (Value, pMA, pMA2, mri.Seg1, mri.Off1);

      /* Update address of 'modified' byte(s) in touched memory area.
       */
      UpdateModifiedMA (
         pMachState,
         pRetData,                           /* Upd. flag for EXECUTABLE MA.*/
         pMA,                                /* Memory area for memo instr. */
         pMA2,                               /* M.A. if word crossing border*/
         2,                                  /* Bytes was written: 1 or 2.  */
         mri.Seg1,                           /* Real segment written to.    */
         mri.Off1);                          /* Real offset written to.     */
      }

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/



int         JMP_CALL_rm (
               MACHINE_STATE  *pMachState,   /* Machine state.             */
               RETURN_DATA    *pRetData,     /* Return data.               */
               WORD           *pTempIP,      /* Ptr to temp IP.            */
               BOOL           jmp,           /* 1-JMP, 0-CALL.             */
               BOOL           Near,          /* 1-NEAR, 0-FAR.             */
               BYTE           rel_seg        /* Relative to segment.       */
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    mri;
   MEMORY_AREA    *pMA, *pMA2, *pSegMA, *pSegMA2;
/*-------------------------------------------------------------------------*/


   /* For area, which is suppose to address where to jump to.
    */
   if (GenInit_r_rm (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         pTempIP,                            /* Ptr to temp IP.            */
         1,                                  /* Bit0-W(1)/B, bit1-Direction*/
         rel_seg,                            /* Relative to segment.       */
         READABLE,                           /* Use it if set (CMP, 8x ..).*/
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
    * JMP.
    */
   if (mri.pReg1)
      {
      /* Here we have JMP/CALL reg16.  No memory related instructions.
       * Set TempIP to the new position.
       */
      if (Near)
         {
         if (!jmp)
            {
            /* CALL Near Reg16
             */
            if (do_PUSH_POP (
                  pMachState,                /* <-> Input/Output.          */
                  pRetData,                  /* <-  Output.                */
                  pTempIP,                   /* Ptr to value.              */
                  1) == -1                   /* TRUE - PUSH, FALSE - POP   */
               )
               {
               /* Canot PUSH return IP into stack. Write TRAP? Return...
                */
               return (-1);
               }
            }

         *pTempIP = *(WORD *)(mri.pReg1);
         if (!jmp && (mri.pReg1 == ((PVOID)&rSP)))
            *pTempIP += 2;
         }
      else
         {
         /* Real mode doesn't support JMP/CALL Far BX -type instructions.
          * Note: JMP/CALL Far [BX] is ok.
          */
         pRetData->SegTrap    = rCS;
         pRetData->OffTrap    = rIP;
         pRetData->ulRetCode |= UNSUPPORTED_OPCODE;

         return (-1);
         }
      }
   else
      {
      /* Here we have JMP/CALL Far/Near [r/m]. No register related
       * instructions. Query contents of addressed memory.
       */
      if (!Near)
         {
         /* JMP/CALL Far [r/m].
          * To query 4 bytes we must map second 2-bytes (segment part)
          * to memory area again.
          */
         if (MapAddressToMemoryArea (
               pMachState,                   /* Machine state.             */
               pRetData,                     /* Return data.               */
               mri.Seg1,                     /* Segment to map.            */
               (WORD)(mri.Off1+2),           /* Offset to map.             */
               READABLE,                     /* Desired memory attributes. */
               1,                            /* 1-word, 0-byte.            */
               &pSegMA,                      /* Memory area for memo instr.*/
               &pSegMA2) == -1)              /* M.A. if word crossing border.*/
            {
            /* READ/WRITE trap occurs. <pRetData> already initialized.
             * Exiting...
             */
            return (-1);
            }

         if (!jmp)
            {
            /* CALL Far [r/m]
             * For CALL we must PUSH return address into stack.
             */
            if (do_PUSH_POP (pMachState, pRetData, 
               &rCS, 1) == -1)
               {
               /* Canot PUSH return CS into stack. Write TRAP? Return...
                */
               return (-1);
               }
            }

         /* JMP/CALL Far [r/m].
          * Change CS for JMP/CALL Far.
          */
         rCS = ReadWordFromMA (pSegMA, pSegMA2, mri.Seg1, (WORD)(mri.Off1+2));

         }


      /* JMP/CALL Far/Near
       */
      if (!jmp)
         {
         /* CALL Far/Near. PUSH IP into stack.
          */
         if (do_PUSH_POP (pMachState, pRetData, 
            pTempIP, 1) == -1)
            {
            /* Canot PUSH return IP into stack. Write TRAP? Return...
             */
            return (-1);
            }
         }


      /* JMP/CALL Far/Near
       */
      *pTempIP = ReadWordFromMA (pMA, pMA2, mri.Seg1, mri.Off1);

      }

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/


int         do_PUSH_POP (
               MACHINE_STATE  *pMachState,   /* <-> Input/Output.          */
               RETURN_DATA    *pRetData,     /* <-  Output.                */
               WORD           *pValue,       /* Ptr to value.              */
               BOOL           push           /* TRUE - PUSH, FALSE - POP   */
               )
{
/*-------------------------------------------------------------------------*/
   WORD           StOff;
   MEMORY_AREA    *pStackMA, *pStackMA2;
/*-------------------------------------------------------------------------*/


   StOff = (WORD)(push?(rSP-2):(rSP));


   /* Find stack buffer (memory area).
    */
   if (MapAddressToMemoryArea (
               pMachState,                   /* Machine state.             */
               pRetData,                     /* Return data.               */
               rSS,                          /* Segment to map.            */
               StOff,                        /* Offset to map.             */
               push?WRITEABLE:READABLE,      /* Desired memory attributes. */
               1,                            /* 1-word, 0-byte.            */
               &pStackMA,                    /* Memory area for memo instr.*/
               &pStackMA2) == -1)            /* M.A. if word crossing border.*/
      {
      /* READ/WRITE trap occurs. <pRetData> already initialized. Exiting...
       */
      return (-1);
      }

   /* Ok, here we have pStackMA (and probably pStackMA2) initialized.
    * Word operands in effect.
    * Do actual operation.
    */
   if (push)
      WriteWordToMA (*pValue, pStackMA, pStackMA2, rSS, StOff);
   else
      *pValue = ReadWordFromMA (pStackMA, pStackMA2, rSS, StOff);

   /* Do not update modified memory areas for CALL and INT instructions.
    * Some viruses have many dummy CALLs/INT instructions, which are not
    * modifying any data in virus body. Although at the same time CALL/INT
    * would increment counter of active instructions and codeRunner would
    * be terminated earlier because of that.
    */
   if (push)
      {
      int   op1 = *(pMachState->pOp1);
      int   op2 = *(pMachState->pOp2);
      if (
         (op1 != 0x9A) &&    /* CALL Far instruction?                         */
         (op1 != 0xE8) &&    /* CALL     instruction?                         */
         ((op1 != 0xFF) || (((op2>>4)&3)!=1)) && /* Indirect CALL.            */
         (op1 != 0xCC) &&    /* INT3     instruction?                         */
         (op1 != 0xCD) &&    /* INT      instruction?                         */
         (op1 != 0xCE)       /* INTO     instruction?                         */
         )
         /* Update address of 'modified' byte(s) in stack memory area.
          */
         UpdateModifiedMA (
            pMachState,
            pRetData,                     /* Upd. flag for EXECUTABLE MA.  */
            pStackMA,                     /* Memory area for memo instr.   */
            pStackMA2,                    /* M.A. if word crossing border. */
            2,                            /* Bytes was written: 1 or 2.    */
            rSS,                          /* Real segment.                 */
            StOff);                       /* Real offset written to.       */
      }


   /* Ok, PUSH/POP instruction executed. Update rSP value.
    * No flags update required.
    */
   rSP=(WORD)(push?(rSP-2):(rSP+2));

   return(0);
}
/*=========================================================================*/


/*=========================================================================*/


int         do_LEA (
               MACHINE_STATE  *pMachState,   /* <-> Input/Output.          */
               RETURN_DATA    *pRetData,     /* <-  Output.                */
               WORD           *pTempIP       /* Ptr to temp IP.            */
               )
{
/*-------------------------------------------------------------------------*/
   MOD_RM_INFO    MRI;                       /* Mod/rm structure.          */
   int            op2;
/*-------------------------------------------------------------------------*/


   /* Query Mod/RM byte.
    */
   op2 = QueryNextOpcode (pMachState, pRetData, pTempIP);

   if (op2 < 0)
      {
      /* Code buffer not found! Nothing to do. Return...
       * <pRetData> already initialized.
       */
      return (-1);
      }


   /* Initialize <pMRI> structure from Mode/RM byte.
    */
   if (ReadMOD_RM (
         pMachState,                         /*  -> For <QueryNextOpcode>  */
         pRetData,                           /* <-  For <QueryNextOpcode>  */
         pTempIP,                            /* <-> For <QueryNextOpcode>  */
         1,                                  /*  -> 1-use WORD Regs.       */
         0,                                  /*  -> 1-init Reg2 as seg.reg.*/
         op2,                                /*  -> Mod/RM byte itself.    */
         &MRI                                /* <-  Initialized after.     */
         ) < 0)
      {
      /*-------------------------------------------------------------------*/
      /* Something went wrong. <pRetData> already initialized. Return.     */
      /*-------------------------------------------------------------------*/
      return (-1);
      }

   /* Cannot be direct register on the place of MRI.pReg1
    */
   if (MRI.pReg1)
      {
      pRetData->SegTrap    = rCS;
      pRetData->OffTrap    = rIP;
      pRetData->ulRetCode |= UNSUPPORTED_OPCODE;
      return (-1);
      }


   /* Execute actual LEA instruction.
    */
   *(WORD *)(MRI.pReg2) = MRI.Off1;

   return (0);
}
/*=========================================================================*/


/*=========================================================================*/

/*
 * Function will set up all registers and flags, required right before
 * jumping into the INT xx code. Roughly function will do following:
 *
 * push  flags
 * push  cs
 * push  ip
 * mov   cs,0:[int_no*4 + 2]
 * mov   ip,0:[int_no*4]
 * clrIF
 * clrTF
 *
 * Function will return (0) if everything went ok, or (-1) if:
 * -  IVT data area wasnt supplied.
 * -  Not enough stack space.
 */

int         SetRegistersForInt   (
               MACHINE_STATE  *pMachState,   /* <-> Input/Output.          */
               RETURN_DATA    *pRetData,     /* <-  Output.                */
               WORD           RetIP,         /* IP of instruction to       */
                                             /* be executed after INTxx    */
                                             /* will be completed.         */
               int            IntNo)
{
/*-------------------------------------------------------------------------*/
   MEMORY_AREA    *pMACS, *pMACS2, *pMAIP, *pMAIP2;
/*-------------------------------------------------------------------------*/

   if (MapAddressToMemoryArea (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         0,                                  /* Segment to map.            */
         (WORD)(IntNo*4),                    /* Offset to map.             */
         READABLE,                           /* Desired memory attributes. */
         1,                                  /* 1-word, 0-byte.            */
         &pMAIP,                             /* Memory area for memo instr.*/
         &pMAIP2                             /* M.A. if word crossing border.*/
         ) != 0)
      {
      /* IVT isn't supplied. Cannot continue.
       */
      pRetData->ulRetCode |= CANNOT_EXECUTE_INT;
      return (-1);
      }

   if (MapAddressToMemoryArea (
         pMachState,                         /* Machine state.             */
         pRetData,                           /* Return data.               */
         0,                                  /* Segment to map.            */
         (WORD)(IntNo*4+2),                  /* Offset to map.             */
         READABLE,                           /* Desired memory attributes. */
         1,                                  /* 1-word, 0-byte.            */
         &pMACS,                             /* Memory area for memo instr.*/
         &pMACS2                             /* M.A. if word crossing border.*/
         ) != 0)
      {
      /* IVT isn't completely supplied. Cannot continue.
       */
      pRetData->ulRetCode |= CANNOT_EXECUTE_INT;
      return (-1);
      }


   /* PUSH flags.
    */
   if (do_PUSH_POP (pMachState, pRetData, &rFlags, 1) != 0)
      {
      /* Not enough stack available. Cannot continue.
       */
      pRetData->ulRetCode |= CANNOT_EXECUTE_INT;
      return (-1);
      }


   /* PUSH CS.
    */
   if (do_PUSH_POP (pMachState, pRetData, &rCS, 1) != 0)
      {
      /* Not enough stack available. Cannot continue.
       */
      pRetData->ulRetCode |= CANNOT_EXECUTE_INT;
      return (-1);
      }


   /* PUSH IP.
    */
   if (do_PUSH_POP (pMachState, pRetData, &RetIP, 1) != 0)
      {
      /* Not enough stack available. Cannot continue.
       */
      pRetData->ulRetCode |= CANNOT_EXECUTE_INT;
      return (-1);
      }


   /* Initialize New CS and IP values.
    */
   rCS = ReadWordFromMA (pMACS, pMACS2, 0, (WORD)(IntNo*4+2));
   rIP = ReadWordFromMA (pMAIP, pMAIP2, 0, (WORD)(IntNo*4));


   /* Clear flags.
    */
   clrIF;
   clrTF;

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

/* Function will call user-supplied pointer to an INT emulation function.
 * After that results will be analyzed.
 * On entry <*pTempIP> must point to instruction to be executed after
 * INT xx will be completed.
 */

int         ExecuteUserInt  (
               MACHINE_STATE  *pMachState,   /* <-> Input/Output.          */
               RETURN_DATA    *pRetData,     /* <-  Output.                */
               WORD           *pTempIP,
               int            IntNo)
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


   if (pMachState->pfnINTxx)
      {

      /* Execute user supplied pointer to function.
       */
      switch (pMachState->pfnINTxx (pMachState, pRetData, IntNo))
         {
         case  USER_INT_CONTINUE:   /* 0  */
            /*-------------------------------------------------------------*/
            /* Execution successful. Continue normal flow from the next    */
            /* instruction.                                                */
            /* This case also could be considered as 'ignore INT'. Say if  */
            /* user decided to do nothing into interrupt and just continue */
            /* - that would be the proper return code to return.           */
            /*-------------------------------------------------------------*/

            break;


         case  USER_INT_YOURSELF:   /* 1  */
            /*-------------------------------------------------------------*/
            /* Caller's order: Try to execute INT xx directly.             */
            /* Do this part now.                                           */
            /*                                                             */
            /*   push  flags                                               */
            /*   push  cs                                                  */
            /*   push  ip                                                  */
            /*   mov   cs,0:[int_no*4 + 2]                                 */
            /*   mov   ip,0:[int_no*4]                                     */
            /*   clrIF;                                                    */
            /*   clrTF;                                                    */
            /*-------------------------------------------------------------*/

            if (SetRegistersForInt (pMachState, pRetData, *pTempIP, IntNo) == -1)
               /* Either IVT data area wasn't supplied of not enough stack
                * space. <pRetData->ulRetCode> already initialized.
                */
               return (-1);

            /* Set TempIP = rIP.
             * That will forse execution to be resumed
             * from inside of INT xx handler. <rCS> already setted up.
             */
            *pTempIP = rIP;

            /* Ok, here we already have all registers setted up - ready
             * to execute Interrupt itself from possibly supplied memory
             * area. Exiting. IRET will be met somewhere in a future.
             */

            break;

#ifdef CR_TSR
/* jce */
         case USER_INT_TSR:
            pRetData->ulRetCode |= TEST_TSR;
            break;
/* end jce */
#endif

         case  USER_INT_ABORT:      /* -1 */
         default:
            /*-------------------------------------------------------------*/
            /* User requested abort.                                       */
            /* Either execution of INT was unsuccessful of something else  */
            /* happened - this is caller's decision to stop everything.    */
            /* There could also be the cases, when <ExecuteUserInt> get    */
            /* called by Step-Trace mode or by INTO or by INT3(CC).        */
            /* In any case rIP and TempIP points to the next instruction   */
            /* after INT xx.                                               */
            /*-------------------------------------------------------------*/
            pRetData->ulRetCode |= USER_ABORT;
            break;
         }
      }
   else
      {
      /*-------------------------------------------------------------------*/
      /* Pointer to user-defined callback function does not supplied.      */
      /* Main switchboard will check 'ignore' flags to determine whether   */
      /* to continue execution or not.                                     */
      /*-------------------------------------------------------------------*/

      pRetData->ulRetCode |= CANNOT_EXECUTE_INT;
      return (-1);
      }

   return (0);

}
/*=========================================================================*/


/*=========================================================================*/

WORD        ReadWordFromPTR (
               VOID    *ptr,
               VOID    *ptr2
               )
{
/*-------------------------------------------------------------------------*/
   WORD           RetVal;
/*-------------------------------------------------------------------------*/


   if (_LastMRFromFile)
      UpdateMemReadRecords(_ulLastMRLA);
#if defined(CR_DEBUG) && MEMRECDBG
   else
      if (cr_hd && cr_itrace)
         fprintf(cr_dbgf, "Read from region not loaded from file: %08lX\n", _ulLastMRLA);
#endif

   if (!ptr2)
      {
      RetVal = (*(WORD *)(ptr));

      #ifdef   BIG_ENDIAN
         RetVal = SWAPB(RetVal);
      #endif
      }
   else
      {
      RetVal  = *(BYTE *)(ptr);
      RetVal += ((WORD)(((WORD)(*(BYTE *)(ptr2))) << 8));
      }

   return (RetVal);

}
/*=========================================================================*/

/*=========================================================================*/

BYTE        ReadByteFromPTR (
               VOID    *ptr
               )
{
   if (_LastMRFromFile)
      UpdateMemReadRecords(_ulLastMRLA);
#if defined(CR_DEBUG) && MEMRECDBG
   else
      if (cr_hd && cr_itrace)
         fprintf(cr_dbgf, "Read from region not loaded from file: %08lX\n", _ulLastMRLA);
#endif
   return *(BYTE *)(ptr);
}
/*=========================================================================*/


/*=========================================================================*/

VOID        WriteWordToPTR (
               WORD    word,
               VOID    *ptr,
               VOID    *ptr2
               )
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/


   if (!ptr2)
      {
      #ifdef   BIG_ENDIAN
         word = SWAPB(word);
      #endif

      *(WORD *)(ptr)  = word;
      }
   else
      {
      *(BYTE *)(ptr)  = (BYTE)(word & 0xFF);
      *(BYTE *)(ptr2) = (BYTE)(word >> 8);
      }

   return;
}
/*=========================================================================*/


/*=========================================================================*/

/* RealSegment:RealOffset must be inside of pMA,pMA2.
 */

WORD        ReadWordFromMA (
               MEMORY_AREA    *pMA,
               MEMORY_AREA    *pMA2,
               WORD           RealSegment,
               WORD           RealOffset
               )
{
/*-------------------------------------------------------------------------*/
   WORD           RetVal;
/*-------------------------------------------------------------------------*/

   if (pMA->FromFile)
      UpdateMemReadRecords (LinearAddr(RealSegment,RealOffset));
#if defined(CR_DEBUG) && MEMRECDBG
   else
      if (cr_hd && cr_itrace)
         fprintf(cr_dbgf, "Read from region not loaded from file: %08lX\n",
                 LinearAddr(RealSegment,RealOffset));
#endif

   if (!pMA2)
      {
      RetVal  = *(WORD *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA)));

      #ifdef   BIG_ENDIAN
         RetVal = SWAPB(RetVal);
      #endif
      }
   else
      {
      RetVal   = *(BYTE *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA)));
      RetVal  += ((WORD)(((WORD)(*(BYTE *)(pMA2->pBuff+(WORD)(LinearAddr(RealSegment,(WORD)(RealOffset+1))-MALinearAddr(pMA2))))) << 8));
      }

   return (RetVal);
}
/*=========================================================================*/

/*=========================================================================*/

/* RealSegment:RealOffset must be inside of pMA,pMA2.
 */

BYTE       ReadByteFromMA (
               MEMORY_AREA    *pMA,
               WORD           RealSegment,
               WORD           RealOffset
               )
{
   // for codedata stuff
   //if (pMA->FromFile)
   UpdateMemReadRecords (LinearAddr(RealSegment,RealOffset));
#if defined(CR_DEBUG) && MEMRECDBG
   else
      if (cr_hd && cr_itrace)
         fprintf(cr_dbgf, "Read from region not loaded from file: %08lX\n",
                 LinearAddr(RealSegment,RealOffset));
#endif

   return (*(BYTE *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA))));
}

/*=========================================================================*/


/*=========================================================================*/

/* RealSegment:RealOffset must be inside of pMA,pMA2.
 */

VOID        WriteWordToMA (
               WORD           word,
               MEMORY_AREA    *pMA,
               MEMORY_AREA    *pMA2,
               WORD           RealSegment,
               WORD           RealOffset
               )
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

   if (!pMA2)
      {
      #ifdef   BIG_ENDIAN
         word = SWAPB(word);
      #endif

      *(WORD *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA))) = word;
      }
   else
      {
      *(BYTE *)(pMA->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset)-MALinearAddr(pMA))) = (BYTE)(word & 0xFF);
      *(BYTE *)(pMA2->pBuff+(WORD)(LinearAddr(RealSegment,RealOffset+1)-MALinearAddr(pMA2))) = (BYTE)(word >> 8);
      }

   return;

}
/*=========================================================================*/


/*=========================================================================*/


void        SetAndOrXorFlags (WORD *pFlags, int signed_value)
{
/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

   /* Set flags.
    */
   *pFlags &= ~OVERFLOW_FLAG_MASK;
   *pFlags &= ~CARRY_FLAG_MASK;

   /* SF */
   if (signed_value & 0x8000)
      *pFlags |= SIGN_FLAG_MASK;
   else
      *pFlags &= ~SIGN_FLAG_MASK;

   /* ZF */
   if (signed_value==0)
      *pFlags |= ZERO_FLAG_MASK;
   else
      *pFlags &= ~ZERO_FLAG_MASK;

   /* PF */
   if (ParityFlag(signed_value))
      *pFlags |= PARITY_FLAG_MASK;
   else
      *pFlags &= ~PARITY_FLAG_MASK;

   return;
}
/*=========================================================================*/


/*=========================================================================*/

/* Set flags as a result of arithmetic addition.
 * <pFlags> - copy of lfags to be re-initialized.
 * <size> - number of bytes in operands. 1 or 2 - byte or word operands.
 * <int1>, <int2> - operands to be added.
 */

void        SetArithmFlags (WORD *pFlags, int size, long int1, long int2)
{
/*-------------------------------------------------------------------------*/
   long           int3;
/*-------------------------------------------------------------------------*/

   /* CF */
   if ((int1+int2) & (size==1?0xFF00UL:0xFFFF0000UL))
      *pFlags |= CARRY_FLAG_MASK;
   else
      *pFlags &= ~CARRY_FLAG_MASK;


   /* PF */
   if (ParityFlag((int)(int1 + int2)))
      *pFlags |= PARITY_FLAG_MASK;
   else
      *pFlags &= ~PARITY_FLAG_MASK;


   /* AF */
   if (int2 < 0)
      {
      /* sub
       */
      if ((int1&0x0F) < ((-int2)&0x0F))
         *pFlags |= AUX_FLAG_MASK;
      else
         *pFlags &= ~AUX_FLAG_MASK;
      }
   else
      {
      if (((int1&0x0F) + (int2&0x0F)) & 0xF0)
         *pFlags |= AUX_FLAG_MASK;
      else
         *pFlags &= ~AUX_FLAG_MASK;
      }


   /* ZF */
   if (((int1+int2)&((size==1)?0xFF:0xFFFF))==0)
      *pFlags |= ZERO_FLAG_MASK;
   else
      *pFlags &= ~ZERO_FLAG_MASK;


   /* SF */
   if ((int1+int2)&(size==1?0x80:0x8000))
      *pFlags |= SIGN_FLAG_MASK;
   else
      *pFlags &= ~SIGN_FLAG_MASK;


   /* OF */

   int3 =  (size==1)?(long)((char)int1):(long)((short)int1);
   int3 += (size==1)?(long)((char)int2):(long)((short)int2);
   if (int3<0)
      int3=-int3;
   if (int3 & (size==1?0xFF00UL:0xFFFF0000UL))
      *pFlags |= OVERFLOW_FLAG_MASK;
   else
      *pFlags &= ~OVERFLOW_FLAG_MASK;

}
/*=========================================================================*/


/*=========================================================================*/

/* Return value of parity flag for <input>.
 */

int         ParityFlag (int input)
{
/*-------------------------------------------------------------------------*/
   int            bits;
/*-------------------------------------------------------------------------*/

   bits =(input   )&1;
   bits+=(input>>1)&1;
   bits+=(input>>2)&1;
   bits+=(input>>3)&1;
   bits+=(input>>4)&1;
   bits+=(input>>5)&1;
   bits+=(input>>6)&1;
   bits+=(input>>7)&1;
   if (bits&1)
      return (0);
   else
      return (1);

}
/*=========================================================================*/
