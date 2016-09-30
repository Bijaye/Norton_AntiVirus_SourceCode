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


#define HGA_OFFSET ((enable_upper32k && bx_hga_page_displayed)? 0x8000 : 0x0000)

Bit32u bx_hga_io_read_handler(Bit32u address, unsigned io_len);
void   bx_hga_io_write_handler(Bit32u address, Bit32u value, unsigned io_len);

Bit8u bx_hga_mem_read(Bit32u addr);
void  bx_hga_mem_write(Bit32u addr, Bit8u value);

void bx_hga_int10h_handler(int vector);

void bx_init_hga_hardware(void);
void bx_init_hga_bios(void);

void bx_video_message(char *);
void bx_hga_timer_handler(void);

#ifndef IO_HGA_C
extern Bit8u bx_hga_graphics_mode;
extern Bit8u bx_hga_video_enabled;
extern Bit8u bx_hga_page_displayed;

extern Bit8u data_register,
             allow_graphics,
             enable_upper32k;
#endif

#if (BX_EXTERNAL_ENVIRONMENT==1) || (BX_EMULATE_HGA_DUMPS>0)
extern Bit8u bx_hga_memory[64 * 1024];
#else
extern Bit8u *bx_hga_memory;
#endif

