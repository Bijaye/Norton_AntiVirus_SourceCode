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

#ifdef WIN32

void bx_W_init(int argc, char **argv);
void bx_video_outchar(Bit8u outchar);
unsigned bx_handle_W_events(void);
void bx_W_beep(void);
void bx_W_outchar(Bit16u outchar, unsigned x, unsigned y);
void bx_W_outstring(Bit8u *string, Bit8u* attr, unsigned len, unsigned row,
           unsigned col);
void bx_W_clear_screen(unsigned c1, unsigned r1, unsigned c2, unsigned r2,
    Bit8u blank_attr);
void bx_W_scroll_screen(unsigned c1, unsigned r1, unsigned c2, unsigned r2,
    unsigned rows);
void bx_W_draw_point(unsigned x, unsigned y, Bit8u set);
void bx_W_switch_mode(unsigned mode);
void bx_W_update(void);
void bx_W_flush(void);

#else

void bx_X_init(int argc, char **argv);
void bx_video_outchar(Bit8u outchar);
unsigned bx_handle_x_events(void);
void bx_X_beep(void);
void bx_X_outchar(Bit16u outchar, unsigned x, unsigned y);
void bx_X_outstring(Bit8u *string, Bit8u* attr, unsigned len, unsigned row,
           unsigned col);
void bx_X_clear_screen(unsigned c1, unsigned r1, unsigned c2, unsigned r2,
    Bit8u blank_attr);
void bx_X_scroll_screen(unsigned c1, unsigned r1, unsigned c2, unsigned r2,
    unsigned rows);
void bx_X_draw_point(unsigned x, unsigned y, Bit8u set);
void bx_X_switch_mode(unsigned mode);
void bx_X_update(void);
void bx_X_flush(void);

#endif
