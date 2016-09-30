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




#pragma warning(disable:4761) // 'integral size mismatch; conversion supplied'
#pragma warning(disable:4244) // 'conversion from unsigned short to unsigned char'




#include "bochs.h"




  Bit16u
bx_read_segment_register(Bit32u index)
{
  switch (index) {
    case BX_SEG_REG_ES:
      return(bx_cpu.es.selector.value);
    case BX_SEG_REG_CS:
      return(bx_cpu.cs.selector.value);
    case BX_SEG_REG_SS:
      return(bx_cpu.ss.selector.value);
    case BX_SEG_REG_DS:
      return(bx_cpu.ds.selector.value);
#if BX_CPU > 2
    case BX_SEG_REG_FS:
      return(bx_cpu.fs.selector.value);
    case BX_SEG_REG_GS:
      return(bx_cpu.gs.selector.value);
#endif
    }
  bx_panic("bx_read_segment_register() undefined segment register!\n");
  return(0); /* keep compiler from complaining */
}

  void
bx_write_segment_register(Bit32u index, Bit16u value)
{
  switch (index) {
    case BX_SEG_REG_ES:
      bx_load_seg_reg(&bx_cpu.es, value);
      return;
    case BX_SEG_REG_CS:
      bx_load_seg_reg(&bx_cpu.cs, value);
      return;
    case BX_SEG_REG_SS:
      bx_load_seg_reg(&bx_cpu.ss, value);
      return;
    case BX_SEG_REG_DS:
      bx_load_seg_reg(&bx_cpu.ds, value);
      return;
#if BX_CPU >= 3
    case BX_SEG_REG_FS:
      bx_load_seg_reg(&bx_cpu.fs, value);
      return;
    case BX_SEG_REG_GS:
      bx_load_seg_reg(&bx_cpu.gs, value);
      return;
#endif
    }
  bx_panic("bx_write_segment_register() undefined segment register!\n");
}

  void
