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





#define BX_ARITH_C 1

#include "bochs.h"

#ifdef BX_NO_64BIT_TYPE
#  if BX_CPU > 2
#    error "386+: 64bit inherent data type support needed. Use gcc instead?"
#  endif
#endif


  INLINE void
bx_MUL_ALEb()
{
  Bit8u op2, op1;
  Bit16u product_16;
  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;
  Boolean temp_flag;

  /* op2 is mod+r/m, op1 is the AL register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  op1 = AL;

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  product_16 = op1 * op2;

  /* set EFLAGS:
   * MUL affects the following flags: C,O
   */

  temp_flag = ((product_16 & 0xFF00) != 0);
  BX_SET_OxxxxC(temp_flag, temp_flag);

  /* now write product back to destination */

  AX = product_16;
}

  INLINE void
bx_MUL_eAXEv()
{
  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;

  
  /* op2 is mod+r/m, op1 is the AL register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u op1_32, op2_32, product_32h, product_32l;
    Bit64u product_64;
    Boolean temp_flag;

    op1_32 = EAX;

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    product_64 = ((Bit64u) op1_32) * ((Bit64u) op2_32);

    product_32l = (product_64 & 0xFFFFFFFF);
    product_32h = (product_64 >> 32);

    /* now write product back to destination */

    EAX = product_32l;
    EDX = product_32h;

    /* set eflags:
     * MUL affects the following flags: C,O
     */

    temp_flag = (product_32h != 0);
    BX_SET_OxxxxC(temp_flag, temp_flag);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16bit opsize mode */
    Bit16u op1_16, op2_16, product_16h, product_16l;
    Bit32u product_32;
    Boolean temp_flag;

    op1_16 = AX;

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    product_32 = ((Bit32u) op1_16) * ((Bit32u) op2_16);

    product_16l = (product_32 & 0xFFFF);
    product_16h = product_32 >> 16;

    /* now write product back to destination */

    AX = product_16l;
    DX = product_16h;

    /* set eflags:
     * MUL affects the following flags: C,O
     */

    temp_flag = (product_16h != 0);
    BX_SET_OxxxxC(temp_flag, temp_flag);
    }
}

  INLINE void
bx_IMUL_ALEb()
{
  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;

  Bit8s op2, op1;
  Bit16s product_16;
  Bit16u upper_bits;


  /* op2 is mod+r/m, op1 is the AL register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  op1 = AL;

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  product_16 = op1 * op2;

  /* now write product back to destination */

  AX = product_16;

  /* set EFLAGS:
   * IMUL affects the following flags: C,O
   * IMUL r/m8: condition for clearing CF & OF:
   *   AL = sign-extend of AL to 16 bits
   */
  upper_bits = AX & 0xff80;
  if (upper_bits==0xff80  ||  upper_bits==0x0000) {
    BX_SET_OxxxxC(0, 0);
    }
  else {
    BX_SET_OxxxxC(1, 1);
    }
}


  INLINE void
bx_IMUL_eAXEv()
{
  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;


  /* op2 is mod+r/m, op1 is the AL register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32s op1_32, op2_32;
    Bit64s product_64;
    Bit32u product_32h, product_32l;
  
    op1_32 = EAX;

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    product_64 = ((Bit64s) op1_32) * ((Bit64s) op2_32);

    product_32l = (product_64 & 0xFFFFFFFF);
    product_32h = (product_64 >> 32);

    /* now write product back to destination */

    EAX = product_32l;
    EDX = product_32h;

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r/m16: condition for clearing CF & OF:
     *   EDX:EAX = sign-extend of EAX
     */

    if ( (EDX==0xffffffff) && (EAX & 0x80000000) ) {
      BX_SET_OxxxxC(0, 0);
      }
    else if ( (EDX==0x00000000) && (EAX < 0x80000000) ) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16bit opsize mode */
    Bit16s op1_16, op2_16;
    Bit32s product_32;
    Bit16u product_16h, product_16l;

    op1_16 = AX;

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    product_32 = ((Bit32s) op1_16) * ((Bit32s) op2_16);

    product_16l = (product_32 & 0xFFFF);
    product_16h = product_32 >> 16;

    /* now write product back to destination */

    AX = product_16l;
    DX = product_16h;

    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r/m16: condition for clearing CF & OF:
     *   DX:AX = sign-extend of AX
     */

    if ( (DX==0xffff) && (AX & 0x8000) ) {
      BX_SET_OxxxxC(0, 0);
      }
    else if ( (DX==0x0000) && (AX < 0x8000) ) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
}

  INLINE void
