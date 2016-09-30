/*
Copyright Notice
================
BOCHS is Copyright 1994,1995,1996 by Kevin P. Lawton.

BOCHS is commercial software.

For more information, read the file 'LICENSE' included in the bochs
distribution.  If you don't have access to this file, or have questions
regarding the licensing policy, the author may be contacted via:

    US Mail:  Kevin Lawton
              528 Lexington St.
              Waltham, MA 02154

    EMail:    bochs@world.std.com
*/




#include "bochs.h"




  INLINE void
bx_DAS()
{
  Bit8u tmpCF, tmpAL;
#if 0 /* riad */
bx_panic("das\n");
#endif
  /* ??? */
  /* the algorithm for DAS is fashioned after the pseudo code in the
   * Pentium Processor Family Developer's Manual, volume 3.  It seems
   * to have changed from earlier processor's manuals.  I'm not sure
   * if this is a correction in the algorithm printed, or Intel has
   * changed the handling of instruction.  It might not even be
   * correct yet...
   */

  tmpCF = 0;
  tmpAL = AL;

  /* DAS effect the following flags: A,C,S,Z,P */

  if (((tmpAL & 0x0F) > 0x09) || bx_get_AF()) {
    bx_set_AF(1);
    tmpCF = (AL < 0x06) || bx_get_CF();
    AL = AL - 0x06;
    /*tmpCF = (AL < 0) || CF;*/
    }
  if ( (tmpAL > 0x99) || bx_get_CF() ) {
    AL = AL - 0x60;
    tmpCF = 1;
    }

  bx_set_CF(tmpCF);
  bx_set_SF(AL >> 7);
  bx_set_ZF(AL==0);
  bx_set_PF_base(AL);
}

  INLINE void
bx_AAA()
{
  Bit8u ALcarry;
#if 0 /* riad */
bx_panic("aaa\n");
#endif
  ALcarry = AL > 0xf9;

  /* AAA effects the following flags: A,C */
  if ( ((AL & 0x0f) > 9) || bx_get_AF() ) {
    AL = (AL + 6) & 0x0f;
    AH = AH + 1 + ALcarry;
    bx_set_AF(1);
    bx_set_CF(1);
    }
  else {
    bx_set_AF(0);
    bx_set_CF(0);
    AL = AL & 0x0f;
    }
}

  INLINE void
bx_AAS()
{
  Bit8u ALborrow;

  //bx_printf("aas\n");
  /* AAS affects the following flags: A,C */

  ALborrow = AL < 6;

  if ( ((AL & 0x0F) > 0x09) || bx_get_AF() ) {
    AL = (AL - 6) & 0x0f;
    AH = AH - 1 - ALborrow;
    bx_set_AF(1);
    bx_set_CF(1);
    }
  else {
    bx_set_CF(0);
    bx_set_AF(0);
    AL = AL & 0x0f;
    }
}

  INLINE void
bx_AAM()
{
  Bit8u al, imm8;
#if 0 /* riad */
bx_panic("aam\n");
#endif
  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  al = AL;
  AH = al / imm8;
  AL = al % imm8;

  /* AAM affects the following flags: S,Z,P */
  bx_set_SF((AH & 0x80) > 0);
  bx_set_ZF(AX==0);
  bx_set_PF_base(al); /* ??? */
}

  INLINE void
bx_AAD()
{
  Bit8u imm8;

  //bx_printf("aad\n");
  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  AL = AH * imm8 + AL;
  AH = 0;

  /* AAD effects the following flags: S,Z,P */
  bx_set_SF(AL >= 0x80);
  bx_set_ZF(AL == 0);
  bx_set_PF_base(AL);
}

  INLINE void
bx_DAA()
{
  Bit8u al;
#if 0 /* riad */
bx_panic("daa\n");
#endif
  al = AL;

  /* DAA affects the following flags: S,Z,A,P,C */

  if (((al & 0x0F) > 0x09) || bx_get_AF()) {
    al = al + 0x06;
#if 0
??? This if () clause shows up in PT8, but not intel books
    if (al < 0x06) { /* carry out of bit7 */
      bx_set_CF(1);
      bx_printf("DAA: carry out of bit7 after adding 6\n");
      }
#endif
    }

  if ((al > 0x9F) || bx_get_CF()) {
    al = al + 0x60;
    bx_set_CF(1);
    }

  AL = al;

  bx_set_SF(al >> 7);
  bx_set_ZF(al==0);
  bx_set_PF_base(al);
}