bx_load_seg_reg(bx_segment_reg_t *seg, Bit16u new_value)
{

#if BX_CPU >= 3
  if (bx_v8086_mode()) {
    /* ??? don't need to set all these fields */
    seg->selector.value = new_value;
    seg->selector.rpl = 3;
    seg->cache.valid = 1;
    seg->cache.p = 1;
    seg->cache.dpl = 3;
    seg->cache.segment = 1; /* regular segment */
    seg->cache.u.segment.executable = 0; /* data segment */
    seg->cache.u.segment.c_ed = 0; /* expand up */
    seg->cache.u.segment.r_w = 1; /* writeable */
    seg->cache.u.segment.a = 1; /* accessed */
    seg->cache.u.segment.base = new_value << 4;
    seg->cache.u.segment.limit        = 0xffff;
    seg->cache.u.segment.limit_scaled = 0xffff;
    seg->cache.u.segment.g     = 0; /* byte granular */
    seg->cache.u.segment.d_b   = 0; /* default 16bit size */
    seg->cache.u.segment.avl   = 0;

    if (seg == &bx_cpu.ss) {
      bx_cpu.inhibit_interrupts = 1;
      bx_async_event = 1;
      }
    return;
    }
#endif

#if BX_CPU >= 2
  if (bx_protected_mode()) {
    if (seg == &bx_cpu.ss) {
      Bit16u index;
      Bit16u ti;
      Bit8u rpl;
      bx_descriptor_t descriptor;
      Bit32u dword1, dword2;

      if ((new_value & 0xfffc) == 0) { /* null selector */
        bx_panic("load_seg_reg: SS: new_value == 0\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }

      index = new_value >> 3;
      ti = (new_value >> 2) & 0x01;
      rpl = (new_value & 0x03);

      /* examine AR byte of destination selector for legal values: */

      if (ti == 0) { /* GDT */
        if ((index*8 + 7) > bx_cpu.gdtr.limit) {
          bx_panic("load_seg_reg: GDT: %s: index(%04x) > limit(%06x)\n",
            bx_strseg(seg), (unsigned) index, (unsigned) bx_cpu.gdtr.limit);
          bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
          return;
          }
        bx_access_linear(bx_cpu.gdtr.base + index*8,     4, 0,
          BX_READ, &dword1);
        bx_access_linear(bx_cpu.gdtr.base + index*8 + 4, 4, 0,
          BX_READ, &dword2);
        }
      else { /* LDT */
        if (bx_cpu.ldtr.cache.valid==0) /* ??? */
          bx_panic("load_seg_reg: LDT invalid\n");
        if ((index*8 + 7) > bx_cpu.ldtr.cache.u.ldt.limit) {
          bx_panic("load_seg_reg: LDT: index > limit\n");
          bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
          return;
          }
        bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + index*8,     4, 0,
          BX_READ, &dword1);
        bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + index*8 + 4, 4, 0,
          BX_READ, &dword2);
        }

      /* selector's RPL must = CPL, else #GP(selector) */
      if (rpl != CPL) {
        bx_printf("load_seg_reg(): rpl != CPL\n");
        bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
        return;
        }

      bx_parse_descriptor(dword1, dword2, &descriptor);

      if (descriptor.valid==0) {
        bx_panic("load_seg_reg(): valid bit cleared\n");
        /* fault here */
        return;
        }

      /* AR byte must indicate a writable data segment else #GP(selector) */
      if ( (descriptor.segment==0) ||
           descriptor.u.segment.executable ||
           descriptor.u.segment.r_w==0 ) {
        bx_printf("load_seg_reg(): not writable data segment\n");
        bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
        return;
        }

      /* DPL in the AR byte must equal CPL else #GP(selector) */
      if (descriptor.dpl != CPL) {
        bx_panic("load_seg_reg(): dpl != CPL\n");
        bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
        return;
        }

      /* segment must be marked PRESENT else #SS(selector) */
      if (descriptor.p == 0) {
        bx_panic("load_seg_reg(): not present\n");
        bx_exception(BX_SS_EXCEPTION, new_value & 0xfffc, 0);
        return;
        }

      /* load SS with selector, load SS cache with descriptor */
      bx_cpu.ss.selector.value        = new_value;
      bx_cpu.ss.selector.index        = index;
      bx_cpu.ss.selector.ti           = ti;
      bx_cpu.ss.selector.rpl          = rpl;
      bx_cpu.ss.cache = descriptor;
      bx_cpu.ss.cache.valid             = 1;

      bx_cpu.inhibit_interrupts = 1;
      bx_async_event = 1;

      /* now set accessed bit in descriptor */
      dword2 |= 0x0100;
      if (ti == 0) { /* GDT */
        bx_access_linear(bx_cpu.gdtr.base + index*8 + 4, 4, 0,
          BX_WRITE, &dword2);
        }
      else { /* LDT */
        bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + index*8 + 4, 4, 0,
          BX_WRITE, &dword2);
        }

      /* instructions which modify SS don't generate single steps */
      bx_single_step_event = 0; /* ??? */
      if (bx_dbg.protected)
        bx_printf("load_seg_reg(): SS: succeeded!\n");
      return;
      }
    else if ( (seg==&bx_cpu.ds) || (seg==&bx_cpu.es) 
#if BX_CPU >= 3
              || (seg==&bx_cpu.fs) || (seg==&bx_cpu.gs)
#endif
            ) {
      Bit16u index;
      Bit16u ti;
      Bit8u rpl;
      bx_descriptor_t descriptor;
      Bit32u dword1, dword2;


      if ((new_value & 0xfffc) == 0) { /* null selector */
        if (bx_dbg.protected)
          bx_printf("load_seg_reg(): invalidating %s\n", bx_strseg(seg));
        seg->selector.index = 0;
        seg->selector.ti = 0;
        seg->selector.rpl = 0;
        seg->selector.value = 0;
        seg->cache.valid = 0; /* invalidate null selector */
        return;
        }

      index = new_value >> 3;
      ti = (new_value >> 2) & 0x01;
      rpl = (new_value & 0x03);

      /* selector index must be within descriptor limits, else #GP(selector) */

      if (ti == 0) { /* GDT */
        if ((index*8 + 7) > bx_cpu.gdtr.limit) {
          bx_printf("load_seg_reg: GDT: %s: index(%04x) > limit(%06x)\n",
            bx_strseg(seg), (unsigned) index, (unsigned) bx_cpu.gdtr.limit);
          bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
          return;
          }
        bx_access_linear(bx_cpu.gdtr.base + index*8,     4, 0,
          BX_READ, &dword1);
        bx_access_linear(bx_cpu.gdtr.base + index*8 + 4, 4, 0,
          BX_READ, &dword2);
        }
      else { /* LDT */
if (bx_cpu.ldtr.cache.valid==0) bx_panic("load_seg_reg: LDT invalid\n");
        if ((index*8 + 7) > bx_cpu.ldtr.cache.u.ldt.limit) {
          bx_panic("load_seg_reg: LDT: index > limit\n");
          bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
          return;
          }
        bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + index*8,     4, 0,
          BX_READ, &dword1);
        bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + index*8 + 4, 4, 0,
          BX_READ, &dword2);
        }

      bx_parse_descriptor(dword1, dword2, &descriptor);

      if (descriptor.valid==0) {
        bx_panic("load_seg_reg(): valid bit cleared\n");
        /* fault here */
        return;
        }

      /* AR byte must indicate data or readable code segment else #GP(selector) */
      if ( descriptor.segment==0 ||
           (descriptor.u.segment.executable==1 &&
            descriptor.u.segment.r_w==0) ) {
        bx_panic("load_seg_reg(): not data or readable code\n");
        bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
        return;
        }

      /* If data or non-conforming code, then both the RPL and the CPL
       * must be less than or equal to DPL in AR byte else #GP(selector) */
      if ( descriptor.u.segment.executable==0 ||
           descriptor.u.segment.c_ed==0 ) {
        if ((rpl > descriptor.dpl) || (CPL > descriptor.dpl)) {
bx_printf("rpl=%u cpl=%u dpl=%u\n",
  (unsigned)rpl, (unsigned) CPL, (unsigned) descriptor.dpl);
bx_printf("executable=%u\n", (unsigned) descriptor.u.segment.executable);
bx_printf("c_ed=%u\n", (unsigned) descriptor.u.segment.c_ed);
bx_printf("type=%u\n", (unsigned) descriptor.type);
bx_printf("base=%u\n", (unsigned) descriptor.u.segment.base);
bx_printf("limit=%u\n", (unsigned) descriptor.u.segment.limit);
          bx_panic("load_seg_reg: RPL & CPL must be <= DPL\n");
          bx_exception(BX_GP_EXCEPTION, new_value & 0xfffc, 0);
          return;
          }
        }

      /* segment must be marked PRESENT else #NP(selector) */
      if (descriptor.p == 0) {
        bx_printf("load_seg_reg: segment not present\n");
        bx_exception(BX_NP_EXCEPTION, new_value & 0xfffc, 0);
        return;
        }

      /* load segment register with selector */
      /* load segment register-cache with descriptor */
      seg->selector.value        = new_value;
      seg->selector.index        = index;
      seg->selector.ti           = ti;
      seg->selector.rpl          = rpl;
      seg->cache = descriptor;
      seg->cache.valid             = 1;
      if (bx_dbg.protected)
        bx_printf("load_seg_reg(): %s: succeeded!\n", bx_strseg(seg));

      /* now set accessed bit in descriptor */
      dword2 |= 0x0100;
      if (ti == 0) { /* GDT */
        bx_access_linear(bx_cpu.gdtr.base + index*8 + 4, 4, 0,
          BX_WRITE, &dword2);
        }
      else { /* LDT */
        bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + index*8 + 4, 4, 0,
          BX_WRITE, &dword2);
        }
      return;
      }
    else {
      bx_panic("load_seg_reg(): invalid segment register passed!\n");
      return;
      }
    }

  /* real mode */
  /* seg->limit = ; ??? different behaviours depening on seg reg. */
  /* something about honoring previous values */

  /* ??? */
  if (seg == &bx_cpu.cs) {
    bx_cpu.cs.selector.value = new_value;
    /* ??? set valid flag too */
    bx_cpu.cs.cache.p = 1;
    bx_cpu.cs.cache.dpl = 0;
    bx_cpu.cs.cache.segment = 1; /* regular segment */
    bx_cpu.cs.cache.u.segment.executable = 0; /* data segment */
    bx_cpu.cs.cache.u.segment.c_ed = 0; /* expand up */
    bx_cpu.cs.cache.u.segment.r_w = 1; /* writeable */
    bx_cpu.cs.cache.u.segment.a = 1; /* accessed */
    bx_cpu.cs.cache.u.segment.base = new_value << 4;
    bx_cpu.cs.cache.u.segment.limit        = 0xffff;
    bx_cpu.cs.cache.u.segment.limit_scaled = 0xffff;
#if BX_CPU >= 3
    bx_cpu.cs.cache.u.segment.g     = 0; /* byte granular */
    bx_cpu.cs.cache.u.segment.d_b   = 0; /* default 16bit size */
    bx_cpu.cs.cache.u.segment.avl   = 0;
#endif
    }
  else { /* SS, DS, ES, FS, GS */
    seg->selector.value = new_value;
    seg->cache.u.segment.base = new_value << 4;
    seg->cache.segment = 1; /* regular segment */
    seg->cache.u.segment.a = 1; /* accessed */
    /* set G, D_B, AVL bits here ??? */
    if (seg == &bx_cpu.ss) {
      bx_cpu.inhibit_interrupts = 1;
      bx_async_event = 1;
      }
    }
