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



Bit8u bx_mouse_io_read_handler(Bit32u address);
void bx_mouse_io_write_handler(Bit32u address, Bit8u value);
void bx_mouse_int_handler(int vector);
void bx_init_mouse(void);
void bx_mouse_bios(void);
