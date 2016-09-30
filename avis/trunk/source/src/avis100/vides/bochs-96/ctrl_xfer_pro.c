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





#if BX_CPU >= 2
  void
bx_jump_protected(Bit16u cs, Bit32u disp32)
{
  bx_descriptor_t  descriptor;
  bx_selector_t    selector;
  Bit32u dword1, dword2;


  if (bx_v8086_mode()) bx_panic("ctrl_xfer_pro: v8086 mode unsupported\n");

  if (bx_dbg.protected) bx_printf("jump_protected():\n");

  /* destination selector is not null else #GP(0) */
  if ((cs & 0xfffc) == 0) {
    bx_panic("jump_protected: cs == 0\n");
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  bx_parse_selector(cs, &selector);

  /* destination selector index is whithin its descriptor table
     limits else #GP(selector) */
  bx_fetch_raw_descriptor(&selector, &dword1, &dword2, 
    BX_GP_EXCEPTION);
  BX_HANDLE_EXCEPTION()

  /* examine AR byte of destination selector for legal values: */
  bx_parse_descriptor(dword1, dword2, &descriptor);

  if ( descriptor.segment ) {
    if ( descriptor.u.segment.executable==0 ) {
      bx_panic("jump_protected: S=1: descriptor not executable\n");
      bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
      return;
      }
    /* CASE: JUMP CONFORMING CODE SEGMENT: */
    if ( descriptor.u.segment.c_ed ) {
      /* descripor DPL must be <= CPL else #GP(selector) */
      if (descriptor.dpl > CPL) {
        bx_panic("jump_protected: dpl > CPL\n");
        bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }
  
      /* segment must be PRESENT else #NP(selector) */
      if (descriptor.p == 0) {
        bx_panic("jump_protected: p == 0\n");
        bx_exception(BX_NP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }
  
      /* instruction pointer must be in code segment limit else #GP(0) */
      if (disp32 > descriptor.u.segment.limit_scaled) {
        bx_panic("jump_protected: IP > limit\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
  
      /* Load CS:IP from destination pointer */
      /* Load CS-cache with new segment descriptor */
      /* CPL does not change for conforming code segment */
      bx_load_cs(&selector, &descriptor, CPL);
      bx_cpu.eip = disp32;
      if (bx_dbg.protected)
        bx_printf("jump_protected (Beta) (protected mode):"
                     " conforming code segment.\n");
      return;
      }
  
    /* CASE: JUMP NONCONFORMING CODE SEGMENT: */
    else {
      /* RPL of destination selector must be <= CPL else #GP(selector) */
      if (selector.rpl > CPL) {
        bx_panic("jump_protected: rpl > CPL\n");
        bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }
  
      /* descriptor DPL must = CPL else #GP(selector) */
      if (descriptor.dpl != CPL) {
        bx_panic("jump_protected: dpl != CPL\n");
        bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }
  
      /* segment must be PRESENT else #NP(selector) */
      if (descriptor.p == 0) {
        bx_panic("jump_protected: p == 0\n");
        bx_exception(BX_NP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }
  
      /* IP must be in code segment limit else #GP(0) */
      if (disp32 > descriptor.u.segment.limit_scaled) {
        bx_panic("jump_protected: IP > limit\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
  
      /* load CS:IP from destination pointer */
      /* load CS-cache with new segment descriptor */
      /* set RPL field of CS register to CPL */
      bx_load_cs(&selector, &descriptor, CPL);
      bx_cpu.eip = disp32;
      if (bx_dbg.protected)
        bx_printf("jump_protected: non-conforming code segment.\n");
      return;
      }
    bx_panic("jump_protected: segment=1\n");
    }

  else {
    Bit16u          raw_tss_selector;
    bx_selector_t   tss_selector;
    bx_descriptor_t tss_descriptor;

    switch ( descriptor.type ) {
      case  1: /* 286 available TSS */
bx_panic("jump_pro: 16bit TSS\n");
        /* TSS DPL must be >= CPL, else #GP(TSS selector) */
        if (descriptor.dpl < CPL) {
          bx_panic("jump_protected: TSS.dpl < CPL\n");
          bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
          return;
          }

        /* TSS DPL must be >= TSS selector RPL, else #GP(TSS selector) */
        if (descriptor.dpl < selector.rpl) {
          bx_panic("jump_protected: TSS.dpl < selector.rpl\n");
          bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
          return;
          }

        /* descriptor AR byte must specify available TSS,
         *   else #GP(TSS selector) */
        /* this is taken care of by the 'default' case of switch statement */

        /* Task State Seg must be present, else #NP(TSS selector) */
        if (descriptor.p==0) {
          bx_printf("jump_protected: TSS.p == 0\n");
          bx_exception(BX_NP_EXCEPTION, cs & 0xfffc, 0);
          return;
          }

        /* SWITCH_TASKS _without_ nesting to TSS */
        bx_task_switch16(&selector, &descriptor,
          BX_TASK_FROM_JUMP, dword1, dword2);

        /* IP must be in code seg limit, else #GP(0) */
        if (EIP > bx_cpu.cs.cache.u.segment.limit) {
          bx_printf("jump_protected: TSS.p == 0\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        bx_printf("jump_protected: JUMP TO 286 TSS: succeeded\n");
        return;
        break;
      case  4: /* 286 call gate */
        bx_panic("jump_protected: JUMP TO 286 CALL GATE:\n");
        return;
        break;
      case  5: /* task gate */
bx_panic("jump_pro: task gate\n");
        /* gate descriptor DPL must be >= CPL else #GP(gate selector) */
        if (descriptor.dpl < CPL) {
          bx_panic("jump_protected: gate.dpl < CPL\n");
          bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
          return;
          }

        /* gate descriptor DPL must be >= gate selector RPL
         *   else #GP(gate selector) */
        if (descriptor.dpl < selector.rpl) {
          bx_panic("jump_protected: gate.dpl < selector.rpl\n");
          bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
          return;
          }

        /* task gate must be present else #NP(gate selector) */
        if (descriptor.p==0) {
          bx_panic("jump_protected: task gate.p == 0\n");
          bx_exception(BX_NP_EXCEPTION, cs & 0xfffc, 0);
          return;
          }

        /* examine selector to TSS, given in Task Gate descriptor */
        /* must specify global in the local/global bit else #GP(TSS selector) */

        raw_tss_selector = descriptor.u.gate286.dest_selector;
        bx_parse_selector(raw_tss_selector, &tss_selector);
        if (tss_selector.ti) {
          bx_panic("jump_protected: tss_selector.ti=1\n");
          bx_exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }

        /* index must be within GDT limits else #GP(TSS selector) */
        bx_fetch_raw_descriptor(&tss_selector, &dword1, &dword2, 
          BX_GP_EXCEPTION);
        BX_HANDLE_EXCEPTION()

        /* descriptor AR byte must specify available TSS
         *   else #GP(TSS selector) */
        bx_parse_descriptor(dword1, dword2, &tss_descriptor);
        if (tss_descriptor.valid==0 || tss_descriptor.segment) {
          bx_panic("jump_protected: TSS selector points to bad TSS\n");
          bx_exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }
        if (tss_descriptor.type!=9 && tss_descriptor.type!=1) {
          bx_panic("jump_protected: TSS selector points to bad TSS\n");
          bx_exception(BX_GP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }


        /* Task State Segment must be present else #NP(TSS selector) */
        if (tss_descriptor.p==0) {
          bx_panic("jump_protected: TSS.p==0\n");
          bx_exception(BX_NP_EXCEPTION, raw_tss_selector & 0xfffc, 0);
          return;
          }

        /* SWITCH_TASKS _without_ nesting to TSS */
        if (tss_descriptor.type==9) { /* 32bit available TSS */
          bx_task_switch32(&tss_selector, &tss_descriptor,
            BX_TASK_FROM_JUMP, dword1, dword2);
          /* EIP must be in code segment limit else #GP(0) */
          bx_panic("jump_protected: JUMP TO TASK GATE 32bit:\n");
          return;
          }
        else { /* type==1, 16bit available TSS */
          bx_task_switch16(&tss_selector, &tss_descriptor,
            BX_TASK_FROM_JUMP, dword1, dword2);
          /* IP must be in code segment limit else #GP(0) */
          bx_panic("jump_protected: JUMP TO TASK GATE 16bit:\n");
          return;
          }
        break;
      case  9: /* 386 available TSS */
        bx_panic("jump_protected: JUMP TO 386 TSS:\n");
        return;
        break;
      case 12: /* 386 call gate */
        bx_panic("jump_protected: JUMP TO 386 CALL GATE:\n");
        return;
        break;
      default:
        bx_panic("jump_protected: gate type %u unsupported\n",
          (unsigned) descriptor.type);
        bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
        return;
        break;
      }
    }
    return;
}
#endif /* if BX_CPU >= 2 */


#if BX_CPU >= 2
  void
bx_call_protected(Bit16u cs, Bit32u disp32)
{
  bx_selector_t cs_selector;
  Bit32u dword1, dword2;
  bx_descriptor_t cs_descriptor;

  if (bx_v8086_mode()) bx_panic("ctrl_xfer_pro: v8086 mode unsupported\n");

  /* Opsize in effect for CALL is specified by the D bit for the
   * segment containing dest & by any opsize prefix.
   * For gate descriptor, deterermined by type of call gate:
   * 4=16bit, 12=32bit
   * count field: 16bit specifies #words, 32bit specifies #dwords
   */

  if (bx_dbg.protected) bx_printf("call_protected()\n");

  /* new cs selector must not be null, else #GP(0) */
  if ( (cs & 0xfffc) == 0 ) {
    bx_panic("call_protected: CS selector null\n");
    bx_exception(BX_GP_EXCEPTION, 0, 0);
    return;
    }

  bx_parse_selector(cs, &cs_selector);

  /* check new CS selector index within its descriptor limits,
   * else #GP(new CS selector) */
  bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2, 
    BX_GP_EXCEPTION);
  BX_HANDLE_EXCEPTION()

  bx_parse_descriptor(dword1, dword2, &cs_descriptor);

  /* examine AR byte of selected descriptor for various legal values */
  if (cs_descriptor.valid==0) {
    bx_panic("call_protected: invalid CS descriptor\n");
    bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
    return;
    }

  if (cs_descriptor.segment) { /* normal segment */
    if (cs_descriptor.u.segment.executable==0) {
      bx_panic("call_protected: non executable segment\n");
      bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
      return;
      }
    if (cs_descriptor.u.segment.c_ed) { /* CONFORMING CODE SEGMENT */
      bx_panic("call_protected: conforming code segment unfinished\n");
      return;
      }
    else { /* NON-CONFORMING CODE SEGMENT */
      Bit32u temp_ESP;

      if (bx_dbg.protected)
        bx_printf("call_protected: non-conforming code segment\n");

      /* RPL must be <= CPL, else #GP(code seg selector) */
      if (cs_selector.rpl > CPL) {
        bx_panic("call_protected: cs.rpl > CPL\n");
        bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }

      /* DPL must be = CPL, else #GP(code seg selector) */
      if (cs_descriptor.dpl != CPL) {
        bx_panic("call_protected: cs.dpl != CPL\n");
        bx_exception(BX_GP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }

      /* segment must be present, else #NP(code seg selector) */
      if (cs_descriptor.p == 0) {
        bx_printf("call_protected: cs.p = 0\n");
        bx_exception(BX_NP_EXCEPTION, cs & 0xfffc, 0);
        return;
        }

      if (bx_cpu.ss.cache.u.segment.d_b)
        temp_ESP = ESP;
      else
        temp_ESP = SP;

      /* stack must be big enough for return addr, else #SS(0) */
      if (bx_cpu.is_32bit_opsize) {
        if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, 8) ) {
          bx_panic("call_protected: stack doesn't have room for ret addr\n");
          bx_exception(BX_SS_EXCEPTION, 0, 0);
          return;
          }

        /* IP must be in code seg limit, else #GP(0) */
        if (disp32 > cs_descriptor.u.segment.limit_scaled) {
          bx_panic("call_protected: IP not in code seg limit\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }

        /* push return address onto stack */
        bx_push_32((Bit32u) bx_cpu.cs.selector.value); /* padded to 32bits */
        bx_push_32(EIP);
        if (bx_cpu.errno)
          bx_panic("call_protected: push return address failed.\n");
        }
      else { /* 16bit opsize */
        if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, 4) ) {
          bx_panic("call_protected: stack doesn't have room for ret addr\n");
          bx_exception(BX_SS_EXCEPTION, 0, 0);
          return;
          }

        /* IP must be in code seg limit, else #GP(0) */
        if (disp32 > cs_descriptor.u.segment.limit_scaled) {
          bx_panic("call_protected: IP not in code seg limit\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }

        bx_push_16(bx_cpu.cs.selector.value);
        bx_push_16(IP);
        if (bx_cpu.errno)
          bx_panic("call_protected: push return address failed.\n");
        }

      /* load code segment descriptor into CS cache */
      /* load CS with new code segment selector */
      /* set RPL of CS to CPL */
      /* load eIP with new offset */
      bx_load_cs(&cs_selector, &cs_descriptor, CPL);
      bx_cpu.eip = disp32;
      if (bx_cpu.is_32bit_opsize==0)
        bx_cpu.eip &= 0x0000ffff;

      if (bx_dbg.protected)
        bx_printf("call_protected: call to non-conforming code segment"
                  " succeeds\n");
      return;
      }
    return;
    }
  else { /* gate & special segment */
    bx_descriptor_t  gate_descriptor;
    bx_selector_t    gate_selector;
    Bit16u new_IP;

    /* 1 level of indirection via gate, switch gate & cs */
    gate_descriptor = cs_descriptor;
    gate_selector   = cs_selector;

    switch (gate_descriptor.type) {
      case 1: /* available 16bit TSS */
        bx_panic("call_protected: 16bit available TSS\n");
        break;
      case 4: /* 16bit CALL GATE */
        if (bx_dbg.protected)
          bx_printf("call_protected: call gate\n");
        /* call gate DPL must be >= CPL, else #GP(call gate selector) */
        if (gate_descriptor.dpl < CPL) {
          bx_panic("call_protected: DPL < CPL\n");
          bx_exception(BX_GP_EXCEPTION, gate_selector.value & 0xfffc, 0);
          return;
          }

        /* call gate DPL must be >= RPL, else #GP(call gate selector) */
        if ( gate_descriptor.dpl < gate_selector.rpl ) {
          bx_panic("call_protected: DPL < CS.RPL\n");
          bx_exception(BX_GP_EXCEPTION, gate_selector.value & 0xfffc, 0);
          return;
          }

        /* call gate must be present, else #NP(call gate selector) */
        if (gate_descriptor.p==0) {
          bx_panic("call_protected: not present\n");
          bx_exception(BX_NP_EXCEPTION, gate_selector.value & 0xfffc, 0);
          return;
          }

        /* examine code segment selector in call gate descriptor */

        /* selector must not be null else #GP(0) */
        if ( (gate_descriptor.u.gate286.dest_selector & 0xfffc) == 0 ) {
          bx_panic("call_protected: selector in gate null\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        
        bx_parse_selector(gate_descriptor.u.gate286.dest_selector,
                          &cs_selector);

        /* selector must be within its descriptor table limits,
         * else #GP(code segment selector) */
        bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
          BX_GP_EXCEPTION);
        BX_HANDLE_EXCEPTION()
        bx_parse_descriptor(dword1, dword2, &cs_descriptor);

        /* AR byte of selected descriptor must indicate code segment,
         * else #GP(code segment selector) */
        if (cs_descriptor.valid==0 ||
            cs_descriptor.segment==0 ||
            cs_descriptor.u.segment.executable==0) {
          bx_panic("call_protected: selected desciptor not code\n");
          bx_exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
          return;
          }

        /* DPL of selected descriptor must be <= CPL,
         * else #GP(code segment selector) */
        if (cs_descriptor.dpl > CPL) {
          bx_panic("call_protected: DPL selected descr > CPL\n");
          bx_exception(BX_GP_EXCEPTION, cs_selector.value & 0xfffc, 0);
          return;
          }

        /* CALL GATE TO MORE PRIVILEGE */
        /* if non-conforming code segment and DPL < CPL then */
        /* ??? use gate_descriptor.dpl or cs_descriptor.dpl ??? */
        if ( (cs_descriptor.u.segment.c_ed==0)  &&
             (cs_descriptor.dpl < CPL) ) {

          Bit16u SS_for_cpl_x, SP_for_cpl_x;
          bx_selector_t   ss_selector;
          bx_descriptor_t ss_descriptor;
          Bit16u room_needed, word_count;
          Bit16u old_SS, old_SP, return_CS, return_IP;
          int i;
          Bit16u parameter_word[32];
          Bit32u temp_ESP;

          if (bx_dbg.protected)
            bx_printf("         call gate to more privilege\n");

          /* get new SS selector for new privilege level from TSS */
          if (bx_cpu.tr.cache.valid==0) {
            bx_panic("call_protected: TR.cache invalid\n");
            return;
            }
/* ??? use dpl or rpl ??? */
          bx_access_linear(bx_cpu.tr.cache.u.tss286.base + 4 +
            4*cs_descriptor.dpl, 2, 0, BX_READ, &SS_for_cpl_x);
          bx_access_linear(bx_cpu.tr.cache.u.tss286.base + 2 +
            4*cs_descriptor.dpl, 2, 0, BX_READ, &SP_for_cpl_x);

          /* check selector & descriptor for new SS: */
          /* selector must not be null, else #TS(0) */
          if ( (SS_for_cpl_x & 0xfffc) == 0 ) {
            bx_panic("call_protected: new SS null\n");
            bx_exception(BX_TS_EXCEPTION, 0, 0);
            return;
            }

          /* selector index must be within its descriptor table limits,
           * else #TS(SS selector) */
          bx_parse_selector(SS_for_cpl_x, &ss_selector);
          bx_fetch_raw_descriptor(&ss_selector, &dword1, &dword2,
            BX_TS_EXCEPTION);
          BX_HANDLE_EXCEPTION()

          bx_parse_descriptor(dword1, dword2, &ss_descriptor);

          /* selector's RPL must equal DPL of code segment,
           * else #TS(SS selector) */
          if (ss_selector.rpl != cs_descriptor.dpl) {
            bx_panic("call_protected: SS selector.rpl != CS descr.dpl\n");
            bx_exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          /* stack segment DPL must equal DPL of code segment,
           * else #TS(SS selector) */
          if (ss_descriptor.dpl != cs_descriptor.dpl) {
            bx_panic("call_protected: SS descr.rpl != CS descr.dpl\n");
            bx_exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          /* descriptor must indicate writable data segment,
           * else #TS(SS selector) */
          if (ss_descriptor.valid==0 ||
              ss_descriptor.segment==0  ||
              ss_descriptor.u.segment.executable ||
              ss_descriptor.u.segment.r_w==0) {
            bx_panic("call_protected: ss descriptor not writable data seg\n");
            bx_exception(BX_TS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          /* segment must be present, else #SS(SS selector) */
          if (ss_descriptor.p==0) {
            bx_panic("call_protected: ss descriptor not present.\n");
            bx_exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          /* get word count from call gate, mask to 5 bits */
          word_count = gate_descriptor.u.gate286.word_count & 0x1f;

          /* new stack must have room for parameters plus 8 bytes,
           * else #SS(SS selector) */
          room_needed = word_count*2 + 8;
          if ( !bx_can_push(&ss_descriptor, SP_for_cpl_x, room_needed) ) {
            bx_panic("call_protected: stack doesn't have room\n");
            bx_exception(BX_SS_EXCEPTION, SS_for_cpl_x & 0xfffc, 0);
            return;
            }

          /* IP must be in code segment limit else #GP(0) */
          if ( gate_descriptor.u.gate286.dest_offset >
               cs_descriptor.u.segment.limit_scaled ) {
            bx_panic("call_protected: IP not within CS limits\n");
            bx_exception(BX_GP_EXCEPTION, 0, 0);
            return;
            }


          old_SS = bx_cpu.ss.selector.value;
          old_SP = SP;
          return_CS = bx_cpu.cs.selector.value;
          return_IP = IP;

          if (bx_dbg.protected)
            bx_printf("call_protected: copying %u words from old stack\n",
              (unsigned) word_count);

          if (bx_cpu.ss.cache.u.segment.d_b)
            temp_ESP = ESP;
          else
            temp_ESP = SP;

          for (i=0; i<word_count; i++) {
            bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + i*2,
              2, 0, BX_READ, &parameter_word[i]);
            }

          /* load new SS:SP value from TSS */
          /* load SS descriptor */
          bx_load_ss(&ss_selector, &ss_descriptor, ss_descriptor.dpl);
          SP = SP_for_cpl_x;

          /* load new CS:IP value from gate */
          /* load CS descriptor */
          /* set CPL to stack segment DPL */
          /* set RPL of CS to CPL */
          bx_load_cs(&cs_selector, &cs_descriptor, cs_descriptor.dpl);
          bx_cpu.eip = gate_descriptor.u.gate286.dest_offset;

          /* push long pointer of old stack onto new stack */
          bx_push_16(old_SS);
          bx_push_16(old_SP);
          if (bx_cpu.errno) {
            bx_panic("call_protected: error pushing long pointer of old stack\n");
            }

          /* get word count from call gate, mask to 5 bits */
          /* copy parameters from old stack onto new stack */
          for (i=word_count; i>0; i--) {
            bx_push_16(parameter_word[i-1]);
            }
          if (bx_cpu.errno)
            bx_panic("call_protected: error copying parameters!\n");

          /* push return address onto new stack */
          bx_push_16(return_CS);
          bx_push_16(return_IP);
          if (bx_cpu.errno)
            bx_panic("call_protected: error pushing return address.\n");


          if (bx_dbg.protected)
            bx_printf("call gate to more priv succeeding\n");
          return;
          }

        /* CALL GATE TO SAME PRIVILEGE */
        else {
          Bit32u temp_ESP;

          if (bx_cpu.ss.cache.u.segment.d_b)
            temp_ESP = ESP;
          else
            temp_ESP = SP;

          /* stack must have room for 4-byte return address,
           * else #SS(0) */
          if ( !bx_can_push(&bx_cpu.ss.cache, temp_ESP, 4) ) {
            bx_panic("call_protected: stack doesn't have room for 4 bytes\n");
            bx_exception(BX_SS_EXCEPTION, 0, 0);
            return;
            }

          /* IP must be within code segment limit, else #GP(0) */
          new_IP = gate_descriptor.u.gate286.dest_offset;
          if ( new_IP > cs_descriptor.u.segment.limit_scaled ) {
            bx_panic("call_protected: IP not within code segment limits\n");
            bx_exception(BX_GP_EXCEPTION, 0, 0);
            return;
            }

          /* push return address onto stack */
          bx_push_16(bx_cpu.cs.selector.value);
          bx_push_16(IP);
          if (bx_cpu.errno)
            bx_panic("call_protected: push return address failed.\n");

          /* load CS:IP from gate */
          /* load code segment descriptor into CS register */
          /* set RPL of CS to CPL */
          bx_load_cs(&cs_selector, &cs_descriptor, CPL);
          EIP = new_IP;

          if (bx_dbg.protected)
            bx_printf("call gate to same priv succeeding (Beta)\n");
          return;
          }

        bx_panic("call_protected: 16bit call gate unfinished\n");
        return;
      case 5: /* TASK GATE */
        bx_panic("call_protected: task gate unfinished\n");
        return;
      case 9: /* available 32bit TSS */
        bx_panic("call_protected: 32bit available TSS\n");
        break;
      case 12: /* 32bit CALL GATE */
        bx_panic("call_protected: 32bit call gate unfinished\n");
        return;
      default:
        bx_panic("call_protected: type = %d\n",
          (unsigned) cs_descriptor.type);
        return;
      }
    bx_panic("call_protected: gate segment unfinished\n");
    }

  bx_panic("call_protected: shouldn't get here!\n");
  return;
}
#endif /* 286+ */


#if BX_CPU >= 2
  void
bx_return_protected(Bit16u pop_bytes)
{
  Bit16u raw_cs_selector, raw_ss_selector;
  bx_selector_t cs_selector, ss_selector;
  bx_descriptor_t cs_descriptor, ss_descriptor;
  Bit32u stack_cs_offset, stack_param_offset;
  Bit32u return_EIP, return_ESP, temp_ESP;
  Bit32u dword1, dword2;
  Bit16u return_IP, return_SP;

  if (bx_v8086_mode()) bx_panic("ctrl_xfer_pro: v8086 mode unsupported\n");


  /* + 6+N*2: SS      | +12+N*4:     SS */
  /* + 4+N*2: SP      | + 8+N*4:    ESP */
  /*          parm N  | +        parm N */
  /*          parm 3  | +        parm 3 */
  /*          parm 2  | +        parm 2 */
  /*          parm 1  | + 8:     parm 1 */
  /* + 2:     CS      | + 4:         CS */
  /* + 0:     IP      | + 0:        EIP */

#if BX_CPU >= 3
  if ( bx_cpu.is_32bit_opsize ) {
    /* operand size=32: third word on stack must be within stack limits,
     *   else #SS(0); */
    if (!bx_can_pop(6)) {
      bx_panic("return_protected: 3rd word not in stack limits\n");
      /* #SS(0) */
      return;
      }
    stack_cs_offset = 4;
    stack_param_offset = 8;
    }
  else
#endif
    {
    /* operand size=16: second word on stack must be within stack limits,
     *   else #SS(0);
     */
    if ( !bx_can_pop(4) ) {
      bx_panic("return_protected: 2nd word not in stack limits\n");
      /* #SS(0) */
      return;
      }
    stack_cs_offset = 2;
    stack_param_offset = 4;
    }

  if (bx_cpu.ss.cache.u.segment.d_b) temp_ESP = ESP;
  else temp_ESP = SP;

  /* return selector RPL must be >= CPL, else #GP(return selector) */
  bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP +
                       stack_cs_offset, 2, CPL==3, BX_READ, &raw_cs_selector);
  bx_parse_selector(raw_cs_selector, &cs_selector);
  if ( cs_selector.rpl < CPL ) {
    bx_printf("return_protected: CS.rpl < CPL\n");
    bx_printf("  CS.rpl=%u CPL=%u\n", (unsigned) cs_selector.rpl,
      (unsigned) CPL);
    bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
    return;
    }

  /* if return selector RPL == CPL then
   * RETURN TO SAME LEVEL */
  if ( cs_selector.rpl == CPL ) {
    /* return selector must be non-null, else #GP(0) */
    if ( (raw_cs_selector & 0xfffc) == 0 ) {
      bx_panic("return_protected: CS null\n");
      /* #GP(0) */
      return;
      }

    /* selector index must be within its descriptor table limits,
     * else #GP(selector) */
    bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2, 
      BX_GP_EXCEPTION);
    BX_HANDLE_EXCEPTION()

    /* descriptor AR byte must indicate code segment, else #GP(selector) */
    bx_parse_descriptor(dword1, dword2, &cs_descriptor);
    if (cs_descriptor.valid==0 ||
        cs_descriptor.segment==0 ||
        cs_descriptor.u.segment.executable==0) {
      bx_panic("return_protected: AR byte not code\n");
      /* #GP(selector) */
      return;
      }

    /* if non-conforming then code segment DPL must = CPL,
     * else #GP(selector) */
    if (cs_descriptor.u.segment.c_ed==0  && cs_descriptor.dpl!=CPL) {
      bx_panic("return_protected: non-conforming, DPL!=CPL\n");
      /* #GP(selector) */
      return;
      }

    /* if conforming then code segment DPL must be <= CPL,
     * else #GP(selector) */
    if (cs_descriptor.u.segment.c_ed  && cs_descriptor.dpl>CPL) {
      bx_panic("return_protected: conforming, DPL>CPL\n");
      /* #GP(selector) */
      return;
      }

    /* code segment must be present, else #NP(selector) */
    if (cs_descriptor.p==0) {
      bx_panic("return_protected: not present\n");
      /* #NP(selector) */
      return;
      }

    /* top word on stack must be within stack limits, else #SS(0) */
    if ( !bx_can_pop(stack_param_offset + pop_bytes) ) {
      bx_panic("return_protected: top word not in stack limits\n");
      /* #SS(0) */
      return;
      }
    
    /* IP must be in code segment limit, else #GP(0) */
#if BX_CPU >= 3
    if (bx_cpu.is_32bit_opsize) {
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &return_EIP);
      }
    else
#endif
      {
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        2, CPL==3, BX_READ, &return_IP);
      return_EIP = return_IP;
      }

    if ( return_EIP > cs_descriptor.u.segment.limit_scaled ) {
      bx_panic("return_protected: return IP > CS.limit\n");
      /* #GP(0) */
      return;
      }

    /* load CS:IP from stack */
    /* load CS register with descriptor */
    /* increment SP */
    bx_load_cs(&cs_selector, &cs_descriptor, CPL);
    bx_cpu.eip = return_EIP;
    if (bx_cpu.ss.cache.u.segment.d_b)
      ESP += stack_param_offset + pop_bytes;
    else
      SP += stack_param_offset + pop_bytes;

    if (bx_dbg.protected)
      bx_printf("return_protected: return to same level succeeding\n");
    return;
    }

  /* RETURN TO OUTER PRIVILEGE LEVEL */
  else {
    /* + 6+N*2: SS      | +12+N*4:     SS */
    /* + 4+N*2: SP      | + 8+N*4:    ESP */
    /*          parm N  | +        parm N */
    /*          parm 3  | +        parm 3 */
    /*          parm 2  | +        parm 2 */
    /*          parm 1  | + 8:     parm 1 */
    /* + 2:     CS      | + 4:         CS */
    /* + 0:     IP      | + 0:        EIP */

    if (bx_cpu.is_32bit_opsize) {
      /* top 16+immediate bytes on stack must be within stack limits, else #SS(0) */
      if ( !bx_can_pop(16 + pop_bytes) ) {
        bx_panic("return_protected: 8 bytes not within stack limits\n");
        /* #SS(0) */
        return;
        }
      }
    else {
      /* top 8+immediate bytes on stack must be within stack limits, else #SS(0) */
      if ( !bx_can_pop(8 + pop_bytes) ) {
        bx_panic("return_protected: 8 bytes not within stack limits\n");
        /* #SS(0) */
        return;
        }
      }

    /* examine return CS selector and associated descriptor */

    /* selector must be non-null else #GP(0) */
    if ( (raw_cs_selector & 0xfffc) == 0 ) {
      bx_panic("return_protected: CS selector null\n");
      /* #GP(0) */
      return;
      }

    /* selector index must be within its descriptor table limits,
     * else #GP(selector) */
    bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2, 
      BX_GP_EXCEPTION);
    BX_HANDLE_EXCEPTION()
    bx_parse_descriptor(dword1, dword2, &cs_descriptor);

    /* descriptor AR byte must indicate code segment else #GP(selector) */
    if (cs_descriptor.valid==0 ||
        cs_descriptor.segment==0  ||
        cs_descriptor.u.segment.executable==0) {
      bx_panic("return_protected: AR byte not code\n");
      /* #GP(selector) */
      return;
      }

    /* if non-conforming code then code seg DPL must equal return selector RPL
     * else #GP(selector) */
    if (cs_descriptor.u.segment.c_ed==0 &&
        cs_descriptor.dpl!=cs_selector.rpl) {
      bx_panic("return_protected: non-conforming seg DPL != selector.rpl\n");
      /* #GP(selector) */
      return;
      }

    /* if conforming then code segment DPL must be <= return selector RPL
     * else #GP(selector) */
    if (cs_descriptor.u.segment.c_ed &&
        cs_descriptor.dpl>cs_selector.rpl) {
      bx_panic("return_protected: conforming seg DPL > selector.rpl\n");
      /* #GP(selector) */
      return;
      }

    /* segment must be present else #NP(selector) */
    if (cs_descriptor.p==0) {
      bx_panic("return_protected: segment not present\n");
      /* #NP(selector) */
      return;
      }

    /* examine return SS selector and associated descriptor: */
    if (bx_cpu.is_32bit_opsize) {
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 12 + pop_bytes,
        2, CPL==3, BX_READ, &raw_ss_selector);
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 8 + pop_bytes,
        4, CPL==3, BX_READ, &return_ESP);
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &return_EIP);
      }
    else {
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 6 + pop_bytes,
        2, CPL==3, BX_READ, &raw_ss_selector);
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 4 + pop_bytes,
        2, CPL==3, BX_READ, &return_SP);
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        2, CPL==3, BX_READ, &return_IP);
      return_EIP = return_IP;
      }

    /* selector must be non-null else #GP(0) */
    if ( (raw_ss_selector & 0xfffc) == 0 ) {
      bx_panic("return_protected: SS selector null\n");
      /* #GP(0) */
      return;
      }

    /* selector index must be within its descriptor table limits,
     * else #GP(selector) */
    bx_parse_selector(raw_ss_selector, &ss_selector);
    bx_fetch_raw_descriptor(&ss_selector, &dword1, &dword2, 
      BX_GP_EXCEPTION);
    BX_HANDLE_EXCEPTION()
    bx_parse_descriptor(dword1, dword2, &ss_descriptor);
    
    /* selector RPL must = RPL of the return CS selector,
     * else #GP(selector) */
    if (ss_selector.rpl != cs_selector.rpl) {
      bx_panic("return_protected: ss.rpl != cs.rpl\n");
      /* #GP(selector) */
      return;
      }

    /* descriptor AR byte must indicate a writable data segment,
     * else #GP(selector) */
    if (ss_descriptor.valid==0 ||
        ss_descriptor.segment==0 ||
        ss_descriptor.u.segment.executable ||
        ss_descriptor.u.segment.r_w==0) {
      bx_panic("return_protected: SS.AR byte not writable data\n");
      /* #GP(selector) */
      return;
      }

    /* descriptor dpl must = RPL of the return CS selector,
     * else #GP(selector) */
    if (ss_descriptor.dpl != cs_selector.rpl) {
      bx_panic("return_protected: SS.dpl != cs.rpl\n");
      /* #GP(selector) */
      return;
      }

    /* segment must be present else #SS(selector) */
    if (ss_descriptor.p==0) {
      bx_panic("ss.p == 0\n");
      /* #NP(selector) */
      return;
      }

    /* eIP must be in code segment limit, else #GP(0) */
    if (return_EIP > cs_descriptor.u.segment.limit_scaled) {
      bx_panic("return_protected: eIP > cs.limit\n");
      /* #GP(0) */
      return;
      }

    /* set CPL to RPL of return CS selector */
    /* load CS:IP from stack */
    /* set CS RPL to CPL */
    /* load the CS-cache with return CS descriptor */
    bx_load_cs(&cs_selector, &cs_descriptor, cs_selector.rpl);
    bx_cpu.eip = return_EIP;

    /* load SS:SP from stack */
    /* load SS-cache with return SS descriptor */
    bx_load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
    if (bx_cpu.is_32bit_opsize)
      ESP = return_ESP;
    else
      SP  = return_SP;

    /* check ES, DS, FS, GS for validity */
    bx_validate_seg_regs();

    if (bx_dbg.protected)
      bx_printf("return_protected: return to outer level succeeding\n");
    return;
    }

  return;
}
#endif



#if BX_CPU >= 2
  void
bx_iret_protected(void)
{
  Bit16u raw_cs_selector, raw_ss_selector;
  bx_selector_t cs_selector, ss_selector;
  Bit32u dword1, dword2;
  bx_descriptor_t cs_descriptor, ss_descriptor;

  if (bx_dbg.protected) bx_printf("iret():\n");

  if (bx_cpu.eflags.nt) { /* NT = 1: RETURN FROM NESTED TASK */
    /* what's the deal with NT & VM ? */

    /* TASK_RETURN: */
    bx_panic("IRET: nested task return not implemented\n");
    return;
    }

  else { /* NT = 0: INTERRUPT RETURN ON STACK -or STACK_RETURN_TO_V86 */
    Bit16u top_nbytes_same, top_nbytes_outer;
    Bit32u cs_offset, ss_offset;
    Bit32u new_eip, new_esp, temp_ESP, flags32;
    Bit16u new_ip, new_sp, flags16;
    Bit8u prev_cpl;

    /* 16bit opsize  |   32bit opsize
     * ==============================
     * SS     eSP+8  |   SS     eSP+16
     * SP     eSP+6  |   ESP    eSP+12
     * -------------------------------
     * FLAGS  eSP+4  |   EFLAGS eSP+8
     * CS     eSP+2  |   CS     eSP+4
     * IP     eSP+0  |   EIP    eSP+0
     */


    if (bx_cpu.is_32bit_opsize) {
      top_nbytes_same    = 12;
      top_nbytes_outer   = 20;
      cs_offset = 4;
      ss_offset = 16;
      }
    else {
      top_nbytes_same    = 6;
      top_nbytes_outer   = 10;
      cs_offset = 2;
      ss_offset = 8;
      }

    /* CS on stack must be within stack limits, else #SS(0) */
    if ( !bx_can_pop(top_nbytes_same) ) {
      bx_panic("iret: CS not within stack limits\n");
      bx_exception(BX_SS_EXCEPTION, 0, 0);
      return;
      }

    if (bx_cpu.ss.cache.u.segment.d_b)
      temp_ESP = ESP;
    else
      temp_ESP = SP;

    bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + cs_offset,
      2, CPL==3, BX_READ, &raw_cs_selector);
    BX_HANDLE_EXCEPTION();

    if (bx_cpu.is_32bit_opsize) {
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        4, CPL==3, BX_READ, &new_eip);
      BX_HANDLE_EXCEPTION();
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 8,
        4, CPL==3, BX_READ, &flags32);
      BX_HANDLE_EXCEPTION();

      /* if VM=1 in flags image on stack then STACK_RETURN_TO_V86 */
      if (flags32 & 0x00020000) {
        if (CPL != 0)
          bx_panic("iret: VM set on stack, CPL!=0\n");
        bx_stack_return_to_v86(new_eip, raw_cs_selector, flags32);
        return;
        }
      }
    else {
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
        2, CPL==3, BX_READ, &new_ip);
      BX_HANDLE_EXCEPTION();
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 4,
        2, CPL==3, BX_READ, &flags16);
      BX_HANDLE_EXCEPTION();
      }

    /* return CS selector RPL must be >= CPL, else #GP(return selector) */
    bx_parse_selector(raw_cs_selector, &cs_selector);
    if (cs_selector.rpl < CPL) {
      bx_panic("iret: return selector RPL < CPL\n");
      bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
      return;
      }

    if (cs_selector.rpl == CPL) { /* INTERRUPT RETURN TO SAME LEVEL */
      if (bx_dbg.protected) bx_printf("iret: return on stack, same level\n");
      /* top 6/12 bytes on stack must be within limits, else #SS(0) */
      /* satisfied above */

      /* return CS selector must be non-null, else #GP(0) */
      if ( (raw_cs_selector & 0xfffc) == 0 ) {
        bx_panic("iret: return CS selector null\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }
      
      /* selector index must be within descriptor table limits,
       * else #GP(return selector) */
      bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2,
        BX_GP_EXCEPTION);
      BX_HANDLE_EXCEPTION()

      bx_parse_descriptor(dword1, dword2, &cs_descriptor);

      /* AR byte must indicate code segment else #GP(return selector) */
      if ( cs_descriptor.valid==0 ||
           cs_descriptor.segment==0  ||
           cs_descriptor.u.segment.executable==0 ) {
        bx_panic("iret: AR byte indicated non code segment\n");
        bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      /* if non-conforming then code segment DPL must = CPL,
       * else #GP(return selector) */
      if ( cs_descriptor.u.segment.c_ed==0 &&
           cs_descriptor.dpl!=CPL) {
        bx_panic("iret: non-conforming, DPL != CPL\n");
        bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      /* if conforming, then code segment DPL must be <= CPL
       * else #GP(return selector) */
      if ( cs_descriptor.u.segment.c_ed  &&
           cs_descriptor.dpl > CPL ) {
        bx_panic("iret: conforming, DPL > CPL\n");
        bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      /* segment must be present else #NP(return selector) */
      if ( cs_descriptor.p==0 ) {
        bx_panic("iret: not present\n");
        bx_exception(BX_NP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      if (bx_cpu.is_32bit_opsize) {
        /* return EIP must be in code segment limit else #GP(0) */
        if ( new_eip > cs_descriptor.u.segment.limit_scaled ) {
          bx_panic("iret: IP > descriptor limit\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        /* load CS:EIP from stack */
        /* load CS-cache with new code segment descriptor */
        bx_load_cs(&cs_selector, &cs_descriptor, CPL);
        EIP = new_eip;

        /* load EFLAGS with 3rd doubleword from stack */
        bx_write_eflags(flags32, CPL==0, CPL<=IOPL, 0, 1);
        }
      else {
        /* return IP must be in code segment limit else #GP(0) */
        if ( new_ip > cs_descriptor.u.segment.limit_scaled ) {
          bx_panic("iret: IP > descriptor limit\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        /* load CS:IP from stack */
        /* load CS-cache with new code segment descriptor */
        bx_load_cs(&cs_selector, &cs_descriptor, CPL);
        EIP = new_ip;

        /* load flags with third word on stack */
        bx_write_flags(flags16, CPL==0, CPL<=IOPL);
        }

      /* increment stack by 6/12 */
      if (bx_cpu.ss.cache.u.segment.d_b)
        ESP += top_nbytes_same;
      else
        SP += top_nbytes_same;

      if (bx_dbg.protected) bx_printf("iret: same level return succeeded. bombing\n");
      return;
      }
    else { /* INTERRUPT RETURN TO OUTER PRIVILEGE LEVEL */
      /* 16bit opsize  |   32bit opsize
       * ==============================
       * SS     eSP+8  |   SS     eSP+16
       * SP     eSP+6  |   ESP    eSP+12
       * FLAGS  eSP+4  |   EFLAGS eSP+8
       * CS     eSP+2  |   CS     eSP+4
       * IP     eSP+0  |   EIP    eSP+0
       */

      if (bx_dbg.protected) bx_printf("iret: return to outer level\n");

      /* top 10/20 bytes on stack must be within limits else #SS(0) */
      if ( !bx_can_pop(top_nbytes_outer) ) {
        bx_panic("iret: top 10/20 bytes not within stack limits\n");
        bx_exception(BX_SS_EXCEPTION, 0, 0);
        return;
        }

      /* return CS selector must be non-null, else #GP(0) */
      if ( (raw_cs_selector & 0xfffc) == 0 ) {
        bx_panic("iret: return CS selector null\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }

      /* selector index must be within its descriptor table limits,
       * else #GP(return selector) */
      bx_fetch_raw_descriptor(&cs_selector, &dword1, &dword2, 
        BX_GP_EXCEPTION);
      BX_HANDLE_EXCEPTION()

      bx_parse_descriptor(dword1, dword2, &cs_descriptor);

      /* AR byte must indicate code segment else #GP(return selector) */
      if ( cs_descriptor.valid==0 ||
           cs_descriptor.segment==0  ||
           cs_descriptor.u.segment.executable==0 ) {
        bx_panic("iret: AR byte indicated non code segment\n");
        bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      /* if non-conforming then code segment DPL must = CS selector RPL,
       * else #GP(return selector) */
      if ( cs_descriptor.u.segment.c_ed==0  &&
           cs_descriptor.dpl!=cs_selector.rpl) {
        bx_panic("iret: descriptor DPL != selector RPL\n");
        bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      /* if conforming then code segment DPL must be > CPL,
       * else #GP(return selector) */
      if ( cs_descriptor.u.segment.c_ed  &&
           cs_descriptor.dpl <= CPL ) {
        bx_panic("iret: descriptor DPL <= CPL\n");
        bx_exception(BX_GP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      /* segment must be present, else #NP(return selector) */
      if ( cs_descriptor.p==0 ) {
        bx_panic("iret: segment not present\n");
        bx_exception(BX_NP_EXCEPTION, raw_cs_selector & 0xfffc, 0);
        return;
        }

      /* examine return SS selector and associated descriptor */
      bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + ss_offset,
        2, CPL==3, BX_READ, &raw_ss_selector);

      /* selector must be non-null, else #GP(0) */
      if ( (raw_ss_selector & 0xfffc) == 0 ) {
        bx_panic("iret: SS selector null\n");
        bx_exception(BX_GP_EXCEPTION, 0, 0);
        return;
        }

      bx_parse_selector(raw_ss_selector, &ss_selector);
 
      /* selector RPL must = RPL of return CS selector,
       * else #GP(SS selector) */
      if ( ss_selector.rpl != cs_selector.rpl) {
        bx_panic("iret: SS.rpl != CS.rpl\n");
        bx_exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }

      /* selector index must be within its descriptor table limits,
       * else #GP(SS selector) */
      bx_fetch_raw_descriptor(&ss_selector, &dword1, &dword2, 
        BX_GP_EXCEPTION);
      BX_HANDLE_EXCEPTION()

      bx_parse_descriptor(dword1, dword2, &ss_descriptor);

      /* AR byte must indicate a writable data segment,
       * else #GP(SS selector) */
      if ( ss_descriptor.valid==0 ||
           ss_descriptor.segment==0  ||
           ss_descriptor.u.segment.executable  ||
           ss_descriptor.u.segment.r_w==0 ) {
        bx_panic("iret: SS AR byte not writable code segment\n");
        bx_exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }

      /* stack segment DPL must equal the RPL of the return CS selector,
       * else #GP(SS selector) */
      if ( ss_descriptor.dpl != cs_selector.rpl ) {
        bx_panic("iret: SS.dpl != CS selector RPL\n");
        bx_exception(BX_GP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }

      /* SS must be present, else #NP(SS selector) */
      if ( ss_descriptor.p==0 ) {
        bx_panic("iret: SS not present!\n");
        bx_exception(BX_NP_EXCEPTION, raw_ss_selector & 0xfffc, 0);
        return;
        }


      if (bx_cpu.is_32bit_opsize) {
        bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
          4, CPL==3, BX_READ, &new_eip);
        bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 8,
          4, CPL==3, BX_READ, &flags32);
        bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 12,
          4, CPL==3, BX_READ, &new_esp);

        /* EIP must be in code segment limit, else #GP(0) */
        if ( new_eip > cs_descriptor.u.segment.limit_scaled ) {
          bx_panic("iret: IP > descriptor limit\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }

        /* load CS:EIP from stack */
        /* load the CS-cache with CS descriptor */
        /* set CPL to the RPL of the return CS selector */
        prev_cpl = CPL; /* previous CPL */
        bx_load_cs(&cs_selector, &cs_descriptor, cs_selector.rpl);
        bx_cpu.eip = new_eip;

        /* load flags from stack */
/* ??? */
#if 1
        bx_write_eflags(flags32, prev_cpl==0, prev_cpl<=IOPL, 0, 1);
#endif
#if 0
        bx_write_eflags(flags32, CPL==0, CPL<=IOPL, 0, 1);
#endif

        /* load SS:eSP from stack */
        /* load the SS-cache with SS descriptor */
        bx_load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
        bx_cpu.esp = new_esp;
        }
      else {
        bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 0,
          2, CPL==3, BX_READ, &new_ip);
        bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 4,
          2, CPL==3, BX_READ, &flags16);
        bx_access_linear(bx_cpu.ss.cache.u.segment.base + temp_ESP + 6,
          2, CPL==3, BX_READ, &new_sp);

        /* IP must be in code segment limit, else #GP(0) */
        if ( new_ip > cs_descriptor.u.segment.limit_scaled ) {
          bx_panic("iret: IP > descriptor limit\n");
          bx_exception(BX_GP_EXCEPTION, 0, 0);
          return;
          }
        /* load CS:IP from stack */
        /* load the CS-cache with CS descriptor */
        /* set CPL to the RPL of the return CS selector */
        prev_cpl = CPL; /* previous CPL */
        bx_load_cs(&cs_selector, &cs_descriptor, cs_selector.rpl);
        bx_cpu.eip = new_ip;

/* ??? */
        /* load flags from stack */
#if 1
        bx_write_flags(flags16, prev_cpl==0, prev_cpl<=IOPL);
#endif
#if 0
        bx_write_flags(flags16, CPL==0, CPL<=IOPL);
#endif

        /* load SS:eSP from stack */
        /* load the SS-cache with SS descriptor */
        bx_load_ss(&ss_selector, &ss_descriptor, cs_selector.rpl);
        bx_cpu.esp = new_sp;
        }

      bx_validate_seg_regs();

      if (bx_dbg.protected)
        bx_printf("iret: WARNING ES & DS validity checks incomplete!\n");
      return;
      }
    }
  bx_panic("IRET: shouldn't get here!\n");
}
#endif
