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


/* ??? */
void bx_enter_real_mode(void);



  INLINE void
bx_HLT()
{
  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");
#if 0 /* bill */
  bx_panic("Processor encountered a HALT!\n");
#else
  bx_printf("Processor encountered a HALT!\n");
#endif	
}

  INLINE void
bx_CLTS()
{
#if BX_CPU > 2
  bx_panic("CLTS: not implemented for 386\n");
#endif

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  bx_panic("CLTS: not supported yet!\n");
  /* read errata file */
}

INLINE void bx_INVD() {bx_panic("INVD:\n");}
INLINE void bx_WBINVD() {bx_panic("WBINVD:\n");}
INLINE void bx_INVLPG_Ea() {bx_panic("INVLPG_Ea:\n");}

  INLINE void
bx_MOV_DdRd()
{
#if BX_CPU < 3
  bx_panic("MOV_DdRd: not supported on < 386\n");
#else
  Bit32u rm_index;
  unsigned dreg_index, rm_type;
  bx_segment_reg_t *rm_seg_reg;
  Bit32u val_32;

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   *   Protected mode: #GP(0) if attempt to write a 1 to any reserved bit
   *     of CR4
   */

  bx_decode_exgx(&dreg_index, &rm_index, &rm_type, &rm_seg_reg);
  BX_HANDLE_EXCEPTION()
  if (rm_type != BX_REGISTER_REF) {
    bx_panic("MOV_DdRd(): rm field not a register!\n");
    }

  if (bx_protected_mode() && CPL!=0) {
    bx_panic("MOV_DdRd: CPL!=0\n");
    /* #GP(0) if CPL is not 0 */
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  val_32 = BX_READ_32BIT_REG(rm_index);
  if (bx_dbg.dreg)
    bx_printf("MOV_DdRd: DR%u=%08xh unhandled\n",
      (unsigned) dreg_index, (unsigned) val_32);

  switch (dreg_index) {
    case 0: /* DR0 */
      bx_cpu.dr0 = val_32;
      break;
    case 1: /* DR1 */
      bx_cpu.dr1 = val_32;
      break;
    case 2: /* DR2 */
      bx_cpu.dr2 = val_32;
      break;
    case 3: /* DR3 */
      bx_cpu.dr3 = val_32;
      break;
    case 4: /* DR4 */
      bx_cpu.dr4 = val_32;
      break;
    case 5: /* DR5 */
      bx_cpu.dr5 = val_32;
      break;
    case 6: /* DR6 */
      bx_cpu.dr6 = val_32;
      break;
    case 7: /* DR7 */
      bx_cpu.dr7 = val_32;
      break;
    default:
      bx_panic("MOV_DdRd: control register index out of range\n");
      break;
    }
#endif
}

  INLINE void
bx_MOV_RdDd()
{
#if BX_CPU < 3
  bx_panic("MOV_RdDd: not supported on < 386\n");
#else
  Bit32u rm_index;
  unsigned dreg_index, rm_type;
  bx_segment_reg_t *rm_seg_reg;
  Bit32u val_32;

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  bx_decode_exgx(&dreg_index, &rm_index, &rm_type, &rm_seg_reg);
  BX_HANDLE_EXCEPTION()
  if (rm_type != BX_REGISTER_REF) {
    bx_panic("MOV_RdDd(): rm field not a register!\n");
    }

  if (bx_protected_mode() && CPL!=0) {
    bx_panic("MOV_RdDd: CPL!=0\n");
    /* #GP(0) if CPL is not 0 */
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  switch (dreg_index) {
    case 0: /* DR0 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR0 not implemented yet\n");
      val_32 = bx_cpu.dr0;
      break;
    case 1: /* DR1 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR1 not implemented yet\n");
      val_32 = bx_cpu.dr1;
      break;
    case 2: /* DR2 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR2 not implemented yet\n");
      val_32 = bx_cpu.dr2;
      break;
    case 3: /* DR3 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR3 not implemented yet\n");
      val_32 = bx_cpu.dr3;
      break;
    case 4: /* DR4 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR4 not implemented yet\n");
      val_32 = bx_cpu.dr4;
      break;
    case 5: /* DR5 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR5 not implemented yet\n");
      val_32 = bx_cpu.dr5;
      break;
    case 6: /* DR6 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR6 not implemented yet\n");
      val_32 = bx_cpu.dr6;
      break;
    case 7: /* DR7 */
      if (bx_dbg.dreg)
        bx_printf("MOV_RdDd: DR7 not implemented yet\n");
      val_32 = bx_cpu.dr7;
      break;
    default:
      bx_panic("MOV_RdDd: control register index out of range\n");
      val_32 = bx_cpu.dr0;
    }
  BX_WRITE_32BIT_REG(rm_index, val_32);
#endif
}


 INLINE void
bx_LMSW_Ew()
{
#if BX_CPU < 2
  bx_panic("LMSW_Ew(): not supported on 8086!\n");
#else
  Bit32u mem_addr;
  unsigned unused, mem_type;
  bx_segment_reg_t *mem_seg_reg;
  Bit16u msw;
  Boolean pe, mp, em, ts;

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  if ( bx_protected_mode() ) {
    if ( CPL != 0 ) {
      bx_printf("LMSW: CPL != 0, CPL=%u\n", (unsigned) CPL);
      bx_exception(BX_GP_EXCEPTION, 0, 0);
      return;
      }
    }


  if (bx_dbg.protected) bx_printf("lmsw():\n");

  bx_decode_exgx(&unused, &mem_addr, &mem_type, &mem_seg_reg);
  BX_HANDLE_EXCEPTION()

  if (mem_type == BX_REGISTER_REF) {
    msw = BX_READ_16BIT_REG(mem_addr);
    }
  else {
    bx_read_virtual_word(mem_seg_reg, mem_addr, &msw);
    BX_HANDLE_EXCEPTION()
    }

  /* LMSW does not affect PG,CD,NW,AM,WP,NE,ET bits, and cannot clear PE */
  pe = msw & 0x01; msw >>= 1;
  mp = msw & 0x01; msw >>= 1;
  em = msw & 0x01; msw >>= 1;
  ts = msw & 0x01;
  if (bx_dbg.protected)
    bx_printf("LMSW_Ew: pe=%x,mp=%x,em=%x,ts=%x\n",
      (unsigned) pe, (unsigned) mp, (unsigned) em, (unsigned) ts);

  if (mp || ts) {
    bx_panic("unsupported setting of MP, EM, TS bits in LMSW instructions\n");
    }
  bx_cpu.cr0.em = em;

  if (pe) { /* switching to protected mode */
    if (bx_protected_mode()) { /* already there */
      if (bx_dbg.protected)
        bx_printf("LMSW: already in protected mode\n");
      }
    else {
      bx_enter_protected_mode();
      }
    }

  else { /* set bit to real mode */
    if ( bx_real_mode() ) {
      /* stay in real mode, no transition */
      return;
      }
    else {
      bx_panic("LMSW_Ew: transition to real mode not supported!\n");
      }
    }
#endif /* BX_CPU < 2 */
}

 INLINE void
bx_SMSW_Ew()
{
#if BX_CPU < 2
  bx_panic("SMSW_Ew: not supported yet!\n");
#else
  Bit32u op1_addr;
  unsigned unused, op1_type;
  bx_segment_reg_t *op1_seg_reg;
  Bit16u msw;

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  if (bx_dbg.protected)
    bx_printf("SMSW_Ew:!\n");

  bx_decode_exgx(&unused, &op1_addr, &op1_type, &op1_seg_reg);
  BX_HANDLE_EXCEPTION()

#if BX_CPU == 2
  msw = 0xfff0; /* 80286 init value */
  msw |= (bx_cpu.cr0.ts << 3) |
         (bx_cpu.cr0.em << 2) |
         (bx_cpu.cr0.mp << 1) |
         bx_cpu.cr0.pe;
#else /* 386+ */
  /* reserved bits 0 ??? */
  /* should NE bit be included here ??? */
  msw =  (bx_cpu.cr0.ts << 3) |
         (bx_cpu.cr0.em << 2) |
         (bx_cpu.cr0.mp << 1) |
         bx_cpu.cr0.pe;
#endif


  if (op1_type == BX_REGISTER_REF) {
    BX_WRITE_16BIT_REG(op1_addr, msw);
    }
  else {
    bx_write_virtual_word(op1_seg_reg, op1_addr, &msw);
    BX_HANDLE_EXCEPTION()
    }

#endif
}


  INLINE void
bx_MOV_CdRd()
{
  /* mov general register data to control register */
#if BX_CPU < 3
  bx_panic("MOV_CdRd: not supported on < 386\n");
#else
  Bit32u rm_index;
  unsigned creg_index, rm_type;
  bx_segment_reg_t *rm_seg_reg;
  Bit32u val_32;
  Boolean prev_pe, prev_pg;


  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   *   Protected mode: #GP(0) if attempt to write a 1 to any reserved bit
   *     of CR4
   */

  bx_decode_exgx(&creg_index, &rm_index, &rm_type, &rm_seg_reg);
  BX_HANDLE_EXCEPTION()
  if (rm_type != BX_REGISTER_REF) {
    bx_panic("MOV_CdRd(): rm field not a register!\n");
    }

  if (bx_protected_mode() && CPL!=0) {
    bx_panic("MOV_CdRd: CPL!=0\n");
    /* #GP(0) if CPL is not 0 */
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  val_32 = BX_READ_32BIT_REG(rm_index);

  switch (creg_index) {
    case 0: /* CR0 (MSW) */
      /*bx_printf("MOV_CdRd:CR0: R32 = %08x\n", (unsigned) val_32);*/
      prev_pe = bx_cpu.cr0.pe;
      prev_pg = bx_cpu.cr0.pg;

      bx_cpu.cr0.pe = val_32 & 0x01;
      bx_cpu.cr0.mp = (val_32 >> 1) & 0x01;
      bx_cpu.cr0.em = (val_32 >> 2) & 0x01;
      bx_cpu.cr0.ts = (val_32 >> 3) & 0x01;
      /* bx_cpu.cr0.et ??? */
#if BX_CPU >= 4
      bx_cpu.cr0.ne = (val_32 >> 5)  & 0x01;
      bx_cpu.cr0.wp = (val_32 >> 16) & 0x01;
      bx_cpu.cr0.am = (val_32 >> 18) & 0x01;
      bx_cpu.cr0.nw = (val_32 >> 29) & 0x01;
      bx_cpu.cr0.cd = (val_32 >> 30) & 0x01;
#endif
      bx_cpu.cr0.pg = (val_32 >> 31) & 0x01;
      if (bx_cpu.cr0.ts)
        bx_panic("MOV_CdRd:CR0: R32 = %08x\n", (unsigned) val_32);

      if (prev_pe==0 && bx_cpu.cr0.pe) {
        bx_enter_protected_mode();
        }
      else if (prev_pe==1 && bx_cpu.cr0.pe==0) {
        bx_enter_real_mode();
        }

      if (prev_pg==0 && bx_cpu.cr0.pg)
        bx_enable_paging();
      else if (prev_pg==1 && bx_cpu.cr0.pg==0)
        bx_disable_paging();
      break;
    case 1: /* CR1 */
      bx_panic("MOV_CdRd: CR1 not implemented yet\n");
      break;
    case 2: /* CR2 */
      bx_panic("MOV_CdRd: CR2 not implemented yet\n");
      break;
    case 3: /* CR3 */
      if (bx_dbg.creg)
        bx_printf("MOV_CdRd:(%08x)\n", (unsigned) val_32);
      bx_CR3_change(val_32);
      break;
    case 4: /* CR4 */
      bx_panic("MOV_CdRd: CR4 not implemented yet\n");
      break;
    default:
      bx_panic("MOV_CdRd: control register index out of range\n");
      break;
    }
#endif
}

  INLINE void
bx_MOV_RdCd()
{
  /* mov control register data to register */
#if BX_CPU < 3
  bx_panic("MOV_RdCd: not supported on < 386\n");
#else
  Bit32u rm_index;
  unsigned creg_index, rm_type;
  bx_segment_reg_t *rm_seg_reg;
  Bit32u val_32;

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  /* NOTES:
   *   32bit operands always used
   *   r/m field specifies general register
   *   mod field should always be 11 binary
   *   reg field specifies which special register
   */

  bx_decode_exgx(&creg_index, &rm_index, &rm_type, &rm_seg_reg);
  BX_HANDLE_EXCEPTION()
  if (rm_type != BX_REGISTER_REF) {
    bx_panic("MOV_RdCd(): rm field not a register!\n");
    }

  if (bx_protected_mode() && CPL!=0) {
    bx_panic("MOV_RdCd: CPL!=0\n");
    /* #GP(0) if CPL is not 0 */
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  switch (creg_index) {
    case 0: /* CR0 (MSW) */
      val_32 = (bx_cpu.cr0.pe) |
               (bx_cpu.cr0.mp << 1) |
               (bx_cpu.cr0.em << 2) |
               (bx_cpu.cr0.ts << 3) |
               /* ???(bx_cpu.cr0.et << 4) |*/
#if BX_CPU >= 4
               (bx_cpu.cr0.ne << 5) |
               (bx_cpu.cr0.wp << 16) |
               (bx_cpu.cr0.am << 18) |
               (bx_cpu.cr0.nw << 29) |
               (bx_cpu.cr0.cd << 30) |
#endif
               (bx_cpu.cr0.pg << 31);
      break;
    case 1: /* CR1 */
      bx_panic("MOV_RdCd: CR1 not implemented yet\n");
      val_32 = 0;
      break;
    case 2: /* CR2 */
      bx_panic("MOV_RdCd: CR2 not implemented yet\n");
      val_32 = 0;
      break;
    case 3: /* CR3 */
      if (bx_dbg.creg)
        bx_printf("MOV_RdCd: reading CR3\n");
      val_32 = bx_cpu.cr3;
      break;
    case 4: /* CR4 */
      bx_panic("MOV_RdCd: CR4 not implemented yet\n");
      val_32 = 0;
      break;
    default:
      bx_panic("MOV_RdCd: control register index out of range\n");
      val_32 = 0;
    }
  BX_WRITE_32BIT_REG(rm_index, val_32);
#endif
}

INLINE void bx_MOV_TdRd() {bx_panic("MOV_TdRd:\n");}
INLINE void bx_MOV_RdTd() {bx_panic("MOV_RdTd:\n");}

  INLINE void
bx_WAIT()
{
#if BX_CPU > 2
  bx_panic("WAIT: not implemented for 386\n");
#endif

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  bx_single_step_event = 0; /* WAIT doesn't generate single steps */
  bx_panic("WAIT:\n");
}

  INLINE void
bx_LOADALL()
{
#if BX_CPU < 2
  bx_panic("undocumented LOADALL instruction not supported on 8086\n");
#else
  Bit16u msw, tr, flags, ip, ldtr, ds, ss, cs, es;
  Bit16u di, si, bp, sp, bx, dx, cx, ax;
  Bit16u base_15_0, limit;
  Bit8u  base_23_16, access;

  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

#if BX_CPU > 2
  bx_panic("loadall: not implemented for 386\n");
  /* ??? need to set G and other bits, and compute .limit_scaled also */
  /* for all segments CS,DS,SS,... */
#endif

  if (bx_protected_mode()) {
    bx_panic(
      "LOADALL not yet supported for protected mode\n");
    }

  if (bx_dbg.protected) bx_printf("loadall():\n");

  /* MSW */
  bx_access_physical(0x806, 2, BX_READ, &msw);
  bx_cpu.cr0.pe = (msw & 0x01); msw >>= 1;
  bx_cpu.cr0.mp = (msw & 0x01); msw >>= 1;
  bx_cpu.cr0.em = (msw & 0x01); msw >>= 1;
  bx_cpu.cr0.ts = (msw & 0x01);
  if (bx_dbg.protected)
    bx_printf("LOADALL: pe=%u, mp=%u, em=%u, ts=%u\n",
      (unsigned) bx_cpu.cr0.pe, (unsigned) bx_cpu.cr0.mp,
      (unsigned) bx_cpu.cr0.em, (unsigned) bx_cpu.cr0.ts);
  if (bx_cpu.cr0.pe || bx_cpu.cr0.mp || bx_cpu.cr0.em || bx_cpu.cr0.ts)
    bx_panic("LOADALL set PE, MP, EM or TS bits in MSW!\n");

  /* TR */
  bx_access_physical(0x816, 2, BX_READ, &tr);
  bx_cpu.tr.selector.value = tr;
  bx_cpu.tr.selector.rpl   = (tr & 0x03);  tr >>= 2;
  bx_cpu.tr.selector.ti    = (tr & 0x01);  tr >>= 1;
  bx_cpu.tr.selector.index = tr;
  bx_access_physical(0x860, 2, BX_READ, &base_15_0);
  bx_access_physical(0x862, 1, BX_READ, &base_23_16);
  bx_access_physical(0x863, 1, BX_READ, &access);
  bx_access_physical(0x864, 2, BX_READ, &limit);

  if (bx_dbg.protected) {
    bx_printf("loadall: tr.AR byte: %02x\n", (unsigned) access);
    bx_printf("  tr.base : %06x\n", (unsigned) bx_cpu.tr.cache.u.tss286.base);
    bx_printf("  tr.limit: %04x\n", (unsigned) bx_cpu.tr.cache.u.tss286.limit);
    }

  bx_cpu.tr.cache.valid =
  bx_cpu.tr.cache.p           = (access & 0x80) >> 7;
  bx_cpu.tr.cache.dpl         = (access & 0x60) >> 5;
  bx_cpu.tr.cache.segment     = (access & 0x10) >> 4;
  bx_cpu.tr.cache.type        = (access & 0x0f);
  bx_cpu.tr.cache.u.tss286.base  = (base_23_16 << 16) | base_15_0;
  bx_cpu.tr.cache.u.tss286.limit = limit;

  if ( (bx_cpu.tr.selector.value & 0xfffc) == 0 ) {
    if (bx_dbg.protected)
      bx_printf("loadall: tr.value null\n");
    bx_cpu.tr.cache.valid = 0;
    }
  if ( bx_cpu.tr.cache.valid == 0 ) {
    if (bx_dbg.protected)
      bx_printf("LOADALL: TR.valid = 0\n");
    }
  if ( bx_cpu.tr.cache.u.tss286.limit < 43 ) {
    if (bx_dbg.protected) {
      bx_printf("  tr.segment: %u\n", (unsigned) bx_cpu.tr.cache.segment);
      bx_printf("  tr.base : %06x\n", (unsigned) bx_cpu.tr.cache.u.tss286.base);
      bx_printf("  tr.limit: %04x\n", (unsigned) bx_cpu.tr.cache.u.tss286.limit);
      bx_printf("  AR byte: %02x\n", (unsigned) access);
      bx_printf("loadall: tr.limit<43\n");
      }
    bx_cpu.tr.cache.valid = 0;
    }
  if ( bx_cpu.tr.cache.type != 1 ) {
    if (bx_dbg.protected)
      bx_printf("LOADALL: TR.type(%u) != 1\n", (unsigned) bx_cpu.tr.cache.type);
    bx_cpu.tr.cache.valid = 0;
    }
  if ( bx_cpu.tr.cache.segment ) {
    if (bx_dbg.protected)
      bx_printf("LOADALL: TR.segment = 1\n");
    bx_cpu.tr.cache.valid = 0;
    }
  if (bx_cpu.tr.cache.valid==0) {
    bx_cpu.tr.cache.u.tss286.base   = 0;
    bx_cpu.tr.cache.u.tss286.limit  = 0;
    bx_cpu.tr.cache.p            = 0;
    bx_cpu.tr.selector.value     = 0;
    bx_cpu.tr.selector.index     = 0;
    bx_cpu.tr.selector.ti        = 0;
    bx_cpu.tr.selector.rpl       = 0;
    }


  /* FLAGS */
  bx_access_physical(0x818, 2, BX_READ, &flags);
  bx_write_flags(flags, 1, 1);

  /* IP */
  bx_access_physical(0x81a, 2, BX_READ, &ip);
  IP = ip;

  /* LDTR */
  bx_access_physical(0x81c, 2, BX_READ, &ldtr);
  bx_cpu.ldtr.selector.value = ldtr;
  bx_cpu.ldtr.selector.rpl   = (ldtr & 0x03);  ldtr >>= 2;
  bx_cpu.ldtr.selector.ti    = (ldtr & 0x01);  ldtr >>= 1;
  bx_cpu.ldtr.selector.index = ldtr;
  if ( (bx_cpu.ldtr.selector.value & 0xfffc) == 0 ) {
    bx_cpu.ldtr.cache.valid   = 0;
    bx_cpu.ldtr.cache.p       = 0;
    bx_cpu.ldtr.cache.segment = 0;
    bx_cpu.ldtr.cache.type    = 0;
    bx_cpu.ldtr.cache.u.ldt.base = 0;
    bx_cpu.ldtr.cache.u.ldt.limit = 0;
    bx_cpu.ldtr.selector.value = 0;
    bx_cpu.ldtr.selector.index = 0;
    bx_cpu.ldtr.selector.ti    = 0;
    if (bx_dbg.protected)
      bx_printf("loadall: invalidating LDTR, null selector loaded.\n");
    }
  else {
    bx_access_physical(0x854, 2, BX_READ, &base_15_0);
    bx_access_physical(0x856, 1, BX_READ, &base_23_16);
    bx_access_physical(0x857, 1, BX_READ, &access);
    bx_access_physical(0x858, 2, BX_READ, &limit);
    bx_cpu.ldtr.cache.valid      =
    bx_cpu.ldtr.cache.p          = access >> 7;
    bx_cpu.ldtr.cache.dpl        = (access >> 5) & 0x03;
    bx_cpu.ldtr.cache.segment    = (access >> 4) & 0x01;
    bx_cpu.ldtr.cache.type       = (access & 0x0f);
    bx_cpu.ldtr.cache.u.ldt.base = (base_23_16 << 16) | base_15_0;
    bx_cpu.ldtr.cache.u.ldt.limit = limit;

    if (access == 0) {
      bx_panic("loadall: LDTR case access byte=0.\n");
      }
    if ( bx_cpu.ldtr.cache.valid==0 ) {
      bx_panic("loadall: ldtr.valid=0\n");
      }
    if (bx_cpu.ldtr.cache.segment) { /* not a system segment */
      bx_printf("         AR byte = %02x\n", (unsigned) access);
      bx_panic("loadall: LDTR descriptor cache loaded with non system segment\n");
      }
    if ( bx_cpu.ldtr.cache.type != 2 ) {
      bx_panic("loadall: LDTR.type(%u) != 2\n", (unsigned) (access & 0x0f));
      }
    if (bx_dbg.protected) {
      bx_printf("loadall: ldtr.limit = %04x\n",
        (unsigned) bx_cpu.ldtr.cache.u.ldt.limit);
      bx_printf("loadall: ldtr.base = %04x\n",
        (unsigned) bx_cpu.ldtr.cache.u.ldt.base);
      }
    }

  /* DS */
  bx_access_physical(0x81e, 2, BX_READ, &ds);
  bx_cpu.ds.selector.value = ds;
  bx_cpu.ds.selector.rpl   = (ds & 0x03);  ds >>= 2;
  bx_cpu.ds.selector.ti    = (ds & 0x01);  ds >>= 1;
  bx_cpu.ds.selector.index = ds;
  bx_access_physical(0x848, 2, BX_READ, &base_15_0);
  bx_access_physical(0x84a, 1, BX_READ, &base_23_16);
  bx_access_physical(0x84b, 1, BX_READ, &access);
  bx_access_physical(0x84c, 2, BX_READ, &limit);
  bx_cpu.ds.cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  bx_cpu.ds.cache.u.segment.limit = limit;
  bx_cpu.ds.cache.u.segment.a          = (access & 0x01); access >>= 1;
  bx_cpu.ds.cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  bx_cpu.ds.cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  bx_cpu.ds.cache.u.segment.executable = (access & 0x01); access >>= 1;
  bx_cpu.ds.cache.segment    = (access & 0x01); access >>= 1;
  bx_cpu.ds.cache.dpl        = (access & 0x03); access >>= 2;
  bx_cpu.ds.cache.valid      =
  bx_cpu.ds.cache.p          = (access & 0x01);

  if ( (bx_cpu.ds.selector.value & 0xfffc) == 0 ) {
    bx_cpu.ds.cache.valid = 0;
    }
  if (bx_cpu.ds.cache.valid==0  ||
      bx_cpu.ds.cache.segment==0) {
    bx_panic("loadall: DS invalid\n");
    }

  /* SS */
  bx_access_physical(0x820, 2, BX_READ, &ss);
  bx_cpu.ss.selector.value = ss;
  bx_cpu.ss.selector.rpl   = (ss & 0x03); ss >>= 2;
  bx_cpu.ss.selector.ti    = (ss & 0x01); ss >>= 1;
  bx_cpu.ss.selector.index = ss;
  bx_access_physical(0x842, 2, BX_READ, &base_15_0);
  bx_access_physical(0x844, 1, BX_READ, &base_23_16);
  bx_access_physical(0x845, 1, BX_READ, &access);
  bx_access_physical(0x846, 2, BX_READ, &limit);
  bx_cpu.ss.cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  bx_cpu.ss.cache.u.segment.limit = limit;
  bx_cpu.ss.cache.u.segment.a          = (access & 0x01); access >>= 1;
  bx_cpu.ss.cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  bx_cpu.ss.cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  bx_cpu.ss.cache.u.segment.executable = (access & 0x01); access >>= 1;
  bx_cpu.ss.cache.segment    = (access & 0x01); access >>= 1;
  bx_cpu.ss.cache.dpl        = (access & 0x03); access >>= 2;
  bx_cpu.ss.cache.p          = (access & 0x01);

  if ( (bx_cpu.ss.selector.value & 0xfffc) == 0 ) {
    bx_cpu.ss.cache.valid = 0;
    }
  if (bx_cpu.ss.cache.valid==0  ||
      bx_cpu.ss.cache.segment==0) {
    bx_panic("loadall: SS invalid\n");
    }


  /* CS */
  bx_access_physical(0x822, 2, BX_READ, &cs);
  bx_cpu.cs.selector.value = cs;
  bx_cpu.cs.selector.rpl   = (cs & 0x03); cs >>= 2;

  if (bx_dbg.protected)
    bx_printf("LOADALL: setting cs.selector.rpl to %u\n",
      (unsigned) bx_cpu.cs.selector.rpl);

  bx_cpu.cs.selector.ti    = (cs & 0x01); cs >>= 1;
  bx_cpu.cs.selector.index = cs;
  bx_access_physical(0x83c, 2, BX_READ, &base_15_0);
  bx_access_physical(0x83e, 1, BX_READ, &base_23_16);
  bx_access_physical(0x83f, 1, BX_READ, &access);
  bx_access_physical(0x840, 2, BX_READ, &limit);
  bx_cpu.cs.cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  bx_cpu.cs.cache.u.segment.limit = limit;
  bx_cpu.cs.cache.u.segment.a          = (access & 0x01); access >>= 1;
  bx_cpu.cs.cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  bx_cpu.cs.cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  bx_cpu.cs.cache.u.segment.executable = (access & 0x01); access >>= 1;
  bx_cpu.cs.cache.segment    = (access & 0x01); access >>= 1;
  bx_cpu.cs.cache.dpl        = (access & 0x03); access >>= 2;
  bx_cpu.cs.cache.p          = (access & 0x01);

  if ( (bx_cpu.cs.selector.value & 0xfffc) == 0 ) {
    bx_cpu.cs.cache.valid = 0;
    }
  if (bx_cpu.cs.cache.valid==0  ||
      bx_cpu.cs.cache.segment==0) {
    bx_panic("loadall: CS invalid\n");
    }

  /* ES */
  bx_access_physical(0x824, 2, BX_READ, &es);
  bx_cpu.es.selector.value = es;
  bx_cpu.es.selector.rpl   = (es & 0x03); es >>= 2;
  bx_cpu.es.selector.ti    = (es & 0x01); es >>= 1;
  bx_cpu.es.selector.index = es;
  bx_access_physical(0x836, 2, BX_READ, &base_15_0);
  bx_access_physical(0x838, 1, BX_READ, &base_23_16);
  bx_access_physical(0x839, 1, BX_READ, &access);
  bx_access_physical(0x83a, 2, BX_READ, &limit);
  bx_cpu.es.cache.u.segment.base = (base_23_16 << 16) | base_15_0;
  bx_cpu.es.cache.u.segment.limit = limit;
  bx_cpu.es.cache.u.segment.a          = (access & 0x01); access >>= 1;
  bx_cpu.es.cache.u.segment.r_w        = (access & 0x01); access >>= 1;
  bx_cpu.es.cache.u.segment.c_ed       = (access & 0x01); access >>= 1;
  bx_cpu.es.cache.u.segment.executable = (access & 0x01); access >>= 1;
  bx_cpu.es.cache.segment    = (access & 0x01); access >>= 1;
  bx_cpu.es.cache.dpl        = (access & 0x03); access >>= 2;
  bx_cpu.es.cache.p          = (access & 0x01);

  if (bx_dbg.protected) {
    bx_printf("bx_cpu.cs.dpl = %02x\n", (unsigned) bx_cpu.cs.cache.dpl);
    bx_printf("bx_cpu.ss.dpl = %02x\n", (unsigned) bx_cpu.ss.cache.dpl);
    bx_printf("bx_cpu.ds.dpl = %02x\n", (unsigned) bx_cpu.ds.cache.dpl);
    bx_printf("bx_cpu.es.dpl = %02x\n", (unsigned) bx_cpu.es.cache.dpl);
    bx_printf("LOADALL: setting cs.selector.rpl to %u\n",
      (unsigned) bx_cpu.cs.selector.rpl);
    bx_printf("LOADALL: setting ss.selector.rpl to %u\n",
      (unsigned) bx_cpu.ss.selector.rpl);
    bx_printf("LOADALL: setting ds.selector.rpl to %u\n",
      (unsigned) bx_cpu.ds.selector.rpl);
    bx_printf("LOADALL: setting es.selector.rpl to %u\n",
      (unsigned) bx_cpu.es.selector.rpl);
    }

  if ( (bx_cpu.es.selector.value & 0xfffc) == 0 ) {
    bx_cpu.es.cache.valid = 0;
    }
  if (bx_cpu.es.cache.valid==0  ||
      bx_cpu.es.cache.segment==0) {
    bx_panic("loadall: ES invalid\n");
    }

  /* DI */
  bx_access_physical(0x826, 2, BX_READ, &di);
  DI = di;

  /* SI */
  bx_access_physical(0x828, 2, BX_READ, &si);
  SI = si;

  /* BP */
  bx_access_physical(0x82a, 2, BX_READ, &bp);
  BP = bp;

  /* SP */
  bx_access_physical(0x82c, 2, BX_READ, &sp);
  SP = sp;

  /* BX */
  bx_access_physical(0x82e, 2, BX_READ, &bx);
  BX = bx;

  /* DX */
  bx_access_physical(0x830, 2, BX_READ, &dx);
  DX = dx;

  /* CX */
  bx_access_physical(0x832, 2, BX_READ, &cx);
  CX = cx;

  /* AX */
  bx_access_physical(0x834, 2, BX_READ, &ax);
  AX = ax;

  /* GDTR */
  bx_access_physical(0x84e, 2, BX_READ, &base_15_0);
  bx_access_physical(0x850, 1, BX_READ, &base_23_16);
  bx_access_physical(0x851, 1, BX_READ, &access);
  bx_access_physical(0x852, 2, BX_READ, &limit);
  bx_cpu.gdtr.base = (base_23_16 << 16) | base_15_0;
  bx_cpu.gdtr.limit = limit;
  if (access)
    if (bx_dbg.protected)
      bx_printf("LOADALL: GDTR access bits not 0 (%02x).\n",
        (unsigned) access);

  /* IDTR */
  bx_access_physical(0x85a, 2, BX_READ, &base_15_0);
  bx_access_physical(0x85c, 1, BX_READ, &base_23_16);
  bx_access_physical(0x85d, 1, BX_READ, &access);
  bx_access_physical(0x85e, 2, BX_READ, &limit);
  bx_cpu.idtr.base = (base_23_16 << 16) | base_15_0;
  bx_cpu.idtr.limit = limit;
  if (bx_dbg.protected) {
    bx_printf("LOADALL: idtr.limit = %04x\n", (unsigned) bx_cpu.idtr.limit);
    bx_printf("LOADALL: idtr.base  = %06x\n", (unsigned) bx_cpu.idtr.base);
    if (access)
      bx_printf("LOADALL: IDTR access bits not 0 (%02x).\n",
        (unsigned) access);
    }
#endif
}


  INLINE void
bx_CPUID()
{
  if (bx_v8086_mode()) bx_panic("proc_ctrl: v8086 mode unsupported\n");

  bx_panic("CPUID: not implemented\n");
}
