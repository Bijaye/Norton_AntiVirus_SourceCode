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


static void v8086_message(void);


  void
bx_stack_return_to_v86(Bit32u new_eip, Bit32u raw_cs_selector, Bit32u flags32)
{
  bx_printf("stack_return_to_v86: VM bit set in EFLAGS stack image\n");
  v8086_message();
}

  void
bx_stack_return_from_v86(void)
{
  bx_printf("stack_return_from_v86:\n");
  v8086_message();
}

  static void
v8086_message(void)
{
  bx_printf("Program compiled with BX_SUPPORT_V8086_MODE = 0\n");
  bx_printf("You need to rerun the configure script and recompile\n");
  bx_printf("  to use virtual-8086 mode features.\n");
  bx_panic("Bummer!\n");
}
