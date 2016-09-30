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

//#ifdef AUDIT_ENABLED
//#include "audit.h"
//#endif

void bx_shutdown_cpu(void);

#if 1 /* bill or whoever */
extern unsigned long revert;
#endif

/* Exception classes.  These are used as indexes into the 'is_exception_OK'
 * array below, and are stored in the 'exception' array also
 */
#define BX_ET_BENIGN       0
#define BX_ET_CONTRIBUTORY 1
#define BX_ET_PAGE_FAULT   2

#define BX_ET_DOUBLE_FAULT 10

static Bit8u exception[2];
static const Boolean is_exception_OK[3][3] = {
  { 1, 1, 1 }, /* 1st exception is BENIGN */
  { 1, 0, 1 }, /* 1st exception is CONTRIBUTORY */
  { 1, 0, 0 }  /* 1st exception is PAGE_FAULT */
  };


  void
bx_interrupt(Bit8u vector, Boolean is_INT, Boolean is_error_code)
{

  /* ??? use access_linear instead of access_physical below */

#if BX_CPU >= 2
  unsigned prev_errno;

  if (bx_dbg.interrupts)
    bx_printf("bx_interrupt(): vector = %u, INT = %u\n",
      (unsigned) vector, (unsigned) is_INT);

  prev_errno = bx_cpu.errno;

  if (!bx_real_mode()) {
    Bit32u  dword1, dword2;
    bx_descriptor_t gate_descriptor, cs_descriptor;
    bx_selector_t cs_selector;

    /* Comments closely copied from Intel's "80286 and 80287 Programmers's
     * Reference Manual", page B-49 */

    /* interrupt vector must be within IDT table limits,
     * else #GP(vetor number*8 + 2 + EXT) */
    if ( (vector*8 + 7) > bx_cpu.idtr.limit) {
      if (bx_dbg.interrupts) {
        bx_printf("IDT.limit = %04x\n", (unsigned) bx_cpu.idtr.limit);
        bx_printf("IDT.base  = %06x\n", (unsigned) bx_cpu.idtr.base);
        bx_printf("interrupt vector must be within IDT table limits\n");
        bx_printf("bailing\n");
        }
      bx_panic("interrupt(): vector > idtr.limit\n");
      bx_reset_cpu(); /* ??? */
      return;

      bx_exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      return;
      }

    /* descriptor AR byte must indicate interrupt gate, trap gate,
     * or task gate, else #GP(vector*8 + 2 + EXT) */
    bx_access_linear(bx_cpu.idtr.base + vector*8,     4, 0,
      BX_READ, &dword1);
    bx_access_linear(bx_cpu.idtr.base + vector*8 + 4, 4, 0,
      BX_READ, &dword2);

    bx_parse_descriptor(dword1, dword2, &gate_descriptor);

    if ( gate_descriptor.valid==0 || gate_descriptor.segment) {
      bx_panic("interrupt(): gate descriptor is not valid sys seg\n");
      bx_exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      return;
      }

    switch (gate_descriptor.type) {
      case 5: /* task gate */
      case 6: /* 286 interrupt gate */
      case 7: /* 286 trap gate */
      case 14: /* 386 interrupt gate */
      case 15: /* 386 trap gate */
        break;
      default:
        bx_panic("interrupt(): gate.type(%u) != {5,6,7,14,15}\n",
          (unsigned) gate_descriptor.type);
        bx_exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
        return;
      }

    /* if software interrupt, then gate descripor DPL must be >= CPL,
     * else #GP(vector * 8 + 2 + EXT) */
    if (is_INT  &&  (gate_descriptor.dpl < CPL)) {
/* ??? */
      bx_printf("interrupt(): is_INT && (dpl < CPL)\n");
      bx_exception(BX_GP_EXCEPTION, vector*8 + 2, 0);
      return;
      }

    /* Gate must be present, else #NP(vector * 8 + 2 + EXT) */
    if (gate_descriptor.p == 0) { /* not present */
      bx_panic("interrupt(): p == 0\n");
      bx_exception(BX_NP_EXCEPTION, vector*8 + 2, 0);
      }

    switch (gate_descriptor.type) {
      case 5: /* 286/386 task gate */
        bx_panic("interrupt(): task gate not complete\n");
        bx_task_gate(gate_descriptor.u.gate286.dest_selector, 1 /* link */);
        return;
        break;
      case 6: /* 286 interrupt gate */
      case 7: /* 286 trap gate */
        if (bx_v8086_mode()) {
          bx_panic("interrupt: 286-int/trap gate from v8086 mode\n");
          }

        if (gate_descriptor.type==6) {
          if (bx_dbg.interrupts)
            bx_printf("interrupt(): interrupt gate 16\n");
          }
        else {
          if (bx_dbg.interrupts)
            bx_printf("interrupt(): trap gate 16\n");
          }

        /* TRAP-OR-INT-GATE */

        /* examine CS selector and descriptor given in gate descriptor */
        /* selector must be non-null else #GP(EXT) */
        if ( (gate_descriptor.u.gate286.dest_selector & 0xfffc) == 0 ) {
          bx_panic("int_trap_gate286(): selector null\n");
          /* GP(ext) */
          return;
          }

        bx_parse_selector(gate_descriptor.u.gate286.dest_selector,
                          &cs_selector);

        /* fetch 2 dwords of descriptor; call handles out of limits checks */
        bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
                                BX_GP_EXCEPTION);
        if (bx_cpu.errno > prev_errno) {
          bx_panic("interrrupt(): trap_gate286():"
                       " fetch_raw_descriptor failed\n");
          return;
          }

        bx_parse_descriptor(dword1, dword2, &cs_descriptor);

        /* descriptor AR byte must indicate code seg, else #GP(selector+EXT) */
        if ( cs_descriptor.valid==0 ||
             cs_descriptor.segment==0 ||
             cs_descriptor.u.segment.executable==0 ) {
          bx_panic("interrupt(): not code segment\n");
          /* GP(selector + ext) */
          return;
          }

        /* segment must be present, else #NP(selector + EXT) */
        if ( cs_descriptor.p==0 ) {
          bx_panic("interrupt(): segment not present\n");
          /* NP(selector + EXT) */
          return;
          }

        /* if code segment is non-conforming and DPL < CPL then
         * INTERRUPT TO INNER PRIVILEGE: */
        if ( cs_descriptor.u.segment.c_ed==0 && cs_descriptor.dpl<CPL ) {
          Bit16u old_SP, old_SS, old_IP, old_CS;
          Bit16u SS_for_cpl_x, SP_for_cpl_x;
          bx_descriptor_t ss_descriptor;
          bx_selector_t   ss_selector;

          if (bx_dbg.interrupts)
            bx_printf("interrupt(): INTERRUPT TO INNER PRIVILEGE\n");

          /* check selector and descriptor for new stack in current
           * Task State Segment */
          
          if (bx_cpu.tr.cache.valid==0)
            bx_panic("interrupt(): TR.cache invalid\n");
          if (bx_cpu.tr.cache.type!=1)
            bx_panic("interrupt(): gate286: TR.type(%u)!=9\n",
              (unsigned) bx_cpu.tr.cache.type);

          bx_access_linear(bx_cpu.tr.cache.u.tss286.base + 4 +
            4*cs_descriptor.dpl, 2, 0, BX_READ, &SS_for_cpl_x);
          bx_access_linear(bx_cpu.tr.cache.u.tss286.base + 2 +
            4*cs_descriptor.dpl, 2, 0, BX_READ, &SP_for_cpl_x);

          /* Selector must be non-null else #TS(EXT) */
          if ( (SS_for_cpl_x & 0xfffc) == 0 ) {
            bx_panic("interrupt(): SS selector null\n");
            /* TS(ext) */
            return;
            }

          /* selector index must be within its descriptor table limits
           * else #TS(SS selector + EXT) */
          bx_parse_selector(SS_for_cpl_x, &ss_selector);
          /* fetch 2 dwords of descriptor; call handles out of limits checks */
          bx_fetch_raw_descriptor(&ss_selector, &dword1, &dword2,
                                  BX_TS_EXCEPTION);
          if (bx_cpu.errno > prev_errno) {
            bx_panic("interrupt(): SS fetch_raw_descriptor failed\n");
            return;
            }
          bx_parse_descriptor(dword1, dword2, &ss_descriptor);

          /* selector rpl must = dpl of code segment,
           * else #TS(SS selector + ext)*/
          if (ss_selector.rpl != cs_descriptor.dpl) {
            bx_panic("interrupt(): SS.rpl != CS.dpl\n");
            /* TS(SS selector) */
            return;
            }

          /* stack seg DPL must = DPL of code segment,
           * else #TS(SS selector + ext) */
          if (ss_descriptor.dpl != cs_descriptor.dpl) {
            bx_panic("interrupt(): SS.dpl != CS.dpl\n");
            /* TS(SS selector) */
            return;
            }

          /* descriptor must indicate writable data segment,
           * else #TS(SS selector + EXT) */
          if (ss_descriptor.valid==0 ||
              ss_descriptor.segment==0  ||
              ss_descriptor.u.segment.executable==1  ||
              ss_descriptor.u.segment.r_w==0) {
            bx_panic("interrupt(): SS not writable data segment\n");
            /* TS(SS selector) */
            return;
            }

          /* seg must be present, else #SS(SS selector + ext) */
          if (ss_descriptor.p==0) {
            bx_panic("interrupt(): SS not present\n");
            /* SS(SS selector) */
            return;
            }

          /* new stack must have room for 10 bytes, else #SS(0) */
          if ( !bx_can_push(&ss_descriptor, SP_for_cpl_x, 10) ) {
            bx_panic("interrupt(): new stack doesn't have room for 10\n");
            /* SS(0) */
            return;
            }

          /* IP must be within CS segment boundaries, else #GP(0) */
          if (gate_descriptor.u.gate286.dest_offset >
              cs_descriptor.u.segment.limit_scaled) {
            bx_panic("interrupt(): IP > CS.limit\n");
            /* GP(0) */
            return;
            }

          old_SP = SP;
          old_SS = bx_cpu.ss.selector.value;
          old_IP = IP;
          old_CS = bx_cpu.cs.selector.value;

          /* load new SS:SP values from TSS */
          /* load SS descriptor */
          bx_load_ss(&ss_selector, &ss_descriptor, cs_descriptor.dpl);
          bx_cpu.esp = SP_for_cpl_x; /* ??? */

          /* load new CS:IP values from gate */
          /* load CS descriptor */
          /* set CPL to new code segment DPL */
          /* set RPL of CS to CPL */
          bx_load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);
          bx_cpu.eip = gate_descriptor.u.gate286.dest_offset;

          /* push long pointer to old stack onto new stack */
          bx_push_16(old_SS);
          bx_push_16(old_SP);

          /* ??? push flags ??? */
          bx_push_16(bx_read_flags());

          /* push return address onto new stack */
          bx_push_16(old_CS);
          bx_push_16(old_IP);
          if (bx_cpu.errno > prev_errno) {
            bx_panic("interrupt(): error pushing values\n");
            return;
            }

          /* if INTERRUPT GATE set IF to 0 */
          /* set TF to 0 */
          /* set NT to 0 */
          if ( gate_descriptor.type==6 )
            bx_cpu.eflags.if_ = 0;
          bx_cpu.eflags.tf = 0;
          bx_cpu.eflags.nt = 0;

          if (bx_dbg.interrupts)
            bx_printf("interrupt(): INTERRUPT TO INNER PRIVILEGE succeeding\n");
          return;
          }

        /* if code segment is conforming OR code segment DPL = CPL then
         * INTERRUPT TO SAME PRIVILEGE LEVEL: */
        if ( cs_descriptor.u.segment.c_ed==1 || cs_descriptor.dpl==CPL ) {
          int bytes;

          if (bx_dbg.interrupts)
            bx_printf("int_trap_gate286(): INTERRUPT TO SAME PRIVILEGE\n");
      
          /* Current stack limits must allow pushing 6 bytes, else #SS(0)
           * If interrupt was caused by fault with error code the
           * stack limits must allow push of 2 more bytes, else #SS(0) */
          if ( is_error_code )
            bytes = 8;
          else
            bytes = 6;

          if ( !bx_can_push(&bx_cpu.ss.cache, SP, bytes) ) {
            bx_panic("interrupt(): stack doesn't have room\n");
            bx_exception(BX_SS_EXCEPTION, 0, 0);
            return;
            }

          /* IP must be in CS limit else #GP(0) */
          if (gate_descriptor.u.gate286.dest_offset >
              cs_descriptor.u.segment.limit_scaled) {
            bx_panic("interrupt(): IP > cs descriptor limit\n");
            /* #GP(0) */
            return;
            }

          /* push flags onto stack */
          /* push current CS selector onto stack */
          /* push return offset onto stack */
          bx_push_16(bx_read_flags());
          bx_push_16(bx_cpu.cs.selector.value);
          bx_push_16(IP);

          /* load CS:IP from gate */
          /* load CS descriptor */
          /* set the RPL field of CS to CPL */
          bx_load_cs(&cs_selector, &cs_descriptor, CPL);
          bx_cpu.eip = gate_descriptor.u.gate286.dest_offset;

          /* push error code (if any) onto stack */
          /* (do this in segment_exception() ) */

          /* if interrupt gate then set IF to 0 */
          /* set the trap flag to 0 */
          /* set the nested task flag to 0 */
          if ( gate_descriptor.type==6 )
            bx_cpu.eflags.if_ = 0;
          bx_cpu.eflags.tf = 0;
          bx_cpu.eflags.nt = 0;

          if (bx_dbg.interrupts)
            bx_printf("int_trap_gate286(): INTERRUPT TO SAME PRIVILEGE succeeded\n");
          return;
          }

        bx_printf("interrupt: bad descriptor\n");
        /* else #GP(CS selector + ext) */
        bx_printf("c_ed=%u, descriptor.dpl=%u, CPL=%u\n",
          (unsigned) cs_descriptor.u.segment.c_ed,
          (unsigned) cs_descriptor.dpl,
          (unsigned) CPL);
        bx_panic("cs.segment = %u\n", (unsigned) cs_descriptor.segment);
        bx_exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
        return;

        break; 

      case 14: /* 386 interrupt gate */
      case 15: /* 386 trap gate */
        if (gate_descriptor.type==14) {
          if (bx_dbg.interrupts)
            bx_printf("interrupt(): interrupt gate\n");
          }
        else
          bx_panic("interrupt(): trap gate 32\n");

        /* TRAP-OR-INT-GATE */

        /* examine CS selector and descriptor given in gate descriptor */
        /* selector must be non-null else #GP(EXT) */
        if ( (gate_descriptor.u.gate386.dest_selector & 0xfffc) == 0 ) {
          bx_panic("int_trap_gate386(): selector null\n");
          /* GP(ext) */
          return;
          }

        bx_parse_selector(gate_descriptor.u.gate386.dest_selector,
                          &cs_selector);

        /* selector must be within its descriptor table limits
         * else #GP(selector+EXT) */
        bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
                                BX_GP_EXCEPTION);
        if (bx_cpu.errno > prev_errno) {
          bx_panic("interrrupt(): trap_gate386():"
                       " fetch_raw_descriptor failed\n");
          return;
          }

        bx_parse_descriptor(dword1, dword2, &cs_descriptor);

        /* descriptor AR byte must indicate code seg, else #GP(selector+EXT) */
        if ( cs_descriptor.valid==0 ||
             cs_descriptor.segment==0 ||
             cs_descriptor.u.segment.executable==0 ) {
          bx_panic("interrupt(): not code segment\n");
          /* GP(selector + ext) */
          return;
          }

        /* segment must be present, else #NP(selector + EXT) */
        if ( cs_descriptor.p==0 ) {
          bx_panic("interrupt(): segment not present\n");
          /* NP(selector + EXT) */
          return;
          }

        /* if code segment is non-conforming AND DPL < CPL then
         * INTERRUPT TO INNER PRIVILEGE: */