#else /* 8086 */

  seg->selector.value = new_value;
  seg->cache.u.segment.base = new_value << 4;
  if (seg == &bx_cpu.ss) {
    bx_cpu.inhibit_interrupts = 1;
      bx_async_event = 1;
    }
#endif
}



  INLINE void
bx_LES_GvMp()
{
  Bit32u mem_addr;
  unsigned reg_addr, mem_type;
  bx_segment_reg_t *mem_seg_reg;


  bx_decode_exgx(&reg_addr, &mem_addr, &mem_type, &mem_seg_reg);
  if (mem_type == BX_REGISTER_REF) {
    bx_panic("invalid use of LES, must use memory reference!\n");
    bx_exception(6, 0, 0);
    return;
    }
  
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit16u es;
    Bit32u reg_32;

    bx_read_virtual_dword(mem_seg_reg, mem_addr, &reg_32);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 4, &es);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.es, es);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_32BIT_REG(reg_addr, reg_32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit mode */
    Bit16u reg_16, es;

    bx_read_virtual_word(mem_seg_reg, mem_addr, &reg_16);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 2, &es);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.es, es);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_16BIT_REG(reg_addr, reg_16);
    }
}

  INLINE void
bx_LDS_GvMp()
{
  Bit32u mem_addr;
  unsigned reg_addr, mem_type;
  bx_segment_reg_t *mem_seg_reg;


  bx_decode_exgx(&reg_addr, &mem_addr, &mem_type, &mem_seg_reg);
  if (mem_type == BX_REGISTER_REF) {
    bx_panic("invalid use of LDS, must use memory reference!\n");
    bx_exception(6, 0, 0);
    return;
    }
  
#if BX_CPU > 2
  if (bx_cpu.is_32bit_opsize) {
    Bit16u ds;
    Bit32u reg_32;

    bx_read_virtual_dword(mem_seg_reg, mem_addr, &reg_32);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 4, &ds);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ds, ds);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_32BIT_REG(reg_addr, reg_32);
    }
  else