bx_DIV_ALEb()
{
  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;

  Bit8u op2, quotient_8l, remainder_8;
  Bit16u quotient_16, op1;


  op1 = AX;

  /* op2 is mod+r/m, op1 is the AL register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  if (op2 == 0) {
    bx_panic("divide by error in DIV_ALEb\n");
    }
  quotient_16 = op1 / op2;
  remainder_8 = op1 % op2;
  quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l) {
    bx_panic("quotient too large causes divide error fault in DIV_ALEb\n");
    }

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */

  AL = quotient_8l;
  AH = remainder_8;
}

  INLINE void
bx_DIV_eAXEv()
{
  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;


  /* op2 is mod+r/m, op1 is the AX/EAX register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32u op2_32, remainder_32, quotient_32l;
    Bit64u op1_64, quotient_64;

    op1_64 = (((Bit64u) EDX) << 32) + ((Bit64u) EAX);

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_32 == 0) {
      bx_panic("divide by 0 error in DIV_eAXEv\n");
      }
    quotient_64 = op1_64 / op2_32;
    remainder_32 = op1_64 % op2_32;
    quotient_32l = quotient_64 & 0xFFFFFFFF;

    if (quotient_64 != quotient_32l) {
      bx_panic("quotient too large causes divide error fault in DIV_eAXEv\n");
      }

    /* set EFLAGS:
     * DIV affects the following flags: O,S,Z,A,P,C are undefined
     */

    /* now write quotient back to destination */

    EAX = quotient_32l;
    EDX = remainder_32;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize mode */
    Bit16u op2_16, remainder_16, quotient_16l;
    Bit32u op1_32, quotient_32;

    op1_32 = (((Bit32u) DX) << 16) | ((Bit32u) AX);

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_16 == 0) {
      bx_panic("divide by 0 error in DIV_eAXEv\n");
      }
    quotient_32 = op1_32 / op2_16;
    remainder_16 = op1_32 % op2_16;
    quotient_16l = quotient_32 & 0xFFFF;

    if (quotient_32 != quotient_16l) {
      bx_panic("quotient too large causes divide error fault in DIV_eAXEv\n");
      }

    /* set EFLAGS:
     * DIV affects the following flags: O,S,Z,A,P,C are undefined
     */

    /* now write quotient back to destination */

    AX = quotient_16l;
    DX = remainder_16;
    }
}

  INLINE void
bx_IDIV_ALEb()
{
  Bit8s op2, quotient_8l, remainder_8;
  Bit16s quotient_16, op1;

  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;

  op1 = AX;

  /* op2 is mod+r/m, op1 is the AL register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    op2 = BX_READ_8BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_byte(op2_seg_reg, op2_addr, &op2);
    BX_HANDLE_EXCEPTION()
    }

  if (op2 == 0) {
    bx_panic("divide by error in DIV_ALEb\n");
    }

  quotient_16 = op1 / op2;
  remainder_8 = op1 % op2;
  quotient_8l = quotient_16 & 0xFF;

  if (quotient_16 != quotient_8l) {
bx_printf("quotient_16: %04x, remainder_8: %02x, quotient_8l: %02x\n",
  (unsigned) quotient_16, (unsigned) remainder_8, (unsigned) quotient_8l);
AL = quotient_8l;
AH = remainder_8;
bx_printf("AH: %02x, AL: %02x\n", (unsigned) AH, (unsigned) AL);
    bx_panic("quotient too large causes divide error fault in IDIV_ALEb\n");
    }

  /* set EFLAGS:
   * DIV affects the following flags: O,S,Z,A,P,C are undefined
   */

  /* now write quotient back to destination */

  AL = quotient_8l;
  AH = remainder_8;
}

  INLINE void
bx_IDIV_eAXEv()
{
  Bit32u op2_addr;
  unsigned op2_type, unused;
  bx_segment_reg_t *op2_seg_reg;


  /* op2 is mod+r/m, op1 is the AX/EAX register */
  bx_decode_exgx(&unused, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit32s op2_32, remainder_32, quotient_32l;
    Bit64s op1_64, quotient_64;

    op1_64 = (((Bit64u) EDX) << 32) | ((Bit64u) EAX);

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_32 == 0) {
      bx_panic("divide by 0 error in DIV_eAXEv\n");
      }
    quotient_64 = op1_64 / op2_32;
    remainder_32 = op1_64 % op2_32;
    quotient_32l = quotient_64 & 0xFFFFFFFF;

    if (quotient_64 != quotient_32l) {
      bx_panic("quotient too large causes divide error fault in DIV_eAXEv\n");
      }

    /* set EFLAGS:
     * IDIV affects the following flags: O,S,Z,A,P,C are undefined
     */

    /* now write quotient back to destination */

    EAX = quotient_32l;
    EDX = remainder_32;
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit opsize mode */
    Bit16s op2_16, remainder_16, quotient_16l;
    Bit32s op1_32, quotient_32;

    op1_32 = ((((Bit32u) DX) << 16) | ((Bit32u) AX));

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_16 == 0) {
      bx_panic("divide by 0 error in DIV_eAXEv\n");
      }
    quotient_32 = op1_32 / op2_16;
    remainder_16 = op1_32 % op2_16;
    quotient_16l = quotient_32 & 0xFFFF;

    if (quotient_32 != quotient_16l) {
      bx_panic("quotient too large causes divide error fault in DIV_eAXEv\n");
      }

    /* set EFLAGS:
     * IDIV affects the following flags: O,S,Z,A,P,C are undefined
     */

    /* now write quotient back to destination */

    AX = quotient_16l;
    DX = remainder_16;
    }
}


  INLINE void