/* ??? use cs_descriptor.dpl or gate.dpl */
        if ( cs_descriptor.u.segment.c_ed==0 && cs_descriptor.dpl<CPL ) {
          Bit16u old_SS, old_CS, SS_for_cpl_x;
          Bit32u old_ESP, old_EIP, ESP_for_cpl_x;
          bx_descriptor_t ss_descriptor;
          bx_selector_t   ss_selector;

          if (bx_dbg.interrupts)
            bx_printf("interrupt(): INTERRUPT TO INNER PRIVILEGE\n");

          /* check selector and descriptor for new stack in current
           * Task State Segment (TSS) */
          
          if (bx_cpu.tr.cache.valid==0)
            bx_panic("interrupt(): TR.cache invalid\n");
          if (bx_cpu.tr.cache.type!=9)
            bx_panic("interrupt(): gate386: TR.type(%u)!=9\n",
              (unsigned) bx_cpu.tr.cache.type);

/* ??? use cs_descriptor.dpl or gate.dpl */
          /* check selector and descriptor for new stack in current TSS */
          bx_access_linear(bx_cpu.tr.cache.u.tss386.base + 8 +
            8*cs_descriptor.dpl, 2, 0, BX_READ, &SS_for_cpl_x);
          bx_access_linear(bx_cpu.tr.cache.u.tss386.base + 4 +
            8*cs_descriptor.dpl, 4, 0, BX_READ, &ESP_for_cpl_x);

          /* Selector must be non-null else #TS(EXT) */
          if ( (SS_for_cpl_x & 0xfffc) == 0 ) {
            bx_panic("interrupt(): SS selector null\n");
            /* TS(ext) */
            return;
            }

          /* selector index must be within its descriptor table limits
           * else #TS(SS selector + EXT) */
          bx_parse_selector(SS_for_cpl_x, &ss_selector);
          /* fetch 2 dwords of descriptor; call handles out of limits checks */
          bx_fetch_raw_descriptor(&ss_selector, &dword1, &dword2,
                                  BX_TS_EXCEPTION);
          if (bx_cpu.errno > prev_errno) {
            bx_panic("interrupt(): SS fetch_raw_descriptor failed\n");
            return;
            }
          bx_parse_descriptor(dword1, dword2, &ss_descriptor);

          /* selector rpl must = dpl of code segment,
           * else #TS(SS selector + ext)*/
          if (ss_selector.rpl != cs_descriptor.dpl) {
            bx_panic("interrupt(): SS.rpl != CS.dpl\n");
            /* TS(SS selector) */
            return;
            }

          /* stack seg DPL must = DPL of code segment,
           * else #TS(SS selector + ext) */
          if (ss_descriptor.dpl != cs_descriptor.dpl) {
            bx_panic("interrupt(): SS.dpl != CS.dpl\n");
            /* TS(SS selector) */
            return;
            }

          /* descriptor must indicate writable data segment,
           * else #TS(SS selector + EXT) */
          if (ss_descriptor.valid==0 ||
              ss_descriptor.segment==0  ||
              ss_descriptor.u.segment.executable==1  ||
              ss_descriptor.u.segment.r_w==0) {
            bx_panic("interrupt(): SS not writable data segment\n");
            /* TS(SS selector) */
            return;
            }

          /* seg must be present, else #SS(SS selector + ext) */
          if (ss_descriptor.p==0) {
            bx_panic("interrupt(): SS not present\n");
            /* SS(SS selector) */
            return;
            }

          /* new stack must have room for 20 bytes, else #SS(0) */
          if ( !bx_can_push(&ss_descriptor, ESP_for_cpl_x, 20) ) {
            bx_panic("interrupt(): new stack doesn't have room for 10\n");
            /* SS(0) */
            return;
            }

          /* EIP must be within CS segment boundaries, else #GP(0) */
          if (gate_descriptor.u.gate386.dest_offset >
              cs_descriptor.u.segment.limit_scaled) {
            bx_panic("interrupt(): EIP > CS.limit\n");
            /* GP(0) */
            return;
            }

          old_ESP = ESP;
          old_SS  = bx_cpu.ss.selector.value;
          old_EIP = EIP;
          old_CS  = bx_cpu.cs.selector.value;

          /* load new SS:ESP values from TSS */
          /* load SS descriptor */
          bx_load_ss(&ss_selector, &ss_descriptor, cs_descriptor.dpl);
          bx_cpu.esp = ESP_for_cpl_x;

          /* load new CS:EIP values from gate */
          /* load CS descriptor */
          /* set CPL to new code segment DPL */
          /* set RPL of CS to CPL */
          bx_load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);
          bx_cpu.eip = gate_descriptor.u.gate386.dest_offset;

          if (bx_v8086_mode()) {
            bx_push_32(bx_cpu.gs.selector.value);
            bx_push_32(bx_cpu.fs.selector.value);
            bx_push_32(bx_cpu.ds.selector.value);
            bx_push_32(bx_cpu.es.selector.value);
            bx_cpu.gs.cache.valid = 0;
            bx_cpu.fs.cache.valid = 0;
            bx_cpu.ds.cache.valid = 0;
            bx_cpu.es.cache.valid = 0;
            }

          /* push long pointer to old stack onto new stack */
          bx_push_32((Bit32u) old_SS);
          bx_push_32(old_ESP);

          /* ??? push eflags */
          bx_push_32(bx_read_eflags());

          /* push long pointer to return address onto new stack */
          bx_push_32((Bit32u) old_CS);
          bx_push_32(old_EIP);
          if (bx_cpu.errno > prev_errno) {
            bx_panic("interrupt(): error pushing values\n");
            return;
            }

          /* if INTERRUPT GATE set IF to 0 */
          /* set TF to 0 */
          /* set NT to 0 */
          if ( gate_descriptor.type==14 )
            bx_cpu.eflags.if_ = 0;
          bx_cpu.eflags.tf = 0;
          bx_cpu.eflags.nt = 0;
          bx_cpu.eflags.vm = 0;

          if (bx_dbg.interrupts)
            bx_printf("interrupt(): INT TO INNER PRIVILEGE succeeding\n");
          return;
          }

        /* if code segment is conforming OR code segment DPL = CPL then
         * INTERRUPT TO SAME PRIVILEGE LEVEL: */
        if ( cs_descriptor.u.segment.c_ed==1 || cs_descriptor.dpl==CPL ) {
          int bytes;

          if (bx_dbg.interrupts)
            bx_printf("int_trap_gate386(): INTERRUPT TO SAME PRIVILEGE\n");
      
          /* Current stack limits must allow pushing 10 bytes, else #SS(0)
           * If interrupt was caused by fault with error code the
           * stack limits must allow push of 2 more bytes, else #SS(0) */
          if ( is_error_code )
            bytes = 14;
          else
            bytes = 10;

          if ( !bx_can_push(&bx_cpu.ss.cache, ESP, bytes) ) {
            bx_panic("interrupt(): stack doesn't have room\n");
            bx_exception(BX_SS_EXCEPTION, 0, 0);
            return;
            }

          /* EIP must be in CS limit else #GP(0) */
          if (gate_descriptor.u.gate386.dest_offset >
              cs_descriptor.u.segment.limit_scaled) {
            bx_panic("interrupt(): EIP > cs descriptor limit\n");
            /* #GP(0) */
            return;
            }

          /* push flags onto stack */
          /* push current CS selector onto stack */
          /* push return offset onto stack */
          bx_push_32(bx_read_eflags());
          bx_push_32((Bit32u) bx_cpu.cs.selector.value);
          bx_push_32(EIP);

          /* load CS:EIP from gate */
          /* load CS descriptor */
          /* set the RPL field of CS to CPL */
          bx_load_cs(&cs_selector, &cs_descriptor, CPL);
          bx_cpu.eip = gate_descriptor.u.gate386.dest_offset;

          /* push error code (if any) onto stack */
          /* (do this in segment_exception() ) */

          /* if interrupt gate then set IF to 0 */
          /* set the trap flag to 0 */
          /* set the nested task flag to 0 */
          if ( gate_descriptor.type==14 )
            bx_cpu.eflags.if_ = 0;
          bx_cpu.eflags.tf = 0;
          bx_cpu.eflags.nt = 0;

          if (bx_dbg.interrupts)
            bx_printf("int_trap_gate386(): INTERRUPT TO SAME PRIVILEGE succeeded\n");
          return;
          }

        /* else #GP(CS selector + ext) */
        bx_printf("interrupt: bad descriptor\n");
        bx_printf("c_ed=%u, descriptor.dpl=%u, CPL=%u\n",
          (unsigned) cs_descriptor.u.segment.c_ed,
          (unsigned) cs_descriptor.dpl,
          (unsigned) CPL);
        bx_panic("cs.segment = %u\n", (unsigned) cs_descriptor.segment);
        bx_exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
        return;
        break; 

      default:
        bx_panic("bad descriptor type in bx_interrupt()!\n");
        break;
      }
    }
  else