#endif /* BX_CPU > 2 */
    { /* 16 bit mode */
    Bit16u reg_16, ds;

    bx_read_virtual_word(mem_seg_reg, mem_addr, &reg_16);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 2, &ds);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ds, ds);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_16BIT_REG(reg_addr, reg_16);
    }
}

  void
bx_LFS_GvMp()
{
  Bit32u mem_addr;
  unsigned reg_addr, mem_type;
  bx_segment_reg_t *mem_seg_reg;


#if BX_CPU < 3
  bx_panic("lfs_gvmp: not supported on 8086\n");
#else /* 386+ */

  bx_decode_exgx(&reg_addr, &mem_addr, &mem_type, &mem_seg_reg);
  if (mem_type == BX_REGISTER_REF) {
    bx_panic("invalid use of LFS, must use memory reference!\n");
    bx_exception(6, 0, 0);
    return;
    }
  
  if (bx_cpu.is_32bit_opsize) {
    Bit32u reg_32;
    Bit16u fs;

    bx_read_virtual_dword(mem_seg_reg, mem_addr, &reg_32);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 4, &fs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.fs, fs);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_32BIT_REG(reg_addr, reg_32);
    }
  else { /* 16 bit operand size */
    Bit16u reg_16;
    Bit16u fs;

    bx_read_virtual_word(mem_seg_reg, mem_addr, &reg_16);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 2, &fs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.fs, fs);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_16BIT_REG(reg_addr, reg_16);
    }
#endif
}

  void
bx_LGS_GvMp()
{
  Bit32u mem_addr;
  unsigned reg_addr, mem_type;
  bx_segment_reg_t *mem_seg_reg;


#if BX_CPU < 3
  bx_panic("lgs_gvmp: not supported on 8086\n");
#else /* 386+ */

  bx_decode_exgx(&reg_addr, &mem_addr, &mem_type, &mem_seg_reg);
  if (mem_type == BX_REGISTER_REF) {
    bx_panic("invalid use of LGS, must use memory reference!\n");
    bx_exception(6, 0, 0);
    return;
    }
  
  if (bx_cpu.is_32bit_opsize) {
    Bit32u reg_32;
    Bit16u gs;

    bx_read_virtual_dword(mem_seg_reg, mem_addr, &reg_32);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 4, &gs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.gs, gs);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_32BIT_REG(reg_addr, reg_32);
    }
  else { /* 16 bit operand size */
    Bit16u reg_16;
    Bit16u gs;

    bx_read_virtual_word(mem_seg_reg, mem_addr, &reg_16);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 2, &gs);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.gs, gs);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_16BIT_REG(reg_addr, reg_16);
    }
#endif
}

  void
