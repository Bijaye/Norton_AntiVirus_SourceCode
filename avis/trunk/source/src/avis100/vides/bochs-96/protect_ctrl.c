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
bx_ARPL_EwGw()
{
#if BX_CPU < 2
  bx_panic("ARPL_EwRw: not supported on 8086!\n");
#else /* 286+ */
  Bit32u op1_addr;
  unsigned op1_type, op2_addr;
  bx_segment_reg_t *op1_seg_reg;

  Bit16u op2_16, op1_16;


  bx_decode_exgx(&op2_addr, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (bx_protected_mode()) {
    /* op1_16 is a register or memory reference */
    if (op1_type == BX_REGISTER_REF) {
      op1_16 = BX_READ_16BIT_REG(op1_addr);
      }
    else {
      /* pointer, segment address pair */
      bx_read_virtual_word(op1_seg_reg, op1_addr, &op1_16);
      BX_HANDLE_EXCEPTION()
      }

    /* op2_16 is a register, op2_addr is an index of a register */
    op2_16 = BX_READ_16BIT_REG(op2_addr);

    if ( (op1_16 & 0x03) < (op2_16 & 0x03) ) {
      op1_16 = (op1_16 & 0xfffc) | (op2_16 & 0x03);
      /* now write sum back to destination */
      if (op1_type == BX_REGISTER_REF) {
        BX_WRITE_16BIT_REG(op1_addr, op1_16);
        }
      else {
        bx_write_virtual_word(op1_seg_reg, op1_addr, &op1_16);
        BX_HANDLE_EXCEPTION()
        }
      bx_set_ZF(1);
      }
    else {
      bx_set_ZF(0);
      }
    }
  else {
    bx_printf("ARPL_EwRw: not recognized in real mode\n");
    bx_exception(6, 0, 0);
    return;
    }
#endif
}

 INLINE void
bx_LAR_GvEw()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  /* for 16 bit operand size mode */
  Bit16u raw_selector;
  bx_descriptor_t descriptor;
  bx_selector_t   selector;
  Bit32u dword1, dword2;


  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  if (bx_real_mode()) {
    bx_panic("LAR_GvEw: not recognized in real mode\n");
    bx_exception(6, 0, 0);
    return;
    }

  if (bx_dbg.protected)
    bx_printf("LAR_GvEw: entering\n");

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2_16 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    raw_selector = BX_READ_16BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_word(op2_seg_reg, op2_addr, &raw_selector);
    BX_HANDLE_EXCEPTION()
    }

  /* if selector null, clear ZF and done */
  if ( (raw_selector & 0xfffc) == 0 ) {
    bx_set_ZF(0);
    return;
    }

  bx_parse_selector(raw_selector, &selector);

  if ( !bx_fetch_raw_descriptor2(&selector, &dword1, &dword2) ) {
    /* not within descriptor table */
    bx_set_ZF(0);
    return;
    }

  bx_parse_descriptor(dword1, dword2, &descriptor);

  if (descriptor.valid==0) {
    bx_set_ZF(0);
    bx_printf("lar(): descriptor valid bit cleared\n");
    return;
    }

/* ??? */
#if 0
  if ( descriptor.p==0 ) {
    bx_set_ZF(0);
    bx_panic("lar(): descriptor not present\n");
    return;
    }
