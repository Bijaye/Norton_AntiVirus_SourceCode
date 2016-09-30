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


Bit32u bx_dma_io_read_handler(Bit32u, unsigned len);
void  bx_dma_io_write_handler(Bit32u, Bit32u value, unsigned len);
void  bx_init_dma_hardware(void);
void  bx_init_dma_bios(void);

typedef struct {
  Bit8u   mask;
  Boolean flip_flop;
  struct {
    Bit8u   mode;
    Bit16u  base_address;
    Bit16u  current_address;
    Bit16u  base_count;
    Bit16u  current_count;
    Bit8u   page_reg;
    } chan[4]; /* DMA channels 0..3 */
  } dma_controller_t;

extern dma_controller_t dma1;