#endif
    { /* real mode */
    Bit16u cs, ip;

if (bx_cpu.idtr.base != 0) bx_panic("interrupt(real mode) nonzero base\n");
    if ( (vector*4+3) > bx_cpu.idtr.limit )
      bx_panic("interrupt(real mode) vector > limit\n");

    bx_push_16(bx_read_flags());

    cs = bx_cpu.cs.selector.value;
    bx_push_16(cs);
    ip = (Bit16u) bx_cpu.eip;
    bx_push_16(ip);

    bx_access_linear(bx_cpu.idtr.base + 4 * vector,     2, 0, BX_READ, &ip);
    bx_access_linear(bx_cpu.idtr.base + 4 * vector + 2, 2, 0, BX_READ, &cs);

    IP = ip;
    bx_load_seg_reg(&bx_cpu.cs, cs);

//#ifdef AUDIT_ENABLED
//  /* if we haven't done so, grab the int21 segment and offset for
//     shadow interrupt monitoring */
//  switch(vector)
//    {
//    case 0x21:
//      if ((!OrigIntVec[0].seg) && (cs < 0x1000))
//        {
///*        printf("system booted after %ld opcodes\n",revert); */
//          revert = 0L;
//          OrigIntVec[0].seg = cs;
//          OrigIntVec[0].ofs = ip;
///*        printf("Int 21h entry at %04X:%04X\n", cs, ip); */
//        }
//      break;
//    }
//#endif

    /* INT affects the following flags: I,T */
    bx_cpu.eflags.if_ = 0;
    bx_cpu.eflags.tf  = 0;
#if BX_CPU >= 4
    bx_cpu.eflags.ac  = 0;
#endif
    }
}