#endif

  /* if source selector is visible at CPL & RPL,
   * within the descriptor table, and of type accepted by LAR instruction,
   * then load register with segment limit and set ZF
   */

  if ( descriptor.segment ) { /* normal segment */
    if ( descriptor.u.segment.executable && descriptor.u.segment.c_ed ) {
      /* ignore DPL for conforming segments */
      }
    else {
      if ( (descriptor.dpl<CPL) || (descriptor.dpl<selector.rpl) ) {
        bx_set_ZF(0);
        return;
        }
      }
    bx_set_ZF(1);
    if (bx_cpu.is_32bit_opsize) {
      /* masked by 00FxFF00, where x is undefined */
      BX_WRITE_32BIT_REG(op1_addr, dword2 & 0x00ffff00);
      }
    else {
      BX_WRITE_16BIT_REG(op1_addr, dword2 & 0xff00);
      }
    return;
    }
  else { /* system or gate segment */
    switch ( descriptor.type ) {
      case 1: /* available TSS */
      case 2: /* LDT */
      case 3: /* busy TSS */
      case 4: /* 286 call gate */
      case 5: /* task gate */
#if BX_CPU >= 3
      case 9:  /* available 32bit TSS */
      case 11: /* busy 32bit TSS */
      case 12: /* 32bit call gate */
#endif
        break;
      default: /* rest not accepted types to LAR */
        bx_set_ZF(0);
        bx_printf("lar(): not accepted type\n");
        return;
        break;
      }

    if ( (descriptor.dpl<CPL) || (descriptor.dpl<selector.rpl) ) {
      bx_set_ZF(0);
      return;
      }
    bx_set_ZF(1);
    if (bx_cpu.is_32bit_opsize) {
      /* masked by 00FxFF00, where x is undefined ??? */
      BX_WRITE_32BIT_REG(op1_addr, dword2 & 0x00ffff00);
      }
    else {
      BX_WRITE_16BIT_REG(op1_addr, dword2 & 0xff00);
      }
    return;
    }
}

 INLINE void
