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



#include <stdio.h>

#include "bochs.h"


static char command_line[128];

void bx_debug_decode(bx_segment_reg_t *seg, Bit32u offset);


static Boolean continu = 0;

void bx_debug(bx_segment_reg_t *seg, Bit32u offset)
/*void bx_debug(bx_segment_reg_t *seg, Bit32u offset)*/
{
#if BX_CPU <= 2
  bx_printf("\n| AX=%04x  BX=%04x  CX=%04x  DX=%04x  SP=%04x  BP=%04x  SI=%04x  DI=%04x\n",
    (unsigned) AX, (int) BX, (int) CX, (int) DX, (int) SP, (int) BP, (int) SI,
    (unsigned) DI);
  bx_printf("| DS=%04x  ES=%04x  SS=%04x  CS=%04x  IP=%04x   ",
    (unsigned) bx_cpu.ds.selector.value,
    (unsigned) bx_cpu.es.selector.value,
    (unsigned) bx_cpu.ss.selector.value,
    (unsigned) bx_cpu.cs.selector.value,
    (unsigned) bx_cpu.eip);
  bx_printf("%s %s %s %s %s %s %s %s\n",
    bx_get_OF() ? "OV" : "NV",
    bx_get_DF() ? "DW" : "UP",
    bx_cpu.eflags.if_ ? "EI" : "DI",
    bx_get_SF() ? "NG" : "PL",
    bx_get_ZF() ? "ZR" : "NZ",
    bx_get_AF() ? "AC" : "NA",
    bx_get_PF() ? "PE" : "PO",
    bx_get_CF() ? "CY" : "NC");


  bx_printf(">> [%x:%04x] ", (unsigned) seg->selector.value,
    (unsigned) offset);
#else
  bx_printf("\n| EAX=%08x  EBX=%08x  ECX=%08x  EDX=%08x\n",
          (unsigned) EAX, (unsigned) EBX, (unsigned) ECX, (unsigned) EDX);
  bx_printf("| ESP=%08x  EBP=%08x  ESI=%08x  EDI=%08x\n",
          (unsigned) ESP, (unsigned) EBP, (unsigned) ESI, (unsigned) EDI);
  bx_printf("| IOPL=%1u %s %s %s %s %s %s %s %s\n",
    bx_cpu.eflags.iopl,
    bx_get_OF()       ? "OV" : "NV",
    bx_cpu.eflags.df  ? "DW" : "UP",
    bx_cpu.eflags.if_ ? "EI" : "DI",
    bx_get_SF()       ? "NG" : "PL",
    bx_get_ZF()       ? "ZR" : "NZ",
    bx_get_AF()       ? "AC" : "NA",
    bx_get_PF()       ? "PE" : "PO",
    bx_get_CF()       ? "CY" : "NC");
  bx_printf("| SEG selector     base    limit G D\n");
  bx_printf("| SEG sltr(index|ti|rpl)     base    limit G D\n");
  bx_printf("|  DS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) bx_cpu.ds.selector.value,
    (unsigned) bx_cpu.ds.selector.index,
    (unsigned) bx_cpu.ds.selector.ti,
    (unsigned) bx_cpu.ds.selector.rpl,
    (unsigned) bx_cpu.ds.cache.u.segment.base,
    (unsigned) bx_cpu.ds.cache.u.segment.limit,
    (unsigned) bx_cpu.ds.cache.u.segment.g,
    (unsigned) bx_cpu.ds.cache.u.segment.d_b);
  bx_printf("|  ES:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) bx_cpu.es.selector.value,
    (unsigned) bx_cpu.es.selector.index,
    (unsigned) bx_cpu.es.selector.ti,
    (unsigned) bx_cpu.es.selector.rpl,
    (unsigned) bx_cpu.es.cache.u.segment.base,
    (unsigned) bx_cpu.es.cache.u.segment.limit,
    (unsigned) bx_cpu.es.cache.u.segment.g,
    (unsigned) bx_cpu.es.cache.u.segment.d_b);
  bx_printf("|  FS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) bx_cpu.fs.selector.value,
    (unsigned) bx_cpu.fs.selector.index,
    (unsigned) bx_cpu.fs.selector.ti,
    (unsigned) bx_cpu.fs.selector.rpl,
    (unsigned) bx_cpu.fs.cache.u.segment.base,
    (unsigned) bx_cpu.fs.cache.u.segment.limit,
    (unsigned) bx_cpu.fs.cache.u.segment.g,
    (unsigned) bx_cpu.fs.cache.u.segment.d_b);
  bx_printf("|  GS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) bx_cpu.gs.selector.value,
    (unsigned) bx_cpu.gs.selector.index,
    (unsigned) bx_cpu.gs.selector.ti,
    (unsigned) bx_cpu.gs.selector.rpl,
    (unsigned) bx_cpu.gs.cache.u.segment.base,
    (unsigned) bx_cpu.gs.cache.u.segment.limit,
    (unsigned) bx_cpu.gs.cache.u.segment.g,
    (unsigned) bx_cpu.gs.cache.u.segment.d_b);
  bx_printf("|  SS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) bx_cpu.ss.selector.value,
    (unsigned) bx_cpu.ss.selector.index,
    (unsigned) bx_cpu.ss.selector.ti,
    (unsigned) bx_cpu.ss.selector.rpl,
    (unsigned) bx_cpu.ss.cache.u.segment.base,
    (unsigned) bx_cpu.ss.cache.u.segment.limit,
    (unsigned) bx_cpu.ss.cache.u.segment.g,
    (unsigned) bx_cpu.ss.cache.u.segment.d_b);
  bx_printf("|  CS:%04x( %04x| %01u|  %1u) %08x %08x %1u %1u\n",
    (unsigned) bx_cpu.cs.selector.value,
    (unsigned) bx_cpu.cs.selector.index,
    (unsigned) bx_cpu.cs.selector.ti,
    (unsigned) bx_cpu.cs.selector.rpl,
    (unsigned) bx_cpu.cs.cache.u.segment.base,
    (unsigned) bx_cpu.cs.cache.u.segment.limit,
    (unsigned) bx_cpu.cs.cache.u.segment.g,
    (unsigned) bx_cpu.cs.cache.u.segment.d_b);
  bx_printf("| EIP=%08x (%08x)\n", (unsigned) bx_cpu.eip,
    (unsigned) bx_cpu.prev_eip);
  bx_printf(">> ");
#endif

  bx_debug_decode(seg, offset);

return;
  if (continu) return;

  bx_printf("\ndebug> ");
  scanf(" %[^\n]", command_line);
  if (!strcmp(command_line, "continue")) {
    continu = 1;
    }
}