#if BX_CPU >= 2
  void
bx_task_gate(Bit16u selector, Boolean link)
{
  if (bx_v8086_mode()) bx_panic("task_gate: v8086 mode unsupported\n");

  bx_panic("bx_task_gate(): not implemented!\n");
}
#endif



  void
bx_exception(unsigned vector, Bit16u error_code, Boolean is_INT)
  /* vector:     0..255: vector in IDT
   * error_code: if exception generates and error, push this error code
   */
{
  Boolean  push_error;
  Bit8u    exception_type;
  unsigned prev_errno;

  if (bx_dbg.exceptions)
    bx_printf("exception(%02x h)\n", (unsigned) vector);

  bx_cpu.errno++;
  if (bx_cpu.errno >= 3) {
    bx_panic("exception(): 3rd exception with no resolution\n");
    }

  /* careful not to get here with exception[1]==DOUBLE_FAULT */
  /* ...index on DOUBLE_FAULT below, will be out of bounds */

  /* if 1st was a double fault (software INT?), then shutdown */
  if ( (bx_cpu.errno==2) && (exception[0]==BX_ET_DOUBLE_FAULT) ) {
    bx_panic("exception(): tripple fault encountered\n");
    }

  /* ??? this is not totally correct, should be done depending on
   * vector */
  /* backup IP to value before error occurred */
  bx_cpu.eip = bx_cpu.prev_eip;


  switch (vector) {
    case  0: /* DIV by 0 */
      push_error = 0;
      exception_type = BX_ET_CONTRIBUTORY;
      break;
    case  1: /* debug exceptions */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  2: /* NMI */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  3: /* breakpoint */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  4: /* overflow */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  5: /* bounds check */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  6: /* invalid opcode */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  7: /* device not available */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
    case  8: /* double fault */
      push_error = 1;
      exception_type = BX_ET_DOUBLE_FAULT;
      break;
    case  9: /* coprocessor segment overrun (286,386 only) */
      push_error = 0;
      exception_type = BX_ET_CONTRIBUTORY;
      bx_panic("exception(9): unfinished\n");
      break;
    case 10: /* invalid TSS */
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | bx_cpu.EXT;
      break;
    case 11: /* segment not present */
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | bx_cpu.EXT;
      break;
    case 12: /* stack fault */
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | bx_cpu.EXT;
      break;
    case 13: /* general protection */
      push_error = 1;
      exception_type = BX_ET_CONTRIBUTORY;
      error_code = (error_code & 0xfffe) | bx_cpu.EXT;
      break;
    case 14: /* page fault */
      push_error = 1;
      exception_type = BX_ET_PAGE_FAULT;
      /* ??? special format error returned */
      break;
    case 15: /* reserved */
      bx_panic("exception(15): reserved\n");
      push_error = 0;     /* keep compiler happy for now */
      exception_type = 0; /* keep compiler happy for now */
      break;
    case 16: /* floating-point error */
      push_error = 0;
      exception_type = BX_ET_BENIGN;
      break;
#if BX_CPU >= 4
    case 17: /* alignment check */
      bx_panic("exception(): alignment-check, vector 17 unimplemented\n");
      push_error = 0;     /* keep compiler happy for now */
      exception_type = 0; /* keep compiler happy for now */
      break;
#endif
#if BX_CPU >= 5
    case 18: /* machine check */
      bx_panic("exception(): machine-check, vector 18 unimplemented\n");
      push_error = 0;     /* keep compiler happy for now */
      exception_type = 0; /* keep compiler happy for now */
      break;
#endif
    default:
      bx_panic("exception(%u): bad vector\n", (unsigned) vector);
      push_error = 0;     /* keep compiler happy for now */
      exception_type = 0; /* keep compiler happy for now */
      break;
    }

  error_code = (error_code & 0xfffe) | bx_cpu.EXT;
  bx_cpu.EXT = 1;

  /* if we've already had 1st exception, see if 2nd causes a
   * Double Fault instead.  Otherwise, just record 1st exception
   */
  if (bx_cpu.errno >= 2) {
    if (is_exception_OK[exception[0]][exception_type])
      exception[1] = exception_type;
    else
      exception[1] = BX_ET_DOUBLE_FAULT;
    }
  else {
    exception[0] = exception_type;
    }


#if BX_CPU >= 2
  if (!bx_real_mode()) {
    prev_errno = bx_cpu.errno;
    bx_interrupt(vector, 0, push_error);
    if (bx_cpu.errno > prev_errno) {
      bx_printf("segment_exception(): errno changed\n");
      return;
      }

    if (push_error) {
      /* push error code on stack, after handling interrupt */
      /* pushed as a word or dword depending upon default size ??? */
      if (bx_cpu.ss.cache.u.segment.d_b)
        bx_push_32((Bit32u) error_code); /* upper bits reserved */
      else
        bx_push_16(error_code);
      if (bx_cpu.errno > prev_errno) {
        bx_panic("segment_exception(): errno changed\n");
        return;
        }
      }
    }
  else /* real mode */
#endif
    {
    if (vector!=6 && vector!=7 && vector!=13 && vector!=1)
      bx_panic("exception %u occurred in REAL mode\n", (unsigned) vector);
    bx_interrupt(vector, 0, 0); /* not INT, no error code pushed */
    }
}


  int
