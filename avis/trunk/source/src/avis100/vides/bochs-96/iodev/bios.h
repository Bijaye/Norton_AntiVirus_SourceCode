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



void bx_jmp_post(void);
void bx_post_setup(void);
void bx_coldboot(void);

extern Bit8u bx_isr_code_iret[4];
extern Bit8u bx_isr_code_cf[23];