bx_LSS_GvMp()
{
  Bit32u mem_addr;
  unsigned reg_addr, mem_type;
  bx_segment_reg_t *mem_seg_reg;


#if BX_CPU < 3
  bx_panic("lss_gvmp: not supported on 8086\n");
#else /* 386+ */

  bx_decode_exgx(&reg_addr, &mem_addr, &mem_type, &mem_seg_reg);
  if (mem_type == BX_REGISTER_REF) {
    bx_panic("invalid use of LSS, must use memory reference!\n");
    bx_exception(6, 0, 0);
    return;
    }
  
  if (bx_cpu.is_32bit_opsize) {
    Bit32u reg_32;
    Bit16u ss;

    bx_read_virtual_dword(mem_seg_reg, mem_addr, &reg_32);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 4, &ss);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ss, ss);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_32BIT_REG(reg_addr, reg_32);
    }
  else { /* 16 bit operand size */
    Bit16u reg_16;
    Bit16u ss;

    bx_read_virtual_word(mem_seg_reg, mem_addr, &reg_16);
    BX_HANDLE_EXCEPTION()
    bx_read_virtual_word(mem_seg_reg, mem_addr + 2, &ss);
    BX_HANDLE_EXCEPTION()

    bx_load_seg_reg(&bx_cpu.ss, ss);
    BX_HANDLE_EXCEPTION()

    BX_WRITE_16BIT_REG(reg_addr, reg_16);
    }
#endif
}

#if BX_CPU >= 2
  void
bx_parse_selector(Bit16u raw_selector, bx_selector_t *selector)
{
  selector->value  = raw_selector;
  selector->index  = raw_selector >> 3;
  selector->ti     = (raw_selector >> 2) & 0x01;
  selector->rpl    = raw_selector & 0x03;
}
#endif

  void
bx_parse_descriptor(Bit32u dword1, Bit32u dword2, bx_descriptor_t *temp)
{
  Bit8u AR_byte;

  AR_byte        = dword2 >> 8;
  temp->p        = (AR_byte & 0x80);
  temp->dpl      = (AR_byte >> 5) & 0x03;
  temp->segment  = (AR_byte & 0x10);
  temp->type     = (AR_byte & 0x0f);
  temp->valid    = 0; /* start out invalid */


  if (temp->segment) { /* data/code segment descriptors */
    temp->u.segment.executable = (AR_byte & 0x08);
    temp->u.segment.c_ed       = (AR_byte & 0x04);
    temp->u.segment.r_w        = (AR_byte & 0x02);
    temp->u.segment.a          = (AR_byte & 0x01);

    temp->u.segment.limit      = (dword1 & 0xffff);
    temp->u.segment.base       = (dword1 >> 16) |
                                 ((dword2 & 0xFF) << 16);

#if BX_CPU >= 3
    temp->u.segment.limit        |= (dword2 & 0x000F0000);
    temp->u.segment.g            =  (dword2 & 0x00800000) > 0;
    temp->u.segment.d_b          =  (dword2 & 0x00400000) > 0;
    temp->u.segment.avl          =  (dword2 & 0x00100000) > 0;
    temp->u.segment.base         |= (dword2 & 0xFF000000);
    if (temp->u.segment.g)
      temp->u.segment.limit_scaled = (temp->u.segment.limit << 12) | 0x0fff;
    else
#endif
      temp->u.segment.limit_scaled = temp->u.segment.limit;

    temp->valid    = 1;
    }
  else { /* system & gate segment descriptors */
    switch ( temp->type ) {
      case 0: /* invalid */
        if (AR_byte == 0) { /* place-holder (NULL) */
          bx_printf("parse_descriptor(): found place-holder descriptor\n");
          }
        else {
          bx_printf("parse_descriptor(): invalid descriptor AR=%02x\n",
            (unsigned) AR_byte);
          }
        temp->valid    = 0;
        break;
      case 1: /* 286 available TSS */
        temp->u.tss286.base  = (dword1 >> 16) |
                               ((dword2 & 0xff) << 16);
        temp->u.tss286.limit = (dword1 & 0xffff);
        temp->valid    = 1;
        break;
      case 2: /* LDT descriptor */
        temp->u.ldt.base = (dword1 >> 16) |
                           ((dword2 & 0xFF) << 16);
#if BX_CPU >= 3
        temp->u.ldt.base |= (dword2 & 0xff000000);
#endif
        temp->u.ldt.limit = (dword1 & 0xffff);
        temp->valid    = 1;
        break;
      case 4: /* 286 call gate */
      case 5: /* 286/386 task gate */
      case 6: /* 286 interrupt gate */
      case 7: /* 286 trap gate */
        /* word count only used for call gate */
        temp->u.gate286.word_count = dword2 & 0x1f;
        temp->u.gate286.dest_selector = dword1 >> 16;;
        temp->u.gate286.dest_offset   = dword1 & 0xffff;
        temp->valid = 1;
        break;
#if BX_CPU >= 3
      case 9: /* 386 available TSS */
        temp->u.tss386.base  = (dword1 >> 16) |
                               ((dword2 & 0xff) << 16) |
                               (dword2 & 0xff000000);
        temp->u.tss386.limit = (dword1 & 0x0000ffff) |
                               (dword2 & 0x000f0000);
        temp->u.tss386.g     = (dword2 & 0x00800000) > 0;
        temp->u.tss386.avl   = (dword2 & 0x00100000) > 0;
        temp->valid = 1;
        break;
      case 12: /* 386 call gate */
      case 14: /* 386 interrupt gate */
      case 15: /* 386 trap gate */
        /* word count only used for call gate */
        temp->u.gate386.dword_count   = dword2 & 0x1f;
        temp->u.gate386.dest_selector = dword1 >> 16;;
        temp->u.gate386.dest_offset   = (dword2 & 0xffff0000) |
                                        (dword1 & 0x0000ffff);
        temp->valid = 1;
        break;
#endif
      default: bx_panic("parse_descriptor(): case %d unfinished\n",
                 (unsigned) temp->type);
        temp->valid    = 0;
      }
    }
}

  void