bx_int_number(bx_segment_reg_t *seg)
{
  if (seg == &bx_cpu.ss)
    return(BX_SS_EXCEPTION);
  else
    return(BX_GP_EXCEPTION);
}

  void
bx_shutdown_cpu(void)
{
  Bit32u  dword1, dword2;
  bx_descriptor_t gate_descriptor;
  /*bx_selector_t cs_selector;*/
  int vector;
  Bit16u ip, cs;
  
#if BX_CPU > 2
  bx_panic("shutdown_cpu(): not implemented for 386\n");
#endif

  if (bx_real_mode()) {
    bx_panic("shutdown_cpu(): don't know how to handle real mode\n");
    }

    vector = 2;
    bx_access_physical(4 * vector,     2, BX_READ, &ip);
    bx_access_physical(4 * vector + 2, 2, BX_READ, &cs);
    bx_printf("real mode vector 2 to CS:IP = %04x:%04x\n",
      (unsigned) cs, (unsigned) ip);

    vector = 8;
    bx_access_physical(4 * vector,     2, BX_READ, &ip);
    bx_access_physical(4 * vector + 2, 2, BX_READ, &cs);
    bx_printf("real mode vector 8 to CS:IP = %04x:%04x\n",
      (unsigned) cs, (unsigned) ip);

    vector = 2;
    /* descriptor AR byte must indicate interrupt gate, trap gate,
       or task gate, else #GP(vector*8 + 2 + EXT)*/
    bx_access_physical(bx_cpu.idtr.base + vector*8,     4,
      BX_READ, &dword1);
    bx_access_physical(bx_cpu.idtr.base + vector*8 + 4, 4,
      BX_READ, &dword2);

    bx_parse_descriptor(dword1, dword2, &gate_descriptor);

bx_printf("gate.segment = %u\n", (unsigned) gate_descriptor.segment);
bx_printf("gate.type    = %u\n", (unsigned) gate_descriptor.type);
  bx_panic("shutting down CPU due to unrecoverable errors\n");
}