bx_LSL_GvEw()
{
  Bit32u op2_addr;
  unsigned op1_addr, op2_type;
  bx_segment_reg_t *op2_seg_reg;

  /* for 16 bit operand size mode */
  Bit16u raw_selector;
  Bit32u limit32;
  bx_descriptor_t descriptor;
  bx_selector_t   selector;
  Bit32u dword1, dword2;

  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");


  if (bx_real_mode()) {
    bx_panic("LSL_GvEw: not recognized in real mode\n");
    bx_exception(6, 0, 0);
    return;
    }

  if (bx_dbg.protected)
    bx_printf("LSL_GvEw: entering\n");

  bx_decode_exgx(&op1_addr, &op2_addr, &op2_type, &op2_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op2_16 is a register or memory reference */
  if (op2_type == BX_REGISTER_REF) {
    raw_selector = BX_READ_16BIT_REG(op2_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_word(op2_seg_reg, op2_addr, &raw_selector);
    BX_HANDLE_EXCEPTION()
    }

  /* if selector null, clear ZF and done */
  if ( (raw_selector & 0xfffc) == 0 ) {
    bx_set_ZF(0);
    return;
    }

  bx_parse_selector(raw_selector, &selector);

  if ( !bx_fetch_raw_descriptor2(&selector, &dword1, &dword2) ) {
    /* not within descriptor table */
    bx_set_ZF(0);
    return;
    }

  bx_parse_descriptor(dword1, dword2, &descriptor);

  if ( descriptor.valid==0 ) {
    /* not valid */
    bx_set_ZF(0);
    bx_printf("LSL: descriptor valid bit cleared\n");
    return;
    }

/* ??? */
#if 0
  if ( descriptor.p==0 ) {
    /* not present */
    bx_set_ZF(0);
    bx_panic("LSL: descriptor not present\n");
    return;
    }
#endif

  /* if source selector is visible at CPL & RPL,
   * within the descriptor table, and of type accepted by LSL instruction,
   * then load register with segment limit and set ZF
   */

  if ( descriptor.segment ) { /* normal segment */
    if ( descriptor.u.segment.executable && descriptor.u.segment.c_ed ) {
      /* ignore DPL for conforming segments */
      limit32 = descriptor.u.segment.limit_scaled;
      }
    else {
      if ( (descriptor.dpl<CPL) || (descriptor.dpl<selector.rpl) ) {
        bx_set_ZF(0);
        return;
        }
      limit32 = descriptor.u.segment.limit_scaled;
      }
    }
  else { /* system or gate segment */
    switch ( descriptor.type ) {
      case 1: /* available TSS */
      case 3: /* busy TSS */
        limit32 = descriptor.u.tss286.limit;
        break;
      case 2: /* LDT */
        limit32 = descriptor.u.ldt.limit;
        break;
#if BX_CPU >= 3
      case 9:  /* available 32bit TSS */
      case 11: /* busy 32bit TSS */
        limit32 = descriptor.u.tss386.limit;
        if (descriptor.u.tss386.g)
          limit32 = (limit32 << 12) | 0x00000fff;
        break;
#endif
      default: /* rest not accepted types to LSL */
        bx_set_ZF(0);
        bx_panic("lsl(): not accepted type\n");
        return;
        break;
      }
    
    if ( (descriptor.dpl<CPL) || (descriptor.dpl<selector.rpl) ) {
      bx_set_ZF(0);
      return;
      }
    }

  /* all checks pass, limit32 is now byte granular, write to op1 */
  bx_set_ZF(1);
  if (bx_cpu.is_32bit_opsize)
    BX_WRITE_32BIT_REG(op1_addr, limit32)
  else
    BX_WRITE_16BIT_REG(op1_addr, (Bit16u) limit32) /* chop off upper 16 bits */
  return;
}

 INLINE void
bx_SLDT_Ew()
{
#if BX_CPU < 2
  bx_panic("SLDT_Ew: not supported on 8086!\n");
#else
  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  if (bx_dbg.protected) bx_printf("sldt_ew():\n");

  if (bx_real_mode()) {
    /* not recognized in real address mode */
    bx_panic("SLDT_Ew: encountered in real mode.\n");
    bx_exception(6, 0, 0);
    }
  else {
    bx_panic("SLDT_Ew: not supported in protected mode!\n");
    }
#endif
}

 INLINE void
bx_STR_Ew()
{
  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  bx_panic("STR_Ew: not supported yet!\n");
}

 INLINE void
bx_LLDT_Ew()
{
#if BX_CPU < 2
  bx_panic("LLDT_Ew: not supported on 8086!\n");
#else
  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");


  if (bx_real_mode()) {
    bx_panic("lldt: not recognized in real mode\n");
    bx_exception(6, 0, 0); /* not recognized in real mode */
    return;
    }
  else { /* protected mode */
    Bit32u mem_addr;
    unsigned  unused, mem_type;
    bx_segment_reg_t *mem_seg_reg;
    bx_descriptor_t  descriptor;
    bx_selector_t    selector;
    Bit16u raw_selector;
    Bit32u dword1, dword2;

    if (bx_dbg.protected) bx_printf("lldt_ew():\n");

    bx_decode_exgx(&unused, &mem_addr, &mem_type, &mem_seg_reg);
    BX_HANDLE_EXCEPTION()

    /* #GP(0) if the current privilege level is not 0 */
    if (CPL != 0) {
      bx_panic("LLDT: CPL != 0\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    if (mem_type == BX_REGISTER_REF) {
      raw_selector = BX_READ_16BIT_REG(mem_addr);
      }
    else {
      bx_read_virtual_word(mem_seg_reg, mem_addr, &raw_selector);
      BX_HANDLE_EXCEPTION()
      }

    /* if selector is NULL, invalidate and done */
    if ((raw_selector & 0xfffc) == 0) {
      bx_printf("lldt: case selector is null, OK.\n");
      bx_cpu.ldtr.selector.value = 0;
      bx_cpu.ldtr.cache.valid = 0;
      return;
      }

    /* parse fields in selector */
    bx_parse_selector(raw_selector, &selector);

    /* #GP(0) if the selector operand does not point into GDT */
    if (selector.ti != 0) {
      bx_panic("LLDT: selector.ti != 0\n");
      bx_exception(BX_GP_EXCEPTION, raw_selector & 0xfffc, 0);
      return;
      }

    if ((selector.index*8 + 7) > bx_cpu.gdtr.limit) {
      bx_panic("lldt: GDT: index > limit\n");
      bx_exception(BX_GP_EXCEPTION, raw_selector & 0xfffc, 0);
      return;
      }

    bx_access_linear(bx_cpu.gdtr.base + selector.index*8,     4, 0,
      BX_READ, &dword1);
    bx_access_linear(bx_cpu.gdtr.base + selector.index*8 + 4, 4, 0,
      BX_READ, &dword2);

    bx_parse_descriptor(dword1, dword2, &descriptor);

    /* if selector doesn't point to an LDT descriptor #GP(selector) */
    if (descriptor.valid==0 ||
        descriptor.segment  ||
        descriptor.type!=2) {
      bx_panic("lldt: doesn't point to an LDT descriptor!\n");
      bx_exception(BX_GP_EXCEPTION, raw_selector & 0xfffc, 0);
      return;
      }

    /* #NP(selector) if LDT descriptor is not present */
    if (descriptor.p==0) {
      bx_panic("lldt: LDT descriptor not present!\n");
      bx_exception(BX_NP_EXCEPTION, raw_selector & 0xfffc, 0);
      return;
      }

    if (descriptor.u.ldt.limit < 7) {
      bx_panic("load_ldtr(): ldtr.limit < 7\n");
      }

    bx_cpu.ldtr.selector = selector;
    bx_cpu.ldtr.cache = descriptor;
    bx_cpu.ldtr.cache.valid = 1;

    if (bx_dbg.protected)
      bx_printf("LLDT: (Beta) succeeding.\n");
    return;
    }
#endif
}

 INLINE void
bx_LTR_Ew()
{
#if BX_CPU < 2
  bx_panic("LTR_Ew: not supported on 8086!\n");
#else
  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");


  if (bx_protected_mode()) {
    Bit32u mem_addr;
    unsigned unused, mem_type;
    bx_segment_reg_t *mem_seg_reg;
    bx_descriptor_t  descriptor;
    bx_selector_t    selector;
    Bit16u raw_selector;
    Bit32u dword1, dword2;


    if (bx_dbg.protected) bx_printf("ltr_ew():\n");

    bx_decode_exgx(&unused, &mem_addr, &mem_type, &mem_seg_reg);
    BX_HANDLE_EXCEPTION()

    /* #GP(0) if the current privilege level is not 0 */
    if (CPL != 0) {
      bx_panic("LTR: CPL != 0\n");
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }

    if (mem_type == BX_REGISTER_REF) {
      raw_selector = BX_READ_16BIT_REG(mem_addr);
      }
    else {
      bx_read_virtual_word(mem_seg_reg, mem_addr, &raw_selector);
      BX_HANDLE_EXCEPTION()
      }

    /* if selector is NULL, invalidate and done */
    if ((raw_selector & 0xfffc) == 0) {
      bx_panic("ltr: loading with NULL selector!\n");
      /* if this is OK, then invalidate and load selector & descriptor cache */
      /* load here */
      return;
      }

    /* parse fields in selector, then check for null selector */
    bx_parse_selector(raw_selector, &selector);

    if (selector.ti) {
      bx_panic("ltr: selector.ti != 0\n");
      return;
      }

    /* fetch 2 dwords of descriptor; call handles out of limits checks */
    bx_fetch_raw_descriptor(&selector, &dword1, &dword2, BX_GP_EXCEPTION);
    BX_HANDLE_EXCEPTION()

    bx_parse_descriptor(dword1, dword2, &descriptor);

    /* #GP(selector) if object is not a TSS or is already busy */
    if ( (descriptor.valid==0) || descriptor.segment  ||
         (descriptor.type!=1 && descriptor.type!=9) ) {
      bx_panic("ltr: doesn't point to an available TSS descriptor!\n");
      bx_exception(BX_GP_EXCEPTION, raw_selector & 0xfffc, 0); /* 0 ??? */
      return;
      }

    /* #NP(selector) if TSS descriptor is not present */
    if (descriptor.p==0) {
      bx_panic("ltr: LDT descriptor not present!\n");
      bx_exception(BX_NP_EXCEPTION, raw_selector & 0xfffc, 0); /* 0 ??? */
      return;
      }

    if (descriptor.type==1 && descriptor.u.tss286.limit<43) {
      bx_panic("ltr:286TSS: loading tr.limit < 43\n");
      }
    else if (descriptor.type==9 && descriptor.u.tss386.limit<103) {
      bx_panic("ltr:386TSS: loading tr.limit < 103\n");
      }

    bx_cpu.tr.selector = selector;
    bx_cpu.tr.cache    = descriptor;
    bx_cpu.tr.cache.valid = 1;

    /* mark as busy */
    dword2 |= 0x00000200; /* set busy bit */
    bx_access_linear(bx_cpu.gdtr.base + selector.index*8 + 4, 4, 0,
      BX_WRITE, &dword2);

    if (bx_dbg.protected) {
      bx_printf("LTR_Ew: (Beta) succeeding.\n");
      }

    return;
    }
  else {
    bx_panic("ltr_ew: not recognized in real-mode!\n");
    bx_exception(6, 0, 0);
    return;
    }
#endif
}

 INLINE void
bx_VERR_Ew()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  /* for 16 bit operand size mode */
  Bit16u raw_selector;
  bx_descriptor_t descriptor;
  bx_selector_t   selector;
  Bit32u dword1, dword2;

  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");


  if (bx_real_mode()) {
    bx_panic("VERR_Ew: not recognized in real mode\n");
    bx_exception(6, 0, 0);
    return;
    }

  if (bx_dbg.protected)
    bx_printf("VERR_Ew: entering\n");

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1_16 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    raw_selector = BX_READ_16BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_word(op1_seg_reg, op1_addr, &raw_selector);
    BX_HANDLE_EXCEPTION()
    }

  /* if selector null, clear ZF and done */
  if ( (raw_selector & 0xfffc) == 0 ) {
    bx_set_ZF(0);
    bx_printf("VERR: null selector\n");
    return;
    }

  /* if source selector is visible at CPL & RPL,
   * within the descriptor table, and of type accepted by VERR instruction,
   * then load register with segment limit and set ZF */
  bx_parse_selector(raw_selector, &selector);

  if ( !bx_fetch_raw_descriptor2(&selector, &dword1, &dword2) ) {
    /* not within descriptor table */
    bx_set_ZF(0);
    bx_printf("VERR: not in table\n");
    return;
    }

  bx_parse_descriptor(dword1, dword2, &descriptor);

  if ( descriptor.segment==0 ) { /* system or gate descriptor */
    bx_set_ZF(0); /* inaccessible */
    bx_printf("VERR: system descriptor\n");
    return;
    }

  if ( descriptor.valid==0 ) {
    bx_set_ZF(0);
    bx_printf("VERR: valid bit cleared\n");
    return;
    }

/* ??? */
#if 0
  if ( descriptor.p==0 ) { /* if not present */
    bx_set_ZF(0); /* inaccessible */
    bx_printf("VERR: not present\n");
    return;
    }
#endif

  /* normal data/code segment */
  if ( descriptor.u.segment.executable ) { /* code segment */
    /* ignore DPL for readable conforming segments */
    if ( descriptor.u.segment.c_ed &&
         descriptor.u.segment.r_w) {
      bx_set_ZF(1); /* accessible */
      bx_printf("VERR: conforming code, OK\n");
      return;
      }
    if ( descriptor.u.segment.r_w==0 ) {
      bx_set_ZF(0); /* inaccessible */
      bx_printf("VERR: code not readable\n");
      return;
      }
    /* readable, non-conforming code segment */
    if ( (descriptor.dpl<CPL) || (descriptor.dpl<selector.rpl) ) {
      bx_set_ZF(0); /* inaccessible */
      bx_printf("VERR: non-coforming code not withing priv level\n");
      return;
      }
    bx_set_ZF(1); /* accessible */
    bx_printf("VERR: code seg readable\n");
    return;
    }
  else { /* data segment */
    if ( (descriptor.dpl<CPL) || (descriptor.dpl<selector.rpl) ) {
      bx_set_ZF(0); /* not accessible */
      bx_printf("VERR: data seg not withing priv level\n");
      return;
      }
    bx_set_ZF(1); /* accessible */
    bx_printf("VERR: data segment OK\n");
    return;
    }
}

 INLINE void
bx_VERW_Ew()
{
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  /* for 16 bit operand size mode */
  Bit16u raw_selector;
  bx_descriptor_t descriptor;
  bx_selector_t   selector;
  Bit32u dword1, dword2;

  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");


  if (bx_real_mode()) {
    bx_panic("VERW_Ew: not recognized in real mode\n");
    bx_exception(6, 0, 0);
    return;
    }

  if (bx_dbg.protected)
    bx_printf("VERW_Ew: entering\n");

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1_16 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    raw_selector = BX_READ_16BIT_REG(op1_addr);
    }
  else {
    /* pointer, segment address pair */
    bx_read_virtual_word(op1_seg_reg, op1_addr, &raw_selector);
    BX_HANDLE_EXCEPTION()
    }

  /* if selector null, clear ZF and done */
  if ( (raw_selector & 0xfffc) == 0 ) {
    bx_set_ZF(0);
    bx_printf("VERW: null selector\n");
    return;
    }

  /* if source selector is visible at CPL & RPL,
   * within the descriptor table, and of type accepted by VERW instruction,
   * then load register with segment limit and set ZF */
  bx_parse_selector(raw_selector, &selector);

  if ( !bx_fetch_raw_descriptor2(&selector, &dword1, &dword2) ) {
    /* not within descriptor table */
    bx_set_ZF(0);
    bx_printf("VERW: not in table\n");
    return;
    }

  bx_parse_descriptor(dword1, dword2, &descriptor);

  /* rule out system segments & code segments */
  if ( descriptor.segment==0 || descriptor.u.segment.executable ) {
    bx_set_ZF(0);
    bx_printf("VERW: system seg or code\n");
    return;
    }

  if ( descriptor.valid==0 ) {
    bx_set_ZF(0);
    bx_printf("VERW: valid bit cleared\n");
    return;
    }

/* ??? */
#if 0
  if ( descriptor.p==0 ) { /* if not present */
    bx_set_ZF(0); /* inaccessible */
    bx_printf("VERW: not present\n");
    return;
    }
#endif

  /* data segment */
  if ( descriptor.u.segment.r_w ) { /* writable */
    if ( (descriptor.dpl<CPL) || (descriptor.dpl<selector.rpl) ) {
      bx_set_ZF(0); /* not accessible */
      bx_printf("VERW: writable data seg not within priv level\n");
      return;
      }
    bx_set_ZF(1); /* accessible */
    bx_printf("VERW: data seg writable\n");
    return;
    }

  bx_set_ZF(0); /* not accessible */
  bx_printf("VERW: data seg not writable\n");
  return;
}

 INLINE void
