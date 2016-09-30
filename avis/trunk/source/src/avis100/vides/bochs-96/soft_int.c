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
#include "instr_proto.h"





  INLINE void
bx_BOUND_GvMa()
{
#if BX_CPU < 2
  bx_panic("BOUND_GvMa: not supported on 8086!\n");
#else
  Bit32u op2_addr;
  Bit16s op1_16;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;
  Bit16s bound_min, bound_max;

  if (bx_v8086_mode()) bx_panic("soft_int: v8086 mode unsupported\n");

#if BX_CPU >= 3
  bx_panic("BOUND_GvMa: not completed for >80286!\n");
#endif

bx_panic("bound: bombing\n");
  if (bx_protected_mode())
    bx_panic("BOUND_GvMa(protected mode): not supported yet!\n");

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  op1_16 = BX_READ_16BIT_REG(op1_addr);

  /* op1 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    /* undefined opcode exception */
    bx_panic("bound: op2 must be mem ref\n");
    bx_exception(6, 0, 0);
    return;
    }
  else {
    bx_read_virtual_word(op2_seg_reg, op2_addr, &bound_min);
    BX_HANDLE_EXCEPTION()

    bx_read_virtual_word(op2_seg_reg, op2_addr+2, &bound_max);
    BX_HANDLE_EXCEPTION()

    /* ??? */
    if ( (op1_16 < bound_min) || (op1_16 > (bound_max+2)) ) {
      bx_printf("BOUND: fails bounds test\n");
      bx_exception(5, 0, 0);
      return;
      }
    }

  bx_printf("BOUND_GvMa: beta!\n");
#endif
}

  INLINE void
bx_INT3()
{
  /* INT 3 is not IOPL sensitive */
  bx_interrupt(3, 1, 0);
}

  INLINE void
bx_INT_Ib()
{
  Bit8u imm8;

  imm8 = bx_fetch_next_byte();
  BX_HANDLE_EXCEPTION()

  if (bx_v8086_mode() && IOPL<3) {
    bx_printf("int_ib: v8086: IOPL<3\n");
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  bx_interrupt(imm8, 1, 0);
}

  INLINE void
bx_INTO()
{
  /* ??? is this IOPL sensitive ? */
  if (bx_v8086_mode()) bx_panic("soft_int: v8086 mode unsupported\n");

  if (bx_get_OF()) {
    bx_exception(4, 0, 1);
    }
}