bx_load_ldtr(bx_selector_t *selector, bx_descriptor_t *descriptor)
{
  if (bx_dbg.protected) bx_printf("load_ldtr():\n");

  /* check for null selector, if so invalidate LDTR */
  if ( (selector->value & 0xfffc)==0 ) {
    bx_cpu.ldtr.selector = *selector;
    bx_cpu.ldtr.cache.valid = 0;
    if (bx_dbg.protected) {
      bx_printf("load_ldtr: ldtr.limit = %04x\n",
        (unsigned) bx_cpu.ldtr.cache.u.ldt.limit);
      bx_printf("load_ldtr(): invalidating LDTR.\n");
      }
    return;
    }

  if (!descriptor)
    bx_panic("load_ldtr(): descriptor == NULL!\n");

  bx_cpu.ldtr.cache = *descriptor; /* whole structure copy */
  bx_cpu.ldtr.selector = *selector;

  if (bx_dbg.protected)
    bx_printf("load_ldtr: ldtr.limit = %04x\n",
      (unsigned) bx_cpu.ldtr.cache.u.ldt.limit);

  if (bx_cpu.ldtr.cache.u.ldt.limit < 7) {
    bx_panic("load_ldtr(): ldtr.limit < 7\n");
    }

  bx_cpu.ldtr.cache.valid = 1;
}

  void
bx_load_cs(bx_selector_t *selector, bx_descriptor_t *descriptor,
           Bit8u cpl)
{
  if (bx_dbg.protected) bx_printf("load_cs():\n");

  bx_cpu.cs.selector     = *selector;
  bx_cpu.cs.cache        = *descriptor;

  /* caller may request different CPL then in selector */
  bx_cpu.cs.selector.rpl = cpl;
  bx_cpu.cs.cache.valid = 1; /* ??? */
/* ??? */
if ( bx_cpu.cs.selector.rpl != (selector->value & 0x03) )
  bx_panic("load_cs(): CS.rpl != CS.value & 0x03\n");

  if (bx_dbg.protected)
    bx_printf("load_cs(): setting CPL to %d\n",
      (unsigned) bx_cpu.cs.selector.rpl);
}

  void
bx_load_ss(bx_selector_t *selector, bx_descriptor_t *descriptor, Bit8u cpl)
{
  if (bx_dbg.protected) bx_printf("load_ss():\n");

  bx_cpu.ss.selector = *selector;
  bx_cpu.ss.cache = *descriptor;
  bx_cpu.ss.selector.rpl = cpl;

  if ( (bx_cpu.ss.selector.value & 0xfffc) == 0 )
    bx_panic("load_ss(): null selector passed\n");

  if ( !bx_cpu.ss.cache.valid ) {
    bx_panic("load_ss(): invalid selector/descriptor passed.\n");
    }

  if (bx_dbg.protected)
    bx_printf("load_ss(): setting ss.rpl to %d\n",
      (unsigned) bx_cpu.ss.selector.rpl);
}



#if BX_CPU >= 2
  void
