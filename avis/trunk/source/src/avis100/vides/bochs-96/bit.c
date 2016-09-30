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
bx_SETO_Eb()
{
#if BX_CPU < 3
  bx_panic("SETO: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNO_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNO: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_OF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETB_Eb()
{
#if BX_CPU < 3
  bx_panic("SETB: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_CF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNB_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNB: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_CF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETZ_Eb()
{
#if BX_CPU < 3
  bx_panic("SETZ: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_ZF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNZ_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNZ: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_ZF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETBE_Eb()
{
#if BX_CPU < 3
  bx_panic("SETBE: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_CF() || bx_get_ZF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNBE_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNBE: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if ((bx_get_CF()==0) && (bx_get_ZF()==0))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETS_Eb()
{
#if BX_CPU < 3
  bx_panic("SETS: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_SF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNS_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNL: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_SF()==0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETP_Eb()
{
#if BX_CPU < 3
  bx_panic("SETP: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_PF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNP_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNP: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_PF() == 0)
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETL_Eb()
{
#if BX_CPU < 3
  bx_panic("SETL: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_SF() != bx_get_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNL_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNL: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_SF() == bx_get_OF())
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETLE_Eb()
{
#if BX_CPU < 3
  bx_panic("SETLE: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_get_ZF() || (bx_get_SF()!=bx_get_OF()))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}

  INLINE void
bx_SETNLE_Eb()
{
#if BX_CPU < 3
  bx_panic("SETNLE: not available on < 386\n");
#else
  Bit8u result_8;
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if ((bx_get_ZF()==0) && (bx_get_SF()==bx_get_OF()))
    result_8 = 1;
  else
    result_8 = 0;

  /* now write result back to destination */
  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_8BIT_REG(op1_addr, result_8);
    }
  else {
    bx_write_virtual_byte(op1_seg_reg, op1_addr, &result_8);
    BX_HANDLE_EXCEPTION()
    }
#endif
}


  INLINE void
bx_BSF_GvEv()
{
#if BX_CPU < 3
  bx_panic("BSF_GvEv(): not supported on < 386\n");
#else
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32;

    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_32 == 0) {
      bx_set_ZF(1);
      /* op1_32 undefined */
      return;
      }

    op1_32 = 0;
    while ( (op2_32 & 0x01) == 0 ) {
      op1_32++;
      op2_32 >>= 1;
      }
    bx_set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(op1_addr, op1_32);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16;

    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_16 == 0) {
      bx_set_ZF(1);
      /* op1_16 undefined */
      return;
      }

    op1_16 = 0;
    while ( (op2_16 & 0x01) == 0 ) {
      op1_16++;
      op2_16 >>= 1;
      }
    bx_set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(op1_addr, op1_16);
    }
#endif
}

  INLINE void
bx_BSR_GvEv()
{
#if BX_CPU < 3
  bx_panic("BSR_GvEv(): not supported on < 386\n");
#else
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;


  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32;

    /* op2_32 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_32 = BX_READ_32BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op2_seg_reg, op2_addr, &op2_32);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_32 == 0) {
      bx_set_ZF(1);
      /* op1_32 undefined */
      return;
      }

    op1_32 = 31;
    while ( (op2_32 & 0x80000000) == 0 ) {
      op1_32--;
      op2_32 <<= 1;
      }
    bx_set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_32BIT_REG(op1_addr, op1_32);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16;

    /* op2_16 is a register or memory reference */
    if (op2_type == BX_REGISTER_REF) {
      op2_16 = BX_READ_16BIT_REG(op2_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op2_seg_reg, op2_addr, &op2_16);
      BX_HANDLE_EXCEPTION()
      }

    if (op2_16 == 0) {
      bx_set_ZF(1);
      /* op1_16 undefined */
      return;
      }

    op1_16 = 15;
    while ( (op2_16 & 0x8000) == 0 ) {
      op1_16--;
      op2_16 <<= 1;
      }
    bx_set_ZF(0);

    /* now write result back to destination */
    BX_WRITE_16BIT_REG(op1_addr, op1_16);
    }
#endif
}


INLINE void bx_BSWAP_EAX() {bx_panic("BSWAP: not implemented\n");}
INLINE void bx_BSWAP_ECX() {bx_panic("BSWAP: not implemented\n");}
INLINE void bx_BSWAP_EDX() {bx_panic("BSWAP: not implemented\n");}
INLINE void bx_BSWAP_EBX() {bx_panic("BSWAP: not implemented\n");}
INLINE void bx_BSWAP_ESP() {bx_panic("BSWAP: not implemented\n");}
INLINE void bx_BSWAP_EBP() {bx_panic("BSWAP: not implemented\n");}
INLINE void bx_BSWAP_ESI() {bx_panic("BSWAP: not implemented\n");}
INLINE void bx_BSWAP_EDI() {bx_panic("BSWAP: not implemented\n");}

  INLINE void
bx_BT_EvGv()
{
#if BX_CPU < 3
  bx_panic("BT_EvGv: not available on <386\n");
#else
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, index;
    Bit32s displacement32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      op2_32 &= 0x1f;
      bx_set_CF((op1_32 >> op2_32) & 0x01);
      return;
      }

    index = op2_32 & 0x1f;
    displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
    op1_addr += 4 * displacement32;

    /* pointer, segment address pair */
    bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
    BX_HANDLE_EXCEPTION()

    bx_set_CF((op1_32 >> index) & 0x01);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, index;
    Bit32s displacement32;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      bx_panic("BT_EvGv: Ev is a register reference\n");
      }

    index = op2_16 & 0x0f;
    displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
    op1_addr += 2 * displacement32;

    /* pointer, segment address pair */
    bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
    BX_HANDLE_EXCEPTION()

    bx_set_CF((op1_16 >> index) & 0x01);
    }
#endif
}

  INLINE void
bx_BTS_EvGv()
{
#if BX_CPU < 3
  bx_panic("BTS_EvGv: not available on <386\n");
#else
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, bit_i, index;
    Bit32s displacement32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      op2_32 &= 0x1f;
      bx_set_CF((op1_32 >> op2_32) & 0x01);
      op1_32 |= (((Bit32u) 1) << op2_32);

      /* now write diff back to destination */
      BX_WRITE_32BIT_REG(op1_addr, op1_32);
      return;
      }

    index = op2_32 & 0x1f;
    displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
    op1_addr += 4 * displacement32;

    /* pointer, segment address pair */
    bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
    BX_HANDLE_EXCEPTION()

    bit_i = (op1_32 >> index) & 0x01;
    op1_32 |= (((Bit32u) 1) << index);

    bx_write_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
    BX_HANDLE_EXCEPTION()

    bx_set_CF(bit_i);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, bit_i, index;
    Bit32s displacement32;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      bx_panic("BTS_EvGv: Ev is a register reference\n");
      }

    index = op2_16 & 0x0f;
    displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
    op1_addr += 2 * displacement32;

    /* pointer, segment address pair */
    bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
    BX_HANDLE_EXCEPTION()

    bit_i = (op1_16 >> index) & 0x01;
    op1_16 |= (((Bit16u) 1) << index);

    bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
    BX_HANDLE_EXCEPTION()

    bx_set_CF(bit_i);
    }
#endif
}

  INLINE void
bx_BTR_EvGv()
{
#if BX_CPU < 3
  bx_panic("BTR_EvGv: not available on <386\n");
#else
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, op2_32, index, temp_cf;
    Bit32s displacement32;

    /* op2_32 is a register, op2_addr is an index of a register */
    op2_32 = BX_READ_32BIT_REG(op2_addr);

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      op2_32 &= 0x1f;
      bx_set_CF((op1_32 >> op2_32) & 0x01);
      op1_32 &= ~(((Bit32u) 1) << op2_32);

      /* now write diff back to destination */
      BX_WRITE_32BIT_REG(op1_addr, op1_32);
      return;
      }

    index = op2_32 & 0x1f;
    displacement32 = ((Bit32s) (op2_32&0xffffffe0)) / 32;
    op1_addr += 4 * displacement32;

    /* pointer, segment address pair */
    bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
    BX_HANDLE_EXCEPTION()

    temp_cf = (op1_32 >> index) & 0x01;
    op1_32 &= ~(((Bit32u) 1) << index);

    /* now write back to destination */
    bx_write_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
    BX_HANDLE_EXCEPTION()

    bx_set_CF(temp_cf);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, op2_16, index, temp_cf;
    Bit32s displacement32;

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      bx_panic("BTR_EvGv: Ev is a register reference\n");
      }

    index = op2_16 & 0x0f;
    displacement32 = ((Bit16s) (op2_16&0xfff0)) / 16;
    op1_addr += 2 * displacement32;

    /* pointer, segment address pair */
    bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
    BX_HANDLE_EXCEPTION()

    temp_cf = (op1_16 >> index) & 0x01;
    op1_16 &= ~(((Bit16u) 1) << index);

    /* now write back to destination */
    bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
    BX_HANDLE_EXCEPTION()

    bx_set_CF(temp_cf);
    }
#endif
}

  INLINE void
bx_BTC_EvGv()
{
  bx_panic("BTC: not implemented\n");
}

  INLINE void
bx_BT_EvIb()
{
#if BX_CPU < 3
  bx_panic("BT_EvIb: not available on <386\n");
#else
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32;
    Bit8u  op2_8;

    op2_8 = bx_fetch_next_byte();
    op2_8 %= 32;
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    bx_set_CF((op1_32 >> op2_8) & 0x01);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16;
    Bit8u  op2_8;


    op2_8 = bx_fetch_next_byte();
    op2_8 %= 16;
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    bx_set_CF((op1_16 >> op2_8) & 0x01);
    }
#endif
}

  INLINE void
