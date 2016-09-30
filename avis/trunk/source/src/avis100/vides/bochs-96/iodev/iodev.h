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





#include "../bochs.h"
#if BX_SUPPORT_VGA
#  include "vga.h"
#  include "vga_x.h"
#else
#  include "hga.h"
#  include "hga_x.h"
#endif
#include "cmos.h"
#include "dma.h"
#include "floppy.h"
#include "harddrv.h"
#include "keyboard.h"
#include "parallel.h"
#include "pic.h"
#include "pit.h"
#include "serial.h"
#include "unmapped.h"
#include "mem_map.h"
#include "bios.h"
#include "mouse.h"
