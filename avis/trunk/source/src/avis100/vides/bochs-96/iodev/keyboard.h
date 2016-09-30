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


Bit32u bx_keyboard_io_read_handler(Bit32u address, unsigned io_len);
void  bx_keyboard_io_write_handler(Bit32u address, Bit32u value,
                                   unsigned io_len);

void  bx_int09h_handler(int vector);
void  bx_int16h_handler(int vector);
void  bx_init_keyboard_hardware(void);
void  bx_init_keyboard_bios(void);
void  bx_keybd_gen_scancode(Bit8u scancode);
void  bx_set_kbd_clock_enable(Bit8u value);
void  bx_set_aux_clock_enable(Bit8u value);
void  bx_set_mouse_enable(Bit8u value);
Boolean bx_mouse_enQ_packet(Bit8u b1, Bit8u b2, Bit8u b3);
void bx_kbd_port61h_write(Bit8u value);

extern Boolean bx_mouse_device_enabled;
extern Bit8u ascii_to_scancode[0x80];
extern Bit8u shifted_ascii[0x80];

#if BX_CPU >= 2
#  define BX_CALL_INT15_4F 1
#endif