bx_BTS_EvIb()
{
#if BX_CPU < 3
  bx_panic("BTS_EvIb: not available on <386\n");
#else
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, temp_CF;
    Bit8u  op2_8;

    op2_8 = bx_fetch_next_byte();
    op2_8 %= 32;
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    temp_CF = (op1_32 >> op2_8) & 0x01;
    op1_32 |= (((Bit32u) 1) << op2_8);
    
    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, op1_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }
    bx_set_CF(temp_CF);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, temp_CF;
    Bit8u  op2_8;


    op2_8 = bx_fetch_next_byte();
    op2_8 %= 16;
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    temp_CF = (op1_16 >> op2_8) & 0x01;
    op1_16 |= (((Bit16u) 1) << op2_8);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, op1_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }
    bx_set_CF(temp_CF);
    }
#endif
}

  INLINE void
bx_BTC_EvIb()
{
#if BX_CPU < 3
  bx_panic("BTC_EvIb: not available on <386\n");
#else
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, temp_CF;
    Bit8u  op2_8;

    op2_8 = bx_fetch_next_byte();
    op2_8 %= 32;
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    temp_CF = (op1_32 >> op2_8) & 0x01;
    
    op1_32 &= ~(((Bit32u) 1) << op2_8);  /* clear out bit */
    op1_32 |= (((Bit32u) !temp_CF) << op2_8); /* set to complement */

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, op1_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }
    bx_set_CF(temp_CF);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, temp_CF;
    Bit8u  op2_8;


    op2_8 = bx_fetch_next_byte();
    op2_8 %= 16;
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    temp_CF = (op1_16 >> op2_8) & 0x01;
    op1_16 &= ~(((Bit16u) 1) << op2_8);  /* clear out bit */
    op1_16 |= (((Bit16u) !temp_CF) << op2_8); /* set to complement */

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, op1_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }
    bx_set_CF(temp_CF);
    }