bx_SGDT_Ms()
{
#if BX_CPU < 2
  bx_panic("SGDT_Ms: not supported on 8086!\n");
#else
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;
  Bit16u limit_16;
  Bit32u base_32;

  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");


  if (bx_dbg.protected) bx_printf("sgdt_ms():\n");

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    /* undefined opcode exception */
    bx_panic("SGDT_Ms: use of register is undefined opcode.\n");
    bx_exception(6, 0, 0);
    return;
    }

  limit_16 = bx_cpu.gdtr.limit;
  base_32  = bx_cpu.gdtr.base;
#if BX_CPU == 2
  base_32 |= 0xff000000; /* ??? */
#else /* 386+ */
  /* 32bit processors always write 32bits of base */
#endif
  bx_write_virtual_word(op1_seg_reg, op1_addr, &limit_16);
  BX_HANDLE_EXCEPTION()

  bx_write_virtual_dword(op1_seg_reg, op1_addr+2, &base_32);
  BX_HANDLE_EXCEPTION()

  if (bx_dbg.protected)
    bx_printf("SGDT_Ms(real mode): successful\n");
#endif
}

 INLINE void
bx_SIDT_Ms()
{
#if BX_CPU < 2
  bx_panic("SIDT_Ms: not supported on 8086!\n");
#else
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  Bit16u limit_16;
  Bit32u base_32;

  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  if (bx_dbg.protected) bx_printf("sidt_ms():\n");

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    /* undefined opcode exception */
    bx_panic("SIDT: use of register is undefined opcode.\n");
    bx_exception(6, 0, 0);
    return;
    }

  limit_16 = bx_cpu.idtr.limit;
  base_32  = bx_cpu.idtr.base;

