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





Bit32u bx_parallel_io_read_handler(Bit32u address, unsigned io_len);
void  bx_parallel_io_write_handler(Bit32u address, Bit32u value,
                                   unsigned io_len);

void  bx_parallel_int_handler(int vector);
void  bx_init_parallel_hardware(void);
void  bx_init_parallel_bios(void);