bx_fetch_raw_descriptor(bx_selector_t *selector,
                        Bit32u *dword1, Bit32u *dword2, Bit8u exception)
{
  if (selector->ti == 0) { /* GDT */
    if ((selector->index*8 + 7) > bx_cpu.gdtr.limit) {
      bx_panic("fetch_raw_descriptor: GDT: index > limit\n");
      bx_exception(exception, selector->value & 0xfffc, 0);
      return;
      }
    bx_access_linear(bx_cpu.gdtr.base + selector->index*8,     4, 0,
      BX_READ, dword1);
    bx_access_linear(bx_cpu.gdtr.base + selector->index*8 + 4, 4, 0,
      BX_READ, dword2);
    }
  else { /* LDT */
    if (bx_cpu.ldtr.cache.valid==0) {
      bx_panic("fetch_raw_descriptor: LDTR.valid=0\n");
      }
    if ((selector->index*8 + 7) > bx_cpu.ldtr.cache.u.ldt.limit) {
      bx_panic("fetch_raw_descriptor: LDT: index > limit\n");
      bx_exception(exception, selector->value & 0xfffc, 0);
      return;
      }
    bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + selector->index*8,     4, 0,
      BX_READ, dword1);
    bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + selector->index*8 + 4, 4, 0,
      BX_READ, dword2);
    }
}
#endif





  Boolean
bx_fetch_raw_descriptor2(bx_selector_t *selector,
                        Bit32u *dword1, Bit32u *dword2)
{
  if (selector->ti == 0) { /* GDT */
    if ((selector->index*8 + 7) > bx_cpu.gdtr.limit)
      return(0);
    bx_access_linear(bx_cpu.gdtr.base + selector->index*8,     4, 0,
      BX_READ, dword1);
    bx_access_linear(bx_cpu.gdtr.base + selector->index*8 + 4, 4, 0,
      BX_READ, dword2);
    return(1);
    }
  else { /* LDT */
    if ((selector->index*8 + 7) > bx_cpu.ldtr.cache.u.ldt.limit)
      return(0);
    bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + selector->index*8,     4, 0,
      BX_READ, dword1);
    bx_access_linear(bx_cpu.ldtr.cache.u.ldt.base + selector->index*8 + 4, 4, 0,
      BX_READ, dword2);
    return(1);
    }
}


  void
