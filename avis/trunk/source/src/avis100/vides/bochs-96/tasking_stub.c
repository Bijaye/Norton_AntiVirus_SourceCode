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
  /* corresponds to SWITCH_TASKS algorithm in Intel documentation */
  void
bx_task_switch16(bx_selector_t *selector,
                 bx_descriptor_t *descriptor, unsigned source,
                 Bit32u dword1, Bit32u dword2)
{
  bx_printf("task_switch16(): not complete\n");
}
#endif

#if BX_CPU >= 3
  void
bx_task_switch32(bx_selector_t *selector,
                 bx_descriptor_t *descriptor, unsigned source,
                 Bit32u dword1, Bit32u dword2)
{
  bx_panic("task_switch32(): not complete\n");
}
#endif