#if BX_CPU == 2
  base_32 |= 0xff000000;
#else /* 386+ */
  /* ??? regardless of operand size, all 32bits of base are stored */
#endif

  bx_write_virtual_word(op1_seg_reg, op1_addr, &limit_16);
  BX_HANDLE_EXCEPTION()

  bx_write_virtual_dword(op1_seg_reg, op1_addr+2, &base_32);
  BX_HANDLE_EXCEPTION()

  if (bx_dbg.protected)
    bx_printf("SIDT_Ms(real mode): successful\n");
#endif
}

 INLINE void
bx_LGDT_Ms()
{
#if BX_CPU < 2
  bx_panic("LGDT_Ms: not supported on 8086!\n");
#else
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;

  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  if (bx_dbg.protected) bx_printf("lgdt_ms():\n");

  if (bx_protected_mode() && (CPL!=0)) {
    bx_panic("LGDT: protected mode: CPL!=0\n");
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    bx_panic("LGDT generating exception 6\n");
    bx_exception(6, 0, 0);
    return;
    }

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize) {
    Bit16u limit_16;
    Bit32u base0_31;

    bx_read_virtual_word(op1_seg_reg, op1_addr, &limit_16);
    BX_HANDLE_EXCEPTION()

    bx_read_virtual_dword(op1_seg_reg, op1_addr + 2, &base0_31);
    BX_HANDLE_EXCEPTION()

    bx_cpu.gdtr.limit = limit_16;
    bx_cpu.gdtr.base = base0_31;
    if (bx_dbg.protected)
      bx_printf("LGDT_Ms: successful\n");
    }
  else