bx_validate_seg_regs(void)
{
  /* For each of DS, ES, FS, GS: */
  /* if the current register is not valid for the outer level,
   * then zero the register and clear the valid flag.  To be
   * valid, the register setting must satisfy the following properties:
   *   selector index must be within descriptor table limits
   *   AR byte must indicte data or readable code segment
   * if segment is data or non-conforming code, then:
   *   (from various Intel books)
   *   (i286)    DPL must be >= CPL, or DPL must be >= RPL
   *   (i386)    DPL must be >= CPL, or DPL must be >= RPL
   *   (i486)    DPL must be > CPL,  or DPL must be < RPL
   *   (pentium) DPL must be > CPL,  or DPL must be < RPL
   *   ??? which one is correct?
   */


  /* I'm not sure about if the following checks are complete ??? */

  /* validate ES */
  if ( bx_cpu.es.cache.valid ) {
    /* selector index must be within descriptor table limits */
    if ( bx_cpu.es.selector.ti==0 ) {
      if ( (bx_cpu.es.selector.index*8+7) > bx_cpu.gdtr.limit )
        bx_cpu.es.cache.valid = 0;
      }
    else {
      if ( (bx_cpu.es.selector.index*8+7) > bx_cpu.ldtr.cache.u.ldt.limit )
        bx_cpu.es.cache.valid = 0;
      }

    /* AR byte must indicate data or readable code segment */
    if ( bx_cpu.es.cache.u.segment.executable &&
         bx_cpu.es.cache.u.segment.r_w==0 )
      bx_cpu.es.cache.valid = 0;

    /* if segment is data or non-conforming code, then
     * DPL must be >= CPL or CPL must be >= RPL */
    if ( bx_cpu.es.cache.type<12 && bx_cpu.es.cache.dpl<CPL )
      bx_cpu.es.cache.valid = 0;
    }

  if (bx_cpu.es.cache.valid == 0) {
    if (bx_dbg.protected)
      bx_printf("validate_seg_regs: invalidating ES cache\n");
    bx_cpu.es.selector.index = 0;
    bx_cpu.es.selector.ti    = 0;
    bx_cpu.es.selector.value = 0;
    bx_cpu.es.selector.rpl   = 0;
    bx_cpu.es.cache.p = 0;
    }

  /* validate DS */
  if ( bx_cpu.ds.cache.valid ) {
    /* selector index must be within descriptor table limits */
    if ( bx_cpu.ds.selector.ti==0 ) {
      if ( (bx_cpu.ds.selector.index*8+7) > bx_cpu.gdtr.limit )
        bx_cpu.ds.cache.valid = 0;
      }
    else {
      if ( (bx_cpu.ds.selector.index*8+7) > bx_cpu.ldtr.cache.u.ldt.limit )
        bx_cpu.ds.cache.valid = 0;
      }

    /* AR byte must indicate data or readable code segment */
    if ( bx_cpu.ds.cache.u.segment.executable &&
         bx_cpu.ds.cache.u.segment.r_w==0 )
      bx_cpu.ds.cache.valid = 0;

    /* if segment is data or non-conforming code, then
     * DPL must be >= CPL or CPL must be >= RPL */
    if ( bx_cpu.ds.cache.type<12 && bx_cpu.ds.cache.dpl<CPL )
      bx_cpu.ds.cache.valid = 0;
    }

  if (bx_cpu.ds.cache.valid == 0) {
    if (bx_dbg.protected)
      bx_printf("validate_seg_regs: invalidating DS cache\n");
    bx_cpu.ds.selector.index = 0;
    bx_cpu.ds.selector.ti    = 0;
    bx_cpu.ds.selector.value = 0;
    bx_cpu.ds.selector.rpl   = 0;
    bx_cpu.ds.cache.p = 0;
    }

#if BX_CPU >= 3
  /* validate FS */
  if ( bx_cpu.fs.cache.valid ) {
    /* selector index must be within descriptor table limits */
    if ( bx_cpu.fs.selector.ti==0 ) {
      if ( (bx_cpu.fs.selector.index*8+7) > bx_cpu.gdtr.limit )
        bx_cpu.fs.cache.valid = 0;
      }
    else {
      if ( (bx_cpu.fs.selector.index*8+7) > bx_cpu.ldtr.cache.u.ldt.limit )
        bx_cpu.fs.cache.valid = 0;
      }

    /* AR byte must indicate data or readable code segment */
    if ( bx_cpu.fs.cache.u.segment.executable &&
         bx_cpu.fs.cache.u.segment.r_w==0 )
      bx_cpu.fs.cache.valid = 0;

    /* if segment is data or non-conforming code, then
     * DPL must be >= CPL or CPL must be >= RPL */
    if ( bx_cpu.fs.cache.type<12 && bx_cpu.fs.cache.dpl<CPL )
      bx_cpu.fs.cache.valid = 0;
    }

  if (bx_cpu.fs.cache.valid == 0) {
    if (bx_dbg.protected)
      bx_printf("validate_seg_regs: invalidating FS cache\n");
    bx_cpu.fs.selector.index = 0;
    bx_cpu.fs.selector.ti    = 0;
    bx_cpu.fs.selector.value = 0;
    bx_cpu.fs.selector.rpl   = 0;
    bx_cpu.fs.cache.p = 0;
    }

  /* validate GS */
  if ( bx_cpu.gs.cache.valid ) {
    /* selector index must be within descriptor table limits */
    if ( bx_cpu.gs.selector.ti==0 ) {
      if ( (bx_cpu.gs.selector.index*8+7) > bx_cpu.gdtr.limit )
        bx_cpu.gs.cache.valid = 0;
      }
    else {
      if ( (bx_cpu.gs.selector.index*8+7) > bx_cpu.ldtr.cache.u.ldt.limit )
        bx_cpu.gs.cache.valid = 0;
      }

    /* AR byte must indicate data or readable code segment */
    if ( bx_cpu.gs.cache.u.segment.executable &&
         bx_cpu.gs.cache.u.segment.r_w==0 )
      bx_cpu.gs.cache.valid = 0;

    /* if segment is data or non-conforming code, then
     * DPL must be >= CPL or CPL must be >= RPL */
    if ( bx_cpu.gs.cache.type<12 && bx_cpu.gs.cache.dpl<CPL )
      bx_cpu.gs.cache.valid = 0;
    }

  if (bx_cpu.gs.cache.valid == 0) {
    if (bx_dbg.protected)
      bx_printf("validate_seg_regs: invalidating GS cache\n");
    bx_cpu.gs.selector.index = 0;
    bx_cpu.gs.selector.ti    = 0;
    bx_cpu.gs.selector.value = 0;
    bx_cpu.gs.selector.rpl   = 0;
    bx_cpu.gs.cache.p = 0;
    }
#endif /* 386+ */
}

#pragma warning(default:4761) // 'integral size mismatch; conversion supplied'
#pragma warning(default:4244) // 'conversion from unsigned short to unsigned char'