bx_IMUL_GvEvIv()
{
#if BX_CPU < 2
  bx_panic("IMUL_GvEvIv() unsupported on 8086!\n");
#else

  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()
  
  
#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32s op2_32, op3_32, product_32;
    Bit64s product_64;

    op3_32 = bx_fetch_next_dword();
    BX_HANDLE_EXCEPTION()
  
    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }
  
    product_32 = op2_32 * op3_32;
    product_64 = ((Bit64s) op2_32) * ((Bit64s) op3_32);
  
    /* now write product back to destination */
    BX_WRITE_32BIT_REG(op1_addr, product_32);
  
    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */
  
    if (product_64 == product_32) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
  else /* 16 bit operands */
#endif
    {
    Bit16u product_16l;
    Bit16s op2_16, op3_16;
    Bit32s product_32;

    op3_16 = bx_fetch_next_word();
    BX_HANDLE_EXCEPTION()
  
    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }
  
    product_32 = op2_16 * op3_16;
  
    product_16l = (product_32 & 0xFFFF);
  
    /* now write product back to destination */
    BX_WRITE_16BIT_REG(op1_addr, product_16l);
  
    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */
  
    if (product_32 > -32768  && product_32 < 32767) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
#endif
}

  INLINE void
bx_IMUL_GvEvIb()
{
#if BX_CPU < 2
  bx_panic("IMUL_GvEvIb() unsupported on 8086!\n");
#else

  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  /* op1 is a reg, op2 is mod+r/m, op3 is immediate */
  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()
  
#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit32s op2_32, op3_32, product_32;
    Bit64s product_64;

    op3_32 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()
  
    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }
  
    product_32 = op2_32 * op3_32;
    product_64 = ((Bit64s) op2_32) * ((Bit64s) op3_32);
  
    /* now write product back to destination */
    BX_WRITE_32BIT_REG(op1_addr, product_32);
  
    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */
  
    if (product_64 == product_32) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
  else /* 16 bit operands */
#endif
    {
    Bit16u product_16l;
    Bit16s op2_16, op3_16;
    Bit32s product_32;

    op3_16 = (Bit8s) bx_fetch_next_byte();
    BX_HANDLE_EXCEPTION()
  
    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }
  
    product_32 = op2_16 * op3_16;
  
    product_16l = (product_32 & 0xFFFF);
  
    /* now write product back to destination */
    BX_WRITE_16BIT_REG(op1_addr, product_16l);
  
    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */
  
    if (product_32 > -32768  && product_32 < 32767) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
#endif
}

  INLINE void
bx_IMUL_GvEv()
{
#if BX_CPU < 3
  bx_panic("IMUL_GvEv() unsupported on 8086!\n");
#else

  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  /* op2 is mod+r/m, op1 is a register */
  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) {
    Bit32s op1_32, op2_32, product_32;
    Bit64s product_64;

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    op1_32 = BX_READ_32BIT_REG(op1_addr);
  
    product_32 = op1_32 * op2_32;
    product_64 = ((Bit64s) op1_32) * ((Bit64s) op2_32);
  
    /* now write product back to destination */
    BX_WRITE_32BIT_REG(op1_addr, product_32);
  
    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */
  
    if (product_64 == product_32) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
  else { /* 16 bit operands */
    Bit16u product_16l;
    Bit16s op1_16, op2_16;
    Bit32s product_32;

    /* op2 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    op1_16 = BX_READ_16BIT_REG(op1_addr);
  
    product_32 = op1_16 * op2_16;
  
    product_16l = (product_32 & 0xFFFF);
  
    /* now write product back to destination */
    BX_WRITE_16BIT_REG(op1_addr, product_16l);
  
    /* set eflags:
     * IMUL affects the following flags: C,O
     * IMUL r16,r/m16,imm16: condition for clearing CF & OF:
     *   result exactly fits within r16
     */
  
    if (product_32 > -32768  && product_32 < 32767) {
      BX_SET_OxxxxC(0, 0);
      }
    else {
      BX_SET_OxxxxC(1, 1);
      }
    }
#endif
}