#endif
    {
    Bit16u limit_16, base0_15;
    Bit8u base16_23;

    bx_read_virtual_word(op1_seg_reg, op1_addr, &limit_16);
    BX_HANDLE_EXCEPTION()

    bx_read_virtual_word(op1_seg_reg, op1_addr + 2, &base0_15);
    BX_HANDLE_EXCEPTION()

    bx_read_virtual_byte(op1_seg_reg, op1_addr + 4, &base16_23);
    BX_HANDLE_EXCEPTION()

    /* ignore high 8 bits */

    bx_cpu.gdtr.limit = limit_16;
    bx_cpu.gdtr.base = (base16_23 << 16) | base0_15;
    if (bx_dbg.protected)
      bx_printf("LGDT_Ms: successful\n");
    }
#endif
}

 INLINE void
bx_LIDT_Ms()
{
#if BX_CPU < 2
  bx_panic("LIDT_Ms: not supported on 8086!\n");
#else
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;
  Bit16u limit_16;
  Bit32u base_32;


  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  if (bx_dbg.protected) bx_printf("lidt_ms():\n");

  if (bx_protected_mode()) {
    if (CPL != 0) {
      bx_panic("LIDT(): CPL(%u) != 0\n", (unsigned) CPL);
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

  /* op1 is a register or memory reference */
  if (op1_type == BX_REGISTER_REF) {
    /* undefined opcode exception */
    bx_panic("LIDT generating exception 6\n");
    bx_exception(6, 0, 0);
    return;
    }
  
  bx_read_virtual_word(op1_seg_reg, op1_addr, &limit_16);
  BX_HANDLE_EXCEPTION()

  bx_read_virtual_dword(op1_seg_reg, op1_addr + 2, &base_32);
  BX_HANDLE_EXCEPTION()

  bx_cpu.idtr.limit = limit_16;

#if BX_CPU >= 3
  if (bx_cpu.is_32bit_opsize)
    bx_cpu.idtr.base = base_32;
  else
#endif
    bx_cpu.idtr.base = base_32 & 0x00ffffff; /* ignore upper 8 bits */

  if (bx_dbg.protected) {
    bx_printf("LIDT: idtr.limit = %04x\n", (unsigned) bx_cpu.idtr.limit);
    bx_printf("LIDT: idtr.base  = %06x\n", (unsigned) bx_cpu.idtr.base);
    }

#endif
}

  void
bx_enter_protected_mode(void)
{
  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  if (bx_dbg.reset || bx_dbg.protected)
    bx_printf("processor switching into PROTECTED mode!!!\n");
  bx_cpu.cr0.pe = 1;

#if 0
/* ??? */
  /* Execution in protected mode begins with a CPL of 0 */
  bx_cpu.cs.selector.rpl = 0;
  bx_cpu.ss.selector.rpl = 0;
#endif
if ( bx_cpu.cs.selector.rpl!=0 || bx_cpu.ss.selector.rpl!=0 )
  bx_panic("enter_protected_mode: CS or SS rpl != 0\n");

#if 0
  /* ??? */
  if (bx_cpu.eflags.iopl != 0)
    bx_printf("enter_protected_mode: IOPL(%u) not 0\n",
      (unsigned) IOPL);
#endif
}

  void
bx_enter_real_mode(void)
{
  if (bx_v8086_mode()) bx_panic("protect_ctrl: v8086 mode unsupported\n");

  if (bx_dbg.reset || bx_dbg.protected)
    bx_printf("processor switching into REAL mode!!!\n");
  bx_cpu.cr0.pe = 0;

#if 0
  bx_cpu.cs.selector.rpl = 0;
  /* ??? other values set here.  Look at _Protected Mode Basics_ article */
#endif
if ( bx_cpu.cs.selector.rpl!=0 || bx_cpu.ss.selector.rpl!=0 )
  bx_panic("enter_real_mode: CS or SS rpl != 0\n");
}