#endif
}

  INLINE void
bx_BTR_EvIb()
{
#if BX_CPU < 3
  bx_panic("BTR_EvIb: not available on <386\n");
#else
  Bit32u op1_addr;
  unsigned op1_type, unused;
  bx_segment_reg_t *op1_seg_reg;


  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_cpu.is_32bit_opsize) { /* 32 bit operand size mode */
    /* for 32 bit operand size mode */
    Bit32u op1_32, temp_CF;
    Bit8u  op2_8;

    op2_8 = bx_fetch_next_byte();
    op2_8 %= 32;
    BX_HANDLE_EXCEPTION()

    /* op1_32 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_32 = BX_READ_32BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }

    temp_CF = (op1_32 >> op2_8) & 0x01;
    op1_32 &= ~(((Bit32u) 1) << op2_8);
    
    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_32BIT_REG(op1_addr, op1_32);
      }
    else {
      bx_write_virtual_dword(op1_seg_reg, op1_addr, &op1_32);
      BX_HANDLE_EXCEPTION()
      }
    bx_set_CF(temp_CF);
    }
  else { /* 16 bit operand size mode */
    Bit16u op1_16, temp_CF;
    Bit8u  op2_8;


    op2_8 = bx_fetch_next_byte();
    op2_8 %= 16;
    BX_HANDLE_EXCEPTION()

    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    temp_CF = (op1_16 >> op2_8) & 0x01;
    op1_16 &= ~(((Bit16u) 1) << op2_8);

    /* now write diff back to destination */
    if (op1_type == BX_REGISTER_REF) {
      BX_WRITE_16BIT_REG(op1_addr, op1_16);
      }
    else {
      bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }
    bx_set_CF(temp_CF);
    }
#endif
}
